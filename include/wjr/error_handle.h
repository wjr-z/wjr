#pragma once
#ifndef __WJR_ERROR_HANDLE_H
#define __WJR_ERROR_HANDLE_H

namespace wjr {
	// TODO :

	enum class errc {
		ok               = 0x00,
		overflow         = 0x01,
		underflow        = 0x02,
		out_of_range     = overflow | underflow,
		empty			 = 0x04,
		noconv           = 0x05,
		buffer_too_small = 0x06,
	};

}

#endif // __WJR_ERROR_HANDLE_H