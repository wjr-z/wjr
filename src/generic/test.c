#include "test.h"

mp_limb_t
mpn_add_n(mp_ptr rp, mp_srcptr up, mp_srcptr vp, mp_size_t n)
{
    mp_limb_t cy[n];
    mp_limb_t a; mp_limb_t b; mp_limb_t r; mp_limb_t s0; mp_limb_t c0; mp_limb_t c1;
    mp_size_t i;
    int more_carries;

    /* Main add loop.  Generate a raw output sum in rp[] and a carry vector
       in cy[].  */
#pragma _CRI ivdep
    for (i = 0; i < n; i++)
    {
        a = up[i];
        b = vp[i];
        s0 = a + b;
        rp[i] = s0;
        c0 = ((a & b) | ((a | b) & ~s0)) >> 63;
        cy[i] = c0;
    }
    /* Carry add loop.  Add the carry vector cy[] to the raw sum rp[] and
       store the new sum back to rp[0].  If this generates further carry, set
       more_carries.  */
    more_carries = 0;
#pragma _CRI ivdep
    for (i = 1; i < n; i++)
    {
        r = rp[i];
        c0 = cy[i - 1];
        s0 = r + c0;
        rp[i] = s0;
        c0 = (r & ~s0) >> 63;
        more_carries += c0;
    }
    /* If that second loop generated carry, handle that in scalar loop.  */
    if (more_carries)
    {
        mp_limb_t cyrec = 0;
        /* Look for places where rp[k] is zero and cy[k-1] is non-zero.
       These are where we got a recurrency carry.  */
        for (i = 1; i < n; i++)
        {
            r = rp[i];
            c0 = (r == 0 && cy[i - 1] != 0);
            s0 = r + cyrec;
            rp[i] = s0;
            c1 = (r & ~s0) >> 63;
            cyrec = c0 | c1;
        }
        return cyrec | cy[n - 1];
    }

    return cy[n - 1];
}
