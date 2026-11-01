# JSON object container conversion

The existing document serializers convert standard string-keyed `std::map`,
`std::multimap` and `std::unordered_map` containers to/from owning JSON objects.
For example, a dictionary can be parsed once and projected into its usual hash map:

```cpp
wjr::json::ondemand_reader reader;
reader.read(R"({"one":1,"two":2})");
auto parsed = wjr::json::document::parse(reader);
if (parsed) {
    auto values = static_cast<std::unordered_map<std::string, int>>(*parsed);
    wjr::json::document roundtrip(values);
}
```

Include `<unordered_map>`, `<string>` and `<wjr/json/document.hpp>` for this example.
The document owns its keys/values independently of the input. Unordered input is
represented by the document's ordinary object container; iteration order is not
preserved as an unordered hash-table layout. Parsing/duplicate-key/error semantics
are unchanged. This is ordinary eager conversion, not direct SAX decoding.

## Exact object types retain their container operations

When the C++ type is exactly `Document::object_type`, dispatch delegates to its
existing copy/move constructor or assignment. The object serializer previously
compared against `Document::array_type`, so these paths were never selected for
ordinary map-backed objects. `std::unordered_map` also incorrectly selected the
array serializer, rejecting ordinary dictionary construction/projection.

| Operation | Exact-container behavior |
| --- | --- |
| const document to a new object container | Copy constructor, including its comparator and allocator copy-selection rules |
| const document to an existing object | Copy assignment; assigning to the same object storage follows the container's self-copy behavior |
| rvalue document to a new object | Move constructor; map nodes/values transfer according to the container's guarantees |
| rvalue document to an existing object | Move assignment; allocator propagation/equality determines node transfer versus rebuilding |
| exact object to a document | Construct owned object metadata with the destination document's allocator, then use the container copy/move operation |

Moving out a default `document::object_type` preserves the addresses of transferred
map nodes. The original document retains a valid moved-from object container;
destroying/resetting it does not destroy nodes now owned by the extracted map.
Copying produces independent values. Stateful exact-object comparators follow the
underlying container operation instead of being silently default-constructed by a
range constructor. Nonexact object types still use element conversion; moving
between different container types does not imply that their nodes can be stolen.

This does not add a general aliasing guarantee to assignment of an object already
nested inside the destination document. The checked self-copy case is specifically
`doc.get_to(doc.get<object_t>())`, whose destination is the exact object container.
The conversion syntax and reference qualifications retain the existing interface.

## PMR ownership and allocation observations

An exact PMR object extracted by move construction carries the source resource,
like the existing exact-array move path. That resource must outlive the returned
container and its moved-out child values, even after the source document dies.
Moving ownership does not extend a borrowed `memory_resource` lifetime.

Copy construction selects the standard PMR default resource. Assignment retains
the destination resource; unequal-resource moves can rebuild nodes and throw.
Object construction inside an explicit-resource document propagates its resource
through the existing uses-allocator path. Equal resources may exchange storage;
their equality does not combine their lifetimes.

The tests compare conversion allocations against a handwritten move construction
of the same standard container, allocator and contents. They do not assume every
STL has allocation-free map moves. With Windows Clang 18 / MSVC 19.42 STL C++20
baseline, the two-field fixture (96-byte key, 128-byte string value and an integer)
previously made six requests to the default resource during extraction. Exact
dispatch makes zero such requests and one request to the source resource, matching
the handwritten `std::pmr::map` move's sentinel allocation on this STL. The map nodes
themselves retain their addresses. libstdc++ or another STL may have different
sentinel storage; the test derives the matching reference on that platform.

This is a deterministic allocation/ownership observation, not a throughput or RSS
claim. The source document's object metadata remains until it is reset/destroyed.
Object-to-document conversion also needs its normal object metadata allocation;
the change does not claim all directions require zero allocations.

Tests cover exact copy/move construction and assignment in both directions,
self-copy projection, comparator state, ordinary map/multimap/unordered-map
fallbacks, PMR resource selection/retention and failure at every allocation of an
exact PMR map copy. These tests check source preservation and cleanup without
requiring a stronger guarantee than the underlying container for arbitrary
assignment failure. No parser or UTF-8 implementation is replaced.

## Borrowed value projections

An existing live owning document can supply `std::string_view` values without
copying their bytes. This works for const and mutable lvalues, including views
nested in standard array/dictionary conversions and registered structures:

```cpp
struct row_view {
    row_view() = default;
    std::string_view name;
    std::vector<std::string_view> tags;
    WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER(row_view, name, tags)
};

row_view project(const wjr::json::document &source) {
    return static_cast<row_view>(source);
}
```

Include `<string_view>`, `<vector>` and `<wjr/json/document.hpp>`. The caller must
keep the source document and its string storage alive while using the returned
views. Destroying/resetting a source node or invalidating its string storage also
invalidates its views. PMR documents retain the additional requirement that their
borrowed resource outlive the document. Copying a view does not extend either
lifetime. The projection is independent of the parser's input buffer because an
owning document has already materialized its strings. Containers still allocate
their own element/node storage; this is not allocation-free DOM construction.

Direct string views, array elements, dictionary mapped values and registered
fields that reject rvalue documents also reject conversion/assignment from a
temporary document, including const rvalues. A const-reference copy fallback must
not bypass that rejection. Owning values such as `std::string`, their containers
and owning registered structures retain copy/move conversions. Const-rvalue
dispatch checks the actual const-qualified source instead of advertising a
mutable-rvalue-only serializer.

These checks follow each serializer's declared source-category contract; they
cannot discover pointers hidden in arbitrary user-defined values. A custom
serializer or constructor that accepts rvalues is responsible for the lifetime
of its result. Dictionary keys keep their existing container construction rules:
use owning `std::string` keys for these projections. In particular, implicit
`std::string`-to-`string_view` key conversion is not a general ownership guarantee
and this change does not introduce a key-lifetime inference system. Existing type
errors, missing-field exceptions and duplicate-key parsing policy are unchanged.
