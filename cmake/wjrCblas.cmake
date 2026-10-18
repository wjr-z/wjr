include_guard(GLOBAL)

include(CMakePushCheckState)
include(CheckCXXSourceRuns)

# Both build-tree and installed components use the same native ABI check.
function(wjr_find_openblas OUT_TARGET)
    set(${OUT_TARGET} "" PARENT_SCOPE)
    set(WJR_CBLAS_REASON "OpenBLAS LP64 with CBLAS is required." PARENT_SCOPE)
    if(CMAKE_CROSSCOMPILING)
        set(WJR_CBLAS_REASON "The initial CBLAS module requires a native ABI probe." PARENT_SCOPE)
        return()
    endif()
    find_package(PkgConfig QUIET)
    if(NOT PkgConfig_FOUND)
        set(WJR_CBLAS_REASON "The CBLAS module requires pkg-config and OpenBLAS development files." PARENT_SCOPE)
        return()
    endif()

    # FindPkgConfig caches by prefix. Search-path changes must select a fresh
    # prefix as well as rerun the header/library ABI probe.
    string(JOIN "|" search "${PKG_CONFIG_EXECUTABLE}" "${PKG_CONFIG_ARGN}"
        "$ENV{PKG_CONFIG_PATH}" "$ENV{PKG_CONFIG_LIBDIR}" "$ENV{PKG_CONFIG_SYSROOT_DIR}"
        "${CMAKE_PREFIX_PATH}" "$ENV{CMAKE_PREFIX_PATH}" "${PKG_CONFIG_USE_CMAKE_PREFIX_PATH}")
    string(SHA256 key "${search}")
    string(SUBSTRING "${key}" 0 16 key)
    set(prefix "WJR_OPENBLAS_${key}")
    pkg_check_modules(${prefix} QUIET IMPORTED_TARGET GLOBAL openblas)
    if(NOT TARGET PkgConfig::${prefix})
        return()
    endif()

    cmake_push_check_state(RESET)
    set(CMAKE_REQUIRED_LIBRARIES PkgConfig::${prefix})
    set(CMAKE_REQUIRED_QUIET TRUE)
    unset(WJR_CBLAS_LP64 CACHE)
    unset(WJR_CBLAS_LP64)
    check_cxx_source_runs([=[
#include <cblas.h>
#include <cstring>
#include <type_traits>
static_assert(std::is_same<blasint, int>::value && sizeof(blasint) == 4,
              "OpenBLAS LP64 requires 32-bit int arguments");
int main() {
    const char *config = openblas_get_config();
    if (!config || std::strstr(config, "USE64BITINT")) return 1;
    const float x[]{2, 3};
    const double y[]{2, 3};
    if (cblas_sdot(2, x, 1, x, 1) != 13 ||
        cblas_ddot(2, y, 1, y, 1) != 13) return 2;
    float f;
    double d;
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                1, 1, 1, 1, x, 1, x, 1, 0, &f, 1);
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                1, 1, 1, 1, y, 1, y, 1, 0, &d, 1);
    return f == 4 && d == 4 ? 0 : 3;
}
]=] WJR_CBLAS_LP64)
    cmake_pop_check_state()
    if(NOT WJR_CBLAS_LP64)
        set(WJR_CBLAS_REASON "The OpenBLAS C++ header/library LP64 ABI probe failed." PARENT_SCOPE)
        return()
    endif()
    set(${OUT_TARGET} PkgConfig::${prefix} PARENT_SCOPE)
endfunction()
