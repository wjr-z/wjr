#ifndef WJR_JSON_PMR_HPP__
#define WJR_JSON_PMR_HPP__

#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <memory_resource>
#include <string>
#include <type_traits>
#include <vector>

#include <wjr/json/document.hpp>

namespace wjr::json::detail {

struct pmr_document_traits;

template <>
/// @brief Borrow only a resource pointer; destructive relocation preserves that identity.
class document_allocator_state<pmr_document_traits> {
public:
    using allocator_type = std::pmr::polymorphic_allocator<std::byte>;

    /// @brief Return the allocator borrowing this value's resource.
    allocator_type get_allocator() const noexcept { return allocator_type(m_resource); }

protected:
    using storage_allocator_type = allocator_type;
    static constexpr bool uses_allocator = true;

    document_allocator_state() noexcept : m_resource(std::pmr::get_default_resource()) {}
    document_allocator_state(std::allocator_arg_t, const allocator_type &allocator) noexcept
        : m_resource(allocator.resource()) {}
    document_allocator_state(const document_allocator_state &) noexcept
        : document_allocator_state() {}
    document_allocator_state(document_allocator_state &&other) noexcept
        : m_resource(other.m_resource) {}

private:
    document_allocator_state &operator=(const document_allocator_state &) = delete;
    std::pmr::memory_resource *m_resource;
};

struct pmr_document_traits {
    using value_type = basic_document<pmr_document_traits>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using string_type = std::pmr::string;
    using object_type = std::pmr::map<string_type, value_type, std::less<>>;
    using array_type = std::pmr::vector<value_type>;
    using size_type = uint32_t;

    template <typename Key>
    struct is_other_key
        : std::conjunction<std::negation<std::is_convertible<Key, size_type>>,
                           std::negation<std::is_same<string_type, remove_cvref_t<Key>>>,
                           has_compare<typename object_type::key_compare, string_type, Key>,
                           has_compare<typename object_type::key_compare, Key, string_type>> {};
};

} // namespace wjr::json::detail

namespace wjr::json {

/// @brief Owning JSON values borrowing an explicit PMR resource.
/// @pre The resource outlives all values, including moved-out subdocuments.
using pmr_document = basic_document<detail::pmr_document_traits>;

} // namespace wjr::json

#endif // WJR_JSON_PMR_HPP__
