#ifndef WJR_JSON_DOCUMENT_HPP__
#define WJR_JSON_DOCUMENT_HPP__

#include <charconv>
#include <map>
#include <variant>

#include <wjr/json/lexer.hpp>
#include <wjr/math.hpp>
#include <wjr/vector.hpp>

namespace wjr {

struct document;

struct document {
    using string_type = std::string;
    using value_type = double;
    using array_type = wjr::vector<document>;
    using object_type = std::map<string_type, document>;
    using null_type = std::nullptr_t;
    using bool_type = bool;

    using var =
        std::variant<null_type, bool_type, value_type, std::unique_ptr<string_type>,
                     std::unique_ptr<object_type>, std::unique_ptr<array_type>>;

    static bool parse(document &doc, span<const char>);

    template <typename... Args,
              std::enable_if_t<std::is_constructible_v<var, Args...>, int> = 0>
    document(Args &&...args) : value(std::forward<Args>(args)...) {}

    var value;
};

struct node {
    using object_type = typename document::object_type;
    using array_type = typename document::array_type;

    template <typename... Args,
              std::enable_if_t<std::is_constructible_v<
                                   std::variant<object_type *, array_type *>, Args...>,
                               int> = 0>
    node(Args &&...args) : current(std::forward<Args>(args)...) {}

    std::variant<object_type *, array_type *> current;
    std::string_view name;
};

bool document::parse(document &doc, span<const char> sp) {
    lexer lex(sp);
    doc.value.emplace<4>(new object_type);

    unique_stack_allocator stkal(math_details::stack_alloc);
    math_details::weak_stack_alloc<node> al(stkal);
    auto del = [&al](node *ptr) { al.deallocate(ptr, 256); };
    std::unique_ptr<node, decltype(del)> __stk(al.allocate(256), del);
    auto stk = __stk.get();
    node *current = stk;
    uninitialized_construct_using_allocator(current, al, std::in_place_index<0>,
                                            std::get<4>(doc.value).get());

    uint32_t token;
    uint32_t next;

    if (!lex.next(token)) {
        return false;
    }

    switch (sp[token]) {
    case ' ':
    case '\n':
    case '\t':
    case '\r': {
        if (!lex.next(token) || sp[token] != '{') {
            return false;
        }
        break;
    }
    case '{': {
        break;
    }
    default: {
        return false;
    }
    }

OBJECT : {
    if (!lex.next(token)) {
        return false;
    }

OBJECT_TOKEN:

    do {
        switch (sp[token]) {
        case ' ':
        case '\n':
        case '\t':
        case '\r':
        case ',': {
            break;
        }
        case '"': {
            if (!lex.next(next)) {
                return false;
            }

            size_t string_length = next - token;
            current->name = std::string_view(sp.data() + token + 1, string_length - 2);

            switch (sp[next]) {
            case ' ':
            case '\n':
            case '\t':
            case '\r': {
                if (!lex.next(next) || sp[next] != ':') {
                    return false;
                }
                break;
            }
            case ':': {
                break;
            }
            }

            goto OBJECT_STACK;
        }
        case '}': {
            if (current == stk) {
                return true;
            }

            node *back = current - 1;

            // back is an object
            if (back->current.index() == 0) {
                std::get<0>(back->current)
                    ->emplace(std::piecewise_construct, std::make_tuple(back->name),
                              std::make_tuple(std::in_place_index<4>,
                                              std::get<0>(current->current)));
                current = back;
                goto OBJECT;
            } else {
                std::get<1>(back->current)
                    ->emplace_back(std::in_place_index<4>, std::get<0>(current->current));
                current = back;
                goto ARRAY;
            }
        }
        default: {
            return false;
        }
        }
    } while (lex.next(token));
}

OBJECT_TRY : {
    switch (sp[next]) {
    case ' ':
    case '\n':
    case '\t':
    case '\r': {
        if (!lex.next(next)) {
            return false;
        }
        break;
    }
    }

    token = next;
    goto OBJECT_TOKEN;
}

ARRAY : {
    if (!lex.next(token)) {
        return false;
    }
ARRAY_TOKEN:

    do {
        switch (sp[token]) {
        case ' ':
        case '\n':
        case '\t':
        case '\r':
        case ',': {
            break;
        }
        case 'n': {
            if (!lex.next(next)) {
                return false;
            }

            if (next - token != 4 || std::strncmp(sp.data() + token, "null", 4) != 0) {
                return false;
            }

            std::get<1>(current->current)->emplace_back(std::in_place_index<0>);
            goto ARRAY_TRY;
        }
        case 't': {
            if (!lex.next(next)) {
                return false;
            }

            if (next - token != 4 || std::strncmp(sp.data() + token, "ture", 4) != 0) {
                return false;
            }

            std::get<1>(current->current)->emplace_back(std::in_place_index<1>, true);
            goto OBJECT_TRY;
        }
        case 'f': {
            if (!lex.next(next)) {
                return false;
            }

            if (next - token != 5 ||
                std::strncmp(sp.data() + token + 1, "alse", 4) != 0) {
                return false;
            }

            std::get<1>(current->current)->emplace_back(std::in_place_index<1>, false);
            goto ARRAY_TRY;
        }
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            if (!lex.next(next)) {
                return false;
            }

            double value;
            const auto first = sp.data() + token;
            const auto last = sp.data() + next;
            auto ret = std::from_chars(first, last, value);

            if (ret.ec != std::errc{} || ret.ptr != last) {
                return false;
            }

            std::get<1>(current->current)->emplace_back(std::in_place_index<2>, value);
            goto ARRAY_TRY;
        }
        case '"': {
            if (!lex.next(next)) {
                return false;
            }

            std::get<1>(current->current)
                ->emplace_back(std::in_place_index<3>,
                               std::make_unique<string_type>(sp.data() + token + 1,
                                                             next - token - 2));
            goto ARRAY_TRY;
        }
        case '{': {
            ++current;
            uninitialized_construct_using_allocator(current, al, std::in_place_index<0>,
                                                    new object_type);
            goto OBJECT;
        }
        case '[': {
            ++current;
            uninitialized_construct_using_allocator(current, al, std::in_place_index<1>,
                                                    new array_type);
            goto ARRAY;
        }
        case ']': {
            node *back = current - 1;

            if (back->current.index() == 0) {
                std::get<0>(back->current)
                    ->emplace(std::piecewise_construct, std::make_tuple(back->name),
                              std::make_tuple(std::in_place_index<5>,
                                              std::get<1>(current->current)));
                current = back;
                goto OBJECT;
            } else {
                std::get<1>(back->current)
                    ->emplace_back(std::in_place_index<5>, std::get<1>(current->current));
                current = back;
                goto ARRAY;
            }
        }
        default: {
            return false;
        }
        }
    } while (lex.next(token));
}

OBJECT_STACK : {
    lex.next(token);

    switch (sp[token]) {
    case 'n': {
        if (!lex.next(next)) {
            return false;
        }

        if (next - token != 4 || std::strncmp(sp.data() + token, "null", 4) != 0) {
            return false;
        }

        std::get<0>(current->current)
            ->emplace(std::piecewise_construct, std::make_tuple(current->name),
                      std::make_tuple(std::in_place_index<0>));
        goto OBJECT_TRY;
    }
    case 't': {
        if (!lex.next(next)) {
            return false;
        }

        if (next - token != 4 || std::strncmp(sp.data() + token, "true", 4) != 0) {
            return false;
        }

        std::get<0>(current->current)
            ->emplace(std::piecewise_construct, std::make_tuple(current->name),
                      std::make_tuple(std::in_place_index<1>, true));
        goto OBJECT_TRY;
    }
    case 'f': {
        if (!lex.next(next)) {
            return false;
        }

        if (next - token != 5 || std::strncmp(sp.data() + token + 1, "alse", 4) != 0) {
            return false;
        }

        std::get<0>(current->current)
            ->emplace(std::piecewise_construct, std::make_tuple(current->name),
                      std::make_tuple(std::in_place_index<1>, false));
        goto OBJECT_TRY;
    }
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
        if (!lex.next(next)) {
            return false;
        }

        double value;
        const auto first = sp.data() + token;
        const auto last = sp.data() + next;
        auto ret = std::from_chars(first, last, value);

        if (ret.ec != std::errc{} || ret.ptr != last) {
            return false;
        }

        std::get<0>(current->current)
            ->emplace(std::piecewise_construct, std::make_tuple(current->name),
                      std::make_tuple(std::in_place_index<2>, value));
        goto OBJECT_TRY;
    }
    case '"': {
        if (!lex.next(next)) {
            return false;
        }

        std::get<0>(current->current)
            ->emplace(std::piecewise_construct, std::make_tuple(current->name),
                      std::make_tuple(std::in_place_index<3>,
                                      std::make_unique<string_type>(sp.data() + token + 1,
                                                                    next - token - 2)));
        goto OBJECT_TRY;
    }
    case '{': {
        ++current;
        uninitialized_construct_using_allocator(current, al, std::in_place_index<0>,
                                                new object_type);
        goto OBJECT;
    }
    case '[': {
        ++current;
        uninitialized_construct_using_allocator(current, al, std::in_place_index<1>,
                                                new array_type);
        goto ARRAY;
    }
    default: {
        return false;
    }
    }
}

ARRAY_TRY : {
    switch (sp[next]) {
    case ' ':
    case '\n':
    case '\t':
    case '\r': {
        if (!lex.next(next)) {
            return false;
        }
        break;
    }
    }

    token = next;
    goto ARRAY_TOKEN;
}
}

} // namespace wjr

#endif // WJR_JSON_DOCUMENT_HPP__