function(_wjr_check_yyjson_provider result)
    if(NOT TARGET yyjson::yyjson)
        set(${result} FALSE PARENT_SCOPE)
        return()
    endif()
    get_target_property(_wjr_yyjson_imported yyjson::yyjson IMPORTED)
    if(NOT _wjr_yyjson_imported)
        set(${result} FALSE PARENT_SCOPE)
        return()
    endif()
    # Official 0.13.0 exports a config but no ConfigVersion file.
    include(CMakePushCheckState)
    include(CheckCXXSourceCompiles)
    cmake_push_check_state(RESET)
    set(CMAKE_REQUIRED_LIBRARIES yyjson::yyjson)
    unset(_WJR_YYJSON_REVIEWED_PROVIDER CACHE)
    check_cxx_source_compiles("
        #include <yyjson.h>
        #if YYJSON_VERSION_HEX != 0x000D00 || YYJSON_DISABLE_READER || YYJSON_DISABLE_UTF8_VALIDATION
        #error WJR requires yyjson 0.13.0 with reader and UTF-8 validation
        #endif
        int main() {
            yyjson_read_err error;
            char input[] = \"null\";
            yyjson_doc *document = yyjson_read_opts(input, 4, YYJSON_READ_NOFLAG, nullptr, &error);
            yyjson_doc_free(document);
            return yyjson_version() == YYJSON_VERSION_HEX ? 0 : 1;
        }
    " _WJR_YYJSON_REVIEWED_PROVIDER)
    set(${result} "${_WJR_YYJSON_REVIEWED_PROVIDER}" PARENT_SCOPE)
    unset(_WJR_YYJSON_REVIEWED_PROVIDER CACHE)
    cmake_pop_check_state()
endfunction()
