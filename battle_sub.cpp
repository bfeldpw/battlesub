#include <iostream>

#include <Box2D/Box2D.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/ConfigurationValue.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Buffer.h>

#include <Magnum/Math/DualComplex.h>
#include <Magnum/Platform/Sdl2Application.h>

#include "battle_sub.h"
#include "common.h"
#include "global_factories.h"
#include "resource_storage.h"

namespace BattleSub{

BattleSub::BattleSub(const Arguments& arguments): Platform::Application{arguments, NoCreate}
{
    /* Try 8x MSAA, fall back to zero samples if not possible. Enable only 2x
       MSAA if we have enough DPI. */
    {
        const Vector2 dpiScaling = this->dpiScaling({});
        
        Configuration conf;
        conf.setSize({2000, 1000});
        
        conf.setTitle("BattleSub")
            .setSize(conf.size(), dpiScaling);
        
            GLConfiguration glConf;
        glConf.setSampleCount(dpiScaling.max() < 2.0f ? 8 : 2);
        
        if (!tryCreate(conf, glConf))
        {
            create(conf, glConf.setSampleCount(0));
        }
    }

    /* Configure camera */
    CameraObject_ = new Object2D{&Scene_};
    Camera_ = new SceneGraph::Camera2D{*CameraObject_};
    Camera_->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
            .setProjectionMatrix(Matrix3::projection({100.0f, 100.0f}))
            .setViewport(GL::defaultFramebuffer.viewport().size());

    ResourceStorage::Global.init();
            
    /* Create the Box2D world with the usual gravity vector */
    World_.emplace(b2Vec2{0.0f, 0.0f});
    
    Shader_ = Shaders::Flat2D{};
    
    PlayerSub_ = GlobalFactories::Submarines.create();
    b2BodyDef BodyDef;
    BodyDef.type = b2_dynamicBody;
    BodyDef.active = true;
    BodyDef.position.Set(0.0f, -20.0f);
    PlayerSub_->setMeshes(ResourceStorage::Global.getMeshesSubmarine());
    PlayerSub_->setShader(&Shader_);
    PlayerSub_->setShapes(ResourceStorage::Global.getShapesSubmarine());
    PlayerSub_->init(&(*World_), &Scene_, BodyDef, &Drawables_);
    
    
    PlayerSub2_ = GlobalFactories::Submarines.create();
    b2BodyDef BodyDef2;
    BodyDef2.type = b2_dynamicBody;
    BodyDef2.active = true;
    BodyDef2.position.Set(0.0f, 20.0f);
    BodyDef2.angle = 3.14159f;
    PlayerSub2_->setMeshes(ResourceStorage::Global.getMeshesSubmarine());
    PlayerSub2_->setShader(&Shader_);
    PlayerSub2_->setShapes(ResourceStorage::Global.getShapesSubmarine());
    PlayerSub2_->init(&(*World_), &Scene_, BodyDef2, &Drawables_);
    
    
    CanyonBoundary = GlobalFactories::Landscapes.create();
    b2BodyDef BodyDef3;
    BodyDef3.type = b2_staticBody;
    BodyDef3.active = true;
    BodyDef3.position.Set(0.0f, 0.0f);
    CanyonBoundary->setShapes(ResourceStorage::Global.getShapesLandscape());
    CanyonBoundary->setMeshes(ResourceStorage::Global.getMeshesLandscape());
    CanyonBoundary->setShader(&Shader_);
    CanyonBoundary->init(&(*World_), &Scene_, BodyDef3, &Drawables_);
    
    if (!setSwapInterval(1))
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_ANDROID)
        setMinimalLoopPeriod(1.0f/60.0f);
    #endif
}

void BattleSub::keyPressEvent(KeyEvent& Event)
{
    switch (Event.key())
    {
        case KeyEvent::Key::A: KeyPressedMap["a"] = true; break;
        case KeyEvent::Key::D: KeyPressedMap["d"] = true; break;
        case KeyEvent::Key::P:
            KeyPressedMap["p"] = true;
            IsPaused_ ^= true;
            break;
        case KeyEvent::Key::S: KeyPressedMap["s"] = true; break;
        case KeyEvent::Key::W: KeyPressedMap["w"] = true; break;
        case KeyEvent::Key::LeftCtrl: KeyPressedMap["LCTRL"] = true; break;
        case KeyEvent::Key::Esc: KeyPressedMap["Esc"] = true; break;
        case KeyEvent::Key::Space:
            KeyPressedMap["Space"] = true;
            if (IsPaused_) IsStepForward_ = true;
            break;
        default: break;
    }
}

void BattleSub::keyReleaseEvent(KeyEvent& Event)
{
    switch (Event.key())
    {
        case KeyEvent::Key::A: KeyPressedMap["a"] = false; break;
        case KeyEvent::Key::D: KeyPressedMap["d"] = false; break;
        case KeyEvent::Key::P: KeyPressedMap["p"] = false; break;
        case KeyEvent::Key::S: KeyPressedMap["s"] = false; break;
        case KeyEvent::Key::W: KeyPressedMap["w"] = false; break;
        case KeyEvent::Key::LeftCtrl: KeyPressedMap["LCTRL"] = false; break;
        case KeyEvent::Key::Esc: KeyPressedMap["Esc"] = false; break;
        case KeyEvent::Key::Space: KeyPressedMap["Space"] = false; break;
        default: break;
    }
}

void BattleSub::mouseMoveEvent(MouseMoveEvent& Event)
{
    MouseDelta_.x() =  Event.relativePosition().x();
    MouseDelta_.y() = -Event.relativePosition().y();
    
    if (Event.modifiers() & MouseMoveEvent::Modifier::Ctrl)
    {
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
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
    
    if (KeyPressedMap["a"] == true) PlayerSub_->rudderLeft();
    if (KeyPressedMap["d"] == true) PlayerSub_->rudderRight();
    if (KeyPressedMap["s"] == true) PlayerSub_->throttleReverse();
    if (KeyPressedMap["w"] == true) PlayerSub_->throttleForward();
    if (KeyPressedMap["Esc"] == true)
    {
        Platform::Application::Sdl2Application::exit();
    }
    
    Camera_->setProjectionMatrix(Matrix3::projection({VPX_, VPY_}));
    
    if (!IsPaused_ || IsStepForward_)
    {
        // Update game objects
        for (auto Projectile : GlobalFactories::Projectiles.getEntities())
        {
            Projectile.second->update();
            if (Projectile.second->isSunk())
            {
                GlobalFactories::Projectiles.destroy(Projectile.second);
                break;
            }
        }
        for (auto Sub : GlobalFactories::Submarines.getEntities())
        {
            Sub.second->update(Drawables_);
        }
        
        // Update physics
        World_->Step(1.0f/60.0f, 80, 30);

        // Update object visuals    
        for(b2Body* Body = World_->GetBodyList(); Body; Body = Body->GetNext())
        {
            if (Body->IsActive())
            {            
                (*static_cast<Object2D*>(Body->GetUserData()))
                    .setTranslation({Body->GetPosition().x, Body->GetPosition().y})
                    .setRotation(Complex::rotation(Rad(Body->GetAngle())));
            }
        }
        
        IsStepForward_ = false;
        
    } // if (!IsPaused_)
    
    if (GlobalErrorHandler.checkError() == true) Platform::Application::Sdl2Application::exit();
    
    // Draw the scene
    Camera_->draw(Drawables_);

    swapBuffers();
    redraw();
}

}

MAGNUM_APPLICATION_MAIN(BattleSub::BattleSub)
