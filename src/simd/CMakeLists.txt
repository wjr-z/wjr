include(wjrXsimd)
wjr_load_xsimd(${WJR_ENABLE_INSTALL})

add_library(wjr-simd_xsimd INTERFACE)
add_library(wjr::simd_xsimd ALIAS wjr-simd_xsimd)
target_link_libraries(wjr-simd_xsimd INTERFACE
    wjr::config wjr::isa $<BUILD_INTERFACE:xsimd>)
target_compile_features(wjr-simd_xsimd INTERFACE cxx_std_20)
set_target_properties(wjr-simd_xsimd PROPERTIES EXPORT_NAME simd_xsimd)

if(WJR_ENABLE_INSTALL)
    install(TARGETS wjr-simd_xsimd EXPORT wjrSimdXsimdTargets)
    install(EXPORT wjrSimdXsimdTargets
        NAMESPACE wjr::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/wjr
    )
    configure_file(wjr-simd_xsimd-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/wjr-simd_xsimd-config.cmake @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/wjr-simd_xsimd-config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/wjr
    )
endif()
