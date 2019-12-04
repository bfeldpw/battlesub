#ifndef BATTLE_SUB_H
#define BATTLE_SUB_H

#include <string>
#include <unordered_map>

#include <Magnum/ImGuiIntegration/Context.hpp>
#include "imgui.h"

#include "common.h"
#include "contact_listener.h"
#include "fluid_grid.h"
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

        ImGuiIntegration::Context ImGUI_{NoCreate};
        
        std::unordered_map<std::string, bool> KeyPressedMap;
        Vector2i MouseDelta_;
        bool IsExitTriggered_ = false;
        bool IsPaused_ = false;
        bool IsStepForward_ = false;
                
        Object2D* CameraObject_;
        SceneGraph::Camera2D* Camera_;
        
        ContactListener ContactListener_;
        FluidGrid FluidGrid_;

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
