#ifndef SUBMARINE_HULL_H
#define SUBMARINE_HULL_H

#include "game_object.h"
#include "submarine_drawable.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

class SubmarineHull : public GameObject
{
    public:
        
        void init(b2World* World, Scene2D* Scene, const b2BodyDef BodyDef, SceneGraph::DrawableGroup2D* DGrp)
        {
            GameObject::init(World, Scene, BodyDef, DGrp);
            
            for (auto i=0u; i<Meshes_->size(); ++i)
            {
                new SubmarineDrawable(Visuals_, &((*Meshes_)[i]), Shader_, 0x2f83cc_rgbf, DrawableGrp_);
            }
        }
        
        void update();
        
    private:
        
};

#endif // SUBMARINE_HULL_H
