#ifndef WJR_MEMORY_INPLACE_POOL_HPP__
#define WJR_MEMORY_INPLACE_POOL_HPP__

#include <wjr/bitset.hpp>
#include <wjr/memory/uninitialized.hpp>

namespace wjr {

template <typename T, size_t Size>
class inplace_pool {
    static_assert(Size <= 64, "Maybe slow!!");

public:
    WJR_MALLOC T *allocate() {
        WJR_ASSERT_L2(m_mask.any());
        size_t pos = m_mask.countr_one();
        m_mask.set(pos);
        return __get() + pos;
    }

    void deallocate(T *ptr) noexcept {
        size_t pos = ptr - __get();
        m_mask.reset(pos);
    }

private:
    T *__get() noexcept { return reinterpret_cast<T *>(m_storage); }

    bitset<Size> m_mask;
    __aligned_storage_t<T> m_storage[Size];
};

} // namespace wjr

#endif // WJR_MEMORY_INPLACE_POOL_HPP__