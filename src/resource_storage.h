#ifndef RESOURCE_STORAGE_H
#define RESOURCE_STORAGE_H

#include <array>

#include <box2d.h>

#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/SceneGraph/Drawable.h>

#include "common.h"
#include "physics_component.hpp"
#include "status_component.hpp"
#include "visuals_component.hpp"
#include "world_def.h"

enum class DrawableGroupsTypeE : int
{
    WEAPON,
    DEFAULT
};

enum class ShapeEnumType : int
{
    CIRCLE,
    CHAIN,
    POLYGON
};

typedef std::vector<b2Vec2>     ShapeType;
typedef struct
{
    std::vector<ShapeType>    ShapeDefs;
    std::vector<b2FixtureDef> FixtureDefs;
    ShapeEnumType             Type = ShapeEnumType::POLYGON;

} ShapesType;

class ResourceStorage
{
    public:
        
        void init();
        void release();
        
        b2World* getWorld() {return &World_;}
        
        SceneGraph::DrawableGroup2D* getDrawables(DrawableGroupsTypeE t) {assert(IsInitialised == true); return Drawables_[int(t)];};
        Shaders::Flat2D* getShader() {assert(IsInitialised == true); return Shader_;}
        Scene2D*         getScene() {assert(IsInitialised == true); return Scene_;}
        
        ShapesType* getShapes(GameObjectTypeE t) {assert(IsInitialised == true); return &(Shapes_[int(t)]);}
        MeshesType* getMeshes(GameObjectTypeE t) {assert(IsInitialised == true); return &(Meshes_[int(t)]);}
        
        std::vector<float>* getHeightMap() {return &HeightMap_;}

    private:

        void initDebris();
        void initHeightMap();
        void initLandscape();
        void initProjectile();
        void initSubmarine();
        
        bool IsInitialised = false;
        
        b2World World_{{0.0f, 0.0f}};
        
        Scene2D* Scene_ = new Scene2D;
        Shaders::Flat2D* Shader_ = nullptr;

        std::array<SceneGraph::DrawableGroup2D*, int(DrawableGroupsTypeE::DEFAULT)+1> Drawables_{new SceneGraph::DrawableGroup2D, new SceneGraph::DrawableGroup2D};
        std::array<MeshesType, int(GameObjectTypeE::DEFAULT)+1> Meshes_;
        std::array<ShapesType, int(GameObjectTypeE::DEFAULT)+1> Shapes_;
        
        std::vector<float> HeightMap_;
        std::vector<float>* HeightMapPlateausFront_;
        std::vector<float>* HeightMapPlateausBack_;
        std::vector<float> HeightMapPlateaus0_;
        std::vector<float> HeightMapPlateaus1_;

        std::vector<Vector2> convertGeometryPhysicsToGraphics(const std::vector<b2Vec2>& Verts);
};

#endif // RESOURCE_STORAGE_H
