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
    
    /* Create Submarine */
//     PlayerSub_.emplace(&(*World_), &Scene_);
//     PlayerSub_.emplace();
//     PlayerSub_->create(&(*World_), &Scene_);
//     b2BodyDef BodyDef;
//     BodyDef.type = b2_dynamicBody;
//     BodyDef.active = true;
//     BodyDef.position.Set(0.0f, -20.0f);
//     std::cout << "PlayerSub_->init" << std::endl;
//     PlayerSub_->init(BodyDef);
    
    
    
    PlayerSub_ = GlobalSubmarineFactory.createSubmarine();
    PlayerSub_->create(&(*World_), &Scene_);
    b2BodyDef BodyDef;
    BodyDef.type = b2_dynamicBody;
    BodyDef.active = true;
    BodyDef.position.Set(0.0f, -20.0f);
    std::cout << "PlayerSub_->init" << std::endl;
    PlayerSub_->init(BodyDef);
    
    PlayerSub2_ = GlobalSubmarineFactory.createSubmarine();
    PlayerSub2_->create(&(*World_), &Scene_);
    b2BodyDef BodyDef2;
    BodyDef2.type = b2_dynamicBody;
    BodyDef2.active = true;
    BodyDef2.position.Set(0.0f, 20.0f);
    std::cout << "PlayerSub2_->init" << std::endl;
    PlayerSub2_->init(BodyDef2);
    
    /* Create the shader and the box mesh */
    Shader_ = Shaders::Flat2D{};
    Mesh_ = MeshTools::compile(Primitives::squareSolid());
    
//     const struct {
//         Vector2 pos;
//     } data[]{{{-0.1f, 0.0f}},
//             {{  0.1f, 0.0f}},
//             {{  0.1f, 0.8f}},
//             {{  0.0f, 1.0f}},
//             {{ -0.1f, 0.8f}}};
// 
//     GL::Buffer buffer;
//     buffer.setData(data);
//     MeshProjectile_ = GL::Mesh{};
//     MeshProjectile_.setCount(5)
//         .addVertexBuffer(std::move(buffer), 0,
//             Shaders::VertexColor2D::Position{});

    new SubDrawable(PlayerSub_->getVisuals(), Mesh_, Shader_, 0x2f83cc_rgbf, Drawables_);
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
        /* Calculate mouse position in the Box2D world. Make it relative to window,
        with origin at center and then scale to world size with Y inverted. */
//         const auto position = Camera_->projectionSize()*Vector2::yScale(-1.0f)*(Vector2{event.position()}/Vector2{windowSize()} - Vector2{0.5f});
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
        GlobalSubmarineFactory.destroySubmarine(PlayerSub2_);
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
}

void BattleSub::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
    
    if (KeyPressedMap["a"] == true) PlayerSub_->rudderLeft();
    if (KeyPressedMap["d"] == true) PlayerSub_->rudderRight();
    if (KeyPressedMap["s"] == true) PlayerSub_->throttleReverse();
    if (KeyPressedMap["w"] == true) PlayerSub_->throttleForward();
    
    // Update game objects
    PlayerSub_->update(Drawables_);
    PlayerSub2_->update(Drawables_);
    
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
