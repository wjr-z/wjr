#ifndef __WJR_MTOOL_H
#define __WJR_MTOOL_H

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <type_traits>
#include "mString.h"

namespace wjr {

    class time_ref;
    time_ref mtime();
    void mtime(time_ref*);
    double operator-(const time_ref& lhs, const time_ref& rhs);
    class time_ref {
    private:

        typedef std::chrono::high_resolution_clock m_clock;
        typedef std::chrono::microseconds m_mis;

        m_clock::time_point TimePoint;

        friend time_ref mtime();
        friend void mtime(time_ref*);
        static m_clock::time_point now();
        static double testTime(const m_clock::time_point& start, const m_clock::time_point& end);
    public:
        time_ref() = default;
        time_ref(const m_clock::time_point& TimePoint);
        time_ref& operator=(const time_ref&) = default;
        friend double operator-(const time_ref& lhs, const time_ref& rhs);
    };

    std::vector<String> get_all_files(const String&path);
    size_t get_file_length(String_view file_name);
    String read_file(String_view file_name);
    void write_file(String_view file_name,String_view str);
    int create_file(String_view file_name
    #ifdef __linux__
        ,__mode_t __mode = 0755
    #endif
    );

}
#endif
