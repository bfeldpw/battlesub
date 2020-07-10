#ifndef CAMERA_DYNAMICS_H
#define CAMERA_DYNAMICS_H

#include <deque>
#include <Magnum/Magnum.h>
#include <Magnum/Math/CubicHermite.h>

using namespace Magnum;

class CameraMovement
{

    public:

        void  interpolate(Vector2 Target);

        Vector2 Value() {return Auto;}

        Vector2 Auto{Math::ZeroInit};
        float   Speed = 0.1f;
        float   Strength = 0.25f;
        bool    IsAuto = true;

        // The following deque is only for debug visualisation (i.e. to see if splines work)
        std::deque<Vector2> Values;

    private:

        Vector2 Last0_{Math::ZeroInit};
        Vector2 Last1_{Math::ZeroInit};
        Vector2 Min_{Math::ZeroInit};
        Vector2 Max_{Math::ZeroInit};
        Vector2 Target_{Math::ZeroInit};
        float   Phase_ = 0.1f;

        template<class T> using Pt = Magnum::Math::CubicHermite2D<T>;

        Pt<float> From_;
        Pt<float> To_;
};

class CameraZoom
{

    public:

        void  increaseByMultiplication(const float f);
        void  interpolate(float Target);

        float Value() {return Base_+Auto;}

        float Auto = 0.0f;
        float Speed = 0.1f;
        float Strength = 0.005f;
        bool  IsAuto = true;

        // The following deque is only for debug visualisation (i.e. to see if splines work)
        std::deque<float> Values;

    private:

        float Base_   = 1.0f;
        float Last0_  = 0.0f;
        float Last1_  = 0.0f;
        float Min_    = 0.1f;
        float Max_    = 5.0f;
        float Target_ = 0.0f;
        float Phase_  = 1.0f;

        template<class T> using Pt = Magnum::Math::CubicHermite1D<T>;

        Pt<float> From_;
        Pt<float> To_;
};

#endif // CAMERA_DYNAMICS_H
