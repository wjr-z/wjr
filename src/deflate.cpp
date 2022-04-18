#include "../include/deflate.h"
namespace wjr {
#define ZIP_HEADER_SIZE (10)
#define NHEADER_SIZE     (1)
#define HEADER_SIZE     (17)
	constexpr static size_t ZIP_MIN_LENGTH = 1 KB;
	constexpr static size_t BUFFER_SIZE = 256 KB;
	constexpr static size_t WINDOW_SIZE = 32  KB;
	constexpr static uint32_t WINDOW_SIZE_BIT = quick_log2(WINDOW_SIZE);
	constexpr static uint32_t ANTI_WINDOW_SIZE_BIT = 32 - WINDOW_SIZE_BIT;
	constexpr static uint32_t MAX_OFFSET_CODE = 2 * WINDOW_SIZE_BIT;
	constexpr static uint32_t MIN_LENGTH = 3;
	constexpr static uint32_t MAX_LENGTH = 256 + MIN_LENGTH - 1;
	static_assert(!(WINDOW_SIZE& (WINDOW_SIZE - 1)), "WINDOW_SIZE must be power of two");
	static_assert(WINDOW_SIZE <= 256 KB);

	struct DEFLATE_CONFIG {
		uint32_t lazy_match_max_length;
		uint32_t lazy_match_max_step;
		uint32_t max_chain;
	};

	constexpr static DEFLATE_CONFIG deflate_config[10] = {
		{0  ,0  ,0  },
		{0  ,0  ,4  },
		{0  ,0  ,8  },
		{0  ,0  ,16 },
		{8  ,16 ,32 },
		{32 ,16 ,64 },
		{32 ,64 ,96 },
		{128,64 ,128},
		{128,256,192},
		{256,256,256}
	};

	constexpr uint32_t fibonacci = 2654435769;
	uint32_t fib_hash(uint32_t x) {
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

	USE_THREAD_LOCAL static uint32_t head[WINDOW_SIZE],
		prev[WINDOW_SIZE | 1];
	USE_THREAD_LOCAL static size_t   key[WINDOW_SIZE | 1];

	void deflate_push(const uint8_t* ptr, size_t pos) {
		ptr += pos;
		uint32_t hs, x = (pos & (WINDOW_SIZE - 1)) + 1;
		if (pos >= WINDOW_SIZE) {
			hs = get_hash(ptr - WINDOW_SIZE);
			if (head[hs] == x) {
				head[hs] = 0;
			}
		}
		hs = get_hash(ptr);
		key[x] = pos;
		prev[x] = head[hs];
		head[hs] = x;
	}

	USE_THREAD_LOCAL static uint8_t LIT_1_BUFFER[(BUFFER_SIZE + 1) * 2],
		LIT_2_BUFFER[BUFFER_SIZE / 72 + 16],
		DIST_1_BUFFER[BUFFER_SIZE * 8 / 24 + 16],
		DIST_2_BUFFER[BUFFER_SIZE * 13 / 24 + 16];

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
		size_t length, uint32_t& offset, int level, uint32_t min_limit = MIN_LENGTH - 1) {
		if (pos + min_limit >= length) return 0;

		auto max_length = MAX_LENGTH;
		if (unlikely(max_length > length - pos)) [[unlikely]] {
			max_length = length - pos;
		}
		const uint8_t* buffer = ptr + pos;
		const uint8_t* buffer_limit = buffer + max_length;

		uint32_t hs = get_hash(buffer);
		uint32_t longest = min_limit;
		uint32_t max_chain = deflate_config[level].max_chain;

		if (head[hs]) {
			for (uint32_t i = head[hs]; max_chain; --max_chain) {
				const uint8_t* buffer_ptr = buffer;
				const uint8_t* window_ptr = ptr + key[i];
				if (buffer_ptr[longest] == window_ptr[longest]) {
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
						offset = key[i];
						longest = buffer_ptr - buffer;
						if (longest == max_length) {
							break;
						}
					}
				}
				auto j = prev[i];
				if (key[j] > key[i])break;
				i = j;
			}
		}

		offset = pos - offset - 1;
		return longest != min_limit ? longest : 0;
	}

	bool deflate_compress_header(size_t l1, void*& dest, size_t& l2) {
		if (l2 < ZIP_HEADER_SIZE) {
			return false;
		}
		uint8_t* out = (uint8_t*)dest;
		*(out++) = 'P';
		*(out++) = 'K';
		*(uint64_t*)out = auto_bswap<uint64_t>(l1);
		out += 8;
		dest = (void*)(out);
		l2 -= ZIP_HEADER_SIZE;
		key[0] = l1;
		return true;
	}

	size_t deflate_uncompress_header(const void*& src, size_t& l1, void* dest, size_t l2) {
		if (l1 < ZIP_HEADER_SIZE) {
			return -1;
		}
		uint8_t* in = (uint8_t*)src;
		if (*in != 'P' || *(in + 1) != 'K') {
			return -1;
		}
		in += 2;
		uint64_t length = auto_bswap<uint64_t>(*(uint64_t*)in);
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
		uint8_t* out = (uint8_t*)dest;
		*(out++) = (uint8_t)is_last;
		memcpy(out, (uint8_t*)src + pos, cl);
		out += cl;
		dest = (void*)out;
		return cl + NHEADER_SIZE;
	}

	uint32_t deflate_compress_package(const void* src, size_t pos, size_t end,
		void*& dest, size_t res, bool is_last, int level) {
		uint32_t cl = end - pos;
		uint32_t cres = res;
		uint16_t* LIT_1((uint16_t*)LIT_1_BUFFER);
		write_buffer LIT_2(LIT_2_BUFFER);
		uint8_t* DIST_1(DIST_1_BUFFER);
		write_buffer DIST_2(DIST_2_BUFFER);

		uint32_t sz1 = 0, sz2 = 0;

		uint8_t* ptr = (uint8_t*)src;
		uint32_t lazy_match_limit = deflate_config[level].lazy_match_max_length;
		auto push_match_offset = [&sz2, &LIT_1, &LIT_2, &DIST_1, &DIST_2](uint32_t match, uint32_t offset) {
			++sz2;
			match -= MIN_LENGTH;
			if (match < 8) {
				*LIT_1 = 257 + match;
				++LIT_1;
			}
			else {
				if (match == 255) {
					*LIT_1 = 285;
					++LIT_1;
				}
				else {
					auto LIT_bit = deflate_log[match];
					auto LIT_code = 253 + ((LIT_bit << 2) | ((match >> (LIT_bit - 2)) & 3));
					assert(LIT_code < 286);
					*LIT_1 = LIT_code;
					++LIT_1;
					LIT_2.write(match & binary_mask[LIT_bit - 2], LIT_bit - 2);
				}
			}
			if (offset < 4) {
				*(DIST_1++) = offset;
			}
			else {
				auto DIST_bit = quick_log2(offset);
				auto DIST_code = (DIST_bit << 1) | ((offset >> (DIST_bit - 1)) & 1);
				*(DIST_1++) = DIST_code;
				DIST_2.write(offset & binary_mask[DIST_bit - 1], DIST_bit - 1);
			}
		};
		for (size_t i = pos, j; i != end;) {
			j = i;
			uint32_t match = 0;
			uint32_t offset;
			while (j != end && !(match = longest_match(ptr, j, end, offset, level))) {
				deflate_push(ptr, j);
				++j;
			}

			sz1 += j - i;
			while (i != j) {
				*LIT_1 = ptr[i];
				++LIT_1;
				++i;
			}

			if (unlikely(!match)) [[unlikely]] continue;

			uint32_t lazy_match = deflate_config[level].lazy_match_max_step;
			while (lazy_match-- && match < lazy_match_limit) {
				deflate_push(ptr, j);
				++j;
				uint32_t offset2;
				uint32_t match2 = longest_match(ptr, j, end, offset2, level, match);
				if (match2 <= match) {
					break;
				}
				match = match2;
				offset = offset2;
				*LIT_1 = ptr[i++];
				++LIT_1;
				++sz1;
			}

			i += match;
			for (; j < i; ++j) {
				deflate_push(ptr, j);
			}

			push_match_offset(match, offset);
		}

		if (is_last) {
			for (size_t i = end >= WINDOW_SIZE ? end - WINDOW_SIZE : 0; i < end; ++i) {
				uint32_t hs = get_hash(ptr + i);
				head[hs] = 0;
			}
		}

		++sz1;
		*LIT_1 = 256;
		++LIT_1;
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

		uint32_t LIT_1_LENGTH = huffman_compress<uint16_t*, 286>(
			((uint16_t*)LIT_1_BUFFER),
			((uint16_t*)LIT_1_BUFFER + (sz1 + sz2)),
			out, out + res);
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
		memcpy(out, LIT_2_BUFFER, LIT_2_LENGTH);
		res -= LIT_2_LENGTH;
		out += LIT_2_LENGTH;
		uint32_t DIST_1_LENGTH = 0;
		if (sz2) {
			DIST_1_LENGTH = huffman_compress<uint8_t*, MAX_OFFSET_CODE>(DIST_1_BUFFER,
				DIST_1_BUFFER + sz2,
				out, out + res);
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
		memcpy(out, DIST_2_BUFFER, DIST_2_LENGTH);
		res -= DIST_2_LENGTH;
		out += DIST_2_LENGTH;

		uint8_t* header = (uint8_t*)dest;
		*(header++) = (uint8_t)(is_last) | 2;
		uint32_t* u32header = (uint32_t*)header;
		*(u32header++) = auto_bswap<uint32_t>(LIT_1_LENGTH);
		*(u32header++) = auto_bswap<uint32_t>(LIT_2_LENGTH);
		*(u32header++) = auto_bswap<uint32_t>(DIST_1_LENGTH);
		*(u32header++) = auto_bswap<uint32_t>(DIST_2_LENGTH);
		dest = (void*)out;
		return TOT_LENGTH;
	}

	size_t deflate_compress(const void* src, size_t l1, void* dest, size_t l2, int level) {
		size_t cl2 = l2;
		if (!deflate_compress_header(l1, dest, l2)) {
			return -1;
		}
		if (l1 < ZIP_MIN_LENGTH) {
			if (l2 < l1) {
				return -1;
			}
			memcpy(dest, src, l1);
			return ZIP_HEADER_SIZE + l1;
		}
		for (size_t pos = 0;; pos += BUFFER_SIZE) {
			uint32_t len;
			if (pos + BUFFER_SIZE >= l1) {
				len = deflate_compress_package(src, pos, l1, dest, l2, true, level);
				if (unlikely(len == -1)) [[unlikely]] {
					return -1;
				}
				l2 -= len;
				break;
			}
			len = deflate_compress_package(src, pos, pos + BUFFER_SIZE, dest, l2, false, level);
			if (unlikely(len == -1)) [[unlikely]] {
				return -1;
			}
			l2 -= len;
		}
		return cl2 - l2;
	}

	uint32_t deflate_not_uncompress_package(const void*& src, uint64_t length, void*& dest, bool& is_last) {
		uint8_t* in = (uint8_t*)src;
		uint8_t* out = (uint8_t*)dest;
		uint32_t len = 0;
		if (!is_last) {
			len = BUFFER_SIZE;
		}
		else {
			len = length % BUFFER_SIZE;
		}
		memcpy(dest, in + NHEADER_SIZE, len);
		src = (void*)(in + NHEADER_SIZE + len);
		dest = (void*)(out + len);
		return len;
	}

	uint32_t deflate_uncompress_package(const void*& src, uint64_t length, void*& dest, bool& is_last) {
		uint8_t* in = (uint8_t*)src;
		uint8_t* out = (uint8_t*)dest;
		is_last = (*in) & 1;
		uint8_t mode = (*in) >> 1;
		if (!mode) {
			return deflate_not_uncompress_package(src, length, dest, is_last);
		}
		uint32_t* u32in = (uint32_t*)(in + 1);
		uint32_t LIT_1_LENGTH = auto_bswap<uint32_t>(*(u32in++));
		uint32_t LIT_2_LENGTH = auto_bswap<uint32_t>(*(u32in++));
		uint32_t DIST_1_LENGTH = auto_bswap<uint32_t>(*(u32in++));
		uint32_t DIST_2_LENGTH = auto_bswap<uint32_t>(*(u32in++));
		in += HEADER_SIZE;
		if (huffman_uncompress<uint16_t*, 286>(in, in + LIT_1_LENGTH,
			((uint16_t*)LIT_1_BUFFER),
			((uint16_t*)(LIT_1_BUFFER + std::size(LIT_1_BUFFER)))) == -1) {
			return -1;
		}
		uint16_t* LIT_1((uint16_t*)LIT_1_BUFFER);
		in += LIT_1_LENGTH;
		read_buffer LIT_2(in);
		if (DIST_1_LENGTH != 0) {
			in += LIT_2_LENGTH;
			if (huffman_uncompress<uint8_t*, MAX_OFFSET_CODE>(in,
				in + DIST_1_LENGTH, DIST_1_BUFFER, DIST_1_BUFFER + std::size(DIST_1_BUFFER)) == -1) {
				return -1;
			}
		}
		uint8_t* DIST_1(DIST_1_BUFFER);
		in += DIST_1_LENGTH;
		read_buffer DIST_2(in);
		in += DIST_2_LENGTH;
		src = (void*)in;
		for (uint16_t ch = *(LIT_1++); ch != 256; ch = *(LIT_1++)) {
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
				uint32_t offset = 1;
				if (offset_ch < 4) {
					offset += offset_ch;
				}
				else {
					uint16_t a = 2 + (offset_ch & 1);
					uint16_t b = (offset_ch >> 1) - 1;
					offset += (a << b) | DIST_2.read(b);
				}
				uint8_t* window_ptr = out - offset;
				while (match--) {
					*(out++) = *(window_ptr++);
				}
			}
		}
		uint32_t len = out - (uint8_t*)dest;
		dest = (void*)out;
		return len;
	}

	size_t deflate_uncompress(const void* src, size_t l1, void* dest, size_t l2) {
		if (l1 < 10) {
			return -1;
		}
		size_t length = deflate_uncompress_header(src, l1, dest, l2);
		if (length == -1) {
			return -1;
		}
		if (length < ZIP_MIN_LENGTH) {
			if (l2 < length) {
				return -1;
			}
			memcpy(dest, src, length);
			return length;
		}
		bool is_last = false;
		do {
			uint32_t len = deflate_uncompress_package(src, length, dest, is_last);
			if (unlikely(len == -1)) [[unlikely]] {
				return -1;
			}
		} while (!is_last);
		return length;
	}

	String deflate_compress(String_view src, int level) {
		String dest(10 + (src.size() / BUFFER_SIZE + 1) * (BUFFER_SIZE + NHEADER_SIZE), Reserved{});
		size_t l = deflate_compress(src.data(), src.size(), dest.data(), dest.capacity(), level);
		dest.set_size(l);
		dest.shrink_to_fit();
		return dest;
	}

	void deflate_compress(String_view src, String& dest, int level) {
		dest.reserve(10 + (src.size() / BUFFER_SIZE + 1) * (BUFFER_SIZE + NHEADER_SIZE));
		size_t l = deflate_compress(src.data(), src.size(), dest.data(), dest.capacity(), level);
		dest.set_size(l);
		dest.shrink_to_fit();
	}

	String deflate_uncompress(String_view src) {
		uint8_t* in = (uint8_t*)src.data();
		if (*in != 'P' || *(in + 1) != 'K') {
			return String();
		}
		in += 2;
		uint64_t length = auto_bswap<uint64_t>(*(uint64_t*)in);
		in += 8;
		String dest(length, Reserved{});
		size_t l = deflate_uncompress(src.data(), src.size(), dest.data(), dest.capacity());
		dest.set_size(l);
		dest.shrink_to_fit();
		return dest;
	}

	void deflate_uncompress(String_view src, String& dest) {
		dest.clear();
		uint8_t* in = (uint8_t*)src.data();
		if (*in != 'P' || *(in + 1) != 'K') {
			return;
		}
		in += 2;
		uint64_t length = auto_bswap<uint64_t>(*(uint64_t*)in);
		in += 8;
		dest.reserve(length);
		size_t l = deflate_uncompress(src.data(), src.size(), dest.data(), dest.capacity());
		dest.set_size(l);
		dest.shrink_to_fit();
	}
}