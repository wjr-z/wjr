#ifndef WJR_BENCHMARK_BTREE_FOCUSED_HPP
#define WJR_BENCHMARK_BTREE_FOCUSED_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <map>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>

#ifdef WJR_BENCHMARK_ABSEIL
    #include <absl/container/btree_map.h>
    #include <absl/container/btree_set.h>
#endif

namespace btree_focused {

inline constexpr uint64_t seed = 0x62b4e919;
inline constexpr size_t trace_blocks = 256;
using large_value = std::array<std::byte, 64>;

inline std::string standard_library() {
#if defined(_LIBCPP_VERSION)
    return "libc++ " + std::to_string(_LIBCPP_VERSION);
#elif defined(__GLIBCXX__)
    return "libstdc++ " + std::to_string(__GLIBCXX__);
#elif defined(_MSVC_STL_VERSION)
    return "MSVC STL " + std::to_string(_MSVC_STL_VERSION) + "." + std::to_string(_MSVC_STL_UPDATE);
#else
    return "unknown";
#endif
}

template <typename Key, typename Mapped>
struct scenario {
    using key_type = Key;
    using mapped_type = Mapped;
    static constexpr bool is_set = std::is_void_v<Mapped>;
    size_t string_length = 0;

    Key key(uint64_t id) const {
        if constexpr (std::is_same_v<Key, std::string>) {
            auto digits = std::to_string(id);
            return std::string(string_length - digits.size(), '0') + digits;
        } else {
            return id;
        }
    }
};

template <typename S, typename Alloc, size_t N>
struct wjr_backend {
    using type =
        wjr::btree_map<typename S::key_type, typename S::mapped_type, std::less<>, Alloc, N>;
};
template <typename Key, typename Alloc, size_t N>
struct wjr_backend<scenario<Key, void>, Alloc, N> {
    using type = wjr::btree_set<Key, std::less<>, Alloc, N>;
};

template <typename S, template <typename> class A>
struct std_backend {
    using type = std::map<typename S::key_type, typename S::mapped_type, std::less<>,
                          A<std::pair<const typename S::key_type, typename S::mapped_type>>>;
};
template <typename Key, template <typename> class A>
struct std_backend<scenario<Key, void>, A> {
    using type = std::set<Key, std::less<>, A<Key>>;
};

#ifdef WJR_BENCHMARK_ABSEIL
template <typename S, template <typename> class A>
struct absl_backend {
    using type = absl::btree_map<typename S::key_type, typename S::mapped_type, std::less<>,
                                 A<std::pair<const typename S::key_type, typename S::mapped_type>>>;
};
template <typename Key, template <typename> class A>
struct absl_backend<scenario<Key, void>, A> {
    using type = absl::btree_set<Key, std::less<>, A<Key>>;
};
#endif

template <typename S, typename Value>
decltype(auto) get_key(const Value &value) {
    if constexpr (S::is_set) {
        return (value);
    } else {
        return (value.first);
    }
}

template <typename S, typename Tree>
void insert(Tree &tree, const typename S::key_type &key) {
    if constexpr (S::is_set) {
        tree.emplace(key);
    } else {
        tree.emplace(key, typename S::mapped_type{});
    }
}

inline uint64_t digest(uint64_t key) { return key; }
inline uint64_t digest(std::string_view key) {
    uint64_t result = 14695981039346656037ull;
    for (unsigned char ch : key) {
        result = (result ^ ch) * 1099511628211ull;
    }
    return result;
}

enum class operation { lookup, erase, insert };
struct event {
    operation op;
    size_t key;
};

// Every backend receives the same generated keys, insertion permutation and trace.
// IDs 0 and 2*n+1 are explicit before-first / after-last misses.
template <typename S>
struct workload {
    std::vector<typename S::key_type> keys;
    std::vector<size_t> build;
    std::vector<size_t> queries;
    std::vector<event> mixed;

    workload(size_t n, S spec) {
        std::mt19937_64 rng(seed);
        for (size_t i = 0; i < 2 * n + 2; ++i) {
            keys.push_back(spec.key(i));
        }
        for (size_t i = 0; i < n; ++i) {
            build.push_back(2 * i + 1);
        }
        std::shuffle(build.begin(), build.end(), rng);
        if (n == 0) {
            return;
        }
        for (size_t i = 0; i < 1024; ++i) {
            queries.push_back(2 * (rng() % n) + 1);
            queries.push_back(i == 0 ? 0 : i == 1 ? 2 * n + 1 : 2 * (rng() % n) + 2);
        }
        std::shuffle(queries.begin(), queries.end(), rng);
        std::vector<size_t> active(n), spare(n);
        for (size_t i = 0; i < n; ++i) {
            active[i] = 2 * i + 1;
            spare[i] = 2 * i + 2;
        }
        for (size_t block = 0; block < trace_blocks; ++block) {
            for (size_t j = 0; j < 18; ++j) {
                const size_t slot = rng() % n;
                mixed.push_back({operation::lookup, j % 2 ? active[slot] : spare[slot]});
            }
            const size_t slot = rng() % n;
            mixed.push_back({operation::erase, active[slot]});
            mixed.push_back({operation::insert, spare[slot]});
            std::swap(active[slot], spare[slot]);
        }
    }
};

template <typename S, typename Tree>
void build(Tree &tree, const workload<S> &data) {
    for (auto id : data.build) {
        insert<S>(tree, data.keys[id]);
    }
}

template <typename S, typename Tree, typename Key>
uint64_t lookup(const Tree &tree, const Key &key) {
    const auto iter = tree.lower_bound(key);
    return iter == tree.end() ? 0 : digest(get_key<S>(*iter)) + 1;
}

template <typename S, typename Tree>
uint64_t scan(const Tree &tree) {
    uint64_t result = 0;
    for (const auto &value : tree) {
        result = result * 31 + digest(get_key<S>(value)) + 1;
    }
    return result;
}

template <typename S, typename Tree>
uint64_t mixed(Tree &tree, const workload<S> &data) {
    uint64_t result = 0;
    for (auto event : data.mixed) {
        const auto &key = data.keys[event.key];
        switch (event.op) {
        case operation::lookup:
            result += lookup<S>(tree, key);
            break;
        case operation::erase: {
            const auto iter = tree.lower_bound(key);
            if (iter == tree.end() || get_key<S>(*iter) != key) {
                throw std::runtime_error("invalid erase trace");
            }
            tree.erase(iter);
            break;
        }
        case operation::insert:
            insert<S>(tree, key);
            break;
        }
    }
    return result;
}

template <typename S, typename Tree>
void validate(const workload<S> &data) {
    Tree actual;
    typename std_backend<S, std::allocator>::type expected;
    build<S>(actual, data);
    build<S>(expected, data);
    for (auto id : data.queries) {
        if (lookup<S>(actual, data.keys[id]) != lookup<S>(expected, data.keys[id])) {
            throw std::runtime_error("lower_bound result differs from std");
        }
        if constexpr (std::is_same_v<typename S::key_type, std::string>) {
            if (lookup<S>(actual, std::string_view(data.keys[id])) !=
                lookup<S>(expected, std::string_view(data.keys[id]))) {
                throw std::runtime_error("transparent lower_bound differs from std");
            }
        }
    }
    if (mixed<S>(actual, data) != mixed<S>(expected, data) || actual.size() != expected.size() ||
        scan<S>(actual) != scan<S>(expected)) {
        throw std::runtime_error("mixed trace differs from std");
    }
    auto lhs = actual.begin();
    for (const auto &rhs : expected) {
        if (*lhs != rhs) {
            throw std::runtime_error("final content differs from std");
        }
        ++lhs;
    }
}

} // namespace btree_focused
#endif
