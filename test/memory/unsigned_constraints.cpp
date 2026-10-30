#include <wjr/memory/align.hpp>
#include <wjr/memory/detail.hpp>

#include <concepts>
#include <cstddef>
#include <cstdint>

namespace {

template <typename T>
concept can_align = requires(T value) {
    { wjr::align_down(value, 8) } noexcept -> std::same_as<T>;
    { wjr::align_up(value, 8) } noexcept -> std::same_as<T>;
    { wjr::align_down_offset(value, 8) } noexcept -> std::same_as<T>;
    { wjr::align_up_offset(value, 8) } noexcept -> std::same_as<T>;
    { wjr::is_aligned(value, 8) } noexcept -> std::same_as<bool>;
};

template <typename T>
concept can_access_unsigned_memory = requires(void *ptr, const void *source, T value) {
    { wjr::read_memory<T>(source) } noexcept -> std::same_as<T>;
    { wjr::write_memory(ptr, value) } noexcept -> std::same_as<void>;
    { wjr::byteswap(value) } noexcept -> std::same_as<T>;
};

template <typename T>
concept rejects_unsigned_operations =
    !requires(T value) { wjr::align_down(value, 8); } &&
    !requires(T value) { wjr::align_up(value, 8); } &&
    !requires(T value) { wjr::align_down_offset(value, 8); } &&
    !requires(T value) { wjr::align_up_offset(value, 8); } &&
    !requires(T value) { wjr::is_aligned(value, 8); } &&
    !requires(const void *ptr) { wjr::read_memory<T>(ptr); } &&
    !requires(void *ptr, T value) { wjr::write_memory(ptr, value); } &&
    !requires(T value) { wjr::byteswap(value); };

enum class unsigned_enum : unsigned {};

struct unsigned_convertible {
    operator unsigned() const;
};

static_assert(can_align<unsigned char> && can_access_unsigned_memory<unsigned char>);
static_assert(can_align<unsigned short> && can_access_unsigned_memory<unsigned short>);
static_assert(can_align<unsigned> && can_access_unsigned_memory<unsigned>);
static_assert(can_align<unsigned long> && can_access_unsigned_memory<unsigned long>);
static_assert(can_align<unsigned long long> && can_access_unsigned_memory<unsigned long long>);
static_assert(rejects_unsigned_operations<bool>);
static_assert(rejects_unsigned_operations<signed char>);
static_assert(rejects_unsigned_operations<short>);
static_assert(rejects_unsigned_operations<int>);
static_assert(rejects_unsigned_operations<long>);
static_assert(rejects_unsigned_operations<long long>);
static_assert(rejects_unsigned_operations<double>);
static_assert(rejects_unsigned_operations<std::byte>);
static_assert(rejects_unsigned_operations<unsigned_enum>);
static_assert(rejects_unsigned_operations<unsigned_convertible>);
static_assert(rejects_unsigned_operations<unsigned *>);

static_assert(wjr::align_down(19u, 8) == 16u);
static_assert(wjr::align_up(19u, 8) == 24u);
static_assert(wjr::align_down_offset(19u, 8) == 3u);
static_assert(wjr::align_up_offset(19u, 8) == 5u);
static_assert(wjr::is_aligned(24u, 8));
static_assert(!wjr::is_aligned(19u, 8));
static_assert(wjr::align_up(std::uint8_t{19}, 8) == std::uint8_t{24});

} // namespace
