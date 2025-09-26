include(CMakeParseArguments)

#
# macro
#
function(wjr_glob_cpp_source TARGET PATH)
    file(GLOB_RECURSE TMP_SRCS ${WJR_LIB_DIR}/${PATH}/*.cpp ${WJR_LIB_DIR}/${PATH}/**/*.cpp)
    set(${TARGET} ${TMP_SRCS} PARENT_SCOPE)
endfunction()

function(wjr_append_library NAME)
    get_property(WJR_ALL_TARGETS GLOBAL PROPERTY WJR_ALL_TARGETS)
    list(APPEND WJR_ALL_TARGETS ${NAME})
    set_property(GLOBAL PROPERTY WJR_ALL_TARGETS "${WJR_ALL_TARGETS}")
endfunction()

function(wjr_install_library TARGET)
    install(TARGETS ${TARGET} EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endfunction()

# todo: support pch
function(wjr_cc_library)
    cmake_parse_arguments(WJR_CC_LIB
        "DISABLE_INSTALL;PUBLIC;OBJECT;FINAL"
        "NAME"
        "HDRS;SRCS;COPTS;PRIVATE_COPTS;DEFINES;LINKOPTS;DEPS"
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

    if(WJR_CC_LIB_FINAL)
        set(WJR_TARGET "wjr")
    else()
        set(WJR_TARGET "wjr-${WJR_CC_LIB_NAME}")
    endif()

    if(NOT WJR_CC_LIB_OBJECT)
        wjr_append_library(${WJR_CC_LIB_NAME})
    endif()

    set(WJR_CC_COPTS ${WJR_CC_LIB_COPTS} ${WJR_COMMON_CXX_FLAGS})
    set(WJR_CC_PRIVATE_COPTS ${WJR_CC_LIB_PRIVATE_COPTS} ${WJR_CXX_FLAGS_PRIVATE})

    if(WJR_ENABLE_INSTALL AND NOT WJR_CC_LIB_DISABLE_INSTALL)
        set(WJR_NEED_INSTALL ON)
    else()
        set(WJR_NEED_INSTALL OFF)
    endif()

    set(WJR_CC_TARGET_OBJECTS)

    if(WJR_CC_LIB_OBJECT)
        list(APPEND WJR_CC_TARGET_OBJECTS "$<TARGET_OBJECTS:${WJR_TARGET}>")
    endif()

    foreach(deps ${WJR_CC_LIB_DEPS})
        get_target_property(target_objects ${deps} TARGET_OBJECTS)

        if(target_objects)
            list(APPEND WJR_CC_TARGET_OBJECTS ${target_objects})
        else()
            get_target_property(type ${deps} TYPE)

            if(type STREQUAL "OBJECT_LIBRARY")
                list(APPEND WJR_CC_TARGET_OBJECTS "$<TARGET_OBJECTS:${deps}>")
            endif()
        endif()
    endforeach()

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
            PRIVATE $<$<COMPILE_LANGUAGE:CXX>: ${WJR_CC_PRIVATE_COPTS}$<SEMICOLON>>
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

        target_compile_options(${WJR_TARGET}
            INTERFACE $<$<COMPILE_LANGUAGE:CXX>: ${WJR_CC_COPTS}$<SEMICOLON>>
            INTERFACE $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:DEBUG>>: ${WJR_CXX_FLAGS_DEBUG}$<SEMICOLON>>
            INTERFACE $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:RELEASE>>: ${WJR_CXX_FLAGS_RELEASE}$<SEMICOLON>>
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

    if(WJR_CC_LIB_IS_INTERFACE OR WJR_CC_LIB_OBJECT)
        set_target_properties(${WJR_TARGET} PROPERTIES TARGET_OBJECTS "${WJR_CC_TARGET_OBJECTS}")
    else()
        target_sources(${WJR_TARGET} PRIVATE ${WJR_CC_TARGET_OBJECTS})
    endif()

    if(WJR_NEED_INSTALL)
        wjr_install_library(${WJR_TARGET})
    endif()

    add_library(wjr::${WJR_CC_LIB_NAME} ALIAS ${WJR_TARGET})
endfunction()

function(wjr_asm_library)
    cmake_parse_arguments(WJR_ASM_LIB
        ""
        "NAME"
        "SRCS;COPTS;PRIVATE_COPTS"
        ${ARGN}
    )

    set(WJR_ASM_SRCS "${WJR_ASM_LIB_SRCS}")
    set(WJR_TARGET "wjr-${WJR_ASM_LIB_NAME}")
    set(WJR_ASM_COPTS ${WJR_ASM_LIB_COPTS})
    set(WJR_ASM_PRIVATE_COPTS ${WJR_ASM_LIB_PRIVATE_COPTS})

    if(WJR_ENABLE_INSTALL)
        set(WJR_NEED_INSTALL ON)
    else()
        set(WJR_NEED_INSTALL OFF)
    endif()

    add_library(${WJR_TARGET} OBJECT)
    target_sources(${WJR_TARGET} PRIVATE ${WJR_ASM_LIB_SRCS})
    set_target_properties(${WJR_TARGET} PROPERTIES
        LINKER_LANGUAGE ASM_NASM
    )

    target_compile_options(${WJR_TARGET}
        PUBLIC $<$<COMPILE_LANGUAGE:ASM_NASM>: ${WJR_ASM_COPTS}$<SEMICOLON>>
        PRIVATE $<$<COMPILE_LANGUAGE:ASM_NASM>: ${WJR_ASM_PRIVATE_COPTS}$<SEMICOLON>>
    )

    if(WJR_NEED_INSTALL)
        wjr_install_library(${WJR_TARGET})
    endif()

    add_library(wjr::${WJR_ASM_LIB_NAME} ALIAS ${WJR_TARGET})
endfunction()