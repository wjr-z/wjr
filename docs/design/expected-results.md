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

Compressed and ordinary results are different types. Current heterogeneous
expected constructors do not consistently unwrap the source compression tag
when checking error convertibility. Do not assume conversions from compressed
to ordinary expected, or between different compressed value types, are accepted.
Same-type copy/move and ordinary `unexpected(E)` construction are unaffected.
This existing interoperability limitation is not changed by the alias.

## NP shape results

`<wjr/np/checked_view.hpp>` defines `np::shape_result<T>` as
`wjr::expected<T, np::shape_error>`. Checked view, reshape, metadata and owner
factories use this domain spelling. It keeps their exact existing result type,
error values, layout and interoperability: no success sentinel is added to
`shape_error`, and NP results are not silently migrated to compressed storage.
