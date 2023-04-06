#pragma once
#ifndef __WJR_ERROR_HANDLE_H
#define __WJR_ERROR_HANDLE_H

namespace wjr {
	enum class error_code {
		ok = 0,
		overflow,
		underflow,
		noconv,
	};
}

#endif // __WJR_ERROR_HANDLE_H