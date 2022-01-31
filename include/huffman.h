#ifndef WJR_HUFFMAN_H
#define WJR_HUFFMAN_H

#include "mySTL.h"

namespace wjr {

	extern size_t encode(const void* src,void * dest,size_t length);
	extern size_t decode(const void* src,size_t l1,void * dest,size_t l2);

}

#endif