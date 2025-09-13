include(CMakeParseArguments)

function(wjr_add_propagate_object_library object_name interface_name)
    add_library(wjr-${interface_name} INTERFACE)
    add_library(wjr::${interface_name} ALIAS wjr-${interface_name})
    target_link_libraries(wjr-${interface_name} INTERFACE ${object_name} $<TARGET_OBJECTS:${object_name}>)
endfunction()

# todo: support pch
function(wjr_cc_library)
    cmake_parse_arguments(WJR_CC_LIB
        "DISABLE_INSTALL;PUBLIC;TESTONLY;OBJECT;FINAL"
        "NAME"
        "HDRS;SRCS;COPTS;DEFINES;LINKOPTS;DEPS"
        ${ARGN}
    )

    set(WJR_CC_SRCS "${WJR_CC_LIB_SRCS}")

    if(WJR_CC_SRCS STREQUAL "")
        set(WJR_CC_LIB_IS_INTERFACE 1)
    else()
        set(WJR_CC_LIB_IS_INTERFACE 0)
    endif()

    if(WJR_CC_LIB_OBJECT AND WJR_CC_LIB_FINAL)
        message(FATAL_ERROR "Cannot set both OBJECT and FINAL for the same library.")
    endif()

    if(WJR_CC_LIB_OBJECT AND WJR_CC_LIB_IS_INTERFACE)
        message(FATAL_ERROR "Cannot create an OBJECT library without source files.")
    endif()

    if(WJR_CC_LIB_OBJECT)
        set(WJR_TARGET "wjr-${WJR_CC_LIB_NAME}-object")
    elseif(WJR_CC_LIB_FINAL)
        set(WJR_TARGET "wjr")
    else()
        set(WJR_TARGET "wjr-${WJR_CC_LIB_NAME}")
    endif()

    set(WJR_CC_COPTS ${WJR_CC_LIB_COPTS} ${WJR_COMMON_CXX_FLAGS})

    if(NOT WJR_CC_LIB_IS_INTERFACE)
        if(NOT WJR_CC_LIB_OBJECT)
            add_library(${WJR_TARGET} "")
        else()
            add_library(${WJR_TARGET} OBJECT)
        endif()

        target_sources(${WJR_TARGET} PRIVATE ${WJR_CC_LIB_SRCS} ${WJR_CC_LIB_HDRS})

        target_link_libraries(${WJR_TARGET}
            PUBLIC ${WJR_CC_LIB_DEPS}
            PRIVATE
            ${WJR_CC_LIB_LINKOPTS}
            ${WJR_DEFAULT_LINKOPTS}
        )

        target_include_directories(${WJR_TARGET}
            PUBLIC
            $<BUILD_INTERFACE:${WJR_COMMON_INCLUDE_DIRS}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )
        target_compile_options(${WJR_TARGET}
            PUBLIC $<$<COMPILE_LANGUAGE:CXX>: ${WJR_CC_COPTS}$<SEMICOLON>>
            PRIVATE $<$<COMPILE_LANGUAGE:CXX>: ${WJR_CXX_FLAGS_PRIVATE}$<SEMICOLON>>
            PUBLIC $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:DEBUG>>: ${WJR_CXX_FLAGS_DEBUG}$<SEMICOLON>>
            PUBLIC $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:RELEASE>>: ${WJR_CXX_FLAGS_RELEASE}$<SEMICOLON>>
        )
        target_compile_definitions(${WJR_TARGET}
            PUBLIC $<$<COMPILE_LANGUAGE:CXX>: ${WJR_CC_LIB_DEFINES} ${WJR_COMMON_DEFINES}$<SEMICOLON>>
        )

        if(WJR_PROPAGATE_CXX_STD)
            target_compile_features(${WJR_TARGET} PUBLIC ${WJR_INTERNAL_CXX_STD_FEATURE})
        endif()
    else()
        add_library(${WJR_TARGET} INTERFACE)
        target_include_directories(${WJR_TARGET}
            INTERFACE
            $<BUILD_INTERFACE:${WJR_COMMON_INCLUDE_DIRS}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )

        target_link_libraries(${WJR_TARGET}
            INTERFACE
            ${WJR_CC_LIB_DEPS}
            ${WJR_CC_LIB_LINKOPTS}
            ${WJR_DEFAULT_LINKOPTS}
        )
        target_compile_definitions(${WJR_TARGET}
            INTERFACE $<$<COMPILE_LANGUAGE:CXX>: ${WJR_CC_LIB_DEFINES} ${WJR_COMMON_DEFINES}$<SEMICOLON>>
        )

        if(WJR_PROPAGATE_CXX_STD)
            target_compile_features(${WJR_TARGET} INTERFACE ${WJR_INTERNAL_CXX_STD_FEATURE})
        endif()
    endif()

    if(NOT WJR_CC_LIB_OBJECT)
        add_library(wjr::${WJR_CC_LIB_NAME} ALIAS ${WJR_TARGET})
    else()
        wjr_add_propagate_object_library(${WJR_TARGET} ${WJR_CC_LIB_NAME})
    endif()
endfunction()