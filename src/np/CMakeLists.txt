# Review this snapshot as a C++23 mdspan subset, independently of upstream's
# unchanged 0.6.0 project version. Later snapshots add throwing C++26 at().
if(NOT TARGET std::mdspan)
    FetchContent_Declare(
        mdspan
        GIT_REPOSITORY https://github.com/kokkos/mdspan.git
        GIT_TAG 0e4eba59ad53451784b3839bdf98b25fa2d64931
    )
    FetchContent_MakeAvailable(mdspan)
endif()

# Keep optional modules out of the ordinary wjr target and export set.
add_library(wjr-np INTERFACE)
add_library(wjr::np ALIAS wjr-np)
set_target_properties(wjr-np PROPERTIES EXPORT_NAME np)
target_link_libraries(wjr-np INTERFACE wjr::config $<BUILD_INTERFACE:std::mdspan>)
target_compile_features(wjr-np INTERFACE cxx_std_20)
# One indexing spelling in C++20 and C++23, with no public backend switch.
target_compile_definitions(wjr-np INTERFACE
    MDSPAN_USE_PAREN_OPERATOR=1
    MDSPAN_USE_BRACKET_OPERATOR=0
)

if(WJR_ENABLE_INSTALL)
    install(TARGETS wjr-np EXPORT wjrNpTargets)
    install(EXPORT wjrNpTargets
        NAMESPACE wjr::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/wjr"
    )
    install(FILES "${PROJECT_SOURCE_DIR}/cmake/wjr-np-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/wjr"
    )
endif()
