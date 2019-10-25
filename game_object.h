#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <iostream>

#include <Box2D/Box2D.h>

#include "common.h"
#include "entity.h"
#include "timer.h"

constexpr float SINKING_SCALE_FACTOR = 0.995f;
constexpr float SINKING_SCALE_MIN    = 0.01f;

class GameObject : public Entity
{
    public:
        
        b2Body*     getBody() const {return Body_;}
        GL::Mesh*   getMesh() {return Mesh_;}
        Object2D&   getVisuals() {return *Visuals_;}
        bool        isSunk() const {return IsSunk_;}

        void destroy();
        void init(b2World* World, Scene2D* Scene, const b2BodyDef& BodyDef);
        void sink();
        
        void setMesh(GL::Mesh* const Mesh) {Mesh_ = Mesh;}
        
    protected:
        
        // General data
        Timer Age_;
        
        // Physics data
        b2Body*  Body_      = nullptr;
        b2World* World_     = nullptr;
        
        // Graphics data
        GL::Mesh* Mesh_     = nullptr;
        Object2D* Visuals_  = nullptr;
        Scene2D*  Scene_    = nullptr;
        float     Scale_    = 1.0f;
        bool      IsSunk_   = false;
     
    public:
        
        static std::vector<Vector2> convertGeometryPhysicsToGraphics(const std::vector<b2Vec2> Verts);
        
};

#endif // GAME_OBJECT_H
