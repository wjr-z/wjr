if(NOT TARGET std::mdspan)
    # QUIET discovery lets OPTIONAL_COMPONENTS np remain optional. The parent
    # config applies required-component policy after all components are loaded.
    find_package(mdspan 0.6.0 EXACT QUIET CONFIG
        PATHS "${_WJR_PACKAGE_PREFIX}" NO_DEFAULT_PATH)
endif()
if(NOT TARGET std::mdspan)
    set(wjr_np_FOUND FALSE)
    set(wjr_np_NOT_FOUND_MESSAGE "The np component requires the reviewed Kokkos mdspan package.")
    return()
endif()

if(NOT TARGET wjr::np)
    include("${CMAKE_CURRENT_LIST_DIR}/wjrNpTargets.cmake")
    target_link_libraries(wjr::np INTERFACE std::mdspan)
endif()
set(wjr_np_FOUND TRUE)
