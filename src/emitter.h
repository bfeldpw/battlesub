#ifndef EMITTER_H
#define EMITTER_H

#include <random>

#include <entt/entity/registry.hpp>
#include <Box2D/Box2D.h>

#include "entity.h"
#include "global_factories.h"
#include "global_resources.h"

class Emitter : public Entity
{
    
    public:
    
        void destroy() {}
        void emit(entt::registry& _Reg);

        bool isFinished() {return Counter_ >= Number_;}
        void setFrequency(float Frequency) {Frequency_ = Frequency;}
        
        Emitter& setAngle(float a)
        {
            Angle_ = a;
            return *this;
        }
        Emitter& setAngleStdDev(float as)
        {
            AngleStdDev_ = as;
            return *this;
        }
        Emitter& setNumber(int n)
        {
            Number_ = n;
            return *this;
        }
        Emitter& setOrigin(float x, float y)
        {
            OriginX_ = x;
            OriginY_ = y;
            return *this;
        }
        Emitter& setScale(float s)
        {
            Scale_ = s;
            return *this;
        }
        Emitter& setScaleStdDev(float s)
        {
            ScaleStdDev_ = s;
            return *this;
        }
        Emitter& setType(GameObjectTypeE t)
        {
            Type_ = t;
            return *this;
        }
        Emitter& setVelocity(float v)
        {
            Velocity_ = v;
            return *this;
        }
        Emitter& setVelocityStdDev(float vs)
        {
            VelocityStdDev_ = vs;
            return *this;
        }
    
    private:

        static std::mt19937 Generator_;

        GameObjectTypeE Type_ = GameObjectTypeE::DEBRIS_LANDSCAPE;
        
        float  Angle_ = 0.0f;
        float  AngleStdDev_ = 0.0f;
        float  Frequency_ = 0.0f;
        int    Counter_ = 0;
        int    Number_ = 1;
        float  OriginX_ = 0.0f;
        float  OriginY_ = 0.0f;
        float  Scale_ = 1.0f;
        float  ScaleStdDev_ = 0.0f;
        float  Velocity_ = 1.0f;
        float  VelocityStdDev_ = 0.0f;
};

#endif // EMITTER_H
