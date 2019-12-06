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
            ShaderVelocityDisplay_.bindTexture(TexVelocities_);
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
    TexVelocities_ = GL::Texture2D{};
    ShaderDiffusion_ = DiffusionShader();
    ShaderDensityDisplay_ = DensityShader();
    ShaderDensitySources_ = DensitySourcesShader();
    ShaderVelocityDisplay_ = VelocityDisplayShader();
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
    TexVelocities_.setStorage(1, GL::TextureFormat::RG32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
                    
    FBODensities_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBODensities_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexDensities_, 0)
                 .clearColor(0, Color4(0.0f, 0.0f, 0.0f, 0.0f));
    FBOVelocities_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBOVelocities_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexVelocities_, 0);
    FBODiffusion0_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBODiffusion0_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexDiffusionBack_, 0)
                    .clearColor(0, Color4(0.1f, 0.1f, 0.1f, 1.0f));
    FBODiffusion1_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBODiffusion1_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexDiffusionFront_, 0)
                    .clearColor(0, Color4(0.1f, 0.1f, 0.1f, 1.0f));
    
    FBODiffusionBack_ = &FBODiffusion0_;
    FBODiffusionFront_ = &FBODiffusion1_;
    
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
            
    MeshDensityDisplay_ = GL::Mesh{};
    MeshDensityDisplay_.setCount(6)
                        .setPrimitive(GL::MeshPrimitive::Triangles)
                        .addVertexBuffer(std::move(Buffer), 0,
                                    DensityShader::Position{},
                                    DensityShader::TextureCoordinates{});
                    
    ShaderDiffusion_.setTransformation(Matrix3::projection({WORLD_SIZE_X, WORLD_SIZE_Y})) 
                    .bindTextures(TexDensities_, TexDensityBase_ ,TexVelocities_,  TexDiffusionBack_);
            
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
    // Create mesh (points) representing density sources
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
    FBODensities_.clear(GL::FramebufferClear::Color)
                 .bind();
    MeshDensitySources.draw(ShaderDensitySources_);
    
    //------------------------------------------------------------------
    // Draw velocities
    //------------------------------------------------------------------
    FBOVelocities_.bind();
    MeshVelocitySources.draw(ShaderVelocitySources_);
    
    //------------------------------------------------------------------
    // Process diffusion
    //------------------------------------------------------------------
    FBODiffusionFront_->bind();
    
    if (BackBuffer) ShaderDiffusion_.bindTextures(TexDensities_, TexDensityBase_, TexVelocities_, TexDiffusionBack_);
    else ShaderDiffusion_.bindTextures(TexDensities_, TexDensityBase_, TexVelocities_, TexDiffusionFront_);
    BackBuffer ^= 1;
    
    Mesh_.draw(ShaderDiffusion_);
    
    std::swap(FBODiffusionFront_, FBODiffusionBack_);
    
    
}
