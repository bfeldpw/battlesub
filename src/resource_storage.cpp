#include "resource_storage.h"

#include <atomic>
#include <thread>

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Magnum/Mesh.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/MeshData.h>

#include "common.h"
#include "noise.h"
#include "timer.h"
#include "world_def.h"

void ResourceStorage::init()
{
    this->initDebris();
    this->initHeightMap();
    this->initLandscape();
    this->initProjectile();
    this->initSubmarine();
    
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
    constexpr float DEBRIS_SIZE_AVG = 0.05;
    
    ShapeType ShapeG; // Shape for graphics differs from physics shape (circle)
        
    for (auto i=0.0f; i<2.0f*3.14159f; i+=2.0f*3.14159f/5.0f)
    {
        ShapeG.push_back({DEBRIS_SIZE_AVG*std::cos(i),
                          DEBRIS_SIZE_AVG*std::sin(i)});
    }
    
    auto& ShapesLandscape = Shapes_[int(GameObjectTypeE::DEBRIS_LANDSCAPE)];
    auto& ShapesSubmarine = Shapes_[int(GameObjectTypeE::DEBRIS_SUBMARINE)];

    b2FixtureDef Fixture;
    Fixture.density =  2.5f;
    Fixture.friction = 0.02f; // Use low value, since we are using a circle for performance reasons
    Fixture.restitution = 0.2f;
    Fixture.isSensor = false;
    
    // Physics shape (circle), always 3 values here
    // - center x
    // - center y
    // - radius 
    ShapeType ShapeP{{0.0f, 0.0f}, {0.05f, 0.0f}};
        
    ShapesLandscape.ShapeDefs.push_back(ShapeP);
    ShapesLandscape.FixtureDefs.push_back(Fixture);
    ShapesLandscape.Type = ShapeEnumType::CIRCLE;
    ShapesSubmarine.ShapeDefs.push_back(std::move(ShapeP));
    ShapesSubmarine.FixtureDefs.push_back(std::move(Fixture));
    ShapesSubmarine.Type = ShapeEnumType::CIRCLE;
            
    GL::Mesh MeshLandscape;
    GL::Mesh MeshSubmarine;
    GL::Buffer Buffer;
    Buffer.setData(this->convertGeometryPhysicsToGraphics(ShapeG), GL::BufferUsage::StaticDraw);
    MeshLandscape.setCount(ShapeG.size())
                 .setPrimitive(GL::MeshPrimitive::TriangleFan)
                 .addVertexBuffer(Buffer, 0, Shaders::VertexColor2D::Position{});
    MeshSubmarine.setCount(ShapeG.size())
                 .setPrimitive(GL::MeshPrimitive::TriangleFan)
                 .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
    Meshes_[int(GameObjectTypeE::DEBRIS_LANDSCAPE)].push_back(std::move(MeshLandscape));
    Meshes_[int(GameObjectTypeE::DEBRIS_SUBMARINE)].push_back(std::move(MeshSubmarine));
}

void ResourceStorage::initHeightMap()
{
    noise::module::RidgedMulti  HeightmapRidge;
    noise::module::Perlin       HeightmapBillow;
    noise::module::Add          Average;
    noise::module::Turbulence   FinalTerrain;
    noise::module::Exponent     Exponent;
    noise::module::Clamp        Plateus;

    {
        double FrequencyRidged = 0.001;
        double LacunarityRidged = 1.7931;
    
        HeightmapRidge.SetFrequency(FrequencyRidged);
        HeightmapRidge.SetLacunarity(LacunarityRidged);
    //     HeightmapRidge.SetPersistence(0.5371);
        HeightmapRidge.SetNoiseQuality(noise::QUALITY_BEST);
        HeightmapRidge.SetNoiseType(noise::TYPE_VALUE);
    
    
        int OctaveCount = ceil(log2(1.0/FrequencyRidged)/log2(LacunarityRidged));
        if (OctaveCount < 1) OctaveCount = 1;
        
        HeightmapRidge.SetOctaveCount(OctaveCount);
        HeightmapRidge.SetSeed(7);
    }
    {
        double Frequency = 0.01;
        double Lacunarity = 1.9731;
    
        HeightmapBillow.SetFrequency(Frequency);
        HeightmapBillow.SetLacunarity(Lacunarity);
        HeightmapBillow.SetPersistence(0.5371);
        HeightmapBillow.SetNoiseQuality(noise::QUALITY_BEST);
        HeightmapBillow.SetNoiseType(noise::TYPE_VALUE);

        int OctaveCount = ceil(log2(1.0/Frequency)/log2(Lacunarity));
        if (OctaveCount < 1) OctaveCount = 1;
        
        HeightmapBillow.SetOctaveCount(OctaveCount);
        HeightmapBillow.SetSeed(13);
    }
    Average.SetSourceModule(0, HeightmapRidge);
    Average.SetSourceModule(1, HeightmapBillow);
    FinalTerrain.SetSourceModule(0, Average);
    FinalTerrain.SetFrequency(0.005);
    FinalTerrain.SetPower(500.0);
    
    Exponent.SetExponent(2.0);
    Exponent.SetSourceModule(0, FinalTerrain);
    
    Plateus.SetBounds(-1.0, 0.6);
    Plateus.SetSourceModule(0, Exponent);
    
    // Fill heightmap, use multithreading
    HeightMap_.resize(FLUID_GRID_ARRAY_SIZE);
    
    auto NoOfThreads = std::thread::hardware_concurrency();
    
    GlobalMessageHandler.report("Building height map.");
    DBLK(
        GlobalMessageHandler.reportDebug("Number of threads: " + std::to_string(NoOfThreads));
        Timer HeightMapTimer;
        HeightMapTimer.start();
    )
    
    std::vector<std::thread> Workers;
    for (auto i=0u; i<NoOfThreads; ++i)
    {
        Workers.push_back(std::thread(
            [this, &Plateus, NoOfThreads, i]()
            {
                for (auto y =    i *FLUID_GRID_SIZE_Y/NoOfThreads;
                          y < (i+1)*FLUID_GRID_SIZE_Y/NoOfThreads; ++y)
                {
                    for (auto x=0u; x<FLUID_GRID_SIZE_X; ++x)
                    {
                        auto v = float(Plateus.GetValue(double(x), double(y))*0.5+0.6);
                        // if (v>=0.6) v*=2.0;
                        HeightMap_[(y << FLUID_GRID_SIZE_X_BITS) + x] = v;
                    }
                }
            }
        ));
    }
    for (auto i=0u; i<NoOfThreads; ++i) Workers[i].join();
    
    DBLK(
        HeightMapTimer.stop();
        GlobalMessageHandler.reportDebug("Done in " + std::to_string(HeightMapTimer.elapsed()) + "s.");
    )
    
    
//     for (auto y=0u; y<FLUID_GRID_SIZE_Y; ++y)
//     {
//         for (auto x=0u; x<FLUID_GRID_SIZE_X; ++x)
//         {
//             if (HeightMap_[y*FLUID_GRID_SIZE_X+x] < 0.6f)
//                 HeightMap_[y*FLUID_GRID_SIZE_X+x] = 0.0;
//             else
//                 HeightMap_[y*FLUID_GRID_SIZE_X+x] = 1.0;
//         }
//     }
    
    HeightMapPlateausBack_ = &HeightMapPlateaus0_;
    HeightMapPlateausFront_ = &HeightMapPlateaus1_;
    HeightMapPlateausBack_->resize(FLUID_GRID_ARRAY_SIZE);
    HeightMapPlateausFront_->resize(FLUID_GRID_ARRAY_SIZE);
    
    for (auto y=0u; y<FLUID_GRID_SIZE_Y; ++y)
    {
        for (auto x=0u; x<FLUID_GRID_SIZE_X; ++x)
        {
            (*HeightMapPlateausBack_)[y*FLUID_GRID_SIZE_X+x] = HeightMap_[y*FLUID_GRID_SIZE_X+x];
            (*HeightMapPlateausFront_)[y*FLUID_GRID_SIZE_X+x] = HeightMap_[y*FLUID_GRID_SIZE_X+x];
        }
    }
    //--- Erosion and dilation ---//
    // for (auto i=0u; i<5u; ++i)
    // {
    //     for (auto y=1u; y<FLUID_GRID_SIZE_Y-1; ++y)
    //     {
    //         for (auto x=1u; x<FLUID_GRID_SIZE_X-1; ++x)
    //         {
    //             float Value = 1.0f;
    //             float ValueMin = 1.0f;
    //             for (auto m=-1; m<2; ++m)
    //             {
    //                 for (auto n=-1; n<2; ++n)
    //                 {
    //                     Value = (*HeightMapPlateausBack_)[(y+m)*FLUID_GRID_SIZE_X+x+n];
    //                     if (Value < ValueMin) ValueMin = Value;
    //                 }
    //             }
    //             (*HeightMapPlateausFront_)[y*FLUID_GRID_SIZE_X+x] = ValueMin;
    //         }
    //     }
    //     std::swap(HeightMapPlateausFront_, HeightMapPlateausBack_);
    // }
    // for (auto i=0u; i<5u; ++i)
    // {
    //     for (auto y=1u; y<FLUID_GRID_SIZE_Y-1; ++y)
    //     {
    //         for (auto x=1u; x<FLUID_GRID_SIZE_X-1; ++x)
    //         {
    //             float Value = 0.0f;
    //             float ValueMax = 0.0f;
    //             for (auto m=-1; m<2; ++m)
    //             {
    //                 for (auto n=-1; n<2; ++n)
    //                 {
    //                     Value = (*HeightMapPlateausBack_)[(y+m)*FLUID_GRID_SIZE_X+x+n];
    //                     if (Value > ValueMax) ValueMax = Value;
    //                 }
    //             }
    //             (*HeightMapPlateausFront_)[y*FLUID_GRID_SIZE_X+x] = ValueMax;
    //         }
    //     }
    //     std::swap(HeightMapPlateausFront_, HeightMapPlateausBack_);
    // }
    // //--- Kind of Marching Squares ---//
    // std::swap(HeightMapPlateausFront_, HeightMapPlateausBack_);
    // for (auto y=1u; y<FLUID_GRID_SIZE_Y-1; ++y)
    // {
    //     for (auto x=1u; x<FLUID_GRID_SIZE_X-1; ++x)
    //     {
    //         int NoOnes = 0;
    //         for (auto m=-1; m<2; ++m)
    //         {
    //             for (auto n=-1; n<2; ++n)
    //             {
    //                 float Value = (*HeightMapPlateausBack_)[(y+m)*FLUID_GRID_SIZE_X+x+n];
    //                 if (0.4f < Value) ++NoOnes;
    //             }
    //         }
    //         if (NoOnes == 0 ||  NoOnes > 1) (*HeightMapPlateausFront_)[y*FLUID_GRID_SIZE_X+x] = 0.0;
    //         else (*HeightMapPlateausFront_)[y*FLUID_GRID_SIZE_X+x] = 1.0;
    //     }
    // }
    
    for (auto y=0u; y<FLUID_GRID_SIZE_Y; ++y)
    {
        for (auto x=0u; x<FLUID_GRID_SIZE_X; ++x)
        {
            HeightMap_[y*FLUID_GRID_SIZE_X+x] = (*HeightMapPlateausFront_)[y*FLUID_GRID_SIZE_X+x];
        }
    }
}

void ResourceStorage::initLandscape()
{
    // Initialise landscape
    noise::module::Perlin Boundary;
    
    Boundary.SetFrequency(0.008*5.0);
    Boundary.SetPersistence(0.47317);
    Boundary.SetLacunarity(1.9731);
    Boundary.SetNoiseQuality(noise::QUALITY_BEST);
    Boundary.SetNoiseType(noise::TYPE_VALUE);
    
    int OctaveCount = ceil(log2(1/0.005)/log2(1.9731));
    if (OctaveCount < 1) OctaveCount = 1;
    
    Boundary.SetOctaveCount(OctaveCount);
    Boundary.SetSeed(19);
    
    {
        constexpr float b = 10.0f;  // Boundary
        constexpr float w = WORLD_SIZE_DEFAULT_X * 0.5f - b; // Half width
        constexpr float h = WORLD_SIZE_DEFAULT_Y * 0.5f - b; // Half height
        constexpr float a = 10.0f;  // Amplitude
        
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
        
        // Left
        ShapeType ShapeLeft;
        ShapeLeft.push_back({-w-b, -h-b});
        for (auto i=-h-b; i<=h+b; i+=1.0f)
        {
            ShapeLeft.push_back({-w + a * float(Boundary.GetValue(double(-w), double(i))), i});
        }
        ShapeLeft.push_back({-w-b, h+b});
        
        // Bottom
        ShapeType ShapeBottom;
        ShapeBottom.push_back({w+b, -h-b});
        for (auto i=w+b; i>=-w-b; i-=1.0f)
        {
            ShapeBottom.push_back({i, -h + a * float(Boundary.GetValue(double(i), double(-h)))});
        }
        ShapeBottom.push_back({-w-b, -h-b});
        
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
            Buffer.setData(this->convertGeometryPhysicsToGraphics(TmpShape), GL::BufferUsage::StaticDraw);
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
            Buffer.setData(this->convertGeometryPhysicsToGraphics(TmpShape), GL::BufferUsage::StaticDraw);
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
                TmpShape.push_back({-w-b, float((i-1)-h-b)});
                TmpShape.push_back({-w-b, float(i-h-b)});
                TmpShape.push_back(Shapes.ShapeDefs.back()[i]);
                TmpShape.push_back({-w-b, float(i-h-b)});
                TmpShape.push_back(Shapes.ShapeDefs.back()[i+1]);
                TmpShape.push_back(Shapes.ShapeDefs.back()[i]);
            }
            
            GL::Mesh Mesh;
            GL::Buffer Buffer;
            Buffer.setData(this->convertGeometryPhysicsToGraphics(TmpShape), GL::BufferUsage::StaticDraw);
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
                TmpShape.push_back({float(w+b-(i-1)), -h-b});
                TmpShape.push_back({float(w+b-i), -h-b});
                TmpShape.push_back(Shapes.ShapeDefs.back()[i]);
                TmpShape.push_back({float(w+b-i), -h-b});
                TmpShape.push_back(Shapes.ShapeDefs.back()[i+1]);
                TmpShape.push_back(Shapes.ShapeDefs.back()[i]);
            }
            
            GL::Mesh Mesh;
            GL::Buffer Buffer;
            Buffer.setData(this->convertGeometryPhysicsToGraphics(TmpShape), GL::BufferUsage::StaticDraw);
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
        Boundary.SetFrequency(0.008);
        ShapeType Shape;
        for (auto i=0.0f; i<2.0f*b2_pi; i+=2.0f*b2_pi/100.0f)
        {
            auto Value = 5.0f * float(Boundary.GetValue(100.0 * std::cos(double(i)), 100.0 * std::sin(double(i))));
            
            Shape.push_back({(5.0f-Value)*std::cos(i), (5.0f-Value)*std::sin(i)+10.0f});
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
        Buffer.setData(this->convertGeometryPhysicsToGraphics(Shapes.ShapeDefs.back()), GL::BufferUsage::StaticDraw);
        Mesh.setCount(Shapes.ShapeDefs.back().size())
            .setPrimitive(GL::MeshPrimitive::TriangleFan)
            .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
        Meshes_[int(GameObjectTypeE::LANDSCAPE)].push_back(std::move(Mesh));
    }
    {
        Boundary.SetFrequency(0.008);
        ShapeType Shape;
        for (auto i=0.0f; i<2.0f*b2_pi; i+=2.0f*b2_pi/100.0f)
        {
            auto Value = 50.0f * float(Boundary.GetValue(100.0 * std::cos(double(i)), 100.0 * std::sin(double(i))));

            Shape.push_back({(5.0f-Value)*std::cos(i)-20.0f, (5.0f-Value)*std::sin(i)-50.0f});
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
        Buffer.setData(this->convertGeometryPhysicsToGraphics(Shapes.ShapeDefs.back()), GL::BufferUsage::StaticDraw);
        Mesh.setCount(Shapes.ShapeDefs.back().size())
            .setPrimitive(GL::MeshPrimitive::TriangleFan)
            .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
        Meshes_[int(GameObjectTypeE::LANDSCAPE)].push_back(std::move(Mesh));
    }
}

void ResourceStorage::initProjectile()
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
    Buffer.setData(this->convertGeometryPhysicsToGraphics(Shapes.ShapeDefs.back()), GL::BufferUsage::StaticDraw);
    Mesh.setCount(Shapes.ShapeDefs.back().size())
        .setPrimitive(GL::MeshPrimitive::TriangleFan)
        .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
    Meshes_[int(GameObjectTypeE::PROJECTILE)].push_back(std::move(Mesh));
}

void ResourceStorage::initSubmarine()
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
                        this->convertGeometryPhysicsToGraphics(Shapes.ShapeDefs.back()),
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
                        this->convertGeometryPhysicsToGraphics(Shapes.ShapeDefs.back()),
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
        Buffer.setData(this->convertGeometryPhysicsToGraphics(Shapes.ShapeDefs.back()), GL::BufferUsage::StaticDraw);
        Mesh.setCount(Shapes.ShapeDefs.back().size())
            .setPrimitive(GL::MeshPrimitive::TriangleFan)
            .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
        Meshes_[int(GameObjectTypeE::SUBMARINE_RUDDER)].push_back(std::move(Mesh));
    }
    
//     PluginManager::Manager<Trade::AbstractImporter> SceneManager;
//     Containers::Pointer<Trade::AbstractImporter> Importer = SceneManager.loadAndInstantiate("AnySceneImporter");
//     
//     
//     if (!Importer) GlobalErrorHandler.reportError("Couldn't instantiate importer plugin AnySceneImporter.");
// 
//     std::string Filename = "submarine2.obj";
//     if (!Importer->openFile(Filename))
//     {
//         GlobalErrorHandler.reportError("Couldn't open file "+ Filename);
//     }
//     else
//     {
//         GlobalMessageHandler.report("Successfully opened file "+ Filename);
//     }
//     
//     Containers::Array<GL::Mesh> Meshes;
//     Meshes = Containers::Array<GL::Mesh>{Importer->mesh3DCount()};
//     for(UnsignedInt i = 0; i != Importer->mesh3DCount(); ++i)
//     {
//         std::cout << "Importing mesh" << i << Importer->mesh3DName(i) << std::endl;
// 
//         Containers::Optional<Trade::MeshData3D> MeshData = Importer->mesh3D(i);
//         if(!MeshData || !MeshData->hasNormals() || MeshData->primitive() != MeshPrimitive::Triangles)
//         {
//             std::cout << "Cannot load the mesh, skipping" << std::endl;
//         }
// 
//         /* Compile the mesh */
//         Meshes[i] = MeshTools::compile(*MeshData);
//         Meshes_[int(GameObjectTypeE::SUBMARINE_HULL)].push_back(std::move(Meshes[i]));
//     }
    
}

std::vector<Vector2> ResourceStorage::convertGeometryPhysicsToGraphics(const std::vector<b2Vec2>& Verts)
{
    std::vector<Vector2> Tmp;

    for (auto Vec : Verts)
    {
        Tmp.push_back({Vec.x, Vec.y});
    }
    return Tmp; // Note: No std::move needed, might even prevent copy elision
}
