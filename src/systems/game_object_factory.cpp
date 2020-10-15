#include "game_object_factory.hpp"

#include "message_handler.h"
#include "parent_component.hpp"

void GameObjectFactory::create(entt::entity e, std::any _Parent, const GameObjectTypeE _GameObjectType,
                                const DrawableGroupsTypeE _DrawableGroupsType, Color4 _Col,
                                const b2BodyDef& _BodyDef)
{
    auto& Parent  = Reg_.emplace<ParentComponent>(e);
    auto& Physics = Reg_.emplace<PhysicsComponent>(e);
    auto& Status  = Reg_.emplace<StatusComponent>(e);
    auto& Visuals = Reg_.emplace<VisualsComponent>(e);

    Parent.Parent_ = _Parent;

    Physics.World_  = GlobalResources::Get.getWorld();
    Physics.Shapes_ = GlobalResources::Get.getShapes(_GameObjectType);
    assert(Physics.World_ != nullptr);
    assert(Physics.Shapes_ != nullptr);
    Physics.Body_   = Physics.World_->CreateBody(&_BodyDef);

    if (EntityIDs.CountFree_ > 0)
    {
        EntityIDs.IDs_[EntityIDs.FreeIDs_[EntityIDs.CountFree_-1]] = entt::id_type(e);

        EntityIDs.IDMap_[id(e)] = EntityIDs.FreeIDs_[EntityIDs.CountFree_-1];
        Physics.Body_->SetUserData(&EntityIDs.IDs_[EntityIDs.FreeIDs_[EntityIDs.CountFree_-1]]);
        EntityIDs.CountFree_--;
    }
    else
    {
        EntityIDs.IDs_[EntityIDs.Count_] = entt::id_type(e);
        EntityIDs.IDMap_[id(e)] = EntityIDs.Count_;
        Physics.Body_->SetUserData(&EntityIDs.IDs_[EntityIDs.Count_++]);
    }

    DBLK(printInternalEntityLists();)

    this->createShapes(Physics.Body_, Physics.Shapes_);

    Status.Age_.restart();
    Status.Type_ = _GameObjectType;

    Visuals.Color_       = _Col;
    Visuals.DrawableGrp_ = GlobalResources::Get.getDrawables(_DrawableGroupsType);
    Visuals.Meshes_      = GlobalResources::Get.getMeshes(_GameObjectType);
    Visuals.Scene_       = GlobalResources::Get.getScene();
    Visuals.Shader_      = GlobalResources::Get.getShader();
    assert(Visuals.DrawableGrp_ != nullptr);
    assert(Visuals.Meshes_ != nullptr);
    assert(Visuals.Scene_ != nullptr);
    assert(Visuals.Shader_ != nullptr);
    Visuals.Visuals_     = new Object2D(Visuals.Scene_);

    Visuals.Visuals_->setTranslation({Physics.Body_->GetPosition().x, Physics.Body_->GetPosition().y})
                     .setRotation(Complex::rotation(Rad(Physics.Body_->GetAngle())));

    for (auto i=0u; i<Visuals.Meshes_->size(); ++i)
    {
        Visuals.Drawable_ = new DrawableGeneric(Visuals.Visuals_, &((*Visuals.Meshes_)[i]),
                            Visuals.Shader_, Visuals.Color_,
                            Visuals.DrawableGrp_);
    }
}

void GameObjectFactory::createShapes(b2Body* Body, ShapesType* Shapes)
{
    for (auto i=0u; i<Shapes->ShapeDefs.size(); ++i)
    {
        // Shape has to be defined here, since CreateFixture needs a
        // valid Shape (Box2D internal cloning is done within
        // CreateFixture while Fixture is just a structure).
        switch(Shapes->Type)
        {
            case ShapeEnumType::CIRCLE:
            {
                b2CircleShape Circle;
                Circle.m_p.Set(Shapes->ShapeDefs[i][0].x, Shapes->ShapeDefs[i][0].y);
                Circle.m_radius = Shapes->ShapeDefs[i][1].x;
                Shapes->FixtureDefs[i].shape = &Circle;
                Body->CreateFixture(&(Shapes->FixtureDefs[i]));
                break;
            }
            case ShapeEnumType::CHAIN:
            {
                b2ChainShape Chain;
                Chain.CreateLoop(Shapes->ShapeDefs[i].data(), Shapes->ShapeDefs[i].size());
                Shapes->FixtureDefs[i].shape = &Chain;
                Body->CreateFixture(&(Shapes->FixtureDefs[i]));
                break;
            }
            case ShapeEnumType::POLYGON:
            {
                b2PolygonShape Shp;
                Shp.Set(Shapes->ShapeDefs[i].data(), Shapes->ShapeDefs[i].size());
                Shapes->FixtureDefs[i].shape = &Shp;
                Body->CreateFixture(&(Shapes->FixtureDefs[i]));
            }
        }
    }
}

void GameObjectFactory::updateStatus()
{
    Reg_.view<PhysicsComponent, StatusComponent, VisualsComponent>().each(
        [this](auto& _PhysComp, auto& _StatusComp, auto& _VisComp)
    {
        if (_StatusComp.Type_ == GameObjectTypeE::PROJECTILE ||
            _StatusComp.Type_ == GameObjectTypeE::DEBRIS_LANDSCAPE)
        {
            if (!_StatusComp.IsSinking_ &&
                (_StatusComp.Age_.time() > 5.0f ||
                _PhysComp.Body_->GetLinearVelocity().Length() < 0.01f))
            {
                _StatusComp.IsSinking_ = true;
                _StatusComp.Age_.restart();
            }
            if (_StatusComp.IsSinking_)
            {
                _VisComp.Color_.r() *= 1.0f/(1.0f+_StatusComp.Age_.time()*0.01f);
                _VisComp.Color_.g() *= 1.0f/(1.0f+_StatusComp.Age_.time()*0.01f);
                _VisComp.Color_.a() *= 1.0f/(1.0f+_StatusComp.Age_.time()*0.01f);
                _VisComp.Drawable_->setColor(_VisComp.Color_);
                // _PhysComp.Body_->SetLinearVelocity({0.0f, 0.0f});
                // _PhysComp.Body_->SetActive(false);
                // _StatusComp.Age_.stop();

                // // Destroy physics data, Box2D will handle everything from here
                // _PhysComp.World_->DestroyBody(_PhysComp.Body_);

                // // Destroy graphics data, Magnum will handle everything from here
                // // (including drawables)
                // if (_VisComp.Visuals_ != nullptr)
                // {
                //     delete _VisComp.Visuals_;
                //     _VisComp.Visuals_ = nullptr;
                // }
                // auto e = *static_cast<entt::entity*>(_PhysComp.Body_->GetUserData());

                // EntityIDs.CountFree_++;
                // EntityIDs.FreeIDs_[EntityIDs.CountFree_-1] = EntityIDs.IDMap_[id(e)];

                // DBLK(printInternalEntityLists();)

                // Reg_.destroy(e);
            }
        }
    });
}

void GameObjectFactory::updateVisuals()
{
    Reg_.view<PhysicsComponent, VisualsComponent>().each([](auto& _PhysComp, auto& _VisComp)
    {
        b2Body*   const Body    = _PhysComp.Body_;
        Object2D* const Visuals = _VisComp.Visuals_;

        if (Body->IsActive() && Body->GetType() != b2_staticBody)
        {
            Visuals->setTranslation({Body->GetPosition().x, Body->GetPosition().y})
                    .setRotation(Complex::rotation(Rad(Body->GetAngle())));
        }
    });
}


DBLK(
    void GameObjectFactory::printInternalEntityLists() const
    {
        auto& Msg = Reg_.ctx<MessageHandler>();
        Msg.reportDebugRaw("ID:      ", MessageHandler::DEBUG_L2);
        for (auto i=0; i<EntityIDs.Count_; ++i)
        {
            Msg.reportDebugRaw(std::to_string(EntityIDs.IDs_[i]) + " ", MessageHandler::DEBUG_L2);
        }
        Msg.reportDebugRaw("\n", MessageHandler::DEBUG_L2);
        Msg.reportDebugRaw("ID Map:  ", MessageHandler::DEBUG_L2);
        for (auto i=0; i<EntityIDs.Count_; ++i)
        {
            Msg.reportDebugRaw(std::to_string(EntityIDs.IDMap_[i]) + " ", MessageHandler::DEBUG_L2);
        }
        Msg.reportDebugRaw("\n", MessageHandler::DEBUG_L2);
        Msg.reportDebugRaw("ID Free: ", MessageHandler::DEBUG_L2);
        for (auto i=0; i<EntityIDs.CountFree_; ++i)
        {
            Msg.reportDebugRaw(std::to_string(EntityIDs.FreeIDs_[i]) + " ", MessageHandler::DEBUG_L2);
        }
        Msg.reportDebugRaw("\n", MessageHandler::DEBUG_L2);
    }
)
