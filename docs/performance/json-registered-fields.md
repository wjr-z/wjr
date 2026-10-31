# Registered JSON field lookup without temporary keys

Registered from-document constructors and assignments previously called
`object.at(#field)` for each field. The default object is
`std::map<std::string, document, std::less<>>`; its C++20 `at` accepts a key
object, so the string literal creates a temporary string even though the map
supports heterogeneous `find`. Long identifiers allocate character storage.
The PMR object has the same boundary with `std::pmr::string` keys.

The four registered copy/move construction/assignment paths now use an internal
lookup helper. For standard `std::map` with standard byte-string keys and
`std::less<>`, it finds the literal's length-aware `std::string_view` and returns
the existing mapped value with its original constness. The enclosing macro
retains the existing copy or move operation. No public API, value representation,
parser, Unicode validation, duplicate-key policy or output-string ownership
changes. The current `document::at(string_view)` already supports an equivalent
allocation-free handwritten lookup; this change removes overhead from the
registered caller.

If the key is missing, the helper calls the original backend `at(literal)`.
This preserves its exception type/text and construction/assignment ordering,
including partially completed assignment or moves. It adds a lookup on that
failure path and still materializes the missing key there. Custom containers,
nontransparent comparators and custom transparent ordering retain their original
`at` path; transparency alone does not establish identical byte-order semantics.

## Deterministic caller observations

A separate Windows Clang 18.1.8/MSVC 19.42 STL C++20 Release x86-64-v3 executable
counts global `operator new` requests only while constructing the registered
result. Input, reader and parsed DOM are prepared first. The result has three
`uint64_t` fields, so output strings cannot hide key allocations. Its long field
names are `descriptive_counter_larger_than_sso` and
`revision_sequence_larger_than_sso`, alongside `id`; the short case uses
`id`, `count`, `rev`. The checksum is six in every case.

| Projection | Reference `566b6320` calls / requested bytes | Candidate calls / requested bytes |
| --- | ---: | ---: |
| Two long identifiers plus `id` | 2 / 96 | 0 / 0 |
| Three short identifiers | 0 / 0 | 0 / 0 |
| Handwritten existing heterogeneous `document::at` for the long identifiers | 0 / 0 | 0 / 0 |

The short case already fits the measured STL's small-string storage. This is
evidence of removing long-key allocation work, not a measured universal
throughput increase. Independent owned output strings still allocate normally.
Requested bytes are not RSS or allocator heap overhead.

Caller assembly was generated for the actual registered constructor and a
handwritten heterogeneous reference with Clang 18 `-O3 -march=x86-64-v3` against
the fixed old header and candidate. Long-key allocation precedes `map::at` in
the old constructor. The candidate searches using `string_view` first; key
construction/allocation remains reachable on lookup failure. Static allocation
call sites therefore do not disappear from the complete assembly.
The complete probe assembly, including the handwritten reference and emitted
STL/exception support, grows from 2,896 to 3,223 lines (about 11.3%). This is a
single probe's generated assembly-line observation, not a final binary-size
measurement or a code-size improvement claim. Preserving the backend failure
path has a real code-generation tradeoff.

## Validation and reproduction

Six focused cases cover the four registered conversion paths with short and long
keys/owned values, PMR default-resource key-allocation counts, missing-field
exception text and partial assignment, the missing PMR key's retained construction
path, decoded duplicate keys selecting the last value, and custom/nontransparent
comparator fallback. Together with the existing JSON, PMR and minify cases, the
23-case focused suite passes Windows Clang 18 Release. The full clang-format hook
also passes. Linux and MSVC results are delegated to the normal PR matrix; no
unperformed platform timing or sanitizer result is claimed here.

To reproduce the allocation observation, parse the three-field objects above,
then enable an `operator new` request counter only around the registered struct
constructor, disabling it before printing or destroying the DOM. Give the struct
three integer fields with those exact identifiers and use
`WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER`; keep both input preparation and parser
token storage outside the counting interval. Compare the old header at
`566b6320` with the candidate using the same compiler, flags and STL. The PMR
resource counter in [`registered_fields.cpp`](../../test/json/registered_fields.cpp)
is a checked-in regression test of the same field-materialization boundary and
does not require replacing the application's allocator.
