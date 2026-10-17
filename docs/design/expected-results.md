# Result type aliases

`<wjr/expected.hpp>` provides the optional shorthand
`compressed_expected<T, E, Success>` for
`expected<T, compressed_value<E, Success>>`. It adds no wrapper object or state.
A domain can define a short result type once:

```cpp
enum class parse_error { success, invalid_input };
template <class T>
using parse_result = wjr::compressed_expected<T, parse_error, parse_error::success>;

parse_result<void> validate(bool valid) {
    if (!valid)
        return wjr::unexpected(parse_error::invalid_input);
    return {}; // success, stored as parse_error::success
}
```

`error_type` remains `E`; callers use ordinary `unexpected(E)`, value access,
`has_value()` and error access. `Success` is reserved for the value state.
Passing it to an error constructor violates the existing compressed expected
contract: it cannot represent an error with that code. The alias does not add
checks or change construction, destruction, exception or allocation behavior.

Compression is a representation choice, not a promise of faster code or a
smaller object for every `T`. On the measured x64 MSVC/Clang/GCC configurations,
an enum with `sizeof(E)==4` gives an eight-byte ordinary `expected<void,E>` and
a four-byte compressed result. NP owner, view and checked-metadata results had
the same size and alignment with either representation. A standalone boolean
state query used a byte load for ordinary expected, versus compare/set for the
compressed representation. Conversely, compressed error construction omitted
the separate boolean-state store in those probes. These are code-generation
observations, not throughput measurements.

Compressed and ordinary results are different types. Heterogeneous expected
constructors check conversion from the source's underlying error type, while
retaining the complete source expected type for the usual constructor constraints.
They support const-reference and rvalue conversion between compressed and ordinary
results, and between different compressed value types, when their value and error
conversions are valid. Explicit value/error conversions still produce an explicit
expected constructor. No storage or exception-cleanup mechanism is changed.
For a compressed destination, converting an error must not produce its reserved
success value.

## NP shape results

`<wjr/np/checked_view.hpp>` defines `np::shape_result<T>` as
`wjr::compressed_expected<T, np::shape_error, np::shape_error::ok>`. Checked view,
reshape, metadata and owner factories, including expression materialization,
actually use this representation. State is encoded by the stored error code:
`ok` means a value is present, and every existing error code means failure.
`ok` is appended after the existing errors, preserving their numeric values.
It is reserved: never pass `shape_error::ok` to `unexpected`, an error constructor,
error assignment or a conversion whose source is in an error state.

This changes NP result types and ABI, even when sizeof/alignof are unchanged;
rebuild both producers and consumers. The measured non-void owner/view/metadata
results retain their previous sizes, while `shape_result<void>` drops the separate
boolean and occupies one four-byte error code on the tested x64 configurations.
This is not a blanket space or speed improvement.

An explicitly spelled old `expected<T, shape_error>` result can still receive a
new factory result through heterogeneous conversion, including read-only view
conversion. Such compatibility conversions can copy/move the value; factories
themselves construct `shape_result` directly, without an intermediate ordinary
expected. Exact type identity and function-pointer return signatures do change.
