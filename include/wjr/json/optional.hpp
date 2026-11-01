#ifndef WJR_JSON_OPTIONAL_HPP__
#define WJR_JSON_OPTIONAL_HPP__

#include <concepts>
#include <cstddef>
#include <optional>
#include <type_traits>
#include <utility>

#include <wjr/json/document.hpp>

namespace wjr::json {

namespace detail {

template <typename T>
inline constexpr bool needs_document_value_conversion<std::optional<T>> = true;

template <typename T, typename Document>
concept optional_document_constructible =
    document_value_constructible<T, Document> &&
    (std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>);

} // namespace detail

/** @brief Map JSON null to disengagement and other values through the contained serializer. */
template <typename T>
struct document_serializer_impl<std::optional<T>> {
    using value_type = std::optional<T>;

    template <typename Document>
    requires detail::optional_document_constructible<T, Document &&>
    constexpr static value_type construct(Document &&doc, in_place_document_serializer_from_t) {
        if (doc.is_null()) {
            return std::nullopt;
        }

        if constexpr (std::is_move_constructible_v<T>) {
            return value_type(std::in_place, value_to<T>(std::forward<Document>(doc)));
        } else {
            const T value = value_to<T>(std::forward<Document>(doc));
            return value_type(std::in_place, value);
        }
    }

    template <typename Document>
    requires(detail::optional_document_constructible<T, Document &&> &&
             has_assign_from_document_v<T, Document &&>)
    constexpr static void assign(Document &&doc, value_type &value,
                                 in_place_document_serializer_from_t) {
        if (doc.is_null()) {
            value.reset();
        } else if (value.has_value()) {
            document_serializer<T>::assign(std::forward<Document>(doc), *value,
                                           in_place_document_serializer_from);
        } else if constexpr (std::is_move_constructible_v<T>) {
            value.emplace(value_to<T>(std::forward<Document>(doc)));
        } else {
            const T converted = value_to<T>(std::forward<Document>(doc));
            value.emplace(converted);
        }
    }

    template <typename Document, typename Optional>
    requires(std::same_as<remove_cvref_t<Optional>, value_type> &&
             has_assign_to_document_v<Document, decltype(*std::declval<Optional>())>)
    constexpr static void assign(Optional &&value, Document &doc,
                                 in_place_document_serializer_to_t) {
        if (value.has_value()) {
            document_serializer<T>::assign(*std::forward<Optional>(value), doc,
                                           in_place_document_serializer_to);
        } else {
            document_serializer<std::nullptr_t>::assign(nullptr, doc,
                                                        in_place_document_serializer_to);
        }
    }

    template <typename Document, typename Optional>
    requires(std::same_as<remove_cvref_t<Optional>, value_type> &&
             has_construct_to_document_v<Document, decltype(*std::declval<Optional>())>)
    constexpr static Document construct(Optional &&value, in_place_document_serializer_to_t) {
        if (value.has_value()) {
            return document_serializer<T>::template construct<Document>(
                *std::forward<Optional>(value), in_place_document_serializer_to);
        }
        return document_serializer<std::nullptr_t>::template construct<Document>(
            nullptr, in_place_document_serializer_to);
    }
};

} // namespace wjr::json

#endif // WJR_JSON_OPTIONAL_HPP__
