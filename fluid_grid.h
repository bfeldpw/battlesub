#ifndef FLUID_GRID_H
#define FLUID_GRID_H

#include <algorithm>
#include <vector>

#include <Magnum/GL/BufferImage.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Image.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>

#include "density_shader.h"
#include "density_sources_shader.h"
#include "diffusion_shader.h"
#include "drawable_generic.h"
#include "global_resources.h"
#include "world_def.h"

using namespace Magnum;

constexpr GL::SamplerFilter FLUID_GRID_DIFFUSION_FILTER = GL::SamplerFilter::Linear;
constexpr GL::SamplerMipmap FLUID_GRID_DIFFUSION_FILTER_MIP_MAP = GL::SamplerMipmap::Base;

constexpr int FLUID_GRID_SIZE_X_BITS = 11;
constexpr int FLUID_GRID_SIZE_X = 1 << FLUID_GRID_SIZE_X_BITS;
constexpr int FLUID_GRID_SIZE_Y = 1 << (FLUID_GRID_SIZE_X_BITS-1);
constexpr int FLUID_GRID_ARRAY_SIZE = FLUID_GRID_SIZE_X * FLUID_GRID_SIZE_Y;

class FluidGrid
{
    public:
        
        void addDensity(const float x, const float y, const float d)
        {
            DensitySources_.push_back(x);
            DensitySources_.push_back(y);
            DensitySources_.push_back(d);
        }
        
        void changeVelocity(const int x, const int y, const float Vx, const float Vy)
        {
//             VelocityX_[I(x, y)] += Vx;
//             VelocityY_[I(x, y)] += Vy;
        }
        
        void drawDiffusion(const Matrix3 CameraProjection)
        {
            ShaderDensityDisplay_.setTransformation(CameraProjection);
            MeshDensityDisplay_.draw(ShaderDensityDisplay_);
        }
        
        void process()
        {
            static bool FirstUse = true;
            static bool BackBuffer = true;
            
            if (FirstUse)
            {
                TexDensities_ = GL::Texture2D{};
                TexDiffusionBack_ = GL::Texture2D{};
                TexDiffusionFront_ = GL::Texture2D{};
                ShaderDiffusion_ = DiffusionShader();
                ShaderDensityDisplay_ = DensityShader();
                ShaderDensitySources_ = DensitySourcesShader();
                
                TexDensities_.setMagnificationFilter(GL::SamplerFilter::Nearest)
                             .setMinificationFilter(GL::SamplerFilter::Nearest, GL::SamplerMipmap::Nearest)
                             .setStorage(1, GL::TextureFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
                TexDiffusionBack_.setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER)
                                 .setMinificationFilter(FLUID_GRID_DIFFUSION_FILTER, FLUID_GRID_DIFFUSION_FILTER_MIP_MAP)
                                 .setWrapping(GL::SamplerWrapping::ClampToEdge)
                                 .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
                                 .setStorage(1/*Math::log2(FLUID_GRID_SIZE_X)+1*/, GL::TextureFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
                               //.generateMipmap();
                TexDiffusionFront_.setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER)
                               .setMinificationFilter(FLUID_GRID_DIFFUSION_FILTER, FLUID_GRID_DIFFUSION_FILTER_MIP_MAP)
                               .setWrapping(GL::SamplerWrapping::ClampToEdge)
                               .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
                               .setStorage(1/*Math::log2(FLUID_GRID_SIZE_X)+1*/, GL::TextureFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y});
                             //.generateMipmap();
                              
                FBODensities_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
                FBODensities_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexDensities_, 0);
                FBODiffusion0_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
                FBODiffusion0_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexDiffusionBack_, 0);
                FBODiffusion1_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
                FBODiffusion1_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexDiffusionFront_, 0);
                
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
                                .bindTextures(TexDensities_, TexDiffusionBack_);
                       
                ShaderDensitySources_.setTransformation(Matrix3::projection({WORLD_SIZE_X, WORLD_SIZE_Y}));
                ShaderDensityDisplay_.bindTexture(TexDiffusionFront_);
                FirstUse = false;
            }
            
            //------------------------------------------------------------------
            // Create mesh (points) representing density sources
            //------------------------------------------------------------------
            GL::Buffer DensitySourcesBuffer;
            DensitySourcesBuffer.setData(DensitySources_, GL::BufferUsage::StreamDraw);
            
            GL::Mesh Mesh;
            Mesh.setCount(DensitySources_.size()/3)
                .setPrimitive(GL::MeshPrimitive::Points)
                .addVertexBuffer(std::move(DensitySourcesBuffer), 0,
                                 DensitySourcesShader::Position{},
                                 DensitySourcesShader::Amount{});
                
//             for (auto DS : DensitySources_)
//                 std::cout << DS << ", ";
//             std::cout << std::endl;
            DensitySources_.clear();
            
//             this->lin_solve(1000.0f, 1.0f+5000.0f, 1);
            
//             FBODiffusionBack_.clear(GL::FramebufferClear::Color);

            //------------------------------------------------------------------
            // Draw densities
            //------------------------------------------------------------------
            FBODensities_.clear(GL::FramebufferClear::Color)
                         .bind();
            Mesh.draw(ShaderDensitySources_);
            
            //------------------------------------------------------------------
            // Process diffusion
            //------------------------------------------------------------------
            FBODiffusionFront_->bind();
            
            if (BackBuffer) ShaderDiffusion_.bindTextures(TexDensities_, TexDiffusionBack_);
            else ShaderDiffusion_.bindTextures(TexDensities_, TexDiffusionFront_);
            BackBuffer ^= 1;
            
            Mesh_.draw(ShaderDiffusion_);
            
            GL::defaultFramebuffer.bind();
            ShaderDensityDisplay_.bindTexture(TexDiffusionFront_);
            
            std::swap(FBODiffusionFront_, FBODiffusionBack_);
        }
        
    private:
        
        int I(const int x, const int y) const {return (y << FLUID_GRID_SIZE_X_BITS) + x;}
        
        std::mt19937 Generator_;
        
        GL::Framebuffer FBODensities_{NoCreate};
        GL::Framebuffer* FBODiffusionBack_{nullptr};
        GL::Framebuffer* FBODiffusionFront_{nullptr};
        GL::Framebuffer FBODiffusion0_{NoCreate};
        GL::Framebuffer FBODiffusion1_{NoCreate};
        
        DensitySourcesShader ShaderDensitySources_{NoCreate};
        DensityShader ShaderDensityDisplay_{NoCreate};
        DiffusionShader ShaderDiffusion_{NoCreate};
        GL::Mesh Mesh_{NoCreate};
        GL::Mesh MeshDensityDisplay_{NoCreate};
        GL::Texture2D TexDensities_{NoCreate};
        GL::Texture2D TexDiffusionBack_{NoCreate};
        GL::Texture2D TexDiffusionFront_{NoCreate};
        float Viscosity_ = 1.0f;
        
        std::vector<float> DensitySources_;
        
};

#endif // FLUID_GRID_H
