#ifndef SUBMARINE_H
#define SUBMARINE_H

#include "entity.h"
#include "submarine_hull.h"
#include "submarine_rudder.h"

class Submarine : public Entity
{
    public:
        
        SubmarineHull Hull;
        SubmarineHull Rudder;
        
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
        
        void fire(float GunPos);
        
        void rudderLeft()
        {
            Rudder_ = -10.0f;
        }
        void rudderRight()
        {
            Rudder_ = 10.0f;
        }
        void throttleForward()
        {
            Throttle_ = 20.0f;
        }
        void throttleReverse()
        {
            Throttle_ = -20.0f;
        }
        
        void update();
        
    private:

        bool IsInitialised = false;
        
        float Rudder_   = 0.0f;
        float Throttle_ = 0.0f;
        
};

#endif // SUBMARINE_H
