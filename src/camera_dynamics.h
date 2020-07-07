#ifndef CAMERA_DYNAMICS_H
#define CAMERA_DYNAMICS_H

#include <deque>
#include <Magnum/Math/CubicHermite.h>

class CameraZoom
{

    public:

        void increaseByMultiplication(const float f)
        {
            if (!IsAuto)
            {
                Auto = 0.0f;
                Base_ *= f;
                if (Value() < Min) Base_ = Min;
                if (Value() > Max) Base_ = Max;
            }
        }

        void interpolate(float Target)
        {
            if (IsAuto)
            {
                Base_ = 0.2f;
                if (Base_+Target > Max) Target = Max - Base_;
                if (Base_+Target < Min) Target = Min - Base_;

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
                From_.point() = Last0_;
                From_.inTangent() = (Last0_ - Last1_) / Speed;
                From_.outTangent() = (Last0_ - Last1_) / Speed;
                        Auto = Magnum::Math::splerp<float, float>(From_, To_, Phase_);
                Last1_ = Last0_;
                Last0_ = Auto;

                Values.push_front(Base_+Auto);
                if (Values.size() > 2000) Values.pop_back();
            }
        }


        float Value() {return Base_+Auto;}

        float Auto = 0.0f;
        float Speed = 0.1f;
        bool  IsAuto = true;

        // The following deque is only for debug visualisation (i.e. to see if splines work)
        std::deque<float> Values;


    private:

        float Actual_ = 0.0f;
        float Base_   = 1.0f;
        float Last0_  = 0.0f;
        float Last1_  = 0.0f;
        float Min     = 0.1f;
        float Max     = 5.0f;
        float Target_ = 0.0f;
        float Phase_  = 1.0f;

        template<class T> using Pt = Magnum::Math::CubicHermite1D<T>;

        Pt<float> From_;
        Pt<float> To_;
};

#endif // CAMERA_DYNAMICS_H
