# Each module compiles its own memory implementation, as independent static WJR users do.
add_library(wjr_stack_domain_foreign SHARED
    foreign.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../src/memory/stack_allocator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../src/utility/assert.cpp
)
target_link_libraries(wjr_stack_domain_foreign PRIVATE wjr::config)
target_compile_options(wjr_stack_domain_foreign PRIVATE ${WJR_BASELINE_FLAGS})
target_compile_definitions(wjr_stack_domain_foreign PRIVATE WJR_STACK_DOMAIN_EXPORTS=1)
set_target_properties(wjr_stack_domain_foreign PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

add_executable(wjr_stack_domain main.cpp)
target_link_libraries(wjr_stack_domain PRIVATE wjr::memory wjr_stack_domain_foreign)
if(TARGET tests)
    add_dependencies(tests wjr_stack_domain)
endif()
add_test(NAME wjr_stack_domain COMMAND wjr_stack_domain)
set_tests_properties(wjr_stack_domain PROPERTIES LABELS unit)
