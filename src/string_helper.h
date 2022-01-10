#ifndef STRING_HELPER_H
#define STRING_HELPER_H

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string.h>
#include <iostream>
#include <vector>
#include "String.h"

namespace wjr {

	inline double power_of_10(int index);
	double read_double(const char*s,const char*e,const char*&next);

}

#endif
