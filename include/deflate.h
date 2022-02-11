#ifndef __WJR_DEFLATE_H
#define __WJR_DEFLATE_H

#include "huffman.h"

namespace wjr {

#define DEFLATE_LEVEL 6

	extern size_t deflate_compress(void*src,size_t l1,void*dest,size_t l2,int level = DEFLATE_LEVEL);
	extern size_t deflate_decompress(void*src,size_t l1,void*dest,size_t l2);
}

#endif