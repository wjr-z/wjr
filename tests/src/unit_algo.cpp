#include <vector>
#include <chrono>

#include <random>

#include <gtest/gtest.h>
#include <wjr/vector.h>
#include <wjr/allocator.h>
#include <wjr/test_product.h>
#include <wjr/algorithm.h>
#include <wjr/simd/simd_helper.h>

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

std::pair<int, int> get_pos(int n, bool __n = false) {
	if (__n) {
		return { n, n };
	}
	if (!n) {
		return { 0,0 };
	}
	int x = n, y = n;
	x = wjr::math::random<int>.uniform(0, n);
	if (x == n) {
		return { n, n };
	}
	y = wjr::math::random<int>.uniform(0, n);
	if (x > y) {
		std::swap(x, y);
	}
	return { x,y };
}

template<typename T>
const T* __begin(const std::vector<T>& vec) { return vec.data(); }
template<typename T>
T* __begin(std::vector<T>& vec) { return vec.data(); }
template<typename T>
const T* __end(const std::vector<T>& vec) { return vec.data() + vec.size(); }
template<typename T>
T* __end(std::vector<T>& vec) { return vec.data() + vec.size(); }
template<typename T>
std::reverse_iterator<const T*> __rbegin(const std::vector<T>& vec) { 
	return std::make_reverse_iterator(vec.data() + vec.size());
}
template<typename T>
std::reverse_iterator<T*> __rbegin(std::vector<T>& vec) {
	return std::make_reverse_iterator(vec.data() + vec.size());
}
template<typename T>
std::reverse_iterator<const T*> __rend(const std::vector<T>& vec) {
	return std::make_reverse_iterator(vec.data());
}
template<typename T>
std::reverse_iterator<T*> __rend(std::vector<T>& vec) {
	return std::make_reverse_iterator(vec.data());
}

TEST(algorithm, find) {
	auto test1 = [](auto _Type, int n){
		using type = std::decay_t<decltype(_Type)>;
		std::vector<type> vec(n);
		std::generate(__begin(vec), __end(vec), wjr::math::random<type>.engine());
		type val;
		auto p = wjr::math::random<int>.uniform(0, n);
		if (p < n) {
			val = vec[p];
		}
		else {
			val = wjr::math::random<type>();
		}
		{
			auto a = std::find(__begin(vec), __end(vec), val);
			auto b = wjr::find(__begin(vec), __end(vec), val);
			EXPECT_TRUE(a == b) << "type : " << typeid(type).name() 
				<< "\nerror at normal find. \n" << "n = " << n << '\n';
		}
		{
			auto a = std::find(__rbegin(vec), __rend(vec), val);
			auto b = wjr::find(__rbegin(vec), __rend(vec), val);
			EXPECT_TRUE(a == b) << "type : " << typeid(type).name()
				<< "\nerror at reverse find. \n" << "n = " << n << '\n';
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
		test2(128);
		test2(256);
		test2(1023);
		test2(1024);
	}
	{
		const int n = 64;
		std::vector<int8_t> vec(n);
		vec[0] = -1;
		uint8_t val = -1;
		auto pos = wjr::find(__begin(vec), __end(vec), val) - __begin(vec);
		EXPECT_EQ(pos, n);
	}
	{
		const int n = 64;
		std::vector<int32_t> vec(n);
		vec[0] = -1;
		uint32_t val = -1;
		auto pos = wjr::find(__begin(vec), __end(vec), val) - __begin(vec);
		EXPECT_EQ(pos, 0);
	}
}

TEST(algorithm, mismatch) {
	auto test1eq = [](auto _Type, int n) {
		using type = std::decay_t<decltype(_Type)>;
		std::vector<type> vec1(n);
		std::vector<type> vec2(n);
		std::generate(__begin(vec1), __end(vec1), wjr::math::random<type>.engine());
		vec2 = vec1;
		auto [pos1, pos2] = get_pos(n);
		if (pos1 < n) {
			++vec2[pos1];
		}
		if (pos2 < n) {
			++vec2[pos2];
		}
		{
			auto ret1 = std::mismatch(__begin(vec1), __end(vec1), __begin(vec2));
			auto ret2 = wjr::mismatch(__begin(vec1), __end(vec1), __begin(vec2));
			EXPECT_EQ(ret1, ret2) << "type : " << typeid(type).name()
				<< "\nerror at normal mismatch<eq>. \n" << "n = " << n << '\n';
		}
		{
			auto ret1 = std::mismatch(__rbegin(vec1), __rend(vec1), __rbegin(vec2));
			auto ret2 = wjr::mismatch(__rbegin(vec1), __rend(vec1), __rbegin(vec2));
			EXPECT_EQ(ret1, ret2) << "type : " << typeid(type).name()
				<< "\nerror at reverse mismatch<eq>. \n" << "n = " << n << '\n';
		}
	};
	auto test2eq = [&test1eq](int n) {
		test1eq((int8_t)0, n);
		test1eq((int16_t)0, n);
		test1eq((int32_t)0, n);
		test1eq((int64_t)0, n);
	};

	auto test1lt = [](auto _Type, int n) {
		using type = std::decay_t<decltype(_Type)>;
		std::vector<type> vec1(n);
		std::vector<type> vec2(n);
		std::generate(__begin(vec1), __end(vec1), wjr::math::random<type>.engine());
		vec2 = vec1;
		auto __change = [&](int p) {
			if (vec1[p])--vec1[p];
			else ++vec2[p];
		};
		for (int i = 0; i < n; ++i) {
			__change(i);
		}
		auto [pos1, pos2] = get_pos(n);
		if (pos1 < n) {
			vec2[pos1] = vec1[pos1];
		}
		if (pos2 < n) {
			vec2[pos2] = vec1[pos2];
		}
		{
			auto ret1 = std::mismatch(__begin(vec1), __end(vec1), __begin(vec2), std::less<>{});
			auto ret2 = wjr::mismatch(__begin(vec1), __end(vec1), __begin(vec2), std::less<>{});
			EXPECT_EQ(ret1, ret2) << "type : " << typeid(type).name()
				<< "\nerror at normal mismatch<lt>. \n" << "n = " << n << '\n';
		}
		{
			auto ret1 = std::mismatch(__rbegin(vec1), __rend(vec1), __rbegin(vec2), std::less<>{});
			auto ret2 = wjr::mismatch(__rbegin(vec1), __rend(vec1), __rbegin(vec2), std::less<>{});
			EXPECT_EQ(ret1, ret2) << "type : " << typeid(type).name()
				<< "\nerror at reverse mismatch<lt>. \n" << "n = " << n << '\n';
		}
	};
	auto test2lt = [&test1lt](int n) {
		test1lt((int8_t)0, n);
		test1lt((int16_t)0, n);
		test1lt((int32_t)0, n);
		test1lt((int64_t)0, n);

		test1lt((uint8_t)0, n);
		test1lt((uint16_t)0, n);
		test1lt((uint32_t)0, n);
		test1lt((uint64_t)0, n);
	};

	auto test1le = [](auto _Type, int n) {
		using type = std::decay_t<decltype(_Type)>;
		std::vector<type> vec1(n);
		std::vector<type> vec2(n);
		std::generate(__begin(vec1), __end(vec1), wjr::math::random<type>.engine());
		vec2 = vec1;
		auto __change = [&](int p) {
			if (!vec1[p])++vec1[p];
			else --vec2[p];
		};
		auto [pos1, pos2] = get_pos(n);
		if (pos1 < n) {
			__change(pos1);
		}
		if (pos2 < n) {
			__change(pos2);
		}
		{
			auto ret1 = std::mismatch(__begin(vec1), __end(vec1), __begin(vec2), std::less_equal<>{});
			auto ret2 = wjr::mismatch(__begin(vec1), __end(vec1), __begin(vec2), std::less_equal<>{});
			EXPECT_EQ(ret1, ret2) << "type : " << typeid(type).name()
				<< "\nerror at normal mismatch<le>. \n" << "n = " << n << '\n';
		}
		{
			auto ret1 = std::mismatch(__rbegin(vec1), __rend(vec1), __rbegin(vec2), std::less_equal<>{});
			auto ret2 = wjr::mismatch(__rbegin(vec1), __rend(vec1), __rbegin(vec2), std::less_equal<>{});
			EXPECT_EQ(ret1, ret2) << "type : " << typeid(type).name()
				<< "\nerror at reverse mismatch<le>. \n" << "n = " << n << '\n';
		}
	};
	auto test2le = [&test1le](int n) {
		test1le((int8_t)0, n);
		test1le((int16_t)0, n);
		test1le((int32_t)0, n);
		test1le((int64_t)0, n);

		test1le((uint8_t)0, n);
		test1le((uint16_t)0, n);
		test1le((uint32_t)0, n);
		test1le((uint64_t)0, n);
	};

	auto test3 = [&test2eq, &test2lt, &test2le](auto n) {
		test2eq(n);
		test2lt(n);
		test2le(n);
	};

	test3(0);
	for (int i = 0; i < 16; ++i) {
		test3(3);
		test3(4);
	}
	for (int i = 0; i < 32; ++i) {
		test3(15);
		test3(16);
	}
	for (int i = 0; i < 64; ++i) {
		test3(31);
		test3(32);
	}
	for (int i = 0; i < 128; ++i) {
		test3(63);
		test3(64);
	}
	for (int i = 0; i < 1024; ++i) {
		test3(128);
		test3(256);
		test3(1023);
		test3(1024);
	}
	{
		const int n = 64;
		std::vector<int8_t> vec1(n);
		std::vector<uint8_t> vec2(n);
		vec1[0] = -1;
		vec2[0] = -1;
		auto pos = wjr::mismatch(__begin(vec1), __end(vec1), __begin(vec2)).first - __begin(vec1);
		EXPECT_EQ(pos, 0);
	}
	{
		const int n = 64;
		std::vector<int32_t> vec1(n);
		std::vector<uint32_t> vec2(n);
		vec1[0] = -1;
		vec2[0] = -1;
		auto pos = wjr::mismatch(__begin(vec1), __end(vec1), __begin(vec2)).first - __begin(vec1);
		EXPECT_EQ(pos, n);
	}
}

TEST(algorithm, equal) {
	auto test1eq = [](auto _Type, int n) {
		using type = std::decay_t<decltype(_Type)>;
		std::vector<type> vec1(n);
		std::vector<type> vec2(n);
		std::generate(__begin(vec1), __end(vec1), wjr::math::random<type>.engine());
		vec2 = vec1;
		auto [pos1, pos2] = get_pos(n);
		if (pos1 < n) {
			++vec2[pos1];
		}
		if (pos2 < n) {
			++vec2[pos2];
		}
		{
			auto ret1 = std::equal(__begin(vec1), __end(vec1), __begin(vec2));
			auto ret2 = wjr::equal(__begin(vec1), __end(vec1), __begin(vec2));
			EXPECT_EQ(ret1, ret2) << "type : " << typeid(type).name()
				<< "\nerror at normal equal<eq>. \n" << "n = " << n << '\n';
		}
		{
			auto ret1 = std::equal(__rbegin(vec1), __rend(vec1), __rbegin(vec2));
			auto ret2 = wjr::equal(__rbegin(vec1), __rend(vec1), __rbegin(vec2));
			EXPECT_EQ(ret1, ret2) << "type : " << typeid(type).name()
				<< "\nerror at reverse equal<eq>. \n" << "n = " << n << '\n';
		}
	};
	auto test2eq = [&test1eq](int n) {
		test1eq((int8_t)0, n);
		test1eq((int16_t)0, n);
		test1eq((int32_t)0, n);
		test1eq((int64_t)0, n);
	};

	auto test1lt = [](auto _Type, int n) {
		using type = std::decay_t<decltype(_Type)>;
		std::vector<type> vec1(n);
		std::vector<type> vec2(n);
		std::generate(__begin(vec1), __end(vec1), wjr::math::random<type>.engine());
		vec2 = vec1;
		auto __change = [&](int p) {
			if (vec1[p])--vec1[p];
			else ++vec2[p];
		};
		for (int i = 0; i < n; ++i) {
			__change(i);
		}
		auto [pos1, pos2] = get_pos(n);
		if (pos1 < n) {
			vec2[pos1] = vec1[pos1];
		}
		if (pos2 < n) {
			vec2[pos2] = vec1[pos2];
		}
		{
			auto ret1 = std::equal(__begin(vec1), __end(vec1), __begin(vec2), std::less<>{});
			auto ret2 = wjr::equal(__begin(vec1), __end(vec1), __begin(vec2), std::less<>{});
			EXPECT_EQ(ret1, ret2) << "type : " << typeid(type).name()
				<< "\nerror at normal equal<lt>. \n" << "n = " << n << '\n';
		}
		{
			auto ret1 = std::equal(__rbegin(vec1), __rend(vec1), __rbegin(vec2), std::less<>{});
			auto ret2 = wjr::equal(__rbegin(vec1), __rend(vec1), __rbegin(vec2), std::less<>{});
			EXPECT_EQ(ret1, ret2) << "type : " << typeid(type).name()
				<< "\nerror at reverse equal<lt>. \n" << "n = " << n << '\n';
		}
	};
	auto test2lt = [&test1lt](int n) {
		test1lt((int8_t)0, n);
		test1lt((int16_t)0, n);
		test1lt((int32_t)0, n);
		test1lt((int64_t)0, n);

		test1lt((uint8_t)0, n);
		test1lt((uint16_t)0, n);
		test1lt((uint32_t)0, n);
		test1lt((uint64_t)0, n);
	};

	auto test1le = [](auto _Type, int n) {
		using type = std::decay_t<decltype(_Type)>;
		std::vector<type> vec1(n);
		std::vector<type> vec2(n);
		std::generate(__begin(vec1), __end(vec1), wjr::math::random<type>.engine());
		vec2 = vec1;
		auto __change = [&](int p) {
			if (!vec1[p])++vec1[p];
			else --vec2[p];
		};
		auto [pos1, pos2] = get_pos(n);
		if (pos1 < n) {
			__change(pos1);
		}
		if (pos2 < n) {
			__change(pos2);
		}
		{
			auto ret1 = std::equal(__begin(vec1), __end(vec1), __begin(vec2), std::less_equal<>{});
			auto ret2 = wjr::equal(__begin(vec1), __end(vec1), __begin(vec2), std::less_equal<>{});
			EXPECT_EQ(ret1, ret2) << "type : " << typeid(type).name()
				<< "\nerror at normal equal<le>. \n" << "n = " << n << '\n';
		}
		{
			auto ret1 = std::equal(__rbegin(vec1), __rend(vec1), __rbegin(vec2), std::less_equal<>{});
			auto ret2 = wjr::equal(__rbegin(vec1), __rend(vec1), __rbegin(vec2), std::less_equal<>{});
			EXPECT_EQ(ret1, ret2) << "type : " << typeid(type).name()
				<< "\nerror at reverse equal<le>. \n" << "n = " << n << '\n';
		}
	};
	auto test2le = [&test1le](int n) {
		test1le((int8_t)0, n);
		test1le((int16_t)0, n);
		test1le((int32_t)0, n);
		test1le((int64_t)0, n);

		test1le((uint8_t)0, n);
		test1le((uint16_t)0, n);
		test1le((uint32_t)0, n);
		test1le((uint64_t)0, n);
	};

	auto test3 = [&test2eq, &test2lt, &test2le](auto n) {
		test2eq(n);
		test2lt(n);
		test2le(n);
	};

	test3(0);
	for (int i = 0; i < 16; ++i) {
		test3(3);
		test3(4);
	}
	for (int i = 0; i < 32; ++i) {
		test3(15);
		test3(16);
	}
	for (int i = 0; i < 64; ++i) {
		test3(31);
		test3(32);
	}
	for (int i = 0; i < 128; ++i) {
		test3(63);
		test3(64);
	}
	for (int i = 0; i < 1024; ++i) {
		test3(128);
		test3(256);
		test3(1023);
		test3(1024);
	}
}

TEST(algorithm, count) {
	auto test1 = [](auto _Type, int n) {
		using type = std::decay_t<decltype(_Type)>;
		auto val = wjr::math::random<type>();
		std::vector<type> vec(n, val);
		for (int i = 0; i < n; ++i) {
			if (wjr::math::random<bool>()) {
				++vec[i];
			}
		}
		{
			auto ret1 = std::count(__begin(vec), __end(vec), val);
			auto ret2 = wjr::count(__begin(vec), __end(vec), val);
			EXPECT_EQ(ret1, ret2) << "type : " << typeid(type).name()
				<< "\nerror at normal count. \n" << "n = " << n << '\n';
		}
		{
			auto ret1 = std::count(__rbegin(vec), __rend(vec), val);
			auto ret2 = wjr::count(__rbegin(vec), __rend(vec), val);
			EXPECT_EQ(ret1, ret2) << "type : " << typeid(type).name()
				<< "\nerror at reverse count. \n" << "n = " << n << '\n';
		}
	};
	auto test2 = [&test1](int n) {
		test1((int8_t)0, n);
		test1((int16_t)0, n);
		test1((int32_t)0, n);
		test1((int64_t)0, n);

		test1((uint8_t)0, n);
		test1((uint16_t)0, n);
		test1((uint32_t)0, n);
		test1((uint64_t)0, n);
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
	for (int i = 0; i < 1024; ++i) {
		test2(128);
		test2(256);
		test2(1023);
		test2(16384);
	}
}