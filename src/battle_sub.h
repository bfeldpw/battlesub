#ifndef BATTLE_SUB_H
#define BATTLE_SUB_H

#include <string>
#include <unordered_map>

#include <entt/entt.hpp>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/ImGuiIntegration/Context.hpp>

#include "camera_dynamics.h"
#include "common.h"
#include "contact_listener.h"
#include "fluid_grid.h"
#include "main_display_shader.h"
#include "submarine.h"
#include "timer.h"


namespace BattleSub
{

class BattleSub : public Platform::Application
{

    public:
    
        explicit BattleSub(const Arguments& arguments);

    private:
        
        void drawEvent() override;
        void keyPressEvent(KeyEvent& Event) override;
        void keyReleaseEvent(KeyEvent& Event) override;
        void mouseMoveEvent(MouseMoveEvent& Event) override;
        void mousePressEvent(MouseEvent& Event) override;
        void mouseReleaseEvent(MouseEvent& Event) override;
        void viewportEvent(ViewportEvent& Evend) override;
        
        void cleanupAndExit();
        void updateCameraDynamics();
        void updateGameObjects();
        void updateUI();
        
        void setupWindow();
        void setupFrameBuffersMainScreen();
        void setupMainDisplayMesh();
        void setupCameras();
        void setupGameObjects(entt::registry& Reg);
        
        void showTooltip(const std::string& Tooltip);

        ImGuiIntegration::Context ImGUI_{NoCreate};
        Timer SimTime_;

        //--- World ---//
        int   WindowResolutionX_ = WINDOW_RESOLUTION_MAX_X;
        int   WindowResolutionY_ = WINDOW_RESOLUTION_MAX_Y;

        //--- Framebuffer related ---//
        GL::Framebuffer FBOMainDisplay_{NoCreate};
        GL::Mesh MeshMainDisplay_{NoCreate};
        GL::Texture2D TexMainDisplay_{NoCreate};
        MainDisplayShader ShaderMainDisplay_{NoCreate};
       
        //--- Controls ---//
        std::unordered_map<std::string, bool> KeyPressedMap;
        Vector2i MouseDelta_;
        bool IsDebugDisplayed_ = false;
        bool IsExitTriggered_ = false;
        bool IsMainMenuDisplayed_ = false;
        bool IsPaused_ = false;
        bool IsStepForward_ = false;
        bool IsSplitscreen_ = false;
        bool IsTooltipsEnabled_ = true;

        //-- UI --//
        ImGuiStyle* UIStyle_{nullptr};
        ImGuiStyle  UIStyleSubStats_;
        ImGuiStyle  UIStyleDefault_;
        
        Object2D* CameraObjectCurrentPlayer_{nullptr};
        Object2D* CameraObjectOtherPlayer_{nullptr};
        Object2D* CameraObjectPlayer1_;
        Object2D* CameraObjectPlayer2_;
        Object2D* CameraObjectBoundaries_;
        SceneGraph::Camera2D* CameraCurrentPlayer_{nullptr};
        SceneGraph::Camera2D* CameraOtherPlayer_{nullptr};
        SceneGraph::Camera2D* CameraPlayer1_;
        SceneGraph::Camera2D* CameraPlayer2_;
        SceneGraph::Camera2D* CameraBoundaries_;
        
        ContactListener ContactListener_;
        FluidGrid FluidGrid_;
        FluidBufferE FluidBuffer_{FluidBufferE::FINAL_COMPOSITION};
        float DensityDistortion_ = 300.0f;
        float GammaCorrection_ = 2.2f;
        float VelocityAdvectionFactor_ = 0.9f;
        float VelocityDiffusionGain_ = 2.0f;
        float VelocityDiffusionRate_ = 5.0f;
        float VelocityDisplayScale_ = 20.0f;
        bool  VelocityDisplayShowOnlyMagnitude_ = false;
        float VelocitySourceBackprojection_ = 0.08f; 

        Submarine* PlayerSub_  = nullptr;
        Submarine* PlayerSub2_ = nullptr;
        GameObject* CanyonBoundary = nullptr;
        
        float VisRes_ = 10.0f; // Visual Resolution in pixels per meter
        CameraDynamics Cam1MoveAheadX_;
        CameraDynamics Cam1MoveAheadY_;
        CameraDynamics Cam1Zoom_;
        CameraDynamics Cam2MoveAheadX_;
        CameraDynamics Cam2MoveAheadY_;
        CameraDynamics Cam2Zoom_;
        bool DevCam_ = false;

};

}

#endif // BATTLE_SUB_H
