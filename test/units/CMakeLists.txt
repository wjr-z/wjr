include(FetchContent)

set(WJR_DEBUG_LEVEL_DEBUG 3)
set(WJR_DEBUG_LEVEL_RELEASE 3)

set(WJR_PUBLIC_FLAGS ON)

# todo: These options should be set in the parent project
if(MSVC)
   add_compile_definitions(__ADX__)
   add_compile_definitions(__BMI2__)
   add_compile_definitions(__AVX2__)
   add_compile_definitions(__POPCNT__)
   add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/arch:AVX2>)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(WJR_USE_GMP ON)

find_package(GMP QUIET)
if(NOT GMP_FOUND)
   find_package(PkgConfig)
   pkg_check_modules(GMP QUIET gmp)
   if (NOT GMP_FOUND)
      set(WJR_USE_GMP OFF)
   else()
      set(GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIRS})
      set(GMP_LIBRARIES ${GMP_LIBRARIES})
   endif()
else()
   set(GMP_INCLUDE_DIRS)
   set(GMP_LIBRARIES gmp libgmp)
endif()

if (WJR_USE_GMP)
   add_compile_definitions(WJR_USE_GMP)
endif()

if(EXISTS googletest)
   add_subdirectory(googletest)
else()
   FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG release-1.10.0
   )

   set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
   set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
   set(BUILD_GTEST ON CACHE BOOL "" FORCE)

   FetchContent_MakeAvailable(googletest)
endif()

file(GLOB_RECURSE TEST_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(
   tests
   ${TEST_SRCS}
)

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(WJR_ENABLE_COVERAGE)
   set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -std=c++17 -march=native -DWJR_DEBUG_LEVEL=3 -Wall -Wextra -g")

   # find required tools
   # find_program(LCOV lcov REQUIRED)
   # find_program(GENHTML genhtml REQUIRED)
   set(target tests)

   target_compile_options(${target}
      PRIVATE -fprofile-instr-generate -fcoverage-mapping -fno-inline)
   target_link_options(${target}
      PUBLIC -fprofile-instr-generate -fcoverage-mapping)

   # add coverage target
   find_program(LLVM_COV_PATH llvm-cov REQUIRED)
   find_program(LLVM_PROFDATA_PATH llvm-profdata REQUIRED)
   add_custom_target(coverage-${target}
      COMMAND $<TARGET_FILE:${target}>

      # COMMAND del coverage /S /Q
      COMMAND ${LLVM_PROFDATA_PATH} merge
      -sparse default.profraw -o default.profdata
      COMMAND ${LLVM_COV_PATH} show $<TARGET_FILE:${target}>
      -instr-profile=default.profdata
      -show-line-counts-or-regions
      -use-color
      -show-instantiation-summary
      -show-branches=count
      -format=html
      -output-dir=coverage-${target}
      --ignore-filename-regex="\(tests\|gmp\)"
      COMMAND ${LLVM_COV_PATH} report $<TARGET_FILE:${target}>
      -instr-profile=default.profdata
      -show-region-summary=false
      -show-branch-summary=false
      -show-branch-summary=false
      --ignore-filename-regex="\(tests\|gmp\)"
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
   )
endif()

target_include_directories(tests PRIVATE ${GMP_INCLUDE_DIRS})

target_link_libraries(tests
   PRIVATE
   wjr::wjr
   gtest_main
   ${GMP_LIBRARIES}
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})