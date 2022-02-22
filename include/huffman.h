#ifndef __WJR_HUFFMAN_H
#define __WJR_HUFFMAN_H

#include <memory>
#include <algorithm>
#include <string.h>
#include "mallocator.h"

namespace wjr {

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

	template<typename iter,size_t max_number = 256>
	size_t huffman_compress(iter src_first,iter src_last, void*dest_first ,void*dest_last) {

		constexpr static uint32_t max_bit = quick_log2(max_number - 1) + 1;
		constexpr static uint32_t max_bit_len = (max_bit - 1) / 8 + 1;
		size_t l2 = (uint8_t*)dest_last - (uint8_t*)dest_first;
		if (l2 < 12) {
			return -1;
		}

		void* ptr = static_thread_local_at_once_memory<
			sizeof(huffman_node)* max_number +
			(sizeof(huffman_tree_node) + sizeof(huffman_que_node)) * (2 * max_number - 1)>();

		void* mem_block = ptr;
		huffman_node* arr = (huffman_node*)mem_block;
		mem_block = (void*)((char*)(mem_block) + sizeof(huffman_node) * max_number);
		huffman_tree_node* tr = (huffman_tree_node*)mem_block;
		mem_block = (void*)((char*)(mem_block) + sizeof(huffman_tree_node) * (2 * max_number - 1));
		huffman_que_node* que = (huffman_que_node*)mem_block;

		uint8_t* out = (uint8_t*)dest_first;
		uint8_t* end = out + l2;

		for (int i = 0; i < max_number; ++i) {
			arr[i].ch = i;
			arr[i].cnt = 0;
		}

		size_t l1 = 0;
		for (iter i = src_first; i != src_last; ++i) {
			auto ch = *i;
			++arr[ch].cnt;
			++l1;
			assert(ch < max_number);
		}

		int tot = 0;
		for (int i = 0; i < max_number; ++i) {
			if (arr[i].cnt != 0) {
				arr[tot++] = arr[i];
			}
		}

		if (tot == 1) {
			if (l2 < 12 + max_bit_len) {
				return -1;
			}
			*(out++) = 0;
			*(uint16_t*)out = auto_bswap<uint16_t>(1);
			out += 2;
			*(out++) = max_bit_len;
			for (int i = 0; i < max_bit_len; ++i) {
				*(out++) = (uint8_t)(arr[0].ch >> (i << 3));
			}

			*(uint64_t*)out = auto_bswap<uint64_t>(l1);
			out += 8;
			return out - (uint8_t*)dest_first;
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
		*(uint16_t*)out = auto_bswap<uint16_t>(tot);
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

		for (iter i = src_first; i != src_last; ++i) {
			auto ch = *i;
			if (!buffer_append(arr[ch].cnt, arr[ch].ch)) {
				return -1;
			}
		}

		*(uint8_t*)dest_first = 0;
		if (buffer_bit != 8) {
			if (out == end) {
				return -1;
			}
			*(uint8_t*)dest_first = buffer_bit;
			++out;
		}
		return out - (uint8_t*)dest_first;
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

	template<typename iter,size_t max_number = 256>
	size_t huffman_uncompress(const void* src_first, const void* src_last, iter dest_first, iter dest_last) {
		size_t l1 = (uint8_t*)src_last - (uint8_t*)src_first;
		if (!l1) {
			return 0;
		}
		constexpr static int max_bit = quick_log2(max_number - 1) + 1;
		void* ptr = 
			static_thread_local_at_once_memory<
			sizeof(huffman_node)* max_number
			+ sizeof(huffman_tree_node) * (2 * max_number - 1)
			+ sizeof(huffman_decode_next) * 256>();
		void* mem_block = ptr;
		huffman_node* arr = (huffman_node*)mem_block;
		mem_block = (void*)((char*)(mem_block) + sizeof(huffman_node) * max_number);
		huffman_tree_node* tr = (huffman_tree_node*)mem_block;
		mem_block = (void*)((char*)(mem_block) + sizeof(huffman_tree_node) * (2 * max_number - 1));
		huffman_decode_next* dn = (huffman_decode_next*)mem_block;

		uint8_t* in = (uint8_t*)src_first;
		uint8_t* ib = in + l1 - 1;
		uint8_t last_bit = *(in++);
		int tot = 0;
		tot = auto_bswap<uint16_t>(*(uint16_t*)in);
		in += 2;
		assert(tot < max_number);
		if (tot == 1) {
			uint8_t len = *(in++);
			uint32_t ch = 0;
			for (int i = 0; i < len; ++i) {
				ch |= *(in++) << (i << 3);
			}
			size_t length = 0;
			length = auto_bswap<uint64_t>(*(uint64_t*)in);
			in += 8;
			if constexpr (is_any_of_v<iter, char*, uint8_t*>) {
				memset(dest_first, ch, length);
			}
			else {
				for (iter i = dest_first; i != dest_last; ++i) {
					*i = ch;
				}
			}
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

		auto buffer_write = [&dest_first, &dest_last](uint16_t ch)->bool {
			if (dest_first == dest_last) {
				return false;
			}
			*dest_first = ch;
			++dest_first;
			return true;
		};

		auto buffer_flush = [&buffer, &buffer_length,&tr,
			&buffer_write,tot](int x) {
			while (buffer_length--) {
				bool c = (buffer >> buffer_length) & 1;
				x = tr[x].son[c];
				if (tr[x].son[0] == USHORT_MAX) {
					if (!buffer_write(tr[x].son[1])) {
						return false;
					}
					x = tot - 2;
				}
			}
			return true;
		};

		iter c_dest_first(dest_first);
		if (in != ib) {
			buffer_length = l;
			buffer = huffman_get_bit_8(in, l, l);
			while (in != ib) {
				buffer = buffer << 8 | (*in++);
				buffer_length += 8;
				while (buffer_length >= 8) {
					uint8_t ch = (buffer >> (buffer_length - 8));
					if (dn[ch].l) {
						if (!buffer_write(dn[ch].next)) {
							return -1;
						}
						buffer_length -= dn[ch].l;
					}
					else {
						int x = dn[ch].next;
						buffer_length -= 8;
						bool fd = false;
						while (buffer_length--) {
							bool c = (buffer >> buffer_length) & 1;
							x = tr[x].son[c];
							if (tr[x].son[0] == USHORT_MAX) {
								if (!buffer_write(tr[x].son[1])) {
									return -1;
								}
								fd = true;
								break;
							}
						}

						for (; !fd;) {
							if (unlikely(in == ib)) {
								int res = 8 - last_bit;
								if (!res)res = 8;
								buffer = (uint8_t)(*in >> (8 - res));
								buffer_length = res;
								if (!buffer_flush(x)) {
									return -1;
								}
								return std::distance(c_dest_first,dest_first);
							}
							buffer = *(in++);
							buffer_length = 8;
							while (buffer_length--) {
								bool c = (buffer >> buffer_length) & 1;
								x = tr[x].son[c];
								if (tr[x].son[0] == USHORT_MAX) {
									if (!buffer_write(tr[x].son[1])) {
										return -1;
									}
									fd = true;
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
		if (!res) res = 8;
		buffer = buffer << res | (uint8_t)(*in >> (8 - res));
		buffer_length += res;
		if (!buffer_flush(tot - 2)) {
			return -1;
		}
		return std::distance(c_dest_first,dest_first);
	}

}

#endif