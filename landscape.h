#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include <vector>

#include "game_object.h"
#include "landscape_drawable.h"
#include "resource_storage.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

class Landscape : public GameObject
{
    public:
        
        void init(b2World* World, Scene2D* Scene, const b2BodyDef BodyDef, SceneGraph::DrawableGroup2D* DGrp)
        {
            GameObject::init(World, Scene, BodyDef, DGrp);
            
            // Create physical shape and convert to graphics
            std::vector<b2Vec2> Verts;
            Verts.push_back({-500.0f,  300.0f});
            Verts.push_back({ 500.0f,  300.0f});
            Verts.push_back({ 500.0f, -300.0f});
            Verts.push_back({-500.0f, -300.0f});
            
            b2ChainShape Chain;
            Chain.CreateChain(Verts.data(), Verts.size());
            
            b2FixtureDef fixture;
            fixture.friction = 0.8f;
            fixture.shape = &Chain;
            Body_->CreateFixture(&fixture);
            
            new LandscapeDrawable(*Visuals_, ResourceStorage::Global.getMeshLandscape(), Shader_, 0xcccccc_rgbf, *DrawableGrp_);
        }
        
};

#endif // LANDSCAPE_H
