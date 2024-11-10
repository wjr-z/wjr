cmake_minimum_required(VERSION 3.14)
project(tests LANGUAGES CXX)

include(FetchContent)

set(WJR_ENABLE_ASSEMBLY ON)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

if (NOT DEFINED WJR_DEBUG_LEVEL_RELEASE)
   set(WJR_DEBUG_LEVEL_RELEASE 3)
endif()

if(MSVC)
   add_compile_definitions(__ADX__)
   add_compile_definitions(__BMI2__)
   add_compile_definitions(__AVX2__)
   add_compile_definitions(__POPCNT__)
   add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/arch:AVX2>)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../wjr ${CMAKE_CURRENT_BINARY_DIR}/wjr)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_compile_definitions(WJR_USE_GMP)

FetchContent_Declare(
   googletest
   GIT_REPOSITORY https://github.com/google/googletest.git
   GIT_TAG release-1.10.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(
   tests
   src/preprocessor.cpp
   src/math.cpp
   src/memory.cpp
   src/container.cpp
   src/compressed_pair.cpp
   src/tuple.cpp
   src/biginteger.cpp
   src/expected.cpp
   src/fastfloat.cpp
   src/json.cpp
   src/optional.cpp
)

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
            COMMAND ${LLVM_COV_PATH} report $<TARGET_FILE:${target}>
                                    -instr-profile=default.profdata
                                    -show-region-summary=false
                                    -show-branch-summary=false
                                    -show-branch-summary=false
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
endif()

if(MSVC)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /I \"D:\\msys\\home\\19663\\gmp-6.3.0\"")
else()
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I \"D:\\msys\\home\\19663\\gmp-6.3.0\" ")
endif()
message(${CMAKE_CXX_FLAGS})



set(GMP_LIBS "D:\\msys\\home\\19663\\gmp-6.3.0\\.libs")

link_directories(${GMP_LIBS})
target_link_libraries(tests
PRIVATE
   wjr
   ${GMP_LIBS}/libgmp.dll.a
   ${GMP_LIBS}/libgmpxx.dll.a
   gtest_main
)

# target_precompile_headers(tests REUSE_FROM wjr)

enable_testing()