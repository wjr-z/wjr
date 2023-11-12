#ifndef WJR_ATOMIC_HPP__
#define WJR_ATOMIC_HPP__

#include <atomic>

#include <wjr/type_traits.hpp>

#define WJR_ATOMIC_DEFINE_TYPE(name, types, vars, veris)                                 \
    template <typename T, std::enable_if_t<std::is_integral<T>::value, int> = 0>         \
    static inline auto atomic_##name(WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(         \
        WJR_PP_QUEUE_ZIP_2(types, vars), WJR_ATOMIC_CONCAT))) noexcept

#define WJR_ATOMIC_FAST_DEFINE(name, types, vars, veris)                                 \
    WJR_ATOMIC_DEFINE_TYPE(name, types, vars, veris) {                                   \
        WJR_ATOMIC_VERIFY(veris);                                                        \
        return __atomic_##name(WJR_PP_QUEUE_EXPAND(vars));                               \
    }

#define WJR_ATOMIC_CONCAT(x) WJR_ATOMIC_CONCAT_I x
#define WJR_ATOMIC_CONCAT_I(a, b) a b

#define WJR_ATOMIC_VERIFY(veris)                                                         \
    WJR_PP_QUEUE_PUT(WJR_PP_QUEUE_TRANSFORM(veris, WJR_ATOMIC_VERIFY_IMPL))
#define WJR_ATOMIC_VERIFY_IMPL(ptr)                                                      \
    WJR_ASSERT(3, reinterpret_cast<::wjr::uintptr_t>(ptr) % sizeof(T) == 0,              \
               "Atomic operation used incorrectly aligned pointers");

namespace wjr {

WJR_ATOMIC_FAST_DEFINE(load_n, (T *, std::memory_order), (ptr, order), (ptr));
WJR_ATOMIC_FAST_DEFINE(load, (T *, T *, std::memory_order), (ptr, ret, order),
                       (ptr, ret));
WJR_ATOMIC_FAST_DEFINE(store_n, (T *, T, std::memory_order), (ptr, val, order), (ptr));
WJR_ATOMIC_FAST_DEFINE(store, (T *, T *, std::memory_order), (ptr, val, order),
                       (ptr, val));
WJR_ATOMIC_FAST_DEFINE(exchange_n, (T *, T, std::memory_order), (ptr, val, order), (ptr));
WJR_ATOMIC_FAST_DEFINE(exchange, (T *, T *, std::memory_order), (ptr, val, order),
                       (ptr, val));
WJR_ATOMIC_FAST_DEFINE(compare_exchange_n,
                       (T *, T *, T, bool, std::memory_order, std::memory_order),
                       (ptr, expected, desired, weak, success_order, failure_order),
                       (ptr, expected));
WJR_ATOMIC_FAST_DEFINE(compare_exchange,
                       (T *, T *, T *, bool, std::memory_order, std::memory_order),
                       (ptr, expected, desired, weak, success_order, failure_order),
                       (ptr, expected, desired));
WJR_ATOMIC_DEFINE_TYPE(compare_exchange_strong_n,
                       (T *, T *, T, std::memory_order, std::memory_order),
                       (ptr, expected, desired, success_order, failure_order),
                       (ptr, expected)) {
    WJR_ATOMIC_VERIFY((ptr, expected));
    return atomic_compare_exchange_n(ptr, expected, desired, false, success_order,
                                     failure_order);
}
WJR_ATOMIC_DEFINE_TYPE(compare_exchange_weak_n,
                       (T *, T *, T, std::memory_order, std::memory_order),
                       (ptr, expected, desired, success_order, failure_order),
                       (ptr, expected)) {
    WJR_ATOMIC_VERIFY((ptr, expected));
    return atomic_compare_exchange_n(ptr, expected, desired, true, success_order,
                                     failure_order);
}
WJR_ATOMIC_DEFINE_TYPE(compare_exchange_strong,
                       (T *, T *, T *, std::memory_order, std::memory_order),
                       (ptr, expected, desired, success_order, failure_order),
                       (ptr, expected)) {
    WJR_ATOMIC_VERIFY((ptr, expected));
    return atomic_compare_exchange(ptr, expected, desired, false, success_order,
                                   failure_order);
}
WJR_ATOMIC_DEFINE_TYPE(compare_exchange_weak,
                       (T *, T *, T *, std::memory_order, std::memory_order),
                       (ptr, expected, desired, success_order, failure_order),
                       (ptr, expected)) {
    WJR_ATOMIC_VERIFY((ptr, expected));
    return atomic_compare_exchange(ptr, expected, desired, true, success_order,
                                   failure_order);
}

WJR_ATOMIC_FAST_DEFINE(add_fetch, (T *, T, std::memory_order), (ptr, val, order), (ptr));
WJR_ATOMIC_FAST_DEFINE(sub_fetch, (T *, T, std::memory_order), (ptr, val, order), (ptr));
WJR_ATOMIC_FAST_DEFINE(and_fetch, (T *, T, std::memory_order), (ptr, val, order), (ptr));
WJR_ATOMIC_FAST_DEFINE(xor_fetch, (T *, T, std::memory_order), (ptr, val, order), (ptr));
WJR_ATOMIC_FAST_DEFINE(or_fetch, (T *, T, std::memory_order), (ptr, val, order), (ptr));
WJR_ATOMIC_FAST_DEFINE(nand_fetch, (T *, T, std::memory_order), (ptr, val, order), (ptr));

WJR_ATOMIC_FAST_DEFINE(fetch_add, (T *, T, std::memory_order), (ptr, val, order), (ptr));
WJR_ATOMIC_FAST_DEFINE(fetch_sub, (T *, T, std::memory_order), (ptr, val, order), (ptr));
WJR_ATOMIC_FAST_DEFINE(fetch_and, (T *, T, std::memory_order), (ptr, val, order), (ptr));
WJR_ATOMIC_FAST_DEFINE(fetch_xor, (T *, T, std::memory_order), (ptr, val, order), (ptr));
WJR_ATOMIC_FAST_DEFINE(fetch_or, (T *, T, std::memory_order), (ptr, val, order), (ptr));
WJR_ATOMIC_FAST_DEFINE(fetch_nand, (T *, T, std::memory_order), (ptr, val, order), (ptr));

} // namespace wjr

#undef WJR_ATOMIC_VERIFY_IMPL
#undef WJR_ATOMIC_VERIFY
#undef WJR_ATOMIC_CONCAT_I
#undef WJR_ATOMIC_CONCAT
#undef WJR_ATOMIC_FAST_DEFINE
#undef WJR_ATOMIC_DEFINE_TYPE

#endif // WJR_ATOMIC_HPP__