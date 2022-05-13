#include "fluid_grid.h"
#include "lua_manager.hpp"

#include <Corrade/Containers/ArrayViewStl.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Math/Color.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>
#include <entt/entity/fwd.hpp>

using namespace Magnum;

void FluidGrid::loadConfig()
{
    auto& ConfLua = Reg_.ctx<LuaManager>();
    auto Cfg = ConfLua.Lua_["config"];
    Config_.IterationsDensityDiffusion_ = Cfg["iterations_density_diffusion"];
    Config_.IterationsVelocityDiffusion_ = Cfg["iterations_velocity_diffusion"];
    Config_.IterationsPressureEquation_ = Cfg["iterations_pressure_equation"];
}

Vector2 FluidGrid::getVelocity(const float _x, const float _y) const
{
    auto x = int((_x + WORLD_SIZE_DEFAULT_X*0.5f)*float(FLUID_GRID_SIZE_X)/WORLD_SIZE_DEFAULT_X);
    auto y = int((_y + WORLD_SIZE_DEFAULT_Y*0.5f)*float(FLUID_GRID_SIZE_Y)/WORLD_SIZE_DEFAULT_Y);

    int x0 = x >> VELOCITY_READBACK_SUBSAMPLE;
    int y0 = y >> VELOCITY_READBACK_SUBSAMPLE;

    int xi =  ((y0 << (FLUID_GRID_SIZE_X_BITS - VELOCITY_READBACK_SUBSAMPLE)) + x0) << 1;

    auto Vx = VelReadback_[xi];
    auto Vy = VelReadback_[xi+1];
    return {Vx, Vy};
}

FluidGrid& FluidGrid::addDensity(const float x, const float y, const float d)
{
    DensitySourcesPoints_.push_back(x);
    DensitySourcesPoints_.push_back(y);
    DensitySourcesPoints_.push_back(d);
    return *this;
}

FluidGrid& FluidGrid::addDensity(const float x0, const float y0,
                                 const float x1, const float y1, const float d)
{
    DensitySourcesLines_.push_back(x0);
    DensitySourcesLines_.push_back(y0);
    DensitySourcesLines_.push_back(d);
    DensitySourcesLines_.push_back(x1);
    DensitySourcesLines_.push_back(y1);
    DensitySourcesLines_.push_back(d);
    return *this;
}

FluidGrid& FluidGrid::addVelocity(const float x, const float y, const float Vx, const float Vy, const float w)
{
    VelocitySourcesPoints_.push_back(x);
    VelocitySourcesPoints_.push_back(y);
    VelocitySourcesPoints_.push_back(Vx);
    VelocitySourcesPoints_.push_back(Vy);
    VelocitySourcesPoints_.push_back(w);
    return *this;
}

FluidGrid& FluidGrid::addVelocity(const float x0, const float y0, const float Vx0, const float Vy0,
                                  const float x1, const float y1, const float Vx1, const float Vy1,
                                  const float w)
{
    VelocitySourcesLines_.push_back(x0);
    VelocitySourcesLines_.push_back(y0);
    VelocitySourcesLines_.push_back(Vx0);
    VelocitySourcesLines_.push_back(Vy0);
    VelocitySourcesLines_.push_back(w);
    VelocitySourcesLines_.push_back(x1);
    VelocitySourcesLines_.push_back(y1);
    VelocitySourcesLines_.push_back(Vx1);
    VelocitySourcesLines_.push_back(Vy1);
    VelocitySourcesLines_.push_back(w);

    return *this;
}

FluidGrid& FluidGrid::setDensityBase(std::vector<float>* const DensityBase)
{
    DensityBase_ = DensityBase;
    return *this;
}

void FluidGrid::bindBoundaryMap()
{
    FBOBoundaries_.clearColor(0, Color4(0.0f))
                  .bind();
}

void FluidGrid::display(const Matrix3 CameraProjection,
                        const FluidBufferE Buffer)
{
    switch (Buffer)
    {
        case FluidBufferE::BOUNDARIES:
        {
            ShaderVelocityDisplay_.bindTexture(TexBoundaries_)
                                  .setTransformation(CameraProjection)
                                  .draw(MeshFluidGridBuffer_);
            break;
        }
        case FluidBufferE::DENSITY_SOURCES:
        {
            ShaderDensityDisplay_.bindTexture(TexDensitySources_)
                                 .setTransformation(CameraProjection)
                                 .draw(MeshFluidGridBuffer_);
            break;
        }
        case FluidBufferE::DENSITY_BASE:
        {
            ShaderDensityDisplay_.bindTexture(TexDensityBase_)
                                 .setTransformation(CameraProjection)
                                 .draw(MeshFluidGridBuffer_);
            break;
        }
        case FluidBufferE::VELOCITY_SOURCES:
        {
            ShaderVelocityDisplay_.bindTexture(TexVelocitySources_)
                                  .setTransformation(CameraProjection)
                                  .draw(MeshFluidGridBuffer_);
            break;
        }
        case FluidBufferE::VELOCITIES_BACK:
        {
            ShaderVelocityDisplay_.bindTexture(*TexVelocitiesBack_)
                                  .setTransformation(CameraProjection)
                                  .draw(MeshFluidGridBuffer_);
            break;
        }
        case FluidBufferE::VELOCITIES_FRONT:
        {
            ShaderVelocityDisplay_.bindTexture(*TexVelocitiesFront_)
                                  .setTransformation(CameraProjection)
                                  .draw(MeshFluidGridBuffer_);
            break;
        }
        case FluidBufferE::VELOCITY_DIVERGENCE:
        {
            ShaderDensityDisplay_.bindTexture(TexVelocityDivergence_)
                                 .setTransformation(CameraProjection)
                                 .draw(MeshFluidGridBuffer_);
            break;
        }
        case FluidBufferE::PRESSURE_BACK:
        {
            ShaderDensityDisplay_.bindTexture(*TexPressureBack_)
                                 .setTransformation(CameraProjection)
                                 .draw(MeshFluidGridBuffer_);
            break;
        }
        case FluidBufferE::PRESSURE_FRONT:
        {
            ShaderDensityDisplay_.bindTexture(*TexPressureFront_)
                                 .setTransformation(CameraProjection)
                                 .draw(MeshFluidGridBuffer_);
            break;
        }
        case FluidBufferE::VELOCITIES_LOW_RES:
        {
            ShaderVelocityDisplay_.bindTexture(TexVelocitiesLowRes_)
                                  .setTransformation(CameraProjection)
                                  .draw(MeshFluidGridBuffer_);
            break;
        }
        case FluidBufferE::DENSITY_DIFFUSION_FRONT:
        {
            ShaderDensityDisplay_.bindTexture(*TexDensitiesFront_)
                                 .setTransformation(CameraProjection)
                                 .draw(MeshFluidGridBuffer_);
            break;
        }
        case FluidBufferE::DENSITY_DIFFUSION_BACK:
        {
            ShaderDensityDisplay_.bindTexture(*TexDensitiesBack_)
                                 .setTransformation(CameraProjection)
                                 .draw(MeshFluidGridBuffer_);
            break;
        }
        case FluidBufferE::GROUND_DISTORTED:
        {
            ShaderDensityDisplay_.bindTexture(TexGroundDistorted_)
                                 .setTransformation(CameraProjection)
                                 .draw(MeshFluidGridBuffer_);
            break;
        }
        case FluidBufferE::FINAL_COMPOSITION:
        {
            ShaderDensityDisplay_.bindTexture(TexFluidFinalComposition_)
                                 .setTransformation(CameraProjection)
                                 .draw(MeshFluidGridBuffer_);
            break;
        }
        default: break;
    }
}

void FluidGrid::init()
{
    VelocityReadbackFractionDataType VelData0_;
    VelocityReadbackFractionDataType VelData1_;
    PBOVelocity0_ = GL::BufferImage2D{PixelFormat::RG32F,
                                      {FLUID_GRID_SIZE_X >> VELOCITY_READBACK_SUBSAMPLE,
                                       FLUID_GRID_SIZE_Y >> (VELOCITY_READBACK_SUBSAMPLE+VELOCITY_READBACK_FRACTION_SIZE)},
                                       std::move(VelData0_), GL::BufferUsage::DynamicRead};
    PBOVelocity1_ = GL::BufferImage2D{PixelFormat::RG32F,
                                      {FLUID_GRID_SIZE_X >> VELOCITY_READBACK_SUBSAMPLE,
                                       FLUID_GRID_SIZE_Y >> (VELOCITY_READBACK_SUBSAMPLE+VELOCITY_READBACK_FRACTION_SIZE)},
                                       std::move(VelData1_), GL::BufferUsage::DynamicRead};
    PBOVelocityBack_ = &PBOVelocity0_;
    PBOVelocityFront_ = &PBOVelocity1_;

    TexBoundaries_ = GL::Texture2D{};
    TexDensityBase_ = GL::Texture2D{};
    TexDensitySources_ = GL::Texture2D{};
    TexDensities0_ = GL::Texture2D{};
    TexDensities1_ = GL::Texture2D{};
    TexFluidFinalComposition_ = GL::Texture2D{};
    TexGroundDistorted_ = GL::Texture2D{};
    TexPressure0_ = GL::Texture2D{};
    TexPressure1_ = GL::Texture2D{};
    TexVelocitySources_ = GL::Texture2D{};
    TexVelocities0_ = GL::Texture2D{};
    TexVelocities1_ = GL::Texture2D{};
    TexVelocitiesLowRes_ = GL::Texture2D{};
    TexVelocityDivergence_ = GL::Texture2D{};
    ShaderAdvect1d_ = AdvectShader(AdvectShaderVersionE::SCALAR);
    ShaderAdvect2d_ = AdvectShader(AdvectShaderVersionE::VECTOR);
    ShaderDensityDisplay_ = DensityDisplayShader();
    ShaderDensitySources_ = DensitySourcesShader();
    ShaderGroundDistortion_ = GroundDistortionShader();
    ShaderJacobi1d_ = JacobiShader(JacobiShaderVersionE::SCALAR);
    ShaderJacobi2d_ = JacobiShader(JacobiShaderVersionE::VECTOR);
    ShaderFluidFinalComposition_ = FluidFinalCompositionShader();
    ShaderSource1d_ = SourceShader{SourceShaderVersionE::SCALAR};
    ShaderSource2d_ = SourceShader{SourceShaderVersionE::VECTOR};
    ShaderVelocityDisplay_ = VelocityDisplayShader();
    ShaderVelocityDivergence_ = VelocityDivergenceShader();
    ShaderVelocityLowRes_ = Texture2D32FRender2D32FShader();
    ShaderVelocityGradientSubstraction_ = VelocityGradientSubstractionShader();
    ShaderVelocitySources_ = VelocitySourcesShader();
    
    assert(DensityBase_ != nullptr);
    assert(DensityBase_->size() == FLUID_GRID_ARRAY_SIZE);
    ImageView2D Image(PixelFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}, *DensityBase_);

    TexBoundaries_.setStorage(1, GL::TextureFormat::RG32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y})
                  .setMagnificationFilter(GL::SamplerFilter::Nearest);
    TexDensityBase_.setStorage(1, GL::TextureFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y})
                   .setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER)
                   .setSubImage(0, {}, Image);
    TexDensitySources_.setStorage(1, GL::TextureFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y})
                      .setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER);
    TexDensities0_.setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER)
                  .setStorage(1, GL::TextureFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
    TexDensities1_.setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER)
                  .setStorage(1, GL::TextureFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
    TexFluidFinalComposition_.setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER)
                             .setMinificationFilter(FLUID_GRID_DIFFUSION_FILTER, FLUID_GRID_DIFFUSION_FILTER_MIP_MAP)
                             .setWrapping(GL::SamplerWrapping::ClampToEdge)
                             .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
                             .setStorage(1, GL::TextureFormat::RGB32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
    TexGroundDistorted_.setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER)
                       .setMinificationFilter(FLUID_GRID_DIFFUSION_FILTER, FLUID_GRID_DIFFUSION_FILTER_MIP_MAP)
                       .setWrapping(GL::SamplerWrapping::ClampToEdge)
                       .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
                       .setStorage(1, GL::TextureFormat::RGB32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
    TexPressure0_.setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER)
                 .setStorage(1, GL::TextureFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
    TexPressure1_.setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER)
                 .setStorage(1, GL::TextureFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
    TexVelocitySources_.setStorage(1, GL::TextureFormat::RGB32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y})
                       .setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER);
    TexVelocities0_.setStorage(1, GL::TextureFormat::RG32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y})
                   .setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER);
    TexVelocities1_.setStorage(1, GL::TextureFormat::RG32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y})
                   .setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER);
    TexVelocitiesLowRes_.setStorage(1, GL::TextureFormat::RG32F,
                                    {FLUID_GRID_SIZE_X >> VELOCITY_READBACK_SUBSAMPLE,
                                     FLUID_GRID_SIZE_Y >> VELOCITY_READBACK_SUBSAMPLE})
                        .setMagnificationFilter(GL::SamplerFilter::Nearest);
    TexVelocityDivergence_.setStorage(1, GL::TextureFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y})
                          .setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER);
    
    TexDensitiesBack_   = &TexDensities0_;
    TexDensitiesFront_  = &TexDensities1_;
    TexPressureBack_    = &TexPressure0_;
    TexPressureFront_   = &TexPressure1_;
    TexVelocitiesBack_  = &TexVelocities0_;
    TexVelocitiesFront_ = &TexVelocities1_;

    FBOBoundaries_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBOBoundaries_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexBoundaries_, 0)
                  .clearColor(0, Color4(0.0f, 0.0f));
    FBODensitySources_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBODensitySources_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexDensitySources_, 0)
                      .clearColor(0, Color4(0.0f));
    FBODensities0_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBODensities0_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexDensities0_, 0)
                  .clearColor(0, Color4(0.0f));
    FBODensities1_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBODensities1_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexDensities1_, 0)
                  .clearColor(0, Color4(0.0f));
    FBOFluidFinalComposition_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBOFluidFinalComposition_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexFluidFinalComposition_, 0)
                             .clearColor(0, Color4(0.0f, 0.0f, 0.0f));
    FBOGroundDistorted_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBOGroundDistorted_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexGroundDistorted_, 0)
                       .clearColor(0, Color4(0.0f, 0.0f, 0.0f));
    FBOPressure0_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBOPressure0_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexPressure0_, 0)
                 .clearColor(0, Color4(0.0f));
    FBOPressure1_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBOPressure1_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexPressure1_, 0)
                 .clearColor(0, Color4(0.0f));
    FBOVelocitySources_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBOVelocitySources_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexVelocitySources_, 0)
                       .clearColor(0, Color4(0.0f, 0.0f, 0.0f));
    FBOVelocities0_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBOVelocities0_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexVelocities0_, 0)
                   .clearColor(0, Color4(0.0f, 0.0f));
    FBOVelocities1_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBOVelocities1_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexVelocities1_, 0)
                   .clearColor(0, Color4(0.0f, 0.0f));
    FBOVelocitiesLowRes_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X >> VELOCITY_READBACK_SUBSAMPLE,
                                                    FLUID_GRID_SIZE_Y >> VELOCITY_READBACK_SUBSAMPLE}}};
    FBOVelocitiesLowRes_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexVelocitiesLowRes_, 0)
                        .clearColor(0, Color4(0.0f, 0.0f));
    FBOVelocityDivergence_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
    FBOVelocityDivergence_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexVelocityDivergence_, 0)
                          .clearColor(0, Color4(0.0f, 0.0f));
    
    FBODensitiesBack_ = &FBODensities0_;
    FBODensitiesFront_ = &FBODensities1_;
    FBOPressureBack_ = &FBOPressure0_;
    FBOPressureFront_ = &FBOPressure1_;
    FBOVelocitiesBack_ = &FBOVelocities0_;
    FBOVelocitiesFront_ = &FBOVelocities1_;
    
    struct Vertex {
        Vector2 Pos;
        Vector2 Tex;
    };
    constexpr float x = WORLD_SIZE_DEFAULT_X*0.5f;
    constexpr float y = WORLD_SIZE_DEFAULT_Y*0.5f;
    Vertex Data[6]{
        {{-x, -y}, { 0.0f,  0.0f}},
        {{ x, -y}, { 1.0f,  0.0f}},
        {{-x,  y}, { 0.0f,  1.0f}},
        {{-x,  y}, { 0.0f,  1.0f}},
        {{ x, -y}, { 1.0f,  0.0f}},
        {{ x,  y}, { 1.0f,  1.0f}}
    };
    typedef GL::Attribute<0, Vector2> Position;
    typedef GL::Attribute<1, Vector2> TextureCoordinates;

    GL::Buffer Buffer;
    Buffer.setData(Data, GL::BufferUsage::StaticDraw);

    MeshFluidGridBuffer_ = GL::Mesh{};
    MeshFluidGridBuffer_.setCount(6)
                        .setPrimitive(GL::MeshPrimitive::Triangles)
                        .addVertexBuffer(Buffer, 0,
                                          Position{},
                                          TextureCoordinates{});
    
    ShaderAdvect1d_.setTransformation(Matrix3::projection({WORLD_SIZE_DEFAULT_X, WORLD_SIZE_DEFAULT_Y}))
                   .setAdvectionFactor(0.8f)
                   .setDeltaT(FLUID_TIMESTEP)
                   .setGridRes(FLUID_GRID_SIZE_X / WORLD_SIZE_DEFAULT_X)
                   .bindTextures(*TexDensitiesBack_, *TexVelocitiesBack_);
    ShaderAdvect2d_.setTransformation(Matrix3::projection({WORLD_SIZE_DEFAULT_X, WORLD_SIZE_DEFAULT_Y}))
                   .setAdvectionFactor(0.8f)
                   .setDeltaT(FLUID_TIMESTEP)
                   .setGridRes(FLUID_GRID_SIZE_X / WORLD_SIZE_DEFAULT_X)
                   .bindTextures(*TexVelocitiesBack_, *TexVelocitiesBack_);
    ShaderGroundDistortion_.setTransformation(Matrix3::projection({WORLD_SIZE_DEFAULT_X, WORLD_SIZE_DEFAULT_Y}))
                           .setDistortion(100.0f)
                           .setDeltaT(FLUID_TIMESTEP)
                           .setGridRes(FLUID_GRID_SIZE_X / WORLD_SIZE_DEFAULT_X)
                           .bindTextures(TexDensityBase_, *TexVelocitiesBack_);
    ShaderJacobi1d_.setTransformation(Matrix3::projection({WORLD_SIZE_DEFAULT_X, WORLD_SIZE_DEFAULT_Y}))
                   .setAlpha(this->getDiffusionAlpha())
                   .bindTextures(*TexDensitiesBack_, *TexDensitiesBack_);
    ShaderJacobi2d_.setTransformation(Matrix3::projection({WORLD_SIZE_DEFAULT_X, WORLD_SIZE_DEFAULT_Y}))
                   .setAlpha(this->getDiffusionAlpha())
                   .bindTextures(*TexDensitiesBack_, *TexDensitiesBack_);
    ShaderSource1d_.setTransformation(Matrix3::projection({WORLD_SIZE_DEFAULT_X, WORLD_SIZE_DEFAULT_Y}))
                   .bindTextures(*TexDensitiesBack_, TexDensitySources_);
    ShaderSource2d_.setTransformation(Matrix3::projection({WORLD_SIZE_DEFAULT_X, WORLD_SIZE_DEFAULT_Y}))
                   .bindTextures(*TexVelocitiesBack_, TexVelocitySources_);
    ShaderVelocityDivergence_.setTransformation(Matrix3::projection({WORLD_SIZE_DEFAULT_X, WORLD_SIZE_DEFAULT_Y}))
                             .bindTexture(*TexVelocitiesBack_);
    ShaderVelocityLowRes_.setTransformation(Matrix3::projection({WORLD_SIZE_DEFAULT_X, WORLD_SIZE_DEFAULT_Y}))
                         .bindTexture(TexVelocities0_);
    ShaderVelocityGradientSubstraction_.setTransformation(Matrix3::projection({WORLD_SIZE_DEFAULT_X, WORLD_SIZE_DEFAULT_Y}))
                                       .bindTextures(*TexVelocitiesBack_, TexVelocityDivergence_);
    ShaderDensitySources_.setTransformation(Matrix3::projection({WORLD_SIZE_DEFAULT_X, WORLD_SIZE_DEFAULT_Y}));
    ShaderVelocitySources_.setTransformation(Matrix3::projection({WORLD_SIZE_DEFAULT_X, WORLD_SIZE_DEFAULT_Y}));
    ShaderDensityDisplay_.bindTexture(*TexDensitiesFront_);
    ShaderVelocityDisplay_.setScale(20.0f)
                          .setShowOnlyMagnitude(false)
                          .bindTexture(*TexVelocitiesFront_);
    ShaderFluidFinalComposition_.setTransformation(Matrix3::projection({WORLD_SIZE_DEFAULT_X, WORLD_SIZE_DEFAULT_Y}))
                                .bindTextures(TexDensityBase_, *TexDensitiesFront_, TexGroundDistorted_, *TexVelocitiesBack_);
}

void FluidGrid::process(const double SimTime)
{
    //------------------------------------------------------------------
    // Toggle viewport for calculation
    //------------------------------------------------------------------
    //
    // For performance, the fluid field is only calculated every other
    // frame. To ensure a consistent load on the GPU, frames are not
    // skipped, but the fluid is calculated in an alternating manner.
    // On frame calculates the upper half, the next frame the lower half
    // of the fluid.
    //
    // Swap viewport on every call of this function
    // std::swap(ViewportCurrent, ViewportNext);

    // FBOVelocitiesFront_->setViewport(ViewportFull);
    // FBOVelocitiesBack_->setViewport(ViewportFull);
    static std::uint32_t c = 0;

    c++;

    if (c%2 == 0)
    {

    this->renderVelocitySources();
    // FBOVelocitiesFront_->setViewport(*ViewportCurrent);
    // FBOVelocitiesBack_->setViewport(*ViewportCurrent);
    this->diffuseVelocities();
    this->advectVelocities();

    // FBOPressureFront_->setViewport(*ViewportCurrent);
    // FBOPressureBack_->setViewport(*ViewportCurrent);
    // FBOVelocityDivergence_.setViewport(*ViewportCurrent);

    this->calculateDivergence();
    this->calculatePressure();
    this->substractPressureGradient();

    // FBODensitiesFront_->setViewport(ViewportFull);
    // FBODensitiesBack_->setViewport(ViewportFull);

    this->renderDensitySources();
    // FBODensitiesFront_->setViewport(*ViewportCurrent);
    // FBODensitiesBack_->setViewport(*ViewportCurrent);
    this->diffuseDensities();
    this->advectDensities();

    // FBOVelocitiesFront_->setViewport(ViewportFull);
    // FBOVelocitiesBack_->setViewport(ViewportFull);
    // FBODensitiesFront_->setViewport(*ViewportCurrent);
    // FBODensitiesBack_->setViewport(*ViewportNext);

    static int Fraction = 0;
    this->readbackVelocities(Fraction++, VELOCITY_READBACK_FRACTION_SIZE);
    if (Fraction == 1 << VELOCITY_READBACK_FRACTION_SIZE) Fraction = 0;

    DBLK(
            ImageView2D ImageVelReadback(PixelFormat::RG32F,
                                         {FLUID_GRID_SIZE_X >> VELOCITY_READBACK_SUBSAMPLE,
                                          FLUID_GRID_SIZE_Y >> VELOCITY_READBACK_SUBSAMPLE}, VelReadback_);
            TexVelocitiesLowRes_.setSubImage(0, {}, ImageVelReadback);
        )
    //------------------------------------------------------------------
    // Distort ground
    //------------------------------------------------------------------
    // FBOGroundDistorted_.setViewport(*ViewportCurrent);
    FBOGroundDistorted_.bind();
    ShaderGroundDistortion_.bindTextures(TexDensityBase_, *TexVelocitiesFront_)
                           .setTime(SimTime);

    ShaderGroundDistortion_.draw(MeshFluidGridBuffer_);




    //-----------------------------------------------------------------------
    // Final composition of density base, advected densities, and velocities
    //-----------------------------------------------------------------------
    FBOFluidFinalComposition_.setViewport(ViewportFull).bind();

    ShaderFluidFinalComposition_.bindTextures(TexDensityBase_,
                                              *TexDensitiesFront_,
                                              TexGroundDistorted_,
                                              *TexVelocitiesFront_);

    ShaderFluidFinalComposition_.draw(MeshFluidGridBuffer_);
    }
}

void FluidGrid::readbackVelocities(const int _Fraction, const int _SubDivisionBase2)
{
    //------------------------------------------------------------------
    // Readback velocities via PBO
    //------------------------------------------------------------------
    //
    // Data is transferred to PBO backbuffer, while PBO frontbuffer is
    // readback to client memory.
    //
    // The data represents only a subsampled velocity front buffer in
    // order to reduce frame rate. To further improve performance, in
    // each frame only a fraction of the buffer is transferred to the
    // PBO. This corresponds to updating the client side velocity field
    // with a lower frequency (one stripe each frame).
    //
    // Measurements:
    //
    //  - No read back: GPU 7, CPU 5
    //  - One PBO:  GPU 40, CPU 100
    //  - Two PBOs: GPU 40, CPU 50
    //  - Two PBOs+Backbuffer: GPU 40, CPU 50
    //  - Two PBOs+LowRes(1/4 x 1/4): GPU 14, CPU 25
    //  - Two PBOs+LowRes(1/4 x 1/4), 1/8 frequency: GPU 8, CPU 12
    //  - Two PBOs+LowRes(1/4 x 1/4), 1/8 frequency, parallel Box2D thread: GPU 8, CPU 7
    //
    // -----------------------------------------------------------------
    FBOVelocitiesLowRes_.bind();
    ShaderVelocityLowRes_.bindTexture(*TexVelocitiesFront_)
        .draw(MeshFluidGridBuffer_);

    const int FractionSize = FLUID_GRID_SIZE_Y >> (VELOCITY_READBACK_SUBSAMPLE+_SubDivisionBase2);
    Math::Range<2, int> Range = {{0, _Fraction*FractionSize},
                                 {FLUID_GRID_SIZE_X >> VELOCITY_READBACK_SUBSAMPLE, (_Fraction+1)*FractionSize}};
    // Transfer data to PBO back buffer
    *PBOVelocityBack_ = TexVelocitiesLowRes_.subImage(0, Range, {Magnum::PixelFormat::RG32F}, GL::BufferUsage::DynamicRead);

    // Transfer PBO front buffer to client memory
    // Since the front buffer PBO is delayed by one frame due PBO
    // flip-flopping, the position in the client side memory has to be
    // adjusted accordingly
    auto f = _Fraction-1; if (f==-1) f=(1 << VELOCITY_READBACK_FRACTION_SIZE) - 1;
    auto MapData = reinterpret_cast<const float*>(PBOVelocityFront_->buffer().mapRead());
    for (auto i=0; i<(FLUID_GRID_ARRAY_SIZE >> (VELOCITY_READBACK_SUBSAMPLE_XY+_SubDivisionBase2)); ++i)
    {
        VelReadback_[i+f*(FLUID_GRID_ARRAY_SIZE >> (VELOCITY_READBACK_SUBSAMPLE_XY+_SubDivisionBase2))] = MapData[i];
    }

    CORRADE_INTERNAL_ASSERT_OUTPUT(PBOVelocityFront_->buffer().unmap());

    std::swap(PBOVelocityBack_, PBOVelocityFront_);
}

void FluidGrid::advectDensities()
{
    std::swap(FBODensitiesFront_, FBODensitiesBack_);
    std::swap(TexDensitiesFront_, TexDensitiesBack_);

    FBODensitiesFront_->bind();
    ShaderAdvect1d_.bindTextures(*TexDensitiesBack_, *TexVelocitiesFront_)
                   .draw(MeshFluidGridBuffer_);
}

void FluidGrid::advectVelocities()
{
    std::swap(FBOVelocitiesFront_, FBOVelocitiesBack_);
    std::swap(TexVelocitiesFront_, TexVelocitiesBack_);

    FBOVelocitiesFront_->bind();
    ShaderAdvect2d_.bindTextures(*TexVelocitiesBack_, *TexVelocitiesBack_)
                   .draw(MeshFluidGridBuffer_);
}

void FluidGrid::calculatePressure()
{
    FBOPressureFront_->clearColor(0, Color4(0.0f));
    FBOPressureBack_->clearColor(0, Color4(0.0f));
    ShaderJacobi1d_.setAlpha(0.25f)//this->getPressureEquationAlpha())
                   .setRBeta(0.25f);
    for (int i=0; i<Config_.IterationsPressureEquation_; ++i)
    {    
        std::swap(FBOPressureFront_, FBOPressureBack_);
        std::swap(TexPressureFront_, TexPressureBack_);

        FBOPressureFront_->bind();
        ShaderJacobi1d_.bindTextures(*TexPressureBack_, TexVelocityDivergence_)
                       .draw(MeshFluidGridBuffer_);
    }
}

void FluidGrid::substractPressureGradient()
{
    std::swap(FBOVelocitiesFront_, FBOVelocitiesBack_);
    std::swap(TexVelocitiesFront_, TexVelocitiesBack_);

    FBOVelocitiesFront_->bind();
    ShaderVelocityGradientSubstraction_.bindTextures(*TexVelocitiesBack_, *TexPressureFront_);
    ShaderVelocityGradientSubstraction_.draw(MeshFluidGridBuffer_);
}

void FluidGrid::calculateDivergence()
{
    FBOVelocityDivergence_.bind();
    ShaderVelocityDivergence_.bindTexture(*TexVelocitiesFront_);
    ShaderVelocityDivergence_.draw(MeshFluidGridBuffer_);
}

void FluidGrid::diffuseDensities()
{
    ShaderJacobi1d_.setAlpha(this->getDiffusionAlpha())
                   .setRBeta(1.0f/(4.0f+this->getDiffusionAlpha()));
    for (int i=0; i<Config_.IterationsDensityDiffusion_; ++i)
    {
        std::swap(FBODensitiesFront_, FBODensitiesBack_);
        std::swap(TexDensitiesFront_, TexDensitiesBack_);

        FBODensitiesFront_->bind();
        ShaderJacobi1d_.bindTextures(*TexDensitiesBack_, *TexDensitiesBack_)
                       .draw(MeshFluidGridBuffer_);
    }
}

void FluidGrid::diffuseVelocities()
{
    ShaderJacobi2d_.setAlpha(this->getDiffusionAlpha())
                   .setRBeta(1.0f/(4.0f+this->getDiffusionAlpha()));
    for (int i=0; i<Config_.IterationsVelocityDiffusion_; ++i)
    {
        std::swap(FBOVelocitiesFront_, FBOVelocitiesBack_);
        std::swap(TexVelocitiesFront_, TexVelocitiesBack_);

        FBOVelocitiesFront_->bind();
        ShaderJacobi2d_.bindTextures(*TexVelocitiesBack_, *TexVelocitiesBack_)
                       .draw(MeshFluidGridBuffer_);
    }
}

void FluidGrid::renderDensitySources()
{
    //------------------------------------------------------------------
    // Create mesh representing density sources
    //------------------------------------------------------------------
    GL::Buffer DensitySourcesBufferPoints;
    DensitySourcesBufferPoints.setData(DensitySourcesPoints_, GL::BufferUsage::StreamDraw);

    GL::Mesh MeshDensitySourcesPoints;
    MeshDensitySourcesPoints.setCount(DensitySourcesPoints_.size()/3)
                            .setPrimitive(GL::MeshPrimitive::Points)
                            .addVertexBuffer(std::move(DensitySourcesBufferPoints), 0,
                                             DensitySourcesShader::Position{},
                                             DensitySourcesShader::Amount{});

    GL::Buffer DensitySourcesBufferLines;
    DensitySourcesBufferLines.setData(DensitySourcesLines_, GL::BufferUsage::StreamDraw);

    GL::Mesh MeshDensitySourcesLines;
    MeshDensitySourcesLines.setCount(DensitySourcesLines_.size()/3)
                           .setPrimitive(GL::MeshPrimitive::Lines)
                           .addVertexBuffer(std::move(DensitySourcesBufferLines), 0,
                                            DensitySourcesShader::Position{},
                                            DensitySourcesShader::Amount{});

    DensitySourcesPoints_.clear();
    DensitySourcesLines_.clear();

    //------------------------------------------------------------------
    // Render sources to texture
    //------------------------------------------------------------------
    FBODensitySources_.clearColor(0, Color4(0.0f))
                      .bind();
    ShaderDensitySources_.draw(MeshDensitySourcesPoints);
    ShaderDensitySources_.draw(MeshDensitySourcesLines);

    //------------------------------------------------------------------
    // Add sources to density field
    //------------------------------------------------------------------
    std::swap(FBODensitiesFront_, FBODensitiesBack_);
    std::swap(TexDensitiesFront_, TexDensitiesBack_);

    FBODensitiesFront_->bind();
    ShaderSource1d_.bindTextures(*TexDensitiesBack_, TexDensitySources_)
                   .draw(MeshFluidGridBuffer_);
}

void FluidGrid::renderVelocitySources()
{
    //------------------------------------------------------------------
    // Create mesh representing velocity sources
    //------------------------------------------------------------------
    GL::Buffer VelocitySourcesBufferPoints;
    VelocitySourcesBufferPoints.setData(VelocitySourcesPoints_, GL::BufferUsage::StreamDraw);

    GL::Mesh MeshVelocitySourcesPoints;
    MeshVelocitySourcesPoints.setCount(VelocitySourcesPoints_.size()/5)
                             .setPrimitive(GL::MeshPrimitive::Points)
                             .addVertexBuffer(std::move(VelocitySourcesBufferPoints), 0,
                                              VelocitySourcesShader::Position{},
                                              VelocitySourcesShader::Velocity{},
                                              VelocitySourcesShader::Weight{});

    GL::Buffer VelocitySourcesBufferLines;
    VelocitySourcesBufferLines.setData(VelocitySourcesLines_, GL::BufferUsage::StreamDraw);

    GL::Mesh MeshVelocitySourcesLines;
    MeshVelocitySourcesLines.setCount(VelocitySourcesLines_.size()/5)
                            .setPrimitive(GL::MeshPrimitive::Lines)
                            .addVertexBuffer(std::move(VelocitySourcesBufferLines), 0,
                                             VelocitySourcesShader::Position{},
                                             VelocitySourcesShader::Velocity{},
                                             VelocitySourcesShader::Weight{});

    VelocitySourcesPoints_.clear();
    VelocitySourcesLines_.clear();

    //------------------------------------------------------------------
    // Render sources to texture
    //------------------------------------------------------------------
    FBOVelocitySources_.clearColor(0, Color4(0.0f, 0.0f, 0.0f))
                       .bind();
    ShaderVelocitySources_.draw(MeshVelocitySourcesPoints);
    ShaderVelocitySources_.draw(MeshVelocitySourcesLines);

    //------------------------------------------------------------------
    // Add sources to density field
    //------------------------------------------------------------------
    std::swap(FBOVelocitiesFront_, FBOVelocitiesBack_);
    std::swap(TexVelocitiesFront_, TexVelocitiesBack_);

    FBOVelocitiesFront_->bind();
    ShaderSource2d_.bindTextures(*TexVelocitiesBack_, TexVelocitySources_)
                   .draw(MeshFluidGridBuffer_);
}
