#ifndef __WJR_GRAPHIC_H
#define __WJR_GRAPHIC_H

#include <forward_list>
#include <list>
#include <vector>
#include "exist_ptr.h"

namespace wjr {
	template<typename T /*node info*/, typename U /*edge info*/>
	class wnode {
	private:
		class wedge;
		class simple_ptr;
		using node_al = mallocator<T>;
		using edge_al = mallocator<wedge>;
		using simple_ptr_al = mallocator<simple_ptr>;
		class wedge {
		private:
			using edge_list = std::list<wedge, edge_al>;
			using iterator = typename edge_list::iterator;
		public:
			template<typename...Args>
			wedge(wnode* _Nxt, Args&&...args)
				: pr(U(std::forward<Args>(args)...),_Nxt) {
			}

			wedge(const wedge&) = delete;

			wedge& operator=(const wedge&) = delete;

			~wedge() = default;

			U& info() {
				return pr.first();
			}

			const U& info()const {
				return pr.first();
			}

			wnode* nxt() {
				return pr.second();
			}

			const wnode* nxt()const {
				return pr.second();
			}
		private:
			mcompressed_pair<U, wnode*> pr;
			//wnode* _Nxt;
			//U _Info;
		};

	public:
		using node = wnode<T, U>;
		using edge = wedge;
	private:
		using edge_list = std::list<edge, edge_al>;
	public:
		using iterator = typename edge_list::iterator;
		using const_iterator = typename edge_list::const_iterator;
	private:
		struct simple_ptr {
			exist_ptr _Count;
			iterator _Iter;
			simple_ptr(const exist_base& _Count, iterator _Iter)
				: _Count(_Count), _Iter(_Iter) {
			}

			simple_ptr(const simple_ptr&) = delete;

			simple_ptr(simple_ptr&& other) noexcept
				: _Count(std::move(other._Count)),
				_Iter(std::move(other._Iter)) {
			}

			simple_ptr& operator=(const simple_ptr&) = delete;

			~simple_ptr() {
				if (_Count.exist()) {
					get_wnode(_Count)->_Edge.erase(_Iter);
				}
			}
		};

	public:

		template<typename...Args>
		wnode(Args&&...args)
			: _Info(node_al().allocate(1)) {
			new (_Info) T(std::forward<Args>(args)...);
		}

		wnode(const wnode&) = delete;

		wnode(wnode&& other)
			: _Count(std::move(other._Count)),
			_Info(other._Info), _Edge(std::move(other._Edge)),
			_Cleaner(std::move(other._Cleaner)) {
			other._Info = nullptr;
		}

		~wnode() {
			clear();
		}

		wnode& operator=(const wnode&) = delete;

		wnode& operator=(wnode&& other) noexcept {
			if (unlikely(this == std::addressof(other))) [[unlikely]] {
				return *this;
			}
			clear();
			_Count = std::move(other._Count);
			_Info = other._Info;
			other._Info = nullptr;
			_Edge = std::move(other._Edge);
			_Cleaner = std::move(other._Cleaner);
			return *this;
		}

		iterator begin() {
			return _Edge.begin();
		}

		const_iterator begin()const {
			return _Edge.begin();
		}

		const_iterator cbegin()const {
			return _Edge.cbegin();
		}

		iterator end() {
			return _Edge.end();
		}

		const_iterator end()const {
			return _Edge.end();
		}

		const_iterator cend()const {
			return _Edge.cend();
		}

		T& info() {
			return *_Info;
		}

		const T& info()const {
			return *_Info;
		}

		template<typename...Args>
		void link(wnode& _Nxt, Args&&...args) {
			_Edge.emplace_back(&_Nxt, std::forward<Args>(args)...);
			_Nxt._Cleaner.emplace_front(_Count, --_Edge.end());
		}

		exist_base& get_exist_base() {
			return _Count;
		}

		const exist_base& get_exist_base()const {
			return _Count;
		}

	private:

		void clear() {
			if (_Info != nullptr) {
				_Info->~T();
				node_al().deallocate(_Info, 1);
				_Edge.clear();
				_Cleaner.clear();
			}
		}

		constexpr static size_t get_count_offset() {
			return offsetof(wnode, _Count);
		}

		constexpr static wnode* get_wnode(exist_ptr& ptr) {
			return static_cast<wnode*>(static_cast<void*>(
				static_cast<unsigned char*>(static_cast<void*>(ptr.get_base())) -
				get_count_offset()));
		}

		exist_base _Count;
		T* _Info;
		edge_list _Edge;
		std::forward_list<simple_ptr, simple_ptr_al> _Cleaner;
	};
}

#endif