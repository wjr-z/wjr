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
#include <limits>
#include <ostream>
#include <streambuf>

#include <wjr/math/add.hpp>
#include <wjr/span.hpp>
#include <wjr/spanstream_fwd.hpp>

namespace wjr {
#ifndef __cpp_lib_spanstream
template <typename _CharT, typename _Traits>
class basic_spanbuf : public std::basic_streambuf<_CharT, _Traits> {
    using _streambuf_type = std::basic_streambuf<_CharT, _Traits>;

public:
    using char_type = _CharT;
    using int_type = typename _Traits::int_type;
    using pos_type = typename _Traits::pos_type;
    using off_type = typename _Traits::off_type;
    using traits_type = _Traits;

    // [spanbuf.ctor], constructors
    basic_spanbuf() : basic_spanbuf(std::ios_base::in | std::ios_base::out) {}

    explicit basic_spanbuf(std::ios_base::openmode _which) : _streambuf_type(), _M_mode(_which) {}

    explicit basic_spanbuf(wjr::span<_CharT> _s,
                           std::ios_base::openmode _which = std::ios_base::in | std::ios_base::out)
        : _streambuf_type(), _M_mode(_which) {
        span(_s);
    }

    basic_spanbuf(const basic_spanbuf &) = delete;

    /** Move constructor.
     *
     * Transfers the buffer and pointers into the get and put areas from
     * `_rhs` to `*this`.
     *
     * In this implementation `rhs` is left unchanged,
     * but that is not guaranteed by the standard.
     */
    basic_spanbuf(basic_spanbuf &&_rhs) noexcept
        : _streambuf_type(_rhs), _M_mode(_rhs._M_mode), _M_buf(_rhs._M_buf) {}

    // [spanbuf.assign], assignment and swap
    basic_spanbuf &operator=(const basic_spanbuf &) = delete;

    basic_spanbuf &operator=(basic_spanbuf &&_rhs) noexcept {
        basic_spanbuf(std::move(_rhs)).swap(*this);
        return *this;
    }

    void swap(basic_spanbuf &_rhs) {
        _streambuf_type::swap(_rhs);
        std::swap(_M_mode, _rhs._M_mode);
        std::swap(_M_buf, _rhs._M_buf);
    }

    // [spanbuf.members], member functions
    WJR_NODISCARD
    wjr::span<_CharT> span() const noexcept {
        if (_M_mode & std::ios_base::out)
            return {this->pbase(), this->pptr()};
        else
            return _M_buf;
    }

    void span(wjr::span<_CharT> _s) noexcept {
        _M_buf = _s;
        if (_M_mode & std::ios_base::out) {
            this->setp(_s.data(), _s.data() + _s.size());
            if (_M_mode & std::ios_base::ate)
                _bump_put(_s.size());
        }
        if (_M_mode & std::ios_base::in)
            this->setg(_s.data(), _s.data(), _s.data() + _s.size());
    }

protected:
    // [spanbuf.virtuals], overridden virtual functions
    std::basic_streambuf<_CharT, _Traits> *setbuf(_CharT *_s, std::streamsize _n) override {
        WJR_ASSERT(_n >= 0);
        this->span(wjr::span<_CharT>(_s, _n));
        return this;
    }

    pos_type seekoff(off_type offset, std::ios_base::seekdir way,
                     std::ios_base::openmode which = std::ios_base::in |
                                                     std::ios_base::out) override {
        const pos_type failure(off_type(-1));
        const bool input = (which & std::ios_base::in) != 0;
        const bool output = (which & std::ios_base::out) != 0;

        off_type base = 0;
        switch (way) {
        case std::ios_base::beg:
            break;
        case std::ios_base::cur:
            if (input == output)
                return failure;
            base = input ? this->gptr() - this->eback() : this->pptr() - this->pbase();
            break;
        case std::ios_base::end:
            // The buffer's open mode, rather than the requested sequence, determines the end.
            base = (_M_mode & (std::ios_base::in | std::ios_base::out)) == std::ios_base::out
                       ? this->pptr() - this->pbase()
                       : static_cast<off_type>(_M_buf.size());
            break;
        default:
            return failure;
        }

        off_type position;
        if (WJR_UNLIKELY(add_overflow(base, offset, position) || position < 0 ||
                         to_unsigned(position) > _M_buf.size())) {
            return failure;
        }

        if (position != 0 &&
            ((input && this->gptr() == nullptr) || (output && this->pptr() == nullptr))) {
            return failure;
        }

        if (input)
            this->setg(this->eback(), this->eback() + position, this->egptr());

        if (output) {
            this->setp(this->pbase(), this->epptr());
            _bump_put(static_cast<size_t>(position));
        }
        return pos_type(position);
    }

    pos_type seekpos(pos_type _sp, std::ios_base::openmode _which = std::ios_base::in |
                                                                    std::ios_base::out) override {
        return seekoff(off_type(_sp), std::ios_base::beg, _which);
    }

private:
    /// @brief Advance the output cursor without narrowing a span-sized offset.
    void _bump_put(size_t count) noexcept {
        constexpr auto max_step = static_cast<size_t>((std::numeric_limits<int>::max)());
        while (WJR_UNLIKELY(count > max_step)) {
            this->pbump(static_cast<int>(max_step));
            count -= max_step;
        }
        this->pbump(static_cast<int>(count));
    }

    std::ios_base::openmode _M_mode;
    wjr::span<_CharT> _M_buf;
};

using spanbuf = basic_spanbuf<char>;
using wspanbuf = basic_spanbuf<wchar_t>;

template <typename _CharT, typename _Traits>
class basic_ispanstream : public std::basic_istream<_CharT, _Traits> {
    using _istream_type = std::basic_istream<_CharT, _Traits>;

public:
    using char_type = _CharT;
    using int_type = typename _Traits::int_type;
    using pos_type = typename _Traits::pos_type;
    using off_type = typename _Traits::off_type;
    using traits_type = _Traits;

    // [ispanstream.ctor], constructors
    explicit basic_ispanstream(wjr::span<_CharT> _s,
                               std::ios_base::openmode _which = std::ios_base::in)
        : _istream_type(std::addressof(_M_sb)), _M_sb(_s, _which | std::ios_base::in) {}

    basic_ispanstream(const basic_ispanstream &) = delete;

    basic_ispanstream(basic_ispanstream &&_rhs) noexcept
        : _istream_type(std::move(_rhs)), _M_sb(std::move(_rhs._M_sb)) {
        _istream_type::set_rdbuf(std::addressof(_M_sb));
    }

    // [ispanstream.assign], assignment and swap
    basic_ispanstream &operator=(const basic_ispanstream &) = delete;
    basic_ispanstream &operator=(basic_ispanstream &&_rhs) = default;

    void swap(basic_ispanstream &_rhs) {
        _istream_type::swap(_rhs);
        _M_sb.swap(_rhs._M_sb);
    }

    // [ispanstream.members], member functions
    WJR_NODISCARD
    basic_spanbuf<_CharT, _Traits> *rdbuf() const noexcept {
        return const_cast<basic_spanbuf<_CharT, _Traits> *>(std::addressof(_M_sb));
    }

    WJR_NODISCARD
    wjr::span<const _CharT> span() const noexcept { return _M_sb.span(); }

    void span(wjr::span<_CharT> _s) noexcept { return _M_sb.span(_s); }

private:
    basic_spanbuf<_CharT, _Traits> _M_sb;
};

using ispanstream = basic_ispanstream<char>;
using wispanstream = basic_ispanstream<wchar_t>;

template <typename _CharT, typename _Traits>
class basic_ospanstream : public std::basic_ostream<_CharT, _Traits> {
    using _ostream_type = std::basic_ostream<_CharT, _Traits>;

public:
    using char_type = _CharT;
    using int_type = typename _Traits::int_type;
    using pos_type = typename _Traits::pos_type;
    using off_type = typename _Traits::off_type;
    using traits_type = _Traits;

    // [ospanstream.ctor], constructors
    explicit basic_ospanstream(wjr::span<_CharT> _s,
                               std::ios_base::openmode _which = std::ios_base::out)
        : _ostream_type(std::addressof(_M_sb)), _M_sb(_s, _which | std::ios_base::in) {}

    basic_ospanstream(const basic_ospanstream &) = delete;

    basic_ospanstream(basic_ospanstream &&_rhs) noexcept
        : _ostream_type(std::move(_rhs)), _M_sb(std::move(_rhs._M_sb)) {
        _ostream_type::set_rdbuf(std::addressof(_M_sb));
    }

    // [ospanstream.assign], assignment and swap
    basic_ospanstream &operator=(const basic_ospanstream &) = delete;
    basic_ospanstream &operator=(basic_ospanstream &&_rhs) = default;

    void swap(basic_ospanstream &_rhs) {
        _ostream_type::swap(_rhs);
        _M_sb.swap(_rhs._M_sb);
    }

    // [ospanstream.members], member functions
    WJR_NODISCARD
    basic_spanbuf<_CharT, _Traits> *rdbuf() const noexcept {
        return const_cast<basic_spanbuf<_CharT, _Traits> *>(std::addressof(_M_sb));
    }

    WJR_NODISCARD
    wjr::span<_CharT> span() const noexcept { return _M_sb.span(); }

    void span(wjr::span<_CharT> _s) noexcept { return _M_sb.span(_s); }

private:
    basic_spanbuf<_CharT, _Traits> _M_sb;
};

using ospanstream = basic_ospanstream<char>;
using wospanstream = basic_ospanstream<wchar_t>;

template <typename _CharT, typename _Traits>
class basic_spanstream : public std::basic_iostream<_CharT, _Traits> {
    using _iostream_type = std::basic_iostream<_CharT, _Traits>;

public:
    using char_type = _CharT;
    using int_type = typename _Traits::int_type;
    using pos_type = typename _Traits::pos_type;
    using off_type = typename _Traits::off_type;
    using traits_type = _Traits;

    // [spanstream.ctor], constructors
    explicit basic_spanstream(wjr::span<_CharT> _s,
                              std::ios_base::openmode _which = std::ios_base::out |
                                                               std::ios_base::in)
        : _iostream_type(std::addressof(_M_sb)), _M_sb(_s, _which) {}

    basic_spanstream(const basic_spanstream &) = delete;

    basic_spanstream(basic_spanstream &&_rhs) noexcept
        : _iostream_type(std::move(_rhs)), _M_sb(std::move(_rhs._M_sb)) {
        _iostream_type::set_rdbuf(std::addressof(_M_sb));
    }

    // [spanstream.assign], assignment and swap
    basic_spanstream &operator=(const basic_spanstream &) = delete;
    basic_spanstream &operator=(basic_spanstream &&_rhs) = default;

    void swap(basic_spanstream &_rhs) {
        _iostream_type::swap(_rhs);
        _M_sb.swap(_rhs._M_sb);
    }

    // [spanstream.members], members
    WJR_NODISCARD
    basic_spanbuf<_CharT, _Traits> *rdbuf() const noexcept {
        return const_cast<basic_spanbuf<_CharT, _Traits> *>(std::addressof(_M_sb));
    }

    WJR_NODISCARD
    wjr::span<_CharT> span() const noexcept { return _M_sb.span(); }

    void span(wjr::span<_CharT> _s) noexcept { return _M_sb.span(_s); }

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
inline void swap(wjr::basic_spanbuf<_CharT, _Traits> &_x, wjr::basic_spanbuf<_CharT, _Traits> &_y) {
    _x.swap(_y);
}

template <typename _CharT, typename _Traits>
inline void swap(wjr::basic_ispanstream<_CharT, _Traits> &_x,
                 wjr::basic_ispanstream<_CharT, _Traits> &_y) {
    _x.swap(_y);
}

template <typename _CharT, typename _Traits>
inline void swap(wjr::basic_ospanstream<_CharT, _Traits> &_x,
                 wjr::basic_ospanstream<_CharT, _Traits> &_y) {
    _x.swap(_y);
}

template <typename _CharT, typename _Traits>
inline void swap(wjr::basic_spanstream<_CharT, _Traits> &_x,
                 wjr::basic_spanstream<_CharT, _Traits> &_y) {
    _x.swap(_y);
}
} // namespace std

#endif // __cpp_lib_spanstream

#endif // WJR_SPANSTREAM_HPP__
