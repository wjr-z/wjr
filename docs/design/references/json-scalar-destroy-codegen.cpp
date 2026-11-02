#include <cstdint>
#include <memory>
#include <memory_resource>
#include <new>
#include <string>
#include <wjr/json/pmr.hpp>

using doc = wjr::json::document;
using pmr_doc = wjr::json::pmr_document;
extern "C" WJR_NOINLINE void generic_signed(doc *out, int64_t value) { ::new (out) doc(value); }
extern "C" WJR_NOINLINE void tagged_signed(doc *out, int64_t value) {
    ::new (out) doc(wjr::json::number_signed_t{}, value);
}
extern "C" WJR_NOINLINE void generic_bool(doc *out, bool value) { ::new (out) doc(value); }
extern "C" WJR_NOINLINE void tagged_bool(doc *out, bool value) {
    ::new (out) doc(wjr::json::boolean_t{}, value);
}
extern "C" WJR_NOINLINE void generic_double(doc *out, double value) { ::new (out) doc(value); }
extern "C" WJR_NOINLINE void tagged_double(doc *out, double value) {
    ::new (out) doc(wjr::json::number_float_t{}, value);
}
extern "C" WJR_NOINLINE void destroy(doc *first, doc *last) {
    for (; first != last; ++first)
        std::destroy_at(first);
}
extern "C" WJR_NOINLINE void reset(doc *value) { value->reset(); }
extern "C" WJR_NOINLINE void pmr_reset(pmr_doc *value) { value->reset(); }

extern "C" WJR_NOINLINE void generic_unsigned(doc *out, uint64_t value) { ::new (out) doc(value); }
extern "C" WJR_NOINLINE void tagged_unsigned(doc *out, uint64_t value) {
    ::new (out) doc(wjr::json::number_unsigned_t{}, value);
}
extern "C" WJR_NOINLINE void generic_null(doc *out) { ::new (out) doc(nullptr); }
extern "C" WJR_NOINLINE void tagged_null(doc *out) { ::new (out) doc(wjr::json::null_t{}); }
extern "C" WJR_NOINLINE void pmr_signed(pmr_doc *out, std::pmr::memory_resource *resource,
                                        int64_t value) {
    ::new (out) pmr_doc(std::allocator_arg, resource, value);
}
extern "C" WJR_NOINLINE void pmr_tagged(pmr_doc *out, std::pmr::memory_resource *resource,
                                        int64_t value) {
    ::new (out) pmr_doc(std::allocator_arg, resource, wjr::json::number_signed_t{}, value);
}
