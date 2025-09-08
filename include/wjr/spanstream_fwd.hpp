#ifndef WJR_SPANSTREAM_FWD_HPP__
#define WJR_SPANSTREAM_FWD_HPP__

#include <string>

#include <wjr/config/attribute.hpp>

#ifdef __cpp_lib_spanstream
    #include <iosfwd>
#endif

namespace wjr {

#ifndef __cpp_lib_spanstream

template <typename _CharT, typename _Traits = std::char_traits<_CharT>>
class basic_spanbuf;
template <typename _CharT, typename _Traits = std::char_traits<_CharT>>
class basic_ispanstream;
template <typename _CharT, typename _Traits = std::char_traits<_CharT>>
class basic_ospanstream;
template <typename _CharT, typename _Traits = std::char_traits<_CharT>>
class basic_spanstream;

#else

using std::basic_ispanstream;
using std::basic_ospanstream;
using std::basic_spanbuf;
using std::basic_spanstream;

#endif

using spanbuf = basic_spanbuf<char>;
using ispanstream = basic_ispanstream<char>;
using ospanstream = basic_ospanstream<char>;
using spanstream = basic_spanstream<char>;

} // namespace wjr

#endif // WJR_SPANSTREAM_FWD_HPP__