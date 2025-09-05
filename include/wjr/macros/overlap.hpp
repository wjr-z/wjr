#ifndef WJR_MACROS_OVERLAP_HPP__
#define WJR_MACROS_OVERLAP_HPP__

#define WJR_IS_OVERLAP_P(p, pn, q, qn) ((p) + (pn) > (q) && (q) + (qn) > (p))
#define WJR_IS_SEPARATE_P(p, pn, q, qn) (!WJR_IS_OVERLAP_P(p, pn, q, qn))
#define WJR_IS_SAME_OR_SEPARATE_P(p, pn, q, qn) (((p) == (q)) || WJR_IS_SEPARATE_P(p, pn, q, qn))
#define WJR_IS_SAME_OR_INCR_P(p, pn, q, qn) (((p) <= (q)) || ((q) + (qn) <= (p)))
#define WJR_IS_SAME_OR_DECR_P(p, pn, q, qn) (((p) >= (q)) || ((p) + (pn) <= (q)))

#endif // WJR_MACROS_OVERLAP_HPP__