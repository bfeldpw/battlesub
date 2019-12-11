#include "fluid_grid.h"

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Math/Color.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>

using namespace Magnum;


FluidGrid& FluidGrid::addDensity(const float x, const float y, const float d)
{
    DensitySources_.push_back(x);
    DensitySources_.push_back(y);
    DensitySources_.push_back(d);
    return *this;
}

FluidGrid& FluidGrid::addVelocity(const float x, const float y, const float Vx, const float Vy)
{
    VelocitySources_.push_back(x);
    VelocitySources_.push_back(y);
    VelocitySources_.push_back(Vx);
    VelocitySources_.push_back(Vy);
    return *this;
}

FluidGrid& FluidGrid::setDensityBase(std::vector<float>* const DensityBase)
{
    DensityBase_ = DensityBase;
    return *this;
}

void FluidGrid::display(const Matrix3 CameraProjection,
                        const FluidBufferE Buffer)
{
    GL::defaultFramebuffer.bind();
    
    switch (Buffer)
    {
        case FluidBufferE::DENSITY_SOURCES:
        {
            ShaderDensityDisplay_.bindTexture(TexDensities_);
            ShaderDensityDisplay_.setTransformation(CameraProjection);
            MeshDensityDisplay_.draw(ShaderDensityDisplay_);
            break;
        }
        case FluidBufferE::DENSITY_BASE:
        {
            ShaderDensityDisplay_.bindTexture(TexDensityBase_);
            ShaderDensityDisplay_.setTransformation(CameraProjection);
            MeshDensityDisplay_.draw(ShaderDensityDisplay_);
            break;
        }
        case FluidBufferE::VELOCITY_SOURCES:
        {
            ShaderVelocityDisplay_.bindTexture(TexVelocitySources_);
            ShaderVelocityDisplay_.setTransformation(CameraProjection);
            MeshDensityDisplay_.draw(ShaderVelocityDisplay_);
            break;
        }
        case FluidBufferE::VELOCITIES_BACK:
        {
            ShaderVelocityDisplay_.bindTexture(TexVelocitiesBack_);
            ShaderVelocityDisplay_.setTransformation(CameraProjection);
            MeshDensityDisplay_.draw(ShaderVelocityDisplay_);
            break;
        }
        case FluidBufferE::VELOCITIES_FRONT:
        {
            ShaderVelocityDisplay_.bindTexture(TexVelocitiesFront_);
            ShaderVelocityDisplay_.setTransformation(CameraProjection);
            MeshDensityDisplay_.draw(ShaderVelocityDisplay_);
            break;
        }
        case FluidBufferE::DENSITY_DIFFUSION_FRONT:
        {
            ShaderDensityDisplay_.bindTexture(TexDiffusionFront_);
            ShaderDensityDisplay_.setTransformation(CameraProjection);
            MeshDensityDisplay_.draw(ShaderDensityDisplay_);
            break;
        }
        case FluidBufferE::DENSITY_DIFFUSION_BACK:
        {
            ShaderDensityDisplay_.bindTexture(TexDiffusionBack_);
            ShaderDensityDisplay_.setTransformation(CameraProjection);
            MeshDensityDisplay_.draw(ShaderDensityDisplay_);
            break;
        }
        case FluidBufferE::FINAL_COMPOSITION:
        {
            ShaderDensityDisplay_.bindTexture(TexDiffusionFront_);
            ShaderDensityDisplay_.setTransformation(CameraProjection);
            MeshDensityDisplay_.draw(ShaderDensityDisplay_);
            break;
        }
        default: break;
    }
}

void FluidGrid::init()
{
    TexDensityBase_ = GL::Texture2D{};
    TexDensities_ = GL::Texture2D{};
    TexDiffusionBack_ = GL::Texture2D{};
    TexDiffusionFront_ = GL::Texture2D{};
    TexVelocitySources_ = GL::Texture2D{};
    TexVelocitiesBack_ = GL::Texture2D{};
    TexVelocitiesFront_ = GL::Texture2D{};
    ShaderDensityDisplay_ = DensityShader();
    ShaderDensitySources_ = DensitySourcesShader();
    ShaderDiffusion_ = DiffusionShader();
    ShaderVelocityDisplay_ = VelocityDisplayShader();
    ShaderVelocityProcessing_ = VelocityProcessingShader();
    ShaderVelocitySources_ = VelocitySourcesShader();
    
    assert(DensityBase_ != nullptr);
    assert(DensityBase_->size() == FLUID_GRID_ARRAY_SIZE);
    ImageView2D Image(PixelFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}, *DensityBase_);

    TexDensityBase_.setStorage(1, GL::TextureFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y})
                   .setSubImage(0, {}, Image);
    TexDensities_.setStorage(1, GL::TextureFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
    TexDiffusionBack_.setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER)
                     .setMinificationFilter(FLUID_GRID_DIFFUSION_FILTER, FLUID_GRID_DIFFUSION_FILTER_MIP_MAP)
                     .setWrapping(GL::SamplerWrapping::ClampToEdge)
                     .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
                     .setStorage(1/*Math::log2(FLUID_GRID_SIZE_X)+1*/, GL::TextureFormat::RGB32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
                    //.generateMipmap();
    TexDiffusionFront_.setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER)
                      .setMinificationFilter(FLUID_GRID_DIFFUSION_FILTER, FLUID_GRID_DIFFUSION_FILTER_MIP_MAP)
                      .setWrapping(GL::SamplerWrapping::ClampToEdge)
                      .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
                      .setStorage(1/*Math::log2(FLUID_GRID_SIZE_X)+1*/, GL::TextureFormat::RGB32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
                    //.generateMipmap();
    TexVelocitySources_.setStorage(1, GL::TextureFormat::RG32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
    TexVelocitiesBack_.setStorage(1, GL::TextureFormat::RG32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
    TexVelocitiesFront_.setStorage(1, GL::TextureFormat::RG32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
                    
    FBODensities_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBODensities_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexDensities_, 0)
                 .clearColor(0, Color4(0.0f, 0.0f, 0.0f));
    FBODiffusion0_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBODiffusion0_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexDiffusionBack_, 0)
                  .clearColor(0, Color4(0.0f, 0.0f, 0.0f));
    FBODiffusion1_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBODiffusion1_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexDiffusionFront_, 0)
                  .clearColor(0, Color4(0.0f, 0.0f, 0.0f));
    FBOVelocitySources_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBOVelocitySources_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexVelocitySources_, 0)
                       .clearColor(0, Color4(0.0f, 0.0f));
    FBOVelocities0_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBOVelocities0_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexVelocitiesBack_, 0)
                   .clearColor(0, Color4(0.0f, 0.0f));
    FBOVelocities1_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBOVelocities1_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexVelocitiesFront_, 0)
                   .clearColor(0, Color4(0.0f, 0.0f));
    
    FBODiffusionBack_ = &FBODiffusion0_;
    FBODiffusionFront_ = &FBODiffusion1_;
    FBOVelocitiesBack_ = &FBOVelocities0_;
    FBOVelocitiesFront_ = &FBOVelocities1_;
    
    struct Vertex {
        Vector2 Pos;
        Vector2 Tex;
    };
    constexpr float x = WORLD_SIZE_X*0.5f;
    constexpr float y = WORLD_SIZE_Y*0.5f;
    Vertex Data[6]{
        {{-x, -y}, { 0.0f,  0.0f}},
        {{ x, -y}, { 1.0f,  0.0f}},
        {{-x,  y}, { 0.0f,  1.0f}},
        {{-x,  y}, { 0.0f,  1.0f}},
        {{ x, -y}, { 1.0f,  0.0f}},
        {{ x,  y}, { 1.0f,  1.0f}}
    };

    GL::Buffer Buffer;
    Buffer.setData(Data, GL::BufferUsage::StaticDraw);

    Mesh_ = GL::Mesh{};
    Mesh_.setCount(6)
         .setPrimitive(GL::MeshPrimitive::Triangles)
         .addVertexBuffer(Buffer, 0,
                          DiffusionShader::Position{},
                          DiffusionShader::TextureCoordinates{});
         
    MeshVelocities_ = GL::Mesh{};
    MeshVelocities_.setCount(6)
                   .setPrimitive(GL::MeshPrimitive::Triangles)
                   .addVertexBuffer(Buffer, 0,
                                    VelocityProcessingShader::Position{},
                                    VelocityProcessingShader::TextureCoordinates{});
            
    MeshDensityDisplay_ = GL::Mesh{};
    MeshDensityDisplay_.setCount(6)
                        .setPrimitive(GL::MeshPrimitive::Triangles)
                        .addVertexBuffer(std::move(Buffer), 0,
                                    DensityShader::Position{},
                                    DensityShader::TextureCoordinates{});
                    
    ShaderDiffusion_.setTransformation(Matrix3::projection({WORLD_SIZE_X, WORLD_SIZE_Y}))
                    .setDeltaT(1.0f/60.0f)
                    .setGridSize(FLUID_GRID_ARRAY_SIZE)
                    .bindTextures(TexDensities_, TexDensityBase_ , TexDiffusionBack_, TexVelocitiesBack_);
    ShaderVelocityProcessing_.setTransformation(Matrix3::projection({WORLD_SIZE_X, WORLD_SIZE_Y}))
                             .setDeltaT(1.0f/60.0f)
                             .setGridSize(FLUID_GRID_ARRAY_SIZE)
                             .bindTextures(TexVelocitySources_, TexVelocitiesBack_);
            
    ShaderDensitySources_.setTransformation(Matrix3::projection({WORLD_SIZE_X, WORLD_SIZE_Y}));
    ShaderVelocitySources_.setTransformation(Matrix3::projection({WORLD_SIZE_X, WORLD_SIZE_Y}));
    ShaderDensityDisplay_.bindTexture(TexDiffusionFront_);
}

void FluidGrid::process()
{
    static bool BackBuffer = true;
    
    //------------------------------------------------------------------
    // Create mesh (points) representing density sources
    //------------------------------------------------------------------
    GL::Buffer DensitySourcesBuffer;
    DensitySourcesBuffer.setData(DensitySources_, GL::BufferUsage::StreamDraw);
    
    GL::Mesh MeshDensitySources;
    MeshDensitySources.setCount(DensitySources_.size()/3)
                      .setPrimitive(GL::MeshPrimitive::Points)
                      .addVertexBuffer(std::move(DensitySourcesBuffer), 0,
                                       DensitySourcesShader::Position{},
                                       DensitySourcesShader::Amount{});
        
    DensitySources_.clear();
    
    //------------------------------------------------------------------
    // Create mesh (points) representing velocity sources
    //------------------------------------------------------------------
    GL::Buffer VelocitySourcesBuffer;
    VelocitySourcesBuffer.setData(VelocitySources_, GL::BufferUsage::StreamDraw);
    
    GL::Mesh MeshVelocitySources;
    MeshVelocitySources.setCount(VelocitySources_.size()/4)
                       .setPrimitive(GL::MeshPrimitive::Points)
                       .addVertexBuffer(std::move(VelocitySourcesBuffer), 0,
                                        VelocitySourcesShader::Position{},
                                        VelocitySourcesShader::Velocity{});

    VelocitySources_.clear();
    
    //------------------------------------------------------------------
    // Draw densities
    //------------------------------------------------------------------
    FBODensities_.clearColor(0, Color4(0.0f))
                 .bind();
    MeshDensitySources.draw(ShaderDensitySources_);
    
    //------------------------------------------------------------------
    // Draw velocities
    //------------------------------------------------------------------
    FBOVelocitySources_.clearColor(0, Color4(0.0f,0.0f))
                       .bind();
    MeshVelocitySources.draw(ShaderVelocitySources_);
    
    //------------------------------------------------------------------
    // Process velocities
    //------------------------------------------------------------------
    FBOVelocitiesFront_->bind();
    
    if (BackBuffer)
    {
        ShaderVelocityProcessing_.bindTextures(TexVelocitySources_, TexVelocitiesBack_);
    }
    else
    {
        ShaderVelocityProcessing_.bindTextures(TexVelocitySources_, TexVelocitiesFront_);
    }
    
    MeshVelocities_.draw(ShaderVelocityProcessing_);
    
    //------------------------------------------------------------------
    // Process diffusion
    //------------------------------------------------------------------
    FBODiffusionFront_->bind();
    
    if (BackBuffer)
    {
        ShaderDiffusion_.bindTextures(TexDensities_, TexDensityBase_, TexDiffusionBack_, TexVelocitiesBack_);
    }
    else
    {
        ShaderDiffusion_.bindTextures(TexDensities_, TexDensityBase_, TexDiffusionFront_, TexVelocitiesFront_);
    }
    BackBuffer ^= 1;
    
    Mesh_.draw(ShaderDiffusion_);
    
    std::swap(FBOVelocitiesFront_, FBOVelocitiesBack_);
    std::swap(FBODiffusionFront_, FBODiffusionBack_);
}
