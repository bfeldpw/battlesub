#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "game_object.h"

class Projectile : public GameObject
{
    public:
        
        void init(const b2BodyDef BodyDef)
        {
            GameObject::init(BodyDef);
            
            Body_->SetBullet(true);
            
            std::vector<b2Vec2> Verts;
            Verts.push_back({-0.02f, 0.0f});
            Verts.push_back({ 0.02f, 0.0f});
            Verts.push_back({ 0.02f, 0.1f});
            Verts.push_back({ 0.0f,  0.15f});
            Verts.push_back({-0.02f, 0.1f});
            Verts.push_back({-0.02f, 0.0f});
            
            b2PolygonShape Shape;
            Shape.Set(Verts.data(), Verts.size());
            
            b2FixtureDef fixture;
            fixture.friction = 0.8f;
            fixture.shape = &Shape;
            Body_->CreateFixture(&fixture);
            
            GL::Mesh Mesh;
            GL::Buffer Buffer;
            Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(Verts), GL::BufferUsage::StaticDraw);
            Mesh.setCount(Verts.size())
                .setPrimitive(GL::MeshPrimitive::LineLoop)
                .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
            Mesh_ = std::move(Mesh);
        }
        
    private:
        
};

#endif // PROJECTILE_H
