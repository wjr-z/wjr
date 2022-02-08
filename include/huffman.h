#ifndef __WJR_HUFFMAN_H
#define __WJR_HUFFMAN_H

#include "mySTL.h"

namespace wjr {

	struct node_info {
		uint8_t ch;
		unsigned long long cnt;
		bool operator<(const node_info& other)const {
			return cnt < other.cnt;
		}
	};

	extern void huffman_compress(node_info*arr,int tot);
	extern size_t huffman_compress(const void* src,size_t length,void * dest);
	extern size_t huffman_decompress(const void* src,size_t l1,void * dest,size_t l2);

}

#endif