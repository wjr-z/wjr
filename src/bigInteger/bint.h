#ifndef BINT_H
#define BINT_H

#include <vector>
#include <algorithm>
#include <string>
#include "../Allocator.h"

namespace wjr {

	class bint2 {
	private:
		using num_type = std::vector<uint32_t, Allocator<uint32_t>>;
	public:

		bint2();
		bint2(const bint2&);
		bint2(bint2&&)noexcept;

		bint2(int);
		bint2(unsigned int);
		bint2(long long);
		bint2(unsigned long long);

		~bint2() = default;

		bint2& operator=(const bint2&);
		bint2& operator=(bint2&&)noexcept;
		bint2& operator=(int);
		bint2& operator=(unsigned int);
		bint2& operator=(long long);
		bint2& operator=(unsigned long long);

	private:
		inline static uint32_t qadd(const uint32_t,const uint32_t,uint32_t&);
		inline static uint32_t qdel(const uint32_t,const uint32_t,uint32_t&);

		static void slow_mul(bint2&,const bint2&);
		static void fft_mul_4(bint2&,const bint2&);
		static void fft_mul_2(bint2&,const bint2&);
		static void fft_mul(bint2&,const bint2&);
		static size_t almost_fft_time(const size_t n);

		static void knuth_divide(bint2&,bint2&);
		static void unsigned_div(bint2&,bint2&);
		
		static void add(bint2&,const bint2&);
		static void del(bint2&,const bint2&);
		static void mul(bint2&,const bint2&);
		static void div(bint2&,bint2&);
	public:
		bint2& operator+=(const bint2&);
		bint2& operator-=(const bint2&);
		bint2& operator*=(const bint2&);
		bint2& operator/=(const bint2&);

		static int unsigned_compare(const bint2&,const bint2&);
		friend bool operator==(const bint2&,const bint2&);
		friend bool operator!=(const bint2&,const bint2&);
		friend bool operator<(const bint2&,const bint2&);
		friend bool operator<=(const bint2&,const bint2&);
		friend bool operator>(const bint2&,const bint2&);
		friend bool operator>=(const bint2&,const bint2&);

		friend bool operator==(const bint2&, int);
		friend bool operator==(int, const bint2&);
		friend bool operator==(const bint2&, unsigned int);
		friend bool operator==(unsigned int, const bint2&);
		friend bool operator==(const bint2&, long long);
		friend bool operator==(long long, const bint2&);
		friend bool operator==(const bint2&, unsigned long long);
		friend bool operator==(unsigned long long, const bint2&);

		friend bool operator!=(const bint2&,int);
		friend bool operator!=(int,const bint2&);
		friend bool operator!=(const bint2&,unsigned int);
		friend bool operator!=(unsigned int,const bint2&);
		friend bool operator!=(const bint2&,long long);
		friend bool operator!=(long long,const bint2&);
		friend bool operator!=(const bint2&,unsigned long long);
		friend bool operator!=(unsigned long long ,const bint2&);

		friend bool operator<(const bint2&, int);
		friend bool operator<(int, const bint2&);
		friend bool operator<(const bint2&, unsigned int);
		friend bool operator<(unsigned int, const bint2&);
		friend bool operator<(const bint2&, long long);
		friend bool operator<(long long, const bint2&);
		friend bool operator<(const bint2&, unsigned long long);
		friend bool operator<(unsigned long long, const bint2&);

		friend bool operator<=(const bint2&, int);
		friend bool operator<=(int, const bint2&);
		friend bool operator<=(const bint2&, unsigned int);
		friend bool operator<=(unsigned int, const bint2&);
		friend bool operator<=(const bint2&, long long);
		friend bool operator<=(long long, const bint2&);
		friend bool operator<=(const bint2&, unsigned long long);
		friend bool operator<=(unsigned long long, const bint2&);

		friend bool operator>(const bint2&, int);
		friend bool operator>(int, const bint2&);
		friend bool operator>(const bint2&, unsigned int);
		friend bool operator>(unsigned int, const bint2&);
		friend bool operator>(const bint2&, long long);
		friend bool operator>(long long, const bint2&);
		friend bool operator>(const bint2&, unsigned long long);
		friend bool operator>(unsigned long long, const bint2&);

		friend bool operator>=(const bint2&, int);
		friend bool operator>=(int, const bint2&);
		friend bool operator>=(const bint2&, unsigned int);
		friend bool operator>=(unsigned int, const bint2&);
		friend bool operator>=(const bint2&, long long);
		friend bool operator>=(long long, const bint2&);
		friend bool operator>=(const bint2&, unsigned long long);
		friend bool operator>=(unsigned long long, const bint2&);

		uint32_t* begin();
		const uint32_t* begin()const;
		const uint32_t* cbegin()const;

		uint32_t* end();
		const uint32_t* end()const;
		const uint32_t* cend()const;

		void reset();
		void clear();
		inline size_t size()const;
		inline size_t capacity()const;

		bool is_zero()const;
		bool is_positive()const;
		void abs();
		void opposite();

		void maintain();

		static bint2 rand(const size_t);

		std::string to_string();

	private:

		size_t find_non_zero(size_t);

		void append_head(const uint32_t*,const uint32_t*);
		void assign(const uint32_t*,const uint32_t*);

		num_type num;
		bool positive;
	};
	
	template<typename T>
	using is_bint2 = std::enable_if_t<std::is_same_v<std::decay_t<T>, bint2>, int>;

	template<typename T,typename U,is_bint2<T> = 0,is_bint2<U> = 0>
	bint2 operator+(T&& lhs, U&& rhs) {
		if constexpr (std::is_rvalue_reference_v<U>) {
			bint2 ans(std::move(rhs));
			ans += lhs;
			return ans;
		}
		else {
			bint2 ans(std::forward<T>(lhs));
			ans+=rhs;
			return ans;
		}
	}

	template<typename T, typename U, is_bint2<T> = 0, is_bint2<U> = 0>
	bint2 operator-(T&& lhs, U&& rhs) {
		if constexpr (std::is_rvalue_reference_v<U>) {
			bint2 ans(std::move(rhs));
			ans -= lhs;
			ans.opposite();
			return ans;
		}
		else {
			bint2 ans(std::forward<T>(lhs));
			ans -= rhs;
			return ans;
		}
	}

	template<typename T,typename U,is_bint2<T> = 0,is_bint2<U> = 0>
	bint2 operator*(T&& lhs, U&& rhs) {
		if constexpr (std::is_rvalue_reference_v<U>) {
			bint2 ans(std::move(rhs));
			ans *= lhs;
			return ans;
		}
		else {
			bint2 ans(std::forward<T>(lhs));
			ans *= rhs;
			return ans;
		}
	}

}

#endif