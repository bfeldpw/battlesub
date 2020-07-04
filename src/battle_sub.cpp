#include <iostream>

#include <Box2D/Box2D.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/ConfigurationValue.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/TextureFormat.h>
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
    entt::registry Registry;

    this->setupWindow();

    this->setupFrameBuffersMainScreen();
    
    this->setupPlayerMesh();
    this->setupPlayerMeshLeft();
    this->setupPlayerMeshRight();
    
    GlobalResources::Get.init();
    
    this->setupCameras();
    
    FluidGrid_.setDensityBase(GlobalResources::Get.getHeightMap())
              .init();
    
    this->setupGameObjects(Registry);
    
    GlobalResources::Get.getWorld()->SetContactListener(&ContactListener_);
    
    if (!setSwapInterval(1))
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_ANDROID)
        setMinimalLoopPeriod(1.0f/60.0f * 1000.0f);
    #endif

    SimTime_.start();
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
        case KeyEvent::Key::T: PlayerSub_->setPose(-100.0f, 20.0f, 0.3f); break;
        case KeyEvent::Key::S: KeyPressedMap["s"] = true; break;
        case KeyEvent::Key::W: KeyPressedMap["w"] = true; break;
        case KeyEvent::Key::X:
            PlayerSub_->fullStop();
            break;
        case KeyEvent::Key::Esc:
        {
            IsExitTriggered_ = true;
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
    
    // Only react if not in UI mode
    if (!ImGUI_.handleMouseMoveEvent(Event))
    {
        if (Event.modifiers() & MouseMoveEvent::Modifier::Ctrl)
        {
            DevCam_ = true;
            if (Event.buttons() & MouseMoveEvent::Button::Left)
            {
                if (MouseDelta_.y() != 0) Zoom_ *= 1.0f-0.01f*MouseDelta_.y();
            }
            else 
            {
                Platform::Application::Sdl2Application::setMouseLocked(true);
                CameraObjectPlayer1_->translate(0.05f*Zoom_*Vector2(MouseDelta_));
                MouseDelta_ = Vector2i();
            }
        }
        else
        {
            Platform::Application::Sdl2Application::setMouseLocked(false);
        }
    }
}

void BattleSub::mousePressEvent(MouseEvent& Event)
{
    // Only react if not in UI mode
    if (!ImGUI_.handleMousePressEvent(Event))
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
}

void BattleSub::mouseReleaseEvent(MouseEvent& Event)
{
    if (ImGUI_.handleMouseReleaseEvent(Event)) return;
}

void BattleSub::viewportEvent(ViewportEvent& Event)
{
    WindowResolutionX_ = Event.framebufferSize().x();
    WindowResolutionY_ = Event.framebufferSize().y();

    GL::defaultFramebuffer.setViewport({{}, Event.framebufferSize()});

//    TexPlayer1_ = GL::Texture2D{};
//    TexPlayer2_ = GL::Texture2D{};
//    TexPlayer1_.setStorage(Math::log2(WindowResolutionX_)+1, GL::TextureFormat::RGBA8, {WindowResolutionX_, WindowResolutionY_});
//    TexPlayer2_.setStorage(Math::log2(WindowResolutionX_)+1, GL::TextureFormat::RGBA8, {WindowResolutionX_, WindowResolutionY_});
//    FBOPlayer1_.setViewport({{}, Event.framebufferSize()});
//    FBOPlayer2_.setViewport({{}, Event.framebufferSize()});

//    FBOPlayer1_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexPlayer1_, 0)
//               .clearColor(0, Color4(0.0f, 0.0f, 0.0f));
//    FBOPlayer2_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexPlayer2_, 0)
//               .clearColor(0, Color4(0.0f, 0.0f, 0.0f));

//    ShaderMainDisplay_.setTransformation(Matrix3::projection({float(WindowResolutionX_), float(WindowResolutionY_)}));

    CameraPlayer1_->setViewport(Event.framebufferSize());


//    this->setupFrameBuffersMainScreen();

    ImGUI_.relayout(Vector2(Event.windowSize()), Event.windowSize(), Event.framebufferSize());
}

void BattleSub::drawEvent()
{
    if (!IsExitTriggered_)
    {
        GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
        GL::defaultFramebuffer.clearColor(Color4(0.0f, 0.0f, 0.0f, 1.0f));
        
        
//         if (KeyPressedMap["a"] == true) PlayerSub_->rudderLeft();
//         if (KeyPressedMap["d"] == true) PlayerSub_->rudderRight();
        if (KeyPressedMap["s"] == true) PlayerSub_->throttleReverse();
        if (KeyPressedMap["w"] == true) PlayerSub_->throttleForward();
        
        CameraPlayer1_->setProjectionMatrix(Matrix3::projection({WindowResolutionX_/VisRes_*Zoom_, WindowResolutionY_/VisRes_*Zoom_}));
        
        FluidGrid_.setDensityDistortion(DensityDistortion_)
                  .setGammaCorrection(GammaCorrection_)
                  .setVelocityAdvectionFactor(VelocityAdvectionFactor_)
                  .setVelocityDiffusionGain(VelocityDiffusionGain_)
                  .setVelocityDiffusionRate(std::pow(10.0f, VelocityDiffusionRate_))
                  .setVelocityDisplayScale(1.0f/VelocityDisplayScale_)
                  .setVelocityDisplayShowOnlyMagnitude(VelocityDisplayShowOnlyMagnitude_);
        
        if (!IsPaused_ || IsStepForward_)
        {
            updateGameObjects();
            if (!DevCam_)
            {
                auto Pos = PlayerSub_->Hull.getBody()->GetPosition();
                CameraObjectPlayer1_->resetTransformation();
                CameraObjectPlayer1_->translate(Vector2(Pos.x, Pos.y));
            }
        }
        
        if (GlobalErrorHandler.checkError() == true)
        {
            IsExitTriggered_ = true;
        }
        
        // Draw the scene
        if (!IsPaused_ || IsStepForward_)
        {
            FluidGrid_.process(SimTime_.time());
            IsStepForward_ = false;
        }
        
        if (IsSplitscreen_)
        {
            MeshDisplayCurrentPlayer_ = &MeshDisplayPlayerLeft_;
            MeshDisplayOtherPlayer_ = &MeshDisplayPlayerRight_;
        }
        else
        {
            MeshDisplayCurrentPlayer_ = &MeshDisplayPlayer_;
            MeshDisplayOtherPlayer_ = &MeshDisplayPlayer_;
        }
        
        FBOCurrentPlayer_->clearColor(0, Color4(0.0f, 0.0f, 0.0f, 1.0f))
                          .bind();
                
        FluidGrid_.display(CameraCurrentPlayer_->projectionMatrix()*CameraCurrentPlayer_->cameraMatrix(),
                            FluidBuffer_);
        
        CameraCurrentPlayer_->draw(*GlobalResources::Get.getDrawables(DrawableGroupsTypeE::WEAPON));
        CameraCurrentPlayer_->draw(*GlobalResources::Get.getDrawables(DrawableGroupsTypeE::DEFAULT));
        
        if (IsSplitscreen_)
        {
            FBOOtherPlayer_->clearColor(0, Color4(0.0f, 0.0f, 0.0f, 1.0f))
                            .bind();
            
            FluidGrid_.display(CameraOtherPlayer_->projectionMatrix()*CameraOtherPlayer_->cameraMatrix(),
                               FluidBuffer_);
            
            CameraOtherPlayer_->draw(*GlobalResources::Get.getDrawables(DrawableGroupsTypeE::WEAPON));
            CameraOtherPlayer_->draw(*GlobalResources::Get.getDrawables(DrawableGroupsTypeE::DEFAULT));
        }
        
        GL::defaultFramebuffer.bind();
        
        ShaderMainDisplay_.bindTexture(*TexCurrentPlayer_);
        MeshDisplayCurrentPlayer_->draw(ShaderMainDisplay_);
        
        if (IsSplitscreen_)
        {
            ShaderMainDisplay_.bindTexture(*TexOtherPlayer_);
            MeshDisplayOtherPlayer_->draw(ShaderMainDisplay_);
        }

        updateUI();
                
        swapBuffers();
        redraw();
        
        DevCam_ = false;
    }
    else
    {
        cleanupAndExit();
    }
}

void BattleSub::cleanupAndExit()
{
    // Resources need to be released first, as long as the GL context is still
    // valid
    GlobalResources::Get.release();
    
    Platform::Application::Sdl2Application::exit();
}

void BattleSub::updateGameObjects()
{
    // Update physics
    GlobalResources::Get.getWorld()->Step(1.0f/60.0f, 40, 15);
    
    FluidGrid_.addDensity(10.0, 0.0, 10.0)
              .addVelocity(10.0, 0.0, -20.0, 0.0,
                           10.0-20.0*double(VelocitySourceBackprojection_), 0.0, -20.0, 0.0);

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
    for (auto Projectile : GlobalFactories::Projectiles.getEntities())
    {
        Projectile.second->update();
        auto Pos = Projectile.second->getBody()->GetPosition();
        auto Vel = Projectile.second->getBody()->GetLinearVelocity();
        
        if (Projectile.second->isSunk())
        {
            GlobalFactories::Projectiles.destroy(Projectile.second);
            break;
        }
        else
        {
            FluidGrid_.addDensity(Pos.x, Pos.y, Vel.Length() * 10.0f)
                      .addVelocity(Pos.x, Pos.y, Vel.x, Vel.y,
                                   Pos.x-Vel.x*VelocitySourceBackprojection_,
                                   Pos.y-Vel.y*VelocitySourceBackprojection_,
                                   Vel.x, Vel.y);
        }
    }
    for (auto Debris : GlobalFactories::Debris.getEntities())
    {
        Debris.second->update();
        auto Pos = Debris.second->getBody()->GetPosition();
        auto Vel = Debris.second->getBody()->GetLinearVelocity();
        
        if (Debris.second->isSunk())
        {
            GlobalFactories::Debris.destroy(Debris.second);
            break;
        }
        else
        {
            FluidGrid_.addDensity(Pos.x, Pos.y, Vel.Length() * 10.0f)
                      .addVelocity(Pos.x, Pos.y, Vel.x, Vel.y);
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
    auto Direction = PlayerSub_->Rudder.getBody()->GetWorldVector({0.0f, -1.0f});
    auto Front     = PlayerSub_->Hull.getBody()->GetWorldPoint({0.0f,  8.0f});
    auto Back      = PlayerSub_->Hull.getBody()->GetWorldPoint({0.0f, -7.0f});
    
    FluidGrid_.addDensity(Propellor.x, Propellor.y, 0.001f*std::abs(PlayerSub_->getThrottle()))
              .addVelocity(Propellor.x, Propellor.y, 0.001f*Direction.x*PlayerSub_->getThrottle(), 0.001f*Direction.y*PlayerSub_->getThrottle());
              
    FluidGrid_.addDensity(Front.x, Front.y, std::abs(PlayerSub_->Hull.getBody()->GetLinearVelocity().Length())*1.0f)
              .addVelocity(Front.x, Front.y, PlayerSub_->Hull.getBody()->GetLinearVelocity().x, 
                                             PlayerSub_->Hull.getBody()->GetLinearVelocity().y,
                           Back.x, Back.y, PlayerSub_->Hull.getBody()->GetLinearVelocity().x, 
                                           PlayerSub_->Hull.getBody()->GetLinearVelocity().y);
}

void BattleSub::updateUI()
{
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
    
    ImGUI_.newFrame();
    {
        ImGui::Begin("Debug");
        
            ImGui::TextColored(ImVec4(1,1,0,1), "Performance");
            ImGui::Indent();
                ImGui::Text("%.3f ms; (%.1f FPS)",
                    1000.0/Double(ImGui::GetIO().Framerate), Double(ImGui::GetIO().Framerate));
            ImGui::Unindent();
        
            int Buffer = static_cast<int>(FluidBuffer_);
            ImGui::NewLine();
            ImGui::TextColored(ImVec4(1,1,0,1), "Buffer Selection");
            ImGui::Indent();
                ImGui::RadioButton("Density Sources", &Buffer, 0);
                ImGui::RadioButton("Density Base", &Buffer, 1);
                ImGui::RadioButton("Velocity Sources", &Buffer, 2);
                ImGui::RadioButton("Velocity Buffer Front", &Buffer, 3);
                ImGui::RadioButton("Velocity Buffer Back", &Buffer, 4);
                ImGui::RadioButton("Density Buffer Front", &Buffer, 5);
                ImGui::RadioButton("Density Buffer Back", &Buffer, 6);
                ImGui::RadioButton("Ground Distorted", &Buffer, 7);
                ImGui::RadioButton("Final Composition", &Buffer, 8);
                ImGui::NewLine();
                ImGui::Checkbox("Velocity: Show only magnitude", &VelocityDisplayShowOnlyMagnitude_);
                    showTooltip("Show magnitude or show colour-coded direction, too.");
            ImGui::Unindent();
            FluidBuffer_ = static_cast<FluidBufferE>(Buffer);
            
            ImGui::NewLine();
            ImGui::TextColored(ImVec4(1,1,0,1), "Fluid Display");
            ImGui::NewLine();
            ImGui::SliderFloat("Gamma Correction", &GammaCorrection_, 0.5f, 5.0f);
                showTooltip("Gamma correction value for the fluid. Default value of 2.2 should gamma correct monitor defaults.\n"
                            "Higher value will brighten up the scene.");
            ImGui::SliderFloat("Velocity Display Scale [0, x] m/s", &VelocityDisplayScale_, 0.1f, 100.0f);
                showTooltip("Scale colour values for displaying velocity\n"
                            "The given value defines the upper bound in m/s, everything above is capped.\n"
                            "E.g. a value of 20.0 will scale colour values to the interval [0, 20] m/s.");
            ImGui::NewLine();
            ImGui::TextColored(ImVec4(1,1,0,1), "Fluid Parameters");
            ImGui::NewLine();
            ImGui::SliderFloat("Density Distortion", &DensityDistortion_, 1.0f, 1000.0f);
                showTooltip("Amount of distortion due to velocity.\nA constant velocity will lead to a constant distortion.\n"
                            "Base density (background) will be distorted by x * advection, e.g.:\n"
                            "  Value 200: A velocity of 1m/s will distort by 200m");
            ImGui::SliderFloat("Velocity Advection Factor", &VelocityAdvectionFactor_, 0.0f, 2.0f);
                showTooltip("Factor for velocity advection.\nA lower value than 1.0 will move the velocity field slower than self-advection.\n"
                            "A higher value than 1.0 will move the velocity field faster than self-advection.");
            ImGui::SliderFloat("Velocity Diffusion Gain", &VelocityDiffusionGain_, 0.0f, 10.0f);
                showTooltip("The higher the value, the more the velocity sources will be amplified.");
            ImGui::SliderFloat("Velocity Diffusion Rate", &VelocityDiffusionRate_, 0.0f, 10.0f);
                showTooltip("The higher the value, the slower the velocity will diffuse.");
            
            ImGui::SliderFloat("Velocity Source Backprojection [s]", &VelocitySourceBackprojection_, 0.0f, 0.3f);
                showTooltip("Back projection of velocities for x seconds to close gaps between frames for dynamic sources.\n");

        ImGui::End();
        
        ImGui::Begin("Menu");
        
            ImGui::Checkbox("Tooltips", &IsTooltipsEnabled_);
                showTooltip("Guess what...");
            ImGui::Checkbox("Split screen", &IsSplitscreen_);
                showTooltip("Toggle split screen mode");
                    
            static std::string Label;
            if (IsPaused_) Label = "Resume";
            else Label = "Pause";
            if (ImGui::Button(Label.c_str())) IsPaused_ ^= 1;
                showTooltip("Pause/Resume the game");
            if (ImGui::Button("Quit")) IsExitTriggered_ = true;
                showTooltip("Quit BattleSub, exit to desktop");

        ImGui::End();
    }
    
    ImGUI_.drawFrame();
    
    GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::disable(GL::Renderer::Feature::Blending);
}

void BattleSub::setupWindow()
{
    /* Try 8x MSAA, fall back to zero samples if not possible. Enable only 2x
       MSAA if we have enough DPI. */
    const Vector2 dpiScaling = this->dpiScaling({});
    
    Configuration conf;
    conf.setSize({WindowResolutionX_, WindowResolutionY_});
    
    conf.setTitle("BattleSub")
        .setSize(conf.size(), dpiScaling)
        .setWindowFlags(Configuration::WindowFlag::Resizable);

    
    GLConfiguration glConf;
    glConf.setSampleCount(dpiScaling.max() < 2.0f ? 8 : 2);
    
    if (!tryCreate(conf, glConf))
    {
        create(conf, glConf.setSampleCount(0));
    }
    
    // Prepare ImGui
    ImGUI_ = ImGuiIntegration::Context({float(WindowResolutionX_), float(WindowResolutionY_)},
    windowSize(), framebufferSize());
    
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
    GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
    GL::Renderer::BlendFunction::OneMinusSourceAlpha);
}

void BattleSub::setupFrameBuffersMainScreen()
{
    FBOPlayer1_ = GL::Framebuffer{{{0, 0},{WINDOW_RESOLUTION_MAX_X, WINDOW_RESOLUTION_MAX_Y}}};
    FBOPlayer2_ = GL::Framebuffer{{{0, 0},{WINDOW_RESOLUTION_MAX_X, WINDOW_RESOLUTION_MAX_Y}}};
    FBOPlayer1_.setViewport({{}, {WindowResolutionX_, WindowResolutionY_}});
    FBOPlayer2_.setViewport({{}, {WindowResolutionX_, WindowResolutionY_}});
    FBOCurrentPlayer_ = &FBOPlayer1_;
    FBOOtherPlayer_ = &FBOPlayer2_;
    TexPlayer1_ = GL::Texture2D{};
    TexPlayer2_ = GL::Texture2D{};
    TexCurrentPlayer_ = &TexPlayer1_;
    TexOtherPlayer_ = &TexPlayer2_;
    
    TexPlayer1_.setMagnificationFilter(GL::SamplerFilter::Linear)
               .setMinificationFilter(GL::SamplerFilter::Linear, GL::SamplerMipmap::Linear)
               .setWrapping(GL::SamplerWrapping::ClampToBorder)
               .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
               .setStorage(Math::log2(WINDOW_RESOLUTION_MAX_X)+1, GL::TextureFormat::RGBA8, {WINDOW_RESOLUTION_MAX_X, WINDOW_RESOLUTION_MAX_Y})
               .generateMipmap();
    TexPlayer2_.setMagnificationFilter(GL::SamplerFilter::Linear)
               .setMinificationFilter(GL::SamplerFilter::Linear, GL::SamplerMipmap::Linear)
               .setWrapping(GL::SamplerWrapping::ClampToBorder)
               .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
               .setStorage(Math::log2(WINDOW_RESOLUTION_MAX_X)+1, GL::TextureFormat::RGBA8, {WINDOW_RESOLUTION_MAX_X, WINDOW_RESOLUTION_MAX_Y})
               .generateMipmap();
    
    FBOPlayer1_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexPlayer1_, 0)
               .clearColor(0, Color4(0.0f, 0.0f, 0.0f));
    FBOPlayer2_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexPlayer2_, 0)
               .clearColor(0, Color4(0.0f, 0.0f, 0.0f));
               
    ShaderMainDisplay_ = MainDisplayShader{};
    ShaderMainDisplay_.setTransformation(Matrix3::projection({float(WindowResolutionX_), float(WindowResolutionY_)}))
                      .bindTexture(TexPlayer1_);
}

void BattleSub::setupPlayerMesh()
{
    struct Vertex {
        Vector2 Pos;
        Vector2 Tex;
    };
    
    float x = WindowResolutionX_*0.5f;
    float y = WindowResolutionY_*0.5f;
    Vertex Data[6]{
        {{-x, -y}, { 0.0f,  0.0f}},
        {{ x, -y}, { 0.5f,  0.0f}},
        {{-x,  y}, { 0.0f,  0.5f}},
        {{-x,  y}, { 0.0f,  0.5f}},
        {{ x, -y}, { 0.5f,  0.0f}},
        {{ x,  y}, { 0.5f,  0.5f}}};
    GL::Buffer Buffer;
    Buffer.setData(Data, GL::BufferUsage::StaticDraw);

    MeshDisplayPlayer_ = GL::Mesh{};
    MeshDisplayPlayer_.setCount(6)
                      .setPrimitive(GL::MeshPrimitive::Triangles)
                      .addVertexBuffer(std::move(Buffer), 0,
                                       MainDisplayShader::Position{},
                                       MainDisplayShader::TextureCoordinates{});
    MeshDisplayCurrentPlayer_ = &MeshDisplayPlayer_;
    MeshDisplayOtherPlayer_ = &MeshDisplayPlayer_;
}

void BattleSub::setupPlayerMeshLeft()
{
    struct Vertex {
        Vector2 Pos;
        Vector2 Tex;
    };
    
    float x = WindowResolutionX_*0.5f;
    float y = WindowResolutionY_*0.5f;
    Vertex Data[6]{
        {{-x, -y}, {0.25f, 0.0f}},
        {{ 0, -y}, {0.75f, 0.0f}},
        {{-x,  y}, {0.25f, 1.0f}},
        {{-x,  y}, {0.25f, 1.0f}},
        {{ 0, -y}, {0.75f, 0.0f}},
        {{ 0,  y}, {0.75f, 1.0f}}
    };

    GL::Buffer Buffer;
    Buffer.setData(Data, GL::BufferUsage::StaticDraw);

    MeshDisplayPlayerLeft_ = GL::Mesh{};
    MeshDisplayPlayerLeft_.setCount(6)
                          .setPrimitive(GL::MeshPrimitive::Triangles)
                          .addVertexBuffer(std::move(Buffer), 0,
                                           MainDisplayShader::Position{},
                                           MainDisplayShader::TextureCoordinates{});
}

void BattleSub::setupPlayerMeshRight()
{
    struct Vertex {
        Vector2 Pos;
        Vector2 Tex;
    };
    
    float x = WindowResolutionX_*0.5f;
    float y = WindowResolutionY_*0.5f;
    Vertex Data[6]{
        {{0, -y}, {0.25f, 0.0f}},
        {{x, -y}, {0.75f, 0.0f}},
        {{0,  y}, {0.25f, 1.0f}},
        {{0,  y}, {0.25f, 1.0f}},
        {{x, -y}, {0.75f, 0.0f}},
        {{x,  y}, {0.75f, 1.0f}}
    };

    GL::Buffer Buffer;
    Buffer.setData(Data, GL::BufferUsage::StaticDraw);

    MeshDisplayPlayerRight_ = GL::Mesh{};
    MeshDisplayPlayerRight_.setCount(6)
                           .setPrimitive(GL::MeshPrimitive::Triangles)
                           .addVertexBuffer(std::move(Buffer), 0,
                                            MainDisplayShader::Position{},
                                            MainDisplayShader::TextureCoordinates{});
}

void BattleSub::setupCameras()
{
    CameraObjectPlayer1_ = new Object2D{GlobalResources::Get.getScene()};
    CameraPlayer1_ = new SceneGraph::Camera2D{*CameraObjectPlayer1_};
    CameraPlayer1_->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(Matrix3::projection({WindowResolutionX_/VisRes_*Zoom_, WindowResolutionY_/VisRes_*Zoom_}))
                   .setViewport(GL::defaultFramebuffer.viewport().size());
    CameraObjectCurrentPlayer_ = CameraObjectPlayer1_;
    CameraCurrentPlayer_ = CameraPlayer1_;
                   
    CameraObjectPlayer2_ = new Object2D{GlobalResources::Get.getScene()};
    CameraPlayer2_ = new SceneGraph::Camera2D{*CameraObjectPlayer2_};
    CameraPlayer2_->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
                   .setProjectionMatrix(Matrix3::projection({WindowResolutionX_/VisRes_*Zoom_, WindowResolutionY_/VisRes_*Zoom_}))
                   .setViewport(GL::defaultFramebuffer.viewport().size());
    CameraObjectOtherPlayer_ = CameraObjectPlayer2_;
    CameraOtherPlayer_ = CameraPlayer2_;
}

void BattleSub::setupGameObjects(entt::registry& Reg)
{
    auto PlayerSub1 = Reg.create();
    auto PlayerSub2 = Reg.create();

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
}

void BattleSub::showTooltip(const std::string& Tooltip)
{
    if (IsTooltipsEnabled_)
    {
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
                ImGui::Text("%s", Tooltip.c_str());
            ImGui::EndTooltip();
        }
    }
}


}

MAGNUM_APPLICATION_MAIN(BattleSub::BattleSub)
