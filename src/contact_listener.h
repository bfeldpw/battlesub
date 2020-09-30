#ifndef CONTACT_LISTENER_H
#define CONTACT_LISTENER_H

#include <Box2D/Box2D.h>

#include "game_object.h"
#include "global_emitter_factory.h"
#include "global_resources.h"

class ContactListener : public b2ContactListener
{
    void PreSolve(b2Contact* Contact, const b2Manifold* OldManifold) override
    {
        
        b2PointState State1[2], State2[2];
        b2GetPointStates(State1, State2, OldManifold, Contact->GetManifold());
        
        // Only emit debris if new collision occurred
        if (State2[0] == b2_addState)
        {
            GameObjectTypeE DebrisType = GameObjectTypeE::DEFAULT;
        
            GameObject* ObjA = static_cast<GameObject*>(Contact->GetFixtureA()->GetBody()->GetUserData());
            GameObject* ObjB = static_cast<GameObject*>(Contact->GetFixtureB()->GetBody()->GetUserData());
            
            b2Body* Body = nullptr;
            bool    IsContactKept = true;
            if (ObjA->getType() == GameObjectTypeE::LANDSCAPE)
            {
                if (ObjB->getType() == GameObjectTypeE::SUBMARINE_HULL)
                {
                    Body = Contact->GetFixtureB()->GetBody();
                    IsContactKept = true;
                }
                else if (ObjB->getType() == GameObjectTypeE::PROJECTILE)
                {
                    Body = Contact->GetFixtureB()->GetBody();
                    IsContactKept = false;
                }
                DebrisType = GameObjectTypeE::DEBRIS_LANDSCAPE;
            } 
            else if (ObjB->getType() == GameObjectTypeE::LANDSCAPE)
            {
                if (ObjA->getType() == GameObjectTypeE::SUBMARINE_HULL)
                {
                    Body = Contact->GetFixtureA()->GetBody();
                    IsContactKept = true;
                }
                else if (ObjA->getType() == GameObjectTypeE::PROJECTILE)
                {
                    Body = Contact->GetFixtureA()->GetBody();
                    IsContactKept = false;
                }
                DebrisType = GameObjectTypeE::DEBRIS_LANDSCAPE;
            }
            else if (ObjA->getType() == GameObjectTypeE::SUBMARINE_HULL ||
                     ObjA->getType() == GameObjectTypeE::SUBMARINE_RUDDER)
            {
                if (ObjB->getType() == GameObjectTypeE::PROJECTILE)
                {
                    Body = Contact->GetFixtureB()->GetBody();
                    if (Body != nullptr)
                    {
                        b2WorldManifold WorldManifold;
                        Contact->GetWorldManifold(&WorldManifold);
                        {
                            b2Vec2 POC = WorldManifold.points[0];
                            b2Vec2 Velocity = Body->GetLinearVelocityFromWorldPoint(POC);

                            float AngleVel = std::atan2(Velocity.y, Velocity.x);
                            (AngleVel > 0.0f) ? AngleVel -= b2_pi : AngleVel += b2_pi;
                            float Angle =  std::atan2(WorldManifold.normal.y, WorldManifold.normal.x) - AngleVel;
                            if (Angle > -0.7f && Angle < 0.7f)
                            {
                                IsContactKept = false;
                                std::any_cast<Submarine*>(ObjA->getParent())->impact(2.0f);
                            }
                        }
                    }
                    DebrisType = GameObjectTypeE::DEBRIS_SUBMARINE;
                }
            }
            
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

                        Emitter* DebrisEmitter = GlobalEmitterFactory::Get.create();
                        if (DebrisEmitter != nullptr)
                        {
                            DebrisEmitter->setOrigin(POC.x+WorldManifold.normal.x*0.2f, POC.y+WorldManifold.normal.y*0.2f)
                                        .setAngle(Angle)
                                        .setAngleStdDev(1.0f)
                                        .setNumber(10)
                                        .setScale(Velocity.Length()*0.01f+Mass*0.0001f)
                                        .setScaleStdDev(Velocity.Length()*0.05f+Mass*0.001f)
                                        .setType(GameObjectTypeE::DEBRIS_SUBMARINE)
                                        .setVelocity(Velocity.Length()*0.01f+1.0f)
                                        .setVelocityStdDev(Velocity.Length()*0.005f);
                        }
                        else
                        {
                            GlobalErrorHandler.reportError("Couldn't create DebrisEmitter, out of pool memory.");
                        }
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
