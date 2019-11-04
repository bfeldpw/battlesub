#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <iostream>
#include <vector>

#include <Box2D/Box2D.h>

#include "common.h"
#include "entity.h"
#include "timer.h"

constexpr float SINKING_SCALE_FACTOR = 0.995f;
constexpr float SINKING_SCALE_MIN    = 0.01f;

enum class ShapeEnumType : int
{
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
typedef std::vector<GL::Mesh>   MeshesType;

class GameObject : public Entity
{
    public:
        
        b2Body*     getBody() const {assert(Body_ != nullptr); return Body_;}
        b2World*    getWorld() {assert(World_ != nullptr); return World_;}
        ShapesType* getShapes() {assert(Shapes_ != nullptr); return Shapes_;}
        
        MeshesType* getMeshes() {assert(Meshes_ != nullptr); return Meshes_;}
        Object2D*   getVisuals() {return Visuals_;}
        
        bool        isSunk() const {return IsSunk_;}

        void destroy();
        void sink();
        
        GameObject& setMeshes(MeshesType* const Meshes) {Meshes_ = Meshes; return *this;}
        GameObject& setShapes(ShapesType* const Shapes) {Shapes_ = Shapes; return *this;}
        GameObject& setShader(Shaders::Flat2D* const Shader) {Shader_ = Shader; return *this;}
        
    protected:
        
        void init(b2World* World,
                  Scene2D* Scene,
                  const b2BodyDef& BodyDef,
                  SceneGraph::DrawableGroup2D* const DGrp);
        
        // General data
        Timer Age_;
        bool  IsSunk_ = false;
        
        // Physics data
        b2Body*     Body_   = nullptr;
        b2World*    World_  = nullptr;
        ShapesType* Shapes_ = nullptr;
        
        // Graphics data
        MeshesType*                     Meshes_         = nullptr;
        Shaders::Flat2D*                Shader_         = nullptr;
        Object2D*                       Visuals_        = nullptr;
        Scene2D*                        Scene_          = nullptr;
        SceneGraph::DrawableGroup2D*    DrawableGrp_    = nullptr;
        float                           Scale_          = 1.0f;
     
    public:
        
        static std::vector<Vector2> convertGeometryPhysicsToGraphics(const std::vector<b2Vec2> Verts);
        
};

#endif // GAME_OBJECT_H
