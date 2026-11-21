# Direct typed JSON parsing

`<wjr/json/inplace_parser.hpp>` provides `wjr::json::parse<T>(reader)`.
It builds owning registered records and their nested values through the same grammar
used by the JSON document parser and validator. The reader must first receive the
input through `read`.

```cpp
#include <wjr/json/inplace_parser.hpp>

struct point {
    WJR_REGISTER_JSON_FIELDS(point, x, y)
    double x = 0;
    double y = 0;
};

struct shape {
    WJR_REGISTER_JSON_FIELDS(shape, name, points)
    std::string name;
    std::vector<point> points;
};

wjr::json::ondemand_reader reader;
reader.read(R"({"points":[{"y":2,"x":1}],"name":"line"})");
auto parsed_shape = wjr::json::parse<shape>(reader);
if (!parsed_shape) {
    auto code = parsed_shape.error().code;
    auto byte_offset = parsed_shape.error().offset;
}
```

The factory default-constructs `T` and moves it into its successful result. It
requires default and move construction. Move assignment is unnecessary. Supported
values are `bool`, integral types, `float`, `double`, owning
`std::basic_string<char, Traits, Allocator>`, registered objects, `std::vector`, and
`wjr::vector` with its default growable storage. Registered fields must be writable
instance members with supported types. Fields may themselves contain registered
objects or vectors, including recursive vector-held objects.

`WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER` and
`WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER` also supply the descriptors. Existing
document projection remains available. Descriptor generation is deferred so older
document-only registrations of reference or static members remain usable.

## Input and failure semantics

- Every registered field is required, in any input order. An absent field reports
  `NO_SUCH_FIELD` at the object's closing token.
- Duplicate fields are processed in input order. Each occurrence must have the
  required type. The final valid occurrence supplies the value. A duplicate array
  replaces the previous array contents.
- Unknown fields receive complete syntax, Unicode, depth, and numeric-range
  validation while their values are discarded.
- The complete input receives UTF-8 validation before output construction or
  mutation. A malformed sequence reports `UTF8_ERROR` at its first byte. Literal
  control bytes inside strings and keys report `UNESCAPED_CHARS`. JSON escapes for
  these characters are accepted.
- Integral targets accept integral JSON syntax within their exact range.
  Fractional or exponent syntax reports `INCORRECT_TYPE`. Overflow reports
  `NUMBER_OUT_OF_RANGE`. Boolean targets accept JSON booleans.
- Floating targets parse directly at their destination precision, preserving
  signed zero. Overflow and underflow report `NUMBER_OUT_OF_RANGE`.
- Type and value failures report the offending token's byte offset. String failures
  from escapes or literal controls locate the opening quote. A malformed immediate
  token boundary locates that boundary or its scalar token. Incomplete input locates
  EOF. Trailing content locates its first token.
- Parse failures destroy partial factory output. Allocation and user construction
  exceptions propagate after local resource cleanup.

Document projection performs arithmetic casts and projects after document duplicate
replacement. The existing document parser and checker also retain their legacy gaps
in raw UTF-8, unescaped control-byte validation, non-JSON whitespace, and immediately
adjacent text after closing quotes. Performance comparisons use the common valid
input domain and report the extra validation performed by the typed path. The
hand-written reference receives the same validation as the typed path.

The existing `inplace_parser<T>::parse(T*, reader) noexcept -> result<void>` entry
borrows an already constructed writable target. It uses the same grammar and type
operations, can reuse its string/vector capacity, and may leave a partially updated
valid target on failure. Its existing `noexcept` contract applies to allocation and
user-code exceptions. It does not require default or move construction of the root.

## Implementation

Per-type field and scalar operations are static metadata. A non-type-specific
visitor context instantiates the grammar once and dispatches to these operations.
Each registered type has a compiled selector that combines field-name matching and
member selection. The number of comparisons grows linearly with the registered
field count. Scratch frames and required-field bits have inline capacity and grow
for deeper inputs. Scratch references use offsets across growth, and array elements
remain borrowed only until their child frame is removed.

[Complete-path measurements](../performance/json-direct-parser.md) compare the
record workload with document projection and hand-written owned-value references.
