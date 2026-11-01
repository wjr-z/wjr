# Fixed-size JSON projection

Include `<wjr/json/array.hpp>` and use `json::value_to<std::array<T, N>>(doc)`.
Registered structures and existing vector/dictionary serializers also dispatch
fixed-size array elements through this adapter:

```cpp
struct position {
    std::array<double, 2> coordinates;
    WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(position, coordinates)
};
using polygon = std::vector<std::vector<std::array<double, 2>>>;
```

JSON arrays must have exactly N elements. A wrong JSON kind keeps the existing
`bad_document_access` error; a shape mismatch throws `std::length_error` before
converting, moving or assigning any element. Child errors propagate unchanged.
New results directly aggregate-initialize their elements: no default-initializing
pass, temporary vector or heap allocation belongs to `std::array` itself. Child
types and enclosing containers retain their own storage requirements.

Assignment checks shape first, then applies child assignment in order. If a child
fails, earlier elements may have changed. Partially constructed new arrays clean
up completed elements. Nondefault and immovable elements can be constructed
directly when their serializer supports construction; an enclosing standard
container still requires its usual element move/copy capabilities. An empty array
does not require a child serializer. Engaged PMR string elements retain their
destination resource during assignment; document output uses the destination
document's allocator policy.

GCC 11 has an aggregate-return unwinding defect: a later element throwing can
leave an earlier nontrivial element undestroyed, independently of WJR. For N > 1
and nontrivial destructors, the adapter uses a named local aggregate on GCC 11.
The array must therefore be movable/copyable in that compiler domain; otherwise
construction is rejected by its constraint. Existing-array assignment is not
restricted. GCC 12+, Clang and MSVC retain direct immovable construction. Empty,
single-element and trivially destructible arrays retain their direct return path
on every supported compiler, including the double coordinate example.

Source cv/ref and [borrowed-value rules](json-nullable-conversion.md) apply
recursively, including arrays of string views. Length checking is not lifetime
extension. This adapter composes with `<wjr/json/optional.hpp>` in either include
order. Keep the explicit `value_to` entry for aggregate targets.

## Coordinate storage example

[The package consumer](../../examples/json/fixed_coordinates.cpp) projects the
same Polygon DOM into nested vectors and into rings of inline two-dimensional
points. Its stateless allocator counts only typed output; it does not time the
instrumented program or count the input/DOM, allocator overhead or process RSS.
For the fixed [simdjson-data fixture](https://github.com/simdjson/simdjson-data/blob/4197c425e857f0ec38e89822fdd0bd9ea21f4daf/jsonexamples/canada.json),
Windows Clang 18 x64 reports:

| Point representation | Points | Allocation requests | Requested/resident bytes |
| --- | ---: | ---: | ---: |
| `vector<double>` | 55,563 | 56,044 | 2,234,040 |
| `array<double, 2>` | 55,563 | 481 | 900,528 |

Checksums match, and both results release all their counted bytes. The avoided
requests are the 55,563 individual coordinate buffers. This changes the selected
C++ projection representation; it does not change owning DOM construction or
claim a parser throughput improvement. Reproduction commands and fixture hash
are in [examples/json](../../examples/json/README.md).

Two dimensions are this caller's schema, not a universal GeoJSON constraint:
[RFC 7946](https://www.rfc-editor.org/rfc/rfc7946#section-3.1.1) also permits an
altitude coordinate. Exact-size conversion follows the shape-checked approach
used by [Boost.JSON 1.87.0 tuple-like conversion](https://www.boost.org/doc/libs/1_87_0/libs/json/doc/html/json/conversion.html);
the implementation uses WJR serializers and adds no Boost dependency.
