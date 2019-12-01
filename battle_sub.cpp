#include <iostream>

#include <Box2D/Box2D.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/ConfigurationValue.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Math/Color.h>

#include <Magnum/Platform/Sdl2Application.h>

#include "battle_sub.h"
#include "common.h"
#include "global_factories.h"
#include "global_resources.h"
#include "world_def.h"

namespace BattleSub{

BattleSub::BattleSub(const Arguments& arguments): Platform::Application{arguments, NoCreate}
{
    /* Try 8x MSAA, fall back to zero samples if not possible. Enable only 2x
       MSAA if we have enough DPI. */
    {
        const Vector2 dpiScaling = this->dpiScaling({});
        
        Configuration conf;
        conf.setSize({int(WORLD_SIZE_X)*2, int(WORLD_SIZE_Y)*2});
        
        conf.setTitle("BattleSub")
            .setSize(conf.size(), dpiScaling);
        
        GLConfiguration glConf;
        glConf.setSampleCount(dpiScaling.max() < 2.0f ? 8 : 2);
        
        if (!tryCreate(conf, glConf))
        {
            create(conf, glConf.setSampleCount(0));
        }
    }
    
    GlobalResources::Get.init();

    /* Configure camera */
    CameraObject_ = new Object2D{GlobalResources::Get.getScene()};
    Camera_ = new SceneGraph::Camera2D{*CameraObject_};
    Camera_->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
            .setProjectionMatrix(Matrix3::projection({100.0f, 100.0f}))
            .setViewport(GL::defaultFramebuffer.viewport().size());
            
    /* Create the Box2D world with the usual gravity vector */
    GlobalResources::Get.getWorld()->SetContactListener(&ContactListener_);
    
    PlayerSub_ = GlobalFactories::Submarines.create();
    b2BodyDef BodyDef;
    BodyDef.type = b2_dynamicBody;
    BodyDef.active = true;
    BodyDef.position.Set(0.0f, -20.0f);
    BodyDef.angularDamping = 0.8f;
    BodyDef.linearDamping = 0.2f;
    PlayerSub_->Hull.setDrawableGroup(GlobalResources::Get.getDrawables(DrawableGroupsTypeE::DEFAULT))
                    .setMeshes(GlobalResources::Get.getMeshes(GameObjectTypeE::SUBMARINE_HULL))
                    .setScene(GlobalResources::Get.getScene())
                    .setShapes(GlobalResources::Get.getShapes(GameObjectTypeE::SUBMARINE_HULL))
                    .setShader(GlobalResources::Get.getShader())
                    .setWorld(GlobalResources::Get.getWorld())
                    .init(GameObjectTypeE::SUBMARINE_HULL, BodyDef);
    b2BodyDef BodyDefRudder;
    BodyDefRudder.type = b2_dynamicBody;
    BodyDefRudder.active = true;
    BodyDefRudder.position.Set(0.0f, -27.0f);
    BodyDefRudder.angularDamping = 0.8f;
    BodyDefRudder.linearDamping = 0.2f;
    PlayerSub_->Rudder.setDrawableGroup(GlobalResources::Get.getDrawables(DrawableGroupsTypeE::DEFAULT))
                      .setMeshes(GlobalResources::Get.getMeshes(GameObjectTypeE::SUBMARINE_RUDDER))
                      .setScene(GlobalResources::Get.getScene())
                      .setShapes(GlobalResources::Get.getShapes(GameObjectTypeE::SUBMARINE_RUDDER))
                      .setShader(GlobalResources::Get.getShader())
                      .setWorld(GlobalResources::Get.getWorld())
                      .init(GameObjectTypeE::SUBMARINE_RUDDER, BodyDef);
        
    b2RevoluteJointDef jointDef;
    jointDef.lowerAngle = -0.25f * b2_pi; // -45 degrees
    jointDef.upperAngle =  0.25f * b2_pi; // 45 degrees
    jointDef.enableLimit = true;
    jointDef.bodyA = PlayerSub_->Hull.getBody();
    jointDef.localAnchorA = {0.0f, -6.0f};
    jointDef.bodyB = PlayerSub_->Rudder.getBody();
    jointDef.localAnchorB = {0.0f,  1.0f};
    jointDef.maxMotorTorque = 10000.0f;
    jointDef.motorSpeed = 0.0f;
    jointDef.enableMotor = true;
    jointDef.collideConnected = false;
    PlayerSub_->RudderJoint = static_cast<b2RevoluteJoint*>(GlobalResources::Get.getWorld()->CreateJoint(&jointDef));
    
    
    PlayerSub2_ = GlobalFactories::Submarines.create();
    b2BodyDef BodyDef2;
    BodyDef2.type = b2_dynamicBody;
    BodyDef2.active = true;
    BodyDef2.position.Set(10.0f, 20.0f);
    BodyDef2.angle = 3.14159f;
    BodyDef2.angularDamping = 0.8f;
    BodyDef2.linearDamping = 0.2f;
    PlayerSub2_->Hull.setDrawableGroup(GlobalResources::Get.getDrawables(DrawableGroupsTypeE::DEFAULT))
                     .setMeshes(GlobalResources::Get.getMeshes(GameObjectTypeE::SUBMARINE_HULL))
                     .setScene(GlobalResources::Get.getScene())
                     .setShapes(GlobalResources::Get.getShapes(GameObjectTypeE::SUBMARINE_HULL))
                     .setShader(GlobalResources::Get.getShader())
                     .setWorld(GlobalResources::Get.getWorld())
                     .init(GameObjectTypeE::SUBMARINE_HULL, BodyDef2);
    
    CanyonBoundary = GlobalFactories::Landscapes.create();
    b2BodyDef BodyDef3;
    BodyDef3.type = b2_staticBody;
    BodyDef3.active = true;
    BodyDef3.position.Set(0.0f, 0.0f);
    CanyonBoundary->setColor({0.2f, 0.2f, 0.3f})
                   .setDrawableGroup(GlobalResources::Get.getDrawables(DrawableGroupsTypeE::DEFAULT))
                   .setMeshes(GlobalResources::Get.getMeshes(GameObjectTypeE::LANDSCAPE))
                   .setScene(GlobalResources::Get.getScene())
                   .setShapes(GlobalResources::Get.getShapes(GameObjectTypeE::LANDSCAPE))
                   .setShader(GlobalResources::Get.getShader())
                   .setWorld(GlobalResources::Get.getWorld())
                   .init(GameObjectTypeE::LANDSCAPE, BodyDef3);
    
    if (!setSwapInterval(1))
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_ANDROID)
        setMinimalLoopPeriod(1.0f/60.0f);
    #endif
}

void BattleSub::keyPressEvent(KeyEvent& Event)
{
    switch (Event.key())
    {
        case KeyEvent::Key::A:
            KeyPressedMap["a"] = true;
            PlayerSub_->RudderJoint->SetMotorSpeed(-1.0f);
            break;
        case KeyEvent::Key::D:
            KeyPressedMap["d"] = true;
            PlayerSub_->RudderJoint->SetMotorSpeed(1.0f);
            break;
        case KeyEvent::Key::P:
            KeyPressedMap["p"] = true;
            IsPaused_ ^= true;
            break;
        case KeyEvent::Key::S: KeyPressedMap["s"] = true; break;
        case KeyEvent::Key::W: KeyPressedMap["w"] = true; break;
        case KeyEvent::Key::X:
            PlayerSub_->fullStop();
            break;
        case KeyEvent::Key::Esc:
        {
            cleanupAndExit();
            break;
        }
        case KeyEvent::Key::Space:
            if (IsPaused_) IsStepForward_ = true;
            break;
        default: break;
    }
}

void BattleSub::keyReleaseEvent(KeyEvent& Event)
{
    switch (Event.key())
    {
        case KeyEvent::Key::A:
            KeyPressedMap["a"] = false;
            PlayerSub_->RudderJoint->SetMotorSpeed(0.0f);
            break;
        case KeyEvent::Key::D:
            KeyPressedMap["d"] = false;
            PlayerSub_->RudderJoint->SetMotorSpeed(0.0f);
            break;
        case KeyEvent::Key::S: KeyPressedMap["s"] = false; break;
        case KeyEvent::Key::W: KeyPressedMap["w"] = false; break;
        default: break;
    }
}

void BattleSub::mouseMoveEvent(MouseMoveEvent& Event)
{
    MouseDelta_.x() =  Event.relativePosition().x();
    MouseDelta_.y() = -Event.relativePosition().y();
    
    if (Event.modifiers() & MouseMoveEvent::Modifier::Ctrl)
    {
        DevCam_ = true;
        if (Event.buttons() & MouseMoveEvent::Button::Left)
        {
            if (MouseDelta_.y() != 0) Zoom_ *= 1.0f-0.01f*MouseDelta_.y();
            VPX_ = 100.0f * Zoom_;
            VPY_ = 100.0f * Zoom_;
        }
        else 
        {
            Platform::Application::Sdl2Application::setMouseLocked(true);
            CameraObject_->translate(0.05f*Zoom_*Vector2(MouseDelta_));
            MouseDelta_ = Vector2i();
        }
    }
    else
    {
        Platform::Application::Sdl2Application::setMouseLocked(false);
    }
    
}

void BattleSub::mousePressEvent(MouseEvent& Event)
{
    if(Event.button() == MouseEvent::Button::Left)
    {
        if (!(Event.modifiers() & MouseEvent::Modifier::Ctrl))
        {
            PlayerSub_->fire(-1.0f);
        }
    }
    else if (Event.button() == MouseEvent::Button::Right)
    {
        PlayerSub_->fire(1.0f);
    }
}

void BattleSub::drawEvent()
{
    if (!IsExitTriggered_)
    {
        GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
        GL::defaultFramebuffer.clearColor(Color4(0.0f, 0.0f, 0.1f, 1.0f));
        
//         if (KeyPressedMap["a"] == true) PlayerSub_->rudderLeft();
//         if (KeyPressedMap["d"] == true) PlayerSub_->rudderRight();
        if (KeyPressedMap["s"] == true) PlayerSub_->throttleReverse();
        if (KeyPressedMap["w"] == true) PlayerSub_->throttleForward();
        
//         Zoom_ = 20.0f + PlayerSub_->Hull.getBody()->GetLinearVelocity().Length();
//         VPX_ = 2.0f * Zoom_;
//         VPY_ = 2.0f * Zoom_;
        
        Camera_->setProjectionMatrix(Matrix3::projection({VPX_, VPY_}));
        
        if (!IsPaused_ || IsStepForward_)
        {
            updateGameObjects();
            if (!DevCam_)
            {
                auto Pos = PlayerSub_->Hull.getBody()->GetPosition();
                CameraObject_->resetTransformation();
                CameraObject_->translate(Vector2(Pos.x, Pos.y));
            }
        }
        
        if (GlobalErrorHandler.checkError() == true)
        {
            cleanupAndExit();
        }
        
        // Draw the scene
        FluidGrid_.process();
        FluidGrid_.drawDiffusion(Camera_->projectionMatrix()*Camera_->cameraMatrix());
        Camera_->draw(*GlobalResources::Get.getDrawables(DrawableGroupsTypeE::WEAPON));
        Camera_->draw(*GlobalResources::Get.getDrawables(DrawableGroupsTypeE::DEFAULT));
        
        swapBuffers();
        redraw();
        
        DevCam_ = false;
    }
}

void BattleSub::cleanupAndExit()
{
    // Use flag to avoid further processing within drawEvent calls
    IsExitTriggered_ = true;
    
    // Resources need to be released first, as long as the GL context is still
    // valid
    GlobalResources::Get.release();
    
    Platform::Application::Sdl2Application::exit();
}

void BattleSub::updateGameObjects()
{
    for (auto Projectile : GlobalFactories::Projectiles.getEntities())
    {
        Projectile.second->update();
        auto Pos = Projectile.second->getBody()->GetPosition();
        auto Vel = Projectile.second->getBody()->GetLinearVelocity().Length();
        
        if (Projectile.second->isSunk())
        {
            GlobalFactories::Projectiles.destroy(Projectile.second);
            break;
        }
        else
        {
            FluidGrid_.addDensity(Pos.x, Pos.y, Vel * 10.0f);
        }
    }
    for (auto Debris : GlobalFactories::Debris.getEntities())
    {
        Debris.second->update();
        auto Pos = Debris.second->getBody()->GetPosition();
        auto Vel = Debris.second->getBody()->GetLinearVelocity().Length();
        
        if (Debris.second->isSunk())
        {
            GlobalFactories::Debris.destroy(Debris.second);
            break;
        }
        else
        {
            FluidGrid_.addDensity(Pos.x, Pos.y, Vel * 10.0f);
        }
    }
    
    std::vector<EntityIDType> EmittersToBeDeleted;
    for (auto Emitter : GlobalEmitterFactory::Get.getEntities())
    {
        if (Emitter.second->isFinished())
        {
            EmittersToBeDeleted.push_back(Emitter.second->ID);
        }
        Emitter.second->emit();
    }
    for (auto d : EmittersToBeDeleted)
    {
        GlobalEmitterFactory::Get.destroy(GlobalEmitterFactory::Get.getEntities().at(d));
    }
//     for (auto Sub : GlobalFactories::Submarines.getEntities())
//     {
//         Sub.second->update();
//     }
    PlayerSub_->update();
    auto Propellor = PlayerSub_->Hull.getBody()->GetWorldPoint({0.0f, -7.0f});
    FluidGrid_.addDensity(Propellor.x, Propellor.y, 0.01f*std::abs(PlayerSub_->getThrottle()));
    
    // Update physics
    GlobalResources::Get.getWorld()->Step(1.0f/60.0f, 40, 15);

    // Update object visuals    
    for(b2Body* Body = GlobalResources::Get.getWorld()->GetBodyList(); Body; Body = Body->GetNext())
    {
        if (Body->IsActive() && Body->GetType() != b2_staticBody)
        {            
            (*static_cast<GameObject*>(Body->GetUserData())->getVisuals())
                .setTranslation({Body->GetPosition().x, Body->GetPosition().y})
                .setRotation(Complex::rotation(Rad(Body->GetAngle())));
        }
    }
    
    IsStepForward_ = false;
}

}

MAGNUM_APPLICATION_MAIN(BattleSub::BattleSub)
