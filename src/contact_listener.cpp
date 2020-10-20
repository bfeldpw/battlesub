#include "contact_listener.h"

#include <algorithm>

#include "emitter_component.hpp"
#include "global_resources.h"

void ContactListener::PreSolve(b2Contact* Contact, const b2Manifold* OldManifold)
{
    b2PointState State1[2], State2[2];
    b2GetPointStates(State1, State2, OldManifold, Contact->GetManifold());

    bool IsValidContact = false;
    if (Contact->GetManifold()->pointCount == 1)
    {
        if (State2[0] == b2_addState) IsValidContact = true;
    }
    if (Contact->GetManifold()->pointCount == 2)
    {
        if ((State2[0] == b2_addState) || (State2[1] == b2_addState))
            IsValidContact = true;
    }

    // Only emit debris if new collision occurred
    if (IsValidContact)
    {
        GameObjectTypeE DebrisType = GameObjectTypeE::DEFAULT;

        auto EntityA = *(static_cast<entt::entity*>(Contact->GetFixtureA()->GetBody()->GetUserData()));
        auto EntityB = *(static_cast<entt::entity*>(Contact->GetFixtureB()->GetBody()->GetUserData()));
        auto& StatusA = Reg_.get<StatusComponent>(EntityA);
        auto& StatusB = Reg_.get<StatusComponent>(EntityB);
        b2Body* BodyA = Reg_.get<PhysicsComponent>(EntityA).Body_;
        b2Body* BodyB = Reg_.get<PhysicsComponent>(EntityB).Body_;

        b2Body* Body = nullptr;
        bool    IsContactKept = true;

        // ReactionA_.reset();
        // ReactionB_.reset();
        this->testGameObjectTypes(Contact, StatusA.Type_, StatusB.Type_, BodyA, BodyB);
        this->testGameObjectTypes(Contact, StatusB.Type_, StatusA.Type_, BodyB, BodyA);

        // else if (ObjA->getType() == GameObjectTypeE::SUBMARINE_HULL ||
        //          ObjA->getType() == GameObjectTypeE::SUBMARINE_RUDDER)
        // {
        //     if (ObjB->getType() == GameObjectTypeE::PROJECTILE)
        //     {
        //         Body = Contact->GetFixtureB()->GetBody();
        //         if (Body != nullptr)
        //         {
        //             b2WorldManifold WorldManifold;
        //             Contact->GetWorldManifold(&WorldManifold);
        //             {
        //                 b2Vec2 POC = WorldManifold.points[0];
        //                 b2Vec2 Velocity = Body->GetLinearVelocityFromWorldPoint(POC);

        //                 float AngleVel = std::atan2(Velocity.y, Velocity.x);
        //                 (AngleVel > 0.0f) ? AngleVel -= b2_pi : AngleVel += b2_pi;
        //                 float Angle =  std::atan2(WorldManifold.normal.y, WorldManifold.normal.x) - AngleVel;
        //                 if (Angle > -0.7f && Angle < 0.7f)
        //                 {
        //                     IsContactKept = false;
        //                     std::any_cast<Submarine*>(ObjA->getParent())->impact(2.0f);
        //                 }
        //             }
        //         }
        //         DebrisType = GameObjectTypeE::DEBRIS_SUBMARINE;
        //     }
        // }

        // if (ReactionA_.EmitLandscapeDebris_) this->emitLandscapeDebris(Contact, BodyB, BodyA);
        // if (ReactionB_.EmitLandscapeDebris_) this->emitLandscapeDebris(Contact, BodyA, BodyB);
        // if (ReactionA_.EmitSubmarineDebris_) this->emitSubmarineDebris(Contact, BodyA, BodyB);
        // if (ReactionB_.EmitSubmarineDebris_) this->emitSubmarineDebris(Contact, BodyB, BodyA);

        // if (!IsContactKept)
        // {
        //     Contact->SetEnabled(false);
        //     Body->SetLinearVelocity({0.0f, 0.0f}); // Be sure object will be disabled and sinking
        // }
    }
}

void ContactListener::emitLandscapeDebris(b2Contact* const _Contact, b2Body* const _Body1, b2Body* const _Body2)
{
    b2Body* Body = _Body2;
    b2WorldManifold WorldManifold;
    _Contact->GetWorldManifold(&WorldManifold);

    b2Vec2 POC = WorldManifold.points[0];
    b2Vec2 Velocity = Body->GetLinearVelocityFromWorldPoint(POC);
    float  Mass = Body->GetMass();

    float Angle = 2.0f * std::atan2(WorldManifold.normal.y, WorldManifold.normal.x) -
                        (std::atan2(Velocity.y, Velocity.x)+b2_pi);

    auto Emitter = Reg_.create();
    auto& EmComp = Reg_.emplace<EmitterComponent>(Emitter);
    EmComp.Type_ = GameObjectTypeE::DEBRIS_LANDSCAPE;
    EmComp.Angle_ = Angle;
    EmComp.AngleStdDev_ = 1.0f;
    EmComp.Frequency_ = 1.0f;
    EmComp.Number_ = 10;
    EmComp.OriginX_ = POC.x+WorldManifold.normal.x*0.2f;
    EmComp.OriginY_ = POC.y+WorldManifold.normal.y*0.2f;
    EmComp.Scale_= Velocity.Length()*0.05f+Mass*0.0001f;
    EmComp.ScaleStdDev_ = Velocity.Length()*0.05f+Mass*0.0001f;
    EmComp.Velocity_ = Velocity.Length()*0.02f+1.0f;
    EmComp.VelocityStdDev_ = Velocity.Length()*0.02f;
}

void ContactListener::emitSubmarineDebris(b2Contact* const _Contact,
                                          b2Body* const _Body1,
                                          b2Body* const _Body2,
                                          const GameObjectTypeE _TypeOther)
{
    constexpr float SCALE_SIZE_MAX = 1.0f;

    b2Body* Body = _Body1;
    b2WorldManifold WorldManifold;
    _Contact->GetWorldManifold(&WorldManifold);

    b2Vec2 POC = WorldManifold.points[0];

    b2Vec2 Impulse(0.0f, 0.0f);
    b2Vec2 Velocity(0.0f, 0.0f);
    switch (_TypeOther)
    {
        case GameObjectTypeE::LANDSCAPE:
            Velocity = _Body1->GetLinearVelocityFromWorldPoint(POC);
            Impulse  = _Body1->GetMass() * Velocity;

            break;
        case GameObjectTypeE::PROJECTILE:
            Velocity = (_Body2->GetLinearVelocityFromWorldPoint(POC)-
                        _Body1->GetLinearVelocityFromWorldPoint(POC));
            Impulse  =  _Body2->GetMass() * Velocity;
            break;
        case GameObjectTypeE::SUBMARINE_HULL:
        case GameObjectTypeE::SUBMARINE_RUDDER:
            Velocity = (_Body2->GetLinearVelocityFromWorldPoint(POC)-
                        _Body1->GetLinearVelocityFromWorldPoint(POC));
            Impulse  =  _Body2->GetMass() * Velocity;
            break;
    }

    float Scale = std::min(Impulse.Length()*1e-2f, SCALE_SIZE_MAX);

    float Angle = 2.0f * std::atan2(WorldManifold.normal.y, WorldManifold.normal.x) -
                        (std::atan2(Velocity.y, Velocity.x)+b2_pi);

    auto Emitter = Reg_.create();
    auto& EmComp = Reg_.emplace<EmitterComponent>(Emitter);
    EmComp.Type_ = GameObjectTypeE::DEBRIS_SUBMARINE;
    EmComp.Angle_ = Angle;
    EmComp.AngleStdDev_ = 1.0f;
    EmComp.Frequency_ = 1.0f;
    EmComp.Number_ = 10;
    EmComp.OriginX_ = POC.x+WorldManifold.normal.x*0.2f;
    EmComp.OriginY_ = POC.y+WorldManifold.normal.y*0.2f;
    EmComp.Scale_= Scale;
    EmComp.ScaleStdDev_ = Scale;
    EmComp.Velocity_ = Velocity.Length()*0.01f+1.0f;
    EmComp.VelocityStdDev_ = Velocity.Length()*0.01f;
}

void ContactListener::testGameObjectTypes(b2Contact* const _Contact, const GameObjectTypeE _Type1, const GameObjectTypeE _Type2,
                                          b2Body* const _Body1, b2Body* const _Body2)
{
    if (_Type1 == GameObjectTypeE::LANDSCAPE)
    {
        if (_Type2 == GameObjectTypeE::PROJECTILE)
        {
            this->emitLandscapeDebris(_Contact, _Body1, _Body2);
            _Contact->SetEnabled(false);
            _Body2->SetLinearVelocity({0.0f, 0.0f});
        }
        else if (_Type2 == GameObjectTypeE::SUBMARINE_HULL ||
                 _Type2 == GameObjectTypeE::SUBMARINE_RUDDER)

        {
            this->emitLandscapeDebris(_Contact, _Body1, _Body2);
            this->emitSubmarineDebris(_Contact, _Body2, _Body1, GameObjectTypeE::LANDSCAPE);
        }
    }
    else if (_Type1 == GameObjectTypeE::PROJECTILE)
    {
        if (_Type2 == GameObjectTypeE::SUBMARINE_HULL ||
            _Type2 == GameObjectTypeE::SUBMARINE_RUDDER)
        {
            this->emitSubmarineDebris(_Contact, _Body2, _Body1, GameObjectTypeE::SUBMARINE_HULL);
            _Body1->SetLinearVelocity({0.0f, 0.0f});
        }
    }
    else if (_Type1 == GameObjectTypeE::SUBMARINE_HULL ||
             _Type1 == GameObjectTypeE::SUBMARINE_RUDDER)
    {
        if (_Type2 == GameObjectTypeE::SUBMARINE_HULL ||
            _Type2 == GameObjectTypeE::SUBMARINE_RUDDER)
        {
            this->emitSubmarineDebris(_Contact, _Body1, _Body2, GameObjectTypeE::SUBMARINE_HULL);
            this->emitSubmarineDebris(_Contact, _Body2, _Body1, GameObjectTypeE::SUBMARINE_HULL);
        }
    }

}
