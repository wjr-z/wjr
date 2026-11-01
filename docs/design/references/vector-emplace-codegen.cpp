#include <wjr/vector.hpp>

#include <memory>
#include <utility>

/** @brief Compile this caller-only probe to assembly; no benchmark instrumentation is involved. */
extern "C" WJR_NOINLINE void back_int(wjr::vector<int> &value, int incoming) {
    value.emplace_back(incoming);
}

extern "C" WJR_NOINLINE void middle_int(wjr::vector<int> &value, unsigned position, int incoming) {
    value.emplace(value.cbegin() + position, incoming);
}

extern "C" WJR_NOINLINE void append_int(wjr::vector<int> &value, const int *first, unsigned count) {
    value.append(first, first + count);
}

extern "C" WJR_NOINLINE void default_append(wjr::vector<unsigned> &value, unsigned count) {
    value.append(count, wjr::default_construct);
}

extern "C" WJR_NOINLINE void back_owner(wjr::vector<std::unique_ptr<int>> &value,
                                        std::unique_ptr<int> &incoming) {
    value.emplace_back(std::move(incoming));
}

extern "C" WJR_NOINLINE void small_back(wjr::small_vector<int, 8> &value, int incoming) {
    value.emplace_back(incoming);
}

extern "C" WJR_NOINLINE void fill_insert(wjr::vector<int> &value, unsigned position, unsigned count,
                                         int incoming) {
    value.insert(value.cbegin() + position, count, incoming);
}

extern "C" WJR_NOINLINE void fill_replace(wjr::vector<int> &value, unsigned first, unsigned last,
                                          unsigned count, int incoming) {
    value.replace(value.cbegin() + first, value.cbegin() + last, count, incoming);
}

extern "C" WJR_NOINLINE void range_replace(wjr::vector<int> &value, unsigned first, unsigned last,
                                           const int *begin, const int *end) {
    value.replace(value.cbegin() + first, value.cbegin() + last, begin, end);
}
