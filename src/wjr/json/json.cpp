#include <wjr/json/json.hpp>

namespace wjr::json::detail {

class check_parser {
    template <typename Parser>
    friend result<void> visitor_detail::parse(Parser &&par, const reader &rd) noexcept;

public:
    WJR_INTRINSIC_INLINE static result<void> parse(const reader &rd) noexcept {
        return visitor_detail::parse(check_parser(), rd);
    }

protected:
    WJR_INTRINSIC_INLINE result<void> visit_root_null(const char *first) const noexcept {
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_null(const char *first) const noexcept {
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_null(const char *first) const noexcept {
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_true(const char *first) const noexcept {
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_true(const char *first) const noexcept {
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_true(const char *first) const noexcept {
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_false(const char *first) const noexcept {
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_false(const char *first) const noexcept {
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_false(const char *first) const noexcept {
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_number(const char *first,
                                                        const char *last) const noexcept {
        return check_number(first, last);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_number(const char *first, const char *last) const noexcept {
        return check_number(first, last);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_number(const char *first, const char *last) const noexcept {
        return check_number(first, last);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_string(const char *first,
                                                        const char *last) const noexcept {
        return check_string(first, last);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_string(const char *first, const char *last) const noexcept {
        return check_string(first, last);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_string(const char *first, const char *last) const noexcept {
        return check_string(first, last);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_key_string(const char *first, const char *last) const noexcept {
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

    WJR_INTRINSIC_INLINE result<void> visit_root_start_array(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_start_array(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_start_array(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_end_object_to_object(uint32_t) const noexcept {
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
};

result<void> __check_impl(const reader &rd) noexcept { return check_parser::parse(rd); }

} // namespace wjr::json::detail