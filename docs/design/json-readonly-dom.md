# An owning, immutable JSON document

Status: design for review. No immutable document API, compact parser, query
speedup, or allocation reduction is implemented by this change. The accompanying
probe measures the existing mutable DOM and checks candidate record sizes.
The default `json::document` remains a mutable, independently owning 16-byte
value; the separate [PMR proposal and implementation](https://github.com/wjr-z/wjr/pull/212)
addresses explicit resource propagation, not this representation.

## Decision and actual consumer boundary

Pursue a separate owning immutable document with borrowed value views, rather
than changing `basic_document` or adding a query cache to its mutable map.
Its first proposed consumer is parse-once/read-many extraction from the
repository Twitter fixture: iterate statuses and repeatedly read selected
decoded fields, then project selected strings and numeric values into an owning
application record. This consumer and its adapter are **not implemented**.
They must ship with the future capability before a performance claim is made.

The source audit at `3021e612` establishes the following existing paths:

| Existing source | Actual behavior | Implication |
| --- | --- | --- |
| [`reader.hpp`](../../include/wjr/json/reader.hpp) | `ondemand_reader::get()` consumes a bounded structural-token batch | Despite its name, it is not a reusable DOM or field-query API |
| [`visitor.hpp`](../../include/wjr/json/visitor.hpp) | One traversal dispatches validation/materialization events | Reuse this checked grammar traversal for a new builder; do not invent a second grammar |
| [`document.hpp`](../../include/wjr/json/document.hpp) | Allocated strings/maps/arrays; transparent map lookup; mutable references and independently movable subtrees | A contiguous document region cannot preserve these reference and lifetime contracts |
| Registered serializers in that header | Struct conversion calls object `at` once per registered field | A real current field-extraction path exists, but there is no evidence of a production repeated-query bottleneck |
| [`test/json/json.cpp`](../../test/json/json.cpp) | Parse a small object, then construct `test_struct0`; also exercise read-only string views | Future typed projection has a semantic reference, not an already existing immutable consumer |
| [`examples/demo/src/main.cpp`](../../examples/demo/src/main.cpp) | Repeatedly call `check` on Twitter | Validation-only work should continue to avoid DOM construction |
| [`inplace_parser.hpp`](../../include/wjr/json/inplace_parser.hpp) | Generic parse entry remains unreachable/stubbed | Do not advertise an existing direct-to-struct optimization |
| [`benchmarks/src/json.cpp`](../../benchmarks/src/json.cpp) | Minification workloads | These do not establish DOM lookup throughput |

There is therefore no justification here for adding a generic query cache,
JSONPath implementation, lazy mutation tracking, or an index to every existing
mutable object. A direct SAX/visitor projection may ultimately beat any DOM for
a single extraction pass and must be included as a competent baseline.

## Measured shape and allocation cost

The [reproducible probe](references/json-dom-shape-probe.cpp) uses the current
default parser, recursively visits its completed tree, and counts requests to
global `operator new` in a separate, non-timed executable. Input and reader token
storage are prepared before counting. The allocation header used by the probe
is excluded from requested-byte totals. Every counted document allocation is
released before the next case. These are requested bytes, not RSS or allocator
usable sizes. The observed fundamental-alignment allocations cover this probe;
it is not a general interception facility for arbitrary over-aligned clients.

Windows Clang 18.1.8, MSVC 19.42 STL, C++20 Release, x86-64-v3:
`sizeof(document/string/object/array)` is `16/32/16/24`. Candidate node/member/view
sizes are `16/12/16`; the candidate record, member and view types are trivially
copyable. Those observations do not establish an ABI or a future implementation.

| Fixture | Input bytes | Values | Objects | Arrays | String values | Object members | Decoded key bytes | Decoded value-string bytes |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| small_config | 117 | 10 | 2 | 1 | 5 | 7 | 45 | 21 |
| strings_8, 512 elements | 5633 | 513 | 0 | 1 | 512 | 0 | 0 | 4096 |
| strings_64, 512 elements | 34305 | 513 | 0 | 1 | 512 | 0 | 0 | 32768 |
| strings_256, 512 elements | 132609 | 513 | 0 | 1 | 512 | 0 | 0 | 131072 |
| numbers_4096 | 19371 | 4097 | 0 | 1 | 0 | 0 | 0 | 0 |
| repository Twitter | 631514 | 13914 | 1264 | 1050 | 4754 | 13345 | 167201 | 200716 |

Twitter has 2,109 numeric values, maximum nesting depth 10, maximum object degree
40, and maximum array length 100. Its arrays hold 568 direct children in total
with 597 allocated element slots. Object degree histogram `(degree: objects)`:
`1:346, 2:202, 3:40, 4:218, 5:101, 9:1, 10:5, 12:5, 23:85, 24:80, 25:8,
39:16, 40:157`. Thus 907 of 1,264 objects have at most five members. A binary
search is a useful baseline, not proof that it beats a contiguous linear scan
for these small objects. The fixture SHA-256 is
`68f2ed1261eeccb70ac34d8cab3c3b8bc7b7b510b6bd3a97ac5636e27e872d3c`.

| Fixture | Allocation calls | Cumulative requested bytes | Live bytes after parse | Peak live requested bytes |
| --- | ---: | ---: | ---: | ---: |
| small_config | 19 | 984 | 968 | 968 |
| strings_8 | 523 | 32854 | 24639 | 24639 |
| strings_64 | 1035 | 73814 | 65599 | 65599 |
| strings_256 | 1035 | 172118 | 163903 | 163903 |
| numbers_4096 | 14 | 131275 | 65599 | 98406 |
| Twitter | 28558 | 1719616 | 1714928 | 1714928 |

These observations identify a storage/locality opportunity in string/object
trees. They also delimit it: the numeric array already has 4,096 contiguous
16-byte values and little persistent overhead. A compact DOM cannot honestly
promise a significant memory improvement for that case. PMR measurements in
[#212](https://github.com/wjr-z/wjr/pull/212) additionally show that resource
reuse can improve Windows string/object throughput while losing on Linux and
numeric arrays; changing allocation policy alone is not a universal remedy.

## Fixed upstream references and what can transfer

| Reviewed release | Relevant released source | Boundary for WJR |
| --- | --- | --- |
| simdjson 3.9.1, [Apache-2.0](https://github.com/simdjson/simdjson/blob/v3.9.1/LICENSE) | [`dom/document.h`](https://github.com/simdjson/simdjson/blob/v3.9.1/include/simdjson/dom/document.h) owns separate tape/string buffers and is move-only; [`tape_ref-inl.h`](https://github.com/simdjson/simdjson/blob/v3.9.1/include/simdjson/internal/tape_ref-inl.h) shows tagged words, extra numeric words, scope skips and length-prefixed strings | Borrow the owner/view split and explicit subtree skipping; an eight-byte tape word is not eight bytes for every value or a complete memory estimate |
| Same simdjson release | [`dom/object-inl.h`](https://github.com/simdjson/simdjson/blob/v3.9.1/include/simdjson/dom/object-inl.h) performs linear field lookup | A mature compact DOM does not imply indexed lookup or constant-time repeated queries |
| Same simdjson release | [OnDemand basics](https://github.com/simdjson/simdjson/blob/v3.9.1/doc/basics.md) describes iterator-like consumption, padding, parser/input lifetime and single-threaded document access | Compare only matching validation/ownership work; do not present lazy traversal as an eager, input-independent DOM equivalent |
| yyjson 0.10.0, [MIT](https://github.com/ibireme/yyjson/blob/0.10.0/LICENSE) | [`yyjson.h`](https://github.com/ibireme/yyjson/blob/0.10.0/src/yyjson.h), `yyjson_val`, `yyjson_doc`, `yyjson_obj_getn` and `YYJSON_READ_INSITU` | Its immutable values are 16 bytes with document lifetime and linear field lookup; in-situ parsing borrows writable padded input. Its separate mutable domain reinforces the ownership boundary |

These are design references, not dependencies added by this proposal. No vendor
code or upstream byte layout is copied. Any later source reuse must retain the
applicable notices and document the exact release/commit. Numeric range,
rounding, duplicate-key and validation rules must be matched explicitly before
using either library as an end-to-end reference.

## Proposed owner and view contracts

Names here are sketches, not installed APIs:

```cpp
class immutable_document; // owns nodes, members and decoded bytes
class value_view;         // borrows a stable immutable_document plus node index
class object_view;
class array_view;
```

The owner is move-only, with explicit deep `clone` if later needed. A value view
contains a pointer to the owner and a 32-bit node index, with no refcount, hidden
allocation or ownership retention. `root() const &` returns a view; the rvalue
overload is deleted. Strings are `std::string_view`, including embedded NUL;
there is no `std::string&`, writable container reference, implicit conversion to
the mutable DOM, or independently owning moved-out child.

Views are valid until owner destruction, reset, replacement or **owner move**.
Moves invalidate views even if the underlying buffer addresses happen to stay
the same: the descriptor borrows the owner object. Avoid a control-block
allocation merely to make views survive owner moves. Copying a view is cheap
and creates an independent traversal position, not a shared cursor.

Successful parse owns every decoded key and string. It can release its input
and reuse the reader immediately. Borrowed/in-situ input and parser-owned
reusable result buffers are separate future modes, excluded from this contract.
There is no default PMR/arena extension or thread-local resource installation.

| Operation | Proposed contract |
| --- | --- |
| `kind` and exact scalar access | Constant time, no mutation/allocation; wrong type reports `INCORRECT_TYPE` |
| String access | Constant time borrowed length-aware view; no lazy decoding or C-string guarantee |
| Object `find(string_view)` | Decoded UTF-8 byte comparison; exact case-sensitive keys; missing key reports `NO_SUCH_FIELD`; no insertion |
| Object iteration | Multipass read-only iteration in decoded lexicographic key order, matching the current map's observable order |
| Array iteration | Multipass forward traversal with constant-time advance using subtree-end indices |
| Array indexed access | Linear in the requested direct-child index in the initial layout; bounds errors report `INDEX_OUT_OF_BOUNDS` |
| Object size/array size | Constant time after construction |
| Conversion to mutable `document` | Explicit deep materialization; may allocate and throw; resulting values own their storage independently |
| Typed projection | Explicit adapter; copying strings makes records independent, string-view fields retain the document lifetime |

Object lookup should initially use one competent, predictable implementation
over sorted contiguous member records. Compare binary search against a simple
linear scan before selecting it; no empirical threshold is decided here.
Do not reuse the existing registered-serializer macro unchanged: it requires
mutable-domain object types/references and existing exception behavior. A
read-only projection adapter must state admission and conversion rules explicitly.

## Candidate storage and its costs

Use three separately owned contiguous buffers, reusing existing container
facilities rather than writing an allocator:

| Buffer | Candidate record | Meaning |
| --- | --- | --- |
| Nodes in preorder | `uint32_t kind; uint32_t extent; uint64_t payload;` (16 bytes) | Scalars retain full 64-bit payload; strings use length plus byte offset; containers use child count plus two 32-bit indices |
| Object members | `uint32_t key_offset, key_length, child_index;` (12 bytes) | Each object's member range is contiguous and sorted by decoded key |
| Decoded bytes | Bytes with explicit offsets/lengths | Keys and value strings share one owned buffer; no per-string object or required trailing NUL |

For an array, container payload stores first child and subtree end; for an
object it stores first member and subtree end. A scalar's next sibling is the
next node; a container's next sibling is its subtree end. Object member records
can point to child nodes in input/preorder order while member iteration follows
the separate sorted range. Numeric loads use the ordinary typed scalar storage
or `memcpy`/`bit_cast` with valid lifetimes, not pointer punning.

Let `N` be reachable values, `M` unique object members, `A` direct array children,
and `K/S` decoded key/value-string bytes. The exact used payload for this model
is `16*N + 12*M + K + S`, excluding buffer capacities, owner, alignment padding,
allocator bookkeeping, parser token storage and construction scratch. It is a
lower bound **for this representation without spare capacity**, not an
information-theoretic lower bound or an achieved allocation result.

| Fixture | Model used payload | Measured current live requested bytes | Optional full array-index cost |
| --- | ---: | ---: | ---: |
| small_config | 310 | 968 | +8 |
| strings_8 | 12304 | 24639 | +2048 |
| strings_64 | 40976 | 65599 | +2048 |
| strings_256 | 139280 | 163903 | +2048 |
| numbers_4096 | 65552 | 65599 | +16384 |
| Twitter | 750681 | 1714928 | +2272 |

A global 32-bit child-index array can provide constant-time random array access
at `4*A` additional used bytes and another buffer. It is not free, is especially
unattractive for the numeric case, and is not part of the initial recommendation.
A variable-width eight-byte tape may reduce some nodes but requires additional
numeric/container/string metadata and more complex access. Compare it only if
the simpler representation's measured ceiling justifies the complexity.
Owner metadata alone can erase the numeric case's 47-byte modeled difference
from the measured mutable DOM, even before capacity or allocator overhead.

Offsets, lengths, counts, subtree ends and node indices require checked arithmetic
before every narrowing, including spare/sentinel indices. Use `size_t` or a wider
accumulator for sums, check buffer `max_size`, and report `CAPACITY` rather than
wrap. A 32-bit field layout does not by itself promise all 4-GiB inputs fit.
Do not assume host endianness creates a persistent format: raw buffer dumping,
memory mapping, cross-process handles and a stable serialized ABI are excluded.

## Construction, duplicate keys and peak memory

Two routes have different costs and must not be conflated:

1. An explicit freeze of an existing mutable DOM can serve as a correctness
   oracle and a way to retain a smaller read-only snapshot. Traverse and copy
   all reachable values and decoded bytes. Existing map order already gives
   sorted unique members. This adds `O(N+M+K+S)` work and keeps both DOMs alive
   during construction; it cannot claim faster parse-to-first-result.
2. A direct builder driven by the current checked visitor is necessary for an
   end-to-end parse allocation claim. It stores indices, never pointers into
   growing buffers. It must reserve/finalize contiguous member ranges despite
   nested objects, and validate every input value before publishing an owner.

Default object parsing is decoded-key **last value wins**: `try_emplace` finds
the prior key and resets its value. The compact domain should match that result,
including escaped/unescaped equivalent keys. It must still validate overwritten
values; an invalid earlier or later duplicate cannot be skipped. This policy is
not automatically supplied by a linear tape or either upstream API.

A direct preorder builder may temporarily retain superseded subtrees and bytes.
It needs explicit per-object member staging, stable duplicate resolution and
reachable-node/byte compaction before claiming the payload model above. A
finalization pass can rewrite child indices into preorder and compact bytes, but
costs another traversal/copy and overlapping old/new buffers. Retaining dead
nodes instead is an alternative only with an honest high-water bound and
duplicate-heavy/adversarial measurements. Do not promise one allocation or one
pass before resolving this choice. Repeated long duplicate keys and deeply
nested containers belong in the acceptance corpus, not only unique Twitter keys.

Buffer growth, sorting member ranges (`O(sum m log m)` comparison work), string
comparison bytes, zero-initialization behavior, finalization and destruction
must all be measured. Temporary space cannot be omitted from peak reports.
String interning is excluded initially: it trades hashing/comparisons and an
index for lower byte duplication and needs separate workload evidence.

## Validation, failure and concurrency

Reuse WJR's existing numeric parser and Unicode decoder, preserving signed,
unsigned and floating distinctions, range/rounding behavior, escaped control
characters, surrogate validation and exact input-tail bounds. Reuse the visitor
depth limit and root/trailing-content checks. Do not add relaxed validation,
comments, non-finite numbers, input padding, or deferred scalar errors silently.
Current `json::result` exposes an error code, not a public byte-offset record;
matching codes does not justify inventing an existing offset guarantee. If a
new diagnostic offset is proposed, define its unit/location and malformed-input
precedence separately and test it explicitly.

The first parse factory should retain the existing terminating allocation-failure
mode rather than silently relaxing old visitor `noexcept` helpers. Grammar/Unicode/
numeric errors return the existing result codes and destroy partial buffers.
Freeze/deep-materialization operations that allocate through ordinary containers
may throw; RAII owns every completed buffer and the target is published only
after success. A recoverable-allocation parse factory is a separate API boundary
and requires a visitor/callback strategy capable of unwinding or carrying that
failure; adding a catch outside terminating helpers is insufficient.

After successful construction and synchronized publication, independent views
may read one stable owner concurrently. Read-only access, iteration, lookup and
string access never fill caches, decode strings, update shared cursors or allocate.
Each thread has its own iterator/view position. Building, moving, resetting,
replacing or destroying the owner concurrently with any reader is forbidden.
Publication/reclamation belongs to the application (for example a retained
`shared_ptr<const owner>` around the whole snapshot); it must not add an atomic
refcount to every value view. Parser/reader instances remain private to a build
operation. External allocator/resource thread safety and lifetime, if ever added,
would need their own contract.

## Implementation gates and coherent delivery

The design review must settle the initial array-access complexity, sorted-member
layout, duplicate-finalization strategy and view invalidation contract first.
No runtime implementation or dependency adoption is authorized by this document
alone. A later implementation should deliver complete, useful stages:

- Storage/view semantics together with freeze, explicit mutable materialization,
  failure cleanup and the actual typed-projection/read-many example. Validate
  owner/input lifetime, moves, wrong types, bounds, embedded NUL and thread-local
  iterator state; measure freeze work and the period with both representations.
- Direct fully validating parse with duplicate normalization, the same consumer,
  parser/builder exception policy, overflow/depth/tail cases and real end-to-end
  allocation/locality/throughput evidence. Include finalization and cleanup in
  timings. Do not stop at an internal record wrapper with no usable consumer.

Compare default WJR mutable DOM, PMR/new-delete and reused standard pool only
where ownership work matches; add simdjson 3.9.1 DOM and yyjson 0.10.0 immutable
DOM with matching strictness/number/duplicate policy. Treat OnDemand and a direct
visitor projection as separate consume-once baselines with explicit differences.
Use same-ISA Windows/Linux x86-64 runs, alternating backend order and reporting
variation, not a cross-machine ratio. Keep allocation counters out of timing.

Required workloads include the six measured shapes, escaped and long keys,
unique/duplicate objects, absent fields, shuffled query order, small and wide
objects, sequential and random array use, Unicode boundaries, malformed roots,
and documents replaced while application readers retain older snapshots.
Report parse-to-projection, repeated-query-only, sequential traversal, explicit
conversion and destruction separately, plus allocations, used/capacity/peak bytes
and code size. Reject changes that merely move work out of the measured region.
The numeric-array case is a required negative control. An implementation may be
useful for string/object snapshots while explicitly directing unsuitable numeric
workloads to the current DOM; no general speedup is assumed here.

## Reproducing the observations

Build `references/json-dom-shape-probe.cpp` as a C++20 executable linked with
`wjr::json`, using the ordinary Release configuration and the source revision
above. Run it from the repository root so the fixture path resolves. It prints
type sizes, exact tree counts, requested allocation counts/live/peak bytes and
the object-degree histogram. The synthetic fixture construction is in that
source. No timing or sanitizer conclusion is drawn from this instrumentation.
For an installed package, a minimal separate CMake project can use:

```cmake
find_package(wjr CONFIG REQUIRED COMPONENTS json)
add_executable(json_dom_shape /path/to/wjr/docs/design/references/json-dom-shape-probe.cpp)
target_compile_features(json_dom_shape PRIVATE cxx_std_20)
target_link_libraries(json_dom_shape PRIVATE wjr::json)
```

The model is directly reproducible from the printed columns using
`16*values + 12*members + key_bytes + string_bytes`; optional array indices add
`4*array_elements`. The probe does not construct that model or benchmark queries.
