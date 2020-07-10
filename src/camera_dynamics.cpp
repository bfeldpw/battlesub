#include "camera_dynamics.h"

#include <iostream>

void CameraZoom::increaseByMultiplication(const float f)
{
    if (!IsAuto)
    {
        Auto = 0.0f;
        Base_ *= f;
        if (Value() < Min_) Base_ = Min_;
        if (Value() > Max_) Base_ = Max_;
    }
}

void CameraMovement::interpolate(Vector2 Target)
{
    Target *= Strength;
    if (IsAuto)
    {
        constexpr float Epsilon = 0.001f;

        // if (Base_+Target > Max_) Target = Max_ - Base_;
        // if (Base_+Target < Min_) Target = Min_ - Base_;
        // Will never be reached, since Base_ is larger than Min_ and
        // Target is always positive.

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
        To_.inTangent() = {0.0f, 0.0f};
        To_.outTangent() = {0.0f, 0.0f};

        // Test for values near zero to fix oscillations
        auto Diff = Target-Last0_;
        if ((std::abs(Diff[0]) < Epsilon && std::abs(Target[0] < Epsilon)) &&
            (std::abs(Diff[1]) < Epsilon && std::abs(Target[1] < Epsilon)))
        {
            Last0_ = Target;
            Last1_ = Target;
            From_.point() = Target;
            From_.inTangent() = {0.0f, 0.0f};
            From_.outTangent() = {0.0f, 0.0f};
        }
        else
        {
            From_.point() = Last0_;
            From_.inTangent() = (Last0_ - Last1_) / Speed;
            From_.outTangent() = (Last0_ - Last1_) / Speed;
            Auto = Magnum::Math::splerp<Vector2, float>(From_, To_, Phase_);
            // if (Auto > Max_) Auto = Max_;
            Last1_ = Last0_;
            Last0_ = Auto;
        }

        Values.push_front(Auto);
        if (Values.size() > 600) Values.pop_back();
    }
    else
    {
        Auto = {0.0f, 0.0f};
    }
}

void CameraZoom::interpolate(float Target)
{
    Target *= Strength;
    if (IsAuto)
    {
        constexpr float Epsilon = 0.001f;
        Base_ = 0.2f;

        if (Base_+Target > Max_) Target = Max_ - Base_;
        // if (Base_+Target < Min_) Target = Min_ - Base_;
        // Will never be reached, since Base_ is larger than Min_ and
        // Target is always positive.

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
            if (Base_+Auto > Max_) Auto = Max_ - Base_;
            Last1_ = Last0_;
            Last0_ = Auto;
        }

        Values.push_front(Base_+Auto);
        if (Values.size() > 600) Values.pop_back();
    }
    else
    {
        Auto = 0.0f;
    }
}
