#ifndef __WJR_EXIST_PTR_H
#define __WJR_EXIST_PTR_H

#include "mallocator.h"

// useless
namespace wjr {
	class exist_ptr;

	class exist_base {
	private:
		friend class exist_ptr;
		struct count_ref {
			size_t count;
			exist_base* ptr;
		};
	public:

		exist_base()
			: ptr(mallocator<count_ref>().allocate(1)) {
			ptr->count = 1;
			ptr->ptr = this;
		}

		exist_base(const exist_base&) = delete;

		exist_base(exist_base&& other) noexcept
			: ptr(other.ptr) {
			other.ptr = nullptr;
		}

		~exist_base() {
			clear();
		}

		exist_base& operator=(const exist_base&) = delete;

		exist_base& operator=(exist_base&& other) noexcept {
			if (unlikely(this == std::addressof(other)))  {
				return *this;
			}
			clear();
			ptr = other.ptr;
			other.ptr = nullptr;
			return *this;
		}

		void release() {
			assert(ptr != nullptr);
			ptr->ptr = nullptr;
		}

		void store() {
			assert(ptr != nullptr);
			ptr->ptr = this;
		}

		bool exist()const {
			assert(ptr != nullptr);
			return ptr->ptr != nullptr;
		}

	private:

		void clear() {
			assert(ptr != nullptr);
			ptr->ptr = nullptr;
			if (!(--ptr->count)) {
				mallocator<count_ref>().deallocate(ptr, 1);
			}
		}

		count_ref* ptr;
	};

	class exist_ptr {
	private:
		using count_ref = exist_base::count_ref;
	public:
		exist_ptr()
			: ptr(nullptr) {
		}

		exist_ptr(const exist_base& p)
			: ptr(nullptr) {
			store(p);
		}

		exist_ptr(const exist_ptr&) = delete;

		exist_ptr(exist_ptr&& other) noexcept
			: ptr(other.ptr) {
			other.ptr = nullptr;
		}

		~exist_ptr() {
			release();
		}

		exist_ptr& operator=(const exist_ptr&) = delete;

		exist_ptr& operator=(exist_ptr&& other) noexcept {
			if (unlikely(this == std::addressof(other)))  {
				return *this;
			}
			release();
			ptr = other.ptr;
			other.ptr = nullptr;
			return *this;
		}

		void store(const exist_base& p) {
			assert(p.ptr != nullptr);
			release();
			ptr = p.ptr;
			++ptr->count;
		}

		void release() {
			if (ptr != nullptr) {
				if (!--ptr->count) {
					mallocator<count_ref>().deallocate(ptr, 1);
				}
				ptr = nullptr;
			}
		}

		bool exist()const {
			return ptr != nullptr && (ptr->ptr != nullptr);
		}

		exist_base* get_base() {
			return ptr->ptr;
		}

		const exist_base* get_base()const {
			return ptr->ptr;
		}

	private:

		count_ref* ptr;
	};
}

#endif