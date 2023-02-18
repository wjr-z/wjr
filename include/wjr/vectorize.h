#pragma once
#ifndef __WJR_VECTORIZE_H__
#define __WJR_VECTORIZE_H__

#include <wjr/type_traits.h>

_WJR_BEGIN

enum class vectorize_t : uint32_t{
	Null,
	// non-constant may eliminate some branches to optimize the overall efficiency
	// don't use this
	IsConstant = 0x01,
	// can overlap such as min, max, set, cmp ...
	CanOverlap = 0x02,
	// let SmallThreshold = S, Vectorized length = I, I0 = I, I1 = I * 4
	// default : I0 < 256, S >= I
	// when n is in [0, S)  , use small func
	// when n is in [I0, I1) , use mid func
	// when n is in [I1, ...), use large func
	// speed :
	// small func : sv, mid func : mv, large func : lv
	// default : 
	// lv <= mv <= sv
	// lv >= mv / (I1 / I0)
	// mv >= sv / I0
	// f(n) is the time required for small func to execute length n
	// g(n) is the time required for mid func to execute n times, 
	// because mid func executes once with length I0, so the total execution length is n * I0
	// h(n) is the time required for large func to execute n times, 
	// because large func executes once with length I1, so the total execution length is n * I1

	// let SmallMidSpeed  = p0
	// then MidSmallRatio = sv / mv >= I0 - p0
	// then g(1) >= f(I0 - p0)
	// execute I0 - P0 times small func as possible as 
	// replace with execute one time mid func
	SmallMidSpeedMask		= 0x000003FC,	
	// let MidLargeSpeed  = p1
	// then LargeMidRatio = lv / mv >= (I1 / I0) - p1
	// then h(1) >= g(I1 / I0 - p1)
	// execute (I1 / I0) - P1 times mid func as possible as
	// replace with execute one time large func
	MidLargeSpeedMask		= 0x00000C00,
	SmallThresholdMask		= 0x000FF000,
	VectorizedLengthMask	= 0x0FF00000,
};

constexpr vectorize_t GetSmallMidSpeed(int k) {
	using namespace enum_ops;
	return static_cast<vectorize_t>(k << 2) & vectorize_t::SmallMidSpeedMask;
}

constexpr vectorize_t GetMidLargeSpeed(int k) {
	using namespace enum_ops;
	return static_cast<vectorize_t>(k << 10) & vectorize_t::MidLargeSpeedMask;
}

constexpr int GetSmallMidSpeed(vectorize_t v) {
	using namespace enum_ops;
	return enum_cast(v & vectorize_t::SmallMidSpeedMask) >> 2;
}

constexpr int GetMidLargeSpeed(vectorize_t v) {
	using namespace enum_ops;
	return enum_cast(v & vectorize_t::MidLargeSpeedMask) >> 10;
}

constexpr vectorize_t GetSmallThreshold(int k) {
	using namespace enum_ops;
	return static_cast<vectorize_t>(k << 12) & vectorize_t::SmallThresholdMask;
}

constexpr int GetSmallThreshold(vectorize_t v) {
	using namespace enum_ops;
	return enum_cast(v & vectorize_t::SmallThresholdMask) >> 12;
}

constexpr vectorize_t GetVectorizedLength(int k) {
	using namespace enum_ops;
	return static_cast<vectorize_t>(k << 20) & vectorize_t::VectorizedLengthMask;
}

constexpr int GetVectorizedLength(vectorize_t v) {
	using namespace enum_ops;
	return enum_cast(v & vectorize_t::VectorizedLengthMask) >> 20;
}

#define WJR_AUTO_VECTORIZE(tag, n, sMacro, mMacro, lMacro, sInitBegin,      \
                           sInitEnd, mInitBegin, mInitEnd, ...)             \
    {                                                                       \
        using namespace enum_ops;                                           \
        constexpr size_t _S = GetSmallThreshold(tag);                       \
        constexpr size_t _I0 = GetVectorizedLength(tag);                    \
        static_assert(_I0 > 1, "SmallThreshold must be greater than 1");    \
        static_assert(_I0 < 256, "SmallThreshold must be less than 256");   \
        static_assert(_S >= _I0,                                            \
                      "SmallThreshold must be greater than or equal to "    \
                      "VectorizedLength");                                  \
        static_assert(                                                      \
            _S % _I0 == 0,                                                  \
            "SmallThreshold must be a multiple of VectorizedLength");       \
        constexpr size_t _I1 = 4 * _I0;                                     \
        constexpr size_t _SI = _S / _I0;                                    \
        constexpr bool _CanOverlap =                                        \
            static_cast<bool>(enum_cast(tag & vectorize_t::CanOverlap));    \
        constexpr auto SmallMidRatio = _I0 - GetSmallMidSpeed(tag);         \
        constexpr auto MidLargeRatio = (_I1 / _I0) - GetMidLargeSpeed(tag); \
        size_t _N = n;                                                      \
        size_t _i = 0;                                                      \
        WJR_EXPAND(sInitBegin(__VA_ARGS__));                                \
        if (_N >= _S) {                                                     \
            WJR_EXPAND(mInitBegin(__VA_ARGS__));                            \
            int _ks = 0;                                                    \
            if (_N >= _I1) {                                                \
                constexpr auto _mask = _I1 - 1;                             \
                auto _j = _N & (~_mask);                                    \
                _N &= _mask;                                                \
                /* don't unroll this loop*/                                 \
                _Pragma("unroll(1)")                                        \
                do {                                                        \
                    WJR_EXPAND(lMacro(__VA_ARGS__, _i));                    \
                    _i += _I1;                                              \
                } while (_i != _j);                                         \
                if constexpr (_CanOverlap) {                                \
                    if (!_N) goto WJR_MACRO_LABEL(mid_end);                 \
                    auto _t = (_N + _I0 - 1) / _I0;                         \
                    _ks = _t >= MidLargeRatio ? 5 : _t;                     \
                }                                                           \
            } else {                                                        \
                if constexpr (_CanOverlap) {                                \
                    if (!_N) goto WJR_MACRO_LABEL(mid_end);                 \
                    auto _t = (_N + _I0 - 1) / _I0;                         \
                    _ks = _t;                                               \
                }                                                           \
            }                                                               \
            if constexpr (_CanOverlap) {                                    \
                _N += _i;                                                   \
                switch (_ks) {                                              \
                    default:                                                \
                        unreachable();                                      \
                        break;                                              \
                    case 5:                                                 \
                        WJR_EXPAND(lMacro(__VA_ARGS__, (_N - _I1)));        \
                        break;                                              \
                    case 4:                                                 \
                        WJR_EXPAND(mMacro(__VA_ARGS__, _i));                \
                        _i += _I0;                                          \
                    case 3:                                                 \
                        WJR_EXPAND(mMacro(__VA_ARGS__, _i));                \
                        _i += _I0;                                          \
                    case 2:                                                 \
                        WJR_EXPAND(mMacro(__VA_ARGS__, _i));                \
                        _i += _I0;                                          \
                    case 1:                                                 \
                        WJR_EXPAND(mMacro(__VA_ARGS__, (_N - _I0)));        \
                }                                                           \
                _i = _N;                                                    \
            } else {                                                        \
                if(_N){                                                     \
                    auto _t = _N / _I0;                                     \
                    _N &= _I0 - 1;                                          \
                    switch (_t) {                                           \
                        default: unreachable(); break;                      \
                        case 3:                                             \
                            WJR_EXPAND(mMacro(__VA_ARGS__, _i));            \
                            _i += _I0;                                      \
                        case 2:                                             \
                            WJR_EXPAND(mMacro(__VA_ARGS__, _i));            \
                            _i += _I0;                                      \
                        case 1:                                             \
                            WJR_EXPAND(mMacro(__VA_ARGS__, _i));            \
                            _i += _I0;                                      \
                    }                                                       \
                }                                                           \
            }                                                               \
            WJR_MACRO_LABEL(mid_end) : WJR_EXPAND(mInitEnd(__VA_ARGS__));   \
        } else {                                                            \
            if constexpr (_CanOverlap) {                                    \
                assume(_N < _S);                                            \
                WJR_EXPAND(sMacro(__VA_ARGS__, _i, _N));                    \
                _i += _N;                                                   \
            }                                                               \
        }                                                                   \
        if constexpr (!_CanOverlap) {                                       \
            assume(_N < _S);                                                \
            WJR_EXPAND(sMacro(__VA_ARGS__, _i, _N));                        \
            _i += _N;                                                       \
        }                                                                   \
        WJR_EXPAND(sInitEnd(__VA_ARGS__, _i));                              \
    }

_WJR_END

#endif // __WJR_VECTORIZE_H__