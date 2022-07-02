#ifndef __WJR_JSON_TOOL_H
#define __WJR_JSON_TOOL_H

#include "../generic/mtype_traits.h"

namespace wjr {

	struct json_func {
		
		constexpr static std::array<size_t, 256> string_skip_step = {
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
		};

		template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
		constexpr static iter skip_string(iter first, [[maybe_unused]] iter last) {
			using value_t = typename std::iterator_traits<iter>::value_type;
			using uvalue_t = std::make_unsigned_t<value_t>;
			if constexpr (std::numeric_limits<uvalue_t>::max() < 256) {
				auto u_value = get_unsigned_value(*first);
				while (string_skip_step[u_value]) {
					first += string_skip_step[u_value];
					u_value = get_unsigned_value(*first);
				}
			}
			else {
				auto u_value = get_unsigned_value(*first);
				while (u_value >= 256 || string_skip_step[u_value]) {
					first += u_value >= 256 ? 1 : string_skip_step[*first];
					u_value = get_unsigned_value(*first);
				}
			}
			return first;
		}

		template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
		constexpr static bool check_string(iter& s, iter e) {
			if (*s != '"')return false;
			++s;
			while (s != e && *s != '"') {
				if (*s == '\\') {
					++s;
					if (s == e)return false;
					switch (*s) {
					case '"':
					case '\\':
					case '/':
					case 'b':
					case 'f':
					case 'n':
					case 'r':
					case 't':
						break;
					case 'u':
						if (s + 4 > e)return false;
						if (!qis_hex_digit(*s))
							return false;
						++s;
						if (!qis_hex_digit(*s))
							return false;
						++s;
						if (!qis_hex_digit(*s))
							return false;
						++s;
						if (!qis_hex_digit(*s))
							return false;
						++s;
						break;
					default:
						return false;
					}
				}
				else {
					if constexpr (std::is_unsigned_v<iter>) {
						if (*s < 32 || *s == 127)
							return false;
					}
					else {
						if ((*s >= 0 && *s < 32) || *s == 127)
							return false;
					}
				}
				++s;
			}
			return s != e;
		}

		template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
		constexpr static iter skip_whitespace(iter first, iter last) {
			return wjr::skip_whitespace(first, last);
		}

		template<typename Char, typename Traits>
		constexpr static void write(basic_String<Char, Traits>& str, double number) {
			str.reserve(str.size() + 64);
			basic_String<Char, Traits>::fixed_number(str.end(), str.end() + 64, number);
		}

	};

}

#endif