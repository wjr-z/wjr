find_package(Threads REQUIRED)

add_library(wjr-executor scoped_executor.cpp)
add_library(wjr::executor ALIAS wjr-executor)
set_target_properties(wjr-executor PROPERTIES EXPORT_NAME executor)
target_link_libraries(wjr-executor PUBLIC
    wjr::config wjr::assert wjr::iterator $<BUILD_INTERFACE:Threads::Threads>)
target_compile_options(wjr-executor PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${WJR_CXX_FLAGS_PRIVATE};${WJR_BASELINE_FLAGS}>")

if(WJR_ENABLE_INSTALL)
    install(TARGETS wjr-executor EXPORT wjrExecutorTargets
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
    install(EXPORT wjrExecutorTargets NAMESPACE wjr::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/wjr")
    install(FILES "${PROJECT_SOURCE_DIR}/cmake/wjr-executor-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/wjr")
endif()
