#include "resource_storage.h"

#include "noise.h"

void ResourceStorage::init()
{
    this->initDebris();
    this->initLandscape();
    
    // Initialise projectile
    {
        ShapeType Shape;
        
        Shape.push_back({-0.02f, 0.0f});
        Shape.push_back({ 0.02f, 0.0f});
        Shape.push_back({ 0.02f, 0.05f});
        Shape.push_back({ 0.0f,  0.15f});
        Shape.push_back({-0.02f, 0.05f});
        
        auto& Shapes = Shapes_[int(GameObjectTypeE::PROJECTILE)];
        
        b2FixtureDef Fixture;
        Fixture.density = 800.0f; // Roughly the density of steel
        Fixture.friction = 0.5f;
        Fixture.restitution = 0.0f;
        Fixture.isSensor = false;
        
//         b2Filter Filter;
//         Filter.maskBits = 0;
//         Fixture.filter = Filter;
        
        Shapes.ShapeDefs.push_back(std::move(Shape));
        Shapes.FixtureDefs.push_back(std::move(Fixture));
                
        GL::Mesh Mesh;
        GL::Buffer Buffer;
        Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(Shapes.ShapeDefs.back()), GL::BufferUsage::StaticDraw);
        Mesh.setCount(Shapes.ShapeDefs.back().size())
            .setPrimitive(GL::MeshPrimitive::TriangleFan)
            .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
        Meshes_[int(GameObjectTypeE::PROJECTILE)].push_back(std::move(Mesh));
    }
    // Initialise submarine
    {
        {
            // Hull front
            ShapeType Shape;
            
            Shape.push_back({-1.8f, -5.0f});
            Shape.push_back({ 1.8f, -5.0f});
            Shape.push_back({ 2.0f,  6.0f});
            Shape.push_back({ 1.5f,  8.0f});
            Shape.push_back({ 0.5f,  9.0f});
            Shape.push_back({-0.5f,  9.0f});
            Shape.push_back({-1.5f,  8.0f});
            Shape.push_back({-2.0f,  6.0f});
            
            auto& Shapes = Shapes_[int(GameObjectTypeE::SUBMARINE_HULL)];
            
            b2FixtureDef Fixture;
            Fixture.density =  400.0f; // Half of the hull consist of steel (roughly 800kg/m³)
            Fixture.friction = 0.2f;
            Fixture.restitution = 0.3f;
            Fixture.isSensor = false;
            
            Shapes.ShapeDefs.push_back(std::move(Shape));
            Shapes.FixtureDefs.push_back(std::move(Fixture));
            
            GL::Mesh Mesh;
            GL::Buffer Buffer;
            Buffer.setData(
                            GameObject::convertGeometryPhysicsToGraphics(Shapes.ShapeDefs.back()),
                            GL::BufferUsage::StaticDraw
                          );
            Mesh.setCount(Shapes.ShapeDefs.back().size())
                .setPrimitive(GL::MeshPrimitive::TriangleFan)
                .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
            Meshes_[int(GameObjectTypeE::SUBMARINE_HULL)].push_back(std::move(Mesh));
        }
        {
            // Hull back
            ShapeType Shape;
            
            Shape.push_back({-1.5f, -7.0f});
            Shape.push_back({ 1.5f, -7.0f});
            Shape.push_back({ 1.8f, -5.0f});
            Shape.push_back({-1.8f, -5.0f});
            
            auto& Shapes = Shapes_[int(GameObjectTypeE::SUBMARINE_HULL)];
            
            b2FixtureDef Fixture;
            Fixture.density =  600.0f; // Half of the hull consist of steel (roughly 800kg/m³) + engine
            Fixture.friction = 0.2f;
            Fixture.restitution = 0.3f;
            Fixture.isSensor = false;
            
            Shapes.ShapeDefs.push_back(std::move(Shape));
            Shapes.FixtureDefs.push_back(std::move(Fixture));
            
            GL::Mesh Mesh;
            GL::Buffer Buffer;
            Buffer.setData(
                            GameObject::convertGeometryPhysicsToGraphics(Shapes.ShapeDefs.back()),
                            GL::BufferUsage::StaticDraw
                          );
            Mesh.setCount(Shapes.ShapeDefs.back().size())
                .setPrimitive(GL::MeshPrimitive::TriangleFan)
                .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
            Meshes_[int(GameObjectTypeE::SUBMARINE_HULL)].push_back(std::move(Mesh));
        }
        {
            // Rudder
            ShapeType Shape;
            
            Shape.push_back({-0.1f, -1.0f});
            Shape.push_back({ 0.1f, -1.0f});
            Shape.push_back({ 0.1f,  1.0f});
            Shape.push_back({-0.1f,  1.0f});
            
            auto& Shapes = Shapes_[int(GameObjectTypeE::SUBMARINE_RUDDER)];
            
            b2FixtureDef Fixture;
            Fixture.density =  800.0f;
            Fixture.friction = 0.2f;
            Fixture.restitution = 0.3f;
            Fixture.isSensor = false;
            
            Shapes.ShapeDefs.push_back(std::move(Shape));
            Shapes.FixtureDefs.push_back(std::move(Fixture));
            
            GL::Mesh Mesh;
            GL::Buffer Buffer;
            Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(Shapes.ShapeDefs.back()), GL::BufferUsage::StaticDraw);
            Mesh.setCount(Shapes.ShapeDefs.back().size())
                .setPrimitive(GL::MeshPrimitive::TriangleFan)
                .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
            Meshes_[int(GameObjectTypeE::SUBMARINE_RUDDER)].push_back(std::move(Mesh));
        }
    }
    
    Shader_ = new Shaders::Flat2D{};
    
    IsInitialised = true;
}

void ResourceStorage::release()
{
    for (auto i=0u; i<Meshes_.size(); ++i) Meshes_[i].clear();
    for (auto i=0u; i<Shapes_.size(); ++i) Shapes_[i].ShapeDefs.clear();
    for (auto i=0u; i<Shapes_.size(); ++i) Shapes_[i].FixtureDefs.clear();

    for (auto Drawable : Drawables_)
    {
        if (Drawable != nullptr)
        {
            delete Drawable;
            Drawable = nullptr;
        }
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

void ResourceStorage::initDebris()
{
    ShapeType Shape;
        
    for (auto i=0.0f; i<2.0f*3.14159f; i+=2.0f*3.14159f/5.0f)
    {
        Shape.push_back({0.05*std::cos(i), 0.05*std::sin(i)});
    }
    
    auto& Shapes = Shapes_[int(GameObjectTypeE::DEBRIS)];
    
    b2FixtureDef Fixture;
    Fixture.density =  1.0f;
    Fixture.friction = 0.9f;
    Fixture.restitution = 0.2f;
    Fixture.isSensor = false;
    
//         b2Filter Filter;
//         Filter.maskBits = 0;
//         Fixture.filter = Filter;
    
    Shapes.ShapeDefs.push_back(std::move(Shape));
    Shapes.FixtureDefs.push_back(std::move(Fixture));
            
    GL::Mesh Mesh;
    GL::Buffer Buffer;
    Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(Shapes.ShapeDefs.back()), GL::BufferUsage::StaticDraw);
    Mesh.setCount(Shapes.ShapeDefs.back().size())
        .setPrimitive(GL::MeshPrimitive::TriangleFan)
        .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
    Meshes_[int(GameObjectTypeE::DEBRIS)].push_back(std::move(Mesh));
}

void ResourceStorage::initLandscape()
{
    // Initialise landscape
    noise::module::Perlin Boundary;
    
    Boundary.SetFrequency(0.008);
    Boundary.SetPersistence(0.47317);
    Boundary.SetLacunarity(1.9731);
    Boundary.SetNoiseQuality(noise::QUALITY_BEST);
    Boundary.SetNoiseType(noise::TYPE_VALUE);
    
    int OctaveCount = ceil(log2(1/0.005)/log2(1.9731));
    if (OctaveCount < 1) OctaveCount = 1;
    
    Boundary.SetOctaveCount(OctaveCount);
    Boundary.SetSeed(7);
    
    {
        constexpr float w = 500.0f; // Half width
        constexpr float h = 300.0f; // Half height
        constexpr float b = 100.0f;  // Boundary
        constexpr float a = 100.0f;  // Amplitude
        
        // Top
        ShapeType ShapeTop;
        ShapeTop.push_back({-w-b, h+b});
        for (auto i=-w-b; i<=w+b; i+=1.0f)
        {
            ShapeTop.push_back({i, h - a * float(Boundary.GetValue(double(i), double(h)))});
        }
        ShapeTop.push_back({w+b, h+b});
        
        // Right
        ShapeType ShapeRight;
        ShapeRight.push_back({w+b, h+b});
        for (auto i=h+b; i>=-h-b; i-=1.0f)
        {
            ShapeRight.push_back({w - a * float(Boundary.GetValue(double(w), double(i))), i});
        }
        ShapeRight.push_back({w+b, -h-b});
        
        // Right
        ShapeType ShapeLeft;
        ShapeLeft.push_back({-w-b, h+b});
        for (auto i=h+b; i>=-h-b; i-=1.0f)
        {
            ShapeLeft.push_back({-w + a * float(Boundary.GetValue(double(-w), double(i))), i});
        }
        ShapeLeft.push_back({-w-b, -h-b});
        
        // Bottom
        ShapeType ShapeBottom;
        ShapeBottom.push_back({-w-b, -h-b});
        for (auto i=-w-b; i<=w+b; i+=1.0f)
        {
            ShapeBottom.push_back({i, -h + a * float(Boundary.GetValue(double(i), double(-h)))});
        }
        ShapeBottom.push_back({w+b, -h-b});
        
        auto& Shapes = Shapes_[int(GameObjectTypeE::LANDSCAPE)];
        
        b2FixtureDef Fixture;
        Fixture.density =  1.0f;
        Fixture.friction = 0.9f;
        Fixture.restitution = 0.2f;
        Fixture.isSensor = false;
        
        Shapes.ShapeDefs.push_back(std::move(ShapeTop));
        Shapes.FixtureDefs.push_back(Fixture);
        Shapes.Type = ShapeEnumType::CHAIN;
        {
            ShapeType TmpShape;
            for (auto i=1u; i<Shapes.ShapeDefs.back().size()-2; ++i)
            {
                TmpShape.push_back({float((i-1)-w-b), h+b});
                TmpShape.push_back({float(i-w-b), h+b});
                TmpShape.push_back(Shapes.ShapeDefs.back()[i]);
                TmpShape.push_back({float(i-w-b), h+b});
                TmpShape.push_back(Shapes.ShapeDefs.back()[i+1]);
                TmpShape.push_back(Shapes.ShapeDefs.back()[i]);
            }
            
            GL::Mesh Mesh;
            GL::Buffer Buffer;
            Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(TmpShape), GL::BufferUsage::StaticDraw);
            Mesh.setCount(TmpShape.size())
                .setPrimitive(GL::MeshPrimitive::Triangles)
                .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
            Meshes_[int(GameObjectTypeE::LANDSCAPE)].push_back(std::move(Mesh));
        }
        Shapes.ShapeDefs.push_back(ShapeRight);
        Shapes.FixtureDefs.push_back(std::move(Fixture));
        {
            ShapeType TmpShape;
            for (auto i=1u; i<Shapes.ShapeDefs.back().size()-2; ++i)
            {
                TmpShape.push_back({w+b, float(h+b-(i-1))});
                TmpShape.push_back({w+b, float(h+b-i)});
                TmpShape.push_back(Shapes.ShapeDefs.back()[i]);
                TmpShape.push_back({w+b, float(h+b-i)});
                TmpShape.push_back(Shapes.ShapeDefs.back()[i+1]);
                TmpShape.push_back(Shapes.ShapeDefs.back()[i]);
            }
            
            GL::Mesh Mesh;
            GL::Buffer Buffer;
            Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(TmpShape), GL::BufferUsage::StaticDraw);
            Mesh.setCount(TmpShape.size())
                .setPrimitive(GL::MeshPrimitive::Triangles)
                .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
            Meshes_[int(GameObjectTypeE::LANDSCAPE)].push_back(std::move(Mesh));
        }
        Shapes.ShapeDefs.push_back(std::move(ShapeLeft));
        Shapes.FixtureDefs.push_back(Fixture);
        {
            ShapeType TmpShape;
            for (auto i=1u; i<Shapes.ShapeDefs.back().size()-2; ++i)
            {
                TmpShape.push_back({-w-b, float(h+b-(i-1))});
                TmpShape.push_back({-w-b, float(h+b-i)});
                TmpShape.push_back(Shapes.ShapeDefs.back()[i]);
                TmpShape.push_back({-w-b, float(h+b-i)});
                TmpShape.push_back(Shapes.ShapeDefs.back()[i+1]);
                TmpShape.push_back(Shapes.ShapeDefs.back()[i]);
            }
            
            GL::Mesh Mesh;
            GL::Buffer Buffer;
            Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(TmpShape), GL::BufferUsage::StaticDraw);
            Mesh.setCount(TmpShape.size())
                .setPrimitive(GL::MeshPrimitive::Triangles)
                .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
            Meshes_[int(GameObjectTypeE::LANDSCAPE)].push_back(std::move(Mesh));
        }
        Shapes.ShapeDefs.push_back(std::move(ShapeBottom));
        Shapes.FixtureDefs.push_back(std::move(Fixture));
        {
            ShapeType TmpShape;
            for (auto i=1u; i<Shapes.ShapeDefs.back().size()-2; ++i)
            {
                TmpShape.push_back({float((i-1)-w-b), -h-b});
                TmpShape.push_back({float(i-w-b), -h-b});
                TmpShape.push_back(Shapes.ShapeDefs.back()[i]);
                TmpShape.push_back({float(i-w-b), -h-b});
                TmpShape.push_back(Shapes.ShapeDefs.back()[i+1]);
                TmpShape.push_back(Shapes.ShapeDefs.back()[i]);
            }
            
            GL::Mesh Mesh;
            GL::Buffer Buffer;
            Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(TmpShape), GL::BufferUsage::StaticDraw);
            Mesh.setCount(TmpShape.size())
                .setPrimitive(GL::MeshPrimitive::Triangles)
                .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
            Meshes_[int(GameObjectTypeE::LANDSCAPE)].push_back(std::move(Mesh));
        }
        
        DBLK(
            GlobalMessageHandler.reportDebug("Boundary octave count: " + std::to_string(OctaveCount), MessageHandler::DEBUG_L1);
            GlobalMessageHandler.reportDebug("Boundary vertex count: " + std::to_string(Shapes.ShapeDefs.back().size()), MessageHandler::DEBUG_L1);
        )
    }
    {
        ShapeType Shape;
        for (auto i=0.0f; i<2.0*b2_pi; i+=2.0*b2_pi/100.0)
        {
            auto Value = 5.0f * float(Boundary.GetValue(100.0 * std::cos(double(i)), 100.0 * std::sin(double(i))));
            
            Shape.push_back({(5.0f-Value)*std::cos(i), (5.0f-Value)*std::sin(i)+50.0f});
        }
        auto& Shapes = Shapes_[int(GameObjectTypeE::LANDSCAPE)];
        
        b2FixtureDef Fixture;
        Fixture.density =  1.0f;
        Fixture.friction = 0.8f;
        Fixture.restitution = 0.0f;
        Fixture.isSensor = false;
        
        Shapes.ShapeDefs.push_back(std::move(Shape));
        Shapes.FixtureDefs.push_back(std::move(Fixture));
        Shapes.Type = ShapeEnumType::CHAIN;
        
        GL::Mesh Mesh;
        GL::Buffer Buffer;
        Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(Shapes.ShapeDefs.back()), GL::BufferUsage::StaticDraw);
        Mesh.setCount(Shapes.ShapeDefs.back().size())
            .setPrimitive(GL::MeshPrimitive::TriangleFan)
            .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
        Meshes_[int(GameObjectTypeE::LANDSCAPE)].push_back(std::move(Mesh));
    }
}
