#ifndef WJR_BIGINTEGER_BIGINTEGER_HPP__
#define WJR_BIGINTEGER_BIGINTEGER_HPP__

#include <wjr/compressed_pair.hpp>
#include <wjr/math.hpp>

namespace wjr {

struct biginteger_data {
    // abs(size) is the number of elements in data
    // size < 0 means the number is negative
    ssize_t m_size = 1;
    uint64_t *m_data = m_small_data;
    union {
        size_t m_capacity = 0;
        uint64_t m_small_data[1];
    };
};

template <typename Alloc = std::allocator<uint64_t>>
class basic_biginteger {
    using data_type = biginteger_data;

public:
    using allocator_type = Alloc;
    using allocator_traits = std::allocator_traits<Alloc>;

private:
    using __alloc = typename allocator_traits::template rebind_alloc<uint64_t>;

public:
    basic_biginteger() = default;
    basic_biginteger(const basic_biginteger &other);
    basic_biginteger(basic_biginteger &&other);
    basic_biginteger &operator=(const basic_biginteger &other);
    basic_biginteger &operator=(basic_biginteger &&other);
    ~basic_biginteger();

private:
    constexpr data_type &__get_data() { return m_data.first(); }
    constexpr const data_type &__get_data() const { return m_data.first(); }
    constexpr __alloc &__get_alloc() { return m_data.second(); }
    constexpr const __alloc &__get_alloc() const { return m_data.second(); }

    constexpr bool is_small() const {
        const auto &data = __get_data();
        return data.m_data == data.m_small_data;
    }

    compressed_pair<data_type, __alloc> m_data;
};

using biginteger = basic_biginteger<>;

} // namespace wjr

#endif