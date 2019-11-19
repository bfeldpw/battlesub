#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <iostream>
#include <vector>

#include <Box2D/Box2D.h>

#include "common.h"
#include "drawable_generic.h"
#include "entity.h"
#include "timer.h"

constexpr float SINKING_SCALE_FACTOR = 0.995f;
constexpr float SINKING_SCALE_FACTOR_COLOR = 0.998f;
constexpr float SINKING_SCALE_MIN    = 0.01f;

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
typedef std::vector<GL::Mesh>   MeshesType;

enum class GameObjectTypeE : int
{
    DEBRIS,
    LANDSCAPE,
    PROJECTILE,
    SUBMARINE_HULL,
    SUBMARINE_RUDDER,
    DEFAULT
};

class GameObject : public Entity
{
    public:
        
        GameObjectTypeE getType() const {return Type_;}
        
        b2Body*     getBody() const {assert(Body_ != nullptr); return Body_;}
        b2World*    getWorld() {assert(World_ != nullptr); return World_;}
        ShapesType* getShapes() {assert(Shapes_ != nullptr); return Shapes_;}
        
        MeshesType* getMeshes() {assert(Meshes_ != nullptr); return Meshes_;}
        Object2D*   getVisuals() {return Visuals_;}
        
        void        init(const GameObjectTypeE Type, const b2BodyDef& BodyDef);
        
        bool        isSunk() const {return IsSunk_;}

        void destroy();
                
        GameObject& setColor(Color4 Color) {Color_ = Color; return *this;}
        GameObject& setDrawableGroup(SceneGraph::DrawableGroup2D* const DrawableGrp) {DrawableGrp_ = DrawableGrp; return *this;}
        GameObject& setMeshes(MeshesType* const Meshes) {Meshes_ = Meshes; return *this;}
        GameObject& setScale(float X, float Y);
        GameObject& setScene(Scene2D* const Scene) {Scene_ = Scene; return *this;}
        GameObject& setShapes(ShapesType* const Shapes) {Shapes_ = Shapes; return *this;}
        GameObject& setShader(Shaders::Flat2D* const Shader) {Shader_ = Shader; return *this;}
        GameObject& setWorld(b2World* const World) {World_ = World; return *this;}
        
        void update()
        {
            assert(Body_ != nullptr);
            
            // Body starts sinking if too slow
            if (Body_->GetLinearVelocity().Length() < 0.01f)
            {
                this->sink();
            }
        }
        
    protected:
        
        void sink();
        
        // General data
        GameObjectTypeE Type_ = GameObjectTypeE::DEFAULT;
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
        Color4                          Color_{0.1f, 0.1f, 0.2f, 1.0f};
        DrawableGeneric*                Drawable        = nullptr;
        SceneGraph::DrawableGroup2D*    DrawableGrp_    = nullptr;
        float                           ScaleX_         = 1.0f;
        float                           ScaleY_         = 1.0f;
        float                           ScaleSinkColor  = 1.0f;
     
    public:
        
        static std::vector<Vector2> convertGeometryPhysicsToGraphics(const std::vector<b2Vec2> Verts);
        
};

#endif // GAME_OBJECT_H
