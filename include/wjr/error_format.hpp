#ifndef WJR_ERROR_FORMAT_HPP__
#define WJR_ERROR_FORMAT_HPP__

#include <system_error>

namespace wjr {

template <typename Err>
struct error_format {
    template <typename Output>
    static decltype(auto) format(Output &output, const Err &err) {
        return output << err;
    }
};

template <>
struct error_format<std::error_code> {
    template <typename Output>
    static decltype(auto) format(Output &output, const std::error_code &err) {
        return output << err.message();
    }
};

template <>
struct error_format<std::errc> {
    template <typename Output>
    static decltype(auto) format(Output &output, const std::errc &err) {
        return output << std::make_error_code(err).message();
    }
};

} // namespace wjr

#endif // WJR_ERROR_FORMAT_HPP__