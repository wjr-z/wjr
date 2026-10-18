# Use a system/prebuilt package. Never fetch or build oneTBB sources here.
find_package(TBB 2021.11 CONFIG REQUIRED COMPONENTS tbb)

add_library(wjr-tbb INTERFACE)
add_library(wjr::tbb ALIAS wjr-tbb)
set_target_properties(wjr-tbb PROPERTIES EXPORT_NAME tbb)
target_link_libraries(wjr-tbb INTERFACE
    wjr::config wjr::assert $<BUILD_INTERFACE:TBB::tbb>)

if(WJR_ENABLE_INSTALL)
    install(TARGETS wjr-tbb EXPORT wjrTbbTargets)
    install(EXPORT wjrTbbTargets
        NAMESPACE wjr::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/wjr"
    )
    install(FILES "${PROJECT_SOURCE_DIR}/cmake/wjr-tbb-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/wjr"
    )
endif()
