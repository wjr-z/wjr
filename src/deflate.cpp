#include "../include/deflate.h"
namespace wjr {

//#define USE_STRING_HASH

#define ZIP_HEADER_SIZE (10)
#define NHEADER_SIZE     (1)
#define HEADER_SIZE     (17)
    constexpr static size_t ZIP_MIN_LENGTH = 1 KB;
    constexpr static size_t BUFFER_SIZE =  256 KB;
    constexpr static size_t WINDOW_SIZE =  32  KB;
    constexpr static uint32_t WINDOW_SIZE_BIT = quick_log2(WINDOW_SIZE);
    constexpr static uint32_t ANTI_WINDOW_SIZE_BIT = 32 - WINDOW_SIZE_BIT;
    constexpr static uint32_t MAX_OFFSET_CODE = 2 * WINDOW_SIZE_BIT;
#ifdef USE_STRING_HASH
    constexpr static uint32_t hash_base = 17;
#endif
    constexpr static uint32_t MIN_LENGTH = 3;
    constexpr static uint32_t MAX_LENGTH = 256 + MIN_LENGTH - 1;
    static_assert(!(WINDOW_SIZE & (WINDOW_SIZE - 1)),"WINDOW_SIZE must be power of two");
    using OFFSET_TYPE = std::conditional_t<WINDOW_SIZE_BIT < 16,uint16_t,uint32_t>;

    struct DEFLATE_CONFIG {
        int lazy_match_max_length;
        int lazy_match_max_step;
        int max_chain;
    };

    constexpr static DEFLATE_CONFIG deflate_config[10] = {
        {0,0,0},
        {0,0,4},
        {0,0,8},
        {0,0,16},
        {8,16,32},
        {32,16,64},
        {32,64,96},
        {128,64,128},
        {128,256,192},
        {256,256,256}
    };

    constexpr uint32_t fibonacci = 2654435769;
    uint32_t fib_hash(uint32_t x){
    #if WINDOW_SIZE_BIT < 24
        return (x * fibonacci) >> ANTI_WINDOW_SIZE_BIT; 
    #else
        return x;
    #endif
    }
    uint32_t get_hash(const uint8_t* ptr) {
    #if is_little_endian
        return fib_hash(*(uint32_t*)ptr & 0x00FFFFFF);
    #else
        return fib_hash(*(uint32_t*)ptr & 0xFFFFFF00);
    #endif
    }

    USE_THREAD_LOCAL static OFFSET_TYPE head[WINDOW_SIZE],
                                        nxt[WINDOW_SIZE|1];

    USE_THREAD_LOCAL static size_t      head_ptr = 0,tail_ptr = 0;
    USE_THREAD_LOCAL static size_t      key[WINDOW_SIZE|1];

    void pop_list(const uint8_t* ptr, size_t pos) {
        uint32_t hs = get_hash(ptr + pos);
        OFFSET_TYPE x = ++head_ptr;
        head_ptr &= WINDOW_SIZE - 1;
        if (head[hs] == x) {
            head[hs] = 0;
        }
    }

    void deflate_push(const uint8_t* ptr, size_t pos) {
        ptr += pos;
        if (pos >= WINDOW_SIZE) {
            uint32_t hs = get_hash(ptr - WINDOW_SIZE);
            OFFSET_TYPE x = ++head_ptr;
            head_ptr &= WINDOW_SIZE - 1;
            tail_ptr = head_ptr;
            if (head[hs] == x) {
                head[hs] = 0;
            }
            hs = get_hash(ptr);
            key[x] = pos;
            nxt[x] = head[hs];
            head[hs] = x;
        }
        else {
            uint32_t hs = get_hash(ptr);
            OFFSET_TYPE x = ++tail_ptr;
            tail_ptr &= WINDOW_SIZE - 1;
            key[x] = pos;
            nxt[x] = head[hs];
            head[hs] = x;
        }
    }

    USE_THREAD_LOCAL static uint8_t LIT_1_BUFFER[(BUFFER_SIZE + 1) * 2],
                                    LIT_2_BUFFER[BUFFER_SIZE / 72 + 16],
                                    DIST_1_BUFFER[BUFFER_SIZE * 8 / 24 + 16],
                                    DIST_2_BUFFER[BUFFER_SIZE * 13 / 24 + 16];
#ifdef USE_STRING_HASH

    constexpr static uint32_t hash_bp[MAX_LENGTH + 1] = {
        0x00000001,0x00000011,0x00000121,0x00001331,0x00014641,0x0015aa51,0x01704f61,0x18754571,
        0x9fc99c81,0x9c636491,0x6299ada1,0x8c3487b1,0x4f7d02c1,0x474d2ed1,0xbc201be1,0x7e21d9f1,
        0x603f7901,0x64370911,0xa7a79a21,0x22213c31,0x4434ff41,0x8784f351,0xffd42861,0xfd16ae71,
        0xce819581,0xb69aed91,0x2049c6a1,0x24e630b1,0x73493bc1,0xa7dcf7d1,0x25ac74e1,0x8073c2f1,
        0x87aff201,0x02af1211,0x2da03321,0x07a36531,0x81d9b841,0x9f753c51,0x96c90161,0x03591771,
        0x38ea8e81,0xc7937691,0x40cadfa1,0x4d78d9b1,0x250674c1,0x756dc0d1,0xcc49cde1,0x90e6abf1,
        0x9f516b01,0x94681b11,0xdae9cc21,0x89868e31,0x21ef7141,0x40e68551,0x4f4eda61,0x443c8071,
        0x88048781,0x084cff91,0x8d1cf8a1,0x5eec82b1,0x4db4adc1,0x28ff89d1,0xb8f826e1,0x487a94f1,
        0xd023e401,0xd2622411,0xf8846521,0x80cab731,0x8d762a41,0x64d8ce51,0xb265b361,0xd8c0e971,
        0x64cf8081,0xb1c78891,0xce4011a1,0xb2412bb1,0xd653e6c1,0x3b9252d1,0xf4b77fe1,0x402f7df1,
        0x43275d01,0x759d2d11,0xcf6ffe21,0xc66fe031,0x2d6de341,0x044c1751,0x490d8c61,0xd9e65271,
        0x784b7981,0xfd031191,0xcd342aa1,0xa076d4b1,0xa7e41fc1,0x26261bd1,0x8887d8e1,0x110566f1,
        0x215bd601,0x37193611,0xa8ac9721,0x33760931,0x6ad69c41,0x18406051,0x9c466561,0x60acbb71,
        0x6b787281,0x22ff9a91,0x52f943a1,0x828d7db1,0xab6558c1,0x61bae4d1,0x7d6931e1,0x53fc4ff1,
        0x93c14f01,0xcfd63f11,0xcd3a3021,0xa0dd3231,0xaeb05541,0x99b5a951,0x35103e61,0x86142471,
        0xe7566b81,0x5cbd2391,0x288f5ca1,0xb18526b1,0xc9d791c1,0x6750add1,0xdc5b8ae1,0xa21438f1,
        0xc357c801,0xf8d44811,0x8618c921,0xe7a55b31,0x61fb0e41,0x81abf251,0x9c6b1761,0x631c8d71,
        0x94e56481,0xe33bac91,0x16f675a1,0x865dcfb1,0xec3acac1,0xafe776d1,0xae5ee3e1,0x944d21f1,
        0xd91f4101,0x6b135111,0x1c486221,0xe0ce8431,0xedb6c741,0xc9233b51,0x5b56f061,0x10c5f671,
        0x1d255d81,0xef7b3591,0xe72e8ea1,0x5a1778b1,0xfb8f03c1,0xb47f3fd1,0xfc733ce1,0xc3a70af1,
        0xfe17ba01,0xdf935a11,0xd8c8fb21,0x6558ad31,0xbae38041,0x691b8451,0xfad3c961,0xa8105f71,
        0x29165681,0xba7bbe91,0x6237a7a1,0x85b221b1,0xe0d43cc1,0xee1808d1,0xcf9895e1,0xc921f3f1,
        0x5b413301,0x0f546311,0x049a9421,0x4e43d631,0x32813941,0x5a94cd51,0x03e1a261,0x41fbc871,
        0x61b84f81,0x7d3d4791,0x5111c0a1,0x622dcab1,0x850a75c1,0xd5b1d1d1,0x30ceeee1,0x3dbddcf1,
        0x199bac01,0xb3566c11,0xe8bd2d21,0x748fff31,0xbd8ff241,0x968f1651,0xff807b61,0xf7883171,
        0x700b4881,0x70bfd091,0x7cbcd9a1,0x488a73b1,0xd131aec1,0xe44c9ad1,0x291647e1,0xba7ac5f1,
        0x62272501,0x84997511,0xce30c621,0xb13d2831,0xc50fab41,0x160a5f51,0x76b05461,0xe1b59a71,
        0xfd0f4181,0xce035991,0xae38f2a1,0x91c81cb1,0xae49e7c1,0x92e863d1,0xc16ea0e1,0xd858aef1,
        0x5de39e01,0x3c1d7e11,0xfdf55f21,0xdd4b5131,0xb2006441,0xd206a851,0xf2712d61,0x19840371,
        0xb1c43a81,0xce07e291,0xae860ba1,0x96e6c5b1,0x055320c1,0x5a852cd1,0x02d7f9e1,0x305797f1,
        0x35d11701,0x92e28711,0xc10af821,0xd1ba7a31,0xed621d41,0xc383f151,0xfbc30661,0xb7f36c71,
        0x372a3381,0xa9cd6b91,0x46a424a1,0xb0e66eb1,0xbf4d59c1,0xb422f5d1,0xf65252e1,0x5b7780f1,
        0x12ef9001,0x41e89011,0x60719121
    };

    constexpr static uint32_t HASH_BUFFER_SIZE = 1u << (quick_log2(BUFFER_SIZE + WINDOW_SIZE  + 1 - 1) + 1);
    USE_THREAD_LOCAL static uint32_t HASH_BUFFER[HASH_BUFFER_SIZE];
    USE_THREAD_LOCAL static uint32_t HASH_BUFFER_PTR;
    uint32_t get_string_hash_back() {
        return HASH_BUFFER[HASH_BUFFER_PTR];
    }
    uint32_t get_string_hash_val(uint32_t delta) {
        return delta <= HASH_BUFFER_PTR ? HASH_BUFFER[HASH_BUFFER_PTR - delta]
            : HASH_BUFFER[HASH_BUFFER_SIZE - (delta - HASH_BUFFER_PTR)];
    }
    void append_string_hash(uint32_t val) {
        HASH_BUFFER_PTR = (HASH_BUFFER_PTR + 1) % HASH_BUFFER_SIZE;
        HASH_BUFFER[HASH_BUFFER_PTR] = val;
    }
    uint32_t get_string_hash(uint32_t delta1, uint32_t delta2) {
        return get_string_hash_val(delta1) - get_string_hash_val(delta2) * hash_bp[delta2 - delta1];
    }

#endif

    static uint32_t deflate_log[256] = {
        0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,
        4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
        5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
    };

    class deflate_lit_reader {
    public:
        constexpr static uint32_t max_number = 286;
        using number_type = uint16_t;
        deflate_lit_reader(const void* p, const void* e)
            : ptr((uint8_t*)p) {

        }
        uint16_t read() {
            uint16_t ch = *(ptr++) << 8;
            ch |= *(ptr++);
            return ch;
        }
        bool write(uint16_t ch) {
            *(ptr++) = (uint8_t)(ch >> 8);
            *(ptr++) = (uint8_t)(ch);
            return true;
        }
        void* get_ptr()const {
            return ptr;
        }
    private:
        uint8_t* ptr;
    };

    class deflate_dist_reader 
        : public default_huffman_reader {
    public:
        constexpr static int max_number = MAX_OFFSET_CODE;
        using base = default_huffman_reader;
        deflate_dist_reader(const void* p, const void* e)
            : base(p,e) {

        }
    };

    struct read_buffer {
        explicit read_buffer(void* p)
            : ptr((uint8_t*)p), bit(8) {

        }
        uint16_t read(int l) {
            if (l < bit) {
                bit -= l;
                return (*(ptr) >> bit) & binary_mask[l];
            }
            l -= bit;
            uint16_t ch = (*(ptr++) & binary_mask[bit]) << l;
            if (l >= 8) {
                l -= 8;
                ch |= *(ptr++) << l;
            }
            bit = 8 - l;
            ch |= (*(ptr) >> bit) & binary_mask[l];
            return ch;
        }

        uint8_t* ptr;
        int bit;
    };

    struct write_buffer {
        explicit write_buffer(void* p)
            : ptr((uint8_t*)p), bit(8) {
            *ptr = 0;
        }
        void write(uint16_t ch, int l) {
            if (l < bit) {
                bit -= l;
                *ptr |= (uint8_t)(ch << bit);
            }
            else {
                l -= bit;
                *(ptr++) |= (uint8_t)(ch >> l);
                if (l >= 8) {
                    l -= 8;
                    *(ptr++) = (uint8_t)(ch >> l);
                }
                bit = 8 - l;
                *ptr = (ch & binary_mask[l]) << bit;
            }
        }
        void flush() {
            if (bit != 8) {
                ++ptr;
            }
        }
        void* get_ptr()const {
            return (void*)ptr;
        }
        uint8_t* ptr;
        int bit;
    };

    uint32_t longest_match(const uint8_t* ptr, size_t pos, 
        size_t length, OFFSET_TYPE& offset,int level,uint32_t min_limit = 2) {
        if(pos + min_limit >= length) return 0;

        auto max_length = MAX_LENGTH;
        if (unlikely(max_length > length - pos)) {
            max_length = length - pos;
        }
        const uint8_t* buffer = ptr + pos ;
        const uint8_t* buffer_limit = buffer + max_length;

        uint32_t hs = get_hash(buffer);
        uint32_t longest = min_limit;
        uint32_t max_chain = deflate_config[level].max_chain;
    #ifdef USE_STRING_HASH
        uint32_t longest_string_hash = get_string_hash(
            length - 1 - (pos + longest),
            length - 1 - (pos - 1)
        );
    #endif
        if(head[hs]){
            for (OFFSET_TYPE i = head[hs]; max_chain; --max_chain) {
                const uint8_t* buffer_ptr = buffer;
                const uint8_t* window_ptr = ptr + key[i];
                if (buffer_ptr[longest] == window_ptr[longest]) {
                #ifdef USE_STRING_HASH
                    uint32_t window_string_hash = get_string_hash(
                        length - 1 - (key[i] + longest),
                        length - 1 - (key[i] - 1)
                    );
                    if (longest_string_hash != window_string_hash) {
                        continue;
                    }
                #endif
                    while (buffer_ptr + 8 <= buffer_limit && *(uint64_t*)buffer_ptr == *(uint64_t*)window_ptr) {
                        buffer_ptr += 8;
                        window_ptr += 8;
                    }

                    if (buffer_ptr + 4 <= buffer_limit && *(uint32_t*)buffer_ptr == *(uint32_t*)window_ptr) {
                        buffer_ptr += 4;
                        window_ptr += 4;
                    }

                    if (buffer_ptr + 2 <= buffer_limit && *(uint16_t*)buffer_ptr == *(uint16_t*)window_ptr) {
                        buffer_ptr += 2;
                        window_ptr += 2;
                    }

                    if (buffer_ptr != buffer_limit && *buffer_ptr == *window_ptr) {
                        ++buffer_ptr;
                        ++window_ptr;
                    }

                    if (buffer_ptr - buffer > longest) {
                        offset = pos - key[i] - 1;
                        longest = buffer_ptr - buffer;
                        if (longest == max_length) {
                            break;
                        }
                    #ifdef USE_STRING_HASH
                        longest_string_hash = get_string_hash(
                            length - 1 - (pos + longest),
                            length - 1 - (pos - 1)
                        );
                    #endif
                    }
                }
                auto j = nxt[i];
                if (key[j] > key[i]) {
                    break;
                }
                assert(j != 0);
                i = j;
            }
        }

        return longest != min_limit ? longest : 0;
    }

    bool deflate_compress_header(size_t l1, void*& dest, size_t&l2) {
        if (l2 < ZIP_HEADER_SIZE) {
            return false;
        }
        uint8_t* out = (uint8_t*)dest;
        *(out++) = 'P';
        *(out++) = 'K';
    #if is_little_endian
        * (uint64_t*)out = l1;
    #else
        * (uint64_t*)out = bswap_64(l1);
    #endif
        out += 8;
        dest = (void*)(out);
        l2 -= ZIP_HEADER_SIZE;
    #ifdef USE_STRING_HASH
        HASH_BUFFER[0] = 0;
        HASH_BUFFER_PTR = 0;
    #endif
        key[0] = l1;
        return true;
    }

    size_t deflate_decompress_header(const void*& src, size_t&l1, void* dest, size_t l2) {
        if (l1 < ZIP_HEADER_SIZE) {
            return -1;
        }
        uint8_t* in = (uint8_t*)src;
        if (*in != 'P' || *(in + 1) != 'K') {
            return -1;
        }
        in += 2;
    #if is_little_endian
        uint64_t length = *(uint64_t*)in;
    #else
        uint64_t length = bswap_64(*(uint64_t*)in);
    #endif
        if (l2 < length) {
            return -1;
        }
        in += 8;
        src = (void*)(in);
        l1 -= ZIP_HEADER_SIZE;
        return length;
    }

    uint32_t deflate_not_compress_package(const void* src, size_t pos, 
        size_t end, void*& dest, size_t res, bool is_last) {
        uint32_t cl = end - pos;
        if (res < cl + NHEADER_SIZE) {
            return -1;
        }
        uint8_t*out = (uint8_t*)dest;
        *(out++) = (uint8_t)is_last;
        memcpy(out,(uint8_t*)src + pos,cl);
        out += cl;
        dest = (void*)out;
        return cl + NHEADER_SIZE;
    }

    uint32_t deflate_compress_package(const void* src, size_t pos,size_t end, 
        void*& dest,size_t res,bool is_last,int level) {
        uint32_t cl = end - pos;
        uint32_t cres = res;
        deflate_lit_reader LIT_1(LIT_1_BUFFER,nullptr);
        write_buffer LIT_2(LIT_2_BUFFER);
        uint8_t* DIST_1(DIST_1_BUFFER);
        write_buffer DIST_2(DIST_2_BUFFER);

        uint32_t sz1 = 0,sz2 = 0;

        uint8_t* ptr = (uint8_t*)src;
    #ifdef USE_STRING_HASH
        for (size_t i = pos; i < end; ++i) {
            uint32_t last_hash = get_string_hash_back();
            uint32_t val = last_hash * hash_base + ptr[i];
            append_string_hash(val);
        }
    #endif
        for (size_t i = pos, j; i < end;) {
            j = i;
            uint32_t match = 0;
            OFFSET_TYPE offset;
            while (j < end && !(match = longest_match(ptr, j, end, offset,level))) {
                deflate_push(ptr,j++);
            }

            sz1 += j - i;
            for (; i < j; ++i) {
                LIT_1.write(ptr[i]);
            }

            if(unlikely(!match))continue;

            int lazy_match = deflate_config[level].lazy_match_max_step;
            int lazy_match_limit = deflate_config[level].lazy_match_max_length;
            if (match != MAX_LENGTH) {
                for (; lazy_match-- && match <= lazy_match_limit && j + 1 < end;) {
                    deflate_push(ptr, j);
                    ++j;
                    OFFSET_TYPE offset2;
                    uint32_t match2 = longest_match(ptr, j, end, offset2,level,match);
                    if (match2 <= match) {
                        break;
                    }
                    match = match2;
                    offset = offset2;
                    LIT_1.write(ptr[i++]);
                    ++sz1;
                }
            }

            i += match;
            for (; j < i; ++j) {
                deflate_push(ptr,j);
            }

            ++sz2;
            match -= MIN_LENGTH;
            if (match < 8) {
                LIT_1.write(257 + match);
            }
            else {
                if (match == 255) {
                    LIT_1.write(285);
                }
                else {
                    auto LIT_bit = deflate_log[match];
                    auto LIT_code = 253 + ((LIT_bit << 2) | ((match >> (LIT_bit - 2)) & 3));
                    assert(LIT_code < 286);
                    LIT_1.write(LIT_code);
                    LIT_2.write(match & binary_mask[LIT_bit-2],LIT_bit-2);
                }
            }
            if (offset < 4) {
                *(DIST_1++) = offset;
            }
            else {
                auto DIST_bit = quick_log2(offset);
                auto DIST_code = (DIST_bit << 1) | ((offset >> (DIST_bit - 1)) & 1);
                *(DIST_1++) = DIST_code;
                DIST_2.write(offset & binary_mask[DIST_bit - 1],DIST_bit-1);
            }
        }

        if (is_last) {
            for (size_t i = end >= WINDOW_SIZE ? end - WINDOW_SIZE : 0; i < end; ++i) {
                pop_list(ptr, i);
            }
        }

        ++sz1;
        LIT_1.write(256);
        LIT_2.flush();
        DIST_2.flush();

        uint32_t TOT_LENGTH = 0;
        uint8_t* out = (uint8_t*)dest;
        if (res < HEADER_SIZE) { // don't have enough length
            return deflate_not_compress_package(src, pos, end, dest, cres, is_last);
        }
        if ((TOT_LENGTH += HEADER_SIZE) >= cl + NHEADER_SIZE) {
            // use mode 0
            // don't compress this package will be better
            return deflate_not_compress_package(src, pos, end, dest, cres, is_last);
        }
        res -= HEADER_SIZE;
        out += HEADER_SIZE;

        uint32_t LIT_1_LENGTH = huffman_compress<deflate_lit_reader>(
            LIT_1_BUFFER,sz1 + sz2,
            out,res);
        if (LIT_1_LENGTH == -1) {
            return deflate_not_compress_package(src, pos, end, dest, cres, is_last);
        }
        if ((TOT_LENGTH += LIT_1_LENGTH) >= cl + NHEADER_SIZE) {
            return deflate_not_compress_package(src, pos, end, dest, cres, is_last);
        }
        assert(res >= LIT_1_LENGTH);
        res -= LIT_1_LENGTH;
        out += LIT_1_LENGTH;
        uint32_t LIT_2_LENGTH = (uint8_t*)LIT_2.get_ptr() - LIT_2_BUFFER;
        if (res < LIT_2_LENGTH) {
            return deflate_not_compress_package(src, pos, end, dest, cres, is_last);
        }
        if ((TOT_LENGTH += LIT_2_LENGTH) >= cl + NHEADER_SIZE) {
            return deflate_not_compress_package(src, pos, end, dest, cres, is_last);
        }
        memcpy(out,LIT_2_BUFFER,LIT_2_LENGTH);
        res -= LIT_2_LENGTH;
        out += LIT_2_LENGTH;
        uint32_t DIST_1_LENGTH = 0;
        if (sz2) {
            DIST_1_LENGTH = huffman_compress<deflate_dist_reader>(DIST_1_BUFFER, sz2,
                out, res);
            if (DIST_1_LENGTH == -1) {
                return deflate_not_compress_package(src, pos, end, dest, cres, is_last);
            }
            if ((TOT_LENGTH += DIST_1_LENGTH) >= cl + NHEADER_SIZE) {
                return deflate_not_compress_package(src, pos, end, dest, cres, is_last);
            }
        }
        assert(res >= DIST_1_LENGTH);
        res -= DIST_1_LENGTH;
        out += DIST_1_LENGTH;
        uint32_t DIST_2_LENGTH = (uint8_t*)DIST_2.get_ptr() - DIST_2_BUFFER;
        if (res < DIST_2_LENGTH) {
            return deflate_not_compress_package(src, pos, end, dest, cres, is_last);
        }
        if ((TOT_LENGTH += DIST_2_LENGTH) >= cl + NHEADER_SIZE) {
            return deflate_not_compress_package(src, pos, end, dest, cres, is_last);
        }
        memcpy(out,DIST_2_BUFFER,DIST_2_LENGTH);
        out += DIST_2_LENGTH;

        uint8_t* header = (uint8_t*)dest;
        *(header++) = (uint8_t)(is_last) | 2;
        uint32_t*u32header = (uint32_t*)header;
    #if is_little_endian
        *(u32header++) = LIT_1_LENGTH;
        *(u32header++) = LIT_2_LENGTH;
        *(u32header++) = DIST_1_LENGTH;
        *(u32header++) = DIST_2_LENGTH;
    #else
        *(u32header++) = bswap_32(LIT_1_LENGTH);
        *(u32header++) = bswap_32(LIT_2_LENGTH);
        *(u32header++) = bswap_32(DIST_1_LENGTH);
        *(u32header++) = bswap_32(DIST_2_LENGTH);
    #endif
        dest = (void*)out;
        return TOT_LENGTH;
    }

    size_t deflate_compress(const void*src,size_t l1,void*dest,size_t l2,int level) {
        size_t cl2 = l2;
        if (!deflate_compress_header(l1, dest, l2)) {
            return -1;
        }
        if (l1 < ZIP_MIN_LENGTH) {
            if (l2 < l1) {
                return -1;
            }
            memcpy(dest,src,l1);
            return ZIP_HEADER_SIZE + l1;
        }
        for (size_t pos = 0;; pos += BUFFER_SIZE) {
            uint32_t len;
            if (pos + BUFFER_SIZE >= l1) {
                len = deflate_compress_package(src, pos, l1, dest, l2, true, level);
                if (unlikely(len == -1)) {
                    return -1;
                }
                l2 -= len;
                break;
            }
            len = deflate_compress_package(src, pos, pos + BUFFER_SIZE, dest, l2, false, level);
            if (unlikely(len == -1)) {
                return -1;
            }
            l2 -= len;
        }
        return cl2 - l2;
    }

    uint32_t deflate_not_decompress_package(const void*& src, uint64_t length, void*& dest, bool& is_last) {
        uint8_t* in = (uint8_t*)src;
        uint8_t* out = (uint8_t*)dest;
        uint32_t len = 0;
        if (!is_last) {
            len = BUFFER_SIZE;
        }
        else {
            len = length % BUFFER_SIZE;
        }
        memcpy(dest,in + NHEADER_SIZE,len);
        src = (void*)(in + NHEADER_SIZE + len);
        dest = (void*)(out + len);
        return len;
    }

    uint32_t deflate_decompress_package(const void*& src, uint64_t length,void*& dest,bool&is_last) {
        uint8_t* in = (uint8_t*)src;
        uint8_t* out = (uint8_t*)dest;
        is_last = (*in) & 1;
        uint8_t mode = (*in) >> 1;
        if (!mode) {
            return deflate_not_decompress_package(src,length,dest,is_last);
        }
        uint32_t* u32in = (uint32_t*)(in+1);
    #if is_little_endian
        uint32_t LIT_1_LENGTH = *(u32in++);
        uint32_t LIT_2_LENGTH = *(u32in++);
        uint32_t DIST_1_LENGTH = *(u32in++);
        uint32_t DIST_2_LENGTH = *(u32in++);
    #else
        uint32_t LIT_1_LENGTH = bswap_32(*(u32in++));
        uint32_t LIT_2_LENGTH = bswap_32(*(u32in++));
        uint32_t DIST_1_LENGTH = bswap_32(*(u32in++));
        uint32_t DIST_2_LENGTH = bswap_32(*(u32in++));
    #endif
        in += HEADER_SIZE;
        if (huffman_decompress<deflate_lit_reader>(in, LIT_1_LENGTH, 
            LIT_1_BUFFER, std::size(LIT_1_BUFFER)) == -1) {
            return -1;
        }
        deflate_lit_reader LIT_1(LIT_1_BUFFER,nullptr);
        in += LIT_1_LENGTH;
        read_buffer LIT_2(in);
        if (DIST_1_LENGTH != 0) {
            in += LIT_2_LENGTH;
            if (huffman_decompress<deflate_dist_reader>(in,
                DIST_1_LENGTH, DIST_1_BUFFER, std::size(DIST_1_BUFFER)) == -1) {
                return -1;
            }
        }
        uint8_t* DIST_1(DIST_1_BUFFER);
        in += DIST_1_LENGTH;
        read_buffer DIST_2(in);
        in += DIST_2_LENGTH;
        src = (void*)in;
        for (uint16_t ch = LIT_1.read(); ch != 256;ch = LIT_1.read()) {
            if (ch < 256) {
                *(out++) = (uint8_t)ch;
            }
            else {
                ch -= 257;
                uint16_t match = MIN_LENGTH;
                if (ch < 8) {
                    match += ch;
                }
                else {
                    if (ch == 28) {
                        match = 258;
                    }
                    else {
                        uint16_t a = 4 + (ch & 3);
                        uint16_t b = (ch >> 2) - 1;
                        match += (a << b) | LIT_2.read(b);
                    }
                }
                uint8_t offset_ch = *(DIST_1++);
                OFFSET_TYPE offset = 1;
                if (offset_ch < 4) {
                    offset += offset_ch;
                }
                else {
                    uint16_t a = 2 + (offset_ch & 1);
                    uint16_t b = (offset_ch >> 1) - 1;
                    offset += (a << b) | DIST_2.read(b);
                }
                uint8_t*window_ptr = out - offset;
                while (match--) {
                    *(out++) = *(window_ptr++);
                }
            }
        }
        uint32_t len = out - (uint8_t*)dest;
        dest = (void*)out;
        return len;
    }

    size_t deflate_decompress(const void* src, size_t l1, void* dest, size_t l2) {
        if (l1 < 10) {
            return -1;
        }
        size_t length = deflate_decompress_header(src,l1,dest,l2);
        if (length == -1) {
            return -1;
        }
        if (length < ZIP_MIN_LENGTH) {
            if (l2 < length) {
                return -1;
            }
            memcpy(dest,src,length);
            return length;
        }
        bool is_last = false;
        do {
            uint32_t len = deflate_decompress_package(src,length,dest,is_last);
            if (unlikely(len == -1)) {
                return -1;
            }
        }while(!is_last);
        return length;
    }

    String deflate_compress(String_view src, int level) {
        String dest(10 + ((src.size() - 1) / BUFFER_SIZE + 1) * (BUFFER_SIZE + NHEADER_SIZE), Reserved{});
        size_t l = deflate_compress(src.data(), src.size(), dest.data(), dest.capacity(), level);
        dest.set_size(l);
        dest.shrink_to_fit();
        return dest;
    }

    void deflate_compress(String_view src, String& dest, int level) {
        dest.reserve(10 + ((src.size() - 1) / BUFFER_SIZE + 1) * (BUFFER_SIZE + NHEADER_SIZE));
        size_t l = deflate_compress(src.data(), src.size(), dest.data(), dest.capacity(), level);
        dest.set_size(l);
        dest.shrink_to_fit();
    }

    String deflate_decompress(String_view src) {
        uint8_t*in = (uint8_t*)src.data();
        if (*in != 'P' || *(in + 1) != 'K') {
            return String();
        }
        in += 2;
    #if is_little_endian
        uint64_t length = *(uint64_t*)in;
    #else
        uint64_t length = bswap_64(*(uint64_t*)in);
    #endif
        in += 8;
        String dest(length, Reserved{});
        size_t l = deflate_decompress(src.data(), src.size(), dest.data(), dest.capacity());
        dest.set_size(l);
        dest.shrink_to_fit();
        return dest;
    }

    void deflate_decompress(String_view src, String& dest) {
        dest.clear();
        uint8_t* in = (uint8_t*)src.data();
        if (*in != 'P' || *(in + 1) != 'K') {
            return ;
        }
        in += 2;
    #if is_little_endian
        uint64_t length = *(uint64_t*)in;
    #else
        uint64_t length = bswap_64(*(uint64_t*)in);
    #endif
        in += 8;
        dest.reserve(length);
        size_t l = deflate_decompress(src.data(), src.size(), dest.data(), dest.capacity());
        dest.set_size(l);
        dest.shrink_to_fit();
    }

}