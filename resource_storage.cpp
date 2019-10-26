#include "resource_storage.h"

ResourceStorage ResourceStorage::Global;

void ResourceStorage::init()
{
    {
        std::vector<b2Vec2> Verts;
        Verts.push_back({-500.0f,  300.0f});
        Verts.push_back({ 500.0f,  300.0f});
        Verts.push_back({ 500.0f, -300.0f});
        Verts.push_back({-500.0f, -300.0f});
        
        GL::Mesh Mesh;
        GL::Buffer Buffer;
        Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(Verts), GL::BufferUsage::StaticDraw);
        Mesh.setCount(Verts.size())
            .setPrimitive(GL::MeshPrimitive::LineLoop)
            .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
        MeshLandscape_ = std::move(Mesh);
    }
    {
        std::vector<b2Vec2> Verts;
        Verts.push_back({-0.02f, 0.0f});
        Verts.push_back({ 0.02f, 0.0f});
        Verts.push_back({ 0.02f, 0.1f});
        Verts.push_back({ 0.0f,  0.15f});
        Verts.push_back({-0.02f, 0.1f});
        
        GL::Mesh Mesh;
        GL::Buffer Buffer;
        Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(Verts), GL::BufferUsage::StaticDraw);
        Mesh.setCount(Verts.size())
            .setPrimitive(GL::MeshPrimitive::TriangleFan)
            .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
        MeshProjectile_ = std::move(Mesh);
    }
    
    MeshSub_ = MeshTools::compile(Primitives::squareSolid());
    
    IsInitialised = true;
}
