#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include "game_object.h"
// #include "landscape_drawable.h"

class Landscape : public GameObject
{
    public:
        
        void init(const b2BodyDef BodyDef)
        {
            GameObject::init(BodyDef);
            
            // Projectile size: 10cm x 30cm
            constexpr float SizeX = 0.05f;
            constexpr float SizeY = 0.15f;
            
            // This a chain shape with isolated vertices
            b2Vec2 vs[4];
            vs[0].Set(1.7f, 0.0f);
            vs[1].Set(1.0f, 0.25f);
            vs[2].Set(0.0f, 0.0f);
            vs[3].Set(-1.7f, 0.4f);
            b2ChainShape Chain;
            Chain.CreateChain(vs, 4);
            
            b2FixtureDef fixture;
            fixture.friction = 0.8f;
            fixture.shape = &Chain;
            Body_->CreateFixture(&fixture);
            
//             Visuals_->setScaling({SizeX, SizeY});
        }
                
    private:
        
};

#endif // LANDSCAPE_H
