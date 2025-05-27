/**
 * @file inplace_parser.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-05-25
 * 
 * @todo 
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef WJR_JSON_INPLACE_PARSER_HPP__
#define WJR_JSON_INPLACE_PARSER_HPP__

#include <wjr/json/visitor.hpp>

namespace wjr::json {

struct inplace_parser_base {};

template <typename T>
struct inplace_parser_impl : inplace_parser_base {};

template <typename T>
class inplace_parser {
public:
    WJR_INTRINSIC_INLINE static result<void> parse(T *root, ondemand_reader &r) noexcept {
        return visitor_detail::parse(*this, rd);
    }

protected:
    WJR_INTRINSIC_INLINE result<void> visit_root_null(const char *first) const noexcept {
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_null(const char *first) const noexcept {
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_null(const char *first) const noexcept {
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_true(const char *first) const noexcept {
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_true(const char *first) const noexcept {
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_true(const char *first) const noexcept {
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_false(const char *first) const noexcept {
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_false(const char *first) const noexcept {
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_false(const char *first) const noexcept {
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_number(const char *first,
                                                        const char *last) const noexcept {
        return check_number(first, last);
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_number(const char *first,
                                                          const char *last) const noexcept {
        return check_number(first, last);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_number(const char *first,
                                                         const char *last) const noexcept {
        return check_number(first, last);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_string(const char *first,
                                                        const char *last) const noexcept {
        return check_string(first, last);
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_string(const char *first,
                                                          const char *last) const noexcept {
        return check_string(first, last);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_string(const char *first,
                                                         const char *last) const noexcept {
        return check_string(first, last);
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_key_string(const char *first,
                                                              const char *last) const noexcept {
        return check_string(first, last);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_start_object(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_start_object(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_start_object(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_start_array(uint32_t) const noexcept { return {}; }

    WJR_INTRINSIC_INLINE result<void> visit_object_start_array(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_start_array(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_object_to_object(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_object_to_array(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_object_to_root(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_object(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_array(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_root(uint32_t) const noexcept {
        return {};
    }

private:
    struct Stack {
        inplace_parser_base *base;
        unique_stack_allocator al;
    };

    inplace_vector<Stack, 256> m_stack;
};

} // namespace wjr::json

#endif // WJR_JSON_INPLACE_PARSER_HPP__