include(FetchContent)

set(WJR_USE_GMP_DEFAULT ON)

if(WJR_USE_GMP)
   set(WJR_USE_GMP_DEFAULT ${WJR_USE_GMP})
else()
   find_package(GMP QUIET)

   if(NOT GMP_FOUND)
      find_package(PkgConfig)
      pkg_check_modules(GMP QUIET gmp)

      if(NOT GMP_FOUND)
         set(WJR_USE_GMP_DEFAULT OFF)
      else()
         set(GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIRS})
         set(GMP_LIBRARIES ${GMP_LIBRARIES})
      endif()
   else()
      set(GMP_INCLUDE_DIRS)
      set(GMP_LIBRARIES gmp libgmp)
   endif()
endif()

option(WJR_USE_GMP_TESTS "Use GMP library for tests" ${WJR_USE_GMP_DEFAULT})

if(WJR_USE_GMP_TESTS)
   add_compile_definitions(WJR_USE_GMP)
else()
   message(STATUS "GMP library not found, tests will be built without GMP support.")
endif()

if(WJR_ENABLE_ASAN AND WIN32)
   # ASAN runtime uses static CRT (/MT); gtest must match.
   set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
else()
   set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)

if(EXISTS googletest)
   add_subdirectory(googletest)
else()
   FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG v1.17.0
   )

   FetchContent_MakeAvailable(googletest)
endif()

if(WJR_ENABLE_ASAN)
   # gtest must be compiled with -fsanitize=address so annotate_string matches wjr libs.
   foreach(_gtest_target gtest gtest_main)
      if(TARGET ${_gtest_target})
         target_compile_options(${_gtest_target} PRIVATE -fsanitize=address)
      endif()
   endforeach()
endif()

file(GLOB_RECURSE TEST_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
# Installed-package consumers have their own main and are built independently.
list(FILTER TEST_SRCS EXCLUDE REGEX "/consumer/")
list(FILTER TEST_SRCS EXCLUDE REGEX "/np/")
list(FILTER TEST_SRCS EXCLUDE REGEX "/automatic_free_pool_failure\\.cpp$")
list(FILTER TEST_SRCS EXCLUDE REGEX "/assert_contract\\.cpp$")

add_executable(
   tests
   ${TEST_SRCS}
)

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(WJR_ENABLE_XSIMD)
   target_link_libraries(tests PRIVATE wjr::simd_xsimd)
   target_compile_definitions(tests PRIVATE WJR_TEST_XSIMD=1)
endif()

add_test(NAME wjr_tests COMMAND tests)
set_tests_properties(wjr_tests PROPERTIES
   LABELS unit
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set(WJR_POLICY_CMAKE_ARGS
   -G ${CMAKE_GENERATOR}
   -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
)
if(CMAKE_GENERATOR_PLATFORM)
   list(APPEND WJR_POLICY_CMAKE_ARGS -A ${CMAKE_GENERATOR_PLATFORM})
endif()
if(CMAKE_GENERATOR_TOOLSET)
   list(APPEND WJR_POLICY_CMAKE_ARGS -T ${CMAKE_GENERATOR_TOOLSET})
endif()

add_test(NAME wjr_build_policy
   COMMAND ${CMAKE_COMMAND}
   -S ${CMAKE_CURRENT_SOURCE_DIR}/cmake
   -B ${CMAKE_CURRENT_BINARY_DIR}/build-policy
   ${WJR_POLICY_CMAKE_ARGS}
   -DWJR_CXX_STANDARD=${WJR_CXX_STANDARD}
   -DWJR_CPU_TARGET=${WJR_CPU_TARGET}
   -DEXPECT_STANDARD=${WJR_CXX_STANDARD}
   -DWJR_MINIMAL_ASSERT=${WJR_MINIMAL_ASSERT}
)
add_test(NAME wjr_invalid_standard
   COMMAND ${CMAKE_COMMAND}
   -S ${CMAKE_CURRENT_SOURCE_DIR}/cmake
   -B ${CMAKE_CURRENT_BINARY_DIR}/invalid-standard
   ${WJR_POLICY_CMAKE_ARGS}
   -DWJR_CXX_STANDARD=17
)
set_tests_properties(wjr_invalid_standard PROPERTIES WILL_FAIL TRUE)
add_test(NAME wjr_legacy_standard
   COMMAND ${CMAKE_COMMAND}
   -S ${CMAKE_CURRENT_SOURCE_DIR}/cmake
   -B ${CMAKE_CURRENT_BINARY_DIR}/legacy-standard
   ${WJR_POLICY_CMAKE_ARGS}
   -DWJR_CXX_STANDARD=20
   -DWJR_DISABLE_CXX_20=ON
)
set_tests_properties(wjr_legacy_standard PROPERTIES WILL_FAIL TRUE)
set_tests_properties(wjr_build_policy wjr_invalid_standard wjr_legacy_standard
   PROPERTIES LABELS configuration)

if(WJR_ENABLE_COVERAGE)
   # find required tools
   find_program(LLVM_COV_PATH llvm-cov REQUIRED)
   find_program(LLVM_PROFDATA_PATH llvm-profdata REQUIRED)
   find_package(Python3 COMPONENTS Interpreter REQUIRED)

   target_compile_options(tests PRIVATE
      -fprofile-instr-generate -fcoverage-mapping -fno-inline
   )
   target_link_options(tests PRIVATE
      -fprofile-instr-generate -fcoverage-mapping
   )

   set(COVERAGE_IGNORE
      "(test|third-party|benchmark|build|_deps|simd.*(sse|avx|simd_cast))"
   )

   # add coverage target
   add_custom_target(coverage
      DEPENDS tests

      # Clean previous coverage data
      COMMAND ${CMAKE_COMMAND} -E remove -f
      ${CMAKE_BINARY_DIR}/default.profraw
      ${CMAKE_BINARY_DIR}/default.profdata
      COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/coverage

      # Run tests to generate coverage data
      COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=default.profraw
      $<TARGET_FILE:tests>

      # Merge profile data
      COMMAND ${LLVM_PROFDATA_PATH} merge
      -sparse default.profraw -o default.profdata

      # Generate HTML coverage report (only for include and src directories)
      COMMAND ${LLVM_COV_PATH} show $<TARGET_FILE:tests>
      -instr-profile=default.profdata
      -show-line-counts-or-regions
      -use-color
      -show-instantiation-summary
      -show-branches=count
      -format=html
      -output-dir=coverage/details
      -ignore-filename-regex="${COVERAGE_IGNORE}"

      # Generate the summary dashboard and text report.
      COMMAND ${Python3_EXECUTABLE}
      ${CMAKE_SOURCE_DIR}/tools/generate_coverage_report.py
      --llvm-cov ${LLVM_COV_PATH}
      --binary $<TARGET_FILE:tests>
      --profile ${CMAKE_BINARY_DIR}/default.profdata
      --output-dir ${CMAKE_BINARY_DIR}/coverage
      --details-dir details

      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Generating coverage dashboard for include and src directories..."
   )
endif()

target_include_directories(tests PRIVATE ${GMP_INCLUDE_DIRS})

target_link_libraries(tests
   PRIVATE
   wjr::wjr
   gtest_main
   ${GMP_LIBRARIES}
)

# Test fatal allocation contracts without changing allocators in the unit-test process.
add_executable(automatic_free_pool_failure memory/automatic_free_pool_failure.cpp)
target_link_libraries(automatic_free_pool_failure PRIVATE wjr::assert wjr::iterator)
add_dependencies(tests automatic_free_pool_failure)
foreach(_WJR_POOL_FAILURE IN ITEMS overflow allocation)
   add_test(NAME wjr_pool_${_WJR_POOL_FAILURE}
      COMMAND automatic_free_pool_failure ${_WJR_POOL_FAILURE})
   set_tests_properties(wjr_pool_${_WJR_POOL_FAILURE} PROPERTIES LABELS unit)
endforeach()

foreach(_WJR_ASSERT_MODE IN ITEMS full light minimal)
   string(TOUPPER "${_WJR_ASSERT_MODE}" _WJR_ASSERT_MODE_MACRO)
   add_executable(assert_contract_${_WJR_ASSERT_MODE} utility/assert_contract.cpp)
   target_link_libraries(assert_contract_${_WJR_ASSERT_MODE} PRIVATE wjr::assert)
   target_compile_definitions(assert_contract_${_WJR_ASSERT_MODE}
      PRIVATE WJR_ASSERT_TEST_${_WJR_ASSERT_MODE_MACRO})
   add_dependencies(tests assert_contract_${_WJR_ASSERT_MODE})
   add_test(NAME wjr_assert_${_WJR_ASSERT_MODE}
      COMMAND assert_contract_${_WJR_ASSERT_MODE} 0)
   add_test(NAME wjr_assert_${_WJR_ASSERT_MODE}_minimal_handler
      COMMAND assert_contract_${_WJR_ASSERT_MODE} 0 minimal)
   set_tests_properties(wjr_assert_${_WJR_ASSERT_MODE}
      wjr_assert_${_WJR_ASSERT_MODE}_minimal_handler PROPERTIES LABELS unit)
endforeach()

if(WJR_ENABLE_NP)
   add_executable(np_tests
      np/view.cpp np/checked_view.cpp np/tensor.cpp
      np/reshape.cpp np/checked_strided_view.cpp np/subview.cpp
      np/transpose.cpp np/elementwise.cpp)
   target_link_libraries(np_tests PRIVATE wjr::np gtest_main)
   add_test(NAME wjr_np_tests COMMAND np_tests)
   set_tests_properties(wjr_np_tests PROPERTIES LABELS unit)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})