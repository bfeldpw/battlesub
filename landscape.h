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
            
            for (auto Shape : *Shapes_)
            {
                b2ChainShape Chain;
                Chain.CreateLoop(Shape.data(), Shape.size());
            
                b2FixtureDef fixture;
                fixture.friction = 0.8f;
                fixture.shape = &Chain;
                Body_->CreateFixture(&fixture);
            }
            
            for (auto i=0; i<Meshes_->size(); ++i)
            {
                new LandscapeDrawable(*Visuals_, &((*Meshes_)[i]), Shader_, 0xcccccc_rgbf, *DrawableGrp_);
            }
        }
        
};

#endif // LANDSCAPE_H
