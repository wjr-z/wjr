#pragma once
#ifndef __WJR_DEQUE_H
#define __WJR_DEQUE_H

#include <wjr/vector.h>

_WJR_BEGIN

template<typename T>
class deque_const_iterator {
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = T;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using pointer = const value_type*;
	using reference = const value_type&;

	static_assert(std::is_signed_v<difference_type>, "");

	WJR_CONSTEXPR20 deque_const_iterator() = default;

	WJR_CONSTEXPR20 deque_const_iterator(
		pointer head, pointer tail,
		difference_type offset
	) : m_head(const_cast<value_type*>(head)), 
		m_tail(const_cast<value_type*>(tail)), 
		m_offset(offset) {}

	WJR_CONSTEXPR20 deque_const_iterator(const deque_const_iterator& other) = default;

	WJR_CONSTEXPR20 deque_const_iterator& operator=(const deque_const_iterator&) = default;

	WJR_CONSTEXPR20 reference operator*() const {
		return m_offset >= 0 ? m_tail[m_offset] : m_head[-m_offset - 1];
	}

	WJR_CONSTEXPR20 reference operator[](difference_type n) const {
		return *((*this) + n);
	}

	WJR_CONSTEXPR20 pointer operator->() const {
		return std::addressof(*this);
	}

	WJR_CONSTEXPR20 deque_const_iterator& operator++() {
		++m_offset;
		return *this;
	}

	WJR_CONSTEXPR20 deque_const_iterator& operator++(int) {
		deque_const_iterator tmp = *this;
		++(*this);
		return tmp;
	}

	WJR_CONSTEXPR20 deque_const_iterator& operator--() {
		--m_offset;
		return *this;
	}

	WJR_CONSTEXPR20 deque_const_iterator& operator--(int) {
		deque_const_iterator tmp = *this;
		--(*this);
		return tmp;
	}

	WJR_CONSTEXPR20 deque_const_iterator& operator+=(difference_type n) {
		m_offset += n;
		return *this;
	}

	WJR_CONSTEXPR20 deque_const_iterator operator+(difference_type n) const {
		deque_const_iterator tmp = *this;
		return tmp += n;
	}

	WJR_CONSTEXPR20 deque_const_iterator& operator-=(difference_type n) {
		m_offset -= n;
		return *this;
	}

	WJR_CONSTEXPR20 deque_const_iterator operator-(difference_type n) const {
		deque_const_iterator tmp = *this;
		return tmp -= n;
	}

	WJR_CONSTEXPR20 bool operator==(const deque_const_iterator& other) const {
		return m_offset == other.m_offset;
	}

	WJR_CONSTEXPR20 bool operator!=(const deque_const_iterator& other) const {
		return !(*this == other);
	}

	WJR_CONSTEXPR20 difference_type operator-(const deque_const_iterator& other) const {
		return m_offset - other.m_offset;
	}

	WJR_CONSTEXPR20 difference_type offset() const { return m_offset; }

private:
	value_type* m_head = nullptr;
	value_type* m_tail = nullptr;
	difference_type m_offset;
	// m_offset >= 0 ? ptr in tail : ptr in head
};

template<typename T>
class deque_iterator : public deque_const_iterator<T> {
public:
	using _Mybase = deque_const_iterator<T>;

	using iterator_category = std::random_access_iterator_tag;
	using value_type = T;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using pointer = value_type*;
	using reference = value_type&;

	static_assert(std::is_signed_v<difference_type>, "");
	
	WJR_CONSTEXPR20 deque_iterator() = default;

	WJR_CONSTEXPR20 deque_iterator(
		pointer head, pointer tail,
		difference_type offset
	) : _Mybase(head, tail, offset) {}

	WJR_CONSTEXPR20 deque_iterator(const deque_iterator& other) = default;

	WJR_CONSTEXPR20 deque_iterator& operator=(const deque_iterator&) = default;

	WJR_CONSTEXPR20 reference operator*() const {
		return const_cast<reference>(_Mybase::operator*());
	}

	WJR_CONSTEXPR20 reference operator[](difference_type n) const {
		return const_cast<reference>(_Mybase::operator[](n));
	}

	WJR_CONSTEXPR20 pointer operator->() const {
		return const_cast<pointer>(_Mybase::operator->());
	}

	WJR_CONSTEXPR20 deque_iterator& operator++() {
		deque_const_iterator<T>::operator++();
		return *this;
	}

	WJR_CONSTEXPR20 deque_iterator& operator++(int) {
		deque_iterator tmp = *this;
		++(*this);
		return tmp;
	}

	WJR_CONSTEXPR20 deque_iterator& operator--() {
		deque_const_iterator<T>::operator--();
		return *this;
	}

	WJR_CONSTEXPR20 deque_iterator& operator--(int) {
		deque_iterator tmp = *this;
		--(*this);
		return tmp;
	}

	WJR_CONSTEXPR20 deque_iterator& operator+=(difference_type n) {
		deque_const_iterator<T>::operator+=(n);
		return *this;
	}

	WJR_CONSTEXPR20 deque_iterator operator+(difference_type n) const {
		deque_iterator tmp = *this;
		return tmp += n;
	}

	WJR_CONSTEXPR20 deque_iterator& operator-=(difference_type n) {
		deque_const_iterator<T>::operator-=(n);
		return *this;
	}

	WJR_CONSTEXPR20 deque_iterator operator-(difference_type n) const {
		deque_iterator tmp = *this;
		return tmp -= n;
	}

	WJR_CONSTEXPR20 bool operator==(const deque_iterator& other) const {
		return deque_const_iterator<T>::operator==(other);
	}

	WJR_CONSTEXPR20 bool operator!=(const deque_iterator& other) const {
		return !(*this == other);
	}

	WJR_CONSTEXPR20 difference_type operator-(const deque_iterator& other) const {
		return deque_const_iterator<T>::operator-(other);
	}

	WJR_CONSTEXPR20 difference_type offset() const { return _Mybase::offset();; }
};

template <typename T, typename Alloc = std::allocator<T>, typename Data = wjr::vector_data<T, Alloc>>
class deque {
public:
	using value_type = T;
	using allocator_type = Alloc;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using reference = T&;
	using const_reference = const T;
	using pointer = typename std::allocator_traits<allocator_type>::pointer;
	using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;
	
	// The iterator has not been optimized yet
	// If you need optimization, please use [head/tail]_ end
	using iterator = deque_iterator<T>;
	using const_iterator = deque_const_iterator<T>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	using vector_type = wjr::vector<T, Alloc, Data>;

	static_assert(std::is_signed_v<difference_type>, "");
	
	WJR_CONSTEXPR20 deque() = default;
	
	WJR_CONSTEXPR20 explicit deque(const allocator_type& al)
		: head(al), tail(al) {}

	WJR_CONSTEXPR20 deque(size_type n, const T& val, const allocator_type& al = allocator_type())
		: head(al), tail(n, val, al) { }

	WJR_CONSTEXPR20 explicit deque(size_type n, const allocator_type& al = allocator_type())
		: head(al), tail(n, al) { }
	
	template <typename InputIt>
	WJR_CONSTEXPR20 deque(InputIt first, InputIt last, const allocator_type& al = allocator_type())
		: head(al), tail(first, last, al) { }
	
	WJR_CONSTEXPR20 deque(const deque& other)
		: head(other.head), tail(other.tail) {}

	WJR_CONSTEXPR20 deque(deque&& other) noexcept
		: head(std::move(other.head)), tail(std::move(other.tail)) {}

	WJR_CONSTEXPR20 deque(std::initializer_list<T> il, const allocator_type& al = allocator_type())
		: head(al), tail(il, al) { }

	~deque() = default;
	
	WJR_CONSTEXPR20 deque& operator=(const deque& other) {
		if (is_likely(this != std::addressof(other))) {
			head = other.head;
			tail = other.tail;
		}
		return *this;
	}
	
	WJR_CONSTEXPR20 deque& operator=(deque&& other) noexcept {
		if (is_likely(this != std::addressof(other))) {
			head = std::move(other.head);
			tail = std::move(other.tail);
		}
		return *this;
	}

	WJR_CONSTEXPR20 deque& operator=(std::initializer_list<T> il) {
		assign(il);
		return *this;
	}

	WJR_CONSTEXPR20 void assign(size_type n, const T& val) {
		head.clear();
		tail.assign(n, val);
	}

	template <typename InputIt>
	WJR_CONSTEXPR20 void assign(InputIt first, InputIt last) {
		head.clear();
		tail.assign(first, last);
	}

	WJR_CONSTEXPR20 void assign(std::initializer_list<T> il) {
		head.clear();
		tail.assign(il);
	}

	WJR_CONSTEXPR20 reference at(size_type pos) {
		if (is_unlikely(pos >= size())) {
			throw std::out_of_range("deque::at");
		}
		return (*this)[pos];
	}

	WJR_CONSTEXPR20 const_reference at(size_type pos) const {
		if (is_unlikely(pos >= size())) {
			throw std::out_of_range("deque::at");
		}
		return (*this)[pos];
	}

	WJR_CONSTEXPR20 reference operator[](size_type pos) {
		return pos < head.size() ? head[head.size() - pos - 1] : tail[pos - head.size()];
	}

	WJR_CONSTEXPR20 const_reference operator[](size_type pos) const {
		return pos < head.size() ? head[head.size() - pos - 1] : tail[pos - head.size()];
	}
	
	WJR_CONSTEXPR20 reference front() {
		return head.empty() ? tail.front() : head.back();
	}
	
	WJR_CONSTEXPR20 const_reference front() const {
		return head.empty() ? tail.front() : head.back();
	}
	
	WJR_CONSTEXPR20 reference back() {
		return tail.empty() ? head.front() : tail.back();
	}

	WJR_CONSTEXPR20 const_reference back() const {
		return tail.empty() ? head.front() : tail.back();
	}

	WJR_CONSTEXPR20 iterator begin() noexcept {
		const auto offset = -std::max(
			static_cast<ptrdiff_t>(0),
			static_cast<ptrdiff_t>(head.size())
		);
		return iterator(head.begin(), tail.begin(), offset);
	}

	WJR_CONSTEXPR20 const_iterator begin() const noexcept {
		return cbegin();
	}

	WJR_CONSTEXPR20 const_iterator cbegin() const noexcept {
		const auto offset = -std::max(
			static_cast<difference_type>(0),
			static_cast<difference_type>(head.size())
		);
		return const_iterator(head.begin(), tail.begin(), offset);
	}

	WJR_CONSTEXPR20 iterator end() noexcept {
		const auto offset = std::max(
			static_cast<difference_type>(0),
			static_cast<difference_type>(tail.size())
		);
		return iterator(head.begin(), tail.begin(), offset);
	}

	WJR_CONSTEXPR20 const_iterator end() const noexcept {
		return cend();
	}

	WJR_CONSTEXPR20 const_iterator cend() const noexcept {
		const auto offset = std::max(
			static_cast<difference_type>(0),
			static_cast<difference_type>(tail.size())
		);
		return const_iterator(head.begin(), tail.begin(), offset);
	}

	WJR_CONSTEXPR20 reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}

	WJR_CONSTEXPR20 const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}

	WJR_CONSTEXPR20 const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(end());
	}

	WJR_CONSTEXPR20 reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}

	WJR_CONSTEXPR20 const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}

	WJR_CONSTEXPR20 const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(begin());
	}

	WJR_CONSTEXPR20 bool empty() const noexcept {
		return head.empty() && tail.empty();
	}

	WJR_CONSTEXPR20 size_type size() const noexcept {
		return head.size() + tail.size();
	}

	WJR_CONSTEXPR20 size_type max_size() const noexcept {
		return std::min(head.max_size(), tail.max_size());
	}

	WJR_CONSTEXPR20 void reserve(size_type new_cap) {
		head.reserve(new_cap / 2 + 1);
		tail.reserve(new_cap / 2 + 1);
	}

	WJR_CONSTEXPR20 size_type capacity() const noexcept {
		return head.capacity() + tail.capacity();
	}

	WJR_CONSTEXPR20 void shrink_to_fit() {
		head.shrink_to_fit();
		tail.shrink_to_fit();
	}

	WJR_CONSTEXPR20 void clear() noexcept {
		head.clear();
		tail.clear();
	}

	WJR_CONSTEXPR20 iterator insert(const_iterator pos, const T& value) {
		const auto _Pos = pos - cbegin();
		const auto offset = pos.offset();
		if (offset < 0) {
			head.insert(head.begin() - offset - 1, value);
		}
		else {
			tail.insert(tail.begin() + offset, value);
		}
		return begin() + _Pos;
	}

	WJR_CONSTEXPR20 iterator insert(const_iterator pos, T&& value) {
		const auto _Pos = pos - cbegin();
		const auto offset = pos.offset();
		if (offset < 0) {
			head.insert(head.begin() - offset - 1, std::move(value));
		}
		else {
			tail.insert(tail.begin() + offset, std::move(value));
		}
		return begin() + _Pos;
	}

	WJR_CONSTEXPR20 iterator insert(const_iterator pos, size_type count, const T& value) {
		const auto _Pos = pos - cbegin();
		const auto offset = pos.offset();
		if (offset < 0) {
			head.insert(head.begin() - offset - 1, count, value);
		}
		else {
			tail.insert(tail.begin() + offset, count, value);
		}
		return begin() + _Pos;
	}

	template <typename InputIt>
	WJR_CONSTEXPR20 iterator insert(const_iterator pos, InputIt first, InputIt last) {
		const auto _Pos = pos - cbegin();
		const auto offset = pos.offset();
		if (offset < 0) {
			head.insert(head.begin() - offset - 1, 
				std::make_reverse_iterator(last), 
				std::make_reverse_iterator(first));
		}
		else {
			tail.insert(tail.begin() + offset, first, last);
		}
		return begin() + _Pos;
	}

	WJR_CONSTEXPR20 iterator insert(const_iterator pos, std::initializer_list<T> il) {
		return insert(pos, il.begin(), il.end());
	}

	template<typename...Args>
	WJR_CONSTEXPR20 iterator emplace(const_iterator pos, Args&&...args) {
		const auto _Pos = pos - cbegin();
		const auto offset = pos.offset();
		if (offset < 0) {
			head.emplace(head.begin() - offset - 1, std::forward<Args>(args)...);
		}
		else {
			tail.insert(tail.begin() + offset, std::forward<Args>(args)...);
		}
		return begin() + _Pos;
	}

	WJR_CONSTEXPR20 iterator erase(const_iterator pos) {
		const auto _Pos = pos - cbegin();
		const auto offset = pos.offset();
		if (offset < 0) {
			head.erase(head.begin() - offset - 1);
		}
		else {
			tail.erase(tail.begin() + offset);
		}
		return begin() + _Pos;
	}

	WJR_CONSTEXPR20 iterator erase(const_iterator first, const_iterator last) {
		const auto _Pos = first - cbegin();
		const auto offset1 = first.offset();
		const auto offset2 = last.offset();
		WJR_ASSUME(offset1 <= offset2);
		if (offset1 < 0) {
			if (offset2 < 0) {
				head.erase(head.begin() - offset2 - 1, head.begin() - offset1 - 1);
			}
			else {
				head.erase(head.begin(), head.begin() - offset1 - 1);
				tail.erase(tail.begin(), tail.begin() + offset2);
			}
		}
		else {
			tail.erase(tail.begin() + offset1, tail.begin() + offset2);
		}
		return begin() + _Pos;
	}

	WJR_CONSTEXPR20 void push_back(const T& value) {
		tail.push_back(value);
	}

	WJR_CONSTEXPR20 void push_back(T&& value) {
		tail.push_back(std::move(value));
	}

	template<typename...Args>
	WJR_CONSTEXPR20 reference emplace_back(Args&&...args) {
		return tail.emplace_back(std::forward<Args>(args)...);
	}

	WJR_CONSTEXPR20 void pop_back() {
		if (tail.empty()) {
			const auto mid = head.size() / 2 + 1;
			tail.assign(
				std::make_move_iterator(std::make_reverse_iterator(head.begin() + mid)),
				std::make_move_iterator(std::make_reverse_iterator(head.begin())));
			head.erase(head.begin(), head.begin() + mid);
		}
		tail.pop_back();
	}

	WJR_CONSTEXPR20 void push_front(const T& value) {
		head.push_back(value);
	}

	WJR_CONSTEXPR20 void push_front(T&& value) {
		head.push_back(std::move(value));
	}

	template<typename...Args>
	WJR_CONSTEXPR20 reference emplace_front(Args&&...args) {
		return head.emplace_back(std::forward<Args>(args)...);
	}

	WJR_CONSTEXPR20 void pop_front() {
		if (head.empty()) {
			const auto mid = tail.size() / 2 + 1;
			head.assign(
				std::make_move_iterator(std::make_reverse_iterator(tail.begin() + mid)), 
				std::make_move_iterator(std::make_reverse_iterator(tail.begin())));
			tail.erase(tail.begin(), tail.begin() + mid);
		}
		head.pop_back();
	}

	WJR_CONSTEXPR20 void resize(size_type count) {
		const auto old_size = size();
		if (count < old_size) {
			erase(begin() + count, end());
		}
		else {
			tail.append(count - old_size, value_construct_tag());
		}
	}

	WJR_CONSTEXPR20 void resize(size_type count, const T& value) {
		const auto old_size = size();
		if (count < old_size) {
			erase(begin() + count, end());
		}
		else {
			tail.append(count - old_size, value);
		}
	}

	WJR_CONSTEXPR20 void swap(deque& other) noexcept {
		head.swap(other.head);
		tail.swap(other.tail);
	}

	// non-standard extension functions

	vector_type& head_container() { return head; }
	const vector_type& head_container() const { return head; }

	vector_type& tail_container() { return head; }
	const vector_type& tail_container() const { return head; }

	reverse_iterator head_begin() { return head.rbegin(); }
	const_reverse_iterator head_begin() const { return head.rbegin(); }
	const_reverse_iterator head_cbegin() const { return head.rbegin(); }
	reverse_iterator head_end() { return head.rend(); }
	const_reverse_iterator head_end() const { return head.rend(); }
	const_reverse_iterator head_cend() const { return head.rend(); }

	iterator tail_begin() { return tail.begin(); }
	const_iterator tail_begin() const { return tail.begin(); }
	const_iterator tail_cbegin() const { return tail.begin(); }
	iterator tail_end() { return tail.end(); }
	const_iterator tail_end() const { return tail.end(); }
	const_iterator tail_cend() const { return tail.end(); }

	reverse_iterator tail_rbegin() { return tail.rbegin(); }
	const_reverse_iterator tail_rbegin() const { return tail.rbegin(); }
	const_reverse_iterator tail_crbegin() const { return tail.rbegin(); }
	reverse_iterator tail_rend() { return tail.rend(); }
	const_reverse_iterator tail_rend() const { return tail.rend(); }
	const_reverse_iterator tail_crend() const { return tail.rend(); }

private:
	vector_type head;
	vector_type tail;
};

template <typename T, typename Alloc>
WJR_CONSTEXPR20 bool operator==(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs) {
	return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T, typename Alloc>
WJR_CONSTEXPR20 bool operator!=(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs) {
	return !(lhs == rhs);
}

template <typename T, typename Alloc>
WJR_CONSTEXPR20 bool operator<(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T, typename Alloc>
WJR_CONSTEXPR20 bool operator<=(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs) {
	return !(rhs < lhs);
}

template <typename T, typename Alloc>
WJR_CONSTEXPR20 bool operator>(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs) {
	return rhs < lhs;
}

template <typename T, typename Alloc>
WJR_CONSTEXPR20 bool operator>=(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs) {
	return !(lhs < rhs);
}

_WJR_END

namespace std {
	template <typename T, typename Alloc>
	WJR_CONSTEXPR20 void swap(wjr::deque<T, Alloc>& lhs, wjr::deque<T, Alloc>& rhs) noexcept {
		lhs.swap(rhs);
	}
}

#endif // __WJR_DEQUE_H