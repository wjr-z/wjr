# Allocator instances for an owning JSON document

Status: the explicit PMR document domain is implemented in
[`<wjr/json/pmr.hpp>`](../../include/wjr/json/pmr.hpp). It provides a borrowed
resource for a complete owning document, using standard PMR facilities.
The default `json::document`, its 16-byte value representation, accessors,
allocation policy, and successful hot paths remain unchanged.

The source audit uses revision `56f2171a` and the allocation observations below
were collected from the unchanged materialization paths at `cd5960c4`.
The boundaries below describe that original default domain. The new opt-in
`pmr_document` closes them with allocator-aware constructors, recursive standard
uses-allocator propagation, and `parse_with_allocator(reader, allocator)`.
It is 24 bytes on the tested x64 toolchains; the original document remains
16 bytes. See the [working consumer](../../examples/json/README.md) and
[measured tradeoffs](../performance/json-pmr.md). Pool reuse improves several
Windows workloads, while Linux and numeric-array results include regressions;
the default document remains the default.

## The measured problem

The default document owns a separately allocated `std::string` object for every
string value, including empty and small strings. Long character buffers allocate
again. Object keys live inside separately allocated `std::map` nodes. This is a
plausible workload for reusing standard pool storage across repeated documents;
it is not evidence that a pool will beat the existing allocator.

Windows observations used Clang 18.1.8, the MSVC 19.42 STL, C++20 Release, and
the i5-13500HX machine. A counting executable intercepted global `operator new`;
reader token storage was prepared before counting. A separate timing executable
used the ordinary CRT allocator. Bytes are sizes requested from `operator new`,
including STL allocation rounding, rather than RSS or allocator heap overhead.

| Owning input | Allocation calls | Requested bytes |
| --- | ---: | ---: |
| Root string, 0 or 8 characters | 1 | 32 |
| Root string, 32 characters | 2 | 80 |
| Root string, 4096 characters | 2 | 4183 |
| Array of 32 empty or 8-character strings | 39 | 2056 |
| Array of 32 32-character strings | 71 | 3592 |
| Object with 32 distinct short keys and empty or 8-character string values | 66 | 3680 |
| Same object with 32-character string values | 98 | 5216 |
| Repository Twitter fixture | 28558 | 1719616 |

The array includes container metadata and growth allocations; the object includes
tree metadata and nodes. These totals are not all attributable to string objects.
Changing 32 short values to 32 long values adds 32 character-buffer allocations.

Two cheaper materialization candidates were rejected before this proposal:

| Candidate | Useful observation | Rejection |
| --- | --- | --- |
| Borrow raw unescaped keys for transparent lookup, then use a valid insertion hint | A 128-field, 4096-character-key object with 90% duplicates reduced calls from 142 to 26 | Twitter throughput median was 0.910 times the reference |
| Decode short keys into a 32-byte stack buffer before lookup | Ordinary synthetic short keys changed by about +4%; duplicate cases benefited | Twitter median was 0.982 times the reference; no clear common-load gain |

The normal default release path also has no full zero-fill pass to remove:
`try_uninitialized_resize` already selects the registered string operation.
C++23 uses `resize_and_overwrite`; known C++20 STLs use the existing specialized
path. ASan and unsupported-container fallbacks intentionally use ordinary resize.
This proposal does not add or extend private STL access.

## Existing allocator boundaries

| Boundary | Current implementation | Consequence |
| --- | --- | --- |
| [Default traits](../../include/wjr/json/document.hpp#L45) | `std::allocator<char>` for strings and `std::allocator<document_type>` for arrays; default map allocator | No allocator instance enters the traits |
| [Payload object creation/destruction](../../include/wjr/json/document.hpp#L461) | `_document_create<T>` and `_document_destroy<T>` construct fresh `std::allocator<T>` instances | String, object, and array metadata do not use an external resource |
| [Parse entry](../../include/wjr/json/document.hpp#L2198) | `parse(reader)` default-constructs its parser and document | No resource argument or retained allocation context |
| [String materialization](../../include/wjr/json/document.hpp#L1915) | Default-constructed string object, resize, decode, final size adjustment | Neither metadata nor characters receive an external allocator |
| [Object keys](../../include/wjr/json/document.hpp#L1964) | Default-constructed temporary string, then `try_emplace` | A PMR map alone does not redirect an ordinary `std::string` buffer |
| [Nested objects/arrays](../../include/wjr/json/document.hpp#L1981) | Default-constructed containers and document elements | No uses-allocator constructor on `basic_document` to receive a container's allocator |
| [Copy/move/assignment](../../include/wjr/json/document.hpp#L538) | Clone payloads, or transfer `m_value`; no allocator state | Their existing guarantees rely on the current allocation model |
| [Swap](../../include/wjr/json/document.hpp#L963) | Swap `m_value`, unconditionally `noexcept` | A future allocator state must not be left attached to storage from an unrelated resource |
| [Relocation](../../include/wjr/json/document.hpp#L2402) | `get_relocate_mode<basic_document<Traits>>` is trivial | A stateful extension needs a fresh proof; the current pointer representation is deliberate |
| Output strings | `dump`/`to_string` allocator template, or caller-supplied `dump_impl` container | Output allocation is separate from DOM allocation |

In the audited original implementation, custom traits can change container types, but cannot inject a runtime
allocator into all these boundaries. Merely substituting PMR strings/maps/vectors
would still leave metadata on `std::allocator` and default-construct resources
at other sites. There is no existing JSON allocator-instance entry to demonstrate
as a complete pool-backed document. The implemented PMR specialization adds this
entry explicitly; it does not promise arbitrary stateful allocator traits.

The existing weak stack allocator is for nested scratch and growth-oriented
lifetimes, not arbitrary independently destroyed DOM values. Its own contract
warns against frequent shrink/reuse patterns. The dynamic object pool owns an
arena and is not a copyable borrowed standard allocator instance. Neither is a
drop-in substitute for the required lifetime and propagation model.

## Fixed upstream ownership references

The reviewed sources are simdjson
[v3.9.1](https://github.com/simdjson/simdjson/tree/v3.9.1), under
[Apache-2.0](https://github.com/simdjson/simdjson/blob/v3.9.1/LICENSE), and yyjson
[0.10.0](https://github.com/ibireme/yyjson/tree/0.10.0), under
[MIT](https://github.com/ibireme/yyjson/blob/0.10.0/LICENSE). This review copies no
upstream implementation, changes no vendored source, and adds no dependency.
The existing minification comparison already uses the pinned simdjson release;
its throughput does not establish a DOM allocation result.

| Reference | Source-grounded allocation/lifetime design | Difference from WJR owning values |
| --- | --- | --- |
| simdjson DOM | [document.h](https://github.com/simdjson/simdjson/blob/v3.9.1/include/simdjson/dom/document.h) stores owning tape and string buffers; document copying is disabled | Elements refer to document storage; independently owned mutable `std::string&` values and detached owning subtrees are a different contract |
| simdjson OnDemand | [parser.h](https://github.com/simdjson/simdjson/blob/v3.9.1/include/simdjson/generic/ondemand/parser.h) owns intermediate/string storage; parsing proceeds during iteration, with input/parser lifetime and padding requirements | Iteration and projected access cannot be timed as if they materialized the entire mutable WJR tree |
| yyjson immutable/mutable DOM | [yyjson.h](https://github.com/ibireme/yyjson/blob/0.10.0/src/yyjson.h) retains an allocator in the document; mutable values/strings use document pools, and values share the document lifetime | Individual values cannot be freed independently; in-situ read mode also changes input ownership/mutability |

The reusable lesson is explicit allocation context and capacity/storage reuse.
It does not justify copying a tape offset, document pool pointer, or byte tag into
the existing WJR value. WJR's default mutable string references and transferable
owning subdocuments would need a separate lifetime/access design before adopting
a document-region representation. The proposed borrowed PMR resource retains
individual typed ownership and matching destruction instead.

An upstream parser benchmark must state eager versus traversed/projected work,
whether strings are copied/unescaped, padding, input lifetime, duplicate-key
policy, numeric range behavior, and validation settings. Use a common valid
input domain or normalize those contracts explicitly. In particular, do not
disable upstream UTF-8 validation to manufacture a result. Read-only region DOM
or OnDemand figures can be useful reference categories, but are not a zero-cost
ceiling for constructing WJR's owning mutable representation. No region-DOM or
OnDemand performance result is claimed by this proposal.

## Proposed capability and scope

Add an opt-in `wjr/json/pmr.hpp` with `json::pmr_document`. Use
`std::pmr::string`, `std::pmr::map`, `std::pmr::vector`, and
`std::pmr::polymorphic_allocator`; reuse
`std::pmr::unsynchronized_pool_resource` in the example and benchmark.
Do not implement another pool, arena, shared owner, pointer encoding, or implicit
process/thread-local allocation context.

The first capability is PMR-specific. It does not promise an arbitrary stateful
allocator protocol with unimplemented propagation traits. Existing custom traits
continue to use their current behavior. A later generic allocator extension must
implement its advertised traits rather than infer PMR semantics for other types.

The opt-in type retains allocator context even while null, boolean, or numeric.
Otherwise inserting a null child and assigning a string later would lose the
parent's resource. The state belongs only to the opt-in document specialization;
the default type receives no resource pointer or reference counting.

Proposed API shape, not an API available in this revision:

```cpp
#include <wjr/json/pmr.hpp>

std::pmr::unsynchronized_pool_resource storage;
using document = wjr::json::pmr_document;
using allocator = document::allocator_type;

wjr::json::ondemand_reader reader;
reader.read(input);
auto result = document::parse_with_allocator(reader, allocator{&storage});

document empty(std::allocator_arg, allocator{&storage});
document copy(std::allocator_arg, allocator{&storage}, *result);
```

Allocator-aware constructors use the standard leading `allocator_arg` convention.
`get_allocator()` returns the borrowed polymorphic allocator by value. Existing
tag/value constructors must have the allocator-aware counterparts actually used
by PMR map/vector construction, including scalar values and `default_construct`.
Do not add unrelated conversion overloads or silently widen existing admission.

Default `document::get<string_t>()` still returns `std::string&`. The opt-in type
returns its `string_type&`, which is `std::pmr::string&`; it does not pretend that
strings with different allocator types are interchangeable references.

## Storage cost and borrowing

Local layout probes on Windows Clang 18.1.8 and MSVC 19.42 observed:

| Type/model | Bytes |
| --- | ---: |
| Existing `document` / `basic_value` | 16 / 16 |
| Existing value plus an ordinary resource pointer, model only | 24 |
| `std::string` / `std::pmr::string` | 32 / 40 |

The 24-byte figure is a model, not an implemented PMR document ABI. A base/member
layout could produce a different result; measure actual Clang/GCC/MSVC objects.
An opt-in 24-byte value increases dense numeric-array element storage by 50%.
PMR strings add their own allocator state, and tree nodes include both key and
mapped representation costs. Pool size classes and retained chunks add slack.
These costs must appear beside upstream-call and throughput improvements.

Embedding a full 32-byte default string in the existing value would require at
least 40 bytes in this ABI, affecting every numeric value as well. That is not
the proposed solution to the separate string-object allocation.

The caller owns the resource. It must outlive every document, detached/moved
subdocument, container, and string that still refers to it. Moving a child out
of a parent does not extend the resource's lifetime. Do not call `release()` while
any such storage is live. No document-side shared ownership is introduced.
An unsynchronized pool is reused sequentially or externally synchronized; a
future concurrent workload can choose an appropriate standard resource instead.

## Full propagation, not just a container typedef

The allocator-aware path must route all four persistent layers to the supplied
resource:

1. Separately allocated string, object, and array metadata.
2. String value buffers and object key buffers.
3. Map nodes and array element storage.
4. Allocator context in newly constructed nested documents, including scalars.

Use rebound polymorphic allocators and standard uses-allocator construction.
C++20 `polymorphic_allocator::new_object<T>` allocates typed metadata, forwards
the allocator into construction, and rolls back that allocation if construction
throws. `delete_object` performs matching destruction and deallocation. A future
implementation may use equivalent standard allocator-traits code when justified,
but must retain the same origin, size, alignment, and construction behavior.

PMR map construction handles its `pair` members individually. The key must be a
PMR string, and the mapped document must expose a compatible `allocator_type`
and matching constructors. PMR vector construction must pass the allocator to
each new document. Explicitly construct the parser's temporary key with the same
resource; it is outside the map until insertion.

An ordinary `std::string` does not use a polymorphic allocator supplied to its
container. An ordinary outer allocator also does not recursively propagate just
because an element contains PMR data. `scoped_allocator_adaptor` can distinguish
outer-node and inner-element allocation, but is unnecessary when the complete
PMR graph already supplies the right uses-allocator protocol. Combining both
mechanisms without a demonstrated need risks double injection.

The standard rules are specified in
[uses-allocator construction](https://eel.is/c++draft/allocator.uses.construction)
and [polymorphic allocator construction](https://eel.is/c++draft/mem.poly.allocator.mem).
The executable appendix checks PMR map keys/mapped children, vector children,
metadata factories, plain outer allocators, and scoped inner propagation.

Reader token storage, parser scratch, numeric-conversion scratch, and caller-owned
serialization output are separate lifetimes. The initial DOM resource API does
not claim to replace these allocators. Continue to support `dump_impl` into an
explicitly constructed PMR output string; do not silently bind returned output
to the DOM resource.

## String filling across standard versions

The default string specialization already has optimized uninitialized growth.
The proposed PMR string is a different allocator specialization. Under C++20 it
must not inherit a new private-STL access trick merely to match that path.
Use safe standard resize as the portable fallback and account for its additional
initialization pass on long strings.

Where `__cpp_lib_string_resize_and_overwrite >= 202110L`, decode inside the
`resize_and_overwrite` callback and return the actual decoded size. Bound input
reads by the original token range, independent of capacity passed by the STL.
Capture a decoding failure for the outer JSON result and leave a valid string
state. Do not let a throwing operation cross that callback's contract.

Both versions keep Unicode/escape validation, error codes, exact input tails,
and decoded output unchanged. Measure C++20 and C++23 separately: PMR pooling
can lose on long strings if the portable fallback adds an otherwise absent pass.
No default-standard upgrade is part of this proposal.

## Copy, move, mutation, swap, and relocation

PMR propagation traits are false and `is_always_equal` is false. Its
`select_on_container_copy_construction` returns a default-constructed allocator,
not the source resource. Follow these rules explicitly; do not infer source
resource retention from a nested PMR container copy.

| Operation on the proposed opt-in type | Resource and ownership rule |
| --- | --- |
| Default construction | Borrow the current default resource at construction |
| Explicit allocator construction | Retain that resource even for a scalar/null |
| Ordinary copy construction | Select the default resource, then deep-copy every persistent layer into it |
| Copy construction with allocator | Deep-copy into the requested resource |
| Ordinary move construction | Retain the source resource and transfer payload; source becomes null |
| Move construction with equal allocator | Transfer payload without allocation |
| Move construction with unequal allocator | Rebuild into the destination resource; release source storage only according to the documented successful-move postcondition |
| Copy assignment | Retain the destination resource; build a replacement before committing |
| Equal-resource move assignment | Retain destination state and transfer payload |
| Unequal-resource move assignment | Rebuild in destination storage; it can allocate and must not be unconditionally `noexcept` |
| Swap | Preserve allocator states; require equal allocators, as allocator-aware containers do when propagation is false |
| Reset, scalar assignment, then later materialization | Destroy old payload through its retained context; keep the resource for the next allocation |

Normal move construction can remain non-throwing for a borrowed resource pointer.
Do not copy the default type's unconditional move-assignment guarantee onto an
unequal-resource operation. Compare resource equality, not just pointer identity:
distinct resource objects can define equality.

Public mutable PMR string/map/vector access must retain standard container
allocator rules. Allocation-aware child construction is also needed for mutations
performed through those references. PMR payload adoption should remain an
internal operation with a proven origin; do not expose an unqualified raw-pointer
adoption API that forgets which resource must free the metadata. Existing default
document constructors and access contracts are preserved.

The current trivial-relocation customization is justified by its pointer-based
representation. A resource pointer plus indirect payload can also be relocatable,
but that needs a specific proof and tests for the selected PMR state. Do not
automatically extend the guarantee to an arbitrary future allocator containing
address-sensitive or nontrivial state, and do not unnecessarily remove the
default document's existing relocation optimization.

## Failure modes and transaction boundaries

The existing parse/visitor path is deliberately `noexcept`; allocation failure
can terminate. Its pointer-transfer and helper assumptions must be read in that
mode. The current factory's allocate-then-construct sequence, swap contract,
`noexcept` declarations, and relocation trait are extension boundaries, not a
reason to diagnose the current supported path as defective or add guards to it.

The first PMR parse entry should explicitly retain the same terminating
allocation-failure mode. Invalid JSON still returns the existing error codes,
and a partial result must destroy its live payloads through the matching resource.
Do not silently turn the old hot visitor/helper into a recoverable engine.

Ordinary allocator-aware constructors and copies that can unwind need typed
metadata rollback and transactional replacement. Use the standard factory's
failure handling for the new path. For an assignment that may allocate, keep the
old payload live until its replacement is complete. Avoid publishing a new type
tag before ownership is established. Unwinding a partially built map/vector must
destroy each successfully constructed child with its correct resource.

A recoverable allocation-failure parse mode is a separate future boundary and
function. Changing only an outer signature cannot catch an exception that has
already crossed a `noexcept` visitor. Such a mode needs a deliberately separated
engine/entry, exact error policy, and failure-injection tests. It is not an
implicit requirement to add cold or hot checks to the old terminating mode.

## One coherent implementation deliverable

The capability is ready for review only when the following pieces work together;
these are work groups, not reasons to split one invariant across tiny MRs.

1. Opt-in type and explicit resource entry: allocator-aware construction, context
   retention for scalars, typed metadata ownership, and unchanged default layout.
2. Complete parser and mutation propagation: temporary keys, values, objects,
   arrays, raw numeric insertion, serializers that materialize new storage, copy,
   move, reset, swap, and the selected failure mode. Keep cross-traits conversion
   out of the initial API unless a real caller needs it.
3. A real owning JSON example using `unsynchronized_pool_resource`, resource
   identity/failure tests, and benchmark/memory evidence against competent
   standard allocation under the same document semantics.

Acceptance requires all of these checks:

- Default `sizeof(document) == 16`, alignment, noexcept/relocation properties,
  overload admission, and representative constructor/move/access/destructor
  assembly remain equivalent. A same-size type alone is not a zero-overhead proof.
- Recursively inspect actual key/value/container allocators after parse,
  insertion, null-to-string assignment, copy, and unequal-resource move. Use
  independent resources; no `set_default_resource` or thread-local injection.
- A tracking resource proves metadata, character buffers, map nodes, array
  storage, and reconstructed children use matching resources and alignments.
  Test unequal-but-equal resource identities, detached children, duplicates,
  escaped aliases, Unicode errors, exact tails, and maximum nesting.
- Constructor/copy failure injection verifies rollback in unwinding operations.
  Subprocess/death tests verify the terminating parse mode without claiming it
  is recoverable. Invalid-input cleanup returns client live bytes to zero.
- PMR resource lifetime, default-resource copy selection, destination-preserving
  assignments, equal/unequal moves, swap preconditions, and accurate traits have
  compile-time and runtime checks. ASan/UBSan cover relevant ownership paths.
- No new Windows matrix is needed. Use existing Windows MSVC/Clang coverage and
  Linux GCC/Clang C++20/C++23 cases, with additional focused manual measurements.

## Performance and memory acceptance

Compare three implementations within each machine/compiler/standard profile:

| Case | Purpose |
| --- | --- |
| Default `document` with its current standard allocator | End-to-end cost of choosing the capability |
| Proposed PMR document with `new_delete_resource` | Isolate PMR state, representation, virtual-resource calls, and fill-path costs |
| Same PMR document with `unsynchronized_pool_resource` | Isolate pooling without changing document semantics again |

Time parsing and destruction together, with the same preloaded input and reader
reuse. Include cold resource creation/destruction and warm resource reuse as
separate cases; never hide cold construction or retained memory in a warm-only
claim. Use Twitter, small configuration objects, string arrays, numeric arrays,
long strings, escaped keys/values, and mixed sizes. Duplicate-heavy data is a
supplement, not the acceptance workload.

Instrumentation runs separately from timing. Report logical client allocation
calls/bytes, upstream calls/bytes, peak client live storage, upstream retained
storage after document destruction, capacity slack, and final release. A pool
can reduce upstream calls while preserving every logical per-string allocation;
do not report that as eliminating string objects or making memory overhead zero.
Keep object/code sizes, compiler/ISA flags, raw samples, variation, and losses.

Only claim a benefit after Windows and Linux same-domain results support the
targeted workload. A useful opt-in workload may trade memory for throughput,
but the trade must be explicit, and default users must not pay for it.

## Standard-library proof and remaining uncertainty

The appendix was compiled and run with Windows Clang 18.1.8 and MSVC 19.42 using
C++20 and the MSVC STL. Both observed the layout table above, 13 logical resource
allocation calls, 9 upstream calls, and 1888 upstream-live bytes before pool
destruction. Client live bytes returned to zero after object deletion; upstream
live bytes returned to zero after pool destruction. It also checks constructor
rollback and copy-resource selection.

Those numbers describe a tiny standard-library propagation proof, not a JSON
parser benchmark, not a PMR DOM implementation, and not a promise about other
STLs' pool geometry. Linux layout/propagation runs and all DOM throughput,
retention, and failure results remain implementation acceptance work.

The source appendix is intentionally independent of the proposed API. It proves
which standard mechanisms can be reused while leaving the owning document's
state, parser, and mutation work visible for review.

## Executable propagation proof

Save the following as `proof.cpp` and compile against repository headers after
initializing fmt. For example, from the repository root:

```sh
clang++ -std=c++20 -O2 -DWJR_DEBUG_LEVEL=0 -Iinclude \
  -Ithird-party/fmt/include proof.cpp -o proof
./proof
```

MSVC additionally uses `/std:c++20 /EHsc /utf-8 /Zc:preprocessor /Zc:lambda` and
the corresponding `/I` include options. No WJR library is linked: WJR types are
queried for layout only, and the exercised allocation mechanisms are standard PMR.


```cpp
#include <cassert>
#include <cstddef>
#include <iostream>
#include <map>
#include <memory>
#include <memory_resource>
#include <scoped_allocator>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <wjr/json/document.hpp>

class counting_resource : public std::pmr::memory_resource {
public:
    explicit counting_resource(std::pmr::memory_resource *upstream) : upstream(upstream) {}
    std::size_t calls = 0;
    std::size_t live_bytes = 0;

private:
    void *do_allocate(std::size_t bytes, std::size_t alignment) override {
        auto *pointer = upstream->allocate(bytes, alignment);
        ++calls;
        live_bytes += bytes;
        return pointer;
    }
    void do_deallocate(void *pointer, std::size_t bytes, std::size_t alignment) override {
        assert(live_bytes >= bytes);
        live_bytes -= bytes;
        upstream->deallocate(pointer, bytes, alignment);
    }
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
        return this == &other;
    }
    std::pmr::memory_resource *upstream;
};

struct child {
    using allocator_type = std::pmr::polymorphic_allocator<std::byte>;
    explicit child(std::string_view text) : text(text) {}
    child(std::allocator_arg_t, const allocator_type &allocator, std::string_view text)
        : text(text, allocator) {}
    child(std::allocator_arg_t, const allocator_type &allocator, const child &other)
        : text(other.text, allocator) {}
    child(std::allocator_arg_t, const allocator_type &allocator, child &&other)
        : text(std::move(other.text), allocator) {}
    std::pmr::string text;
};

struct throwing_child {
    using allocator_type = child::allocator_type;
    throwing_child(std::allocator_arg_t, const allocator_type &) { throw 17; }
};

struct state_layout {
    std::pmr::memory_resource *resource;
    wjr::json::basic_value value;
};

int main() {
    using allocator = child::allocator_type;
    static_assert(std::uses_allocator_v<child, allocator>);
    static_assert(std::uses_allocator_v<std::pmr::string, allocator>);
    static_assert(!std::uses_allocator_v<std::string, allocator>);
    static_assert(!std::uses_allocator_v<wjr::json::document, allocator>);
    static_assert(!std::allocator_traits<allocator>::propagate_on_container_copy_assignment::value);
    static_assert(!std::allocator_traits<allocator>::propagate_on_container_move_assignment::value);
    static_assert(!std::allocator_traits<allocator>::propagate_on_container_swap::value);
    static_assert(!std::allocator_traits<allocator>::is_always_equal::value);
    std::cout << "document=" << sizeof(wjr::json::document)
              << ",value=" << sizeof(wjr::json::basic_value)
              << ",pointer_state_layout=" << sizeof(state_layout)
              << ",string=" << sizeof(std::string) << ",pmr_string=" << sizeof(std::pmr::string)
              << '\n';

    counting_resource upstream(std::pmr::new_delete_resource());
    {
        std::pmr::unsynchronized_pool_resource pool(&upstream);
        counting_resource client(&pool);
        allocator allocate(&client);
        const std::string_view text = "a string longer than every tested small-string capacity";
        using map_type = std::pmr::map<std::pmr::string, child, std::less<>>;
        auto *map = allocate.new_object<map_type>();
        map->emplace(std::piecewise_construct, std::forward_as_tuple(text),
                     std::forward_as_tuple(text));
        assert(map->get_allocator().resource() == &client);
        assert(map->begin()->first.get_allocator().resource() == &client);
        assert(map->begin()->second.text.get_allocator().resource() == &client);
        auto *array = allocate.new_object<std::pmr::vector<child>>();
        array->emplace_back(text);
        assert(array->front().text.get_allocator().resource() == &client);
        auto *string = allocate.new_object<std::pmr::string>(text);
        assert(string->get_allocator().resource() == &client);

        auto copied = *string;
        assert(copied.get_allocator().resource() == std::pmr::get_default_resource());
        assert(std::allocator_traits<allocator>::select_on_container_copy_construction(allocate)
                   .resource() == std::pmr::get_default_resource());

        {
            using pair_type = std::pair<const std::pmr::string, child>;
            using outer = std::allocator<pair_type>;
            std::map<std::pmr::string, child, std::less<>, outer> plain;
            plain.emplace(std::piecewise_construct, std::forward_as_tuple(text),
                          std::forward_as_tuple(text));
            assert(plain.begin()->first.get_allocator().resource() ==
                   std::pmr::get_default_resource());
            assert(plain.begin()->second.text.get_allocator().resource() ==
                   std::pmr::get_default_resource());

            using scoped = std::scoped_allocator_adaptor<outer, allocator>;
            std::map<std::pmr::string, child, std::less<>, scoped> nested(
                std::less<>{}, scoped(outer{}, allocate));
            nested.emplace(std::piecewise_construct, std::forward_as_tuple(text),
                           std::forward_as_tuple(text));
            assert(nested.begin()->first.get_allocator().resource() == &client);
            assert(nested.begin()->second.text.get_allocator().resource() == &client);
        }

        const auto before = client.live_bytes;
        try {
            (void)allocate.new_object<throwing_child>();
            return 1;
        } catch (int error) {
            assert(error == 17);
        }
        assert(client.live_bytes == before);
        allocate.delete_object(string);
        allocate.delete_object(array);
        allocate.delete_object(map);
        assert(client.live_bytes == 0);
        std::cout << "logical_resource_calls=" << client.calls
                  << ",upstream_calls=" << upstream.calls
                  << ",upstream_live_before_pool_destruction=" << upstream.live_bytes << '\n';
    }
    assert(upstream.live_bytes == 0);
    std::cout << "deep_resource_and_constructor_rollback=pass\n";
}
```
