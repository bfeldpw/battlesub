#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include <vector>

#include "game_object.h"
#include "landscape_drawable.h"

class Landscape : public GameObject
{
    public:
        
        void init(const b2BodyDef BodyDef)
        {
            GameObject::init(BodyDef);
            
            // Create physical shape and convert to graphics
            std::vector<b2Vec2> Verts;
            Verts.push_back({-500.0f,  300.0f});
            Verts.push_back({ 500.0f,  300.0f});
            Verts.push_back({ 500.0f, -300.0f});
            Verts.push_back({-500.0f, -300.0f});
            
            b2ChainShape Chain;
            Chain.CreateChain(Verts.data(), Verts.size());
            
            b2FixtureDef fixture;
            fixture.friction = 0.8f;
            fixture.shape = &Chain;
            Body_->CreateFixture(&fixture);
            
            GL::Mesh Mesh;
            GL::Buffer Buffer;
            Buffer.setData(convertGeometryPhysicsToGraphics(Verts), GL::BufferUsage::StaticDraw);
            Mesh.setCount(Verts.size())
                .setPrimitive(GL::MeshPrimitive::LineLoop)
                .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
            Mesh_ = std::move(Mesh);
        }
        
};

#endif // LANDSCAPE_H
