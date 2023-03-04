#include <array>
#include <wjr/algorithm.h>
#include <wjr/random.h>

#include <gtest/gtest.h>

static_assert(wjr::__has_fast_fill_v<int*, int>, "wjr::__has_fast_fill_v<int*, int>");
static_assert(wjr::__has_fast_fill_v<int*, const int>, "wjr::__has_fast_fill_v<int*, const int>");
static_assert(!wjr::__has_fast_fill_v<int*, volatile int>, "!wjr::__has_fast_fill_v<int*, volatile int>");
static_assert(!wjr::__has_fast_fill_v<volatile int*, int>, "!wjr::__has_fast_fill_v<volatile int*, int>");
static_assert(wjr::__has_fast_fill_v<uint64_t*, uint8_t>, "wjr::__has_fast_fill_v<uint64_t*, uint8_t>");
static_assert(wjr::__has_fast_fill_v<uint64_t*, int8_t>, "wjr::__has_fast_fill_v<uint64_t*, int8_t>");
static_assert(wjr::__has_fast_fill_v<char*, int>, "wjr::__has_fast_fill_v<char*, int>");
static_assert(wjr::__has_fast_fill_v<std::array<int, 10>*, std::array<int, 10>>,
	"wjr::__has_fast_fill_v<std::array<int, 10>*, std::array<int,10>>");
static_assert(wjr::__has_fast_fill_v<std::reverse_iterator<int*>, int>,
	"wjr::__has_fast_fill_v<std::reverse_iterator<int*>, int>");
static_assert(wjr::__has_fast_fill_v<int*, int&&>, "!wjr::__has_fast_fill_v<int*, int&&>");

static_assert(!wjr::__has_fast_fill_v<wjr::disable_tag*, wjr::disable_tag>,
	"!wjr::__has_fast_copy<wjr::zero_tag*, wjr::zero_tag>::value");

static_assert(wjr::__has_fast_copy_v<int*, int*>, "wjr::__has_fast_copy_v<int*, int*>");
static_assert(wjr::__has_fast_copy_v<const int*, int*>, "wjr::__has_fast_copy_v<const int*, int*>");

static_assert(!wjr::__has_fast_copy_v<volatile int*, int*>,
	"!wjr::__has_fast_copy<volatile int*, int*>::value");

struct _Node {
	int x;
};

static_assert(wjr::__has_fast_fill_v<_Node*, _Node>, "wjr::__has_fast_fill_v<_Node*, _Node>");
static_assert(wjr::__has_fast_fill_v<std::reverse_iterator<_Node*>, _Node>,
	"wjr::__has_fast_fill_v<std::reverse_iterator<_Node*>, _Node>");
static_assert(wjr::__has_fast_fill_v<_Node*, _Node&&>, "!wjr::__has_fast_fill_v<_Node*, _Node&&>");

struct _Move_node {
	_Move_node() = default;
	_Move_node(const _Move_node&) = default;
	_Move_node(_Move_node&&) = default;
	_Move_node& operator=(const _Move_node&) = default;
	_Move_node& operator=(_Move_node&& other) {
		x = other.x;
		other.x = 0;
		return *this;
	}
	~_Move_node() = default;
	int x;
};

namespace wjr {
	template<>
	struct is_byte_assignable<_Move_node, const _Move_node&> {
		constexpr static bool is_copy = true;
		constexpr static bool is_fill = true;
	};
}

static_assert(wjr::__has_fast_copy_v<const _Move_node*, _Move_node*>,
	"wjr::__has_fast_copy_v<_Move_node*, _Move_node*>");

std::pair<int, int> get_pos(int n, bool __n = false) {
	if (__n) {
		return { n, n };
	}
	if (!n) {
		return { 0,0 };
	}
	int x = n, y = n;
	x = wjr::Random::get<int>(0, n);
	if (x == n) {
		return { n, n };
	}
	y = wjr::Random::get<int>(0, n);
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
	auto test1 = [](auto _Type, int n) {
		using type = std::decay_t<decltype(_Type)>;
		std::vector<type> vec(n);
		std::generate(__begin(vec), __end(vec), wjr::Random::engine());
		type val;
		auto p = wjr::Random::get<int>(0, n);
		if (p < n) {
			val = vec[p];
		}
		else {
			val = wjr::Random::get<type>();
		}
		{
			auto a = std::find(__begin(vec), __end(vec), val);
			auto b = wjr::find(__begin(vec), __end(vec), val);
			EXPECT_TRUE(a == b) << "type : " << typeid(type).name()
				<< "\nerror at normal find. \n" << "n = " << n << '\n';
			if (a != b) {
				std::cout << "std pos = " << std::distance(__begin(vec), a) << '\n';
				std::cout << "wjr pos = " << std::distance(__begin(vec), b) << '\n';
				wjr::find(__begin(vec), __end(vec), val);
			}
		}
		{
			auto a = std::find(__rbegin(vec), __rend(vec), val);
			auto b = wjr::find(__rbegin(vec), __rend(vec), val);
			EXPECT_TRUE(a == b) << "type : " << typeid(type).name()
				<< "\nerror at reverse find. \n" << "n = " << n << '\n';
			if (a != b) {
				std::cout << "std pos = " << std::distance(__rbegin(vec), a) << '\n';
				std::cout << "wjr pos = " << std::distance(__rbegin(vec), b) << '\n';
				wjr::find(__begin(vec), __end(vec), val);
			}
		}

		{
			auto a = std::find_if(__begin(vec), __end(vec), [val](auto x) {return x != val; });
			auto b = wjr::find(__begin(vec), __end(vec), val, std::not_equal_to<>{});
			EXPECT_TRUE(a == b) << "type : " << typeid(type).name()
				<< "\nerror at normal find. \n" << "n = " << n << '\n';
			if (a != b) {
				std::cout << "std pos = " << std::distance(__begin(vec), a) << '\n';
				std::cout << "wjr pos = " << std::distance(__begin(vec), b) << '\n';
				wjr::find(__begin(vec), __end(vec), val);
			}
		}
		{
			auto a = std::find_if(__rbegin(vec), __rend(vec), [val](auto x) {return x != val; });
			auto b = wjr::find(__rbegin(vec), __rend(vec), val, std::not_equal_to<>{});
			EXPECT_TRUE(a == b) << "type : " << typeid(type).name()
				<< "\nerror at reverse find. \n" << "n = " << n << '\n';
			if (a != b) {
				std::cout << "std pos = " << std::distance(__rbegin(vec), a) << '\n';
				std::cout << "wjr pos = " << std::distance(__rbegin(vec), b) << '\n';
				wjr::find(__begin(vec), __end(vec), val);
			}
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
		test2(1);
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
#define __TEST(type1, value1, type2, value2, _pos)	                            \
	{	                                                                        \
		const int n = 64;	                                                    \
		std::vector<type1> vec(n, value1);	                                    \
		vec[1] = value2;														\
		type2 val = value2;	                                                    \
		auto pos = wjr::find(__begin(vec), __end(vec), val) - __begin(vec);	    \
		EXPECT_EQ(pos, _pos);	                                                \
	}	                                                                        \
	{		                                                                    \
		const int n = 64;	                                                    \
		std::vector<type1> vec(n, value1);	                                    \
		vec[n-2] = value2;														\
		type2 val = value2;	                                                    \
		auto pos = wjr::find(__rbegin(vec), __rend(vec), val) - __rbegin(vec);	\
		EXPECT_EQ(pos, _pos);	                                                \
	}

	// (int8_t)(-1) != (uint8_t)(-1)
	__TEST(int8_t, -1, uint8_t, -1, n);
	__TEST(uint8_t, -1, int8_t, -1, n);
	__TEST(uint8_t, 1, int8_t, 1, 0);
	__TEST(uint8_t, -1, int8_t, 1, 1);
	// (int32_t)(-1) == (uint32_t)(-1)
	__TEST(int32_t, -1, uint32_t, -1, 0);
	// (int8_t)(-1) == (uint32_t)(-1)
	__TEST(int8_t, -1, uint32_t, -1, 0);
	__TEST(int8_t, -1, int32_t, -1, 0);
#undef __TEST
}

TEST(algorithm, mismatch) {
	auto test1eq = [](auto _Type, int n) {
		using type = std::decay_t<decltype(_Type)>;
		std::vector<type> vec1(n);
		std::vector<type> vec2(n);
		std::generate(__begin(vec1), __end(vec1), wjr::Random::engine());
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
		std::generate(__begin(vec1), __end(vec1), wjr::Random::engine());
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
		std::generate(__begin(vec1), __end(vec1), wjr::Random::engine());
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
		test3(1);
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
		std::generate(__begin(vec1), __end(vec1), wjr::Random::engine());
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
			if (ret1 != ret2) {
				std::cout << "pos1 = " << pos1 << ", pos2 = " << pos2 << '\n';
				for (int i = 0; i < n; ++i) {
					std::cout << vec1[i] << ' ';
				}
				std::cout << '\n';
				for (int i = 0; i < n; ++i) {
					std::cout << vec2[i] << ' ';
				}
				std::cout << '\n';
				wjr::equal(__begin(vec1), __end(vec1), __begin(vec2));
			}
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
		std::generate(__begin(vec1), __end(vec1), wjr::Random::engine());
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
		std::generate(__begin(vec1), __end(vec1), wjr::Random::engine());
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
		test3(1);
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
		auto val = wjr::Random::get<type>();
		std::vector<type> vec(n, val);
		for (int i = 0; i < (n / 2); ++i) {
			++vec[i];
		}
		std::shuffle(__begin(vec), __end(vec), wjr::Random::engine());
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
		test2(1);
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
		test2(16384);
	}

	{
		const int n = 64;
		std::vector<int8_t> vec1(n, -1);
		auto cnt = wjr::count(__begin(vec1), __end(vec1), (uint8_t)255);
		EXPECT_EQ(cnt, 0);
	}

}

TEST(algorithm, fill) {
	auto test1 = [](auto _Type, int n) {
		using type = std::decay_t<decltype(_Type)>;
		auto val = wjr::Random::get<type>();
		std::vector<type> vec(n);
		{
			auto vec2 = vec;
			wjr::fill(__begin(vec2), __end(vec2), val);
			for (auto& e : vec2) {
				EXPECT_EQ(e, val);
			}
		}
		{
			auto vec2 = vec;
			wjr::fill(__rbegin(vec2), __rend(vec2), val);
			for (auto& e : vec2) {
				EXPECT_EQ(e, val);
			}
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

	for (int i = 0; i < 512; ++i) {
		test2(i);
	}
	test2(1000);
	test2(1024 * 1);
	test2(1024 * 2);
	test2(1024 * 4);
	test2(1024 * 8);
	test2(1024 * 32);
	test2(1024 * 128);
	test2(1024 * 1024);
	test2(1024 * 1024 * 4);

	{
		const int n = 64;
		std::vector<uint64_t> vec1(n);
		wjr::fill(__begin(vec1), __end(vec1), (int8_t)-1);
		for (auto& e : vec1) {
			EXPECT_EQ(e, (uint64_t)0xffffffffffffffff);
		}
	}
	{
		const int n = 64;
		std::vector<uint64_t> vec1(n);
		wjr::fill(__begin(vec1), __end(vec1), (uint8_t)1);
		for (auto& e : vec1) {
			EXPECT_EQ(e, 1);
		}
	}
}
