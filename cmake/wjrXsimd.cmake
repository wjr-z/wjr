include(FetchContent)

# Reuse one reviewed version in the optional module and benchmark comparisons.
function(wjr_load_xsimd ENABLE_INSTALL)
    if(NOT TARGET xsimd)
        if(ENABLE_INSTALL)
            set(XSIMD_SKIP_INSTALL OFF)
        else()
            set(XSIMD_SKIP_INSTALL ON)
        endif()
        FetchContent_Declare(xsimd
            GIT_REPOSITORY https://github.com/xtensor-stack/xsimd.git
            GIT_TAG e88a72831858123924f7118f345dfe5d70d95991 # 14.3.0
            FIND_PACKAGE_ARGS 14.3.0 EXACT CONFIG
        )
        FetchContent_MakeAvailable(xsimd)
    endif()
endfunction()
