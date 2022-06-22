#ifndef __WJR_SPTR_WRAPPER_H
#define __WJR_SPTR_WRAPPER_H

#include "mallocator.h"

namespace wjr {
	
    enum class sptr_wrapper_mode {
        normal,
        unique_ptr,
        shared_ptr,
    };

    template<typename T, sptr_wrapper_mode>
    class sptr_wrapper;

    template<typename T>
    class sptr_wrapper<T, sptr_wrapper_mode::normal> {
    public:
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using const_pointer = const T*;
        using const_reference = const T&;
        constexpr sptr_wrapper() = default;
        constexpr sptr_wrapper(const sptr_wrapper&) = default;
        constexpr sptr_wrapper(sptr_wrapper&&) noexcept = default;
        template<typename...Args, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
        constexpr sptr_wrapper(Args&&...args) : value(std::forward<Args>(args)...) {}
        constexpr sptr_wrapper& operator=(const sptr_wrapper&) = default;
        constexpr sptr_wrapper& operator=(sptr_wrapper&&) noexcept = default;
        template<typename U, std::enable_if_t<std::is_constructible_v<T, U>, int> = 0>
        constexpr sptr_wrapper& operator=(U&& other) {
            value = std::forward<U>(other);
            return *this;
        }
        constexpr reference operator*() noexcept {
            return value;
        }
        constexpr const_reference operator*() const noexcept {
            return value;
        }
        constexpr pointer get() noexcept {
            return &value;
        }
        constexpr const_pointer get() const noexcept {
            return &value;
        }
		constexpr pointer operator->() noexcept {
            return get();
		}
        constexpr const_pointer operator->() const noexcept {
            return get();
        }
    private:
        T value;
    };

    template<typename T>
    class sptr_wrapper<T, sptr_wrapper_mode::unique_ptr> {
    public:
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using const_pointer = const T*;
        using const_reference = const T&;
        constexpr sptr_wrapper()
            : ptr(mallocator_make_unique<T>()) {
        }
        constexpr sptr_wrapper(const sptr_wrapper& other)
            : ptr(mallocator_make_unique<T>(*other.ptr)) {
        }
        constexpr sptr_wrapper(sptr_wrapper&& other) noexcept = default;
		template<typename...Args, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
		constexpr sptr_wrapper(Args&&...args) 
            : ptr(mallocator_make_unique<T>(std::forward<Args>(args)...)) {}
		constexpr sptr_wrapper& operator=(const sptr_wrapper& other) {
            if (unlikely(this == std::addressof(other))) {
                return *this;
            }
            if (!ptr) {
				ptr = mallocator_make_unique<T>(*other.ptr);
            }
            else {
                *ptr = *other;
            }
			return *this;
		}
		constexpr sptr_wrapper& operator=(sptr_wrapper&& other) noexcept = default;
		template<typename U, std::enable_if_t<std::is_constructible_v<T, U>, int> = 0>
        constexpr sptr_wrapper& operator=(U&& other) {
            if (!ptr) {
				ptr = mallocator_make_unique<T>(std::forward<U>(other));
            }
            else {
                *ptr = std::forward<U>(other);
            }
            return *this;
        }
		
        constexpr reference operator*() noexcept {
            WASSERT_LEVEL_1(ptr.get() != nullptr);
			return *ptr;
		}
        constexpr const_reference operator*() const noexcept {
            WASSERT_LEVEL_1(ptr.get() != nullptr);
            return *ptr;
        }
        constexpr pointer get() noexcept {
            WASSERT_LEVEL_1(ptr.get() != nullptr);
            return ptr.get();
        }
        constexpr const_pointer get() const noexcept {
            WASSERT_LEVEL_1(ptr.get() != nullptr);
            return ptr.get();
        }
        constexpr pointer operator->() noexcept {
            return get();
        }			
		constexpr const_pointer operator->() const noexcept {
			return get();
		}
    private:
        mallocator_unique_ptr<T> ptr;
    };

	template<typename T>
    class sptr_wrapper<T, sptr_wrapper_mode::shared_ptr> {
    public:
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using const_pointer = const T*;
        using const_reference = const T&;
        constexpr sptr_wrapper()
            : ptr(std::allocate_shared<T>(mallocator<T>{})) {
        }
        constexpr sptr_wrapper(const sptr_wrapper& other) = default;
		constexpr sptr_wrapper(sptr_wrapper&& other) noexcept = default;
		template<typename...Args, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
		constexpr sptr_wrapper(Args&&...args) 
            : ptr(std::allocate_shared<T>(mallocator<T>{}, std::forward<Args>(args)...)) {}
		constexpr sptr_wrapper& operator=(const sptr_wrapper& other) = default;
		constexpr sptr_wrapper& operator=(sptr_wrapper&& other) noexcept = default;
		template<typename U, std::enable_if_t<std::is_constructible_v<T, U>, int> = 0>
        constexpr sptr_wrapper& operator=(U&& other) {
            if (!ptr) {
                ptr = std::allocate_shared<T>(mallocator<T>{}, std::forward<U>(other));
            }
            else {
                *ptr = std::forward<U>(other);
            }
            return *this;
        }
        constexpr reference operator*() noexcept {
            WASSERT_LEVEL_1(ptr.get() != nullptr);
            return *ptr;
        }
		constexpr const_reference operator*() const noexcept {
			WASSERT_LEVEL_1(ptr.get() != nullptr);
			return *ptr;
		}
        constexpr pointer get() noexcept {
            WASSERT_LEVEL_1(ptr.get() != nullptr);
            return ptr.get();
        }
        constexpr const_pointer get() const noexcept {
            WASSERT_LEVEL_1(ptr.get() != nullptr);
            return ptr.get();
        }
        constexpr pointer operator->() noexcept {
            return get();
        }
        constexpr const_pointer operator->() const noexcept {
            return get();
        }
    private:
        std::shared_ptr<T> ptr;
    };

}

#endif