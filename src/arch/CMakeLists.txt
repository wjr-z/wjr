if(WJR_ARCH)
    add_subdirectory(${WJR_ARCH})
else()
    message(WARNING "No architecture is specified, so arch library is disabled.")

    wjr_cc_library(
        NAME
        arch
    )
endif()