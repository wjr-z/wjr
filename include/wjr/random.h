#ifndef __WJR_RANDOM_H
#define __WJR_RANDOM_H

#include <random>

#include <wjr/type_traits.h>

_WJR_BEGIN

template<typename Engine>
struct basic_random_static_wrapper {
	static Engine m_engine;
	Engine& engine() const {
		return m_engine;
	}
};

template<typename Engine>
Engine basic_random_static_wrapper<Engine>::m_engine(std::random_device{}());

template<typename Engine>
struct basic_random_thread_local_wrapper {
	Engine& engine() const {
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
class __basic_random : public EngineWrapper<Engine> {

	template<typename T>
	using _Is_container = std::conjunction<
		has_global_function_std_begin<T>,
		has_global_function_std_end<T>
	>;

public:
	using _Mybase = EngineWrapper<Engine>;

	using engine_type = Engine;

	template<typename T>
	using integer_dist_t = IntegerDist<T>;

	template<typename T>
	using real_dist_t = RealDist<T>;

	using bool_dist_t = BoolDist;

	using common = std::common_type<>;

	using result_type = typename engine_type::result_type;

	using _Mybase::engine;

private:

	template<typename _Seq>
	struct _Is_seed_seq : std::bool_constant<
		!std::is_convertible_v<_Seq, result_type>
		&& !std::is_same_v<remove_cvref_t<_Seq>, engine_type>
		&& !std::is_same_v<remove_cvref_t<_Seq>, __basic_random>
	> {};

	template<typename _Seq>
	constexpr static bool _Is_seed_seq_v = _Is_seed_seq<_Seq>::value;

public:

	constexpr static result_type min() {
		return engine_type::min();
	}

	constexpr static result_type max() {
		return engine_type::max();
	}

	void discard(size_t n) const {
		engine().discard(n);
	}

	void reseed() const {
		engine().seed(std::random_device{}());
	}

	void seed(result_type seed = engine_type::default_seed) const {
		engine().seed(seed);
	}

	template<typename _Seq, std::enable_if_t<_Is_seed_seq_v<_Seq>, int> = 0>
	void seed(_Seq& seq) const {
		engine().seed(seq);
	}

	result_type get() const {
		return engine()();
	}

	template<typename T, std::enable_if_t<std::conjunction_v<
		std::is_arithmetic<T>, std::negation<std::is_same<T, bool>>>, int> = 0>
	T get(
		T __min = std::numeric_limits<T>::min(),
		T __max = std::numeric_limits<T>::max()) const {
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
	 R get(
			T __min = std::numeric_limits<T>::min(),
			U __max = std::numeric_limits<U>::max()) const {
		return get<R>(static_cast<R>(__min), static_cast<R>(__max));
	}

	template<typename T, std::enable_if_t<std::is_same_v<T, bool>, int> = 0>
	T get(double p = 0.5) const {
		return bool_dist_t{ p }(engine());
	}

	template<typename Dist, typename...Args>
	typename Dist::result_type get(Args&&...args) const {
		return Dist{ std::forward<Args>(args)... }(engine());
	}

	template<typename Dist>
	typename Dist::result_type get(Dist& dist) const {
		return dist(engine());
	}

	template<typename _Container, std::enable_if_t<std::conjunction_v<
		has_global_function_std_begin<_Container>,
		has_global_function_std_end<_Container>>, int> = 0>
	decltype(auto) get(_Container& container) const {
		auto first = std::begin(container);
		auto last = std::end(container);
		auto n = wjr::size(container);
		if (n == 0)return last;
		using diff_t = wjr::iter_diff_t<decltype(first)>;
		return std::next(first, get<diff_t>(0, n - 1));
	}

	template<typename iter, std::enable_if_t<wjr::is_iterator_v<iter>, int> = 0>
	iter get(iter first, iter last) const {
		return get(wjr::make_iter_wrapper(first, last));
	}

	template<typename T>
	T get(std::initializer_list<T> il) const {
		return *get(std::begin(il), std::end(il));
	}

	template<typename _Ty, size_t _Size>
	_Ty get(_Ty(&arr)[_Size]) const {
		return *get(std::begin(arr), std::end(arr));
	}

	template<typename _Container>
	void shuffle(_Container& container) const {
		std::shuffle(std::begin(container), std::end(container), engine());
	}

	template<typename iter>
	void shuffle(iter first, iter last) const {
		shuffle(wjr::make_iter_wrapper(first, last));
	}

};

template<typename Engine,
	template<typename>typename IntegerDist = std::uniform_int_distribution,
	template<typename>typename RealDist = std::uniform_real_distribution,
	typename BoolDist = std::bernoulli_distribution>
using basic_random = __basic_random<Engine, basic_random_static_wrapper, IntegerDist, RealDist, BoolDist>;

template<typename Engine,
	template<typename>typename IntegerDist = std::uniform_int_distribution,
	template<typename>typename RealDist = std::uniform_real_distribution,
	typename BoolDist = std::bernoulli_distribution>
using basic_random_thread_local = __basic_random<Engine, basic_random_thread_local_wrapper, IntegerDist, RealDist, BoolDist>;

template<
	template<typename>typename IntegerDist = std::uniform_int_distribution,
	template<typename>typename RealDist = std::uniform_real_distribution,
	typename BoolDist = std::bernoulli_distribution>
using default_random = basic_random<std::mt19937, IntegerDist, RealDist, BoolDist>;

template<
	template<typename>typename IntegerDist = std::uniform_int_distribution,
	template<typename>typename RealDist = std::uniform_real_distribution,
	typename BoolDist = std::bernoulli_distribution>
using default_random_thread_local = basic_random_thread_local<std::mt19937, IntegerDist, RealDist, BoolDist>;

inline constexpr default_random<> random;
inline constexpr default_random_thread_local<> random_thread_local;

_WJR_END

#endif // __WJR_RANDOM_H