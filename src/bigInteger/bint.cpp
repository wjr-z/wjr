#include "bint.h"
#include <ctime>
#include <random>
#include <cassert>
namespace wjr {

	extern "C" { // use FFT
		extern void cdft(int, int, double*);
	}

	inline namespace bint_random { 
		static std::mt19937 mt_rand(std::time(NULL));
	}

	inline namespace bint_mem { // some memory operation
		inline void copy(uint32_t* dest, uint64_t value) { // copy a uint64_t number 
			*((uint64_t*)dest) = value;
		}
		uint64_t get64(const uint32_t* dest) {
			return *((uint64_t*)dest);
		}
	}

	inline namespace bint_information { // some information

		template<size_t n>
		struct _Bit_Pow {
			constexpr static size_t value = _Bit_Pow<n / 2>::value + 1;
		};
		template<>
		struct _Bit_Pow<1> {
			constexpr static size_t value = 0;
		};

		constexpr static size_t byte          = sizeof(uint32_t);
		constexpr static size_t bit_length    = byte << 3;
		constexpr static size_t bit_pow       = _Bit_Pow<bit_length>::value;
		constexpr static size_t bit_mask      = bit_length - 1;
		constexpr static uint32_t max_uint    = (uint32_t)(-1);
		constexpr static uint64_t max_ull     = (uint64_t)(-1);
		constexpr static uint32_t uint32_mask = max_uint ^ ((uint32_t)(1) << bit_mask);
		constexpr static uint64_t uint64_mask = max_ull ^ ((uint64_t)(1) << ((sizeof(uint64_t) << 3) - 1));

		inline uint32_t qabs(int value) {
			return value >= 0 ? value : -value;
		}
		
		inline uint64_t qabs(long long value) {
			return value >= 0 ? value : -value;
		}

	}

	inline namespace bint_math { // math for bigInteger
		static const char* const tabel = "\0\0\1\1\2\2\2\2\3\3\3\3\3\3\3\3";

		size_t qlog2(uint32_t x) {
			size_t ans = 0;
			if (x >> 16){ ans |= 16; x >>= 16; }
			if (x >> 8) { ans |= 8 ;  x >>= 8; }
			if (x >> 4) { ans |= 4 ;  x >>= 4; }
			return ans | tabel[x];
		}

		size_t qlog2(uint64_t x) {
			size_t ans = 0;
			if (x >> 32){ ans |= 32; x >>= 32; }
			if (x >> 16){ ans |= 16; x >>= 16; }
			if (x >> 8) { ans |= 8 ;  x >>= 8; }
			if (x >> 4) { ans |= 4 ;  x >>= 4; }
			return ans | tabel[x];
		}
	}

	bint2::bint2()
		: num(1) , positive(true) {

	}

	bint2::bint2(const bint2& other) 
		: num(other.num) , positive(other.positive){

	}

	bint2::bint2(bint2&& other)noexcept
		: num(std::move(other.num)), positive(other.positive) {

	}

	bint2::bint2(int value)
		: num(1,qabs(value)), positive(value >= 0) {

	}

	bint2::bint2(unsigned int value)
		: num(1,value), positive(true) {

	}

	bint2::bint2(long long value)
		: num(2), positive(value >= 0) {
		unsigned long long u_value = qabs(value);
		uint32_t* num_ptr = &num[0];
		copy(num_ptr,u_value);
		if(!num_ptr[1])num.pop_back();
	}

	bint2::bint2(unsigned long long value)
		: num(2), positive(value >= 0) {
		uint32_t* num_ptr = &num[0];
		copy(num_ptr, value);
		if(!num_ptr[1])num.pop_back();
	}

	bint2& bint2::operator=(const bint2& other) {
		if(this == std::addressof(other))return *this;
		num = other.num;
		return *this;
	}

	bint2& bint2::operator=(bint2&& other)noexcept {
		if(this == std::addressof(other))return *this;
		num = std::move(other.num);
		return *this;
	}

	bint2& bint2::operator=(int value) {
		unsigned int u_value = qabs(value);
		num.resize(1);
		num[0] = u_value;
		positive = value >= 0;
		return *this;
	}

	bint2& bint2::operator=(unsigned int value) {
		num.resize(1);
		num[0] = value;
		positive = true;
		return *this;
	}
	
	bint2& bint2::operator=(long long value) {
		unsigned long long u_value = qabs(value);
		num.resize(2);
		uint32_t* num_ptr = &num[0];
		copy(num_ptr,u_value);
		if(!num_ptr[1])num.pop_back();
		positive = value >= 0;
		return *this;
	}

	bint2& bint2::operator=(unsigned long long value) {
		num.resize(2);
		uint32_t* num_ptr = &num[0];
		copy(num_ptr,value);
		if(!num_ptr[1])num.pop_back();
		positive = value >= 0;
		return *this;
	}

	inline uint32_t bint2::qadd(const uint32_t x, const uint32_t y, uint32_t& _jw) {
		//x += y + jw
		//if x overflows then jw = 1 
		//else jw = 0
		//if (!y && !jw) then jw = 0
		//else if ((x+y+jw) < x) then jw = 1
		uint32_t val = x + y + _jw;
		//Eliminate branch optimization
		_jw = static_cast<uint32_t>((val) <= (x - (!y && !_jw)));
		return val;
	}

	inline uint32_t bint2::qdel(const uint32_t x, const uint32_t y, uint32_t& _jw) {
		uint32_t val = x - (y + _jw);
		//Eliminate branch optimization
		_jw = static_cast<uint32_t>((x + (!y && !_jw)) <= (val));
		return val;
	}

	void bint2::slow_mul(bint2& lhs, const bint2& rhs) {
		const size_t l = lhs.size(),
						r = rhs.size();
		const size_t len = l + r - 1; 
		// the size of lhs may be len or len + 1
		// to avoid reallocating memory
		// the initial length is defined as len + 1
		num_type vec(len + 1); // temporary array
		const uint32_t* numl = lhs.begin();
		const uint32_t* numr = rhs.begin();
		uint32_t* num = &vec[0]; // just use pointer 
		uint64_t _jw = 0;
		for (size_t _i = 0; _i < len; ++_i) {
			uint32_t val = static_cast<uint32_t>(_jw);
			_jw >>= bit_length;
			size_t _end = l < _i + 1 ? l : _i + 1;
			for (size_t _j = _i > r - 1 ? _i - r + 1 : 0, _k = _i - _j; _j < _end; ++_j,--_k) {
				uint64_t Val = val + static_cast<uint64_t>(numl[_j]) * numr[_k] ;
				val = static_cast<uint32_t>(Val);
				_jw += Val >> bit_length;
			}
			num[_i] = val;
		}
		if (_jw) {
			num[len] = _jw;
		}else vec.pop_back();
		lhs.num.swap(vec);
	}

	void bint2::fft_mul_4(bint2& lhs, const bint2& rhs) {

		constexpr static size_t quarter_bit_length       = bit_length >> 2;
		constexpr static size_t two_quarter_bit_length   = quarter_bit_length << 1;
		constexpr static size_t three_quarter_bit_length = quarter_bit_length + two_quarter_bit_length;
		constexpr static uint32_t quarter_mask           = (1 << quarter_bit_length) - 1;

		const size_t n = lhs.size() << 2, 
					 m = rhs.size() << 2;

		const size_t len   = n + m - 1;
		const uint32_t bit = qlog2(len - 1) + 1;
		const size_t s     = (size_t)(1) << bit;

		double* a = new double[s << 1]; // temporaray array for FFT

		auto numl = lhs.begin();
		auto numr = rhs.begin();

		for (size_t i = 0; i < n; i += 4) {
			uint32_t val = numl[i >> 2];

			const uint32_t q = val & quarter_mask;
			val            >>= quarter_bit_length;
			const uint32_t w = val & quarter_mask;
			val            >>= quarter_bit_length;
			const uint32_t e = val & quarter_mask;
			val            >>= quarter_bit_length;
			const uint32_t r = val & quarter_mask;

			a[i << 1]       = a[i << 1 | 1]       = q * 0.5;
			a[(i + 1) << 1] = a[(i + 1) << 1 | 1] = w * 0.5;
			a[(i + 2) << 1] = a[(i + 2) << 1 | 1] = e * 0.5;
			a[(i + 3) << 1] = a[(i + 3) << 1 | 1] = r * 0.5;
		}

		memset(a + (n << 1),0,sizeof(double) * ((s - n) << 1));

		for (size_t i = 0; i < m; i += 4) {
			uint32_t val = numr[i >> 2];

			const uint32_t q = val & quarter_mask;
			val			   >>= quarter_bit_length;
			const uint32_t w = val & quarter_mask;
			val			   >>= quarter_bit_length;
			const uint32_t e = val & quarter_mask;
			val			   >>= quarter_bit_length;
			const uint32_t r = val & quarter_mask;

			a[i << 1]           += q * 0.5;
			a[i << 1 | 1]       -= q * 0.5;
			a[(i + 1) << 1]     += w * 0.5;
			a[(i + 1) << 1 | 1] -= w * 0.5;
			a[(i + 2) << 1]     += e * 0.5;
			a[(i + 2) << 1 | 1] -= e * 0.5;
			a[(i + 3) << 1]     += r * 0.5;
			a[(i + 3) << 1 | 1] -= r * 0.5;
		}

		cdft(s << 1, 1, a);

		for (size_t i = 0; i < s; ++i) {
			const double sa = a[i << 1], sb = a[i << 1 | 1];
			a[i << 1]     = sa * sa - sb * sb;
			a[i << 1 | 1] = 2 * sa * sb;
		}

		cdft(s << 1, -1, a);

		const double invs = 1.0 / static_cast<double>(s);
		for (size_t i = 0; i < s; ++i)
			a[i << 1] *= invs;

		lhs.num.resize((len >> 2) + 1);
		auto num = lhs.begin();
		num[0] = 0;

		for (size_t i = 0; i < len - 3; i += 4) {
			const uint64_t now 
				= static_cast<uint64_t>(a[i << 1] + 0.5)
				+ (static_cast<uint64_t>(a[(i + 1) << 1] + 0.5) << quarter_bit_length)
				+ (static_cast<uint64_t>(a[(i + 2) << 1] + 0.5) << two_quarter_bit_length)
				+ (static_cast<uint64_t>(a[(i + 3) << 1] + 0.5) << three_quarter_bit_length)
				+ num[i >> 2];
			num[i >> 2]       = static_cast<uint32_t>(now);
			num[(i >> 2) + 1] = now >> bit_length;
		}

		num[len >> 2] += static_cast<long long>(a[(len - 3) << 1] + 0.5) +
			(static_cast<long long>(a[(len - 2) << 1] + 0.5) << quarter_bit_length) +
			(static_cast<long long>(a[(len - 1) << 1] + 0.5) << two_quarter_bit_length);

		delete[]a;
	}

	void bint2::fft_mul_2(bint2& lhs, const bint2& rhs) {

		constexpr static size_t half_bit_length = bit_length >> 1;
		constexpr static uint32_t half_mask     = (1 << half_bit_length) - 1;

		const size_t n = lhs.size() << 1, 
					 m = rhs.size() << 1;

		const size_t len   = n + m - 1;
		const uint32_t bit = qlog2(len - 1) + 1;
		const size_t s     = (size_t)(1) << bit;

		double* a = new double[s << 1];

		auto numl = lhs.begin();
		auto numr = rhs.begin();

		for (size_t i = 0; i < n; i += 2) {

			const uint32_t q = numl[i>>1] >> half_bit_length, 
						   p = numl[i>>1] & half_mask;

			a[i << 1]		= a[i << 1 | 1]		  = p * 0.5;
			a[(i + 1) << 1] = a[(i + 1) << 1 | 1] = q * 0.5;
		}

		memset(a+(n<<1),0,sizeof(double)*((s-n)<<1));

		for (size_t i = 0; i < m; i += 2) {

			const uint32_t q = numr[i>>1] >> half_bit_length, 
						   p = numr[i>>1] & half_mask;

			a[i << 1]		    += p * 0.5;
			a[i << 1 | 1]		-= p * 0.5;
			a[(i + 1) << 1]	    += q * 0.5;
			a[(i + 1) << 1 | 1] -= q * 0.5;
		}

		cdft(s << 1, 1, a);

		for (size_t i = 0; i < s; ++i) {
			const double sa = a[i << 1], sb = a[i << 1 | 1];
			a[i << 1]	  = sa * sa - sb * sb;
			a[i << 1 | 1] = 2 * sa * sb;
		}

		cdft(s << 1, -1, a);

		const double invs = 1.0 / static_cast<double>(s);
		for (size_t i = 0; i < s; ++i)
			a[i << 1] *= invs;

		lhs.num.resize((len >> 1) + 1);
		auto num = lhs.begin();
		num[0] = 0;

		for (size_t i = 0; i < len - 1; i += 2) {
			const uint64_t now 
				= static_cast<uint64_t>(a[i << 1] + 0.5) + 
				(static_cast<uint64_t>(a[(i + 1) << 1] + 0.5) << half_bit_length)
				+ num[i >> 1];
			num[i >> 1]		  = static_cast<uint32_t>(now);
			num[(i >> 1) + 1] = now >> bit_length;
		}
		num[len >> 1] += static_cast<uint64_t>(a[(len - 1) << 1] + 0.5);
		lhs.maintain();
		delete[]a;
	}

	void bint2::fft_mul(bint2& lhs, const bint2& rhs) {
		static_assert(sizeof(double) == 8, "sizeof double must be 8");
		const size_t size = lhs.size() + rhs.size();
		if (size <= (1 << (48 - (bit_length)))) { // ensure accuracy
			fft_mul_2(lhs,rhs);
		}
		else {
			fft_mul_4(lhs,rhs);
		}
		// always reserve one more bit for possible carry-bit
		// if num.back() == 0 then need to pop_back
		if (!lhs.num.back())
			lhs.num.pop_back();
	}

	size_t bint2::almost_fft_time(const size_t n) {
		if (n <= (1 << (48 - bit_length))) {
			const size_t bit = qlog2(2 * n - 1) + 1;
			const size_t s = (size_t)(1) << bit;
			return 11 * s * bit;
		}
		else {
			const size_t bit = qlog2(4 * n - 1) + 1;
			const size_t s = (size_t)(1) << bit;
			return 15 * s * bit;
		}
	}

	void bint2::knuth_divide(bint2& lhs,bint2& rhs) {
		bint2 ans;

		const uint32_t a = 1 << (bit_length - 1);
		const uint32_t b = rhs.num[rhs.size() - 1];
		const uint32_t k = b < a ? (a + b - 1) / b : 1;
		bint2 _k(k);

		if (k != 1) {
			lhs *= _k;
			rhs *= _k;
		}

		const size_t n = rhs.size();
		const size_t m = lhs.size() - n;

		auto numl = lhs.begin();
		auto numr = rhs.begin();

		bint2 re;
		re.num.reserve(n + 1);
		re.assign(numl + m,numl + n + m);

		size_t i = n + m, j = m;

		const uint32_t _back = numr[n-1];

		ans.num.resize(m + 1);

		auto num = ans.begin();
		auto num_re = re.begin();
		do {
			uint32_t q;
			if (re.size() > n) {
				q = std::min((uint64_t)max_uint,(((uint64_t)(num_re[n])<<bit_length) | num_re[n-1]) / _back);
			}
			else if (re.size() > n - 1) {
				q = std::min(max_uint,num_re[n-1] / _back);
			}
			else q = 0;

			re -= rhs * bint2(q);

			while (!re.positive) {
				--q;
				re += rhs;
			}

			if (j > 0) {
				re.num.insert(re.num.begin(),0);
				num_re[0] = numl[j-1];
			}

			num[j] = q;
			--i;
			--j;
		} while (i >= n);

		lhs = std::move(ans);
		rhs = std::move(re);

		lhs.maintain();

		if (k != 1)rhs /= k;
	}

	void bint2::unsigned_div(bint2& lhs, bint2& rhs) {

		if (unsigned_compare(lhs,rhs) < 0) {
			lhs.num.swap(rhs.num);
			lhs.reset();
			return ;
		}

		const size_t n = lhs.size(), m = rhs.size();
		//if (m <= ((qlog2(n) + 16) << 1)) {
			return knuth_divide(lhs,rhs);
		//}
	}

	void bint2::add(bint2& lhs, const bint2& rhs) {

		// the special case of 0 does not need to be considered
		// the speed is almost the same

		const size_t l = lhs.size(), 
						r = rhs.size();
		// to avoid reallocating memory
		lhs.num.reserve(r + 1);
		auto numr = rhs.begin();

		if (l < r) { 
			// the len of lhs is less than rhs
			// then copy [l,r) form rhs instead adding
			lhs.append_head(numr + l,numr + r);
		}

		auto numl = lhs.begin();
		const size_t min_length = l < r ? l : r;
		
		uint32_t _jw = 0;
		// only the part less than the minimum length needs to be calculated
		for (size_t _i = 0; _i < min_length; ++_i) {
			numl[_i] = qadd(numl[_i],numr[_i],_jw);
		}

		if (_jw) {
			const size_t max_length = l > r ? l : r;
			size_t _i			   = min_length;
			// find a continuous segment of max_uint
			for (; _i < max_length && numl[_i] == max_uint; ++_i);
			// only need to memset
			memset(numl + min_length, 0, sizeof(uint32_t) * (_i - min_length));
			// if they are all max_uint ,then need to add one digit
			if (_i == max_length) {
				lhs.num.push_back(1);
			}
			else ++numl[_i];
		}
		//else don't need to maintain
	}

	void bint2::del(bint2& lhs, const bint2& rhs) {

		// the special case of 0 does not need to be considered
		// the speed is almost the same

		const size_t l = lhs.size(),
					 r = rhs.size();

		auto numl = lhs.begin();
		auto numr = rhs.begin();
		uint32_t _jw = 0;

		if (unsigned_compare(lhs,rhs) >= 0) {
			for (size_t _i = 0; _i < r; ++_i) {
				numl[_i] = qdel(numl[_i],numr[_i],_jw);
			}
		}
		else {
			// if lhs < rhs
			// then need to change the sign
			lhs.opposite();
			lhs.append_head(numr + l,numr + r);
			numl = lhs.begin();
			for (size_t _i = 0; _i < l; ++_i) {
				numl[_i] = qdel(numr[_i],numl[_i],_jw);
			}
		}
		if (_jw) {
			const size_t min_length = l < r ? l : r;
			const size_t max_length = l > r ? l : r;
			size_t _i			   = min_length;
			for (; _i < max_length && numl[_i] == 0; ++_i);
			memset(numl + min_length, 0xff, sizeof(uint32_t) * (_i - min_length));
			//i won't be max_length
			assert(_i < max_length);
			--numl[_i];
		}
		//need to maintain 
		//because the length of lhs may reduce
		lhs.maintain();
	}

	void bint2::mul(bint2& lhs, const bint2& rhs) {
		if (lhs.is_zero() || rhs.is_zero()) {
			lhs.reset();
			return ;
		}

		lhs.positive ^= !rhs.positive;
		// do not consider symbols in slow_mul of fft_mul
		const size_t l = lhs.size(),
					 r = rhs.size();
		const size_t _min = l <= r ? l : r,
					 _max = l >= r ? l : r;

		if (_min <= 32 || (5 * l * r <= almost_fft_time(l + r - 1))) {
			slow_mul(lhs,rhs);
		}
		else {
			fft_mul(lhs,rhs);
		}

		if(lhs.is_zero())
			lhs.positive = true;
	}

	void bint2::div(bint2& lhs, bint2& rhs) {
		bool positive = !(lhs.positive ^ rhs.positive);
		lhs.abs(),rhs.abs();
		unsigned_div(lhs,rhs);
	}

	bint2& bint2::operator+=(const bint2& other) {
		if (this == std::addressof(other)) {
			const size_t length = size();
			uint32_t* _num = &num[0];
			uint32_t _jw = 0;
			for (size_t _i = 0; _i < length; ++_i) {
				_num[_i] = qadd(_num[_i],_num[_i],_jw);
			}
			if (_jw) {
				num.push_back(_jw);
			}
		}
		else {
			if(positive == other.positive)
				add(*this,other);
			else del(*this,other);
		}
		return *this;
	}

	bint2& bint2::operator-=(const bint2& other) {
		if (this == std::addressof(other)) {
			reset();
		}
		else {
			if(positive == other.positive)
				del(*this,other);
			else add(*this,other);
		}

		return *this;
	}

	bint2& bint2::operator*=(const bint2& other) {
		mul(*this,other);
		return *this;
	}

	bint2& bint2::operator/=(const bint2& other) {
		bint2 mo(other);
		div(*this,mo);
		return *this;
	}

	bint2 operator+(const bint2&lhs, const bint2&rhs) {
		bint2 ans(lhs);
		ans += rhs;
		return ans;
	}

	int bint2::unsigned_compare(const bint2& lhs, const bint2& rhs) {
		const size_t l = lhs.size(), r = rhs.size();
		if(l != r)
			return l < r ? -1 : 1;
		auto lv = lhs.begin() + l - 1, rv = rhs.begin() + l - 1;
		auto e = lv - l;
		while (lv != e) {
			if (*lv != *rv)return (*lv < *rv) ? -1 : 1;
			--lv;
			--rv;
		}
		return 0;
	}

	bool operator==(const bint2& lhs, const bint2& rhs) {
		const size_t l = lhs.size(), r = rhs.size();
		return (l == r) && (lhs.positive == rhs.positive) 
			&& (memcmp(lhs.begin(),rhs.begin(),sizeof(uint32_t) * l) == 0);
	}

	bool operator!=(const bint2& lhs, const bint2& rhs) {
		const size_t l = lhs.size(),r = rhs.size();
		return (l != r) || (lhs.positive != rhs.positive) 
			|| (memcmp(lhs.begin(), rhs.begin(), sizeof(uint32_t) * l) != 0);
	}

	bool operator<(const bint2& lhs, const bint2& rhs) {
		const size_t l = lhs.size(),r = rhs.size();
		if(lhs.positive != rhs.positive)
			return !lhs.positive;
		if(l != r)
			return (l < r) ^ (!lhs.positive);
		auto lv = lhs.begin() + l - 1,rv = rhs.begin() + l - 1;
		auto e = lv - l;
		while (lv != e) {
			if(*lv != *rv)return (*lv < *rv) ^ (!lhs.positive);
			--lv;
			--rv;
		}
		return false;
	}

	bool operator<=(const bint2& lhs, const bint2& rhs) {
		const size_t l = lhs.size(), r = rhs.size();
		if (lhs.positive != rhs.positive)
			return !lhs.positive;
		if (l != r)
			return (l < r) ^ (!lhs.positive);
		auto lv = lhs.begin() + l - 1, rv = rhs.begin() + l - 1;
		auto e = lv - l;
		while (lv != e) {
			if (*lv != *rv)return (*lv < *rv) ^ (!lhs.positive);
			--lv;
			--rv;
		}
		return true;
	}

	bool operator>(const bint2& lhs, const bint2& rhs) {
		return rhs < lhs;
	}

	bool operator>=(const bint2& lhs, const bint2& rhs) {
		return rhs <= lhs;
	}

	bool operator==(const bint2& lhs, int rhs) { //checked
		unsigned int u_value = qabs(rhs);
		return (lhs.size() == 1) && (lhs.positive == (rhs >= 0)) && (lhs.num[0] == u_value);
	}

	bool operator==(int lhs, const bint2& rhs) { //checked
		return rhs == lhs;
	}

	bool operator==(const bint2& lhs, unsigned int rhs) { //checked
		return (lhs.size() == 1) && (lhs.positive) && (lhs.num[0] == rhs);
	}

	bool operator==(unsigned int lhs, const bint2& rhs) { //checked
		return rhs == lhs;
	}

	bool operator==(const bint2& lhs, long long rhs) { //checked
		const size_t n = lhs.size();
		unsigned long long u_value = qabs(rhs);
		switch (n) {
		case 1 : return (u_value <= max_uint) && (lhs.positive == (rhs >= 0)) && (lhs.num[0] == (unsigned int)u_value);
		case 2 : return (lhs.positive == (rhs >= 0)) && (get64(lhs.begin()) == u_value);
		default: return false;
		}
	}

	bool operator==(long long lhs, const bint2& rhs) { //checked
		return rhs == lhs;
	}

	bool operator==(const bint2& lhs, unsigned long long rhs) { //checked
		const size_t n = lhs.size();
		switch (n) {
		case 1 : return (rhs <= max_uint) && (lhs.positive) && (lhs.num[0] == (unsigned int)rhs);
		case 2 : return (lhs.positive) && (get64(lhs.begin()) == rhs);
		default: return false;
		}
	}

	bool operator==(unsigned long long lhs, const bint2& rhs) { //checked
		return rhs == lhs;
	}

	bool operator!=(const bint2& lhs, int rhs) { //checked
		return !(lhs == rhs);
	}

	bool operator!=(int lhs, const bint2& rhs) { //checked
		return !(rhs == lhs);
	}

	bool operator!=(const bint2& lhs, unsigned int rhs) { //checked
		return !(lhs == rhs);
	}

	bool operator!=(unsigned int lhs, const bint2& rhs) { //checked
		return !(rhs == lhs);
	}

	bool operator!=(const bint2& lhs, long long rhs) { //checked
		return !(lhs == rhs);
	}

	bool operator!=(long long lhs, const bint2& rhs) { //checked
		return !(rhs == lhs);
	}

	bool operator!=(const bint2& lhs, unsigned long long rhs) { //checked
		return !(lhs == rhs);
	}

	bool operator!=(unsigned long long lhs, const bint2& rhs) { //checked
		return !(rhs == lhs);
	}

	bool operator<(const bint2& lhs, int rhs) { //checked
		unsigned int u_value = qabs(rhs);
		return (((lhs.positive == (rhs >= 0)) && (lhs.size() == 1)) && (lhs.num[0] < u_value))
			^ (!lhs.positive);
	}

	bool operator<(int lhs, const bint2& rhs) { //checked
		unsigned int u_value = qabs(lhs);
		return ((rhs.positive == (lhs >= 0)) && !((rhs.size() == 1) && !(lhs < rhs.num[0])) )
			^ (lhs < 0);
	}

	bool operator<(const bint2& lhs, unsigned int rhs) { //checked
		return ((lhs.positive) && (lhs.size() == 1) && (lhs.num[0] < rhs) );
	}

	bool operator<(unsigned int lhs, const bint2& rhs) { //checked
		return ((rhs.positive) && !((rhs.size() == 1) && !(lhs < rhs.num[0])) );
	}

	bool operator<(const bint2& lhs, long long rhs) {
		const size_t n = lhs.size();
		unsigned long long u_value = qabs(rhs);
		switch (n) {
		case 1 : return ((u_value <= max_uint) && (lhs.positive == (rhs >= 0) ) 
			&& (lhs.num[0] < (unsigned int)(u_value))) ^ (!lhs.positive);
		case 2 : return (((lhs.positive == (rhs >= 0))) 
			&& (get64(lhs.begin()) < u_value)) ^ (!lhs.positive);
		default: return !lhs.positive;
		}
	}

	bool operator<(long long lhs, const bint2& rhs) {
		const size_t n = rhs.size();
		unsigned long long u_value = qabs(lhs);
		switch (n) {
		case 1 : return ((u_value <= max_uint) && (rhs.positive == (lhs >= 0))
			&& ((unsigned int)(u_value) < rhs.num[0])) ^ (lhs < 0);
		case 2 : return ((rhs.positive == (lhs >= 0)) 
			&& (u_value < get64(rhs.begin()))) ^ (lhs < 0);
		default: return rhs.positive;
		}
	}

	bool operator<(const bint2& lhs, unsigned long long rhs) {
		const size_t n = lhs.size();
		switch (n) {
		case 1: return ((rhs <= max_uint) && (lhs.positive)
			&& (lhs.num[0] < (unsigned int)(rhs)));
		case 2: return ((lhs.positive)
			&& (get64(lhs.begin()) < rhs));
		default: return !lhs.positive;
		}
	}

	bool operator<(unsigned long long lhs, const bint2& rhs) {
		const size_t n = rhs.size();
		switch (n) {
		case 1: return ((lhs <= max_uint) && (rhs.positive)
			&& ((unsigned int)(lhs) < rhs.num[0]));
		case 2: return ((rhs.positive)
			&& (lhs < get64(rhs.begin())));
		default: return rhs.positive;
		}
	}

	bool operator<=(const bint2& lhs, int rhs) {
		unsigned int u_value = qabs(rhs);
		return (((lhs.positive == (rhs >= 0)) && (lhs.size() == 1)) && (lhs.num[0] <= u_value))
			^ (!lhs.positive);
	}

	bool operator<=(int lhs, const bint2& rhs) {
		unsigned int u_value = qabs(lhs);
		return ((rhs.positive == (lhs >= 0)) && !((rhs.size() == 1) && !(lhs <= rhs.num[0])))
			^ (lhs < 0);
	}

	bool operator<=(const bint2& lhs, unsigned int rhs) {
		return ((lhs.positive) && (lhs.size() == 1) && (lhs.num[0] <= rhs));
	}

	bool operator<=(unsigned int lhs, const bint2& rhs) {
		return ((rhs.positive) && !((rhs.size() == 1) && !(lhs <= rhs.num[0])));
	}

	bool operator<=(const bint2& lhs, long long rhs) {
		const size_t n = lhs.size();
		unsigned long long u_value = qabs(rhs);
		switch (n) {
		case 1: return ((u_value <= max_uint) && (lhs.positive == (rhs >= 0))
			&& (lhs.num[0] <= (unsigned int)(u_value))) ^ (!lhs.positive);
		case 2: return (((lhs.positive == (rhs >= 0)))
			&& (get64(lhs.begin()) <= u_value)) ^ (!lhs.positive);
		default: return !lhs.positive;
		}
	}

	bool operator<=(long long lhs, const bint2& rhs) {
		const size_t n = rhs.size();
		unsigned long long u_value = qabs(lhs);
		switch (n) {
		case 1: return ((u_value <= max_uint) && (rhs.positive == (lhs >= 0))
			&& ((unsigned int)(u_value) <= rhs.num[0])) ^ (lhs < 0);
		case 2: return ((rhs.positive == (lhs >= 0))
			&& (u_value <= get64(rhs.begin()))) ^ (lhs < 0);
		default: return rhs.positive;
		}
	}

	bool operator<=(const bint2& lhs, unsigned long long rhs) {
		const size_t n = lhs.size();
		switch (n) {
		case 1: return ((rhs <= max_uint) && (lhs.positive)
			&& (lhs.num[0] <= (unsigned int)(rhs)));
		case 2: return (((lhs.positive))
			&& (get64(lhs.begin()) <= rhs));
		default: return !lhs.positive;
		}
	}

	bool operator<=(unsigned long long lhs, const bint2& rhs) {
		const size_t n = rhs.size();
		switch (n) {
		case 1: return ((lhs <= max_uint) && (rhs.positive)
			&& ((unsigned int)(lhs) <= rhs.num[0]));
		case 2: return ((rhs.positive == (lhs >= 0))
			&& (lhs <= get64(rhs.begin())));
		default: return rhs.positive;
		}
	}

	bool operator>(const bint2& lhs, int rhs) {
		return rhs < lhs;
	}

	bool operator>(int lhs, const bint2& rhs) {
		return rhs < lhs;
	}

	bool operator>(const bint2& lhs, unsigned int rhs) {
		return rhs < lhs;
	}

	bool operator>(unsigned int lhs, const bint2& rhs) {
		return rhs < lhs;
	}

	bool operator>(const bint2& lhs, long long rhs) {
		return rhs < lhs;
	}

	bool operator>(long long lhs, const bint2& rhs) {
		return rhs < lhs;
	}

	bool operator>(const bint2& lhs, unsigned long long rhs) {
		return rhs < lhs;
	}

	bool operator>(unsigned long long lhs, const bint2& rhs) {
		return rhs < lhs;
	}

	bool operator>=(const bint2& lhs, int rhs) {
		return rhs <= lhs;
	}

	bool operator>=(int lhs, const bint2& rhs) {
		return rhs <= lhs;
	}

	bool operator>=(const bint2& lhs, unsigned int rhs) {
		return rhs <= lhs;
	}

	bool operator>=(unsigned int lhs, const bint2& rhs) {
		return rhs <= lhs;
	}

	bool operator>=(const bint2& lhs, long long rhs) {
		return rhs <= lhs;
	}

	bool operator>=(long long lhs, const bint2& rhs) {
		return rhs <= lhs;
	}

	bool operator>=(const bint2& lhs, unsigned long long rhs) {
		return rhs <= lhs;
	}

	bool operator>=(unsigned long long lhs, const bint2& rhs) {
		return rhs <= lhs;
	}

	uint32_t* bint2::begin() {
		return &num[0];
	}

	const uint32_t* bint2::begin()const {
		return &num[0];
	}

	const uint32_t* bint2::cbegin()const {
		return &num[0];
	}

	uint32_t* bint2::end() {
		return begin() + size();
	}

	const uint32_t* bint2::end()const {
		return begin() + size();
	}

	const uint32_t* bint2::cend()const {
		return begin() + size();
	}

	void bint2::reset() {
		num.resize(1);
		num[0] = 0 ;
		positive = true;
	}

	void bint2::clear() {
		reset();
	}

	size_t bint2::size()const {
		return num.size();
	}

	size_t bint2::capacity()const {
		return num.capacity();
	}

	bool bint2::is_zero()const {
		return size() == 1 && num[0] == 0;
	}

	bool bint2::is_positive()const {
		return positive == true;
	}

	void bint2::abs() {
		positive = true;
	}

	void bint2::opposite() {
	#ifdef _DEBUG
		if(is_zero())
			assert(positive == true);
	#endif
		positive = !(positive ^ is_zero());
	}

	void bint2::maintain() {
		num.resize(find_non_zero(size() - 1));
		if(is_zero())positive = true;
	}

	bint2 bint2::rand(const size_t n) {
		bint2 x;
		const size_t l = n >> bit_pow;
		auto mask = n & bit_mask;
		x.num.resize(l + (mask != 0));
		auto num = &x.num[0];
		for (size_t _i = 0; _i < l; ++_i) {
			num[_i] = static_cast<uint32_t>(mt_rand());
		}
		if (mask != 0) {
			num[l] = static_cast<uint32_t>(mt_rand()) % (max_uint) + 1;
		}
		return x;
	}

	std::string bint2::to_string() {
		std::string str;
		auto x = num[size()-1];
		while (x != 0) {
			str.push_back('0'+(x&1));
			x>>=1;
		}
		std::reverse(std::begin(str),std::end(str));
		for (size_t i = size() - 2; ~i; --i) {
			for (size_t j = 31; ~j; --j) {
				str.push_back('0' + ((num[i]>>j)&1));
			}
		}return str;
	}

	size_t bint2::find_non_zero(size_t _i) {
		uint32_t*e = begin();
		uint32_t*s = e + _i;
		for(;s!=e && !*s;--s);
		return s - e + 1;
	}

	void bint2::append_head(const uint32_t* l,const uint32_t* r) {
		num.insert(num.end(),l,r);
	}

	void bint2::assign(const uint32_t* l, const uint32_t* r) {
		num.assign(l,r);
	}
}