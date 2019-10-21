#ifndef BATTLE_SUB_H
#define BATTLE_SUB_H

#include <string>
#include <unordered_map>

#include "common.h"
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
        void mousePressEvent(MouseEvent& event) override;

        b2Body* createBody(Object2D& object, const Vector2& size, b2BodyType type, const DualComplex& transformation, Float density = 1.0f);

        std::unordered_map<std::string, bool> KeyPressedMap;
        
        GL::Mesh Mesh_{NoCreate};
        GL::Mesh MeshProjectile_{NoCreate};
        Shaders::Flat2D Shader_{NoCreate};

        Scene2D Scene_;
        Object2D* CameraObject_;
        SceneGraph::Camera2D* Camera_;
        SceneGraph::DrawableGroup2D Drawables_;
        
        Containers::Optional<b2World> World_;
        
        Sub* PlayerSub_  = nullptr;
        Sub* PlayerSub2_ = nullptr;
        
        float VPX_ = 100.0f;
        float VPY_ = 100.0f;
        float Zoom_ = 1.0f;

};

}

#endif // BATTLE_SUB_H
