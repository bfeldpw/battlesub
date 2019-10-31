#ifndef RESOURCE_STORAGE_H
#define RESOURCE_STORAGE_H

#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Square.h>
// #include <Magnum/Shaders/Flat.h>
// #include <Magnum/Shaders/VertexColor.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Trade/MeshData2D.h>

#include "game_object.h"

class ResourceStorage
{
    public:
        
        void init();
        void release();
        
        SceneGraph::DrawableGroup2D* getDrawables() {assert(IsInitialised == true); return Drawables_;};
        Shaders::Flat2D* getShader() {assert(IsInitialised == true); return Shader_;}
        Scene2D*         getScene() {assert(IsInitialised == true); return Scene_;}
        
        ShapesType* getShapesLandscape() {assert(IsInitialised == true); return &ShapesLandscape_;}
        ShapesType* getShapesProjectile() {assert(IsInitialised == true); return &ShapesProjectile_;}
        ShapesType* getShapesSubmarine() {assert(IsInitialised == true); return &ShapesSubmarine_;}
        
        MeshesType* getMeshesLandscape() {assert(IsInitialised == true); return &MeshesLandscape_;}
        MeshesType* getMeshesProjectile() {assert(IsInitialised == true); return &MeshesProjectile_;}
        MeshesType* getMeshesSubmarine() {assert(IsInitialised == true); return &MeshesSub_;}

    private:

        bool IsInitialised = false;
        
        Scene2D* Scene_ = new Scene2D;
        SceneGraph::DrawableGroup2D* Drawables_ = new SceneGraph::DrawableGroup2D;
        Shaders::Flat2D* Shader_ = nullptr;
        ShapesType  ShapesLandscape_;
        ShapesType  ShapesProjectile_;
        ShapesType  ShapesSubmarine_;
        MeshesType  MeshesLandscape_;
        MeshesType  MeshesProjectile_;
        MeshesType  MeshesSub_;
        
};

#endif // RESOURCE_STORAGE_H
