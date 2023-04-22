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
		if constexpr (std::is_same_v<value_t, char> && is_contiguous_iter_v<iter>) {
			do {
				// very fast for most cases
				// In most cases, skipping characters greater than or equal to 2 
				// will be faster than regular loops above this threshold.
				//And in most cases, even if there are transfer characters, 
				// there are only a few
				auto pos = wjr::find(first, last, '"');
				auto pos2 = wjr::find_if(std::reverse_iterator(pos), std::reverse_iterator(first), [](auto x) {
					return x != '\\';
					});
				if (static_cast<size_t>(pos - pos2.base()) % 2 == 0) {
					return pos;
				}
				first = pos + 1;
			} while (true);
		}
		else {
			auto u_value = make_unsigned_v(*first);
			while (string_skip_step[u_value]) {
				first += string_skip_step[u_value];
				u_value = make_unsigned_v(*first);
			}
			return first;
		}
	}
	else {
		auto u_value = make_unsigned_v(*first);
		while (u_value >= 256 || string_skip_step[u_value]) {
			first += u_value >= 256 ? 1 : string_skip_step[u_value];
			u_value = make_unsigned_v(*first);
		}
		return first;
	}
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
			if ((*s >= 0 && *s < 32) || *s == 127)
				return false;
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

bool json::accept(const char* first, const char* last) {
	first = ascii::encode::skipw(first, last);
	if (first == last) {
		return false;
	}

	switch (*first) {
	case '[':
		++first;
		if (!__accept(first, last, ']'))
			return false;
		first = ascii::encode::skipw(first, last);
		return first == last;
	case '{':
		++first;
		if (!__accept(first, last, '}'))
			return false;
		first = ascii::encode::skipw(first, last);
		return first == last;
	default:
		return false;
	}
}

json json::__parse(const char*& first, const char* last) {
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
	case '"': {
		++first;
		auto t = skip_string(first, last);
		json it(std::in_place_type_t<string>{}, first, t);
		first = t + 1;
		return it;
	}
	case '[': {
		json it(std::in_place_type_t<array>{});
		auto& arr = it.get_array();
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
	case '{': {
		json it(std::in_place_type_t<object>{});
		auto& obj = it.get_object();
		++first;
		first = ascii::encode::skipw(first, last);
		if (*first == '}') {
			++first;
			return it;
		}
		for(;;) {
			first = ascii::encode::skipw(first, last);
			++first;
			auto p = skip_string(first, last);
			string name(first, p);
			first = p + 1;
			first = ascii::encode::skipw(first, last);
			++first;
			obj.insert_or_assign(std::move(name), __parse(first, last));
			first = ascii::encode::skipw(first, last);
			if (*first == '}') {
				++first;
				break;
			}
			++first;
		}
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

template<int m>
void json::_stringify(string& str, int a) const noexcept {
	constexpr int LEN = []() {
		switch (m) {
		case SHORTEST:return 0;
		case TWO_SPACE_ALIGN: return 2;
		case FOUR_SPACE_ALIGN: return 4;
		case TAB_ALGIN: return 1;
		}
	}();
	switch (m_value.index()) {
	case 0: {
		str.append("null");
		break;
	}
	case 1: {
		if (get_boolean()) {
			str.append("true");
		}
		else {
			str.append("false");
		}
		break;
	}
	case 2: {
		str.reserve(str.size() + 256);
		size_t length = 0;
		ascii::encode::from_floating_point<double>(
			get_number(), str.end(), 256, &length);
		str.inc_size(length);
		break;
	}
	case 3: {
		str.append('"').append(get_string()).append('"');
		break;
	}
	case 4: {
		const auto& arr = get_array();
		str.push_back('[');
		if (!arr.empty()) {
			bool head = true;
			for (const auto& val : arr) {
				if (head) head = false;
				else str.push_back(',');
				if constexpr (m != SHORTEST) {
					str.push_back('\n');
					if constexpr (m == TAB_ALGIN) {
						str.append(a + LEN, '\t');
					}
					else {
						str.append(a + LEN, ' ');
					}
				}
				val.template _stringify<m>(str, a + LEN);
			}
			if constexpr (m != SHORTEST) {
				str.push_back('\n');
				if constexpr (m == TAB_ALGIN) {
					str.append(a + LEN, '\t');
				}
				else {
					str.append(a + LEN, ' ');
				}
			}
		}
		str.push_back(']');
		break;
	}
	case 5: {
		auto& obj = get_object();
		str.push_back('{');
		if (!obj.empty()) {
			bool head = true;
			for (const auto& [name, val] : obj) {
				if (head) head = false;
				else str.push_back(',');
				if constexpr (m != SHORTEST) {
					str.push_back('\n');
					if constexpr (m == TAB_ALGIN) {
						str.append(a + LEN, '\t');
					}
					else {
						str.append(a + LEN, ' ');
					}
				}
				str.append('"').append(name).append("\": ");
				val.template _stringify<m>(str, a + LEN);
			}
			if constexpr (m != SHORTEST) {
				str.push_back('\n');
				if constexpr (m == TAB_ALGIN) {
					str.append(a, '\t');
				}
				else {
					str.append(a, ' ');
				}
			}
		}
		str.push_back('}');
		break;
	}
	default:WJR_UNREACHABLE; break;
	}
}

template void json::_stringify<0>(string& str, int a) const noexcept;
template void json::_stringify<1>(string& str, int a) const noexcept;
template void json::_stringify<2>(string& str, int a) const noexcept;
template void json::_stringify<3>(string& str, int a) const noexcept;

bool operator==(const json& a, const json& b) {
	if (a.index() != b.index()) {
		return false;
	}
	return wjr::visit([&b](const auto& x) {
		using value_type = remove_cvref_t<decltype(x)>;
		return x == wjr::get<value_type>(b);
		}, a);
}


_WJR_END