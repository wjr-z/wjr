#include <wjr/biginteger.hpp>

namespace wjr {

namespace biginteger_detail {

template from_chars_result<const char *> __from_chars_impl<true, default_biginteger_storage>(
    const char *first, const char *last, basic_biginteger<default_biginteger_storage> *dst,
    unsigned int base) noexcept;

template void
__addsub_impl<false, default_biginteger_storage>(basic_biginteger<default_biginteger_storage> *dst,
                                                 const biginteger_data *lhs,
                                                 const biginteger_data *rhs) noexcept;

template void
__addsub_impl<true, default_biginteger_storage>(basic_biginteger<default_biginteger_storage> *dst,
                                                const biginteger_data *lhs,
                                                const biginteger_data *rhs) noexcept;

template void
__mul_impl<default_biginteger_storage>(basic_biginteger<default_biginteger_storage> *dst,
                                       const biginteger_data *lhs,
                                       const biginteger_data *rhs) noexcept;

template void
__sqr_impl<default_biginteger_storage>(basic_biginteger<default_biginteger_storage> *dst,
                                       const biginteger_data *src) noexcept;

template void
__addsubmul_impl<default_biginteger_storage>(basic_biginteger<default_biginteger_storage> *dst,
                                             const biginteger_data *lhs, const biginteger_data *rhs,
                                             int32_t xmask) noexcept;

} // namespace biginteger_detail

std::ostream &operator<<(std::ostream &os, const biginteger_data &src) noexcept {
    if (const std::ostream::sentry ok(os); ok) {
        unique_stack_allocator stkal;
        vector<char, weak_stack_allocator<char>> buffer(stkal);
        buffer.clear_if_reserved(256);

        const std::ios_base::fmtflags flags = os.flags();

        if ((flags & std::ios::showpos) && !src.is_negate()) {
            buffer.push_back('+');
        }

        int base = 10;

        if (const auto basefield = flags & std::ios::basefield; basefield != 0) {
            if (basefield == std::ios::oct) {
                base = 8;
                if (flags & std::ios::showbase) {
                    buffer.append('0');
                }
            } else if (basefield == std::ios::hex) {
                base = 16;
                if (flags & std::ios::showbase) {
                    buffer.append({'0', 'x'});
                }
            }
        }

        (void)to_chars_unchecked(std::back_inserter(buffer), src, base);
        // seems won't be empty
        WJR_ASSERT(!buffer.empty());
        __ostream_insert_unchecked(os, buffer.data(), buffer.size());
    } else {
        os.setstate(std::ios::badbit);
    }

    return os;
}

} // namespace wjr

namespace fmt {

void formatter<wjr::biginteger_data>::do_format(
    const wjr::biginteger_data &value,
    wjr::vector<char, wjr::weak_stack_allocator<char>> &buffer) const {
    buffer.clear_if_reserved(256);

    if (m_specs.sign() != sign::none && !value.is_negate()) {
        buffer.push_back('+');
    }

    int base = 10;
    switch (m_specs.type()) {
    case presentation_type::bin:
        base = 2;
        if (m_specs.alt())
            buffer.append({'0', m_specs.upper() ? 'B' : 'b'});
        break;
    case presentation_type::oct:
        base = 8;
        if (m_specs.alt())
            buffer.append('0');
        break;
    case presentation_type::hex:
        base = 16;
        if (m_specs.alt())
            buffer.append({'0', m_specs.upper() ? 'X' : 'x'});
        break;
    case presentation_type::dec:
        WJR_FALLTHROUGH;
    case presentation_type::none:
        break;
    default:
        WJR_UNREACHABLE();
    }

    (void)to_chars_unchecked(std::back_inserter(buffer), value, base);
}
} // namespace fmt