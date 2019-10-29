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
            
            for (auto Shape : *Shapes_)
            {
                b2PolygonShape Shp;
                Shp.Set(Shape.data(), Shape.size());
                
                b2FixtureDef fixture;
                fixture.friction = 0.8f;
                fixture.density = 1.0f;
                fixture.shape = &Shp;
                
                Body_->CreateFixture(&fixture);
            }
            
            for (auto i=0; i<Meshes_->size(); ++i)
            {
                new SubDrawable(*Visuals_, &((*Meshes_)[i]), Shader_, 0x2f83cc_rgbf, *DrawableGrp_);
            }
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
