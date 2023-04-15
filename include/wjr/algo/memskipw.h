#ifndef __WJR_ALGO_ALOG_H
#error "This file should not be included directly. Include <wjr/algo.h> instead."
#endif

#if defined(_WJR_FAST_MEMSKIPW)

_WJR_ALGO_BEGIN

static inline const bool __memskipw_table[] = {
false,false,false,false,false,false,false,false,false, true, true, true, true, true,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
 true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false
};

_WJR_ALGO_END

#define __WJR_MEMSKIPW_ONE(st, ptr)

#define WJR_SIMD_IS_BACKWARD 0

#define __WJR_MEMSKIPW_NAME __memskipw
#define __WJR_MEMSKIPW_PRED(x) (!__memskipw_table[x])
#include <wjr/algo/memskipw-impl.h>
#undef __WJR_MEMSKIPW_NAME

#undef WJR_SIMD_IS_BACKWARD

#define WJR_SIMD_IS_BACKWARD 0

#define __WJR_MEMSKIPW_NAME __memskipnw
#define __WJR_MEMSKIPW_PRED(x) (__memskipw_table[x])
#include <wjr/algo/memskipw-impl.h>
#undef __WJR_MEMSKIPW_NAME

#undef WJR_SIMD_IS_BACKWARD

#undef __WJR_MEMSKIPW_ONE
#undef __WJR_MEMSKIPW_FOUR

#endif // _WJR_FAST_MEMSKIPW