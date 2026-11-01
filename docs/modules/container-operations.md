# Generic container operations

`<wjr/container/detail.hpp>` provides the existing `wjr::resize` and `wjr::append`
function objects, customization classes, detection traits and uninitialized-growth
helpers. These operations support JSON serialization and bulk copy; they add no
container state or runtime dispatch.

`resizable_container<Container, Args...>` and
`appendable_container<Container, Args...>` check the actual call expression on a
container **lvalue**, preserving forwarded argument categories. Unsupported calls
fail constraints, including inside `std::invocable`. A container's own unconstrained
member-template body is outside expression checking, as with ordinary requires.

```cpp
template <wjr::resizable_container<std::size_t &> Container>
constexpr void set_count(Container &cont, std::size_t count)
    noexcept(noexcept(wjr::resize(cont, count))) {
    wjr::resize(cont, count);
}
```

The example tests `size_t &` because `count` is a named lvalue. For a forwarded
rvalue argument, query the corresponding non-reference type instead. Calls borrow
the target and forward arguments without introducing ownership or allocation.
The function objects propagate the customization's exception specification and
support constant evaluation when the selected implementation does.

Keep existing `resize_fn_impl<Container>` and `append_fn_impl<Container>`
specializations for types whose behavior is not a public member. A customization's
return value is ignored, as before. Legacy `has_container_*` traits remain available;
append/reserve/insert detection uses the same lvalue target as actual consumers.

The uninitialized helpers choose only at compile time:

- Resize prefers `resize(count, default_construct)`, then ordinary resize.
- Append prefers `append(count, default_construct)`, then `resize(size()+count)`
  through the resize helper. These retain the existing priority for containers
  supporting those paths. An append-only container can finally fall back to its
  ordinary `append(count)` without requiring a nonexistent resize/size operation.

The count parameter and global tag are checked in their actual local-lvalue and
const-tag categories. A tag-only customization can be used directly; unsupported
fallbacks fail constraints. Helpers retain their existing exception contract.
Previously valid ordinary-member/customization paths retain their growth and
default-initialization semantics. Lvalue-qualified bulk append also becomes
eligible for optimized copy. Primitive elements appended via WJR's
default-initialized path must be filled before being read.
