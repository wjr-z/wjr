cmake_minimum_required(VERSION 3.20)
project(wjr_build_policy_tests LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../cmake")
set(WJR_ARCH x86)
include(wjrBuildOptions)

if(WJR_MINIMAL_ASSERT AND NOT "WJR_MINIMAL_ASSERT" IN_LIST WJR_COMPILE_DEFINITIONS)
    message(FATAL_ERROR "Minimal assertion mode must propagate through the public config.")
endif()

if(DEFINED EXPECT_STANDARD AND NOT WJR_CXX_STANDARD STREQUAL EXPECT_STANDARD)
    message(FATAL_ERROR "Expected C++${EXPECT_STANDARD}, got C++${WJR_CXX_STANDARD}.")
endif()

# A supported probe must not make a different unsupported flag appear supported.
if(_WJR_MSVC_FRONTEND)
    set(SUPPORTED_FLAG /W4)
    set(UNSUPPORTED_FLAG /std:wjr-invalid-standard)
else()
    set(SUPPORTED_FLAG -Wall)
    set(UNSUPPORTED_FLAG -fwjr-invalid-option)
endif()
set(PROBED_FLAGS)
AddFlagsIfExists(PROBED_FLAGS "${SUPPORTED_FLAG} ${UNSUPPORTED_FLAG}")
if(NOT SUPPORTED_FLAG IN_LIST PROBED_FLAGS OR UNSUPPORTED_FLAG IN_LIST PROBED_FLAGS)
    message(FATAL_ERROR "Compiler flag probes reused the wrong cached result: ${PROBED_FLAGS}")
endif()

if(WJR_CPU_TARGET STREQUAL "baseline" AND NOT _WJR_MSVC_FRONTEND AND
   NOT "-march=x86-64" IN_LIST WJR_ISA_FLAGS)
    message(FATAL_ERROR "The x86 baseline must select an explicit baseline ISA.")
endif()

# Direct header users must get the same minimum-language diagnostic as CMake users.
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/header-standard.cpp"
    "#include <wjr/config/compiler.hpp>\nint main() {}\n")
try_compile(WJR_HEADER_ACCEPTS_CXX17
    "${CMAKE_CURRENT_BINARY_DIR}/header-cxx17"
    SOURCES "${CMAKE_CURRENT_BINARY_DIR}/header-standard.cpp"
    CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${CMAKE_CURRENT_LIST_DIR}/../../include"
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF
    OUTPUT_VARIABLE WJR_HEADER_CXX17_OUTPUT
)
if(WJR_HEADER_ACCEPTS_CXX17 OR NOT WJR_HEADER_CXX17_OUTPUT MATCHES "WJR requires C[+][+]20 or later")
    message(FATAL_ERROR "The public header must reject C++17 with the minimum-language diagnostic:\n${WJR_HEADER_CXX17_OUTPUT}")
endif()
try_compile(WJR_HEADER_ACCEPTS_STANDARD
    "${CMAKE_CURRENT_BINARY_DIR}/header-cxx${WJR_CXX_STANDARD}"
    SOURCES "${CMAKE_CURRENT_BINARY_DIR}/header-standard.cpp"
    CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${CMAKE_CURRENT_LIST_DIR}/../../include"
    CXX_STANDARD ${WJR_CXX_STANDARD} CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF
    OUTPUT_VARIABLE WJR_HEADER_STANDARD_OUTPUT
)
if(NOT WJR_HEADER_ACCEPTS_STANDARD)
    message(FATAL_ERROR "The public header must accept C++${WJR_CXX_STANDARD}:\n${WJR_HEADER_STANDARD_OUTPUT}")
endif()
