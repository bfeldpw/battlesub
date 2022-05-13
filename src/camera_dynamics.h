#ifndef CAMERA_DYNAMICS_H
#define CAMERA_DYNAMICS_H

#include <deque>
#include <Magnum/Magnum.h>
#include <Magnum/Math/CubicHermite.h>

using namespace Magnum;

class CameraDynamics
{

    public:

        void  increaseByMultiplication(const float f);
        void  interpolate(float Target);

        float Value() {return Base+Auto;}

        float Auto = 0.0f;
        float Base = 1.0f;
        float Speed = 0.1f;
        float Strength = 0.1f;
        bool  IsAuto = true;

        // The following deque is only for debug visualisation (i.e. to see if splines work)
        std::deque<float> Values;

    private:

        float Last0_  = 0.0f;
        float Last1_  = 0.0f;
        float Target_ = 0.0f;
        float Phase_  = 1.0f;

        template<class T> using Pt = Magnum::Math::CubicHermite1D<T>;

        Pt<float> From_;
        Pt<float> To_;
};

#endif // CAMERA_DYNAMICS_H
