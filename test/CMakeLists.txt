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

if(EXISTS Catch2)
   add_subdirectory(Catch2)
else()
   FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG v3.8.1
   )

   FetchContent_MakeAvailable(Catch2)
   set_target_properties(Catch2 PROPERTIES CXX_STANDARD 17)
   target_compile_definitions(Catch2
   PUBLIC
      CATCH_CONFIG_CPP17_STRING_VIEW
   )
endif()

file(GLOB_RECURSE TEST_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(
   tests
   ${TEST_SRCS}
)

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(WJR_ENABLE_COVERAGE)
   # find required tools
   find_program(LLVM_COV_PATH llvm-cov REQUIRED)
   find_program(LLVM_PROFDATA_PATH llvm-profdata REQUIRED)

   set(COVERAGE_IGNORE
      "(test|third-party|benchmark|build|_deps|simd.*(sse|avx|simd_cast))"
   )

   # add coverage target
   add_custom_target(coverage

      # Clean previous coverage data
      COMMAND ${CMAKE_COMMAND} -E remove -f default.profraw default.profdata
      COMMAND ${CMAKE_COMMAND} -E remove_directory coverage

      # Run tests to generate coverage data
      COMMAND $<TARGET_FILE:tests>

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
      -output-dir=coverage
      -ignore-filename-regex="${COVERAGE_IGNORE}"

      # Generate text summary report (only for include and src directories)
      COMMAND ${LLVM_COV_PATH} report $<TARGET_FILE:tests>
      -instr-profile=default.profdata
      -show-region-summary=true
      -show-branch-summary=true
      -ignore-filename-regex="${COVERAGE_IGNORE}"

      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Generating coverage report for include and src directories..."
   )
endif()

target_include_directories(tests PRIVATE ${GMP_INCLUDE_DIRS})

target_link_libraries(tests
   PRIVATE
   wjr::wjr
   Catch2::Catch2WithMain
   ${GMP_LIBRARIES}
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})