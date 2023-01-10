#pragma once
#ifndef __WJR_PRODUCT_H
#define __WJR_PRODUCT_H

#include <vector>
#include <functional>
#include <random>
#include <iostream>

#include <wjr/type_traits.h>

_WJR_MATH_BEGIN

template<size_t bits>
class __random_engine;

template<size_t bits>
static __random_engine<bits> __static_random_engine{};

template<>
class __random_engine<64>{
	std::mt19937_64 gen{ std::random_device{}() };
public:
	uint64_t operator()() {
		return gen();
	}
	std::mt19937_64& engine() {
		return gen;
	}
};

template<>
class __random_engine<32> {
	std::mt19937 gen{ std::random_device{}() };
public:
	uint32_t operator()() {
		return gen();
	}
	std::mt19937& engine() {
		return gen;
	}
};

template<>
class __random_engine<16> {
	uint32_t buf = 0;
	uint32_t siz = 0;
public:
	uint16_t operator()() {
		if (siz == 0) {
			buf = __static_random_engine<32>();
			siz = 32;
		}
		siz -= 16;
		return static_cast<uint16_t>(buf >> siz);
	}
};

template<>
class __random_engine<8> {
	uint32_t buf = 0;
	uint32_t siz = 0;
public:
	uint8_t operator()() {
		if (siz == 0) {
			buf = __static_random_engine<32>();
			siz = 32;
		}
		siz -= 8;
		return static_cast<uint8_t>(buf >> siz);
	}
};

template<>
class __random_engine<1>{
	uint32_t buf = 0;
	uint32_t siz = 0;
public:
	bool operator()() {
		if (siz == 0) {
			buf = __static_random_engine<32>();
			siz = 32;
		}
		siz -= 1;
		return static_cast<bool>(buf >> siz);
	}
};

template<typename T>
class __random;

template<typename T>
inline static __random<T> random{};

template<typename T>
class __random_int_base {
public:
	using result_type = T;
private:
	constexpr static size_t digits = std::numeric_limits<std::make_unsigned_t<T>>::digits;
	constexpr static size_t __digits = digits <= 32 ? 32 : digits;
	using __result_type = std::conditional_t<
		digits <= 8,
		std::conditional_t<std::is_unsigned_v<T>, unsigned short, short>,
		result_type
	>;
public:

	result_type operator()() const {
		return __static_random_engine<digits>();
	}

	decltype(auto) engine() const {
		return __static_random_engine<__digits>.engine();
	}

	result_type uniform(result_type min, result_type max) const {
		return std::uniform_int_distribution<__result_type>(min, max)(engine());
	}
	result_type binomial(result_type n, double p = 0.5) const {
		return std::binomial_distribution<__result_type>(n, p)(engine());
	}
	result_type negative_binomial(result_type n, double p = 0.5) const {
		return std::negative_binomial_distribution<__result_type>(n, p)(engine());
	}
	result_type geometric(double p = 0.5) const {
		std::uniform_real_distribution<double> dis;
		return std::geometric_distribution<__result_type>(p)(engine());
	}
	result_type poisson(double mean = 1) const {
		return std::poisson_distribution<__result_type>(mean)(engine());
	}

	template<typename Distribution>
	result_type distribution(Distribution&& dist) const {
		return std::forward<Distribution>(dist)(engine());
	}
};

template<>
class __random_int_base<bool> {
public:
	constexpr static size_t digits = 1;
	using result_type = bool;

	result_type operator()() const {
		return __static_random_engine<digits>();
	}
	
	decltype(auto) engine() const {
		return __static_random_engine<32>.engine();
	}

	result_type bernoulli(double p = 0.5) const {
		return std::bernoulli_distribution(p)(engine());
	}
	
};

#define __REGISTER_WJR_RANDOM(x)						\
template<>	                                            \
class __random<x> : public __random_int_base<x>{};

__REGISTER_WJR_RANDOM(bool)
__REGISTER_WJR_RANDOM(char)
__REGISTER_WJR_RANDOM(signed char)
__REGISTER_WJR_RANDOM(unsigned char)
__REGISTER_WJR_RANDOM(wchar_t)
__REGISTER_WJR_RANDOM(char16_t)
__REGISTER_WJR_RANDOM(char32_t)
__REGISTER_WJR_RANDOM(short)
__REGISTER_WJR_RANDOM(unsigned short)
__REGISTER_WJR_RANDOM(int)
__REGISTER_WJR_RANDOM(unsigned int)
__REGISTER_WJR_RANDOM(long)
__REGISTER_WJR_RANDOM(unsigned long)
__REGISTER_WJR_RANDOM(long long)
__REGISTER_WJR_RANDOM(unsigned long long)

#undef __REGISTER_WJR_RANDOM

template<typename T>
class __random_real_base {
public:
	using result_type = T;
private:
	constexpr static size_t __digits =
		std::is_same_v<T, float> ? 32 : 64;
public:

	decltype(auto) engine() const {
		return __static_random_engine<__digits>.engine();
	}

	result_type uniform(result_type min, result_type max) const {
		return std::uniform_real_distribution<result_type>(min, max)(engine());
	}

	result_type exponential(double lambda = 1) const {
		return std::exponential_distribution<result_type>(lambda)(engine());
	}

	result_type gamma(double alpha = 1, double beta = 1) const {
		return std::gamma_distribution<result_type>(alpha, beta)(engine());
	}

	result_type weibull(double a = 1, double b = 1) const {
		return std::weibull_distribution<result_type>(a, b)(engine());
	}

	result_type extreme_value(double a = 0, double b = 1) const {
		return std::extreme_value_distribution<result_type>(a, b)(engine());
	}

	result_type normal(double mean = 0, double stddev = 1) const {
		return std::normal_distribution<result_type>(mean, stddev)(engine());
	}

	result_type lognormal(double mean = 0, double stddev = 1) const {
		return std::lognormal_distribution<result_type>(mean, stddev)(engine());
	}

	result_type chi_squared(double n) const {
		return std::chi_squared_distribution<result_type>(n)(engine());
	}

	result_type cauchy(double a = 0, double b = 1) const {
		return std::cauchy_distribution<result_type>(a, b)(engine());
	}

	result_type fisher_f(double m, double n) const {
		return std::fisher_f_distribution<result_type>(m, n)(engine());
	}

	result_type student_t(double n) const {
		return std::student_t_distribution<result_type>(n)(engine());
	}

	template<typename Distribution>
	result_type distribution(Distribution&& dist) const {
		return std::forward<Distribution>(dist)(engine());
	}
};


template<>
class __random<float> : public __random_real_base<float> {
};

template<>
class __random<double> : public __random_real_base<double> {
};

template<>
class __random<long double> : public __random_real_base<long double> {
};

_WJR_MATH_END

#endif // !PRODUCT_H