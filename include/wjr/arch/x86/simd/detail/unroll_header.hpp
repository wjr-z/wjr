#ifndef WJRX_ALLOW_OVERLAP
    #define WJRX_ALLOW_OVERLAP 0
#endif

#ifndef WJRX_MIN_COUNT
    #define WJRX_MIN_COUNT 0
#endif

#ifndef WJRX_START_COUNT
    #define WJRX_START_COUNT 0
#endif

#ifndef WJRX_COUNT
    #error "WJRX_COUNT must be defined"
#endif

#ifndef WJRX_SCALAR_I
    #error "WJRX_SCALAR_I must be defined"
#endif

#ifndef WJRX_UNROLL2_I
    #error "WJRX_UNROLL2_I must be defined"
#endif

#ifndef WJRX_UNROLL4_I
    #error "WJRX_UNROLL4_I must be defined"
#endif

#ifndef WJRX_INIT2
    #define WJRX_INIT2()
#endif

#ifndef WJRX_INIT4
    #define WJRX_INIT4()
#endif

#ifndef WJRX_RETURN_I
    #define WJRX_RETURN_I(I)
#endif

#ifndef WJRX_REVERSE
    #define WJRX_REVERSE 0
#endif

#if !WJRX_REVERSE
    #define WJRX_SCALAR(I) WJRX_SCALAR_I(I)
    #define WJRX_UNROLL2(I) WJRX_UNROLL2_I(I)
    #define WJRX_UNROLL4(I) WJRX_UNROLL4_I(I)
    #define WJRX_RETURN(I) WJRX_RETURN_I(I)
#else
    #define WJRX_SCALAR(I) WJRX_SCALAR_I((WJRX_COUNT - 1 - (I)))
    #define WJRX_UNROLL2(I) WJRX_UNROLL2_I((WJRX_COUNT - 2 - (I)))
    #define WJRX_UNROLL4(I) WJRX_UNROLL4_I((WJRX_COUNT - 4 - (I)))
    #define WJRX_RETURN(I) WJRX_RETURN_I((WJRX_COUNT - (I)))
#endif