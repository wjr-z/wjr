#pragma once
#ifndef __WJR_STRING_H
#define __WJR_STRING_H

#pragma push_macro("new")
#undef new

#include <wjr/vector.h>

_WJR_BEGIN

template<typename Char, typename Traits, typename Alloc>
class basic_string {
	using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<Char>;
	using _Alty_traits = std::allocator_traits<_Alty>;
public:
	using value_type = Char;
	using traits_type = Traits;
	using allocator_type = Alloc;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = typename _Alty_traits::pointer;
	using const_pointer = typename _Alty_traits::const_pointer;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	

private:
	wjr::vector<Char, Alloc, wjr::vector_sso_core<Char, Alloc>> _Myval;
};

_WJR_END

#pragma pop_macro("new")

#endif // !__WJR_STRING_H
