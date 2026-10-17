cmake_minimum_required(VERSION 3.20)
project(wjr_build_policy_tests LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../cmake")
set(WJR_ARCH x86)
include(wjrBuildOptions)

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
