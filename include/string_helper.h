#ifndef __WJR_STRING_HELPER_H
#define __WJR_STRING_HELPER_H

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string.h>
#include <iostream>
#include <vector>
#include "mString.h"

namespace wjr {

	// For some reasons, there are only simple double reads for the time being
	inline double power_of_10(int index);
	double read_double(const char*s,const char*e,const char*&next);

}

#endif
