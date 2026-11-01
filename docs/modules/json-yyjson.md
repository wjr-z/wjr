# Readonly yyjson component

`wjr::yyjson` adapts the independently owning readonly parser from
[yyjson 0.13.0](https://github.com/ibireme/yyjson/tree/6447536015f3d600f3d65323b10976103b337ca7)
([MIT](https://github.com/ibireme/yyjson/blob/6447536015f3d600f3d65323b10976103b337ca7/LICENSE)).
It supplies RAII, explicit borrowed views, native error reporting and C++20
forward ranges. It adds no parser, reflection framework, allocator or shared
control block. The existing mutable `json::document` and its serializers keep
their own representation and contracts.

## Build and use

Install the reviewed upstream release first, then configure WJR with
`-DWJR_ENABLE_YYJSON=ON -DCMAKE_PREFIX_PATH=/path/to/yyjson/install`.
The option defaults OFF: neither ordinary builds nor ordinary installed-package
discovery search for, download or link yyjson. WJR never builds its sources.

```cmake
find_package(wjr CONFIG REQUIRED COMPONENTS yyjson)
target_link_libraries(app PRIVATE wjr::yyjson)
```

Include `<wjr/json/yyjson.hpp>` explicitly. The official upstream installation
exports `yyjson::yyjson` without a ConfigVersion file. WJR therefore checks the
installed target by compiling/linking against exactly `YYJSON_VERSION_HEX ==
0x000D00`, with reader and UTF-8 validation enabled. The public header repeats
that version/feature check; tests verify `yyjson_version()` against the header.
Compatibility with other 0.x releases is not presumed. Repeated component
discovery is supported; an unavailable optional component reports not found.

```cpp
namespace json = wjr::json::yyjson;
yyjson_read_err error{};
auto owner = json::document::parse(R"({"id":42})", &error);
if (!owner) {
    // error.code, error.pos and error.msg are the original native diagnostics.
    return;
}
uint64_t id;
if (!owner.root().get_object_unchecked().find_last("id").try_get_uint(id)) {
    return;
}
```

`parse` always uses `YYJSON_READ_NOFLAG` and the native default allocator. Input
bytes are copied/decoded into document-owned storage and may be released when
the call returns. There is no in-situ mode, implicit file I/O, custom allocator
state, throwing parse exception or mapping to the mutable DOM's error enum.
Failure returns an empty owner and optionally fills the native error struct,
including allocation failures. Native grammar, number rounding/range decisions
and configured depth limits apply.

## Ownership and access

`document` is one pointer, move-only and nonthrowing to move/reset. `value_view`,
`array_view` and `object_view` are each one pointer with no ownership retention.
Moves transfer the native allocation without relocating it: existing views stay
valid while the new owner retains that allocation. Reset, destruction or
replacement of that allocation invalidates all its values, strings and
iterators. The owner must outlive every use of its borrows. Independent readers
can share immutable views with externally guaranteed lifetime; concurrent reset
or mutation through native interop is outside this contract.

`root()` and the owner's `native_handle()` reject both nonconst and const rvalue
owners. Keep the owner in a variable. Temporary value/member/range views can be
used because they borrow the allocation, not the temporary wrapper. Ranges are
multipass common forward/borrowed ranges. Iterators hold one current tape pointer,
with no index counter or hidden allocation. They reuse the pinned header's
`unsafe_yyjson_get_first/get_next` primitives and the native object value accessor;
no byte layout is replicated. A container's next pointer is its end; parsed empty
containers have identical first/end pointers. Native/proxy iteration tests check
empty containers and nested sibling skipping. Borrowed strings preserve explicit
lengths, including embedded NUL.

| Operation | Contract |
| --- | --- |
| View `operator bool` | Pointer validity; JSON null and false remain valid values |
| `try_get_bool/uint/sint/string/array/object(out)` | Exact native kind/subtype; false leaves `out` unchanged |
| `try_get_number(double&)` | Any native numeric subtype; conversion follows `yyjson_get_num` |
| `get_*_unchecked()` | Corresponding `is_*()` must be true; existing WJR assertion/assumption rules apply |
| `array_view::at(i)` | Native indexed access, worst-case linear; out of bounds returns an invalid view |
| `object_view::find_first(key)` | Native linear lookup, first duplicate wins |
| `object_view::find_last(key)` | Linear scan, last duplicate wins, no index allocation |
| Object iteration/size | Input order, includes every duplicate member |

Positive integral literals normally have native unsigned subtype; `sint` does
not reinterpret an unsigned payload or silently narrow it. Use numeric-to-double
access when that is the intended native conversion. This API does not imitate
the mutable DOM's sorted unique map, conversion overloads or last-wins storage.
The [owning status example](../../examples/json/yyjson/statuses.cpp) explicitly
selects last-wins field lookup, copies fields into application records, then
releases the document before using those records.

## Scope and verification

Existing [cpp-yyjson 0.6.0](https://github.com/yosh-matsuda/cpp-yyjson/tree/506d59a8627ae58de92d9934113245ba83872a4c)
provides broader conversion APIs and shared ownership; it also depends on
nameof and fmt. [xyjson 1.0.0](https://github.com/lymslive/xyjson/tree/296a8fa502e69f5996bca7cc872f4d5a76851cd3)
already has one-pointer owners/views, with a different operator/error and
unqualified root-borrowing interface. WJR implements this narrow lifetime/error
contract directly against native yyjson rather than importing either complete
wrapper. Their implementation is not copied.

The [caller probe](../design/references/yyjson-codegen.cpp) compares competent
native C operations and the same wrapper contracts. Windows Clang 18/C++20/O3/
x86-64 baseline, with function sections, emits identical instruction bytes and
relocations for all seven pairs: checked scalar access, unchecked access, array
iteration, lookup and parse/extract/destruction. MSVC 19.42/O2/Ob2/Gy matches the
four scalar pairs exactly; array traversal and lookup retain small instruction
layout differences, while the parse caller is smaller. These are individual
caller checks, not a linked-library-size or throughput claim. No blanket
force-inline policy is imposed on consumers.

The optional tests cover native errors, scalar boundaries, input ownership,
move/reset, duplicate/NUL keys and range lifetime/multipass behavior. Installed
consumer tests check ordinary discovery isolation, repeated component discovery,
and missing required/optional dependencies. The manual yyjson workflow exercises
the real fixed provider and sanitizer configuration without expanding ordinary
Windows CI.
