#include <array>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#include <wjr/memory/uninitialized.hpp>

#if !defined(WJR_DISABLE_EXCEPTIONS)
namespace rollback_test {

struct failure {};

struct value {
    inline static int live = 0;
    int number = 0;

    value() noexcept { ++live; }
    value(const value &other) noexcept : number(other.number) { ++live; }
    ~value() { --live; }
};

struct allocator : std::allocator<value> {
    int attempts = 0;
    int constructed = 0;
    int destroyed = 0;
    int direct_calls = 0;
    int throw_at = 0;

    #if !defined(WJR_CPP_23)
    template <typename... Args>
    void construct(value *ptr, Args &&...args) noexcept {
        ++direct_calls;
        std::construct_at(ptr, std::forward<Args>(args)...);
    }

    void destroy(value *ptr) noexcept {
        ++direct_calls;
        std::destroy_at(ptr);
    }
    #else
    template <typename... Args>
    void construct(value *ptr, Args &&...args) {
        if (++attempts == throw_at) {
            throw failure{};
        }
        std::construct_at(ptr, std::forward<Args>(args)...);
        ++constructed;
    }

    void destroy(value *ptr) noexcept {
        ++destroyed;
        std::destroy_at(ptr);
    }
    #endif
};

} // namespace rollback_test

    // C++23 disallows program-defined allocator_traits specializations (P2652R2).
    #if !defined(WJR_CPP_23)
template <>
struct std::allocator_traits<rollback_test::allocator>
    : std::allocator_traits<std::allocator<rollback_test::value>> {
    using allocator_type = rollback_test::allocator;

    template <typename... Args>
    static void construct(allocator_type &alloc, value_type *ptr, Args &&...args) {
        if (++alloc.attempts == alloc.throw_at) {
            throw rollback_test::failure{};
        }
        std::construct_at(ptr, std::forward<Args>(args)...);
        ++alloc.constructed;
    }

    static void destroy(allocator_type &alloc, value_type *ptr) noexcept {
        ++alloc.destroyed;
        std::destroy_at(ptr);
    }
};
    #endif

namespace rollback_test {

static_assert(std::is_nothrow_default_constructible_v<value>);
static_assert(std::is_nothrow_copy_constructible_v<value>);
    #if !defined(WJR_CPP_23)
static_assert(noexcept(std::declval<allocator &>().construct(std::declval<value *>())));
    #endif
static_assert(!noexcept(std::allocator_traits<allocator>::construct(std::declval<allocator &>(),
                                                                    std::declval<value *>())));

enum class input_operation { dereference, increment, compare };

struct input_state {
    input_operation operation;
    int throw_at;
    int attempts = 0;

    void visit(input_operation current) {
        if (operation == current && ++attempts == throw_at) {
            throw failure{};
        }
    }
};

struct sentinel {
    const value *end = nullptr;
};

struct input_iterator {
    using value_type = value;
    using difference_type = std::ptrdiff_t;
    using pointer = const value *;
    using reference = const value &;
    using iterator_category = std::input_iterator_tag;

    const value *ptr = nullptr;
    input_state *state = nullptr;

    reference operator*() const {
        state->visit(input_operation::dereference);
        return *ptr;
    }

    input_iterator &operator++() {
        state->visit(input_operation::increment);
        ++ptr;
        return *this;
    }

    input_iterator operator++(int) {
        auto previous = *this;
        ++*this;
        return previous;
    }

    friend bool operator==(const input_iterator &first, sentinel last) {
        first.state->visit(input_operation::compare);
        return first.ptr == last.end;
    }

    friend bool operator==(const input_iterator &first, const input_iterator &last) {
        return first == sentinel{last.ptr};
    }
};

// Keep GTest's exception machinery out of TestBody on Clang/Windows ASan.
WJR_NOINLINE void check_input_failure(int algorithm, input_operation operation, int throw_at) {
    ASSERT_EQ(value::live, 0);
    std::array<value, 4> source;
    std::allocator<value> storage;
    auto *dest = storage.allocate(source.size());
    allocator alloc;
    input_state state{operation, throw_at};
    const input_iterator first{source.data(), &state};
    const sentinel last{source.data() + source.size()};
    auto construct = [&] {
        if (algorithm == 0) {
            return wjr::uninitialized_copy(first, last, dest);
        }
        if (algorithm == 1) {
            return wjr::uninitialized_copy_using_allocator(first, last, dest, alloc);
        }
        return wjr::uninitialized_copy_n_using_allocator(first, source.size(), dest, alloc);
    };
    EXPECT_THROW(construct(), failure);
    EXPECT_EQ(value::live, static_cast<int>(source.size()));
    const int completed = throw_at - (operation == input_operation::increment ? 0 : 1);
    if (algorithm != 0) {
        EXPECT_EQ(alloc.constructed, completed);
        EXPECT_EQ(alloc.destroyed, completed);
        EXPECT_EQ(alloc.direct_calls, 0);
    }
    storage.deallocate(dest, source.size());
}

void construct(int operation, value *dest, const std::array<value, 4> &source, allocator &alloc) {
    auto first = std::counted_iterator(dest, source.size());
    const auto last = std::default_sentinel;
    switch (operation) {
    case 0:
        (void)wjr::uninitialized_copy_using_allocator(
            std::counted_iterator(source.data(), source.size()), last, dest, alloc);
        break;
    case 1:
        (void)wjr::uninitialized_copy_n_using_allocator(source.data(), source.size(), dest, alloc);
        break;
    case 2:
        wjr::uninitialized_default_construct_using_allocator(first, last, alloc);
        break;
    case 3:
        wjr::uninitialized_default_construct_n_using_allocator(dest, source.size(), alloc);
        break;
    case 4:
        wjr::uninitialized_value_construct_using_allocator(first, last, alloc);
        break;
    case 5:
        wjr::uninitialized_value_construct_n_using_allocator(dest, source.size(), alloc);
        break;
    case 6:
        wjr::uninitialized_fill_using_allocator(first, last, alloc, source[0]);
        break;
    case 7:
        wjr::uninitialized_fill_n_using_allocator(dest, source.size(), alloc, source[0]);
        break;
    }
}

WJR_NOINLINE void check_traits_construction(int operation, int throw_at) {
    ASSERT_EQ(value::live, 0);
    std::array<value, 4> source;
    std::allocator<value> storage;
    auto *dest = storage.allocate(source.size());
    allocator alloc;
    alloc.throw_at = throw_at;
    if (throw_at != 0) {
        EXPECT_THROW(construct(operation, dest, source, alloc), failure);
        EXPECT_EQ(value::live, static_cast<int>(source.size()));
        EXPECT_EQ(alloc.constructed, throw_at - 1);
        EXPECT_EQ(alloc.destroyed, throw_at - 1);
    } else {
        construct(operation, dest, source, alloc);
        EXPECT_EQ(value::live, static_cast<int>(source.size() * 2));
        EXPECT_EQ(alloc.constructed, static_cast<int>(source.size()));
        EXPECT_EQ(alloc.destroyed, 0);
        for (std::size_t i = 0; i < source.size(); ++i) {
            std::allocator_traits<allocator>::destroy(alloc, dest + i);
        }
    }
    EXPECT_EQ(alloc.direct_calls, 0);
    storage.deallocate(dest, source.size());
}

} // namespace rollback_test

TEST(uninitialized_rollback, input_operations_may_throw) {
    using namespace rollback_test;
    for (int algorithm = 0; algorithm < 3; ++algorithm) {
        SCOPED_TRACE(algorithm);
        for (auto operation :
             {input_operation::dereference, input_operation::increment, input_operation::compare}) {
            SCOPED_TRACE(static_cast<int>(operation));
            if (algorithm == 2 && operation == input_operation::compare) {
                continue;
            }
            const int attempts = operation == input_operation::compare ? 5 : 4;
            for (int position = 1; position <= attempts; ++position) {
                SCOPED_TRACE(position);
                check_input_failure(algorithm, operation, position);
            }
        }
    }
    EXPECT_EQ(value::live, 0);
}

TEST(uninitialized_rollback, allocator_traits_customization_is_preserved) {
    for (int operation = 0; operation < 8; ++operation) {
        SCOPED_TRACE(operation);
        for (int position = 0; position <= 4; ++position) {
            SCOPED_TRACE(position);
            rollback_test::check_traits_construction(operation, position);
        }
    }
    EXPECT_EQ(rollback_test::value::live, 0);
}
#endif // WJR_DISABLE_EXCEPTIONS
