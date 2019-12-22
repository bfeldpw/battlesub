#ifndef SUBMARINE_H
#define SUBMARINE_H

#include "entity.h"
#include "game_object.h"

class Submarine : public Entity
{
    public:
        
        GameObject       Hull;
        GameObject       Rudder;
        b2RevoluteJoint* RudderJoint = nullptr;
        
        Submarine& setPose(float PosX, float PosY, float Angle=0.0f)
        {
            if (IsInitialised)
            {
                Hull.getBody()->SetTransform({PosX, PosY}, Angle);
                Rudder.getBody()->SetTransform({PosX*std::cos(Angle), (PosY-15.0f)*std::sin(Angle)}, Angle);
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
        
        void fire(float GunPos);
        
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
        void throttleForward()
        {
            if (Throttle_ < 60000.0f) Throttle_ += 2000.0f;
        }
        void throttleReverse()
        {
            if (Throttle_ > -60000.0f) Throttle_ -= 2000.0f;
        }
        
        void update();
        
    private:

        bool IsInitialised = false;
        
        float Rudder_   = 0.0f;
        float Throttle_ = 0.0f;
        
        float HitPoints_ = 1000.0f;
        
};

#endif // SUBMARINE_H
