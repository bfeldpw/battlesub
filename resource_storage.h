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
        
        GeoType*  getGeoLandscape() {assert(IsInitialised == true); return &GeoLandscape_;}
        GeoType*  getGeoProjectile() {assert(IsInitialised == true); return &GeoProjectile_;}
        GeoType*  getGeoSubmarine() {assert(IsInitialised == true); return &GeoSubmarine_;}
        
        GL::Mesh* getMeshLandscape() {assert(IsInitialised == true); return &MeshLandscape_;}
        GL::Mesh* getMeshProjectile() {assert(IsInitialised == true); return &MeshProjectile_;}
        GL::Mesh* getMeshSubmarine() {assert(IsInitialised == true); return &MeshSub_;}

    private:

        bool IsInitialised = false;
        GeoType  GeoLandscape_;
        GeoType  GeoProjectile_;
        GeoType  GeoSubmarine_;
        GL::Mesh MeshLandscape_{NoCreate};
        GL::Mesh MeshProjectile_{NoCreate};
        GL::Mesh MeshSub_{NoCreate};
        
    public:
        
        static ResourceStorage Global;
        
};

#endif // RESOURCE_STORAGE_H
