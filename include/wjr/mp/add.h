#pragma once
#ifndef __WJR_MP_ADD_H
#define __WJR_MP_ADD_H

#include <wjr/mp/type_traits.h>
#include <wjr/asm/adc.h>

_WJR_MP_BEGIN

inline limb_t add(limb_t* rp, const limb_t* ap, size_t n, limb_t bp) {
	assume(WJR_PRE_OR_SEPARATE_P_N(rp, ap, n));
	assume(n > 0);
	limb_t x;
	x = ap[0] + bp;
	rp[0] = x;
	size_t i = 1;
	if (x < bp) {
		while (i != n) {
			x = ap[i] + 1;
			rp[i] = x;
			++i;
			if (x != 0) goto Loop;
		}
		return 1;
	}
Loop:
	if (rp != ap) {
		for (; i < n; ++i)rp[i] = ap[i];
	}
	return 0;
}

inline limb_t add(limb_t* rp, const limb_t* ap, const limb_t* bp, size_t n) {
	assume(WJR_PRE_OR_SEPARATE_P_N(rp, ap, n));
	assume(WJR_PRE_OR_SEPARATE_P_N(rp, bp, n));
	limb_t cf = 0;
	size_t i = n & 3;
#if !defined(WJR_INLINE_ASM)
	switch (i) {
	case 3: {
		rp[0] = masm::adc(ap[0], bp[0], 0, &cf);
		rp[1] = masm::adc(ap[1], bp[1], cf, &cf);
		rp[2] = masm::adc(ap[2], bp[2], cf, &cf);
		break;
	}
	case 2: {
		rp[0] = masm::adc(ap[0], bp[0], 0, &cf);
		rp[1] = masm::adc(ap[1], bp[1], cf, &cf);
		break;
	}
	case 1: {
		rp[0] = masm::adc(ap[0], bp[0], 0, &cf);
		break;
	}
	}
	while (i != n) {
		rp[i] = masm::adc(ap[i], bp[i], cf, &cf);
		rp[i + 1] = masm::adc(ap[i + 1], bp[i + 1], cf, &cf);
		rp[i + 2] = masm::adc(ap[i + 2], bp[i + 2], cf, &cf);
		rp[i + 3] = masm::adc(ap[i + 3], bp[i + 3], cf, &cf);
		i += 4;
	}
#else
	limb_t tmp;
	switch (i) {
	case 1: {
		asm volatile(
			"mov (%3), %1\n\t"
			"add (%4), %1\n\t"
			"mov %1, (%2)\n\t"
			"setb %b0"
			: "=r"(cf), "=&r"(tmp)
			: "r"(rp), "%r"(ap), "r"(bp), "i"(byte_width)
			: "cc"
			);
		break;
	}
	case 2: {
		asm volatile(
			"mov (%3), %1\n\t"
			"add (%4), %1\n\t"
			"mov %1, (%2)\n\t"
			"mov (%c5)(%3), %1\n\t"
			"adc (%c5)(%4), %1\n\t"
			"mov %1, (%c5)(%2)\n\t"
			"setb %b0"
			: "=r"(cf), "=&r"(tmp)
			: "r"(rp), "%r"(ap), "r"(bp), "i"(byte_width)
			: "cc"
			);
		break;
	}
	case 3: {
		asm volatile(
			"mov (%3), %1\n\t"
			"add (%4), %1\n\t"
			"mov %1, (%2)\n\t"
			"mov (%c5)(%3), %1\n\t"
			"adc (%c5)(%4), %1\n\t"
			"mov %1, (%c5)(%2)\n\t"
			"mov (%c5 * 2)(%3), %1\n\t"
			"adc (%c5 * 2)(%4), %1\n\t"
			"mov %1, (%c5 * 2)(%2)\n\t"
			"setb %b0"
			: "=r"(cf), "=&r"(tmp)
			: "r"(rp), "%r"(ap), "r"(bp), "i"(byte_width)
			: "cc"
			);
		break;
	}
	}

	while (i != n) {
		asm volatile(
			"addb $255, %b0\n\t"
			"mov (%3, %5, %c6), %1\n\t"
			"adc (%4, %5, %c6), %1\n\t"
			"mov %1, (%2, %5, %c6)\n\t"
			"mov (%c6)(%3, %5, %c6), %1\n\t"
			"adc (%c6)(%4, %5, %c6), %1\n\t"
			"mov %1, (%c6)(%2, %5, %c6)\n\t"
			"mov (%c6 * 2)(%3, %5, %c6), %1\n\t"
			"adc (%c6 * 2)(%4, %5, %c6), %1\n\t"
			"mov %1, (%c6 * 2)(%2, %5, %c6)\n\t"
			"mov (%c6 * 3)(%3, %5, %c6), %1\n\t"
			"adc (%c6 * 3)(%4, %5, %c6), %1\n\t"
			"mov %1, (%c6 * 3)(%2, %5, %c6)\n\t"
			"setb %b0"
			: "+r"(cf), "=&r"(tmp)
			: "r"(rp), "%r"(ap), "r"(bp), "r"(i), "i"(byte_width)
			);
		i += 4;
	}

#endif // !defined(WJR_INLINE_ASM)

	return cf;
}

inline limb_t add(limb_t* rp, const limb_t* ap, size_t n, const limb_t* bp, size_t m) {
	assume(n >= m);
	limb_t cf;
	cf = add(rp, ap, bp, m);
	if (n > m) {
		cf = add(rp + m, ap + m, n - m, cf);
	}
	return cf;
}

_WJR_MP_END

#endif // __WJR_MP_ADD_H