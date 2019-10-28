#include "resource_storage.h"

ResourceStorage ResourceStorage::Global;

#include "noise.h"

void ResourceStorage::init()
{
    // Initialise landscape
    {
        noise::module::Perlin Boundary;
        
        Boundary.SetFrequency(0.005);
        Boundary.SetPersistence(0.57317);
        Boundary.SetLacunarity(1.9731);
        Boundary.SetNoiseQuality(noise::QUALITY_BEST);
        Boundary.SetNoiseType(noise::TYPE_VALUE);
        
        int OctaveCount = ceil(log2(1/0.005)/log2(1.9731));
        if (OctaveCount < 1) OctaveCount = 1;
        GlobalMessageHandler.reportDebug("Boundary octave count: " + std::to_string(OctaveCount), MessageHandler::DEBUG_L1);
        
        Boundary.SetOctaveCount(OctaveCount);
        Boundary.SetSeed(7);
        
        ShapeType Shape;
        
        for (auto i=-500.0f; i<=500.0f; i+=1.0f)
        {
            Shape.push_back({i, 300.0f - 50.0f * float(Boundary.GetValue(i, 300.0))});
        }
        for (auto i=Shape.back().y; i>=-300.0f; i-=1.0f)
        {
            Shape.push_back({500.0f - 50.0f * float(Boundary.GetValue(500.0, i)), i});
        }
        for (auto i=Shape.back().x; i>=-500.0f; i-=1.0f)
        {
            Shape.push_back({i, -300.0f - 50.0f * float(Boundary.GetValue(i, -300.0))});
        }
        for (auto i=Shape.back().y; i<=300.0f; i+=1.0f)
        {
            Shape.push_back({-500.0f - 50.0f * float(Boundary.GetValue(-500.0f, i)), i});
        }
        
        ShapesLandscape_.push_back(std::move(Shape));
//         for (auto i=0.0f; i<2.0*b2_pi; i+=2.0*b2_pi/1000.0)
//         {
//             auto Value = 50.0f * Boundary.GetValue(500.0f * std::cos(i), 500.0f * std::sin(i));
//             
//             Shape.push_back({(500.0f-Value)*std::cos(i), (500.0f-Value)*std::sin(i)});
//         }

//         Shape.push_back({ 500.0f, -300.0f});
//         Shape.push_back({-500.0f, -300.0f});
        
        GL::Mesh Mesh;
        GL::Buffer Buffer;
        Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(ShapesLandscape_.front()), GL::BufferUsage::StaticDraw);
        Mesh.setCount(ShapesLandscape_.front().size())
            .setPrimitive(GL::MeshPrimitive::LineLoop)
            .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
        MeshLandscape_ = std::move(Mesh);
    }
    // Initialise projectile
    {
        ShapeType Shape;
        
        Shape.push_back({-0.02f, 0.0f});
        Shape.push_back({ 0.02f, 0.0f});
        Shape.push_back({ 0.02f, 0.1f});
        Shape.push_back({ 0.0f,  0.15f});
        Shape.push_back({-0.02f, 0.1f});
        
        ShapesProjectile_.push_back(std::move(Shape));
        
        GL::Mesh Mesh;
        GL::Buffer Buffer;
        Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(ShapesProjectile_.front()), GL::BufferUsage::StaticDraw);
        Mesh.setCount(ShapesProjectile_.front().size())
            .setPrimitive(GL::MeshPrimitive::TriangleFan)
            .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
        MeshProjectile_ = std::move(Mesh);
    }
    // Initialise submarine
    {
        ShapeType Shape;
        
        Shape.push_back({-2.0f, -6.0f});
        Shape.push_back({ 2.0f, -6.0f});
        Shape.push_back({ 2.0f,  6.0f});
        Shape.push_back({ 1.5f,  8.0f});
        Shape.push_back({ 0.5f,  9.0f});
        Shape.push_back({-0.5f,  9.0f});
        Shape.push_back({-1.5f,  8.0f});
        Shape.push_back({-2.0f,  6.0f});
        
        ShapesSubmarine_.push_back(std::move(Shape));
        
        GL::Mesh Mesh;
        GL::Buffer Buffer;
        Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(ShapesSubmarine_.front()), GL::BufferUsage::StaticDraw);
        Mesh.setCount(ShapesSubmarine_.front().size())
            .setPrimitive(GL::MeshPrimitive::TriangleFan)
            .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
        MeshSub_ = std::move(Mesh);
    }
    
    IsInitialised = true;
}
