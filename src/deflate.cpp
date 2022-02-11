#include "../include/deflate.h"

namespace wjr {

#define NHEADER_SIZE 5
#define HEADER_SIZE  17
#define BUFFER_SIZE  256 KB
#define WINDOW_SIZE  32  KB

    struct DEFLATE_CONFIG {
        int lazy_match_max_length;
        int lazy_match_max_step;
        int max_chain;
    };

    static DEFLATE_CONFIG deflate_config[10] = {
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

    constexpr static int MIN_LENGTH = 3;
    constexpr static int MAX_LENGTH = 256 + MIN_LENGTH - 1;

    const uint32_t fibonacci = 2654435769;
    uint32_t fib_hash(uint32_t x){return (x * fibonacci) >> 17; }
    uint32_t get_hash(const void* ptr) {
    #if is_little_endian
        return fib_hash(*(uint32_t*)ptr & 0x00FFFFFF);
    #else
        return fib_hash(*(uint32_t*)ptr & 0xFFFFFF00);
    #endif
    }

    USE_THREAD_LOCAL static uint16_t head[WINDOW_SIZE],
                                     nxt[WINDOW_SIZE|1],pre[WINDOW_SIZE|1],
                                     head_ptr = 1,tail_ptr = 1;
    USE_THREAD_LOCAL static size_t   key[WINDOW_SIZE|1];

    void push_list(const uint8_t* ptr, size_t pos) {
        uint32_t hs = get_hash(ptr + pos);
        uint16_t x = tail_ptr;
        if (++tail_ptr > WINDOW_SIZE) {
            tail_ptr = 1;
        }
        key[x] = pos;
        if (head[hs]) {
            nxt[x] = head[hs];
            pre[head[hs]] = x;
            head[hs] = x;
        }
        else {
            head[hs] = x;
            nxt[x] = 0;
        }
    }

    void pop_list(const uint8_t* ptr, size_t pos) {
        uint32_t hs = get_hash(ptr + pos);
        uint16_t x = head_ptr;
        if (++head_ptr > WINDOW_SIZE) {
            head_ptr = 1;
        }
        if (head[hs] != x) {
            nxt[pre[x]] = 0;
        }
        else {
            head[hs] = 0;
        }
    }

    void deflate_push(const uint8_t* ptr, size_t pos) {
        if (pos >= WINDOW_SIZE) {
            pop_list(ptr, pos - WINDOW_SIZE);
        }
        push_list(ptr,pos);
    }

    class deflate_reader {
    public:
        constexpr static int max_number = 286;
        using number_type = uint16_t;
        deflate_reader(const void* p, const void* e)
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

    uint32_t longest_match(const uint8_t* ptr, size_t pos, size_t length, uint16_t& offset,int level) {

        if(pos + 2 >= length) return 0;

        const uint8_t* buffer = ptr + pos ;
        const uint8_t* buffer_limit = buffer + MAX_LENGTH;
        if (buffer_limit > ptr + length) {
            buffer_limit = ptr + length;
        }

        uint32_t hs = get_hash(buffer);

        uint32_t longest = 0;

        int max_chain = deflate_config[level].max_chain;
        for (uint32_t i = head[hs]; i && max_chain; i = nxt[i], --max_chain) {
            const uint8_t* buffer_ptr = buffer;
            const uint8_t* window_ptr = ptr + key[i];
            if(buffer_ptr[longest] != window_ptr[longest])continue;
            while (buffer_ptr != buffer_limit && *buffer_ptr == *window_ptr) {
                ++buffer_ptr;
                ++window_ptr;
            }
            if (buffer_ptr - buffer > longest) {
                offset = pos - key[i] - 1;
                longest = buffer_ptr - buffer;
                if (longest == MAX_LENGTH) {
                    break;
                }
            }
        }

        return longest >= 3 ? longest : 0;
    }

    USE_THREAD_LOCAL static uint8_t LIT_1_BUFFER[BUFFER_SIZE * 2 + 2],
                                    LIT_2_BUFFER[BUFFER_SIZE /  72],
                                    DIST_1_BUFFER[BUFFER_SIZE * 8 / 24],
                                    DIST_2_BUFFER[BUFFER_SIZE * 13 / 24];

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

    struct write_buffer {
        explicit write_buffer(void* p) 
            : ptr((uint8_t*)p),bit(8) {
            *ptr = 0;
        }
        void write(uint16_t ch,int l) {
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

    struct read_buffer {
        explicit read_buffer(void* p)
            : ptr((uint8_t*)p),bit(8) {

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

        uint8_t*ptr;
        int bit;
    };


    uint32_t deflate_not_compress_package(void* src, size_t pos, size_t end, void*& dest, size_t res, bool is_last) {
        uint32_t cl = end - pos;
        if (res < cl + NHEADER_SIZE) {
            return -1;
        }
        uint8_t*out = (uint8_t*)dest;
        *(out++) = (uint8_t)is_last;
    #if is_little_endian
        *(uint32_t*)out = cl;
    #else
        *(uint32_t*)out = bswap_32(cl);
    #endif
        out += 4;
        memcpy(out,(uint8_t*)src + pos,cl);
        out += cl;
        dest = (void*)out;
        return cl + NHEADER_SIZE;
    }

    uint32_t deflate_compress_package(void* src, size_t pos,size_t end, 
        void*& dest,size_t res,bool is_last,int level) {
        uint32_t cl = end - pos;
        uint32_t cres = res;
        deflate_reader LIT_1(LIT_1_BUFFER,nullptr);
        write_buffer LIT_2(LIT_2_BUFFER);
        uint8_t* DIST_1(DIST_1_BUFFER);
        write_buffer DIST_2(DIST_2_BUFFER);

        uint32_t sz1 = 0,sz2 = 0;

        uint8_t* ptr = (uint8_t*)src;
        for (size_t i = pos, j; i < end;) {
            j = i;
            uint32_t match = 0;
            uint16_t offset;
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
                for (; j + 1 < end && lazy_match-- && match <= lazy_match_limit;) {
                    deflate_push(ptr, j);
                    ++j;
                    uint16_t offset2;
                    uint32_t match2 = longest_match(ptr, j, end, offset2,level);
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
            match -= 3;
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

        uint32_t LIT_1_LENGTH = huffman_compress<deflate_reader>(
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
            return -1;
        }
        if ((TOT_LENGTH += LIT_2_LENGTH) >= cl + NHEADER_SIZE) {
            return deflate_not_compress_package(src, pos, end, dest, cres, is_last);
        }
        memcpy(out,LIT_2_BUFFER,LIT_2_LENGTH);
        res -= LIT_2_LENGTH;
        out += LIT_2_LENGTH;
        uint32_t DIST_1_LENGTH = 0;
        if (sz2 != 0) {
            DIST_1_LENGTH = huffman_compress(DIST_1_BUFFER, sz2,
                out, res);
            if (DIST_1_LENGTH == -1) {
                return -1;
            }
            if ((TOT_LENGTH += DIST_1_LENGTH) >= cl + NHEADER_SIZE) {
                return deflate_not_compress_package(src, pos, end, dest, cres, is_last);
            }
            assert(res >= DIST_1_LENGTH);
            res -= DIST_1_LENGTH;
            out += DIST_1_LENGTH;
        }
        uint32_t DIST_2_LENGTH = (uint8_t*)DIST_2.get_ptr() - DIST_2_BUFFER;
        if (res < DIST_2_LENGTH) {
            return -1;
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

    size_t deflate_compress(void*src,size_t l1,void*dest,size_t l2,int level) {
        if (l2 < 2) {
            return -1;
        }
        size_t cl2 = l2;
        uint8_t* out = (uint8_t*)dest;
        *(out++) = 'P';
        *(out++) = 'K';
        dest = (void*)out;
        l2 -= 2;
        for (size_t pos = 0;; pos += BUFFER_SIZE) {
            uint32_t len;
            if (pos + BUFFER_SIZE < l1) {
                len = deflate_compress_package(src,pos,pos + BUFFER_SIZE,dest,l2,false,level);
                if (unlikely(len == -1)) {
                    return -1;
                }
                l2 -= len;
            }
            else {
                len = deflate_compress_package(src,pos,l1,dest,l2,true,level);
                if (unlikely(len == -1)) {
                    return -1;
                }
                l2 -= len;
                break;
            }
        }
        return cl2 - l2;
    }

    uint32_t deflate_not_decompress_package(void*& src, void*& dest, size_t res, bool& is_last) {
        uint8_t* in = (uint8_t*)src;
        uint8_t* out = (uint8_t*)dest;
        uint32_t* u32in = (uint32_t*)(in + 1);
    #if is_little_endian
        uint32_t len = *u32in;
    #else
        uint32_t len = bswap_32(*u32in);
    #endif
        if (res < len) {
            return -1;
        }
        memcpy(dest,in + 5,len);
        src = (void*)(in + 5 + len);
        dest = (void*)(out + len);
        return len;
    }

    uint32_t deflate_decompress_package(void*& src, void*& dest, size_t res,bool&is_last) {
        uint8_t* in = (uint8_t*)src;
        uint8_t* out = (uint8_t*)dest;
        is_last = (*in) & 1;
        uint8_t mode = (*in) >> 1;
        if (!mode) {
            return deflate_not_decompress_package(src,dest,res,is_last);
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
        src = (void*)(in + HEADER_SIZE + LIT_1_LENGTH + LIT_2_LENGTH + DIST_1_LENGTH + DIST_2_LENGTH);
        if (huffman_decompress<deflate_reader>(in + HEADER_SIZE, LIT_1_LENGTH, 
            LIT_1_BUFFER, std::size(LIT_1_BUFFER)) == -1) {
            return -1;
        }
        deflate_reader LIT_1(LIT_1_BUFFER,nullptr);
        read_buffer LIT_2(in + HEADER_SIZE + LIT_1_LENGTH);
        if (DIST_1_LENGTH != 0) {
            if (huffman_decompress(in + HEADER_SIZE + LIT_1_LENGTH + LIT_2_LENGTH,
                DIST_1_LENGTH, DIST_1_BUFFER, std::size(DIST_1_BUFFER)) == -1) {
                return -1;
            }
        }
        uint8_t* DIST_1(DIST_1_BUFFER);
        read_buffer DIST_2(in + HEADER_SIZE + LIT_1_LENGTH + LIT_2_LENGTH + DIST_1_LENGTH);
        for (uint16_t ch = LIT_1.read(); ch != 256;ch = LIT_1.read()) {
            if (ch < 256) {
                *(out++) = (uint8_t)ch;
            }
            else {
                ch -= 257;
                uint16_t match = 3;
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
                uint16_t offset_ch = *(DIST_1++);
                uint32_t offset = 1;
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

    size_t deflate_decompress(void* src, size_t l1, void* dest, size_t l2) {
        if (l1 < 2) {
            return -1;
        }
        uint8_t*in = (uint8_t*)src;
        if (*in != 'P' || *(in + 1) != 'K') {
            return -1;
        }
        src = (void*)(in + 2);

        size_t cl2 = l2;
        bool is_last = false;
        do {
            uint32_t len = deflate_decompress_package(src,dest,l2,is_last);
            assert(is_last || len == BUFFER_SIZE);
            if (unlikely(len == -1)) {
                return -1;
            }
            l2 -= len;
        }while(!is_last);
        return cl2 - l2;
    }

}