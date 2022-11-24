#include <vector>
#include <chrono>

#include <random>

#include <gtest/gtest.h>
#include <wjr/vector.h>
#include <wjr/allocator.h>
#include <wjr/test_product.h>
#include <wjr/algorithm.h>

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

TEST(algorithm, find) {
	auto test1 = [](auto _Type, int n){
		using type = std::decay_t<decltype(_Type)>;
		std::vector<type> vec(n);
		std::generate(std::begin(vec), std::end(vec), wjr::math::random<type>.get());
		type val;
		auto p = wjr::math::random<int>.uniform(0, n);
		if (p < n) {
			val = vec[p];
		}
		else {
			val = wjr::math::random<type>();
		}
		{
			auto a = std::find(std::begin(vec), std::end(vec), val);
			auto b = wjr::find(std::begin(vec), std::end(vec), val);
			EXPECT_TRUE(a == b) << "type : " << typeid(type).name() 
				<< "\nerror at normal find. \n" << "n = " << n << '\n';
			if (a != b) {
				testing::AssertionResult result = testing::AssertionFailure();

				for (auto i : vec) {
					result << (int)i << ' ';
				}
				result << '\n';
				result << "val = " << (int)val << '\n';
				result << (a - std::begin(vec)) << ' ' << (b - std::begin(vec)) << '\n';
				EXPECT_TRUE(result);
			}
		}
		{
			auto a = std::find(std::rbegin(vec), std::rend(vec), val);
			auto b = wjr::find(std::rbegin(vec), std::rend(vec), val);
			EXPECT_TRUE(a == b) << "type : " << typeid(type).name()
				<< "\nerror at reverse find. \n" << "n = " << n << '\n';
			if (a != b) {
				for (auto i : vec) {
					std::cout << i << ' ';
				}
				std::cout << '\n';
				std::cout << "val = " << val << '\n';
				std::cout << (a - std::rbegin(vec)) << ' ' << (b - std::rbegin(vec)) << '\n';
			}
		}
		{
			auto a = std::find(std::make_reverse_iterator(std::rend(vec)), 
				std::make_reverse_iterator(std::rbegin(vec)), val);
			auto b = wjr::find(std::make_reverse_iterator(std::rend(vec)),
				std::make_reverse_iterator(std::rbegin(vec)), val);
			EXPECT_TRUE(a == b) << "type : " << typeid(type).name()
				<< "\nerror at reverse<reverse> find. \n" << "n = " << n << '\n';
		}
	};
	auto test2 = [&test1](int n) {
		test1((char)0, n);
		test1((short)0, n);
		test1((int)0, n);
		test1((long long)0, n);
	};

	test2(0);
	for (int i = 0; i < 16; ++i) {
		test2(3);
		test2(4);
	}
	for (int i = 0; i < 32; ++i) {
		test2(15);
		test2(16);
	}
	for (int i = 0; i < 64; ++i) {
		test2(31);
		test2(32);
	}
	for (int i = 0; i < 128; ++i) {
		test2(63);
		test2(64);
	}
	for (int i = 0; i < 256; ++i) {
		test2(256);
		test2(1000);
		test2(1024);
	}
	for (int i = 0; i < 1024; ++i) {
		test2(wjr::math::random<int>.uniform(1, 256));
	}
}