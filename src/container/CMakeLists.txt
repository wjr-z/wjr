wjr_cc_library(
    NAME
    ring_buffer

    DEPS
    wjr::assert
    wjr::iterator
)

wjr_cc_library(
    NAME
    lru_cache

    DEPS
    wjr::assert
)