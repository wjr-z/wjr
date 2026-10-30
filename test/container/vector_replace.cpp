#include <wjr/vector.hpp>

#include <array>
#include <iterator>
#include <vector>

#include <gtest/gtest.h>

namespace {

/** @brief Force the single-pass replacement path for a borrowed sequence. */
template <typename T>
struct input_iterator {
    using iterator_category = std::input_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T *;
    using reference = const T &;

    pointer ptr;
    reference operator*() const { return *ptr; }
    input_iterator &operator++() {
        ++ptr;
        return *this;
    }
    input_iterator operator++(int) {
        auto old = *this;
        ++*this;
        return old;
    }
    bool operator==(const input_iterator &) const = default;
};

/** @brief Observe the live objects when replacement erases part of a vector. */
struct tracked_value {
    static inline int live = 0;
    int value;

    explicit tracked_value(int v = 0) : value(v) { ++live; }
    tracked_value(const tracked_value &other) : value(other.value) { ++live; }
    tracked_value(tracked_value &&other) noexcept : value(other.value) { ++live; }
    tracked_value &operator=(const tracked_value &) = default;
    tracked_value &operator=(tracked_value &&) noexcept = default;
    ~tracked_value() { --live; }
};

template <bool Input>
void check_replacement_ranges() {
    const std::array replacement{10, 11, 12, 13, 14, 15, 16};
    for (std::size_t size = 0; size != 7; ++size) {
        for (std::size_t first = 0; first <= size; ++first) {
            for (std::size_t last = first; last <= size; ++last) {
                for (std::size_t count = 0; count <= replacement.size(); ++count) {
                    for (const bool reserve : {false, true}) {
                        SCOPED_TRACE(::testing::Message()
                                     << "size=" << size << " first=" << first << " last=" << last
                                     << " count=" << count << " reserve=" << reserve);
                        wjr::vector<int> actual;
                        if (reserve) {
                            actual.reserve(32);
                        }
                        std::vector<int> expected;
                        for (std::size_t i = 0; i != size; ++i) {
                            actual.push_back(static_cast<int>(i));
                            expected.push_back(static_cast<int>(i));
                        }
                        expected.erase(expected.begin() + first, expected.begin() + last);
                        expected.insert(expected.begin() + first, replacement.data(),
                                        replacement.data() + count);
                        if constexpr (Input) {
                            actual.replace(actual.cbegin() + first, actual.cbegin() + last,
                                           input_iterator<int>{replacement.data()},
                                           input_iterator<int>{replacement.data() + count});
                        } else {
                            actual.replace(actual.cbegin() + first, actual.cbegin() + last,
                                           replacement.data(), replacement.data() + count);
                        }
                        EXPECT_EQ((std::vector<int>{actual.begin(), actual.end()}), expected);
                    }
                }
            }
        }
    }
}

TEST(vector_replace, input_ranges_preserve_the_suffix) { check_replacement_ranges<true>(); }

TEST(vector_replace, forward_ranges_preserve_the_suffix) { check_replacement_ranges<false>(); }

TEST(vector_replace, shrinking_ranges_destroy_only_removed_objects) {
    EXPECT_EQ(tracked_value::live, 0);
    {
        const std::array replacement{tracked_value{20}, tracked_value{21}};
        wjr::vector<tracked_value> values;
        for (int i = 0; i != 7; ++i) {
            values.emplace_back(i);
        }
        values.replace(values.cbegin() + 1, values.cbegin() + 5,
                       input_iterator<tracked_value>{replacement.data()},
                       input_iterator<tracked_value>{replacement.data() + replacement.size()});
        ASSERT_EQ(values.size(), 5u);
        EXPECT_EQ(tracked_value::live, 7);
        EXPECT_EQ(values[1].value, 20);
        EXPECT_EQ(values[3].value, 5);
        EXPECT_EQ(values[4].value, 6);

        values.replace(values.cbegin() + 1, values.cbegin() + 3, replacement.data(),
                       replacement.data() + 1);
        ASSERT_EQ(values.size(), 4u);
        EXPECT_EQ(tracked_value::live, 6);
        EXPECT_EQ(values[1].value, 20);
        EXPECT_EQ(values[2].value, 5);
        EXPECT_EQ(values[3].value, 6);
    }
    EXPECT_EQ(tracked_value::live, 0);
}

} // namespace
