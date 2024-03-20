#ifndef WJR_CRTP_NONMOVABLE_HPP__
#define WJR_CRTP_NONMOVABLE_HPP__

namespace wjr {

class nonmovable {
protected:
    nonmovable() = default;
    nonmovable(const nonmovable &) = default;
    nonmovable(nonmovable &&) = delete;
    nonmovable &operator=(const nonmovable &) = default;
    nonmovable &operator=(nonmovable &&) = delete;
    ~nonmovable() = default;
};

} // namespace wjr

#endif // WJR_CRTP_NONMOVABLE_HPP__