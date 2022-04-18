#ifndef __WJR_BIGINTEGER_H
#define __WJR_BIGINTEGER_H

#include "mallocator.h"
#include "mString.h"
#include "mtool.h"

extern "C" {
    extern void cdft(int, int, double*);
}

namespace wjr {

    template<size_t base>
    struct biginteger_traits {
        using type = uint32_t;
        using twice_type = uint64_t;
        constexpr static size_t type_bit = sizeof(type) * 8;
        constexpr static bool is_power_of_two = (base & (base - 1)) == 0;
        constexpr static size_t quick_log2_base = quick_log2(base);
        constexpr static size_t __get_bit_length() {
            if constexpr (is_power_of_two && (type_bit % quick_log2_base == 0)) {
                return type_bit;
            }
            else {
                return quick_log<base>(std::numeric_limits<type>::max());
            }
        }

        constexpr static size_t bit_length = __get_bit_length();
        constexpr static twice_type max_value = quick_pow((twice_type)base, (twice_type)bit_length);

        constexpr static type _max = quick_pow((type)base, (type)bit_length) - 1; ;

        constexpr static type _mod = _max + 1;

        template<typename T>
        constexpr static size_t __get_bit_of_t() {
            auto v = std::numeric_limits<T>::max();
            size_t result = 1;
            while (v > _max) {
                v /= max_value;
                ++result;
            }
            return result;
        }

        template<typename T>
        constexpr static size_t bit_of_t = __get_bit_of_t<T>();

        enum class mode {
            is_full = 0,
            is_le_mid = 1,
            is_mt_mid = 2
        };

        constexpr static mode __get_mode() {
            if constexpr (_max == std::numeric_limits<type>::max()) {
                return mode::is_full;
            }
            else if constexpr (_max <= (std::numeric_limits<type>::max() / 2)) {
                return mode::is_le_mid;
            }
            else return mode::is_mt_mid;
        }

        constexpr static mode get_mode = __get_mode();

        template<typename T>
        constexpr static type get_low(T x) {
            if constexpr (std::numeric_limits<T>::max() <= _max || get_mode == mode::is_full) {
                return static_cast<type>(x);
            }
            else {
                return x % _mod ;
            }
        }

        template<typename T>
        constexpr static T get_high(T x) {
            if constexpr (std::numeric_limits<T>::max() <= _max) {
                return 0;
            }
            else {
                if constexpr (get_mode == mode::is_full) {
                    return static_cast<T>(x >> 32);
                }
                else {
                    return static_cast<T>(x / _mod );
                }
            }
        }

        constexpr static type quick_add(type lhs, type rhs, type& jw) {
            type tmp = lhs + rhs + jw;
            if constexpr (get_mode == mode::is_full) {
                jw = static_cast<type>((tmp <= lhs) && (rhs || jw));
                return tmp;
            }
            else {
                if constexpr (get_mode == mode::is_le_mid) {
                    jw = static_cast<type>(_mod  <= tmp);
                }
                else {
                    jw = static_cast<type>(_mod  <= tmp || tmp < lhs);
                }
                return jw ? tmp - _mod  : tmp;
            }
        }

        constexpr static type quick_sub(type lhs, type rhs, type& jw) {
            type tmp = lhs - (rhs + jw);
            if constexpr (get_mode == mode::is_full) {
                jw = static_cast<type>((lhs <= tmp) && (rhs || jw));
                return tmp;
            }
            else {
                if constexpr (get_mode == mode::is_le_mid) {
                    jw = static_cast<type>(_mod  <= tmp);
                }
                else {
                    jw = static_cast<size_t>(_mod  <= tmp || lhs < tmp);
                }
                return jw ? tmp - _mod  : tmp;
            }
        }

        struct constexpr_base_power_array {
            constexpr constexpr_base_power_array() : arr() {
                arr[0] = 1;
                for (size_t i = 1; i < bit_length; ++i) {
                    arr[i] = arr[i - 1] * base;
                }
            }
            std::array<type, bit_length> arr;
        };

        constexpr static constexpr_base_power_array 
            static_base_power_array = constexpr_base_power_array();
        constexpr static type get_base_power(size_t kth) {
            WASSERT_LEVEL_2(kth < bit_length);
            return static_base_power_array.arr[kth];
        }
        struct magic_divider_of_base_power {
            magic_divider_of_base_power() {
                using T = libdivide::divider<type>;
                for (size_t i = 0; i < bit_length; ++i) {
                    new (&magic_divider[i]) T(get_base_power(i));
                }
            }
            std::array<libdivide::divider<type>, bit_length> magic_divider;
        };
        static magic_divider_of_base_power static_magic_divider; 
        constexpr static type quick_div(type val,size_t kth) {
            WASSERT_LEVEL_2(kth < bit_length);
            if constexpr (is_power_of_two) {
                return val >> (quick_log2_base * kth);
            }
            else {
                return val / static_magic_divider.magic_divider[kth];
            }
        }
        constexpr static type quick_mod(type val, size_t kth) {
            WASSERT_LEVEL_2(kth < bit_length);
            if constexpr (is_power_of_two) {
                return val & (((type)(1) << (quick_log2_base * kth)) - 1);
            }
            else {
                return val - quick_div(val,kth) * get_base_power(kth);
            }
        }

    };

    template<size_t base>
    typename biginteger_traits<base>::magic_divider_of_base_power
        biginteger_traits<base>::static_magic_divider = magic_divider_of_base_power();

    template<size_t base, size_t length>
    class virtual_unsigned_biginteger;

    template<size_t base,size_t length>
    class virtual_unsigned_biginteger_reference {
        using traits = biginteger_traits<base>;
        using virtual_unsigned_biginteger = wjr::virtual_unsigned_biginteger<base, length>;
    public:
        using value_type = typename traits::type;
        virtual_unsigned_biginteger_reference(
            const virtual_unsigned_biginteger& ptr, size_t _K,size_t index)
            : ptr(ptr), _K(_K),index(index) { }
        size_t pos1()const { return index / traits::bit_length; }
        size_t pos2()const { return index % traits::bit_length; }
        virtual_unsigned_biginteger_reference(
            const virtual_unsigned_biginteger_reference&) = default;
        virtual_unsigned_biginteger_reference& operator=(
            const virtual_unsigned_biginteger_reference&) = default;
        operator value_type() const{
            auto p = ptr.data();
            size_t _Pos1 = pos1();
            size_t _Pos2 = pos2();
            value_type low = traits::quick_div(p[_Pos1], _Pos2);
            size_t low_l = traits::bit_length - _Pos2;
            if (_K <= low_l || _Pos1 == ptr.size() - 1) {
                return get_mod(low);
            }
            size_t high_l = _K - low_l;
            value_type high = traits::quick_mod(p[_Pos1 + 1], high_l);
            return high * traits::get_base_power(low_l) + low;
        }
        virtual_unsigned_biginteger_reference& operator=(value_type val) {
            auto p = ptr.data();
            size_t _Pos1 = pos1();
            size_t _Pos2 = pos2();
            value_type low = traits::quick_div(p[_Pos1],_Pos2);
            size_t low_l = traits::bit_length - _Pos2;
            if (_K <= low_l || _Pos1 == ptr.size() - 1) {
                p[_Pos1] += (val - get_mod(low)) * traits::get_base_power(_Pos2);
            }
            else {
                size_t high_l = _K - low_l;
                value_type high = traits::quick_mod(p[_Pos1 + 1], high_l);
                value_type val_high = traits::quick_div(val,low_l);
                value_type val_low = val - val_high * traits::get_base_power(low_l);
                p[_Pos1] += (val_low - low) * traits::get_base_power(_Pos2);
                p[_Pos1 + 1] += (val_high - high);
            }
            return *this;
        }
        virtual_unsigned_biginteger_reference& zero_change(value_type val) {
            WASSERT_LEVEL_2((value_type)(*this) == 0);
            auto p = ptr.data();
            size_t _Pos1 = pos1();
            size_t _Pos2 = pos2();
            size_t low_l = traits::bit_length - _Pos2;
            if (_K <= low_l || _Pos1 == ptr.size() - 1) {
                p[_Pos1] += val * traits::get_base_power(_Pos2);
            }
            else {
                size_t high_l = _K - low_l;
                value_type val_high = traits::quick_div(val, low_l);
                value_type val_low = val - val_high * traits::get_base_power(low_l);
                p[_Pos1] += val_low * traits::get_base_power(_Pos2);
                p[_Pos1 + 1] += val_high;
            }
            return *this;
        }
    private:
        constexpr value_type get_mod(value_type val) const {
            if constexpr (traits::get_mode == traits::mode::is_full) {
                return _K != traits::bit_length ? traits::quick_mod(val,_K)
                    : val;
            }
            else {
                return traits::quick_mod(val, _K);
            }
        }
        size_t index;
        const size_t _K;
        virtual_unsigned_biginteger ptr;
    };

    template<size_t base, size_t length>
    class virtual_unsigned_biginteger_const_iterator {
        using traits = biginteger_traits<base>;
        using virtual_unsigned_biginteger = wjr::virtual_unsigned_biginteger<base, length>;
    public:
        using value_type = typename traits::type;
        using pointer = virtual_unsigned_biginteger&;
        using const_pointer = const pointer;
        using reference = virtual_unsigned_biginteger_reference<base, length>;
        using const_reference = const reference;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;
        virtual_unsigned_biginteger_const_iterator(const virtual_unsigned_biginteger&ptr, size_t _K)
            : ptr(ptr)
            , _K(_K >= traits::bit_length ? traits::bit_length : _K) {}
        virtual_unsigned_biginteger_const_iterator(const virtual_unsigned_biginteger&ptr,size_t _K,
            size_t index) : ptr(ptr),_K(_K >= traits::bit_length ? traits::bit_length : _K),
            index(index) {
            WASSERT_LEVEL_1(vaild());
        }
        virtual_unsigned_biginteger_const_iterator(
            const virtual_unsigned_biginteger_const_iterator&) = default;
        virtual_unsigned_biginteger_const_iterator& operator=(
            const virtual_unsigned_biginteger_const_iterator&) = default;
        size_t pos1()const {
            return index / traits::bit_length;
        }
        size_t pos2()const {
            return index % traits::bit_length;
        }
        bool vaild()const {
            return pos1() < ptr.size();
        }
        bool is_last()const {
            return pos1() == ptr.size() - 1;
        }
        virtual_unsigned_biginteger_const_iterator& operator++() {
            index += _K;
            return *this;
        }
        virtual_unsigned_biginteger_const_iterator operator++(int) {
            virtual_unsigned_biginteger_const_iterator it(*this);
            ++(*this);
            return it;
        }
        virtual_unsigned_biginteger_const_iterator& operator+=(size_t delta) {
            index += delta * _K;
            return *this;
        }
        virtual_unsigned_biginteger_const_iterator operator+(size_t delta) const {
            virtual_unsigned_biginteger_const_iterator it(*this);
            it += delta;
            return it;
        }
        virtual_unsigned_biginteger_const_iterator& operator--() {
            index -= _K;
            return *this;
        }
        virtual_unsigned_biginteger_const_iterator operator--(int) {
            virtual_unsigned_biginteger_const_iterator it(*this);
            --(*this);
            return it;
        }
        virtual_unsigned_biginteger_const_iterator& operator-=(size_t delta) {
            index -= _K * delta;
            return *this;
        }
        virtual_unsigned_biginteger_const_iterator operator-(size_t delta)const {
            virtual_unsigned_biginteger_const_iterator it(*this);
            it -= delta;
            return *this;
        }
        const_reference operator*()const {
            return const_reference(ptr, _K, index);
        }
        const_pointer operator->()const {
            return ptr;
        }
        friend difference_type operator-(
            const virtual_unsigned_biginteger_const_iterator& lhs,
            const virtual_unsigned_biginteger_const_iterator& rhs
            ) {
            WASSERT_LEVEL_1(lhs._K == rhs._K);
            return (rhs.index - lhs.index) / lhs._K;
        }
        friend bool operator==(
            const virtual_unsigned_biginteger_const_iterator& lhs,
            const virtual_unsigned_biginteger_const_iterator& rhs
            ) {
            return memcmp(std::addressof(lhs), std::addressof(rhs), sizeof(lhs)) == 0;
        }
        friend bool operator!=(
            const virtual_unsigned_biginteger_const_iterator& lhs,
            const virtual_unsigned_biginteger_const_iterator& rhs
            ) {
            return !(lhs == rhs);
        }
    protected:
        size_t index = 0;
        const size_t _K;
        virtual_unsigned_biginteger ptr;
    };

    template<size_t base, size_t length>
    class virtual_unsigned_biginteger_iterator
        : public virtual_unsigned_biginteger_const_iterator<base, length> {
        using _Base = virtual_unsigned_biginteger_const_iterator<base, length>;
        using traits = biginteger_traits<base>;
        using virtual_unsigned_biginteger = wjr::virtual_unsigned_biginteger<base, length>;
    public:
        using value_type = typename traits::type;
        using pointer = virtual_unsigned_biginteger&;
        using const_pointer = const pointer;
        using reference = virtual_unsigned_biginteger_reference<base, length>;
        using const_reference = const reference;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;
        virtual_unsigned_biginteger_iterator(const virtual_unsigned_biginteger& ptr, size_t _K)
            : _Base(ptr, _K) {}
        virtual_unsigned_biginteger_iterator(const virtual_unsigned_biginteger& ptr, size_t _K,
            size_t index) : _Base(ptr, _K, index) {}
        virtual_unsigned_biginteger_iterator(const virtual_unsigned_biginteger_iterator&) = default;
        virtual_unsigned_biginteger_iterator& operator=(
            const virtual_unsigned_biginteger_iterator&) = default;
        virtual_unsigned_biginteger_iterator& operator++() {
            _Base::operator++();
            return *this;
        }
        virtual_unsigned_biginteger_iterator operator++(int) {
            _Base::operator++(0);
            return *this;
        }
        virtual_unsigned_biginteger_iterator& operator+=(size_t index) {
            _Base::operator+=(index);
            return *this;
        }
        virtual_unsigned_biginteger_iterator operator+(size_t index) const{
            virtual_unsigned_biginteger_iterator it(*this);
            it += index;
            return it;
        }
        virtual_unsigned_biginteger_iterator& operator--() {
            _Base::operator--();
            return *this;
        }
        virtual_unsigned_biginteger_iterator operator--(int) {
            _Base::operator--(0);
            return *this;
        }
        virtual_unsigned_biginteger_iterator& operator-=(size_t index) const{
            _Base::operator-=(index);
            return *this;
        }
        virtual_unsigned_biginteger_iterator operator-(size_t index) {
            virtual_unsigned_biginteger_iterator it(*this);
            it -= index;
            return *this;
        }
        reference operator*() {
            return reference(_Base::ptr, _Base::_K, _Base::index);
        }
        pointer operator->() {
            return _Base::ptr;
        }
        const_pointer operator->()const {
            return _Base::ptr;
        }
    };

    // unsigned biginteger or integeral view
    template<size_t base, size_t length>
    class virtual_unsigned_biginteger {
        static_assert(length <= 4, "");
        using traits = biginteger_traits<base>;
        template<size_t index, typename T>
        constexpr void _Make(T val) {
            if constexpr (index == traits::template bit_of_t<T> - 1) {
                _Data[index] = val;
                _Size = index + 1;
                WASSERT_LEVEL_2(traits::get_high(val) == 0);
            }
            else {
                _Data[index] = traits::get_low(val);
                auto _Val = traits::get_high(val);
                if (_Val) {
                    _Make<index + 1>(_Val);
                }
                else _Size = index + 1;
            }
        }
    public:
        using value_type = typename traits::type;
        using const_iterator = virtual_unsigned_biginteger_const_iterator<base, length>;
        using iterator = virtual_unsigned_biginteger_iterator<base, length>;
        template<typename T, std::enable_if_t<_Is_unsigned_integeral_v<T>, int> = 0>
        constexpr explicit virtual_unsigned_biginteger(T val) : _Data() {
            _Make<0>(val);
        }
        constexpr virtual_unsigned_biginteger(const virtual_unsigned_biginteger&) = default;
        constexpr virtual_unsigned_biginteger& operator=(
            const virtual_unsigned_biginteger&) = default;
        constexpr value_type* data() { return _Data; }
        constexpr const value_type* data()const { return _Data; }
        constexpr size_t size() const { return _Size; }
        constexpr value_type* begin() { return _Data; }
        constexpr const value_type* begin()const { return _Data; }
        constexpr value_type* end() { return _Data + _Size; }
        constexpr const value_type* end()const { return _Data + _Size; }
        bool zero()const {
            if constexpr (length == 1) {
                return _Data[0] == 0;
            }
            else {
                return _Size == 1 && _Data[0] == 0;
            }
        }
        iterator begin(size_t k) {
            return iterator(*this, k);
        }
        const_iterator begin(size_t k)const {
            return const_iterator(*this, k);
        }
        const_iterator cbegin(size_t k)const {
            return const_iterator(*this, k);
        }
        template<typename T>
        T to()const {
            T _Val = 0;
            for (size_t i = _Size; i--;) {
				WASSERT_LEVEL_2(_Data[i] <= std::numeric_limits<T>::max() 
                    && (_Val <= (std::numeric_limits<T>::max() - _Data[i]) / traits::max_value));
                _Val = _Val * traits::max_value + _Data[i];
            }
            return _Val;
        }
        template<typename T>
        explicit operator T()const {
            return to<T>();
        }
    private:
        size_t _Size = 0;
        value_type _Data[length];
    };

    template<size_t base>
    class virtual_unsigned_biginteger<base, 0> {
        using traits = biginteger_traits<base>;
    public:
        using value_type = typename traits::type;
        using const_iterator = virtual_unsigned_biginteger_const_iterator<base, 0>;
        using iterator = virtual_unsigned_biginteger_iterator<base, 0>;
        constexpr virtual_unsigned_biginteger(value_type* _Data, size_t _Size)
            : _Data(_Data), _Size(_Size) {}
        constexpr virtual_unsigned_biginteger(const value_type* _Data, size_t _Size)
            : _Data(const_cast<value_type*>(_Data)), _Size(_Size) {}
        constexpr virtual_unsigned_biginteger(const virtual_unsigned_biginteger&) = default;
        constexpr virtual_unsigned_biginteger& operator=(
            const virtual_unsigned_biginteger&) = default;
        constexpr value_type* data() { return _Data; }
        constexpr const value_type* data()const { return _Data; }
        constexpr size_t size()const { return _Size; }
        constexpr value_type* begin() { return _Data; }
        constexpr const value_type* begin()const { return _Data; }
        constexpr value_type* end() { return _Data + _Size; }
        constexpr const value_type* end()const { return _Data + _Size; }
        constexpr bool zero()const { return _Size == 1 && _Data[0] == 0; }
        iterator begin(size_t k) {
            return iterator(*this, k);
        }
        const_iterator begin(size_t k)const {
            return const_iterator(*this, k);
        }
        const_iterator cbegin(size_t k)const {
            return const_iterator(*this, k);
        }
        template<typename T>
        T to()const {
            T _Val = 0;
            for (size_t i = _Size; i--;) {
                WASSERT_LEVEL_2(_Data[i] <= std::numeric_limits<T>::max()
                    && (_Val <= (std::numeric_limits<T>::max() - _Data[i]) / traits::max_value));
                _Val = _Val * traits::max_value + _Data[i];
            }
            return _Val;
        }
        template<typename T>
        explicit operator T() const {
            return to<T>();
        }
        virtual_unsigned_biginteger& maintain() {
            while (_Size != 1 && !_Data[_Size - 1])--_Size;
            return *this;
        }
    private:
        value_type* _Data;
        size_t _Size;
    };

    template<size_t base>
    class unsigned_biginteger;

    template<typename T>
    struct unsigned_biginteger_base_helper {
        constexpr static size_t value = _Is_unsigned_integeral_v<T> ? 0 : -1;
    };

    template<size_t base>
    struct unsigned_biginteger_base_helper<unsigned_biginteger<base>> {
        constexpr static size_t value = base;
    };

    template<size_t base,size_t length>
    struct unsigned_biginteger_base_helper<virtual_unsigned_biginteger<base, length>> {
        constexpr static size_t value = base;
    };

    template<typename T>
    struct unsigned_biginteger_base : unsigned_biginteger_base_helper<std::decay_t<T>>{
    };

    template<typename T>
    constexpr static size_t unsigned_biginteger_base_v = unsigned_biginteger_base<T>::value;

    template<typename T,typename...Args>
    struct unsigned_biginteger_base_mask {
    private:
        constexpr static size_t lvalue = unsigned_biginteger_base_v<T>;
        constexpr static size_t rvalue = unsigned_biginteger_base_mask<Args...>::value;
        constexpr static size_t get_value() {
            if constexpr (lvalue == -1 || rvalue == -1) {
                // at least one is illegal
                return -1;
            }
            else if constexpr (lvalue == 0) {
                // if one is integral
                return rvalue;
            }
            else if constexpr (rvalue == 0) {
                return lvalue;
            }
            else if constexpr (lvalue == rvalue) {
                return lvalue;
            }
            else return -1; // illegal
        }
    public:
        constexpr static size_t value = get_value();
    };

    template<typename T>
    struct unsigned_biginteger_base_mask<T> {
        constexpr static size_t value = unsigned_biginteger_base_v<T>;
    };

    template<typename T,typename...Args>
    constexpr static size_t unsigned_biginteger_base_mask_v = 
        unsigned_biginteger_base_mask<T, Args...>::value;

    template<typename T>
    struct _Is_unsigned_biginteger : std::false_type {};

    template<size_t base>
    struct _Is_unsigned_biginteger<unsigned_biginteger<base>> : std::true_type {};

    template<typename T>
    constexpr static bool _Is_unsigned_biginteger_v = _Is_unsigned_biginteger<T>::value;

    template<typename T>
    struct _Is_original_virtual_unsigned_biginteger : std::false_type {};

    template<size_t base,size_t length>
    struct _Is_original_virtual_unsigned_biginteger<
        virtual_unsigned_biginteger<base, length>> : std::true_type {};

    template<typename T>
    constexpr static bool _Is_original_virtual_unsigned_biginteger_v = 
        _Is_original_virtual_unsigned_biginteger<T>::value;

    template<size_t base, size_t s>
    struct _Is_base_mask {
        constexpr static bool value = (base != -1) && (s == 0 || s == base);
    };

    template<size_t base, size_t s>
    constexpr static bool _Is_base_mask_v = _Is_base_mask<base, s>::value;

    template<size_t base, size_t...s>
    struct _Is_base_masks : std::conjunction<_Is_base_mask<base, s>...> {};

    template<size_t base, size_t...s>
    constexpr static bool _Is_base_masks_v = _Is_base_masks<base, s...>::value;

    template<size_t base, typename...Args>
    struct _Is_unsigned_biginteger_base_masks :
        std::conjunction<_Is_base_mask<base, unsigned_biginteger_base_v<Args>>...> {};

    template<size_t base, typename...Args>
    constexpr static bool _Is_unsigned_biginteger_base_masks_v =
        _Is_unsigned_biginteger_base_masks<base, Args...>::value;

    template<size_t base,typename T,
        std::enable_if_t<_Is_base_mask_v<base,unsigned_biginteger_base_v<T>>,int> = 0>
    constexpr static decltype(auto) get_virtual_unsigned_biginteger(const T& t) {
        using traits = biginteger_traits<base>;
        if constexpr (_Is_unsigned_integeral_v<T>) {
            return virtual_unsigned_biginteger<base, traits::template bit_of_t<T>>(t);
        }
        else if constexpr (_Is_unsigned_biginteger_v<T>) {
            return virtual_unsigned_biginteger<base, 0>(t);
        }
        else {
            return t;
        }
    }

#define REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(FUNC,...)		        \
        if constexpr (ir == 1) {					    	        \
            return FUNC<1>(__VA_ARGS__);					        \
        }													        \
        else if constexpr (ir == 2) {				    	        \
            if (rhs.size() == 1) {							        \
                return FUNC<1>(__VA_ARGS__);				        \
            }												        \
            else {											        \
                return FUNC<2>(__VA_ARGS__);				        \
            }												        \
        }													        \
        else if constexpr (ir == 3) {					            \
            switch (rhs.size()) {							        \
            case 1: return FUNC<1>(__VA_ARGS__);			        \
            case 2: return FUNC<2>(__VA_ARGS__);			        \
            case 3: return FUNC<3>(__VA_ARGS__);			        \
            }												        \
        }													        \
        else {												        \
            switch (rhs.size()) {							        \
            case 1: return FUNC<1>(__VA_ARGS__);			        \
            case 2: return FUNC<2>(__VA_ARGS__);			        \
            case 3: return FUNC<3>(__VA_ARGS__);			        \
            case 4: return FUNC<4>(__VA_ARGS__);			        \
            default:return FUNC<0>(__VA_ARGS__);			        \
            }												        \
        }													        \

#define VIRTUAL_UNSIGNED_BIGINTEGER_DECLARATION                                                 \
    template<typename T, typename U,                                                            \
    std::enable_if_t<_Is_unsigned_biginteger_base_masks_v<base, T, U>, int> = 0>                \

    template<size_t base>
    class biginteger;

    template<size_t base>
    class unsigned_biginteger {

        template<size_t _Base>
        friend class biginteger;

        using traits = biginteger_traits<base>;
        template<size_t length>
        using virtual_unsigned_biginteger = wjr::virtual_unsigned_biginteger<base, length>;
        template<typename T>
        using _Is_virtual_unsigned_biginteger = 
            _Is_base_mask<base, unsigned_biginteger_base_v<T>>;
        template<typename T>
        constexpr static bool _Is_virtual_unsigned_biginteger_v = 
            _Is_virtual_unsigned_biginteger<T>::value;
        template<typename T>
        using _Is_rubint = std::conjunction<
            std::is_rvalue_reference<T>, 
            _Is_unsigned_biginteger<T>,
            _Is_virtual_unsigned_biginteger<T>>;
        template<typename T>
        constexpr static bool _Is_rubint_v = _Is_rubint<T>::value;

    public:
        using value_type = typename traits::type;
        using twice_value_type = typename traits::twice_type;
        unsigned_biginteger() : vec(1, 0) {}
        unsigned_biginteger(const unsigned_biginteger& other) = default;
        unsigned_biginteger(unsigned_biginteger&& other) noexcept = default;
        template<size_t length>
        explicit unsigned_biginteger(const virtual_unsigned_biginteger<length>& rhs)
            : vec(rhs.begin(), rhs.end()) {}
        template<typename T, std::enable_if_t<_Is_virtual_unsigned_biginteger_v<T>, int> = 0>
        explicit unsigned_biginteger(const T&val)
            : unsigned_biginteger(get_virtual_unsigned_biginteger<base>(val)) {}
        template<typename iter,typename Func>
        unsigned_biginteger(iter first, iter last, Func fn) {
            WASSERT_LEVEL_2(first <= last);
            auto n = last - first;
            size_t k = n / traits::bit_length;
            vec.resize(k + 1);
            virtual_unsigned_biginteger_iterator<base, 0>
                write(get_virtual_unsigned_biginteger<base>(*this), 1,n);
            while (first != last) {
                *(--write) = fn(*first);
                ++first;
            }
            maintain();
        }
        unsigned_biginteger& operator=(const unsigned_biginteger&) = default;
        unsigned_biginteger& operator=(unsigned_biginteger&& other) noexcept = default;
        template<size_t length>
        unsigned_biginteger& operator=(const virtual_unsigned_biginteger<length>& rhs) {
            vec.assign(rhs.begin(), rhs.end());
            return *this;
        }
        template<typename T, std::enable_if_t<_Is_virtual_unsigned_biginteger_v<T>, int> = 0>
        unsigned_biginteger& operator=(const T&val) {
            return ((*this) = get_virtual_unsigned_biginteger<base>(val));
        }
        template<typename iter, typename Func>
        unsigned_biginteger& assign(iter first, iter last, Func fn) {
            WASSERT_LEVEL_2(first <= last);
            auto n = last - first;
            size_t k = n / traits::bit_length;
            vec.clear();
            vec.resize(k + 1);
            virtual_unsigned_biginteger_iterator<base, 0>
                write(get_virtual_unsigned_biginteger<base>(*this), 1, n);
            while (first != last) {
                *(--write) = fn(*first);
                ++first;
            }
            maintain();
            return *this;
        }

        value_type* data() { return vec.data(); }
        const value_type* data()const { return vec.data(); }
        size_t size()const { return vec.size(); }
        bool zero()const { return size() == 1 && *data() == 0; }
        void set_zero() { this->vec.clear(); this->vec.push_back(0); }

        explicit operator virtual_unsigned_biginteger<0>() {
            return virtual_unsigned_biginteger<0>(data(), size());
        }

        explicit operator virtual_unsigned_biginteger<0>() const {
            return virtual_unsigned_biginteger<0>(data(), size());
        }

        unsigned_biginteger& quick_mul_base_power(size_t index) {
            if (!index || zero())return *this;
            size_t n = size();
            size_t k = index / traits::bit_length;
            size_t r = index % traits::bit_length;
            if (!r) {
                vec.insert(vec.begin(), k, 0);
            }
            else {
                value_type mul_tag = traits::get_base_power(r);
                value_type mod_tag = traits::get_base_power(traits::bit_length - r);
                libdivide::divider<value_type> Mod(mod_tag);
                size_t delta = k + (vec[n - 1] >= mod_tag);
                vec.resize(n + delta);
                auto p = data();
                if (p[n - 1] >= mod_tag) {
                    p[n + delta - 1] = p[n - 1] / Mod;
                    --delta;
                }
                for (size_t i = n - 1; i; --i) {
                    p[i + delta] = (p[i] - (p[i] / Mod) * mod_tag) * mul_tag + p[i - 1] / Mod;
                }
                p[delta] = (p[0] - (p[0] / Mod) * mod_tag) * mul_tag;
                memset(p, 0, sizeof(value_type) * delta);
            }
            return *this;
        }

        unsigned_biginteger& quick_divide_base_power(size_t index) {
            if (!index || zero())return * this;
            size_t n = size();
            size_t k = index / traits::bit_length;
            size_t r = index % traits::bit_length;
            if (k >= n) {
                set_zero();
                return * this;
            }
            vec.erase(vec.begin(), vec.begin() + k);
            n = size();
            if (r) {
                value_type mod_tag = traits::get_base_power(r);
                value_type mul_tag = traits::get_base_power(traits::bit_length - r);
				libdivide::divider<value_type> Mod(mod_tag);
                auto p = data();
                for (size_t i = 0; i != n - 1; ++i) {
                    p[i] = (p[i] / Mod) + ((p[i + 1] - (p[i+1] / Mod) * mod_tag) * mul_tag);
                }
                p[n - 1] = p[n - 1] / Mod;
                while (n && !p[n - 1])--n;
                vec.resize(n);
            }
            return *this;
        }

        void maintain() {
            size_t n = size();
            auto p = data();
            while (n != 1 && !p[n - 1])--n;
            vec.resize(n);
        }

    private:
        template<size_t index,size_t il,size_t ir>
        static bool QEqual(
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs);
        template<size_t il,size_t ir>
        static bool Equal(
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs) {
            REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(QEqual, lhs, rhs);
        }

        template<size_t index, size_t il, size_t ir>
        static bool QLess(
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs);
        template<size_t il, size_t ir>
        static bool Less(
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs) {
            REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(QLess, lhs, rhs);
        }

        template<size_t index, size_t il, size_t ir>
        static bool QLess_eq(
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs);
        template<size_t il, size_t ir>
        static bool Less_eq(
            const virtual_unsigned_biginteger<il>& lhs,
            const virtual_unsigned_biginteger<ir>& rhs) {
            REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(QLess_eq, lhs, rhs);
        }

        template<size_t index, size_t ir>
        void QAdd(const virtual_unsigned_biginteger<ir>& rhs);
        template<size_t ir>
        void Add(const virtual_unsigned_biginteger<ir>& rhs) {
            REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(QAdd,rhs);
        }

        template<size_t index, size_t ir>
        void QSub(const virtual_unsigned_biginteger<ir>& rhs);
        template<size_t ir>
        void Sub(const virtual_unsigned_biginteger<ir>& rhs) {
            REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(QSub, rhs);
        }

        template<size_t index, size_t ir>
        void QSub_by(const virtual_unsigned_biginteger<ir>& rhs);
        template<size_t ir>
        void Sub_by(const virtual_unsigned_biginteger<ir>& rhs) {
            REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(QSub_by, rhs);
        }

        template<size_t index, size_t length>
        void slow_mul(const virtual_unsigned_biginteger<length>& rhs);
        template<size_t length>
        void fft_mul(const virtual_unsigned_biginteger<length>& rhs);
        template<size_t index, size_t length>
        void QMul(const virtual_unsigned_biginteger<length>& rhs);
        template<size_t ir>
        void Mul(const virtual_unsigned_biginteger<ir>& rhs) {
            REGISTER_VIRTUAL_BIGINTEGER_FUNCTION(QMul, rhs);
        }

    public:
        template<typename T,std::enable_if_t<_Is_virtual_unsigned_biginteger_v<T>,int> = 0>
        void add(const T& rhs) {
            Add(get_virtual_unsigned_biginteger<base>(rhs));
        }

        template<typename T, std::enable_if_t<_Is_virtual_unsigned_biginteger_v<T>, int> = 0>
        void sub(const T& rhs) {
            Sub(get_virtual_unsigned_biginteger<base>(rhs));
        }

        template<typename T, std::enable_if_t<_Is_virtual_unsigned_biginteger_v<T>, int> = 0>
        void sub_by(const T& rhs) {
            Sub_by(get_virtual_unsigned_biginteger<base>(rhs));
        }

        template<typename T, std::enable_if_t<_Is_virtual_unsigned_biginteger_v<T>, int> = 0>
        void mul(const T& rhs) {
            Mul(get_virtual_unsigned_biginteger<base>(rhs));
        }

        VIRTUAL_UNSIGNED_BIGINTEGER_DECLARATION
        static bool equal(const T&lhs,const U&rhs){
            return Equal(
                get_virtual_unsigned_biginteger<base>(lhs),
                get_virtual_unsigned_biginteger<base>(rhs)
            );
        }
        VIRTUAL_UNSIGNED_BIGINTEGER_DECLARATION
        static bool nequal(const T&lhs,const U&rhs){
            return !equal(lhs, rhs);
        }
        VIRTUAL_UNSIGNED_BIGINTEGER_DECLARATION
        static bool less(const T&lhs,const U&rhs){
            return Less(
                get_virtual_unsigned_biginteger<base>(lhs),
                get_virtual_unsigned_biginteger<base>(rhs)
            );
        }
        VIRTUAL_UNSIGNED_BIGINTEGER_DECLARATION
        static bool less_eq(const T&lhs,const U&rhs){
            return Less_eq(
                get_virtual_unsigned_biginteger<base>(lhs),
                get_virtual_unsigned_biginteger<base>(rhs)
            );
        }
        VIRTUAL_UNSIGNED_BIGINTEGER_DECLARATION
        static unsigned_biginteger add(T&&lhs,U&&rhs){
            if constexpr (_Is_rubint_v<T>) {
                lhs.add(std::forward<U>(rhs));
                return std::forward<T>(lhs);
            }
            else if constexpr (_Is_rubint_v<U>) {
                rhs.add(std::forward<T>(lhs));
                return std::forward<U>(rhs);
            }
            else {
                unsigned_biginteger result(std::forward<T>(lhs));
                result.add(std::forward<U>(rhs));
                return result;
            }
        }
        VIRTUAL_UNSIGNED_BIGINTEGER_DECLARATION
        static unsigned_biginteger sub(T&&lhs,U&&rhs){
            if constexpr (_Is_rubint_v<T>) {
                lhs.sub(std::forward<U>(rhs));
                return std::forward<T>(lhs);
            }
            else if constexpr (_Is_rubint_v<U>) {
                rhs.sub_by(std::forward<T>(lhs));
                return std::forward<U>(rhs);
            }
            else {
                unsigned_biginteger result(std::forward<T>(lhs));
                result.sub(std::forward<U>(rhs));
                return result;
            }
        }
        VIRTUAL_UNSIGNED_BIGINTEGER_DECLARATION
        static unsigned_biginteger mul(T&&lhs,U&&rhs){
            if constexpr (_Is_rubint_v<T>) {
                lhs.mul(std::forward<U>(rhs));
                return std::forward<T>(lhs);
            }
            else if constexpr (_Is_rubint_v<U>) {
                rhs.mul(std::forward<T>(lhs));
                return std::forward<U>(rhs);
            }
            else {
                unsigned_biginteger result(std::forward<T>(lhs));
                result.mul(std::forward<U>(rhs));
                return result;
            }
        }
    private:

        std::vector<value_type, mallocator<value_type>> vec;
    };

    template<size_t base>
    template<size_t index, size_t il,size_t ir>
    bool unsigned_biginteger<base>::QEqual(
        const virtual_unsigned_biginteger<il>&lhs,
        const virtual_unsigned_biginteger<ir>&rhs) {
        size_t n = lhs.size();
        if constexpr (index != 0) {
            if (n != index)return false;
            auto p = lhs.data();
            auto q = rhs.data();
            if (index == 1) {
                return p[0] == q[0];
            }
            else if (index == 2) {
                return p[0] == q[0] && p[1] == q[1];
            }
            else if (index == 3) {
                return p[0] == q[0] && p[1] == q[1] && p[2] == q[2];
            }
            else {
                return p[0] == q[0] && p[1] == q[1] && p[2] == q[2] && p[3] == q[3];
            }
        }
        else {
            return (n == rhs.size()) && 
                (memcmp(lhs.data(), rhs.data(), sizeof(value_type) * n) == 0);
        }
    }

#define GENERATE_BIGINTEGER_OPERATOR(op,F)						\
    size_t n = lhs.size();										\
    if constexpr(index != 0){									\
        if (n != index) return n op index;						\
        auto p = lhs.data();									\
        auto q = rhs.data();									\
        if constexpr(index == 1) {								\
            return p[0] op q[0];								\
        }else if constexpr (index == 2){						\
            return p[1] != q[1] ? p[1] op q[1] : p[0] op q[0];	\
        }else if constexpr (index == 3){						\
            return p[2] != q[2] ? p[2] op q[2] : p[1] != q[1] ?	\
                p[1] op q[1] : p[0] op q[0];					\
        }else {													\
            return p[3] != q[3] ? p[3] op q[3] : p[2] != q[2] ?	\
                p[2] op q[2] : p[1] != q[1] ? p[1] op q[1] :	\
                p[0] op q[0];									\
        }														\
    }else {														\
        size_t m = rhs.size();									\
        if (n != m) return n op m;								\
        auto p = lhs.data() + n;								\
        auto q = rhs.data() + n;								\
        for (; n--;) {											\
            --p;												\
            --q;												\
            if (*p != *q)return *p op * q;						\
        }														\
        return F;												\
    }

    template<size_t base>
    template<size_t index, size_t il,size_t ir>
    bool unsigned_biginteger<base>::QLess(
        const virtual_unsigned_biginteger<il>&lhs,
        const virtual_unsigned_biginteger<ir>&rhs) {
        GENERATE_BIGINTEGER_OPERATOR(< , false);
    }

    template<size_t base>
    template<size_t index, size_t il,size_t ir>
    bool unsigned_biginteger<base>::QLess_eq(
        const virtual_unsigned_biginteger<il>&lhs,
        const virtual_unsigned_biginteger<ir>&rhs) {
        GENERATE_BIGINTEGER_OPERATOR(<=, true);
    }

    template<size_t base>
    template<size_t index, size_t length>
    void unsigned_biginteger<base>::
        QAdd(const virtual_unsigned_biginteger<length>& rhs) {
        size_t n = size();
        auto q = rhs.data();
        value_type jw = 0;
        if constexpr (index != 0) {
            vec.reserve(index + 1);
            if constexpr (index != 1) {
                if (n < index) {
                    vec.resize(index, 0);
                    n = index;
                }
            }
            auto p = data();
            if constexpr (index >= 1) {
                p[0] = traits::quick_add(p[0], q[0], jw);
            }
            if constexpr (index >= 2) {
                p[1] = traits::quick_add(p[1], q[1], jw);
            }
            if constexpr (index >= 3) {
                p[2] = traits::quick_add(p[2], q[2], jw);
            }
            if constexpr (index >= 4) {
                p[3] = traits::quick_add(p[3], q[3], jw);
            }
            if (jw) {
                size_t pos = index;
                for (; pos != n && p[pos] == traits::_max; ++pos)p[pos] = 0;
                if (pos == n)vec.push_back(1);
                else ++p[pos];
            }
        }
        else {
            size_t m = rhs.size();
            vec.reserve(m + 1);
            size_t _Min;
            if (n < m) {
                _Min = n;
                vec.insert(vec.end(), rhs.data() + n, rhs.data() + m);
                n = m;
            }
            else _Min = m;
            auto p = data();
            for (size_t i = 0; i < _Min; ++i) {
                p[i] = traits::quick_add(p[i], q[i], jw);
            }
            if (jw) {
                size_t pos = _Min;
                for (; pos != n && p[pos] == traits::_max; ++pos)p[pos] = 0;
                if (pos == n)vec.push_back(1);
                else ++p[pos];
            }
        }
    }

    template<size_t base>
    template<size_t index, size_t length>
    void unsigned_biginteger<base>::
        QSub(const virtual_unsigned_biginteger<length>& rhs) {
        size_t n = size();
        auto p = data();
        auto q = rhs.data();
        value_type jw = 0;
        if constexpr (index != 0) {
            if constexpr (index >= 1) {
                p[0] = traits::quick_sub(p[0], q[0], jw);
            }
            if constexpr (index >= 2) {
                p[1] = traits::quick_sub(p[1], q[1], jw);
            }
            if constexpr (index >= 3) {
                p[2] = traits::quick_sub(p[2], q[2], jw);
            }
            if constexpr (index >= 4) {
                p[3] = traits::quick_sub(p[3], q[3], jw);
            }
            if (jw) {
                size_t pos = index;
                for (; pos != n && !p[pos]; ++pos)p[pos] = traits::_max;
                --p[pos];
            }
        }
        else {
            size_t m = rhs.size();
            for (size_t i = 0; i < m; ++i) {
                p[i] = traits::quick_sub(p[i], q[i], jw);
            }
            if (jw) {
                size_t pos = m;
                for (; pos != n && !p[pos]; ++pos)p[pos] = traits::_max;
                --p[pos];
            }
        }
        while (n != 1 && !p[n - 1])--n;
        vec.resize(n);
    }

    template<size_t base>
    template<size_t index, size_t length>
    void unsigned_biginteger<base>::
        QSub_by(const virtual_unsigned_biginteger<length>& rhs) {
        size_t n = size();
        auto q = rhs.data();
        value_type jw = 0;
        if constexpr (index != 0) {
            this->vec.resize(index, 0);
            n = index;
            auto p = data();
            if constexpr (index >= 1) {
                p[0] = traits::quick_sub(q[0], p[0], jw);
            }
            if constexpr (index >= 2) {
                p[1] = traits::quick_sub(q[1], p[1], jw);
            }
            if constexpr (index >= 3) {
                p[2] = traits::quick_sub(q[2], p[2], jw);
            }
            if constexpr (index >= 4) {
                p[3] = traits::quick_sub(q[3], p[3], jw);
            }
            assert(jw == 0);
            while (n != 1 && !p[n - 1])--n;
            this->vec.resize(n);
        }
        else {
            size_t m = rhs.size();
            this->vec.insert(this->vec.end(), rhs.data() + n, rhs.data() + m);
            auto p = data();
            for (size_t i = 0; i != n; ++i) {
                p[i] = traits::quick_sub(q[i], p[i], jw);
            }
            if (jw) {
                size_t pos = n;
                for (; pos != m && !p[pos]; ++pos)p[pos] = traits::_max;
                --p[pos];
            }
            n = m;
            while (n != 1 && !p[n - 1])--n;
            this->vec.resize(n);
        }
    }

    template<size_t base>
    template<size_t index, size_t length>
    void unsigned_biginteger<base>::
        slow_mul(const virtual_unsigned_biginteger<length>& rhs) {
        USE_THREAD_LOCAL static twice_value_type static_array[256];
        size_t n = size();
        size_t m = rhs.size();
        size_t len = n + m;
        this->vec.reserve(len);
        auto p = data();
        auto q = rhs.data();
        twice_value_type* temp_array;
        if constexpr (index != 1) {
            if (len <= 256) temp_array = static_array;
            else temp_array = new twice_value_type[len];
            memset(temp_array, 0, sizeof(twice_value_type) * len);
        }
        if constexpr (index != 0) {
            if constexpr (index == 1) {
                twice_value_type _Val = 0;
                auto qc = static_cast<twice_value_type>(*q);
                for (size_t i = 0; i != n; ++i) {
                    _Val += qc * p[i];
                    p[i] = traits::get_low(_Val);
                    _Val = traits::get_high(_Val);
                }
                if (_Val) {
                    this->vec.push_back(static_cast<value_type>(_Val));
                }
            }
            else {
                if constexpr (index >= 1) {
                    for (size_t i = 0; i < n; ++i) {
                        auto val = static_cast<twice_value_type>(p[i]) * q[0] + temp_array[i];
                        temp_array[i + 1] += traits::get_high(val);
                        temp_array[i] = traits::get_low(val);
                    }
                }
                if constexpr (index >= 2) {
                    for (size_t i = 0; i < n; ++i) {
                        auto val = static_cast<twice_value_type>(p[i]) * q[1] + temp_array[i + 1];
                        temp_array[i + 2] += traits::get_high(val);
                        temp_array[i + 1] = traits::get_low(val);
                    }
                }
                if constexpr (index >= 3) {
                    for (size_t i = 0; i < n; ++i) {
                        auto val = static_cast<twice_value_type>(p[i]) * q[2] + temp_array[i + 2];
                        temp_array[i + 3] += traits::get_high(val);
                        temp_array[i + 2] = traits::get_low(val);
                    }
                }
                if constexpr (index >= 4) {
                    for (size_t i = 0; i < n; ++i) {
                        auto val = static_cast<twice_value_type>(p[i]) * q[3] + temp_array[i + 3];
                        temp_array[i + 4] += traits::get_high(val);
                        temp_array[i + 3] = traits::get_low(val);
                    }
                }
            }
        }
        else {
            for (size_t i = 0; i < n; ++i) {
                for (size_t j = 0; j < m; ++j) {
                    const auto val = static_cast<uint64_t>(p[i]) * q[j] + temp_array[i + j];
                    temp_array[i + j + 1] += traits::get_high(val);
                    temp_array[i + j] = traits::get_low(val);
                }
            }
        }

        if constexpr (index != 1) {
            this->vec.assign(temp_array, temp_array + len - 1);
            if (temp_array[len - 1]) {
                WASSERT_LEVEL_1(temp_array[len - 1] <= traits::_max);
                this->vec.push_back(temp_array[len - 1]);
            }
            if (n + m > 256)delete[]temp_array;
        }
    }

    template<size_t base>
    template<size_t length>
    void unsigned_biginteger<base>::
        fft_mul(const virtual_unsigned_biginteger<length>& rhs) {
        constexpr uint64_t _Maxn = (uint64_t)(1) << 49;
        size_t pn = size();
        size_t pm = rhs.size();
        size_t plen = (pn + pm) * traits::bit_length;
        size_t pk = 1;
        uint64_t G = base * base;
        uint64_t X = G;
        uint64_t maxn = _Maxn / G;
        // X * G * plen / (pk + 1) <= Maxn
        for (; X <= maxn / ((plen + pk) / (pk + 1)); ++pk, X *= G) {
            WASSERT_LEVEL_2(pk <= traits::bit_length);
        }

        virtual_unsigned_biginteger<0> lhs(data(), size());
        auto lhs_iter = lhs.cbegin(pk);
        auto rhs_iter = rhs.cbegin(pk);
        size_t n = (pn * traits::bit_length + pk - 1) / pk;
        size_t m = (pm * traits::bit_length + pk - 1) / pk;
        size_t len = n + m - 1;
        auto bit = quick_log2(len - 1) + 1;
        auto s = ((uint64_t)1) << bit;

        auto a = new double[s << 1];
        for (size_t i = 0; i < n; ++lhs_iter, ++i) {
            value_type val = *lhs_iter;
            a[i << 1]      = val * 0.5;
            a[i << 1 | 1]  = val * 0.5;
        }
        for (size_t i = (n << 1); i < (s << 1); ++i) {
            a[i] = 0;
        }
        for (size_t i = 0; i < m; ++rhs_iter, ++i) {
            value_type val = *rhs_iter;
            a[i << 1]     += val * 0.5;
            a[i << 1 | 1] -= val * 0.5;
        }

        cdft(s << 1, 1, a);
        for (size_t i = 0; i < s; ++i) {
            double sa     = a[i << 1];
            double sb     = a[i << 1 | 1];
            a[i << 1]     = sa * sa - sb * sb;
            a[i << 1 | 1] = 2 * sa * sb;
        }

        cdft(s << 1, -1, a);
        double invs = 1.0 / static_cast<double>(s);
        for (size_t i = 0; i < s; ++i) {
            a[i << 1] *= invs;
        }

        this->vec.clear();
        this->vec.resize(pn + pm, 0);
        virtual_unsigned_biginteger<0> vir(data(), size());
        auto write = vir.begin(pk);
        uint64_t val = 0;
        uint64_t Mod = traits::get_base_power(pk);
		libdivide::divider<uint64_t> divider(Mod);
        for (size_t i = 0; i < len; ++i, ++write) {
            val += static_cast<uint64_t>(a[i << 1] + 0.5);
            uint64_t d = val / divider;
            (*write) = val - d * Mod;
            val = d;
        }
        while (val) {
            uint64_t d = val / divider;
            *(write) = val - d * Mod;
            val = d;
        }

        size_t pos = size();
        auto p = data();
        while (pos != 1 && !p[pos - 1])--pos;
        vec.resize(pos, 0);
        delete[]a;
    }

    template<size_t base>
    template<size_t index, size_t length>
    void unsigned_biginteger<base>::
        QMul(const virtual_unsigned_biginteger<length>& rhs) {
        size_t n = size();
        if (zero()) {
            return;
        }
        if constexpr (index != 0) {
            if constexpr (index == 1) {
                if (rhs.zero()) {
                    this->set_zero();
                    return;
                }
            }
            slow_mul<index>(rhs);
        }
        else {
            size_t m = rhs.size();
            size_t _Min = n < m ? n : m;
            size_t _Max = n > m ? n : m;
            if (_Min <= 32 || 
                ((uint64_t)(1) << (std::min(uint64_t(60),((_Min - 32) / 2)))) <= _Max) {
                slow_mul<index>(rhs);
            }
            else {
                fft_mul(rhs);
            }
        }
    }

    // signed biginteger or integeral view
    template<size_t base, size_t length>
    class virtual_biginteger : public virtual_unsigned_biginteger<base, length> {
        using _Base = virtual_unsigned_biginteger<base, length>;
    public:
        template<typename T, std::enable_if_t<_Is_unsigned_integeral_v<T>, int> = 0>
        constexpr explicit virtual_biginteger(T val)
            : _Base(val) {}
        template<typename T, std::enable_if_t<_Is_signed_integeral_v<T>, int> = 0>
        constexpr explicit virtual_biginteger(T val)
            : _Signal(val >= 0),
            _Base(static_cast<std::make_unsigned_t<T>>(val >= 0 ? (val) : (0 - val))) {}
        constexpr explicit virtual_biginteger(
            const virtual_unsigned_biginteger<base, length>& rhs) : _Base(rhs) {}
        constexpr explicit virtual_biginteger(
            bool _Signal,
            const virtual_unsigned_biginteger<base, length>& rhs) : _Signal(_Signal), _Base(rhs) {}
        constexpr virtual_biginteger(const virtual_biginteger&) = default;
        constexpr virtual_biginteger& operator=(const virtual_biginteger&) = default;
        constexpr bool signal()const { return _Signal; }
        constexpr _Base& get_unsigned() { return *this; }
        constexpr const _Base& get_unsigned()const { return *this; }
        template<typename T>
        explicit operator T()const {
            auto _Val = _Base::template to<T>();
            return _Signal ? _Val : (0 - _Val);
        }
        void change_signal() { 
            _Signal = _Base::zero() || (!_Signal);
        }
        void abs() {
            _Signal = true;
        }
    private:
        bool _Signal = true;
    };

    template<size_t base>
    class virtual_biginteger<base, 0> : public virtual_unsigned_biginteger<base, 0> {
        using _Base = virtual_unsigned_biginteger<base, 0>;
        using value_type = typename _Base::value_type;
    public:
        constexpr virtual_biginteger(value_type* _Data, size_t _Size)
            : _Base(_Data, _Size) {}
        constexpr virtual_biginteger(const value_type* _Data, size_t _Size)
            : _Base(_Data, _Size) {}
        constexpr virtual_biginteger(bool _Signal, value_type* _Data, size_t _Size)
            : _Signal(_Signal), _Base(_Data, _Size) {}
        constexpr virtual_biginteger(bool _Signal, const value_type* _Data, size_t _Size)
            : _Signal(_Signal), _Base(_Data, _Size) {}
        constexpr explicit virtual_biginteger(
            const virtual_unsigned_biginteger<base, 0>& rhs) : _Base(rhs) {}
        constexpr explicit virtual_biginteger(
            bool _Signal,
            const virtual_unsigned_biginteger<base, 0>& rhs) : _Signal(_Signal), _Base(rhs) {}
        constexpr virtual_biginteger(const virtual_biginteger&) = default;
        constexpr virtual_biginteger& operator=(const virtual_biginteger&) = default;
        bool signal()const { 
            WASSERT_LEVEL_1(!(_Base::zero() && !_Signal));
            return _Signal; 
        }
        constexpr _Base& get_unsigned() { 
            return *this; 
        }
        constexpr const _Base& get_unsigned()const { 
            return *this; 
        }
        template<typename T>
        explicit operator T()const {
            auto _Val = _Base::template to<T>();
            return _Signal ? _Val : (0 - _Val);
        }
        virtual_biginteger& maintain() {
            _Base::maintain();
            if (_Base::zero()) {
                _Signal = true;
            }
            return *this;
        }
        void change_signal() {
            _Signal = _Base::zero() || (!_Signal);
        }
        void abs() {
            _Signal = true;
        }
    private:
        bool _Signal = true;
    };

    template<typename T>
    struct biginteger_base_helper {
        constexpr static size_t value = std::is_integral_v<T> ? 0 : -1;
    };

    template<size_t base>
    struct biginteger_base_helper<unsigned_biginteger<base>> {
        constexpr static size_t value = base;
    };

    template<size_t base>
    struct biginteger_base_helper<biginteger<base>> {
        constexpr static size_t value = base;
    };

    template<size_t base, size_t length>
    struct biginteger_base_helper<virtual_unsigned_biginteger<base, length>> {
        constexpr static size_t value = base;
    };

    template<size_t base, size_t length>
    struct biginteger_base_helper<virtual_biginteger<base, length>> {
        constexpr static size_t value = base;
    };

    template<typename T>
    struct biginteger_base : biginteger_base_helper<std::decay_t<T>> {
    };

    template<typename T>
    constexpr static size_t biginteger_base_v = biginteger_base<T>::value;

    template<typename T, typename...Args>
    struct biginteger_base_mask {
    private:
        constexpr static size_t lvalue = biginteger_base_v<T>;
        constexpr static size_t rvalue = biginteger_base_mask<Args...>::value;
        constexpr static size_t get_value() {
            if constexpr (lvalue == -1 || rvalue == -1) {
                // at least one is illegal
                return -1;
            }
            else if constexpr (lvalue == 0) {
                // if one is integral
                return rvalue;
            }
            else if constexpr (rvalue == 0) {
                return lvalue;
            }
            else if constexpr (lvalue == rvalue) {
                return lvalue;
            }
            else return -1; // illegal
        }
    public:
        constexpr static size_t value = get_value();
    };

    template<typename T>
    struct biginteger_base_mask<T> {
        constexpr static size_t value = biginteger_base_v<T>;
    };

    template<typename T, typename...Args>
    constexpr static size_t biginteger_base_mask_v =
        biginteger_base_mask<T, Args...>::value;

    template<typename T>
    struct _Is_biginteger : std::false_type {};

    template<size_t base>
    struct _Is_biginteger<biginteger<base>> : std::true_type {};

    template<typename T>
    constexpr static bool _Is_biginteger_v = _Is_biginteger<T>::value;

    template<typename T>
    struct _Is_original_virtual_biginteger : std::false_type {};

    template<size_t base, size_t length>
    struct _Is_original_virtual_biginteger<
        virtual_biginteger<base, length>> : std::true_type {};

    template<typename T>
    constexpr static bool _Is_original_virtual_biginteger_v = _Is_original_virtual_biginteger<T>::value;

    template<size_t base, typename T,
        std::enable_if_t<_Is_base_mask_v<base,biginteger_base_v<T>>,int> = 0>
    constexpr static decltype(auto) get_virtual_biginteger(const T& t) {
        using traits = biginteger_traits<base>;
        if constexpr (std::is_integral_v<T>) {
            return virtual_biginteger<base, traits::template bit_of_t<T>>(t);
        }
        else if constexpr (_Is_unsigned_biginteger_v<T>) {
            return virtual_biginteger<base, 0>(true, virtual_biginteger<base, 0>(t));
        }
        else if constexpr (_Is_biginteger_v<T>) {
            return virtual_biginteger<base, 0>(t);
        }
        else if constexpr (_Is_original_virtual_unsigned_biginteger_v<T>) {
            return virtual_biginteger<base, 0>(true, t);
        }
        else return t;
    }

    template<size_t base, typename...Args>
    struct _Is_biginteger_base_masks :
        std::conjunction<_Is_base_mask<base, biginteger_base_v<Args>>...> {};

    template<size_t base, typename...Args>
    constexpr static bool _Is_biginteger_base_masks_v =
        _Is_biginteger_base_masks<base, Args...>::value;

#define VIRTUAL_BIGINTEGER_DECLARATION                                                          \
    template<typename T, typename U, size_t _Base = biginteger_base_mask_v<T, U>,               \
    std::enable_if_t<                                                                           \
    std::conjunction_v<_Is_biginteger_base_masks<_Base, T, U>>, int> = 0>                       \

#define VIRTUAL_BIGINTEGER_DEFINITION                                                           \
    template<typename T, typename U, size_t _Base,                                              \
    std::enable_if_t<                                                                           \
    std::conjunction_v<_Is_biginteger_base_masks<_Base, T, U>>, int> >                          \

    VIRTUAL_BIGINTEGER_DECLARATION
    bool operator==(const T& lhs, const U& rhs);
    VIRTUAL_BIGINTEGER_DECLARATION
    bool operator!=(const T& lhs, const U& rhs);
    VIRTUAL_BIGINTEGER_DECLARATION
    bool operator<(const T& lhs, const U& rhs);
    VIRTUAL_BIGINTEGER_DECLARATION
    bool operator<=(const T& lhs, const U& rhs);
    VIRTUAL_BIGINTEGER_DECLARATION
    bool operator>(const T& lhs, const U& rhs);
    VIRTUAL_BIGINTEGER_DECLARATION
    bool operator>=(const T& lhs, const U& rhs);
    
    VIRTUAL_BIGINTEGER_DECLARATION
    biginteger<_Base> operator+(T&&lhs,U&&rhs);
    VIRTUAL_BIGINTEGER_DECLARATION
    biginteger<_Base> operator-(T&&lhs,U&&rhs);
    VIRTUAL_BIGINTEGER_DECLARATION
    biginteger<_Base> operator*(T&&lhs,U&&rhs);
    VIRTUAL_BIGINTEGER_DECLARATION
    biginteger<_Base> operator/(T&&lhs,U&&rhs);
    VIRTUAL_BIGINTEGER_DECLARATION
    biginteger<_Base> operator%(T&&lhs,U&&rhs);

    template<size_t _Base>
    void divmod(biginteger<_Base>& lhs, biginteger<_Base>& rhs);

    template<size_t toBase,typename T,std::enable_if_t<_Is_biginteger_v<T>,int> = 0>
    biginteger<toBase> base_conversion(const T&);

    // generate a biginteger of length n
    template<size_t _Base>
    void random_biginteger(biginteger<_Base>& result,size_t n);
    template<size_t _Base>
    biginteger<_Base> random_biginteger(size_t n);

    template<size_t base>
    class biginteger {
        using traits = biginteger_traits<base>;
        using unsigned_traits = unsigned_biginteger<base>;
        template<size_t length>
        using virtual_biginteger = wjr::virtual_biginteger<base, length>;
        template<typename T>
        using _Is_virtual_biginteger =
            _Is_base_mask<base, biginteger_base_v<T>>;
        template<typename T>
        constexpr static bool _Is_virtual_biginteger_v =
            _Is_virtual_biginteger<T>::value;
        template<typename T>
        using _Is_rbint = std::conjunction<
            std::is_rvalue_reference<T>,
            _Is_biginteger<T>,
            _Is_virtual_biginteger<T>>;
        template<typename T>
        constexpr static bool _Is_rbint_v = _Is_rbint<T>::value;
    public:
        using value_type = typename traits::type;
        using twice_value_type = typename traits::twice_type;
        using iterator = virtual_unsigned_biginteger_iterator<base, 0>;
        using const_iterator = virtual_unsigned_biginteger_const_iterator<base, 0>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using reverse_const_iterator = std::reverse_iterator<const_iterator>;
        biginteger() = default;
        biginteger(const biginteger&) = default;
        biginteger(biginteger&&) noexcept = default;
        template<size_t length>
        explicit biginteger(const virtual_biginteger<length>& rhs)
            : _Signal(rhs.signal()), ubint(rhs.get_unsigned()) {}
        template<typename T,std::enable_if_t<_Is_virtual_biginteger_v<T>,int> = 0>
        explicit biginteger(const T& val)
            : biginteger(get_virtual_biginteger<base>(val)) {}
        template<typename iter,typename Func>
        biginteger(iter first, iter last, Func&&fn) 
            : ubint(first, last, std::forward<Func>(fn)) {}
        template<typename iter ,typename Func>
        biginteger(bool _Signal, iter first, iter last, Func&&fn)
            : _Signal(_Signal), ubint(first, last, std::forward<Func>(fn)) {
            maintain_signal();
        }
        biginteger& operator=(const biginteger&) = default;
        biginteger& operator=(biginteger&&) noexcept = default;
        template<size_t length>
        biginteger& operator=(const virtual_biginteger<length>& rhs) {
            _Signal = rhs.signal();
            ubint = rhs.get_unsigned();
            return *this;
        }
        template<typename T, std::enable_if_t<_Is_virtual_biginteger_v<T>, int> = 0>
        biginteger& operator=(const T& rhs) {
            (*this) = get_virtual_biginteger<base>(rhs);
            return *this;
        }
        template<typename iter, typename Func>
        biginteger& assign(bool _Sign, iter first, iter last, Func&& fn) {
            _Signal = _Sign;
            ubint.assign(first, last, std::forward<Func>(fn));
            maintain_signal();
            return *this;
        }

        value_type* data() { return ubint.data(); }
        const value_type* data()const { return ubint.data(); }
        size_t size()const { return ubint.size(); }
        bool zero()const { 
            bool f = (size() == 1 && *data() == 0);
            WASSERT_LEVEL_1(!(f && !_Signal));
            return f;
        }
        bool signal()const { return _Signal; }
        void set_zero() { _Signal = true; ubint.set_zero(); }
        void maintain_signal() { _Signal = ubint.zero() || _Signal; }
        void change_signal() { _Signal = ubint.zero() || (!_Signal); }
        void reserve(size_t k) { ubint.vec.reserve((k + traits::bit_length - 1) / traits::bit_length); }
        void resize(size_t k) { ubint.vec.resize((k + traits::bit_length - 1) / traits::bit_length); }
        void abs() { _Signal = true; }

        size_t bit_length() const{
            size_t _Pos = ubint.vec.size() - 1;
            return _Pos * traits::bit_length + quick_log<base>(ubint.vec[_Pos]) + 1;
        }
        iterator begin() {
            return iterator(
                get_virtual_unsigned_biginteger<base>(ubint),
                1
            );
        }
        const_iterator begin()const {
            return const_iterator(
                get_virtual_unsigned_biginteger<base>(ubint),
                1
            );
        }
        const_iterator cbegin()const {
            return const_iterator(
                get_virtual_unsigned_biginteger<base>(ubint),
                1
            );
        }

        iterator end() {
            return begin() + bit_length();
        }
        const_iterator end()const {
            return begin() + bit_length();
        }
        const_iterator cend() const{
            return begin() + bit_length();
        }
        reverse_iterator rbegin() {
            return reverse_iterator(end());
        }
        reverse_const_iterator rbegin()const {
            return reverse_const_iterator(end());
        }
        reverse_const_iterator rcbegin()const {
            return reverse_const_iterator(end());
        }
        reverse_iterator rend() {
            return reverse_iterator(begin());
        }
        reverse_const_iterator rend()const {
            return reverse_const_iterator(begin());
        }
        reverse_const_iterator rcend()const {
            return reverse_const_iterator(begin());
        }

        explicit operator virtual_biginteger<0>()const {
            return virtual_biginteger<0>(_Signal, ubint.data(), ubint.size());
        }

        virtual_biginteger<0> get_virtual()const {
            return virtual_biginteger<0>(*this);
        }

        virtual_unsigned_biginteger<base,0> get_unsigned_virtual()const {
            return get_virtual().get_unsigned();
        }

        template<typename T>
        explicit operator T()const {
            return T(get_virtual());
        }

        String tostr()const {
            size_t n = bit_length();
            String str;
            str.reserve(n + 1);
            if (!_Signal)str.push_back('-');
            for (auto _View = rbegin(); _View != rend(); ++_View) {
                str.push_back('0' + *_View);
            }
            return str;
        }

    private:

        template<size_t index>
        void Unsigned_Add(const virtual_biginteger<index>& rhs) {
            ubint.add(rhs.get_unsigned());
        }
        template<size_t index>
        void Unsigned_Sub(const virtual_biginteger<index>& rhs) {
            if (unsigned_traits::less_eq(ubint, rhs.get_unsigned())) {
                ubint.sub_by(rhs.get_unsigned());
                change_signal();
            }
            else {
                ubint.sub(rhs.get_unsigned());
            }
        }

        template<size_t index>
        void Signed_Add(const virtual_biginteger<index>& rhs) {
            if (signal() != rhs.signal()) {
                Unsigned_Sub(rhs);
            }
            else {
                Unsigned_Add(rhs);
            }
        }
        template<size_t index>
        void Signed_Sub(const virtual_biginteger<index>& rhs) {
            if (signal() != rhs.signal()) {
                Unsigned_Add(rhs);
            }
            else {
                Unsigned_Sub(rhs);
            }
        }
        template<size_t index>
        void Signed_Mul(const virtual_biginteger<index>& rhs) {
            ubint.mul(rhs.get_unsigned());
            if (!rhs.signal())change_signal();
        }

        template<size_t ir>
        static value_type one_divmod(
            biginteger& lhs, 
            const virtual_biginteger<ir>& rhs) {
            WASSERT_LEVEL_2(rhs.size() == 1); // unlikely
            size_t n = lhs.size();
            auto p = lhs.data();
            twice_value_type _Val = 0;
            value_type v = *rhs.data();
            WASSERT_LEVEL_1(v != 0);
            libdivide::divider<twice_value_type> _Divider(v);
            for (size_t i = n; i--;) {
                _Val = _Val * traits::max_value + p[i];
                p[i] = _Val / _Divider;
                _Val = _Val - p[i] * v;
            }
            while (n != 1 && p[n - 1] == 0)--n;
            lhs.ubint.vec.resize(n);
            return static_cast<value_type>(_Val);
        }

        template<size_t il,size_t ir>
        static value_type one_divmod(
            const virtual_biginteger<il>& lhs,
            const virtual_biginteger<ir>& rhs) {
            WASSERT_LEVEL_2(rhs.size() == 1);
            size_t n = lhs.size();
            auto p = lhs.data();
            twice_value_type _Val = 0;
            value_type v = *rhs.data();
            WASSERT_LEVEL_1(v != 0);
            libdivide::divider<twice_value_type> _Divider(v);
            for (size_t i = n; i--;) {
                _Val = _Val * traits::max_value + p[i];
                _Val = _Val - (_Val / _Divider) * v;
            }
            return static_cast<value_type>(_Val);
        }

        static void knuth_divmod(biginteger& lhs, biginteger& rhs) {
            biginteger result;
            auto max_value = traits::max_value;
            auto _max = traits::_max;
            auto mid_max_value = max_value >> 1;

            value_type vr = rhs.data()[rhs.size() - 1];
            auto kth =  static_cast<value_type>((mid_max_value + vr - 1) / vr);
            if (kth != 1) {
                lhs *= kth;
                rhs *= kth;
            }

            auto pl = lhs.data();
            auto pr = rhs.data();
            size_t n = rhs.size();
            size_t m = lhs.size() - rhs.size();
            virtual_biginteger<0> _View(lhs.signal(), pl + m,n);
            biginteger r(_View);

            auto val = pr[n - 1];
			libdivide::divider<value_type> _Divider1(val);
            libdivide::divider<twice_value_type> _Divider2(val);
            result.ubint.vec.resize(m + 1);
            auto presult = result.data();

            for (size_t j = m + 1; j--;) {
                value_type q = 0;
                if (r.size() == n + 1) {
                    q = std::min(traits::_max,
                        static_cast<value_type>((max_value * r.data()[n] + r.data()[n - 1]) / _Divider2));
                }
                else if (r.size() == n) {
                    q = std::min(traits::_max, r.data()[n - 1] / _Divider1);
                }
                else q = 0;
                r -= rhs * q;
                while (!r.signal()) {
                    --q;
                    r += rhs;
                }
                if (j != 0) {
                    r <<= traits::bit_length;
                    r.data()[0] = pl[j - 1];
                }
                presult[j] = q;
            }

            size_t size = result.size();
            while (size != 1 && !presult[size - 1])--size;
            result.ubint.vec.resize(size);
            lhs = std::move(result);
            rhs = std::move(r);
            if (kth != 1) {
                one_divmod(rhs, get_virtual_biginteger<base>(kth));
            }
        }

        static void small_divmod(biginteger& lhs, biginteger& rhs) {
            size_t n = lhs.size();
            size_t m = rhs.size();
            size_t mid = (n - m) >> 1;
            biginteger copyA(virtual_biginteger<0>(true, lhs.data() + mid, n - mid));
            biginteger mo(rhs);
            quick_divmod(copyA, mo);
            copyA <<= (mid * traits::bit_length);
            mo <<= (mid * traits::bit_length);
            if (mo.size() < mid)mo.ubint.vec.resize(mid);
            memcpy(mo.data(), lhs.data(), sizeof(value_type) * mid);
            lhs = std::move(copyA);
            quick_divmod(mo, rhs);
            memcpy(lhs.data(), mo.data(), sizeof(value_type) * mo.size());
        }

        static void mid_divmod(biginteger& lhs, biginteger& rhs) {
            WASSERT_LEVEL_2(lhs.size() == 2 * rhs.size() - 2);
            size_t n = lhs.size();
            size_t m = rhs.size();
            size_t mid = (n - m) >> 1;
            size_t k = (n - 2 * mid) + 1;

            biginteger E;
            // initialize E as base ^ (k * bit_length)
            E.ubint.vec.resize(k + 1, 0);
            E.ubint.vec[k] = 1;

            biginteger d(virtual_biginteger<0>(true, rhs.data() + mid, m - mid));
            quick_divmod(E, d);

            biginteger ans1(virtual_biginteger<0>(
                true, lhs.data() + 2 * mid, n - (2 * mid)));
            ++ans1;

            ans1 *= E;
            ans1 >>= (k * traits::bit_length);

            biginteger mo1(virtual_biginteger<0>(true, lhs.data() + mid, n - mid));
            mo1 -= ans1 * rhs;

#if WDEBUG_LEVEL >= 1
            size_t step = 0;
#endif
            while (!mo1.signal()) {
                --ans1;
                mo1 += rhs;
                WASSERT_LEVEL_1((++step) <= 2);
            }
#if WDEBUG_LEVEL >= 1
            step = 0;
#endif
            while (mo1 >= rhs) {
                ++ans1;
                mo1 -= rhs;
                WASSERT_LEVEL_1((++step) <= 2);
            }

            biginteger ans2(mo1);
            ++ans2;
            ans2 *= E;
            ans2 >>= (k * traits::bit_length);
            mo1 <<= (mid * traits::bit_length);
            mo1 += virtual_biginteger<0>(true, lhs.data(), mid).maintain();

            biginteger mo2(mo1 - rhs * ans2);
#if WDEBUG_LEVEL >= 1
            step = 0;
#endif
            while (!mo2.signal()) {
                --ans2;
                mo2 += rhs;
                WASSERT_LEVEL_1((++step) <= 2);
            }
#if WDEBUG_LEVEL >= 1
            step = 0;
#endif
            while (mo2 >= rhs) {
                ++ans2;
                mo2 -= rhs;
                WASSERT_LEVEL_1((++step) <= 2);
            }

            ans1 <<= (mid * traits::bit_length);
            ans1 += ans2;

            lhs = std::move(ans1);
            rhs = std::move(mo2);
        }

        static void large_divmod(biginteger& lhs, biginteger& rhs) {
            size_t n = lhs.size();
            size_t m = rhs.size();
            size_t mid = 2 * m - n - 2;
            biginteger copyA(virtual_biginteger<0>(true, lhs.data() + mid, n - mid));
            biginteger copyB(virtual_biginteger<0>(true, rhs.data() + mid, m - mid));
            ++copyA;
            quick_divmod(copyA, copyB);
            --copyB;
            if (!copyB.signal()) {
                copyB <<= (mid * traits::bit_length);
                copyB += lhs;
            }
            else {
                if (copyB.zero()) {
                    copyB = std::move(lhs);
                    copyB.ubint.vec.resize(mid);
                    size_t pos = copyB.size() - 1;
                    auto p = copyB.data();
                    while (pos != 1 && !p[pos - 1])--pos;
                    copyB.ubint.vec.resize(pos);
                }
                else {
                    copyB <<= (mid * traits::bit_length);
                    memcpy(copyB.data(), lhs.data(), sizeof(value_type) * mid);
                }
            }

            copyB -= virtual_biginteger<0>(true, rhs.data(), mid) * copyA;
            if (!copyB.signal()) {
                --copyA;
                copyB += rhs;
            }

            lhs = std::move(copyA);
            rhs = std::move(copyB);
        }

        static void quick_divmod(biginteger& lhs, biginteger& rhs) {
            WASSERT_LEVEL_1(!rhs.zero() && lhs.signal() && rhs.signal());
            if (unsigned_biginteger<base>::less(lhs.ubint, rhs.ubint)) {
                rhs = std::move(lhs);
                lhs = biginteger();
                return;
            }
            size_t n = lhs.size();
            size_t m = rhs.size();
#if WDEBUG_LEVEL >= 3
            biginteger p(lhs), q(rhs);
#endif
            if (m == 1) {
                rhs = one_divmod(lhs, get_virtual_biginteger<base>(rhs));
            }else if (m <= ((quick_log2(n) + 4) << 1)) {
                knuth_divmod(lhs, rhs);
            }else if (n == 2 * m - 2) {
                mid_divmod(lhs, rhs);
            }else if (n * 3 <= m * 5) {
                large_divmod(lhs, rhs);
            }
            else {
                small_divmod(lhs, rhs);
            }
            WASSERT_LEVEL_3(q * lhs + rhs == p);
        }

    public:

        template<typename T, std::enable_if_t<_Is_virtual_biginteger_v<T>, int> = 0>
        biginteger& operator+=(const T& rhs) {
            Signed_Add(get_virtual_biginteger<base>(rhs));
            return *this;
        }

        template<typename T, std::enable_if_t<_Is_virtual_biginteger_v<T>, int> = 0>
        biginteger& operator-=(const T& rhs) {
            Signed_Sub(get_virtual_biginteger<base>(rhs));
            return *this;
        }

        template<typename T, std::enable_if_t<_Is_virtual_biginteger_v<T>, int> = 0>
        biginteger& operator*=(const T& rhs) {
            Signed_Mul(get_virtual_biginteger<base>(rhs));
            return *this;
        }

        template<typename T,std::enable_if_t<_Is_virtual_biginteger_v<T>,int> = 0>
        biginteger& operator/=(T&& rhs) {
            auto _Rhs = get_virtual_biginteger<base>(std::forward<T>(rhs));
            bool f = signal() ^ (!_Rhs.signal());
            abs();
            _Rhs.abs();
            if constexpr (std::is_same_v<decltype(_Rhs), virtual_biginteger<1>>) {
                one_divmod(*this, _Rhs);
            }
            else {
                if constexpr (_Is_rbint_v<T>) {
                    biginteger R(std::forward<T>(rhs));
                    R.abs();
                    quick_divmod(*this, R);
                }
                else {
                    biginteger R(_Rhs);
                    R.abs();
                    quick_divmod(*this, R);
                }
            }
            this->_Signal = f;
            maintain_signal();
            return *this;
        }

        template<typename T, std::enable_if_t<_Is_virtual_biginteger_v<T>, int> = 0>
        biginteger& operator%=(T&& rhs) {
            auto _Rhs = get_virtual_biginteger<base>(std::forward<T>(rhs));
            bool f = signal();
            abs();
            _Rhs.abs();
            if constexpr (std::is_same_v<decltype(_Rhs), virtual_biginteger<1>>) {
                *this = one_divmod(*this, _Rhs);
            }
            else {
                if constexpr (_Is_rbint_v<T>) {
                    biginteger R(std::forward<T>(rhs));
                    R.abs();
                    quick_divmod(*this, R);
                    *this = std::move(R);
                }
                else {
                    biginteger R(_Rhs);
                    R.abs();
                    quick_divmod(*this, R);
                    *this = std::move(R);
                }
            }
            this->_Signal = f;
            maintain_signal();
            return *this;
        }

        biginteger& operator++() {
            (*this) += virtual_biginteger<1>((uint32_t)1);
            return *this;
        }

        biginteger operator++(int) {
            biginteger it(*this);
            ++(*this);
            return it;
        }

        biginteger& operator--() {
            (*this) -= virtual_biginteger<1>((uint32_t)1);
            return *this;
        }

        biginteger operator--(int) {
            biginteger it(*this);
            --(*this);
            return it;
        }

        biginteger& operator<<=(size_t index) {
            // quick mul base ^ index
            ubint.quick_mul_base_power(index);
            return *this;
        }

        biginteger& operator>>=(size_t index) {
            // quick divide base ^ index
            ubint.quick_divide_base_power(index);
            maintain_signal();
            return *this;
        }

        template<typename T,std::enable_if_t<_Is_virtual_biginteger_v<T>,int> = 0>
        friend biginteger operator>>(T&& lhs, size_t index) {
            biginteger result(std::forward<T>(lhs));
            result >>= index;
            return result;
        }

        template<typename T, std::enable_if_t<_Is_virtual_biginteger_v<T>, int> = 0>
        friend biginteger operator<<(T&& lhs, size_t index) {
            biginteger result(std::forward<T>(lhs));
            result <<= index;
            return result;
        }

        void maintain() {
            ubint.maintain();
            maintain_signal();
        }

    private:

        std::vector<value_type, mallocator<value_type>>& 
            get_vec() {
            return ubint.vec;
        }

        const std::vector<value_type, mallocator<value_type>>&
            get_vec()const {
            return ubint.vec;
        }

        template<size_t il,size_t ir>
        static bool equal(
            const virtual_biginteger<il>& lhs,
            const virtual_biginteger<ir>& rhs) {
            return lhs.signal() == rhs.signal()
                && unsigned_traits::equal(lhs.get_unsigned(), rhs.get_unsigned());
        }
        template<size_t il,size_t ir>
        static bool less(
            const virtual_biginteger<il>& lhs,
            const virtual_biginteger<ir>& rhs) {
            return lhs.signal() != rhs.signal() ?
                lhs.signal() < rhs.signal() :
                (unsigned_traits::less(lhs.get_unsigned(), rhs.get_unsigned()) ^ (!lhs.signal()));
        }
        template<size_t il,size_t ir>
        static bool less_eq(
            const virtual_biginteger<il>& lhs,
            const virtual_biginteger<ir>& rhs) {
            return lhs.signal() != rhs.signal() ?
                lhs.signal() < rhs.signal() :
                (unsigned_traits::less_eq(lhs.get_unsigned(), rhs.get_unsigned()) ^ (!lhs.signal()));
        }

        template<size_t toBase>
        static biginteger<toBase> dc_base_conversion_helper(
            const virtual_unsigned_biginteger<base,0>& v,
            std::vector<std::pair<size_t,biginteger<toBase>>>&cache) {
            size_t n = v.size();
            if (n <= 2) {
                return biginteger<toBase>((twice_value_type)(v));
            }
            size_t mid = n >> 1;  
            virtual_unsigned_biginteger<base, 0> lhs(v.data() + mid, n - mid);
            virtual_unsigned_biginteger<base, 0> rhs(v.data(), mid);
            rhs.maintain();
            auto _Lhs = dc_base_conversion_helper<toBase>(lhs,cache);
            auto _Rhs = dc_base_conversion_helper<toBase>(rhs,cache);
            auto iter =
                std::lower_bound(cache.begin(), cache.end(), mid,
                    [](const auto& x, size_t c) {return x.first < c; });
            
            if (iter != cache.end() && iter->first == mid) {
                _Lhs *= iter->second;
            }
            else {
                auto iter_mid = std::lower_bound(cache.begin(), cache.end(), mid / 2,
                    [](const auto& x, size_t c) {return x.first < c; });
                biginteger<toBase> _Mid = iter_mid->second * iter_mid->second;
                if (mid & 1)_Mid *= cache[0].second;
                _Lhs *= _Mid;
                cache.emplace(iter, mid, std::move(_Mid));
            }
            return _Lhs += _Rhs;
        }

        template<size_t toBase, size_t index>
        static biginteger<toBase> base_conversion_helper(
            const virtual_biginteger<index>& v) {
            if constexpr (toBase == base) {
                return biginteger<toBase>(v);
            }
            else {
                std::vector<std::pair<size_t, biginteger<toBase>>> cache;
                cache.reserve(30);
                cache.emplace_back(1, biginteger<toBase>(traits::max_value));
                cache.emplace_back(2, cache[0].second * cache[0].second);
                biginteger<toBase> result = dc_base_conversion_helper<toBase>(v.get_unsigned(), cache);
                if (!v.signal())result.change_signal();
                return result;
            }
        }

        VIRTUAL_BIGINTEGER_DEFINITION
        friend bool operator==(const T&,const U&);
        VIRTUAL_BIGINTEGER_DEFINITION
        friend bool operator!=(const T&,const U&);
        VIRTUAL_BIGINTEGER_DEFINITION
        friend bool operator<(const T&,const U&);
        VIRTUAL_BIGINTEGER_DEFINITION
        friend bool operator<=(const T&,const U&);
        VIRTUAL_BIGINTEGER_DEFINITION
        friend bool operator>(const T&,const U&);
        VIRTUAL_BIGINTEGER_DEFINITION
        friend bool operator>=(const T&,const U&);

        VIRTUAL_BIGINTEGER_DEFINITION
        friend biginteger<_Base> operator+(T&&,U&&);
        VIRTUAL_BIGINTEGER_DEFINITION
        friend biginteger<_Base> operator-(T&&,U&&);
        VIRTUAL_BIGINTEGER_DEFINITION
        friend biginteger<_Base> operator*(T&&,U&&);
        VIRTUAL_BIGINTEGER_DEFINITION
        friend biginteger<_Base> operator/(T&&,U&&);
        VIRTUAL_BIGINTEGER_DEFINITION
        friend biginteger<_Base> operator%(T&&,U&&);
        template<size_t _Base>
        friend void divmod(biginteger<_Base>& lhs, biginteger<_Base>& rhs);
        template<size_t toBase, typename T, std::enable_if_t<_Is_biginteger_v<T>, int> >
        friend biginteger<toBase> base_conversion(const T&);
        template<size_t _Base>
        friend void random_biginteger(biginteger<_Base>& result, size_t n);

        bool _Signal = true;
        unsigned_biginteger<base> ubint;
    };

    VIRTUAL_BIGINTEGER_DEFINITION
        bool operator==(const T& lhs, const U& rhs) {
        return biginteger<_Base>::equal(
            get_virtual_biginteger<_Base>(lhs),
            get_virtual_biginteger<_Base>(rhs)
        );
    }
    VIRTUAL_BIGINTEGER_DEFINITION
        bool operator!=(const T& lhs, const U& rhs) {
        return !(lhs == rhs);
    }
    VIRTUAL_BIGINTEGER_DEFINITION
        bool operator<(const T& lhs, const U& rhs) {
        return biginteger<_Base>::less(
            get_virtual_biginteger<_Base>(lhs),
            get_virtual_biginteger<_Base>(rhs)
        );
    }
    VIRTUAL_BIGINTEGER_DEFINITION
        bool operator<=(const T& lhs, const U& rhs) {
        return biginteger<_Base>::less_eq(
            get_virtual_biginteger<_Base>(lhs),
            get_virtual_biginteger<_Base>(rhs)
        );
    }
    VIRTUAL_BIGINTEGER_DEFINITION
        bool operator>(const T& lhs, const U& rhs) {
        return rhs < lhs;
    }
    VIRTUAL_BIGINTEGER_DEFINITION
        bool operator>=(const T& lhs, const U& rhs) {
        return rhs <= lhs;
    }

    VIRTUAL_BIGINTEGER_DEFINITION
        biginteger<_Base> operator+(T&& lhs, U&& rhs) {
        if constexpr (biginteger<_Base>::template _Is_rbint_v<T>) {
            lhs += std::forward<U>(rhs);
            return std::forward<T>(lhs);
        }
        else if constexpr (biginteger<_Base>::template _Is_rbint_v<U>) {
            rhs += std::forward<T>(lhs);
            return std::forward<U>(rhs);
        }
        else {
            biginteger<_Base> result(std::forward<T>(lhs));
            result += std::forward<U>(rhs);
            return result;
        }
    }

    VIRTUAL_BIGINTEGER_DEFINITION
        biginteger<_Base> operator-(T&& lhs, U&& rhs) {
        if constexpr (biginteger<_Base>::template _Is_rbint_v<T>) {
            lhs -= std::forward<U>(rhs);
            return std::forward<T>(lhs);
        }
        else if constexpr (biginteger<_Base>::template _Is_rbint_v<U>) {
            rhs -= std::forward<T>(lhs);
            rhs.change_signal();
            return std::forward<U>(rhs);
        }
        else {
            biginteger<_Base> result(std::forward<T>(lhs));
            result -= std::forward<U>(rhs);
            return result;
        }
    }

    VIRTUAL_BIGINTEGER_DEFINITION
        biginteger<_Base> operator*(T&& lhs, U&& rhs) {
        if constexpr (biginteger<_Base>::template _Is_rbint_v<T>) {
            lhs *= std::forward<U>(rhs);
            return std::forward<T>(lhs);
        }
        else if constexpr (biginteger<_Base>::template _Is_rbint_v<U>) {
            rhs *= std::forward<T>(lhs);
            return std::forward<U>(rhs);
        }
        else {
            biginteger<_Base> result(std::forward<T>(lhs));
            result *= std::forward<U>(rhs);
            return result;
        }
    }

    VIRTUAL_BIGINTEGER_DEFINITION
        biginteger<_Base> operator/(T&& lhs, U&& rhs) {
        biginteger<_Base> result(std::forward<T>(lhs));
        result /= std::forward<U>(rhs);
        return result;
    }

    VIRTUAL_BIGINTEGER_DEFINITION
        biginteger<_Base> operator%(T&& lhs, U&& rhs) {
        biginteger<_Base> result(std::forward<T>(lhs));
        result %= std::forward<U>(rhs);
        return result;
    }

    template<size_t _Base>
    void divmod(biginteger<_Base>& lhs, biginteger<_Base>& rhs) {
        biginteger<_Base>::quick_divmod(lhs, rhs);
    }

    template<size_t toBase, typename T, std::enable_if_t<_Is_biginteger_v<T>, int>>
    biginteger<toBase> base_conversion(const T& v) {
        constexpr static size_t fromBase = biginteger_base_v<T>;
        auto result = biginteger<fromBase>::
            template base_conversion_helper<toBase>(get_virtual_biginteger<fromBase>(v));
#if WDEBUG_LEVEL >= 3
        auto iresult = biginteger<toBase>::
            template base_conversion_helper<fromBase>(result.get_virtual());
        assert(iresult == v);
#endif
        return result;
    }

    template<size_t _Base>
    void random_biginteger(biginteger<_Base>&result, size_t n) {
        using traits = biginteger_traits<_Base>;
        USE_THREAD_LOCAL static std::random_device rd;
        USE_THREAD_LOCAL static std::mt19937 mt_rand(rd());
        size_t k = (n - 1) / traits::bit_length;
        size_t r = (n - 1) % traits::bit_length + 1;
        size_t index = k + (r != traits::bit_length);
        std::uniform_int_distribution<typename traits::type> rc(0, traits::_max);
        result.get_vec().resize(index);
        for (size_t i = 0; i < index; ++i) {
            result.get_vec()[i] = rc(mt_rand);
        }
        result.maintain();
    }

    template<size_t _Base>
    biginteger<_Base> random_biginteger(size_t n) {
        biginteger<_Base> result;
        random_biginteger(result, n);
        return result;
    }


#undef GENERATE_BIGINTEGER_OPERATOR
#undef VIRTUAL_BIGINTEGER_DEFINITION
#undef VIRTUAL_BIGINTEGER_DECLARATION
#undef REGISTER_VIRTUAL_BIGINTEGER_FUNCTION

}

#endif
