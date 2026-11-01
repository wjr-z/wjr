# Automatic discovery must not leak an ABI-incompatible imported target.
set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL FALSE)
find_package(GTest 1.17.0 EXACT QUIET CONFIG)
if(NOT GTest_FOUND OR NOT TARGET GTest::gtest OR NOT TARGET GTest::gtest_main)
    return()
endif()

# In particular, reject a Windows CRT or standard-library ABI mismatch before
# committing to the prebuilt provider. Compile/link only; cross builds may reuse it.
set(_wjr_gtest_probe_flags "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
if(CMAKE_MSVC_RUNTIME_LIBRARY)
    list(APPEND _wjr_gtest_probe_flags
        "-DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}")
endif()
set(CMAKE_TRY_COMPILE_CONFIGURATION "${CMAKE_BUILD_TYPE}")
set(CMAKE_TRY_COMPILE_TARGET_TYPE EXECUTABLE)
try_compile(_wjr_gtest_compatible
    "${CMAKE_CURRENT_BINARY_DIR}/check"
    "${CMAKE_CURRENT_SOURCE_DIR}/probe.cpp"
    CMAKE_FLAGS ${_wjr_gtest_probe_flags}
    LINK_LIBRARIES GTest::gtest_main
    OUTPUT_VARIABLE _wjr_gtest_probe_output
)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/probe.log" "${_wjr_gtest_probe_output}")
if(NOT _wjr_gtest_compatible)
    message(STATUS "Installed GoogleTest failed the compile/link check; using source")
    return()
endif()

# Promote the complete official export set, including optional GoogleMock targets.
foreach(_wjr_gtest_target GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
    if(TARGET ${_wjr_gtest_target})
        set_property(TARGET ${_wjr_gtest_target} PROPERTY IMPORTED_GLOBAL TRUE)
    endif()
endforeach()
