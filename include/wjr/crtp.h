#pragma once
#ifndef __WJR_CRTP_H
#define __WJR_CRTP_H

#include <wjr/type_traits.h>

_WJR_BEGIN

class nocopyable {
protected:
	nocopyable() {}
	~nocopyable() {}
private:
	nocopyable(const nocopyable&);
	const nocopyable& operator=(const nocopyable&);
};

class nomoveable {
protected:
	nomoveable() {}
	~nomoveable() {}
	nomoveable(const nomoveable&) = default;
	nomoveable& operator=(const nomoveable&) = default;
private:
	nomoveable(nomoveable&&);
	nomoveable& operator=(nomoveable&&);
};

class nocopy_and_moveable : public nocopyable, public nomoveable { };

// TODO :
// CRTP for iterator

_WJR_END

#endif // __WJR_CRTP_H