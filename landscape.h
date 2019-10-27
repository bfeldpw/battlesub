#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include <vector>

#include "game_object.h"
#include "landscape_drawable.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

class Landscape : public GameObject
{
    public:
        
        void init(b2World* World, Scene2D* Scene, const b2BodyDef BodyDef, SceneGraph::DrawableGroup2D* DGrp)
        {
            GameObject::init(World, Scene, BodyDef, DGrp);
            
            b2ChainShape Chain;
            Chain.CreateChain(Geometry_->data(), Geometry_->size());
            
            b2FixtureDef fixture;
            fixture.friction = 0.8f;
            fixture.shape = &Chain;
            Body_->CreateFixture(&fixture);
            
            new LandscapeDrawable(*Visuals_, Mesh_, Shader_, 0xcccccc_rgbf, *DrawableGrp_);
        }
        
};

#endif // LANDSCAPE_H
