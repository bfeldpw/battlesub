#include "game_object.h"

void GameObject::destroy()
{
    // Stop timer, since objects will be reused from pool
    Age_.stop();
    
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

GameObject& GameObject::setScale(float X, float Y)
{
    assert(Visuals_ != nullptr);
    ScaleX_ = X;
    ScaleY_ = Y;
    Visuals_->setScaling({X, Y});
    return *this;
}

void GameObject::init(const GameObjectTypeE Type, const b2BodyDef& BodyDef)
{
    assert(World_ != nullptr);
    assert(Shapes_ != nullptr);
    assert(Meshes_ != nullptr);
    assert(Shader_ != nullptr);
    assert(Scene_ != nullptr);
    assert(DrawableGrp_ != nullptr);
    
    Age_.restart();
    
    Type_ = Type;
    
    Visuals_ = new Object2D(Scene_);
    
    Body_=World_->CreateBody(&BodyDef);
    Body_->SetUserData(this);
    
    for (auto i=0u; i<Shapes_->ShapeDefs.size(); ++i)
    {
        // Shape has to be defined here, since CreateFixture needs a
        // valid Shape (Box2D internal cloning is done within
        // CreateFixture while Fixture is just a structure).
        switch(Shapes_->Type)
        {
            case ShapeEnumType::CIRCLE:
            {
                b2CircleShape Circle;
                Circle.m_p.Set(Shapes_->ShapeDefs[i][0].x, Shapes_->ShapeDefs[i][0].y);
                Circle.m_radius = Shapes_->ShapeDefs[i][1].x;
                Shapes_->FixtureDefs[i].shape = &Circle;
                Body_->CreateFixture(&(Shapes_->FixtureDefs[i]));
                break;
            }
            case ShapeEnumType::CHAIN:
            {
                b2ChainShape Chain;
                Chain.CreateLoop(Shapes_->ShapeDefs[i].data(), Shapes_->ShapeDefs[i].size());
                Shapes_->FixtureDefs[i].shape = &Chain;
                Body_->CreateFixture(&(Shapes_->FixtureDefs[i]));
                break;
            }
            case ShapeEnumType::POLYGON:
            {
                b2PolygonShape Shp;
                Shp.Set(Shapes_->ShapeDefs[i].data(), Shapes_->ShapeDefs[i].size());
                Shapes_->FixtureDefs[i].shape = &Shp;
                Body_->CreateFixture(&(Shapes_->FixtureDefs[i]));
            }
        }
        
    }
    
    ScaleX_  = 1.0f;
    ScaleY_  = 1.0f;
    ScaleSinkColor = 1.0f;
    IsSunk_ = false;

    for (auto i=0u; i<Meshes_->size(); ++i)
    {
        Drawable = new DrawableGeneric(Visuals_, &((*Meshes_)[i]), Shader_, Color_, DrawableGrp_);
    }
    
    DBLK(GlobalMessageHandler.reportDebug("GameObject created\n"
                                          " * Mass:    "+std::to_string(Body_->GetMass())+" kg\n"
                                          " * Inertia: "+std::to_string(Body_->GetInertia()),
                                          MessageHandler::DEBUG_L1);)
}

void GameObject::sink()
{
    // Just change the visuals
    ScaleX_ *= SINKING_SCALE_FACTOR;
    ScaleY_ *= SINKING_SCALE_FACTOR;
    ScaleSinkColor *= SINKING_SCALE_FACTOR_COLOR;
    if (ScaleX_ <= SINKING_SCALE_MIN && ScaleY_ <= SINKING_SCALE_MIN) IsSunk_ = true;
    Visuals_->setScaling({ScaleX_, ScaleY_});
    
    Drawable->setColorScale(ScaleSinkColor);

    // if (ScaleX_ < 0.8f || ScaleY_ < 0.8f)
    Body_->SetLinearVelocity({0.0f, 0.0f});
    Body_->SetActive(false);
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
