#pragma once
#ifndef __WJR_RANDOM_H
#define __WJR_RANDOM_H

#include <random>

#include <wjr/type_traits.h>

_WJR_BEGIN

template<typename Engine>
struct basic_random_static_wrapper {
	static Engine m_engine;
	static Engine& engine() {
		return m_engine;
	}
};

template<typename Engine>
Engine basic_random_static_wrapper<Engine>::m_engine(std::random_device{}());

template<typename Engine>
struct basic_random_thread_local_wrapper {
	static Engine& engine() {
		thread_local Engine m_engine(std::random_device{}());
		return m_engine;
	}
};

template<
	typename Engine,
	template<typename> typename EngineWrapper,
	template<typename> typename IntegerDist = std::uniform_int_distribution,
	template<typename> typename RealDist = std::uniform_real_distribution,
	typename BoolDist = std::bernoulli_distribution
>
class basic_random : public EngineWrapper<Engine>{

	template<typename T>
	using _Is_container = std::conjunction<
		has_global_function_std_begin<T>,
		has_global_function_std_end<T>
	>;

	template<typename T>
	using _Has_size = has_global_function_std_size<T>;

public:
	using _Mybase = EngineWrapper<Engine>;
	
	using engine_type = Engine;

	template<typename T>
	using integer_dist_t = IntegerDist<T>;

	template<typename T>
	using real_dist_t = RealDist<T>;

	using bool_dist_t = BoolDist;

	using common = std::common_type<>;

	using _Mybase::engine;

	basic_random() = delete;

	constexpr static typename engine_type::result_type min() {
		return engine_type::min();
	}
	
	constexpr static typename engine_type::result_type max() {
		return engine_type::max();
	}

	static void discard(size_t n) {
		engine().discard(n);
	}

	static void reseed() {
		engine().seed(std::random_device{}());
	}

	static void seed(typename engine_type::result_type seed = engine_type::default_seed) {
		engine().seed(seed);
	}

	template<typename Seq>
	static void seed(Seq& seq) {
		engine().seed(seq);
	}

	static typename engine_type::result_type get() {
		return engine()();
	}

	template<typename T, std::enable_if_t<std::conjunction_v<
		std::is_arithmetic<T>, std::negation<std::is_same<T, bool>>>, int> = 0>
	static T get(
			T __min = std::numeric_limits<T>::min(),
			T __max = std::numeric_limits<T>::max()) {
		if constexpr (std::is_integral_v<T>) {
			if constexpr (sizeof(T) < sizeof(short)) {
				using short_t = std::conditional_t<std::is_signed<T>::value,
					short, unsigned short>;
				return static_cast<T>(integer_dist_t<short_t>{__min, __max}(engine()));
			}
			else {
				using type = int_or_uint_t<sizeof(T) * 8, std::is_signed_v<T>>;
				return integer_dist_t<type>{__min, __max}(engine());
			}
		}
		else {
			return real_dist_t<T>{__min, __max}(engine());
		}
	}

	template<typename X, typename T, typename U, typename R = std::common_type_t<T, U>,
		std::enable_if_t<std::conjunction_v<
		std::is_same<X, common>,
		std::is_arithmetic<T>,
		std::is_arithmetic<U>>, int> = 0>
	static R get(
			T __min = std::numeric_limits<T>::min(),
			U __max = std::numeric_limits<U>::max()) {
		return get<R>(static_cast<R>(__min), static_cast<R>(__max));
	}

	template<typename T, std::enable_if_t<std::is_same_v<T, bool>, int> = 0>
	static T get(double p = 0.5) {
		return bool_dist_t{ p }(engine());
	}

	template<typename Dist, typename...Args>
	static typename Dist::result_type get(Args&&...args) {
		return Dist{ std::forward<Args>(args)... }(engine());
	}

	template<typename Dist>
	static typename Dist::result_type get(Dist& dist) {
		return dist(engine());
	}

	template<typename _Container, std::enable_if_t<std::conjunction_v<
		has_global_function_std_begin<_Container>,
		has_global_function_std_end<_Container>>, int> = 0>
	static decltype(auto) get(_Container& container) {
		auto first = std::begin(container);
		auto last = std::end(container);
		auto n = wjr::size(container);
		if (n == 0)return last;
		using diff_t = wjr::iter_diff_t<decltype(first)>;
		return std::next(first, get<diff_t>(0, n - 1));
	}

	template<typename iter, std::enable_if_t<wjr::is_iterator_v<iter>, int> = 0>
	static iter get(iter first, iter last) {
		return get(wjr::make_iter_wrapper(first, last));
	}

	template<typename T>
	static T get(std::initializer_list<T> il) {
		return *get(std::begin(il), std::end(il));
	}

	template<typename _Ty, size_t _Size>
	static _Ty get(_Ty(&arr)[_Size]) {
		return *get(std::begin(arr), std::end(arr));
	}

	template<typename _Container>
	static void shuffle(_Container& container) {
		std::shuffle(std::begin(container), std::end(container), engine());
	}

	template<typename iter>
	static void shuffle(iter first, iter last) {
		shuffle(wjr::make_iter_wrapper(first, last));
	}

};

template<typename Engine, 
	template<typename>typename IntegerDist = std::uniform_int_distribution,
	template<typename>typename RealDist = std::uniform_real_distribution,
	typename BoolDist = std::bernoulli_distribution>
using basic_random_static = basic_random<Engine, basic_random_static_wrapper, IntegerDist, RealDist, BoolDist>;

template<typename Engine,
	template<typename>typename IntegerDist = std::uniform_int_distribution,
	template<typename>typename RealDist = std::uniform_real_distribution,
	typename BoolDist = std::bernoulli_distribution>
using basic_random_thread_local = basic_random<Engine, basic_random_thread_local_wrapper, IntegerDist, RealDist, BoolDist>;

using random_static = basic_random_static<std::mt19937>;
using random_thread_local = basic_random_thread_local<std::mt19937>;

using Random = random_static;

_WJR_END

#endif // __WJR_RANDOM_H