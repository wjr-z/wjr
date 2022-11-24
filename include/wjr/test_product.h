#pragma once
#ifndef __WJR_TEST_PRODUCT_H
#define __WJR_TEST_PRODUCT_H

#include <wjr/random.h>

_WJR_BEGIN

template<typename T>
struct __test_random : public wjr::math::__random<T>{
};

template<>
struct __test_random<std::string> {
	std::string operator()()const {
		std::string str;
		// generate a random string 
		// length in [1,20], Gaussian distribution
		auto len = static_cast<int>(std::round(wjr::math::random<double>.normal(10, 5)));
		for (int i = 0; i < len; ++i) {
			str += wjr::math::random<char>();
		}
		return str;
	}
};

template<typename T>
class __test_vector_product {
public:
	explicit __test_vector_product(size_t n) : vec(n) {
		std::generate_n(vec.begin(), n, __test_random<T>{});
	}
	explicit __test_vector_product(std::initializer_list<T> il) : vec(il) {}
	__test_vector_product(const __test_vector_product&) = default;
	__test_vector_product(__test_vector_product&&) noexcept = default;
	__test_vector_product& operator=(const __test_vector_product&) = default;
	__test_vector_product& operator=(__test_vector_product&&) noexcept = default;

	WJR_CONSTEXPR20 decltype(auto) begin() {
		return vec.begin();
	}
	WJR_CONSTEXPR20 decltype(auto) begin()const {
		return vec.begin();
	}
	WJR_CONSTEXPR20 decltype(auto) end() {
		return vec.end();
	}
	WJR_CONSTEXPR20 decltype(auto) end()const {
		return vec.end();
	}
	WJR_CONSTEXPR20 size_t size()const {
		return vec.size();
	}
	WJR_CONSTEXPR20 std::vector<T>& get() {
		return vec;
	}
	
private:
	std::vector<T> vec;
};

template<typename iter>
struct __test_input_iterator : public iter {
	using iter::iter;
	using iterator_category = std::input_iterator_tag;
	explicit __test_input_iterator(const iter& it) : iter(it) {}
	explicit __test_input_iterator(iter&& it) : iter(std::move(it)) {}
};

template<typename T>
class __test_list_product {
public:
	explicit __test_list_product(size_t n) : lst(n) {
		std::generate_n(lst.begin(), n, __test_random<T>{});
	}
	explicit __test_list_product(std::initializer_list<T> il) : lst(il) {}
	__test_list_product(const __test_list_product&) = default;
	__test_list_product(__test_list_product&&) noexcept = default;
	__test_list_product& operator=(const __test_list_product&) = default;
	__test_list_product& operator=(__test_list_product&&) noexcept = default;

	WJR_CONSTEXPR20 decltype(auto) begin() {
		return __test_input_iterator<decltype(lst.begin())>(lst.begin());
	}
	WJR_CONSTEXPR20 decltype(auto) begin()const {
		return __test_input_iterator<decltype(lst.begin())>{lst.begin()};
	}
	WJR_CONSTEXPR20 decltype(auto) end() {
		return __test_input_iterator<decltype(lst.end())>{lst.end()};
	}
	WJR_CONSTEXPR20 decltype(auto) end()const {
		return __test_input_iterator<decltype(lst.end())>{lst.end()};
	}
	WJR_CONSTEXPR20 size_t size()const {
		return lst.size();
	}
	WJR_CONSTEXPR20 std::vector<T>& get() {
		return lst;
	}
private:
	std::vector<T> lst;
};

_WJR_END

#endif