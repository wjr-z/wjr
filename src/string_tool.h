#ifndef STRING_TOOL_H
#define STRING_TOOL_H

#include "mallocator.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string.h>
#include <iostream>
#include <vector>

namespace wjr {

	template<class _RanItPat, class _Pred_eq = std::equal_to<>>
	class skmp_searcher_fshift_builder {

		using _Value_t = typename std::iterator_traits<_RanItPat>::value_type;
		using _Diff_t = typename std::iterator_traits<_RanItPat>::difference_type;

	public:

		skmp_searcher_fshift_builder(
			const _RanItPat _First, const _RanItPat _Last, const _Pred_eq _Eq
		) : _UFirst(_First), _ULast(_Last) {

			_Size = _Last - _First;

			_Fshift = mallocator<_Diff_t>().allocate(_Size + 1);
			_Fm = mallocator<_Diff_t>().allocate(_Size + 1);

			_Diff_t* _Fnxt = mallocator<_Diff_t>().allocate(_Size + 1);
			_Diff_t i = 0, j = -1;
			_Fnxt[0] = -1;

			while (i < _Size) { // KMP
				if (j == -1 || _Eq(*(_ULast - 1 - i), *(_ULast - 1 - j)))
					_Fnxt[++i] = ++j;
				else j = _Fnxt[j];
			}

			std::fill_n(_Fshift, _Size + 1, _Diff_t(-1)); // init fshift

			for (i = 1; i <= _Size; ++i) { // get fshift
				while (i < _Size && _Fnxt[i + 1] == _Fnxt[i] + 1)
					++i;
				j = i;
				while (_Fnxt[j] != -1 && _Fshift[_Fnxt[j]] == -1) {
					_Fshift[_Fnxt[j]] = i - _Fnxt[j];
					j = _Fnxt[j];
				}
			}

			for (i = 0; i <= _Size; ++i)_Fm[i] = _Size; // init fm
			i = _Size;

			while (_Fnxt[i] != -1) {
				_Fm[_Fnxt[i]] = _Size - _Fnxt[i]; // get fm
				i = _Fnxt[i];
			}

			for (i = 1; i <= _Size; ++i)_Fm[i] = std::min(_Fm[i], _Fm[i - 1]);

			for (i = 0; i <= _Size; ++i)
				_Fshift[i] = ~_Fshift[i] ? _Fshift[i] + i : 0;

			mallocator<_Diff_t>().deallocate(_Fnxt,_Size + 1);
		}

		skmp_searcher_fshift_builder(const skmp_searcher_fshift_builder& other)
			: _Size(other._Size), _UFirst(other._UFirst), _ULast(other._ULast) {
			_Fshift = mallocator<_Diff_t>().allocate(_Size + 1);
			_Fm = mallocator<_Diff_t>().allocate(_Size + 1);
			memcpy(_Fshift, other._Fshift, sizeof(_Diff_t) * (_Size + 1));
			memcpy(_Fm, other._Fm, sizeof(_Diff_t) * (_Size + 1));
		}

		skmp_searcher_fshift_builder(skmp_searcher_fshift_builder&& other)noexcept
			: _Fshift(other._Fshift), _Fm(other._Fm), _Size(other._Size),
			_UFirst(std::move(other._UFirst)), _ULast(std::move(other._ULast)) {
			other._Fshift = other._Fm = nullptr;
		}

		~skmp_searcher_fshift_builder() {
			mallocator<_Diff_t>().deallocate(_Fshift,_Size + 1);
			mallocator<_Diff_t>().deallocate(_Fm,_Size + 1);
		}

		_RanItPat _Get_First()const {
			return _UFirst;
		}

		_RanItPat _Get_Last() const {
			return _ULast;
		}

		_Diff_t* _Get_Fshift() const {
			return _Fshift;
		}

		_Diff_t* _Get_Fm()const {
			return _Fm;
		}

		_Diff_t size()const {
			return _Size;
		}

	private:

		_Diff_t* _Fshift, * _Fm, _Size;
		_RanItPat _UFirst, _ULast;
	};

	template <class _RanItPat, class _Hash_ty =
		std::hash<typename std::iterator_traits<_RanItPat>::value_type>, class _Pred_eq = std::equal_to<>>
		class skmp_searcher_char_builder // shift for char
		: public skmp_searcher_fshift_builder<_RanItPat, _Pred_eq> {
		using _Base = skmp_searcher_fshift_builder<_RanItPat, _Pred_eq>;
		using _Value_t = typename std::iterator_traits<_RanItPat>::value_type;
		using _Diff_t = typename std::iterator_traits<_RanItPat>::difference_type;
		public:

			skmp_searcher_char_builder(
				_RanItPat _First, _RanItPat _Last, _Hash_ty _Hash_fn = _Hash_ty(), _Pred_eq _Eq = _Pred_eq()
			) : _Base(_First, _Last, _Eq) {
				auto _UFirst = _Base::_Get_First();
				const auto _ULast = _Base::_Get_Last();
				const auto _Size = _Base::size();
				_Shift = mallocator<_Diff_t>().allocate(256);
				std::fill_n(_Shift, 256, _Size + 1);
				_Diff_t i = _Size;
				while (_UFirst != _ULast) {
					const auto _UValue = static_cast<std::make_unsigned_t<_Value_t>>(*_UFirst);
					_Shift[_UValue] = i;
					++_UFirst;
					--i;
				}
			}

			skmp_searcher_char_builder(const skmp_searcher_char_builder& other)
				: _Base(other) {
				_Shift = mallocator<_Diff_t>().allocate(256);
				memcpy(_Shift, other._Shift, sizeof(_Diff_t) * 256);
			}

			skmp_searcher_char_builder(skmp_searcher_char_builder&& other)noexcept
				: _Base(std::move(other)), _Shift(other._Shift) {
				other._Shift = nullptr;
			}

			~skmp_searcher_char_builder() {
				mallocator<_Diff_t>().deallocate(_Shift,256);
			}

			_Diff_t _Get_Shift(const _Value_t _Value)const {
				const auto _UValue = static_cast<std::make_unsigned_t<_Value_t>>(_Value);
				return _Shift[_UValue];
			}

			std::equal_to<> _Get_eq()const {
				return {};
			}

		private:
			_Diff_t* _Shift;
	};

	template <class _RanItPat, class _Hash_ty =
		std::hash<typename std::iterator_traits<_RanItPat>::value_type>, class _Pred_eq = std::equal_to<>>
		class skmp_searcher_general_builder // shift for more complex types
		: public skmp_searcher_fshift_builder<_RanItPat, _Pred_eq> {
		using _Base = skmp_searcher_fshift_builder<_RanItPat, _Pred_eq>;
		using _Value_t = typename std::iterator_traits<_RanItPat>::value_type;
		using _Diff_t = typename std::iterator_traits<_RanItPat>::difference_type;
		public:

			skmp_searcher_general_builder(
				_RanItPat _First, _RanItPat _Last, _Hash_ty _Hash_fn = _Hash_ty(), _Pred_eq _Eq = _Pred_eq()
			) : _Base(_First, _Last, _Eq) {
				auto _UFirst = _Base::_Get_First();
				const auto _ULast = _Base::_Get_Last();
				const auto _Size = _Base::size();
				_Diff_t i = _Size;
				while (_UFirst < _ULast) {
					_Map.insert_or_assign(*_UFirst, i);
					++_UFirst;
					--i;
				}
			}

			skmp_searcher_general_builder(const skmp_searcher_general_builder& other)
				: _Base(other), _Map(other._Map) {

			}

			skmp_searcher_general_builder(skmp_searcher_general_builder&& other)noexcept
				: _Base(std::move(other)), _Map(std::move(other._Map)) {

			}

			_Diff_t _Get_Shift(const _Value_t& _Value)const {
				const auto Iter = _Map.find(_Value);
				if (Iter == _Map.end()) {
					return _Base::size() + 1;
				}
				return Iter->second;
			}

			_Pred_eq _Get_eq()const {
				return _Map.key_eq();
			}

		private:
			std::unordered_map<_Value_t, _Diff_t, _Hash_ty, _Pred_eq> _Map;
	};

	template <class _RanItPat, class _Hash_ty, class _Pred_eq, class _Value_t =
		typename std::iterator_traits<_RanItPat>::value_type>
		using sunday_kmp_traits = std::conditional_t<std::is_integral_v<_Value_t> && sizeof(_Value_t) == 1
		&& (std::is_same_v<std::equal_to<>, _Pred_eq> || std::is_same_v<std::equal_to<_Value_t>, _Pred_eq>),
		skmp_searcher_char_builder<_RanItPat, _Hash_ty, _Pred_eq>,
		skmp_searcher_general_builder<_RanItPat, _Hash_ty, _Pred_eq>>;

	template<typename _Container, typename = void>
	struct _Has_begin_end : std::false_type {};

	template<typename _Container>
	struct _Has_begin_end < _Container, std::void_t<
		decltype(std::declval<_Container>().begin(), std::declval<_Container>().end()) >>
		: std::true_type{};

	template<typename _Container>
	using HAS_BEGIN_END = std::enable_if_t<_Has_begin_end<_Container>::value, int>;

	template<class U, class = void>
	struct _Has_Iterator : std::false_type {};
	template<class U>
	struct _Has_Iterator<U, std::void_t<decltype(std::declval<U>().begin())>> : std::true_type {};

	template<class U>
	struct _Iterator_Traits {
		using type = decltype(std::declval<U>().begin());
	};

	template<typename _Container>
	using container_iterator_type = typename _Iterator_Traits<_Container>::type;

	template <class _RanItPat, class _Hash_ty
		= std::hash<typename std::iterator_traits<_RanItPat>::value_type>, 
		class _Pred_eq = std::equal_to<>>
	class skmp_searcher {

		using _Value_t = typename std::iterator_traits<_RanItPat>::value_type;
		using _Diff_t = typename std::iterator_traits<_RanItPat>::difference_type;

		public:

			using value_type = _Value_t;
			using difference = _Diff_t;

			skmp_searcher(
				const _RanItPat _First, const _RanItPat _Last, _Hash_ty _Hash_Fn = _Hash_ty(), _Pred_eq _Eq = _Pred_eq()
			) : _Searcher(_First, _Last, _Hash_Fn, _Eq) {

			}

			template<class _Container,HAS_BEGIN_END<const _Container&> = 0>
			skmp_searcher(
					const _Container& _Cont, _Hash_ty _Hash_Fn = _Hash_ty(), _Pred_eq _Eq = _Pred_eq()
				) : _Searcher(_Cont.cbegin(), _Cont.cend(), _Hash_Fn, _Eq) { 
				// containers that have begin() and end() 
			}

			skmp_searcher(const _RanItPat _First, const size_t _Length,_Hash_ty _Hash_fn = _Hash_ty(), _Pred_eq _Eq = _Pred_eq()) 
				: _Searcher(_First,_First + _Length,_Hash_fn,_Eq) {

			}

			skmp_searcher(const skmp_searcher& other)
				: _Searcher(other._Searcher) {

			}

			skmp_searcher(skmp_searcher&& other)noexcept
				: _Searcher(std::move(other._Searcher)) {

			}

			~skmp_searcher() {

			}

			template <class _RanItHaystack>
			std::pair<_RanItHaystack, _RanItHaystack> operator()(
				_RanItHaystack _First, _RanItHaystack _Last)const;

		private:
			using _Traits = sunday_kmp_traits<_RanItPat, _Hash_ty, _Pred_eq>;
			_Traits _Searcher;
	};

	template <class _RanItPat, class _Hash_ty,class _Pred_eq>
	template <class _RanItHaystack>
	std::pair<_RanItHaystack, _RanItHaystack> skmp_searcher<_RanItPat,_Hash_ty,_Pred_eq>::operator()(
		_RanItHaystack _First, _RanItHaystack _Last)const {

		const auto _Size = _Searcher.size(); // pattern string 's size

		if (_Size == 0) {
			return { _First,_First };
		}

		const auto pattern_first = _Searcher._Get_First();
		const auto pattern_last = _Searcher._Get_Last();
		const auto pattern_end = pattern_last - 1;

		const auto text_first = _First;
		const auto text_last = _Last;
		const auto text_end = text_last - 1;

		const auto _Eq = _Searcher._Get_eq();

		const auto* _Fshift = _Searcher._Get_Fshift();
		const auto* _Fm = _Searcher._Get_Fm();

		auto ch = *pattern_end;

		auto text_ptr = text_first + _Size - 1;
		//sufsearch : last jump distance
		//alsearch  : last match length
		_Diff_t sufsearch = 0, alsearch = 0;
		size_t remain_size = text_end - text_ptr;
		for(;;) {

			auto s_text_ptr(text_ptr);
			auto s_pattern_ptr(pattern_end);
			const auto s_ptr_mid(text_ptr - sufsearch);

			while (s_text_ptr != s_ptr_mid && _Eq(*s_text_ptr, *s_pattern_ptr)) {
				--s_text_ptr, --s_pattern_ptr;
			}

			//to avoid matching twice 
			if (s_text_ptr == s_ptr_mid) {

				auto text_ptr_head = text_ptr - (_Size - 1);

				if (alsearch + sufsearch == _Size) {
					return { text_ptr_head ,text_ptr + 1 };
				}

				s_text_ptr -= alsearch;
				s_pattern_ptr -= alsearch;

				while (_Eq(*s_text_ptr, *s_pattern_ptr)) {
					if (s_text_ptr == text_ptr_head) {
						return { text_ptr_head ,text_ptr + 1 };
					}
					--s_text_ptr;
					--s_pattern_ptr;
				}
			}

			if (text_ptr == text_end)
				break;

			const _Diff_t _Match = text_ptr - s_text_ptr;
			const auto fs = _Fshift[_Match]; 
			const auto sh = _Searcher._Get_Shift(*(text_ptr + 1));
			const auto fm = _Fm[_Match];

			if (fs) {
				if (fs >= sh) {
					sufsearch = fs - _Match;
					alsearch = _Match;
				}
				else {
					sufsearch = sh;
					alsearch = 0;
				}
			}
			else {
				if (sh <= _Size) {
					sufsearch = fm;
					alsearch = _Size - fm;
				}
				else {
					sufsearch = sh;
					alsearch = 0;
				}
			}

			if(sufsearch > remain_size)
				break;

			remain_size -= sufsearch;
			text_ptr += sufsearch;

			if constexpr ( std::is_integral_v<_Value_t> && sizeof(_Value_t) == 1 
				&& std::is_pointer_v<_RanItHaystack>) {
				//if can use memchr to speed up
				const auto l = sufsearch + alsearch;
				if (l <= 32 && !_Eq(*text_ptr, ch)) {
					const auto pos = memchr(text_ptr + 1, ch, remain_size);
					if (pos == nullptr)
						break;
					const _Diff_t delta = (const _Value_t*)pos - text_ptr;
					if (delta > l) {
						remain_size -= delta;
						text_ptr += delta;
						sufsearch = 0;
						alsearch = 0;
					}
				}
			}
			
			if (sufsearch >= _Size) {
				sufsearch = 0;
				alsearch = 0;
			}

		}

		return { text_last,text_last };
	}

	template<typename _Container,typename _Hash,typename _Pred ,
		HAS_BEGIN_END<const _Container&> = 0>
	skmp_searcher(const _Container&,_Hash,_Pred)
		->skmp_searcher<container_iterator_type<const _Container&>,_Hash,_Pred>;

	template<typename _Container,typename _Hash,HAS_BEGIN_END<const _Container&> = 0>
	skmp_searcher(const _Container&, _Hash)
		->skmp_searcher<container_iterator_type<const _Container&>, _Hash>;

	template<typename _Container, 
		HAS_BEGIN_END<const _Container&> = 0>
	skmp_searcher(const _Container&)->skmp_searcher<container_iterator_type<const _Container&>>;

	template <class _Elem, class _Traits, class _Alloc>
	std::vector<std::basic_string<_Elem, _Traits, _Alloc>> split(
		const std::basic_string<_Elem, _Traits, _Alloc>& str
		,const char Separator) {
		using string_type = std::basic_string<_Elem, _Traits, _Alloc>;
		const size_t length = str.length();
		const _Elem* data = str.c_str();
		std::vector<string_type> strVec;
		size_t pos = 0;
		while (pos < length) {
			void* fnd = memchr((void*)(data + pos), Separator, length - pos);
			if (fnd == nullptr) {
				strVec.emplace_back(str.substr(pos, length - pos));
				break;
			}
			size_t nxt = (const _Elem*)fnd - data;
			if (pos != nxt)
				strVec.emplace_back(str.substr(pos, nxt - pos));
			pos = nxt + 1;
		}
		return strVec;
	}

	template <class _Elem, class _Traits, class _Alloc,class _Elem2,class _Traits2,class _Alloc2>
	std::vector<std::basic_string<_Elem, _Traits, _Alloc>> split(
		const std::basic_string<_Elem, _Traits, _Alloc>& str
		, const std::basic_string<_Elem2, _Traits2, _Alloc2>& Separator) {
		using string_type = std::basic_string<_Elem, _Traits, _Alloc>;
		const size_t length = str.length();
		const size_t separator_length = Separator.length();
		const _Elem* data1 = str.c_str();
		const _Elem2* data2 = Separator.c_str();
		std::vector<string_type> strVec;
		size_t pos = 0;
		skmp_searcher searcher(data2,data2+separator_length);
		while (pos < length) {
			auto fnd = searcher(data1 + pos,data1 + length);
			if (fnd.first == data1 + length) {
				strVec.emplace_back(str.substr(pos, length - pos));
				break;
			}
			size_t nxt = fnd.first - data1;
			if (pos != nxt)
				strVec.emplace_back(str.substr(pos, nxt - pos));
			pos = nxt + separator_length;
		}
		return strVec;
	}

	std::tuple<size_t, size_t> split(const char* ptr, const char cl, const char cr, const size_t _MaxCount);

	template<typename _Elem,typename _Traits,typename _Alloc>
	std::basic_string<_Elem, _Traits, _Alloc>& ltrim(std::basic_string<_Elem, _Traits, _Alloc>& str) {
		if (str.empty()) return str;
		auto iter = str.cbegin();
		auto e = str.cend();
		while (iter != e && isspace(*iter))++iter;
		str.erase(str.cbegin(),iter);
		return str;
	}

	template<typename _Elem, typename _Traits, typename _Alloc>
	std::basic_string<_Elem, _Traits, _Alloc>& rtrim(std::basic_string<_Elem, _Traits, _Alloc>& str) {
		if (str.empty()) return str;
		auto iter = str.cend();
		auto e = str.cbegin();
		while (iter != e && isspace(*(--iter)));
		str.erase(++iter,str.cend());
		return str;
	}

	template<typename _Elem, typename _Traits, typename _Alloc>
	std::basic_string<_Elem, _Traits, _Alloc>& trim(std::basic_string<_Elem, _Traits, _Alloc>& str) {
		return ltrim(rtrim(str));
	}

	size_t find_first_pos(const char* ptr, const char ch, const size_t _MaxCount);
	size_t get_number_of_char(const char* str, const size_t length, const char ch);
	size_t get_number_of_char(const char* str, const char ch);
	size_t get_number_of_char(const std::string&, const char ch);

	bool isdigit_or_sign(uint8_t ch);

	template<typename T>
	size_t read_integer(const char* str,T& val) {
		auto* ptr = (const uint8_t*)str;
		auto* s = ptr;
		val = 0;
		bool neg = false;
		for (; !isdigit_or_sign(*ptr); ++ptr);
		switch (*ptr) {
		case '-':neg = true; ++ptr; break;
		case '+':++ptr; break;
		}
		val = *ptr - '0';
		++ptr;
		for (; isdigit(*ptr); ++ptr) {
			val = val * 10 + (*ptr - '0');
		}
		if(!neg) val = -val;
		return ptr - s;
	}

	size_t read_uint(const char*str,unsigned int&);
	size_t read_int(const char*str,int&);
	size_t read_ull(const char*str,unsigned long long&);
	size_t read_ll(const char*str,long long&);

	inline double power_of_10(int index);
	size_t read_double(const char*str,double&);

}

#endif
