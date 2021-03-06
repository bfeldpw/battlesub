#include "camera_dynamics.h"

void CameraDynamics::increaseByMultiplication(const float f)
{
    if (!IsAuto)
    {
        Auto = 0.0f;
        Base *= f;
    }
}

void CameraDynamics::interpolate(float Target)
{
    Target *= Strength;
    if (IsAuto)
    {
        constexpr float Epsilon = 0.001f;

        if (Target != Target_)
        {
            Phase_ = Speed;
            Target_ = Target;
        }
        else
        {
            Phase_ += Speed;
        }
        To_.point() = Target;
        To_.inTangent() = 0.0f;
        To_.outTangent() = 0.0f;

        // Test for values near zero to fix oscillations
        if (std::abs(Target- Last0_) < Epsilon && Target < Epsilon)
        {
            Last0_ = Target;
            Last1_ = Target;
            From_.point() = Target;
            From_.inTangent() = 0.0f;
            From_.outTangent() = 0.0f;
        }
        else
        {
            From_.point() = Last0_;
            From_.inTangent() = (Last0_ - Last1_) / Speed;
            From_.outTangent() = (Last0_ - Last1_) / Speed;
            Auto = Magnum::Math::splerp<float, float>(From_, To_, Phase_);
            Last1_ = Last0_;
            Last0_ = Auto;
        }

        Values.push_front(Base+Auto);
        if (Values.size() > 600) Values.pop_back();
    }
    else
    {
        Auto = 0.0f;
    }
}
