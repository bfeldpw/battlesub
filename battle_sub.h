#ifndef BATTLE_SUB_H
#define BATTLE_SUB_H

#include <string>
#include <unordered_map>

#include "common.h"
#include "phys_obj_entity_map.h"
#include "sub.h"
#include "sub_vis.h"

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
        
        PhysObjEntityMap PhysObjEntityMap_;
        
        GL::Mesh _mesh{NoCreate};
        Shaders::Flat2D _shader{NoCreate};

        Scene2D _scene;
        Object2D* _cameraObject;
        SceneGraph::Camera2D* Camera_;
        SceneGraph::DrawableGroup2D Drawables_;
        
        Containers::Optional<b2World> World_;
        Containers::Optional<Sub> PlayerSub_;
        Containers::Optional<Sub> PlayerSub2_;
        Containers::Optional<SubVis> PlayerSubVis_;
        Containers::Optional<SubVis> PlayerSubVis2_;
};

}

#endif // BATTLE_SUB_H
