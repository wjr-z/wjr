#ifndef __WJR_SPTR_WRAPPER_H
#define __WJR_SPTR_WRAPPER_H

#include "mallocator.h"

namespace wjr {

    enum class sptr_wrapper_mode {
        normal,
        unique,
        shared,
    };

    template<typename T, sptr_wrapper_mode>
    class sptr_wrapper : public T {
    public:
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using const_pointer = const T*;
        using const_reference = const T&;
		
        constexpr operator reference() { return *this; }
		constexpr operator const_reference() const { return *this; }
		constexpr reference get_ref(){ return *this; }
        constexpr const_reference get_ref() const { return *this; }
		constexpr reference get_mutable_ref() { return *this; }
		constexpr const_reference get_mutable_ref() const { return *this; }
    };

    template<typename T>
    class sptr_wrapper<T, sptr_wrapper_mode::unique> {
    public:
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using const_pointer = const T*;
        using const_reference = const T&;
        constexpr sptr_wrapper() noexcept(std::is_nothrow_default_constructible_v<T>)
            : m_ptr(mallocate_unique<T>(mallocator<T>{})) {}
        constexpr sptr_wrapper(const sptr_wrapper& other) noexcept(std::is_nothrow_copy_constructible_v<T>)
            : m_ptr(mallocate_unique<T>(mallocator<T>{}, *other.m_ptr)) {}
        constexpr sptr_wrapper(sptr_wrapper&&) noexcept = default;
        template<typename...Args, std::enable_if_t<std::is_constructible_v<T, Args...>,int> = 0 >
        constexpr sptr_wrapper(Args&&...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
            : m_ptr(mallocate_unique<T>(mallocator<T>{}, std::forward<Args>(args)...)) {}
		
		template<std::enable_if_t<std::is_copy_constructible_v<T>,int> = 0>
        constexpr sptr_wrapper& operator=(const sptr_wrapper& other) {
            if (unlikely(this == std::addressof(other))) {
                return *this;
            }
            if constexpr (std::is_copy_assignable_v<T>) {
                if (!m_ptr) {
					m_ptr = mallocate_unique<T>(mallocator<T>{}, other.get());
				} else {
					*m_ptr = *other.m_ptr;
                }
            }
            else {
                m_ptr = mallocate_unique<T>(mallocator<T>{}, other.get());
            }
            return *this;
        }

        constexpr sptr_wrapper& operator=(sptr_wrapper&& other) noexcept = default;
        template<typename U, std::enable_if_t<std::is_constructible_v<T, U>,int> = 0>
		constexpr sptr_wrapper& operator=(U&& other) {
            if constexpr (std::is_assignable_v<T, U>) {
                if (!m_ptr) {
                    m_ptr = mallocate_unique<T>(mallocator<T>{}, std::forward<U>(other));
                }
                else {
                    *m_ptr = std::forward<U>(other);
                }
            }
            else {
				m_ptr = mallocate_unique<T>(mallocator<T>{}, std::forward<U>(other));
            }
			return *this;
		}

        constexpr operator reference() noexcept { return *m_ptr; }
        constexpr operator const_reference() const noexcept { return *m_ptr; }
		constexpr reference get_ref() { return *m_ptr; }
		constexpr const_reference get_ref() const { return *m_ptr; }
		constexpr pointer get_pointer() { return m_ptr.get(); }
		constexpr const_pointer get_pointer() const { return m_ptr.get(); }
		constexpr reference get_mutable_ref() { return *m_ptr; }
		constexpr const_reference get_mutable_ref() const { return *m_ptr; }
		
    private:
        mallocate_unique_ptr_t<T, mallocator<T>> m_ptr;
    };

	template<typename T>
    class sptr_wrapper<T, sptr_wrapper_mode::shared> {
    public:
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using const_pointer = const T*;
		using const_reference = const T&;
		constexpr sptr_wrapper() noexcept(std::is_nothrow_default_constructible_v<T>)
            : m_ptr(std::allocate_shared<T>(mallocator<T>{})) {}
        constexpr sptr_wrapper(const sptr_wrapper& other) 
            noexcept(std::is_nothrow_copy_constructible_v<T>) = default;
		constexpr sptr_wrapper(sptr_wrapper&&) noexcept = default;
		template<typename...Args, std::enable_if_t<std::is_constructible_v<T, Args...>,int> = 0 >
		constexpr sptr_wrapper(Args&&...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
			: m_ptr(std::allocate_shared<T>(mallocator<T>{}, std::forward<Args>(args)...)) {}

        constexpr sptr_wrapper& operator=(const sptr_wrapper&) = default;
        constexpr sptr_wrapper& operator=(sptr_wrapper&&) noexcept = default;
		template<typename U, std::enable_if_t<std::is_constructible_v<T, U>,int> = 0>
        constexpr sptr_wrapper& operator=(U&& other) {
            if constexpr(std::is_assignable_v<T, U>) {
                if (!m_ptr) {
					m_ptr = std::allocate_shared<T>(mallocator<T>{}, std::forward<U>(other));
				}
				else {
					*m_ptr = std::forward<U>(other);
                }
			}
			else {
				m_ptr = std::allocate_shared<T>(mallocator<T>{}, std::forward<U>(other));
			}
            return *this;
        }

        constexpr operator reference() noexcept { return *m_ptr; }
        constexpr operator const_reference() const noexcept { return *m_ptr; }
        constexpr reference get_ref() noexcept { return *m_ptr; }
        constexpr const_reference get_ref() const noexcept { return *m_ptr; }
        constexpr reference get_mutable_ref() noexcept { unshare(); return get_ref(); }
        constexpr const_reference get_mutable_ref() const noexcept { return get_ref(); }
        void unshare() {
            if (m_ptr.use_count() != 1) {
                std::allocate_shared<T>(mallocator<T>{}, *m_ptr).swap(m_ptr);
            }
        }

    private:
		std::shared_ptr<T> m_ptr;
    };

    template<typename T>
	using normal_sptr_wrapper = sptr_wrapper<T, sptr_wrapper_mode::normal>;

	template<typename T>
	using unique_sptr_wrapper = sptr_wrapper<T, sptr_wrapper_mode::unique>;

	template<typename T>
	using shared_sptr_wrapper = sptr_wrapper<T, sptr_wrapper_mode::shared>;

    template<typename T>
    struct wjr_is_sptr_wrapper {
		static constexpr bool value = false;
    };

	template<typename T, sptr_wrapper_mode Mode>
	struct wjr_is_sptr_wrapper<sptr_wrapper<T, Mode>> {
		static constexpr bool value = true;
	};

	template<typename T>
    constexpr bool wjr_is_sptr_wrapper_v = wjr_is_sptr_wrapper<T>::value;

	template<typename T>
    constexpr decltype(auto) get_sptr_ref(T&& value) {
    if constexpr (wjr_is_sptr_wrapper_v<std::decay_t<T>>) {
			return value.get_ref();
		}
		else {
			return std::forward<T>(value);
		}
    }

	template<typename T>
	constexpr decltype(auto) get_sptr_mutable_ref(T&& value) {
		if constexpr (wjr_is_sptr_wrapper_v<std::decay_t<T>>) {
			return value.get_mutable_ref();
		}
		else {
			return std::forward<T>(value);
		}
	}

}

#endif