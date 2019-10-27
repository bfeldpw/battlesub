#include "resource_storage.h"

ResourceStorage ResourceStorage::Global;

#include <random>

void ResourceStorage::init()
{
    // Initialise landscape
    {
        std::mt19937 Generator;
        std::poisson_distribution<int> Distribution(50);
        
        Generator.seed(219);
        for (auto i=-500.0f; i<=500.0f; i+=10.0f)
        {
            auto Base = float(Distribution(Generator));
            for (auto j=0.0f; j<10.0f; j+=1.0f)
            {
                GeoLandscape_.push_back({i, 300.0f-Base+0.2*(float(Distribution(Generator))-20)});
            }
        }
//         GeoLandscape_.push_back({-500.0f,  300.0f});
        GeoLandscape_.push_back({ 500.0f,  300.0f});
        GeoLandscape_.push_back({ 500.0f, -300.0f});
        GeoLandscape_.push_back({-500.0f, -300.0f});
        
        GL::Mesh Mesh;
        GL::Buffer Buffer;
        Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(GeoLandscape_), GL::BufferUsage::StaticDraw);
        Mesh.setCount(GeoLandscape_.size())
            .setPrimitive(GL::MeshPrimitive::LineLoop)
            .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
        MeshLandscape_ = std::move(Mesh);
    }
    // Initialise projectile
    {
        GeoProjectile_.push_back({-0.02f, 0.0f});
        GeoProjectile_.push_back({ 0.02f, 0.0f});
        GeoProjectile_.push_back({ 0.02f, 0.1f});
        GeoProjectile_.push_back({ 0.0f,  0.15f});
        GeoProjectile_.push_back({-0.02f, 0.1f});
        
        GL::Mesh Mesh;
        GL::Buffer Buffer;
        Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(GeoProjectile_), GL::BufferUsage::StaticDraw);
        Mesh.setCount(GeoProjectile_.size())
            .setPrimitive(GL::MeshPrimitive::TriangleFan)
            .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
        MeshProjectile_ = std::move(Mesh);
    }
    // Initialise submarine
    {
        GeoSubmarine_.push_back({-2.0f, -6.0f});
        GeoSubmarine_.push_back({ 2.0f, -6.0f});
        GeoSubmarine_.push_back({ 2.0f,  6.0f});
        GeoSubmarine_.push_back({ 1.5f,  8.0f});
        GeoSubmarine_.push_back({ 0.5f,  9.0f});
        GeoSubmarine_.push_back({-0.5f,  9.0f});
        GeoSubmarine_.push_back({-1.5f,  8.0f});
        GeoSubmarine_.push_back({-2.0f,  6.0f});
        
        GL::Mesh Mesh;
        GL::Buffer Buffer;
        Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(GeoSubmarine_), GL::BufferUsage::StaticDraw);
        Mesh.setCount(GeoSubmarine_.size())
            .setPrimitive(GL::MeshPrimitive::TriangleFan)
            .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
        MeshSub_ = std::move(Mesh);
    }
    
//     MeshSub_ = MeshTools::compile(Primitives::squareSolid());
    
    IsInitialised = true;
}
