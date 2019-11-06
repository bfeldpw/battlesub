#ifndef RESOURCE_STORAGE_H
#define RESOURCE_STORAGE_H

#include <array>

#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Trade/MeshData2D.h>

#include "game_object.h"

constexpr int DRAWABLE_GROUPS_TYPE_ENUM_SIZE = 2;
constexpr int GAME_OBJECT_TYPE_ENUM_SIZE = 4;

enum class GameObjectTypeE : int
{
    LANDSCAPE,
    PROJECTILE,
    SUBMARINE_HULL,
    SUBMARINE_RUDDER
};

enum class DrawableGroupsTypeE : int
{
    DEFAULT,
    WEAPON
};

class ResourceStorage
{
    public:
        
        void init();
        void release();
        
        SceneGraph::DrawableGroup2D* getDrawables(DrawableGroupsTypeE t) {assert(IsInitialised == true); return Drawables_[int(t)];};
        Shaders::Flat2D* getShader() {assert(IsInitialised == true); return Shader_;}
        Scene2D*         getScene() {assert(IsInitialised == true); return Scene_;}
        
        
        ShapesType* getShapes(GameObjectTypeE t) {assert(IsInitialised == true); return &(Shapes_[int(t)]);}
        MeshesType* getMeshes(GameObjectTypeE t) {assert(IsInitialised == true); return &(Meshes_[int(t)]);}

    private:

        bool IsInitialised = false;
        
        Scene2D* Scene_ = new Scene2D;
        Shaders::Flat2D* Shader_ = nullptr;

        std::array<SceneGraph::DrawableGroup2D*, DRAWABLE_GROUPS_TYPE_ENUM_SIZE> Drawables_
            {new SceneGraph::DrawableGroup2D, new SceneGraph::DrawableGroup2D};
        std::array<MeshesType, GAME_OBJECT_TYPE_ENUM_SIZE> Meshes_;
        std::array<ShapesType, GAME_OBJECT_TYPE_ENUM_SIZE> Shapes_;
        
};

#endif // RESOURCE_STORAGE_H
