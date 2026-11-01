find_package(Threads REQUIRED)
add_executable(wjr_mimalloc_consumer main.cpp)
target_link_libraries(wjr_mimalloc_consumer PRIVATE wjr::mimalloc wjr::json Threads::Threads)
if(WIN32)
    add_custom_command(TARGET wjr_mimalloc_consumer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:mimalloc> $<TARGET_FILE_DIR:wjr_mimalloc_consumer>)
endif()
if(TARGET tests)
    add_dependencies(tests wjr_mimalloc_consumer)
endif()
add_test(NAME wjr_mimalloc_consumer COMMAND wjr_mimalloc_consumer)
set_tests_properties(wjr_mimalloc_consumer PROPERTIES LABELS unit)
