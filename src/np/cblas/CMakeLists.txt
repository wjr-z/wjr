include(wjrCblas)
wjr_find_openblas(WJR_CBLAS_TARGET)
if(NOT WJR_CBLAS_TARGET)
    message(FATAL_ERROR "${WJR_CBLAS_REASON}")
endif()

add_library(wjr-np_cblas INTERFACE)
add_library(wjr::np_cblas ALIAS wjr-np_cblas)
set_target_properties(wjr-np_cblas PROPERTIES EXPORT_NAME np_cblas)
target_link_libraries(wjr-np_cblas INTERFACE wjr::np $<BUILD_INTERFACE:${WJR_CBLAS_TARGET}>)
target_compile_definitions(wjr-np_cblas INTERFACE WJR_NP_CBLAS_LP64=1)

if(WJR_ENABLE_INSTALL)
    install(TARGETS wjr-np_cblas EXPORT wjrNpCblasTargets)
    install(EXPORT wjrNpCblasTargets
        NAMESPACE wjr::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/wjr"
    )
    install(FILES
        "${PROJECT_SOURCE_DIR}/cmake/wjr-np_cblas-config.cmake"
        "${PROJECT_SOURCE_DIR}/cmake/wjrCblas.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/wjr"
    )
endif()
