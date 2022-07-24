#ifndef __WJR_MTOOL_H
#define __WJR_MTOOL_H

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <sstream>
#include <type_traits>
#include <utility>
#include "mString.h"

namespace wjr {

    template<typename Duration, typename Func, typename...Args>
    inline decltype(auto) test_runtime(std::in_place_type_t<Duration>, Func&& fn, Args&&...args) {
        static_assert(std::is_void_v<std::invoke_result_t<Func, Args...>>, "test_runtime must return void");
        using namespace std::chrono;
        auto start = high_resolution_clock::now();
        std::forward<Func>(fn)(std::forward<Args>(args)...);
        auto end = high_resolution_clock::now();
        return duration_cast<Duration>(end - start);
    }

    template<typename Func, typename...Args>
    inline decltype(auto) test_runtime(Func&& fn, Args&&...args) {
        return test_runtime(std::in_place_type_t<std::chrono::milliseconds>{}, std::forward<Func>(fn), std::forward<Args>(args)...);
    }

#if !defined(WJR_CPP_20)
#define _IF_PERIOD_RETURN_SUFFIX_ELSE(_TYPE, _SUFFIX) \
    if constexpr (std::is_same_v<_Period, _TYPE>) {   \
        if constexpr (std::is_same_v<_CharT, char>) { \
            return _SUFFIX;                           \
        } else {                                      \
            return L##_SUFFIX;                        \
        }                                             \
    } else

    template <class _CharT, class _Period>
    [[nodiscard]] constexpr const _CharT* _Get_literal_unit_suffix() {
        using namespace std;
        using namespace std::chrono;
        _IF_PERIOD_RETURN_SUFFIX_ELSE(atto, "as")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(femto, "fs")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(pico, "ps")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(nano, "ns")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(micro, "us")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(milli, "ms")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(centi, "cs")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(deci, "ds")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(chrono::seconds::period, "s")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(deca, "das")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(hecto, "hs")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(kilo, "ks")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(mega, "Ms")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(giga, "Gs")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(tera, "Ts")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(peta, "Ps")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(exa, "Es")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(chrono::minutes::period, "min")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(chrono::hours::period, "h")
            _IF_PERIOD_RETURN_SUFFIX_ELSE(ratio<86400>, "d")

        {
            return nullptr;
        }
    }

#undef _IF_PERIOD_RETURN_SUFFIX_ELSE

    template <class _Period, class _CharT, class _Traits>
    void _Write_unit_suffix(std::basic_ostream<_CharT, _Traits>& _Os) {
        constexpr auto _Suffix = _Get_literal_unit_suffix<_CharT, _Period>();
        static_assert(_Suffix != nullptr);
        _Os << _Suffix;
    }

    template <class _CharT, class _Traits, class _Rep, class _Period>
    std::basic_ostream<_CharT, _Traits>& operator<<(
        std::basic_ostream<_CharT, _Traits>& _Os, const std::chrono::duration<_Rep, _Period>& _Dur) {
        std::basic_ostringstream<_CharT, _Traits> _Sstr;
        _Sstr.flags(_Os.flags());
        _Sstr.imbue(_Os.getloc());
        _Sstr.precision(_Os.precision());
        _Sstr << _Dur.count();
        _Write_unit_suffix<_Period>(_Sstr);

        return _Os << _Sstr.str();
    }
#endif

    std::vector<String> get_all_files(String_view path);
    size_t get_file_length(String_view file_name);
    String read_file(String_view file_name);
    void write_file(String_view file_name, String_view str);
    int create_file(String_view file_name
#ifdef __linux__
        , __mode_t __mode = 0755
#endif
    );
}
#endif
