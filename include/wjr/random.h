#pragma once
#ifndef __WJR_PRODUCT_H
#define __WJR_PRODUCT_H

#include <vector>
#include <functional>
#include <random>
#include <iostream>

#include <wjr/type_traits.h>

#define _WJR_MATH_BEGIN namespace math{
#define _WJR_MATH_END }

_WJR_BEGIN
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
	std::mt19937_64& get() {
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
	std::mt19937& get() {
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

	decltype(auto) get() const {
		return __static_random_engine<__digits>.get();
	}

	result_type uniform(result_type min, result_type max) const {
		return std::uniform_int_distribution<__result_type>(min, max)(get());
	}
	result_type binomial(result_type n, double p = 0.5) const {
		return std::binomial_distribution<__result_type>(n, p)(get());
	}
	result_type negative_binomial(result_type n, double p = 0.5) const {
		return std::negative_binomial_distribution<__result_type>(n, p)(get());
	}
	result_type geometric(double p = 0.5) const {
		std::uniform_real_distribution<double> dis;
		return std::geometric_distribution<__result_type>(p)(get());
	}
	result_type poisson(double mean = 1) const {
		return std::poisson_distribution<__result_type>(mean)(get());
	}
	
	/*
	template<typename iter, std::enable_if_t<
		std::conjunction_v<wjr::is_iterator<iter>, std::is_same<wjr::iter_val_t<iter>, result_type>>, int> = 0>
	result_type discrete(iter first, iter last) const {
		return std::discrete_distribution<__result_type>(first, last)(get());
	}
	result_type discrete(const std::initializer_list<double>& il) const {
		return std::discrete_distribution<__result_type>(il)(get());
	}
	*/

	template<typename Distribution>
	result_type distribution(Distribution&& dist) const {
		return std::forward<Distribution>(dist)(get());
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
	
	decltype(auto) get() const {
		return __static_random_engine<32>.get();
	}

	result_type bernoulli(double p = 0.5) const {
		return std::bernoulli_distribution(p)(get());
	}
	
};

template<>
class __random<unsigned long long> : public __random_int_base<unsigned long long> {
};

template<>
class __random<unsigned long> : public __random_int_base<unsigned long> {
};

template<>
class __random<unsigned int> : public __random_int_base<unsigned int> {
};

template<>
class __random<unsigned short> : public __random_int_base<unsigned short> {
};

template<>
class __random<unsigned char> : public __random_int_base<unsigned char> {
};

template<>
class __random<bool> : public __random_int_base<bool> {
};

template<>
class __random<long long> : public __random_int_base<long long> {
};

template<>
class __random<long> : public __random_int_base<long> {
};

template<>
class __random<int> : public __random_int_base<int> {
};

template<>
class __random<short> : public __random_int_base<short> {
};

template<>
class __random<char> : public __random_int_base<char> {
};

template<typename T>
class __random_real_base {
public:
	using result_type = T;
private:
	constexpr static size_t __digits =
		std::is_same_v<T, float> ? 32 : 64;
public:

	decltype(auto) get() const {
		return __static_random_engine<__digits>.get();
	}

	result_type uniform(result_type min, result_type max) const {
		return std::uniform_real_distribution<result_type>(min, max)(get());
	}

	result_type exponential(double lambda = 1) const {
		return std::exponential_distribution<result_type>(lambda)(get());
	}

	result_type gamma(double alpha = 1, double beta = 1) const {
		return std::gamma_distribution<result_type>(alpha, beta)(get());
	}

	result_type weibull(double a = 1, double b = 1) const {
		return std::weibull_distribution<result_type>(a, b)(get());
	}

	result_type extreme_value(double a = 0, double b = 1) const {
		return std::extreme_value_distribution<result_type>(a, b)(get());
	}

	result_type normal(double mean = 0, double stddev = 1) const {
		return std::normal_distribution<result_type>(mean, stddev)(get());
	}

	result_type lognormal(double mean = 0, double stddev = 1) const {
		return std::lognormal_distribution<result_type>(mean, stddev)(get());
	}

	result_type chi_squared(double n) const {
		return std::chi_squared_distribution<result_type>(n)(get());
	}

	result_type cauchy(double a = 0, double b = 1) const {
		return std::cauchy_distribution<result_type>(a, b)(get());
	}

	result_type fisher_f(double m, double n) const {
		return std::fisher_f_distribution<result_type>(m, n)(get());
	}

	result_type student_t(double n) const {
		return std::student_t_distribution<result_type>(n)(get());
	}

	template<typename Distribution>
	result_type distribution(Distribution&& dist) const {
		return std::forward<Distribution>(dist)(get());
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
_WJR_END

#undef _WJR_PRODUCT_BEGIN
#undef _WJR_PRODUCT_END

#endif // !PRODUCT_H