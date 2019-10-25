#ifndef SUB_H
#define SUB_H

#include "game_object.h"
#include "resource_storage.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

class Sub : public GameObject
{
    public:
        
        void init(b2World* World, Scene2D* Scene, const b2BodyDef BodyDef)
        {
            GameObject::init(World, Scene, BodyDef);
            
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
        }
        
        void fire(Shaders::Flat2D& Shader, SceneGraph::DrawableGroup2D& Drawables,
                  float GunPos, ResourceStorage* Resources);
        
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
