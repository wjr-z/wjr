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
	using encode_type = typename json_traits::encode_type;
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
				if (!encode_type::isalnum(*s)) {
					return false;
				}
				++s;
				if (!encode_type::isalnum(*s)) {
					return false;
				}
				++s;
				if (!encode_type::isalnum(*s)) {
					return false;
				}
				++s;
				if (!encode_type::isalnum(*s)) {
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
		first = encode_type::skipw(first, last);
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
			first = encode_type::skipw(first, last);
			if (first == last || *first != ':') return false;
			++first;
			first = encode_type::skipw(first, last);
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
			(void)encode_type::to_floating_point<double>(first, last, &pos, &c,
				std::integral_constant<encode_type::to_f_flags, encode_type::to_f_flags::ALLOW_TRAILING_JUNK>());
			if (c != errc::ok)
				return false;
			first = pos;
			break;
		}

		first = encode_type::skipw(first, last);
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
	first = encode_type::skipw(first, last);
	if (first == last) {
		return false;
	}

	switch (*first) {
	case '[':
		++first;
		if (!__accept(first, last, ']'))
			return false;
		first = encode_type::skipw(first, last);
		return first == last;
	case '{':
		++first;
		if (!__accept(first, last, '}'))
			return false;
		first = encode_type::skipw(first, last);
		return first == last;
	default:
		return false;
	}
}

json::json(const char*& first, const char* last, parse_tag) {
	first = encode_type::skipw(first, last);
	switch (*first) {
	case 'n': {
		this->emplace_from<0, null>();
		first += 4;
		break;
	}
	case 't': {
		this->emplace_from<0, boolean>(true);
		first += 4;
		break;
	}
	case 'f': {
		this->emplace_from<0, boolean>(false);
		first += 5;
		break;
	}
	case '"': {
		++first;
		auto t = skip_string(first, last);
		this->emplace_from<0, string>(first, t);
		first = t + 1;
		break;
	}
	case '[': {
		this->emplace_from<0, array>();
		auto& arr = this->get_array();
		++first;
		first = encode_type::skipw(first, last);
		if (*first == ']') {
			++first;
			break;
		}
		for (;;) {
			arr.emplace_back(first, last, parse_tag{});
			first = encode_type::skipw(first, last);
			if (*first == ']') {
				++first;
				break;
			}
			++first;
		}
		arr.shrink_to_fit();
		break;
	}
	case '{': {
		this->emplace_from<0, object>();
		auto& obj = this->get_object();
		++first;
		first = encode_type::skipw(first, last);
		if (*first == '}') {
			++first;
			break;
		}
		for(;;) {
			first = encode_type::skipw(first, last);
			++first;
			auto p = skip_string(first, last);
			string name(first, p);
			first = p + 1;
			first = encode_type::skipw(first, last);
			++first;
			obj.emplace(std::piecewise_construct,
				std::forward_as_tuple(std::move(name)),
				std::forward_as_tuple(first, last, parse_tag{}));
			first = encode_type::skipw(first, last);
			if (*first == '}') {
				++first;
				break;
			}
			++first;
		}
		break;
	}
	default: {
		const char* pos = nullptr;
		auto val = encode_type::to_floating_point<double>(first, last, &pos, nullptr, 
			std::integral_constant<encode_type::to_f_flags, encode_type::to_f_flags::ALLOW_TRAILING_JUNK>());
		first = pos;
		this->emplace_from<0, number>(val);
		break;
	}
	}
}

template<int m>
void json::_stringify(string& str, int a) const noexcept {
	constexpr int LEN = []() {
		switch (m) {
		case SHORTEST:return 0;
		case TAB_ALIGN: return 1;
		case TWO_SPACE_ALIGN: return 2;
		case FOUR_SPACE_ALIGN: return 4;
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
		ascii_modifier it(str.lastPtr(), str.endPtr() - str.lastPtr());
		char* e = nullptr;
		it.from_floating_point(get_number(), &e);
		str.set_size(e - str.data());
		str.finalize();
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
					if constexpr (m == TAB_ALIGN) {
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
				if constexpr (m == TAB_ALIGN) {
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
					if constexpr (m == TAB_ALIGN) {
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
				if constexpr (m == TAB_ALIGN) {
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