#include "resource_storage.h"

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
        
        Boundary.SetOctaveCount(OctaveCount);
        Boundary.SetSeed(7);
        
        {
            ShapeType Shape;
            constexpr float w = 500.0f; // Half width
            constexpr float h = 350.0f; // Half height
            
            Shape.push_back({-w, h});
            for (auto i=-w; i<=w; i+=1.0f)
            {
                Shape.push_back({i, h-50.0f - 50.0f * float(Boundary.GetValue(i, h-50.0f))});
            }
            Shape.push_back({w, h});
            
//             for (auto i=Shape.back().y; i>=-300.0f; i-=1.0f)
//             for (auto i=w; i>=-300.0f; i-=1.0f)
//             {
//                 Shape.push_back({500.0f - 50.0f * float(Boundary.GetValue(500.0, i)), i});
//             }
//             for (auto i=Shape.back().x; i>=-500.0f; i-=1.0f)
//             {
//                 Shape.push_back({i, -300.0f - 50.0f * float(Boundary.GetValue(i, -300.0))});
//             }
//             for (auto i=Shape.back().y; i<=300.0f; i+=1.0f)
//             {
//                 Shape.push_back({-500.0f - 50.0f * float(Boundary.GetValue(-500.0f, i)), i});
//             }
            
            Shapes_[int(GameObjectType::LANDSCAPE)].push_back(std::move(Shape));
            
            GL::Mesh Mesh;
            GL::Buffer Buffer;
            Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(Shapes_[int(GameObjectType::LANDSCAPE)].front()), GL::BufferUsage::StaticDraw);
            Mesh.setCount(Shapes_[int(GameObjectType::LANDSCAPE)].front().size())
                .setPrimitive(GL::MeshPrimitive::LineLoop)
                .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
            Meshes_[int(GameObjectType::LANDSCAPE)].push_back(std::move(Mesh));
            
            DBLK(
                GlobalMessageHandler.reportDebug("Boundary octave count: " + std::to_string(OctaveCount), MessageHandler::DEBUG_L1);
                GlobalMessageHandler.reportDebug("Boundary vertex count: " + std::to_string(Shapes_[int(GameObjectType::LANDSCAPE)].front().size()), MessageHandler::DEBUG_L1);
            )
        }
        {
            ShapeType Shape;
            for (auto i=0.0f; i<2.0*b2_pi; i+=2.0*b2_pi/100.0)
            {
                auto Value = 5.0f * Boundary.GetValue(100.0f * std::cos(i), 100.0f * std::sin(i));
                
                Shape.push_back({(5.0f-Value)*std::cos(i), (5.0f-Value)*std::sin(i)+200.0f});
            }
            Shapes_[int(GameObjectType::LANDSCAPE)].push_back(std::move(Shape));
            
            GL::Mesh Mesh;
            GL::Buffer Buffer;
            Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(Shapes_[int(GameObjectType::LANDSCAPE)].back()), GL::BufferUsage::StaticDraw);
            Mesh.setCount(Shapes_[int(GameObjectType::LANDSCAPE)].back().size())
                .setPrimitive(GL::MeshPrimitive::TriangleFan)
                .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
            Meshes_[int(GameObjectType::LANDSCAPE)].push_back(std::move(Mesh));
        }
    }
    // Initialise projectile
    {
        ShapeType Shape;
        
        Shape.push_back({-0.02f, 0.0f});
        Shape.push_back({ 0.02f, 0.0f});
        Shape.push_back({ 0.02f, 0.1f});
        Shape.push_back({ 0.0f,  0.15f});
        Shape.push_back({-0.02f, 0.1f});
        
        Shapes_[int(GameObjectType::PROJECTILE)].push_back(std::move(Shape));
        
        GL::Mesh Mesh;
        GL::Buffer Buffer;
        Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(Shapes_[int(GameObjectType::PROJECTILE)].front()), GL::BufferUsage::StaticDraw);
        Mesh.setCount(Shapes_[int(GameObjectType::PROJECTILE)].front().size())
            .setPrimitive(GL::MeshPrimitive::TriangleFan)
            .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
        Meshes_[int(GameObjectType::PROJECTILE)].push_back(std::move(Mesh));
    }
    // Initialise submarine
    {
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
            
            Shapes_[int(GameObjectType::SUBMARINE_HULL)].push_back(std::move(Shape));
            
            GL::Mesh Mesh;
            GL::Buffer Buffer;
            Buffer.setData(
                            GameObject::convertGeometryPhysicsToGraphics(
                            Shapes_[int(GameObjectType::SUBMARINE_HULL)].front()),
                            GL::BufferUsage::StaticDraw
                          );
            Mesh.setCount(Shapes_[int(GameObjectType::SUBMARINE_HULL)].front().size())
                .setPrimitive(GL::MeshPrimitive::TriangleFan)
                .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
            Meshes_[int(GameObjectType::SUBMARINE_HULL)].push_back(std::move(Mesh));
        }
//         {
//             ShapeType Shape;
//             
//             Shape.push_back({-2.0f, -6.0f});
//             Shape.push_back({ 2.0f, -6.0f});
//             Shape.push_back({ 2.0f,  6.0f});
//             Shape.push_back({ 1.5f,  8.0f});
//             Shape.push_back({ 0.5f,  9.0f});
//             Shape.push_back({-0.5f,  9.0f});
//             Shape.push_back({-1.5f,  8.0f});
//             Shape.push_back({-2.0f,  6.0f});
//             
//             ShapesSubmarineRudder_.push_back(std::move(Shape));
//             
//             GL::Mesh Mesh;
//             GL::Buffer Buffer;
//             Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(ShapesSubmarineRudder_.front()), GL::BufferUsage::StaticDraw);
//             Mesh.setCount(ShapesSubmarineRudder_.front().size())
//                 .setPrimitive(GL::MeshPrimitive::TriangleFan)
//                 .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
//             MeshesSub_.push_back(std::move(Mesh));
//         }
    }
    
    Shader_ = new Shaders::Flat2D{};
    
    IsInitialised = true;
}

void ResourceStorage::release()
{
    for (auto i=0u; i<Meshes_.size(); ++i) Meshes_[i].clear();
    for (auto i=0u; i<Shapes_.size(); ++i) Shapes_[i].clear();

    if (Drawables_ != nullptr)
    {
        delete Drawables_;
        Drawables_ = nullptr;
    }
    if (Scene_ != nullptr)
    {
        delete Scene_;
        Scene_ = nullptr;
    }
    if (Shader_ != nullptr)
    {
        delete Shader_;
        Shader_ = nullptr;
    }
    
    IsInitialised = false;
}
