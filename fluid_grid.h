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
#include "diffusion_shader.h"
#include "drawable_generic.h"
#include "global_resources.h"
#include "world_def.h"

using namespace Magnum;

constexpr GL::SamplerFilter FLUID_GRID_DIFFUSION_FILTER = GL::SamplerFilter::Linear;
constexpr GL::SamplerMipmap FLUID_GRID_DIFFUSION_FILTER_MIP_MAP = GL::SamplerMipmap::Base;

constexpr int FLUID_GRID_SIZE_X_BITS = 10;
constexpr int FLUID_GRID_SIZE_X = 1 << FLUID_GRID_SIZE_X_BITS;
constexpr int FLUID_GRID_SIZE_Y = 1 << (FLUID_GRID_SIZE_X_BITS-1);
constexpr int FLUID_GRID_ARRAY_SIZE = FLUID_GRID_SIZE_X * FLUID_GRID_SIZE_Y;

class FluidGrid
{
    public:
        
        FluidGrid()
        {
            Density0_.resize(FLUID_GRID_ARRAY_SIZE);
            Density1_.resize(FLUID_GRID_ARRAY_SIZE);
            VelocityX_.resize(FLUID_GRID_ARRAY_SIZE);
            VelocityY_.resize(FLUID_GRID_ARRAY_SIZE);
            Density_ = &Density0_;
            DensityB_ = &Density1_;
            std::fill(Density0_.begin(), Density0_.end(), 0.1f);
            std::fill(Density1_.begin(), Density1_.end(), 0.1f);
            std::fill(VelocityX_.begin(), VelocityX_.end(), 0);
            std::fill(VelocityY_.begin(), VelocityY_.end(), 0);
        }
        
        void addDensity(const int x, const int y, const float d)
        {
            (*Density_)[I(x, y)] += d;
        }
        
        void changeVelocity(const int x, const int y, const float Vx, const float Vy)
        {
            VelocityX_[I(x, y)] += Vx;
            VelocityY_[I(x, y)] += Vy;
        }
        
        void drawDiffusion(const Matrix3 CameraProjection)
        {
            ShaderDiffusion_.setTransformation(CameraProjection);
            MeshDiffusion_.draw(ShaderDiffusion_);
        }
        
        void process()
        {
            static bool FirstUse = true;
            
            if (FirstUse)
            {
                TexDensitySrcs_ = GL::Texture2D{};
                TexDiffusion_ = GL::Texture2D{};
                Shader_ = DiffusionShader();
                ShaderDiffusion_ = DensityShader();
                
                ImageView2D Image(PixelFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}, *Density_);
                
                TexDensitySrcs_.setMagnificationFilter(GL::SamplerFilter::Nearest)
                               .setMinificationFilter(GL::SamplerFilter::Nearest, GL::SamplerMipmap::Nearest)
                               .setStorage(1, GL::TextureFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y})
                               .setSubImage(0, {}, Image);
                TexDiffusion_.setMagnificationFilter(FLUID_GRID_DIFFUSION_FILTER)
                             .setMinificationFilter(FLUID_GRID_DIFFUSION_FILTER, FLUID_GRID_DIFFUSION_FILTER_MIP_MAP)
                             .setWrapping(GL::SamplerWrapping::ClampToEdge)
                             .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
                             .setStorage(Math::log2(FLUID_GRID_SIZE_X)+1, GL::TextureFormat::RGBA8, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y})
                             .generateMipmap();

                FBODiffusion_ = GL::Framebuffer{{{0, 0},{FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}}};
                FBODiffusion_.attachTexture(GL::Framebuffer::ColorAttachment{0}, TexDiffusion_, 0);
                
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
                     
                MeshDiffusion_ = GL::Mesh{};
                MeshDiffusion_.setCount(6)
                              .setPrimitive(GL::MeshPrimitive::Triangles)
                              .addVertexBuffer(std::move(Buffer), 0,
                                               DensityShader::Position{},
                                               DensityShader::TextureCoordinates{});
            
                Shader_.setTransformation(Matrix3::projection({FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y})) 
                       .bindTextures(TexDensitySrcs_, TexDiffusion_);
                
                
                FirstUse = false;
            }
            this->lin_solve(1000.0f, 1.0f+5000.0f, 1);
            
            ImageView2D Image(PixelFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}, *Density_);
            TexDensitySrcs_.setSubImage(0, {}, Image);
            
            FBODiffusion_.clear(GL::FramebufferClear::Color);
            FBODiffusion_.bind();
            Shader_.bindTextures(TexDensitySrcs_, TexDiffusion_);
            Mesh_.draw(Shader_);
            ShaderDiffusion_.bindTexture(TexDiffusion_);
            GL::defaultFramebuffer.bind();
        }
        
        void lin_solve(float a, float c, int iter)
        {
            std::swap(Density_, DensityB_);
            float cRecip = 1.0f / c;
            for (int k = 0; k < iter; k++)
            {
                for (int j = 1; j < FLUID_GRID_SIZE_Y - 1; j++)
                {
                    for (int i = 1; i < FLUID_GRID_SIZE_X - 1; i++)
                    {
                        (*Density_)[I(i, j)] = (0.1f +
                                      a*((*DensityB_)[I(i, j)]
                                       + (*DensityB_)[I(i-1, j)]
                                       + (*DensityB_)[I(i+1, j)]
                                       + (*DensityB_)[I(i  , j+1)]
                                       + (*DensityB_)[I(i  , j-1)]
                                      )) * cRecip;
                    }
                }
            }
        }
        
    private:
        
        int I(const int x, const int y) const {return (y << FLUID_GRID_SIZE_X_BITS) + x;}
        
        std::mt19937 Generator_;
        
        GL::Framebuffer FBODiffusion_{NoCreate};
        
        DensityShader ShaderDiffusion_{NoCreate};
        DiffusionShader Shader_{NoCreate};
        GL::Mesh Mesh_{NoCreate};
        GL::Mesh MeshDiffusion_{NoCreate};
        GL::Texture2D TexDensitySrcs_{NoCreate};
        GL::Texture2D TexDiffusion_{NoCreate};
        float Viscosity_ = 1.0f;
        
        std::vector<float>* Density_;
        std::vector<float>* DensityB_;
        std::vector<float> Density0_;
        std::vector<float> Density1_;
        std::vector<float> VelocityX_;
        std::vector<float> VelocityY_;
        
};

#endif // FLUID_GRID_H
