# Bitset ranges

`wjr::bitset<N>` exposes its logical bits as a mutable or const C++20 random-access
range. Iteration runs from bit index zero to `N - 1`, independently of the
high-bit-first order used by `to_string()`. Reverse and const iterator accessors
are available as well.

```cpp
wjr::bitset<130> bits;
std::ranges::fill(bits, true);
std::ranges::reverse(bits);
std::ranges::sort(bits);
```

Mutable iterators return bit references. Assignment through a const proxy writes
the referenced bit; it does not make a const bitset writable. A const bitset
returns iterators that read `bool` values and cannot be output iterators.
`iter_move` reads a value, while `iter_swap` and proxy `swap` exchange bit values
without rebinding proxies. Both same-word and cross-container swaps work.

The range is sized and common, and supports the standard copy, fill, reverse,
rotate and sort algorithms. It is not contiguous: `data()` continues to expose
the backing integer words, whereas dereferencing an iterator exposes one bit.
Algorithms never visit padding bits outside the logical size. Existing
word-oriented operations such as `count()` remain available; per-bit range
algorithms do not replace those specialized operations.

The range accessors and proxy operations allocate no memory and add no state to
the bitset. Iterators and proxies borrow their bitset and must not outlive it.
Their positions must be valid for the operation being performed.
