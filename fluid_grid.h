#ifndef FLUID_GRID_H
#define FLUID_GRID_H

#include <algorithm>

#include <Magnum/GL/BufferImage.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Image.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>

#include "density_shader.h"
#include "drawable_generic.h"
#include "global_resources.h"

using namespace Magnum;

constexpr int FLUID_GRID_SIZE_X_BITS = 9;
constexpr int FLUID_GRID_SIZE_X = 1 << FLUID_GRID_SIZE_X_BITS;
constexpr int FLUID_GRID_SIZE_Y = 1 << (FLUID_GRID_SIZE_X_BITS-1);
constexpr int FLUID_GRID_ARRAY_SIZE = FLUID_GRID_SIZE_X * FLUID_GRID_SIZE_Y;

class FluidGrid
{
    public:
        
        FluidGrid()
        {
            std::fill(Density_.begin(), Density_.end(), 64/*255*256*256*256+25*256*256*/);
            std::fill(VelocityX_.begin(), VelocityX_.end(), 0);
            std::fill(VelocityY_.begin(), VelocityY_.end(), 0);
        }
        
        void changeDensity(const int x, const int y, const float d)
        {
            Density_[I(x, y)] += d;
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

                std::poisson_distribution<std::uint8_t> Dist(64);
                for (auto y=0u; y<FLUID_GRID_SIZE_Y; ++y)
                {
                    for (auto x=0u; x<FLUID_GRID_SIZE_X; ++x)
                    {
                        Density_[I(x, y)] = Dist(Generator_)/**256*256 + 255*256*256*256*/;
                    }
                }
                
                ImageView2D Image(PixelFormat::R8Unorm, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}, Density_);
                
                DensityTexture_.setMagnificationFilter(GL::SamplerFilter::Linear)
                       .setMinificationFilter(GL::SamplerFilter::Linear, GL::SamplerMipmap::Linear)
                       .setWrapping(GL::SamplerWrapping::ClampToEdge)
                       .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
                       .setImage(0, GL::TextureFormat::RGBA8, Image)
                       .generateMipmap();
                
                
                struct Vertex {
                    Vector2 Pos;
                    Vector2 Tex;
                };
                Vertex Data[6]{
                    {{-600.0f, -400.0f}, { 0.0f,  0.0f}},
                    {{ 600.0f, -400.0f}, { 1.0f,  0.0f}},
                    {{-600.0f,  400.0f}, { 0.0f,  1.0f}},
                    {{-600.0f,  400.0f}, { 0.0f,  1.0f}},
                    {{ 600.0f, -400.0f}, { 1.0f,  0.0f}},
                    {{ 600.0f,  400.0f}, { 1.0f,  1.0f}}
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
            
//             for (auto y=0u; y<FLUID_GRID_SIZE_Y; ++y)
//             {
//                 for (auto x=0u; x<FLUID_GRID_SIZE_X; ++x)
//                 {
//                     Density_[I(x, y)] *= 1.1;
//                 }
//             }
            this->lin_solve(0.2f, 0, 1);
            
            ImageView2D Image(PixelFormat::R8Unorm, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}, Density_);
            DensityTexture_.setImage(0, GL::TextureFormat::RGBA8, Image);

            Shader_.setTransformation(CameraProjection);

            Mesh_.draw(Shader_);
        }
        
        void lin_solve(float a, float c, int iter)
        {
//             float cRecip = 1.0f / c;
//             for (int k = 0; k < iter; k++)
//             {
//                 for (int j = 1; j < FLUID_GRID_SIZE_Y - 1; j++)
//                 {
//                     for (int i = 1; i < FLUID_GRID_SIZE_X - 1; i++)
//                     {
//                         Density_[I(i, j)] = 0.01 *
//                                   ((    96.0 * double(Density_[I(i  , j)]) 
//                                        + 1.0 * double(Density_[I(i+1, j)])
//                                        + 1.0 * double(Density_[I(i-1, j)])
//                                        + 1.0 * double(Density_[I(i  , j+1)])
//                                        + 1.0 * double(Density_[I(i  , j-1)])
//                                     ))/** cRecip*/;
//                     }
//                 }
//             }
        }
        
    private:
        
        int I(const int x, const int y) const {return (y << FLUID_GRID_SIZE_X_BITS) + x;}
        
        std::mt19937 Generator_;
        
        DensityShader Shader_{NoCreate};
        GL::Mesh Mesh_{NoCreate};
        GL::Texture2D DensityTexture_{NoCreate};
        float Viscosity_ = 1.0f;
        
        std::array<std::uint8_t, FLUID_GRID_ARRAY_SIZE> Density_;
        std::array<float, FLUID_GRID_ARRAY_SIZE> VelocityX_;
        std::array<float, FLUID_GRID_ARRAY_SIZE> VelocityY_;
        
};

#endif // FLUID_GRID_H
