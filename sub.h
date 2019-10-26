#ifndef SUB_H
#define SUB_H

#include "game_object.h"
#include "sub_drawable.h"
#include "resource_storage.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

class Sub : public GameObject
{
    public:
        
        void init(b2World* World, Scene2D* Scene, const b2BodyDef BodyDef, SceneGraph::DrawableGroup2D* DGrp)
        {
            GameObject::init(World, Scene, BodyDef, DGrp);
            
            // Submarine size: 4m x 16m
            constexpr float SizeX = 2.0f;
            constexpr float SizeY = 8.0f;
            
            b2PolygonShape shape;
            shape.SetAsBox(SizeX, SizeY);
            
            b2FixtureDef fixture;
            fixture.friction = 0.8f;
            fixture.density = 1.0f;
            fixture.shape = &shape;
            
            Body_->CreateFixture(&fixture);
            
            Visuals_->setScaling({SizeX, SizeY});
            
            new SubDrawable(*Visuals_, ResourceStorage::Global.getMeshSub(), Shader_, 0x2f83cc_rgbf, *DrawableGrp_);
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
        
        void update(SceneGraph::DrawableGroup2D& Drawables);
        
    private:
        
        float Rudder_   = 0.0f;
        float Throttle_ = 0.0f;
        
};

#endif // SUB_H
