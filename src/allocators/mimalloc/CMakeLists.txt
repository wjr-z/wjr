# This opt-in module uses a prebuilt shared provider; it never downloads/builds mimalloc.
if(NOT TARGET mimalloc)
    find_package(mimalloc 3.6 CONFIG REQUIRED)
    if(TARGET mimalloc)
        set_property(TARGET mimalloc PROPERTY IMPORTED_GLOBAL TRUE)
    endif()
endif()
if(NOT TARGET mimalloc)
    message(FATAL_ERROR "WJR's mimalloc adapter requires the package's shared mimalloc target.")
endif()

add_library(wjr-mimalloc STATIC mimalloc.cpp)
add_library(wjr::mimalloc ALIAS wjr-mimalloc)
set_target_properties(wjr-mimalloc PROPERTIES EXPORT_NAME mimalloc)
target_link_libraries(wjr-mimalloc PUBLIC wjr::config $<BUILD_INTERFACE:mimalloc>)

if(WJR_ENABLE_INSTALL)
    install(TARGETS wjr-mimalloc EXPORT wjrMimallocTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
    install(EXPORT wjrMimallocTargets NAMESPACE wjr::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/wjr")
    install(FILES "${PROJECT_SOURCE_DIR}/cmake/wjr-mimalloc-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/wjr")
endif()
