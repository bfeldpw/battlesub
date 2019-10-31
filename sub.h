#ifndef SUB_H
#define SUB_H

#include "entity.h"
#include "submarine_hull.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

class Sub : public Entity
{
    public:
        
        SubmarineHull Hull;
        SubmarineHull Hull2;
        
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
        
        float Rudder_   = 0.0f;
        float Throttle_ = 0.0f;
        
};

#endif // SUB_H
