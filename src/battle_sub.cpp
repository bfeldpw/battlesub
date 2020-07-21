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
    this->setupWindow();
    this->setupFrameBuffersMainScreen();
    this->setupMainDisplayMesh();

    GlobalResources::Get.init();
    
    this->setupCameras();
    
    FluidGrid_.setDensityBase(GlobalResources::Get.getHeightMap())
              .init();
    
    this->setupGameObjects();
    
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
        case KeyEvent::Key::A: KeyPressedMap["a"] = true; break;
        case KeyEvent::Key::D: KeyPressedMap["d"] = true; break;
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
        case KeyEvent::Key::Down: KeyPressedMap["down"] = true; break;
        case KeyEvent::Key::Left: KeyPressedMap["left"] = true; break;
        case KeyEvent::Key::Right: KeyPressedMap["right"] = true; break;
        case KeyEvent::Key::Up: KeyPressedMap["up"] = true; break;
        case KeyEvent::Key::LeftCtrl:
            KeyPressedMap["ctrl_l"] = true;
            DevCam_ = true;
            break;
        case KeyEvent::Key::RightCtrl: KeyPressedMap["ctrl_r"] = true; break;
        case KeyEvent::Key::LeftShift: KeyPressedMap["shift_l"] = true; break;
        case KeyEvent::Key::RightShift: KeyPressedMap["shift_r"] = true; break;
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
        case KeyEvent::Key::A: KeyPressedMap["a"] = false; break;
        case KeyEvent::Key::D: KeyPressedMap["d"] = false; break;
        case KeyEvent::Key::S: KeyPressedMap["s"] = false; break;
        case KeyEvent::Key::W: KeyPressedMap["w"] = false; break;
        case KeyEvent::Key::Down: KeyPressedMap["down"] = false; break;
        case KeyEvent::Key::Left: KeyPressedMap["left"] = false; break;
        case KeyEvent::Key::Right: KeyPressedMap["right"] = false; break;
        case KeyEvent::Key::Up: KeyPressedMap["up"] = false; break;
        case KeyEvent::Key::LeftCtrl:
            KeyPressedMap["ctrl_l"] = false;
            DevCam_ = false;
            break;
        case KeyEvent::Key::RightCtrl: KeyPressedMap["ctrl_r"] = false; break;
        case KeyEvent::Key::LeftShift: KeyPressedMap["shift_l"] = false; break;
        case KeyEvent::Key::RightShift: KeyPressedMap["shift_r"] = false; break;
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
            if (Event.buttons() & MouseMoveEvent::Button::Left)
            {
                if (MouseDelta_.y() != 0)
                {
                    Cam1Zoom_.increaseByMultiplication(1.0f-0.01f*MouseDelta_.y());
                }
            }
            else 
            {
                Platform::Application::Sdl2Application::setCursor(Cursor::HiddenLocked);
                CameraObjectPlayer1_->translate(0.05f*Cam1Zoom_.Value()*Vector2(MouseDelta_));
                MouseDelta_ = Vector2i();
            }
        }
        else
        {
            Platform::Application::Sdl2Application::setCursor(Cursor::Arrow);
        }
    }
}

void BattleSub::mousePressEvent(MouseEvent& Event)
{
    // Only react if not in UI mode
    if (ImGUI_.handleMousePressEvent(Event)) return;
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

    ImGUI_.relayout(Vector2(Event.windowSize()), Event.windowSize(), Event.framebufferSize());
}

void BattleSub::drawEvent()
{
    if (!IsExitTriggered_)
    {
        GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
        GL::defaultFramebuffer.clearColor(Color4(0.0f, 0.0f, 0.0f, 1.0f));
        
        
        if (KeyPressedMap["a"] == true) PlayerSub_->rudderLeft();
        if (KeyPressedMap["d"] == true) PlayerSub_->rudderRight();
        if (KeyPressedMap["s"] == true) PlayerSub_->throttleReverse();
        if (KeyPressedMap["w"] == true) PlayerSub_->throttleForward();
        if (KeyPressedMap["down"] == true) PlayerSub2_->throttleReverse();
        if (KeyPressedMap["left"] == true) PlayerSub2_->rudderLeft();
        if (KeyPressedMap["right"] == true) PlayerSub2_->rudderRight();
        if (KeyPressedMap["up"] == true) PlayerSub2_->throttleForward();
        if (KeyPressedMap["shift_l"] == true) PlayerSub_->fire();
        if (KeyPressedMap["shift_r"] == true) PlayerSub2_->fire();


        
        CameraPlayer1_->setProjectionMatrix(Matrix3::projection({WindowResolutionX_/VisRes_*Cam1Zoom_.Value(),
                                                                 WindowResolutionY_/VisRes_*Cam1Zoom_.Value()}));
        CameraPlayer2_->setProjectionMatrix(Matrix3::projection({WindowResolutionX_/VisRes_*Cam2Zoom_.Value(),
                                                                 WindowResolutionY_/VisRes_*Cam2Zoom_.Value()}));
        
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
                CameraObjectPlayer1_->translate(Vector2(Pos.x+Cam1MoveAheadX_.Value(),
                                                        Pos.y+Cam1MoveAheadY_.Value()));
                if (IsSplitscreen_)
                {
                    auto Pos = PlayerSub2_->Hull.getBody()->GetPosition();

                    CameraObjectPlayer2_->resetTransformation();
                    CameraObjectPlayer2_->translate(Vector2(Pos.x+Cam2MoveAheadX_.Value(),
                                                            Pos.y+Cam2MoveAheadY_.Value()));
                }
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

        //----------------------------------
        // Adjust viewports and projections
        //----------------------------------
        // FBO viewport may not exceed maximum size of underlying texture. Above
        // this size, texture won't be pixel perfect but interpolated
        //
        auto WindowResX = WindowResolutionX_;
        auto WindowResMaxX = WINDOW_RESOLUTION_MAX_X;
        if (IsSplitscreen_)
        {
            WindowResX /= 2;
            WindowResMaxX /= 2;
        }

        FBOMainDisplay_.clearColor(0, Color4(0.2f, 0.2f, 0.3f, 1.0f))
                       .setViewport({{0, 0}, {std::min(WindowResX, WindowResMaxX),
                                              std::min(WindowResolutionY_, WINDOW_RESOLUTION_MAX_Y)}})
                       .bind();
        // Camera projection and viewport have to use the full resolution, even
        // if above texture size, in order to render with the correct aspect ratio
        CameraCurrentPlayer_->setProjectionMatrix(Matrix3::projection({WindowResX/VisRes_*Cam1Zoom_.Value(),
                                                                       WindowResolutionY_/VisRes_*Cam1Zoom_.Value()}))
                             .setViewport({{WindowResX, WindowResolutionY_}});

        FluidGrid_.display(CameraCurrentPlayer_->projectionMatrix()*CameraCurrentPlayer_->cameraMatrix(),
                            FluidBuffer_);
        
        CameraCurrentPlayer_->draw(*GlobalResources::Get.getDrawables(DrawableGroupsTypeE::WEAPON));
        CameraCurrentPlayer_->draw(*GlobalResources::Get.getDrawables(DrawableGroupsTypeE::DEFAULT));

        // In case of splitscreen, also render the second view
        if (IsSplitscreen_)
        {
            FBOMainDisplay_.setViewport({{std::min(WindowResX, WindowResMaxX), 0},
                                         {std::min(WindowResolutionX_, WINDOW_RESOLUTION_MAX_X),
                                          std::min(WindowResolutionY_, WINDOW_RESOLUTION_MAX_Y)}});
            CameraOtherPlayer_->setProjectionMatrix(Matrix3::projection({WindowResX/VisRes_*Cam2Zoom_.Value(),
                                                                         WindowResolutionY_/VisRes_*Cam2Zoom_.Value()}))
                               .setViewport({WindowResX, WindowResolutionY_});

            FluidGrid_.display(CameraOtherPlayer_->projectionMatrix()*CameraOtherPlayer_->cameraMatrix(),
                               FluidBuffer_);
            
            CameraOtherPlayer_->draw(*GlobalResources::Get.getDrawables(DrawableGroupsTypeE::WEAPON));
            CameraOtherPlayer_->draw(*GlobalResources::Get.getDrawables(DrawableGroupsTypeE::DEFAULT));
        }

        //-----------------------------------------------------------------------
        // Render final scene, i.e. render FBOMainDisplay to default framebuffer
        //-----------------------------------------------------------------------
        GL::defaultFramebuffer.bind();
        
        ShaderMainDisplay_.bindTexture(TexMainDisplay_)
                          .setTexScale(std::min(1.0f, float(WindowResolutionX_)/WINDOW_RESOLUTION_MAX_X),
                                       std::min(1.0f, float(WindowResolutionY_)/WINDOW_RESOLUTION_MAX_Y));
        MeshMainDisplay_.draw(ShaderMainDisplay_);

        if (!IsPaused_ || IsStepForward_)
        {
            updateCameraDynamics();
        }
        updateUI();
                
        swapBuffers();
        redraw();
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

void BattleSub::updateCameraDynamics()
{
    {
        Vector2 vs{PlayerSub_->Hull.getBody()->GetLinearVelocity().x,
                   PlayerSub_->Hull.getBody()->GetLinearVelocity().y};
        float s = PlayerSub_->Hull.getBody()->GetLinearVelocity().Length();
        float s_x = PlayerSub_->Hull.getBody()->GetLinearVelocity().x;
        float s_y = PlayerSub_->Hull.getBody()->GetLinearVelocity().y;
        Cam1MoveAheadX_.interpolate(s_x*s_x*Math::sign(s_x));
        Cam1MoveAheadY_.interpolate(s_y*s_y*Math::sign(s_y));
        Cam1Zoom_.interpolate(s*s);
    }
    if (IsSplitscreen_)
    {
        Vector2 vs{PlayerSub2_->Hull.getBody()->GetLinearVelocity().x,
                   PlayerSub2_->Hull.getBody()->GetLinearVelocity().y};
        float s = PlayerSub2_->Hull.getBody()->GetLinearVelocity().Length();
        float s_x = PlayerSub2_->Hull.getBody()->GetLinearVelocity().x;
        float s_y = PlayerSub2_->Hull.getBody()->GetLinearVelocity().y;
        Cam2MoveAheadX_.interpolate(s_x*s_x*Math::sign(s_x));
        Cam2MoveAheadY_.interpolate(s_y*s_y*Math::sign(s_y));
        Cam2Zoom_.interpolate(s*s);
    }
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
    for (auto Sub : GlobalFactories::Submarines.getEntities())
    {
        Sub.second->update();
        auto Propellor = Sub.second->Hull.getBody()->GetWorldPoint({0.0f, -7.0f});
        auto Direction = Sub.second->Rudder.getBody()->GetWorldVector({0.0f, -1.0f});
        auto Front     = Sub.second->Hull.getBody()->GetWorldPoint({0.0f,  8.0f});
        auto Back      = Sub.second->Hull.getBody()->GetWorldPoint({0.0f, -7.0f});

        FluidGrid_.addDensity(Propellor.x, Propellor.y, 0.001f*std::abs(Sub.second->getThrottle()))
                  .addVelocity(Propellor.x, Propellor.y, 0.001f*Direction.x*Sub.second->getThrottle(), 0.001f*Direction.y*Sub.second->getThrottle());

        FluidGrid_.addDensity(Front.x, Front.y, std::abs(Sub.second->Hull.getBody()->GetLinearVelocity().Length())*1.0f)
                  .addVelocity(Front.x, Front.y, Sub.second->Hull.getBody()->GetLinearVelocity().x,
                                                 Sub.second->Hull.getBody()->GetLinearVelocity().y,
                               Back.x,   Back.y, Sub.second->Hull.getBody()->GetLinearVelocity().x,
                                                 Sub.second->Hull.getBody()->GetLinearVelocity().y);
    }
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

        ImGui::Begin("Camera Dynamics Debug");

            ImGui::TextColored(ImVec4(1,1,0,1), "AutoZoom");
            ImGui::Indent();
                ImGui::Checkbox("Enable AutoZoom", &Cam1Zoom_.IsAuto);
                    showTooltip("Toggle automatic zooming of camera based on submarine speed.\n"
                                "Zoom out when accelerating, zooms in when decelerating");
                    if (Cam1Zoom_.IsAuto) Cam1Zoom_.Base = 0.2f;
                std::vector<float> Vals;
                for (const auto Val : Cam1Zoom_.Values) {Vals.push_back(Val);}
                ImGui::PlotLines("Zoom", Vals.data(), Vals.size());
                    showTooltip("Zoom history, 600 frames = 10s");
                ImGui::SliderFloat("AutoZoom Speed", &Cam1Zoom_.Speed, 0.01f, 0.5f);
                    showTooltip("Speed to adapt to new target value, lower is more dampened");
                ImGui::SliderFloat("AutoZoom Strength", &Cam1Zoom_.Strength, 0.0f, 0.02f);
                    showTooltip("Amplitude of AutoZoom, lower is less zooming");
            ImGui::Unindent();

            ImGui::NewLine();

            ImGui::TextColored(ImVec4(1,1,0,1), "AutoMove");
            ImGui::Indent();
                static bool Cam1MoveAheadIsAuto_s = true;

                ImGui::Checkbox("Enable AutoMove", &Cam1MoveAheadIsAuto_s);
                    showTooltip("Toggle automatic movement of camera based on submarine speed.\n"
                                "Moves in velocity direction when accelerating, moves back when decelerating");

                Cam1MoveAheadX_.IsAuto = Cam1MoveAheadIsAuto_s;
                Cam1MoveAheadY_.IsAuto = Cam1MoveAheadIsAuto_s;

                std::vector<float> ValsMoveX;
                std::vector<float> ValsMoveY;
                for (const auto ValMove : Cam1MoveAheadX_.Values) ValsMoveX.push_back(ValMove);
                for (const auto ValMove : Cam1MoveAheadY_.Values) ValsMoveY.push_back(ValMove);
                ImGui::PlotLines("Move X", ValsMoveX.data(), ValsMoveX.size());
                    showTooltip("AutoMove history, 600 frames = 10s, x component");
                ImGui::PlotLines("Move Y", ValsMoveY.data(), ValsMoveY.size());
                    showTooltip("AutoMove history, 600 frames = 10s, y component");

                static float Cam1MoveAheadSpeed_s = 0.1f;
                static float Cam1MoveAheadStrength_s = 0.25f;

                ImGui::SliderFloat("AutoMove Speed", &Cam1MoveAheadSpeed_s, 0.01f, 0.5f);
                    showTooltip("Speed to adapt to new target value, lower is more dampened");
                ImGui::SliderFloat("AutoMove Strength", &Cam1MoveAheadStrength_s, 0.0f, 1.0f);
                    showTooltip("Amplitude of AutoMove, lower is less movement");

                Cam1MoveAheadX_.Speed = Cam1MoveAheadSpeed_s;
                Cam1MoveAheadY_.Speed = Cam1MoveAheadSpeed_s;
                Cam1MoveAheadX_.Strength = Cam1MoveAheadStrength_s;
                Cam1MoveAheadY_.Strength = Cam1MoveAheadStrength_s;
            ImGui::Unindent();

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
    FBOMainDisplay_ = GL::Framebuffer{{{0, 0},{WINDOW_RESOLUTION_MAX_X, WINDOW_RESOLUTION_MAX_Y}}};
    TexMainDisplay_ = GL::Texture2D{};

    TexMainDisplay_.setMagnificationFilter(GL::SamplerFilter::Linear)
                   .setMinificationFilter(GL::SamplerFilter::Linear, GL::SamplerMipmap::Linear)
                   .setWrapping(GL::SamplerWrapping::ClampToBorder)
                   .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
                   .setStorage(1, GL::TextureFormat::RGBA8, {WINDOW_RESOLUTION_MAX_X, WINDOW_RESOLUTION_MAX_Y});

    FBOMainDisplay_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexMainDisplay_, 0)
                   .clearColor(0, Color4(0.0f, 0.0f, 0.0f));
              
    ShaderMainDisplay_ = MainDisplayShader{};
    ShaderMainDisplay_.bindTexture(TexMainDisplay_);
                      // .setTexScale(1.5f);
}

void BattleSub::setupMainDisplayMesh()
{
    MeshMainDisplay_ = GL::Mesh{};
    MeshMainDisplay_.setCount(3)
                    .setPrimitive(GL::MeshPrimitive::Triangles);
}

void BattleSub::setupCameras()
{
    CameraObjectPlayer1_ = new Object2D{GlobalResources::Get.getScene()};
    CameraPlayer1_ = new SceneGraph::Camera2D{*CameraObjectPlayer1_};
    CameraPlayer1_->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::NotPreserved)
        .setProjectionMatrix(Matrix3::projection({WindowResolutionX_/VisRes_*Cam1Zoom_.Value(),
                                                  WindowResolutionY_/VisRes_*Cam1Zoom_.Value()}))
                   .setViewport(GL::defaultFramebuffer.viewport().size());
    CameraObjectCurrentPlayer_ = CameraObjectPlayer1_;
    CameraCurrentPlayer_ = CameraPlayer1_;
                   
    CameraObjectPlayer2_ = new Object2D{GlobalResources::Get.getScene()};
    CameraPlayer2_ = new SceneGraph::Camera2D{*CameraObjectPlayer2_};
    CameraPlayer2_->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::NotPreserved)
        .setProjectionMatrix(Matrix3::projection({WindowResolutionX_/VisRes_*Cam1Zoom_.Value(),
                                                  WindowResolutionY_/VisRes_*Cam1Zoom_.Value()}))
                   .setViewport(GL::defaultFramebuffer.viewport().size());
    CameraObjectOtherPlayer_ = CameraObjectPlayer2_;
    CameraOtherPlayer_ = CameraPlayer2_;

    Cam1Zoom_.Base = 0.2f;
    Cam1Zoom_.Speed = 0.1f;
    Cam1Zoom_.Strength = 0.005f;
    Cam1MoveAheadX_.Base = 0.0f;
    Cam1MoveAheadX_.Speed = 0.1f;
    Cam1MoveAheadX_.Strength = 0.25f;
    Cam1MoveAheadY_.Base = 0.0f;
    Cam1MoveAheadY_.Speed = 0.1f;
    Cam1MoveAheadY_.Strength = 0.25f;
    Cam2Zoom_.Base = 0.2f;
    Cam2Zoom_.Speed = 0.1f;
    Cam2Zoom_.Strength = 0.005f;
    Cam2MoveAheadX_.Base = 0.0f;
    Cam2MoveAheadX_.Speed = 0.1f;
    Cam2MoveAheadX_.Strength = 0.25f;
    Cam2MoveAheadY_.Base = 0.0f;
    Cam2MoveAheadY_.Speed = 0.1f;
    Cam2MoveAheadY_.Strength = 0.25f;
}

void BattleSub::setupGameObjects()
{
    PlayerSub_ = GlobalFactories::Submarines.create();
    PlayerSub_->create(0.0f, -20.0f, 0.0f);

    PlayerSub2_ = GlobalFactories::Submarines.create();
    PlayerSub2_->create(10.0f, 40.0f, 3.14159f);
    static Submarine* Sub3 = GlobalFactories::Submarines.create();
    Sub3->create(-20.0f, 20.0f, 4.5f);

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
