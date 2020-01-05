#ifndef FLUID_GRID_H
#define FLUID_GRID_H

#include <algorithm>
#include <vector>

#include <Magnum/GL/Framebuffer.h>

#include "density_advection_shader.h"
#include "density_diffusion_shader.h"
#include "density_display_shader.h"
#include "density_sources_shader.h"
#include "drawable_generic.h"
#include "global_resources.h"
#include "velocity_advection_shader.h"
#include "velocity_diffusion_shader.h"
#include "velocity_display_shader.h"
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
        FluidGrid& setVelocityAdvectionFactor(const float f) {ShaderVelocityAdvection_.setAdvectionFactor(f); return *this;}
        FluidGrid& setVelocityDiffusionGain(const float f) {ShaderVelocityDiffusion_.setGain(f); return *this;}
        FluidGrid& setVelocityDisplayScale(const float f) {ShaderVelocityDisplay_.setScale(f); return *this;}
        FluidGrid& setDensityBase(std::vector<float>* const DensityBase);
        
        void display(const Matrix3 CameraProjection,
                     const FluidBufferE Buffer = FluidBufferE::FINAL_COMPOSITION);
        void init();
        void process();
        
    private:
        
        int I(const int x, const int y) const {return (y << FLUID_GRID_SIZE_X_BITS) + x;}
        
        GL::Framebuffer* FBODensitiesBack_{nullptr};
        GL::Framebuffer* FBODensitiesFront_{nullptr};
        GL::Framebuffer* FBOVelocitiesBack_{nullptr};
        GL::Framebuffer* FBOVelocitiesFront_{nullptr};
        GL::Framebuffer FBODensitySources_{NoCreate};
        GL::Framebuffer FBODensities0_{NoCreate};
        GL::Framebuffer FBODensities1_{NoCreate};
        GL::Framebuffer FBOVelocitySources_{NoCreate};
        GL::Framebuffer FBOVelocities0_{NoCreate};
        GL::Framebuffer FBOVelocities1_{NoCreate};
        
        DensityAdvectionShader ShaderDensityAdvection_{NoCreate};
        DensityDiffusionShader ShaderDensityDiffusion_{NoCreate};
        DensityDisplayShader ShaderDensityDisplay_{NoCreate};
        DensitySourcesShader ShaderDensitySources_{NoCreate};
        VelocityAdvectionShader ShaderVelocityAdvection_{NoCreate};
        VelocityDiffusionShader ShaderVelocityDiffusion_{NoCreate};
        VelocityDisplayShader ShaderVelocityDisplay_{NoCreate};
        VelocitySourcesShader ShaderVelocitySources_{NoCreate};
        
        GL::Mesh MeshDensityAdvection_{NoCreate};
        GL::Mesh MeshDensityDisplay_{NoCreate};
        GL::Mesh MeshDensityDiffusion_{NoCreate};
        GL::Mesh MeshVelocities_{NoCreate};
        GL::Mesh MeshVelocityAdvection_{NoCreate};
        
        GL::Texture2D* TexVelocitiesBack_{nullptr};
        GL::Texture2D* TexVelocitiesFront_{nullptr};
        GL::Texture2D* TexDensitiesBack_{nullptr};
        GL::Texture2D* TexDensitiesFront_{nullptr};
        GL::Texture2D TexDensityBase_{NoCreate};        // Heightmap
        GL::Texture2D TexDensitySources_{NoCreate};     // Density sources
        GL::Texture2D TexDensities0_{NoCreate};         // Density buffer
        GL::Texture2D TexDensities1_{NoCreate};         // Density buffer
        GL::Texture2D TexVelocitySources_{NoCreate};    // Velocity sources
        GL::Texture2D TexVelocities0_{NoCreate};        // Velocities back buffer
        GL::Texture2D TexVelocities1_{NoCreate};        // Velocities front buffer
        float Viscosity_ = 1.0f;
        
        std::vector<float>* DensityBase_{nullptr};
        std::vector<float> DensitySources_;
        std::vector<float> VelocitySources_;
        
};

#endif // FLUID_GRID_H
