#ifndef __WJR_UINT128_H
#define __WJR_UINT128_H

#include "mmacro.h"

namespace wjr::math {
	struct uint128_t {
		uint64_t hi;
		uint64_t lo;
	};
}

#endif