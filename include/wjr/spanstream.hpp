// Streams based on std::span -*- C++ -*-

// Copyright The GNU Toolchain Authors.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

#ifndef WJR_SPANSTREAM_HPP__
#define WJR_SPANSTREAM_HPP__

#include <istream>
#include <ostream>
#include <streambuf>

#include <wjr/math/add.hpp>
#include <wjr/span.hpp>
#include <wjr/spanstream_fwd.hpp>

namespace wjr {
#ifndef __cpp_lib_spanstream
template <typename _CharT, typename _Traits>
class basic_spanbuf : public std::basic_streambuf<_CharT, _Traits> {
    using __streambuf_type = std::basic_streambuf<_CharT, _Traits>;

public:
    using char_type = _CharT;
    using int_type = typename _Traits::int_type;
    using pos_type = typename _Traits::pos_type;
    using off_type = typename _Traits::off_type;
    using traits_type = _Traits;

    // [spanbuf.ctor], constructors
    basic_spanbuf() : basic_spanbuf(std::ios_base::in | std::ios_base::out) {}

    explicit basic_spanbuf(std::ios_base::openmode __which)
        : __streambuf_type(), _M_mode(__which) {}

    explicit basic_spanbuf(wjr::span<_CharT> __s,
                           std::ios_base::openmode __which = std::ios_base::in | std::ios_base::out)
        : __streambuf_type(), _M_mode(__which) {
        span(__s);
    }

    basic_spanbuf(const basic_spanbuf &) = delete;

    /** Move constructor.
     *
     * Transfers the buffer and pointers into the get and put areas from
     * `__rhs` to `*this`.
     *
     * In this implementation `rhs` is left unchanged,
     * but that is not guaranteed by the standard.
     */
    basic_spanbuf(basic_spanbuf &&__rhs)
        : __streambuf_type(__rhs), _M_mode(__rhs._M_mode), _M_buf(__rhs._M_buf) {}

    // [spanbuf.assign], assignment and swap
    basic_spanbuf &operator=(const basic_spanbuf &) = delete;

    basic_spanbuf &operator=(basic_spanbuf &&__rhs) {
        basic_spanbuf(std::move(__rhs)).swap(*this);
        return *this;
    }

    void swap(basic_spanbuf &__rhs) {
        __streambuf_type::swap(__rhs);
        std::swap(_M_mode, __rhs._M_mode);
        std::swap(_M_buf, __rhs._M_buf);
    }

    // [spanbuf.members], member functions
    WJR_NODISCARD
    wjr::span<_CharT> span() const noexcept {
        if (_M_mode & std::ios_base::out)
            return {this->pbase(), this->pptr()};
        else
            return _M_buf;
    }

    void span(wjr::span<_CharT> __s) noexcept {
        _M_buf = __s;
        if (_M_mode & std::ios_base::out) {
            this->setp(__s.data(), __s.data() + __s.size());
            if (_M_mode & std::ios_base::ate)
                this->pbump(__s.size());
        }
        if (_M_mode & std::ios_base::in)
            this->setg(__s.data(), __s.data(), __s.data() + __s.size());
    }

protected:
    // [spanbuf.virtuals], overridden virtual functions
    std::basic_streambuf<_CharT, _Traits> *setbuf(_CharT *__s, std::streamsize __n) override {
        WJR_ASSERT(__n >= 0);
        this->span(wjr::span<_CharT>(__s, __n));
        return this;
    }

    pos_type seekoff(off_type __off, std::ios_base::seekdir __way,
                     std::ios_base::openmode __which = std::ios_base::in |
                                                       std::ios_base::out) override {
        pos_type __ret = pos_type(off_type(-1));

        if (__way == std::ios_base::beg) {
            if (0 <= __off && to_unsigned(__off) <= _M_buf.size()) {
                if (__which & std::ios_base::in)
                    this->setg(this->eback(), this->eback() + __off, this->egptr());

                if (__which & std::ios_base::out) {
                    this->setp(this->pbase(), this->epptr());
                    this->pbump(__off);
                }

                __ret = pos_type(__off);
            }
        } else {
            off_type __base{};
            __which &= (std::ios_base::in | std::ios_base::out);

            if (__which == std::ios_base::out)
                __base = this->pptr() - this->pbase();
            else if (__way == std::ios_base::cur) {
                if (__which == std::ios_base::in)
                    __base = this->gptr() - this->eback();
                else
                    return __ret;
            } else if (__way == std::ios_base::end)
                __base = _M_buf.size();
            else /* way is not ios::beg, ios::cur, or ios::end */ /* [[unlikely]] */
                return __ret;

            WJR_ASSERT(__base >= 0);

            std::make_unsigned_t<off_type> __uoff;
            if (WJR_UNLIKELY(add_overflow(__base, __off, __uoff)))
                return __ret;

            __off = static_cast<off_type>(__uoff);

            if (WJR_UNLIKELY(__off < 0 || to_unsigned(__off) > _M_buf.size()))
                return __ret;

            if (__which & std::ios_base::in)
                this->setg(this->eback(), this->eback() + __off, this->egptr());

            if (__which & std::ios_base::out) {
                this->setp(this->pbase(), this->epptr());
                this->pbump(__off);
            }

            __ret = pos_type(__off);
        }
        return __ret;
    }

    pos_type seekpos(pos_type __sp, std::ios_base::openmode __which = std::ios_base::in |
                                                                      std::ios_base::out) override {
        return seekoff(off_type(__sp), std::ios_base::beg, __which);
    }

private:
    std::ios_base::openmode _M_mode;
    wjr::span<_CharT> _M_buf;
};

using spanbuf = basic_spanbuf<char>;
using wspanbuf = basic_spanbuf<wchar_t>;

template <typename _CharT, typename _Traits>
class basic_ispanstream : public std::basic_istream<_CharT, _Traits> {
    using __istream_type = std::basic_istream<_CharT, _Traits>;

public:
    using char_type = _CharT;
    using int_type = typename _Traits::int_type;
    using pos_type = typename _Traits::pos_type;
    using off_type = typename _Traits::off_type;
    using traits_type = _Traits;

    // [ispanstream.ctor], constructors
    explicit basic_ispanstream(wjr::span<_CharT> __s,
                               std::ios_base::openmode __which = std::ios_base::in)
        : __istream_type(std::addressof(_M_sb)), _M_sb(__s, __which | std::ios_base::in) {}

    basic_ispanstream(const basic_ispanstream &) = delete;

    basic_ispanstream(basic_ispanstream &&__rhs)
        : __istream_type(std::move(__rhs)), _M_sb(std::move(__rhs._M_sb)) {
        __istream_type::set_rdbuf(std::addressof(_M_sb));
    }

    // [ispanstream.assign], assignment and swap
    basic_ispanstream &operator=(const basic_ispanstream &) = delete;
    basic_ispanstream &operator=(basic_ispanstream &&__rhs) = default;

    void swap(basic_ispanstream &__rhs) {
        __istream_type::swap(__rhs);
        _M_sb.swap(__rhs._M_sb);
    }

    // [ispanstream.members], member functions
    WJR_NODISCARD
    basic_spanbuf<_CharT, _Traits> *rdbuf() const noexcept {
        return const_cast<basic_spanbuf<_CharT, _Traits> *>(std::addressof(_M_sb));
    }

    WJR_NODISCARD
    wjr::span<const _CharT> span() const noexcept { return _M_sb.span(); }

    void span(wjr::span<_CharT> __s) noexcept { return _M_sb.span(__s); }

private:
    basic_spanbuf<_CharT, _Traits> _M_sb;
};

using ispanstream = basic_ispanstream<char>;
using wispanstream = basic_ispanstream<wchar_t>;

template <typename _CharT, typename _Traits>
class basic_ospanstream : public std::basic_ostream<_CharT, _Traits> {
    using __ostream_type = std::basic_ostream<_CharT, _Traits>;

public:
    using char_type = _CharT;
    using int_type = typename _Traits::int_type;
    using pos_type = typename _Traits::pos_type;
    using off_type = typename _Traits::off_type;
    using traits_type = _Traits;

    // [ospanstream.ctor], constructors
    explicit basic_ospanstream(wjr::span<_CharT> __s,
                               std::ios_base::openmode __which = std::ios_base::out)
        : __ostream_type(std::addressof(_M_sb)), _M_sb(__s, __which | std::ios_base::in) {}

    basic_ospanstream(const basic_ospanstream &) = delete;

    basic_ospanstream(basic_ospanstream &&__rhs)
        : __ostream_type(std::move(__rhs)), _M_sb(std::move(__rhs._M_sb)) {
        __ostream_type::set_rdbuf(std::addressof(_M_sb));
    }

    // [ospanstream.assign], assignment and swap
    basic_ospanstream &operator=(const basic_ospanstream &) = delete;
    basic_ospanstream &operator=(basic_ospanstream &&__rhs) = default;

    void swap(basic_ospanstream &__rhs) {
        __ostream_type::swap(__rhs);
        _M_sb.swap(__rhs._M_sb);
    }

    // [ospanstream.members], member functions
    WJR_NODISCARD
    basic_spanbuf<_CharT, _Traits> *rdbuf() const noexcept {
        return const_cast<basic_spanbuf<_CharT, _Traits> *>(std::addressof(_M_sb));
    }

    WJR_NODISCARD
    wjr::span<_CharT> span() const noexcept { return _M_sb.span(); }

    void span(wjr::span<_CharT> __s) noexcept { return _M_sb.span(__s); }

private:
    basic_spanbuf<_CharT, _Traits> _M_sb;
};

using ospanstream = basic_ospanstream<char>;
using wospanstream = basic_ospanstream<wchar_t>;

template <typename _CharT, typename _Traits>
class basic_spanstream : public std::basic_iostream<_CharT, _Traits> {
    using __iostream_type = std::basic_iostream<_CharT, _Traits>;

public:
    using char_type = _CharT;
    using int_type = typename _Traits::int_type;
    using pos_type = typename _Traits::pos_type;
    using off_type = typename _Traits::off_type;
    using traits_type = _Traits;

    // [spanstream.ctor], constructors
    explicit basic_spanstream(wjr::span<_CharT> __s,
                              std::ios_base::openmode __which = std::ios_base::out |
                                                                std::ios_base::in)
        : __iostream_type(std::addressof(_M_sb)), _M_sb(__s, __which) {}

    basic_spanstream(const basic_spanstream &) = delete;

    basic_spanstream(basic_spanstream &&__rhs)
        : __iostream_type(std::move(__rhs)), _M_sb(std::move(__rhs._M_sb)) {
        __iostream_type::set_rdbuf(std::addressof(_M_sb));
    }

    // [spanstream.assign], assignment and swap
    basic_spanstream &operator=(const basic_spanstream &) = delete;
    basic_spanstream &operator=(basic_spanstream &&__rhs) = default;

    void swap(basic_spanstream &__rhs) {
        __iostream_type::swap(__rhs);
        _M_sb.swap(__rhs._M_sb);
    }

    // [spanstream.members], members
    WJR_NODISCARD
    basic_spanbuf<_CharT, _Traits> *rdbuf() const noexcept {
        return const_cast<basic_spanbuf<_CharT, _Traits> *>(std::addressof(_M_sb));
    }

    WJR_NODISCARD
    wjr::span<_CharT> span() const noexcept { return _M_sb.span(); }

    void span(wjr::span<_CharT> __s) noexcept { return _M_sb.span(__s); }

private:
    basic_spanbuf<_CharT, _Traits> _M_sb;
};

using spanstream = basic_spanstream<char>;
using wspanstream = basic_spanstream<wchar_t>;

#endif // __cpp_lib_spanstream
} // namespace wjr

#ifndef __cpp_lib_spanstream

namespace std {
template <typename _CharT, typename _Traits>
inline void swap(wjr::basic_spanbuf<_CharT, _Traits> &__x,
                 wjr::basic_spanbuf<_CharT, _Traits> &__y) {
    __x.swap(__y);
}

template <typename _CharT, typename _Traits>
inline void swap(wjr::basic_ispanstream<_CharT, _Traits> &__x,
                 wjr::basic_ispanstream<_CharT, _Traits> &__y) {
    __x.swap(__y);
}

template <typename _CharT, typename _Traits>
inline void swap(wjr::basic_ospanstream<_CharT, _Traits> &__x,
                 wjr::basic_ospanstream<_CharT, _Traits> &__y) {
    __x.swap(__y);
}

template <typename _CharT, typename _Traits>
inline void swap(wjr::basic_spanstream<_CharT, _Traits> &__x,
                 wjr::basic_spanstream<_CharT, _Traits> &__y) {
    __x.swap(__y);
}
} // namespace std

#endif // __cpp_lib_spanstream

#endif // WJR_SPANSTREAM_HPP__
