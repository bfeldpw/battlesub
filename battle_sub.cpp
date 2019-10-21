#include <iostream>

#include <Box2D/Box2D.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/ConfigurationValue.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Buffer.h>

#include <Magnum/Math/DualComplex.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/Trade/MeshData2D.h>

#include "battle_sub.h"
#include "box_drawable.h"
#include "common.h"
#include "landscape_factory.h"
#include "landscape_drawable.h"
#include "sub_drawable.h"
#include "submarine_factory.h"

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

    /* Create the Box2D world with the usual gravity vector */
    World_.emplace(b2Vec2{0.0f, 0.0f});
    
    PlayerSub_ = GlobalSubmarineFactory.create();
    PlayerSub_->create(&(*World_), &Scene_);
    b2BodyDef BodyDef;
    BodyDef.type = b2_dynamicBody;
    BodyDef.active = true;
    BodyDef.position.Set(0.0f, -20.0f);
    PlayerSub_->init(BodyDef);
    
    PlayerSub2_ = GlobalSubmarineFactory.create();
    PlayerSub2_->create(&(*World_), &Scene_);
    b2BodyDef BodyDef2;
    BodyDef2.type = b2_dynamicBody;
    BodyDef2.active = true;
    BodyDef2.position.Set(0.0f, 20.0f);
    PlayerSub2_->init(BodyDef2);
    
    auto CanyonBoundary = GlobalLandscapeFactory.create();
    CanyonBoundary->create(&(*World_), &Scene_);
    b2BodyDef BodyDef3;
    BodyDef3.type = b2_staticBody;
    BodyDef3.active = true;
    BodyDef3.position.Set(0.0f, 20.0f);
    CanyonBoundary->init(BodyDef3);
    
    /* Create the shader and the box mesh */
    Shader_ = Shaders::Flat2D{};
    Mesh_ = MeshTools::compile(Primitives::squareSolid());
    
    const struct {
        Vector2 pos;
    } data[]{{{-0.1f, 0.0f}},
            {{  0.1f, 0.0f}},
            {{ -0.1f, 0.8f}},
            {{ -0.1f, 0.8f}},
            {{  0.1f, 0.0f}},
            {{  0.1f, 0.8f}},
            {{  0.1f, 0.8f}},
            {{  0.0f, 1.0f}},
            {{ -0.1f, 0.8f}}};

    GL::Mesh Mesh;
    GL::Buffer buffer;
    buffer.setData(data, GL::BufferUsage::StaticDraw);
    MeshProjectile_ = GL::Mesh{};
    MeshProjectile_.setCount(9)
        .addVertexBuffer(std::move(buffer), 0,
            Shaders::VertexColor2D::Position{});

    new SubDrawable(PlayerSub_->getVisuals(), MeshProjectile_, Shader_, 0x2f83cc_rgbf, Drawables_);
    new SubDrawable(PlayerSub2_->getVisuals(), Mesh_, Shader_, 0x5f83cc_rgbf, Drawables_);
    
    if (!setSwapInterval(1)) std::cerr << "UUPPPS" << std::endl;
//     #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_ANDROID)
    setMinimalLoopPeriod(100);
//     #endif
}

void BattleSub::mousePressEvent(MouseEvent& event)
{
    if(event.button() == MouseEvent::Button::Left)
    {
        PlayerSub_->fire(Mesh_, Shader_, Drawables_, -1.0f);
    }
    else if (event.button() == MouseEvent::Button::Right)
    {
        PlayerSub_->fire(Mesh_, Shader_, Drawables_, 1.0f);
    }
}

void BattleSub::keyPressEvent(KeyEvent& Event)
{
    if (Event.key() == KeyEvent::Key::A)
    {
        KeyPressedMap["a"] = true;
        GlobalSubmarineFactory.destroy(PlayerSub2_);
    }
    else if (Event.key() == KeyEvent::Key::D)
    {
        KeyPressedMap["d"] = true;
    }
    else if (Event.key() == KeyEvent::Key::S)
    {
        KeyPressedMap["s"] = true;
    }
    else if (Event.key() == KeyEvent::Key::W)
    {
        KeyPressedMap["w"] = true;
    }
    else if (Event.key() == KeyEvent::Key::P)
    {
        KeyPressedMap["p"] = true;
    }
    else if (Event.key() == KeyEvent::Key::F1)
    {
        KeyPressedMap["F1"] = true;
    }
    else if (Event.key() == KeyEvent::Key::F2)
    {
        KeyPressedMap["F2"] = true;
    }
    else if (Event.key() == KeyEvent::Key::Esc)
    {
        Platform::Application::Sdl2Application::exit();
    }
}

void BattleSub::keyReleaseEvent(KeyEvent& Event)
{
    if (Event.key() == KeyEvent::Key::A)
    {
        KeyPressedMap["a"] = false;
    }
    else if (Event.key() == KeyEvent::Key::D)
    {
        KeyPressedMap["d"] = false;
    }
    else if (Event.key() == KeyEvent::Key::S)
    {
        KeyPressedMap["s"] = false;
    }
    else if (Event.key() == KeyEvent::Key::W)
    {
        KeyPressedMap["w"] = false;
    }
    else if (Event.key() == KeyEvent::Key::F1)
    {
        KeyPressedMap["F1"] = false;
    }
    else if (Event.key() == KeyEvent::Key::F2)
    {
        KeyPressedMap["F2"] = false;
    }
}

void BattleSub::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
    
    if (KeyPressedMap["a"] == true) PlayerSub_->rudderLeft();
    if (KeyPressedMap["d"] == true) PlayerSub_->rudderRight();
    if (KeyPressedMap["s"] == true) PlayerSub_->throttleReverse();
    if (KeyPressedMap["w"] == true) PlayerSub_->throttleForward();
    if (KeyPressedMap["F1"] == true) 
    {
        Zoom_ *= 0.99f;
        VPX_ = 100.0f * Zoom_;
        VPY_ = 100.0f * Zoom_;
    }
    if (KeyPressedMap["F2"] == true) 
    {
        Zoom_ *= 1.01f;
        VPX_ = 100.0f * Zoom_;
        VPY_ = 100.0f * Zoom_;
    }
    Camera_->setProjectionMatrix(Matrix3::projection({VPX_, VPY_}));
    
    // Update game objects
    for (auto Sub : GlobalSubmarineFactory.getEntities())
    {
        Sub.second->update(Drawables_);
    }
    
    // Update physics
    World_->Step(1.0f/60.0f, 8, 3);

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
    
    if (GlobalErrorHandler.checkError() == true) Platform::Application::Sdl2Application::exit();
    
    // Draw the scene
    Camera_->draw(Drawables_);

    swapBuffers();
    redraw();
}

}

MAGNUM_APPLICATION_MAIN(BattleSub::BattleSub)
