#include "../include/huffman.h"
#include <memory>
#include <algorithm>

namespace wjr {

	struct tree_node {
		unsigned short son[2];
	};

	struct que_node {
		short id;
	};

	struct unique_mem {
		unique_mem(size_t l)
			: ptr(malloc(l)) {

		}
		~unique_mem() {
			free(ptr);
		}
		void* ptr;
	};

	unsigned long long buffer_mask[9] = {
		0x00,
		0x01,
		0x03,
		0x07,
		0x0F,
		0x1F,
		0x3F,
		0x7F,
		0xFF
	};

	void build_tree_step1(node_info* arr, tree_node* tr,int tot) {
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

	void huffman_compress(node_info* arr, int tot) {
		if (tot == 1) {
			uint8_t ch = arr[0].ch;
			arr[ch].ch = 1;
			arr[ch].cnt = 0;
			return ;
		}
		std::sort(arr, arr + tot);
		int Maxn = tot;
		for (int i = 0; i < tot; ++i) {
			if (arr[i].ch > Maxn) {
				Maxn = arr[i].ch;
			}
		}
		Maxn = 2 * Maxn - 1;
		unique_mem mem((sizeof(tree_node) + sizeof(que_node)) * Maxn);
		void*mem_block = mem.ptr;
		tree_node*tr = (tree_node*)mem_block;
		mem_block = (void*)((char*)(mem_block) + Maxn * sizeof(tree_node));
		que_node* que = (que_node*)mem_block;

		build_tree_step1(arr, tr, tot);
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
				uint8_t ch = (uint8_t)tr[id].son[1];
				arr[ch].ch = dep;
				arr[ch].cnt = code++;
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

	size_t huffman_compress(const void* src, size_t length,void* dest) {

		if (length <= 4) {
			return -1;
		}

		USE_THREAD_LOCAL static void* ptr = 
			malloc(sizeof(node_info) * 256 + (sizeof(tree_node) + sizeof(que_node)) * 511);
		void* mem_block = ptr;
		node_info* arr = (node_info*)mem_block;
		mem_block = (void*)((char*)(mem_block)+256 * sizeof(node_info));
		tree_node* tr = (tree_node*)mem_block;
		mem_block = (void*)((char*)(mem_block)+511 * sizeof(tree_node));
		que_node* que = (que_node*)mem_block;

		const uint8_t* in = (const uint8_t*)src;
		uint8_t* out = (uint8_t*)dest;
		uint8_t* end = out + length;

		for (int i = 0; i < 256; ++i) {
			arr[i].cnt = 0;
			arr[i].ch = i;
		}

		const uint8_t*ar = in;
		for (size_t i = 0; i < length; ++i,++ar) {
			++arr[*ar].cnt;
		}

		int tot = 0;
		for (int i = 0; i < 256; ++i) {
			if (arr[i].cnt != 0) {
				arr[tot++] = arr[i];
			}
		}

		if (tot == 1) {
			size_t l = length >> 3;
			size_t r = length & 7;
			*(out++) = r != 0 ? 8 - r : 0;
			*(out++) = 1;
			*(out++) = arr[0].ch;
			while (l--) {
				*(out++) = 0;
			}
			if (r != 0) {
				*(out++) = 0;
			}
			return out - (uint8_t*)dest;
		}

		std::sort(arr,arr + tot);

		int buffer_bit = 8;

		auto buffer_append = 
			[&buffer_bit,&out,end](unsigned long long v,uint8_t l)->bool{
			if (l >= buffer_bit) {
				l -= buffer_bit;
				*(out++) |= (uint8_t)((v >> l) & 0xFF);
				while (l >= 8) {
					l -= 8;
					*(out++) = (uint8_t)((v >> l) & 0xFF);
				}
				buffer_bit = 8 - l;
				*out = (uint8_t)((v & buffer_mask[l]) << buffer_bit);
			}
			else {
				buffer_bit -= l;
				*out |= v << buffer_bit;
			}
			return true;
		};

		// 8 bit : last_bit
		// 8 bit : how many char

		build_tree_step1(arr,tr,tot);

		*(++out) = tot & 0xFF;
		*(++out) = 0;

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
				assert(id < 511 && que_head <= 511 && que_tail <= 511);
				if (tr[id].son[0] == USHORT_MAX) {
					uint8_t ch = (uint8_t)tr[id].son[1];
					arr[ch].ch = dep;
					arr[ch].cnt = code++; 
					if (!buffer_append(ch,8)) {
						return -1;
					}
					if (!buffer_append(dep,6)) {
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

		ar = in;
		for (size_t i = 0; i < length; ++i,++ar) {
			uint8_t ch = *ar;
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

	uint8_t get_bit1(uint8_t*& in, int& l) {
		if (l != 1) {
			--l;
			return (*in >> l) & buffer_mask[1];
		}
		uint8_t r = (*in) & buffer_mask[1];
		++in;
		l = 8;
		return r;
	}

	uint8_t get_bit8(uint8_t*& in, int&l, int bit) {
		if (bit < l) {
			l -= bit;
			return (*in >> l) & buffer_mask[bit];
		}
		uint8_t r = (*in) & buffer_mask[l];
		bit -= l;
		++in;
		l = 8 - bit;
		return (r << bit) | ((*in >>l) & buffer_mask[bit]);
	}

	unsigned long long get_bit64(uint8_t*& in,int&l,int bit) {
		if (bit < l) {
			l -= bit;
			return (*in >> l) & buffer_mask[bit];
		}
		unsigned long long r = (*in) & buffer_mask[l];
		bit -= l;
		++in;
		while (bit >= 8) {
			r = r << 8 | *in;
			++in;
			bit -= 8;
		}
		l = 8 - bit;
		return r << bit | ((*in >> l)&buffer_mask[bit]);
	}

	struct decode_next {
		uint8_t next;
		uint8_t l;
	};

	unsigned short dfs_init_decode_tree(
		tree_node* tr,node_info*arr, unsigned short&X,int&now,int dep,int tot) {
		if (arr[now].ch == dep) {
			++now;
			return now + tot - 2;
		}
		unsigned short l = dfs_init_decode_tree(tr,arr,X,now,dep + 1,tot);
		unsigned short r = dfs_init_decode_tree(tr,arr,X,now,dep + 1,tot);
		tr[X].son[0] = l;
		tr[X].son[1] = r;
		return X++;
	}

	void dfs_init_decode_next(tree_node* tr,short x,decode_next*dn, int dep, uint8_t v) {
		if (tr[x].son[0] == USHORT_MAX) {
			for (int i = (1<<(dep+1))-1;~i; --i) {
				uint8_t id = v | i;
				dn[id].l = 7 - dep;
				dn[id].next = (uint8_t)tr[x].son[1];
			}
			return ;
		}
		if (dep == -1) {
			dn[v].l = 0;
			dn[v].next = x;
			return ;
		}
		dfs_init_decode_next(tr,tr[x].son[0],dn,dep-1,v);
		dfs_init_decode_next(tr,tr[x].son[1],dn,dep-1,v | (1<<dep));
	}
	
	size_t huffman_decompress(const void* src, size_t l1, void* dest, size_t l2) {
		USE_THREAD_LOCAL static void* ptr = 
			malloc(sizeof(node_info) * 256 + sizeof(tree_node) * 511 + sizeof(decode_next) * 256);
		void* mem_block = ptr;
		node_info* arr = (node_info*)mem_block;
		mem_block = (void*)((char*)(mem_block)+256 * sizeof(node_info));
		tree_node* tr = (tree_node*)mem_block;
		mem_block = (void*)((char*)(mem_block)+511 * sizeof(tree_node));
		decode_next* dn = (decode_next*)mem_block;

		uint8_t* in  = (uint8_t*)src;
		uint8_t* out = (uint8_t*)dest;
		uint8_t* ib = in + l1 - 1;
		uint8_t* end = out + l2;
		uint8_t last_bit = *(in++);
		int tot = *(in++);
		size_t res = (l1 - 2) * 8 - last_bit;
		if(!tot) tot = 256;
		if (tot == 1) {
			uint8_t ch = *(in++);
			res -= 8;
			if (l2 < res) {
				return -1;
			}
			while (res--) {
				*(out++) = ch;
			}
			return out - (uint8_t*)dest;
		}
		int l = 8;
		uint8_t dep = 0;
		unsigned long long code = 0;
		for(int i = 0 ;i < tot;++i) {
			uint8_t ch = get_bit8(in,l,8);
			uint8_t bit_length = get_bit8(in,l,6);
			code <<= (bit_length - dep);
			arr[i].ch = bit_length;
			arr[ch].cnt = code++;
			tr[i + tot - 1].son[0] = -1;
			tr[i + tot - 1].son[1] = ch;
			dep = bit_length;
		}
		unsigned short X = 0;
		int Y = 0;
		dfs_init_decode_tree(tr,arr,X,Y,0,tot);
		dfs_init_decode_next(tr,tot-2,dn,7,0);
		
		auto dest_append = [&out,&end](uint8_t ch)->bool {
			if (out == end) {
				return false;
			}
			*(out++) = ch;
			return true;
		};
		
		unsigned long long buffer = 0;
		int buffer_length = 0;

		if (in != ib) {
			buffer_length = l;
			buffer = get_bit8(in,l,l);
			while (in != ib) {
				buffer = buffer << 8 | (*in++);
				buffer_length += 8;
				while (buffer_length >= 8) {
					uint8_t ch = (buffer >> (buffer_length - 8)) & 0xFF;
					if (dn[ch].l) {
						if (!dest_append(dn[ch].next)) {
							return -1;
						}
						buffer_length -= dn[ch].l;
					}
					else {
						int x = dn[ch].next;
						buffer_length -= 8 ;
						while (buffer_length--) {
							bool c = (buffer >> buffer_length) & 1;
							x = tr[x].son[c];
							if (tr[x].son[0] == USHORT_MAX) {
								if (!dest_append((uint8_t)tr[x].son[1])) {
									return -1;
								}
								break;
							}
						}
						for (;buffer_length == -1;) {
							if (unlikely(in == ib)) {
								buffer = (uint8_t)(*(in++) >> (8 - last_bit));
								for (buffer_length = last_bit - 1; buffer_length >= 0; --buffer_length) {
									bool c = (buffer >> buffer_length) & 1;
									x = tr[x].son[c];
									if (tr[x].son[0] == USHORT_MAX) {
										if (!dest_append((uint8_t)tr[x].son[1])) {
											return -1;
										}
										break;
									}
								}
								return out - (uint8_t*)dest;
							}
							buffer = *(in++);
							for (buffer_length = 7; buffer_length >= 0; --buffer_length) {
								bool c = (buffer >> buffer_length) & 1;
								x = tr[x].son[c];
								if (tr[x].son[0] == USHORT_MAX) {
									if (!dest_append((uint8_t)tr[x].son[1])) {
										return -1;
									}
									break;
								}
							}
						}
					}
				}
				buffer &= buffer_mask[buffer_length];
			}
		}

		if (res) {
			buffer = buffer << res | get_bit8(in, l, res);
			buffer_length += res;
		}

		int x = tot - 2;
		for (int i = buffer_length - 1; ~i; --i) {
			bool c = (buffer >> i) & 1;
			x = tr[x].son[c];
			if (tr[x].son[0] == USHORT_MAX) { //leafy
				if (!dest_append((uint8_t)tr[x].son[1])) {
					return -1;
				}
				x = tot - 2;
			}
		}
		return out - (uint8_t*)dest;
	}

}