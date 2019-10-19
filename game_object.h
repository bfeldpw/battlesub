#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <iostream>

#include <Box2D/Box2D.h>

#include "common.h"
#include "entity.h"

class GameObject : public Entity
{
    public:
        
        GameObject() : Visuals_(new Object2D) {}
//         GameObject(b2World* World, Scene2D* Scene) : World_(World), Visuals_(Scene), Scene_(Scene)
//         {
// //             b2BodyDef BodyDef;
// //             BodyDef.active = false;
// //             Body_ = World.CreateBody(&BodyDef);
// //             Body_->SetUserData(this);
//             std::cout << ID << std::endl;
//         }
        
        void create(b2World* World, Scene2D* Scene)
        {
            World_ = World;
            Scene_ = Scene;
            
            Visuals_->setParent(Scene);
        }
        
        b2Body* const   getBody() const {return Body_;}
        Object2D&       getVisuals() {return *Visuals_;}

        void init(const b2BodyDef& BodyDef)
        {
            std::cout << "PhysObj::init" << std::endl;
            Body_=World_->CreateBody(&BodyDef);
            Body_->SetUserData(Visuals_);
        }
        
    protected:
        
        b2Body*  Body_  = nullptr;
        b2World* World_ = nullptr;
        
        Object2D* Visuals_;
        Scene2D* Scene_ = nullptr;
        
};

#endif // GAME_OBJECT_H
