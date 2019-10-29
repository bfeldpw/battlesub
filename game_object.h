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

typedef std::vector<b2Vec2>     ShapeType;
typedef std::vector<ShapeType>  ShapesType;
typedef std::vector<GL::Mesh>   MeshesType;

class GameObject : public Entity
{
    public:
        
        b2Body*     getBody() const {return Body_;}
        Object2D&   getVisuals() {return *Visuals_;}
        bool        isSunk() const {return IsSunk_;}

        void destroy();
        void init(b2World* World, Scene2D* Scene,
                  const b2BodyDef& BodyDef, SceneGraph::DrawableGroup2D* const DGrp);
        void sink();
        
        void setShapes(ShapesType* const Shapes) {Shapes_ = Shapes;}
        void setMeshes(MeshesType* const Meshes) {Meshes_ = Meshes;}
        void setShader(Shaders::Flat2D* const Shader) {Shader_ = Shader;}
        
    protected:
        
        // General data
        Timer Age_;
        
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
        bool                            IsSunk_         = false;
     
    public:
        
        static std::vector<Vector2> convertGeometryPhysicsToGraphics(const std::vector<b2Vec2> Verts);
        
};

#endif // GAME_OBJECT_H
