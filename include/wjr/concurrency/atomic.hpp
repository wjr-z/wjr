#ifndef WJR_CONCURRENCY_ATOMIC_HPP__
#define WJR_CONCURRENCY_ATOMIC_HPP__

#include <wjr/preprocessor.hpp>

#if defined(WJR_CXX_20)
    #include <atomic>
#else
    #include <boost/atomic.hpp>
#endif

namespace wjr {
#if defined(WJR_CXX_20)
namespace atomic_namespace = std;
#else
namespace atomic_namespace = boost;
#endif

using atomic_namespace::memory_order;
using atomic_namespace::memory_order_acq_rel;
using atomic_namespace::memory_order_acquire;
using atomic_namespace::memory_order_consume;
using atomic_namespace::memory_order_relaxed;
using atomic_namespace::memory_order_release;
using atomic_namespace::memory_order_seq_cst;

using atomic_namespace::atomic;
using atomic_namespace::atomic_flag;
using atomic_namespace::atomic_ref;

using atomic_namespace::atomic_bool;
using atomic_namespace::atomic_char;
using atomic_namespace::atomic_char16_t;
using atomic_namespace::atomic_char32_t;
using atomic_namespace::atomic_int;
using atomic_namespace::atomic_int16_t;
using atomic_namespace::atomic_int32_t;
using atomic_namespace::atomic_int64_t;
using atomic_namespace::atomic_int8_t;
using atomic_namespace::atomic_int_fast16_t;
using atomic_namespace::atomic_int_fast32_t;
using atomic_namespace::atomic_int_fast64_t;
using atomic_namespace::atomic_int_fast8_t;
using atomic_namespace::atomic_int_least16_t;
using atomic_namespace::atomic_int_least32_t;
using atomic_namespace::atomic_int_least64_t;
using atomic_namespace::atomic_int_least8_t;
using atomic_namespace::atomic_intmax_t;
using atomic_namespace::atomic_intptr_t;
using atomic_namespace::atomic_llong;
using atomic_namespace::atomic_long;
using atomic_namespace::atomic_ptrdiff_t;
using atomic_namespace::atomic_schar;
using atomic_namespace::atomic_short;
using atomic_namespace::atomic_size_t;
using atomic_namespace::atomic_uchar;
using atomic_namespace::atomic_uint;
using atomic_namespace::atomic_uint16_t;
using atomic_namespace::atomic_uint32_t;
using atomic_namespace::atomic_uint64_t;
using atomic_namespace::atomic_uint8_t;
using atomic_namespace::atomic_uint_fast16_t;
using atomic_namespace::atomic_uint_fast32_t;
using atomic_namespace::atomic_uint_fast64_t;
using atomic_namespace::atomic_uint_fast8_t;
using atomic_namespace::atomic_uint_least16_t;
using atomic_namespace::atomic_uint_least32_t;
using atomic_namespace::atomic_uint_least64_t;
using atomic_namespace::atomic_uint_least8_t;
using atomic_namespace::atomic_uintmax_t;
using atomic_namespace::atomic_uintptr_t;
using atomic_namespace::atomic_ullong;
using atomic_namespace::atomic_ulong;
using atomic_namespace::atomic_ushort;
using atomic_namespace::atomic_wchar_t;

using atomic_namespace::atomic_flag;
using atomic_namespace::atomic_signal_fence;
using atomic_namespace::atomic_thread_fence;

using atomic_namespace::atomic_ref;

#define WJR_ATOMIC_FLAG_INIT                                                             \
    {}

} // namespace wjr

#endif // WJR_CONCURRENCY_ATOMIC_HPP__