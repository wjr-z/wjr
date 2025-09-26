if(WJR_ARCH)
    add_subdirectory(${WJR_ARCH})
else()
    message(WARNING "No architecture is specified, so arch library is disabled.")
endif()

function(add_if_not_exists target)
    if(NOT TARGET wjr::${target})
        wjr_cc_library(
            NAME
            ${target}
        )
    endif()
endfunction()

add_if_not_exists(arch-math)
add_if_not_exists(arch-format)
add_if_not_exists(arch-biginteger)
add_if_not_exists(arch-json)