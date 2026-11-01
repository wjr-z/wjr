# Nullable JSON projection

Include `<wjr/json/optional.hpp>` and link `wjr::json`. `value_to<T>(document)`
explicitly selects the existing document serializer. This matters for types such
as `std::optional<T>` whose own converting constructors can otherwise intercept a
C++ cast. Use `value_to`, rather than `static_cast<std::optional<T>>`.

```cpp
struct reply {
    std::optional<std::int64_t> in_reply_to_status_id;
    std::optional<std::string> in_reply_to_screen_name;
    WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(reply, in_reply_to_status_id,
                                            in_reply_to_screen_name)
};

wjr::json::ondemand_reader reader;
reader.read(R"({"in_reply_to_status_id":null,"in_reply_to_screen_name":"alice"})");
auto parsed = wjr::json::document::parse(reader);
if (parsed) {
    auto row = wjr::json::value_to<reply>(*parsed);
    wjr::json::document roundtrip(row);
}
```

The [installed consumer](../../test/consumer/json_optional.cpp) includes all
required headers and projects an array of these records. Registered fields,
supported vector containers and supported string-keyed maps recursively dispatch
their optional elements through the serializer. Other element types retain their
existing constructor/assignment paths.

| Input/operation | Result |
| --- | --- |
| JSON null to `optional<T>` | Disengaged optional |
| Other JSON value to `optional<T>` | Convert through T's existing serializer; propagate its type/error semantics |
| Empty optional to JSON | Null |
| Engaged optional to JSON | Serialize the contained value, including `false`, zero and empty strings |
| Missing registered field | Existing `std::out_of_range`; missing and null are distinct |
| Repeated JSON key | Existing parser behavior: last value wins |

`value_to<T>` preserves the source's constness and value category. Owning strings
can move from mutable rvalues. Borrowed `string_view` results, including those
nested in optional/array/map/registered fields, require an lvalue document; its
string storage must remain alive and unmodified while the view is used. User
serializers retain their own lifetime contracts. References are not accepted as
`value_to<T>` targets.

An optional contains its T inline; the adapter adds no separate allocation.
Constructing an optional requires a movable or copyable T, even if the input
happens to be null. Assignment to an engaged optional uses T's serializer and
preserves an existing PMR string's destination resource. An initially empty
optional constructs T with that serializer's ordinary construction policy; it
does not inherit the source document's allocator. Assigning an optional to a PMR
document retains the destination document's resource.

Exceptions follow the child operation: a failed new optional construction leaves
no value, and an already engaged T has its serializer's assignment guarantee.
Container/registered assignment can partially update earlier elements or fields;
this is not a transactional decode API. Array construction reserves its result
storage once, then constructs elements without an intermediate document/vector.

JSON has one null representation. Both an empty outer optional and an engaged
outer optional containing an empty inner optional serialize as null; decoding
null always disengages the outer optional. This is not a lossless encoding for
all states of `optional<optional<T>>`.

The canonical entry and null/engaged semantics follow the established approach
documented by [Boost.JSON 1.87.0](https://www.boost.org/doc/libs/1_87_0/libs/json/doc/html/json/conversion.html).
Its [optional conversion source](https://github.com/boostorg/json/blob/boost-1.87.0/include/boost/json/detail/value_to.hpp)
is distributed under Boost Software License 1.0. This implementation reuses WJR's
serializer and ownership contracts; it adds no Boost dependency or copied code.
