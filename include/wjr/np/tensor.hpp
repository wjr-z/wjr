#ifndef WJR_NP_TENSOR_HPP__
#define WJR_NP_TENSOR_HPP__

#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

#include <wjr/compressed_pair.hpp>
#include <wjr/container/vector.hpp>
#include <wjr/np/checked_view.hpp>

namespace wjr::np {

namespace detail {

template <class C>
inline constexpr bool is_std_array = false;
template <class T, size_t N>
inline constexpr bool is_std_array<std::array<T, N>> = true;

template <class C>
inline constexpr bool is_std_vector = false;
template <class T, class A>
inline constexpr bool is_std_vector<std::vector<T, A>> = true;

template <class T, class C>
concept tensor_container =
    std::is_object_v<T> && std::same_as<T, std::remove_cv_t<T>> &&
    requires {
        sizeof(T);
        typename C::value_type;
        typename C::size_type;
    } && std::same_as<T, typename C::value_type> && std::unsigned_integral<typename C::size_type> &&
    std::is_nothrow_destructible_v<C> &&
    requires(C &storage, const C &constant, typename C::size_type i) {
        { storage.data() } noexcept -> std::same_as<T *>;
        { constant.data() } noexcept -> std::same_as<const T *>;
        { constant.size() } noexcept -> std::same_as<typename C::size_type>;
        { constant.max_size() } noexcept -> std::same_as<typename C::size_type>;
        { storage[i] } -> std::same_as<T &>;
        { constant[i] } -> std::same_as<const T &>;
    };

template <class C>
concept allocated_tensor_container =
    requires(const C &storage, const typename C::allocator_type &a, typename C::size_type n) {
        C(a);
        C(n, a);
        { storage.get_allocator() } -> std::convertible_to<typename C::allocator_type>;
    };

template <class C>
concept default_initialized_tensor_container =
    allocated_tensor_container<C> && !is_std_vector<C> &&
    requires(const typename C::allocator_type &a, typename C::size_type n) {
        C(n, default_construct, a);
    };

template <class C>
concept default_tensor_storage =
    is_std_array<C> || std::default_initializable<typename C::allocator_type>;

template <class T, class Extents>
inline constexpr auto default_tensor_metadata = [] {
    std::array<size_t, Extents::rank()> shape{};
    for (size_t r = 0; r != Extents::rank(); ++r) {
        if (Extents::static_extent(r) != dynamic_extent) {
            shape[r] = Extents::static_extent(r);
        }
    }
    return check_metadata<T, Extents>(shape);
}();

template <class T, class Extents, class Layout, bool = (Extents::rank_dynamic() == 0)>
class tensor_mapping_holder {
    using mapping_type = typename Layout::template mapping<Extents>;

public:
    constexpr tensor_mapping_holder() noexcept = default;
    constexpr explicit tensor_mapping_holder(const mapping_type &mapping) noexcept
        : m_mapping(mapping) {}
    constexpr const mapping_type &mapping() const noexcept { return m_mapping; }

private:
    mapping_type m_mapping;
};

template <class T, class Extents, class Layout>
class tensor_mapping_holder<T, Extents, Layout, true> {
    using mapping_type = typename Layout::template mapping<Extents>;
    static_assert(default_tensor_metadata<T, Extents>.has_value(),
                  "tensor static shape is not representable");
    inline static constexpr mapping_type m_mapping{
        Extents(default_tensor_metadata<T, Extents>->dimensions)};

public:
    constexpr tensor_mapping_holder() noexcept = default;
    constexpr explicit tensor_mapping_holder(const mapping_type &) noexcept {}
    constexpr const mapping_type &mapping() const noexcept { return m_mapping; }
};

struct tensor_access {
    template <class Owner, class... Args>
    static constexpr shape_result<Owner>
    make(const checked_metadata<typename Owner::extents_type> &metadata, Args &&...args) {
        using mapping_type = typename Owner::mapping_type;
        return shape_result<Owner>(std::in_place, typename Owner::construction_key{},
                                   mapping_type(typename Owner::extents_type(metadata.dimensions)),
                                   std::forward<Args>(args)...);
    }
};

template <class T>
inline constexpr bool is_tensor = false;

} // namespace detail

// Owns objects through Container; computation and ordinary indexing use view().
template <class T, class Extents, class Layout = layout_right, class Container = wjr::vector<T>>
requires(detail::is_extents<Extents> && detail::tensor_container<T, Container> &&
         (std::same_as<Layout, layout_right> || std::same_as<Layout, layout_left>) &&
         (detail::is_std_array<Container> || detail::allocated_tensor_container<Container>))
class tensor {
    class construction_key {
        constexpr construction_key() = default;
        friend struct detail::tensor_access;
    };
    using holder_type = detail::tensor_mapping_holder<T, Extents, Layout>;
    static constexpr auto default_metadata = detail::default_tensor_metadata<T, Extents>;
    static_assert(default_metadata.has_value(), "tensor default shape is not representable");
    static_assert(wjr::in_range<typename Container::size_type>(default_metadata->size),
                  "tensor default shape exceeds container size_type");
    static_assert(
        [] {
            if constexpr (detail::is_std_array<Container>) {
                return Extents::rank_dynamic() == 0 &&
                       std::tuple_size_v<Container> == default_metadata->size;
            }
            return true;
        }(),
        "array storage requires matching fully static extents");
    static_assert(
        std::is_nothrow_copy_constructible_v<typename Layout::template mapping<Extents>> &&
        std::is_nothrow_copy_assignable_v<typename Layout::template mapping<Extents>>);

public:
    using element_type = T;
    using extents_type = Extents;
    using layout_type = Layout;
    using container_type = Container;
    using index_type = typename Extents::index_type;
    using size_type = typename Container::size_type;
    using mapping_type = typename Layout::template mapping<Extents>;
    using view_type = tensor_view<T, Extents, Layout>;
    using const_view_type = tensor_view<const T, Extents, Layout>;

    constexpr tensor()
    requires(std::default_initializable<T> && detail::default_tensor_storage<Container>)
        : m_storage(std::piecewise_construct, wjr::tuple<>{}, default_storage_args()) {}

    // The private key lets checked factories construct expected's value in place.
    constexpr tensor(construction_key, const mapping_type &mapping,
                     Container &&storage) noexcept(std::is_nothrow_move_constructible_v<Container>)
        : m_storage(holder_type(mapping), std::move(storage)) {}
    constexpr tensor(construction_key, const mapping_type &mapping)
    requires detail::is_std_array<Container>
        : m_storage(std::piecewise_construct, wjr::tuple<const mapping_type &>{mapping},
                    wjr::tuple<>{}) {}
    template <class C = Container>
    requires detail::allocated_tensor_container<C>
    constexpr tensor(construction_key, const mapping_type &mapping, size_type count,
                     const typename C::allocator_type &allocator)
        : m_storage(std::piecewise_construct, wjr::tuple<const mapping_type &>{mapping},
                    wjr::tuple<size_type, const typename C::allocator_type &>{count, allocator}) {}
    template <class C = Container>
    requires detail::default_initialized_tensor_container<C>
    constexpr tensor(construction_key, const mapping_type &mapping, size_type count,
                     default_construct_t, const typename C::allocator_type &allocator)
        : m_storage(std::piecewise_construct, wjr::tuple<const mapping_type &>{mapping},
                    wjr::tuple<size_type, default_construct_t, const typename C::allocator_type &>{
                        count, default_construct, allocator}) {}
    tensor(const tensor &) = default;
    constexpr tensor(tensor &&other) noexcept(std::is_nothrow_move_constructible_v<Container>)
    requires std::is_move_constructible_v<Container>
        : m_storage(other.m_storage.first(), std::move(other.m_storage.second())) {}

    constexpr tensor &
    operator=(const tensor &other) noexcept(std::is_nothrow_copy_assignable_v<Container>)
    requires std::is_copy_assignable_v<Container>
    {
        if (this != std::addressof(other)) {
            m_storage.second() = other.m_storage.second();
            m_storage.first() = other.m_storage.first();
        }
        return *this;
    }
    constexpr tensor &
    operator=(tensor &&other) noexcept(std::is_nothrow_move_assignable_v<Container>)
    requires std::is_move_assignable_v<Container>
    {
        if (this != std::addressof(other)) {
            m_storage.second() = std::move(other.m_storage.second());
            m_storage.first() = other.m_storage.first();
        }
        return *this;
    }

    static constexpr size_t rank() noexcept { return Extents::rank(); }
    constexpr const mapping_type &mapping() const & noexcept { return m_storage.first().mapping(); }
    const mapping_type &mapping() const && = delete;
    constexpr const Extents &extents() const & noexcept { return mapping().extents(); }
    const Extents &extents() const && = delete;
    constexpr index_type extent(size_t r) const noexcept { return extents().extent(r); }
    constexpr size_type size() const noexcept {
        return static_cast<size_type>(mapping().required_span_size());
    }
    constexpr bool empty() const noexcept { return size() == 0; }
    constexpr size_type storage_size() const noexcept { return m_storage.second().size(); }
    constexpr bool has_storage() const noexcept { return storage_size() == size(); }

    // Precondition: has_storage(). Use try_view() for a recoverable state check.
    constexpr view_type view() & noexcept {
        WJR_ASSERT(has_storage());
        return view_type(m_storage.second().data(), mapping());
    }
    constexpr const_view_type view() const & noexcept {
        WJR_ASSERT(has_storage());
        return const_view_type(m_storage.second().data(), mapping());
    }
    view_type view() && = delete;
    const_view_type view() const && = delete;
    constexpr std::optional<view_type> try_view() & noexcept {
        if (!has_storage())
            return std::nullopt;
        return view_type(m_storage.second().data(), mapping());
    }
    constexpr std::optional<const_view_type> try_view() const & noexcept {
        if (!has_storage())
            return std::nullopt;
        return const_view_type(m_storage.second().data(), mapping());
    }
    std::optional<view_type> try_view() && = delete;
    std::optional<const_view_type> try_view() const && = delete;

private:
    friend struct detail::tensor_access;
    static constexpr auto default_storage_args() {
        if constexpr (detail::is_std_array<Container>) {
            return wjr::tuple<>{};
        } else {
            using allocator_type = typename Container::allocator_type;
            allocator_type allocator;
            if constexpr (default_metadata->size != 0) {
                if (WJR_UNLIKELY(wjr::cmp_greater(
                        default_metadata->size,
                        std::allocator_traits<allocator_type>::max_size(allocator)))) {
                    WJR_THROW(std::length_error("tensor default shape exceeds allocator max_size"));
                }
            }
            return wjr::tuple<size_type, typename Container::allocator_type>{
                static_cast<size_type>(default_metadata->size), std::move(allocator)};
        }
    }
    compressed_pair<holder_type, Container> m_storage;
};

namespace detail {
template <class T, class E, class L, class C>
inline constexpr bool is_tensor<tensor<T, E, L, C>> = true;

template <class Owner, class Integer>
concept tensor_shape =
    is_tensor<Owner> && std::integral<Integer> && !std::same_as<std::remove_cv_t<Integer>, bool>;

template <class Owner, std::integral Integer, class... Initialization>
constexpr expected<Owner, shape_error>
make_allocated_tensor(const std::array<Integer, Owner::rank()> &shape,
                      const typename Owner::container_type::allocator_type &allocator,
                      Initialization... initialization) {
    using C = typename Owner::container_type;
    using allocator_traits = std::allocator_traits<typename C::allocator_type>;
    auto metadata =
        check_metadata<typename Owner::element_type, typename Owner::extents_type>(shape);
    if (!metadata)
        return unexpected(metadata.error());
    if (!wjr::in_range<typename C::size_type>(metadata->size))
        return unexpected(shape_error::size_overflow);
    if (metadata->size > C(allocator).max_size() ||
        wjr::cmp_greater(metadata->size, allocator_traits::max_size(allocator)))
        return unexpected(shape_error::container_size_limit);
    return tensor_access::make<Owner>(*metadata, static_cast<typename C::size_type>(metadata->size),
                                      initialization..., allocator);
}
} // namespace detail

template <class Owner, std::integral Integer>
requires(detail::tensor_shape<Owner, Integer> &&
         detail::allocated_tensor_container<typename Owner::container_type> &&
         std::default_initializable<typename Owner::element_type>)
[[nodiscard]] constexpr shape_result<Owner>
make_tensor(const std::array<Integer, Owner::rank()> &shape, std::allocator_arg_t,
            const typename Owner::container_type::allocator_type &allocator) {
    return detail::make_allocated_tensor<Owner>(shape, allocator);
}

// Default-initializes live objects. Scalar values must be written before reading.
template <class Owner, std::integral Integer>
requires(detail::tensor_shape<Owner, Integer> &&
         detail::default_initialized_tensor_container<typename Owner::container_type> &&
         std::default_initializable<typename Owner::element_type>)
[[nodiscard]] constexpr expected<Owner, shape_error>
make_tensor(const std::array<Integer, Owner::rank()> &shape, default_construct_t,
            std::allocator_arg_t, const typename Owner::container_type::allocator_type &allocator) {
    return detail::make_allocated_tensor<Owner>(shape, allocator, default_construct);
}

template <class Owner, std::integral Integer>
requires(detail::tensor_shape<Owner, Integer> &&
         detail::default_initialized_tensor_container<typename Owner::container_type> &&
         std::default_initializable<typename Owner::container_type::allocator_type> &&
         std::default_initializable<typename Owner::element_type>)
[[nodiscard]] constexpr expected<Owner, shape_error>
make_tensor(const std::array<Integer, Owner::rank()> &shape, default_construct_t) {
    return make_tensor<Owner>(shape, default_construct, std::allocator_arg,
                              typename Owner::container_type::allocator_type{});
}

template <class Owner, std::integral Integer>
requires(detail::tensor_shape<Owner, Integer> &&
         std::default_initializable<typename Owner::element_type> &&
         detail::default_tensor_storage<typename Owner::container_type>)
[[nodiscard]] constexpr shape_result<Owner>
make_tensor(const std::array<Integer, Owner::rank()> &shape) {
    using C = typename Owner::container_type;
    if constexpr (detail::is_std_array<C>) {
        auto metadata =
            detail::check_metadata<typename Owner::element_type, typename Owner::extents_type>(
                shape);
        if (!metadata)
            return unexpected(metadata.error());
        return detail::tensor_access::make<Owner>(*metadata);
    } else {
        return make_tensor<Owner>(shape, std::allocator_arg, typename C::allocator_type{});
    }
}

template <class Owner, std::integral Integer>
requires(detail::tensor_shape<Owner, Integer> &&
         std::is_move_constructible_v<typename Owner::container_type>)
[[nodiscard]] constexpr shape_result<Owner>
adopt_tensor(const std::array<Integer, Owner::rank()> &shape,
             typename Owner::container_type &&storage) {
    using C = typename Owner::container_type;
    auto metadata =
        detail::check_metadata<typename Owner::element_type, typename Owner::extents_type>(shape);
    if (!metadata)
        return unexpected(metadata.error());
    if (!wjr::in_range<typename C::size_type>(metadata->size))
        return unexpected(shape_error::size_overflow);
    if (metadata->size > storage.max_size())
        return unexpected(shape_error::container_size_limit);
    if (metadata->size != storage.size())
        return unexpected(shape_error::storage_size_mismatch);
    return detail::tensor_access::make<Owner>(*metadata, std::move(storage));
}

} // namespace wjr::np

#endif // WJR_NP_TENSOR_HPP__
