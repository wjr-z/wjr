#ifndef WJR_STRING_H
#define WJR_STRING_H

#include <iostream>
#include <string.h>
#include <cassert>
#include <string>
#include <iterator>
#include <string_view>

#include "mallocator.h"
#include "mySTL.h"

namespace wjr {

	template<typename Char>
	class String_core {
	public:

		String_core() {
			reset();
		}

		String_core(const String_core& other) {
			if (!other.category()) {
				copySmall(other);
			}
			else {
				copyMedium(other);
			}
		}

		String_core(String_core&& other)noexcept 
			: _Ml(other._Ml) {
			other.reset();
		}

		String_core(const Char* _Data, const size_t s) {
			if (s <= maxSmallSize) {
				initSmall(_Data, s);
			}
			else {
				initMedium(_Data, s);
			}
		}

		String_core& operator=(const String_core&) = delete;

		~String_core()noexcept {
			if (!category())return;
			Getal().deallocate(_Ml._Data, _Ml.capacity() + 1);
		}

		void swap(String_core& other) {
			std::swap(_Ml, other._Ml);
		}

		Char* data() {
			return category() ? _Ml._Data : _Small;
		}

		const Char* data()const {
			return category() ? _Ml._Data : _Small;
		}

		const Char* c_str()const {
			return category() ? _Ml._Data : _Small;
		}

		bool empty()const {
			return !size();
		}

		void shrink(const size_t delta) {
			if (!category())
				shrinkSmall(delta);
			else shrinkMedium(delta);
		}

		void reserve(const size_t s) {
			if (!category())
				reserveSmall(s);
			else reserveMedium(s);
		}

		// expand size of n without init
		Char* expandNoinit(const size_t n, bool expGrowth = false);

		void push_back(char c) { *expandNoinit(1,true) = c; }

		size_t size()const {
			return category() ? MediumSize() : SmallSize();
		}

		size_t capacity()const {
			return category() ? _Ml.capacity() : maxSmallSize;
		}

		void shrink_to_fit();

		static mallocator<Char>& Getal() {
			static mallocator<Char> _alloc;
			return _alloc;
		}

	private:
		Char* c_str() {
			return category() ? _Ml._Data : _Small;
		}

		struct _Medium {
			Char* _Data;
			size_t _Size;
			size_t _Capacity;
			inline size_t capacity()const {
				return _Capacity & capacityExtractMask;
			}
		};
		constexpr static size_t lastChar = sizeof(_Medium) - 1;
		constexpr static size_t maxSmallSize = lastChar / sizeof(Char);
		constexpr static uint8_t categoryExtractMask = 0x80;
		constexpr static size_t kShift = (sizeof(size_t) - 1) * 8;
		constexpr static size_t capacityExtractMask = ~((size_t)categoryExtractMask << kShift);
		constexpr static size_t capacityOr = ~capacityExtractMask;

		static_assert(
			!(sizeof(_Medium) % sizeof(Char)),
			"Corrupt memory layout for String.");

		union {
			uint8_t _Byte[sizeof(_Medium)];
			Char _Small[sizeof(_Medium) / sizeof(Char)];
			_Medium _Ml;
		};

		inline bool category()const {
			return _Byte[lastChar] == categoryExtractMask;
		}

		void setCapacity(const size_t c) {
			_Ml._Capacity = c | capacityOr;
		}

		void reset() { setSmallSize(0); }

		void setSmallSize(const size_t s) {
			_Small[maxSmallSize] = static_cast<Char>(maxSmallSize - s);
			_Small[s] = '\0';
		}

		void setMediumSize(const size_t s) {
			_Ml._Size = s;
			_Ml._Data[s] = '\0';
		}

		size_t SmallSize()const {
			return maxSmallSize - _Small[maxSmallSize];
		}

		size_t MediumSize()const {
			return _Ml._Size;
		}

		void initSmall(const Char* _Data, const size_t s) {
			memcpy(_Small, _Data, sizeof(Char) * s);
			setSmallSize(s);
		}

		void initMedium(const Char* _Data, const size_t s) {
			_Ml._Data = Getal().allocate(s + 1);
			memcpy(_Ml._Data, _Data, sizeof(Char) * s);
			setMediumSize(s);
			setCapacity(s);
		}

		void copySmall(const String_core& other) {
			_Ml = other._Ml;
		}

		void copyMedium(const String_core& other) {
			const size_t s = other.size();
			_Ml._Data = Getal().allocate(s + 1);
			memcpy(_Ml._Data, other._Ml._Data, sizeof(Char) * s);
			setMediumSize(s);
			setCapacity(s);
		}

		void shrinkSmall(const size_t delta) {
			setSmallSize(SmallSize() - delta);
		}

		void shrinkMedium(const size_t delta) {
			setMediumSize(MediumSize() - delta);
		}

		void reserveSmall(const size_t s) ;

		void reserveMedium(const size_t s);

	};

	template<typename Char>
	Char* String_core<Char>::expandNoinit(const size_t n, bool expGrowth ) {
		size_t siz, newsiz;
		if (!category()) {
			siz = SmallSize();
			newsiz = siz + n;
			if (newsiz <= maxSmallSize) {
				setSmallSize(newsiz);
				return _Small + siz;
			}
			reserveSmall(expGrowth ? std::max(newsiz, 2 * maxSmallSize) : newsiz);
		}
		else {
			siz = _Ml._Size;
			newsiz = siz + n;
			const auto _Old_Capacity = _Ml.capacity();
			if (newsiz > _Old_Capacity) {
				reserveMedium(expGrowth ? std::max(newsiz,
					1 + _Old_Capacity + _Old_Capacity / 2) : newsiz);
			}
		}
		setMediumSize(newsiz);
		return _Ml._Data + siz;
	}

	template<typename Char>
	void String_core<Char>::shrink_to_fit() {
		// if is small then do nothing
		if (category()) {
			const size_t _size = MediumSize();
			const size_t _capacity = _Ml.capacity();
			const Char* _data = _Ml._Data;
			auto& al = Getal();
			if (size() <= maxSmallSize) {
				initSmall(_data, _size);
			}
			else {
				initMedium(_data, _size);
			}
			al.deallocate(_data, _capacity + 1);
		}
	}

	template<typename Char>
	void String_core<Char>::reserveSmall(const size_t s) {
		if (s <= maxSmallSize)return;
		const auto _New_Data = Getal().allocate(s + 1);
		const auto _Old_Size = SmallSize();
		memcpy(_New_Data, _Small, sizeof(Char) * _Old_Size);
		_Ml._Data = _New_Data;
		setMediumSize(_Old_Size);
		setCapacity(s);
	}

	template<typename Char>
	void String_core<Char>::reserveMedium(const size_t s) {
		const auto _Old_Capacity = _Ml.capacity();
		if (s <= _Old_Capacity)
			return;

		auto Al = Getal();
		const auto _New_Data = Al.allocate(s + 1);
		const auto _Old_Size = _Ml._Size;
		memcpy(_New_Data, _Ml._Data, sizeof(Char) * _Old_Size);
		Al.deallocate(_Ml._Data, _Old_Capacity + 1);
		_Ml._Data = _New_Data;
		setMediumSize(_Old_Size);
		setCapacity(s);
	}

	template<typename Char,typename Traits = std::char_traits<Char>>
	class basic_String {
	private:

		using _Elem = Char;
		template <class iter>
		using is_char_ptr = 
			std::bool_constant<is_any_of_v<iter,const _Elem*const,const _Elem*,_Elem*const,_Elem*>>;

		template<typename iter>
		using is_length = 
			std::bool_constant<std::numeric_limits<iter>::is_specialized>;

		template<typename iter>
		using is_const_iterator = std::enable_if_t<is_char_ptr<iter>::value,int>;

	public:

		using traits_type = Traits;
		using allocator_type = mallocator<Char>;

		using value_type = Char;
		using reference = value_type&;
		using const_reference = const value_type&;

		using size_type = size_t;
		using difference_type = size_t;

		using pointer = Char*;
		using const_pointer = const Char*;

		using iterator = Char*;
		using const_iterator = const Char*;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		constexpr static size_type npos = static_cast<size_type>(-1);

	private:
		static size_t traits_length(const value_type*s);

		static size_type npos_min(size_type n, const size_type x) {
			if constexpr (std::is_unsigned_v<size_type>) {
				return n < x ? n : x;
			}
			else {
				assert(n >=0 || n == npos);
				return (n == npos || n > x) ? x : n;
			}
		}

	public:

		basic_String()noexcept {}

		basic_String(const basic_String& other)
			:core(other.core) {

		}

		basic_String(basic_String&& other)
			:core(std::move(other.core)) {

		}

		basic_String(const basic_String& other, const size_type pos, const size_type n = npos) {
			assign(other, pos, n);
		}

		basic_String(const value_type* s)
			: core(s, traits_length(s)) {

		}

		basic_String(const value_type* s, const size_type n)
			: core(s, n) {

		}

		basic_String(const size_type n, const value_type c) {
			auto const pData = core.expandNoinit(n);
			std::fill(pData, pData + n, c);
		}

		basic_String(const value_type* first, const value_type* last)
			: core(first,static_cast<size_type>(last - first)) {

		}

		template<typename iter>
		basic_String(iter first, iter last) {
			assign(first, last, is_char_ptr<iter>{});
		}

		basic_String(std::initializer_list<value_type> il) {
			assign(il.begin(),il.end());
		}

		~basic_String()noexcept {}

		basic_String& operator=(const basic_String& other) {
			if (this == &other)
				return *this;
			return assign(other.data(), other.size());
		}

		basic_String& operator=(basic_String&& other) {
			core.swap(other.core);
			return *this;
		}

		basic_String& operator=(const value_type c) {
			resize(1);
			*begin() = c;
			return *this;
		}

		basic_String& operator=(const value_type* str) {
			return assign(str);
		}

		template<typename _Traits,typename _Alloc>
		basic_String& operator=(const std::basic_string<value_type, _Traits, _Alloc>& str) {
			return assign(str.data(),str.size());
		}

		basic_String& operator=(std::initializer_list<value_type> il) {
			return assign(il.begin(),il.end());
		}

		operator std::basic_string_view<Char, Traits>() const noexcept {
			return {data(),size()};
		}

		iterator begin() { return core.data(); }

		const_iterator begin()const { return core.data(); }

		const_iterator cbegin()const { return begin(); }

		iterator end() { return core.data() + core.size(); }

		const_iterator end()const { return core.data() + core.size(); }

		const_iterator cend()const { return end(); }

		reverse_iterator rbegin(){return reverse_iterator(end()); }

		const_reverse_iterator rbegin()const{return const_reverse_iterator(end()); }

		const_reverse_iterator crbegin()const{return const_reverse_iterator(cend()); }

		reverse_iterator rend(){return reverse_iterator(begin()); }

		const_reverse_iterator rend()const{return const_reverse_iterator(begin()); }

		const_reverse_iterator crend()const{return const_reverse_iterator(begin()); }

		reference front() { return *begin(); }

		const_reference front()const { return *begin(); }

		reference back() {
			assert(!empty());
			return *(end() - 1);
		}

		const_reference back()const {
			assert(!empty());
			return *(end() - 1);
		}

		size_type size() const {
			return core.size();
		}

		size_type length() const {
			return core.size();
		}

		void resize(const size_type n, const value_type c = value_type()) ;

		size_type capacity() const {
			return core.capacity();
		}

		void reserve(const size_type c) {
			core.reserve(c);
		}

		void shrink_to_fit() {
			core.shrink_to_fit();
		}

		void clear() { resize(0); }

		bool empty() { return size() == 0; }

		reference operator[](const size_type index) {
			return *(begin() + index);
		}

		const_reference operator[](const size_type index)const {
			return *(begin() + index);
		}

		reference at(const size_type index) {
			return *(begin() + index);
		}

		const_reference at(const size_type index)const {
			return *(begin() + index);
		}

		basic_String& operator+=(const basic_String& other) { return append(other); }

		basic_String& operator+=(const value_type* s) { return append(s); }

		basic_String& operator+=(const value_type c) { push_back(c); return *this; }

		basic_String& operator+=(std::initializer_list<value_type> il) {
			append(il);
			return *this;
		}

		basic_String& append(const basic_String& other) {
			return append(other.data(), other.size());
		}

		basic_String& append(
			const basic_String& other, const size_type pos, const size_type n = npos) {
			return append(other.data() + pos, npos_min(n,other.size()-pos));
		}

		basic_String& append(const value_type* s, size_type n) ;

		basic_String& append(const value_type* s) {
			return append(s, traits_length(s));
		}

		basic_String& append(const size_type n, const value_type c) {
			auto pData = core.expandNoinit(n,true);
			std::fill(pData, pData + n, c);
			return *this;
		}

		basic_String& append(const value_type* s, const value_type* e) {
			return append(s, static_cast<size_type>(e - s));
		}

	private:
		template<typename iter>
		basic_String& append(iter first, iter last,std::true_type) {
			return append((const value_type*)first,(const value_type*)last);
		}

		template<typename iter>
		basic_String& append(iter first, iter last, std::false_type) {
			return append(basic_String(first,last));
		}
	public:


		template<typename iter>
		basic_String& append(iter first, iter last) {
			return append(first,last,is_char_ptr<iter>{});
		}

		basic_String& append(std::initializer_list<value_type> il) {
			return append(il.begin(),il.end());
		}

		void push_back(const value_type c) {
			core.push_back(c);
		}

		void pop_back() {
			assert(!empty());
			core.shrink(1);
		}

		basic_String& assign(const basic_String& other) {
			if (this == &other)
				return *this;
			assign(other.data(), other.size());
		}

		basic_String& assign(basic_String&& other) {
			*this = std::move(other);
			return *this;
		}

		basic_String& assign(const basic_String& other, const size_type pos, const size_type n = npos) {
			return assign(other.data() + pos, npos_min(n,other.size()-pos));
		}

		basic_String& assign(const value_type* s, const size_type n);

		basic_String& assign(const value_type* s, const value_type* e) {
			return assign(s , static_cast<size_type>(e - s));
		}

	private:
		template<typename iter>
		basic_String& assign(iter first, iter last, std::input_iterator_tag) {
			clear();
			for(;first != last;++first)
				push_back(*first);
			return *this;
		}
		
		template<typename iter>
		basic_String& assign(iter first, iter last, std::forward_iterator_tag) {
			resize(static_cast<size_type>(std::distance(first,last)));
			auto _data = data();
			for (; first != last; ++_data,++first)
				*_data = *first;
			return *this;
		}

		template<typename iter>
		basic_String& assign(iter first, iter last, std::true_type) {
			return assign((const value_type*)first,(const value_type*)last);
		}

		template<typename iter>
		basic_String& assign(iter first, iter last, std::false_type) {
			return assign(first, last, typename std::iterator_traits<iter>::iterator_category{});
		}

	public:

		template<typename iter>
		basic_String& assign(iter first, iter last) {
			return assign(first, last, is_char_ptr<iter>{});
		}

		basic_String& assign(const value_type* s) {
			assign(s, traits_length(s));
			return *this;
		}

		basic_String& insert(const size_type pos, const basic_String& str) {
			return insert(pos, str.data(), str.size());
		}

		basic_String& insert(const size_type pos1, 
			const basic_String& s, const size_type pos2, const size_type n = npos) {
			return insert(pos1,s.data() + pos2,npos_min(n,s.size()-pos2));
		}

		basic_String& insert(const size_type pos, const value_type* s, const size_type n);

		basic_String& insert(const size_type pos, const value_type* s, const value_type* e) {
			return insert(pos,s , static_cast<size_type>(e - s));
		}

		basic_String& insert(const size_type pos, const value_type* str) {
			return insert(pos, str, traits_length(str));
		}

		basic_String& insert(const size_type pos, const value_type c) {
			return insert(pos, c, 1);
		}

		basic_String& insert(const size_type pos, const size_type n, const value_type c) ;

	private:
		template<typename iter>
		basic_String& insert(const size_type pos, iter first, iter last, std::true_type) {
			return insert(pos, (const value_type*)first, (const value_type*)last);
		}
		template<typename iter>
		basic_String& insert(const size_type pos, iter first, iter last, std::false_type) {
			return insert(pos, basic_String(first, last));
		}
	public:

		template<typename iter>
		basic_String& insert(const size_type pos, iter first, iter last) {
			return insert(pos, first, last, is_char_ptr<iter>{});
		}

		basic_String& insert(const size_type pos, std::initializer_list<value_type> il) {
			return insert(pos,il.begin(),il.end());
		}

		template<typename T,is_const_iterator<T> = 0>
		iterator insert(T p, const basic_String& str) {
			const size_type pos = p - data();
			insert(pos,str);
			return data() + pos;
		}

		template<typename T,is_const_iterator<T> = 0>
		iterator insert(T p, 
			const basic_String& s, const size_type pos2, const size_type n = npos) {
			const size_type pos1 = p - data();
			insert(pos1,s,pos2, n);
			return data() + pos1;
		}

		template<typename T,is_const_iterator<T> = 0>
		iterator insert(T p, const value_type* s, const size_type n) {
			const size_type pos = p - data();
			insert(pos,s,n);
			return data() + pos;
		}

		template<typename T,is_const_iterator<T> = 0>
		iterator insert(T p, const value_type* s, const value_type* e) {
			const size_type pos = p - data();
			insert(pos,s,e);
			return data() + pos;
		}

		template<typename T,is_const_iterator<T> = 0>
		iterator insert(T p, const value_type* s) {
			const size_type pos = p - data();
			insert(pos,s);
			return data() + pos;
		}

		template<typename T,is_const_iterator<T> = 0>
		iterator insert(T p, const value_type c) {
			const size_type pos = p - data();
			insert(pos, c);
			return data() + pos;
		}

		template<typename T,is_const_iterator<T> = 0>
		iterator insert(T p, const size_type n, const value_type c) {
			const size_type pos = p - data();
			insert(pos, n, c);
			return data() + pos;
		}

		template<typename T,typename iter,is_const_iterator<T> = 0>
		iterator insert(T p, iter first, iter last) {
			const size_type pos = p - data();
			insert(pos,first,last);
			return data() + pos;
		}

		template<typename T,is_const_iterator<T> = 0>
		iterator insert(T p, std::initializer_list<value_type> il) {
			const size_type pos = p - data();
			insert(pos,il);
			return data() + pos;
		}

		basic_String& erase(const size_type pos, size_type n = npos);

		template<typename T,is_const_iterator<T> = 0>
		iterator erase(T p) {
			auto _data = data();
			const size_type pos(p - _data);
			erase(pos, 1);
			return _data + pos;
		}

		template<typename T,is_const_iterator<T> = 0>
		iterator erase(T first, T last) {
			const auto _data = begin();
			const size_type pos(first - _data);
			erase(pos, last - first);
			return _data + pos;
		}

		basic_String& replace(
			const size_type pos, const size_type n,const basic_String& s) {
			return replace(pos,n,s.data(),s.size());
		}

		basic_String& replace(
			const size_type pos1, const size_type n1,
			const basic_String& s, const size_type pos2, const size_type n2 = npos) {
			return replace(pos1,n1,s.data() + pos2,npos_min(n2,s.size() - pos2));
		}

		basic_String& replace(const size_type pos, 
			const size_type n, const value_type* s) {
			return replace(pos,n,s,traits_length(s));
		}

		basic_String& replace(const size_type pos, const size_type n1
			, const value_type* s, const size_type n2);

		template<typename T,is_const_iterator<T> = 0>
		basic_String& replace(T first, T last, const basic_String& s) {
			return replace(first - data(),last - first,s);
		}

		template<typename T,is_const_iterator<T> = 0>
		basic_String& replace(T first, T last, const basic_String& s, 
			const size_type pos,const size_type n = npos) {
			return replace(first - data(),last - first,s,pos,n);
		}

		template<typename T,is_const_iterator<T> = 0>
		basic_String& replace(T first, T last, const value_type* s) {
			return replace(first - data(),last - first,s);
		}

		template<typename T,is_const_iterator<T> = 0>
		basic_String& replace(T first, T last, const value_type* s, const size_type n) {
			return replace(first - data(),last - first,s,n);
		}

		void swap(basic_String& other) { core.swap(other.core); }

		const value_type* c_str() const { return core.c_str(); }

		const value_type* data() const { return core.data(); }

		value_type* data() { return core.data(); }

		bool empty()const {
			return core.empty();
		}

		basic_String substr(const size_type pos, const size_type n = npos)const& {
			return basic_String(data() + pos, npos_min(n,size() - pos));
		}

		basic_String substr(const size_type pos, const size_type n = npos)&& {
			erase(0,pos);
			resize(npos_min(n,size()));
			return std::move(*this);
		}

		basic_String& ltrim() {
			if(empty())
				return *this;
			auto s = begin(),e = s;
			while(*e == ' ')++e;
			erase(s,e);
			return *this;
		}

		basic_String& rtrim() {
			if(empty())
				return *this;
			auto s = end(),e = s;
			while(*(--s) == ' ');
			erase(++s,e);
			return *this;
		}

		basic_String& trim() {
			return ltrim(rtrim());
		}

	private:
		String_core<Char> core;
	};

	template<typename Char,typename Traits>
	size_t basic_String<Char, Traits>::traits_length(const value_type* s) {
		return traits_type::length(s);
	}

	template<typename Char,typename Traits>
	void basic_String<Char,Traits>::resize(const size_type n, const value_type c) {
		const size_type sz = size();

		if (n < sz) {
			core.shrink(sz - n);
		}

		if (n > sz) {
			const auto delta = n - sz;
			auto pData = core.expandNoinit(delta, true);
			std::fill(pData, pData + delta, c);
		}

		// do not do anything when n == size

		assert(size() == n);
	}
	
	template<typename Char,typename Traits>
	basic_String<Char,Traits>& basic_String<Char,Traits>::append(
		const value_type* s, size_type n) {

		const auto oldData = data();
		const auto oldSize = size();
		const auto pData = core.expandNoinit(n, true);

		if (oldData <= s && s < oldData + oldSize) {
			s = (pData - oldSize) + (s - oldData);
			memmove(pData, s, sizeof(value_type) * n);
		}
		else {
			memcpy(pData, s, sizeof(value_type) * n);
		}

		return *this;
	}

	template<typename Char,typename Traits>
	basic_String<Char,Traits>& basic_String<Char,Traits>::assign(
		const value_type* s, const size_type n) {
		if (!n) {
			resize(0);
		}
		else if (size() >= n) {
			std::memmove(core.data(), s, sizeof(value_type) * n);
			core.shrink(size() - n);
		}
		else {
			resize(0);
			// O(n) to copy
			// so don't need to use expGrowth
			std::memmove(core.expandNoinit(n), s, sizeof(value_type) * n);
		}
		return *this;
	}

	template<typename Char,typename Traits>
	basic_String<Char,Traits>& basic_String<Char,Traits>::insert(
		const size_type pos, const value_type* s, const size_type n) {

		if (!n) {
			return *this;
		}

		const auto oldData = data();
		const auto oldSize = size();

		const auto pData = core.expandNoinit(n, true);
		const auto Data = pData - oldSize;

		const auto ins_first = Data + pos;
		const auto ins_last = ins_first + n;

		memmove(ins_last, ins_first, sizeof(value_type) * (oldSize - pos));

		if (oldData <= s && s < oldData + oldSize) {
			s += (Data - oldData);
			size_type left_size;
			if (s + n <= ins_first || s >= ins_last) {
				left_size = n;
			}
			else {
				left_size = ins_first - s;
			}
			memcpy(ins_first, s, sizeof(value_type) * left_size);
			memcpy(ins_first + left_size, s + left_size + n, sizeof(value_type) * (n - left_size));
		}
		else {
			memcpy(ins_first, s, sizeof(value_type) * n);
		}

		return *this;
	}

	template<typename Char,typename Traits>
	basic_String<Char, Traits>& basic_String<Char, Traits>::insert(
		const size_type pos, const size_type n, const value_type c
	) {
		const auto old_size = size();
		const auto data = core.expandNoinit(n,true);
		auto data_first = data - old_size;
		auto data_pos = data_first + pos;
		auto data_end = data;
		memmove(data_pos + n,data_pos,sizeof(value_type) * (old_size - pos));
		std::fill_n(data_pos,n,c);
		return *this;
	}

	template<typename Char,typename Traits>
	basic_String<Char,Traits>& basic_String<Char,Traits>::erase(const size_type pos, size_type n) {
		if (!n) {
			return *this;
		}
		const size_type res = size() - pos;
		n = npos_min(n,res);
		const auto _Data = data();
		memcpy(_Data + pos, _Data + pos + n, sizeof(value_type) * (size() - pos - n));
		core.shrink(n);
		return *this;
	}

	template<typename Char,typename Traits>
	basic_String<Char, Traits>& basic_String<Char, Traits>::replace(
		const size_type pos1, const size_type n1, const value_type* s, const size_type n2) {
		std::copy(s,s + std::min(n1,n2),data() + pos1);
		if (n1 < n2) {
			insert(pos1 + n1,s + n1,n2 - n1);
		}
		else {
			erase(pos1 + n2,n1 - n2);
		}
		return *this;
	}

	//skmp searcher

	template<typename RanItPat,typename Pred_eq = std::equal_to<>> 
	class skmp_searcher_fshift_builder {
	private:
		using value_t = typename std::iterator_traits<RanItPat>::value_type;
		using diff_t = typename std::iterator_traits<RanItPat>::difference_type;

	public:

		using value_type = value_t;
		using difference_type = diff_t;
		using allocator_type = mallocator<diff_t>;

		skmp_searcher_fshift_builder(
			const RanItPat First, const RanItPat Last, const Pred_eq Eq
		) : first(First), last(Last) {
			size = std::distance(first,last);
			auto al = mallocator<diff_t>();
			fshift = al.allocate(size + 1);
			fm = al.allocate(size + 1);

			diff_t* fnxt = al.allocate(size + 1);
			diff_t i = 0,j = -1;
			fnxt[0] = -1;
			while (i < size) {
				if (j == -1 || Eq(*(last - 1 - i), *(last - 1 - j))) {
					++i;
					++j;
				}else j = fnxt[j];
			}
			std::fill_n(fshift,size + 1,static_cast<diff_t>(-1));
			for (i = 1; i <= size; ++i) {
				while(i < size && fnxt[i + 1] == fnxt[i] + 1)
					++i;
				j = i;
				while (fnxt[j] != -1 && fshift[fnxt[j]] == -1) {
					fshift[fnxt[j]] = i - fnxt[j];
					j = fnxt[j];
				}
			}

			std::fill_n(fm,size + 1,size);

			i = size;

			while (fnxt[i] != -1) {
				fm[fnxt[i]] = size - fnxt[i];
				i = fnxt[i];
			}

			for(i = 1;i <= size;++i)
				fm[i] = fm[i] < fm[i-1] ? fm[i] : fm[i-1];

			for(i = 0;i<=size;++i)
				fshift[i] = ~fshift[i] ? fshift[i] + i : 0;

			al.deallocate(fnxt,size + 1);
		}

		skmp_searcher_fshift_builder(const skmp_searcher_fshift_builder& other)
			: size(other.size),first(other.first),last(other.last) {
			auto al = mallocator<diff_t>();
			fshift = al.allocate(size + 1);
			fm = al.allocate(size + 1);
			std::copy(other.fshift,other.fshift + size + 1,fshift);
			std::copy(other.fm,other.fm + size + 1,fm);
		}

		skmp_searcher_fshift_builder(skmp_searcher_fshift_builder&& other)noexcept
			: fshift(other.fshift),fm(other.fm),size(other.size),
			first(other.first),last(other.last) {
			other.fshift = other.fm	= nullptr ;
		}

		~skmp_searcher_fshift_builder() {
			auto al = mallocator<diff_t>();
			if (fshift) {
				al.deallocate(fshift,size + 1);
				al.deallocate(fm,size + 1);
			}
		}

		RanItPat get_first()const {
			return first;
		}

		RanItPat get_last()const {
			return last;
		}

		diff_t* get_fshift()const {
			return fshift;
		}

		diff_t* get_fm()const {
			return fm;
		}

		diff_t get_size()const {
			return size;
		}

	private:
		diff_t *fshift,*fm,size;
		RanItPat first,last;
	};

	template<typename RanItPat,typename Hash_ty = 
		std::hash<typename std::iterator_traits<RanItPat>::value_type>,class Pred_eq = std::equal_to<>>
	class skmp_searcher_char_builder 
		: public skmp_searcher_fshift_builder<RanItPat,Pred_eq> {
	private:

		using Base = skmp_searcher_fshift_builder<RanItPat,Pred_eq>;
		using value_t = typename Base::value_type;
		using diff_t = typename Base::difference_type;

	public:
		using value_type = value_t;
		using difference_type = diff_t;

		skmp_searcher_char_builder(
			RanItPat First, RanItPat Last, Hash_ty fn = Hash_ty(), Pred_eq Eq = Pred_eq()
		) : Base(First, Last, Eq) {
			auto first = Base::get_first();
			const auto last = Base::get_last();
			const auto size = Base::get_size();
			shift = mallocator<diff_t>().allocate(256);
			std::fill_n(shift,256,size + 1);
			diff_t i = size;
			while (first != last) {
				const auto u_value = static_cast<std::make_unsigned_t<value_t>>(*first);
				shift[u_value] = i;
				++first;
				--i;
			}
		}

		skmp_searcher_char_builder(const skmp_searcher_char_builder& other)
			: Base(other) {
			shift = mallocator<diff_t>().allocate(256);
			std::copy(other.shift,other.shift + 256,shift);
		}

		skmp_searcher_char_builder(skmp_searcher_char_builder&& other)noexcept
			: Base(std::move(other)), shift(other.shift) {
			other.shift = nullptr;
		}

		~skmp_searcher_char_builder() {
			if (shift != nullptr) {
				mallocator<diff_t>().deallocate(shift,256);
			}
		}

		diff_t get_shift(const value_t value)const {
			const auto u_value = static_cast<std::make_unsigned_t<value_t>>(value);
			return shift[u_value];
		}

		std::equal_to<> key_eq()const {
			return {};
		}

	private:
		diff_t* shift;
	};

	template<typename RanItPat,typename Hash_ty = 
		std::hash<typename std::iterator_traits<RanItPat>::value_type>,
		class Pred_eq = std::equal_to<>>
	class skmp_searcher_general_builder 
		: public skmp_searcher_fshift_builder<RanItPat,Pred_eq>{
	private:
		using Base = skmp_searcher_fshift_builder<RanItPat,Pred_eq>;
		using value_t = typename Base::value_type;
		using diff_t = typename Base::difference_type;

	public:
		using value_type = value_t;
		using difference_type = diff_t;

		skmp_searcher_general_builder(
			RanItPat First, RanItPat Last, Hash_ty fn = Hash_ty(), Pred_eq Eq = Pred_eq()
		) : Base(First, Last, Eq) {
			auto first = Base::get_first();
			const auto last = Base::get_last();
			const auto size = Base::get_size();
			diff_t i = size;
			while (first != last) {
				Map.insert_or_assign(*first,i);
				++first;
				--i;
			}
		}

		skmp_searcher_general_builder(const skmp_searcher_general_builder& other)
			: Base(other), Map(other.Map) {

		}

		skmp_searcher_general_builder(skmp_searcher_general_builder&& other)noexcept
			: Base(std::move(other)), Map(std::move(other.Map)) {

		}

		diff_t get_shift(const value_t& value)const {
			const auto iter = Map.find(value);
			if (iter == Map.end()) {
				return Base::get_size() + 1;
			}
			return iter->second;
		}

		Pred_eq key_eq()const {
			return Map.key_eq();
		}

	private:
		std::unordered_map<value_t,diff_t,Hash_ty,Pred_eq> Map;
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

			template<class _Container, HAS_BEGIN_END<const _Container&> = 0>
			skmp_searcher(
				const _Container& _Cont, _Hash_ty _Hash_Fn = _Hash_ty(), _Pred_eq _Eq = _Pred_eq()
			) : _Searcher(_Cont.cbegin(), _Cont.cend(), _Hash_Fn, _Eq) {
				// containers that have begin() and end() 
			}

			skmp_searcher(const _RanItPat _First, const size_t _Length, _Hash_ty _Hash_fn = _Hash_ty(), _Pred_eq _Eq = _Pred_eq())
				: _Searcher(_First, _First + _Length, _Hash_fn, _Eq) {

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

	template <class _RanItPat, class _Hash_ty, class _Pred_eq>
	template <class _RanItHaystack>
	std::pair<_RanItHaystack, _RanItHaystack> skmp_searcher<_RanItPat, _Hash_ty, _Pred_eq>::operator()(
		_RanItHaystack _First, _RanItHaystack _Last)const {

		const auto _Size = _Searcher.get_size(); // pattern string 's size

		if (_Size == 0) {
			return { _First,_First };
		}

		const auto pattern_first = _Searcher.get_first();
		const auto pattern_last = _Searcher.get_last();
		const auto pattern_end = pattern_last - 1;

		const auto text_first = _First;
		const auto text_last = _Last;
		const auto text_end = text_last - 1;

		const auto _Eq = _Searcher.key_eq();

		const auto* _Fshift = _Searcher.get_fshift();
		const auto* _Fm = _Searcher.get_fm();

		auto ch = *pattern_end;

		auto text_ptr = text_first + _Size - 1;
		//sufsearch : last jump distance
		//alsearch  : last match length
		_Diff_t sufsearch = 0, alsearch = 0;
		size_t remain_size = text_end - text_ptr;
		for (;;) {

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
			const auto sh = _Searcher.get_shift(*(text_ptr + 1));
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

			if (sufsearch > remain_size)
				break;

			remain_size -= sufsearch;
			text_ptr += sufsearch;

			if constexpr (std::is_integral_v<_Value_t> && sizeof(_Value_t) == 1
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

	template<typename _Container, typename _Hash, typename _Pred,
		HAS_BEGIN_END<const _Container&> = 0>
		skmp_searcher(const _Container&, _Hash, _Pred)
		->skmp_searcher<container_iterator_type<const _Container&>, _Hash, _Pred>;

	template<typename _Container, typename _Hash, HAS_BEGIN_END<const _Container&> = 0>
	skmp_searcher(const _Container&, _Hash)
		->skmp_searcher<container_iterator_type<const _Container&>, _Hash>;

	template<typename _Container,
		HAS_BEGIN_END<const _Container&> = 0>
		skmp_searcher(const _Container&)->skmp_searcher<container_iterator_type<const _Container&>>;

	//

	typedef basic_String<char,std::char_traits<char>> String;
}

#endif