#ifndef WJR_MEMORY_INPLACE_POOL_HPP__
#define WJR_MEMORY_INPLACE_POOL_HPP__

#include <wjr/bitset.hpp>
#include <wjr/memory/uninitialized.hpp>

namespace wjr {

template <typename T, size_t Size>
class inplace_pool {
public:
    WJR_MALLOC T *allocate() {
        auto *const raw = malloc(n + aligned_header_size);
        head.push_back(static_cast<chunk *>(raw));
        return static_cast<void *>(static_cast<std::byte *>(raw) + aligned_header_size);
    }

    void deallocate(void *ptr) noexcept {
        auto *const node = static_cast<std::byte *>(ptr) - aligned_header_size;
        reinterpret_cast<chunk *>(node)->remove();
        free(node);
    }

private:
    bitset<Size> m_bs;
};

} // namespace wjr

#endif // WJR_MEMORY_INPLACE_POOL_HPP__