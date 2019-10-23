#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <iostream>

#include <Box2D/Box2D.h>

#include "common.h"
#include "entity.h"

class GameObject : public Entity
{
    public:
        
        void create(b2World* World, Scene2D* Scene)
        {
            World_ = World;
            Scene_ = Scene;
            
            Visuals_ = new Object2D(Scene);
        }
        
        void destroy()
        {
            // Destroy physics data, Box2D will handle everything from here
            World_->DestroyBody(Body_);
            
            // Destroy graphics data, Magnum will handle everything from here
            // (including drawables)
            if (Visuals_ != nullptr)
            {
                delete Visuals_;
                Visuals_ = nullptr;
            }
        }
        
        b2Body*         getBody() const {return Body_;}
        GL::Mesh&       getMesh() {return Mesh_;}
        Object2D&       getVisuals() {return *Visuals_;}

        void init(const b2BodyDef& BodyDef)
        {
            Body_=World_->CreateBody(&BodyDef);
            Body_->SetUserData(Visuals_);
        }
        
    protected:
        
        b2Body*  Body_      = nullptr;
        b2World* World_     = nullptr;
        
        GL::Mesh Mesh_{NoCreate};
        Object2D* Visuals_  = nullptr;
        Scene2D* Scene_     = nullptr;
     
    public:
        
        static std::vector<Vector2> convertGeometryPhysicsToGraphics(const std::vector<b2Vec2> Verts)
        {
            std::vector<Vector2> Tmp;
            
            for (auto Vec : Verts)
            {
                Tmp.push_back({Vec.x, Vec.y});
            }
            return Tmp; // Note: No std::move needed, might even prevent copy elision
        }
        
};

#endif // GAME_OBJECT_H
