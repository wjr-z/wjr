#if !(WJRX_MIN_COUNT == 3 || WJRX_MIN_COUNT == 5)
    #error "WJRX_MIN_COUNT must be 2, 4 or 8"
#endif

#if WJRX_START_COUNT != (WJRX_MIN_COUNT - 1)
    #error "WJRX_START_COUNT must be WJRX_MIN_COUNT - 1"
#endif

#if !WJRX_ALLOW_OVERLAP
    #error "WJRX_ALLOW_OVERLAP must be 1"
#endif

WJR_ASSUME(WJRX_COUNT >= WJRX_MIN_COUNT);

WJRX_INIT2();
#if WJRX_MIN_COUNT == 3
if (WJRX_COUNT <= 6) {
    if (WJR_UNLIKELY(WJRX_COUNT == 3)) {
        WJRX_SCALAR(2);
        return WJRX_RETURN(3);
    }

    WJRX_UNROLL2(2);
    if (WJRX_COUNT > 4) {
        WJRX_UNROLL2(WJRX_COUNT - 2);
    }

    return WJRX_RETURN(WJRX_COUNT);
}

WJRX_UNROLL2(WJRX_START_COUNT);
#endif

#if WJRX_MIN_COUNT == 5
if (WJRX_COUNT <= 6) {
    if (WJR_UNLIKELY(WJRX_COUNT == 5)) {
        WJRX_SCALAR(4);
        return WJRX_RETURN(5);
    }

    WJRX_UNROLL2(4);
    return WJRX_RETURN(6);
}
#endif

WJRX_INIT4();

const size_t rem4 = (WJRX_COUNT - 1) % 4 + 1;
WJR_ASSUME(rem4 != WJRX_COUNT);