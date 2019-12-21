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
            ShaderDensityDisplay_.bindTexture(TexDensitySources_);
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
            ShaderVelocityDisplay_.bindTexture(*TexVelocitiesBack_);
            ShaderVelocityDisplay_.setTransformation(CameraProjection);
            MeshDensityDisplay_.draw(ShaderVelocityDisplay_);
            break;
        }
        case FluidBufferE::VELOCITIES_FRONT:
        {
            ShaderVelocityDisplay_.bindTexture(*TexVelocitiesFront_);
            ShaderVelocityDisplay_.setTransformation(CameraProjection);
            MeshDensityDisplay_.draw(ShaderVelocityDisplay_);
            break;
        }
        case FluidBufferE::DENSITY_DIFFUSION_FRONT:
        {
            ShaderDensityDisplay_.bindTexture(*TexDensitiesFront_);
            ShaderDensityDisplay_.setTransformation(CameraProjection);
            MeshDensityDisplay_.draw(ShaderDensityDisplay_);
            break;
        }
        case FluidBufferE::DENSITY_DIFFUSION_BACK:
        {
            ShaderDensityDisplay_.bindTexture(*TexDensitiesBack_);
            ShaderDensityDisplay_.setTransformation(CameraProjection);
            MeshDensityDisplay_.draw(ShaderDensityDisplay_);
            break;
        }
        case FluidBufferE::FINAL_COMPOSITION:
        {
            ShaderDensityDisplay_.bindTexture(*TexDensitiesFront_);
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
    TexDensitySources_ = GL::Texture2D{};
    TexDensities0_ = GL::Texture2D{};
    TexDensities1_ = GL::Texture2D{};
    TexVelocitySources_ = GL::Texture2D{};
    TexVelocities0_ = GL::Texture2D{};
    TexVelocities1_ = GL::Texture2D{};
    ShaderDensityAdvection_ = DensityAdvectionShader();
    ShaderDensityDiffusion_ = DensityDiffusionShader();
    ShaderDensityDisplay_ = DensityDisplayShader();
    ShaderDensitySources_ = DensitySourcesShader();
    ShaderVelocityAdvection_ = VelocityAdvectionShader();
    ShaderVelocityDiffusion_ = VelocityDiffusionShader();
    ShaderVelocityDisplay_ = VelocityDisplayShader();
    ShaderVelocitySources_ = VelocitySourcesShader();
    
    assert(DensityBase_ != nullptr);
    assert(DensityBase_->size() == FLUID_GRID_ARRAY_SIZE);
    ImageView2D Image(PixelFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}, *DensityBase_);

    TexDensityBase_.setStorage(1, GL::TextureFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y})
                   .setSubImage(0, {}, Image);
    TexDensitySources_.setStorage(1, GL::TextureFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
    TexDensities0_.setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER)
                  .setMinificationFilter(FLUID_GRID_DIFFUSION_FILTER, FLUID_GRID_DIFFUSION_FILTER_MIP_MAP)
                  .setWrapping(GL::SamplerWrapping::ClampToEdge)
                  .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
                  .setStorage(1/*Math::log2(FLUID_GRID_SIZE_X)+1*/, GL::TextureFormat::RGB32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
                    //.generateMipmap();
    TexDensities1_.setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER)
                  .setMinificationFilter(FLUID_GRID_DIFFUSION_FILTER, FLUID_GRID_DIFFUSION_FILTER_MIP_MAP)
                  .setWrapping(GL::SamplerWrapping::ClampToEdge)
                  .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
                  .setStorage(1/*Math::log2(FLUID_GRID_SIZE_X)+1*/, GL::TextureFormat::RGB32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
                    //.generateMipmap();
    TexVelocitySources_.setStorage(1, GL::TextureFormat::RG32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
    TexVelocities0_.setStorage(1, GL::TextureFormat::RG32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
    TexVelocities1_.setStorage(1, GL::TextureFormat::RG32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
    
    TexDensitiesBack_   = &TexDensities0_;
    TexDensitiesFront_  = &TexDensities1_;
    TexVelocitiesBack_  = &TexVelocities0_;
    TexVelocitiesFront_ = &TexVelocities1_;
                    
    FBODensitySources_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBODensitySources_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexDensitySources_, 0)
                      .clearColor(0, Color4(0.0f, 0.0f, 0.0f));
    FBODensities0_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBODensities0_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexDensities0_, 0)
                  .clearColor(0, Color4(0.0f, 0.0f, 0.0f));
    FBODensities1_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBODensities1_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexDensities1_, 0)
                  .clearColor(0, Color4(0.0f, 0.0f, 0.0f));
    FBOVelocitySources_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBOVelocitySources_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexVelocitySources_, 0)
                       .clearColor(0, Color4(0.0f, 0.0f));
    FBOVelocities0_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBOVelocities0_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexVelocities0_, 0)
                   .clearColor(0, Color4(0.0f, 0.0f));
    FBOVelocities1_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBOVelocities1_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexVelocities1_, 0)
                   .clearColor(0, Color4(0.0f, 0.0f));
    
    FBODensitiesBack_ = &FBODensities0_;
    FBODensitiesFront_ = &FBODensities1_;
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

    MeshDensityAdvection_ = GL::Mesh{};
    MeshDensityAdvection_.setCount(6)
                         .setPrimitive(GL::MeshPrimitive::Triangles)
                         .addVertexBuffer(Buffer, 0,
                                          DensityAdvectionShader::Position{},
                                          DensityAdvectionShader::TextureCoordinates{});
    
    MeshDensityDiffusion_ = GL::Mesh{};
    MeshDensityDiffusion_.setCount(6)
                         .setPrimitive(GL::MeshPrimitive::Triangles)
                         .addVertexBuffer(Buffer, 0,
                                          DensityDiffusionShader::Position{},
                                          DensityDiffusionShader::TextureCoordinates{});
         
    MeshVelocityAdvection_ = GL::Mesh{};
    MeshVelocityAdvection_.setCount(6)
                          .setPrimitive(GL::MeshPrimitive::Triangles)
                          .addVertexBuffer(Buffer, 0,
                                           VelocityAdvectionShader::Position{},
                                           VelocityAdvectionShader::TextureCoordinates{});
         
    MeshVelocities_ = GL::Mesh{};
    MeshVelocities_.setCount(6)
                   .setPrimitive(GL::MeshPrimitive::Triangles)
                   .addVertexBuffer(Buffer, 0,
                                    VelocityDiffusionShader::Position{},
                                    VelocityDiffusionShader::TextureCoordinates{});
            
    MeshDensityDisplay_ = GL::Mesh{};
    MeshDensityDisplay_.setCount(6)
                       .setPrimitive(GL::MeshPrimitive::Triangles)
                       .addVertexBuffer(std::move(Buffer), 0,
                                        DensityDiffusionShader::Position{},
                                        DensityDisplayShader::TextureCoordinates{});

    ShaderDensityAdvection_.setTransformation(Matrix3::projection({WORLD_SIZE_X, WORLD_SIZE_Y}))
                           .setDeltaT(1.0f/60.0f)
                           .setGridRes(FLUID_GRID_SIZE_X / WORLD_SIZE_X)
                           .bindTextures(*TexDensitiesBack_, *TexVelocitiesBack_);
    ShaderDensityDiffusion_.setTransformation(Matrix3::projection({WORLD_SIZE_X, WORLD_SIZE_Y}))
                           .setDeltaT(1.0f/60.0f)
                           .setGridSize(FLUID_GRID_ARRAY_SIZE)
                           .bindTextures(TexDensitySources_, TexDensityBase_ , *TexDensitiesBack_, *TexVelocitiesFront_);
    ShaderVelocityAdvection_.setTransformation(Matrix3::projection({WORLD_SIZE_X, WORLD_SIZE_Y}))
                            .setDeltaT(1.0f/60.0f)
                            .setGridRes(FLUID_GRID_SIZE_X / WORLD_SIZE_X)
                            .bindTexture(*TexVelocitiesBack_);
    ShaderVelocityDiffusion_.setTransformation(Matrix3::projection({WORLD_SIZE_X, WORLD_SIZE_Y}))
                            .setDeltaT(1.0f/60.0f)
                            .setGridSize(FLUID_GRID_ARRAY_SIZE)
                            .bindTextures(TexVelocitySources_, *TexVelocitiesBack_);
    ShaderDensitySources_.setTransformation(Matrix3::projection({WORLD_SIZE_X, WORLD_SIZE_Y}));
    ShaderVelocitySources_.setTransformation(Matrix3::projection({WORLD_SIZE_X, WORLD_SIZE_Y}));
    ShaderDensityDisplay_.bindTexture(*TexDensitiesFront_);
}

void FluidGrid::process()
{
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
    FBODensitySources_.clearColor(0, Color4(0.0f))
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
    std::swap(FBOVelocitiesFront_, FBOVelocitiesBack_);
    std::swap(TexVelocitiesFront_, TexVelocitiesBack_);
    
    FBOVelocitiesFront_->bind(); // FBOVelocities0_
    ShaderVelocityDiffusion_.bindTextures(TexVelocitySources_, *TexVelocitiesBack_); // TexVelocities1_
    
    MeshVelocities_.draw(ShaderVelocityDiffusion_);
    
    //------------------------------------------------------------------
    // Advect velocities
    //------------------------------------------------------------------
    std::swap(FBOVelocitiesFront_, FBOVelocitiesBack_);
    std::swap(TexVelocitiesFront_, TexVelocitiesBack_);
    
    FBOVelocitiesFront_->bind(); // FBOVelocities1_
    ShaderVelocityAdvection_.bindTexture(*TexVelocitiesBack_); // TexVelocities0_
    
    MeshVelocityAdvection_.draw(ShaderVelocityAdvection_);
    
    //------------------------------------------------------------------
    // Diffuse densities
    //------------------------------------------------------------------
    std::swap(FBODensitiesFront_, FBODensitiesBack_);
    std::swap(TexDensitiesFront_, TexDensitiesBack_);
    FBODensitiesFront_->bind();
    
    ShaderDensityDiffusion_.bindTextures(TexDensitySources_, TexDensityBase_, *TexDensitiesBack_, *TexVelocitiesFront_);
    
    MeshDensityDiffusion_.draw(ShaderDensityDiffusion_);
    
    //------------------------------------------------------------------
    // Advect densities
    //------------------------------------------------------------------
    std::swap(FBODensitiesFront_, FBODensitiesBack_);
    std::swap(TexDensitiesFront_, TexDensitiesBack_);
    FBODensitiesFront_->bind();
    
    ShaderDensityAdvection_.bindTextures(*TexDensitiesBack_, *TexVelocitiesFront_);
    
    MeshDensityAdvection_.draw(ShaderDensityAdvection_);
}
