#ifndef WJR_JSON_ARRAY_HPP__
#define WJR_JSON_ARRAY_HPP__

#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

#include <wjr/json/document.hpp>

namespace wjr::json {

namespace detail {

template <typename T, std::size_t N>
inline constexpr bool needs_document_value_conversion<std::array<T, N>> = true;

/** @brief Report a projection shape mismatch without instantiating a throw path for each T. */
[[noreturn]] void throw_array_size_mismatch();

} // namespace detail

/** @brief Convert fixed-size arrays with exact shape validation before element conversion. */
template <typename T, std::size_t N>
struct document_serializer_impl<std::array<T, N>> {
    using value_type = std::array<T, N>;

private:
    template <bool Move, typename Array>
    constexpr static decltype(auto) element(Array &array, std::size_t index) {
        if constexpr (Move) {
            return std::move(array[index]);
        } else {
            return (array[index]);
        }
    }

    template <bool Move, typename Array, std::size_t... Index>
    constexpr static value_type construct(Array &array, std::index_sequence<Index...>) {
        return {{value_to<T>(element<Move>(array, Index))...}};
    }

    template <typename Source>
    using serialized_element =
        std::conditional_t<std::is_lvalue_reference_v<Source> ||
                               std::is_const_v<std::remove_reference_t<Source>>,
                           const T &, T &&>;

public:
    template <typename Document>
    requires(is_document_v<remove_cvref_t<Document>> &&
             (N == 0 || has_construct_from_document_v<T, Document &&>))
    constexpr static value_type construct(Document &&doc, in_place_document_serializer_from_t) {
        auto &array = doc.template get<array_t>();
        if (array.size() != N) {
            detail::throw_array_size_mismatch();
        }
        return construct<!std::is_lvalue_reference_v<Document>>(array,
                                                                std::make_index_sequence<N>());
    }

    template <typename Document>
    requires(is_document_v<remove_cvref_t<Document>> &&
             (N == 0 || has_assign_from_document_v<T, Document &&>))
    constexpr static void assign(Document &&doc, value_type &value,
                                 in_place_document_serializer_from_t) {
        auto &array = doc.template get<array_t>();
        if (array.size() != N) {
            detail::throw_array_size_mismatch();
        }
        if constexpr (N != 0) {
            for (std::size_t index = 0; index != N; ++index) {
                element<!std::is_lvalue_reference_v<Document>>(array, index).get_to(value[index]);
            }
        }
    }

    template <typename Document, typename Array>
    requires(std::same_as<remove_cvref_t<Array>, value_type> &&
             (N == 0 || (has_construct_to_document_v<Document, serialized_element<Array &&>> &&
                         has_assign_to_document_v<Document, serialized_element<Array &&>>)))
    constexpr static void assign(Array &&value, Document &doc, in_place_document_serializer_to_t) {
        if constexpr (N == 0) {
            if (doc.is_array()) {
                doc.template get_unsafe<array_t>().clear();
            } else {
                doc.emplace_array();
            }
        } else {
            _document_serializer_array<value_type>::assign(std::forward<Array>(value), doc,
                                                           in_place_document_serializer_to);
        }
    }

    template <typename Document, typename Array>
    requires(std::same_as<remove_cvref_t<Array>, value_type> &&
             (N == 0 || has_construct_to_document_v<Document, serialized_element<Array &&>>))
    constexpr static Document construct(Array &&value, in_place_document_serializer_to_t) {
        if constexpr (N == 0) {
            return Document::_construct_array();
        } else {
            return _document_serializer_array<value_type>::template construct<Document>(
                std::forward<Array>(value), in_place_document_serializer_to);
        }
    }
};

} // namespace wjr::json

#endif // WJR_JSON_ARRAY_HPP__
