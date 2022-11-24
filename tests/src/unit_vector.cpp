#include <vector>
#include <chrono>

#include <random>

#include <gtest/gtest.h>

#include <wjr/vector.h>
#include <wjr/allocator.h>

#include <wjr/test_product.h>

template<typename Output, typename iter>
void op(Output& o, iter _First, iter _Last) {
	int n = std::distance(_First, _Last);
	o << '[';
	if (n <= 32) {
		for (int i = 0; i < n; ++i) {
			if (i != 0)o << ',';
			o << _First[i];
		}
	}
	else {
		for (int i = 0; i < 16; ++i) {
			if (i != 0)o << ',';
			o << _First[i];
		}
		o << "... ";
		for (int i = n - 16; i < n; ++i) {
			if (i != n - 16)o << ',';
			o << _First[i] << ' ';
		}
	}
	o << ']';
}

template<typename iter1, typename iter2>
testing::AssertionResult eq(iter1 _First1, iter1 _Last1, iter2 _First2, iter2 _Last2) {
	using value_type1 = typename std::iterator_traits<iter1>::value_type;
	using value_type2 = typename std::iterator_traits<iter2>::value_type;
	std::vector<value_type1> data1(_First1, _Last1);
	std::vector<value_type2> data2(_First2, _Last2);
	bool eq = std::equal(data1.begin(), data1.end(), data2.begin(), data2.end());
	if (eq) {
		return testing::AssertionSuccess();
	}
	else {
		testing::AssertionResult result = testing::AssertionFailure();
		result << "Expected: ";
		op(result, data2.begin(), data2.end());
		result << "Actual: ";
		op(result, data1.begin(), data1.end());

		if (data1.size() != data2.size()) {
			result << "size mismatch: " << data1.size() << " != " << data2.size();
		}
		else {
			for (int i = 0; i < data1.size(); ++i) {
				if (data1[i] != data2[i]) {
					result << "First difference at " << i << ": " << data1[i] << " " << data2[i];
					break;
				}
			}
		}

		return result;
	}
}

template<typename iter>
testing::AssertionResult teq(iter _First, iter _Last, 
	std::initializer_list<typename std::iterator_traits<iter>::value_type> il) {
	return eq(_First, _Last, il.begin(), il.end());
}

// pod : char, int
// non-pod : string

// TODO :
// 1. test for non-pod with different allocator.

template<typename T>
using default_allocator = wjr::test_allocator<T>;

template<typename T>
using normal_vector = wjr::vector<T, default_allocator<T>, wjr::vector_core<T, default_allocator<T>>>;

template<typename T, size_t C = 0>
using sso_vector = wjr::vector<T, default_allocator<T>, wjr::vector_sso_core<T, default_allocator<T>, C>>;

#define __INIT_TEST														\
	using container_type = typename TestFixture::container_type;		\
	using value_type = typename TestFixture::value_type;				\
	constexpr size_t min_capacity = container_type::min_capacity;		

using vectorTestList = testing::Types <
	normal_vector<char>,
	normal_vector<int>,
	normal_vector<std::string>,
	//sso_vector<int, 1>,
	//sso_vector<int, 2>,
	//sso_vector<int, 4>,
	sso_vector<int, 8>,
	sso_vector<std::string, 1>,
	sso_vector<std::string, 2>,
	sso_vector<std::string, 4>,
	sso_vector<std::string, 8>
>;

template<typename T>
class vectorTest : public testing::Test {
public:
	using container_type = T;
	using value_type = typename container_type::value_type;
};

TYPED_TEST_SUITE(vectorTest, vectorTestList);

// vector();
TYPED_TEST(vectorTest, constructor_1) {
	__INIT_TEST;
	container_type v;
	EXPECT_EQ(v.size(), 0);
	EXPECT_GE(v.capacity(), 0);
}

// vector(const allocator_type& al);
TYPED_TEST(vectorTest, constructor_2) {
	__INIT_TEST;
	container_type vec(default_allocator<value_type>{});
	EXPECT_EQ(vec.size(), 0);
	EXPECT_GE(vec.capacity(), 0);
}

// vector(size_type n, const T& val, const allocator_type& al = allocator_type());
TYPED_TEST(vectorTest,constructor_3) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		{
			auto val = wjr::__test_random<value_type>{}();
			container_type vec(n, val);
			EXPECT_EQ(vec.size(), n) << "when " << "n = " << n;
			EXPECT_GE(vec.capacity(), n) << "when " << "n = " << n;
			for (auto& v : vec) {
				EXPECT_EQ(v, val);
			}
		}
		{
			auto val = value_type{};
			container_type vec(n, val);
			EXPECT_EQ(vec.size(), n) << "when " << "n = " << n;
			EXPECT_GE(vec.capacity(), n) << "when " << "n = " << n;
			for (auto& v : vec) {
				EXPECT_EQ(v, val);
			}
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}


// vector(size_type _Count, const allocator_type& al = allocator_type());
TYPED_TEST(vectorTest, constructor_4) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		auto val = value_type{};
		container_type vec(n);
		EXPECT_EQ(vec.size(), n) << "when " << "n = " << n;
		EXPECT_GE(vec.capacity(), n) << "when " << "n = " << n;
		for (auto& v : vec) {
			EXPECT_EQ(v, val);
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector(iter _First, iter _Last, const allocator_type& al = allocator_type());
TYPED_TEST(vectorTest, constructor_5) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			EXPECT_EQ(vec.size(), n) << "when " << "n = " << n;
			EXPECT_GE(vec.capacity(), n) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec.begin(), vec.end(), product.begin(), product.end()));
		}
		{
			wjr::__test_list_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			EXPECT_EQ(vec.size(), n) << "when " << "n = " << n;
			EXPECT_GE(vec.capacity(), n) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec.begin(), vec.end(), product.begin(), product.end()));
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector(const vector& other);
TYPED_TEST(vectorTest,constructor_6) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		wjr::__test_vector_product<value_type> product(n);
		container_type vec(product.begin(), product.end());
		container_type vec2(vec);
		EXPECT_EQ(vec2.size(), n) << "when " << "n = " << n;
		EXPECT_GE(vec2.capacity(), n) << "when " << "n = " << n;
		EXPECT_TRUE(eq(vec2.begin(), vec2.end(), product.begin(), product.end()));
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector(const vector&, const allocator_type& al);
TYPED_TEST(vectorTest, constructor_7) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		wjr::__test_vector_product<value_type> product(n);
		container_type vec(product.begin(), product.end());
		container_type vec2(vec, default_allocator<value_type>{});
		EXPECT_EQ(vec2.size(), n) << "when " << "n = " << n;
		EXPECT_GE(vec2.capacity(), n) << "when " << "n = " << n;
		EXPECT_TRUE(eq(vec2.begin(), vec2.end(), product.begin(), product.end()));
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector(vector&& other);
TYPED_TEST(vectorTest, constructor_8) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		wjr::__test_vector_product<value_type> product(n);
		container_type vec(product.begin(), product.end());
		container_type vec2(std::move(vec));
		EXPECT_EQ(vec2.size(), n) << "when " << "n = " << n;
		EXPECT_GE(vec2.capacity(), n) << "when " << "n = " << n;
		EXPECT_EQ(vec.size(), 0) << "when " << "n = " << n;
		EXPECT_TRUE(eq(vec2.begin(), vec2.end(), product.begin(), product.end()));
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector(vector&&, const allocator_type& al);
TYPED_TEST(vectorTest, constructor_9) {
	// TODO
}

// vector(initializer_list<value_type> ilist);
TYPED_TEST(vectorTest, constructor_10) {
	// TODO;
}

// vector& operator=(const vector& other);
TYPED_TEST(vectorTest, operator_assign_1) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		// vector product :
		// 1. n <= size()
		// 2. size() < n <= capacity()
		// 3. capacity() < n

		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			container_type vec2;
			vec2.resize(n);
			vec2 = vec;
			EXPECT_EQ(vec2.size(), n) << "when " << "n = " << n;
			EXPECT_GE(vec2.capacity(), n) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec2.begin(), vec2.end(), product.begin(), product.end()));
		}
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			container_type vec2;
			vec2.reserve(n);
			vec2 = vec;
			EXPECT_EQ(vec2.size(), n) << "when " << "n = " << n;
			EXPECT_GE(vec2.capacity(), n) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec2.begin(), vec2.end(), product.begin(), product.end()));
		}
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			container_type vec2;
			vec2 = vec;
			EXPECT_EQ(vec2.size(), n) << "when " << "n = " << n;
			EXPECT_GE(vec2.capacity(), n) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec2.begin(), vec2.end(), product.begin(), product.end()));
		}

	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector& operator=(vector&& other);
TYPED_TEST(vectorTest, operator_assign_2) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			container_type vec2;
			vec2 = std::move(vec);
			EXPECT_EQ(vec2.size(), n) << "when " << "n = " << n;
			EXPECT_GE(vec2.capacity(), n) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec2.begin(), vec2.end(), product.begin(), product.end()));
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector& operator=(initializer_list<value_type> ilist);
TYPED_TEST(vectorTest, operator_assign_3) {
	// TODO;
}

// vector::assign(size_type count, const value_type& value);
TYPED_TEST(vectorTest, assign_1) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		// vector product :
		// 1. n <= size()
		// 2. size() < n <= capacity()
		// 3. capacity() < n

		{
			auto val = wjr::__test_random<value_type>{}();
			container_type vec;
			vec.resize(n);
			vec.assign(n, val);
			EXPECT_EQ(vec.size(), n) << "when " << "n = " << n;
			EXPECT_GE(vec.capacity(), n) << "when " << "n = " << n;
			for (auto& i : vec) {
				EXPECT_EQ(i, val);
			}
		}
		{
			auto val = wjr::__test_random<value_type>{}();
			container_type vec;
			vec.reserve(n);
			vec.assign(n, val);
			EXPECT_EQ(vec.size(), n) << "when " << "n = " << n;
			EXPECT_GE(vec.capacity(), n) << "when " << "n = " << n;
			for (auto& i : vec) {
				EXPECT_EQ(i, val);
			}
		}
		{
			auto val = wjr::__test_random<value_type>{}();
			container_type vec;
			vec.assign(n, val);
			EXPECT_EQ(vec.size(), n) << "when " << "n = " << n;
			EXPECT_GE(vec.capacity(), n) << "when " << "n = " << n;
			for (auto& i : vec) {
				EXPECT_EQ(i, val);
			}
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector::assign(iter _First, iter _Last);
TYPED_TEST(vectorTest, assign_2) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		// vector product :
		// 1. n <= size()
		// 2. size() < n <= capacity()
		// 3. capacity() < n

		// list product :
		// 1. n <= size()
		// 2. size() < n

		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec;
			vec.resize(n);
			vec.assign(product.begin(), product.end());
			EXPECT_EQ(vec.size(), n) << "when " << "n = " << n;
			EXPECT_GE(vec.capacity(), n) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec.begin(), vec.end(), product.begin(), product.end()));
		}
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec;
			vec.reserve(n);
			vec.assign(product.begin(), product.end());
			EXPECT_EQ(vec.size(), n) << "when " << "n = " << n;
			EXPECT_GE(vec.capacity(), n) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec.begin(), vec.end(), product.begin(), product.end()));
		}
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec;
			vec.assign(product.begin(), product.end());
			EXPECT_EQ(vec.size(), n) << "when " << "n = " << n;
			EXPECT_GE(vec.capacity(), n) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec.begin(), vec.end(), product.begin(), product.end()));
		}

		{
			wjr::__test_list_product<value_type> product(n);
			container_type vec;
			vec.resize(n);
			vec.assign(product.begin(), product.end());
			EXPECT_EQ(vec.size(), n) << "when " << "n = " << n;
			EXPECT_GE(vec.capacity(), n) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec.begin(), vec.end(), product.begin(), product.end()));
		}
		{
			wjr::__test_list_product<value_type> product(n);
			container_type vec;
			vec.assign(product.begin(), product.end());
			EXPECT_EQ(vec.size(), n) << "when " << "n = " << n;
			EXPECT_GE(vec.capacity(), n) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec.begin(), vec.end(), product.begin(), product.end()));
		}

	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector::assign(initializer_list<value_type> ilist);
TYPED_TEST(vectorTest, assign_3) {
	// TODO;
}

// vector::at(size_type pos);
TYPED_TEST(vectorTest, at_1) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			auto iter = product.begin();
			for (size_t i = 0; i < n; ++i, ++iter) {
				EXPECT_EQ(vec.at(i), (*iter)) << "when " << "n = " << n << ", i = " << i;
			}
			try {
				vec.at(n);
				FAIL() << "when " << "n = " << n;
			}
			catch (std::out_of_range&) {
				// pass
			}
			catch (...) {
				FAIL() << "when " << "n = " << n;
			}
		}
		{
			wjr::__test_list_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			auto iter = product.begin();
			for (size_t i = 0; i < n; ++i, ++iter) {
				EXPECT_EQ(vec.at(i), (*iter)) << "when " << "n = " << n << ", i = " << i;
			}
			try {
				vec.at(n);
				FAIL() << "when " << "n = " << n;
			}
			catch (std::out_of_range&) {
				// pass
			}
			catch (...) {
				FAIL() << "when " << "n = " << n;
			}
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector::at(size_type pos) const;
TYPED_TEST(vectorTest, at_2) {
	// TODO;
}



// vector::operator[]
TYPED_TEST(vectorTest, operator_square_brackets_1) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			auto iter = product.begin();
			for (size_t i = 0; i < n; ++i, ++iter) {
				EXPECT_EQ(vec[i], (*iter)) << "when " << "n = " << n << ", i = " << i;
			}
		}
		{
			wjr::__test_list_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			auto iter = product.begin();
			for (size_t i = 0; i < n; ++i, ++iter) {
				EXPECT_EQ(vec[i], (*iter)) << "when " << "n = " << n << ", i = " << i;
			}
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector::operator[] const
void test_vector_operator_square_brackets_2() {
	// TODO
}

// vector::front()
TYPED_TEST(vectorTest, front_1) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			EXPECT_EQ(vec.front(), *product.begin()) << "when " << "n = " << n;
		}
		{
			wjr::__test_list_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			EXPECT_EQ(vec.front(), *product.begin()) << "when " << "n = " << n;
		}
	};
	//test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector::front() const
TYPED_TEST(vectorTest, front_2) {
	// TODO
}

// vector::back()
TYPED_TEST(vectorTest, back_1) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			EXPECT_EQ(vec.back(), *(--product.end())) << "when " << "n = " << n;
		}
		{
			wjr::__test_list_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			EXPECT_EQ(vec.back(), *(--product.end())) << "when " << "n = " << n;
		}
	};
	//test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector::back() const
void test_vector_back_2() {
	// TODO
}

// vector::data()
TYPED_TEST(vectorTest, data_1) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			EXPECT_EQ(vec.data(), &vec[0]) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec.data(), vec.data() + vec.size(), product.begin(), product.end())) << "when " << "n = " << n;
		}
		{
			wjr::__test_list_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			EXPECT_EQ(vec.data(), &vec[0]) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec.data(), vec.data() + vec.size(), product.begin(), product.end())) << "when " << "n = " << n;
		}
	};
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector::data() const
TYPED_TEST(vectorTest, data_2) {
	// TODO
}

// vector::empty() const
TYPED_TEST(vectorTest, empty) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			EXPECT_EQ(vec.empty(), n == 0) << "when " << "n = " << n;
		}
		{
			wjr::__test_list_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			EXPECT_EQ(vec.empty(), n == 0) << "when " << "n = " << n;
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector::size() const
TYPED_TEST(vectorTest, size) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			EXPECT_EQ(vec.size(), n) << "when " << "n = " << n;
		}
		{
			wjr::__test_list_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			EXPECT_EQ(vec.size(), n) << "when " << "n = " << n;
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector::reserve()
TYPED_TEST(vectorTest, reserve) {
	__INIT_TEST;
	constexpr auto __test = [](size_t n, size_t m) {
		{
			wjr::__test_vector_product<value_type> product(m);
			container_type vec(product.begin(), product.end());
			vec.resize(n);
			vec.reserve(m);
			EXPECT_EQ(vec.size(), n) << "when " << "n = " << n << ", m = " << m;
			EXPECT_GE(vec.capacity(), m) << "when " << "n = " << n << ", m = " << m;
		}
		{
			wjr::__test_list_product<value_type> product(m);
			container_type vec(product.begin(), product.end());
			vec.resize(n);
			vec.reserve(m);
			EXPECT_EQ(vec.size(), n) << "when " << "n = " << n << ", m = " << m;
			EXPECT_GE(vec.capacity(), m) << "when " << "n = " << n << ", m = " << m;
		}
	};
	constexpr auto test = [__test](size_t n) {
		__test(n, n);
		__test(n, n + 1);
		__test(n, n + 4);
		__test(n, n + 16);
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector::capacity() const
TYPED_TEST(vectorTest, capacity) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			EXPECT_GE(vec.capacity(), n) << "when " << "n = " << n;
		}
		{
			wjr::__test_list_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			EXPECT_GE(vec.capacity(), n) << "when " << "n = " << n;
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector::shrink_to_fit()

// vector:;clear()
TYPED_TEST(vectorTest, clear) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			vec.clear();
			EXPECT_EQ(vec.size(), 0) << "when " << "n = " << n;
			EXPECT_GE(vec.capacity(), n) << "when " << "n = " << n;
		}
		{
			wjr::__test_list_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			vec.clear();
			EXPECT_EQ(vec.size(), 0) << "when " << "n = " << n;
			EXPECT_GE(vec.capacity(), n) << "when " << "n = " << n;
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}


// vector::insert(const_iterator pos, const T& value)
TYPED_TEST(vectorTest, insert_1) {
	__INIT_TEST;
	constexpr auto __test = [min_capacity](size_t n) {
		// 1. size() == capacity()
		// 2. size() < capacity() 
		// 2.2 where == end()
		// 2.3 where != end()
		
		{
			const size_t k = std::max(n, min_capacity);
			wjr::__test_vector_product<value_type> product(k);
			container_type vec(product.begin(), product.end());
			vec.resize(k);
			ASSERT_EQ(vec.size(), vec.capacity()) << "when " << "n = " << n;
			auto pos = wjr::math::random<size_t>.uniform(0, vec.size());
			auto val = wjr::__test_random<value_type>{}();
			vec.insert(vec.begin() + pos, val);
			EXPECT_EQ(vec.size(), k + 1) << "when " << "n = " << n << ", pos = " << pos;
			EXPECT_EQ(vec[pos], val) << "when " << "n = " << n << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin(), vec.begin() + pos, product.begin(), product.begin() + pos))
				<< "when " << "n = " << n << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin() + pos + 1, vec.end(), product.begin() + pos, product.end()))
				<< "when " << "n = " << n << ", pos = " << pos;
		}
		
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			vec.reserve(n + 1);
			auto val = wjr::__test_random<value_type>{}();
			vec.insert(vec.end(), val);
			EXPECT_EQ(vec.size(), n + 1) << "when " << "n = " << n;
			EXPECT_EQ(vec.back(), val) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec.begin(), vec.end() - 1, product.begin(), product.end()))
				<< "when " << "n = " << n;
		}
		
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			vec.reserve(n + 1);
			auto pos = wjr::math::random<size_t>.uniform(0, vec.size());
			auto val = wjr::__test_random<value_type>{}();
			vec.insert(vec.begin() + pos, val);
			EXPECT_EQ(vec.size(), n + 1) << "when " << "n = " << n << ", pos = " << pos;
			EXPECT_EQ(vec[pos], val) << "when " << "n = " << n << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin(), vec.begin() + pos, product.begin(), product.begin() + pos))
				<< "when " << "n = " << n << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin() + pos + 1, vec.end(), product.begin() + pos, product.end()))
				<< "when " << "n = " << n << ", pos = " << pos;
		}
	};

	constexpr auto test = [__test](size_t n) {
		auto _Count = n < 1 ? 1 : n;
		for (size_t i = 0; i < _Count; ++i) {
			__test(n);
		}
	};

	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector::insert(const_iterator pos, T&& value)
TYPED_TEST(vectorTest, insert_2) {
	// 
}

// vector::insert(const_iterator pos, size_type count, const T& value)
TYPED_TEST(vectorTest, insert_3) {
	__INIT_TEST;
	constexpr auto __test = [](int n, int m) {
		// __rest = capacity() - size()
		// __elements_after = size() - pos
		// 1. m <= __rest
		// 1.1 m < _elements_after
		// 1.2 m >= _elements_after
		// 2. m > __rest
		int kl = std::max(n - m, 0);
		if(kl > 0) {
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			vec.reserve(m + vec.size());
			auto pos = wjr::math::random<size_t>.uniform(0, kl - 1);
			auto val = wjr::__test_random<value_type>{}();
			vec.insert(vec.begin() + pos, m, val);
			EXPECT_EQ(vec.size(), n + m) << "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			for (int i = 0; i < m; ++i) {
				EXPECT_EQ(vec[pos + i], val);
			}
			EXPECT_TRUE(eq(vec.begin(), vec.begin() + pos, product.begin(), product.begin() + pos))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin() + pos + m, vec.end(), product.begin() + pos, product.end()))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
		}
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			vec.reserve(m + vec.size());
			auto pos = wjr::math::random<size_t>.uniform(kl, n);
			auto val = wjr::__test_random<value_type>{}();
			vec.insert(vec.begin() + pos, m, val);
			EXPECT_EQ(vec.size(), n + m) << "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			for (int i = 0; i < m; ++i) {
				EXPECT_EQ(vec[pos + i], val);
			}
			EXPECT_TRUE(eq(vec.begin(), vec.begin() + pos, product.begin(), product.begin() + pos))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin() + pos + m, vec.end(), product.begin() + pos, product.end()))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
		}
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			auto pos = wjr::math::random<size_t>.uniform(0, vec.size());
			auto val = wjr::__test_random<value_type>{}();
			vec.insert(vec.begin() + pos, m, val);
			EXPECT_EQ(vec.size(), n + m) << "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			for (int i = 0; i < m; ++i) {
				EXPECT_EQ(vec[pos + i], val);
			}
			EXPECT_TRUE(eq(vec.begin(), vec.begin() + pos, product.begin(), product.begin() + pos))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin() + pos + m, vec.end(), product.begin() + pos, product.end()))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
		}
	};
	constexpr auto test = [__test](int n) {
		auto _Count = n < 1 ? 1 : n;
		for (int i = 0; i < _Count; ++i) {
			__test(n, 1);
			__test(n, 2);
			__test(n, 4);
			__test(n, 8);
			__test(n, 16);
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}



// vector::insert(const_iterator pos, InputIt first, InputIt last)
TYPED_TEST(vectorTest, insert_4) {
	// 根据 <wjr/vector.h> 中对应 insert 函数的各个分支进行相应的测试
	__INIT_TEST;
	constexpr auto __test = [](int n, int m) {
		// vector product :
		// __rest = capacity () - size()
		// __elements_after = size() - pos
		// 1 m <= __rest
		// 1.1 m < __elements_after
		// 1.2 m >= __elements_after
		// 2 m > __rest
		// list product :
		// 1. pos == size()
		// 2. pos != size()

		int kl = std::max(n - m, 0);
		// 1.1 m < __elements_after
		if(kl > 0) {
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			vec.reserve(m + vec.size());
			auto pos = wjr::math::random<size_t>.uniform(0, kl - 1);
			wjr::__test_vector_product<value_type> product2(m);
			vec.insert(vec.begin() + pos, product2.begin(), product2.end());
			EXPECT_EQ(vec.size(), n + m) << "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin(), vec.begin() + pos, product.begin(), product.begin() + pos))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin() + pos + m, vec.end(), product.begin() + pos, product.end()))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin() + pos, vec.begin() + pos + m, product2.begin(), product2.end()))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
		}
		// 1.2 m >= __elements_after
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			vec.reserve(m + vec.size());
			auto pos = wjr::math::random<size_t>.uniform(kl, n);
			wjr::__test_vector_product<value_type> product2(m);
			vec.insert(vec.begin() + pos, product2.begin(), product2.end());
			EXPECT_EQ(vec.size(), n + m) << "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin(), vec.begin() + pos, product.begin(), product.begin() + pos))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin() + pos + m, vec.end(), product.begin() + pos, product.end()))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin() + pos, vec.begin() + pos + m, product2.begin(), product2.end()))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
		}
		
		// 2 m > __rest
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			auto pos = wjr::math::random<size_t>.uniform(0, vec.size());
			wjr::__test_vector_product<value_type> product2(m);
			vec.insert(vec.begin() + pos, product2.begin(), product2.end());
			EXPECT_EQ(vec.size(), n + m) << "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin(), vec.begin() + pos, product.begin(), product.begin() + pos))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin() + pos + m, vec.end(), product.begin() + pos, product.end()))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin() + pos, vec.begin() + pos + m, product2.begin(), product2.end()))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
		}
		// 1. pos == size()
		{
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			wjr::__test_list_product<value_type> product2(m);
			vec.insert(vec.end(), product2.begin(), product2.end());
			EXPECT_EQ(vec.size(), n + m) << "when " << "n = " << n << ", m = " << m;
			EXPECT_TRUE(eq(vec.begin(), vec.begin() + n, product.begin(), product.end()))
				<< "when " << "n = " << n << ", m = " << m;
			EXPECT_TRUE(eq(vec.begin() + n, vec.end(), product2.begin(), product2.end()))
				<< "when " << "n = " << n << ", m = " << m;
		}
		
		// 2. pos != size()
		if(n > 0) {
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			auto pos = wjr::math::random<size_t>.uniform(0, n - 1);
			wjr::__test_list_product<value_type> product2(m);
			vec.insert(vec.begin() + pos, product2.begin(), product2.end());
			EXPECT_EQ(vec.size(), n + m) << "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin(), vec.begin() + pos, product.begin(), product.begin() + pos))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin() + pos + m, vec.end(), product.begin() + pos, product.end()))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin() + pos, vec.begin() + pos + m, product2.begin(), product2.end()))
				<< "when " << "n = " << n << ", m = " << m << ", pos = " << pos;
		}
		

	};
	constexpr auto test = [__test](int n) {
		auto _Count = n < 1 ? 1 : n;
		for (int _ = 0; _ < _Count; ++_) {
			__test(n, 1);
			__test(n, 2);
			__test(n, 4);
			__test(n, 8);
			__test(n, 16);
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}


// vector::insert(const_iterator pos, std::initializer_list<T> ilist)
TYPED_TEST(vectorTest, insert_5) {
	// TODO;
}

// vector::emplace(const_iterator pos, Args&&... args)
TYPED_TEST(vectorTest, emplace) {
	// TODO;
}

// vector::erase(const_iterator pos)
TYPED_TEST(vectorTest, erase_1) {
	__INIT_TEST;
	constexpr auto __test = [](int n) {
		// 1. pos == size() - 1
		// 2. pos != size() - 1

		if (n > 1) {
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			vec.erase(vec.end() - 1);
			EXPECT_EQ(vec.size(), n - 1) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec.begin(), vec.end(), product.begin(), product.end() - 1))
				<< "when " << "n = " << n;
		}
		if (n > 1) {
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			auto pos = wjr::math::random<size_t>.uniform(0, n - 2);
			vec.erase(vec.begin() + pos);
			EXPECT_EQ(vec.size(), n - 1) << "when " << "n = " << n << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin(), vec.begin() + pos, product.begin(), product.begin() + pos))
				<< "when " << "n = " << n << ", pos = " << pos;
			EXPECT_TRUE(eq(vec.begin() + pos, vec.end(), product.begin() + pos + 1, product.end()))
				<< "when " << "n = " << n << ", pos = " << pos;
		}
	};
	constexpr auto test = [__test](int n) {
		auto _Count = n < 1 ? 1 : n;
		for (int _ = 0; _ < _Count; ++_) {
			__test(n);
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector::erase(const_iterator first, const_iterator last)
TYPED_TEST(vectorTest, erase_2) {
	__INIT_TEST;
	constexpr auto __test = [](int n) {
		// 1. first == last (TODO)
		// 2. first != last
		// 2.1 last == end() - 1
		// 2.2 last != end() - 1
		{
			// TODO;
		}
		if(n >= 1) {
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			auto first = wjr::math::random<size_t>.uniform(0, n - 1);
			auto last = n - 1;
			vec.erase(vec.begin() + first, vec.begin() + last);
			EXPECT_EQ(vec.size(), n - (last - first)) << "when " << "n = " << n << ", first = " << first << ", last = " << last;
			EXPECT_TRUE(eq(vec.begin(), vec.begin() + first, product.begin(), product.begin() + first))
				<< "when " << "n = " << n << ", first = " << first << ", last = " << last;
			EXPECT_TRUE(eq(vec.begin() + first, vec.end(), product.begin() + last, product.end()))
				<< "when " << "n = " << n << ", first = " << first << ", last = " << last;
		}
		if(n >= 2) {
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			std::uniform_int_distribution<int> dist(0, n - 2);
			auto first = wjr::math::random<size_t>.distribution(dist);
			auto last = wjr::math::random<size_t>.distribution(dist);
			if (first > last) std::swap(first, last);
			vec.erase(vec.begin() + first, vec.begin() + last + 1);
			EXPECT_EQ(vec.size(), n - (last - first + 1)) << "when " << "n = " << n << ", first = " << first << ", last = " << last;
			EXPECT_TRUE(eq(vec.begin(), vec.begin() + first, product.begin(), product.begin() + first))
				<< "when " << "n = " << n << ", first = " << first << ", last = " << last;
			EXPECT_TRUE(eq(vec.begin() + first, vec.end(), product.begin() + last + 1, product.end()))
				<< "when " << "n = " << n << ", first = " << first << ", last = " << last;
		}
	};
	constexpr auto test = [__test](int n) {
		auto _Count = n < 1 ? 1 : n;
		for (int _ = 0; _ < _Count; ++_) {
			__test(n);
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector::push_back(const T& value)
TYPED_TEST(vectorTest, push_back_1) {
	__INIT_TEST;
	constexpr auto __test = [min_capacity](size_t n) {
		// 1. size() == capacity()
		// 2. size() < capacity()
		size_t kl = std::max(n, min_capacity);
		{
			wjr::__test_vector_product<value_type> product(kl);
			container_type vec(product.begin(), product.end());
			auto val = wjr::__test_random<value_type>{}();
			vec.push_back(val);
			EXPECT_EQ(vec.size(), kl + 1) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec.begin(), vec.end() - 1, product.begin(), product.end()))
				<< "when " << "n = " << n;
			EXPECT_EQ(vec.back(), val) << "when " << "n = " << n;
			
		}
		{
			wjr::__test_vector_product<value_type> product(kl);
			container_type vec(product.begin(), product.end());
			vec.reserve(vec.size() + 1);
			auto cap = vec.capacity();
			auto val = wjr::__test_random<value_type>{}();
			vec.push_back(val);
			EXPECT_EQ(vec.size(), kl + 1) << "when " << "n = " << n;
			EXPECT_EQ(vec.capacity(), cap) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec.begin(), vec.end()- 1, product.begin(), product.end()))
				<< "when " << "n = " << n;
			EXPECT_EQ(vec.back(), val) << "when " << "n = " << n;
		}
	};
	constexpr auto test = [__test](size_t n) {
		auto _Count = n < 1 ? 1 : n;
		for (int _ = 0; _ < _Count; ++_) {
			__test(n);
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector::push_back(T&& value)
TYPED_TEST(vectorTest, push_back_2){
	// TODO;
}


// vector::emplace_back
TYPED_TEST(vectorTest, emplace_back) {
	// TODO;
}

// vector::pop_back
TYPED_TEST(vectorTest, pop_back) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		if(n > 0) {
			wjr::__test_vector_product<value_type> product(n);
			container_type vec(product.begin(), product.end());
			vec.pop_back();
			EXPECT_EQ(vec.size(), n - 1) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec.begin(), vec.end(), product.begin(), product.end() - 1))
				<< "when " << "n = " << n;
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}


// vector::resize(size_type _Count)
TYPED_TEST(vectorTest, resize_1) {
	__INIT_TEST;
	constexpr auto test = [min_capacity](size_t n) {
		// __rest = capacity () - size()
		// 1. size() < n
		// 1.1 (n - size()) <= __rest -> n <= capacity()
		// 1.2 (n - size()) > __rest -> n > capacity()
		// 2. size() > n
		size_t kl = std::max(n, min_capacity);
		{
			wjr::__test_vector_product<value_type> product(kl);
			container_type vec(product.begin(), product.end());
			vec.reserve(kl + 1);
			auto val = wjr::__test_random<value_type>{}();
			vec.resize(kl + 1, val);
			EXPECT_EQ(vec.size(), kl + 1) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec.begin(), vec.end() - 1, product.begin(), product.end()))
				<< "when " << "n = " << n;
			EXPECT_EQ(vec.back(), val) << "when " << "n = " << n;
		}
		{
			wjr::__test_vector_product<value_type> product(kl);
			container_type vec(product.begin(), product.end());
			auto val = wjr::__test_random<value_type>{}();
			vec.resize(kl + 1, val);
			EXPECT_EQ(vec.size(), kl + 1) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec.begin(), vec.end() - 1, product.begin(), product.end()))
				<< "when " << "n = " << n;
			EXPECT_EQ(vec.back(), val) << "when " << "n = " << n;
		}
		if(kl >= 1) {
			wjr::__test_vector_product<value_type> product(kl);
			container_type vec(product.begin(), product.end());
			vec.resize(kl - 1);
			EXPECT_EQ(vec.size(), kl - 1) << "when " << "n = " << n;
			EXPECT_TRUE(eq(vec.begin(), vec.end(), product.begin(), product.end() - 1))
				<< "when " << "n = " << n;
		}
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

// vector::resize(size_type _Count, const _Ty& _Val)
TYPED_TEST(vectorTest, resize_2) {
	// TODO;
}


// vector::swap(vector& _Right)
TYPED_TEST(vectorTest, swap) {
	__INIT_TEST;
	constexpr auto test = [](size_t n) {
		wjr::__test_vector_product<value_type> product(n);
		container_type vec(product.begin(), product.end());
		container_type vec2;
		vec.swap(vec2);
		EXPECT_EQ(vec.size(), 0) << "when " << "n = " << n;
		EXPECT_EQ(vec2.size(), n) << "when " << "n = " << n;
		EXPECT_TRUE(eq(vec2.begin(), vec2.end(), product.begin(), product.end()))
			<< "when " << "n = " << n;
	};
	test(0);
	test(1);
	test(2);
	test(4);
	test(8);
	test(16);
}

#undef __INIT_TEST