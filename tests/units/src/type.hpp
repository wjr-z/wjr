#ifndef WJR_UNITS_TYPE_HPP__
#define WJR_UNITS_TYPE_HPP__

namespace wjr {
struct disable_all {
    disable_all() = delete;
    disable_all(const disable_all &) = delete;
    disable_all(disable_all &&) = delete;
    disable_all &operator=(const disable_all &) = delete;
    disable_all &operator=(disable_all &&) = delete;
    ~disable_all() = delete;
};

struct disable_copy {
    disable_copy() = default;
    disable_copy(const disable_copy &) = delete;
    disable_copy(disable_copy &&) = default;
    disable_copy &operator=(const disable_copy &) = delete;
    disable_copy &operator=(disable_copy &&) = default;
    ~disable_copy() = default;
};

struct disable_move {
    disable_move() = default;
    disable_move(const disable_move &) = delete;
    disable_move(disable_move &&) = delete;
    disable_move &operator=(const disable_move &) = delete;
    disable_move &operator=(disable_move &&) = delete;
    ~disable_move() = default;
};

struct disable_copy_construct {
    disable_copy_construct() = default;
    disable_copy_construct(const disable_copy_construct &) = delete;
    disable_copy_construct(disable_copy_construct &&) = default;
    disable_copy_construct &operator=(const disable_copy_construct &) = default;
    disable_copy_construct &operator=(disable_copy_construct &&) = default;
    ~disable_copy_construct() = default;
};

struct disable_move_construct {
    disable_move_construct() = default;
    disable_move_construct(const disable_move_construct &) = delete;
    disable_move_construct(disable_move_construct &&) = delete;
    disable_move_construct &operator=(const disable_move_construct &) = default;
    disable_move_construct &operator=(disable_move_construct &&) = default;
    ~disable_move_construct() = default;
};

struct disable_copy_assign {
    disable_copy_assign() = default;
    disable_copy_assign(const disable_copy_assign &) = default;
    disable_copy_assign(disable_copy_assign &&) = default;
    disable_copy_assign &operator=(const disable_copy_assign &) = delete;
    disable_copy_assign &operator=(disable_copy_assign &&) = default;
    ~disable_copy_assign() = default;
};

struct disable_move_assign {
    disable_move_assign() = default;
    disable_move_assign(const disable_move_assign &) = default;
    disable_move_assign(disable_move_assign &&) = default;
    disable_move_assign &operator=(const disable_move_assign &) = delete;
    disable_move_assign &operator=(disable_move_assign &&) = delete;
    ~disable_move_assign() = default;
};

struct disable_default {
    disable_default() = delete;
    disable_default(const disable_default &) = default;
    disable_default(disable_default &&) = default;
    disable_default &operator=(const disable_default &) = default;
    disable_default &operator=(disable_default &&) = default;
    ~disable_default() = default;
};
} // namespace wjr

#endif // WJR_UNITS_TYPE_HPP__