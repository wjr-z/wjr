#ifndef WJR_CRTP_NONCOPYABLE_HPP__
#define WJR_CRTP_NONCOPYABLE_HPP__

namespace wjr {

/**
 * @brief A type to disable copying the object.
 *
 */
class noncopyable {
protected:
    noncopyable() = default;
    noncopyable(const noncopyable &) = delete;
    noncopyable(noncopyable &&) = default;
    noncopyable &operator=(const noncopyable &) = delete;
    noncopyable &operator=(noncopyable &&) = default;
    ~noncopyable() = default;
};

} // namespace wjr

#endif // WJR_CRTP_NONCOPYABLE_HPP__