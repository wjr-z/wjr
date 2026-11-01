# Use only the reviewed installed provider; never fetch/build yyjson here.
find_package(yyjson CONFIG REQUIRED)
include("${PROJECT_SOURCE_DIR}/cmake/wjrYyjsonProvider.cmake")
_wjr_check_yyjson_provider(_WJR_YYJSON_VALID)
if(NOT _WJR_YYJSON_VALID)
    message(FATAL_ERROR "The yyjson component requires prebuilt yyjson 0.13.0 with reader and UTF-8 validation.")
endif()
unset(_WJR_YYJSON_VALID)

add_library(wjr-yyjson INTERFACE)
add_library(wjr::yyjson ALIAS wjr-yyjson)
set_target_properties(wjr-yyjson PROPERTIES EXPORT_NAME yyjson)
target_compile_definitions(wjr-yyjson INTERFACE WJR_YYJSON_013=1)
target_link_libraries(wjr-yyjson INTERFACE
    wjr::config wjr::assert $<BUILD_INTERFACE:yyjson::yyjson>)

if(WJR_ENABLE_INSTALL)
    install(TARGETS wjr-yyjson EXPORT wjrYyjsonTargets)
    install(EXPORT wjrYyjsonTargets NAMESPACE wjr::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/wjr")
    install(FILES "${PROJECT_SOURCE_DIR}/cmake/wjr-yyjson-config.cmake"
        "${PROJECT_SOURCE_DIR}/cmake/wjrYyjsonProvider.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/wjr")
endif()
