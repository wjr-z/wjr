#ifndef TEST_H
#define TEST_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef uint64_t mp_limb_t;
typedef uint64_t* mp_ptr;
typedef const uint64_t* mp_srcptr;
typedef size_t mp_size_t;

 mp_limb_t mpn_add_n(mp_ptr rp, mp_srcptr up, mp_srcptr vp, mp_size_t n);

#ifdef __cplusplus
}
#endif


#endif // TEST_H
