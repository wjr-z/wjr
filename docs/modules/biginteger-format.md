# Big integer formatting

Link `wjr::biginteger` and include `<wjr/biginteger.hpp>`.

The fmt formatter supports decimal, lowercase binary/octal/hexadecimal,
`+`/`-`/space sign options, alternate base prefixes, fixed or dynamic width,
alignment, and fill. It follows the integer formatting rules of the pinned
[fmt 11.2 syntax](https://fmt.dev/11.2/syntax/).

```cpp
const wjr::biginteger value(-255);
fmt::format("{:#x}", value);     // "-0xff"
fmt::format("{:#012x}", value);  // "-0x0000000ff"
fmt::format("{: }", wjr::biginteger(255)); // " 255"
```

Signs precede base prefixes; numeric zero padding follows both. Default numeric
alignment is right. Alternate octal zero is `0`; alternate hexadecimal/binary
zero retains its prefix, matching fmt integers. Uppercase, localized, character,
floating-point and precision presentations throw `fmt::format_error`.

Stream insertion supports decimal/octal/hexadecimal with the existing
sign-magnitude representation: negative hexadecimal is `-0xff`, rather than a
fixed-width two's-complement bit pattern. Both ordinary and custom-traits streams
place the sign before `showbase`; `showpos`, left/right padding, and one-shot
width remain supported. This does not change the separate parsing contract.

Formatting borrows the original limbs without copying or changing the integer.
The existing stack-allocator-backed character buffer stores the representation;
fmt emits numeric padding directly to the destination, without another padded
temporary buffer.
