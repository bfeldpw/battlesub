#ifndef BATTLE_SUB_H
#define BATTLE_SUB_H

#include <string>
#include <unordered_map>

#include "common.h"
#include "landscape.h"
#include "sub.h"

namespace BattleSub
{
    
using namespace Magnum;
using namespace Magnum::Math::Literals;

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
        
        void updateGameObjects();

        std::unordered_map<std::string, bool> KeyPressedMap;
        Vector2i MouseDelta_;
        bool IsPaused_ = false;
        bool IsStepForward_ = false;
        
        Shaders::Flat2D Shader_{NoCreate};

        Scene2D Scene_;
        Object2D* CameraObject_;
        SceneGraph::Camera2D* Camera_;
        SceneGraph::DrawableGroup2D Drawables_;
        
        Containers::Optional<b2World> World_;

        Sub* PlayerSub_  = nullptr;
        Sub* PlayerSub2_ = nullptr;
        Landscape* CanyonBoundary = nullptr;
        
        float VPX_ = 100.0f;
        float VPY_ = 100.0f;
        float Zoom_ = 1.0f;

};

}

#endif // BATTLE_SUB_H
