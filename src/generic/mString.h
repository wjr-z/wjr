#ifndef __WJR_MSTRING_H
#define __WJR_MSTRING_H

#define TEST_SHARED_STRING

#include <atomic>
#include <codecvt>
#include <iostream>
#include <iterator>
#include <string.h>
#include <string_view>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

#include "mallocator.h"

bool fill_double(double v, char* buffer);

namespace wjr {
	
	// skmp-searcher for String such as find,rfind
	template<typename RanItPat, typename Traits>
	class skmp_searcher_fshift_builder {
	private:
		using value_t = typename std::iterator_traits<RanItPat>::value_type;
		using diff_t  = typename std::iterator_traits<RanItPat>::difference_type;

	public:

		using value_type      = value_t;
		using difference_type = diff_t;
		using allocator_type  = mallocator<diff_t>;

		skmp_searcher_fshift_builder(const RanItPat First, const RanItPat Last);

		skmp_searcher_fshift_builder(const skmp_searcher_fshift_builder& other);

		skmp_searcher_fshift_builder(skmp_searcher_fshift_builder&& other)noexcept;

		~skmp_searcher_fshift_builder();

		skmp_searcher_fshift_builder& operator=(const skmp_searcher_fshift_builder&) = delete;
		
		skmp_searcher_fshift_builder& operator=(skmp_searcher_fshift_builder&&)noexcept = delete;
		
		RanItPat get_first()const { return first; }
		RanItPat get_last()const { return last; }

		diff_t* get_fshift()const { return fshift; }
		diff_t* get_fm()const { return fm; }
		diff_t get_size()const { return size; }

	private:
		diff_t* fshift;
		diff_t* fm;
		diff_t size;
		RanItPat first;
		RanItPat last;
	};

	template<typename RanItPat, typename Traits>
	skmp_searcher_fshift_builder<RanItPat, Traits>::skmp_searcher_fshift_builder(
		const RanItPat First, const RanItPat Last
	) : first(First), last(Last) {
		size = last - first;
		auto al = mallocator<diff_t>();
		fshift = al.allocate(2 * size);
		fm = fshift + size;
		diff_t* fnxt = al.allocate(size + 1);
		diff_t i = 0, j = -1;
		fnxt[0] = -1;
		while (i < size) {
			if (j == -1 || Traits::eq(*(last - 1 - i), *(last - 1 - j))) {
				fnxt[++i] = ++j;
			}
			else j = fnxt[j];
		}

		std::fill_n(fshift, size, static_cast<diff_t>(-1));

		for (i = 1; i < size; ++i) {
			while (i < size && fnxt[i + 1] == fnxt[i] + 1)
				++i;
			j = i;
			while (fnxt[j] != -1 && fshift[fnxt[j]] == -1) {
				fshift[fnxt[j]] = i - fnxt[j];
				j = fnxt[j];
			}
		}

		std::fill_n(fm, size, size);

		i = size;

		while (fnxt[i] != -1) {
			fm[fnxt[i]] = size - fnxt[i];
			i = fnxt[i];
		}

		for (i = 1; i < size; ++i)
			fm[i] = fm[i] < fm[i - 1] ? fm[i] : fm[i - 1];

		for (i = 0; i < size; ++i)
			fshift[i] = ~fshift[i] ? fshift[i] : size;

		al.deallocate(fnxt, size + 1);
	}

	template<typename RanItPat, typename Traits>
	skmp_searcher_fshift_builder<RanItPat, Traits>::
		skmp_searcher_fshift_builder(const skmp_searcher_fshift_builder& other)
		: size(other.size), first(other.first), last(other.last) {
		auto al = mallocator<diff_t>();
		fshift = al.allocate(2 * size);
		fm = fshift + size;
		std::copy(other.fshift, other.fshift + size, fshift);
		std::copy(other.fm, other.fm + size, fm);
	}

	template<typename RanItPat, typename Traits>
	skmp_searcher_fshift_builder<RanItPat, Traits>::
		skmp_searcher_fshift_builder(skmp_searcher_fshift_builder&& other)noexcept
		: fshift(other.fshift), fm(other.fm), size(other.size),
		first(other.first), last(other.last) {
		other.fshift = other.fm = nullptr;
	}

	template<typename RanItPat, typename Traits>
	skmp_searcher_fshift_builder<RanItPat, Traits>::
		~skmp_searcher_fshift_builder() {
		if (fshift != nullptr) {
			mallocator<diff_t>().deallocate(fshift, 2 * size);
		}
	}

	template<typename Traits>
	struct base_traits_char_map {
		using char_type        = typename Traits::char_type;
		using is_default_equal = std::false_type;
		constexpr static decltype(auto) to(const char_type& value) {
			return static_cast<std::make_unsigned_t<char_type>>(value);
		}
	};

	template<typename Traits>
	struct traits_char_map : base_traits_char_map<Traits> {
	};

	template<typename Char>
	struct traits_char_map<std::char_traits<Char>>
		: base_traits_char_map<std::char_traits<Char>> {
		using is_default_equal = std::true_type;
	};

	template<typename RanItPat, typename Traits>
	class skmp_searcher_char_builder
		: public skmp_searcher_fshift_builder<RanItPat, Traits> {
	private:

		using Base     = skmp_searcher_fshift_builder<RanItPat, Traits>;
		using value_t  = typename Base::value_type;
		using diff_t   = typename Base::difference_type;
		using char_map = traits_char_map<Traits>;

	public:
		using value_type = value_t;
		using difference_type = diff_t;

		skmp_searcher_char_builder(
			RanItPat First, RanItPat Last
		);

		skmp_searcher_char_builder(const skmp_searcher_char_builder& other)
			: Base(other), shift(mallocator<diff_t>().allocate(256)) {
			std::copy(other.shift, other.shift + 256, shift);
		}

		skmp_searcher_char_builder(skmp_searcher_char_builder&& other)noexcept
			: Base(std::move(other)), shift(other.shift) {
			other.shift = nullptr;
		}

		~skmp_searcher_char_builder() {
			if (shift != nullptr) {
				mallocator<diff_t>().deallocate(shift, 256);
			}
		}

		skmp_searcher_char_builder& operator=(const skmp_searcher_char_builder&) = delete;

		skmp_searcher_char_builder& operator=(skmp_searcher_char_builder&&) noexcept = delete;

		diff_t get_shift(const value_t value)const {
			return shift[char_map::to(value)];
		}

	private:
		diff_t* shift;
	};

	template<typename RanItPat, typename Traits>
	skmp_searcher_char_builder<RanItPat, Traits>::skmp_searcher_char_builder(
		RanItPat First, RanItPat Last
	) : Base(First, Last) ,shift(mallocator<diff_t>().allocate(256)) {
		auto first = Base::get_first();
		const auto last = Base::get_last();
		const auto size = Base::get_size();
		std::fill_n(shift, 256, size + 1);
		diff_t i = size;
		while (first != last) {
			shift[char_map::to(*first)] = i;
			++first;
			--i;
		}
	}

	template<typename Traits>
	struct traits_equal {
		using char_type = typename Traits::char_type;
		constexpr bool operator()(const char_type& l, const char_type& r)const noexcept {
			return Traits::eq(l, r);
		}
	};

	template<typename RanItPat, typename Traits>
	class skmp_searcher_general_builder
		: public skmp_searcher_fshift_builder<RanItPat, Traits> {
	private:
		using Base    = skmp_searcher_fshift_builder<RanItPat, Traits>;
		using value_t = typename Base::value_type;
		using diff_t  = typename Base::difference_type;

	public:
		using value_type = value_t;
		using difference_type = diff_t;

		skmp_searcher_general_builder(
			RanItPat First, RanItPat Last
		);

		skmp_searcher_general_builder(const skmp_searcher_general_builder&) = default;

		skmp_searcher_general_builder(skmp_searcher_general_builder&&) noexcept = default;

		~skmp_searcher_general_builder() = default;

		skmp_searcher_general_builder& operator=(const skmp_searcher_general_builder&) = delete;
		
		skmp_searcher_general_builder& operator=(skmp_searcher_general_builder&&) noexcept = delete;

		diff_t get_shift(const value_t& value)const {
			const auto iter = Map.find(value);
			if (iter == Map.end()) {
				return Base::get_size() + 1;
			}
			return iter->second;
		}

	private:
		std::unordered_map<value_t, diff_t,
			std::hash<typename Traits::char_type>, traits_equal<Traits>,
			mallocator<std::pair<const value_t,diff_t>>> Map;
	};

	template<typename RanItPat, typename Traits>
	skmp_searcher_general_builder<RanItPat, Traits>::skmp_searcher_general_builder(
		RanItPat First, RanItPat Last
	) : Base(First, Last) {
		auto first = Base::get_first();
		const auto last = Base::get_last();
		const auto size = Base::get_size();
		diff_t i = size;
		while (first != last) {
			Map.insert_or_assign(*first, i);
			++first;
			--i;
		}
	}

	template<typename value_type, typename Traits>
	struct can_make_bit_map : std::conditional_t<
		std::is_integral_v<value_type> && sizeof(value_type) == 1 &&
		traits_char_map<Traits>::is_default_equal::value, std::true_type, std::false_type> { };
	
	template<typename Char,typename Traits>
	constexpr static size_t is_simple_skmp_searcher_v = can_make_bit_map<Char, Traits>::value;
	
	template<typename RanItPat, typename Traits, typename value_t =
		typename Traits::char_type>
	using skmp_searcher_traits = std::conditional_t<is_simple_skmp_searcher_v<value_t,Traits>,
		skmp_searcher_char_builder <RanItPat, Traits>,
		skmp_searcher_general_builder<RanItPat, Traits>>;

	template<typename RanItPat, typename Traits>
	class skmp_searcher {
	private:
		using value_t = typename std::iterator_traits<RanItPat>::value_type;
		using diff_t  = typename std::iterator_traits<RanItPat>::difference_type;
	public:
		using value_type      = value_t;
		using difference_type = diff_t;
		constexpr static size_t is_simple_searcher_v = is_simple_skmp_searcher_v<value_t, Traits>;
		constexpr static size_t small_search_size = is_simple_searcher_v ? 8 : 16;

		skmp_searcher(
			const RanItPat First, const RanItPat Last
		) : Searcher(First, Last) {
		}

		skmp_searcher(
			const RanItPat First, const size_t length
		) : Searcher(First, First + length) {
		}

		skmp_searcher(const skmp_searcher&) = default;

		skmp_searcher(skmp_searcher&&) noexcept = default;

		~skmp_searcher() = default;

		skmp_searcher& operator=(const skmp_searcher&) = delete;

		template<typename iter>
		std::pair<iter, iter> operator()(iter First, iter Last)const;
		
		RanItPat data() {
			return Searcher.get_first();
		}
		
		const RanItPat data() const { return Searcher.get_first(); }
		
		size_t size()const { return Searcher.get_last() - Searcher.get_first(); }

	private:

		skmp_searcher_traits<RanItPat, Traits> Searcher;
	};

	template<typename RanItPat, typename Traits>
	template<typename iter>
	std::pair<iter, iter> skmp_searcher<RanItPat, Traits>::operator()(
		iter First, iter Last
		)const {
		const auto size = Searcher.get_size();

		if (unlikely(size == 0))  {
			return { First,First };
		}

		if (unlikely(Last - First < size))  {
			return { Last,Last };
		}

		const auto pattern_first = Searcher.get_first();
		const auto pattern_back = pattern_first + (size - 1);
		const auto& ch = *pattern_back;

		auto text_ptr = First + (size - 1);
		const auto text_back = Last - 1;

		const auto Eq = traits_equal<Traits>{};
		const auto fshift = Searcher.get_fshift();
		const auto fm = Searcher.get_fm();

		diff_t sufsearch = size, alsearch = 0;
		diff_t res = text_back - text_ptr;

		while(true) {
			if (!Eq(*text_ptr, ch)) {
				// find at least one match
				do {
					if (unlikely(res == 0))  {
						return { Last,Last };
					}
					const auto def = Searcher.get_shift(*(text_ptr + 1));
					if (unlikely(res < def))  {
						return { Last,Last };
					}
					text_ptr += def;
					res -= def;
				} while (!Eq(*text_ptr, ch));
				sufsearch = size;
				alsearch = 0;
			}

			auto s_text_ptr      = text_ptr;
			auto s_pattern_ptr   = pattern_back;
			const auto s_ptr_mid = text_ptr - sufsearch;

			while (s_text_ptr != s_ptr_mid && Eq(*s_text_ptr, *s_pattern_ptr)) {
				--s_text_ptr;
				--s_pattern_ptr;
			}

			if (s_text_ptr == s_ptr_mid) {
				if (alsearch + sufsearch != size) {
					s_text_ptr -= alsearch;
					s_pattern_ptr -= alsearch;

					while (s_pattern_ptr != pattern_first && Eq(*s_text_ptr, *s_pattern_ptr)) {
						--s_text_ptr;
						--s_pattern_ptr;
					}

					if (s_pattern_ptr == pattern_first && Eq(*s_text_ptr, *s_pattern_ptr)) {
						++text_ptr;
						return { text_ptr - size,text_ptr };
					}
				}
				else {
					++text_ptr;
					return { text_ptr - size,text_ptr };
				}
			}

			if (unlikely(text_ptr == text_back))  {
				break;
			}

			diff_t Match = text_ptr - s_text_ptr;
			const auto Fs = fshift[Match];
			const auto Sh = Searcher.get_shift(*(text_ptr + 1));

			if (Fs <= Sh) {
				sufsearch = Sh;
				alsearch = 0;
			}
			else {
				if (Fs != size) {
					sufsearch = Fs;
					alsearch = Match;
				}
				else {
					sufsearch = fm[Match];
					alsearch = size - sufsearch;
				}
			}

			if (unlikely(sufsearch > res)) 
				break;

			text_ptr += sufsearch;
			res -= sufsearch;

			if (sufsearch > size) {
				sufsearch = size;
			}
		}

		return { Last,Last };
	}

	template<typename Char>
	class String_core {
		static_assert(std::is_default_constructible_v<Char>,
			"Corrupt Char type for String.");
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

		void swap(String_core& other) noexcept {
			const auto t = _Ml;
			_Ml = other._Ml;
			other._Ml = t;
		}

		Char* data() {
			return category() ? _Ml._Data : _Small;
		}

		const Char* data()const {
			return category() ? _Ml._Data : _Small;
		}

		Char* mutableData() {
			return data();
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

		void push_back(Char c) { *expandNoinit(1, true) = c; }

		size_t size()const {
			return category() ? MediumSize() : SmallSize();
		}

		void setSize(const size_t s) {
			if (category()) {
				setMediumSize(s);
			}
			else {
				setSmallSize(s);
			}
		}

		size_t capacity()const {
			return category() ? _Ml.capacity() : maxSmallSize;
		}

		void shrink_to_fit();

		static mallocator<Char>& Getal() {
			static mallocator<Char> _alloc;
			return _alloc;
		}

		constexpr static size_t max_small_size() {
			return maxSmallSize;
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
				if constexpr (is_little_endian) {
					return _Capacity & capacityExtractMask;
				}
				else {
					return _Capacity >> 1;
				}
			}
		};

		constexpr static size_t lastChar = sizeof(_Medium) - 1;
		constexpr static size_t maxSmallSize = lastChar / sizeof(Char);
		constexpr static uint8_t categoryExtractMask = is_little_endian ? 0x80 : 0x01;
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
			return (_Byte[lastChar] & categoryExtractMask) != 0;
		}

		void setCapacity(const size_t c) {
			if constexpr (is_little_endian) {
				_Ml._Capacity = c | capacityOr;
			}
			else {
				_Ml._Capacity = c << 1 | 1;
			}
		}

		void reset() { setSmallSize(0); }

		void setSmallSize(const size_t s) {
			WASSERT_LEVEL_2(s <= maxSmallSize);
			if constexpr (is_little_endian) {
				_Byte[lastChar] = static_cast<uint8_t>(maxSmallSize - s);
			}
			else {
				_Byte[lastChar] = static_cast<uint8_t>((maxSmallSize - s) << 1);
			}
			_Small[s] = static_cast<Char>('\0');
		}

		void setMediumSize(const size_t s) {
			_Ml._Size = s;
			_Ml._Data[s] = static_cast<Char>('\0');
		}

		size_t SmallSize()const {
			if constexpr (is_little_endian) {
				return maxSmallSize - static_cast<size_t>(_Byte[lastChar]);
			}
			else {
				return maxSmallSize - static_cast<size_t>(_Byte[lastChar] >> 1);
			}
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

		void reserveSmall(const size_t s);

		void reserveMedium(const size_t s);
	};

	template<typename Char>
	Char* String_core<Char>::expandNoinit(const size_t n, bool expGrowth) {
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
		if (!category()) {
			return;
		}
		const size_t _size = _Ml._Size;
		const size_t _capacity = _Ml.capacity();
		if (_capacity < _size * 9 / 8) {
			return;
		}
		String_core<Char>(data(), _size).swap(*this);
	}

	template<typename Char>
	void String_core<Char>::reserveSmall(const size_t s) {
		if (s <= maxSmallSize)return;
		const size_t new_size = std::max(s, 2 * maxSmallSize);
		const auto _New_Data = Getal().allocate(new_size + 1);
		const auto _Old_Size = SmallSize();
		memcpy(_New_Data, _Small, sizeof(Char) * _Old_Size);
		_Ml._Data = _New_Data;
		setMediumSize(_Old_Size);
		setCapacity(new_size);
	}

	template<typename Char>
	void String_core<Char>::reserveMedium(const size_t s) {
		const auto _Old_Capacity = _Ml.capacity();
		if (s <= _Old_Capacity)
			return;

		const size_t new_size = std::max(s, 1 + _Old_Capacity + _Old_Capacity / 2);
		auto Al = Getal();
		const auto _New_Data = Al.allocate(new_size + 1);
		const auto _Old_Size = _Ml._Size;
		memcpy(_New_Data, _Ml._Data, sizeof(Char) * _Old_Size);
		Al.deallocate(_Ml._Data, _Old_Capacity + 1);
		_Ml._Data = _New_Data;
		setMediumSize(_Old_Size);
		setCapacity(new_size);
	}

#ifdef TEST_SHARED_STRING
	template <typename Pod>
	inline void pod_copy(const Pod* b, const Pod* e, Pod* d) {
		WASSERT_LEVEL_2(b != nullptr);
		WASSERT_LEVEL_2(e != nullptr);
		WASSERT_LEVEL_2(d != nullptr);
		WASSERT_LEVEL_2(b <= e);
		WASSERT_LEVEL_2(d >= e || d + (e - b) <= b);
		memcpy(d, b, (e - b) * sizeof(Pod));
	}

	static void* smart_realloc(void* data, size_t size, size_t capacity, size_t new_size) {
		auto new_data = (void*)mallocator<char>().allocate(new_size);
		memcpy(new_data, data, size);
		mallocator<char>().deallocate((char*)data, capacity);
		return new_data;
	}

	//the code from facebook/folly/FBString.h
	template <class Char>
	class shared_String_core {
	public:
		shared_String_core() noexcept {
			reset();
		}

		shared_String_core(const shared_String_core& rhs) {
			WASSERT_LEVEL_2(this != std::addressof(rhs));
			switch (rhs.category()) {
			case Category::isSmall:
				copySmall(rhs);
				break;
			case Category::isMedium:
				copyMedium(rhs);
				break;
			case Category::isLarge:
				copyLarge(rhs);
				break;
			}
			WASSERT_LEVEL_2(size() == rhs.size());
		}

		shared_String_core& operator=(const shared_String_core& rhs) = delete;

		shared_String_core(shared_String_core&& goner) noexcept {
			// Take goner's guts
			ml_ = goner.ml_;
			// Clean goner's carcass
			goner.reset();
		}

		shared_String_core(
			const Char* const data,
			const size_t size) {
			if (size <= maxSmallSize) {
				initSmall(data, size);
			}
			else if (size <= maxMediumSize) {
				initMedium(data, size);
			}
			else {
				initLarge(data, size);
			}
			assert(this->size() == size);
		}

		~shared_String_core() noexcept {
			if (category() == Category::isSmall) {
				return;
			}
			destroyMediumLarge();
		}

		// swap below doesn't test whether &rhs == this (and instead
		// potentially does extra work) on the premise that the rarity of
		// that situation actually makes the check more expensive than is
		// worth.
		void swap(shared_String_core& rhs)noexcept {
			auto const t = ml_;
			ml_ = rhs.ml_;
			rhs.ml_ = t;
		}

		// In C++11 data() and c_str() are 100% equivalent.
		const Char* data() const { return c_str(); }

		Char* data() { return c_str(); }

		Char* mutableData() {
			switch (category()) {
			case Category::isSmall:
				return small_;
			case Category::isMedium:
				return ml_.data_;
			case Category::isLarge:
				return mutableDataLarge();
			}
		}

		const Char* c_str() const {
			const Char* ptr = ml_.data_;
			// With this syntax, GCC and Clang generate a CMOV instead of a branch.
			ptr = (category() == Category::isSmall) ? small_ : ptr;
			return ptr;
		}

		void shrink(const size_t delta) {
			if (category() == Category::isSmall) {
				shrinkSmall(delta);
			}
			else if (
				category() == Category::isMedium || RefCounted::refs(ml_.data_) == 1) {
				shrinkMedium(delta);
			}
			else {
				shrinkLarge(delta);
			}
		}

		void reserve(size_t minCapacity) {
			switch (category()) {
			case Category::isSmall:
				reserveSmall(minCapacity);
				break;
			case Category::isMedium:
				reserveMedium(minCapacity);
				break;
			case Category::isLarge:
				reserveLarge(minCapacity);
				break;
			}
			assert(capacity() >= minCapacity);
		}

		Char* expandNoinit(
			const size_t delta,
			bool expGrowth = false);

		void push_back(Char c) { *expandNoinit(1, /* expGrowth = */ true) = c; }

		size_t size() const {
			size_t ret = ml_.size_;
			if  constexpr (is_little_endian) {
				// We can save a couple instructions, because the category is
				// small if the last char, as unsigned, is <= maxSmallSize.
				using UChar = std::make_unsigned_t<Char>;
				using ssize_t = std::make_signed_t<size_t>;
				auto maybeSmallSize = size_t(maxSmallSize) -
					size_t(static_cast<UChar>(small_[maxSmallSize]));
				// With this syntax, GCC and Clang generate a CMOV instead of a branch.
				ret = (static_cast<ssize_t>(maybeSmallSize) >= 0) ? maybeSmallSize : ret;
			}
			else {
				ret = (category() == Category::isSmall) ? smallSize() : ret;
			}
			return ret;
		}

		size_t capacity() const {
			switch (category()) {
			case Category::isSmall:
				return maxSmallSize;
			case Category::isLarge:
				// For large-sized strings, a multi-referenced chunk has no
				// available capacity. This is because any attempt to append
				// data would trigger a new allocation.
				if (RefCounted::refs(ml_.data_) > 1) {
					return ml_.size_;
				}
				break;
			case Category::isMedium:
			default:
				break;
			}
			return ml_.capacity();
		}

		bool isShared() const {
			return category() == Category::isLarge && RefCounted::refs(ml_.data_) > 1;
		}

		void setSize(size_t new_size) {
			if (category() == Category::isSmall) {
				setSmallSize(new_size);
			}
			else {
				ml_.size_ = new_size;
				ml_.data_[new_size] = '\0';
			}
		}

		void shrink_to_fit();

	private:
		Char* c_str() {
			Char* ptr = ml_.data_;
			// With this syntax, GCC and Clang generate a CMOV instead of a branch.
			ptr = (category() == Category::isSmall) ? small_ : ptr;
			return ptr;
		}

		void reset() { setSmallSize(0); }

		void destroyMediumLarge() noexcept {
			auto const c = category();
			assert(c != Category::isSmall);
			if (c == Category::isMedium) {
				mallocator<Char>().deallocate(ml_.data_, ml_.capacity_ + 1);
			}
			else {
				RefCounted::decrementRefs(ml_.data_, ml_.capacity_ + 1);
			}
		}

		struct RefCounted {
			std::atomic<size_t> refCount_;
			Char data_[1];

			constexpr static size_t getDataOffset() {
				return offsetof(RefCounted, data_);
			}

			static RefCounted* fromData(Char* p) {
				return static_cast<RefCounted*>(static_cast<void*>(
					static_cast<unsigned char*>(static_cast<void*>(p)) -
					getDataOffset()));
			}

			static size_t refs(Char* p) {
				return fromData(p)->refCount_.load(std::memory_order_acquire);
			}

			static void incrementRefs(Char* p) {
				fromData(p)->refCount_.fetch_add(1, std::memory_order_acq_rel);
			}

			static void decrementRefs(Char* p, size_t cnt) {
				auto const dis = fromData(p);
				size_t oldcnt = dis->refCount_.fetch_sub(1, std::memory_order_acq_rel);
				assert(oldcnt > 0);
				if (oldcnt == 1) {
					mallocator<char>().deallocate((char*)dis, getDataOffset() + cnt * sizeof(Char));
				}
			}

			static RefCounted* create(size_t size) {
				const size_t allocSize =
					getDataOffset() + (size + 1) * sizeof(Char);
				auto result = static_cast<RefCounted*>((void*)mallocator<char>().allocate(allocSize));
				result->refCount_.store(1, std::memory_order_release);
				return result;
			}

			static RefCounted* create(const Char* data, size_t size) {
				const size_t effectiveSize = size;
				auto result = create(size);
				if (likely(effectiveSize > 0))  {
					memcpy(result->data_, data, effectiveSize * sizeof(Char));
				}
				return result;
			}

			static RefCounted* reallocate(
				Char* const data,
				const size_t currentSize,
				const size_t currentCapacity,
				size_t newCapacity) {
				assert(newCapacity > 0 && newCapacity > currentSize);
				const size_t allocNewCapacity = getDataOffset() + (newCapacity + 1) * sizeof(Char);
				auto const dis = fromData(data);
				assert(dis->refCount_.load(std::memory_order_acquire) == 1);
				auto result = static_cast<RefCounted*>(smart_realloc(
					dis,
					getDataOffset() + (currentSize + 1) * sizeof(Char),
					getDataOffset() + (currentCapacity + 1) * sizeof(Char),
					allocNewCapacity));
				assert(result->refCount_.load(std::memory_order_acquire) == 1);
				return result;
			}
		};

		using category_type = uint8_t;

		enum class Category : category_type {
			isSmall = 0,
			isMedium = is_little_endian ? 0x80 : 0x2,
			isLarge = is_little_endian ? 0x40 : 0x1,
		};

		Category category() const {
			// works for both big-endian and little-endian
			return static_cast<Category>(bytes_[lastChar] & categoryExtractMask);
		}

		struct MediumLarge {
			Char* data_;
			size_t size_;
			size_t capacity_;

			size_t capacity() const {
				return is_little_endian ? capacity_ & capacityExtractMask : capacity_ >> 2;
			}

			void setCapacity(size_t cap, Category cat) {
				capacity_ = is_little_endian
					? cap | (static_cast<size_t>(cat) << kCategoryShift)
					: (cap << 2) | static_cast<size_t>(cat);
			}
		};

		union {
			uint8_t bytes_[sizeof(MediumLarge)]; // For accessing the last byte.
			Char small_[sizeof(MediumLarge) / sizeof(Char)];
			MediumLarge ml_;
		};

		constexpr static size_t lastChar = sizeof(MediumLarge) - 1;
		constexpr static size_t maxSmallSize = lastChar / sizeof(Char);
		constexpr static size_t maxMediumSize = 254 / sizeof(Char);
		constexpr static uint8_t categoryExtractMask = is_little_endian ? 0xC0 : 0x3;
		constexpr static size_t kCategoryShift = (sizeof(size_t) - 1) * 8;
		constexpr static size_t capacityExtractMask = is_little_endian
			? ~(size_t(categoryExtractMask) << kCategoryShift)
			: 0x0 /* unused */;

		static_assert(
			!(sizeof(MediumLarge) % sizeof(Char)),
			"Corrupt memory layout for fbstring.");

		size_t smallSize() const {
			assert(category() == Category::isSmall);
			constexpr auto shift = is_little_endian ? 0 : 2;
			auto smallShifted = static_cast<size_t>(small_[maxSmallSize]) >> shift;
			assert(static_cast<size_t>(maxSmallSize) >= smallShifted);
			return static_cast<size_t>(maxSmallSize) - smallShifted;
		}

		void setSmallSize(size_t s) {
			// Warning: this should work with wjr_uninitialized_tag strings too,
			// so don't assume anything about the previous value of
			// small_[maxSmallSize].
			assert(s <= maxSmallSize);
			constexpr auto shift = is_little_endian ? 0 : 2;
			small_[maxSmallSize] = char((maxSmallSize - s) << shift);
			small_[s] = '\0';
			assert(category() == Category::isSmall && size() == s);
		}

		void copySmall(const shared_String_core&);
		void copyMedium(const shared_String_core&);
		void copyLarge(const shared_String_core&);

		void initSmall(const Char* data, size_t size);
		void initMedium(const Char* data, size_t size);
		void initLarge(const Char* data, size_t size);

		void reserveSmall(size_t minCapacity);
		void reserveMedium(size_t minCapacity);
		void reserveLarge(size_t minCapacity);

		void shrinkSmall(size_t delta);
		void shrinkMedium(size_t delta);
		void shrinkLarge(size_t delta);

		void unshare(size_t minCapacity = 0);
		Char* mutableDataLarge();
	};

	template <class Char>
	inline void shared_String_core<Char>::copySmall(const shared_String_core& rhs) {
		static_assert(offsetof(MediumLarge, data_) == 0, "fbstring layout failure");
		static_assert(
			offsetof(MediumLarge, size_) == sizeof(ml_.data_),
			"fbstring layout failure");
		static_assert(
			offsetof(MediumLarge, capacity_) == 2 * sizeof(ml_.data_),
			"fbstring layout failure");
		// Just write the whole thing, don't look at details. In
		// particular we need to copy capacity anyway because we want
		// to set the size (don't forget that the last character,
		// which stores a short string's length, is shared with the
		// ml_.capacity field).
		ml_ = rhs.ml_;
		assert(category() == Category::isSmall && this->size() == rhs.size());
	}

	template <class Char>
	void shared_String_core<Char>::copyMedium(const shared_String_core& rhs) {
		// Medium strings are copied eagerly. Don't forget to allocate
		// one extra Char for the null terminator.
		auto const allocSize = (1 + rhs.ml_.size_) * sizeof(Char);
		ml_.data_ = static_cast<Char*>(mallocator<char>().allocate(allocSize));
		// Also copies terminator.
		pod_copy(
			rhs.ml_.data_, rhs.ml_.data_ + rhs.ml_.size_ + 1, ml_.data_);
		ml_.size_ = rhs.ml_.size_;
		ml_.setCapacity(rhs.ml_.size_, Category::isMedium);
		assert(category() == Category::isMedium);
	}

	template <class Char>
	void shared_String_core<Char>::copyLarge(const shared_String_core& rhs) {
		// Large strings are just refcounted
		ml_ = rhs.ml_;
		RefCounted::incrementRefs(ml_.data_);
		assert(category() == Category::isLarge && size() == rhs.size());
	}

	// Small strings are bitblitted
	template <class Char>
	inline void shared_String_core<Char>::initSmall(
		const Char* const data, const size_t size) {
		// Layout is: Char* data_, size_t size_, size_t capacity_
		static_assert(
			sizeof(*this) == sizeof(Char*) + 2 * sizeof(size_t),
			"fbstring has unexpected size");
		static_assert(
			sizeof(Char*) == sizeof(size_t), "fbstring size assumption violation");
		// sizeof(size_t) must be a power of 2
		static_assert(
			(sizeof(size_t) & (sizeof(size_t) - 1)) == 0,
			"fbstring size assumption violation");

		memcpy(small_, data, size * sizeof(Char));
		setSmallSize(size);
	}

	template <class Char>
	void shared_String_core<Char>::initMedium(
		const Char* const data, const size_t size) {
		// Medium strings are allocated normally. Don't forget to
		// allocate one extra Char for the terminating null.
		auto const allocSize = (1 + size) * sizeof(Char);
		ml_.data_ = static_cast<Char*>(mallocator<char>().allocate(allocSize));
		if (likely(size > 0))  {
			pod_copy(data, data + size, ml_.data_);
		}
		ml_.size_ = size;
		ml_.setCapacity(size, Category::isMedium);
		ml_.data_[size] = '\0';
	}

	template <class Char>
	void shared_String_core<Char>::initLarge(
		const Char* const data, const size_t size) {
		// Large strings are allocated differently
		size_t effectiveCapacity = size;
		auto const newRC = RefCounted::create(data, effectiveCapacity);
		ml_.data_ = newRC->data_;
		ml_.size_ = size;
		ml_.setCapacity(effectiveCapacity, Category::isLarge);
		ml_.data_[size] = '\0';
	}

	template <class Char>
	void shared_String_core<Char>::unshare(size_t minCapacity) {
		assert(category() == Category::isLarge);
		size_t effectiveCapacity = std::max(minCapacity, ml_.capacity());
		auto const newRC = RefCounted::create(effectiveCapacity);
		// If this fails, someone placed the wrong capacity in an
		// fbstring.
		assert(effectiveCapacity >= ml_.capacity());
		// Also copies terminator.
		pod_copy(ml_.data_, ml_.data_ + ml_.size_ + 1, newRC->data_);
		RefCounted::decrementRefs(ml_.data_, ml_.capacity_ + 1);
		ml_.data_ = newRC->data_;
		ml_.setCapacity(effectiveCapacity, Category::isLarge);
		// size_ remains unchanged.
	}

	template <class Char>
	inline Char* shared_String_core<Char>::mutableDataLarge() {
		assert(category() == Category::isLarge);
		if (RefCounted::refs(ml_.data_) > 1) { // Ensure unique.
			unshare();
		}
		return ml_.data_;
	}

	template <class Char>
	void shared_String_core<Char>::reserveLarge(size_t minCapacity) {
		assert(category() == Category::isLarge);
		if (RefCounted::refs(ml_.data_) > 1) { // Ensure unique
		  // We must make it unique regardless; in-place reallocation is
		  // useless if the string is shared. In order to not surprise
		  // people, reserve the new block at current capacity or
		  // more. That way, a string's capacity never shrinks after a
		  // call to reserve.
			unshare(minCapacity);
		}
		else {
			// String is not shared, so let's try to realloc (if needed)
			if (minCapacity > ml_.capacity()) {
				// Asking for more memory
				auto const newRC = RefCounted::reallocate(
					ml_.data_, ml_.size_, ml_.capacity(), minCapacity);
				ml_.data_ = newRC->data_;
				ml_.setCapacity(minCapacity, Category::isLarge);
			}
			assert(capacity() >= minCapacity);
		}
	}

	template <class Char>
	void shared_String_core<Char>::reserveMedium(
		const size_t minCapacity) {
		assert(category() == Category::isMedium);
		// String is not shared
		if (minCapacity <= ml_.capacity()) {
			return; // nothing to do, there's enough room
		}
		if (minCapacity <= maxMediumSize) {
			// Keep the string at medium size. Don't forget to allocate
			// one extra Char for the terminating null.
			size_t capacityBytes = (1 + minCapacity) * sizeof(Char);
			// Also copies terminator.
			ml_.data_ = static_cast<Char*>(smart_realloc(
				ml_.data_,
				(ml_.size_ + 1) * sizeof(Char),
				(ml_.capacity() + 1) * sizeof(Char),
				capacityBytes));
			ml_.setCapacity(capacityBytes / sizeof(Char) - 1, Category::isMedium);
		}
		else {
			// Conversion from medium to large string
			shared_String_core nascent;
			// Will recurse to another branch of this function
			nascent.reserve(minCapacity);
			nascent.ml_.size_ = ml_.size_;
			// Also copies terminator.
			pod_copy(
				ml_.data_, ml_.data_ + ml_.size_ + 1, nascent.ml_.data_);
			nascent.swap(*this);
			assert(capacity() >= minCapacity);
		}
	}

	template <class Char>
	void shared_String_core<Char>::reserveSmall(
		size_t minCapacity) {
		assert(category() == Category::isSmall);
		if (minCapacity <= maxSmallSize) {
			// small
			// Nothing to do, everything stays put
		}
		else if (minCapacity <= maxMediumSize) {
			// medium
			// Don't forget to allocate one extra Char for the terminating null
			auto const allocSizeBytes =
				(1 + minCapacity) * sizeof(Char);
			auto const pData = static_cast<Char*>(mallocator<char>().allocate(allocSizeBytes));
			auto const size = smallSize();
			// Also copies terminator.
			pod_copy(small_, small_ + size + 1, pData);
			ml_.data_ = pData;
			ml_.size_ = size;
			ml_.setCapacity(allocSizeBytes / sizeof(Char) - 1, Category::isMedium);
		}
		else {
			// large
			auto const newRC = RefCounted::create(minCapacity);
			auto const size = smallSize();
			// Also copies terminator.
			pod_copy(small_, small_ + size + 1, newRC->data_);
			ml_.data_ = newRC->data_;
			ml_.size_ = size;
			ml_.setCapacity(minCapacity, Category::isLarge);
			assert(capacity() >= minCapacity);
		}
	}

	template <class Char>
	inline Char* shared_String_core<Char>::expandNoinit(
		const size_t delta,
		bool expGrowth /* = false */) {
		// Strategy is simple: make room, then change size
		assert(capacity() >= size());
		size_t sz, newSz;
		if (category() == Category::isSmall) {
			sz = smallSize();
			newSz = sz + delta;
			if (likely(newSz <= maxSmallSize))  {
				setSmallSize(newSz);
				return small_ + sz;
			}
			reserveSmall(
				expGrowth ? std::max(newSz, 2 * maxSmallSize) : newSz);
		}
		else {
			sz = ml_.size_;
			newSz = sz + delta;
			if (unlikely(newSz > capacity()))  {
				// ensures not shared
				reserve(expGrowth ? std::max(newSz, 1 + capacity() * 3 / 2) : newSz);
			}
		}
		assert(capacity() >= newSz);
		// Category can't be small - we took care of that above
		assert(category() == Category::isMedium || category() == Category::isLarge);
		ml_.size_ = newSz;
		ml_.data_[newSz] = '\0';
		assert(size() == newSz);
		return ml_.data_ + sz;
	}

	template <class Char>
	inline void shared_String_core<Char>::shrinkSmall(const size_t delta) {
		// Check for underflow
		assert(delta <= smallSize());
		setSmallSize(smallSize() - delta);
	}

	template <class Char>
	inline void shared_String_core<Char>::shrinkMedium(const size_t delta) {
		// Medium strings and unique large strings need no special
		// handling.
		assert(ml_.size_ >= delta);
		ml_.size_ -= delta;
		ml_.data_[ml_.size_] = '\0';
	}

	template <class Char>
	inline void shared_String_core<Char>::shrinkLarge(const size_t delta) {
		assert(ml_.size_ >= delta);
		// Shared large string, must make unique. This is because of the
		// durn terminator must be written, which may trample the shared
		// data.
		if (delta) {
			shared_String_core(ml_.data_, ml_.size_ - delta).swap(*this);
		}
		// No need to write the terminator.
	}

	template<class Char>
	void shared_String_core<Char>::shrink_to_fit() {
		if (category() == Category::isSmall) {
			return;
		}
		const size_t _size = ml_.size_;
		const size_t _capacity = ml_.capacity();
		if (_capacity <= _size * 9 / 8) {
			return;
		}
		shared_String_core<Char>(data(), _size).swap(*this);
	}

#endif // TEST_SHARED_STRING

	template<typename Char, typename Traits = std::char_traits<Char>>
	class basic_String_view;

	template<typename Char, typename Traits = std::char_traits<Char>, typename Core = String_core<Char>>
	class basic_String;	

	template<typename Char>
	class String_trim_helper {
	public:
		constexpr static const Char* data() {
			return tr;
		}

		constexpr static const Char* begin() {
			return tr;
		}

		constexpr static const Char* end() {
			return tr + size();
		}

		constexpr static size_t size() {
			return 6;
		}
	private:
		constexpr static Char tr[7] = {
			std::integral_constant<Char,' ' >(),
			std::integral_constant<Char,'\r'>(),
			std::integral_constant<Char,'\n'>(),
			std::integral_constant<Char,'\t'>(),
			std::integral_constant<Char,'\f'>(),
			std::integral_constant<Char,'\v'>(),
			std::integral_constant<Char,'\0'>()
		};
	};
	
	template<typename Traits>
	struct case_insensitive_traits : public Traits {
		using base      = Traits;
		using char_type = typename base::char_type;

		static constexpr bool eq(const char_type& lhs, const char_type& rhs) noexcept {
			return base::eq(toupper(lhs), toupper(rhs));
		}

		static constexpr bool lt(const char_type& lhs, const char_type& rhs)noexcept {
			return base::lt(toupper(lhs), toupper(rhs));
		}

		static int compare(const char_type* l, const char_type* r, const size_t count)noexcept {
			const char_type* e = l + count;
			while (l != e) {
				char_type lc = toupper(*l);
				char_type rc = toupper(*r);
				if (!base::eq(lc, rc)) {
					return base::lt(lc, rc) ? -1 : 1;
				}
				++l;
				++r;
			}
			return 0;
		}

		static constexpr const char_type* find(
			const char_type* first, size_t count, const char_type& ch) noexcept {
			for (; 0 < count; --count, ++first) {
				if (eq(*first, ch)) {
					return first;
				}
			}
			return nullptr;
		}
	};

	template<typename Traits>
	struct traits_char_map<case_insensitive_traits<Traits>>
		: base_traits_char_map<case_insensitive_traits<Traits>> {
		using char_type        = typename
			base_traits_char_map<case_insensitive_traits<Traits>>::char_type;
		using is_default_equal = std::true_type;
		constexpr static decltype(auto) to(const char_type& value) {
			return toupper(static_cast<std::make_unsigned_t<char_type>>(value));
		}
	};

	template<typename T>
	struct case_insensitive_String {
		using type = case_insensitive_traits<typename T::traits_type>;
	};

	template<typename T>
	using case_insensitive_String_t = typename case_insensitive_String<T>::type;
		
	// judge if T has a member function operator()(const Char*,const Char*)
	template<typename Char,typename T,typename = void>
	struct String_has_find_function : std::false_type {};
	
	template<typename Char,typename T>
	struct String_has_find_function<Char, T, 
		std::void_t<decltype(std::declval<T>()(std::declval<const Char*>(),
			std::declval<const Char*>()))>> : std::true_type {};
	
	// judge if T has a member function size()
	template<typename T, typename = void>
	struct String_has_size_function : std::false_type {};
	
	template<typename T>
	struct String_has_size_function<T,
		std::void_t<decltype(std::declval<T>().size())>> : std::true_type {};
	
	template<typename Char,typename Traits,typename T>
	struct is_String_find_wrapper {
		constexpr static bool value = 
			String_has_find_function<Char,T>::value &&
			String_has_size_function<T>::value;
		using searcher_type = std::reference_wrapper<const T>;
	};
	
	template<typename Char,typename Traits>
	struct is_String_find_wrapper<Char, Traits, Char> {
		constexpr static bool value = true;
		using searcher_type = Char;
	};
	
	template<typename Char,typename Traits>
	struct is_String_find_wrapper<Char, Traits, basic_String_view<Char, Traits>> {
		constexpr static bool value = true;
		using searcher_type = basic_String_view<Char, Traits>;
	};

	template<typename Char,typename Traits,typename T>
	class String_find_wrapper {
	public:
		using value_type = Char;
		using traits_type = Traits;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using searcher_type = typename 
			is_String_find_wrapper<Char, Traits, T>::searcher_type;
		constexpr static bool is_char = std::is_same_v<T, Char>;
		constexpr static bool is_string_view = 
			std::is_same_v<T, basic_String_view<Char, Traits>>;
		constexpr static size_t npos = (size_t)(-1);
		static_assert(is_String_find_wrapper<Char,Traits,T>::value, 
			"the type of String_find_wrapper is wrong");
		String_find_wrapper(const T& srch)
			: srch(srch) {
		}
		String_find_wrapper(const String_find_wrapper&) = default;
		String_find_wrapper(String_find_wrapper&&) noexcept = default;
		String_find_wrapper& operator=(const String_find_wrapper&) = default;
		String_find_wrapper& operator=(String_find_wrapper&&) noexcept = default;
		size_t size()const {
			if constexpr (is_char) {
				return 1;
			}
			else if constexpr (is_string_view) {
				return srch.size();
			}
			else {
				return srch.get().size();
			}
		}
		constexpr static size_t find(const Char* s, const Char* e,const T& srch) {
			assert(s <= e);
			if constexpr (is_char) {
				auto pos = Traits::find(s, static_cast<size_t>(e - s), srch);
				return pos == nullptr ? npos : static_cast<size_t>(pos - s);
			}
			else if constexpr (is_string_view) {
				auto n = static_cast<size_t>(e - s);
				size_t m = srch.size();
				if (unlikely(m == 0))  {
					return 0;
				}
				if (n < m) {
					return npos;
				}
				const auto match_end = s + (n - m) + 1;
				for (auto match_try = s;; ++match_try) {
					match_try = traits_type::find(match_try,
						static_cast<size_t>(match_end - match_try), srch.front());
					if (!match_try) {
						return npos;
					}
					if (traits_type::compare(match_try, srch.data(), m) == 0) {
						return static_cast<size_t>(match_try - s);
					}
				}
			}
			else {
				auto pos = srch(s, e);
				return pos.first == e ? npos : static_cast<size_t>(pos.first - s);
			}
		}
		
		size_t operator()(const Char* s, const Char* e)const {
			return String_find_wrapper<Char,Traits,T>::find(s, e, srch);
		}
		
	private:
		searcher_type srch;
	};

	template<typename Char, typename Traits = std::char_traits<Char>>
	struct basic_String_traits {
		template<typename _Char, typename _Traits>
		friend class basic_String_view;
		template<typename _Char, typename _Traits, typename _Core>
		class basic_String;
		using value_type  = Char;
		using traits_type = Traits;
		using size_type   = size_t;

		constexpr static size_type npos = static_cast<size_type>(-1);
		
		template<typename T>
		using string_find_wrapper = String_find_wrapper<Char, Traits, T>;

		using string_find_helper  = skmp_searcher<const value_type*, Traits>;
		using string_rfind_helper = skmp_searcher<std::reverse_iterator<const value_type*>, Traits>;
		
		using string_find_wrapper_char = string_find_wrapper<Char>;
		using string_find_wrapper_string_view = string_find_wrapper<basic_String_view<Char, Traits>>;
		using string_find_wrapper_searcher = string_find_wrapper<string_find_helper>;
		
		constexpr static size_type is_simple_searcher_v = string_find_helper::is_simple_searcher_v;
		constexpr static size_type small_search_size = string_find_helper::small_search_size;

	private:
		struct bit_map {
			// if Char is char or other possible kind
			constexpr static size_t ubit = sizeof(uint32_t);
			constexpr static size_t size = ubit << 3;

			bit_map(const value_type* s, const value_type* e) {
				while (s != e) {
					auto value = static_cast<std::make_unsigned_t<value_type>>(*s);
					c[value / size] |= 1 << (value & (size - 1));
					++s;
				}
			}

			bool count(const value_type ch)const {
				const auto value = static_cast<std::make_unsigned_t<value_type>>(ch);
				return (c[value / size] >> (value & (size - 1))) & 1;
			}

			uint32_t c[256 / size] = {};
		};

		struct general_map {
			using little_map = bit_map;
			using large_map  = std::unordered_set<value_type,
				std::hash<value_type>, traits_equal<Traits>, mallocator<value_type>>;

			general_map(const value_type* s, const value_type* e) {
				const value_type* ptr = s;
				while (ptr != e) {
					if (*ptr > 0xFF || *ptr < 0) {
						is_large = true;
					}
					++ptr;
				}
				if (!is_large) {
					a = mallocator<little_map>().allocate(1);
					new (a) little_map(s, e);
				}
				else {
					b = mallocator<large_map>().allocate(1);
					new (b) large_map(s, e);
				}
			}

			bool count(const value_type ch)const {
				if (!is_large) {
					if (ch > 0xFF || ch < 0) {
						return false;
					}
					return a->count(ch);
				}
				else {
					return b->count(ch);
				}
			}

			union {
				little_map* a;
				large_map* b;
			};

			bool is_large = false;
		};

	public:

		using string_find_of_helper = std::conditional_t<
			can_make_bit_map<value_type, traits_type>::value,
			bit_map,
			std::conditional_t<traits_char_map<Traits>::is_default_equal::value
			, general_map, typename general_map::large_map>>;

		static string_find_of_helper trim_map;

		constexpr static size_type traits_length(const value_type* s) {
			return traits_type::length(s);
		}

		constexpr static size_type npos_min(size_type n, const size_type x) {
			if constexpr (std::is_unsigned_v<size_type>) {
				return n < x ? n : x;
			}
			else {
				assert(n >= 0 || n == npos);
				return (n == npos || n > x) ? x : n;
			}
		}
		
		template<typename T>
		static string_find_wrapper<T> get_find_wrapper(const T& srch) {
			return string_find_wrapper<T>(srch);
		}

		static string_find_helper
			get_find_helper(const value_type* s, const value_type* e);
		static size_type find(const value_type* s, const size_type n,
			const size_type off, const value_type ch);
		// only need to find once ,then use this
		// for this won't initial at first
		static size_type find(const value_type* s1, const size_type n1,
			const size_type off, const value_type* s2, const size_type n2);
		static size_type find(const value_type* s1, const size_type n1,
			const size_type off, const string_find_helper& srch);

		static string_rfind_helper
			get_rfind_helper(const value_type* s, const value_type* e);
		static size_type rfind(const value_type* s, const size_type n,
			const size_type off, const value_type ch);
		static size_type rfind(const value_type* s1, const size_type n1,
			size_type off, const value_type* s2, const size_type n2);
		static size_type rfind(const value_type* s1, const size_type n1,
			size_type off, const string_rfind_helper& srch);

		static string_find_of_helper get_find_of_helper(const value_type* s, const value_type* e);

		static size_type find_first_of_ch(const value_type* s,
			const size_type n, size_type off, const value_type ch);
		static size_type normal_find_first_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2);
		static size_type map_find_first_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2);
		static size_type find_first_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2);
		static size_type find_first_of(const value_type* s1,
			const size_type n1, const size_type off, const string_find_of_helper& srch);

		static size_type find_last_of_ch(const value_type* s,
			const size_type n, size_type off, const value_type ch);
		static size_type normal_find_last_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2);
		static size_type map_find_last_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2);
		static size_type find_last_of(const value_type* s1,
			const size_type n1, size_type off, const value_type* s2, const size_type n2);
		static size_type find_last_of(const value_type* s1,
			const size_type n1, const size_type off, const string_find_of_helper& srch);

		static size_type find_first_not_of_ch(const value_type* s,
			const size_type n, const size_type off, const value_type ch);
		static size_type normal_find_first_not_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2);
		static size_type map_find_first_not_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2);
		static size_type find_first_not_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2);
		static size_type find_first_not_of(const value_type* s1,
			const size_type n1, const size_type off, const string_find_of_helper& srch);

		static size_type find_last_not_of_ch(const value_type* s,
			const size_type n, const size_type off, const value_type ch);
		static size_type normal_find_last_not_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2);
		static size_type map_find_last_not_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2);
		static size_type find_last_not_of(const value_type* s1,
			const size_type n1, size_type off, const value_type* s2, const size_type n2);
		static size_type find_last_not_of(const value_type* s1,
			const size_type n1, const size_type off, const string_find_of_helper& srch);

		static int compare(const value_type* s1, const size_type n1,
			const value_type* s2, const size_type n2) {
			const int r = traits_type::compare(s1, s2, std::min(n1, n2));
			return r != 0 ? r : n1 > n2 ? 1 : n1 < n2 ? -1 : 0;
		}

		static bool equal(const value_type* s1, const size_type n1,
			const value_type* s2, const size_type n2) {
			return (n1 == n2) && (traits_type::compare(s1, s2, n1) == 0);
		}

		static bool starts_with(const value_type* s, const size_type n, const value_type ch) {
			return (n != 0) && (traits_type::eq(*s, ch));
		}

		static bool starts_with(const value_type* s1, const size_type n1,
			const value_type* s2, const size_type n2) {
			return (n1 >= n2) && (traits_type::compare(s1, s2, n2) == 0);
		}

		static bool ends_with(const value_type* s, const size_type n, const value_type ch) {
			return (n != 0) && (traits_type::eq(*s, ch));
		}

		static bool ends_with(const value_type* s1, const size_type n1,
			const value_type* s2, const size_type n2) {
			return (n1 >= n2) && (traits_type::compare(s1 + n1 - n2, s2, n2) == 0);
		}

		template<typename string_list>
		static string_list split(const value_type* s, const size_type n,
			const value_type ch, bool keep_empty_parts);

		template<typename string_list>
		static string_list split(const value_type* s1, const size_type n1,
			const value_type* s2, const size_type n2, bool keep_empty_parts);

		static size_type trim_left(const value_type* s, const size_type n);
		static size_type trim_right(const value_type* s, const size_type n);

	private:
		template<typename Val, std::enable_if_t<std::is_signed_v<Val>, int> = 0>
		static Val first_to_val_helper(const value_type* s, const value_type* e,
			const value_type*& next, bool* ok, int base);
		template<typename UVal, std::enable_if_t<std::is_unsigned_v<UVal>, int> = 0>
		static UVal first_to_val_helper(const value_type* s, const value_type* e,
			const value_type*& next, bool* ok, int base);

		template<typename Val, std::enable_if_t<std::is_signed_v<Val>, int> = 0>
		static Val range_to_val_helper(const value_type* s, const value_type* e,
			bool* ok, int base);
		template<typename UVal, std::enable_if_t<std::is_unsigned_v<UVal>, int> = 0>
		static UVal range_to_val_helper(const value_type* s, const value_type* e,
			bool* ok, int base);

		template<typename Val, std::enable_if_t<std::is_signed_v<Val>, int> = 0>
		static Val unsafe_first_to_val_helper(const value_type* s,
			const value_type* e, const value_type*& next, int base);
		template<typename UVal, std::enable_if_t<std::is_unsigned_v<UVal>, int> = 0>
		static UVal unsafe_first_to_val_helper(const value_type* s,
			const value_type* e, const value_type*& next, int base);

		template<typename Val, std::enable_if_t<std::is_signed_v<Val>, int> = 0>
		static Val unsafe_range_to_val_helper(const value_type* s,
			const value_type* e, int base);
		template<typename UVal, std::enable_if_t<std::is_unsigned_v<UVal>, int> = 0>
		static UVal unsafe_range_to_val_helper(const value_type* s,
			const value_type* e, int base);

		static void set_ok(bool* ok) {
			if (ok != nullptr) {
				*ok = true;
			}
		}
		static void set_nok(bool* ok) {
			if (ok != nullptr) {
				*ok = false;
			}
		}

	public:
		constexpr static Char nan[4] = {
			static_cast<Char>('n'),
			static_cast<Char>('a'),
			static_cast<Char>('n'),
			static_cast<Char>('\0')
		};
		constexpr static Char inf[4] = {
			static_cast<Char>('i'),
			static_cast<Char>('n'),
			static_cast<Char>('f'),
			static_cast<Char>('\0')
		};

		// safe version
		// get the first integer of the string
		// there can only be white space characters before the first integer
		// the string can contains more than one legal integer
		static int first_to_int(const value_type* s, const value_type* e,
			const value_type*& next, bool* ok = nullptr, int base = 10);
		static unsigned int first_to_uint(const value_type* s, const value_type* e,
			const value_type*& next, bool* ok = nullptr, int base = 10);
		static long long first_to_ll(const value_type* s, const value_type* e,
			const value_type*& next, bool* ok = nullptr, int base = 10);
		static unsigned long long first_to_ull(const value_type* s, const value_type* e,
			const value_type*& next, bool* ok = nullptr, int base = 10);

		// get the integer of the string
		// if it only has on legal integer then ok is true
		static int range_to_int(const value_type* s, const value_type* e,
			bool* ok = nullptr, int base = 10);
		static unsigned int range_to_uint(const value_type* s, const value_type* e,
			bool* ok = nullptr, int base = 10);
		static long long range_to_ll(const value_type* s, const value_type* e,
			bool* ok = nullptr, int base = 10);
		static unsigned long long range_to_ull(const value_type* s, const value_type* e,
			bool* ok = nullptr, int base = 10);

		// unsafe version,won't check whether is a right integer
		// faster than safe version
		static int unsafe_first_to_int(const value_type* s, const value_type* e,
			const value_type*& next, int base = 10);
		static unsigned int unsafe_first_to_uint(const value_type* s, const value_type* e,
			const value_type*& next, int base = 10);
		static long long unsafe_first_to_ll(const value_type* s, const value_type* e,
			const value_type*& next, int base = 10);
		static unsigned long long unsafe_first_to_ull(const value_type* s, const value_type* e,
			const value_type*& next, int base = 10);

		static int unsafe_range_to_int(const value_type* s, const value_type* e,
			int base = 10);
		static unsigned int unsafe_range_to_uint(const value_type* s, const value_type* e,
			int base = 10);
		static long long unsafe_range_to_ll(const value_type* s, const value_type* e,
			int base = 10);
		static unsigned long long unsafe_range_to_ull(const value_type* s, const value_type* e,
			int base = 10);

		// because of performance problem,there is no double conversion for the time being
	};

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::string_find_of_helper
		basic_String_traits<Char, Traits>::trim_map(
			String_trim_helper<Char>::begin(), String_trim_helper<Char>::end());

	template<typename Char, typename Traits = std::char_traits<Char>>
	using String_traits_helper = basic_String_traits<Char, Traits>;

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::string_find_helper
		basic_String_traits<Char, Traits>::get_find_helper(
			const value_type* s, const value_type* e) {
		return string_find_helper(s, e);
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::find(const value_type* s, const size_type n,
			const size_type off, const value_type ch) {
		if (off < n) {
			auto pos = string_find_wrapper_char::find(s + off, s + n, ch);
			if (pos == npos) {
				return npos;
			}
			return pos + off;
		}
		return npos;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::find(const value_type* s1, const size_type n1,
			const size_type off, const value_type* s2, const size_type n2) {
		if (n2 > n1 || off > n1 - n2) {
			return npos;
		}

		if (!n2) {
			return off;
		}

		if (n2 > small_search_size) {
			auto finder = get_find_helper(s2, s2 + n2);
			auto pos = string_find_wrapper_searcher::find(s1 + off, s1 + n1, finder);
			if(pos == npos) {
				return npos;
			}
			return pos + off;
		}
		
		auto pos = string_find_wrapper_string_view::find(s1 + off, s1 + n1,
			basic_String_view<Char, Traits>(s2, n2));
		if (pos == npos) {
			return npos;
		}
		return pos + off;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::find(const value_type* s1, const size_type n1,
			const size_type off, const string_find_helper& srch) {
		const size_type n2 = srch.size();
		if (n2 > n1 || off > n1 - n2) {
			return npos;
		}

		if (!n2) {
			return off;
		}

		auto pos = string_find_wrapper_searcher::find(s1 + off, s1 + n1, srch);
		if (pos == npos) {
			return npos;
		}
		return pos + off;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::string_rfind_helper
		basic_String_traits<Char, Traits>::get_rfind_helper(
			const value_type* s, const value_type* e) {
		return string_rfind_helper(
			std::reverse_iterator(e), std::reverse_iterator(s)
		);
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::rfind(const value_type* s,
			const size_type n, const size_type off, const value_type ch) {
		if (n != 0) {
			for (auto match_try = s + npos_min(off, n - 1);; --match_try) {
				if (traits_type::eq(*match_try, ch)) {
					return static_cast<size_type>(match_try - s);
				}
				if (match_try == s) {
					break;
				}
			}
		}
		return npos;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::rfind(const value_type* s1,
			const size_type n1, size_type off, const value_type* s2, const size_type n2) {
		off = npos_min(off, n1 - n2);
		if (n2 == 0) {
			return off;
		}

		if (n2 <= n1) {
			if (n2 > 4) {
				return rfind(s1, n1, off, get_rfind_helper(s2, s2 + n2));
			}

			for (auto match_try = s1 + off;; --match_try) {
				if (traits_type::eq(*match_try, *s2) && traits_type::compare(match_try, s2, n2) == 0) {
					return static_cast<size_type>(match_try - s1);
				}
				if (match_try == s1) {
					break;
				}
			}
		}
		return npos;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::rfind(const value_type* s1,
			const size_type n1, size_type off, const string_rfind_helper& srch) {
		const auto n2 = srch.size();
		off = npos_min(off, n1 - n2);
		if (n2 == 0) {
			return off;
		}

		if (n2 <= n1) {
			auto _end = std::reverse_iterator(s1);
			auto pos = srch(std::reverse_iterator(s1 + off + n2), _end).first;
			auto diff = _end - pos;
			if (!diff) {
				return npos;
			}
			return diff - n2;
		}
		return npos;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::string_find_of_helper
		basic_String_traits<Char, Traits>::get_find_of_helper(
			const value_type* s, const value_type* e) {
		return string_find_of_helper(s, e);
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::find_first_of_ch(const value_type* s,
			const size_type n, const size_type off, const value_type ch) {
		return find(s, n, off, ch);
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::normal_find_first_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
		const auto end = s1 + n1;
		for (auto match_try = s1 + off; match_try < end; ++match_try) {
			if (traits_type::find(s2, n2, *match_try)) {
				return static_cast<size_type>(match_try - s1);
			}
		}
		return npos;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::map_find_first_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
		return find_first_of(s1, n1, off, get_find_of_helper(s2, s2 + n2));
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::find_first_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
		if (n2 != 0 && off < n1) {
			if constexpr (std::is_same_v<string_find_of_helper, bit_map>) {
				if (n1 <= 8) {
					return normal_find_first_of(s1, n1, off, s2, n2);
				}
				else {
					return map_find_first_of(s1, n1, off, s2, n2);
				}
			}
			else {
				if (n2 <= 16) { // only need to use normal find
					return normal_find_first_of(s1, n1, off, s2, n2);
				}
				else {
					// may use Map_Search_first_of
					// firstly ,use normal find to match no more than length of 1024 / n2
					const size_type in = 1024 / n2;
					const size_type first_to_search = (n1 - off <= in ? n1 : off + in);
					auto first_to_search_match = normal_find_first_of(s1, first_to_search, off, s2, n2);
					if (first_to_search_match != npos) {
						return first_to_search_match;
					}

					if (first_to_search != n1) {
						return map_find_first_of(s1, n1, first_to_search, s2, n2);
					}
				}
			}
		}
		return npos;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::find_first_of(const value_type* s1,
			const size_type n1, const size_type off, const string_find_of_helper& srch) {
		const auto end = s1 + n1;
		for (auto match_try = s1 + off; match_try < end; ++match_try) {
			if (srch.count(*match_try)) {
				return static_cast<size_type>(match_try - s1);
			}
		}
		return npos;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::find_last_of_ch(const value_type* s,
			const size_type n, size_type off, const value_type ch) {
		return rfind(s, n, off, ch);
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::normal_find_last_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
		for (auto match_try = s1 + npos_min(off, n1 - 1);; --match_try) {
			if (traits_type::find(s2, n2, *match_try)) {
				return static_cast<size_type>(match_try - s1);
			}
			if (match_try == s1)
				break;
		}
		return npos;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::map_find_last_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
		return find_last_of(s1, n1, off, get_find_of_helper(s2, s2 + n2));
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::find_last_of(const value_type* s1,
			const size_type n1, size_type off, const value_type* s2, const size_type n2) {
		off = npos_min(off, n1 - 1);
		if (n1 != 0 && n2 != 0) {
			if constexpr (std::is_same_v<string_find_of_helper, bit_map>) {
				if (n1 <= 8) {
					return normal_find_last_of(s1, n1, off, s2, n2);
				}
				else {
					return map_find_last_of(s1, n1, off, s2, n2);
				}
			}
			else {
				if (n2 <= 16) {
					return normal_find_last_of(s1, n1, off, s2, n2);
				}
				else {
					const size_type in = 1024 / n2;
					const size_type first_to_search = off <= in ? 0 : off - in;
					auto first_to_search_match = normal_find_last_of(
						s1 + first_to_search, n1 - first_to_search, off - first_to_search, s2, n2);
					if (first_to_search_match != npos) {
						return first_to_search + first_to_search_match;
					}
					if (first_to_search != 0) {
						return normal_find_last_of(s1, n1, first_to_search, s2, n2);
					}
				}
			}
		}
		return npos;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::find_last_of(const value_type* s1,
			const size_type n1, const size_type off, const string_find_of_helper& srch) {
		for (auto match_try = s1 + npos_min(off, n1 - 1);; --match_try) {
			if (srch.count(*match_try)) {
				return static_cast<size_type>(match_try - s1);
			}

			if (match_try == s1)
				break;
		}
		return npos;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::find_first_not_of_ch(const value_type* s,
			const size_type n, const size_type off, const value_type ch) {
		if (off < n) {
			auto match_end = s + n;
			for (auto match_try = s + off; match_try != match_end; ++match_try) {
				if (!traits_type::eq(*match_try, ch)) {
					return static_cast<size_type>(match_try - s);
				}
			}
		}
		return npos;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::normal_find_first_not_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
		const auto end = s1 + n1;
		for (auto match_try = s1 + off; match_try < end; ++match_try) {
			if (!traits_type::find(s2, n2, *match_try)) {
				return static_cast<size_type>(match_try - s1);
			}
		}
		return npos;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::map_find_first_not_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
		return find_first_not_of(s1, n1, off, get_find_of_helper(s2, s2 + n2));
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::find_first_not_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
		if (n2 != 0 && off < n1) {
			if constexpr (std::is_same_v<string_find_of_helper, bit_map>) {
				if (n1 <= 8) {
					return normal_find_first_not_of(s1, n1, off, s2, n2);
				}
				else {
					return map_find_first_not_of(s1, n1, off, s2, n2);
				}
			}
			else {
				if (n2 <= 16) { // only need to use normal find
					return normal_find_first_not_of(s1, n1, off, s2, n2);
				}
				else {
					// may use Map_Search_first_of
					// firstly ,use normal find to match no more than length of 32
					const size_type in = 1024 / n2;
					const size_type first_to_search = (n1 - off <= in ? n1 : off + in);
					auto first_to_search_match = normal_find_first_not_of(s1, first_to_search, off, s2, n2);
					if (first_to_search_match != npos) {
						return first_to_search_match;
					}

					if (first_to_search != n1) {
						return map_find_first_not_of(s1, n1, first_to_search, s2, n2);
					}
				}
			}
		}
		return npos;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::find_first_not_of(const value_type* s1,
			const size_type n1, const size_type off, const string_find_of_helper& srch) {
		const auto end = s1 + n1;
		for (auto match_try = s1 + off; match_try < end; ++match_try) {
			if (!srch.count(*match_try)) {
				return static_cast<size_type>(match_try - s1);
			}
		}
		return npos;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::find_last_not_of_ch(const value_type* s,
			const size_type n, const size_type off, const value_type ch) {
		if (n != 0) {
			for (auto match_try = s + npos_min(off, n - 1);; --match_try) {
				if (!traits_type::eq(*match_try, ch)) {
					return static_cast<size_type>(match_try - s);
				}
				if (match_try == s) {
					break;
				}
			}
		}
		return npos;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::normal_find_last_not_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
		for (auto match_try = s1 + npos_min(off, n1 - 1);; --match_try) {
			if (!traits_type::find(s2, n2, *match_try)) {
				return static_cast<size_type>(match_try - s1);
			}

			if (match_try == s1)
				break;
		}
		return npos;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::map_find_last_not_of(const value_type* s1,
			const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
		return find_last_not_of(s1, n1, off, get_find_of_helper(s2, s2 + n2));
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::find_last_not_of(const value_type* s1,
			const size_type n1, size_type off, const value_type* s2, const size_type n2) {
		off = npos_min(off, n1 - 1);
		if (n1 != 0 && n2 != 0) {
			if constexpr (std::is_same_v<string_find_of_helper, bit_map>) {
				if (n1 <= 8) {
					return normal_find_last_not_of(s1, n1, off, s2, n2);
				}
				else {
					return map_find_last_not_of(s1, n1, off, s2, n2);
				}
			}
			else {
				if (n2 <= 16) {
					return normal_find_last_not_of(s1, n1, off, s2, n2);
				}
				else {
					const size_type in = 1024 / n2;
					const size_type first_to_search = off <= in ? 0 : off - in;
					auto first_to_search_match = normal_find_last_not_of(
						s1 + first_to_search, n1 - first_to_search, off - first_to_search, s2, n2);
					if (first_to_search_match != npos) {
						return first_to_search + first_to_search_match;
					}
					if (first_to_search != 0) {
						return normal_find_last_not_of(s1, n1, first_to_search, s2, n2);
					}
				}
			}
		}
		return npos;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::find_last_not_of(const value_type* s1,
			const size_type n1, const size_type off, const string_find_of_helper& srch) {
		for (auto match_try = s1 + npos_min(off, n1 - 1);; --match_try) {
			if (!srch.count(*match_try)) {
				return static_cast<size_type>(match_try - s1);
			}

			if (match_try == s1)
				break;
		}
		return npos;
	}

	template<typename Char, typename Traits>
	template<typename string_list>
	string_list basic_String_traits<Char, Traits>::split(
		const value_type* s, const size_type n,
		const value_type ch, bool keep_empty_parts) {
		string_list ans;
		size_type off = 0;
		if (keep_empty_parts) {
			for (;;) {
				size_type pos = find(s, n, off, ch);
				if (pos == npos) {
					ans.emplace_back(s + off, static_cast<size_type>(n - off));
					break;
				}
				ans.emplace_back(s + off, static_cast<size_type>(pos - off));
				off = pos + 1;
			}
		}
		else {
			for (;;) {
				const size_type pos = find(s, n, off, ch);
				if (pos == npos) {
					if (off != n) {
						ans.emplace_back(s + off, static_cast<size_type>(n - off));
					}
					break;
				}
				if (off != pos) {
					ans.emplace_back(s + off, static_cast<size_type>(pos - off));
				}
				off = pos + 1;
			}
		}
		return ans;
	}

	template<typename Char, typename Traits>
	template<typename string_list>
	string_list basic_String_traits<Char, Traits>::split(
		const value_type* s1, const size_type n1,
		const value_type* s2, const size_type n2, bool keep_empty_parts) {
		string_list ans;
		if (n1 < n2) {
			if (n1 != 0 || keep_empty_parts) {
				ans.emplace_back(s1, s1 + n1);
			}
			return ans;
		}

		if (n2 == 1) {
			return split<string_list>(s1, n1, *s2, keep_empty_parts);
		}

		auto srch = get_find_helper(s2, s2 + n2);
		size_type off = 0;
		if (keep_empty_parts) {
			for (;;) {
				const size_type pos = find(s1, n1, off, srch);
				if (pos == npos) {
					ans.emplace_back(s1 + off, static_cast<size_type>(n1 - off));
					break;
				}
				ans.emplace_back(s1 + off, static_cast<size_type>(pos - off));
				off = pos + n2;
			}
		}
		else {
			for (;;) {
				const size_type pos = find(s1, n1, off, srch);
				if (pos == npos) {
					if (off != n1) {
						ans.emplace_back(s1 + off, static_cast<size_type>(n1 - off));
					}
					break;
				}
				if (off != pos) {
					ans.emplace_back(s1 + off, static_cast<size_type>(pos - off));
				}
				off = pos + n2;
			}
		}

		return ans;
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::trim_left(const value_type* s, const size_type n) {
		return find_first_not_of(s, n, 0, trim_map);
	}

	template<typename Char, typename Traits>
	typename basic_String_traits<Char, Traits>::size_type
		basic_String_traits<Char, Traits>::trim_right(const value_type* s, const size_type n) {
		return find_last_not_of(s, n, npos, trim_map);
	}

	template<typename Char, typename Traits>
	template<typename Val, std::enable_if_t<std::is_signed_v<Val>, int>>
	Val basic_String_traits<Char, Traits>::first_to_val_helper(const value_type* s, const value_type* e,
		const value_type*& next, bool* ok, int base) {
		using UVal = std::make_unsigned_t<Val>;
		if (unlikely(base > 36))  {
			set_nok(ok);
			return static_cast<Val>(0);
		}
		bool sign = true;
		size_t pos = trim_left(s, static_cast<size_t>(e - s));
		if (pos == npos) {
			set_nok(ok);
			return static_cast<Val>(0);
		}
		next = s + pos;
		if (*next == std::integral_constant<Char, '+'>()) {
			++next;
		}
		else if (*next == std::integral_constant<Char, '-'>()) {
			sign = false;
			++next;
		}
		if (next == e || !qisdigit(*next)) {
			set_nok(ok);
			return static_cast<Val>(0);
		}
		UVal val = get_digit(*next);
		++next;
		for (; next != e && qisdigit(*next); ++next) {
			UVal digit_val = get_digit(*next);
			// if val * base + digit_val will flow,then return
			if (unlikely(val > (std::numeric_limits<UVal>::max() - digit_val) / base))  {
				set_nok(ok);
				return static_cast<Val>(0);
			}
			val = val * base + digit_val;
		}

		if (sign) {
			if (val > std::numeric_limits<Val>::max()) {
				set_nok(ok);
				return static_cast<Val>(0);
			}
			set_ok(ok);
			return static_cast<Val>(val);
		}

		if (val > static_cast<UVal>(0 - std::numeric_limits<Val>::min())) {
			set_nok(ok);
			return static_cast<Val>(0);
		}
		set_ok(ok);
		return static_cast<Val>(0 - val);
	}

	template<typename Char, typename Traits>
	template<typename UVal, std::enable_if_t<std::is_unsigned_v<UVal>, int>>
	UVal basic_String_traits<Char, Traits>::first_to_val_helper(const value_type* s, const value_type* e,
		const value_type*& next, bool* ok, int base) {
		if (unlikely(base > 36))  {
			set_nok(ok);
			return static_cast<UVal>(0);
		}
		size_t pos = trim_left(s, static_cast<size_t>(e - s));
		if (pos == npos) {
			set_nok(ok);
			return static_cast<UVal>(0);
		}
		next = s + pos;
		if (*next == std::integral_constant<Char, '+'>()) {
			++next;
		}
		if (next == e || !qisdigit(*next)) {
			set_nok(ok);
			return static_cast<UVal>(0);
		}
		UVal val = get_digit(*next);
		++next;
		for (; next != e && qisdigit(*next); ++next) {
			UVal digit_val = get_digit(*next);
			if (val > (std::numeric_limits<UVal>::max() - digit_val) / base) {
				set_nok(ok);
				return static_cast<UVal>(0);
			}
			val = val * base + digit_val;
		}

		set_ok(ok);
		return val;
	}

	template<typename Char, typename Traits>
	int basic_String_traits<Char, Traits>::
		first_to_int(const value_type* s, const value_type* e,
			const value_type*& next, bool* ok, int base) {
		return first_to_val_helper<int>(s, e, next, ok, base);
	}

	template<typename Char, typename Traits>
	unsigned int basic_String_traits<Char, Traits>::
		first_to_uint(const value_type* s, const value_type* e,
			const value_type*& next, bool* ok, int base) {
		return first_to_val_helper<unsigned int>(s, e, next, ok, base);
	}

	template<typename Char, typename Traits>
	long long basic_String_traits<Char, Traits>::
		first_to_ll(const value_type* s, const value_type* e,
			const value_type*& next, bool* ok, int base) {
		return first_to_val_helper<long long>(s, e, next, ok, base);
	}

	template<typename Char, typename Traits>
	unsigned long long basic_String_traits<Char, Traits>::
		first_to_ull(const value_type* s, const value_type* e,
			const value_type*& next, bool* ok, int base) {
		return first_to_val_helper<unsigned long long>(s, e, next, ok, base);
	}

	template<typename Char, typename Traits>
	template<typename Val, std::enable_if_t<std::is_signed_v<Val>, int>>
	Val basic_String_traits<Char, Traits>::range_to_val_helper(
		const value_type* s, const value_type* e,
		bool* ok, int base) {
		using UVal = std::make_unsigned_t<Val>;
		if (unlikely(base > 36))  {
			set_nok(ok);
			return static_cast<Val>(0);
		}
		bool sign = true;
		size_t pos = trim_left(s, static_cast<size_t>(e - s));
		size_t end_pos = trim_right(s, static_cast<size_t>(e - s));
		e = s + end_pos + 1;
		s = s + pos;
		if (*s == std::integral_constant<Char, '+'>()) {
			++s;
		}
		else if (*s == std::integral_constant<Char, '-'>()) {
			sign = false;
			++s;
		}
		if (s == e || !qisdigit(*s)) {
			set_nok(ok);
			return static_cast<Val>(0);
		}
		UVal val = get_digit(*s);
		++s;
		for (; s != e && qisdigit(*s); ++s) {
			UVal digit_val = get_digit(*s);
			if (val > (std::numeric_limits<UVal>::max() - digit_val) / base) {
				set_nok(ok);
				return static_cast<Val>(0);
			}
			val = val * base + digit_val;
		}

		if (sign) {
			if (s != e || val > std::numeric_limits<Val>::max()) {
				set_nok(ok);
				return static_cast<Val>(0);
			}
			set_ok(ok);
			return static_cast<Val>(val);
		}

		if (s != e || val > static_cast<UVal>(0 - std::numeric_limits<Val>::min())) {
			set_nok(ok);
			return static_cast<Val>(0);
		}
		set_ok(ok);
		return static_cast<Val>(0 - val);
	}

	template<typename Char, typename Traits>
	template<typename UVal, std::enable_if_t<std::is_unsigned_v<UVal>, int>>
	UVal basic_String_traits<Char, Traits>::range_to_val_helper(
		const value_type* s, const value_type* e,
		bool* ok, int base) {
		if (unlikely(base > 36))  {
			set_nok(ok);
			return static_cast<UVal>(0);
		}
		size_t pos = trim_left(s, static_cast<size_t>(e - s));
		size_t end_pos = trim_right(s, static_cast<size_t>(e - s));
		e = s + end_pos + 1;
		s = s + pos;
		if (*s == std::integral_constant<Char, '+'>()) {
			++s;
		}
		if (s == e || !qisdigit(*s)) {
			set_nok(ok);
			return static_cast<UVal>(0);
		}
		UVal val = get_digit(*s);
		++s;
		for (; s != e && qisdigit(*s); ++s) {
			UVal digit_val = get_digit(*s);
			if (val > (std::numeric_limits<UVal>::max() - digit_val) / base) {
				set_nok(ok);
				return static_cast<UVal>(0);
			}
			val = val * base + digit_val;
		}
		if (s != e) {
			set_nok(ok);
			return static_cast<UVal>(0);
		}

		set_ok(ok);
		return val;
	}

	template<typename Char, typename Traits>
	int basic_String_traits<Char, Traits>::
		range_to_int(const value_type* s, const value_type* e,
			bool* ok, int base) {
		return range_to_val_helper<int>(s, e, ok, base);
	}

	template<typename Char, typename Traits>
	unsigned int basic_String_traits<Char, Traits>::
		range_to_uint(const value_type* s, const value_type* e,
			bool* ok, int base) {
		return range_to_val_helper<unsigned int>(s, e, ok, base);
	}

	template<typename Char, typename Traits>
	long long basic_String_traits<Char, Traits>::
		range_to_ll(const value_type* s, const value_type* e,
			bool* ok, int base) {
		return range_to_val_helper<long long>(s, e, ok, base);
	}

	template<typename Char, typename Traits>
	unsigned long long basic_String_traits<Char, Traits>::
		range_to_ull(const value_type* s, const value_type* e,
			bool* ok, int base) {
		return range_to_val_helper<unsigned long long>(s, e, ok, base);
	}

	template<typename Char, typename Traits>
	template<typename Val, std::enable_if_t<std::is_signed_v<Val>, int>>
	Val basic_String_traits<Char, Traits>::
		unsafe_first_to_val_helper(const value_type* s, const value_type* e, const value_type*& next, int base) {
		using UVal = std::make_unsigned_t<Val>;
		if (unlikely(base > 36))  {
			return static_cast<Val>(0);
		}
		bool sign = true;
		size_t pos = trim_left(s, static_cast<size_t>(e - s));
		next = s + pos;
		if (*next == std::integral_constant<Char, '+'>()) {
			++next;
		}
		else if (*next == std::integral_constant<Char, '-'>()) {
			sign = false;
			++next;
		}
		UVal val = get_digit(*next);
		++next;
		for (; next != e && qisdigit(*next); ++next) {
			UVal digit_val = get_digit(*next);
			val = val * base + digit_val;
		}

		if (sign) {
			return static_cast<Val>(val);
		}

		return static_cast<Val>(0 - val);
	}

	template<typename Char, typename Traits>
	template<typename UVal, std::enable_if_t<std::is_unsigned_v<UVal>, int>>
	UVal basic_String_traits<Char, Traits>::
		unsafe_first_to_val_helper(const value_type* s, const value_type* e, const value_type*& next, int base) {
		if (unlikely(base > 36))  {
			return static_cast<UVal>(0);
		}
		size_t pos = trim_left(s, static_cast<size_t>(e - s));
		next = s + pos;
		if (*next == std::integral_constant<Char, '+'>()) {
			++next;
		}
		UVal val = get_digit(*next);
		++next;
		for (; next != e && qisdigit(*next); ++next) {
			UVal digit_val = get_digit(*next);
			val = val * base + digit_val;
		}
		return val;
	}

	template<typename Char, typename Traits>
	int basic_String_traits<Char, Traits>::
		unsafe_first_to_int(const value_type* s, const value_type* e, const value_type*& next, int base) {
		return unsafe_first_to_val_helper<int>(s, e, next, base);
	}

	template<typename Char, typename Traits>
	unsigned int basic_String_traits<Char, Traits>::
		unsafe_first_to_uint(const value_type* s, const value_type* e, const value_type*& next, int base) {
		return unsafe_first_to_val_helper<unsigned int>(s, e, next, base);
	}

	template<typename Char, typename Traits>
	long long basic_String_traits<Char, Traits>::
		unsafe_first_to_ll(const value_type* s, const value_type* e, const value_type*& next, int base) {
		return unsafe_first_to_val_helper<long long>(s, e, next, base);
	}

	template<typename Char, typename Traits>
	unsigned long long basic_String_traits<Char, Traits>::
		unsafe_first_to_ull(const value_type* s, const value_type* e, const value_type*& next, int base) {
		return unsafe_first_to_val_helper<unsigned long long>(s, e, next, base);
	}

	template<typename Char, typename Traits>
	template<typename Val, std::enable_if_t<std::is_signed_v<Val>, int>>
	Val basic_String_traits<Char, Traits>::
		unsafe_range_to_val_helper(const value_type* s, const value_type* e, int base) {
		using UVal = std::make_unsigned_t<Val>;
		if (unlikely(base > 36))  {
			return static_cast<Val>(0);
		}
		bool sign = true;
		size_t pos = trim_left(s, static_cast<size_t>(e - s));
		size_t end_pos = trim_right(s, static_cast<size_t>(e - s));
		e = s + end_pos + 1;
		s = s + pos;
		if (*s == std::integral_constant<Char, '+'>()) {
			++s;
		}
		else if (*s == std::integral_constant<Char, '-'>()) {
			sign = false;
			++s;
		}
		UVal val = get_digit(*s);
		++s;
		for (; s != e && qisdigit(*s); ++s) {
			UVal digit_val = get_digit(*s);
			val = val * base + digit_val;
		}

		if (sign) {
			return static_cast<Val>(val);
		}

		return static_cast<Val>(0 - val);
	}

	template<typename Char, typename Traits>
	template<typename UVal, std::enable_if_t<std::is_unsigned_v<UVal>, int>>
	UVal basic_String_traits<Char, Traits>::
		unsafe_range_to_val_helper(const value_type* s, const value_type* e, int base) {
		if (unlikely(base > 36))  {
			return static_cast<UVal>(0);
		}
		size_t pos = trim_left(s, static_cast<size_t>(e - s));
		size_t end_pos = trim_right(s, static_cast<size_t>(e - s));
		e = s + end_pos + 1;
		s = s + pos;
		if (*s == std::integral_constant<Char, '+'>()) {
			++s;
		}
		if (s == e) {
			return static_cast<UVal>(0);
		}
		UVal val = get_digit(*s);
		++s;
		for (; s != e && qisdigit(*s); ++s) {
			UVal digit_val = get_digit(*s);
			val = val * base + digit_val;
		}
		return val;
	}

	template<typename Char, typename Traits>
	int basic_String_traits<Char, Traits>::
		unsafe_range_to_int(const value_type* s, const value_type* e, int base) {
		return unsafe_range_to_val_helper<int>(s, e, base);
	}

	template<typename Char, typename Traits>
	unsigned int basic_String_traits<Char, Traits>::
		unsafe_range_to_uint(const value_type* s, const value_type* e, int base) {
		return unsafe_range_to_val_helper<unsigned int>(s, e, base);
	}

	template<typename Char, typename Traits>
	long long basic_String_traits<Char, Traits>::
		unsafe_range_to_ll(const value_type* s, const value_type* e, int base) {
		return unsafe_range_to_val_helper<long long>(s, e, base);
	}

	template<typename Char, typename Traits>
	unsigned long long basic_String_traits<Char, Traits>::
		unsafe_range_to_ull(const value_type* s, const value_type* e, int base) {
		return unsafe_range_to_val_helper<unsigned long long>(s, e, base);
	}

	template<typename T>
	constexpr static size_t max_SSO_size = 4;

	template<typename Char, typename Traits, typename Core>
	constexpr static size_t max_SSO_size<basic_String<Char, Traits, Core>> = Core::max_small_size();

	template<typename Char, typename Traits>
	class basic_String_view {
	private:
		using default_traits     = basic_String_traits<Char, Traits>;
		template<typename T>
		using non_default_traits = basic_String_traits<Char, T>;

		using Elem               = Char;
		template <class iter>
		using is_char_ptr        =
			std::bool_constant<wjr_is_any_of_v<iter, const Elem* const, const Elem*, Elem* const, Elem*>>;

		// is const iterator -> char pointer
		template<typename iter>
		using is_const_iterator  = std::enable_if_t<is_char_ptr<iter>::value, int>;

	public:

		using traits_type            = Traits;
		using allocator_type         = mallocator<Char>;
		using string_traits          = default_traits;

		using value_type             = Char;
		using reference              = value_type&;
		using const_reference        = const value_type&;

		using size_type              = size_t;
		using difference_type        = size_t;

		using pointer                = value_type*;
		using const_pointer          = const value_type*;

		using iterator               = value_type*;
		using const_iterator         = const value_type*;
		using reverse_iterator       = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		using case_insensitive       = case_insensitive_traits<Traits>;
		template<typename Other>
		using default_convert_type   = std::codecvt<Char, Other, mbstate_t>;
		
		constexpr static size_type npos = default_traits::npos;

	private:
		constexpr static size_type traits_length(const value_type* s) {
			return default_traits::traits_length(s);
		}
		constexpr static size_type npos_min(size_type n, const size_type x) {
			return default_traits::npos_min(n, x);
		}

	public:

		constexpr basic_String_view() noexcept = default;
		constexpr basic_String_view(const basic_String_view&) noexcept = default;

		constexpr basic_String_view(std::basic_string_view<Char, Traits> s) noexcept
			: Myfirst(s.data()), Mysize(s.size()) {
		}

		template<typename A>
		constexpr basic_String_view(const std::basic_string<Char, Traits, A>& s)
			: Myfirst(s.data()), Mysize(s.size()) {
		}

		constexpr basic_String_view(const const_pointer s) noexcept
			: Myfirst(s), Mysize(traits_length(s)) {
		}

		constexpr basic_String_view(const const_pointer s, const size_type n) noexcept
			: Myfirst(s), Mysize(n) {
		}

		constexpr basic_String_view& operator=(const basic_String_view&) noexcept = default;

		constexpr operator std::basic_string_view<Char, Traits>()const noexcept {
			return { Myfirst,Mysize };
		}

		constexpr const_iterator begin()const noexcept {
			return Myfirst;
		}

		constexpr const_iterator cbegin()const noexcept {
			return Myfirst;
		}

		constexpr const_iterator end()const noexcept {
			return Myfirst + Mysize;
		}

		constexpr const_iterator cend()const noexcept {
			return Myfirst + Mysize;
		}

		constexpr const_reverse_iterator rbegin()const noexcept {
			return const_reverse_iterator(end());
		}

		constexpr const_reverse_iterator crbegin()const noexcept {
			return const_reverse_iterator(cend());
		}

		constexpr const_reverse_iterator rend()const noexcept {
			return const_reverse_iterator(begin());
		}

		constexpr const_reverse_iterator crend()const noexcept {
			return const_reverse_iterator(begin());
		}

		constexpr const_reference front()const noexcept {
			return *begin();
		}

		constexpr const_reference back()const noexcept {
			assert(!empty());
			return *(end() - 1);
		}

		constexpr size_type size() const noexcept {
			return Mysize;
		}

		constexpr size_type length() const noexcept {
			return Mysize;
		}

		constexpr size_type max_size() const noexcept { return std::numeric_limits<size_type>::max(); }

		constexpr const_reference operator[](const size_type index)const noexcept {
			assert(0 <= index && index < size());
			return *(begin() + index);
		}

		constexpr const_reference at(const size_type index)const noexcept {
			assert(0 <= index && index < size());
			return *(begin() + index);
		}

		constexpr void remove_prefix(const size_type n)noexcept {
			Myfirst += n;
			Mysize -= n;
		}

		constexpr void remove_suffix(const size_type n)noexcept {
			Mysize -= n;
		}

		constexpr void swap(basic_String_view& other)noexcept {
			const basic_String_view temp(*this);
			*this = other;
			other = temp;
		}

		constexpr const value_type* c_str() const noexcept {
			return Myfirst;
		}

		constexpr const value_type* data() const noexcept {
			return Myfirst;
		}

		constexpr bool empty()const noexcept {
			return !Mysize;
		}

		template<typename T = Traits>
		constexpr int compare(const basic_String_view& other)const noexcept {
			return non_default_traits<T>::compare(data(), size(), other.data(), other.size());
		}

		template<typename T = Traits>
		constexpr bool equal(const basic_String_view& other)const noexcept {
			return non_default_traits<T>::equal(data(), size(), other.data(), other.size());
		}

		template<typename T = Traits>
		constexpr bool equal(const value_type* s)const noexcept {
			return equal<T>(s, traits_length(s));
		}

		template<typename T = Traits>
		constexpr bool equal(const value_type* s, const size_type n)const noexcept {
			return non_default_traits<T>::equal(data(), size(), s, n);
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_helper
			get_find_helper(const value_type* s, const value_type* e) {
			return non_default_traits<T>::get_find_helper(s, e);
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_helper
			get_find_helper(const value_type* s, const size_type n) {
			return get_find_helper<T>(s, s + n);
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_helper
			get_find_helper(const value_type* s) {
			return get_find_helper<T>(s, s + traits_length(s));
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_helper
			get_find_helper(const basic_String_view& s) {
			return get_find_helper<T>(s.data(), s.size());
		}

		template<typename T = Traits>
		constexpr size_type find(const value_type ch, size_type off = 0)const noexcept {
			return non_default_traits<T>::find(data(), size(), off, ch);
		}

		template<typename T = Traits>
		constexpr size_type find(const basic_String_view& other, size_type off = 0)const noexcept {
			return non_default_traits<T>::find(data(), size(), off, other.data(), other.size());
		}

		template<typename T = Traits>
		constexpr size_type find(const typename non_default_traits<T>::
			string_find_helper& srch, size_type off = 0)const noexcept {
			return non_default_traits<T>::find(data(), size(), off, srch);
		}

		template<typename T = Traits>
		constexpr size_type find(const value_type* s, size_type off = 0)const noexcept {
			return find<T>(s, off, traits_length(s));
		}

		template<typename T = Traits>
		constexpr size_type find(const value_type* s, size_type off, const size_type n)const noexcept {
			return non_default_traits<T>::find(data(), size(), off, s, n);
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_rfind_helper
			get_rfind_helper(const value_type* s, const value_type* e) {
			return non_default_traits<T>::get_rfind_helper(s, e);
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_rfind_helper
			get_rfind_helper(const value_type* s, const size_type n) {
			return get_rfind_helper<T>(s, s + n);
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_helper
			get_rfind_helper(const value_type* s) {
			return get_rfind_helper<T>(s, s + traits_length(s));
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_helper
			get_rfind_helper(const basic_String_view& s) {
			return get_rfind_helper<T>(s.data(), s.size());
		}

		template<typename T = Traits>
		constexpr size_type rfind(const value_type ch, size_type off = npos)const noexcept {
			return non_default_traits<T>::rfind(data(), size(), off, ch);
		}

		template<typename T = Traits>
		constexpr size_type rfind(const basic_String_view& other, size_type off = npos)const noexcept {
			return non_default_traits<T>::rfind(data(), size(), off, other.data(), other.size());
		}

		template<typename T = Traits>
		constexpr size_type rfind(const typename non_default_traits<T>::
			string_rfind_helper& srch, size_type off = npos)const noexcept {
			return non_default_traits<T>::rfind(data(), size(), off, srch);
		}

		template<typename T = Traits>
		constexpr size_type rfind(const value_type* s, size_type off = npos)const noexcept {
			return rfind<T>(s, off, traits_length(s));
		}

		template<typename T = Traits>
		constexpr size_type rfind(const value_type* s, size_type off, const size_type n)const noexcept {
			return non_default_traits<T>::rfind(data(), size(), off, s, n);
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_of_helper get_find_of_helper(
			const value_type* s, const value_type* e
		) {
			return non_default_traits<T>::get_find_of_helper(s, e);
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_of_helper get_find_of_helper(
			const value_type* s, const size_type n
		) {
			return get_find_of_helper<T>(s, s + n);
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_of_helper get_find_of_helper(
			const value_type* s
		) {
			return get_find_of_helper<T>(s, s + traits_length(s));
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_of_helper get_find_of_helper(
			const basic_String_view& s
		) {
			return get_find_of_helper<T>(s.data(), s.size());
		}

		template<typename T = Traits>
		constexpr size_type find_first_of(const value_type ch, size_type off = 0)const noexcept {
			return non_default_traits<T>::find_first_of_ch(data(), size(), off, ch);
		}

		template<typename T = Traits>
		constexpr size_type find_first_of(const basic_String_view& other, size_type off = 0)const noexcept {
			return non_default_traits<T>::find_first_of(data(), size(), off, other.data(), other.size());
		}

		template<typename T = Traits>
		constexpr size_type find_first_of(const typename non_default_traits<T>::
			string_find_of_helper& srch, size_type off = 0)const noexcept {
			return non_default_traits<T>::find_first_of(data(), size(), off, srch);
		}

		template<typename T = Traits>
		constexpr size_type find_first_of(const value_type* s, size_type off = 0)const noexcept {
			return find_first_of<T>(s, off, traits_length(s));
		}

		template<typename T = Traits>
		constexpr size_type find_first_of(const value_type* s, size_type off, const size_type n)const noexcept {
			return non_default_traits<T>::find_first_of(data(), size(), off, s, n);
		}

		template<typename T = Traits>
		constexpr size_type find_last_of(const value_type ch, size_type off = npos)const noexcept {
			return non_default_traits<T>::find_last_of_ch(data(), size(), off, ch);
		}

		template<typename T = Traits>
		constexpr size_type find_last_of(const basic_String_view& other, size_type off = npos)const noexcept {
			return non_default_traits<T>::find_last_of(data(), size(), off, other.data(), other.size());
		}

		template<typename T = Traits>
		constexpr size_type find_last_of(const typename non_default_traits<T>::
			string_find_of_helper& srch, size_type off = npos)const noexcept {
			return non_default_traits<T>::find_last_of(data(), size(), off, srch);
		}

		template<typename T = Traits>
		constexpr size_type find_last_of(const value_type* s, size_type off = npos)const noexcept {
			return find_last_of<T>(s, off, traits_length(s));
		}

		template<typename T = Traits>
		constexpr size_type find_last_of(const value_type* s, size_type off, const size_type n)const noexcept {
			return non_default_traits<T>::find_last_of(data(), size(), off, s, n);
		}

		template<typename T = Traits>
		constexpr size_type find_first_not_of(const value_type ch, size_type off = 0)const noexcept {
			return non_default_traits<T>::find_first_not_of_ch(data(), size(), off, ch);
		}

		template<typename T = Traits>
		constexpr size_type find_first_not_of(const basic_String_view& other, size_type off = 0)const noexcept {
			return non_default_traits<T>::find_first_not_of(data(), size(), off, other.data(), other.size());
		}

		template<typename T = Traits>
		constexpr size_type find_first_not_of(const typename non_default_traits<T>::
			string_find_of_helper& srch, size_type off = 0)const noexcept {
			return non_default_traits<T>::find_first_not_of(data(), size(), off, srch);
		}

		template<typename T = Traits>
		constexpr size_type find_first_not_of(const value_type* s, size_type off = 0)const noexcept {
			return find_first_not_of<T>(s, off, traits_length(s));
		}

		template<typename T = Traits>
		constexpr size_type find_first_not_of(const value_type* s, size_type off, const size_type n)const noexcept {
			return non_default_traits<T>::find_first_not_of(data(), size(), off, s, n);
		}

		template<typename T = Traits>
		constexpr size_type find_last_not_of(const value_type ch, size_type off = npos)const noexcept {
			return non_default_traits<T>::find_last_not_of_ch(data(), size(), off, ch);
		}

		template<typename T = Traits>
		constexpr size_type find_last_not_of(const basic_String_view& other, size_type off = npos)const noexcept {
			return non_default_traits<T>::find_last_not_of(data(), size(), off, other.data(), other.size());
		}

		template<typename T = Traits>
		constexpr size_type find_last_not_of(const typename non_default_traits<T>::
			string_find_of_helper& srch, size_type off = npos)const noexcept {
			return non_default_traits<T>::find_last_not_of(data(), size(), off, srch);
		}

		template<typename T = Traits>
		constexpr size_type find_last_not_of(const value_type* s, size_type off = npos)const noexcept {
			return find_last_not_of<T>(s, off, traits_length(s));
		}

		template<typename T = Traits>
		constexpr size_type find_last_not_of(const value_type* s, size_type off, const size_type n)const noexcept {
			return non_default_traits<T>::find_last_not_of(data(), size(), off, s, n);
		}

		template<typename T = Traits>
		constexpr bool starts_with(const value_type ch)const noexcept {
			return non_default_traits<T>::starts_with(data(), size(), ch);
		}

		template<typename T = Traits>
		constexpr bool starts_with(const basic_String_view& other)const noexcept {
			return non_default_traits<T>::starts_with(data(), size(), other.data(), other.size());
		}

		template<typename T = Traits>
		constexpr bool starts_with(const value_type* s)const noexcept {
			return starts_with<T>(s, traits_length(s));
		}

		template<typename T = Traits>
		constexpr bool starts_with(const value_type* s, const size_type n)const noexcept {
			return non_default_traits<T>::starts_with(data(), size(), s, n);
		}

		template<typename T = Traits>
		constexpr bool ends_with(const value_type ch)const noexcept {
			return non_default_traits<T>::ends_with(data(), size(), ch);
		}

		template<typename T = Traits>
		constexpr bool ends_with(const basic_String_view& other)const noexcept {
			return non_default_traits<T>::ends_with(data(), size(), other.data(), other.size());
		}

		template<typename T = Traits>
		constexpr bool ends_with(const value_type* s)const noexcept {
			return ends_with<T>(s, traits_length(s));
		}

		template<typename T = Traits>
		constexpr bool ends_with(const value_type* s, const size_type n)const noexcept {
			return non_default_traits<T>::ends_with(data(), size(), s, n);
		}

		constexpr basic_String_view substr(const size_type off = 0, const size_type n = npos)const noexcept {
			return { Myfirst + off , npos_min(n,size() - off) };
		}

		basic_String_view left(const size_type n) {
			return basic_String(data(), npos_min(n, size()));
		}

		basic_String_view right(size_type n) {
			const auto _size = size();
			n = npos_min(n, _size);
			return basic_String_view(data() + _size - n, n);
		}

		template<typename T = Traits, typename string_list = std::vector<basic_String_view>>
		string_list split(const value_type ch, bool keep_empty_parts = true)const noexcept;

		template<typename T = Traits, typename string_list = std::vector<basic_String_view>>
		string_list split(const basic_String_view& other, bool keep_empty_parts = true)const noexcept;

		template<typename T = Traits, typename string_list = std::vector<basic_String_view>>
		string_list split(const value_type* s, bool keep_empty_parts = true)const noexcept;

		template<typename T = Traits, typename string_list = std::vector<basic_String_view>>
		string_list split(const value_type* s,
			const size_type n, bool keep_empty_parts = true)const noexcept;

		basic_String_view trim_left()const noexcept {
			auto l = default_traits::trim_left(Myfirst, Mysize);
			if (l == npos) {
				return { Myfirst,0 };
			}
			return { Myfirst + l,Mysize - l };
		}

		basic_String_view trim_right()const noexcept {
			auto r = default_traits::trim_right(Myfirst, Mysize);
			if (r == npos) {
				return { Myfirst,0 };
			}
			return { Myfirst,r + 1 };
		}

		basic_String_view trim()const noexcept {
			auto l = default_traits::trim_left(Myfirst, Mysize);
			if (l == npos) {
				return { Myfirst,0 };
			}
			auto r = default_traits::trim_right(Myfirst, Mysize);
			return { Myfirst + l,r - l + 1 };
		}

	private:
		template<typename Val>
		Val to_val_helper(bool* ok, int base)const;
		template<typename Val>
		Val first_to_val_helper(const value_type*& next, bool* ok, int base)const;
	public:

		int to_int(bool* ok = nullptr, int base = 10)const;
		unsigned int to_uint(bool* ok = nullptr, int base = 10)const;
		long long to_ll(bool* ok = nullptr, int base = 10)const;
		unsigned long long to_ull(bool* ok = nullptr, int base = 10)const;

		int to_int(const value_type*& next, bool* ok = nullptr, int base = 10)const;
		unsigned int to_uint(const value_type*& next, bool* ok = nullptr, int base = 10)const;
		long long to_ll(const value_type*& next, bool* ok = nullptr, int base = 10)const;
		unsigned long long to_ull(const value_type*& next, bool* ok = nullptr, int base = 10)const;

		constexpr size_t hash_code()const {
			return std::hash<basic_String_view<Char, Traits>>()(*this);
		}

		template<typename T = Traits>
		constexpr bool contains(const value_type ch)const {
			return find<T>(ch) != npos;
		}

		template<typename T = Traits>
		constexpr bool contains(const basic_String_view& s)const {
			return find<T>(s) != npos;
		}

		template<typename T = Traits>
		constexpr bool contains(const typename non_default_traits<T>::string_find_helper& srch)const {
			return find<T>(srch) != npos;
		}

		template<typename T = Traits>
		constexpr bool contains(const value_type* s)const {
			return find<T>(s) != npos;
		}

		template<typename T = Traits>
		constexpr bool contains(const value_type* s, const size_type n)const {
			return find<T>(s, n) != npos;
		}

		template<typename OString, typename _codecvt =
			default_convert_type<typename OString::value_type>>
			OString convert_to(const std::locale& loc = std::locale())const {
			const _codecvt& cvt = std::use_facet<_codecvt>(loc);
			using Other = typename OString::value_type;
			using codecvt_type = _codecvt;
			const auto s = data();
			const auto len = size();
			mbstate_t it;
			OString ans;
			size_type Reserved_Capacity = std::max(
				max_SSO_size<OString>,
				len * ((sizeof(Other) + sizeof(Char) - 1) / sizeof(Other)));
			// reserve possible size
			const Char* next1;
			Other* next2;
			typename codecvt_type::result result;
			do {
				ans.reserve(Reserved_Capacity);
				memset(&it, 0, sizeof(it));
				const auto _data = ans.data();
				result = cvt.out(it, s, s + len, next1, _data, _data + Reserved_Capacity, next2);
				assert(result != codecvt_type::error);
				Reserved_Capacity <<= 1;
			} while (result == codecvt_type::partial);
			ans.set_size(next2 - ans.data());
			ans.shrink_to_fit();
			return ans;
		}

	private:
		const_pointer Myfirst;
		size_type Mysize;
	};

	template<typename Char, typename Traits>
	template<typename T, typename string_list>
	string_list basic_String_view<Char, Traits>::split(
		const value_type ch, bool keep_empty_parts)const noexcept {
		return non_default_traits<T>::template split<string_list>(data(), size(), ch, keep_empty_parts);
	}

	template<typename Char, typename Traits>
	template<typename T, typename string_list>
	string_list basic_String_view<Char, Traits>::split(
		const basic_String_view& other, bool keep_empty_parts)const noexcept {
		return split<T, string_list>(other.data(), other.size(), keep_empty_parts);
	}

	template<typename Char, typename Traits>
	template<typename T, typename string_list>
	string_list basic_String_view<Char, Traits>::split(
		const value_type* s, bool keep_empty_parts)const noexcept {
		return split<T, string_list>(s, traits_length(s), keep_empty_parts);
	}

	template<typename Char, typename Traits>
	template<typename T, typename string_list>
	string_list basic_String_view<Char, Traits>::split(
		const value_type* s,
		const size_type n, bool keep_empty_parts)const noexcept {
		return non_default_traits<T>::template split<string_list>(data(), size(), s, n, keep_empty_parts);
	}

	template<typename Char, typename Traits>
	template<typename Val>
	Val basic_String_view<Char, Traits>::to_val_helper(bool* ok, int base)const {
		const value_type* _data = data();
		const auto _size = size();
		return default_traits::template range_to_val_helper<Val>(_data, _data + _size, ok, base);
	}

	template<typename Char, typename Traits>
	int basic_String_view<Char, Traits>::to_int(bool* ok, int base)const {
		return to_val_helper<int>(ok, base);
	}

	template<typename Char, typename Traits>
	unsigned int basic_String_view<Char, Traits>::to_uint(bool* ok, int base)const {
		return to_val_helper<unsigned int>(ok, base);
	}

	template<typename Char, typename Traits>
	long long basic_String_view<Char, Traits>::to_ll(bool* ok, int base)const {
		return to_val_helper<long long>(ok, base);
	}

	template<typename Char, typename Traits>
	unsigned long long basic_String_view<Char, Traits>::to_ull(bool* ok, int base)const {
		return to_val_helper<unsigned long long>(ok, base);
	}

	template<typename Char, typename Traits>
	template<typename Val>
	Val basic_String_view<Char, Traits>::
		first_to_val_helper(const value_type*& next, bool* ok, int base)const {
		auto _data = data();
		auto _size = size();
		return default_traits::template first_to_val_helper<Val>(_data, _data + _size, next, ok, base);
	}

	template<typename Char, typename Traits>
	int basic_String_view<Char, Traits>::to_int(const value_type*& next, bool* ok, int base)const {
		return first_to_val_helper<int>(next, ok, base);
	}

	template<typename Char, typename Traits>
	unsigned int basic_String_view<Char, Traits>::
		to_uint(const value_type*& next, bool* ok, int base)const {
		return first_to_val_helper<int>(next, ok, base);
	}

	template<typename Char, typename Traits>
	long long basic_String_view<Char, Traits>::
		to_ll(const value_type*& next, bool* ok, int base)const {
		return first_to_val_helper<int>(next, ok, base);
	}

	template<typename Char, typename Traits>
	unsigned long long basic_String_view<Char, Traits>::
		to_ull(const value_type*& next, bool* ok, int base)const {
		return first_to_val_helper<int>(next, ok, base);
	}

	template<typename Char, typename Traits>
	constexpr bool operator==(
		const basic_String_view<Char, Traits> lhs,
		const basic_String_view<Char, Traits> rhs
		) {
		return lhs.equal(rhs);
	}

	template<typename Char, typename Traits>
	constexpr bool operator==(
		const basic_String_view<Char, Traits> lhs,
		const midentity_t<basic_String_view<Char, Traits>> rhs
		) {
		return lhs.equal(rhs);
	}

	template<typename Char, typename Traits>
	constexpr bool operator==(
		const midentity_t<basic_String_view<Char, Traits>> lhs,
		const basic_String_view<Char, Traits> rhs
		) {
		return lhs.equal(rhs);
	}

	template<typename Char, typename Traits>
	constexpr bool operator!=(
		const basic_String_view<Char, Traits> lhs,
		const basic_String_view<Char, Traits> rhs
		) {
		return !(lhs.equal(rhs));
	}

	template<typename Char, typename Traits>
	constexpr bool operator!=(
		const basic_String_view<Char, Traits> lhs,
		const midentity_t<basic_String_view<Char, Traits>> rhs
		) {
		return !(lhs.equal(rhs));
	}

	template<typename Char, typename Traits>
	constexpr bool operator!=(
		const midentity_t<basic_String_view<Char, Traits>> lhs,
		const basic_String_view<Char, Traits> rhs
		) {
		return !(lhs.euqal(rhs));
	}

	template<typename Char, typename Traits>
	constexpr bool operator<(
		const basic_String_view<Char, Traits> lhs,
		const basic_String_view<Char, Traits> rhs
		) {
		return lhs.compare(rhs) < 0;
	}

	template<typename Char, typename Traits>
	constexpr bool operator<(
		const basic_String_view<Char, Traits> lhs,
		const midentity_t<basic_String_view<Char, Traits>> rhs
		) {
		return lhs.compare(rhs) < 0;
	}

	template<typename Char, typename Traits>
	constexpr bool operator<(
		const midentity_t<basic_String_view<Char, Traits>> lhs,
		const basic_String_view<Char, Traits> rhs
		) {
		return lhs.compare(rhs) < 0;
	}

	template<typename Char, typename Traits>
	constexpr bool operator>(
		const basic_String_view<Char, Traits> lhs,
		const basic_String_view<Char, Traits> rhs
		) {
		return lhs.compare(rhs) > 0;
	}

	template<typename Char, typename Traits>
	constexpr bool operator>(
		const basic_String_view<Char, Traits> lhs,
		const midentity_t<basic_String_view<Char, Traits>> rhs
		) {
		return lhs.compare(rhs) > 0;
	}

	template<typename Char, typename Traits>
	constexpr bool operator>(
		const midentity_t<basic_String_view<Char, Traits>> lhs,
		const basic_String_view<Char, Traits> rhs
		) {
		return lhs.comprare(rhs) > 0;
	}

	template<typename Char, typename Traits>
	constexpr bool operator<=(
		const basic_String_view<Char, Traits> lhs,
		const basic_String_view<Char, Traits> rhs
		) {
		return lhs.compare(rhs) <= 0;
	}

	template<typename Char, typename Traits>
	constexpr bool operator<=(
		const basic_String_view<Char, Traits> lhs,
		const midentity_t<basic_String_view<Char, Traits>> rhs
		) {
		return lhs.compare(rhs) <= 0;
	}

	template<typename Char, typename Traits>
	constexpr bool operator<=(
		const midentity_t<basic_String_view<Char, Traits>> lhs,
		const basic_String_view<Char, Traits> rhs
		) {
		return lhs.compare(rhs) <= 0;
	}

	template<typename Char, typename Traits>
	constexpr bool operator>=(
		const basic_String_view<Char, Traits> lhs,
		const basic_String_view<Char, Traits> rhs
		) {
		return lhs.compare(rhs) >= 0;
	}

	template<typename Char, typename Traits>
	constexpr bool operator>=(
		const basic_String_view<Char, Traits> lhs,
		const midentity_t<basic_String_view<Char, Traits>> rhs
		) {
		return lhs.compare(rhs) >= 0;
	}

	template<typename Char, typename Traits>
	constexpr bool operator>=(
		const midentity_t<basic_String_view<Char, Traits>> lhs,
		const basic_String_view<Char, Traits>& rhs
		) {
		return lhs.compare(rhs) >= 0;
	}

	template<typename Char, typename Traits>
	constexpr void swap(basic_String_view<Char, Traits>& lhs,
		basic_String_view<Char, Traits>& rhs) noexcept{
		lhs.swap(rhs);
	}

	template <typename Char, typename Traits>
	inline std::basic_ostream<Char, Traits>&
		operator<<(
			std::basic_ostream<Char, Traits>& os,
			const basic_String_view<Char, Traits>& str) {
#ifdef _LIBCPP_VERSION
		using ostream_type = std::basic_ostream<Char, Traits>;
		typename ostream_type::sentry _s(os);
		if (_s) {
			using _Ip = std::ostreambuf_iterator<Char, Traits>;
			size_t __len = str.size();
			bool __left =
				(os.flags() & ostream_type::adjustfield) == ostream_type::left;
			if (__pad_and_output(
				_Ip(os),
				str.data(),
				__left ? str.data() + __len : str.data(),
				str.data() + __len,
				os,
				os.fill())
				.failed()) {
				os.setstate(ostream_type::badbit | ostream_type::failbit);
			}
		}
#elif defined(_MSC_VER)
		typedef decltype(os.precision()) streamsize;
		// MSVC doesn't define __ostream_insert
		os.write(str.data(), static_cast<streamsize>(str.size()));
#else
		std::__ostream_insert(os, str.data(), str.size());
#endif
		return os;
	}

	template<typename Char, typename Traits, typename Core>
	class basic_String {
	private:
		template<typename Char2,typename Traits2,typename Core2>
		friend class basic_String;

		using default_traits = basic_String_traits<Char, Traits>;
		template<typename T>
		using non_default_traits = basic_String_traits<Char, T>;

		using Elem = Char;
		template <class iter>
		using is_char_ptr =
			std::bool_constant<wjr_is_any_of_v<iter, const Elem* const, const Elem*, Elem* const, Elem*>>;

		// is const iterator -> char pointer
		template<typename iter>
		using is_const_iterator = std::enable_if_t<is_char_ptr<iter>::value, int>;

	public:

		using traits_type = Traits;
		using allocator_type = mallocator<Char>;
		using string_traits = default_traits;

		using value_type = Char;
		using reference = value_type&;
		using const_reference = const value_type&;

		using size_type = size_t;
		using difference_type = size_t;

		using pointer = value_type*;
		using const_pointer = const value_type*;

		using iterator = value_type*;
		using const_iterator = const value_type*;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		constexpr static size_type npos = default_traits::npos;

		using case_insensitive = case_insensitive_traits<Traits>;
		using core_type = Core;
		template<typename Other>
		using default_convert_type = std::codecvt<Char, Other, mbstate_t>;

	private:

		constexpr static size_type traits_length(const value_type* s) {
			return default_traits::traits_length(s);
		}

		constexpr static size_type npos_min(size_type n, const size_type x) {
			return default_traits::npos_min(n, x);
		}

	public:

		basic_String() = default;

		basic_String(const basic_String& other)
			: core(other.core) {
		}

		basic_String(basic_String&& other) noexcept
			: core(std::move(other.core)) {
		}

		basic_String(const basic_String& other, const size_type pos, const size_type n = npos)
			: core(other.data() + pos, npos_min(n, other.size() - pos)) {
		}

		basic_String(const value_type* s)
			: core(s, traits_length(s)) {
		}

		basic_String(const value_type* s, const size_type n)
			: core(s, n) {
		}

		basic_String(const value_type* first, const value_type* last)
			: core(first, static_cast<size_type>(last - first)) {
		}

		// for diffierent core and traits type of T
		template<typename T, typename C>
		explicit basic_String(const basic_String<Char, T, C>& other)
			: core(other.data(), other.size()) {
		}

		// for same core and different traits
		// only need to copy core
		template<typename T>
		explicit basic_String(const basic_String<Char, T, Core>& other)
			: core(other.core) {
		}

		// for same core and different traits
		// only need to move core,for traits won't affect the core
		template<typename T>
		explicit basic_String(basic_String<Char, T, Core>&& other) noexcept
			: core(std::move(other.core)) {
		}

		template<typename T, typename A>
		explicit basic_String(const std::basic_string<Char, T, A>& other)
			: core(other.data(), other.size()) {
		}

		explicit basic_String(const size_type n, const value_type c = value_type()) {
			auto const pData = core.expandNoinit(n);
			std::fill(pData, pData + n, c);
		}

		template<typename iter>
		basic_String(iter first, iter last) {
			assign(first, last, is_char_ptr<iter>{});
		}

		explicit basic_String(std::initializer_list<value_type> il) {
			assign(il.begin(), il.end());
		}

		explicit basic_String(const basic_String_view<Char, Traits>& s)
			: core(s.data(), s.size()) {
		}

		basic_String(const size_type len, tag::uninitialized_tag) {
			core.expandNoinit(len);
		}

		basic_String(const size_type len, tag::reserved_tag) {
			reserve(len);
		}

		template<typename Other, typename _codecvt =
			default_convert_type<Other>>
		explicit basic_String(const Other* s, const std::locale& loc = std::locale()) {
			convert_from(s, loc);
		}

		template<typename Other, typename _codecvt =
			default_convert_type<Other>>
		explicit basic_String(const Other* s, const Other* e,
			const std::locale& loc = std::locale()) {
			convert_from(s, e, loc);
		}

		template<typename Other, typename _codecvt =
			default_convert_type<Other>>
		explicit basic_String(const Other* s, const size_type len,
			const std::locale& loc = std::locale()) {
			convert_from(s, len, loc);
		}

		template<typename Other, typename T, typename C,
			typename _codecvt = default_convert_type<Other>>
		explicit basic_String(const basic_String<Other, T, C>& other,
			const std::locale& loc = std::locale()) {
			convert_from(other, loc);
		}

		template<typename Other, typename T, typename _codecvt =
			default_convert_type<Other>>
		explicit basic_String(const basic_String_view<Other, T>& other,
			const std::locale& loc = std::locale()) {
			convert_from(other, loc);
		}

		template<typename Other, typename T, typename A,
			typename _codecvt = default_convert_type<Other>>
		explicit basic_String(const std::basic_string<Other, T, A>& other,
			const std::locale& loc = std::locale()) {
			convert_from(other, loc);
		}

		template<typename Other, typename T, typename _codecvt =
			default_convert_type<Other>>
		explicit basic_String(const std::basic_string_view<Other, T>& other,
			const std::locale& loc = std::locale()) {
			convert_from(other, loc);
		}

		explicit operator std::basic_string_view<Char, Traits>() const noexcept {
			return { data(),size() };
		}

		operator basic_String_view<Char, Traits>() const noexcept {
			return { data(), size() };
		}

		~basic_String() noexcept = default;

		basic_String& operator=(const basic_String& other) {
			if (unlikely(this == std::addressof(other)))  {
				return *this;
			}
			return assign(other.data(), other.size());
		}

		basic_String& operator=(basic_String&& other)noexcept {
			if (unlikely(this == std::addressof(other)))  {
				return *this;
			}
			this->~basic_String();
			new (&core) Core(std::move(other.core));
			return *this;
		}

		template<typename T, typename C>
		basic_String& operator=(const basic_String<Char, T, C>& other) {
			if (unlikely(this == std::addressof(other)))  {
				return *this;
			}
			return assign(other.data(), other.size());
		}

		template<typename T>
		basic_String& operator=(const basic_String<Char, T, Core>& other) {
			if (unlikely(this == std::addressof(other)))  {
				return *this;
			}
			return assign(other.data(), other.size());
		}

		template<typename T>
		basic_String& operator=(basic_String<Char, T, Core>&& other) noexcept {
			if (unlikely(this == std::addressof(other)))  {
				return *this;
			}
			this->~basic_String();
			new (&core) Core(std::move(other.core));
			return *this;
		}

		basic_String& operator=(const value_type* str) {
			return assign(str);
		}

		template<typename _Traits, typename _Alloc>
		basic_String& operator=(const std::basic_string<Char, _Traits, _Alloc>& str) {
			return assign(str.data(), str.size());
		}

		basic_String& operator=(const value_type c) {
			resize(1);
			*begin() = c;
			return *this;
		}

		basic_String& operator=(const basic_String_view<Char, Traits>& s) {
			return assign(s.data(), s.size());
		}

		iterator begin() { return core.mutableData(); }
		const_iterator begin()const { return core.data(); }
		const_iterator cbegin()const { return begin(); }

		iterator end() { return core.mutableData() + core.size(); }
		const_iterator end()const { return core.data() + core.size(); }
		const_iterator cend()const { return end(); }

		reverse_iterator rbegin() { return reverse_iterator(end()); }
		const_reverse_iterator rbegin()const { return const_reverse_iterator(end()); }
		const_reverse_iterator crbegin()const { return const_reverse_iterator(cend()); }

		reverse_iterator rend() { return reverse_iterator(begin()); }
		const_reverse_iterator rend()const { return const_reverse_iterator(begin()); }
		const_reverse_iterator crend()const { return const_reverse_iterator(begin()); }

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

		void resize(const size_type n, const value_type c = value_type());

		void reserve(const size_type c) {
			core.reserve(c);
			assert(capacity() >= c);
		}

		void shrink_to_fit() {
			core.shrink_to_fit();
		}

		void clear() {
			resize(0);
		}

		size_type size() const {
			return core.size();
		}

		// set size() without init
		void set_size(const size_type s) {
			assert(s <= capacity());
			core.setSize(s);
		}

		size_type length() const {
			return core.size();
		}

		size_type max_size() const { return std::numeric_limits<size_type>::max(); }

		size_type capacity() const {
			return core.capacity();
		}

		reference operator[](const size_type index) {
			assert(0 <= index && index < size());
			return *(begin() + index);
		}

		const_reference operator[](const size_type index)const {
			assert(0 <= index && index < size());
			return *(begin() + index);
		}

		reference at(const size_type index) {
			assert(0 <= index && index < size());
			return *(begin() + index);
		}

		const_reference at(const size_type index)const {
			assert(0 <= index && index < size());
			return *(begin() + index);
		}

		basic_String& operator+=(const basic_String& other) { return append(other); }
		basic_String& operator+=(const value_type* s) { return append(s); }
		basic_String& operator+=(const value_type c) { push_back(c); return *this; }
		basic_String& operator+=(std::initializer_list<value_type> il) {
			append(il);
			return *this;
		}

		basic_String& operator+=(const basic_String_view<Char, Traits>& s) {
			return append(s.data(), s.size());
		}

		basic_String& append(const basic_String& other) {
			return append(other.data(), other.size());
		}

		basic_String& append(
			const basic_String& other, const size_type pos, const size_type n = npos) {
			return append(other.data() + pos, npos_min(n, other.size() - pos));
		}

		basic_String& append(const value_type* s, size_type n);

		basic_String& append(const value_type* s) {
			return append(s, traits_length(s));
		}

		basic_String& append(const size_type n, const value_type c) {
			auto pData = core.expandNoinit(n, true);
			std::fill(pData, pData + n, c);
			return *this;
		}

		basic_String& append(const value_type* s, const value_type* e) {
			return append(s, static_cast<size_type>(e - s));
		}

	private:
		template<typename iter>
		basic_String& append(iter first, iter last, std::true_type) {
			return append((const value_type*)first, (const value_type*)last);
		}

		template<typename iter>
		basic_String& append(iter first, iter last, std::false_type) {
			return append(basic_String(first, last));
		}
	public:

		template<typename iter>
		basic_String& append(iter first, iter last) {
			return append(first, last, is_char_ptr<iter>{});
		}

		basic_String& append(std::initializer_list<value_type> il) {
			return append(il.begin(), il.end());
		}

		basic_String& append(const value_type c) {
			push_back(c);
			return *this;
		}

		basic_String& append(const basic_String_view<Char, Traits>& s) {
			return append(s.data(), s.size());
		}

		basic_String& prepend(const basic_String& other) {
			return prepend(other.data(), other.size());
		}

		basic_String& prepend(const basic_String& other, const size_type pos, const size_type n = npos) {
			return prepend(other.data() + pos, npos_min(n, other.size() - pos));
		}

		basic_String& prepend(const value_type* s, size_type n);

		basic_String& prepend(const value_type* s) {
			return prepend(s, traits_length(s));
		}

		basic_String& prepend(const size_type n, const value_type c) {
			auto _size = size();
			core.expandNoinit(n, true);
			auto _data = data();
			memmove(_data + n, _data, sizeof(value_type) * _size);
			std::fill(_data, _data + n, c);
			return *this;
		}

		basic_String& prepend(const value_type* s, const value_type* e) {
			return prepend(s, static_cast<size_type>(e - s));
		}

	private:
		template<typename iter>
		basic_String& prepend(iter first, iter last, std::true_type) {
			return prepend((const value_type*)first, (const value_type*)last);
		}

		template<typename iter>
		basic_String& prepend(iter first, iter last, std::false_type) {
			return prepend(basic_String(first, last));
		}

	public:
		template<typename iter>
		basic_String& prepend(iter first, iter last) {
			return prepend(first, last, is_char_ptr<iter>{});
		}

		basic_String& prepend(std::initializer_list<value_type> il) {
			return prepend(il.begin(), il.end());
		}

		basic_String& prepend(const value_type c) {
			return prepend(1, c);
		}

		basic_String& prepend(const basic_String_view<Char, Traits>& s) {
			return prepend(s.data(), s.size());
		}

		void push_back(const value_type c) {
			core.push_back(c);
		}

		void push_front(const value_type c) {
			prepend(1, c);
		}

		void pop_back() {
			assert(!empty());
			core.shrink(1);
		}

		basic_String& assign(const basic_String& other) {
			return (*this) = other;
		}

		basic_String& assign(basic_String&& other) noexcept {
			return (*this) = std::move(other);
		}

		basic_String& assign(const basic_String& other, const size_type pos, const size_type n = npos) {
			return assign(other.data() + pos, npos_min(n, other.size() - pos));
		}

		basic_String& assign(const value_type* s, const value_type* e) {
			return assign(s, static_cast<size_type>(e - s));
		}

		basic_String& assign(const value_type* s) {
			assign(s, traits_length(s));
			return *this;
		}

		basic_String& assign(const value_type* s, const size_type n);

	private:
		template<typename iter>
		basic_String& assign(iter first, iter last, std::input_iterator_tag) {
			clear();
			for (; first != last; ++first)
				push_back(*first);
			return *this;
		}

		template<typename iter>
		basic_String& assign(iter first, iter last, std::forward_iterator_tag) {
			resize(static_cast<size_type>(std::distance(first, last)));
			auto _data = data();
			for (; first != last; ++_data, ++first)
				*_data = *first;
			return *this;
		}

		template<typename iter>
		basic_String& assign(iter first, iter last, std::true_type) {
			return assign(static_cast<const value_type*>(first), static_cast<size_type>(last - first));
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

		basic_String& assign(const basic_String_view<Char, Traits>& s) {
			return assign(s.data(), s.size());
		}

		basic_String& insert(const size_type pos, const basic_String& str) {
			return insert(pos, str.data(), str.size());
		}

		basic_String& insert(const size_type pos1,
			const basic_String& s, const size_type pos2, const size_type n = npos) {
			return insert(pos1, s.data() + pos2, npos_min(n, s.size() - pos2));
		}

		basic_String& insert(const size_type pos, const value_type* s, const size_type n);

		basic_String& insert(const size_type pos, const value_type* s, const value_type* e) {
			return insert(pos, s, static_cast<size_type>(e - s));
		}

		basic_String& insert(const size_type pos, const value_type* str) {
			return insert(pos, str, traits_length(str));
		}

		basic_String& insert(const size_type pos, const value_type c) {
			return insert(pos, c, 1);
		}

		basic_String& insert(const size_type pos, const size_type n, const value_type c);

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
			return insert(pos, il.begin(), il.end());
		}

		template<typename T, is_const_iterator<T> = 0>
		iterator insert(T p, const basic_String& str) {
			const size_type pos = p - data();
			insert(pos, str);
			return data() + pos;
		}

		template<typename T, is_const_iterator<T> = 0>
		iterator insert(T p,
			const basic_String& s, const size_type pos2, const size_type n = npos) {
			const size_type pos1 = p - data();
			insert(pos1, s, pos2, n);
			return data() + pos1;
		}

		template<typename T, is_const_iterator<T> = 0>
		iterator insert(T p, const value_type* s, const size_type n) {
			const size_type pos = p - data();
			insert(pos, s, n);
			return data() + pos;
		}

		template<typename T, is_const_iterator<T> = 0>
		iterator insert(T p, const value_type* s, const value_type* e) {
			const size_type pos = p - data();
			insert(pos, s, e);
			return data() + pos;
		}

		template<typename T, is_const_iterator<T> = 0>
		iterator insert(T p, const value_type* s) {
			const size_type pos = p - data();
			insert(pos, s);
			return data() + pos;
		}

		template<typename T, is_const_iterator<T> = 0>
		iterator insert(T p, const value_type c) {
			const size_type pos = p - data();
			insert(pos, c);
			return data() + pos;
		}

		template<typename T, is_const_iterator<T> = 0>
		iterator insert(T p, const size_type n, const value_type c) {
			const size_type pos = p - data();
			insert(pos, n, c);
			return data() + pos;
		}

		template<typename T, typename iter, is_const_iterator<T> = 0>
		iterator insert(T p, iter first, iter last) {
			const size_type pos = p - data();
			insert(pos, first, last);
			return data() + pos;
		}

		template<typename T, is_const_iterator<T> = 0>
		iterator insert(T p, std::initializer_list<value_type> il) {
			const size_type pos = p - data();
			insert(pos, il);
			return data() + pos;
		}

		basic_String& insert(const size_type pos, const basic_String_view<Char, Traits>& s) {
			return insert(pos, s.data(), s.size());
		}

		template<typename T, is_const_iterator<T> = 0>
		iterator insert(T p, const basic_String_view<Char, Traits>& s) {
			const size_type pos = p - data();
			insert(pos, s);
			return data() + pos;
		}

		basic_String& erase(const size_type pos = 0, size_type n = npos);

		template<typename T, is_const_iterator<T> = 0>
		iterator erase(T p) {
			auto _data = data();
			const size_type pos(p - _data);
			erase(pos, 1);
			return _data + pos;
		}

		template<typename T, is_const_iterator<T> = 0>
		iterator erase(T first, T last) {
			const auto _data = begin();
			const size_type pos(first - _data);
			erase(pos, static_cast<size_t>(last - first));
			return _data + pos;
		}

		template<typename T = Traits>
		basic_String& remove(const value_type ch, const size_type off = 0) {
			return remove(&ch, off, 1);
		}

		template<typename T = Traits>
		basic_String& remove(const value_type* s, const size_type off, const size_type n);

		template<typename T = Traits>
		basic_String& remove(const value_type* s, const size_type n) {
			return remove<T>(s, 0, n);
		}

		template<typename T = Traits>
		basic_String& remove(const value_type* s) {
			return remove<T>(s, 0, traits_length(s));
		}

		template<typename T = Traits>
		basic_String& remove(const basic_String& other, const size_type off = 0) {
			return remove<T>(other.data(), off, other.size());
		}

		template<typename T = Traits>
		basic_String& remove(const basic_String_view<Char, Traits>& other, const size_type off = 0) {
			return remove<T>(other.data(), off, other.size());
		}

		basic_String& replace(const size_type pos, const size_type n, const value_type ch) {
			return replace(pos, n, &ch, 1);
		}

		basic_String& replace(
			const size_type pos, const size_type n, const basic_String& s) {
			return replace(pos, n, s.data(), s.size());
		}

		basic_String& replace(
			const size_type pos, const size_type n, const basic_String_view<Char, Traits>& s
		) {
			return replace(pos, n, s.data(), s.size());
		}

		basic_String& replace(
			const size_type pos1, const size_type n1,
			const basic_String& s, const size_type pos2, const size_type n2 = npos) {
			return replace(pos1, n1, s.data() + pos2, npos_min(n2, s.size() - pos2));
		}

		basic_String& replace(const size_type pos,
			const size_type n, const value_type* s) {
			return replace(pos, n, s, traits_length(s));
		}

		basic_String& replace(const size_type pos, const size_type n1
			, const value_type* s, const size_type n2);

		template<typename T, is_const_iterator<T> = 0>
		basic_String& replace(T first, T last, const basic_String& s) {
			return replace(first - data(), last - first, s.data(), s.size());
		}

		template<typename T, is_const_iterator<T> = 0>
		basic_String& replace(T first, T last, const basic_String_view<Char, Traits>& s) {
			return replace(first - data(), last - first, s.data(), s.size());
		}

		template<typename T, is_const_iterator<T> = 0>
		basic_String& replace(T first, T last, const basic_String& s,
			const size_type pos, const size_type n = npos) {
			return replace(first - data(), last - first, s, pos, n);
		}

		template<typename T, is_const_iterator<T> = 0>
		basic_String& replace(T first, T last, const value_type* s) {
			return replace(first - data(), last - first, s);
		}

		template<typename T, is_const_iterator<T> = 0>
		basic_String& replace(T first, T last, const value_type* s, const size_type n) {
			return replace(first - data(), last - first, s, n);
		}

		template<typename T = Traits>
		basic_String& replace(const value_type before, const value_type after);

		template<typename T = Traits>
		basic_String& replace(const value_type* s1, const size_type n1,
			const value_type* s2, const size_type n2);

		template<typename T = Traits>
		basic_String& replace(const value_type* s1, const value_type* s2) {
			return replace<T>(s1, traits_length(s1), s2, traits_length(s2));
		}

		template<typename T = Traits>
		basic_String& replace(const basic_String& before, const basic_String& after) {
			return replace<T>(before.data(), before.size(), after.data(), after.size());
		}

		template<typename T = Traits>
		basic_String& replace(const basic_String_view<Char, Traits>& before,
			const basic_String_view<Char, Traits>& after) {
			return replace<T>(before.data(), before.size(), after.data(), after.size());
		}

		template<typename T = Traits>
		basic_String& replace_first(const value_type* s1, const value_type* s2) {
			return replace_first<T>(s1, traits_length(s1), s2, traits_length(s2));
		}

		template<typename T = Traits>
		basic_String& replace_first(const value_type* s1, const size_type n1,
			const value_type* s2, const size_type n2) {
			size_type pos = find<T>(s1, 0, n1);
			if (pos != npos) {
				replace(pos, n1, s2, n2);
			}
			return *this;
		}

		template<typename T = Traits>
		basic_String& replace_first(const basic_String& s1, const basic_String& s2) {
			return replace_first<T>(s1.data(), s1.size(), s2.data(), s2.size());
		}

		template<typename T = Traits>
		basic_String& replace_first(const typename non_default_traits<T>::string_find_helper& s1,
			const value_type* s2) {
			return replace_first<T>(s1, s2, traits_length(s2));
		}

		template<typename T = Traits>
		basic_String& replace_first(const typename non_default_traits<T>::string_find_helper& s1,
			const value_type* s2, const size_type n2) {
			const size_type pos = find<T>(s1);
			if (pos != npos) {
				replace(pos, s1.size(), s2, n2);
			}
			return *this;
		}

		template<typename T = Traits>
		basic_String& replace_first(const typename non_default_traits<T>::string_find_helper& s1,
			const basic_String& s2) {
			return replace<T>(s1, s2.data(), s2.size());
		}

		template<typename T = Traits>
		basic_String& replace_last(const value_type* s1, const value_type* s2) {
			return replace_last<T>(s1, traits_length(s1), s2, traits_length(s2));
		}

		template<typename T = Traits>
		basic_String& replace_last(const value_type* s1, const size_type n1,
			const value_type* s2, const size_type n2) {
			size_type pos = rfind<T>(s1, npos, n1);
			if (pos != npos) {
				replace(pos, n1, s2, n2);
			}
			return *this;
		}

		template<typename T = Traits>
		basic_String& replace_last(const basic_String& s1, const basic_String& s2) {
			return replace_last<T>(s1.data(), s1.size(), s2.data(), s2.size());
		}

		template<typename T = Traits>
		basic_String& replace_last(const typename non_default_traits<T>::string_find_helper& s1,
			const value_type* s2) {
			return replace_last<T>(s1, s2, traits_length(s2));
		}

		template<typename T = Traits>
		basic_String& replace_last(const typename non_default_traits<T>::string_find_helper& s1,
			const value_type* s2, const size_type n2) {
			const size_type pos = rfind<T>(s1);
			if (pos != npos) {
				replace(pos, s1.size(), s2, n2);
			}
			return *this;
		}

		template<typename T = Traits>
		basic_String& replace_last(const typename non_default_traits<T>::string_find_helper& s1,
			const basic_String& s2) {
			return replace<T>(s1, s2.data(), s2.size());
		}

		void swap(basic_String& other)noexcept { core.swap(other.core); }

		const value_type* c_str() const { return core.c_str(); }
		value_type* data() { return core.data(); }
		const value_type* data() const { return core.data(); }

		value_type* mutable_data() { return core.mutableData(); }

		bool empty()const { return core.empty(); }

		// by using template that you
		// can compare in different ways
		// and it will be compared by default traits_type
		template<typename T = Traits>
		int compare(const basic_String& other)const {
			return compare<T>(0, npos, other);
		}

		template<typename T = Traits>
		int compare(const size_type pos1, const size_type n1, const basic_String& other)const {
			return compare<T>(pos1, n1, other.data(), other.size());
		}

		template<typename T = Traits>
		int compare(const size_type pos1, const size_type n1, const value_type* s)const {
			return compare<T>(pos1, n1, s, traits_length(s));
		}

		template<typename T = Traits>
		int compare(const size_type pos1, const size_type n1,
			const value_type* s, const size_type n2)const {
			return non_default_traits<T>::compare(data() + pos1, npos_min(n1, size() - pos1), s, n2);
		}

		template<typename T = Traits>
		int compare(const size_type pos1, const size_type n1,
			const basic_String& other, const size_type pos2, const size_type n2 = npos)const {
			return compare<T>(pos1, n1, other.data() + pos2, npos_min(n2, other.size() - pos2));
		}

		template<typename T = Traits>
		int compare(const value_type* s, const size_type n)const {
			return compare<T>(0, size(), s, n);
		}

		template<typename T = Traits>
		int compare(const value_type* s)const {
			return compare<T>(0, size(), s, traits_length(s));
		}

		template<typename T = Traits>
		int compare(const size_type pos1, const size_type n1,
			const basic_String_view<Char, Traits>& s)const {
			return compare<T>(pos1, n1, s.data(), s.size());
		}

		template<typename T = Traits>
		int compare(const basic_String_view<Char, Traits>& s)const {
			return compare<T>(0, size(), s.data(), s.size());
		}

		template<typename T = Traits>
		bool equal(const basic_String& other)const {
			return equal<T>(0, size(), other);
		}

		template<typename T = Traits>
		bool equal(const size_type pos1, const size_type n1, const basic_String& other)const {
			return equal<T>(pos1, n1, other.data(), other.size());
		}

		template<typename T = Traits>
		bool equal(const size_type pos1, const size_type n1, const value_type* s)const {
			return equal<T>(pos1, n1, s, traits_length(s));
		}

		template<typename T = Traits>
		bool equal(const size_type pos1, const size_type n1,
			const value_type* s, const size_type n2) const {
			return non_default_traits<T>::equal(data() + pos1, npos_min(n1, size() - pos1), s, n2);
		}

		template<typename T = Traits>
		bool equal(const value_type* s, const size_type n)const {
			return equal<T>(0, size(), s, n);
		}

		template<typename T = Traits>
		bool equal(const value_type* s)const {
			return equal<T>(s, traits_length(s));
		}

		template<typename T = Traits>
		bool equal(const size_type pos1, const size_type n1,
			const basic_String_view<Char, Traits>& s)const {
			return equal<T>(pos1, n1, s.data(), s.size());
		}

		template<typename T = Traits>
		bool equal(const basic_String_view<Char, Traits>& s)const {
			return equal<T>(0, size(), s.data(), s.size());
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_helper
			get_find_helper(const value_type* s, const value_type* e) {
			return non_default_traits<T>::get_find_helper(s, e);
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_helper
			get_find_helper(const value_type* s, const size_type n) {
			return get_find_helper<T>(s, s + n);
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_helper
			get_find_helper(const value_type* s) {
			return get_find_helper<T>(s, s + traits_length(s));
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_helper
			get_find_helper(const basic_String& s) {
			return get_find_helper<T>(s.begin(), s.end());
		}

		template<typename T = Traits>
		size_type find(const value_type ch, const size_type off = 0)const {
			return non_default_traits<T>::find(data(), size(), off, ch);
		}

		template<typename T = Traits>
		size_type find(const value_type* s, const size_type off = 0)const {
			return find<T>(s, off, traits_length(s));
		}

		template<typename T = Traits>
		size_type find(const value_type* s, const size_type off, const size_type count)const {
			return non_default_traits<T>::find(data(), size(), off, s, count);
		}

		template<typename T = Traits>
		size_type find(const basic_String& other, const size_type off = 0)const {
			return find<T>(other.data(), off, other.size());
		}

		template<typename T = Traits>
		size_type find(const typename non_default_traits<T>::string_find_helper& srch, const size_type off = 0)const {
			return non_default_traits<T>::find(data(), size(), off, srch);
		}

		template<typename T = Traits>
		size_type find(const basic_String_view<Char, Traits>& s, const size_type off = 0)const {
			return find<T>(s.data(), off, s.size());
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_rfind_helper
			get_rfind_helper(const value_type* s, const value_type* e) {
			return non_default_traits<T>::get_rfind_helper(s, e);
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_rfind_helper
			get_rfind_helper(const value_type* s, const size_type n) {
			return get_rfind_helper<T>(s, s + n);
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_rfind_helper
			get_rfind_helper(const value_type* s) {
			return get_rfind_helper<T>(s, s + traits_length(s));
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_rfind_helper
			get_rfind_helper(const basic_String& s) {
			return get_rfind_helper<T>(s.data(), s.size());
		}

		template<typename T = Traits>
		size_type rfind(const value_type ch, const size_type off = npos)const {
			return non_default_traits<T>::rfind(data(), size(), off, ch);
		}

		template<typename T = Traits>
		size_type rfind(const value_type* s, const size_type off = npos)const {
			return rfind<T>(s, off, traits_length(s));
		}

		template<typename T = Traits>
		size_type rfind(const value_type* s, const size_type off, const size_type count)const {
			return non_default_traits<T>::rfind(data(), size(), off, s, count);
		}

		template<typename T = Traits>
		size_type rfind(const basic_String& other, const size_type off = npos)const {
			return rfind<T>(other.data(), off, other.size());
		}

		template<typename T = Traits>
		size_type rfind(const typename non_default_traits<T>::
			string_rfind_helper& srch, const size_type off = npos)const {
			return non_default_traits<T>::rfind(data(), size(), off, srch);
		}

		template<typename T = Traits>
		size_type rfind(const basic_String_view<Char, Traits>& s, const size_type off = npos)const {
			return rfind<T>(s.data(), off, s.size());
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_of_helper get_find_of_helper(
			const value_type* s, const value_type* e
		) {
			return non_default_traits<T>::get_find_of_helper(s, e);
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_of_helper get_find_of_helper(
			const value_type* s, const size_type n
		) {
			return get_find_of_helper<T>(s, s + n);
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_of_helper get_find_of_helper(
			const value_type* s
		) {
			return get_find_of_helper<T>(s, s + traits_length(s));
		}

		template<typename T = Traits>
		static typename non_default_traits<T>::string_find_of_helper get_find_of_helper(
			const basic_String& s
		) {
			return get_find_of_helper<T>(s.data(), s.size());
		}

		template<typename T = Traits>
		size_type find_first_of(const value_type ch, const size_type off = 0)const {
			return non_default_traits<T>::find_first_of_ch(data(), size(), off, ch);
		}

		template<typename T = Traits>
		size_type find_first_of(const value_type* s, const size_type off = 0)const {
			return find_first_of<T>(s, off, traits_length(s));
		}

		template<typename T = Traits>
		size_type find_first_of(const value_type* s, const size_type off, const size_type count)const {
			return non_default_traits<T>::find_first_of(data(), size(), off, s, count);
		}

		template<typename T = Traits>
		size_type find_first_of(const basic_String& other, const size_type off = 0)const {
			return find_first_of<T>(other.data(), off, other.size());
		}

		template<typename T = Traits>
		size_type find_first_of(const typename non_default_traits<T>::string_find_of_helper& srch,
			const size_type off = 0)const {
			return non_default_traits<T>::find_first_of(data(), size(), off, srch);
		}

		template<typename T = Traits>
		size_type find_first_of(const basic_String_view<Char, Traits>& s, const size_type off = 0)const {
			return find_first_of<T>(s.data(), off, s.size());
		}

		template<typename T = Traits>
		size_type find_last_of(const value_type ch, const size_type off = npos)const {
			return non_default_traits<T>::find_last_of_ch(data(), size(), off, ch);
		}

		template<typename T = Traits>
		size_type find_last_of(const value_type* s, const size_type off = npos)const {
			return find_last_of<T>(s, off, traits_length(s));
		}

		template<typename T = Traits>
		size_type find_last_of(const value_type* s, const size_type off, const size_type count)const {
			return non_default_traits<T>::find_last_of(data(), size(), off, s, count);
		}

		template<typename T = Traits>
		size_type find_last_of(const basic_String& other, const size_type off = npos)const {
			return find_last_of<T>(other.data(), off, other.size());
		}

		template<typename T = Traits>
		size_type find_last_of(const typename non_default_traits<T>::
			string_find_of_helper& srch, const size_type off = npos)const {
			return non_default_traits<T>::find_last_of(data(), size(), off, srch);
		}

		template<typename T = Traits>
		size_type find_last_of(const basic_String_view<Char, Traits>& s, const size_type off = npos)const {
			return find_last_of<T>(s.data(), off, s.size());
		}

		template<typename T = Traits>
		size_type find_first_not_of(const value_type ch, const size_type off = 0)const {
			return non_default_traits<T>::find_first_not_of_ch(data(), size(), off, ch);
		}

		template<typename T = Traits>
		size_type find_first_not_of(const value_type* s, const size_type off = 0)const {
			return find_first_not_of<T>(s, off, traits_length(s));
		}

		template<typename T = Traits>
		size_type find_first_not_of(const value_type* s, const size_type off, const size_type count)const {
			return non_default_traits<T>::find_first_not_of(data(), size(), off, s, count);
		}

		template<typename T = Traits>
		size_type find_first_not_of(const basic_String& other, const size_type off = 0)const {
			return find_first_not_of<T>(other.data(), off, other.size());
		}

		template<typename T = Traits>
		size_type find_first_not_of(const typename non_default_traits<T>::string_find_of_helper& srch,
			const size_type off = 0)const {
			return non_default_traits<T>::find_first_not_of(data(), size(), off, srch);
		}

		template<typename T = Traits>
		size_type find_first_not_of(const basic_String_view<Char, Traits>& s, const size_type off = 0)const {
			return find_first_not_of<T>(s.data(), off, s.size());
		}

		template<typename T = Traits>
		size_type find_last_not_of(const value_type ch, const size_type off = npos)const {
			return non_default_traits<T>::find_last_not_of_ch(data(), size(), off, ch);
		}

		template<typename T = Traits>
		size_type find_last_not_of(const value_type* s, const size_type off = npos)const {
			return find_last_not_of<T>(s, off, traits_length(s));
		}

		template<typename T = Traits>
		size_type find_last_not_of(const value_type* s, const size_type off, const size_type count)const {
			return non_default_traits<T>::find_last_not_of(data(), size(), off, s, count);
		}

		template<typename T = Traits>
		size_type find_last_not_of(const basic_String& other, const size_type off = npos)const {
			return find_last_not_of<T>(other.data(), off, other.size());
		}

		template<typename T = Traits>
		size_type find_last_not_of(const typename non_default_traits<T>::string_find_of_helper& srch,
			const size_type off = npos)const {
			return non_default_traits<T>::find_last_not_of(data(), size(), off, srch);
		}

		template<typename  T = Traits>
		size_type find_last_not_of(const basic_String_view<Char, Traits>& s, const size_type off = npos)const {
			return find_last_not_of<T>(s.data(), off, s.size());
		}

		template<typename T = Traits>
		bool starts_with(const value_type ch)const {
			return non_default_traits<T>::starts_with(data(), size(), ch);
		}

		template<typename T = Traits>
		bool starts_with(const value_type* s)const {
			return starts_with<T>(s, traits_length(s));
		}

		template<typename T = Traits>
		bool starts_with(const value_type* s, const size_type n)const {
			return non_default_traits<T>::starts_with(data(), size(), s, n);
		}

		template<typename T = Traits>
		bool starts_with(const basic_String_view<Char, Traits>& s)const {
			return starts_with<T>(s.data(), s.size());
		}

		template<typename T = Traits>
		bool starts_with(const basic_String& other)const {
			return starts_with<T>(other.data(), other.size());
		}

		template<typename T = Traits>
		bool ends_with(const value_type ch)const {
			return non_default_traits<T>::ends_with(data(), size(), ch);
		}

		template<typename T = Traits>
		bool ends_with(const value_type* s)const {
			return ends_with<T>(s, traits_length(s));
		}

		template<typename T = Traits>
		bool ends_with(const value_type* s, const size_type n)const {
			return non_default_traits<T>::ends_with(data(), size(), s, n);
		}

		template<typename T = Traits>
		bool ends_with(const basic_String& other)const {
			return ends_with<T>(other.data(), other.size());
		}

		template<typename T = Traits>
		bool ends_with(const basic_String_view<Char, Traits>& s)const {
			return ends_with<T>(s.data(), s.size());
		}

		basic_String substr(const size_type pos, const size_type n = npos)const& {
			return basic_String(data() + pos, npos_min(n, size() - pos));
		}

		basic_String substr(const size_type pos, const size_type n = npos) && noexcept {
			erase(0, pos);
			resize(npos_min(n, size()));
			return std::move(*this);
		}

		basic_String left(const size_type n)const& {
			return basic_String(data(), npos_min(n, size()));
		}

		basic_String left(const size_type n) && noexcept {
			erase(npos_min(n, size()), npos);
			return std::move(*this);
		}

		basic_String right(size_type n)const& {
			const auto _size = size();
			n = npos_min(n, _size);
			return basic_String(data() + _size - n, n);
		}

		basic_String right(const size_type n) && noexcept {
			const auto _size = size();
			erase(0, _size - npos_min(n, _size));
			return std::move(*this);
		}

		basic_String trim_left()const&;

		basic_String trim_left()&&;

		basic_String trim_right()const&;

		basic_String trim_right()&&;

		basic_String trim()const&;

		basic_String trim()&&;

		void chop(const size_type n) {
			core.shrink(npos_min(n, size()));
		}

		void remove_prefix(size_type n) {
			auto _size = size();
			n = npos_min(n, _size);
			if (n != 0) {
				auto _data = data();
				memmove(_data, _data + n, sizeof(value_type) * (_size - n));
				chop(n);
			}
		}

		void remove_suffix(const size_type n) {
			chop(n);
		}

		basic_String& fill(value_type ch, const size_type n = npos);

		template<typename T = Traits, typename string_list = std::vector<basic_String>>
		string_list split(const value_type ch, bool keep_empty_parts = true)const;

		template<typename T = Traits, typename string_list = std::vector<basic_String>>
		string_list split(const basic_String& other, bool keep_empty_parts = true)const;

		template<typename T = Traits, typename string_list = std::vector<basic_String>>
		string_list split(const value_type* s, bool keep_empty_parts = true)const;

		template<typename T = Traits, typename string_list = std::vector<basic_String>>
		string_list split(const value_type* s,
			const size_type n, bool keep_empty_parts = true)const;

		template<typename T = Traits, typename string_list = std::vector<basic_String>>
		string_list split(const basic_String_view<Char, Traits>& s, bool keep_empty_parts = true)const {
			return split<T, string_list>(s.data(), s.size(), keep_empty_parts);
		}

		basic_String to_lower()const&;
		basic_String to_lower()&&;
		basic_String to_upper()const&;
		basic_String to_upper()&&;

		static basic_String number(int, int base = 10);
		static basic_String number(unsigned int, int base = 10);
		static basic_String number(long long, int base = 10);
		static basic_String number(unsigned long long, int base = 10);
		static basic_String number(double, char f = 'g', int prec = 6);
		static basic_String fixed_number(double);

		static Char* number(Char*, const Char*, int, int base = 10);
		static Char* number(Char*, const Char*, unsigned int, int base = 10);
		static Char* number(Char*, const Char*, long long, int base = 10);
		static Char* number(Char*, const Char*, unsigned long long, int base = 10);
		static Char* number(Char*, const Char*, double, char f = 'g', int prec = 6);
		static Char* fixed_number(Char*, const Char*, double);

		basic_String& set_number(int, int base = 10);
		basic_String& set_number(unsigned int, int base = 10);
		basic_String& set_number(long long, int base = 10);
		basic_String& set_number(unsigned long long, int base = 10);
		basic_String& set_number(double, char f = 'g', int prec = 6);

		int to_int(bool* ok = nullptr, int base = 10)const;
		unsigned int to_uint(bool* ok = nullptr, int base = 10)const;
		long long to_ll(bool* ok = nullptr, int base = 10)const;
		unsigned long long to_ull(bool* ok = nullptr, int base = 10)const;

		int to_int(const value_type*& next, bool* ok = nullptr, int base = 10)const;
		unsigned int to_uint(const value_type*& next, bool* ok = nullptr, int base = 10)const;
		long long to_ll(const value_type*& next, bool* ok = nullptr, int base = 10)const;
		unsigned long long to_ull(const value_type*& next, bool* ok = nullptr, int base = 10)const;

		size_t hash_code()const {
			return std::hash<basic_String<Char, Traits, Core>>()(*this);
		}

		template<typename...Args>
		static basic_String asprintf(const char* format, Args&&...args) {
			const auto len = static_cast<size_t>(_scprintf(format, std::forward<Args>(args)...));
			basic_String str(len, tag::uninitialized);
			sprintf_s(&str[0], len + 1, format, std::forward<Args>(args)...);
			return str;
		}

	private:
		struct string_connect_helper {
		public:
			string_connect_helper(const basic_String& other)
				: sv(other.data()), count(other.size()) {
			}
			string_connect_helper(const value_type* s)
				: sv(s), count(traits_length(s)) {
			}
			template<typename T, typename C>
			string_connect_helper(const basic_String<Char, T, C>& other)
				: sv(other.data()), count(other.size()) {
			}
			template<typename T, typename A>
			string_connect_helper(const std::basic_string<Char, T, A>& other)
				: sv(other.data()), count(other.size()) {
			}
			string_connect_helper(std::initializer_list<value_type> il)
				: sv(il.begin()), count(il.size()) {
			}
			string_connect_helper(const basic_String_view<Char, Traits>& s)
				: sv(s.data()), count(s.size()) {
			}
			string_connect_helper(const std::basic_string_view<Char, Traits>& s)
				: sv(s.data()), count(s.size()) {
			}
			string_connect_helper(const value_type& ch)
				: sv(&ch), count(1) {
			}
			const value_type* data()const {
				return sv;
			}
			size_type size()const {
				return count;
			}
		private:
			const value_type* sv;
			const size_type count;
		};
		static basic_String _Connect(std::initializer_list<string_connect_helper> il);
		static basic_String _Connect_init(basic_String&& init, std::initializer_list<string_connect_helper> il);

	public:

		template<typename...Args>
		static basic_String connect(Args&&...args) {
			return _Connect({ std::forward<Args>(args)... });
		}

		template<typename...Args>
		static basic_String connect(basic_String&& init, Args&&...args) {
			return _Connect_init(std::move(init), { std::forward<Args>(args)... });
		}

		template<typename...Args>
		basic_String& multiple_append(Args&&...args) noexcept {
			*this = _Connect_init(std::move(*this), { std::forward<Args>(args)... });
			return *this;
		}

		basic_String repeated(int times)const;

		static basic_String format_time(const char* const format,
			struct tm const* date, const size_type l = 0);

		basic_String simplified()const&;

		basic_String simplified()&&;

		basic_String left_justified(int width, const value_type fill = value_type(' '))const& {
			const size_type _size = size();
			if (_size < width) {
				basic_String str(_size, tag::reserved);
				str.append(*this).append(width - _size, fill);
				return str;
			}
			return *this;
		}

		basic_String left_justified(int width, const value_type fill = value_type(' '))&& {
			const size_type _size = size();
			if (_size < width) {
				append(width - _size, fill);
			}
			return std::move(*this);
		}

		basic_String right_justified(int width, const value_type fill = value_type(' '))const& {
			const size_type _size = size();
			if (_size < width) {
				basic_String str(_size, tag::reserved);
				str.append(width - _size, fill).append(*this);
				return str;
			}
			return *this;
		}

		basic_String right_justified(int width, const value_type fill = value_type(' '))&& {
			const size_type _size = size();
			if (_size < width) {
				prepend(width - _size, fill);
			}
			return std::move(*this);
		}

		template<typename T = Traits>
		bool contains(const value_type ch, const size_type off = 0)const {
			return find<T>(ch, off) != npos;
		}

		template<typename T = Traits>
		bool contains(const basic_String& s, const size_type off = 0)const {
			return find<T>(s, off) != npos;
		}

		template<typename T = Traits>
		bool contains(const typename non_default_traits<T>::string_find_helper& srch,
			const size_type off = 0)const {
			return find<T>(srch, off) != npos;
		}

		template<typename T = Traits>
		bool contains(const basic_String_view<Char, Traits>& s, const size_type off = 0)const {
			return find<T>(s, off) != npos;
		}

		template<typename T = Traits>
		bool contains(const value_type* s, const size_type off = 0)const {
			return find<T>(s, off) != npos;
		}

		template<typename T = Traits>
		bool contains(const value_type* s, const size_type off, const size_type n)const {
			return find<T>(s, off, n) != npos;
		}

		template<typename Other, typename _codecvt =
			default_convert_type<Other>>
			basic_String & convert_from(const Other* s, const std::locale& loc = std::locale()) {
			return convert_from(s, basic_String_traits<Other>::traits_length(s), loc);
		}

		template<typename Other, typename _codecvt =
			default_convert_type<Other>>
			basic_String & convert_from(const Other* s, const Other* e,
				const std::locale& loc = std::locale()) {
			return convert_from(s, static_cast<size_type>(e - s), loc);
		}

		template<typename Other, typename _codecvt =
			default_convert_type<Other>>
			basic_String & convert_from(const Other* s, const size_type len,
				const std::locale& loc = std::locale()) {
			const _codecvt& cvt = std::use_facet<_codecvt>(loc);
			using codecvt_type = _codecvt;
			mbstate_t it;
			size_type Reserved_Capacity = std::max(
				Core::max_small_size(),
				len * ((sizeof(Other) + sizeof(Char) - 1) / sizeof(Char)));
			// reserve possible size
			const Other* next1;
			Char* next2;
			typename codecvt_type::result result;
			do {
				reserve(Reserved_Capacity);
				memset(&it, 0, sizeof(it));
				const auto _data = data();
				result = cvt.in(it, s, s + len, next1, _data, _data + Reserved_Capacity, next2);
				Reserved_Capacity <<= 1;
				assert(result != codecvt_type::error);
			} while (result == codecvt_type::partial);
			set_size(next2 - data());
			shrink_to_fit();
			return *this;
		}

		template<typename Other, typename T, typename C,
			typename _codecvt = default_convert_type<Other>>
			basic_String & convert_from(const basic_String<Other, T, C>& other,
				const std::locale& loc = std::locale()) {
			return convert_from(other.data(), other.size(), loc);
		}

		template<typename Other, typename T, typename _codecvt =
			default_convert_type<Other>>
			basic_String & convert_from(const basic_String_view<Other, T>& other,
				const std::locale& loc = std::locale()) {
			return convert_from(other.data(), other.size(), loc);
		}

		template<typename Other, typename T, typename A,
			typename _codecvt = default_convert_type<Other>>
			basic_String & convert_from(const std::basic_string<Other, T, A>& other,
				const std::locale& loc = std::locale()) {
			return convert_from(other.data(), other.size(), loc);
		}

		template<typename Other, typename T,
			typename _codecvt = default_convert_type<Other>>
			basic_String & convert_from(const std::basic_string_view<Other, T>& other,
				const std::locale& loc = std::locale()) {
			return convert_from(other.data(), other.size(), loc);
		}

		template<typename OString, typename _codecvt =
			default_convert_type<typename OString::value_type>>
			OString convert_to(const std::locale& loc = std::locale())const {
			return basic_String_view<Char, Traits>(data(), size()).
				template convert_to<OString, _codecvt>(loc);
		}

	private:
		Core core;
	};

	template<typename Char, typename Traits, typename Core>
	void basic_String<Char, Traits, Core>::
		resize(const size_type n, const value_type c) {
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

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core>&
		basic_String<Char, Traits, Core>::append(
			const value_type* s, size_type n) {
		const auto old_data = data();
		const auto old_size = size();
		const auto ptr = core.expandNoinit(n, true);

		if (old_data <= s && s < old_data + old_size) {
			s = (ptr - old_size) + (s - old_data);
		}
		memcpy(ptr, s, sizeof(value_type) * n);
		return *this;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core>&
		basic_String<Char, Traits, Core>::prepend(
			const value_type* s, size_type n) {
		if (!n) {
			return *this;
		}
		const auto old_data = data();
		const auto old_size = size();
		core.expandNoinit(n, true);

		auto ptr = data();
		memmove(ptr + n, ptr, sizeof(value_type) * old_size);

		if (old_data <= s && s < old_data + old_size) {
			s = ptr + (s - old_data) + n;
		}
		memcpy(ptr, s, sizeof(value_type) * n);
		return *this;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core>&
		basic_String<Char, Traits, Core>::assign(
			const value_type* s, const size_type n) {
		if (unlikely(!n)) {
			resize(0);
		}
		else if (size() >= n) {
			memmove(core.mutableData(), s, sizeof(value_type) * n);
			core.shrink(size() - n);
		}
		else {
			resize(0);
			// O(n) to copy
			// so don't need to use expGrowth
			memmove(core.expandNoinit(n), s, sizeof(value_type) * n);
		}
		return *this;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core>&
		basic_String<Char, Traits, Core>::insert(
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

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core>&
		basic_String<Char, Traits, Core>::insert(
			const size_type pos, const size_type n, const value_type c
		) {
		const auto old_size = size();
		const auto data = core.expandNoinit(n, true);
		auto data_first = data - old_size;
		auto data_pos = data_first + pos;
		auto data_end = data;
		memmove(data_pos + n, data_pos, sizeof(value_type) * (old_size - pos));
		std::fill_n(data_pos, n, c);
		return *this;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core>&
		basic_String<Char, Traits, Core>::erase(const size_type pos, size_type n) {
		if (!n) {
			return *this;
		}
		const size_type res = size() - pos;
		n = npos_min(n, res);
		const auto _Data = data();
		memcpy(_Data + pos, _Data + pos + n, sizeof(value_type) * (size() - pos - n));
		core.shrink(n);
		return *this;
	}

	template<typename Char, typename Traits, typename Core>
	template<typename T>
	basic_String<Char, Traits, Core>& basic_String<Char, Traits, Core>::
		remove(const value_type* s, const size_type _off, const size_type n) {
		auto _data = data();
		size_type off = _off;
		auto srch = get_find_helper<T>(s, s + n);
		size_type appear_time = 0;
		for (;;) {
			const auto pos = find<T>(srch, off);
			if (pos == npos) {
				break;
			}
			if (appear_time) {
				assert(off >= n * appear_time);
				memmove(_data + off - n * appear_time,
					_data + off, sizeof(value_type) *
					static_cast<size_type>(pos - off));
			}
			++appear_time;
			off = pos + n;
		}
		if (appear_time) {
			assert(off >= n * appear_time);
			memmove(_data + off - n * appear_time,
				_data + off, sizeof(value_type) *
				static_cast<size_type>(size() - off));
		}
		chop(n * appear_time);
		return *this;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core>&
		basic_String<Char, Traits, Core>::replace(
			const size_type pos1, const size_type n1, const value_type* s, const size_type n2) {
		std::copy(s, s + std::min(n1, n2), data() + pos1);
		if (n1 < n2) {
			insert(pos1 + n1, s + n1, n2 - n1);
		}
		else {
			erase(pos1 + n2, n1 - n2);
		}
		return *this;
	}

	template<typename Char, typename Traits, typename Core>
	template<typename T>
	basic_String<Char, Traits, Core>& basic_String<Char, Traits, Core>::replace(
		const value_type before, const value_type after) {
		auto _data = data();
		size_type off = 0;
		for (;;) {
			off = find<T>(before, off);
			if (off == npos) {
				break;
			}
			*(_data + off) = after;
			++off;
		}
		return *this;
	}

	template<typename Char, typename Traits, typename Core>
	template<typename T>
	basic_String<Char, Traits, Core>& basic_String<Char, Traits, Core>::replace(
		const value_type* s1, const size_type n1, const value_type* s2, const size_type n2) {
		size_type off = 0;
		auto srch = get_find_helper(s1, s1 + n1);
		if (n1 == n2) {
			for (;;) {
				const size_type pos = find<T>(srch, off);
				if (pos == npos) {
					break;
				}
				replace(pos, n1, s2, n2);
				off = pos + n1;
			}
		}
		else {
			basic_String temp;
			for (;;) {
				const size_type pos = find<T>(srch, off);
				if (pos == npos) {
					temp.append(*this, off, npos);
					break;
				}
				temp.append(*this, off, pos - off).append(s2, n2);
				off = pos + n1;
			}
			this->~basic_String();
			new (this) basic_String(std::move(temp));
		}
		return *this;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::trim_left()const& {
		auto _data = data();
		auto _size = size();
		auto l = default_traits::trim_left(_data, _size);
		if (l == npos) {
			return basic_String();
		}
		return basic_String(_data + l, _size - l);
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::trim_left()&& {
		auto _data = data();
		auto _size = size();
		auto l = default_traits::trim_left(_data, _size);
		if (l == npos) {
			clear();
			return std::move(*this);
		}
		memmove(_data, _data + l, sizeof(value_type) * (_size - l));
		set_size(_size - l);
		return std::move(*this);
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::trim_right()const& {
		auto _data = data();
		auto _size = size();
		auto r = default_traits::trim_right(_data, _size);
		if (r == npos) {
			return basic_String();
		}
		return basic_String(_data, r + 1);
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::trim_right()&& {
		auto _data = data();
		auto _size = size();
		auto r = default_traits::trim_right(_data, _size);
		if (r == npos) {
			clear();
			return std::move(*this);
		}
		set_size(r + 1);
		return std::move(*this);
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::trim()const& {
		auto _data = data();
		auto _size = size();
		auto l = default_traits::trim_left(_data, _size);
		if (l == npos) {
			return basic_String();
		}
		auto r = default_traits::trim_right(_data, _size);
		return basic_String(_data + l, r - l + 1);
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::trim()&& {
		auto _data = data();
		auto _size = size();
		auto l = default_traits::trim_left(_data, _size);
		if (l == npos) {
			clear();
			return std::move(*this);
		}
		auto r = default_traits::trim_right(_data, _size);
		memmove(_data, _data + l, sizeof(value_type) * (r - l + 1));
		set_size(r - l + 1);
		return std::move(*this);
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core>&
		basic_String<Char, Traits, Core>::fill(value_type ch, const size_type n) {
		auto _size = size();
		if (n == npos) {
			std::fill_n(data(), _size, ch);
		}
		else {
			if (_size < n) {
				core.expandNoinit(n - _size, true);
			}
			if (_size > n) {
				core.shrink(_size - n);
			}
			std::fill_n(data(), n, ch);
		}
		return *this;
	}

	template<typename Char, typename Traits, typename Core>
	template<typename T, typename string_list>
	string_list basic_String<Char, Traits, Core>::split(
		const value_type ch, bool keep_empty_parts
	)const {
		return non_default_traits<T>::template split<string_list>(data(), size(), ch, keep_empty_parts);
	}

	template<typename Char, typename Traits, typename Core>
	template<typename T, typename string_list>
	string_list basic_String<Char, Traits, Core>::split(
		const basic_String<Char, Traits, Core>& other, bool keep_empty_parts)const {
		return split<T, string_list>(other.data(), other.size(), keep_empty_parts);
	}

	template<typename Char, typename Traits, typename Core>
	template<typename T, typename string_list>
	string_list basic_String<Char, Traits, Core>::split(
		const value_type* s, bool keep_empty_parts)const {
		return split<T, string_list>(s, traits_length(s), keep_empty_parts);
	}

	template<typename Char, typename Traits, typename Core>
	template<typename T, typename string_list>
	string_list basic_String<Char, Traits, Core>::split(
		const value_type* s, const size_type n,
		bool keep_empty_parts)const {
		return non_default_traits<T>::template split<string_list>(data(), size(), s, n, keep_empty_parts);
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::to_lower()const& {
		basic_String ans(*this);
		return std::move(ans).to_lower();
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::to_lower()&& {
		for (auto& i : (*this)) {
			i = tolower(i);
		}
		return std::move(*this);
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::to_upper()const& {
		basic_String ans(*this);
		return std::move(ans).to_upper();
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::to_upper()&& {
		for (auto& i : (*this)) {
			i = toupper(i);
		}
		return std::move(*this);
	}

	template<typename Char, typename UVal>
	Char* get_number_positive(Char* buff, UVal val, int base) {
		static_assert(std::is_unsigned_v<UVal>, "UVal must be unsigned");
		assert(base >= 0);
		do {
			UVal mod = val % base;
			*(--buff) = static_cast<Char>(mod < 10 ? '0' + mod : 'a' + (mod - 10));
			val /= base;
		} while (val != 0);
		return buff;
	}

	template<typename Char, typename UVal>
	Char* get_number_negative(Char* buff, UVal val, bool p, int base) {
		static_assert(std::is_unsigned_v<UVal>, "UVal must be unsigned");
		assert(base < 0);
		bool f = p;
		auto ubase = static_cast<unsigned int>(-base);
		do {
			UVal mod = val % ubase;
			UVal k = val / ubase;
			if (mod != 0 && !f) {
				mod = ubase - mod;
				if (!f) {
					++k;
				}
				else {
					--k;
				}
			}
			f = !f;
			val = k;
			*(--buff) = static_cast<Char>(mod < 10 ? '0' + mod : 'a' + (mod - 10));
		} while (val != 0);
		return buff;
	}

	template<typename Char, typename Val, std::enable_if_t<std::is_signed_v<Val>, int> = 0>
	Char* get_number_helper(Char* buff, Val val, int base) {
		static_assert(!std::is_unsigned_v<Val>, "Val must be signed");
		using uval = std::make_unsigned_t<Val>;
		if (base >= 0) {
			if (val < 0) {
				buff = get_number_positive(buff, static_cast<uval>(0 - val), base);
				*(--buff) = static_cast<Char>('-');
			}
			else {
				buff = get_number_positive(buff, static_cast<uval>(val), base);
			}
		}
		else {
			if (val < 0) {
				buff = get_number_negative(buff, static_cast<uval>(0 - val), false, base);
			}
			else {
				buff = get_number_negative(buff, static_cast<uval>(val), true, base);
			}
		}
		return buff;
	}

	template<typename Char, typename UVal, std::enable_if_t<std::is_unsigned_v<UVal>, int> = 0>
	Char* get_number_helper(Char* buff, UVal val, int base) {
		static_assert(std::is_unsigned_v<UVal>, "UVal must be unsigned");
		if (base >= 0) {
			buff = get_number_positive(buff, val, base);
		}
		else {
			buff = get_number_negative(buff, val, true, base);
		}
		return buff;
	}

	template<typename Char, typename Traits, typename Core, typename T>
	basic_String<Char, Traits, Core>
		_Get_number(T val, int base) {
		Char buff[std::numeric_limits<T>::digits + 2];
		Char* const buff_end = std::end(buff);
		Char* pos = buff_end;
		pos = get_number_helper(pos, val, base);
		assert(pos >= buff);
		return basic_String<Char, Traits, Core>(pos, buff_end);
	}

	template<typename Char, typename Traits, typename Core, typename T>
	Char* _Get_number(Char* _First, const Char* _Last, T val, int base) {
		Char buff[std::numeric_limits<T>::digits + 2];
		Char* const buff_end = std::end(buff);
		Char* pos = buff_end;
		pos = get_number_helper(pos, val, base);
		size_t len = buff_end - pos;
		if (_Last - _First < len) {
			return nullptr;
		}
		memcpy(_First, pos, len * sizeof(Char));
		return _First + len;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::
		number(int val, int base) {
		return _Get_number<Char, Traits, Core>(val, base);
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::
		number(unsigned int val, int base) {
		return _Get_number<Char, Traits, Core>(val, base);
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::
		number(long long val, int base) {
		return _Get_number<Char, Traits, Core>(val, base);
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::
		number(unsigned long long val, int base) {
		return _Get_number<Char, Traits, Core>(val, base);
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::
		number(double val, char f, int prec) {
		if (std::isnan(val)) {
			return basic_String(default_traits::nan);
		}
		if (std::isinf(val)) {
			return basic_String(default_traits::inf);
		}
		if (unlikely(prec > 99))  { prec = 99; }
		char form[6];
		char* pos = form;
		*(pos++) = static_cast<Char>('%');
		*(pos++) = static_cast<Char>('.');

		if (prec < 10) {
			*(pos++) = static_cast<Char>('0' + prec);
		}
		else {
			*(pos++) = static_cast<Char>('0' + (prec / 10));
			*(pos++) = static_cast<Char>('0' + (prec % 10));
			*(pos++) = static_cast<Char>('0' + (prec % 10));
		}
		*(pos++) = f;
		*(pos++) = '\0';
		char buff[64];
		const auto len = sprintf(buff, form, val);
		return basic_String<Char, Traits, Core>(buff, buff + len);
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::fixed_number(double val) {
		if (!val) {
			return basic_String<Char, Traits, Core>(1, std::integral_constant<Char, '0'>());
		}
		char buff[64];
		char* ptr = buff;
		if (val < 0) {
			val = -val;
			*(ptr++) = '-';
		}
		::fill_double(val, ptr);
		ptr = (char*)memchr(buff, '\0', 64);
		return basic_String<Char, Traits, Core>(buff, ptr);
	}

	template<typename Char, typename Traits, typename Core>
	Char* basic_String<Char, Traits, Core>::
		number(Char* _First, const Char* _Last, int val, int base) {
		return _Get_number<Char, Traits, Core>(_First, _Last, val, base);
	}

	template<typename Char, typename Traits, typename Core>
	Char* basic_String<Char, Traits, Core>::
		number(Char* _First, const Char* _Last, unsigned int val, int base) {
		return _Get_number<Char, Traits, Core>(_First, _Last, val, base);
	}

	template<typename Char, typename Traits, typename Core>
	Char* basic_String<Char, Traits, Core>::
		number(Char* _First, const Char* _Last, long long val, int base) {
		return _Get_number<Char, Traits, Core>(_First, _Last, val, base);
	}

	template<typename Char, typename Traits, typename Core>
	Char* basic_String<Char, Traits, Core>::
		number(Char* _First, const Char* _Last, unsigned long long val, int base) {
		return _Get_number<Char, Traits, Core>(_First, _Last, val, base);
	}

	template<typename Char, typename Traits, typename Core>
	Char* basic_String<Char, Traits, Core>::
		number(Char* _First, const Char* _Last, double val, char f, int prec) {
		size_t str_len = _Last - _First;
		size_t len;
		if (std::isnan(val)) {
			len = std::size(default_traits::nan);
			if (str_len < len) {
				return nullptr;
			}
			memcpy(_First, default_traits::nan, len * sizeof(Char));
			return _First + len;
		}
		if (std::isinf(val)) {
			len = std::size(default_traits::inf);
			memcpy(_First, default_traits::inf, len * sizeof(Char));
			return _First + len;
		}
		if (unlikely(prec > 99))  { prec = 99; }
		char form[6];
		char* pos = form;
		*(pos++) = static_cast<Char>('%');
		*(pos++) = static_cast<Char>('.');

		if (prec < 10) {
			*(pos++) = static_cast<Char>('0' + prec);
		}
		else {
			*(pos++) = static_cast<Char>('0' + (prec / 10));
			*(pos++) = static_cast<Char>('0' + (prec % 10));
			*(pos++) = static_cast<Char>('0' + (prec % 10));
		}
		*(pos++) = f;
		*(pos++) = '\0';
		char buff[64];
		len = sprintf(buff, form, val);
		if (str_len < len) {
			return nullptr;
		}
		std::copy(buff, buff + len, _First);
		return _First + len;
	}

	template<typename Char, typename Traits, typename Core>
	Char* basic_String<Char, Traits, Core>::fixed_number(Char* _First, const Char* _Last, double val) {
		size_t str_len = _Last - _First;
		if (!val) {
			if (!str_len) {
				return nullptr;
			}
			*_First = '0';
			return ++_First;
		}
		char buff[64];
		char* p = buff;
		if (val < 0) {
			val = -val;
			*(p++) = '-';
		}
		::fill_double(val, p);
		p = (char*)memchr(buff, '\0', 64);
		if (str_len < p - buff) {
			return nullptr;
		}
		std::copy(buff, p, _First);
		return _First + (p - buff);
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core>& basic_String<Char, Traits, Core>::
		set_number(int val, int base) {
		*this = std::move(number(val, base));
		return *this;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core>& basic_String<Char, Traits, Core>::
		set_number(unsigned int val, int base) {
		*this = std::move(number(val, base));
		return *this;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core>& basic_String<Char, Traits, Core>::
		set_number(long long val, int base) {
		*this = std::move(number(val, base));
		return *this;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core>& basic_String<Char, Traits, Core>::
		set_number(unsigned long long val, int base) {
		*this = std::move(number(val, base));
		return *this;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core>& basic_String<Char, Traits, Core>::
		set_number(double val, char f, int prec) {
		*this = std::move(number(val, f, prec));
		return *this;
	}

	template<typename Char, typename Traits, typename Core>
	int basic_String<Char, Traits, Core>::to_int(bool* ok, int base)const {
		return basic_String_view<Char, Traits>(data(), size()).to_int(ok, base);
	}

	template<typename Char, typename Traits, typename Core>
	unsigned int basic_String<Char, Traits, Core>::to_uint(bool* ok, int base)const {
		return basic_String_view<Char, Traits>(data(), size()).to_uint(ok, base);
	}

	template<typename Char, typename Traits, typename Core>
	long long basic_String<Char, Traits, Core>::to_ll(bool* ok, int base)const {
		return basic_String_view<Char, Traits>(data(), size()).to_ll(ok, base);
	}

	template<typename Char, typename Traits, typename Core>
	unsigned long long basic_String<Char, Traits, Core>::to_ull(bool* ok, int base)const {
		return basic_String_view<Char, Traits>(data(), size()).to_ull(ok, base);
	}

	template<typename Char, typename Traits, typename Core>
	int basic_String<Char, Traits, Core>::to_int(
		const value_type*& next, bool* ok, int base)const {
		return basic_String_view<Char, Traits>(data(), size()).to_int(next, ok, base);
	}

	template<typename Char, typename Traits, typename Core>
	unsigned int basic_String<Char, Traits, Core>::to_uint(
		const value_type*& next, bool* ok, int base)const {
		return basic_String_view<Char, Traits>(data(), size()).to_uint(next, ok, base);
	}

	template<typename Char, typename Traits, typename Core>
	long long basic_String<Char, Traits, Core>::to_ll(
		const value_type*& next, bool* ok, int base)const {
		return basic_String_view<Char, Traits>(data(), size()).to_ll(next, ok, base);
	}

	template<typename Char, typename Traits, typename Core>
	unsigned long long basic_String<Char, Traits, Core>::to_ull(
		const value_type*& next, bool* ok, int base)const {
		return basic_String_view<Char, Traits>(data(), size()).to_ull(next, ok, base);
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::
		_Connect(std::initializer_list<string_connect_helper> il) {
		size_type count = 0;
		for (auto& i : il) {
			count += i.size();
		}
		basic_String it;
		auto ptr = it.core.expandNoinit(count);
		for (auto& i : il) {
			memcpy(ptr, i.data(), sizeof(value_type) * i.size());
			ptr += i.size();
		}
		return it;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::
		_Connect_init(basic_String&& it, std::initializer_list<string_connect_helper> il) {
		size_type count = 0;
		for (auto& i : il) {
			count += i.size();
		}
		auto ptr = it.core.expandNoinit(count);
		for (auto& i : il) {
			memcpy(ptr, i.data(), sizeof(value_type) * i.size());
			ptr += i.size();
		}
		return std::move(it);
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::
		repeated(int times)const {
		if (times <= 0) times = 0;
		auto _data = data();
		auto _size = size();
		basic_String it;
		auto ptr = it.core.expandNoinit(times * _size);
		for (; times; --times) {
			memcpy(ptr, _data, sizeof(value_type) * _size);
			ptr += _size;
		}
		return it;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::
		format_time(const char* const format, struct tm const* date, const size_type l) {
		size_t max_len = Core::max_small_size();
		if (l != 0) {
			max_len = std::max(max_len, l);
		}
		else {
			max_len = std::max(max_len,
				basic_String_traits<char, std::char_traits<char>>::traits_length(format));
		}
		size_t len = 0;
		basic_String it;
		do {
			it.reserve(max_len);
			len = strftime(it.data(), max_len, format, date);
			max_len <<= 1;
		} while (len == 0);
		it.set_size(len);
		it.shrink_to_fit();
		return it;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::simplified()const& {
		const auto _data = data();
		const auto _size = size();
		basic_String it;
		size_type off = default_traits::find_first_not_of(_data, _size, 0, default_traits::trim_map);
		if (off != npos) {
			const size_type pos = default_traits::
				find_first_of(_data, _size, off, default_traits::trim_map);
			it.append(*this, off, pos - off);
			off = default_traits::find_first_not_of(_data, _size, pos + 1, default_traits::trim_map);
			if (off != npos) {
				while(true) {
					pos = default_traits::
						find_first_of(_data, _size, off, default_traits::trim_map);
					if (pos == npos) {
						it.append(*this, off, npos);
						break;
					}
					// don't need to use mutiple_append
					it.reserve(it.size() + 1 + (pos - off));
					it.push_back(' ');
					it.append(*this, off, pos - off);
					off = default_traits::find_first_not_of(_data, _size, pos + 1, default_traits::trim_map);
					if (off == npos) {
						break;
					}
				}
			}
		}
		return it;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::simplified()&& {
		const auto _data = data();
		const auto _size = size();
		size_type off = default_traits::find_first_not_of(_data, _size, 0, default_traits::trim_map);
		size_type used_size = 0;
		auto ptr = _data;
		if (off != npos) {
			const size_type pos = default_traits::
				find_first_of(_data, _size, off, default_traits::trim_map);
			used_size += pos - off;
			memcpy(ptr, _data + off, sizeof(value_type) * (pos - off));
			ptr += pos - off;
			off = default_traits::find_first_not_of(_data, _size, pos + 1, default_traits::trim_map);
			if (off != npos) {
				while(true) {
					pos = default_traits::
						find_first_of(_data, _size, off, default_traits::trim_map);
					if (pos == npos) {
						used_size += _size - off;
						memcpy(ptr, _data + off, sizeof(value_type) * (_size - off));
						break;
					}
					// don't need to use mutiple_append
					used_size += 1 + pos - off;
					*ptr = static_cast<Char>(' ');
					++ptr;
					memcpy(ptr, _data + off, sizeof(value_type) * (pos - off));
					ptr += pos - off;
					off = default_traits::find_first_not_of(_data, _size, pos + 1, default_traits::trim_map);
					if (off == npos) {
						break;
					}
				}
			}
		}
		set_size(used_size);
		return std::move(*this);
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> operator+(
		const basic_String<Char, Traits, Core>& lhs,
		const basic_String<Char, Traits, Core>& rhs
		) {
		basic_String<Char, Traits, Core> result;
		result.reserve(lhs.size() + rhs.size());
		result.append(lhs).append(rhs);
		return result;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> operator+(
		basic_String<Char, Traits, Core>&& lhs,
		const basic_String<Char, Traits, Core>& rhs
		) {
		return std::move(lhs.append(rhs));
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> operator+(
		const basic_String<Char, Traits, Core>& lhs,
		basic_String<Char, Traits, Core>&& rhs
		) {
		if (rhs.capacity() >= lhs.size() + rhs.size()) {
			return std::move(rhs.insert(0, lhs));
		}
		auto const& rc = rhs;
		return lhs + rc;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> operator+(
		basic_String<Char, Traits, Core>&& lhs,
		basic_String<Char, Traits, Core>&& rhs
		) {
		return std::move(lhs.append(rhs));
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> operator+(
		const basic_String_view<Char, Traits>& lhs,
		const basic_String<Char, Traits, Core>& rhs
		) {
		const auto len = lhs.size();
		basic_String<Char, Traits, Core> result;
		result.reserve(len + rhs.size());
		result.append(lhs).append(rhs);
		return result;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> operator+(
		const basic_String_view<Char, Traits>& lhs,
		basic_String<Char, Traits, Core>&& rhs
		) {
		const auto len = lhs.size();
		if (rhs.capacity() >= len + rhs.size()) {
			rhs.insert(0, lhs);
			return std::move(rhs);
		}
		basic_String<Char, Traits, Core> result;
		result.reserve(len + rhs.size());
		result.append(lhs).append(rhs);
		return result;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> operator+(
		const Char* lhs,
		const basic_String<Char, Traits, Core>& rhs
		) {
		using traits_type = typename basic_String<Char, Traits, Core>::traits_type;
		const auto len = traits_type::length(lhs);
		basic_String<Char, Traits, Core> result;
		result.reserve(len + rhs.size());
		result.append(lhs, len).append(rhs);
		return result;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> operator+(
		const Char* lhs,
		basic_String<Char, Traits, Core>&& rhs
		) {
		using traits_type = typename basic_String<Char, Traits, Core>::traits_type;
		const auto len = traits_type::length(lhs);
		if (rhs.capacity() >= len + rhs.size()) {
			rhs.insert(0, lhs, len);
			return std::move(rhs);
		}
		basic_String<Char, Traits, Core> result;
		result.reserve(len + rhs.size());
		result.append(lhs, len).append(rhs);
		return result;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> operator+(
		const Char ch,
		const basic_String<Char, Traits, Core>& rhs
		) {
		basic_String<Char, Traits, Core> result;
		result.reserve(1 + rhs.size());
		result.push_back(ch);
		result.append(rhs);
		return result;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> operator+(
		const Char ch,
		basic_String<Char, Traits, Core>&& rhs
		) {
		if (rhs.capacity() >= rhs.size() + 1) {
			rhs.insert(0, ch);
			return std::move(rhs);
		}
		auto const& rc = rhs;
		return ch + rhs;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> operator+(
		const basic_String<Char, Traits, Core>& lhs,
		const basic_String_view<Char, Traits>& rhs
		) {
		const auto len = rhs.data();
		basic_String<Char, Traits, Core> result;
		result.reserve(lhs.size() + len);
		result.append(lhs).append(rhs);
		return result;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> operator+(
		basic_String<Char, Traits, Core>&& lhs,
		const basic_String_view<Char, Traits>& rhs
		) {
		return std::move(lhs.append(rhs));
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> operator+(
		const basic_String<Char, Traits, Core>& lhs,
		const Char* rhs
		) {
		using traits_type = typename basic_String<Char, Traits, Core>::traits_type;
		const auto len = traits_type::length(rhs);
		basic_String<Char, Traits, Core> result;
		result.reserve(lhs.size() + len);
		result.append(lhs).append(rhs, len);
		return result;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> operator+(
		basic_String<Char, Traits, Core>&& lhs,
		const Char* rhs
		) {
		return std::move(lhs.append(rhs));
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> operator+(
		const basic_String<Char, Traits, Core>& lhs,
		const Char ch
		) {
		basic_String<Char, Traits, Core> result;
		result.reserve(lhs.size() + 1);
		result.append(lhs).push_back(ch);
		return result;
	}

	template<typename Char, typename Traits, typename Core>
	basic_String<Char, Traits, Core> operator+(
		basic_String<Char, Traits, Core>&& lhs,
		const Char ch
		) {
		return std::move(lhs.append(ch));
	}

	template<typename Char, typename Traits, typename Core>
	bool operator==(
		const basic_String<Char, Traits, Core>& lhs,
		const basic_String<Char, Traits, Core>& rhs
		) {
		return lhs.equal(rhs);
	}

	template<typename Char, typename Traits, typename Core>
	bool operator==(
		const basic_String<Char, Traits, Core>& lhs,
		const Char* rhs
		) {
		return lhs.equal(rhs);
	}

	template<typename Char, typename Traits, typename Core>
	bool operator==(
		const Char* lhs,
		const basic_String<Char, Traits, Core>& rhs
		) {
		return rhs.equal(lhs);
	}

	template<typename Char, typename Traits, typename Core>
	bool operator!=(
		const basic_String<Char, Traits, Core>& lhs,
		const basic_String<Char, Traits, Core>& rhs
		) {
		return !(lhs.equal(rhs));
	}

	template<typename Char, typename Traits, typename Core>
	bool operator!=(
		const basic_String<Char, Traits, Core>& lhs,
		const Char* rhs
		) {
		return !(lhs.equal(rhs));
	}

	template<typename Char, typename Traits, typename Core>
	bool operator!=(
		const Char* lhs,
		const basic_String<Char, Traits, Core>& rhs
		) {
		return !(rhs.equal(lhs));
	}

	template<typename Char, typename Traits, typename Core>
	bool operator<(
		const basic_String<Char, Traits, Core>& lhs,
		const basic_String<Char, Traits, Core>& rhs
		) {
		return lhs.compare(rhs) < 0;
	}

	template<typename Char, typename Traits, typename Core>
	bool operator<(
		const basic_String<Char, Traits, Core>& lhs,
		const Char* rhs
		) {
		return lhs.compare(rhs) < 0;
	}

	template<typename Char, typename Traits, typename Core>
	bool operator<(
		const Char* lhs,
		const basic_String<Char, Traits, Core>& rhs
		) {
		return rhs.compare(lhs) > 0;
	}

	template<typename Char, typename Traits, typename Core>
	bool operator>(
		const basic_String<Char, Traits, Core>& lhs,
		const basic_String<Char, Traits, Core>& rhs
		) {
		return lhs.compare(rhs) > 0;
	}

	template<typename Char, typename Traits, typename Core>
	bool operator>(
		const basic_String<Char, Traits, Core>& lhs,
		const Char* rhs
		) {
		return lhs.compare(rhs) > 0;
	}

	template<typename Char, typename Traits, typename Core>
	bool operator>(
		const Char* lhs,
		const basic_String<Char, Traits, Core>& rhs
		) {
		return rhs.compare(lhs) < 0;
	}

	template<typename Char, typename Traits, typename Core>
	bool operator<=(
		const basic_String<Char, Traits, Core>& lhs,
		const basic_String<Char, Traits, Core>& rhs
		) {
		return lhs.compare(rhs) <= 0;
	}

	template<typename Char, typename Traits, typename Core>
	bool operator<=(
		const basic_String<Char, Traits, Core>& lhs,
		const Char* rhs
		) {
		return lhs.compare(rhs) <= 0;
	}

	template<typename Char, typename Traits, typename Core>
	bool operator<=(
		const Char* lhs,
		const basic_String<Char, Traits, Core>& rhs
		) {
		return rhs.compare(lhs) >= 0;
	}

	template<typename Char, typename Traits, typename Core>
	bool operator>=(
		const basic_String<Char, Traits, Core>& lhs,
		const basic_String<Char, Traits, Core>& rhs
		) {
		return lhs.compare(rhs) >= 0;
	}

	template<typename Char, typename Traits, typename Core>
	bool operator>=(
		const basic_String<Char, Traits, Core>& lhs,
		const Char* rhs
		) {
		return lhs.compare(rhs) >= 0;
	}

	template<typename Char, typename Traits, typename Core>
	bool operator>=(
		const Char* lhs,
		const basic_String<Char, Traits, Core>& rhs
		) {
		return rhs.compare(lhs) <= 0;
	}

	template<typename Char, typename Traits, typename Core>
	void swap(basic_String<Char, Traits, Core>& lhs, 
		basic_String<Char, Traits, Core>& rhs) noexcept {
		lhs.swap(rhs);
	}

	template <typename Char, typename Traits, typename Core>
	std::basic_istream<Char, Traits>&
		operator>>(
			std::basic_istream<Char, Traits>& is,
			basic_String<Char, Traits, Core>& str) {
		using istream_type = std::basic_istream<Char, Traits>;
		typename istream_type::sentry sentry(is);
		size_t extracted = 0;
		typename istream_type::iostate err = istream_type::goodbit;
		if (sentry) {
			auto n = is.width();
			if (n <= 0) {
				n = str.max_size();
			}
			str.clear();
			auto sn = static_cast<size_t>(n);
			for (auto got = is.rdbuf()->sgetc(); extracted != sn; ++extracted) {
				if (got == Traits::eof()) {
					err |= istream_type::eofbit;
					is.width(0);
					break;
				}
				if (std::isspace(got)) {
					break;
				}
				str.push_back(got);
				got = is.rdbuf()->snextc();
			}
		}
		if (!extracted) {
			err |= istream_type::failbit;
		}
		if (err) {
			is.setstate(err);
		}
		return is;
	}

	template <typename Char, typename Traits, typename Core>
	inline std::basic_ostream<Char, Traits>&
		operator<<(
			std::basic_ostream<Char, Traits>& os,
			const basic_String<Char, Traits, Core>& str) {
#ifdef _LIBCPP_VERSION
		using ostream_type = std::basic_ostream<Char, Traits>;
		typename ostream_type::sentry _s(os);
		if (_s) {
			using _Ip = std::ostreambuf_iterator<Char, Traits>;
			size_t __len = str.size();
			bool __left =
				(os.flags() & ostream_type::adjustfield) == ostream_type::left;
			if (__pad_and_output(
				_Ip(os),
				str.data(),
				__left ? str.data() + __len : str.data(),
				str.data() + __len,
				os,
				os.fill())
				.failed()) {
				os.setstate(ostream_type::badbit | ostream_type::failbit);
			}
		}
#elif defined(_MSC_VER)
		typedef decltype(os.precision()) streamsize;
		// MSVC doesn't define __ostream_insert
		os.write(str.data(), static_cast<streamsize>(str.size()));
#else
		std::__ostream_insert(os, str.data(), str.size());
#endif
		return os;
	}

	// FUNCTION TEMPLATE getline
	template <class _Elem, class _Traits>
	std::basic_istream<_Elem, _Traits>& getline(std::basic_istream<_Elem, _Traits>&& istr,
		basic_String<_Elem, _Traits>& str,
		const _Elem _Delim) { // get characters into string, discard delimiter
		using _Myis = std::basic_istream<_Elem, _Traits>;

		typename _Myis::iostate _State = _Myis::goodbit;
		bool _Changed = false;
		const typename _Myis::sentry _Ok(istr, true);

		if (_Ok) { // state okay, extract characters
			str.clear();
			const typename _Traits::int_type _Metadelim = _Traits::to_int_type(_Delim);
			typename _Traits::int_type _Meta = istr.rdbuf()->sgetc();

			for (;; _Meta = istr.rdbuf()->snextc()) {
				if (_Traits::eq_int_type(_Traits::eof(), _Meta)) { // end of file, quit
					_State |= _Myis::eofbit;
					break;
				}
				else if (_Traits::eq_int_type(_Meta, _Metadelim)) { // got a delimiter, discard it and quit
					_Changed = true;
					istr.rdbuf()->sbumpc();
					break;
				}
				else if (str.max_size() <= str.size()) { // string too large, quit
					_State |= _Myis::failbit;
					break;
				}
				else { // got a character, add it to string
					str += _Traits::to_char_type(_Meta);
					_Changed = true;
				}
			}
		}

		if (!_Changed) {
			_State |= _Myis::failbit;
		}

		istr.setstate(_State);
		return istr;
	}

	template <class _Elem, class _Traits>
	std::basic_istream<_Elem, _Traits>& getline(std::basic_istream<_Elem, _Traits>&& istr,
		basic_String<_Elem, _Traits>& str) { // get characters into string, discard newline
		return getline(istr, str, istr.widen('\n'));
	}

	template <class _Elem, class _Traits>
	std::basic_istream<_Elem, _Traits>& getline(std::basic_istream<_Elem, _Traits>& istr, basic_String<_Elem, _Traits>& str,
		const _Elem _Delim) { // get characters into string, discard delimiter
		return getline(std::move(istr), str, _Delim);
	}

	template <class _Elem, class _Traits>
	std::basic_istream<_Elem, _Traits>& getline(std::basic_istream<_Elem, _Traits>& istr,
		basic_String<_Elem, _Traits>& str) { // get characters into string, discard newline
		return getline(std::move(istr), str, istr.widen('\n'));
	}

	using String           = basic_String<char, std::char_traits<char>>;
	using wString          = basic_String<wchar_t, std::char_traits<wchar_t>>;
#if defined(WJR_CPP_20)
	using u8String         = basic_String<char8_t, std::char_traits<char8_t>>;
#endif
	using u16String        = basic_String<char16_t, std::char_traits<char16_t>>;
	using u32String        = basic_String<char32_t, std::char_traits<char32_t>>;

	using String_view      = basic_String_view<char, std::char_traits<char>>;
	using wString_view     = basic_String_view<wchar_t, std::char_traits<wchar_t>>;
#if defined(WJR_CPP_20)
	using u8String_view    = basic_String_view<char8_t, std::char_traits<char8_t>>;
#endif
	using u16String_view   = basic_String_view<char16_t, std::char_traits<char16_t>>;
	using u32String_view   = basic_String_view<char32_t, std::char_traits<char32_t>>;

#ifdef TEST_SHARED_STRING

	using shared_String    = basic_String<char, std::char_traits<char>, shared_String_core<char>>;
	using shared_wString   = basic_String<wchar_t, std::char_traits<wchar_t>, shared_String_core<wchar_t>>;
#if defined(WJR_CPP_20)
	using shared_u8String  = basic_String<char8_t, std::char_traits<char8_t>, shared_String_core<char8_t>>;
#endif
	using shared_u16String = basic_String<char16_t, std::char_traits<char16_t>, shared_String_core<char16_t>>;
	using shared_u32String = basic_String<char32_t, std::char_traits<char32_t>, shared_String_core<char32_t>>;

#endif
}

#ifdef TEST_SHARED_STRING
#define DEFAULT_SHARED_STRING_HASH(T)                                                       \
template<>                                                                                  \
struct hash<wjr::basic_String<T, char_traits<T>, wjr::shared_String_core<T>>> {             \
    size_t operator()(const wjr::basic_String<T,                                            \
        char_traits<T>, wjr::shared_String_core<T>>& s)const {                              \
        return wjr::hash_array_representation(s.data(),s.size());                           \
    }                                                                                       \
};
#else
#define DEFAULT_SHARED_STRING_HASH(T)
#endif

#define DEFAULT_STRING_HASH(T)											                    \
template<>																                    \
struct hash<wjr::basic_String<T, char_traits<T>>> {						                    \
    size_t operator()(const wjr::basic_String<T,char_traits<T>>& s)const{                   \
        return wjr::hash_array_representation(s.data(),s.size());		                    \
    }																	                    \
};                                                                                          \
template<>                                                                                  \
struct hash<wjr::basic_String_view<T,char_traits<T>>> {                                     \
    constexpr size_t operator()(const wjr::basic_String_view<T,char_traits<T>>& s)const{    \
        return wjr::hash_array_representation(s.data(),s.size());                           \
    }                                                                                       \
};                                                                                          \
DEFAULT_SHARED_STRING_HASH(T)

namespace std {
	DEFAULT_STRING_HASH(char)
	DEFAULT_STRING_HASH(wchar_t)
#if defined(WJR_CPP_20)
	DEFAULT_STRING_HASH(char8_t)
#endif
	DEFAULT_STRING_HASH(char16_t)
	DEFAULT_STRING_HASH(char32_t)
}

#endif