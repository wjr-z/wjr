#ifndef WJR_CONCURRENCY_ATOMIC_HPP__
#define WJR_CONCURRENCY_ATOMIC_HPP__

#include <boost/atomic.hpp>

namespace wjr {

using boost::atomic;
using boost::memory_order;
using boost::memory_order_acq_rel;
using boost::memory_order_acquire;
using boost::memory_order_consume;
using boost::memory_order_relaxed;
using boost::memory_order_release;
using boost::memory_order_seq_cst;

} // namespace wjr

#endif // WJR_CONCURRENCY_ATOMIC_HPP__