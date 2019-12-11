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
#include "velocity_display_shader.h"
#include "velocity_processing_shader.h"
#include "velocity_sources_shader.h"
#include "world_def.h"

using namespace Magnum;

enum class FluidBufferE : int
{
    DENSITY_SOURCES = 0,
    DENSITY_BASE = 1,
    VELOCITY_SOURCES = 2,
    VELOCITIES_FRONT = 3,
    VELOCITIES_BACK = 4,
    DENSITY_DIFFUSION_FRONT = 5,
    DENSITY_DIFFUSION_BACK = 6,
    FINAL_COMPOSITION = 7
};

class FluidGrid
{
    public:
        
        FluidGrid& addDensity(const float x, const float y, const float d);
        FluidGrid& addVelocity(const float x, const float y, const float Vx, const float Vy);
        FluidGrid& setDensityBase(std::vector<float>* const DensityBase);
        
        void display(const Matrix3 CameraProjection,
                     const FluidBufferE Buffer = FluidBufferE::FINAL_COMPOSITION);
        void init();
        void process();
        
    private:
        
        int I(const int x, const int y) const {return (y << FLUID_GRID_SIZE_X_BITS) + x;}
        
        GL::Framebuffer FBODensities_{NoCreate};
        GL::Framebuffer* FBODiffusionBack_{nullptr};
        GL::Framebuffer* FBODiffusionFront_{nullptr};
        GL::Framebuffer FBODiffusion0_{NoCreate};
        GL::Framebuffer FBODiffusion1_{NoCreate};
        GL::Framebuffer FBOVelocitySources_{NoCreate};
        GL::Framebuffer* FBOVelocitiesBack_{nullptr};
        GL::Framebuffer* FBOVelocitiesFront_{nullptr};
        GL::Framebuffer FBOVelocities0_{NoCreate};
        GL::Framebuffer FBOVelocities1_{NoCreate};
        
        DensitySourcesShader ShaderDensitySources_{NoCreate};
        DensityShader ShaderDensityDisplay_{NoCreate};
        DiffusionShader ShaderDiffusion_{NoCreate};
        VelocitySourcesShader ShaderVelocitySources_{NoCreate};
        VelocityProcessingShader ShaderVelocityProcessing_{NoCreate};
        VelocityDisplayShader ShaderVelocityDisplay_{NoCreate};
        GL::Mesh Mesh_{NoCreate};
        GL::Mesh MeshDensityDisplay_{NoCreate};
        GL::Mesh MeshVelocities_{NoCreate};
        GL::Texture2D TexDensityBase_{NoCreate};        // Heightmap
        GL::Texture2D TexDensities_{NoCreate};          // Density sources
        GL::Texture2D TexDiffusionBack_{NoCreate};      // Diffusion buffer
        GL::Texture2D TexDiffusionFront_{NoCreate};     // Diffusion buffer
        GL::Texture2D TexVelocitySources_{NoCreate};    // Velocity sources
        GL::Texture2D TexVelocitiesBack_{NoCreate};     // Velocities back buffer
        GL::Texture2D TexVelocitiesFront_{NoCreate};    // Velocities front buffer
        float Viscosity_ = 1.0f;
        
        std::vector<float>* DensityBase_{nullptr};
        std::vector<float> DensitySources_;
        std::vector<float> VelocitySources_;
        
};

#endif // FLUID_GRID_H
