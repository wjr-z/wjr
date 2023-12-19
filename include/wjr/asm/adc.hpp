#ifndef WJR_ASM_ADC_HPP__
#define WJR_ASM_ADC_HPP__

#include <wjr/preprocessor.hpp>

namespace wjr {
namespace masm {

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_adc(T a, T b, T c_in, T &c_out) {
    
}

} // namespace masm
} // namespace wjr

#endif // WJR_ASM_ADC_HPP__