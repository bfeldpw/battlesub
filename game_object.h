#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <Box2D/Box2D.h>

#include "common.h"
#include "entity.h"

class GameObject : public Entity
{
    public:
        
        GameObject(b2World& World, Scene2D& Scene) : World_(&World), Visuals_(&Scene)
        {
//             b2BodyDef BodyDef;
//             BodyDef.active = false;
//             Body_ = World.CreateBody(&BodyDef);
//             Body_->SetUserData(this);
            
            std::cout << ID << std::endl;
        }
        
        const b2Body* const getBody() const {return Body_;}
        Object2D&           getVisuals() {return Visuals_;}

        void init(const b2BodyDef& BodyDef)
        {
            std::cout << "PhysObj::init" << std::endl;
            Body_=World_->CreateBody(&BodyDef);
            Body_->SetUserData(&Visuals_);
            
            b2PolygonShape shape;
            shape.SetAsBox(1.0f, 1.0f);
            
            b2FixtureDef fixture;
            fixture.friction = 0.8f;
            fixture.density = 1.0f;
            fixture.shape = &shape;
            Body_->CreateFixture(&fixture);
        }
        
    protected:
        
        b2Body*  Body_;
        b2World* const World_;
        Object2D Visuals_;
        
};

#endif // GAME_OBJECT_H
