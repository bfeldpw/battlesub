#ifndef CONTACT_LISTENER_H
#define CONTACT_LISTENER_H

#include <Box2D/Box2D.h>

#include "game_object.h"
#include "global_emitter_factory.h"
#include "global_resources.h"

#include <entt/entity/registry.hpp>
#include <entt/entity/helper.hpp>

class ContactListener : public b2ContactListener
{
public:
    ContactListener(entt::registry& _Reg) : Reg_{_Reg} {}
    entt::registry& Reg_;

    void PreSolve(b2Contact* Contact, const b2Manifold* OldManifold) override
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
            if (StatusA.Type_ == GameObjectTypeE::LANDSCAPE)
            {
                if (StatusB.Type_ == GameObjectTypeE::PROJECTILE)
                {
                    IsContactKept = false;
                Body = BodyB;
                }
                else if (StatusB.Type_ == GameObjectTypeE::SUBMARINE_HULL)
                {
                    IsContactKept = true;
                Body = BodyB;
                }
                DebrisType = GameObjectTypeE::DEBRIS_LANDSCAPE;
            }
            else if (StatusB.Type_ == GameObjectTypeE::LANDSCAPE)
            {
                if (StatusA.Type_ == GameObjectTypeE::PROJECTILE)
                {
                    IsContactKept = false;
                Body = BodyA;
                }
                else if (StatusA.Type_ == GameObjectTypeE::SUBMARINE_HULL)
                {
                    IsContactKept = true;
                Body = BodyA;
                }
                DebrisType = GameObjectTypeE::DEBRIS_LANDSCAPE;
            }
            // b2Body* Body = nullptr;
            // bool    IsContactKept = true;
            // if (ObjA->getType() == GameObjectTypeE::LANDSCAPE)
            // {
            //     if (ObjB->getType() == GameObjectTypeE::SUBMARINE_HULL)
            //     {
            //         Body = Contact->GetFixtureB()->GetBody();
            //         IsContactKept = true;
            //     }
            //     else if (ObjB->getType() == GameObjectTypeE::PROJECTILE)
            //     {
            //         Body = Contact->GetFixtureB()->GetBody();
            //         IsContactKept = false;
            //     }
            //     DebrisType = GameObjectTypeE::DEBRIS_LANDSCAPE;
            // }
            // else if (ObjB->getType() == GameObjectTypeE::LANDSCAPE)
            // {
            //     if (ObjA->getType() == GameObjectTypeE::SUBMARINE_HULL)
            //     {
            //         Body = Contact->GetFixtureA()->GetBody();
            //         IsContactKept = true;
            //     }
            //     else if (ObjA->getType() == GameObjectTypeE::PROJECTILE)
            //     {
            //         Body = Contact->GetFixtureA()->GetBody();
            //         IsContactKept = false;
            //     }
            //     DebrisType = GameObjectTypeE::DEBRIS_LANDSCAPE;
            // }
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

            if (Body != nullptr)
            {
                if (DebrisType == GameObjectTypeE::DEBRIS_LANDSCAPE)
                {
                    b2WorldManifold WorldManifold;
                    Contact->GetWorldManifold(&WorldManifold);
                    {
                        b2Vec2 POC = WorldManifold.points[0];
                        b2Vec2 Velocity = Body->GetLinearVelocityFromWorldPoint(POC);
                        float  Mass = Body->GetMass();

                        float Angle = 2.0f * std::atan2(WorldManifold.normal.y, WorldManifold.normal.x) -
                                            (std::atan2(Velocity.y, Velocity.x)+b2_pi);

                        Emitter* DebrisEmitter = GlobalEmitterFactory::Get.create();
                        if (DebrisEmitter != nullptr)
                        {
                            DebrisEmitter->setOrigin(POC.x+WorldManifold.normal.x*0.2f, POC.y+WorldManifold.normal.y*0.2f)
                                        .setAngle(Angle)
                                        .setAngleStdDev(1.0f)
                                        .setNumber(10)
                                        .setScale(Velocity.Length()*0.05f+Mass*0.0001f)
                                        .setScaleStdDev(Velocity.Length()*0.05f+Mass*0.0001f)
                                        .setType(GameObjectTypeE::DEBRIS_LANDSCAPE)
                                        .setVelocity(Velocity.Length()*0.02f+1.0f)
                                        .setVelocityStdDev(Velocity.Length()*0.02f);
                        }
                        else
                        {
                            GlobalErrorHandler.reportError("Couldn't create DebrisEmitter, out of pool memory.");
                        }
                    }
                }
                else if ((DebrisType == GameObjectTypeE::DEBRIS_SUBMARINE) && !IsContactKept)
                {
                    b2WorldManifold WorldManifold;
                    Contact->GetWorldManifold(&WorldManifold);
                    {
                        b2Vec2 POC = WorldManifold.points[0];
                        b2Vec2 Velocity = Body->GetLinearVelocityFromWorldPoint(POC);
                        float  Mass = Body->GetMass();

                        float Angle = 2.0f * std::atan2(WorldManifold.normal.y, WorldManifold.normal.x) -
                                            (std::atan2(Velocity.y, Velocity.x)+b2_pi);

                        // Emitter* DebrisEmitter = GlobalEmitterFactory::Get.create();
                        // if (DebrisEmitter != nullptr)
                        // {
                        //     DebrisEmitter->setOrigin(POC.x+WorldManifold.normal.x*0.2f, POC.y+WorldManifold.normal.y*0.2f)
                        //                 .setAngle(Angle)
                        //                 .setAngleStdDev(1.0f)
                        //                 .setNumber(10)
                        //                 .setScale(Velocity.Length()*0.01f+Mass*0.0001f)
                        //                 .setScaleStdDev(Velocity.Length()*0.05f+Mass*0.001f)
                        //                 .setType(GameObjectTypeE::DEBRIS_SUBMARINE)
                        //                 .setVelocity(Velocity.Length()*0.01f+1.0f)
                        //                 .setVelocityStdDev(Velocity.Length()*0.005f);
                        // }
                        // else
                        // {
                        //     GlobalErrorHandler.reportError("Couldn't create DebrisEmitter, out of pool memory.");
                        // }
                    }

                }
            }

            if (!IsContactKept)
            {
                Contact->SetEnabled(false);
                Body->SetLinearVelocity({0.0f, 0.0f}); // Be sure object will be disabled and sinking
            }
        }
    }
    
};

#endif // CONTACT_LISTENER_H
