#ifndef SUBMARINE_RUDDER_H
#define SUBMARINE_RUDDER_H

#include "game_object.h"
#include "submarine_drawable.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

class SubmarineRudder : public GameObject
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
            
            for (auto i=0u; i<Meshes_->size(); ++i)
            {
                new SubmarineDrawable(Visuals_, &((*Meshes_)[i]), Shader_, 0x2f83cc_rgbf, DrawableGrp_);
            }
        }
        
        void update();
        
    private:
        
};

#endif // SUBMARINE_RUDDER_H
