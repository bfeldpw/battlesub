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

        ImGuiIntegration::Context ImGUI_{NoCreate};
        
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

        Submarine* PlayerSub_  = nullptr;
        Submarine* PlayerSub2_ = nullptr;
        GameObject* CanyonBoundary = nullptr;
        
        float VPX_ = 100.0f;
        float VPY_ = 100.0f;
        float Zoom_ = 1.0f;
        bool DevCam_ = false;

};

}

#endif // BATTLE_SUB_H
