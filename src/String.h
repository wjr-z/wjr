#ifndef WJR_STRING_H
#define WJR_STRING_H

#include <iostream>
#include <string.h>
#include <cassert>
#include <string>
#include <iterator>

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

		void reserveSmall(const size_t s) {
			if (s <= maxSmallSize)return;
			const auto _New_Data = Getal().allocate(s + 1);
			const auto _Old_Size = SmallSize();
			memcpy(_New_Data, _Small, sizeof(Char) * _Old_Size);
			_Ml._Data = _New_Data;
			setMediumSize(_Old_Size);
			setCapacity(s);
		}

		void reserveMedium(const size_t s) {
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

	template<typename Char,typename Traits = std::char_traits<Char>>
	class basic_String {
	private:

		using _Elem = Char;
		template <class iter>
		using is_char_ptr = 
			std::bool_constant<is_any_of_v<iter,const _Elem*const,const _Elem*,_Elem*const,_Elem*>>;

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

	private:
		static size_t traits_length(const value_type*s);

	public:

		basic_String()noexcept {}

		basic_String(const basic_String& other)
			:core(other.core) {

		}

		basic_String(basic_String&& other)
			:core(std::move(other.core)) {

		}

		basic_String(const basic_String& other, const size_type pos, const size_type n) {
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
			if constexpr (is_char_ptr<iter>::value) {
				assign((const value_type*)first,(const value_type*)last);
			}
			else {
				assign(first, last);
			}
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

		iterator begin() { return core.data(); }

		const_iterator begin()const { return core.data(); }

		const_iterator cbegin()const { return begin(); }

		iterator end() { return core.data() + core.size(); }

		const_iterator end()const { return core.data() + core.size(); }

		const_iterator cend()const { return end(); }

		reverse_iterator rbegin(){return end(); }

		const_reverse_iterator rbegin()const{return end(); }

		const_reverse_iterator crbegin()const{return end(); }

		reverse_iterator rend(){return begin(); }

		const_reverse_iterator rend()const{return begin(); }

		const_reverse_iterator crend()const{return begin(); }

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

		void pop_back() {
			assert(!empty());
			core.shrink(1);
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
			const basic_String& other, const size_type pos, const size_type n) {
			return append(other.data() + pos, std::min(other.size() - pos,n));
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

		template<typename iter>
		basic_String& append(iter first, iter last) {
			if constexpr (is_char_ptr<iter>::value) {
				return append((const value_type*)first,(const value_type*)last);
			}
			return append(basic_String(first,last));
		}

		basic_String& append(std::initializer_list<value_type> il) {
			return append(il.begin(),il.end());
		}

		void push_back(const value_type c) {
			core.push_back(c);
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

		basic_String& assign(const basic_String& other, const size_type pos, const size_type n) {
			return assign(other.data() + pos, std::min(other.size() - pos,n));
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

	public:

		template<typename iter>
		basic_String& assign(iter first, iter last) {
			if constexpr (is_char_ptr<iter>::value) {
				return assign((const value_type*)first,(const value_type*)last);
			}
			else {
				return assign(first, last, typename std::iterator_traits<iter>::iterator_category{});
			}
		}

		basic_String& assign(const value_type* s) {
			assign(s, traits_length(s));
			return *this;
		}

		basic_String& insert(const size_type pos, const basic_String& str) {
			return insert(pos, str.data(), str.size());
		}

		basic_String& insert(const size_type pos1, 
			const basic_String& str, const size_type pos2, const size_type n) {
			return insert(pos1,str.data() + pos2,std::min(str.size() - pos2,n));
		}

		basic_String& insert(const size_type pos, const value_type* s, const size_type n) ;

		basic_String& insert(const size_type pos, const value_type* s, const value_type* e) {
			return insert(pos,s , static_cast<size_type>(e - s));
		}

		basic_String& insert(const size_type pos, const value_type* str) {
			return insert(pos, str, traits_length(str));
		}

		basic_String& insert(const size_type pos, const value_type c) {
			return insert(pos, &c, 1);
		}

		template<typename iter>
		basic_String& insert(const size_type pos, iter first, iter last) {
			if constexpr (is_char_ptr<iter>::value) {
				return insert(pos,(const value_type*)first,(const value_type*)last);
			}
			else {
				basic_String temp(first, last);
				return insert(pos, temp.begin(), temp.size());
			}
		}

		basic_String& insert(const size_type pos, std::initializer_list<value_type> il) {
			return insert(pos,il.begin(),il.end());
		}

		iterator insert(const_iterator p, const basic_String& str) {
			const size_type pos = p - data();
			insert(pos,str);
			return data() + pos;
		}

		iterator insert(const_iterator p, 
			const basic_String& str, const size_type pos2, const size_type n) {
			const size_type pos1 = p - data();
			insert(pos1,str,pos2,n);
			return data() + pos1;
		}

		iterator insert(const_iterator p, const value_type* s, const size_type n) {
			const size_type pos = p - data();
			insert(pos,s,n);
			return data() + pos;
		}

		iterator insert(const_iterator p, const value_type* s, const value_type* e) {
			const size_type pos = p - data();
			insert(pos,s,e);
			return data() + pos;
		}

		iterator insert(const_iterator p, const value_type* s) {
			const size_type pos = p - data();
			insert(pos,s);
			return data() + pos;
		}

		iterator insert(const_iterator p, const value_type c) {
			const size_type pos = p - data();
			insert(pos, c);
			return data() + pos;
		}

		iterator insert(const_iterator p, const size_type n, const value_type c) {
			const size_type pos = p - data();
			insert(pos, n, c);
			return data() + pos;
		}

		template<typename iter>
		iterator insert(const_iterator p, iter first, iter last) {
			const size_type pos = p - data();
			insert(pos,first,last);
			return data() + pos;
		}

		iterator insert(const_iterator p, std::initializer_list<value_type> il) {
			const size_type pos = p - data();
			insert(pos,il);
			return data() + pos;
		}

		basic_String& erase(const size_type pos, const size_type n) {
			if (!n) {
				return *this;
			}
			const auto _Data = data();
			memcpy(_Data + pos, _Data + pos + n, sizeof(value_type) * (size() - pos - n));
			core.shrink(n);
			return *this;
		}

		iterator erase(const_iterator pos) {
			const auto _Data = begin();
			const size_type pos(pos - _Data);
			erase(pos, 1);
			return _Data + pos;
		}

		iterator erase(const_iterator first, const_iterator last) {
			const auto _Data = begin();
			const size_type pos(first - _Data);
			erase(pos, last - first);
			return _Data + pos;
		}

		void swap(basic_String& other) { core.swap(other.core); }

		const value_type* c_str() const { return core.c_str(); }

		const value_type* data() const { return core.data(); }

		value_type* data() { return core.data(); }

		bool empty()const {
			return core.empty();
		}

		basic_String substr(const size_type pos, const size_type n) {
			return basic_String(data() + pos, std::min(n, size() - pos));
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

		// dont't do anything when n == size

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

	typedef basic_String<char,std::char_traits<char>> String;
}

#endif