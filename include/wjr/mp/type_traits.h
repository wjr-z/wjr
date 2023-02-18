#pragma once
#ifndef __WJR_MP_TYPE_TRAITS_H
#define __WJR_MP_TYPE_TRAITS_H

#include <wjr/mp/macro.h>
#include <wjr/type_traits.h>

_WJR_MP_BEGIN

#if defined(WJR_X64)
using limb_t = uint64_t;
using limb_signed_t = int64_t;
using limb_unsigned_t = uint64_t;
#else
using limb_t = uint32_t;
using limb_signed_t = int32_t;
using limb_unsigned_t = uint32_t;
#endif 

constexpr size_t limb_bits = sizeof(limb_t) * 8;
constexpr size_t limb_bytes = sizeof(limb_t);

_WJR_MP_END

#endif // __WJR_MP_TYPE_TRAITS_H