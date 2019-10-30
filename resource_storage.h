#ifndef RESOURCE_STORAGE_H
#define RESOURCE_STORAGE_H

#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/Trade/MeshData2D.h>

#include "game_object.h"

class ResourceStorage
{
    public:
        
        void init();
        void release();
        
        ShapesType* getShapesLandscape() {assert(IsInitialised == true); return &ShapesLandscape_;}
        ShapesType* getShapesProjectile() {assert(IsInitialised == true); return &ShapesProjectile_;}
        ShapesType* getShapesSubmarine() {assert(IsInitialised == true); return &ShapesSubmarine_;}
        
        MeshesType* getMeshesLandscape() {assert(IsInitialised == true); return &MeshesLandscape_;}
        MeshesType* getMeshesProjectile() {assert(IsInitialised == true); return &MeshesProjectile_;}
        MeshesType* getMeshesSubmarine() {assert(IsInitialised == true); return &MeshesSub_;}

    private:

        bool IsInitialised = false;
        ShapesType  ShapesLandscape_;
        ShapesType  ShapesProjectile_;
        ShapesType  ShapesSubmarine_;
        MeshesType  MeshesLandscape_;
        MeshesType  MeshesProjectile_;
        MeshesType  MeshesSub_;
        
};

#endif // RESOURCE_STORAGE_H
