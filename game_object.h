#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <iostream>

#include <Box2D/Box2D.h>

#include "common.h"
#include "entity.h"

class GameObject : public Entity
{
    public:
        
        void create(b2World* World, Scene2D* Scene)
        {
            World_ = World;
            Scene_ = Scene;
            
            Visuals_ = new Object2D(Scene);
        }
        
        void destroy()
        {
            // Destroy physics data, Box2D will handle everything from here
            World_->DestroyBody(Body_);
            
            // Destroy graphics data, Magnum will handle everything from here
            // (including drawables)
            if (Visuals_ != nullptr)
            {
                delete Visuals_;
                Visuals_ = nullptr;
            }
        }
        
        b2Body*         getBody() const {return Body_;}
        Object2D&       getVisuals() {return *Visuals_;}

        void init(const b2BodyDef& BodyDef)
        {
            std::cout << "PhysObj::init" << std::endl;
            Body_=World_->CreateBody(&BodyDef);
            Body_->SetUserData(Visuals_);
        }
        
    protected:
        
        b2Body*  Body_      = nullptr;
        b2World* World_     = nullptr;
        
        Object2D* Visuals_  = nullptr;
        Scene2D* Scene_     = nullptr;
        
};

#endif // GAME_OBJECT_H
