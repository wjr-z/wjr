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

#include <wjr/macros/exceptions.hpp>

namespace wjr {

/// @private Called only while handling an exception from stream output.
template <typename CharT, typename Traits>
void _ostream_handle_exception(std::basic_ostream<CharT, Traits> &os) {
    WJR_TRY { os.setstate(std::ios_base::badbit); }
    WJR_CATCH(...) {
        // Preserve the original output exception when badbit is enabled.
    }
    if (os.exceptions() & std::ios_base::badbit) {
        WJR_XTHROW;
    }
}

/// @private
template <typename CharT, typename Traits>
void _ostream_write_unchecked(std::basic_ostream<CharT, Traits> &os, const CharT *str,
                              std::streamsize n) {
    std::streamsize _put;
    WJR_TRY { _put = os.rdbuf()->sputn(str, n); }
    WJR_CATCH(...) {
        _ostream_handle_exception(os);
        return;
    }
    if (_put != n) {
        os.setstate(std::ios_base::badbit);
    }
}

/// @private
template <typename CharT, typename Traits>
void _ostream_fill_unchecked(std::basic_ostream<CharT, Traits> &os, std::streamsize n) {
    WJR_TRY {
        const auto ch = os.fill();
        while (n != 0) {
            const auto _put = os.rdbuf()->sputc(ch);
            if (Traits::eq_int_type(_put, Traits::eof())) {
                break;
            }
            --n;
        }
    }
    WJR_CATCH(...) {
        _ostream_handle_exception(os);
        return;
    }
    if (n != 0) {
        os.setstate(std::ios_base::badbit);
    }
}

/// @private
template <typename CharT, typename Traits>
void _ostream_insert_unchecked(std::basic_ostream<CharT, Traits> &os, const CharT *str,
                               std::streamsize n) {
    const std::streamsize _w = os.width();
    if (_w > n) {
        const std::streamsize _pad = _w - n;
        const bool _left = ((os.flags() & std::ios_base::adjustfield) == std::ios_base::left);

        if (!_left) {
            _ostream_fill_unchecked(os, _pad);
        }
        if (os.good()) {
            _ostream_write_unchecked(os, str, n);
        }
        if (_left && os.good()) {
            _ostream_fill_unchecked(os, _pad);
        }
    } else {
        _ostream_write_unchecked(os, str, n);
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
std::basic_ostream<CharT, Traits> &_ostream_insert(std::basic_ostream<CharT, Traits> &os,
                                                   const CharT *str, std::streamsize n) {
    const typename std::basic_ostream<CharT, Traits>::sentry ok(os);
    if (ok) {
        _ostream_insert_unchecked(os, str, n);
    }

    return os;
}

} // namespace wjr

#endif // WJR_FORMAT_OSTREAM_INSERT_HPP__