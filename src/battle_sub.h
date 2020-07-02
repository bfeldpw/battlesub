#ifndef BATTLE_SUB_H
#define BATTLE_SUB_H

#include <string>
#include <unordered_map>

#include <Magnum/GL/Framebuffer.h>
#include <Magnum/ImGuiIntegration/Context.hpp>

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
        void updateGameObjects();
        void updateUI();
        
        void setupWindow();
        void setupFrameBuffersMainScreen();
        void setupPlayerMesh();
        void setupPlayerMeshLeft();
        void setupPlayerMeshRight();
        void setupCameras();
        void setupGameObjects();
        
        void showTooltip(const std::string& Tooltip);

        ImGuiIntegration::Context ImGUI_{NoCreate};
        Timer SimTime_;

        //--- World ---//
        int   WindowResolutionX_ = WINDOW_RESOLUTION_DEFAULT_X;
        int   WindowResolutionY_ = WINDOW_RESOLUTION_DEFAULT_Y;

        //--- Framebuffer related ---//
        GL::Framebuffer* FBOCurrentPlayer_{nullptr};
        GL::Framebuffer* FBOOtherPlayer_{nullptr};
        GL::Framebuffer FBOPlayer1_{NoCreate};
        GL::Framebuffer FBOPlayer2_{NoCreate};
        
        GL::Texture2D* TexCurrentPlayer_{nullptr};
        GL::Texture2D* TexOtherPlayer_{nullptr};
        GL::Texture2D TexPlayer1_{NoCreate};
        GL::Texture2D TexPlayer2_{NoCreate};
        
        MainDisplayShader ShaderMainDisplay_{NoCreate};
        
        GL::Mesh* MeshDisplayCurrentPlayer_{nullptr};
        GL::Mesh* MeshDisplayOtherPlayer_{nullptr};
        GL::Mesh MeshDisplayPlayer_{NoCreate};
        GL::Mesh MeshDisplayPlayerLeft_{NoCreate};
        GL::Mesh MeshDisplayPlayerRight_{NoCreate};
        
        //--- Controls ---//
        std::unordered_map<std::string, bool> KeyPressedMap;
        Vector2i MouseDelta_;
        bool IsExitTriggered_ = false;
        bool IsPaused_ = false;
        bool IsStepForward_ = false;
        bool IsSplitscreen_ = false;
        bool IsTooltipsEnabled_ = true;
        
        Object2D* CameraObjectCurrentPlayer_{nullptr};
        Object2D* CameraObjectOtherPlayer_{nullptr};
        Object2D* CameraObjectPlayer1_;
        Object2D* CameraObjectPlayer2_;
        SceneGraph::Camera2D* CameraCurrentPlayer_{nullptr};
        SceneGraph::Camera2D* CameraOtherPlayer_{nullptr};
        SceneGraph::Camera2D* CameraPlayer1_;
        SceneGraph::Camera2D* CameraPlayer2_;
        
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
        float Zoom_ = 1.0f;
        bool DevCam_ = false;

};

}

#endif // BATTLE_SUB_H
