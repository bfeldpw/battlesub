#ifndef PHYS_OBJ_H
#define PHYS_OBJ_H

#include <Box2D/Box2D.h>

#include "common.h"
#include "entity.h"

class PhysObj : public Entity
{
    public:
        
        PhysObj(b2World& World) : World_(&World)
        {
//             b2BodyDef BodyDef;
//             BodyDef.active = false;
//             Body_ = World.CreateBody(&BodyDef);
//             Body_->SetUserData(this);
            
            std::cout << ID << std::endl;
        }
        
        const b2Body* const getBody() const {return Body_;}

        void init(const b2BodyDef& BodyDef)
        {
            std::cout << "PhysObj::init" << std::endl;
            Body_=World_->CreateBody(&BodyDef);
            Body_->SetUserData(this);
            
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
        
};

#endif // PHYS_OBJ_H
