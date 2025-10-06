/**
 * @file format/ostream_insert.hpp
 * @brief Optimized ostream insertion utilities
 * @author wjr
 *
 * Provides fast string insertion to output streams with proper padding support.
 */

#ifndef WJR_FORMAT_OSTREAM_INSERT_HPP__
#define WJR_FORMAT_OSTREAM_INSERT_HPP__

#include <ostream>

namespace wjr {

/// @private
template <typename CharT, typename Traits>
void __ostream_write_unchecked(std::basic_ostream<CharT, Traits> &os, const CharT *str,
                               std::streamsize n) noexcept {
    const auto __put = os.rdbuf()->sputn(str, n);
    if (__put != n) {
        os.setstate(std::ios_base::badbit);
    }
}

/// @private
template <typename CharT, typename Traits>
void __ostream_fill_unchecked(std::basic_ostream<CharT, Traits> &os, std::streamsize n) noexcept {
    const auto ch = os.fill();
    while (n--) {
        const auto __put = os.rdbuf()->sputc(ch);
        if (Traits::eq_int_type(__put, Traits::eof())) {
            os.setstate(std::ios_base::badbit);
            break;
        }
    }
}

/// @private
template <typename CharT, typename Traits>
void __ostream_insert_unchecked(std::basic_ostream<CharT, Traits> &os, const CharT *str,
                                std::streamsize n) noexcept {
    const std::streamsize __w = os.width();
    if (__w > n) {
        const std::streamsize __pad = __w - n;
        const bool __left = ((os.flags() & std::ios_base::adjustfield) == std::ios_base::left);

        if (!__left) {
            __ostream_fill_unchecked(os, __pad);
        }
        if (os.good()) {
            __ostream_write_unchecked(os, str, n);
        }
        if (__left && os.good()) {
            __ostream_fill_unchecked(os, __pad);
        }
    } else {
        __ostream_write_unchecked(os, str, n);
    }

    os.width(0);
}

/**
 * @brief Optimized string insertion to output stream
 *
 * Fast output with padding support respecting stream width and alignment flags.
 *
 * @tparam CharT Character type
 * @tparam Traits Character traits
 * @param[in,out] os Output stream
 * @param[in] str String to insert
 * @param[in] n String length
 * @return Reference to the output stream
 */
template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits> &__ostream_insert(std::basic_ostream<CharT, Traits> &os,
                                                    const CharT *str, std::streamsize n) noexcept {
    const std::ostream::sentry ok(os);
    if (ok) {
        __ostream_insert_unchecked(os, str, n);
    }

    return os;
}

} // namespace wjr

#endif // WJR_FORMAT_OSTREAM_INSERT_HPP__