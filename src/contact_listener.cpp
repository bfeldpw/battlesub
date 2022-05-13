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
        auto EntityA = *(reinterpret_cast<entt::entity*>(Contact->GetFixtureA()->GetBody()->GetUserData().pointer));
        auto EntityB = *(reinterpret_cast<entt::entity*>(Contact->GetFixtureB()->GetBody()->GetUserData().pointer));
        auto& StatusA = Reg_.get<StatusComponent>(EntityA);
        auto& StatusB = Reg_.get<StatusComponent>(EntityB);
        b2Body* BodyA = Reg_.get<PhysicsComponent>(EntityA).Body_;
        b2Body* BodyB = Reg_.get<PhysicsComponent>(EntityB).Body_;

        this->testGameObjectTypes(Contact, StatusA, StatusB, BodyA, BodyB);
        this->testGameObjectTypes(Contact, StatusB, StatusA, BodyB, BodyA);

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
    EmComp.Scale_= 4.0;
    EmComp.ScaleStdDev_ = 2.0;
    EmComp.Velocity_ = 50.0f;
    EmComp.VelocityStdDev_ = 25.0f;
    EmComp.VelocityWeight_ = 0.0f;
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
        default: break;
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
    EmComp.Velocity_ = Velocity.Length()*0.1f+5.0f;
    EmComp.VelocityStdDev_ = Velocity.Length()*0.1f;
    EmComp.VelocityWeight_ = 0.1f;
}

void ContactListener::testGameObjectTypes(b2Contact* const _Contact, StatusComponent& _Status1, StatusComponent& _Status2,
                                          b2Body* const _Body1, b2Body* const _Body2)
{
    if (_Status1.Type_== GameObjectTypeE::LANDSCAPE)
    {
        if (_Status2.Type_== GameObjectTypeE::PROJECTILE)
        {
            this->emitLandscapeDebris(_Contact, _Body1, _Body2);
            _Contact->SetEnabled(false);
            _Body2->SetLinearVelocity({0.0f, 0.0f});
            _Status2.IsSinking_ = true;
            _Status2.IsSunk_ = true;
        }
        else if (_Status2.Type_ == GameObjectTypeE::SUBMARINE_HULL ||
                 _Status2.Type_ == GameObjectTypeE::SUBMARINE_RUDDER)

        {
            this->emitLandscapeDebris(_Contact, _Body1, _Body2);
            this->emitSubmarineDebris(_Contact, _Body2, _Body1, GameObjectTypeE::LANDSCAPE);
        }
    }
    else if (_Status1.Type_ == GameObjectTypeE::PROJECTILE)
    {
        if (_Status2.Type_ == GameObjectTypeE::SUBMARINE_HULL ||
            _Status2.Type_ == GameObjectTypeE::SUBMARINE_RUDDER)
        {
            this->emitSubmarineDebris(_Contact, _Body2, _Body1, GameObjectTypeE::SUBMARINE_HULL);
            _Contact->SetEnabled(false);
            _Body1->SetLinearVelocity({0.0f, 0.0f});
            _Status1.IsSinking_ = true;
            _Status1.IsSunk_ = true;
        }
    }
    else if (_Status1.Type_ == GameObjectTypeE::SUBMARINE_HULL ||
             _Status1.Type_ == GameObjectTypeE::SUBMARINE_RUDDER)
    {
        if (_Status2.Type_ == GameObjectTypeE::SUBMARINE_HULL ||
            _Status2.Type_ == GameObjectTypeE::SUBMARINE_RUDDER)
        {
            this->emitSubmarineDebris(_Contact, _Body1, _Body2, GameObjectTypeE::SUBMARINE_HULL);
            this->emitSubmarineDebris(_Contact, _Body2, _Body1, GameObjectTypeE::SUBMARINE_HULL);

        }
    }
}
