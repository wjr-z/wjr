#include <array>

#include <wjr/json.h>

_WJR_BEGIN

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

template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
constexpr iter skip_string(iter first, WJR_MAYBE_UNUSED iter last) {
	using value_t = typename std::iterator_traits<iter>::value_type;
	using uvalue_t = std::make_unsigned_t<value_t>;
	if constexpr (std::numeric_limits<uvalue_t>::max() < 256) {
		auto u_value = make_unsigned_v(*first);
		while (string_skip_step[u_value]) {
			first += string_skip_step[u_value];
			u_value = make_unsigned_v(*first);
		}
	}
	else {
		auto u_value = make_unsigned_v(*first);
		while (u_value >= 256 || string_skip_step[u_value]) {
			first += u_value >= 256 ? 1 : string_skip_step[*first];
			u_value = make_unsigned_v(*first);
		}
	}
	return first;
}

template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
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
				if (!ascii::encode::isalnum(*s)) {
					return false;
				}
				++s;
				if (!ascii::encode::isalnum(*s)) {
					return false;
				}
				++s;
				if (!ascii::encode::isalnum(*s)) {
					return false;
				}
				++s;
				if (!ascii::encode::isalnum(*s)) {
					return false;
				}
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

bool json::__accept(const char*& first, const char* last, uint8_t state) {
	bool head = true;
	for (;; ++first) {
		first = ascii::encode::skipw(first, last);
		if (first == last)return false;

		if (state == '}') {
			if (*first == '}') {
				if (!head) return false;
				++first;
				return true;
			}
			if (*first != '"')return false;
			if (!check_string(first, last))
				return false;
			++first;
			first = ascii::encode::skipw(first, last);
			if (first == last || *first != ':') return false;
			++first;
			first = ascii::encode::skipw(first, last);
			if (first == last) return false;
		}
		else {
			if (*first == ']') {
				if (!head)return false;
				++first;
				return true;
			}
		}
		head = false;

		switch (*first) {
		case 'n':
			if (first + 3 < last) {
				if(memcmp(first + 1, "ull", 3) == 0) {
					first += 4;
					break;
				}
			}
			return false;
		case 't':
			if (first + 3 < last) {
				if (memcmp(first + 1, "rue", 3) == 0) {
					first += 4;
					break;
				}
			}
			return false;
		case 'f':
			if (first + 4 < last) {
				if (memcmp(first + 1, "alse", 4) == 0) {
					first += 5;
					break;
				}
			}
			return false;
		case '"':
			if (!check_string(first, last))
				return false;
			++first;
			break;
		case '[':
			++first;
			if (!__accept(first, last, ']'))
				return false;
			break;
		case '{':
			++first;
			if (!__accept(first, last, '}'))
				return false;
			break;
		default:
			const char* pos = nullptr;
			errc c = errc::ok;
			(void)ascii::encode::to_floating_point<double>(first, last, &pos, &c,
				std::integral_constant<ascii::encode::to_f_flags, ascii::encode::to_f_flags::ALLOW_TRAILING_JUNK>());
			if (c != errc::ok)
				return false;
			first = pos;
			break;
		}

		first = ascii::encode::skipw(first, last);
		if (first == last)return false;
		if (*first == ',')
			continue;
		if (*first != state)
			return false;
		++first;
		return true;
	}
}

json json::__parse(const char*& first, const char* last) {
	wjr::string_view ss(first, std::min<int>(128, last - first));
	first = ascii::encode::skipw(first, last);
	switch (*first) {
	case 'n': {
		json it(std::in_place_type_t<null>{});
		first += 4;
		return it;
	}
	case 't': {
		json it(std::in_place_type_t<boolean>{}, true);
		first += 4;
		return it;
	}
	case 'f': {
		json it(std::in_place_type_t<boolean>{}, false);
		first += 5;
		return it;
	}
	case '{': {
		json it(std::in_place_type_t<vobject>{});
		auto& obj = wjr::get<vobject>(it);
		++first;
		first = ascii::encode::skipw(first, last);
		if (*first == '}') {
			++first;
			return it;
		}
		while (true) {
			first = ascii::encode::skipw(first, last);
			++first;
			auto p = skip_string(first, last);
			auto& item = obj[vstring(first, p)];
			first = p + 1;
			first = ascii::encode::skipw(first, last);
			++first;
			item = __parse(first, last);
			first = ascii::encode::skipw(first, last);
			if (*first == '}') {
				++first;
				break;
			}
			++first;
		}
		return it;
	}
	case '[': {
		json it(std::in_place_type_t<varray>{});
		auto& arr = wjr::get<varray>(it);
		++first;
		first = ascii::encode::skipw(first, last);
		if (*first == ']') {
			++first;
			return it;
		}
		for (;;) {
			arr.emplace_back(__parse(first, last));
			first = ascii::encode::skipw(first, last);
			if (*first == ']') {
				++first;
				break;
			}
			++first;
		}
		arr.shrink_to_fit();
		return it;
	}
	case '"': {
		++first;
		auto t = skip_string(first, last);
		json it(std::in_place_type_t<vstring>{}, first, t);
		first = t + 1;
		return it;
	}
	default: {
		const char* pos = nullptr;
		auto val = ascii::encode::to_floating_point<double>(first, last, &pos, nullptr, 
			std::integral_constant<ascii::encode::to_f_flags, ascii::encode::to_f_flags::ALLOW_TRAILING_JUNK>());
		first = pos;
		json it(std::in_place_type_t<number>{}, val);
		return it;
	}
	}
}

_WJR_END