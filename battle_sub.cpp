#include <iostream>

#include <Box2D/Box2D.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/ConfigurationValue.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/DualComplex.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/TranslationRotationScalingTransformation2D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Trade/MeshData2D.h>

#include "battle_sub.h"
#include "box_drawable.h"
#include "sub_drawable.h"

namespace BattleSub{
    
b2Body* BattleSub::createBody(Object2D& object, const Vector2& halfSize, const b2BodyType type, const DualComplex& transformation, const Float density)
{
    b2BodyDef bodyDefinition;
    bodyDefinition.position.Set(transformation.translation().x(), transformation.translation().y());
    bodyDefinition.angle = Float(transformation.rotation().angle());
    bodyDefinition.type = type;
    bodyDefinition.linearDamping = 0.01f;
    bodyDefinition.angularDamping = 0.01f;
    b2Body* body = World_->CreateBody(&bodyDefinition);

    b2PolygonShape shape;
    shape.SetAsBox(halfSize.x(), halfSize.y());

    b2FixtureDef fixture;
    fixture.friction = 0.8f;
    fixture.density = density;
    fixture.shape = &shape;
    body->CreateFixture(&fixture);

    body->SetUserData(&object);
    object.setScaling(halfSize);

    return body;
}

BattleSub::BattleSub(const Arguments& arguments): Platform::Application{arguments, NoCreate}
{
    /* Make it possible for the user to have some fun */
    Utility::Arguments args;
    args.addOption("transformation", "1 0 0 0").setHelp("transformation", "initial pyramid transformation")
        .addSkippedPrefix("magnum", "engine-specific options")
        .parse(arguments.argc, arguments.argv);
        
    const DualComplex globalTransformation = args.value<DualComplex>("transformation").normalized();

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
        if(!tryCreate(conf, glConf))
            create(conf, glConf.setSampleCount(0));
    }

    /* Configure camera */
    CameraObject_ = new Object2D{&Scene_};
    Camera_ = new SceneGraph::Camera2D{*CameraObject_};
    Camera_->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(Matrix3::projection({20.0f, 20.0f}))
        .setViewport(GL::defaultFramebuffer.viewport().size());

    /* Create the Box2D world with the usual gravity vector */
    World_.emplace(b2Vec2{0.0f, 0.0f});
    
    /* Create Submarine */
    PlayerSub_.emplace(*World_, Scene_);
    b2BodyDef BodyDef;
    BodyDef.type = b2_dynamicBody;
    BodyDef.active = true;
    std::cout << "PlayerSub_->init" << std::endl;
    PlayerSub_->init(BodyDef);
    
    PlayerSub2_.emplace(*World_, Scene_);
    b2BodyDef BodyDef2;
    BodyDef2.type = b2_dynamicBody;
    BodyDef2.active = true;
    BodyDef2.position.Set(0.0f, 5.0f);
    std::cout << "PlayerSub2_->init" << std::endl;
    PlayerSub2_->init(BodyDef2);
    
    /* Create the shader and the box mesh */
    Shader_ = Shaders::Flat2D{};
    Mesh_ = MeshTools::compile(Primitives::squareSolid());

    new SubDrawable(PlayerSub_->getVisuals(), Mesh_, Shader_, 0x2f83cc_rgbf, Drawables_);
    new SubDrawable(PlayerSub2_->getVisuals(), Mesh_, Shader_, 0x2f83cc_rgbf, Drawables_);

    /* Create the ground */
//     auto ground = new Object2D{&_scene};
//     createBody(*ground, {11.0f, 0.5f}, b2_staticBody, DualComplex::translation(Vector2::yAxis(-8.0f)));
//     new BoxDrawable{*ground, _mesh, _shader, 0xa5c9ea_rgbf, Drawables_};

    /* Create a pyramid of boxes */
//     for(std::size_t row = 0; row != 15; ++row) {
//         for(std::size_t item = 0; item != 15 - row; ++item) {
//             auto box = new Object2D{&_scene};
//             const DualComplex transformation = globalTransformation*DualComplex::translation(
//                 {Float(row)*0.6f + Float(item)*1.2f - 8.5f, Float(row)*1.0f - 6.0f});
//             createBody(*box, {0.5f, 0.5f}, b2_dynamicBody, transformation);
//             new BoxDrawable{*box, _mesh, _shader, 0x2f83cc_rgbf, Drawables_};
//         }
//     }
    
    if (!setSwapInterval(1)) std::cerr << "UUPPPS" << std::endl;
//     #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_ANDROID)
    setMinimalLoopPeriod(100);
//     #endif
}

void BattleSub::mousePressEvent(MouseEvent& event)
{
    if(event.button() == MouseEvent::Button::Left)
    {

        /* Calculate mouse position in the Box2D world. Make it relative to window,
        with origin at center and then scale to world size with Y inverted. */
//         const auto position = Camera_->projectionSize()*Vector2::yScale(-1.0f)*(Vector2{event.position()}/Vector2{windowSize()} - Vector2{0.5f});
// 
//         auto destroyer = new Object2D{&_scene};
//         createBody(*destroyer, {0.5f, 0.5f}, b2_dynamicBody, DualComplex::translation(position), 2.0f);
//         new BoxDrawable{*destroyer, _mesh, _shader, 0xffff66_rgbf, Drawables_};
    }
    else if (event.button() == MouseEvent::Button::Right)
    {
        
    }
}

void BattleSub::keyPressEvent(KeyEvent& Event)
{
    if (Event.key() == KeyEvent::Key::A)
    {
        KeyPressedMap["a"] = true;
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
    PlayerSub_->update();
    PlayerSub2_->update();
    
    // Update physics
    World_->Step(1.0f/60.0f, 80, 30);

    // Update object visuals    
    for(b2Body* body = World_->GetBodyList(); body; body = body->GetNext())
    {
        if (body->IsActive())
        {            
            (*static_cast<Object2D*>(body->GetUserData()))
                .setTranslation({body->GetPosition().x, body->GetPosition().y})
                .setRotation(Complex::rotation(Rad(body->GetAngle())));
        }
    }
    
    // Draw the scene
    Camera_->draw(Drawables_);

    swapBuffers();
    redraw();
}

}

MAGNUM_APPLICATION_MAIN(BattleSub::BattleSub)
