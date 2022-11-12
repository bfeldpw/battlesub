#ifndef MATH_HELPER_HPP
#define MATH_HELPER_HPP

#include <cmath>

namespace bfe
{
namespace mh
{

constexpr double MATH_PI=3.141592653589793;
constexpr double MATH_2PI=2.0*MATH_PI;
constexpr float MATH_PI_F=3.141592653589793;
constexpr float MATH_2PI_F=2.0*MATH_PI;

inline float constrainAngle(float _Angle)
{
    _Angle = fmodf(_Angle+MATH_PI_F, MATH_2PI_F);
    if (_Angle < 0.0f) _Angle += MATH_2PI_F;
    return _Angle - MATH_PI_F;
}

} // namespace mh
} // namespace bfe

#endif // MATH_HELPER_HPP
