#include "game_object.h"

void GameObject::destroy()
{
    // Destroy physics data, Box2D will handle everything from here
    World_->DestroyBody(Body_);
    
    // Destroy graphics data, Magnum will handle everything from here
    // (including drawables)
    if (Visuals_ != nullptr)
    {
        delete Visuals_;
        Visuals_ = nullptr;
    }
}

void GameObject::init(b2World* World, Scene2D* Scene, const b2BodyDef& BodyDef, SceneGraph::DrawableGroup2D* const DGrp)
{
    World_ = World;
    Scene_ = Scene;
    Visuals_ = new Object2D(Scene);
    DrawableGrp_ = DGrp;
    
    Body_=World_->CreateBody(&BodyDef);
    Body_->SetUserData(Visuals_);
    
    Scale_  = 1.0f;
    IsSunk_ = false;
}

void GameObject::sink()
{
    // Just change the visuals
    Scale_ *= SINKING_SCALE_FACTOR;
    if (Scale_ <= SINKING_SCALE_MIN) IsSunk_ = true;
    Visuals_->setScaling({Scale_, Scale_});
    
    // Physics: Filter collisions to not collide with sinking objects
    for (auto Fixture = Body_->GetFixtureList(); Fixture; Fixture = Fixture->GetNext())
    {
        b2Filter Filter;
        Filter.maskBits = 0;
        Fixture->SetFilterData(Filter);
    }
}
        
std::vector<Vector2> GameObject::convertGeometryPhysicsToGraphics(const std::vector<b2Vec2> Verts)
{
    std::vector<Vector2> Tmp;
    
    for (auto Vec : Verts)
    {
        Tmp.push_back({Vec.x, Vec.y});
    }
    return Tmp; // Note: No std::move needed, might even prevent copy elision
}
