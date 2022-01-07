#ifndef WJR_STRING_H
#define WJR_STRING_H

#include <iostream>
#include <string.h>
#include <cassert>
#include <string>
#include <iterator>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include "mallocator.h"
#include "mySTL.h"

namespace wjr {

    //skmp searcher

    template<typename RanItPat, typename Pred_eq = std::equal_to<>>
    class skmp_searcher_fshift_builder {
    private:
        using value_t = typename std::iterator_traits<RanItPat>::value_type;
        using diff_t = typename std::iterator_traits<RanItPat>::difference_type;

    public:

        using value_type = value_t;
        using difference_type = diff_t;
        using allocator_type = mallocator<diff_t>;

        skmp_searcher_fshift_builder(const RanItPat First, const RanItPat Last, const Pred_eq Eq);

        skmp_searcher_fshift_builder(const skmp_searcher_fshift_builder& other);

        skmp_searcher_fshift_builder(skmp_searcher_fshift_builder&& other)noexcept;

        ~skmp_searcher_fshift_builder() ;

        RanItPat get_first()const { return first; }
        RanItPat get_last()const { return last; }

        diff_t* get_fshift()const { return fshift; }
        diff_t* get_fm()const { return fm; }
        diff_t get_size()const { return size; }

    private:
        diff_t* fshift, * fm, size;
        RanItPat first, last;
    };

    template<typename RanItPat,typename Pred_eq>
    skmp_searcher_fshift_builder<RanItPat,Pred_eq>::skmp_searcher_fshift_builder(
        const RanItPat First, const RanItPat Last, const Pred_eq Eq
    ) : first(First), last(Last) {
        size = last - first;
        auto al = mallocator<diff_t>();
        fshift = al.allocate(size + 1);
        fm = al.allocate(size + 1);

        diff_t* fnxt = al.allocate(size + 1);
        diff_t i = 0, j = -1;
        fnxt[0] = -1;

        while (i < size) {
            if (j == -1 || Eq(*(last - 1 - i), *(last - 1 - j))) {
                fnxt[++i] = ++j;
            }
            else j = fnxt[j];
        }

        std::fill_n(fshift, size + 1, static_cast<diff_t>(-1));

        for (i = 1; i <= size; ++i) {
            while (i < size && fnxt[i + 1] == fnxt[i] + 1)
                ++i;
            j = i;
            while (fnxt[j] != -1 && fshift[fnxt[j]] == -1) {
                fshift[fnxt[j]] = i - fnxt[j];
                j = fnxt[j];
            }
        }

        std::fill_n(fm, size + 1, size);

        i = size;

        while (fnxt[i] != -1) {
            fm[fnxt[i]] = size - fnxt[i];
            i = fnxt[i];
        }

        for (i = 1; i <= size; ++i)
            fm[i] = fm[i] < fm[i - 1] ? fm[i] : fm[i - 1];

        for (i = 0; i <= size; ++i)
            fshift[i] = ~fshift[i] ? fshift[i] + i : 0;

        al.deallocate(fnxt, size + 1);
    }

    template<typename RanItPat, typename Pred_eq>
    skmp_searcher_fshift_builder<RanItPat, Pred_eq>::
        skmp_searcher_fshift_builder(const skmp_searcher_fshift_builder& other)
        : size(other.size), first(other.first), last(other.last) {
        auto al = mallocator<diff_t>();
        fshift = al.allocate(size + 1);
        fm = al.allocate(size + 1);
        std::copy(other.fshift, other.fshift + size + 1, fshift);
        std::copy(other.fm, other.fm + size + 1, fm);
    }

    template<typename RanItPat, typename Pred_eq>
    skmp_searcher_fshift_builder<RanItPat, Pred_eq>::
        skmp_searcher_fshift_builder(skmp_searcher_fshift_builder&& other)noexcept
        : fshift(other.fshift), fm(other.fm), size(other.size),
        first(other.first), last(other.last) {
        other.fshift = other.fm = nullptr;
    }

    template<typename RanItPat, typename Pred_eq>
    skmp_searcher_fshift_builder<RanItPat, Pred_eq>::
        ~skmp_searcher_fshift_builder() {
        auto al = mallocator<diff_t>();
        if (fshift) {
            al.deallocate(fshift, size + 1);
            al.deallocate(fm, size + 1);
        }
    }

    template<typename RanItPat, typename Hash_ty =
        std::hash<typename std::iterator_traits<RanItPat>::value_type>, typename Pred_eq = std::equal_to<>>
    class skmp_searcher_char_builder
        : public skmp_searcher_fshift_builder<RanItPat, Pred_eq> {
        private:

            using Base = skmp_searcher_fshift_builder<RanItPat, Pred_eq>;
            using value_t = typename Base::value_type;
            using diff_t = typename Base::difference_type;

        public:
            using value_type = value_t;
            using difference_type = diff_t;

            skmp_searcher_char_builder(
                RanItPat First, RanItPat Last, Hash_ty fn = Hash_ty(), Pred_eq Eq = Pred_eq()
            ) ;

            skmp_searcher_char_builder(const skmp_searcher_char_builder& other)
                : Base(other) {
                shift = mallocator<diff_t>().allocate(256);
                std::copy(other.shift, other.shift + 256, shift);
            }

            skmp_searcher_char_builder(skmp_searcher_char_builder&& other)noexcept
                : Base(std::move(other)), shift(other.shift) {
                other.shift = nullptr;
            }

            ~skmp_searcher_char_builder() {
                if (shift != nullptr) {
                    mallocator<diff_t>().deallocate(shift, 256);
                }
            }

            diff_t get_shift(const value_t value)const {
                const auto u_value = static_cast<std::make_unsigned_t<value_t>>(value);
                return shift[u_value];
            }

            std::equal_to<> key_eq()const { return {}; }

        private:
            diff_t* shift;
    };

    template<typename RanItPat, typename Hash_ty, typename Pred_eq>
    skmp_searcher_char_builder<RanItPat,Hash_ty,Pred_eq>::skmp_searcher_char_builder(
        RanItPat First, RanItPat Last, Hash_ty fn, Pred_eq Eq
    ) : Base(First, Last, Eq) {
        auto first = Base::get_first();
        const auto last = Base::get_last();
        const auto size = Base::get_size();
        shift = mallocator<diff_t>().allocate(256);
        std::fill_n(shift, 256, size + 1);
        diff_t i = size;
        while (first != last) {
            const auto u_value = static_cast<std::make_unsigned_t<value_t>>(*first);
            shift[u_value] = i;
            ++first;
            --i;
        }
    }

    template<typename RanItPat, typename Hash_ty =
        std::hash<typename std::iterator_traits<RanItPat>::value_type>,
        typename Pred_eq = std::equal_to<>>
    class skmp_searcher_general_builder
        : public skmp_searcher_fshift_builder<RanItPat, Pred_eq> {
        private:
            using Base = skmp_searcher_fshift_builder<RanItPat, Pred_eq>;
            using value_t = typename Base::value_type;
            using diff_t = typename Base::difference_type;

        public:
            using value_type = value_t;
            using difference_type = diff_t;

            skmp_searcher_general_builder(
                RanItPat First, RanItPat Last, Hash_ty fn = Hash_ty(), Pred_eq Eq = Pred_eq()
            ) ;

            skmp_searcher_general_builder(const skmp_searcher_general_builder& other)
                : Base(other), Map(other.Map) {

            }

            skmp_searcher_general_builder(skmp_searcher_general_builder&& other)noexcept
                : Base(std::move(other)), Map(std::move(other.Map)) {

            }

            diff_t get_shift(const value_t& value)const {
                const auto iter = Map.find(value);
                if (iter == Map.end()) {
                    return Base::get_size() + 1;
                }
                return iter->second;
            }

            Pred_eq key_eq()const { return Map.key_eq(); }

        private:
            std::unordered_map<value_t, diff_t, Hash_ty, Pred_eq> Map;
    };

    template<typename RanItPat, typename Hash_ty, typename Pred_eq>
    skmp_searcher_general_builder<RanItPat,Hash_ty,Pred_eq>::skmp_searcher_general_builder(
        RanItPat First, RanItPat Last, Hash_ty fn, Pred_eq Eq
    ) : Base(First, Last, Eq) {
        auto first = Base::get_first();
        const auto last = Base::get_last();
        const auto size = Base::get_size();
        diff_t i = size;
        while (first != last) {
            Map.insert_or_assign(*first, i);
            ++first;
            --i;
        }
    }

    template<typename RanItPat, typename Hash_ty, typename Pred_eq, typename value_t =
        typename std::iterator_traits<RanItPat>::value_type>
        using skmp_searcher_traits = std::conditional_t<std::is_integral_v<value_t> && sizeof(value_t) == 1
        && (std::is_same_v<std::equal_to<>, Pred_eq> || std::is_same_v<std::equal_to<value_t>, Pred_eq>),
        skmp_searcher_char_builder <RanItPat, Hash_ty, Pred_eq>,
        skmp_searcher_general_builder<RanItPat, Hash_ty, Pred_eq>>;

    template<typename Container, typename = void>
    struct Has_begin_end : std::false_type {};

    template<typename Container>
    struct Has_begin_end<Container, std::void_t<
        decltype(std::declval<Container>().begin(), std::declval<Container>().end())>>
        : std::true_type{};

    template<typename Container>
    using is_Has_begin_end = std::enable_if_t<Has_begin_end<Container>::value, int>;

    template<typename T, typename = void>
    struct Has_iterator : std::false_type {};

    template<typename T>
    struct Has_iterator<T, std::void_t<decltype(std::declval<T>().begin())>> : std::true_type {};

    template<typename T>
    struct Iterator_traits_begin {
        using type = decltype(std::declval<T>().begin());
    };

    template<typename Container>
    using container_iterator_type = typename Iterator_traits_begin<Container>::type;

    template<typename RanItPat, typename Hash_ty
        = std::hash<typename std::iterator_traits<RanItPat>::value_type>,
        typename Pred_eq = std::equal_to<>>
        class skmp_searcher {
        private:
            using value_t = typename std::iterator_traits<RanItPat>::value_type;
            using diff_t = typename std::iterator_traits<RanItPat>::difference_type;
        public:
            using value_type = value_t;
            using difference_type = diff_t;

            skmp_searcher(
                const RanItPat First, const RanItPat Last, Hash_ty fn = Hash_ty(), Pred_eq Eq = Pred_eq()
            ) : Searcher(First, Last, fn, Eq) {

            }

            skmp_searcher(
                const RanItPat First, const size_t length, Hash_ty fn = Hash_ty(), Pred_eq Eq = Pred_eq()
            ) : Searcher(First, First + length, fn, Eq) {

            }

            skmp_searcher(const skmp_searcher& other)
                : Searcher(other.Searcher) {

            }

            skmp_searcher(skmp_searcher&& other) noexcept
                : Searcher(std::move(other.Searcher)) {

            }

            ~skmp_searcher() = default;

            skmp_searcher& operator=(const skmp_searcher&) = delete;

            template<typename iter>
            std::pair<iter, iter> operator()(iter First, iter Last)const;

        private:
            template<typename T, typename = void>
            struct _has_find : std::false_type {};

            template<typename T>
            struct _has_find<T, std::void_t<decltype(T::find(
                std::declval<const value_type*>(),
                std::declval<diff_t>(), std::declval<const value_type&>()))>>
                : std::true_type{};

            using is_has_find = _has_find<Pred_eq>;

            using Traits = skmp_searcher_traits<RanItPat, Hash_ty, Pred_eq>;
            Traits Searcher;
    };

    template<typename RanItPat, typename Hash_ty, typename Pred_eq>
    template<typename iter>
    std::pair<iter, iter> skmp_searcher<RanItPat, Hash_ty, Pred_eq>::operator()(
        iter First, iter Last
        )const {
        const auto size = Searcher.get_size();

        if (size == 0) {
            return { First,First };
        }

        const auto pattern_first = Searcher.get_first();
        const auto pattern_last = pattern_first + size;
        const auto pattern_back = pattern_last - 1;

        const auto text_first = First;
        const auto text_last = Last;
        const auto text_back = text_last - 1;

        if (text_last - text_first < size) {
            return { Last,Last };
        }

        const auto Eq = Searcher.key_eq();
        const auto fshift = Searcher.get_fshift();
        const auto fm = Searcher.get_fm();
        auto& ch = *pattern_back;
        auto text_ptr = text_first + size - 1;

        diff_t sufsearch = 0, alsearch = 0;
        diff_t res = text_back - text_ptr;

        for (;;) {
            auto s_text_ptr = text_ptr;
            auto s_pattern_ptr = pattern_back;
            const auto s_ptr_mid = text_ptr - sufsearch;

            while (s_text_ptr != s_ptr_mid && Eq(*s_text_ptr, *s_pattern_ptr)) {
                --s_text_ptr;
                --s_pattern_ptr;
            }

            if (s_text_ptr == s_ptr_mid) {
                if (alsearch + sufsearch != size) {
                    s_text_ptr -= alsearch;
                    s_pattern_ptr -= alsearch;

                    while (s_pattern_ptr != pattern_first && Eq(*s_text_ptr, *s_pattern_ptr)) {
                        --s_text_ptr;
                        --s_pattern_ptr;
                    }

                    if (s_pattern_ptr == pattern_first && Eq(*s_text_ptr, *s_pattern_ptr)) {
                        ++text_ptr;
                        return { text_ptr - size,text_ptr };
                    }
                }
                else {
                    ++text_ptr;
                    return { text_ptr - size,text_ptr };
                }
            }
            if (text_ptr == text_back) {
                break;
            }
            diff_t Match = text_ptr - s_text_ptr;
            const auto Fs = fshift[Match];
            const auto Sh = Searcher.get_shift(*(text_ptr + 1));

            if (!Fs) {
                if (Sh >= size) {
                    sufsearch = Sh;
                    alsearch = 0;
                }
                else {
                    const auto Fm = fm[Match];
                    sufsearch = Fm;
                    alsearch = size - Fm;
                }
            }
            else {
                if (Sh >= Fs) {
                    sufsearch = Sh;
                    alsearch = 0;
                }
                else {
                    sufsearch = Fs - Match;
                    alsearch = Match;
                }
            }

            if (sufsearch > res)
                break;
            text_ptr += sufsearch;
            res -= sufsearch;
            if constexpr (is_has_find::value) {
                const auto l = sufsearch + alsearch;
                if (l <= 32 && !Eq(*text_ptr, ch)) {
                    auto pos = Pred_eq::find(text_ptr + 1, res, ch);
                    if (pos == nullptr) {
                        break;
                    }
                    const diff_t delta = (const value_t*)pos - text_ptr;
                    if (delta >= l) {
                        res -= delta;
                        text_ptr += delta;
                        sufsearch = alsearch = 0;
                    }
                }
            }
            else {
                if constexpr (std::is_pointer_v<iter> &&
                    std::is_same_v<Pred_eq, std::equal_to<>> &&
                    sizeof(value_t) <= sizeof(int) && std::is_integral_v<value_t>) {
                    const auto l = sufsearch + alsearch;
                    if (l <= 32 && !Eq(*text_ptr, ch)) {
                        void* pos = memchr((void*)(text_ptr + 1), ch, res);
                        if (pos == nullptr)
                            break;
                        const diff_t delta = (const value_t*)pos - text_ptr;
                        if (delta >= l) {
                            res -= delta;
                            text_ptr += delta;
                            sufsearch = alsearch = 0;
                        }
                    }
                }
            }

            if (sufsearch >= size) {
                sufsearch = alsearch = 0;
            }
        }
        return { Last,Last };
    }

    //--------------------------------------------------------------------------------//
    //--------------------------------------------------------------------------------//
    //--------------------------------------------------------------------------------//

    template<typename RanItPat, typename Traits>
    struct String_find_helper {
    public:
        using value_type = typename std::iterator_traits<RanItPat>::value_type;
        using skmp_searcher_type = skmp_searcher<RanItPat, std::hash<value_type>, Traits>;

    public:

        String_find_helper(RanItPat s, RanItPat e)
            : srch(s,e,std::hash<value_type>(),Traits()), _size(static_cast<size_t>(e - s)) {

        }

        template<typename iter>
        std::pair<iter,iter> operator()(iter s, iter e)const {
            return srch(s, e);
        }

        size_t size()const { return _size; }

    private:
        skmp_searcher_type srch;
        size_t _size;
    };

    template<typename Char,typename Traits>
    struct String_find_traits : public Traits{
        using value_type = Char;
        using size_type = size_t;
        using traits_type = Traits;
        bool operator()(const value_type& a, const value_type& b)const {
            return traits_type::eq(a, b);
        }
    };

    template<typename Char,typename Traits>
    struct String_rfind_traits { 
        using value_type = Char;
        using size_type = size_t;
        using traits_type = Traits;
        bool operator()(const value_type& a, const value_type& b)const {
            return traits_type::eq(a, b);
        }
        static int compare(const value_type* a, const value_type* b, const size_t count) {
            return Traits::compare(a,b,count);
        }
    };

    template<typename Traits>
    struct case_insensitive : public Traits{
        using base = Traits;
        using char_type = typename base::char_type;

        static constexpr bool eq(const char_type& lhs, const char_type& rhs) noexcept {
            return base::eq(tolower(lhs),tolower(rhs));
        }

        static constexpr bool lt(const char_type& lhs, const char_type& rhs)noexcept {
            if (isalpha(lhs) && isalpha(rhs)) {
                return base::lt(tolower(lhs),tolower(rhs));
            }
            else {
                return base::lt(lhs,rhs);
            }
        }

        static int compare(const char_type* l, const char_type* r, const size_t count)noexcept {
            const char_type* e = l + count;
            while (l != e) {
                if (isalpha(*l) && isalpha(*r)) {
                    char_type lc = tolower(*l);
                    char_type rc = tolower(*r);
                    if (!base::eq(lc,rc)) {
                        return base::lt(lc,rc) ? -1 : 1;
                    }
                }
                else {
                    if (!base::eq(*l,*r)) {
                        return base::lt(*l,*r) ? -1 : 1;
                    }
                }
                ++l;
                ++r;
            }
            return 0;
        }

        static constexpr const char_type* find(
            const char_type* first, size_t count, const char_type& ch) noexcept {
            for (; 0 < count; --count, ++first) {
                if (eq(*first, ch)) {
                    return first;
                }
            }
            return nullptr;
        }
    };

    template<typename Char>
    class String_trim_helper {
    public:
        constexpr static const Char* data() {
            return tr;
        }
        constexpr static size_t size() {
            return 6;
        }
    private:
        constexpr static Char tr[7] = {
            static_cast<Char>(' '),
            static_cast<Char>('\r'),
            static_cast<Char>('\n'),
            static_cast<Char>('\r'),
            static_cast<Char>('\f'),
            static_cast<Char>('\0')
        };
    };

    template<typename Char, typename Traits>
    struct basic_String_traits {
    public:
        using value_type = Char;
        using traits_type = Traits;
        using size_type = size_t;

        constexpr static size_type npos = static_cast<size_type>(-1);
    private:
        struct bit_map {
            constexpr static size_t ubit = sizeof(uint32_t);
            constexpr static size_t size = ubit << 3;

            bit_map(const value_type* s, const value_type* e) {
                while (s != e) {
                    auto value = static_cast<std::make_unsigned_t<value_type>>(*s);
                    c[value / size] |= 1 << (value & (size - 1));
                    ++s;
                }
            }

            bool count(const value_type ch)const {
                const auto value = static_cast<std::make_unsigned_t<value_type>>(ch);
                return (c[value / size] >> (value & (size - 1))) & 1;
            }

            uint32_t c[256 / size] = {};
        };
    public:
        using string_find_traits = String_find_traits<Char,Traits>;
        using string_rfind_traits = String_rfind_traits<Char,Traits>;
        using string_find_helper = String_find_helper<const value_type*,string_find_traits>;
        using string_rfind_helper = String_find_helper<
            std::reverse_iterator<const value_type*>,string_rfind_traits>;

        using string_find_of_helper = std::conditional_t<
            std::is_integral_v<value_type> && sizeof(value_type) <= 1
            && std::is_same_v<Traits, std::char_traits<value_type>> ,
            bit_map,
            std::unordered_set<value_type, std::hash<value_type>, string_find_traits, mallocator<value_type>>>;

        constexpr static inline size_type traits_length(const value_type* s) {
            return traits_type::length(s);
        }

        constexpr static inline size_type npos_min(size_type n, const size_type x) {
            if constexpr (std::is_unsigned_v<size_type>) {
                return n < x ? n : x;
            }
            else {
                assert(n >= 0 || n == npos);
                return (n == npos || n > x) ? x : n;
            }
        }

        static string_find_helper
            get_find_helper(const value_type*s,const value_type* e);
        static size_type find(const value_type* s, const size_type n,
            const size_type off, const value_type ch);
        // only need to find once ,then use this
        // for this won't initial at first 
        static size_type find(const value_type* s1, const size_type n1,
            const size_type off, const value_type* s2, const size_type n2);
        static size_type find(const value_type* s1,const size_type n1,
            const size_type off,const string_find_helper&srch);

        static string_rfind_helper
            get_rfind_helper(const value_type* s, const value_type*e);
        static size_type rfind(const value_type* s, const size_type n,
            const size_type off, const value_type ch);
        static size_type rfind(const value_type* s1, const size_type n1,
            size_type off, const value_type* s2, const size_type n2);
        static size_type rfind(const value_type* s1,const size_type n1,
            size_type off,const string_rfind_helper&srch);

        static string_find_of_helper get_find_of_helper(const value_type*s,const value_type* e);

        static size_type find_first_of_ch(const value_type* s,
            const size_type n,size_type off,const value_type ch);
        static size_type normal_find_first_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2);
        static size_type map_find_first_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2);
        static size_type find_first_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2);
        static size_type find_first_of(const value_type*s1,
            const size_type n1,const size_type off,const string_find_of_helper& srch);

        static size_type find_last_of_ch(const value_type* s,
            const size_type n, size_type off, const value_type ch);
        static size_type normal_find_last_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2);
        static size_type map_find_last_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2);
        static size_type find_last_of(const value_type* s1,
            const size_type n1, size_type off, const value_type* s2, const size_type n2);
        static size_type find_last_of(const value_type* s1,
            const size_type n1, const size_type off, const string_find_of_helper& srch);

        static size_type find_first_not_of_ch(const value_type* s,
            const size_type n, const size_type off, const value_type ch);
        static size_type normal_find_first_not_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2);
        static size_type map_find_first_not_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2);
        static size_type find_first_not_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2);
        static size_type find_first_not_of(const value_type* s1,
            const size_type n1, const size_type off, const string_find_of_helper& srch);

        static size_type find_last_not_of_ch(const value_type* s,
            const size_type n, const size_type off, const value_type ch);
        static size_type normal_find_last_not_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2);
        static size_type map_find_last_not_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2);
        static size_type find_last_not_of(const value_type* s1,
            const size_type n1, size_type off, const value_type* s2, const size_type n2);
        static size_type find_last_not_of(const value_type* s1,
            const size_type n1, const size_type off, const string_find_of_helper& srch);

        static int compare(const value_type* s1, const size_type n1,
            const value_type* s2, const size_type n2) {
            const int r = traits_type::compare(s1, s2, std::min(n1, n2));
            return r != 0 ? r : n1 > n2 ? 1 : n1 < n2 ? -1 : 0;
        }

        static bool equal(const value_type* s1, const size_type n1,
            const value_type* s2, const size_type n2) {
            return (n1 == n2) && (traits_type::compare(s1, s2, n1) == 0);
        }
        
        static bool starts_with(const value_type*s,const size_type n,const value_type ch) {
            return (n != 0) && (traits_type::eq(*s,ch));
        }

        static bool starts_with(const value_type* s1, const size_type n1,
            const value_type* s2, const size_type n2) {
            return (n1 >= n2) && (traits_type::compare(s1,s2,n2) == 0);
        }

        static bool ends_with(const value_type* s, const size_type n, const value_type ch) {
            return (n != 0) && (traits_type::eq(*s,ch));
        }

        static bool ends_with(const value_type* s1, const size_type n1,
            const value_type* s2, const size_type n2) {
            return (n1 >= n2) && (traits_type::compare(s1 + n1 - n2,s2,n2) == 0);
        }

        template<typename string_list>
        static string_list split(const value_type* s,const size_type n,
            const value_type ch,bool keep_empty_parts);

        template<typename string_list>
        static string_list split(const value_type* s1,const size_type n1,
            const value_type* s2,const size_type n2,bool keep_empty_parts);

        static size_type left_trim(const value_type* s,const size_type n);
        static size_type right_trim(const value_type* s,const size_type n);

    };

    template<typename Char,typename Traits>
    typename basic_String_traits<Char,Traits>::string_find_helper
        basic_String_traits<Char, Traits>::get_find_helper(
            const value_type* s,const value_type* e) {
        return string_find_helper(s,e);
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::find(const value_type* s, const size_type n,
            const size_type off, const value_type ch) {
        if (off < n) {
            const auto pos = traits_type::find(s + off, n - off, ch);
            if (pos) {
                return static_cast<size_type>(pos - s);
            }
        }
        return npos;
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::find(const value_type* s1, const size_type n1,
            const size_type off, const value_type* s2, const size_type n2) {

        if (n2 > n1 || off > n1 - n2) {
            return npos;
        }

        if (!n2) {
            return off;
        }

        if (n2 > 4) {
            return find(s1,n1,off,get_find_helper(s2,s2 + n2));
        }

        const auto match_end = s1 + (n1 - n2) + 1;
        for (auto match_try = s1 + off;; ++match_try) {
            match_try = traits_type::find(match_try, static_cast<size_type>(match_end - match_try), *s2);
            if (!match_try) {
                return npos;
            }
            if (traits_type::compare(match_try, s2, n2) == 0) {
                return static_cast<size_type>(match_try - s1);
            }
        }
    }

    template<typename Char,typename Traits>
    typename basic_String_traits<Char, Traits>::size_type 
        basic_String_traits<Char, Traits>::find(const value_type* s1, const size_type n1,
        const size_type off, const string_find_helper& srch) {
        const size_type n2 = srch.size();
        if (n2 > n1 || off > n1 - n2) {
            return npos;
        }

        if (!n2) {
            return off;
        }

        auto pos = srch(s1 + off,s1 + n1).first;
        const size_type diff = pos - s1;
        if (diff == n1) {
            return npos;
        }
        return diff;
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::string_rfind_helper
        basic_String_traits<Char, Traits>::get_rfind_helper(
            const value_type* s, const value_type * e) {
        return string_rfind_helper(
            std::reverse_iterator(e),std::reverse_iterator(s)
            );
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::rfind(const value_type* s,
            const size_type n, const size_type off, const value_type ch) {
        if (n != 0) {
            for (auto match_try = s + npos_min(off, n - 1);; --match_try) {
                if (traits_type::eq(*match_try, ch)) {
                    return static_cast<size_type>(match_try - s);
                }
                if (match_try == s) {
                    break;
                }
            }
        }
        return npos;
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::rfind(const value_type* s1,
            const size_type n1, size_type off, const value_type* s2, const size_type n2) {
        off = npos_min(off, n1 - n2);
        if (n2 == 0) {
            return off;
        }

        if (n2 <= n1) {
            if (n2 > 4) {
                return rfind(s1,n1,off,get_rfind_helper(s2,s2 + n2));
            }

            for (auto match_try = s1 + off;; --match_try) {
                if (traits_type::eq(*match_try, *s2) && traits_type::compare(match_try, s2, n2) == 0) {
                    return static_cast<size_type>(match_try - s1);
                }
                if (match_try == s1) {
                    break;
                }
            }
        }
        return npos;
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::rfind(const value_type* s1,
            const size_type n1, size_type off,const string_rfind_helper& srch) {
        const auto n2 = srch.size();
        off = npos_min(off, n1 - n2);
        if (n2 == 0) {
            return off;
        }

        if (n2 <= n1) {

            auto _end = std::reverse_iterator(s1);
            auto pos = srch(std::reverse_iterator(s1 + off + n2),_end).first;
            auto diff = _end - pos;
            if (!diff) {
                return npos;
            }
            return diff - n2;
        }
        return npos;
    }

    template<typename Char,typename Traits>
    typename basic_String_traits<Char,Traits>::string_find_of_helper
        basic_String_traits<Char, Traits>::get_find_of_helper(
            const value_type* s, const value_type* e) {
        return string_find_of_helper(s,e);
    }

    template<typename Char,typename Traits>
    typename basic_String_traits<Char,Traits>::size_type
        basic_String_traits<Char, Traits>::find_first_of_ch(const value_type* s,
            const size_type n, const size_type off, const value_type ch) {
        return find(s,n,off,ch);
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::normal_find_first_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
        const auto end = s1 + n1;
        for (auto match_try = s1 + off; match_try < end; ++match_try) {
            if (traits_type::find(s2, n2, *match_try)) {
                return static_cast<size_type>(match_try - s1);
            }
        }
        return npos;
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::map_find_first_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
        return find_first_of(s1,n1,off,get_find_of_helper(s2,s2+n2));
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::find_first_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
        if (n2 != 0 && off < n1) {
            if (n2 < 32) { // only need to use normal find
                return normal_find_first_of(s1, n1, off, s2, n2);
            }
            else {
                // may use Map_Search_first_of
                // firstly ,use normal find to match no more than length of 32

                const size_type first_to_search = (n1 - off <= 32 ? n1 : off + 32);
                auto first_to_search_match = normal_find_first_of(s1, first_to_search, off, s2, n2);
                if (first_to_search_match != npos) {
                    return first_to_search_match;
                }

                if (first_to_search != n1) {
                    return map_find_first_of(s1, n1, first_to_search, s2, n2);
                }

            }
        }
        return npos;
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::find_first_of(const value_type* s1,
            const size_type n1, const size_type off, const string_find_of_helper& srch) {

        const auto end = s1 + n1;
        for (auto match_try = s1 + off; match_try < end; ++match_try) {
            if (srch.count(*match_try)) {
                return static_cast<size_type>(match_try - s1);
            }
        }
        return npos;
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::find_last_of_ch(const value_type* s,
            const size_type n, size_type off, const value_type ch) {
        return rfind(s,n,off,ch);
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::normal_find_last_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
        assert(off <= n1 - 1);
        for (auto match_try = s1 + off;; --match_try) {
            if (traits_type::find(s2, n2, *match_try)) {
                return static_cast<size_type>(match_try - s1);
            }

            if (match_try == s1)
                break;
        }
        return npos;
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::map_find_last_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
        return find_last_of(s1,n1,off,get_find_of_helper(s2,s2 + n2));
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::find_last_of(const value_type* s1,
            const size_type n1, size_type off, const value_type* s2, const size_type n2) {
        off = npos_min(off, n1 - 1);
        if (n1 != 0 && n2 != 0) {
            if (n2 < 32) {
                return normal_find_last_of(s1, n1, off, s2, n2);
            }
            else {
                const size_type first_to_search = off <= 32 ? 0 : off - 32;
                auto first_to_search_match = normal_find_last_of(
                    s1 + first_to_search, n1 - first_to_search, off - first_to_search, s2, n2);
                if (first_to_search_match != npos) {
                    return first_to_search + first_to_search_match;
                }
                if (first_to_search != 0) {
                    return normal_find_last_of(s1, n1, first_to_search, s2, n2);
                }
            }
        }
        return npos;
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::find_last_of(const value_type* s1,
            const size_type n1, const size_type off, const string_find_of_helper& srch) {

        assert(off <= n1 - 1);
        for (auto match_try = s1 + off;; --match_try) {
            if (srch.count(*match_try)) {
                return static_cast<size_type>(match_try - s1);
            }

            if (match_try == s1)
                break;
        }
        return npos;
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::find_first_not_of_ch(const value_type* s,
            const size_type n, const size_type off, const value_type ch) {
        if (off < n) {
            auto match_end = s + n;
            for (auto match_try = s + off; match_try != match_end; ++match_try) {
                if (!traits_type::eq(*match_try, ch)) {
                    return static_cast<size_type>(match_try - s);
                }
            }
        }
        return npos;
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::normal_find_first_not_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
        const auto end = s1 + n1;
        for (auto match_try = s1 + off; match_try < end; ++match_try) {
            if (!traits_type::find(s2, n2, *match_try)) {
                return static_cast<size_type>(match_try - s1);
            }
        }
        return npos;
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::map_find_first_not_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
        return find_first_not_of(s1,n1,off,get_find_of_helper(s2,s2+n2));
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::find_first_not_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
        if (n2 != 0 && off < n1) {
            if (n2 < 32) { // only need to use normal find
                return normal_find_first_not_of(s1, n1, off, s2, n2);
            }
            else {
                // may use Map_Search_first_of
                // firstly ,use normal find to match no more than length of 32

                const size_type first_to_search = (n1 - off <= 32 ? n1 : off + 32);
                auto first_to_search_match = normal_find_first_not_of(s1, first_to_search, off, s2, n2);
                if (first_to_search_match != npos) {
                    return first_to_search_match;
                }

                if (first_to_search != n1) {
                    return map_find_first_not_of(s1, n1, first_to_search, s2, n2);
                }

            }
        }
        return npos;
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::find_first_not_of(const value_type* s1,
            const size_type n1, const size_type off, const string_find_of_helper& srch) {

        const auto end = s1 + n1;
        for (auto match_try = s1 + off; match_try < end; ++match_try) {
            if (!srch.count(*match_try)) {
                return static_cast<size_type>(match_try - s1);
            }
        }
        return npos;
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::find_last_not_of_ch(const value_type* s,
            const size_type n, const size_type off, const value_type ch) {
        if (n != 0) {
            for (auto match_try = s + npos_min(off, n - 1);; --match_try) {
                if (!traits_type::eq(*match_try, ch)) {
                    return static_cast<size_type>(match_try - s);
                }
                if (match_try == s) {
                    break;
                }
            }
        }
        return npos;
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::normal_find_last_not_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
        assert(off <= n1 - 1);
        for (auto match_try = s1 + off;; --match_try) {
            if (!traits_type::find(s2, n2, *match_try)) {
                return static_cast<size_type>(match_try - s1);
            }

            if (match_try == s1)
                break;
        }
        return npos;
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::map_find_last_not_of(const value_type* s1,
            const size_type n1, const size_type off, const value_type* s2, const size_type n2) {
        return find_last_not_of(s1,n1,off,get_find_of_helper(s2,s2+n2));
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::find_last_not_of(const value_type* s1,
            const size_type n1, size_type off, const value_type* s2, const size_type n2) {
        off = npos_min(off, n1 - 1);
        if (n1 != 0 && n2 != 0) {
            if (n2 < 32) {
                return normal_find_last_not_of(s1, n1, off, s2, n2);
            }
            else {
                const size_type first_to_search = off <= 32 ? 0 : off - 32;
                auto first_to_search_match = normal_find_last_not_of(
                    s1 + first_to_search, n1 - first_to_search, off - first_to_search, s2, n2);
                if (first_to_search_match != npos) {
                    return first_to_search + first_to_search_match;
                }
                if (first_to_search != 0) {
                    return normal_find_last_not_of(s1, n1, first_to_search, s2, n2);
                }
            }
        }
        return npos;
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::find_last_not_of(const value_type* s1,
            const size_type n1, const size_type off, const string_find_of_helper& srch) {

        assert(off <= n1 - 1);
        for (auto match_try = s1 + off;; --match_try) {

            if (!srch.count(*match_try)) {
                return static_cast<size_type>(match_try - s1);
            }

            if (match_try == s1)
                break;
        }
        return npos;
    }

    template<typename Char,typename Traits>
    template<typename string_list>
    string_list basic_String_traits<Char, Traits>::split(
        const value_type* s, const size_type n,
        const value_type ch, bool keep_empty_parts) {
        string_list ans;
        size_type off = 0;
        if (keep_empty_parts) {
            for (;;) {
                const size_type pos = find(s,n,off,ch);
                if (pos == npos) {
                    ans.emplace_back(s + off, s + n);
                    break;
                }
                ans.emplace_back(s + off, s + pos);
                off = pos + 1;
            }
        }
        else {
            for (;;) {
                const size_type pos = find(s,n,off,ch);
                if (pos == npos) {
                    if (off != n) {
                        ans.emplace_back(s + off, s + n);
                    }
                    break;
                }
                if (off != pos) {
                    ans.emplace_back(s + off, s + pos);
                }
                off = pos + 1;
            }
        }
        return ans;
    }

    template<typename Char,typename Traits>
    template<typename string_list>
    string_list basic_String_traits<Char, Traits>::split(
        const value_type* s1, const size_type n1,
        const value_type* s2, const size_type n2, bool keep_empty_parts) {
        string_list ans;
        if (n1 < n2) {
            if (n1 != 0 || keep_empty_parts) {
                ans.emplace_back(s1, s1 + n1);
            }
            return ans;
        }

        if (n2 == 1) {
            return split<string_list>(s1,n1,*s2,keep_empty_parts);
        }

        auto srch = get_find_helper(s2 , s2 + n2);
        size_type off = 0;
        if (keep_empty_parts) {
            for (;;) {
                const size_type pos = find(s1,n1,off,srch);
                if (pos == npos) {
                    ans.emplace_back(s1 + off, s1 + n1);
                    break;
                }
                ans.emplace_back(s1 + off, s1 + pos);
                off = pos + n2;
            }
        }
        else {
            for (;;) {
                const size_type pos = find(s1,n1,off,srch);
                if (pos == npos) {
                    if (off != n1) {
                        ans.emplace_back(s1 + off, s1 + n1);
                    }
                    break;
                }
                if (off != pos) {
                    ans.emplace_back(s1 + off, s1 + pos);
                }
                off = pos + n2;
            }
        }

        return ans;
    }

    template<typename Char,typename Traits>
    typename basic_String_traits<Char,Traits>::size_type 
        basic_String_traits<Char, Traits>::left_trim(const value_type* s, const size_type n) {
        return find_first_not_of(s,n,0,
            String_trim_helper<Char>::data(),String_trim_helper<Char>::size());
    }

    template<typename Char, typename Traits>
    typename basic_String_traits<Char, Traits>::size_type
        basic_String_traits<Char, Traits>::right_trim(const value_type* s, const size_type n) {
        return find_last_not_of(s, n, npos, 
            String_trim_helper<Char>::data(), String_trim_helper<Char>::size());
    }

    template<typename Char>
    class String_core {
        static_assert(std::is_default_constructible_v<Char>,
            "Corrupt Char type for String.");
    public:

        String_core() {
            reset();
        }

        String_core(const String_core& other) {
            if (!other.category()) {
                copySmall(other);
            }
            else {
                copyMedium(other);
            }
        }

        String_core(String_core&& other)noexcept
            : _Ml(other._Ml) {
            other.reset();
        }

        String_core(const Char* _Data, const size_t s) {
            if (s <= maxSmallSize) {
                initSmall(_Data, s);
            }
            else {
                initMedium(_Data, s);
            }
        }

        String_core& operator=(const String_core&) = delete;

        ~String_core()noexcept {
            if (!category())return;
            Getal().deallocate(_Ml._Data, _Ml.capacity() + 1);
        }

        void swap(String_core& other) {
            const auto t = _Ml;
            _Ml = other._Ml;
            other._Ml = t;
        }

        Char* data() {
            return category() ? _Ml._Data : _Small;
        }

        const Char* data()const {
            return category() ? _Ml._Data : _Small;
        }

        const Char* c_str()const {
            return category() ? _Ml._Data : _Small;
        }

        bool empty()const {
            return !size();
        }

        void shrink(const size_t delta) {
            if (!category())
                shrinkSmall(delta);
            else shrinkMedium(delta);
        }

        void reserve(const size_t s) {
            if (!category())
                reserveSmall(s);
            else reserveMedium(s);
        }

        // expand size of n without init
        Char* expandNoinit(const size_t n, bool expGrowth = false);

        void push_back(Char c) { *expandNoinit(1,true) = c; }

        size_t size()const {
            return category() ? MediumSize() : SmallSize();
        }

        size_t capacity()const {
            return category() ? _Ml.capacity() : maxSmallSize;
        }

        void shrink_to_fit();

        static mallocator<Char>& Getal() {
            static mallocator<Char> _alloc;
            return _alloc;
        }

    private:
        Char* c_str() {
            return category() ? _Ml._Data : _Small;
        }

        struct _Medium {
            Char* _Data;
            size_t _Size;
            size_t _Capacity;
            inline size_t capacity()const {
                return _Capacity & capacityExtractMask;
            }
        };
        constexpr static size_t lastChar = sizeof(_Medium) - 1;
        constexpr static size_t maxSmallSize = lastChar / sizeof(Char);
        constexpr static uint8_t categoryExtractMask = 0x80;
        constexpr static size_t kShift = (sizeof(size_t) - 1) * 8;
        constexpr static size_t capacityExtractMask = ~((size_t)categoryExtractMask << kShift);
        constexpr static size_t capacityOr = ~capacityExtractMask;

        static_assert(
            !(sizeof(_Medium) % sizeof(Char)),
            "Corrupt memory layout for String.");

        union {
            uint8_t _Byte[sizeof(_Medium)];
            Char _Small[sizeof(_Medium) / sizeof(Char)];
            _Medium _Ml;
        };

        inline bool category()const {
            return _Byte[lastChar] == categoryExtractMask;
        }

        void setCapacity(const size_t c) {
            _Ml._Capacity = c | capacityOr;
        }

        void reset() { setSmallSize(0); }

        void setSmallSize(const size_t s) {
            assert(s <= maxSmallSize);
            _Byte[lastChar] = static_cast<uint8_t>(maxSmallSize - s);
            _Small[s] = '\0';
        }

        void setMediumSize(const size_t s) {
            _Ml._Size = s;
            _Ml._Data[s] = '\0';
        }

        size_t SmallSize()const {
            return maxSmallSize - _Byte[lastChar];
        }

        size_t MediumSize()const {
            return _Ml._Size;
        }

        void initSmall(const Char* _Data, const size_t s) {
            memcpy(_Small, _Data, sizeof(Char) * s);
            setSmallSize(s);
        }

        void initMedium(const Char* _Data, const size_t s) {
            _Ml._Data = Getal().allocate(s + 1);
            memcpy(_Ml._Data, _Data, sizeof(Char) * s);
            setMediumSize(s);
            setCapacity(s);
        }

        void copySmall(const String_core& other) {
            _Ml = other._Ml;
        }

        void copyMedium(const String_core& other) {
            const size_t s = other.size();
            _Ml._Data = Getal().allocate(s + 1);
            memcpy(_Ml._Data, other._Ml._Data, sizeof(Char) * s);
            setMediumSize(s);
            setCapacity(s);
        }

        void shrinkSmall(const size_t delta) {
            setSmallSize(SmallSize() - delta);
        }

        void shrinkMedium(const size_t delta) {
            setMediumSize(MediumSize() - delta);
        }

        void reserveSmall(const size_t s) ;

        void reserveMedium(const size_t s);

    };

    template<typename Char>
    Char* String_core<Char>::expandNoinit(const size_t n, bool expGrowth ) {
        size_t siz, newsiz;
        if (!category()) {
            siz = SmallSize();
            newsiz = siz + n;
            if (newsiz <= maxSmallSize) {
                setSmallSize(newsiz);
                return _Small + siz;
            }
            reserveSmall(expGrowth ? std::max(newsiz, 2 * maxSmallSize) : newsiz);
        }
        else {
            siz = _Ml._Size;
            newsiz = siz + n;
            const auto _Old_Capacity = _Ml.capacity();
            if (newsiz > _Old_Capacity) {
                reserveMedium(expGrowth ? std::max(newsiz,
                    1 + _Old_Capacity + _Old_Capacity / 2) : newsiz);
            }
        }
        setMediumSize(newsiz);
        return _Ml._Data + siz;
    }

    template<typename Char>
    void String_core<Char>::shrink_to_fit() {
        // if is small then do nothing
        if (category()) {
            const size_t _size = MediumSize();
            const size_t _capacity = _Ml.capacity();
            if (_capacity < _size * 9 / 8) {
                return ;
            }
            const Char* _data = _Ml._Data;
            auto& al = Getal();
            if (_size <= maxSmallSize) {
                initSmall(_data, _size);
            }
            else {
                initMedium(_data, _size);
            }
            al.deallocate(_data, _capacity + 1);
        }
    }

    template<typename Char>
    void String_core<Char>::reserveSmall(const size_t s) {
        if (s <= maxSmallSize)return;
        const size_t new_size = std::max(s, 2 * maxSmallSize);
        const auto _New_Data = Getal().allocate(new_size + 1);
        const auto _Old_Size = SmallSize();
        memcpy(_New_Data, _Small, sizeof(Char) * _Old_Size);
        _Ml._Data = _New_Data;
        setMediumSize(_Old_Size);
        setCapacity(new_size);
    }

    template<typename Char>
    void String_core<Char>::reserveMedium(const size_t s) {
        const auto _Old_Capacity = _Ml.capacity();
        if (s <= _Old_Capacity)
            return;

        const size_t new_size = std::max(s, 1 + _Old_Capacity + _Old_Capacity / 2);
        auto Al = Getal();
        const auto _New_Data = Al.allocate(new_size + 1);
        const auto _Old_Size = _Ml._Size;
        memcpy(_New_Data, _Ml._Data, sizeof(Char) * _Old_Size);
        Al.deallocate(_Ml._Data, _Old_Capacity + 1);
        _Ml._Data = _New_Data;
        setMediumSize(_Old_Size);
        setCapacity(new_size);
    }

    template<typename Char,typename Traits = std::char_traits<Char>>
    class basic_String_view {
    private:
        using default_traits = basic_String_traits<Char, Traits>;
        template<typename T>
        using non_default_traits = basic_String_traits<Char, T>;

        using Elem = Char;
        template <class iter>
        using is_char_ptr =
            std::bool_constant<is_any_of_v<iter, const Elem* const, const Elem*, Elem* const, Elem*>>;

        // is const iterator -> char pointer
        template<typename iter>
        using is_const_iterator = std::enable_if_t<is_char_ptr<iter>::value, int>;

    public:

        using traits_type = Traits;
        using allocator_type = mallocator<Char>;
        using string_traits = default_traits;

        using value_type = Char;
        using reference = value_type&;
        using const_reference = const value_type&;

        using size_type = size_t;
        using difference_type = size_t;

        using pointer = value_type*;
        using const_pointer = const value_type*;

        using iterator = value_type*;
        using const_iterator = const value_type*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        constexpr static size_type npos = default_traits::npos;

    private:
        constexpr static inline size_type traits_length(const value_type* s) {
            return default_traits::traits_length(s);
        }
        constexpr static inline size_type npos_min(size_type n, const size_type x) {
            return default_traits::npos_min(n, x);
        }

    public:

        constexpr basic_String_view() noexcept = default;
        constexpr basic_String_view(const basic_String_view&) noexcept = default;
        constexpr basic_String_view& operator=(const basic_String_view&) noexcept = default;

        template<typename C,typename T,typename A>
        constexpr basic_String_view(const std::basic_string<C, T, A>& s)
            : Myfirst(s.data()), Mysize(s.size()) {

        }

        constexpr basic_String_view(const const_pointer s) noexcept
            : Myfirst(s), Mysize(traits_length(s)) {

        }

        constexpr basic_String_view(const const_pointer s, const size_type n) noexcept
            : Myfirst(s), Mysize(n) {

        }

        constexpr const_iterator begin()const noexcept{ 
            return Myfirst;
        }

        constexpr const_iterator cbegin()const noexcept{ 
            return Myfirst;
        }

        constexpr const_iterator end()const noexcept {
            return Myfirst + Mysize;
        }

        constexpr const_iterator cend()const noexcept {
            return Myfirst + Mysize;
        }

        constexpr const_reverse_iterator rbegin()const noexcept {
            return const_reverse_iterator(end()); 
        }
        
        constexpr const_reverse_iterator crbegin()const noexcept {
            return const_reverse_iterator(cend()); 
        }

        constexpr const_reverse_iterator rend()const noexcept {
            return const_reverse_iterator(begin()); 
        }
        
        constexpr const_reverse_iterator crend()const noexcept {
            return const_reverse_iterator(begin()); 
        }

        constexpr const_reference front()const noexcept {
            return *begin(); 
        }

        constexpr const_reference back()const noexcept {
            assert(!empty());
            return *(end() - 1);
        }

        constexpr size_type size() const noexcept {
            return Mysize;
        }

        constexpr size_type length() const noexcept {
            return Mysize;
        }

        constexpr size_type max_size() const noexcept { return std::numeric_limits<size_type>::max(); }

        constexpr const_reference operator[](const size_type index)const noexcept {
            assert(0 <= index && index < size());
            return *(begin() + index);
        }

        constexpr const_reference at(const size_type index)const noexcept {
            assert(0 <= index && index < size());
            return *(begin() + index);
        }

        constexpr void remove_prefix(const size_type n)noexcept {
            Myfirst += n;
            Mysize -= n;
        }

        constexpr void remove_suffix(const size_type n)noexcept {
            Mysize -= n;
        }

        constexpr void swap(basic_String_view & other)noexcept { 
            const basic_String_view temp(*this);
            *this = other;
            other = temp;
        }

        constexpr const value_type* c_str() const noexcept {
            return Myfirst;
        }

        constexpr const value_type* data() const noexcept{ 
            return Myfirst;
        }

        constexpr bool empty()const noexcept{ 
            return !Mysize;
        }

        template<typename T = Traits>
        constexpr int compare(const basic_String_view & other)const noexcept{
            return non_default_traits<T>::compare(data(),size(),other.data(),other.size());
        }

        template<typename T = Traits>
        constexpr bool equal(const basic_String_view & other)const noexcept{
            return non_default_traits<T>::equal(data(),size(),other.data(),other.size());
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_helper
            get_find_helper(const value_type * s, const value_type * e) {
            return non_default_traits<T>::get_find_helper(s, e);
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_helper
            get_find_helper(const value_type * s, const size_type n) {
            return get_find_helper<T>(s, s + n);
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_helper
            get_find_helper(const value_type * s) {
            return get_find_helper<T>(s, s + traits_length(s));
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_helper
            get_find_helper(const basic_String_view & s) {
            return get_find_helper<T>(s.data(), s.size());
        }

        template<typename T = Traits>
        constexpr size_type find(const value_type ch)const noexcept{
            return non_default_traits<T>::find(data(), size(), 0, ch);
        }

        template<typename T = Traits>
        constexpr size_type find(const basic_String_view & other)const noexcept{
            return non_default_traits<T>::find(data(),size(),0,other.data(),other.size());
        }

        template<typename T = Traits>
        constexpr size_type find(const typename non_default_traits<T>::string_find_helper & srch)const noexcept{
            return non_default_traits<T>::find(data(), size(), 0, srch);
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_rfind_helper
            get_rfind_helper(const value_type * s, const value_type * e) {
            return non_default_traits<T>::get_rfind_helper(s, e);
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_rfind_helper
            get_rfind_helper(const value_type * s, const size_type n) {
            return get_rfind_helper<T>(s, s + n);
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_helper
            get_rfind_helper(const value_type * s) {
            return get_rfind_helper<T>(s, s + traits_length(s));
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_helper
            get_rfind_helper(const basic_String_view & s) {
            return get_rfind_helper<T>(s.data(), s.size());
        }

        template<typename T = Traits>
        constexpr size_type rfind(const value_type ch)const noexcept{
            return non_default_traits<T>::rfind(data(), size(), 0, ch);
        }

        template<typename T = Traits>
        constexpr size_type rfind(const basic_String_view & other)const noexcept{
            return non_default_traits<T>::rfind(data(),size(),0,other.data(),other.size());
        }

        template<typename T = Traits>
        constexpr size_type rfind(const typename non_default_traits<T>::string_rfind_helper & srch)const noexcept{
            return non_default_traits<T>::rfind(data(), size(), 0, srch);
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_of_helper get_find_of_helper(
            const value_type * s, const value_type * e
        ) {
            return non_default_traits<T>::get_find_of_helper(s, e);
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_of_helper get_find_of_helper(
            const value_type * s, const size_type n
        ) {
            return get_find_of_helper<T>(s, s + n);
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_of_helper get_find_of_helper(
            const value_type * s
        ) {
            return get_find_of_helper<T>(s, s + traits_length(s));
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_of_helper get_find_of_helper(
            const basic_String_view & s
        ) {
            return get_find_of_helper<T>(s.data(), s.size());
        }

        template<typename T = Traits>
        constexpr size_type find_first_of(const value_type ch)const noexcept{
            return non_default_traits<T>::find_first_of_ch(data(), size(), 0, ch);
        }

        template<typename T = Traits>
        constexpr size_type find_first_of(const basic_String_view & other)const noexcept{
            return non_default_traits<T>::find_first_of(data(),size(),0,other.data(),other.size());
        }

        template<typename T = Traits>
        constexpr size_type find_first_of(const typename 
            non_default_traits<T>::string_find_of_helper & srch)const noexcept{
            return non_default_traits<T>::find_first_of(data(), size(), 0, srch);
        }

        template<typename T = Traits>
        constexpr size_type find_last_of(const value_type ch)const noexcept{
            return non_default_traits<T>::find_last_of_ch(data(), size(), 0, ch);
        }

        template<typename T = Traits>
        constexpr size_type find_last_of(const basic_String_view & other)const noexcept{
            return non_default_traits<T>::find_last_of(data(),size(),0,other.data(),other.size());
        }

        template<typename T = Traits>
        constexpr size_type find_last_of(const typename 
            non_default_traits<T>::string_find_of_helper & srch)const noexcept{
            return non_default_traits<T>::find_last_of(data(), size(), 0, srch);
        }

        template<typename T = Traits>
        constexpr size_type find_first_not_of(const value_type ch)const noexcept{
            return non_default_traits<T>::find_first_not_of_ch(data(), size(), 0, ch);
        }

        template<typename T = Traits>
        constexpr size_type find_first_not_of(const basic_String_view & other)const noexcept{
            return non_default_traits<T>::find_first_not_of(data(),size(),0,other.data(),other.size());
        }

        template<typename T = Traits>
        constexpr size_type find_first_not_of(const typename 
            non_default_traits<T>::string_find_of_helper & srch)const noexcept{
            return non_default_traits<T>::find_first_not_of(data(), size(), 0, srch);
        }

        template<typename T = Traits>
        constexpr size_type find_last_not_of(const value_type ch)const noexcept{
            return non_default_traits<T>::find_last_not_of_ch(data(), size(), 0, ch);
        }

        template<typename T = Traits>
        constexpr size_type find_last_not_of(const basic_String_view & other)const noexcept{
            return non_default_traits<T>::find_last_not_of(data(),size(),0,other.data(),other.size());
        }

        template<typename T = Traits>
        constexpr size_type find_last_not_of(const typename 
            non_default_traits<T>::string_find_of_helper & srch)const noexcept{
            return non_default_traits<T>::find_last_not_of(data(), size(), 0, srch);
        }

        template<typename T = Traits>
        constexpr bool starts_with(const value_type ch)const noexcept{
            return non_default_traits<T>::starts_with(data(), size(), ch);
        }

        template<typename T = Traits>
        constexpr bool starts_with(const basic_String_view & other)const noexcept{
            return non_default_traits<T>::starts_with(data(),size(),other.data(),other.size());
        }

        template<typename T = Traits>
        constexpr bool ends_with(const value_type ch)const noexcept{
            return non_default_traits<T>::ends_with(data(), size(), ch);
        }

        template<typename T = Traits>
        constexpr bool ends_with(const basic_String_view& other)const noexcept {
            return non_default_traits<T>::ends_with(data(),size(),other.data(),other.size());
        }

        constexpr basic_String_view substr(const size_type off = 0, const size_type n = npos) {
            return { Myfirst + off , npos_min(n,size() - off) };
        }

        template<typename T = Traits, typename string_list = std::vector<basic_String_view>>
        string_list split(const value_type ch, bool keep_empty_parts = true);

        template<typename T = Traits, typename string_list = std::vector<basic_String_view>>
        string_list split(const basic_String_view & other, bool keep_empty_parts = true);

        template<typename T = Traits, typename string_list = std::vector<basic_String_view>>
        string_list split(const value_type * s, bool keep_empty_parts = true);

        template<typename T = Traits, typename string_list = std::vector<basic_String_view>>
        string_list split(const value_type * s,
            const size_type n, bool keep_empty_parts = true);

        basic_String_view trim() {
            auto l = default_traits::left_trim(Myfirst,Mysize);
            if (l == npos) {
                return {Myfirst,0};
            }
            auto r = default_traits::right_trim(Myfirst,Mysize);
            return {Myfirst + l,r - l + 1};
        }
        
    private:
        const_pointer Myfirst;
        size_type Mysize;
    };

    template<typename Char,typename Traits>
    template<typename T, typename string_list>
    string_list basic_String_view<Char, Traits>::split(const value_type ch, bool keep_empty_parts) {
        return non_default_traits<T>::template split<string_list>(data(),size(),ch,keep_empty_parts);
    }

    template<typename Char, typename Traits>
    template<typename T, typename string_list>
    string_list basic_String_view<Char, Traits>::split(const basic_String_view& other, bool keep_empty_parts) {
        return split<T,string_list>(other.data(),other.size(),keep_empty_parts);
    }

    template<typename Char, typename Traits>
    template<typename T, typename string_list>
    string_list basic_String_view<Char, Traits>::split(const value_type* s, bool keep_empty_parts) {
        return split<T,string_list>(s,traits_length(s),keep_empty_parts);
    }

    template<typename Char, typename Traits>
    template<typename T, typename string_list>
    string_list basic_String_view<Char, Traits>::split(
        const value_type* s,
        const size_type n, bool keep_empty_parts) {
        return non_default_traits<T>::template split<string_list>(data(),size(),s,n,keep_empty_parts);
    }

    template<typename Char,typename Traits = 
        std::char_traits<Char>,typename Core = String_core<Char>>
    class basic_String  {
    private:
        using default_traits = basic_String_traits<Char, Traits>;
        template<typename T>
        using non_default_traits = basic_String_traits<Char, T>;

        using Elem = Char;
        template <class iter>
        using is_char_ptr =
            std::bool_constant<is_any_of_v<iter, const Elem* const, const Elem*, Elem* const, Elem*>>;

        // is const iterator -> char pointer
        template<typename iter>
        using is_const_iterator = std::enable_if_t<is_char_ptr<iter>::value, int>;

    public:

        using traits_type = Traits;
        using allocator_type = mallocator<Char>;
        using string_traits = default_traits;

        using value_type = Char;
        using reference = value_type&;
        using const_reference = const value_type&;

        using size_type = size_t;
        using difference_type = size_t;

        using pointer = value_type*;
        using const_pointer = const value_type*;

        using iterator = value_type*;
        using const_iterator = const value_type*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        constexpr static size_type npos = default_traits::npos;

    private:

        constexpr static inline size_type traits_length(const value_type* s) {
            return default_traits::traits_length(s);
        }

        constexpr static inline size_type npos_min(size_type n, const size_type x) {
            return default_traits::npos_min(n,x);
        }

    public:

        basic_String()= default;

        basic_String(const basic_String & other)
            : core(other.core) {

        }

        basic_String(basic_String && other) noexcept
            : core(std::move(other.core)) {
            
        }

        // other[pos...pos + n - 1]
        // if (n == npos || n > other.size() - pos)) then copy other[pos...(other.size() - 1)]
        basic_String(const basic_String & other, const size_type pos, const size_type n = npos)
            : core(other.data() + pos, npos_min(n, other.size() - pos)) {

        }

        basic_String(const value_type ch) {
            push_back(ch);
        }

        basic_String(const value_type * s)
            : core(s, traits_length(s)) {

        }

        basic_String(const value_type * s, const size_type n)
            : core(s, n) {

        }

        basic_String(const value_type * first, const value_type * last)
            : core(first, static_cast<size_type>(last - first)) {

        }

        // for diffierent core and traits type of T
        template<typename T, typename C>
        basic_String(const basic_String<Char, T, C>&other)
            : core(other.data(), other.size()) {

        }

        // for same core and different traits
        // only need to copy core
        template<typename T>
        basic_String(const basic_String<Char, T, Core>&other)
            : core(other.core) {

        }

        // for same core and different traits
        // only need to move core,for traits won't affect the core
        template<typename T>
        basic_String(basic_String<Char, T, Core>&& other)
            : core(std::move(other.core)) {

        }

        template<typename T,typename A>
        basic_String(const std::basic_string<Char, T, A>& other)
            : core(other.data(), other.size()) {

        }

        basic_String(const size_type n, const value_type c) {
            auto const pData = core.expandNoinit(n);
            std::fill(pData, pData + n, c);
        }

        template<typename iter>
        basic_String(iter first, iter last) {
            assign(first, last, is_char_ptr<iter>{});
        }

        basic_String(std::initializer_list<value_type> il) {
            assign(il.begin(), il.end());
        }

        basic_String(const basic_String_view<Char,Traits>& s)
            : core(s.data(), s.size()) {

        }

        basic_String(const std::basic_string_view<Char, Traits>& s)
            : core(s.data(), s.size()) {

        }

        operator std::basic_string_view<value_type, traits_type>() const noexcept {
            return { data(), size() };
        }

        operator basic_String_view<value_type, traits_type>() const noexcept {
            return { data(), size() };
        }

        ~basic_String() noexcept = default;

        template<typename T,typename fs = std::char_traits<T>>
        static basic_String from(const T* s) {
            basic_String it(s, s + basic_String_traits<T, fs>::traits_length(s));
            return it;
        }

        template<typename T>
        static basic_String from(const T* s, const size_type n) {
            basic_String it(s, s + n);
            return it;
        }

        basic_String& operator=(const basic_String& other) {
            if (this != std::addressof(other)) {
                assign(other.data(), other.size());
            }
            return *this;
        }

        basic_String& operator=(basic_String&& other)noexcept {
            if (this != std::addressof(other)) {
                this->~basic_String();
                new (&core) Core(std::move(other.core));
            }
            return *this;
        }

        template<typename T,typename C>
        basic_String& operator=(const basic_String<Char, T, C>& other) {
            if (this != std::addressof(other)) {
                assign(other.data(),other.size());
            }
            return *this;
        }

        template<typename T>
        basic_String& operator=(const basic_String<Char, T, Core>& other) {
            if (this != std::addressof(other)) {
                assign(other.data(), other.size());
            }
            return *this;
        }

        template<typename T>
        basic_String& operator=(basic_String<Char, T, Core>&& other) {
            if (this != std::addressof(other)) {
                this->~basic_String();
                new (&core) Core(std::move(other.core));
            }
            return *this;
        }

        basic_String& operator=(const value_type* str) {
            return assign(str);
        }

        template<typename _Traits, typename _Alloc>
        basic_String& operator=(const std::basic_string<Char, _Traits, _Alloc>& str) {
            return assign(str.data(), str.size());
        }

        basic_String& operator=(const value_type c) {
            resize(1);
            *begin() = c;
            return *this;
        }

        iterator begin() { return core.data(); }
        const_iterator begin()const { return core.data(); }
        const_iterator cbegin()const { return begin(); }

        iterator end() { return core.data() + core.size(); }
        const_iterator end()const { return core.data() + core.size(); }
        const_iterator cend()const { return end(); }

        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin()const { return const_reverse_iterator(end()); }
        const_reverse_iterator crbegin()const { return const_reverse_iterator(cend()); }

        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend()const { return const_reverse_iterator(begin()); }
        const_reverse_iterator crend()const { return const_reverse_iterator(begin()); }

        reference front() { return *begin(); }
        const_reference front()const { return *begin(); }

        reference back() {
            assert(!empty());
            return *(end() - 1);
        }

        const_reference back()const {
            assert(!empty());
            return *(end() - 1);
        }

        void resize(const size_type n, const value_type c = value_type());

        void reserve(const size_type c) {
            core.reserve(c);
            assert(capacity() >= c);
        }

        void shrink_to_fit() {
            core.shrink_to_fit();
        }

        void clear() { 
            resize(0); 
        }

        size_type size() const {
            return core.size();
        }

        size_type length() const {
            return core.size();
        }

        size_type max_size() const { return std::numeric_limits<size_type>::max(); }

        size_type capacity() const {
            return core.capacity();
        }

        reference operator[](const size_type index) {
            assert(0 <= index && index < size());
            return *(begin() + index);
        }

        const_reference operator[](const size_type index)const {
            assert(0 <= index && index < size());
            return *(begin() + index);
        }

        reference at(const size_type index) {
            assert(0 <= index && index < size());
            return *(begin() + index);
        }

        const_reference at(const size_type index)const {
            assert(0 <= index && index < size());
            return *(begin() + index);
        }

        basic_String& operator+=(const basic_String& other) { return append(other); }
        basic_String& operator+=(const value_type* s) { return append(s); }
        basic_String& operator+=(const value_type c) { push_back(c); return *this; }
        basic_String& operator+=(std::initializer_list<value_type> il) {
            append(il);
            return *this;
        }

        basic_String& append(const basic_String& other) {
            return append(other.data(), other.size());
        }

        basic_String& append(
            const basic_String& other, const size_type pos, const size_type n = npos) {
            return append(other.data() + pos, npos_min(n, other.size() - pos));
        }

        basic_String& append(const value_type* s, size_type n);

        basic_String& append(const value_type* s) {
            return append(s, traits_length(s));
        }

        basic_String& append(const size_type n, const value_type c) {
            auto pData = core.expandNoinit(n, true);
            std::fill(pData, pData + n, c);
            return *this;
        }

        basic_String& append(const value_type* s, const value_type* e) {
            return append(s, static_cast<size_type>(e - s));
        }

    private:
        template<typename iter>
        basic_String& append(iter first, iter last, std::true_type) {
            return append((const value_type*)first, (const value_type*)last);
        }

        template<typename iter>
        basic_String& append(iter first, iter last, std::false_type) {
            return append(basic_String(first, last));
        }
    public:

        template<typename iter>
        basic_String& append(iter first, iter last) {
            return append(first, last, is_char_ptr<iter>{});
        }

        basic_String& append(std::initializer_list<value_type> il) {
            return append(il.begin(), il.end());
        }

        basic_String& append(const value_type c) {
            push_back(c);
            return *this;
        }

        basic_String& prepend(const basic_String& other) {
            return prepend(other.data(), other.size());
        }

        basic_String& prepend(const basic_String& other, const size_type pos, const size_type n = npos) {
            return prepend(other.data() + pos,npos_min(n,other.size() - pos));
        }

        basic_String& prepend(const value_type* s,size_type n);

        basic_String& prepend(const value_type* s) {
            return prepend(s,traits_length(s));
        }

        basic_String& prepend(const size_type n, const value_type c) {
            auto _size = size();
            core.expandNoinit(n,true);
            auto _data = data();
            memmove(_data + n,_data,sizeof(value_type) * _size);
            std::fill(_data,_data + n,c);
            return *this;
        }

        basic_String& prepend(const value_type* s, const value_type* e) {
            return prepend(s,static_cast<size_type>(e-s));
        }

    private:
        template<typename iter>
        basic_String& prepend(iter first, iter last, std::true_type) {
            return prepend((const value_type*)first,(const value_type*)last);
        }

        template<typename iter>
        basic_String& prepend(iter first, iter last, std::false_type) {
            return prepend(basic_String(first,last));
        }

    public:
        template<typename iter>
        basic_String& prepend(iter first, iter last) {
            return prepend(first, last, is_char_ptr<iter>{});
        }

        basic_String& prepend(std::initializer_list<value_type> il) {
            return prepend(il.begin(),il.end());
        }

        basic_String& prepend(const value_type c) {
            return prepend(1,c);
        }

        void push_back(const value_type c) {
            core.push_back(c);
        }

        void pop_back() {
            assert(!empty());
            core.shrink(1);
        }

        basic_String& assign(const basic_String& other) {
            return (*this) = other;
        }

        basic_String& assign(basic_String&& other) {
            return (*this) = std::move(other);
        }

        basic_String& assign(const basic_String& other, const size_type pos, const size_type n = npos) {
            return assign(other.data() + pos, npos_min(n, other.size() - pos));
        }

        basic_String& assign(const value_type* s, const value_type* e) {
            return assign(s, static_cast<size_type>(e - s));
        }

        basic_String& assign(const value_type* s) {
            assign(s, traits_length(s));
            return *this;
        }

        basic_String& assign(const value_type* s, const size_type n); 

    private:
        template<typename iter>
        basic_String& assign(iter first, iter last, std::input_iterator_tag) {
            clear();
            for (; first != last; ++first)
                push_back(static_cast<value_type>(*first));
            return *this;
        }

        template<typename iter>
        basic_String& assign(iter first, iter last, std::forward_iterator_tag) {
            resize(static_cast<size_type>(std::distance(first, last)));
            auto _data = data();
            for (; first != last; ++_data, ++first)
                *_data = static_cast<value_type>(*first);
            return *this;
        }

        template<typename iter>
        basic_String& assign(iter first, iter last, std::true_type) {
            return assign(static_cast<const value_type*>(first), static_cast<size_type>(last - first));
        }

        template<typename iter>
        basic_String& assign(iter first, iter last, std::false_type) {
            return assign(first, last, typename std::iterator_traits<iter>::iterator_category{});
        }

    public:

        template<typename iter>
        basic_String& assign(iter first, iter last) {
            return assign(first, last, is_char_ptr<iter>{});
        }

        basic_String& insert(const size_type pos, const basic_String& str) {
            return insert(pos, str.data(), str.size());
        }

        basic_String& insert(const size_type pos1,
            const basic_String& s, const size_type pos2, const size_type n = npos) {
            return insert(pos1, s.data() + pos2, npos_min(n, s.size() - pos2));
        }

        basic_String& insert(const size_type pos, const value_type* s, const size_type n);

        basic_String& insert(const size_type pos, const value_type* s, const value_type* e) {
            return insert(pos, s, static_cast<size_type>(e - s));
        }

        basic_String& insert(const size_type pos, const value_type* str) {
            return insert(pos, str, traits_length(str));
        }

        basic_String& insert(const size_type pos, const value_type c) {
            return insert(pos, c, 1);
        }

        basic_String& insert(const size_type pos, const size_type n, const value_type c);

    private:
        template<typename iter>
        basic_String& insert(const size_type pos, iter first, iter last, std::true_type) {
            return insert(pos, (const value_type*)first, (const value_type*)last);
        }
        template<typename iter>
        basic_String& insert(const size_type pos, iter first, iter last, std::false_type) {
            return insert(pos, basic_String(first, last));
        }
    public:

        template<typename iter>
        basic_String& insert(const size_type pos, iter first, iter last) {
            return insert(pos, first, last, is_char_ptr<iter>{});
        }

        basic_String& insert(const size_type pos, std::initializer_list<value_type> il) {
            return insert(pos, il.begin(), il.end());
        }

        template<typename T, is_const_iterator<T> = 0>
        iterator insert(T p, const basic_String& str) {
            const size_type pos = p - data();
            insert(pos, str);
            return data() + pos;
        }

        template<typename T, is_const_iterator<T> = 0>
        iterator insert(T p,
            const basic_String& s, const size_type pos2, const size_type n = npos) {
            const size_type pos1 = p - data();
            insert(pos1, s, pos2, n);
            return data() + pos1;
        }

        template<typename T, is_const_iterator<T> = 0>
        iterator insert(T p, const value_type* s, const size_type n) {
            const size_type pos = p - data();
            insert(pos, s, n);
            return data() + pos;
        }

        template<typename T, is_const_iterator<T> = 0>
        iterator insert(T p, const value_type* s, const value_type* e) {
            const size_type pos = p - data();
            insert(pos, s, e);
            return data() + pos;
        }

        template<typename T, is_const_iterator<T> = 0>
        iterator insert(T p, const value_type* s) {
            const size_type pos = p - data();
            insert(pos, s);
            return data() + pos;
        }

        template<typename T, is_const_iterator<T> = 0>
        iterator insert(T p, const value_type c) {
            const size_type pos = p - data();
            insert(pos, c);
            return data() + pos;
        }

        template<typename T, is_const_iterator<T> = 0>
        iterator insert(T p, const size_type n, const value_type c) {
            const size_type pos = p - data();
            insert(pos, n, c);
            return data() + pos;
        }

        template<typename T, typename iter, is_const_iterator<T> = 0>
        iterator insert(T p, iter first, iter last) {
            const size_type pos = p - data();
            insert(pos, first, last);
            return data() + pos;
        }

        template<typename T, is_const_iterator<T> = 0>
        iterator insert(T p, std::initializer_list<value_type> il) {
            const size_type pos = p - data();
            insert(pos, il);
            return data() + pos;
        }

        basic_String& erase(const size_type pos = 0, size_type n = npos);

        template<typename T, is_const_iterator<T> = 0>
        iterator erase(T p) {
            auto _data = data();
            const size_type pos(p - _data);
            erase(pos, 1);
            return _data + pos;
        }

        template<typename T, is_const_iterator<T> = 0>
        iterator erase(T first, T last) {
            const auto _data = begin();
            const size_type pos(first - _data);
            erase(pos, static_cast<size_t>(last - first));
            return _data + pos;
        }

        template<typename T = Traits>
        basic_String& remove(const value_type ch, const size_type off = 0) {
            return remove(&ch,off,1);
        }

        template<typename T = Traits>
        basic_String& remove(const value_type* s,const size_type off,const size_type n);

        template<typename T = Traits>
        basic_String& remove(const value_type* s, const size_type n) {
            return remove<T>(s,0,n);
        }

        template<typename T = Traits>
        basic_String& remove(const value_type* s) {
            return remove<T>(s,0,traits_length(s));
        }

        template<typename T = Traits>
        basic_String& remove(const basic_String& other,const size_type off = 0) {
            return remove<T>(other.data(),off,other.size());
        }

        basic_String& replace(const size_type pos, const size_type n, const value_type ch) {
            return replace(pos,n,&ch,1);
        }

        basic_String& replace(
            const size_type pos, const size_type n, const basic_String& s) {
            return replace(pos, n, s.data(), s.size());
        }

        basic_String& replace(
            const size_type pos1, const size_type n1,
            const basic_String& s, const size_type pos2, const size_type n2 = npos) {
            return replace(pos1, n1, s.data() + pos2, npos_min(n2, s.size() - pos2));
        }

        basic_String& replace(const size_type pos,
            const size_type n, const value_type* s) {
            return replace(pos, n, s, traits_length(s));
        }

        basic_String& replace(const size_type pos, const size_type n1
            , const value_type* s, const size_type n2);

        template<typename T, is_const_iterator<T> = 0>
        basic_String& replace(T first, T last, const basic_String& s) {
            return replace(first - data(), last - first, s);
        }

        template<typename T, is_const_iterator<T> = 0>
        basic_String& replace(T first, T last, const basic_String& s,
            const size_type pos, const size_type n = npos) {
            return replace(first - data(), last - first, s, pos, n);
        }

        template<typename T, is_const_iterator<T> = 0>
        basic_String& replace(T first, T last, const value_type* s) {
            return replace(first - data(), last - first, s);
        }

        template<typename T, is_const_iterator<T> = 0>
        basic_String& replace(T first, T last, const value_type* s, const size_type n) {
            return replace(first - data(), last - first, s, n);
        }

        template<typename T = Traits>
        basic_String& replace(const value_type before,const value_type after);

        template<typename T = Traits>
        basic_String& replace(const value_type*s1,const size_type n1,
            const value_type* s2,const size_type n2);

        template<typename T = Traits>
        basic_String& replace(const value_type* s1, const value_type* s2) {
            return replace<T>(s1,traits_length(s1),s2,traits_length(s2));
        }

        template<typename T = Traits>
        basic_String& replace(const basic_String& before, const basic_String& after) {
            return replace<T>(before.data(),before.size(),after.data(),after.size());
        }

        void swap(basic_String & other) { core.swap(other.core); }

        const value_type* c_str() const { return core.c_str(); }
        value_type* data() { return core.data(); }
        const value_type* data() const { return core.data(); }

        bool empty()const { return core.empty(); }

        // by using template that you
        // can compare in different ways
        // and it will be compared by default traits_type
        template<typename T = Traits>
        int compare(const basic_String & other)const {
            return compare<T>(0, npos, other);
        }

        template<typename T = Traits>
        int compare(const size_type pos1, const size_type n1, const basic_String & other)const {
            return compare<T>(pos1, n1, other.data(), other.size());
        }

        template<typename T = Traits>
        int compare(const size_type pos1, const size_type n1, const value_type * s)const {
            return compare<T>(pos1, n1, s, traits_length(s));
        }

        template<typename T = Traits>
        int compare(const size_type pos1, const size_type n1,
            const value_type * s, const size_type n2)const {
            return non_default_traits<T>::compare(data() + pos1, npos_min(n1, size() - pos1), s, n2);
        }

        template<typename T = Traits>
        int compare(const size_type pos1, const size_type n1,
            const basic_String & other, const size_type pos2, const size_type n2 = npos)const {
            return compare<T>(pos1, n1, other.data() + pos2, npos_min(n2, other.size() - pos2));
        }

        template<typename T = Traits>
        int compare(const value_type * s, const size_type n)const {
            return compare<T>(0, size(), s, n);
        }

        template<typename T = Traits>
        int compare(const value_type * s)const {
            return compare<T>(0, size(), s, traits_length(s));
        }

        template<typename T = Traits>
        bool equal(const basic_String & other)const {
            return equal<T>(0, size(), other);
        }

        template<typename T = Traits>
        bool equal(const size_type pos1, const size_type n1, const basic_String & other)const {
            return equal<T>(pos1, n1, other.data(), other.size());
        }

        template<typename T = Traits>
        bool equal(const size_type pos1, const size_type n1, const value_type * s)const {
            return equal<T>(pos1, n1, s, traits_length(s));
        }

        template<typename T = Traits>
        bool equal(const size_type pos1, const size_type n1,
            const value_type * s, const size_type n2) const {
            return non_default_traits<T>::equal(data() + pos1, npos_min(n1, size() - pos1), s, n2);
        }

        template<typename T = Traits>
        bool equal(const value_type * s, const size_type n)const {
            return equal<T>(0, size(), s, n);
        }

        template<typename T = Traits>
        bool equal(const value_type * s)const {
            return equal<T>(s, traits_length(s));
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_helper
            get_find_helper(const value_type * s, const value_type * e) {
            return non_default_traits<T>::get_find_helper(s, e);
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_helper
            get_find_helper(const value_type * s, const size_type n) {
            return get_find_helper<T>(s, s + n);
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_helper
            get_find_helper(const value_type * s) {
            return get_find_helper<T>(s, s + traits_length(s));
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_helper
            get_find_helper(const basic_String& s) {
            return get_find_helper<T>(s.data(),s.size());
        }

        template<typename T = Traits>
        size_type find(const value_type ch, const size_type off = 0)const {
            return non_default_traits<T>::find(data(), size(), off, ch);
        }

        template<typename T = Traits>
        size_type find(const value_type * s, const size_type off = 0)const {
            return find<T>(s,off,traits_length(s));
        }

        template<typename T = Traits>
        size_type find(const value_type * s, const size_type off, const size_type count)const {
            return non_default_traits<T>::find(data(), size(), off, s, count);
        }

        template<typename T = Traits>
        size_type find(const basic_String & other, const size_type off = 0)const {
            return find<T>(other.data(),off,other.size());
        }

        template<typename T = Traits>
        size_type find(const typename non_default_traits<T>::string_find_helper & srch, const size_type off = 0)const {
            return non_default_traits<T>::find(data(), size(), off, srch);
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_rfind_helper
            get_rfind_helper(const value_type * s, const value_type * e) {
            return non_default_traits<T>::get_rfind_helper(s, e);
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_rfind_helper
            get_rfind_helper(const value_type * s, const size_type n) {
            return get_rfind_helper<T>(s, s + n);
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_helper
            get_rfind_helper(const value_type * s) {
            return get_rfind_helper<T>(s,s + traits_length(s));
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_helper
            get_rfind_helper(const basic_String& s) {
            return get_rfind_helper<T>(s.data(),s.size());
        }

        template<typename T = Traits>
        size_type rfind(const value_type ch, const size_type off = npos)const {
            return non_default_traits<T>::rfind(data(), size(), off, ch);
        }

        template<typename T = Traits>
        size_type rfind(const value_type * s, const size_type off = npos)const {
            return rfind<T>(s,off,traits_length(s));
        }

        template<typename T = Traits>
        size_type rfind(const value_type * s, const size_type off, const size_type count)const {
            return non_default_traits<T>::rfind(data(), size(), off, s, count);
        }

        template<typename T = Traits>
        size_type rfind(const basic_String & other, const size_type off = npos)const {
            return rfind<T>(other.data(),off,other.size());
        }

        template<typename T = Traits>
        size_type rfind(const typename non_default_traits<T>::string_rfind_helper & srch, const size_type off = npos)const {
            return non_default_traits<T>::rfind(data(), size(), off, srch);
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_of_helper get_find_of_helper(
            const value_type * s, const value_type * e
        ) {
            return non_default_traits<T>::get_find_of_helper(s, e);
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_of_helper get_find_of_helper(
            const value_type * s, const size_type n
        ) {
            return get_find_of_helper<T>(s,s + n);
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_of_helper get_find_of_helper(
            const value_type * s
        ) {
            return get_find_of_helper<T>(s,s + traits_length(s));
        }

        template<typename T = Traits>
        static typename non_default_traits<T>::string_find_of_helper get_find_of_helper(
            const basic_String&s
        ) {
            return get_find_of_helper<T>(s.data(),s.size());
        }

        template<typename T = Traits>
        size_type find_first_of(const value_type ch, const size_type off = 0)const {
            return non_default_traits<T>::find_first_of_ch(data(), size(), off, ch);
        }

        template<typename T = Traits>
        size_type find_first_of(const value_type* s, const size_type off = 0)const {
            return find_first_of<T>(s, off, traits_length(s));
        }

        template<typename T = Traits>
        size_type find_first_of(const value_type * s, const size_type off, const size_type count)const {
            return non_default_traits<T>::find_first_of(data(), size(), off, s, count);
        }

        template<typename T = Traits>
        size_type find_first_of(const basic_String & other, const size_type off = 0)const {
            return find_first_of<T>(other.data(),off, other.size());
        }

        template<typename T = Traits>
        size_type find_first_of(const typename non_default_traits<T>::string_find_of_helper & srch,
            const size_type off = 0)const {
            return non_default_traits<T>::find_first_of(data(), size(), off, srch);
        }

        template<typename T = Traits>
        size_type find_last_of(const value_type ch, const size_type off = npos)const {
            return non_default_traits<T>::find_last_of_ch(data(), size(), off, ch);
        }

        template<typename T = Traits>
        size_type find_last_of(const value_type * s, const size_type off = npos)const {
            return find_last_of<T>(s,off,traits_length(s));
        }

        template<typename T = Traits>
        size_type find_last_of(const value_type * s, const size_type off, const size_type count)const {
            return non_default_traits<T>::find_last_of(data(), size(), off, s, count);
        }

        template<typename T = Traits>
        size_type find_last_of(const basic_String & other, const size_type off = npos)const {
            return find_last_of<T>(other.data(),off,other.size());
        }

        template<typename T = Traits>
        size_type find_last_of(const typename non_default_traits<T>::string_find_of_helper & srch,
            const size_type off = 0)const {
            return non_default_traits<T>::find_last_of(data(), size(), off, srch);
        }

        template<typename T = Traits>
        size_type find_first_not_of(const value_type ch, const size_type off = 0)const {
            return non_default_traits<T>::find_first_not_of_ch(data(), size(), off, ch);
        }

        template<typename T = Traits>
        size_type find_first_not_of(const value_type * s, const size_type off = 0)const {
            return find_first_not_of<T>(s,off,traits_length(s));
        }

        template<typename T = Traits>
        size_type find_first_not_of(const value_type * s, const size_type off, const size_type count)const {
            return non_default_traits<T>::find_first_not_of(data(), size(), off, s, count);
        }

        template<typename T = Traits>
        size_type find_first_not_of(const basic_String & other, const size_type off = 0)const {
            return find_first_not_of<T>(other.data(),off,other.size());
        }

        template<typename T = Traits>
        size_type find_first_not_of(const typename non_default_traits<T>::string_find_of_helper & srch,
            const size_type off = 0)const {
            return non_default_traits<T>::find_first_not_of(data(), size(), off, srch);
        }

        template<typename T = Traits>
        size_type find_last_not_of(const value_type ch, const size_type off = npos)const {
            return non_default_traits<T>::find_last_not_of_ch(data(), size(), off, ch);
        }

        template<typename T = Traits>
        size_type find_last_not_of(const value_type * s, const size_type off = npos)const {
            return find_last_not_of<T>(s,off,traits_length(s));
        }

        template<typename T = Traits>
        size_type find_last_not_of(const value_type * s, const size_type off, const size_type count)const {
            return non_default_traits<T>::find_last_not_of(data(), size(), off, s, count);
        }

        template<typename T = Traits>
        size_type find_last_not_of(const basic_String & other, const size_type off = npos)const {
            return find_last_not_of<T>(other.data(),off,other.size());
        }

        template<typename T = Traits>
        size_type find_last_not_of(const typename non_default_traits<T>::string_find_of_helper & srch,
            const size_type off = 0)const {
            return non_default_traits<T>::find_last_not_of(data(), size(), off, srch);
        }

        template<typename T = Traits>
        bool starts_with(const value_type ch)const {
            return non_default_traits<T>::starts_with(data(), size(), ch);
        }

        template<typename T = Traits>
        bool starts_with(const value_type * s)const {
            return starts_with<T>(s, traits_length(s));
        }

        template<typename T = Traits>
        bool starts_with(const value_type * s, const size_type n)const {
            return non_default_traits<T>::starts_with(data(), size(), s, n);
        }

        template<typename T = Traits>
        bool starts_with(const basic_String & other)const {
            return starts_with<T>(other.data(), other.size());
        }

        template<typename T = Traits>
        bool ends_with(const value_type ch)const {
            return non_default_traits<T>::ends_with(data(), size(), ch);
        }

        template<typename T = Traits>
        bool ends_with(const value_type * s)const {
            return ends_with<T>(s, traits_length(s));
        }

        template<typename T = Traits>
        bool ends_with(const value_type * s, const size_type n)const {
            return non_default_traits<T>::ends_with(data(), size(), s, n);
        }

        template<typename T = Traits>
        bool ends_with(const basic_String& other)const {
            return ends_with<T>(other.data(),other.size());
        }

        basic_String substr(const size_type pos, const size_type n = npos)const& {
            return basic_String(data() + pos, npos_min(n, size() - pos));
        }

        basic_String substr(const size_type pos, const size_type n = npos)&& {
            erase(0, pos);
            resize(npos_min(n, size()));
            return std::move(*this);
        }

        basic_String trim()const& {
            auto _data = data();
            auto _size = size();
            auto l = default_traits::left_trim(_data,_size);
            if (l != npos) {
                auto r = default_traits::right_trim(_data,_size);
                return basic_String(data() + l, r - l + 1);
            }
            return basic_String();
        }

        basic_String trim()&& {
            auto _data = data();
            auto _size = size();
            auto l = default_traits::left_trim(_data, _size);
            erase(0,l);
            if (l == npos) {
                return std::move(*this);
            }
            auto r = default_traits::right_trim(_data, _size);
            erase(r,npos);
            return std::move(*this);
        }

        void chop(const size_type n) {
            core.shrink(npos_min(n,size()));
        }

        void remove_prefix(size_type n) {
            auto _size = size();
            n = npos_min(n,_size);
            if (n != 0) {
                auto _data = data();
                memmove(_data,_data + n,sizeof(value_type) * (_size - n));
                chop(n);
            }
        }

        void remove_suffix(const size_type n) {
            chop(n);
        }

        basic_String& fill(value_type ch, const size_type n = npos);

        template<typename T = Traits, typename string_list = std::vector<basic_String>>
        string_list split(const value_type ch, bool keep_empty_parts = true);

        template<typename T = Traits, typename string_list = std::vector<basic_String>>
        string_list split(const basic_String& other, bool keep_empty_parts = true);

        template<typename T = Traits, typename string_list = std::vector<basic_String>>
        string_list split(const value_type* s, bool keep_empty_parts = true);

        template<typename T = Traits, typename string_list = std::vector<basic_String>>
        string_list split(const value_type* s,
            const size_type n, bool keep_empty_parts = true);

        basic_String to_lower()const&;
        basic_String to_lower()&&;
        basic_String to_upper()const&;
        basic_String to_upper()&&;

        static basic_String number(int, int base = 10);
        static basic_String number(unsigned int, int base = 10);
        static basic_String number(long long, int base = 10);
        static basic_String number(unsigned long long, int base = 10);
        static basic_String number(double, char f = 'g', int prec = 6);
        static basic_String fixed_number(double);

        basic_String& set_number(int,int base = 10);
        basic_String& set_number(unsigned int ,int base = 10);
        basic_String& set_number(long long,int base = 10);
        basic_String& set_number(unsigned long long,int base = 10);
        basic_String& set_number(double,char f = 'g',int prec = 6);

    private:
        struct string_connect_helper {
        public:
            string_connect_helper(const basic_String& other)
                : sv(other.data()), count(other.size()) {

            }

            string_connect_helper(basic_String&& other) noexcept
                : sv(other.data()), count(other.size()) {

            }
            
            string_connect_helper(const value_type* s)
                : sv(s), count(traits_length(s)) {

            }

            template<typename T,typename C>
            string_connect_helper(const basic_String<Char, T, C>& other)
                : sv(other.data()), count(other.size()) {

            }

            template<typename T,typename A>
            string_connect_helper(const std::basic_string<Char, T, A>& other)
                : sv(other.data()), count(other.size()) {

            }

            string_connect_helper(std::initializer_list<value_type> il)
                : sv(il.begin()), count(il.size()) {

            }

            string_connect_helper(const basic_String_view<Char, Traits>& s)
                : sv(s.data()), count(s.size()) {

            }

            string_connect_helper(const std::basic_string_view<Char, Traits>& s)
                : sv(s.data()), count(s.size()) {

            }

            string_connect_helper(const value_type&ch)
                : sv(&ch), count(1) {
                
            }

            const value_type* data()const {
                return sv;
            }

            const size_type size()const {
                return count;
            }

        private:
            const value_type* sv;
            const size_type count;
        };

        static basic_String _Connect(std::initializer_list<string_connect_helper> il) ;

        static basic_String _Connect_init(basic_String&& init, std::initializer_list<string_connect_helper> il);

    public:

        template<typename...Args>
        static basic_String connect(Args&&...args) {
            return _Connect({std::forward<Args>(args)...});
        }

        template<typename...Args>
        static basic_String connect(basic_String&&init, Args&&...args) {
            return _Connect_init(std::move(init),{std::forward<Args>(args)...});
        }

        template<typename...Args>
        basic_String& multiple_append(Args&&...args) {
            *this = _Connect_init(std::move(*this), { std::forward<Args>(args)... });
            return *this;
        }

        basic_String repeated(int times);

    private:
        Core core;
    };

    template<typename Char, typename Traits, typename Core>
    void basic_String<Char, Traits, Core>::
        resize(const size_type n, const value_type c) {
        const size_type sz = size();

        if (n < sz) {
            core.shrink(sz - n);
        }

        if (n > sz) {
            const auto delta = n - sz;
            auto pData = core.expandNoinit(delta, true);
            std::fill(pData, pData + delta, c);
        }

        // do not do anything when n == size

        assert(size() == n);
    }

    template<typename Char, typename Traits, typename Core>
    basic_String<Char, Traits, Core>& 
        basic_String<Char, Traits, Core>::append(
        const value_type* s, size_type n) {

        const auto old_data = data();
        const auto old_size = size();
        const auto ptr = core.expandNoinit(n, true);

        if (old_data <= s && s < old_data + old_size) {
            s = (ptr - old_size) + (s - old_data);
        }

        memcpy(ptr,s,sizeof(value_type) * n);

        return *this;
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char,Traits,Core>&
        basic_String<Char, Traits, Core>::prepend(
            const value_type* s, size_type n) {
        if (!n) {
            return *this;
        }
        const auto old_data = data();
        const auto old_size = size();
        core.expandNoinit(n, true);

        auto ptr = data();
        memmove(ptr + n,ptr,sizeof(value_type) * old_size);

        if (old_data <= s && s < old_data + old_size) {
            s = ptr + (s - old_data) + n;
        }

        memcpy(ptr,s,sizeof(value_type) * n);

        return *this;
    }

    template<typename Char, typename Traits, typename Core>
    basic_String<Char, Traits, Core>& 
        basic_String<Char, Traits, Core>::assign(
        const value_type* s, const size_type n) {
        if (!n) {
            resize(0);
        }
        else if (size() >= n) {
            memmove(core.data(), s, sizeof(value_type) * n);
            core.shrink(size() - n);
        }
        else {
            resize(0);
            // O(n) to copy
            // so don't need to use expGrowth
            memmove(core.expandNoinit(n), s, sizeof(value_type) * n);
        }
        return *this;
    }

    template<typename Char, typename Traits, typename Core>
    basic_String<Char, Traits, Core>& 
        basic_String<Char, Traits, Core>::insert(
        const size_type pos, const value_type* s, const size_type n) {

        if (!n) {
            return *this;
        }

        const auto oldData = data();
        const auto oldSize = size();

        const auto pData = core.expandNoinit(n, true);
        const auto Data = pData - oldSize;

        const auto ins_first = Data + pos;
        const auto ins_last = ins_first + n;

        memmove(ins_last, ins_first, sizeof(value_type) * (oldSize - pos));

        if (oldData <= s && s < oldData + oldSize) {
            s += (Data - oldData);
            size_type left_size;
            if (s + n <= ins_first || s >= ins_last) {
                left_size = n;
            }
            else {
                left_size = ins_first - s;
            }
            memcpy(ins_first, s, sizeof(value_type) * left_size);
            memcpy(ins_first + left_size, s + left_size + n, sizeof(value_type) * (n - left_size));
        }
        else {
            memcpy(ins_first, s, sizeof(value_type) * n);
        }

        return *this;
    }

    template<typename Char, typename Traits, typename Core>
    basic_String<Char, Traits, Core>& 
        basic_String<Char, Traits, Core>::insert(
        const size_type pos, const size_type n, const value_type c
    ) {
        const auto old_size = size();
        const auto data = core.expandNoinit(n, true);
        auto data_first = data - old_size;
        auto data_pos = data_first + pos;
        auto data_end = data;
        memmove(data_pos + n, data_pos, sizeof(value_type) * (old_size - pos));
        std::fill_n(data_pos, n, c);
        return *this;
    }

    template<typename Char, typename Traits, typename Core>
    basic_String<Char, Traits, Core>&
        basic_String<Char, Traits, Core>::erase(const size_type pos, size_type n) {
        if (!n) {
            return *this;
        }
        const size_type res = size() - pos;
        n = npos_min(n, res);
        const auto _Data = data();
        memcpy(_Data + pos, _Data + pos + n, sizeof(value_type) * (size() - pos - n));
        core.shrink(n);
        return *this;
    }

    template<typename Char,typename Traits,typename Core>
    template<typename T>
    basic_String<Char,Traits,Core>& basic_String<Char,Traits,Core>::
        remove(const value_type* s, const size_type _off, const size_type n) {
        auto _data = data();
        size_type off = _off;
        auto srch = get_find_helper<T>(s,s + n);
        size_type appear_time = 0;
        for (;;) {
            const auto pos = find<T>(srch,off);
            if (pos == npos) {
                break;
            }
            if (appear_time) {
                assert(off >= n * appear_time);
                memmove(_data + off - n * appear_time,
                    _data + off, sizeof(value_type) * 
                    static_cast<size_type>(pos - off));
            }
            ++appear_time;
            off = pos + n;
        }
        if (appear_time) {
            assert(off >= n * appear_time);
            memmove(_data + off - n * appear_time,
                _data + off, sizeof(value_type) *
                static_cast<size_type>(size() - off));
        }
        chop(n * appear_time);
        return *this;
    }

    template<typename Char, typename Traits, typename Core>
    basic_String<Char, Traits, Core>&
        basic_String<Char, Traits, Core>::replace(
            const size_type pos1, const size_type n1, const value_type* s, const size_type n2) {
        std::copy(s, s + std::min(n1, n2), data() + pos1);
        if (n1 < n2) {
            insert(pos1 + n1, s + n1, n2 - n1);
        }
        else {
            erase(pos1 + n2, n1 - n2);
        }
        return *this;
    }

    template<typename Char, typename Traits, typename Core>
    template<typename T>
    basic_String<Char, Traits, Core>& basic_String<Char, Traits, Core>::replace(
        const value_type before, const value_type after) {
        auto _data = data();
        size_type off = 0;
        for (;;) {
            off = find<T>(before,off);
            if (off == npos) {
                break;
            }
            *(_data + off) = after;
            ++off;
        }
        return *this;
    }

    template<typename Char, typename Traits, typename Core>
    template<typename T>
    basic_String<Char, Traits, Core>& basic_String<Char, Traits, Core>::replace(
        const value_type*s1,const size_type n1,const value_type* s2,const size_type n2) {
        size_type off = 0;
        auto srch = get_find_helper(s1,s1 + n1);
        if (n1 == n2) {
            for (;;) {
                const size_type pos = find<T>(srch,off);
                if (pos == npos) {
                    break;
                }
                replace(pos,n1,s2,n2);
                off = pos + n1;
            }
        }
        else {
            basic_String temp;
            for (;;) {
                const size_type pos = find<T>(srch,off);
                if (pos == npos) {
                    temp.append(*this,off,npos);
                    break;
                }
                temp.append(*this,pos,pos - off).append(s2,n2);
                off = pos + n1;
            }
            this->~basic_String();
            new (this) basic_String(std::move(temp));
        }
        return *this;
    }

    template<typename Char, typename Traits, typename Core>
    basic_String<Char, Traits, Core>&
        basic_String<Char, Traits, Core>::fill(value_type ch, const size_type n) {
        auto _size = size();
        if (n == npos) {
            std::fill_n(data(), _size, ch);
        }
        else {
            if (_size < n) {
                core.expandNoinit(n - _size, true);
            }
            if (_size > n) {
                core.shrink(_size - n);
            }
            std::fill_n(data(), n, ch);
        }
        return *this;
    }

    template<typename Char,typename Traits,typename Core>
    template<typename T,typename string_list>
    string_list basic_String<Char, Traits, Core>::split(
        const value_type ch, bool keep_empty_parts
    ) {
        return non_default_traits<T>::template split<string_list>(data(),size(),ch,keep_empty_parts);
    }

    template<typename Char,typename Traits,typename Core>
    template<typename T, typename string_list>
    string_list basic_String<Char,Traits,Core>::split(
        const basic_String<Char,Traits,Core>& other, bool keep_empty_parts) {
        return split<T,string_list>(other.data(),other.size(),keep_empty_parts);
    }

    template<typename Char, typename Traits, typename Core>
    template<typename T, typename string_list>
    string_list basic_String<Char, Traits, Core>::split(
        const value_type* s,bool keep_empty_parts) {
        return split<T,string_list>(s,traits_length(s),keep_empty_parts);
    }

    template<typename Char, typename Traits, typename Core>
    template<typename T, typename string_list>
    string_list basic_String<Char, Traits, Core>::split(
        const value_type* s, const size_type n,
        bool keep_empty_parts) {
        return non_default_traits<T>::template split<string_list>(data(),size(),s,n,keep_empty_parts);
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::to_lower()const& {
        basic_String ans(*this);
        return std::move(ans).to_lower();
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::to_lower()&& {
        for (auto& i : (*this)) {
            i = tolower(i);
        }
        return std::move(*this);
    }

    template<typename Char, typename Traits, typename Core>
    basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::to_upper()const& {
        basic_String ans(*this);
        return std::move(ans).to_upper();
    }

    template<typename Char, typename Traits, typename Core>
    basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::to_upper()&& {
        for (auto& i : (*this)) {
            i = toupper(i);
        }
        return std::move(*this);
    }

    template<typename Char, typename UVal>
    Char* get_number_positive(Char* buff, UVal val, int base) {
        static_assert(std::is_unsigned_v<UVal>, "UVal must be unsigned");
        assert(base >= 0);
        do {
            UVal mod = val % base;
            *(--buff) = static_cast<Char>(mod < 10 ? '0' + mod : 'a' + (mod -10));
            val /= base;
        } while (val != 0);
        return buff;
    }

    template<typename Char,typename UVal>
    Char* get_number_negative(Char* buff, UVal val, bool p, int base) {
        static_assert(std::is_unsigned_v<UVal>, "UVal must be unsigned");
        assert(base < 0);
        bool f = p;
        unsigned int ubase = static_cast<unsigned int>(-base);
        do {
            UVal mod = val % ubase;
            UVal k = val / ubase;
            if (mod != 0 && !f) {
                mod = ubase - mod;
                if (!f) {
                    ++k;
                }
                else {
                    --k;
                }
            }
            f = !f;
            val = k;
            *(--buff) = static_cast<Char>(mod < 10 ? '0' + mod : 'a' + (mod - 10));
        }while(val != 0);
        return buff;
    }

    template<typename Char,typename Val>
    Char* get_number_signed_helper(Char* buff, Val val, int base) {
        static_assert(!std::is_unsigned_v<Val>,"Val must be signed");
        using uval = std::make_unsigned_t<Val>;
        if (base >= 0) {
            if (val < 0) {
                buff = get_number_positive(buff,static_cast<uval>(0 - val),base);
                *(--buff) = static_cast<Char>('-');
            }
            else {
                buff = get_number_positive(buff,static_cast<uval>(val),base);
            }
        }
        else {
            if (val < 0) {
                buff = get_number_negative(buff,static_cast<uval>(0 - val),false,base);
            }
            else {
                buff = get_number_negative(buff,static_cast<uval>(val),true,base);
            }
        }
        return buff;
    }

    template<typename Char, typename UVal>
    Char* get_number_unsigned_helper(Char* buff, UVal val, int base) {
        static_assert(std::is_unsigned_v<UVal>, "UVal must be unsigned");
        if (base >= 0) {
            buff = get_number_positive(buff,val,base);
        }
        else {
            buff = get_number_negative(buff,val,true,base);
        }
        return buff;
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::
        number(int val, int base) {
        using uval = unsigned int;
        Char buff[33];
        Char* const buff_end = std::end(buff);
        Char* pos = buff_end;
        pos = get_number_signed_helper(pos,val,base);
        return basic_String<Char,Traits,Core>(pos,buff_end);
    }

    template<typename Char, typename Traits, typename Core>
    basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::
        number(unsigned int val, int base) {
        Char buff[33];
        Char* const buff_end = std::end(buff);
        Char* pos = buff_end;
        pos = get_number_unsigned_helper(pos,val,base);
        return basic_String<Char, Traits, Core>(pos, buff_end);
    }

    template<typename Char, typename Traits, typename Core>
    basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::
        number(long long val, int base) {
        using uval = unsigned long long;
        Char buff[65];
        Char* const buff_end = std::end(buff);
        Char* pos = buff_end;
        pos = get_number_signed_helper(pos,val,base);
        return basic_String<Char, Traits, Core>(pos, buff_end);
    }

    template<typename Char, typename Traits, typename Core>
    basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::
        number(unsigned long long val, int base) {
        Char buff[65];
        Char* const buff_end = std::end(buff);
        Char* pos = buff_end;
        pos = get_number_unsigned_helper(pos, val, base);
        return basic_String<Char, Traits, Core>(pos, buff_end);
    }

    template<typename Char, typename Traits, typename Core>
    basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::
        number(double val, char f, int prec) {
        if(prec > 99){prec = 99;}
        char form[6];
        char* pos = form;
        *(pos++) = static_cast<Char>('%');
        *(pos++) = static_cast<Char>('.');

        if (prec < 10) {
            *(pos++) = static_cast<Char>('0' + prec);
        }
        else {
            *(pos++) = static_cast<Char>('0' + (prec / 10));
            *(pos++) = static_cast<Char>('0' + (prec % 10));
        }
        *(pos++) = f;
        *(pos++) = '\0';
        char buff[64];
        const auto len = sprintf(buff,form,val);
        return basic_String<Char,Traits,Core>(buff,buff + len);
    }

    extern "C" {
        bool fill_double(double v, char* buffer);
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char, Traits, Core> basic_String<Char, Traits, Core>::fixed_number(double val) {
        if (!val) {
            return basic_String<Char,Traits,Core>(static_cast<value_type>('0'));
        }
        char buff[64];
        fill_double(val, buff);
        const char* ptr = buff;
        for (int n = 0;n < 64 && *ptr !='\0'; ++n,++ptr);
        return basic_String<Char,Traits,Core>(buff,ptr);
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char,Traits,Core>& basic_String<Char,Traits,Core>::
        set_number(int val, int base) {
        *this = std::move(number(val,base));
        return *this;
    }

    template<typename Char, typename Traits, typename Core>
    basic_String<Char, Traits, Core>& basic_String<Char, Traits, Core>::
        set_number(unsigned int val, int base) {
        *this = std::move(number(val, base));
        return *this;
    }

    template<typename Char, typename Traits, typename Core>
    basic_String<Char, Traits, Core>& basic_String<Char, Traits, Core>::
        set_number(long long val, int base) {
        *this = std::move(number(val,base));
        return *this;
    }

    template<typename Char, typename Traits, typename Core>
    basic_String<Char, Traits, Core>& basic_String<Char, Traits, Core>::
        set_number(unsigned long long val, int base) {
        *this = std::move(number(val,base));
        return *this;
    }

    template<typename Char, typename Traits, typename Core>
    basic_String<Char, Traits, Core>& basic_String<Char, Traits, Core>::
        set_number(double val, char f, int prec) {
        *this = std::move(number(val,f,prec));
        return *this;
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char,Traits,Core> basic_String<Char,Traits,Core>::
        _Connect(std::initializer_list<string_connect_helper> il) {
        size_type count = 0;
        for (auto& i : il) {
            count += i.size();
        }
        basic_String it;
        auto ptr = it.core.expandNoinit(count);
        for (auto& i : il) {
            memcpy(ptr, i.data(), sizeof(value_type) * i.size());
            ptr += i.size();
        }
        return it;
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char,Traits,Core> basic_String<Char,Traits,Core>::
        _Connect_init(basic_String&& init, std::initializer_list<string_connect_helper> il) {
        size_type count = 0;
        for (auto& i : il) {
            count += i.size();
        }
        basic_String it(std::move(init));
        auto ptr = it.core.expandNoinit(count);
        for (auto& i : il) {
            memcpy(ptr, i.data(), sizeof(value_type) * i.size());
            ptr += i.size();
        }
        return it;
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char,Traits,Core> basic_String<Char,Traits,Core>::
        repeated(int times) {
        if (times <= 0) times = 0;
        auto _data = data();
        auto _size = size();
        basic_String it;
        auto ptr = it.core.expandNoinit(times * _size);
        for (; times; --times) {
            memcpy(ptr, _data, sizeof(value_type) * _size);
            ptr += _size;
        }
        return it;
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char,Traits,Core> operator+(
        const basic_String<Char,Traits,Core>& lhs,
        const basic_String<Char,Traits,Core>& rhs
        ) {
        basic_String<Char,Traits,Core> result;
        result.reserve(lhs.size() + rhs.size());
        result.append(lhs).append(rhs);
        return result;
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char,Traits,Core> operator+(
        basic_String<Char,Traits,Core>&& lhs,
        const basic_String<Char,Traits,Core>& rhs
        ) {
        return std::move(lhs.append(rhs));
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char,Traits,Core> operator+(
        const basic_String<Char,Traits,Core>& lhs,
        basic_String<Char,Traits,Core>&& rhs
        ) {
        if (rhs.capacity() >= lhs.size() + rhs.size()) {
            return std::move(rhs.insert(0,lhs));
        }
        auto const& rc = rhs;
        return lhs + rc;
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char,Traits,Core> operator+(
        basic_String<Char,Traits,Core>&& lhs,
        basic_String<Char,Traits,Core>&& rhs
        ) {
        return std::move(lhs.append(rhs));
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char,Traits,Core> operator+(
        const Char* lhs,
        const basic_String<Char,Traits,Core>& rhs
        ) {
        using traits_type = typename basic_String<Char,Traits,Core>::traits_type;
        const auto len = traits_type::length(lhs);
        basic_String<Char,Traits,Core> result;
        result.reserve(len + rhs.size());
        result.append(lhs,len).append(rhs);
        return result;
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char,Traits,Core> operator+(
        const Char* lhs,
        basic_String<Char,Traits,Core>&& rhs
        ) {
        using traits_type = typename basic_String<Char,Traits,Core>::traits_type;
        const auto len = traits_type::length(lhs);
        if (rhs.capacity() >= len + rhs.size()) {
            rhs.insert(0,lhs,len);
            return std::move(rhs);
        }
        basic_String<Char,Traits,Core> result;
        result.reserve(len + rhs.size());
        result.append(lhs,len).append(rhs);
        return result;
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char,Traits,Core> operator+(
        const Char ch,
        const basic_String<Char,Traits,Core>& rhs
        ) {
        basic_String<Char,Traits,Core> result;
        result.reserve(1 + rhs.size());
        result.push_back(ch);
        result.append(rhs);
        return result;
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char,Traits,Core> operator+(
        const Char ch,
        basic_String<Char,Traits,Core>&& rhs
        ) {
        if (rhs.capacity() >= rhs.size() + 1) {
            rhs.insert(0,ch);
            return std::move(rhs);
        }
        auto const& rc = rhs;
        return ch + rhs;
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char,Traits,Core> operator+(
        const basic_String<Char,Traits,Core>& lhs,
        const Char* rhs
        ) {
        using traits_type = typename basic_String<Char,Traits,Core>::traits_type;
        const auto len = traits_type::length(rhs);
        basic_String<Char,Traits,Core> result;
        result.reserve(lhs.size() + len);
        result.append(lhs).append(rhs,len);
        return result;
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char,Traits,Core> operator+(
        basic_String<Char,Traits,Core>&& lhs,
        const Char* rhs
        ) {
        return std::move(lhs.append(rhs));
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char,Traits,Core> operator+(
        const basic_String<Char,Traits,Core>& lhs,
        const Char ch
        ) {
        basic_String<Char,Traits,Core> result;
        result.reserve(lhs.size() + 1);
        result.append(lhs).push_back(ch);
        return result;
    }

    template<typename Char,typename Traits,typename Core>
    basic_String<Char,Traits,Core> operator+(
        basic_String<Char,Traits,Core>&& lhs,
        const Char ch
        ) {
        return std::move(lhs.append(ch));
    }

    template<typename Char,typename Traits,typename Core>
    bool operator==(
        const basic_String<Char,Traits,Core>& lhs,
        const basic_String<Char,Traits,Core>& rhs
        ) {
        return lhs.equal(rhs);
    }

    template<typename Char,typename Traits,typename Core>
    bool operator==(
        const basic_String<Char,Traits,Core>& lhs,
        const Char* rhs
        ) {
        return lhs.equal(rhs);
    }

    template<typename Char,typename Traits,typename Core>
    bool operator!=(
        const basic_String<Char,Traits,Core>& lhs,
        const basic_String<Char,Traits,Core>& rhs
        ) {
        return !(lhs == rhs);
    }

    template<typename Char,typename Traits,typename Core>
    bool operator!=(
        const basic_String<Char,Traits,Core>& lhs,
        const Char* rhs
        ) {
        return !(lhs == rhs);
    }

    template<typename Char,typename Traits,typename Core>
    bool operator!=(
        const Char* lhs,
        const basic_String<Char,Traits,Core>& rhs
        ) {
        return !(lhs == rhs);
    }

    template<typename Char,typename Traits,typename Core>
    bool operator<(
        const basic_String<Char,Traits,Core>& lhs,
        const basic_String<Char,Traits,Core>& rhs
        ) {
        return lhs.compare(rhs) < 0;
    }

    template<typename Char,typename Traits,typename Core>
    bool operator<(
        const basic_String<Char,Traits,Core>& lhs,
        const Char* rhs
        ) {
        return lhs.compare(rhs) < 0;
    }

    template<typename Char,typename Traits,typename Core>
    bool operator<(
        const Char* lhs,
        const basic_String<Char,Traits,Core>& rhs
        ) {
        return rhs.compare(lhs) > 0;
    }

    template<typename Char,typename Traits,typename Core>
    bool operator>(
        const basic_String<Char,Traits,Core>& lhs,
        const basic_String<Char,Traits,Core>& rhs
        ) {
        return lhs.compare(rhs) > 0;
    }

    template<typename Char,typename Traits,typename Core>
    bool operator>(
        const basic_String<Char,Traits,Core>& lhs,
        const Char* rhs
        ) {
        return lhs.compare(rhs) > 0;
    }

    template<typename Char,typename Traits,typename Core>
    bool operator>(
        const Char* lhs,
        const basic_String<Char,Traits,Core>& rhs
        ) {
        return rhs.compare(lhs) < 0;
    }

    template<typename Char,typename Traits,typename Core>
    bool operator<=(
        const basic_String<Char,Traits,Core>& lhs,
        const basic_String<Char,Traits,Core>& rhs
        ) {
        return lhs.compare(rhs) <= 0;
    }

    template<typename Char,typename Traits,typename Core>
    bool operator<=(
        const basic_String<Char,Traits,Core>& lhs,
        const Char* rhs
        ) {
        return lhs.compare(rhs) <= 0;
    }

    template<typename Char,typename Traits,typename Core>
    bool operator<=(
        const Char* lhs,
        const basic_String<Char,Traits,Core>& rhs
        ) {
        return rhs.compare(lhs) >= 0;
    }

    template<typename Char,typename Traits,typename Core>
    bool operator>=(
        const basic_String<Char,Traits,Core>& lhs,
        const basic_String<Char,Traits,Core>& rhs
        ) {
        return lhs.compare(rhs) >= 0;
    }

    template<typename Char,typename Traits,typename Core>
    bool operator>=(
        const basic_String<Char,Traits,Core>& lhs,
        const Char* rhs
        ) {
        return lhs.compare(rhs) >= 0;
    }

    template<typename Char,typename Traits,typename Core>
    bool operator>=(
        const Char* lhs,
        const basic_String<Char,Traits,Core>& rhs
        ) {
        return rhs.compare(lhs) <= 0;
    }

    template<typename Char,typename Traits,typename Core>
    void swap(basic_String<Char,Traits,Core>& lhs, basic_String<Char,Traits,Core>& rhs) {
        lhs.swap(rhs);
    }

    // TODO: make this faster.
    template <typename Char,typename Traits,typename Core>
    std::basic_istream<Char,Traits>&
        operator>>(
            std::basic_istream<Char,Traits>& is,
            basic_String<Char,Traits,Core>& str) {
        using istream_type = std::basic_istream<Char,Traits>;
        typename istream_type::sentry sentry(is);
        size_t extracted = 0;
        typename istream_type::iostate err = istream_type::goodbit;
        if (sentry) {
            auto n = is.width();
            if (n <= 0) {
                n = str.max_size();
            }
            str.clear();
            size_t sn = static_cast<size_t>(n);
            for (auto got = is.rdbuf()->sgetc(); extracted != sn; ++extracted) {
                if (got == Traits::eof()) {
                    err |= istream_type::eofbit;
                    is.width(0);
                    break;
                }
                if (std::isspace(got)) {
                    break;
                }
                str.push_back(got);
                got = is.rdbuf()->snextc();
            }
        }
        if (!extracted) {
            err |= istream_type::failbit;
        }
        if (err) {
            is.setstate(err);
        }
        return is;
    }

    template <typename Char,typename Traits,typename Core>
    inline std::basic_ostream<Char,Traits>&
        operator<<(
            std::basic_ostream<Char,Traits>& os,
            const basic_String<Char,Traits,Core>& str) {
    #ifdef _LIBCPP_VERSION
        using ostream_type = std::basic_ostream<Char,Traits>;
        typename ostream_type::sentry _s(os);
        if (_s) {
            using _Ip = std::ostreambuf_iterator<Char,Traits>;
            size_t __len = str.size();
            bool __left =
                (os.flags() & ostream_type::adjustfield) == ostream_type::left;
            if (__pad_and_output(
                _Ip(os),
                str.data(),
                __left ? str.data() + __len : str.data(),
                str.data() + __len,
                os,
                os.fill())
                .failed()) {
                os.setstate(ostream_type::badbit | ostream_type::failbit);
            }
        }
    #elif defined(_MSC_VER)
        typedef decltype(os.precision()) streamsize;
        // MSVC doesn't define __ostream_insert
        os.write(str.data(), static_cast<streamsize>(str.size()));
    #else
        std::__ostream_insert(os, str.data(), str.size());
    #endif
        return os;
    }

    using String = basic_String<char,std::char_traits<char>>;
    using wString = basic_String<wchar_t,std::char_traits<wchar_t>>;
    using u16String = basic_String<char16_t,std::char_traits<char16_t>>;
    using u32String = basic_String<char32_t,std::char_traits<char32_t>>;

    using String_view = basic_String_view<char,std::char_traits<char>>;
    using wString_view = basic_String_view<wchar_t,std::char_traits<wchar_t>>;
    using u16String_view = basic_String_view<char16_t,std::char_traits<char16_t>>;
    using u32String_view = basic_String_view<char32_t,std::char_traits<char32_t>>;

    template<typename T>
    struct case_insensitive_String {
        using type = case_insensitive<typename T::traits_type>;
    };

    template<typename T>
    using case_insensitive_String_t = typename case_insensitive_String<T>::type;

}

#define DEFAULT_STRING_HASH(T)											  \
template<>																  \
struct hash<wjr::basic_String<T, char_traits<T>>> {						  \
    size_t operator()(const wjr::basic_String<T,char_traits<T>>& s)const{ \
        return wjr::hash_array_representation(s.data(),s.size());		  \
    }																	  \
};                                                                        \

namespace std {
    DEFAULT_STRING_HASH(char) 
    DEFAULT_STRING_HASH(wchar_t)
    DEFAULT_STRING_HASH(char16_t)
    DEFAULT_STRING_HASH(char32_t)
}

#endif