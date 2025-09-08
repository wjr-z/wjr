#ifndef WJR_UNITS_RANDOM_HPP__
#define WJR_UNITS_RANDOM_HPP__

#include "detail.hpp"

namespace wjr {

template <typename T, typename Settings = void>
struct trandom_fn {};

template <typename T, typename Settings = void>
inline constexpr trandom_fn<T, Settings> trandom{};

template <>
struct trandom_fn<int, void> {
    int operator()() const { return mt_rand(); }
};

template <>
struct trandom_fn<std::string, void> {
    std::string operator()() const {
        std::string s;
        size_t n = mt_rand() % 64;
        for (size_t i = 0; i < n; ++i) {
            s.push_back(static_cast<char>(mt_rand()));
        }

        return s;
    }
};
} // namespace wjr

#endif // WJR_UNITS_RANDOM_HPP__