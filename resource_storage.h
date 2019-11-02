#ifndef RESOURCE_STORAGE_H
#define RESOURCE_STORAGE_H

#include <array>

#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Square.h>
// #include <Magnum/Shaders/Flat.h>
// #include <Magnum/Shaders/VertexColor.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Trade/MeshData2D.h>

#include "game_object.h"

constexpr int GAME_OBJECT_TYPE_ENUM_MAX = 3;

enum class GameObjectType : int
{
    LANDSCAPE,
    PROJECTILE,
    SUBMARINE_HULL,
    SUBMARINE_RUDDER
};

class ResourceStorage
{
    public:
        
        void init();
        void release();
        
        SceneGraph::DrawableGroup2D* getDrawables() {assert(IsInitialised == true); return Drawables_;};
        Shaders::Flat2D* getShader() {assert(IsInitialised == true); return Shader_;}
        Scene2D*         getScene() {assert(IsInitialised == true); return Scene_;}
        
        
        ShapesType* getShapes(GameObjectType t) {assert(IsInitialised == true); return &(Shapes_[int(t)]);}
        MeshesType* getMeshes(GameObjectType t) {assert(IsInitialised == true); return &(Meshes_[int(t)]);}

    private:

        bool IsInitialised = false;
        
        Scene2D* Scene_ = new Scene2D;
        SceneGraph::DrawableGroup2D* Drawables_ = new SceneGraph::DrawableGroup2D;
        Shaders::Flat2D* Shader_ = nullptr;

        std::array<MeshesType, GAME_OBJECT_TYPE_ENUM_MAX> Meshes_;
        std::array<ShapesType, GAME_OBJECT_TYPE_ENUM_MAX> Shapes_;
        
};

#endif // RESOURCE_STORAGE_H
