#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "projectile_drawable.h"

class Projectile : public PhysObj
{
    public:
        
        Projectile(b2World& World) : PhysObj(World)
        {
            // This defines a triangle in CCW order.
            b2Vec2 Vertices[5];
            Vertices[0].Set(-0.01f, -0.04f);
            Vertices[1].Set( 0.01f, -0.04f);
            Vertices[2].Set( 0.01f,  0.08f);
            Vertices[3].Set( 0.0f,   0.1f);
            Vertices[4].Set(-0.01f,  0.08f);
            int32 Count = 5;
            b2PolygonShape Polygon;
            Polygon.Set(Vertices, Count);
        }
        
        void init(const b2BodyDef BodyDef)
        {
            std::cout << "Projectile::init" << std::endl;
            PhysObj::init(BodyDef);
            Body_->SetBullet(true);
        }
                
    private:
        
};

#endif // PROJECTILE_H
