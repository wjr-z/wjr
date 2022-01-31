#include "../include/huffman.h"
#include <memory>
#include <algorithm>

namespace wjr {

	struct node_info { 
		uint8_t ch;
		unsigned long long cnt;
		bool operator<(const node_info& other)const {
			return cnt < other.cnt;
		}
	};

	struct tree_node {
		unsigned short son[2];
	};

	struct que_node {
		short id;
		uint8_t length; // length of code
		unsigned long long code;
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
		0x0f,
		0x1f,
		0x3f,
		0x7f,
		0xff
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

	void build_tree_step2(node_info*arr,tree_node*tr,que_node*que,int tot) {
		int que_head = 0, que_tail = 1;
		que[0].id = tot - 2;
		que[0].length = 0;
		que[0].code = 0;
		while (que_head < que_tail) {
			auto& x = que[que_head++];
			int id = x.id;
			assert(id < 511 && que_head <= 511 && que_tail <= 511);
			if (tr[id].son[0] == USHORT_MAX) {
				uint8_t ch = (uint8_t)tr[id].son[1];
				arr[ch].ch = x.length;
				arr[ch].cnt = x.code;
			}
			else {
				++x.length;
				que[que_tail].id = tr[id].son[0];
				que[que_tail].length = x.length;
				que[que_tail].code = x.code << 1;
				++que_tail;
				que[que_tail].id = tr[id].son[1];
				que[que_tail].length = x.length;
				que[que_tail].code = x.code << 1 | 1;
				++que_tail;
			}
		}
	}

	size_t encode(const void* src, void* dest,size_t length) {

		if (length <= 4) {
			return -1;
		}

		unique_mem mem(sizeof(node_info) * 256 + (sizeof(tree_node) + sizeof(que_node)) * 511);
		void* mem_block = mem.ptr;
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

		unsigned long long buffer = 0;
		int buffer_length = 0;

		auto buffer_append = 
			[&buffer,&buffer_length,&out,end](unsigned long long v,uint8_t l)->bool{
			buffer_length += l;
			buffer = buffer << l | v;
			if (buffer_length >= 8) {
				do {
					if(unlikely(out == end)){
						return false;
					}
					buffer_length -= 8;
					*(out++) = (uint8_t)((buffer >> buffer_length) & 0xFF);
				}while(buffer_length >= 8);
				buffer &= buffer_mask[buffer_length];
			}
			return true;
		};

		unsigned long long bit_number = 2;
		int bit_length = 1;
		
		// 8 bit : last_bit
		// 8 bit : how many char

		++out;
		*(out++) = tot & 0xFF;

		for (int i = 0; i < tot; ++i) {
			while (arr[i].cnt >= bit_number) {
				++bit_length;
				bit_number <<= 1;
			}
			if(!buffer_append(arr[i].ch,8))return -1;
			if(!buffer_append(bit_length,6))return -1;
			if(!buffer_append(arr[i].cnt,bit_length))return -1;
		}

		build_tree_step1(arr,tr,tot);
		build_tree_step2(arr,tr,que,tot);

		ar = in;
		for (size_t i = 0; i < length; ++i,++ar) {
			uint8_t ch = *ar;
			if (!buffer_append(arr[ch].cnt, arr[ch].ch)) {
				return -1;
			}
		}

		*(uint8_t*)dest = 0;
		if (buffer_length != 0) {
			if (out == end) {
				return -1;
			}
			*(uint8_t*)dest = 8 - buffer_length;
			*(out++) = buffer << (8 - buffer_length);
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
	
	size_t decode(const void* src, size_t l1, void* dest, size_t l2) {

		unique_mem mem(sizeof(node_info) * 256 + sizeof(tree_node) * 511 + sizeof(decode_next) * 256);
		void* mem_block = mem.ptr;
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
		for(int i = 0 ;i < tot;++i) {
			uint8_t ch = get_bit8(in,l,8);
			uint8_t bit_length = get_bit8(in,l,6);
			unsigned long long cnt = get_bit64(in,l,bit_length);
			arr[i].ch = ch;
			arr[i].cnt = cnt;
			res -= 14 + bit_length;
		}

		build_tree_step1(arr,tr,tot);
		dfs_init_decode_next(tr,tot-2,dn,7,0);
		
		auto dest_append = [&out,&end](uint8_t ch)->bool {
			if (out == end) {
				return false;
			}
			*(out++) = ch;
			return true;
		};
		
		uint8_t buffer = 0 ;
		int buffer_length = 0;

		while (res >= (8 - buffer_length)) {
			buffer = (buffer << (8 - buffer_length))
					 | get_bit8(in, l, 8 - buffer_length);
			res -= (8 - buffer_length);
			if (dn[buffer].l) {
				if (!dest_append((uint8_t)dn[buffer].next)) {
					return -1;
				}
				buffer_length = 8 - dn[buffer].l;
				buffer &= buffer_mask[buffer_length];
			}
			else {
				int x = dn[buffer].next;
				do {
					bool c = get_bit1(in, l);
					x = tr[x].son[c];
					--res;
				}while(tr[x].son[0] != USHORT_MAX);
				if (!dest_append((uint8_t)tr[x].son[1])) {
					return -1;
				}
				buffer = 0;
				buffer_length = 0;
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