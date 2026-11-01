include(FetchContent)

if(NOT TARGET CpuFeatures::cpu_features)
    # Preserve explicit source choices and the provider-scope fixtures.
    if(NOT FETCHCONTENT_SOURCE_DIR_CPU_FEATURES AND
       NOT FETCHCONTENT_TRY_FIND_PACKAGE_MODE STREQUAL "NEVER")
        find_package(CpuFeatures 0.10.1 EXACT QUIET CONFIG)
        if(TARGET CpuFeatures::cpu_features)
            # The official package imports this target in the current directory.
            # Keep it visible to embedding projects, like the source-built alias.
            set_property(TARGET CpuFeatures::cpu_features PROPERTY IMPORTED_GLOBAL TRUE)
        endif()
    endif()

    if(NOT TARGET CpuFeatures::cpu_features)
        # Package selection above already enforced the public X86Info ABI version.
        set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE NEVER)
        FetchContent_Declare(cpu_features
            GIT_REPOSITORY https://github.com/google/cpu_features.git
            GIT_TAG v0.10.1
            GIT_SHALLOW ON
        )

        # Disable provider tests here without changing the embedding project.
        set(BUILD_TESTING OFF)
        FetchContent_MakeAvailable(cpu_features)
    endif()
endif()

wjr_cc_library(
    NAME
    arch-cpuinfo

    SRCS
    cpuinfo.cpp

    DEPS
    CpuFeatures::cpu_features

    OBJECT
)

set(WJR_SRCS)
wjr_glob_cpp_source(WJR_SRCS arch/x86/math)

wjr_cc_library(
    NAME
    arch-math

    ARCH_DEPENDENT

    SRCS
    ${WJR_SRCS}

    DEPS
    wjr::assert
    wjr::arch-cpuinfo
    wjr::iterator
)

set(WJR_SRCS)
wjr_glob_cpp_source(WJR_SRCS arch/x86/format)

wjr_cc_library(
    NAME
    arch-format

    ARCH_DEPENDENT

    SRCS
    ${WJR_SRCS}

    DEPS
    wjr::assert
    wjr::iterator

    OBJECT
)

set(WJR_SRCS)
wjr_glob_cpp_source(WJR_SRCS arch/x86/json)

wjr_cc_library(
    NAME
    arch-json

    ARCH_DEPENDENT

    SRCS
    ${WJR_SRCS}

    DEPS
    wjr::assert
    wjr::iterator

    OBJECT
)

add_subdirectory(biginteger)