#ifndef FLUID_GRID_H
#define FLUID_GRID_H

#include <algorithm>
#include <vector>

#include <Magnum/GL/BufferImage.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Image.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>

#include "density_shader.h"
#include "drawable_generic.h"
#include "global_resources.h"
#include "world_def.h"

using namespace Magnum;

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
        
        void init(const Matrix3 CameraProjection)
        {
            static bool FirstUse = true;
            
            if (FirstUse)
            {
                DensityTexture_ = GL::Texture2D{};
                Shader_ = DensityShader();
                Shader_.setColor({1.0f, 1.0f, 1.0f});
                
                ImageView2D Image(PixelFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}, *Density_);
                
                DensityTexture_.setMagnificationFilter(GL::SamplerFilter::Linear)
                       .setMinificationFilter(GL::SamplerFilter::Linear, GL::SamplerMipmap::Linear)
                       .setWrapping(GL::SamplerWrapping::ClampToEdge)
                       .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
                       .setImage(0, GL::TextureFormat::R32F, Image)
                       .generateMipmap();
                
                
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
                     .addVertexBuffer(std::move(Buffer), 0,
                                      DensityShader::Position{},
                                      DensityShader::TextureCoordinates{});
            
                Shader_.bindTexture(DensityTexture_);
                
                FirstUse = false;
            }
            this->lin_solve(10000.0f, 1.0f+50000.0f, 1);
            
            ImageView2D Image(PixelFormat::R32F, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}, *Density_);
            DensityTexture_.setImage(0, GL::TextureFormat::R32F, Image)
                           .generateMipmap();

            Shader_.setTransformation(CameraProjection);

            Mesh_.draw(Shader_);
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
        
        DensityShader Shader_{NoCreate};
        GL::Mesh Mesh_{NoCreate};
        GL::Texture2D DensityTexture_{NoCreate};
        float Viscosity_ = 1.0f;
        
        std::vector<float>* Density_;
        std::vector<float>* DensityB_;
        std::vector<float> Density0_;
        std::vector<float> Density1_;
        std::vector<float> VelocityX_;
        std::vector<float> VelocityY_;
        
};

#endif // FLUID_GRID_H
