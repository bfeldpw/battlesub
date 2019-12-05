#ifndef FLUID_GRID_H
#define FLUID_GRID_H

#include <algorithm>
#include <vector>

// #include <Magnum/GL/BufferImage.h>
#include <Magnum/GL/Framebuffer.h>
// #include <Magnum/GL/Texture.h>
// #include <Magnum/GL/TextureFormat.h>
// #include <Magnum/Image.h>

#include "density_shader.h"
#include "density_sources_shader.h"
#include "diffusion_shader.h"
#include "drawable_generic.h"
#include "global_resources.h"
#include "velocity_sources_shader.h"
#include "world_def.h"

using namespace Magnum;

class FluidGrid
{
    public:
        
        FluidGrid& addDensity(const float x, const float y, const float d);
        FluidGrid& addVelocity(const float x, const float y, const float Vx, const float Vy);
        FluidGrid& setDensityBase(std::vector<float>* const DensityBase);
        
        void display(const Matrix3 CameraProjection);
        void init();
        void process();
        
    private:
        
        int I(const int x, const int y) const {return (y << FLUID_GRID_SIZE_X_BITS) + x;}
        
        GL::Framebuffer FBODensities_{NoCreate};
        GL::Framebuffer* FBODiffusionBack_{nullptr};
        GL::Framebuffer* FBODiffusionFront_{nullptr};
        GL::Framebuffer FBODiffusion0_{NoCreate};
        GL::Framebuffer FBODiffusion1_{NoCreate};
        GL::Framebuffer FBOVelocities_{NoCreate};
        
        DensitySourcesShader ShaderDensitySources_{NoCreate};
        DensityShader ShaderDensityDisplay_{NoCreate};
        DiffusionShader ShaderDiffusion_{NoCreate};
        VelocitySourcesShader ShaderVelocitySources_{NoCreate};
        GL::Mesh Mesh_{NoCreate};
        GL::Mesh MeshDensityDisplay_{NoCreate};
        GL::Texture2D TexDensityBase_{NoCreate};    // Heightmap
        GL::Texture2D TexDensities_{NoCreate};      // Density sources
        GL::Texture2D TexDiffusionBack_{NoCreate};  // Diffusion buffer
        GL::Texture2D TexDiffusionFront_{NoCreate}; // Diffusion buffer
        GL::Texture2D TexVelocities_{NoCreate};     // Velocity sources
        float Viscosity_ = 1.0f;
        
        std::vector<float>* DensityBase_{nullptr};
        std::vector<float> DensitySources_;
        std::vector<float> VelocitySources_;
        
};

#endif // FLUID_GRID_H
