#ifndef WJR_CONTIANER_GENERIC_BMAP_HPP__
#define WJR_CONTIANER_GENERIC_BMAP_HPP__

#include <wjr/container/generic/btree.hpp>

namespace wjr {

template <typename Key, typename Ty, typename Pr = std::less<Key>>
using __bmap_traits = btree_traits<Key, Ty, Pr, 16, false>;

template <typename Key, typename Ty, typename Pr = std::less<Key>>
class bmap : public basic_btree<__bmap_traits> {
    using Mybase = basic_btree<__bmap_traits>;

public:
    using key_type = typename Mybase::key_type;
    using mapped_type = typename Mybase::mapped_type;
    using value_type = typename Mybase::value_type;
    using size_type = typename Mybase::size_type;
    using difference_type = typename Mybase::difference_type;
    using key_compare = typename Mybase::key_compare;
    using allocator_type = typename Mybase::allocator_type;
    using reference = typename Mybase::reference;
    using const_reference = typename Mybase::const_reference;
    using pointer = typename Mybase::pointer;
    using const_pointer = typename Mybase::const_pointer;
    using iterator = typename Mybase::iterator;
    using const_iterator = typename Mybase::const_iterator;
    using reverse_iterator = typename Mybase::reverse_iterator;
    using const_reverse_iterator = typename Mybase::const_reverse_iterator;

private:
    size_t m_size;
};

} // namespace wjr

#endif // WJR_CONTIANER_GENERIC_BMAP_HPP__