#if defined(WJR_DEBUG_LEVEL)
    #undef WJR_DEBUG_LEVEL
#endif
#define WJR_DEBUG_LEVEL 2

#include <cstdlib>
#include <exception>
#include <limits>
#include <string_view>

#include <wjr/memory/aligned_alloc.hpp>

namespace {
bool allocation_expected = false;
}

// Isolate failure injection from the unit-test executable and global operator new.
namespace wjr::mem {
template <>
void *_default_allocate<default_new_alignment>(size_t, std::nothrow_t) {
    if (!allocation_expected) {
        std::_Exit(EXIT_FAILURE);
    }
    return nullptr;
}
} // namespace wjr::mem

#include <wjr/memory/automatic_free_pool.hpp>

int main(int argc, char **argv) {
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    const std::string_view mode(argv[1]);
    if (mode != "overflow" && mode != "allocation") {
        return EXIT_FAILURE;
    }
    allocation_expected = mode == "allocation";
    std::set_terminate([] { std::_Exit(EXIT_SUCCESS); });
    wjr::automatic_free_pool pool;
    const auto size = allocation_expected ? size_t{64}
                                          : std::numeric_limits<size_t>::max() -
                                                wjr::automatic_free_pool::aligned_header_size + 1;
    (void)pool.allocate(size);
    return EXIT_FAILURE;
}
