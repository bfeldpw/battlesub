#ifndef SUB_H
#define SUB_H

#include "game_object.h"
#include "projectile.h"

class Sub : public GameObject
{
    public:
        
        Sub(b2World& World, Scene2D& Scene) : GameObject(World, Scene)/*, Fire(World) */{}
        
        void init(const b2BodyDef BodyDef)
        {
            std::cout << "Sub::init" << std::endl;
            
            GameObject::init(BodyDef);
//             Fire.init(BodyDef);
        }
        
        void rudderLeft()
        {
            Rudder_ = -0.1f;
        }
        void rudderRight()
        {
            Rudder_ = 0.1f;
        }
        void throttleForward()
        {
            Throttle_ = 0.2f;
            
        }
        void throttleReverse()
        {
            Throttle_ = -0.2f;
        }
        
        void update()
        {
            this->Body_->ApplyForce(this->Body_->GetWorldVector({Rudder_, 0.0f}),
                                    this->Body_->GetWorldPoint({0.0f, 1.0f}), true);
            this->Body_->ApplyForce(this->Body_->GetWorldVector({0.0f, Throttle_}),
                                    this->Body_->GetWorldPoint({0.0f, 0.0f}), true);
        }
        
        
    private:
        
//         Projectile  Fire;
        float Rudder_ = 0.0f;
        float Throttle_ = 0.0f;
        
};

#endif // SUB_H
