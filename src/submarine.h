#ifndef SUBMARINE_H
#define SUBMARINE_H

#include <box2d.h>
#include <entt/entity/registry.hpp>

#include "entity.h"
#include "game_object_factory.hpp"
#include "timer.h"

class Submarine : public Entity
{
    public:
        
        // GameObject       Hull;
        // GameObject       Rudder;
        entt::entity Hull;
        entt::entity Rudder;

        Submarine()
        {
            GunRateTimer.start();
        }

        Submarine& setPose(float PosX, float PosY, float Angle=0.0f)
        {
            if (IsInitialised)
            {
                // Hull.getBody()->SetTransform({PosX, PosY}, Angle);
                // Rudder.getBody()->SetTransform({PosX*std::cos(Angle), (PosY-15.0f)*std::sin(Angle)}, Angle);
            }
            else
            {
                b2BodyDef Bodydef;
                Bodydef.position = {PosX, PosY};
                Bodydef.angle    = Angle;
            }
            return *this;
        }
        
        float getThrottle() const {return Throttle_;}
        float getHullIntegrity() const {return HullIntegrity_;}
        void impact(const float Strength)
        {
            HullIntegrity_ -= Strength;
            if (HullIntegrity_ < 0.0f) HullIntegrity_ = 0.0f;
        }

        void create(entt::registry& _Reg, const float PosX, const float PosY, const float Angle);
        void fire(entt::registry& _Reg);
        
        void fullStop()
        {
            Throttle_ = 0.0f;
        }
        void rudderLeft()
        {
            RudderJoint->SetMotorSpeed(-1.0f);
            Rudder_ = -10.0f;
        }
        void rudderRight()
        {
            RudderJoint->SetMotorSpeed(1.0f);
            Rudder_ = 10.0f;
        }
        void rudderStop()
        {
            RudderJoint->SetMotorSpeed(0.0f);
            Rudder_ = 0.0f;
        }
        void throttleForward()
        {
            if (Throttle_ < 60000.0f) Throttle_ += 2000.0f;
        }
        void throttleReverse()
        {
            if (Throttle_ > -60000.0f) Throttle_ -= 2000.0f;
        }
        
        void update(entt::registry& _Reg);
        
    private:

        constexpr static bool GUN_LEFT=false;
        constexpr static bool GUN_RIGHT=true;
        bool GunSide = GUN_LEFT;
        Timer GunRateTimer;

        b2RevoluteJoint* RudderJoint = nullptr;

        bool IsInitialised = false;

        float Rudder_   = 0.0f;
        float Throttle_ = 0.0f;
        
        float HullIntegrity_ = 100.0f;
        
};

#endif // SUBMARINE_H
