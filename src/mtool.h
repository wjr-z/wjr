#ifndef WJR_MTOOL_H
#define WJR_MTOOL_H

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>
#include <io.h>

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

    std::vector<std::string> get_all_files(const std::string& path);
    std::string read_file(const std::string& filename);
    void write_file(const std::string& filename, const std::string& str);

}
#endif
