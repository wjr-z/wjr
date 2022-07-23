/*
  Copyright (c) 2009 Florian Loitsch

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation
  files (the "Software"), to deal in the Software without
  restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following
  conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.
  */
#pragma once
#ifndef __WJR_DIY_FP_H
#define __WJR_DIY_FP_H

#include <cstdint>
#include <cassert>
#include <type_traits>

#include "../mmacro.h"

struct diy_fp_t {
	uint64_t f;
	int e;
};

constexpr diy_fp_t minus(diy_fp_t x, diy_fp_t y) {
	WASSERT_LEVEL_1(x.e == y.e);
	WASSERT_LEVEL_1(x.f >= y.f);
	return diy_fp_t{ x.f - y.f, x.e };
}

WJR_CONSTEXPR20 diy_fp_t multiply(diy_fp_t x, diy_fp_t y) {
#if defined(_MSC_VER) && (defined(__x86_64__) || defined(_M_X64))
#if defined(WJR_CPP_20)
	if (!std::is_constant_evaluated())
#endif
	{
		diy_fp_t r;
		r.f = __umulh(x.f, y.f);
		r.e = x.e + y.e + 64;
		return r;
	}
#elif defined(__SIZEOF_INT128__)
	{
		diy_fp_t r;
		r.f = ((__uint128_t)(x.f) * (__uint128_t)(y.f)) >> 64;
		r.e = x.e + y.e + 64;
		return r;
	}
#endif
	uint64_t a, b, c, d, ac, bc, ad, bd, tmp;
	diy_fp_t r;
	uint64_t M32 = 0xFFFFFFFF;
	a = x.f >> 32; b = x.f & M32;
	c = y.f >> 32; d = y.f & M32;
	ac = a * c; bc = b * c; ad = a * d; bd = b * d;
	tmp = (bd >> 32) + (ad & M32) + (bc & M32);
	tmp += 1U << 31; /// mult_round
	r.f = ac + (ad >> 32) + (bc >> 32) + (tmp >> 32);
	r.e = x.e + y.e + 64;
	return r;
}

#endif
