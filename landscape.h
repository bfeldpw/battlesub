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
            
            for (auto i=0u; i<Meshes_->size(); ++i)
            {
                new LandscapeDrawable(Visuals_, &((*Meshes_)[i]), Shader_, 0x444455_rgbf, DrawableGrp_);
            }
        }
        
};

#endif // LANDSCAPE_H
