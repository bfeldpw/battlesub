#include <iostream>

#include <thread>

#include <box2d.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/ConfigurationValue.h>
#include <Magnum/DebugTools/FrameProfiler.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Math/Color.h>

#include <Magnum/Platform/Sdl2Application.h>

#include "battle_sub.h"
#include "common.h"
#include "debug_render_system.hpp"
#include "emitter_system.hpp"
#include "fluid_grid.h"
#include "fluid_interaction_system.hpp"
#include "fluid_probes_component.hpp"
#include "fluid_source_component.hpp"
#include "global_factories.h"
#include "global_resources.h"
#include "lua_manager.hpp"
#include "world_def.h"

namespace BattleSub{

BattleSub::BattleSub(const Arguments& arguments): Platform::Application{arguments, NoCreate}
{
    this->setupWindow();
    this->setupECS();
    this->setupLua();
    this->setupFrameBuffersMainScreen();
    this->setupMainDisplayMesh();

    GlobalResources::Get.init();
    
    this->setupCameras();
    
    Reg_.ctx<FluidGrid>().setDensityBase(GlobalResources::Get.getHeightMap())
              .init();
    this->setupGameObjects();
    
    GlobalResources::Get.getWorld()->SetContactListener(&Reg_.ctx<ContactListener>());
    
    // if (!setSwapInterval(1))
    setSwapInterval(0);
    setMinimalLoopPeriod(1.0f/Frequency_ * 1000.0f);

    SimTime_.start();
}

void BattleSub::keyPressEvent(KeyEvent& Event)
{
    switch (Event.key())
    {
        case KeyEvent::Key::A: KeyPressedMap["a"] = true; break;
        case KeyEvent::Key::D:
            if (Event.modifiers() & KeyEvent::Modifier::Ctrl)
                IsDebugDisplayed_ ^= true;
            else
                KeyPressedMap["d"] = true;
            break;
        case KeyEvent::Key::F:
            if (Event.modifiers() & KeyEvent::Modifier::Ctrl)
            {
                Cam1MoveAheadIsAuto_ ^= true;
                Cam1Zoom_.IsAuto ^= true;
            }
            break;
        case KeyEvent::Key::P:
            KeyPressedMap["p"] = true;
            IsPaused_ ^= true;
            break;
        case KeyEvent::Key::Q:
            if (Event.modifiers() & KeyEvent::Modifier::Ctrl)
                IsExitTriggered_ = true;
            break;
        case KeyEvent::Key::T: PlayerSub_->setPose(-100.0f, 20.0f, 0.3f); break;
        case KeyEvent::Key::S:
            if (Event.modifiers() & KeyEvent::Modifier::Ctrl)
                IsSplitscreen_ ^= true;
            else
                KeyPressedMap["s"] = true;
            break;
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
            IsMainMenuDisplayed_ ^= true;
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
        static DebugTools::GLFrameProfiler Profiler
        {
            DebugTools::GLFrameProfiler::Value::FrameTime|
            DebugTools::GLFrameProfiler::Value::GpuDuration, 50
        };
        Profiler.beginFrame();

        auto& Fluid = Reg_.ctx<FluidGrid>();

        GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
        GL::defaultFramebuffer.clearColor(Color4(0.0f, 0.0f, 0.0f, 1.0f));
        
        if (KeyPressedMap["a"] == true) PlayerSub_->rudderLeft();
        else if (KeyPressedMap["d"] == true) PlayerSub_->rudderRight();
        else PlayerSub_->rudderStop();
        if (KeyPressedMap["s"] == true) PlayerSub_->throttleReverse();
        if (KeyPressedMap["w"] == true) PlayerSub_->throttleForward();
        if (KeyPressedMap["down"] == true) PlayerSub2_->throttleReverse();
        if (KeyPressedMap["left"] == true) PlayerSub2_->rudderLeft();
        else if (KeyPressedMap["right"] == true) PlayerSub2_->rudderRight();
        else PlayerSub2_->rudderStop();
        if (KeyPressedMap["up"] == true) PlayerSub2_->throttleForward();
        if (KeyPressedMap["shift_l"] == true) PlayerSub_->fire(Reg_);
        if (KeyPressedMap["shift_r"] == true) PlayerSub2_->fire(Reg_);

        CameraPlayer1_->setProjectionMatrix(Matrix3::projection({WindowResolutionX_/VisRes_*Cam1Zoom_.Value(),
                                                                 WindowResolutionY_/VisRes_*Cam1Zoom_.Value()}));
        CameraPlayer2_->setProjectionMatrix(Matrix3::projection({WindowResolutionX_/VisRes_*Cam2Zoom_.Value(),
                                                                 WindowResolutionY_/VisRes_*Cam2Zoom_.Value()}));
        
        Fluid.setDensityDistortion(DensityDistortion_)
             .setScalarFieldDisplayScale(ScalarFieldDisplayScale_)
             .setVelocityDisplayScale(1.0f/VelocityDisplayScale_)
             .setVelocityDisplayShowOnlyMagnitude(VelocityDisplayShowOnlyMagnitude_);
        
        if (!IsPaused_ || IsStepForward_)
        {
            updateGameObjects();
            if (!DevCam_)
            {
                auto Pos = Reg_.get<PhysicsComponent>(PlayerSub_->Hull).Body_->GetPosition();

                CameraObjectPlayer1_->resetTransformation();
                CameraObjectPlayer1_->translate(Vector2(Pos.x+Cam1MoveAheadX_.Value(),
                                                        Pos.y+Cam1MoveAheadY_.Value()));
                if (IsSplitscreen_)
                {
                    auto Pos = Reg_.get<PhysicsComponent>(PlayerSub2_->Hull).Body_->GetPosition();

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
            Fluid.bindBoundaryMap();
            // Shaders::Flat2D Shader;
            // Shader.setColor(Color4(1.0f, 1.0f));
            // Matrix3 Transformation = Matrix3::translation(Vector2(
            //                                                   PlayerSub_->Hull.getBody()->GetPosition().x,
            //                                                   PlayerSub_->Hull.getBody()->GetPosition().y
            //                                               ))*
            //                          Matrix3::rotation(Rad(PlayerSub_->Hull.getBody()->GetAngle()));
            // Shader.setTransformationProjectionMatrix(Matrix3::projection({WORLD_SIZE_DEFAULT_X, WORLD_SIZE_DEFAULT_Y})*
            //                                          Transformation);
            // auto Meshes = PlayerSub_->Hull.getMeshes();
            // for (auto it = Meshes->begin(); it != Meshes->end(); it++)
            //     Shader.draw(*it);
            // CameraBoundaries_->draw(*GlobalResources::Get.getDrawables(DrawableGroupsTypeE::WEAPON));
            // CameraBoundaries_->draw(*GlobalResources::Get.getDrawables(DrawableGroupsTypeE::DEFAULT));

            std::thread ThreadPhysics(&BattleSub::updateWorld, this);

            Fluid.process(SimTime_.time());
            IsStepForward_ = false;

            ThreadPhysics.join();
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

        Fluid.display(CameraCurrentPlayer_->projectionMatrix()*CameraCurrentPlayer_->cameraMatrix(),
                      FluidBuffer_);

        GL::Renderer::enable(GL::Renderer::Feature::Blending);
        if (FluidBuffer_ != FluidBufferE::BOUNDARIES)
        {
            CameraCurrentPlayer_->draw(*GlobalResources::Get.getDrawables(DrawableGroupsTypeE::WEAPON));
            CameraCurrentPlayer_->draw(*GlobalResources::Get.getDrawables(DrawableGroupsTypeE::DEFAULT));

            if (DebugRender_.IsVelocityProbesEnabled)
            {
                Reg_.ctx<DebugRenderSystem>().renderVelocityProbes(CameraCurrentPlayer_->projectionMatrix()*
                                                                   CameraCurrentPlayer_->cameraMatrix());
            }
            if (DebugRender_.IsVelocityVectorsEnabled)
            {
                Reg_.ctx<DebugRenderSystem>().renderVelocityVectors(CameraCurrentPlayer_->projectionMatrix()*
                                                                    CameraCurrentPlayer_->cameraMatrix());
            }
        }

        // In case of splitscreen, also render the second view
        if (IsSplitscreen_)
        {
            FBOMainDisplay_.setViewport({{std::min(WindowResX, WindowResMaxX), 0},
                                         {std::min(WindowResolutionX_, WINDOW_RESOLUTION_MAX_X),
                                          std::min(WindowResolutionY_, WINDOW_RESOLUTION_MAX_Y)}});
            CameraOtherPlayer_->setProjectionMatrix(Matrix3::projection({WindowResX/VisRes_*Cam2Zoom_.Value(),
                                                                         WindowResolutionY_/VisRes_*Cam2Zoom_.Value()}))
                               .setViewport({WindowResX, WindowResolutionY_});

            Fluid.display(CameraOtherPlayer_->projectionMatrix()*CameraOtherPlayer_->cameraMatrix(),
                          FluidBuffer_);

            if (FluidBuffer_ != FluidBufferE::BOUNDARIES)
            {
                CameraOtherPlayer_->draw(*GlobalResources::Get.getDrawables(DrawableGroupsTypeE::WEAPON));
                CameraOtherPlayer_->draw(*GlobalResources::Get.getDrawables(DrawableGroupsTypeE::DEFAULT));

                if (DebugRender_.IsVelocityProbesEnabled)
                {
                    Reg_.ctx<DebugRenderSystem>().renderVelocityProbes(CameraOtherPlayer_->projectionMatrix()*
                                                                       CameraOtherPlayer_->cameraMatrix());
                }
                if (DebugRender_.IsVelocityVectorsEnabled)
                {
                    Reg_.ctx<DebugRenderSystem>().renderVelocityVectors(CameraOtherPlayer_->projectionMatrix()*
                                                                        CameraOtherPlayer_->cameraMatrix());
                }
            }
        }

        //-----------------------------------------------------------------------
        // Render final scene, i.e. render FBOMainDisplay to default framebuffer
        //-----------------------------------------------------------------------
        GL::defaultFramebuffer.bind();

        ShaderMainDisplay_.bindTexture(TexMainDisplay_)
                          .setTexScale(std::min(1.0f, float(WindowResolutionX_)/WINDOW_RESOLUTION_MAX_X),
                                       std::min(1.0f, float(WindowResolutionY_)/WINDOW_RESOLUTION_MAX_Y));
        ShaderMainDisplay_.draw(MeshMainDisplay_);

        // ShaderMainDisplay_.draw(PlayerSub_->Hull.getMeshes()->at(0));

        if (!IsPaused_ || IsStepForward_)
        {
            updateCameraDynamics();
        }


        static double GPUDur = 0.0;
        if (Profiler.isMeasurementAvailable(DebugTools::GLFrameProfiler::Value::GpuDuration))
            GPUDur = Profiler.gpuDurationMean();

        updateUI(GPUDur);

        GL::Renderer::disable(GL::Renderer::Feature::Blending);

        Profiler.endFrame();

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
        b2Body* const HullBody = Reg_.get<PhysicsComponent>(PlayerSub_->Hull).Body_;
        Vector2 vs{HullBody->GetLinearVelocity().x,
                   HullBody->GetLinearVelocity().y};
        float s = HullBody->GetLinearVelocity().Length();
        float s_x = HullBody->GetLinearVelocity().x;
        float s_y = HullBody->GetLinearVelocity().y;
        Cam1MoveAheadX_.interpolate(s_x*s_x*Math::sign(s_x));
        Cam1MoveAheadY_.interpolate(s_y*s_y*Math::sign(s_y));
        Cam1Zoom_.interpolate(s*s);
    }
    if (IsSplitscreen_)
    {
        b2Body* const HullBody2 = Reg_.get<PhysicsComponent>(PlayerSub2_->Hull).Body_;
        Vector2 vs{HullBody2->GetLinearVelocity().x,
                   HullBody2->GetLinearVelocity().y};
        float s = HullBody2->GetLinearVelocity().Length();
        float s_x = HullBody2->GetLinearVelocity().x;
        float s_y = HullBody2->GetLinearVelocity().y;
        Cam2MoveAheadX_.interpolate(s_x*s_x*Math::sign(s_x));
        Cam2MoveAheadY_.interpolate(s_y*s_y*Math::sign(s_y));
        Cam2Zoom_.interpolate(s*s);
    }
}

void BattleSub::updateGameObjects()
{
    auto& Fluid = Reg_.ctx<FluidGrid>();
    Fluid.addDensity(10.0, 0.0, 100.0)
         .addVelocity(10.0, 0.0, -20.0, 0.0,
                           10.0-20.0*double(VelocitySourceBackprojection_), 0.0, -500.0, 0.0);
    Fluid.addDensity(-10.0, -10.0, 100.0)
         .addVelocity(-10.0, -10.0, 20.0, 0.0,
                           -10.0+20.0*double(VelocitySourceBackprojection_), -10.0, 100.0, 0.0);

    Reg_.ctx<GameObjectFactory>().updateVisuals();
    Reg_.ctx<GameObjectFactory>().updateStatus();
    Reg_.ctx<EmitterSystem>().emit();
    Reg_.ctx<FluidInteractionSystem>().addSources();
    Reg_.ctx<FluidInteractionSystem>().applyForces();


    for (auto Sub : GlobalFactories::Submarines.getEntities())
    {
        Sub.second->update(Reg_);
        b2Body* const HullBody   = Reg_.get<PhysicsComponent>(Sub.second->Hull).Body_;
        b2Body* const RudderBody = Reg_.get<PhysicsComponent>(Sub.second->Rudder).Body_;

        auto Propellor = HullBody->GetWorldPoint({0.0f, -7.0f});
        auto Direction = RudderBody->GetWorldVector({0.0f, -1.0f});

        Fluid.addDensity(Propellor.x, Propellor.y, 0.01f*std::abs(Sub.second->getThrottle()))
             .addVelocity(Propellor.x, Propellor.y, 0.01f*Direction.x*Sub.second->getThrottle(), 0.01f*Direction.y*Sub.second->getThrottle());
    }
}

void BattleSub::updateUI(const double _GPUTime)
{
    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);

    ImGUI_.newFrame();
    {
        if (IsDebugDisplayed_)
        {
            ImGui::Begin("Debug");

                ImGui::TextColored(ImVec4(1,1,0,1), "Performance");
                ImGui::Indent();
                    ImGui::Text("Frame Time:  %.3f ms; (%.1f FPS)",
                        1000.0/Double(ImGui::GetIO().Framerate), Double(ImGui::GetIO().Framerate));
                    ImGui::Text("GPU Time: %.2f ms", Double(_GPUTime*1.0e-6));
                ImGui::Unindent();

                ImGui::NewLine();
                ImGui::TextColored(ImVec4(1,1,0,1), "Debug Renderer");
                ImGui::Indent();
                    ImGui::Checkbox("Velocity Probes", &DebugRender_.IsVelocityProbesEnabled);
                    ImGui::Checkbox("Velocity Vectors", &DebugRender_.IsVelocityVectorsEnabled);
                ImGui::Unindent();

                int Buffer = static_cast<int>(FluidBuffer_);
                ImGui::NewLine();
                ImGui::TextColored(ImVec4(1,1,0,1), "Buffer Selection");
                ImGui::Indent();
                    ImGui::RadioButton("Boundaries", &Buffer, 0);
                    ImGui::RadioButton("Density Sources", &Buffer, 1);
                    ImGui::RadioButton("Density Base", &Buffer, 2);
                    ImGui::RadioButton("Velocity Sources", &Buffer, 3);
                    ImGui::RadioButton("Velocity Buffer Front", &Buffer, 4);
                    ImGui::RadioButton("Velocity Buffer Back", &Buffer, 5);
                    ImGui::RadioButton("Velocity Buffer Readback", &Buffer, 6);
                    ImGui::RadioButton("Velocity Divergence Buffer", &Buffer, 7);
                    ImGui::RadioButton("Pressure Buffer Front", &Buffer, 8);
                    ImGui::RadioButton("Pressure Buffer Back", &Buffer, 9);
                    ImGui::RadioButton("Density Buffer Front", &Buffer, 10);
                    ImGui::RadioButton("Density Buffer Back", &Buffer, 11);
                    ImGui::RadioButton("Ground Distorted", &Buffer, 12);
                    ImGui::RadioButton("Final Composition", &Buffer, 13);
                    ImGui::NewLine();
                    ImGui::Checkbox("Velocity: Show only magnitude", &VelocityDisplayShowOnlyMagnitude_);
                        showTooltip("Show magnitude or show colour-coded direction, too.");
                ImGui::Unindent();
                FluidBuffer_ = static_cast<FluidBufferE>(Buffer);

                ImGui::NewLine();
                ImGui::TextColored(ImVec4(1,1,0,1), "Fluid Display");
                ImGui::NewLine();
                ImGui::SliderFloat("Scalar Field Display Scale", &ScalarFieldDisplayScale_, 0.01, 10.0f);
                ImGui::SliderFloat("Velocity Display Scale [0, x] m/s", &VelocityDisplayScale_, 0.1f, 100.0f);
                    showTooltip("Scale colour values for displaying velocity\n"
                                "The given value defines the upper bound in m/s, everything above is capped.\n"
                                "E.g. a value of 20.0 will scale colour values to the interval [0, 20] m/s.");
                ImGui::NewLine();
                ImGui::TextColored(ImVec4(1,1,0,1), "Fluid Parameters");
                ImGui::NewLine();
                ImGui::SliderInt("Density Diffsion Iterations", &Reg_.ctx<FluidGrid>().Conf.IterationsDensityDiffusion, 1, 20);
                    showTooltip("Number of iterations for numerical diffusion calculation.");
                ImGui::SliderInt("Velocity Diffsion Iterations", &Reg_.ctx<FluidGrid>().Conf.IterationsVelocityDiffusion, 1, 20);
                    showTooltip("Number of iterations for numerical diffusion calculation.");
                ImGui::SliderInt("Pressure Equation Iterations", &Reg_.ctx<FluidGrid>().Conf.IterationsPressureEquation, 1, 80);
                    showTooltip("Number of iterations for solving the pressure equation.");
                ImGui::SliderFloat("Density Distortion", &DensityDistortion_, 1.0f, 100.0f);
                    showTooltip("Amount of distortion due to velocity.\nA constant velocity will lead to a constant distortion.\n"
                                "Base density (background) will be distorted by x * advection, e.g.:\n"
                                "  Value 200: A velocity of 1m/s will distort by 200m");
                ImGui::SliderFloat("Velocity Advection Factor", &Reg_.ctx<FluidGrid>().Conf.VelocityAdvectionFactor, 0.0f, 2.0f);
                    showTooltip("Factor for velocity advection.\nA lower value than 1.0 will move the velocity field slower than self-advection.\n"
                                "A higher value than 1.0 will move the velocity field faster than self-advection.");

                ImGui::SliderFloat("Velocity Source Backprojection [s]", &VelocitySourceBackprojection_, 0.0f, 0.3f);
                    showTooltip("Back projection of velocities for x seconds to close gaps between frames for dynamic sources.\n");

                ImGui::NewLine();
                ImGui::TextColored(ImVec4(1,1,0,1), "Camera");
                ImGui::NewLine();

                ImGui::Indent();
                    ImGui::Text("Toggle Auto Zoom&Move: <ctrl f>");
                    ImGui::NewLine();
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

                    ImGui::NewLine();

                    ImGui::Checkbox("Enable AutoMove", &Cam1MoveAheadIsAuto_);
                        showTooltip("Toggle automatic movement of camera based on submarine speed.\n"
                                    "Moves in velocity direction when accelerating, moves back when decelerating");


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
        }
        // Set correct values for camera dynamics outside of window visibility scope
        // Otherwise, the value is only set if the window is visible
        Cam1MoveAheadX_.IsAuto = Cam1MoveAheadIsAuto_;
        Cam1MoveAheadY_.IsAuto = Cam1MoveAheadIsAuto_;

        if (IsMainMenuDisplayed_)
        {
            ImGui::Begin("Menu");

                ImGui::Checkbox("Tooltips", &IsTooltipsEnabled_);
                    showTooltip("Guess what...");
                ImGui::Checkbox("Split screen   <ctrl-s>", &IsSplitscreen_);
                    showTooltip("Toggle split screen mode");
                ImGui::Checkbox("Debug          <ctrl-d>", &IsDebugDisplayed_);
                    showTooltip("Toggle debug mode");
                if (ImGui::Button("Reload lua config")) this->setupLua();
                    showTooltip("Reload the default lua configuration file");

                static std::string Label;
                if (IsPaused_) Label = "Resume                 <p>";
                else Label = "Pause                  <p>";
                if (ImGui::Button(Label.c_str())) IsPaused_ ^= 1;
                    showTooltip("Pause/Resume the game");
                if (ImGui::Button("Quit              <ctrl-q>")) IsExitTriggered_ = true;
                    showTooltip("Quit BattleSub, exit to desktop");

            ImGui::End();
        }

        // Submarine stats
        ImVec2 StatsPosSub1 = ImVec2(10, 10);
        ImGui::SetNextWindowPos(StatsPosSub1, ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background
        ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoDecoration |
                                       ImGuiWindowFlags_AlwaysAutoResize |
                                       ImGuiWindowFlags_NoSavedSettings |
                                       ImGuiWindowFlags_NoFocusOnAppearing |
                                       ImGuiWindowFlags_NoInputs |
                                       ImGuiWindowFlags_NoNav |
                                       ImGuiWindowFlags_NoMove;
        bool CloseButton = false;
        float Integrity = PlayerSub_->getHullIntegrity() / 100.0f;
        UIStyleSubStats_.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f-Integrity, Integrity, 0.0f, 0.5f);
        *UIStyle_ = UIStyleSubStats_;
        ImGui::Begin("Submarine 1", &CloseButton, WindowFlags);
            ImGui::Text("Hull Integrity                    ");
            ImGui::ProgressBar(Integrity);
        ImGui::End();
        if (IsSplitscreen_)
        {
            ImVec2 StatsPosSub2 = ImVec2(WindowResolutionX_/2 + 10, 10);
            ImGui::SetNextWindowPos(StatsPosSub2, ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background
            ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoDecoration |
                                           ImGuiWindowFlags_AlwaysAutoResize |
                                           ImGuiWindowFlags_NoSavedSettings |
                                           ImGuiWindowFlags_NoFocusOnAppearing |
                                           ImGuiWindowFlags_NoInputs |
                                           ImGuiWindowFlags_NoNav |
                                           ImGuiWindowFlags_NoMove;
            bool CloseButton = false;
            float Integrity = PlayerSub2_->getHullIntegrity() / 100.0f;
            UIStyleSubStats_.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f-Integrity, Integrity, 0.0f, 0.5f);
            *UIStyle_ = UIStyleSubStats_;
            ImGui::Begin("Submarine 2", &CloseButton, WindowFlags);
                ImGui::Text("Hull Integrity                    ");
                ImGui::ProgressBar(Integrity);
            ImGui::End();
        }
        *UIStyle_ = UIStyleDefault_;
    }

    ImGUI_.drawFrame();

    GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
}

void BattleSub::updateWorld()
{
    // Update physics
    GlobalResources::Get.getWorld()->Step(1.0f/Frequency_, 40, 15);
}

void BattleSub::setupECS()
{
    Reg_.set<LuaManager>(Reg_);
    Reg_.set<ContactListener>(Reg_);
    Reg_.set<DebugRenderSystem>(Reg_);
    Reg_.set<EmitterSystem>(Reg_);
    Reg_.set<FluidGrid>(Reg_);
    Reg_.set<FluidInteractionSystem>(Reg_);
    Reg_.set<GameObjectFactory>(Reg_);
    Reg_.set<MessageHandler>();
    Reg_.ctx<MessageHandler>().setLevel(MessageHandler::DEBUG_L1);
    Reg_.set<ErrorHandler>();
}

void BattleSub::setupLua()
{
    Reg_.ctx<LuaManager>().loadFile("config.lua");
    Reg_.ctx<FluidGrid>().loadConfig();
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
    UIStyle_ = &ImGui::GetStyle();
    UIStyleDefault_ = *UIStyle_;
    UIStyleSubStats_ = *UIStyle_;
    UIStyleSubStats_.WindowRounding = 0.0f;

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
                   .clearColor(0, Color4(0.0f, 0.0f, 0.0f, 1.0f));
              
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


    CameraObjectBoundaries_ = new Object2D{GlobalResources::Get.getScene()};
    CameraBoundaries_ = new SceneGraph::Camera2D{*CameraObjectBoundaries_};
    CameraBoundaries_->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(Matrix3::projection({WORLD_SIZE_DEFAULT_X, WORLD_SIZE_DEFAULT_Y}))
        .setViewport({int(WORLD_SIZE_DEFAULT_X), int(WORLD_SIZE_DEFAULT_Y)});
}

void BattleSub::setupGameObjects()
{
    PlayerSub_ = GlobalFactories::Submarines.create();
    PlayerSub_->create(Reg_, 0.0f, -20.0f, 0.0f);

    PlayerSub2_ = GlobalFactories::Submarines.create();
    PlayerSub2_->create(Reg_, 10.0f, 40.0f, 3.14159f);
    static Submarine* Sub3 = GlobalFactories::Submarines.create();
    Sub3->create(Reg_, -20.0f, 20.0f, 4.5f);

    auto CanyonBoundary = Reg_.create();
    b2BodyDef BodyDef;
    BodyDef.type = b2_staticBody;
    BodyDef.enabled = true;
    BodyDef.position.Set(0.0f, 0.0f);
    Reg_.ctx<GameObjectFactory>().create(CanyonBoundary, nullptr, GameObjectTypeE::LANDSCAPE,
                                         DrawableGroupsTypeE::DEFAULT, {0.05f, 0.05f, 0.1f, 1.0f}, BodyDef);
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
