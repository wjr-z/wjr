#ifndef WJR_JSON_DOCUMENT_HPP__
#define WJR_JSON_DOCUMENT_HPP__

#include <map>

#include <wjr/json/formatter.hpp>
#include <wjr/json/visitor.hpp>

namespace wjr::json {

template <typename Traits>
class basic_document;

template <typename T>
struct is_document : std::false_type {};

template <typename Traits>
struct is_document<basic_document<Traits>> : std::true_type {};

template <typename T>
inline constexpr bool is_document_v = is_document<T>::value;

namespace detail {

template <template <typename Char, typename Traits, typename Alloc> typename String,
          template <typename Key, typename Ty, typename Pr, typename Alloc>
          typename Object,
          template <typename T, typename Alloc> typename Array>
struct basic_document_traits {
private:
    using document_type = basic_document<basic_document_traits>;

public:
    using string_type = String<char, std::char_traits<char>, memory_pool<char>>;
    using object_type = Object<string_type, document_type, std::less<>,
                               memory_pool<std::pair<const string_type, document_type>>>;
    using array_type = Array<document_type, memory_pool<document_type>>;

    using value_type = document_type;
    using reference = value_type &;
    using const_reference = const value_type &;

    using size_type = uint32_t;

    static_assert(has_compare_is_transparent_v<std::less<>>);

    template <typename Key>
    struct is_other_key
        : std::conjunction<
              std::negation<std::is_convertible<Key, size_type>>,
              std::negation<std::is_same<string_type, remove_cvref_t<Key>>>,
              has_compare<typename object_type::key_compare, string_type, Key>,
              has_compare<typename object_type::key_compare, Key, string_type>> {};
};

using default_document_traits =
    basic_document_traits<std::basic_string, std::map, vector>;
using __default_document_string = typename default_document_traits::string_type;

template <typename Document>
class basic_document_parser;

class check_parser;

} // namespace detail

WJR_INTRINSIC_INLINE result<void> check(const reader &rd) noexcept;

template <typename T>
struct __document_get_impl;

#define WJR_REGISTER_DOCUMENT_GET_IMPL(T)                                                \
    template <>                                                                          \
    struct __document_get_impl<T##_t> {                                                  \
        template <typename Document>                                                     \
        WJR_PURE WJR_INTRINSIC_CONSTEXPR static auto get(Document &&doc) noexcept        \
            -> decltype(std::declval<Document &&>().__get_##T()) {                       \
            return std::forward<Document>(doc).__get_##T();                              \
        }                                                                                \
    }

WJR_REGISTER_DOCUMENT_GET_IMPL(boolean);
WJR_REGISTER_DOCUMENT_GET_IMPL(number_unsigned);
WJR_REGISTER_DOCUMENT_GET_IMPL(number_signed);
WJR_REGISTER_DOCUMENT_GET_IMPL(number_float);
WJR_REGISTER_DOCUMENT_GET_IMPL(string);
WJR_REGISTER_DOCUMENT_GET_IMPL(object);
WJR_REGISTER_DOCUMENT_GET_IMPL(array);

#undef WJR_REGISTER_DOCUMENT_GET_IMPL

class bad_document_access : public std::exception {
public:
    explicit bad_document_access(error_code e) : m_err(std::move(e)) {}

    const error_code &error() const & { return m_err; }
    error_code &error() & { return m_err; }
    const error_code &&error() const && { return std::move(m_err); }
    error_code &&error() && { return std::move(m_err); }

    WJR_NODISCARD virtual const char *what() const noexcept override {
        return "Bad document access";
    }

private:
    error_code m_err;
};

struct in_place_document_serializer_from_t {};
inline constexpr in_place_document_serializer_from_t in_place_document_serializer_from{};

struct in_place_document_serializer_to_t {};
inline constexpr in_place_document_serializer_to_t in_place_document_serializer_to{};

WJR_REGISTER_HAS_TYPE(Mybase_assign_from_document,
                      Mybase::assign(std::declval<Document>(), std::declval<T &>(),
                                     in_place_document_serializer_from),
                      Mybase, T, Document);

WJR_REGISTER_HAS_TYPE(Mybase_construct_from_document,
                      Mybase::construct(std::declval<Document>(),
                                        in_place_document_serializer_from),
                      Mybase, T, Document);

WJR_REGISTER_HAS_TYPE(Mybase_assign_to_document,
                      Mybase::assign(std::declval<T>(), std::declval<Document &>(),
                                     in_place_document_serializer_to),
                      Mybase, Document, T);

WJR_REGISTER_HAS_TYPE(Mybase_construct_to_document,
                      Mybase::template construct<Document>(
                          std::declval<T>(), in_place_document_serializer_to),
                      Mybase, Document, T);

struct in_place_document_serializer_object_copy_t {};
inline constexpr in_place_document_serializer_object_copy_t
    in_place_document_serializer_object_copy{};

struct in_place_document_serializer_object_move_t {};
inline constexpr in_place_document_serializer_object_move_t
    in_place_document_serializer_object_move{};

template <typename Mybase>
class document_serializer_auto_completion : private Mybase {
public:
    using value_type = typename Mybase::value_type;

    /// from_document

    /// Use functions of Mybase.

    template <typename Document, WJR_REQUIRES(has_Mybase_assign_from_document_v<
                                              Mybase, value_type, Document &&>)>
    constexpr static void assign(Document &&doc, value_type &val,
                                 in_place_document_serializer_from_t) {
        Mybase::assign(std::forward<Document>(doc), val,
                       in_place_document_serializer_from);
    }

    template <typename Document, WJR_REQUIRES(has_Mybase_construct_from_document_v<
                                              Mybase, value_type, Document &&>)>
    constexpr static value_type construct(Document &&doc,
                                          in_place_document_serializer_from_t) {
        return Mybase::construct(std::forward<Document>(doc),
                                 in_place_document_serializer_from);
    }

    /// Use member functions of value_type

    template <typename Document,
              WJR_REQUIRES(
                  !has_Mybase_assign_from_document_v<Mybase, value_type, Document &&> &&
                  std::is_assignable_v<value_type &, Document &&>)>
    constexpr static void assign(Document &&doc, value_type &val,
                                 in_place_document_serializer_from_t) {
        val = std::forward<Document>(doc);
    }

    template <typename Document,
              WJR_REQUIRES(!has_Mybase_construct_from_document_v<Mybase, value_type,
                                                                 Document &&> &&
                           std::is_constructible_v<value_type, Document &&,
                                                   in_place_document_serializer_from_t>)>
    constexpr static value_type construct(Document &&doc,
                                          in_place_document_serializer_from_t) {
        return value_type(std::forward<Document>(doc), in_place_document_serializer_from);
    }

    /// to_document

    /// Use functions of Mybase.

    template <typename Document, typename U = value_type,
              WJR_REQUIRES(has_Mybase_assign_to_document_v<Mybase, Document, U &&>)>
    constexpr static void assign(U &&val, Document &doc,
                                 in_place_document_serializer_to_t) {
        Mybase::assign(std::forward<U>(val), doc, in_place_document_serializer_to);
    }

    template <typename Document, typename U = value_type,
              WJR_REQUIRES(has_Mybase_construct_to_document_v<Mybase, Document, U &&>)>
    constexpr static Document construct(U &&val, in_place_document_serializer_to_t) {
        return Mybase::template construct<Document>(std::forward<U>(val),
                                                    in_place_document_serializer_to);
    }

    /// Use static member functions of value_type

    template <typename Document, typename U = value_type,
              WJR_REQUIRES(!has_Mybase_assign_to_document_v<Mybase, Document, U &&> &&
                           has_Mybase_assign_to_document_v<value_type, Document, U &&>)>
    constexpr static void assign(U &&val, Document &doc,
                                 in_place_document_serializer_to_t) {
        value_type::assign(std::forward<U>(val), doc, in_place_document_serializer_to);
    }

    template <
        typename Document, typename U = value_type,
        WJR_REQUIRES(!has_Mybase_construct_to_document_v<Mybase, Document, U &&> &&
                     has_Mybase_construct_to_document_v<value_type, Document, U &&>)>
    constexpr static Document construct(U &&val, in_place_document_serializer_to_t) {
        return value_type::template construct<Document>(std::forward<U>(val),
                                                        in_place_document_serializer_to);
    }
};

template <typename T, typename Enable = void>
struct document_serializer_impl {
    using value_type = T;
};

template <typename T>
using document_serializer =
    document_serializer_auto_completion<document_serializer_impl<remove_cvref_t<T>>>;

template <typename Arithmetic>
struct __document_serializer_arithmetic;

template <typename T>
struct __document_serializer_array;

template <typename T>
struct __document_serializer_object;

WJR_REGISTER_HAS_TYPE(assign_from_document,
                      document_serializer<T>::assign(std::declval<Document>(),
                                                     std::declval<T &>(),
                                                     in_place_document_serializer_from),
                      T, Document);

WJR_REGISTER_HAS_TYPE(construct_from_document,
                      document_serializer<T>::construct(
                          std::declval<Document>(), in_place_document_serializer_from),
                      T, Document);

WJR_REGISTER_HAS_TYPE(assign_to_document,
                      document_serializer<T>::assign(std::declval<T>(),
                                                     std::declval<Document &>(),
                                                     in_place_document_serializer_to),
                      Document, T);
WJR_REGISTER_HAS_TYPE(construct_to_document,
                      document_serializer<T>::template construct<Document>(
                          std::declval<T>(), in_place_document_serializer_to),
                      Document, T);

// from_document

#define __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_COPY_CONSTRUCTOR_ENABLER_CALLER(  \
    var)                                                                                 \
    ::wjr::json::has_construct_from_document<                                            \
        decltype(TT::var), const ::wjr::json::basic_document<Traits> &>

#define __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_COPY_CONSTRUCTOR_CALLER(var)      \
    var(__wjr_obj.at(#var))

#define __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_MOVE_CONSTRUCTOR_ENABLER_CALLER(  \
    var)                                                                                 \
    ::wjr::json::has_construct_from_document<decltype(TT::var),                          \
                                             ::wjr::json::basic_document<Traits> &&>

#define __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_MOVE_CONSTRUCTOR_CALLER(var)      \
    var(std::move(__wjr_obj.at(#var)))

#define __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_COPY_ASSIGN_ENABLER_CALLER(var)   \
    ::wjr::json::has_assign_from_document<decltype(TT::var),                             \
                                          const ::wjr::json::basic_document<Traits> &>

#define __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_COPY_ASSIGN_CALLER(var)           \
    __wjr_obj.at(#var).get_to(var)

#define __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_MOVE_ASSIGN_ENABLER_CALLER(var)   \
    ::wjr::json::has_assign_from_document<decltype(TT::var),                             \
                                          ::wjr::json::basic_document<Traits> &&>

#define __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_MOVE_ASSIGN_CALLER(var)           \
    std::move(__wjr_obj.at(#var)).get_to(var)

#define WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER(Type, ...)                                     \
private:                                                                                            \
    template <typename Object>                                                                      \
    explicit Type(const Object &__wjr_obj,                                                          \
                  ::wjr::json::in_place_document_serializer_object_copy_t)                          \
        : WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                                               \
              (__VA_ARGS__),                                                                        \
              __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_COPY_CONSTRUCTOR_CALLER)) {            \
    }                                                                                               \
    template <typename Object>                                                                      \
    explicit Type(Object &&__wjr_obj,                                                               \
                  ::wjr::json::in_place_document_serializer_object_move_t)                          \
        : WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                                               \
              (__VA_ARGS__),                                                                        \
              __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_MOVE_CONSTRUCTOR_CALLER)) {            \
    }                                                                                               \
                                                                                                    \
public:                                                                                             \
    template <                                                                                      \
        typename Traits, typename TT = Type,                                                        \
        WJR_REQUIRES(                                                                               \
            std::conjunction_v<WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                          \
                (__VA_ARGS__),                                                                      \
                __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_COPY_CONSTRUCTOR_ENABLER_CALLER))>)> \
    explicit Type(const ::wjr::json::basic_document<Traits> &__wjr_document,                        \
                  ::wjr::json::in_place_document_serializer_from_t)                                 \
        : Type(__wjr_document.template get<::wjr::json::object_t>(),                                \
               ::wjr::json::in_place_document_serializer_object_copy) {}                            \
    template <                                                                                      \
        typename Traits, typename TT = Type,                                                        \
        WJR_REQUIRES(                                                                               \
            std::conjunction_v<WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                          \
                (__VA_ARGS__),                                                                      \
                __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_MOVE_CONSTRUCTOR_ENABLER_CALLER))>)> \
    explicit Type(::wjr::json::basic_document<Traits> &&__wjr_document,                             \
                  ::wjr::json::in_place_document_serializer_from_t)                                 \
        : Type(std::move(__wjr_document.template get<::wjr::json::object_t>()),                     \
               ::wjr::json::in_place_document_serializer_object_move) {}                            \
                                                                                                    \
private:                                                                                            \
    template <typename Object>                                                                      \
    void __assign(const Object &__wjr_obj,                                                          \
                  ::wjr::json::in_place_document_serializer_object_copy_t) {                        \
        WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                                                 \
            (__VA_ARGS__),                                                                          \
            __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_COPY_ASSIGN_CALLER));                    \
    }                                                                                               \
    template <typename Object>                                                                      \
    void __assign(Object &&__wjr_obj,                                                               \
                  ::wjr::json::in_place_document_serializer_object_move_t) {                        \
        WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                                                 \
            (__VA_ARGS__),                                                                          \
            __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_MOVE_ASSIGN_CALLER));                    \
    }                                                                                               \
                                                                                                    \
public:                                                                                             \
    template <                                                                                      \
        typename Traits, typename TT = Type,                                                        \
        WJR_REQUIRES(                                                                               \
            std::conjunction_v<WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                          \
                (__VA_ARGS__),                                                                      \
                __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_COPY_ASSIGN_ENABLER_CALLER))>)>      \
    Type &operator=(const ::wjr::json::basic_document<Traits> &__wjr_document) {                    \
        __assign(__wjr_document.template get<::wjr::json::object_t>(),                              \
                 ::wjr::json::in_place_document_serializer_object_copy);                            \
        return *this;                                                                               \
    }                                                                                               \
    template <                                                                                      \
        typename Traits, typename TT = Type,                                                        \
        WJR_REQUIRES(                                                                               \
            std::conjunction_v<WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                          \
                (__VA_ARGS__),                                                                      \
                __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_MOVE_ASSIGN_ENABLER_CALLER))>)>      \
    Type &operator=(::wjr::json::basic_document<Traits> &&__wjr_document) {                         \
        __assign(std::move(__wjr_document.template get<::wjr::json::object_t>()),                   \
                 ::wjr::json::in_place_document_serializer_object_move);                            \
        return *this;                                                                               \
    }

// to_document

#define __WJR_REGISTER_TO_DOCUMENT_OBJECT_SERIALIZER_COPY_ENABLER_CALLER(var)            \
    std::is_constructible<decltype(TT::var), const Document &>

#define __WJR_REGISTER_TO_DOCUMENT_OBJECT_SERIALIZER_COPY_CALLER(var)                    \
    __wjr_obj.emplace(#var, __wjr_tp.var)

#define __WJR_REGISTER_TO_DOCUMENT_OBJECT_SERIALIZER_MOVE_ENABLER_CALLER(var)            \
    std::is_constructible<decltype(TT::var), Document &&>

#define __WJR_REGISTER_TO_DOCUMENT_OBJECT_SERIALIZER_MOVE_CALLER(var)                    \
    __wjr_obj.emplace(#var, std::move(__wjr_tp.var))

#define WJR_REGISTER_TO_DOCUMENT_OBJECT_SERIALIZER(Type, ...)                                       \
private:                                                                                            \
    template <typename Document>                                                                    \
    static void assign_object(const Type &__wjr_tp, Document &__wjr_document,                       \
                              ::wjr::json::in_place_document_serializer_object_copy_t) {            \
        auto &__wjr_obj = __wjr_document.template get_unsafe<::wjr::json::object_t>();              \
        WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                                                 \
            (__VA_ARGS__), __WJR_REGISTER_TO_DOCUMENT_OBJECT_SERIALIZER_COPY_CALLER));              \
    }                                                                                               \
    template <typename Document>                                                                    \
    static void assign_object(Type &&__wjr_tp, Document &__wjr_document,                            \
                              ::wjr::json::in_place_document_serializer_object_move_t) {            \
        auto &__wjr_obj = __wjr_document.template get_unsafe<::wjr::json::object_t>();              \
        WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                                                 \
            (__VA_ARGS__), __WJR_REGISTER_TO_DOCUMENT_OBJECT_SERIALIZER_MOVE_CALLER));              \
    }                                                                                               \
                                                                                                    \
public:                                                                                             \
    template <                                                                                      \
        typename Document, typename TT = Type,                                                      \
        WJR_REQUIRES(                                                                               \
            std::conjunction_v<WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                          \
                (__VA_ARGS__),                                                                      \
                __WJR_REGISTER_TO_DOCUMENT_OBJECT_SERIALIZER_COPY_ENABLER_CALLER))>)>               \
    static Document construct(const Type &__wjr_tp,                                                 \
                              ::wjr::json::in_place_document_serializer_to_t) {                     \
        using namespace wjr::json;                                                                  \
        using object_type = typename Document::object_type;                                         \
        Document __wjr_document(object_t(), __document_create<object_type>());                      \
        assign_object(__wjr_tp, __wjr_document,                                                     \
                      ::wjr::json::in_place_document_serializer_object_copy);                       \
        return __wjr_document;                                                                      \
    }                                                                                               \
    template <                                                                                      \
        typename Document, typename TT = Type,                                                      \
        WJR_REQUIRES(                                                                               \
            std::conjunction_v<WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                          \
                (__VA_ARGS__),                                                                      \
                __WJR_REGISTER_TO_DOCUMENT_OBJECT_SERIALIZER_MOVE_ENABLER_CALLER))>)>               \
    static Document construct(Type &&__wjr_tp,                                                      \
                              ::wjr::json::in_place_document_serializer_to_t) {                     \
        using namespace wjr::json;                                                                  \
        using object_type = typename Document::object_type;                                         \
        Document __wjr_document(object_t(), __document_create<object_type>());                      \
        assign_object(std::move(__wjr_tp), __wjr_document,                                          \
                      ::wjr::json::in_place_document_serializer_object_move);                       \
        return __wjr_document;                                                                      \
    }                                                                                               \
    template <                                                                                      \
        typename Traits, typename TT = Type,                                                        \
        WJR_REQUIRES(                                                                               \
            std::conjunction_v<WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                          \
                (__VA_ARGS__),                                                                      \
                __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_COPY_CONSTRUCTOR_ENABLER_CALLER))>)> \
    static void assign(const Type &__wjr_tp,                                                        \
                       ::wjr::json::basic_document<Traits> &__wjr_document,                         \
                       ::wjr::json::in_place_document_serializer_to_t) {                            \
        using namespace wjr::json;                                                                  \
        if (__wjr_document.type() != value_t::object) {                                             \
            __wjr_document.emplace_object();                                                        \
        }                                                                                           \
        assign_object(__wjr_tp, __wjr_document,                                                     \
                      in_place_document_serializer_object_copy);                                    \
    }                                                                                               \
    template <                                                                                      \
        typename Traits, typename TT = Type,                                                        \
        WJR_REQUIRES(                                                                               \
            std::conjunction_v<WJR_PP_QUEUE_EXPAND(WJR_PP_QUEUE_TRANSFORM(                          \
                (__VA_ARGS__),                                                                      \
                __WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER_MOVE_CONSTRUCTOR_ENABLER_CALLER))>)> \
    static void assign(Type &&__wjr_tp,                                                             \
                       ::wjr::json::basic_document<Traits> &__wjr_document,                         \
                       ::wjr::json::in_place_document_serializer_to_t) {                            \
        using namespace wjr::json;                                                                  \
        if (__wjr_document.type() != value_t::object) {                                             \
            __wjr_document.emplace_object();                                                        \
        }                                                                                           \
        assign_object(std::move(__wjr_tp), __wjr_document,                                          \
                      in_place_document_serializer_object_move);                                    \
    }

#define WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(Type, ...)                               \
    WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER(Type, __VA_ARGS__)                      \
    WJR_REGISTER_TO_DOCUMENT_OBJECT_SERIALIZER(Type, __VA_ARGS__)

template <typename T, typename... Args>
T *__document_create(Args &&...args) noexcept(
    noexcept(std::declval<memory_pool<T>>().allocate(1)) &&
    std::is_nothrow_constructible_v<T, Args &&...>) {
    memory_pool<T> al;
    auto *const ptr = al.allocate(1);
    wjr::construct_at(ptr, std::forward<Args>(args)...);
    return ptr;
}

template <typename T>
void __document_destroy(T *ptr) noexcept(std::is_nothrow_destructible_v<T> && noexcept(
    std::declval<memory_pool<T>>().deallocate(std::declval<T *>(), 1))) {
    std::destroy_at(ptr);
    memory_pool<T> al;
    al.deallocate(ptr, 1);
}

namespace detail {
template <typename Formatter, typename Traits>
void format_impl(Formatter fmt, const basic_document<Traits> &doc,
                 unsigned int depth) noexcept;
}

template <typename Formatter, typename Traits>
void format(Formatter fmt, const basic_document<Traits> &doc) noexcept {
    detail::format_impl(fmt, doc, 0);
}

/**
 * @details At present, it's a simple but flexible implementation solution. This is not as
 * good as a only iteration in terms of performance. \n
 * Performance (use std::map and wjr::vector as container on my local x64 2.50 GHz
 * machine) : \n twitter.document : 700~720 MB/s \n canada.document : 660~680 MB/s
 * @todo 1. For most strings, only need to check first character to compare.
 * 2. Maybe use hash like std::unordered_map, but for long strings, compare even faster
 * than hash.
 * 3. Use B plus tree ?
 * 4. In place construct without using low performance basic_document.
 *
 */
template <typename Traits>
class basic_document {
    friend class detail::basic_document_parser<basic_document>;

    template <typename T>
    friend struct __document_get_impl;

    template <typename T, typename Enable>
    friend struct document_serializer_impl;

    template <typename Arithmetic>
    friend struct __document_serializer_arithmetic;

    template <typename T>
    friend struct __document_serializer_array;

    template <typename T>
    friend struct __document_serializer_object;

public:
    using traits_type = Traits;

    using null_type = std::nullptr_t;
    using boolean_type = bool;
    using number_unsigned_type = uint64_t;
    using number_signed_type = int64_t;
    using number_float_type = double;
    using string_type = typename traits_type::string_type;
    using object_type = typename traits_type::object_type;
    using array_type = typename traits_type::array_type;

    using value_type = typename traits_type::value_type;
    using reference = typename traits_type::reference;
    using const_reference = typename traits_type::const_reference;

    using size_type = typename traits_type::size_type;

    basic_document() = default;
    basic_document(const basic_document &other) {
        switch (other.type()) {
        case value_t::null:
        case value_t::number_unsigned:
        case value_t::number_signed:
        case value_t::number_float: {
            // use a simple memcpy
            m_value = other.m_value;
            break;
        }
        case value_t::string: {
            __emplace_string(other.__get_string());
            break;
        }
        case value_t::object: {
            __emplace_object(other.__get_object());
            break;
        }
        case value_t::array: {
            __emplace_array(other.__get_array());
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }
    }

    basic_document(basic_document &&other) noexcept : m_value(other.m_value) {
        other.m_value.m_type = value_t::null;
    }

    basic_document &operator=(const basic_document &other) {
        if (WJR_UNLIKELY(this == std::addressof(other))) {
            return *this;
        }

        switch (type()) {
        case value_t::null:
        case value_t::boolean:
        case value_t::number_unsigned:
        case value_t::number_signed:
        case value_t::number_float: {
            switch (other.type()) {
            case value_t::null:
            case value_t::boolean:
            case value_t::number_unsigned:
            case value_t::number_signed:
            case value_t::number_float: {
                m_value = other.m_value;
                break;
            }
            case value_t::string: {
                __emplace_string(other.__get_string());
                break;
            }
            case value_t::object: {
                __emplace_object(other.__get_object());
                break;
            }
            case value_t::array: {
                __emplace_array(other.__get_array());
                break;
            }
            default: {
                WJR_UNREACHABLE();
                break;
            }
            }
            break;
        }
        case value_t::string: {
            switch (other.type()) {
            case value_t::null:
            case value_t::boolean:
            case value_t::number_unsigned:
            case value_t::number_signed:
            case value_t::number_float: {
                __document_destroy(std::addressof(__get_string()));
                m_value = other.m_value;
                break;
            }
            case value_t::string: {
                __get_string() = other.__get_string();
                break;
            }
            case value_t::object: {
                __document_destroy(std::addressof(__get_string()));
                __emplace_object(other.__get_object());
                break;
            }
            case value_t::array: {
                __document_destroy(std::addressof(__get_string()));
                __emplace_array(other.__get_array());
                break;
            }
            default: {
                WJR_UNREACHABLE();
                break;
            }
            }
            break;
        }
        case value_t::object: {
            switch (other.type()) {
            case value_t::null:
            case value_t::boolean:
            case value_t::number_unsigned:
            case value_t::number_signed:
            case value_t::number_float: {
                __document_destroy(std::addressof(__get_object()));
                m_value = other.m_value;
                break;
            }
            case value_t::string: {
                __document_destroy(std::addressof(__get_object()));
                __emplace_string(other.__get_string());
                break;
            }
            case value_t::object: {
                __get_object() = other.__get_object();
                break;
            }
            case value_t::array: {
                __document_destroy(std::addressof(__get_object()));
                __emplace_array(other.__get_array());
                break;
            }
            default: {
                WJR_UNREACHABLE();
                break;
            }
            }
            break;
        }
        case value_t::array: {
            switch (other.type()) {
            case value_t::null:
            case value_t::boolean:
            case value_t::number_unsigned:
            case value_t::number_signed:
            case value_t::number_float: {
                __document_destroy(std::addressof(__get_array()));
                m_value = other.m_value;
                break;
            }
            case value_t::string: {
                __document_destroy(std::addressof(__get_array()));
                __emplace_string(other.__get_string());
                break;
            }
            case value_t::object: {
                __document_destroy(std::addressof(__get_array()));
                __emplace_object(other.__get_object());
                break;
            }
            case value_t::array: {
                __get_array() = other.__get_array();
                break;
            }
            default: {
                WJR_UNREACHABLE();
                break;
            }
            }
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        return *this;
    }

    basic_document &operator=(basic_document &&other) noexcept {
        if (WJR_UNLIKELY(this == std::addressof(other))) {
            return *this;
        }

        __destroy();
        m_value = other.m_value;
        other.m_value.m_type = value_t::null;
        return *this;
    }

    /**
     * @brief Destroy the basic document object
     * @todo Optimize by not using recursion
     *
     */
    ~basic_document() noexcept { __destroy(); }

    explicit basic_document(null_t) noexcept : m_value(null_t()) {}
    basic_document(boolean_t, bool f) noexcept : m_value(boolean_t(), f) {}
    basic_document(number_signed_t, int64_t value) noexcept
        : m_value(number_unsigned_t(), value) {}
    basic_document(number_unsigned_t, uint64_t value) noexcept
        : m_value(number_signed_t(), value) {}
    basic_document(number_float_t, double value) noexcept
        : m_value(number_float_t(), value) {}

    basic_document(string_t, string_type *ptr) noexcept : m_value(string_t(), ptr) {}
    basic_document(object_t, object_type *ptr) noexcept : m_value(object_t(), ptr) {}
    basic_document(array_t, array_type *ptr) noexcept : m_value(array_t(), ptr) {}

    explicit basic_document(basic_value value) noexcept : m_value(value) {}
    explicit basic_document(default_construct_t) noexcept : basic_document() {}

    value_t type() const noexcept { return m_value.m_type; }

    template <typename T>
    decltype(auto) get_unsafe() noexcept {
        return __document_get_impl<T>::get(*this);
    }

    template <typename T>
    decltype(auto) get_unsafe() const noexcept {
        return __document_get_impl<T>::get(*this);
    }

    template <typename T>
    decltype(auto) get() {
        if (WJR_UNLIKELY(type() != T::value)) {
            WJR_THROW(bad_document_access(error_code::INCORRECT_TYPE));
        }

        return get_unsafe<T>();
    }

    template <typename T>
    decltype(auto) get() const {
        if (WJR_UNLIKELY(type() != T::value)) {
            WJR_THROW(bad_document_access(error_code::INCORRECT_TYPE));
        }

        return get_unsafe<T>();
    }

    WJR_CONST static size_type max_depth_size() noexcept { return 256; }

    static result<basic_document> parse(const reader &rd) noexcept;

    template <typename Container>
    void dump_impl(Container &cont, unsigned indents = -1) const noexcept {
        static_assert(std::is_same_v<typename Container::value_type, char>);

        if (indents == -1u) {
            format(minify_formatter(std::back_inserter(cont)), *this);
        } else {
            format(pretty_formatter(std::back_inserter(cont), indents), *this);
        }
    }

    template <typename _Traits = std::char_traits<char>,
              typename Alloc = std::allocator<char>>
    std::basic_string<char, _Traits, Alloc> dump(unsigned indents = -1) const noexcept {
        std::basic_string<char, _Traits, Alloc> str;
        dump_impl(str, indents);
        return str;
    }

    /**
     * @brief Use minify formatter as default.
     */
    template <typename _Traits = std::char_traits<char>,
              typename Alloc = std::allocator<char>>
    std::basic_string<char, _Traits, Alloc> to_string() const noexcept {
        return dump<_Traits, Alloc>();
    }

    reference at(size_type idx) { return get<array_t>().at(idx); }
    const_reference at(size_type idx) const { return get<array_t>().at(idx); }

    reference at(const typename object_type::key_type &key) {
        return get<object_t>().at(key);
    }

    const_reference at(const typename object_type::key_type &key) const {
        return get<object_t>().at(key);
    }

    template <typename Key, WJR_REQUIRES(traits_type::template is_other_key<Key>::value)>
    reference at(const Key &key) {
        auto &obj = get<object_t>();
        const auto iter = obj.find(key);

        if (iter == obj.end()) {
            WJR_THROW(std::out_of_range("basic_json::at"));
        }

        return iter->second;
    }

    template <typename Key, WJR_REQUIRES(traits_type::template is_other_key<Key>::value)>
    const_reference at(const Key &key) const {
        const auto &obj = get<object_t>();
        const auto iter = obj.find(key);

        if (iter == obj.end()) {
            WJR_THROW(std::out_of_range("basic_json::at"));
        }

        return iter->second;
    }

    reference operator[](size_type idx) { return get<array_t>()[idx]; }
    const_reference operator[](size_type idx) const { return get<array_t>()[idx]; }

    reference operator[](const typename object_type::key_type &key) {
        return get<object_t>()[key];
    }

    reference operator[](typename object_type::key_type &&key) {
        return get<object_t>()[std::move(key)];
    }

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           !has_construct_to_document_v<basic_document, T &&>)>
    explicit basic_document(T &&) = delete;

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           has_construct_to_document_v<basic_document, T &&>)>
    explicit basic_document(T &&val)
        : basic_document(document_serializer<T>::template construct<basic_document>(
              std::forward<T>(val), in_place_document_serializer_to)) {}

    template <typename T, WJR_REQUIRES(!has_assign_to_document_v<basic_document, T &&>)>
    basic_document &operator=(T &&) = delete;

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           has_assign_to_document_v<basic_document, T &&>)>
    basic_document &operator=(T &&val) {
        document_serializer<T>::assign(std::forward<T>(val), *this,
                                       in_place_document_serializer_to);
        return *this;
    }

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           !has_construct_from_document_v<T, const basic_document &>)>
    explicit operator T() & = delete;

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           !has_construct_from_document_v<T, basic_document &&>)>
    explicit operator T() const & = delete;

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           !has_construct_from_document_v<T, basic_document &&>)>
    explicit operator T() && = delete;

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           !has_construct_from_document_v<T, basic_document &&>)>
    explicit operator T() const && = delete;

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           has_construct_from_document_v<T, const basic_document &>)>
    explicit operator T() & {
        return document_serializer<T>::construct(*this,
                                                 in_place_document_serializer_from);
    }

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           has_construct_from_document_v<T, const basic_document &>)>
    explicit operator T() const & {
        return document_serializer<T>::construct(*this,
                                                 in_place_document_serializer_from);
    }

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           has_construct_from_document_v<T, basic_document &&>)>
    explicit operator T() && {
        return document_serializer<T>::construct(std::move(*this),
                                                 in_place_document_serializer_from);
    }

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           has_construct_from_document_v<T, basic_document &&>)>
    explicit operator T() const && {
        return document_serializer<T>::construct(std::move(*this),
                                                 in_place_document_serializer_from);
    }

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           !has_assign_from_document_v<T, const basic_document &>)>
    void get_to(T &) & = delete;

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           !has_assign_from_document_v<T, const basic_document &>)>
    void get_to(T &) const & = delete;

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           !has_assign_from_document_v<T, basic_document &&>)>
    void get_to(T &) && = delete;

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           !has_assign_from_document_v<T, basic_document &&>)>
    void get_to(T &) const && = delete;

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           has_assign_from_document_v<T, const basic_document &>)>
    void get_to(T &val) & {
        document_serializer<T>::assign(*this, val, in_place_document_serializer_from);
    }

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           has_assign_from_document_v<T, const basic_document &>)>
    void get_to(T &val) const & {
        document_serializer<T>::assign(*this, val, in_place_document_serializer_from);
    }

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           has_assign_from_document_v<T, basic_document &&>)>
    void get_to(T &val) && {
        document_serializer<T>::assign(std::move(*this), val,
                                       in_place_document_serializer_from);
    }

    template <typename T,
              WJR_REQUIRES(!std::is_same_v<remove_cvref_t<T>, basic_document> &&
                           has_assign_from_document_v<T, basic_document &&>)>
    void get_to(T &val) const && {
        document_serializer<T>::assign(std::move(*this), val,
                                       in_place_document_serializer_from);
    }

    void get_to(basic_document &val) & { val = *this; }
    void get_to(basic_document &val) const & { val = *this; }
    void get_to(basic_document &val) && { val = std::move(*this); }
    void get_to(basic_document &val) const && { val = std::move(*this); }

    void reset() noexcept {
        __destroy();
        m_value.set(null_t());
    }

    void swap(basic_document &other) noexcept { std::swap(m_value, other.m_value); }

    bool is_null() const noexcept { return type() == value_t::null; }
    bool is_boolean() const noexcept { return type() == value_t::boolean; }
    bool is_number_unsigned() const noexcept {
        return type() == value_t::number_unsigned;
    }
    bool is_number_signed() const noexcept { return type() == value_t::number_signed; }
    bool is_number_float() const noexcept { return type() == value_t::number_float; }

    WJR_PURE bool is_number() const noexcept {
        switch (type()) {
        case value_t::number_unsigned:
        case value_t::number_signed:
        case value_t::number_float: {
            return true;
        }
        default: {
            return false;
        }
        }
    }

    bool is_string() const noexcept { return type() == value_t::string; }
    bool is_object() const noexcept { return type() == value_t::object; }
    bool is_array() const noexcept { return type() == value_t::array; }

    void __emplace_null() noexcept { m_value.set(null_t()); }
    void __emplace_boolean(bool value) noexcept { m_value.set(boolean_t(), value); }
    void __emplace_number_unsigned(bool value) noexcept {
        m_value.set(number_unsigned_t(), value);
    }
    void __emplace_number_signed(bool value) noexcept {
        m_value.set(number_signed_t(), value);
    }
    void __emplace_number_float(bool value) noexcept {
        m_value.set(number_float_t(), value);
    }

    template <typename... Args>
    void __emplace_string(Args &&...args) {
        m_value.set(string_t(),
                    __document_create<string_type>(std::forward<Args>(args)...));
    }

    template <typename... Args>
    void __emplace_object(Args &&...args) {
        m_value.set(object_t(),
                    __document_create<object_type>(std::forward<Args>(args)...));
    }

    template <typename... Args>
    void __emplace_array(Args &&...args) {
        m_value.set(array_t(),
                    __document_create<array_type>(std::forward<Args>(args)...));
    }

    void emplace_null() noexcept {
        reset();
        __emplace_null();
    }

    void emplace_boolean() noexcept {
        reset();
        __emplace_boolean();
    }

    void emplace_number_unsigned() noexcept {
        reset();
        __emplace_number_unsigned();
    }

    void emplace_number_signed() noexcept {
        reset();
        __emplace_number_signed();
    }

    void emplace_number_float() noexcept {
        reset();
        __emplace_number_float();
    }

    void emplace_string() {
        reset();
        __emplace_string();
    }

    void emplace_object() {
        reset();
        __emplace_object();
    }

    void emplace_array() {
        reset();
        __emplace_array();
    }

    basic_document(const basic_document &other, in_place_document_serializer_from_t)
        : basic_document(other) {}

    basic_document(basic_document &&other, in_place_document_serializer_from_t) noexcept
        : basic_document(std::move(other)) {}

    static void assign(const basic_document &other, basic_document &doc,
                       in_place_document_serializer_to_t) {
        doc = other;
    }
    static void assign(basic_document &&other, basic_document &doc,
                       in_place_document_serializer_to_t) noexcept {
        doc = std::move(other);
    }

    template <typename Document, WJR_REQUIRES(std::is_same_v<Document, basic_document>)>
    static Document construct(const basic_document &val,
                              in_place_document_serializer_to_t) {
        return val;
    }

    template <typename Document, WJR_REQUIRES(std::is_same_v<Document, basic_document>)>
    static Document construct(basic_document &&val, in_place_document_serializer_to_t) {
        return std::move(val);
    }

private:
    void __destroy() noexcept {
        if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P(type())) {
            switch (type()) {
            case value_t::null:
            case value_t::boolean:
            case value_t::number_unsigned:
            case value_t::number_signed:
            case value_t::number_float: {
                return;
            }
            default: {
                break;
            }
            }
        }

        __destroy_impl();
    }

    void __destroy_impl() noexcept {
        switch (type()) {
        case value_t::null:
        case value_t::boolean:
        case value_t::number_unsigned:
        case value_t::number_signed:
        case value_t::number_float: {
            break;
        }
        case value_t::string: {
            __document_destroy(std::addressof(__get_string()));
            break;
        }
        case value_t::object: {
            __document_destroy(std::addressof(__get_object()));
            break;
        }
        case value_t::array: {
            __document_destroy(std::addressof(__get_array()));
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }
    }

    boolean_type &__get_boolean() noexcept { return m_value.m_boolean; }
    const boolean_type &__get_boolean() const noexcept { return m_value.m_boolean; }

    number_unsigned_type &__get_number_unsigned() noexcept {
        return m_value.m_number_unsigned;
    }
    const number_unsigned_type &__get_number_unsigned() const noexcept {
        return m_value.m_number_unsigned;
    }

    number_signed_type &__get_number_signed() noexcept { return m_value.m_number_signed; }
    const number_signed_type &__get_number_signed() const noexcept {
        return m_value.m_number_signed;
    }

    number_float_type &__get_number_float() noexcept { return m_value.m_number_float; }
    const number_float_type &__get_number_float() const noexcept {
        return m_value.m_number_float;
    }

    string_type &__get_string() noexcept {
        return *static_cast<string_type *>(m_value.m_ptr);
    }
    const string_type &__get_string() const noexcept {
        return *static_cast<const string_type *>(m_value.m_ptr);
    }

    object_type &__get_object() noexcept {
        return *static_cast<object_type *>(m_value.m_ptr);
    }
    const object_type &__get_object() const noexcept {
        return *static_cast<const object_type *>(m_value.m_ptr);
    }

    array_type &__get_array() noexcept {
        return *static_cast<array_type *>(m_value.m_ptr);
    }
    const array_type &__get_array() const noexcept {
        return *static_cast<const array_type *>(m_value.m_ptr);
    }

    basic_value m_value;
};

template <>
struct document_serializer_impl<std::nullptr_t, void> {
    using value_type = std::nullptr_t;

    /// from_document

    template <typename Document>
    constexpr static void assign(const Document &doc, value_type &val,
                                 in_place_document_serializer_from_t) {
        if (WJR_UNLIKELY(!doc.is_null())) {
            WJR_THROW(bad_document_access(error_code::TAPE_ERROR));
        }

        val = nullptr;
    }

    template <typename Document>
    WJR_PURE static value_type construct(const Document &doc,
                                         in_place_document_serializer_from_t) {
        value_type val;
        assign(doc, val, in_place_document_serializer_from);
        return val;
    }

    /// to_document

    template <typename Document>
    constexpr static void assign(const value_type &, Document &doc,
                                 in_place_document_serializer_to_t) {
        doc.reset();
        doc.m_value.set(null_t());
    }

    template <typename Document>
    constexpr static Document construct(const value_type &,
                                        in_place_document_serializer_to_t) {
        return Document(null_t());
    }
};

template <>
struct document_serializer_impl<bool, void> {
    using value_type = bool;

    /// from_document

    template <typename Document>
    constexpr static void assign(const Document &doc, value_type &val,
                                 in_place_document_serializer_from_t) {
        val = doc.template get<boolean_t>();
    }

    template <typename Document>
    WJR_PURE static value_type construct(const Document &doc,
                                         in_place_document_serializer_from_t) {
        value_type val;
        assign(doc, val, in_place_document_serializer_from);
        return val;
    }

    /// to_document

    template <typename Document>
    constexpr static void assign(const value_type &val, Document &doc,
                                 in_place_document_serializer_to_t) {
        doc.reset();
        doc.m_value.set(boolean_t(), val);
    }

    template <typename Document>
    constexpr static Document construct(const value_type &val,
                                        in_place_document_serializer_to_t) {
        return Document(boolean_t(), val);
    }
};

template <typename Arithmetic>
struct __document_serializer_arithmetic {
    using value_type = Arithmetic;

    /// from_document

    template <typename Document>
    constexpr static void assign(const Document &doc, value_type &val,
                                 in_place_document_serializer_from_t) {
        switch (doc.type()) {
        case value_t::number_unsigned: {
            val = static_cast<value_type>(doc.template get_unsafe<number_unsigned_t>());
            break;
        }
        case value_t::number_signed: {
            val = static_cast<value_type>(doc.template get_unsafe<number_signed_t>());
            break;
        }
        case value_t::number_float: {
            val = static_cast<value_type>(doc.template get_unsafe<number_float_t>());
            break;
        }
        default: {
            WJR_THROW(bad_expected_access(error_code::TAPE_ERROR));
            WJR_UNREACHABLE();
            break;
        }
        }
    }

    template <typename Document>
    WJR_PURE static value_type construct(const Document &doc,
                                         in_place_document_serializer_from_t) {
        value_type val;
        assign(doc, val, in_place_document_serializer_from);
        return val;
    }

    /// to_document

    template <typename Document>
    constexpr static void assign(const value_type &val, Document &doc,
                                 in_place_document_serializer_to_t) {
        doc.reset();
        if constexpr (std::is_floating_point_v<value_type>) {
            doc.m_value.set(number_float_t(), val);
        } else if constexpr (std::is_unsigned_v<value_type>) {
            doc.m_value.set(number_unsigned_t(), val);
        } else {
            doc.m_value.set(number_signed_t(), val);
        }
    }

    template <typename Document>
    constexpr static Document construct(const value_type &val,
                                        in_place_document_serializer_to_t) {
        if constexpr (std::is_floating_point_v<value_type>) {
            return Document(number_float_t(), val);
        } else if constexpr (std::is_unsigned_v<value_type>) {
            return Document(number_unsigned_t(), val);
        } else {
            return Document(number_signed_t(), val);
        }
    }
};

using document = basic_document<detail::default_document_traits>;

template <typename Arithmetic>
struct document_serializer_impl<Arithmetic,
                                std::enable_if_t<std::is_arithmetic_v<Arithmetic>>>
    : __document_serializer_arithmetic<Arithmetic> {};

template <>
struct document_serializer_impl<std::string_view, void> {
    using value_type = std::string_view;

    /// from_document

    template <typename Document>
    constexpr static void assign(const Document &doc, value_type &val,
                                 in_place_document_serializer_from_t) {
        val = doc.template get<string_t>();
    }

    template <typename Document, WJR_REQUIRES(!std::is_lvalue_reference_v<Document>)>
    constexpr static void assign(Document &&doc, value_type &val,
                                 in_place_document_serializer_from_t) = delete;

    template <typename Document>
    constexpr static value_type construct(const Document &doc,
                                          in_place_document_serializer_from_t) {
        return value_type(doc.template get<string_t>());
    }

    template <typename Document, WJR_REQUIRES(!std::is_lvalue_reference_v<Document>)>
    constexpr static value_type construct(Document &&doc,
                                          in_place_document_serializer_from_t) = delete;

    /// to_document

    template <typename Document>
    constexpr static void assign(const value_type &val, Document &doc,
                                 in_place_document_serializer_to_t) {
        if (doc.type() != value_t::string) {
            doc.reset();
            doc.__emplace_string(val);
        } else {
            doc.template get_unsafe<string_t>() = val;
        }
    }

    template <typename Document>
    constexpr static Document construct(const value_type &val,
                                        in_place_document_serializer_to_t) {
        return Document(string_t(),
                        __document_create<typename Document::string_type>(val));
    }
};

template <typename Alloc>
struct document_serializer_impl<std::basic_string<char, std::char_traits<char>, Alloc>,
                                void> {
    using value_type = std::basic_string<char, std::char_traits<char>, Alloc>;

    /// from_document

    template <typename Document>
    constexpr static void assign(const Document &doc, value_type &val,
                                 in_place_document_serializer_from_t) {
        val = doc.template get<string_t>();
    }

    template <typename Document, WJR_REQUIRES(!std::is_lvalue_reference_v<Document>)>
    constexpr static void assign(Document &&doc, value_type &val,
                                 in_place_document_serializer_from_t) {
        val = std::move(doc.template get<string_t>());
    }

    template <typename Document>
    constexpr static value_type construct(const Document &doc,
                                          in_place_document_serializer_from_t) {
        return value_type(doc.template get<string_t>());
    }

    template <typename Document, WJR_REQUIRES(!std::is_lvalue_reference_v<Document>)>
    constexpr static value_type construct(Document &&doc,
                                          in_place_document_serializer_from_t) {
        return value_type(std::move(doc.template get<string_t>()));
    }

    /// to_document

    template <typename Document>
    constexpr static void assign(const value_type &val, Document &doc,
                                 in_place_document_serializer_to_t) {
        if (doc.type() != value_t::string) {
            doc.reset();
            doc.__emplace_string(val);
        } else {
            doc.template get_unsafe<string_t>() = val;
        }
    }

    template <typename Document>
    constexpr static void assign(value_type &&val, Document &doc,
                                 in_place_document_serializer_to_t) {
        if (doc.type() != value_t::string) {
            doc.reset();
            doc.__emplace_string(std::move(val));
        } else {
            doc.template get_unsafe<string_t>() = std::move(val);
        }
    }

    template <typename Document>
    constexpr static Document construct(const value_type &val,
                                        in_place_document_serializer_to_t) {
        return Document(string_t(),
                        __document_create<typename Document::string_type>(val));
    }

    template <typename Document>
    constexpr static Document construct(value_type &&val,
                                        in_place_document_serializer_to_t) {
        return Document(string_t(), __document_create<typename Document::string_type>(
                                        std::move(val)));
    }
};

template <typename T>
struct __document_serializer_array {
    using value_type = T;

    /// from_document

    template <typename Document,
              WJR_REQUIRES(std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static void assign(const Document &doc, value_type &val,
                                 in_place_document_serializer_from_t) {
        val = doc.template get<array_t>();
    }

private:
    template <typename Iter>
    constexpr static void assign(Iter first, Iter last, value_type &val) {
        using element_reference = decltype(*val.begin());

        if constexpr (std::is_assignable_v<element_reference,
                                           iterator_reference_t<Iter>>) {
            val.assign(first, last);
        } else {
            using size_type = typename value_type::size_type;
            const auto n = static_cast<size_type>(last - first);
            if (n > val.capacity()) {
                val.clear();
                val.reserve(n);

                if constexpr (has_container_append_v<value_type, Iter, Iter>) {
                    append(val, first, last);
                } else {
                    for (; first != last; ++first) {
                        val.emplace_back(*first);
                    }
                }

                return;
            }

            auto val_first = val.begin();

            if (n > val.size()) {
                const auto val_last = val.end();
                for (; val_first != val_last; ++first, ++val_first) {
                    (*first).get_to(*val_first);
                }

                if constexpr (has_container_append_v<value_type, Iter, Iter>) {
                    append(val, first, last);
                } else {
                    for (; first != last; ++first) {
                        val.emplace_back(*first);
                    }
                }
            } else {
                val.resize(n);
                for (; first != last; ++first, ++val_first) {
                    (*first).get_to(*val_first);
                }
            }
        }
    }

public:
    template <
        typename Document,
        WJR_REQUIRES(!std::is_same_v<typename Document::array_type, value_type> &&
                     has_assign_from_document_v<
                         iterator_value_t<decltype(std::declval<value_type &>().begin())>,
                         const Document &> &&
                     has_construct_from_document_v<
                         iterator_value_t<decltype(std::declval<value_type &>().begin())>,
                         const Document &>)>
    constexpr static void assign(const Document &doc, value_type &val,
                                 in_place_document_serializer_from_t) {
        auto &arr = doc.template get<array_t>();
        assign(arr.begin(), arr.end(), val);
    }

    template <typename Document,
              WJR_REQUIRES(!std::is_lvalue_reference_v<Document> &&
                           std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static void assign(Document &&doc, value_type &val,
                                 in_place_document_serializer_from_t) {
        val = std::move(doc.template get<array_t>());
    }

    template <
        typename Document,
        WJR_REQUIRES(!std::is_lvalue_reference_v<Document> &&
                     !std::is_same_v<typename Document::array_type, value_type> &&
                     has_assign_from_document_v<
                         iterator_value_t<decltype(std::declval<value_type &>().begin())>,
                         Document &&> &&
                     has_construct_from_document_v<
                         iterator_value_t<decltype(std::declval<value_type &>().begin())>,
                         Document &&>)>
    constexpr static void assign(Document &&doc, value_type &val,
                                 in_place_document_serializer_from_t) {
        auto &arr = doc.template get<array_t>();
        assign(std::make_move_iterator(arr.begin()), std::make_move_iterator(arr.end()),
               val);
    }

    template <typename Document,
              WJR_REQUIRES(std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static value_type construct(const Document &doc,
                                          in_place_document_serializer_from_t) {
        return doc.template get<array_t>();
    }

    template <
        typename Document,
        WJR_REQUIRES(!std::is_same_v<typename Document::array_type, value_type> &&
                     has_construct_from_document_v<
                         iterator_value_t<decltype(std::declval<value_type &>().begin())>,
                         const Document &>)>
    constexpr static value_type construct(const Document &doc,
                                          in_place_document_serializer_from_t) {
        auto &arr = doc.template get<array_t>();
        return value_type(arr.begin(), arr.end());
    }

    template <typename Document,
              WJR_REQUIRES(!std::is_lvalue_reference_v<Document> &&
                           std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static value_type construct(Document &&doc,
                                          in_place_document_serializer_from_t) {
        return std::move(doc.template get<array_t>());
    }

    template <
        typename Document,
        WJR_REQUIRES(!std::is_lvalue_reference_v<Document> &&
                     !std::is_same_v<typename Document::array_type, value_type> &&
                     has_construct_from_document_v<
                         iterator_value_t<decltype(std::declval<value_type &>().begin())>,
                         Document &&>)>
    constexpr static value_type construct(Document &&doc,
                                          in_place_document_serializer_from_t) {
        auto &arr = doc.template get<array_t>();
        return value_type(std::make_move_iterator(arr.begin()),
                          std::make_move_iterator(arr.end()));
    }

    /// to_document

    template <typename Document,
              WJR_REQUIRES(std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static void assign(const value_type &val, Document &doc,
                                 in_place_document_serializer_to_t) {
        if (doc.type() != value_t::array) {
            doc.reset();
            doc.__emplace_array(val);
        } else {
            doc.template get_unsafe<array_t>() = val;
        }
    }

    template <typename Document,
              WJR_REQUIRES(
                  !std::is_same_v<typename Document::array_type, value_type> &&
                  has_assign_to_document_v<
                      Document, const iterator_value_t<
                                    decltype(std::declval<value_type &>().begin())> &> &&
                  has_construct_to_document_v<
                      Document, const iterator_value_t<
                                    decltype(std::declval<value_type &>().begin())> &>)>
    constexpr static void assign(const value_type &val, Document &doc,
                                 in_place_document_serializer_to_t) {
        if (doc.type() != value_t::array) {
            doc.reset();
            doc.__emplace_array(val.begin(), val.end());
        } else {
            doc.template get_unsafe<array_t>().assign(val.begin(), val.end());
        }
    }

    template <typename Document,
              WJR_REQUIRES(std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static void assign(value_type &&val, Document &doc,
                                 in_place_document_serializer_to_t) {
        if (doc.type() != value_t::array) {
            doc.reset();
            doc.__emplace_array(std::move(val));
        } else {
            doc.template get_unsafe<array_t>() = std::move(val);
        }
    }

    template <
        typename Document,
        WJR_REQUIRES(
            !std::is_same_v<typename Document::array_type, value_type> &&
            has_assign_to_document_v<
                Document,
                iterator_value_t<decltype(std::declval<value_type &>().begin())> &&> &&
            has_construct_to_document_v<
                Document,
                iterator_value_t<decltype(std::declval<value_type &>().begin())> &&>)>
    constexpr static void assign(value_type &&val, Document &doc,
                                 in_place_document_serializer_to_t) {
        if (doc.type() != value_t::array) {
            doc.reset();
            doc.__emplace_array(std::make_move_iterator(val.begin()),
                                std::make_move_iterator(val.end()));
        } else {
            doc.template get_unsafe<array_t>().assign(
                std::make_move_iterator(val.begin()), std::make_move_iterator(val.end()));
        }
    }

    template <typename Document,
              WJR_REQUIRES(std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static Document construct(const value_type &val,
                                        in_place_document_serializer_to_t) {
        return Document(array_t(), __document_create<typename Document::array_type>(val));
    }

    template <typename Document,
              WJR_REQUIRES(
                  !std::is_same_v<typename Document::array_type, value_type> &&
                  has_construct_to_document_v<
                      Document, const iterator_value_t<
                                    decltype(std::declval<value_type &>().begin())> &>)>
    constexpr static Document construct(const value_type &val,
                                        in_place_document_serializer_to_t) {
        return Document(array_t(), __document_create<typename Document::array_type>(
                                       val.begin(), val.end()));
    }

    template <typename Document,
              WJR_REQUIRES(std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static Document construct(value_type &&val,
                                        in_place_document_serializer_to_t) {
        return Document(array_t(),
                        __document_create<typename Document::array_type>(std::move(val)));
    }

    template <typename Document,
              WJR_REQUIRES(
                  !std::is_same_v<typename Document::array_type, value_type> &&
                  has_construct_to_document_v<
                      Document, iterator_value_t<
                                    decltype(std::declval<value_type &>().begin())> &&>)>
    constexpr static Document construct(value_type &&val,
                                        in_place_document_serializer_to_t) {
        return Document(array_t(), __document_create<typename Document::array_type>(
                                       std::make_move_iterator(val.begin()),
                                       std::make_move_iterator(val.end())));
    }
};

template <typename T, typename Alloc>
struct document_serializer_impl<std::vector<T, Alloc>, void>
    : __document_serializer_array<std::vector<T, Alloc>> {};

template <typename Traits>
struct document_serializer_impl<basic_vector<Traits>, void>
    : __document_serializer_array<basic_vector<Traits>> {};

template <typename T>
struct __document_serializer_object {
    using value_type = T;
    using read_only = std::false_type;

    /// from_document

    template <typename Document,
              WJR_REQUIRES(std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static void assign(const Document &doc, value_type &val,
                                 in_place_document_serializer_from_t) {
        val = doc.template get<object_t>();
    }

    template <typename Document,
              WJR_REQUIRES(!std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static void assign(const Document &doc, value_type &val,
                                 in_place_document_serializer_from_t) {
        auto &obj = doc.template get<object_t>();
        val.clear();
        for (const auto &elem : obj) {
            val.emplace(elem);
        }
    }

    template <typename Document,
              WJR_REQUIRES(!std::is_lvalue_reference_v<Document> &&
                           std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static void assign(Document &&doc, value_type &val,
                                 in_place_document_serializer_from_t) {
        val = std::move(doc.template get<object_t>());
    }

    template <typename Document,
              WJR_REQUIRES(!std::is_lvalue_reference_v<Document> &&
                           !std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static void assign(Document &&doc, value_type &val,
                                 in_place_document_serializer_from_t) {
        auto &obj = doc.template get<object_t>();
        val.clear();
        for (auto &elem : obj) {
            val.emplace(std::move(elem));
        }
    }

    template <typename Document,
              WJR_REQUIRES(std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static value_type construct(const Document &doc,
                                          in_place_document_serializer_from_t) {
        return doc.template get<object_t>();
    }

    template <typename Document,
              WJR_REQUIRES(!std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static value_type construct(const Document &doc,
                                          in_place_document_serializer_from_t) {
        auto &obj = doc.template get<object_t>();
        return value_type(obj.begin(), obj.end());
    }

    template <typename Document,
              WJR_REQUIRES(!std::is_lvalue_reference_v<Document> &&
                           std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static value_type construct(Document &&doc,
                                          in_place_document_serializer_from_t) {
        return std::move(doc.template get<object_t>());
    }

    template <typename Document,
              WJR_REQUIRES(!std::is_lvalue_reference_v<Document> &&
                           !std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static value_type construct(Document &&doc,
                                          in_place_document_serializer_from_t) {
        auto &obj = doc.template get<object_t>();
        return value_type(std::make_move_iterator(obj.begin()),
                          std::make_move_iterator(obj.end()));
    }

    /// to_document

    template <typename Document,
              WJR_REQUIRES(std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static void assign(const value_type &val, Document &doc,
                                 in_place_document_serializer_to_t) {
        doc.reset();
        doc.__emplace_object(val);
    }

    template <typename Document,
              WJR_REQUIRES(!std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static void assign(const value_type &val, Document &doc,
                                 in_place_document_serializer_to_t) {
        doc.reset();
        doc.__emplace_object(val.begin(), val.end());
    }

    template <typename Document,
              WJR_REQUIRES(std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static void assign(value_type &&val, Document &doc,
                                 in_place_document_serializer_to_t) {
        doc.reset();
        doc.__emplace_object(std::move(val));
    }

    template <typename Document,
              WJR_REQUIRES(!std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static void assign(value_type &&val, Document &doc,
                                 in_place_document_serializer_to_t) {
        doc.reset();
        doc.__emplace_object(std::make_move_iterator(val.begin()),
                             std::make_move_iterator(val.end()));
    }

    template <typename Document,
              WJR_REQUIRES(std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static Document construct(const value_type &val,
                                        in_place_document_serializer_to_t) {
        return Document(object_t(),
                        __document_create<typename Document::object_type>(val));
    }

    template <typename Document,
              WJR_REQUIRES(!std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static Document construct(const value_type &val,
                                        in_place_document_serializer_to_t) {
        return Document(object_t(), __document_create<typename Document::object_type>(
                                        val.begin(), val.end()));
    }

    template <typename Document,
              WJR_REQUIRES(std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static Document construct(value_type &&val,
                                        in_place_document_serializer_to_t) {
        return Document(object_t(), __document_create<typename Document::object_type>(
                                        std::move(val)));
    }

    template <typename Document,
              WJR_REQUIRES(!std::is_same_v<typename Document::array_type, value_type>)>
    constexpr static Document construct(value_type &&val,
                                        in_place_document_serializer_to_t) {
        return Document(object_t(), __document_create<typename Document::object_type>(
                                        std::make_move_iterator(val.begin()),
                                        std::make_move_iterator(val.end())));
    }
};

template <typename Key, typename Value, typename Pr, typename Alloc>
struct document_serializer_impl<std::map<Key, Value, Pr, Alloc>, void>
    : __document_serializer_object<std::map<Key, Value, Pr, Alloc>> {};

template <typename Key, typename Value, typename Pr, typename Alloc>
struct document_serializer_impl<std::multimap<Key, Value, Pr, Alloc>, void>
    : __document_serializer_object<std::multimap<Key, Value, Pr, Alloc>> {};

template <typename Key, typename Value, typename Hash, typename Eq, typename Alloc>
struct document_serializer_impl<std::unordered_map<Key, Value, Hash, Eq, Alloc>, void>
    : __document_serializer_array<std::unordered_map<Key, Value, Hash, Eq, Alloc>> {};

template <typename Key, typename Value, typename Hash, typename Eq, typename Alloc>
struct document_serializer_impl<std::unordered_multimap<Key, Value, Hash, Eq, Alloc>,
                                void>
    : __document_serializer_array<std::unordered_multimap<Key, Value, Hash, Eq, Alloc>> {
};

namespace detail {

template <typename Document>
class basic_document_parser {
    template <typename Parser>
    friend result<void> visitor_detail::parse(Parser &&par, const reader &rd) noexcept;

    using document_type = Document;
    using string_type = typename document_type::string_type;
    using object_type = typename document_type::object_type;
    using array_type = typename document_type::array_type;

public:
    WJR_INTRINSIC_INLINE result<document_type> parse(const reader &rd) noexcept {
        document_type doc;
        current = std::addressof(doc);
        WJR_EXPECTED_TRY(visitor_detail::parse(*this, rd));
        return doc;
    }

protected:
    WJR_INTRINSIC_INLINE result<void> visit_root_null(const char *first) const noexcept {
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_null(const char *first) const noexcept {
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_null(const char *first) const noexcept {
        current->__get_array().emplace_back(null_t());
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_true(const char *first) const noexcept {
        current->m_value.set(boolean_t(), true);
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_true(const char *first) const noexcept {
        element->m_value.set(boolean_t(), true);
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_true(const char *first) const noexcept {
        current->__get_array().emplace_back(boolean_t(), true);
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_false(const char *first) const noexcept {
        current->m_value.set(boolean_t(), false);
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_false(const char *first) const noexcept {
        element->m_value.set(boolean_t(), false);
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_false(const char *first) const noexcept {
        current->__get_array().emplace_back(boolean_t(), false);
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_number(const char *first,
                                                        const char *last) const noexcept {
        WJR_EXPECTED_INIT(ret, parse_number(first, last));
        current->m_value = *ret;
        return {};
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_number(const char *first, const char *last) const noexcept {
        WJR_EXPECTED_INIT(ret, parse_number(first, last));
        element->m_value = *ret;
        return {};
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_number(const char *first, const char *last) const noexcept {
        WJR_EXPECTED_INIT(ret, parse_number(first, last));
        current->__get_array().emplace_back(*ret);
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_string(const char *first,
                                                        const char *last) const noexcept {
        string_type *str = __document_create<string_type>();
        try_uninitialized_resize(*str, last - first);

        auto ret = parse_string(str->data(), first, last);
        if (WJR_UNLIKELY(!ret)) {
            __document_destroy(str);
            return unexpected(std::move(ret).error());
        }

        str->resize(*ret - str->data());
        current->m_value.set(string_t(), str);
        return {};
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_string(const char *first, const char *last) const noexcept {
        string_type *str = __document_create<string_type>();
        try_uninitialized_resize(*str, last - first);

        auto ret = parse_string(str->data(), first, last);
        if (WJR_UNLIKELY(!ret)) {
            __document_destroy(str);
            return unexpected(std::move(ret).error());
        }

        str->resize(*ret - str->data());
        element->m_value.set(string_t(), str);
        return {};
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_string(const char *first, const char *last) const noexcept {
        string_type *str = __document_create<string_type>();
        try_uninitialized_resize(*str, last - first);

        auto ret = parse_string(str->data(), first, last);
        if (WJR_UNLIKELY(!ret)) {
            __document_destroy(str);
            return unexpected(std::move(ret).error());
        }

        str->resize(*ret - str->data());
        current->__get_array().emplace_back(string_t(), str);
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_key_string(const char *first,
                                                              const char *last) noexcept {
        string_type str;
        try_uninitialized_resize(str, last - first);

        WJR_EXPECTED_INIT(ret, parse_string(str.data(), first, last));
        str.resize(*ret - str.data());
        const auto iter =
            current->__get_object().try_emplace(std::move(str), default_construct);
        element = std::addressof(iter.first->second);
        if (WJR_UNLIKELY(!iter.second)) {
            element->reset();
            return {};
        }

        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_start_object(uint32_t) const noexcept {
        current->m_value.set(object_t(), __document_create<object_type>());
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_start_object(uint32_t) noexcept {
        element->m_value.set(object_t(), __document_create<object_type>());
        stk.emplace_back(current);
        current = element;
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_start_object(uint32_t) noexcept {
        stk.emplace_back(current);
        current = std::addressof(current->__get_array().emplace_back(
            object_t(), __document_create<object_type>()));
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_start_array(uint32_t) const noexcept {
        current->m_value.set(array_t(), __document_create<array_type>());
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_start_array(uint32_t) noexcept {
        element->m_value.set(array_t(), __document_create<array_type>());
        stk.emplace_back(current);
        current = element;
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_start_array(uint32_t) noexcept {
        stk.emplace_back(current);
        current = std::addressof(current->__get_array().emplace_back(
            array_t(), __document_create<array_type>()));
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_object_to_object(uint32_t) noexcept {
        current = stk.back();
        stk.pop_back();
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_object_to_array(uint32_t) noexcept {
        current = stk.back();
        stk.pop_back();
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_object_to_root(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_object(uint32_t) noexcept {
        current = stk.back();
        stk.pop_back();
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_array(uint32_t) noexcept {
        current = stk.back();
        stk.pop_back();
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_root(uint32_t) const noexcept {
        return {};
    }

private:
    inplace_vector<document_type *, 256> stk;
    document_type *current;
    document_type *element;
};

class check_parser {
    template <typename Parser>
    friend result<void> visitor_detail::parse(Parser &&par, const reader &rd) noexcept;

public:
    WJR_INTRINSIC_INLINE static result<void> parse(const reader &rd) noexcept {
        return visitor_detail::parse(check_parser(), rd);
    }

protected:
    WJR_INTRINSIC_INLINE result<void> visit_root_null(const char *first) const noexcept {
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_null(const char *first) const noexcept {
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_null(const char *first) const noexcept {
        return check_null(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_true(const char *first) const noexcept {
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_true(const char *first) const noexcept {
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_true(const char *first) const noexcept {
        return check_true(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_false(const char *first) const noexcept {
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_false(const char *first) const noexcept {
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_false(const char *first) const noexcept {
        return check_false(first);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_number(const char *first,
                                                        const char *last) const noexcept {
        return check_number(first, last);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_number(const char *first, const char *last) const noexcept {
        return check_number(first, last);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_number(const char *first, const char *last) const noexcept {
        return check_number(first, last);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_string(const char *first,
                                                        const char *last) const noexcept {
        return check_string(first, last);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_string(const char *first, const char *last) const noexcept {
        return check_string(first, last);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_array_string(const char *first, const char *last) const noexcept {
        return check_string(first, last);
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_object_key_string(const char *first, const char *last) const noexcept {
        return check_string(first, last);
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_start_object(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_start_object(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_start_object(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_root_start_array(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_object_start_array(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_array_start_array(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void>
    visit_end_object_to_object(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_object_to_array(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_object_to_root(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_object(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_array(uint32_t) const noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE result<void> visit_end_array_to_root(uint32_t) const noexcept {
        return {};
    }
};

} // namespace detail

namespace visitor_detail {

extern template result<void> parse<detail::basic_document_parser<document> &>(
    detail::basic_document_parser<document> &par, const reader &rd) noexcept;

extern template result<void> parse<detail::check_parser>(detail::check_parser &&par,
                                                         const reader &rd) noexcept;
} // namespace visitor_detail

template <typename Traits>
result<basic_document<Traits>> basic_document<Traits>::parse(const reader &rd) noexcept {
    detail::basic_document_parser<basic_document<Traits>> par;
    return par.parse(rd);
}

inline result<void> check(const reader &rd) noexcept {
    return detail::check_parser::parse(rd);
}

namespace detail {

template <typename Formatter, typename Traits>
void format_impl(Formatter fmt, const basic_document<Traits> &doc,
                 unsigned depth) noexcept {
    switch (doc.type()) {
    case value_t::null: {
        fmt.format_null();
        break;
    }
    case value_t::boolean: {
        if (doc.template get_unsafe<boolean_t>()) {
            fmt.format_true();
        } else {
            fmt.format_false();
        }
        break;
    }
    case value_t::number_unsigned: {
        fmt.format_number_unsigned(doc.template get_unsafe<number_unsigned_t>());
        break;
    }
    case value_t::number_signed: {
        fmt.format_number_signed(doc.template get_unsafe<number_signed_t>());
        break;
    }
    case value_t::number_float: {
        fmt.format_number_float(doc.template get_unsafe<number_float_t>());
        break;
    }
    case value_t::string: {
        fmt.format_string(doc.template get_unsafe<string_t>());
        break;
    }
    case value_t::object: {
        fmt.format_start_object();
        const auto &obj = doc.template get_unsafe<object_t>();
        auto begin = obj.begin();
        const auto end = obj.end();

        if (begin != end) {
            goto LOOP0_HEAD;

            do {
                fmt.format_comma();

            LOOP0_HEAD:

                fmt.format_newline();
                fmt.format_indents(depth + 1);

                fmt.format_key(begin->first);
                fmt.format_space();
                format_impl(fmt, begin->second, depth + 1);
            } while (++begin != end);

            fmt.format_newline();
            fmt.format_indents(depth);
        }

        fmt.format_end_object();
        break;
    }
    case value_t::array: {
        fmt.format_start_array();
        const auto &arr = doc.template get_unsafe<array_t>();
        auto begin = arr.begin();
        const auto end = arr.end();

        if (begin != end) {
            goto LOOP1_HEAD;

            do {
                fmt.format_comma();

            LOOP1_HEAD:

                fmt.format_newline();
                fmt.format_indents(depth + 1);

                format_impl(fmt, *begin, depth + 1);
            } while (++begin != end);

            fmt.format_newline();
            fmt.format_indents(depth);
        }

        fmt.format_end_array();
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }
}

} // namespace detail

template <typename Traits, typename DocumentTraits>
std::basic_ostream<char, Traits> &operator<<(std::basic_ostream<char, Traits> &os,
                                             const basic_document<DocumentTraits> &doc) {
    if (const std::ostream::sentry ok(os); ok) {
        unique_stack_allocator stkal(math_detail::stack_alloc);

        std::basic_string<char, Traits, math_detail::weak_stack_alloc<char>> buffer(
            stkal);
        buffer.reserve(256);

        const std::streamsize indents = os.width();

        doc.dump_impl(buffer, indents);
        __ostream_write_unchecked(os, buffer.data(), buffer.size());
        os.width(0);
    } else {
        os.setstate(std::ios::badbit);
    }

    return os;
}

template <typename DocumentTraits>
std::ostream &operator<<(std::ostream &os, const basic_document<DocumentTraits> &doc) {
    if (const std::ostream::sentry ok(os); ok) {
        unique_stack_allocator stkal(math_detail::stack_alloc);

        vector<char, math_detail::weak_stack_alloc<char>> buffer(stkal);
        buffer.clear_if_reserved(256);

        const std::streamsize indents = os.width();

        doc.dump_impl(buffer, indents);
        __ostream_write_unchecked(os, buffer.data(), buffer.size());
        os.width(0);
    } else {
        os.setstate(std::ios::badbit);
    }

    return os;
}

template <typename Traits>
WJR_PURE bool operator==(const basic_document<Traits> &lhs,
                         const basic_document<Traits> &rhs) noexcept {
    switch (lhs.type()) {
    case value_t::null: {
        return rhs.is_null();
    }
    case value_t::boolean: {
        return rhs.is_boolean();
    }
    case value_t::number_unsigned:
    case value_t::number_signed:
    case value_t::number_float: {
        return rhs.is_number() && (static_cast<double>(lhs) == static_cast<double>(rhs));
    }
    case value_t::string: {
        return rhs.is_string() &&
               lhs.template get_unsafe<string_t>() == rhs.template get_unsafe<string_t>();
    }
    case value_t::object: {
        return rhs.is_object() &&
               lhs.template get_unsafe<object_t>() == rhs.template get_unsafe<object_t>();
    }
    case value_t::array: {
        return rhs.is_array() &&
               lhs.template get_unsafe<array_t>() == rhs.template get_unsafe<array_t>();
    }
    default: {
        WJR_UNREACHABLE();
        return false;
    }
    }
}

template <typename Traits>
WJR_PURE bool operator!=(const basic_document<Traits> &lhs,
                         const basic_document<Traits> &rhs) noexcept {
    return !(lhs == rhs);
}

} // namespace wjr::json

namespace wjr {

template <typename Traits>
struct get_relocate_mode<json::basic_document<Traits>> {
    static constexpr relocate_t value = relocate_t::trivial;
};

} // namespace wjr

namespace std {

template <typename Traits>
constexpr void
swap(wjr::json::basic_document<Traits> &lhs,
     wjr::json::basic_document<Traits> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

} // namespace std

#endif // WJR_JSON_DOCUMENT_HPP__