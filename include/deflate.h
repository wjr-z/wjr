#ifndef __WJR_DEFLATE_H
#define __WJR_DEFLATE_H

#include "huffman.h"
#include "mString.h"

namespace wjr {

#ifndef DEFLATE_LEVEL
#define DEFLATE_LEVEL 6
#endif
	extern size_t deflate_compress(const void*src,size_t l1,void*dest,size_t l2,int level = DEFLATE_LEVEL);
	extern size_t deflate_decompress(const void*src,size_t l1,void*dest,size_t l2);
	extern String deflate_compress(String_view src, int level = DEFLATE_LEVEL);
	extern void deflate_compress(String_view src, String& dest, int level = DEFLATE_LEVEL);
	extern String deflate_decompress(String_view src);
	extern void deflate_decompress(String_view src, String& dest);
}

#endif