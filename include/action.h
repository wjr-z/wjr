#ifndef __WJR_ACTION_H
#define __WJR_ACTION_H

#include "mtype_traits.h"

namespace wjr {
	struct action_tag {
	};

	struct action_logical_not : action_tag {
		template<typename T, typename...Args>
		bool operator()(T&& t, Args&&...args) const {
			return !std::forward<T>(t)(std::forward<Args>(args)...);
		}
	};

	struct action_logical_or : action_tag {
		template<typename T, typename U, typename...Args>
		constexpr bool operator()(T&& lhs, U&& rhs, Args&&...args)const {
			return std::forward<T>(lhs)(std::forward<Args>(args)...)
				|| std::forward<U>(rhs)(std::forward<Args>(args)...);
		}
	};

	struct action_logical_and : action_tag {
		template<typename T, typename U, typename...Args>
		constexpr bool operator()(T&& lhs, U&& rhs, Args&&...args)const {
			return std::forward<T>(lhs)(std::forward<Args>(args)...)
				&& std::forward<U>(rhs)(std::forward<Args>(args)...);
		}
	};

	struct action_logical_xor : action_tag {
		template<typename T, typename U, typename...Args>
		constexpr bool operator()(T&& lhs, U&& rhs, Args&&...args)const {
			return std::forward<T>(lhs)(std::forward<Args>(args)...)
				^ std::forward<U>(rhs)(std::forward<Args>(args)...);
		}
	};

	struct action_link : action_tag {
		template<typename T, typename U, typename...Args>
		constexpr decltype(auto) operator()(T&& lhs, U&& rhs, Args&&...args)const {
			std::forward<T>(lhs)(std::forward<Args>(args)...);
			return std::forward<U>(rhs)(std::forward<Args>(args)...);
		}
	};

	struct action_if : action_tag {
		template<typename Func, typename T, typename...Args>
		constexpr decltype(auto) operator()(Func&& fn, T&& t, Args&&...args)const {
			if (std::forward<Func>(fn)(std::forward<Args>(args)...)) {
				return std::forward<T>(t)(std::forward<Args>(args)...);
			}
		}
	};

	struct action_ifel : action_tag {
		template<typename Func, typename T, typename U, typename...Args>
		constexpr decltype(auto) operator()(Func&& fn, T&& t, U&& u, Args&&...args)const {
			if (std::forward<Func>(fn)(std::forward<Args>(args)...)) {
				return std::forward<T>(t)(std::forward<Args>(args)...);
			}
			else {
				return std::forward<U>(u)(std::forward<Args>(args)...);
			}
		}
	};

	struct action_while : action_tag {
		template<typename T, typename U, typename...Args>
		constexpr decltype(auto) operator()(T&& t, U&& u, Args&&...args)const {
			while (std::forward<T>(t)(std::forward<Args>(args)...)) {
				std::forward<U>(u)(std::forward<Args>(args)...);
			}
		}
	};

	struct action_dowhile : action_tag {
		template<typename T, typename U, typename...Args>
		constexpr decltype(auto) operator()(T&& t, U&& u, Args&&...args)const {
			do {
				std::forward<T>(t)(std::forward<Args>(args)...);
			} while (std::forward<U>(u)(std::forward<Args>(args)...));
		}
	};

	struct action_nothing :action_tag {
		template<typename...Args>
		constexpr void operator()(Args&&...args)const {
		}
	};

	template<typename...T>
	struct action_run : action_tag {
		template<typename Func, typename...Args>
		constexpr decltype(auto) operator()(Func&& fn, T...t, Args&&...args)const {
			return std::forward<Func>(fn)(t..., std::forward<Args>(args)...);
		}
	};

	template<size_t...index>
	struct action_accept_input : action_tag {
		template<typename T, typename...Args>
		constexpr decltype(auto) operator()(T&& t, Args&&...args) const {
			return std::forward<T>(t)(
				std::forward<kth_type<index, Args...>>(
					kth_val<index>(std::forward<Args>(args)...))...);
		}
	};

	template<size_t...index>
	struct action_reject_input : action_tag {
		template<typename T, typename...Args>
		constexpr decltype(auto) operator()(T&& t, Args&&...args) const {
			return accept_input_t<sizeof...(Args)>{}(
				std::forward<T>(t),
				std::forward<Args>(args)...
				);
		}
	private:
		template<size_t cnt>
		using complement_type = make_complement_index_sequence<cnt, std::index_sequence<index...>>;
		template<typename T>
		struct accept_input_helper;

		template<size_t...i>
		struct accept_input_helper<std::index_sequence<i...>> {
			using type = action_accept_input<i...>;
		};
		template<size_t cnt>
		using accept_input_t = typename accept_input_helper<complement_type<cnt>>::type;
	};

	template<typename Enable, typename...T>
	struct action_accept;

	template<size_t...index, typename...T>
	struct action_accept<std::index_sequence<index...>, T...> : action_tag {
		static_assert(sizeof...(index) == sizeof...(T), "");
		static_assert(sizeof...(index) != 0, "");
		template<typename Func, typename...Args>
		constexpr decltype(auto) operator()(Func&& fn, T...t, Args&&...args)const {
			return _Run <std::index_sequence_for<T..., Args...>>::
				run(std::forward<Func>(fn), t..., std::forward<Args>(args)...);
		}
	private:

		template<typename U>
		struct _Run;

		template<size_t...i>
		struct _Run<std::index_sequence<i...>> {
			template<typename Func, typename...Args>
			constexpr static decltype(auto) run(Func&& fn, T...t, Args&&...args) {
				return std::forward<Func>(fn)(get_kth<i>(t..., std::forward<Args>(args)...)...);
			}
		};

		template<size_t pos, size_t kth, size_t val, size_t...i>
		constexpr static size_t get_T_index_helper() {
			if constexpr (val == pos) {
				return kth;
			}
			else if constexpr (sizeof...(i) == 0) {
				return -1;
			}
			else return get_T_index_helper<pos, kth + 1, i...>();
		}

		template<size_t pos>
		constexpr static size_t get_T_index = get_T_index_helper<pos, 0, index...>();

		template<size_t pos>
		constexpr static size_t get_args_index_helper() {
			if constexpr (pos != 0) {
				return get_args_index_helper<pos - 1>() + (get_T_index<pos - 1> == -1);
			}
			else {
				return 0;
			}
		}
		template<size_t pos>
		constexpr static size_t get_args_index = get_args_index_helper<pos>();

		template<size_t kth, typename...Args>
		constexpr static decltype(auto) get_kth(T...t, Args&&...args) {
			if constexpr (get_T_index<kth> != -1) {
				return kth_val<get_T_index<kth>>(t...)(std::forward<Args>(args)...);
			}
			else {
				return kth_val<get_args_index<kth>>(std::forward<Args>(args)...);
			}
		}
	};

	template<typename Enable, typename...T>
	struct action_reject;

	template<size_t...index, typename...T>
	struct action_reject<std::index_sequence<index...>, T...> : action_tag {
		template<typename Func, typename...Args>
		constexpr decltype(auto) operator()(Func&& fn, T...t, Args&&...args) {
			using action_accept_type = action_accept<
				make_complement_index_sequence<sizeof...(T) + sizeof...(Args)>, T...>;
			return action_accept_type{}(std::forward<Func>(fn), t..., std::forward<Args>(args)...);
		}
	};

#define REGISTER_STD_ACTION(FUNC)									\
	struct CONNECT(action_,FUNC) : action_tag{								\
		template<typename...Args>									\
		constexpr decltype(auto) operator()(Args&&...args)const {	\
			return STD_FUNCTION(FUNC)(std::forward<Args>(args)...);		\
		}															\
	};

	REGISTER_STD_ACTION(begin);
	REGISTER_STD_ACTION(end);
	REGISTER_STD_ACTION(sort);
	REGISTER_STD_ACTION(empty);
	REGISTER_STD_ACTION(transform);
	REGISTER_STD_ACTION(reverse);

	struct action_pre_increase : action_tag {
		template<typename T>
		constexpr decltype(auto) operator()(T&& t)const {
			return ++std::forward<T>(t);
		}
	};

	struct action_post_increase : action_tag {
		template<typename T>
		constexpr decltype(auto) operator()(T&& t)const {
			return std::forward<T>(t)++;
		}
	};

	template<typename iter>
	struct action_range : action_tag {
		constexpr action_range(iter first, iter last)
			:first(first), last(last) {}
		constexpr iter begin() { return first; }
		constexpr iter end() { return last; }
		constexpr decltype(auto) operator()() {
			return *this;
		}
	private:
		iter first;
		iter last;
	};

	template<typename Func, typename...Args>
	class action {
	public:
		template<typename F, typename...Other,
			std::enable_if_t<std::is_constructible_v<Func, F>&&
			std::is_constructible_v<std::tuple<Args...>, Other...>, int> = 0>
			constexpr action(F&& fn, Other&&...res)
			: pr(std::forward<F>(fn), std::tuple<Args...>(std::forward<Other>(res)...)) {
		}
		constexpr action(const action&) = default;
		constexpr action(action&&) noexcept = default;
		constexpr action& operator=(const action&) = default;
		constexpr action& operator=(action&&) noexcept = default;
		template<typename...Other>
		constexpr decltype(auto) operator()(Other&&...args) {
			return run(std::index_sequence_for<Args...>{}, std::forward<Other>(args)...);
		}
		template<typename...Other>
		constexpr decltype(auto) operator()(Other&&...args) const {
			return run(std::index_sequence_for<Args...>{}, std::forward<Other>(args)...);
		}
	private:
		template<size_t...index, typename...Other>
		constexpr decltype(auto) run(std::index_sequence<index...>, Other&&...args) {
			return pr.first()(std::get<index>(pr.second())..., std::forward<Other>(args)...);
		}
		template<size_t...index, typename...Other>
		constexpr decltype(auto) run(std::index_sequence<index...>, Other&&...args)const {
			return pr.first()(std::get<index>(pr.second())..., std::forward<Other>(args)...);
		}
		mcompressed_pair<Func, std::tuple<Args...>> pr;
	};

	template<typename Func>
	class action<Func> {
	public:
		template<typename...Other, std::enable_if_t<
			std::is_constructible_v<Func, Other...>, int> = 0>
			constexpr action(Other&&... func)
			: fn(std::forward<Other>(func)...) {
		}
		constexpr action(const action&) = default;
		constexpr action(action&&) noexcept = default;
		constexpr action& operator=(const action&) = default;
		constexpr action& operator=(action&&) noexcept = default;
		template<typename...Args>
		constexpr decltype(auto) operator()(Args&&...args) {
			return fn(std::forward<Args>(args)...);
		}
		template<typename...Args>
		constexpr decltype(auto) operator()(Args&&...args)const {
			return fn(std::forward<Args>(args)...);
		}
	private:
		Func fn;
	};

	template<typename Func, typename...Args>
	constexpr decltype(auto) make_action(Func&& fn, Args&&...args) {
		return action<Func, forward_wrapper_t<Args>...>(std::forward<Func>(fn), std::forward<Args>(args)...);
	}

	template<typename T>
	struct _Is_action : std::is_base_of<action_tag, T> {};

	template<typename...Args>
	struct _Is_action<action<Args...>> : std::true_type {};

	template<typename...Args>
	struct is_action
		: std::disjunction<_Is_action<Args>...> {
	};

	template<typename...Args>
	constexpr static bool is_action_v = is_action<Args...>::value;

	// operator ||
	template<typename T, typename U,
		std::enable_if_t<is_action_v<T, U>, int> = 0>
		constexpr decltype(auto) operator||(T&& lhs, U&& rhs) {
		return make_action(action_logical_or{}, std::forward<T>(lhs), std::forward<U>(rhs));
	}

	// operator&&
	template<typename T, typename U,
		std::enable_if_t<is_action_v<T, U>, int> = 0>
		constexpr decltype(auto) operator&&(T&& lhs, U&& rhs) {
		return make_action(action_logical_and{}, std::forward<T>(lhs), std::forward<U>(rhs));
	}

	// operator^
	template<typename T, typename U,
		std::enable_if_t<is_action_v<T, U>, int> = 0>
		constexpr decltype(auto) operator^(T&& lhs, U&& rhs) {
		return make_action(action_logical_xor{}, std::forward<T>(lhs), std::forward<U>(rhs));
	}

	// operator!
	template<typename T, std::enable_if_t<is_action_v<T>, int> = 0>
	constexpr decltype(auto) operator!(T&& lhs) {
		return make_action(action_logical_not{}, std::forward<T>(lhs));
	}

	// action link
	template<typename T, typename U, std::enable_if_t<is_action_v<T, U>, int> = 0>
	constexpr decltype(auto) operator>(T&& lhs, U&& rhs) {
		return make_action(action_link{}, std::forward<T>(lhs), std::forward<U>(rhs));
	}

	// action if
	template<typename Func, typename T>
	constexpr decltype(auto) make_if(Func&& fn, T&& t) {
		return make_action(action_if{}, std::forward<Func>(fn), std::forward<T>(t));
	}

	// action ifel
	template<typename Func, typename T, typename U>
	constexpr decltype(auto) make_ifel(Func&& fn, T&& t, U&& u) {
		return make_action(action_ifel{}, std::forward<Func>(fn), std::forward<T>(t), std::forward<U>(u));
	}
	template<typename Func, typename T, typename U>
	constexpr decltype(auto) make_if(Func&& fn, T&& t, U&& u) {
		return make_ifel(std::forward<Func>(fn), std::forward<T>(t), std::forward<U>(u));
	}

	template<typename T, typename U>
	constexpr decltype(auto) make_while(T&& t, U&& u) {
		return make_action(action_while{}, std::forward<T>(t), std::forward<U>(u));
	}

	template<typename T, typename U>
	constexpr decltype(auto) make_dowhile(T&& t, U&& u) {
		return make_action(action_dowhile{}, std::forward<T>(t), std::forward<U>(u));
	}

	template<typename...T>
	constexpr decltype(auto) make_run(T&&...t) {
		return make_action(action_run{}, std::forward<T>(t)...);
	}

	// accept some input
	template<size_t...index, typename T>
	constexpr decltype(auto) make_accept_input(T&& t) {
		return make_action(action_accept_input<index...>{}, std::forward<T>(t));
	}

	template<size_t...index, typename T>
	constexpr decltype(auto) make_accept_input(std::index_sequence<index...>, T&& t) {
		return make_accept_input<index...>(std::forward<T>(t));
	}

	// reject some input
	template<size_t...index, typename T>
	constexpr decltype(auto) make_reject_input(T&& t) {
		return make_action(action_reject_input<index...>{}, std::forward<T>(t));
	}

	template<size_t...index, typename T>
	constexpr decltype(auto) make_reject_input(std::index_sequence<index...>, T&& t) {
		return make_reject_input<index...>(std::forward<T>(t));
	}

	// accept some Function's result
	template<size_t...index, typename T, typename...Args>
	constexpr decltype(auto) make_accept(T&& t, Args&&...args) {
		return make_action(action_accept<std::index_sequence<index...>, forward_wrapper_t<Args>...>{},
			std::forward<T>(t), std::forward<Args>(args)...);
	}

	template<size_t...index, typename T, typename...Args>
	constexpr decltype(auto) make_accept(std::index_sequence<index...>, T&& t, Args&&...args) {
		return make_accept<index...>(std::forward<T>(t), std::forward<Args>(args)...);
	}

	// fill Function except index...
	template<size_t...index, typename T, typename...Args>
	constexpr decltype(auto) make_reject(T&& t, Args&&...args) {
		return make_action(action_reject<std::index_sequence<index...>, forward_wrapper_t<Args>...>{},
			std::forward<T>(t), std::forward<Args>(args)...);
	}

	template<size_t...index, typename T, typename...Args>
	constexpr decltype(auto) make_reject(std::index_sequence<index...>, T&& t, Args&&...args) {
		return make_reject<index...>(std::forward<T>(t), std::forward<Args>(args)...);
	}

	template<typename T>
	constexpr decltype(auto) make_accept_range(T&& t) {
		return make_accept<1, 2>(std::forward<T>(t),
			make_accept_input<0>(action_begin{}), make_accept_input<0>(action_end{}));
	}

	template<typename T>
	constexpr decltype(auto) make_reject_range(T&& t) {
		return make_accept<1, 2>(make_reject_input<0>(std::forward<T>(t)),
			make_accept_input<0>(action_begin{}), make_accept_input<0>(action_end{}));
	}

#define ACTION_IF(FUNC)			\
	make_if(FUNC,

#define ACTION_ELSE	,

#define ACTION_WHILE(FUNC)		\
	make_while(FUNC,

#define ACTION_DOWHILE(FUNC)	\
	make_dowhile(FUNC,

#define ACTION_END );
}

#endif