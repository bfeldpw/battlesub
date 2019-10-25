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
        bool            isSunk() const {return IsSunk_;}

        void init(b2World* World, Scene2D* Scene, const b2BodyDef& BodyDef)
        {
            World_ = World;
            Scene_ = Scene;
            Visuals_ = new Object2D(Scene);
            
            Body_=World_->CreateBody(&BodyDef);
            Body_->SetUserData(Visuals_);
            
            Scale_  = 1.0f;
            IsSunk_ = false;
        }
        
        void sink()
        {
            // Just change the visuals
            Scale_ *= SINKING_SCALE_FACTOR;
            if (Scale_ <= SINKING_SCALE_MIN) IsSunk_ = true;
            Visuals_->setScaling({Scale_, Scale_});
            
            // Physics: Filter collisions to not collide with sinking objects
            for (auto Fixture = Body_->GetFixtureList(); Fixture; Fixture = Fixture->GetNext())
            {
                b2Filter Filter;
                Filter.maskBits = 0;
                Fixture->SetFilterData(Filter);
            }
        }
        
    protected:
        
        // General data
        Timer Age_;
        
        // Physics data
        b2Body*  Body_      = nullptr;
        b2World* World_     = nullptr;
        
        // Graphics data
        GL::Mesh  Mesh_{NoCreate};
        Object2D* Visuals_  = nullptr;
        Scene2D*  Scene_    = nullptr;
        float     Scale_    = 1.0f;
        bool      IsSunk_   = false;
     
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
