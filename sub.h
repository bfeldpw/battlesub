#ifndef SUB_H
#define SUB_H

#include "game_object.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

class Sub : public GameObject
{
    public:
        
        void init(const b2BodyDef BodyDef)
        {
            std::cout << "Sub::init" << std::endl;
            
            GameObject::init(BodyDef);
            
            // Submarine size: 4m x 16m
            constexpr float SizeX = 2.0f;
            constexpr float SizeY = 8.0f;
            
            b2PolygonShape shape;
            shape.SetAsBox(SizeX, SizeY);
            
            b2FixtureDef fixture;
            fixture.friction = 0.8f;
            fixture.density = 1.0f;
            fixture.shape = &shape;
            Body_->CreateFixture(&fixture);
            
            Visuals_->setScaling({SizeX, SizeY});
        }
        
        void fire(GL::Mesh& Mesh, Shaders::Flat2D& Shader, SceneGraph::DrawableGroup2D& Drawables,
                  float GunPos);
        
        void rudderLeft()
        {
            Rudder_ = -10.0f;
        }
        void rudderRight()
        {
            Rudder_ = 10.0f;
        }
        void throttleForward()
        {
            Throttle_ = 20.0f;
        }
        void throttleReverse()
        {
            Throttle_ = -20.0f;
        }
        
        void update(SceneGraph::DrawableGroup2D& Drawables)
        {
            this->Body_->ApplyForce(this->Body_->GetWorldVector({Rudder_, 0.0f}),
                                    this->Body_->GetWorldPoint({0.0f, 1.0f}), true);
            this->Body_->ApplyForce(this->Body_->GetWorldVector({0.0f, Throttle_}),
                                    this->Body_->GetWorldPoint({0.0f, 0.0f}), true);
//             for (auto i=0; i<Projectiles_.size(); ++i)
//             {
//                 auto ColorRed = Projectiles_[i]->getBody()->GetLinearVelocity().Length();
//                 ProjectileDrawables_[i]->setColor(Color3(ColorRed, 0.0, 0.0));
//                 
//                 if (ColorRed == 0.0)
//                 {
// //                     Drawables.remove(*ProjectileDrawables_[i]);
// //                     World_->destroy(Projectiles_->getBody());
//                 }
//             }
        }
        
        
    private:
        
        float Rudder_   = 0.0f;
        float Throttle_ = 0.0f;
        
};

#endif // SUB_H
