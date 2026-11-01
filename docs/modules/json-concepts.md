# JSON serializer concepts

Include `<wjr/json/document.hpp>` for four C++20 capability concepts. They preserve
the existing serializer traits and their customization points:

| Concept | Serializer operation |
| --- | --- |
| `from_document_constructible<T, Source>` | Construct a `T` from the exact source category |
| `from_document_assignable<T, Source>` | Assign an existing `T` from the exact source category |
| `to_document_constructible<Document, Source>` | Construct a document from the exact value category |
| `to_document_assignable<Document, Source>` | Assign an existing document from the exact value category |

Use cv/ref qualifiers explicitly: `const document &`, `document &&` and
`const document &&` can have different capabilities. Existing
`has_*_document` traits remain available, with the same qualification requirements.
Concepts query serializer operations; use the canonical `value_to<T>` factory
when constructing projected values.

```cpp
template <wjr::json::from_document_constructible<const wjr::json::document &> T>
T read(const wjr::json::document &source) {
    return wjr::json::value_to<T>(source);
}

template <wjr::json::from_document_assignable<const wjr::json::document &> T>
void read_into(const wjr::json::document &source, T &target) {
    source.get_to(target);
}
```

A construct-only serializer and an assign-only serializer expose distinct
capabilities. Borrowed `string_view` and containers of views retain their lvalue
source requirement. User serializers may accept only mutable rvalues; const
rvalues do not acquire that capability. Registered fields, nullable values,
fixed-size arrays and allocator-aware documents retain their existing rules.

The document interface uses named constrained parameters where they express a
single capability, and requires clauses for compound conditions. Defaulted type
parameters that defer registered-field inspection until the enclosing class is
complete remain necessary. Legacy integer enable-if parameters are removed.
Field checks keep their existing lazy conjunction behavior.

Construction/assignment predicates retain their trait-based lifetime and exception
contracts. For example, an otherwise supported value with a potentially throwing
destructor remains supported; replacing the predicate with
`std::constructible_from` would add a nothrow-destructor requirement. The concepts
add no runtime state or checks. Runtime conversion, numeric-cast, ownership and
allocator behavior remain those of the selected serializer.

[Compiler-inspection probes](../design/references/json-concepts-codegen.cpp) cover
ordinary scalar, borrowed-view and owned-array calls. Compile with C++20, release
optimization and `WJR_DEBUG_LEVEL=0`; compare function instructions and relocations
against the parent revision, accounting for removed enable-if parameter symbols.
