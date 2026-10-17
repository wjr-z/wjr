wjr_cc_library(
    NAME
    config
)

# Shared header and ABI requirements belong to one exported interface target.
target_include_directories(wjr-config INTERFACE
    $<BUILD_INTERFACE:${WJR_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(wjr-config INTERFACE ${WJR_CXX_FEATURE})
target_compile_options(wjr-config INTERFACE
    "$<$<COMPILE_LANGUAGE:CXX>:${WJR_CXX_FLAGS}>"
)
target_compile_definitions(wjr-config INTERFACE
    "$<$<COMPILE_LANGUAGE:CXX>:${WJR_COMPILE_DEFINITIONS}>"
    "$<$<COMPILE_LANGUAGE:CXX>:WJR_DEBUG_LEVEL=$<IF:$<CONFIG:Debug>,${WJR_DEBUG_LEVEL_DEBUG},${WJR_DEBUG_LEVEL_RELEASE}>>"
)
target_link_options(wjr-config INTERFACE ${WJR_LINK_OPTIONS})

# Only explicitly marked numerical modules carry this ISA requirement.
wjr_cc_library(NAME isa)
target_compile_options(wjr-isa INTERFACE
    "$<$<COMPILE_LANGUAGE:CXX>:${WJR_ISA_FLAGS}>"
)