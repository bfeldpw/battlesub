#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "game_object.h"
#include "projectile_drawable.h"

class Projectile : public GameObject
{
    public:
        
        Projectile() {}
        
        void init(const b2BodyDef BodyDef)
        {
            std::cout << "Projectile::init" << std::endl;
            GameObject::init(BodyDef);
            
            Body_->SetBullet(true);
            
            // Projectile size: 10cm x 30cm
            constexpr float SizeX = 0.05f;
            constexpr float SizeY = 0.15f;
            
            b2PolygonShape shape;
            shape.SetAsBox(SizeX, SizeY);
            
            b2FixtureDef fixture;
            fixture.friction = 0.8f;
            fixture.density = 20.0f;
            fixture.shape = &shape;
            Body_->CreateFixture(&fixture);
            
            Visuals_->setScaling({SizeX, SizeY});
            
            // This defines a triangle in CCW order.
//             b2Vec2 Vertices[5];
//             Vertices[0].Set(-0.1f,  0.0f);
//             Vertices[1].Set( 0.1f,  0.0f);
//             Vertices[2].Set( 0.1f,  0.8f);
//             Vertices[3].Set( 0.0f,  1.0f);
//             Vertices[4].Set(-0.1f,  0.8f);
//             int32 Count = 5;
//             b2PolygonShape Polygon;
//             Polygon.Set(Vertices, Count);
//             
//             b2FixtureDef fixture;
//             fixture.friction = 0.8f;
//             fixture.density = 1.0f;
//             fixture.shape = &Polygon;
//             Body_->CreateFixture(&fixture);
            
//             b2CircleShape circle;
//             circle.m_p.Set(.0f, .0f);
//             circle.m_radius = .1f;
//             
//             b2FixtureDef fixture;
//             fixture.friction = 0.8f;
//             fixture.density = 1.0f;
//             fixture.shape = &circle;
//             Body_->CreateFixture(&fixture);
        }
                
    private:
        
};

#endif // PROJECTILE_H
