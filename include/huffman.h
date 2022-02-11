#ifndef __WJR_HUFFMAN_H
#define __WJR_HUFFMAN_H

#include <memory>
#include <algorithm>
#include <string.h>
#include "mySTL.h"

namespace wjr {

	struct default_huffman_reader {
	public:
		constexpr static int max_number = 256;
		using number_type = uint8_t;

		default_huffman_reader(const void* p,const void*e)
			: ptr((uint8_t*)p),end((uint8_t*)e) {

		}

		uint8_t read() {
			assert(ptr != end);
			return *(ptr++);
		}

		bool write(uint8_t ch) {
			if (ptr == end) {
				return false;
			}
			*(ptr++) = ch;
			return true;
		}

		void* get_ptr()const {
			return (void*)ptr;
		}

	private:
		uint8_t* ptr;
		uint8_t* end;
	};

	struct huffman_node {
		unsigned long long ch;
		unsigned long long cnt;
		bool operator<(const huffman_node& other)const {
			return cnt < other.cnt;
		}
	};

	struct huffman_tree_node {
		unsigned short son[2];
	};

	struct huffman_que_node {
		short id;
	};

	struct huffman_decode_next {
		uint8_t l;
		unsigned short next;
	};

	static void build_tree(huffman_node* arr, huffman_tree_node* tr, int tot) {
		int ql = 0, qr = 0;
		int ptot = 0;
		int tl = 0;
		for (int i = 0; i < tot; ++i) {
			tr[i + tot - 1].son[0] = USHORT_MAX;
			tr[i + tot - 1].son[1] = arr[i].ch;
		}
		auto link = [&arr, &ptot, &tr, &tl, tot](int ls, int rs) {
			tr[tl].son[0] = ls < ptot ? ls : ls + tot - 1;
			tr[tl].son[1] = rs < ptot ? rs : rs + tot - 1;
			++tl;
			arr[ptot].cnt = arr[ls].cnt + arr[rs].cnt;
			++ptot;
		};
		while (ql < tot - 1) {
			if (qr < ptot - 1) {
				if (arr[ql + 1].cnt < arr[qr].cnt) {
					link(ql, ql + 1);
					ql += 2;
				}
				else if (arr[ql].cnt < arr[qr + 1].cnt) {
					link(ql, qr);
					++ql;
					++qr;
				}
				else {
					link(qr, qr + 1);
					qr += 2;
				}
			}
			else if (qr < ptot) {
				if (arr[ql + 1].cnt < arr[qr].cnt) {
					link(ql, ql + 1);
					ql += 2;
				}
				else {
					link(ql, qr);
					++ql;
					++qr;
				}
			}
			else {
				link(ql, ql + 1);
				ql += 2;
			}
		}

		while (ql < tot && qr < ptot) {
			if (qr < ptot - 1) {
				if (arr[qr + 1].cnt < arr[ql].cnt) {
					link(qr, qr + 1);
					qr += 2;
				}
				else {
					link(ql, qr);
					++ql;
					++qr;
				}
			}
			else {
				link(ql, qr);
				++ql;
				++qr;
			}
		}

		while (qr < ptot - 1) {
			link(qr, qr + 1);
			qr += 2;
		}
	}

	template<typename reader = default_huffman_reader> 
	size_t huffman_compress(const void* src, size_t l1, void* dest,size_t l2) {

		constexpr static int max_number = reader::max_number;
		constexpr static int max_bit = quick_log2(max_number - 1) + 1;
		using number_type = typename reader::number_type;

		if (l1 <= 16) {
			return -1;
		}

		USE_THREAD_LOCAL static void* ptr =
			malloc(sizeof(huffman_node) * max_number + 
				(sizeof(huffman_tree_node) + sizeof(huffman_que_node)) * (2 * max_number - 1));

		void* mem_block = ptr;
		huffman_node* arr = (huffman_node*)mem_block;
		mem_block = (void*)((char*)(mem_block) + sizeof(huffman_node) * max_number);
		huffman_tree_node* tr = (huffman_tree_node*)mem_block;
		mem_block = (void*)((char*)(mem_block) + sizeof(huffman_tree_node) * (2 * max_number - 1));
		huffman_que_node* que = (huffman_que_node*)mem_block;

		const uint8_t* in = (const uint8_t*)src;
		uint8_t* out = (uint8_t*)dest;
		uint8_t* end = out + l2;

		for (int i = 0; i < max_number; ++i) {
			arr[i].ch = i;
			arr[i].cnt = 0;
		}

		reader it1(src,(char*)src + l1);
		for (size_t i = 0; i < l1; ++i) {
			auto ch = it1.read();
			assert(ch < max_number);
			++arr[ch].cnt;
		}

		int tot = 0;
		for (int i = 0; i < max_number; ++i) {
			if (arr[i].cnt != 0) {
				arr[tot++] = arr[i];
			}
		}

		if (tot == 1) {
			if (l2 < 12) {
				return -1;
			}
			*(out++) = 0;
		#if is_little_endian
			*(uint16_t*)out = 1;
		#else
			*(uint16_t*)out = bswap_16(1);
		#endif
			out += 2;
			*(out++) = arr[0].ch;
			if constexpr (sizeof(size_t) == 8) {
			#if is_little_endian
				*(uint32_t*)out = (l1 >> 32);
			#else
				*(uint32_t*)out = bswap_32(l1 >> 32);
			#endif
			}
			else {
				*(uint32_t*)out = 0;
			}
			out += 4;
		#if is_little_endian
			* (uint32_t*)out = (uint32_t)l1;
		#else
			* (uint32_t*)out = bswap_32((uint32_t)l1);
		#endif
			out += 4;
			return out - (uint8_t*)dest;
		}

		std::sort(arr, arr + tot);

		int buffer_bit = 8;

		auto buffer_append =
			[&buffer_bit, &out, end](unsigned long long v, uint8_t l)->bool {
			if (l >= buffer_bit) {
				l -= buffer_bit;
				*(out++) |= (uint8_t)(v >> l);
				while (l >= 8) {
					l -= 8;
					*(out++) = (uint8_t)(v >> l);
				}
				buffer_bit = 8 - l;
				*out = (uint8_t)((v & binary_mask[l]) << buffer_bit);
			}
			else {
				buffer_bit -= l;
				*out |= v << buffer_bit;
			}
			return true;
		};

		// 8 bit : last_bit
		// 16 bit : how many char

		build_tree(arr, tr, tot);

		++out;
	#if is_little_endian
		*(uint16_t*)out = tot;
	#else
		*(uint16_t*)out = bswap_16(tot);
	#endif
		out += 2;
		*out = 0;

		{
			int que_head = 0, que_tail = 1;
			que[0].id = tot - 2;
			int dep = 0;
			int this_dep_cnt = 1;
			int next_dep_cnt = 0;
			unsigned long long code = 0;
			while (que_head < que_tail) {
				auto& x = que[que_head++];
				int id = x.id;
				if (tr[id].son[0] == USHORT_MAX) {
					unsigned short ch = tr[id].son[1];
					arr[ch].ch = dep;
					arr[ch].cnt = code++;
					if (!buffer_append(ch, max_bit)) {
						return -1;
					}
					if (!buffer_append(dep, 6)) {
						return -1;
					}
				}
				else {
					que[que_tail].id = tr[id].son[0];
					++que_tail;
					que[que_tail].id = tr[id].son[1];
					++que_tail;
					next_dep_cnt += 2;
				}
				if (!(--this_dep_cnt)) {
					code <<= 1;
					this_dep_cnt = next_dep_cnt;
					next_dep_cnt = 0;
					++dep;
				}
			}
		}

		reader it2(src,(char*)src + l1);
		for (size_t i = 0; i < l1;++i) {
			auto ch = it2.read();
			if (!buffer_append(arr[ch].cnt, arr[ch].ch)) {
				return -1;
			}
		}

		*(uint8_t*)dest = 0;
		if (buffer_bit != 8) {
			if (out == end) {
				return -1;
			}
			*(uint8_t*)dest = buffer_bit;
			++out;
		}
		return out - (uint8_t*)dest;
	}

	static uint8_t huffman_get_bit_1(uint8_t*& in, int& l) {
		if (l != 1) {
			--l;
			return (*in >> l) & binary_mask[1];
		}
		uint8_t r = (*in) & binary_mask[1];
		++in;
		l = 8;
		return r;
	}

	static uint8_t huffman_get_bit_8(uint8_t*& in, int& l, int bit) {

		assert(bit <= 8);
		if (bit < l) {
			l -= bit;
			return (*in >> l) & binary_mask[bit];
		}
		uint8_t r = (*in) & binary_mask[l];
		bit -= l;
		++in;
		l = 8 - bit;
		return (r << bit) | ((*in >> l) & binary_mask[bit]);
	}

	static uint16_t huffman_get_bit_16(uint8_t*& in, int& l, int bit) {
		assert(bit <= 16);
		if (bit <= 8) {
			return huffman_get_bit_8(in,l,bit);
		}
		uint16_t ch = huffman_get_bit_8(in,l,8);
		bit -= 8;
		ch <<= bit;
		ch |= huffman_get_bit_8(in,l,bit);
		return ch;
	}

	static unsigned short huffman_init_decode_tree(
		huffman_tree_node* tr, huffman_node* arr, unsigned short& X, int& now, int dep, int tot) {
		if (arr[now].ch == dep) {
			++now;
			return now + tot - 2;
		}
		unsigned short l = huffman_init_decode_tree(tr, arr, X, now, dep + 1, tot);
		unsigned short r = huffman_init_decode_tree(tr, arr, X, now, dep + 1, tot);
		tr[X].son[0] = l;
		tr[X].son[1] = r;
		return X++;
	}

	static void huffman_init_decode_next(huffman_tree_node* tr, unsigned short x, 
		huffman_decode_next* dn, int dep, uint8_t v) {
		if (tr[x].son[0] == USHORT_MAX) {
			for (int i = (1 << (dep + 1)) - 1; ~i; --i) {
				uint8_t id = v | i;
				dn[id].l = 7 - dep;
				dn[id].next = tr[x].son[1];
			}
			return;
		}
		if (dep == -1) {
			dn[v].l = 0;
			dn[v].next = x;
			return;
		}
		huffman_init_decode_next(tr, tr[x].son[0], dn, dep - 1, v);
		huffman_init_decode_next(tr, tr[x].son[1], dn, dep - 1, v | (1 << dep));
	}

	template<typename reader = default_huffman_reader>
	size_t huffman_decompress(const void* src, size_t l1, void* dest, size_t l2) {
		constexpr static int max_number = reader::max_number;
		constexpr static int max_bit = quick_log2(max_number - 1) + 1;
		using number_type = typename reader::number_type;
		USE_THREAD_LOCAL static void* ptr =
			malloc(sizeof(huffman_node) * max_number
				+ sizeof(huffman_tree_node) * (2 * max_number - 1)
				+ sizeof(huffman_decode_next) * 256);
		void* mem_block = ptr;
		huffman_node* arr = (huffman_node*)mem_block;
		mem_block = (void*)((char*)(mem_block) + sizeof(huffman_node) * max_number);
		huffman_tree_node* tr = (huffman_tree_node*)mem_block;
		mem_block = (void*)((char*)(mem_block) + sizeof(huffman_tree_node) * (2 * max_number - 1));
		huffman_decode_next* dn = (huffman_decode_next*)mem_block;

		reader it(dest,(char*)dest + l2);

		uint8_t* in = (uint8_t*)src;
		uint8_t* ib = in + l1 - 1;
		uint8_t* end = (uint8_t*)dest + l2;
		uint8_t last_bit = *(in++);
		int tot = 0;
	#if is_little_endian
		tot = *(uint16_t*)in;
	#else
		tot = bswap_16(*(uint16_t*)in);
	#endif
		in += 2;
		if (tot == 1) {
			uint8_t ch = *(in++);
			size_t length = 0;
			if constexpr (sizeof(size_t) == 8) {
			#if is_little_endian
				length = *(uint32_t*)in;
			#else
				length = bswap_32(*(uint32_t*)in);
			#endif
				length <<= 32;
			}
			in += 4;
		#if is_little_endian
			length |= *(uint32_t*)in;
		#else
			length |= bswap_32(*(uint32_t*)in);
		#endif
			memset(dest,ch,length);
			return length;
			
		}
		int l = 8;
		uint8_t dep = 0;
		unsigned long long code = 0;
		for (int i = 0; i < tot; ++i) {
			uint16_t ch = huffman_get_bit_16(in, l, max_bit);
			uint8_t bit_length = huffman_get_bit_8(in, l, 6);
			code <<= (bit_length - dep);
			arr[i].ch = bit_length;
			arr[ch].cnt = code++;
			tr[i + tot - 1].son[0] = USHORT_MAX;
			tr[i + tot - 1].son[1] = ch;
			dep = bit_length;
		}
		unsigned short X = 0;
		int Y = 0;
		huffman_init_decode_tree(tr, arr, X, Y, 0, tot);
		huffman_init_decode_next(tr, tot - 2, dn, 7, 0);

		unsigned long long buffer = 0;
		int buffer_length = 0;
		*(uint8_t*)dest = 0;
		if (in != ib) {
			buffer_length = l;
			buffer = huffman_get_bit_8(in, l, l);
			while (in != ib) {
				buffer = buffer << 8 | (*in++);
				buffer_length += 8;
				while (buffer_length >= 8) {
					uint8_t ch = (buffer >> (buffer_length - 8));
					if (dn[ch].l) {
						if (!it.write(dn[ch].next)) {
							return -1;
						}
						buffer_length -= dn[ch].l;
					}
					else {
						int x = dn[ch].next;
						buffer_length -= 8;
						while (buffer_length--) {
							bool c = (buffer >> buffer_length) & 1;
							x = tr[x].son[c];
							if (tr[x].son[0] == USHORT_MAX) {
								if (!it.write(tr[x].son[1])) {
									return -1;
								}
								break;
							}
						}
						for (; buffer_length == -1;) {
							if (unlikely(in == ib)) {
								int res = 8 - last_bit;
								if(!res)res = 8;
								buffer = (uint8_t)(*(in++) >> (8 - res));
								for (buffer_length = res - 1; buffer_length >= 0; --buffer_length) {
									bool c = (buffer >> buffer_length) & 1;
									x = tr[x].son[c];
									if (tr[x].son[0] == USHORT_MAX) {
										if (!it.write(tr[x].son[1])) {
											return -1;
										}
										x = tot - 2;
									}
								}
								return (uint8_t*)it.get_ptr() - (uint8_t*)dest;
							}
							buffer = *(in++);
							for (buffer_length = 7; buffer_length >= 0; --buffer_length) {
								bool c = (buffer >> buffer_length) & 1;
								x = tr[x].son[c];
								if (tr[x].son[0] == USHORT_MAX) {
									if (!it.write(tr[x].son[1])) {
										return -1;
									}
									break;
								}
							}
						}
					}
				}
				buffer &= binary_mask[buffer_length];
			}
		}

		int res = 8 - last_bit;
		if(!res) res = 8;
		buffer = buffer << res | huffman_get_bit_8(in, l, res);
		buffer_length += res;

		int x = tot - 2;
		for (int i = buffer_length - 1; ~i; --i) {
			bool c = (buffer >> i) & 1;
			x = tr[x].son[c];
			if (tr[x].son[0] == USHORT_MAX) { //leafy
				if (!it.write(tr[x].son[1])) {
					return -1;
				}
				x = tot - 2;
			}
		}
		return (uint8_t*)it.get_ptr() - (uint8_t*)dest;
	}

}

#endif