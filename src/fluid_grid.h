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
#include "fluid_final_composition_shader.h"
#include "global_resources.h"
#include "ground_distortion_shader.h"
#include "velocity_advection_shader.h"
#include "velocity_diffusion_shader.h"
#include "velocity_display_shader.h"
#include "velocity_sources_shader.h"
#include "world_def.h"

using namespace Magnum;

enum class FluidBufferE : int
{
    BOUNDARIES = 0,
    DENSITY_SOURCES = 1,
    DENSITY_BASE = 2,
    VELOCITY_SOURCES = 3,
    VELOCITIES_FRONT = 4,
    VELOCITIES_BACK = 5,
    DENSITY_DIFFUSION_FRONT = 6,
    DENSITY_DIFFUSION_BACK = 7,
    GROUND_DISTORTED = 8,
    FINAL_COMPOSITION = 9
};

class FluidGrid
{
    public:
        
        FluidGrid& addDensity(const float x, const float y, const float d);
        FluidGrid& addVelocity(const float x, const float y, const float Vx, const float Vy);
        FluidGrid& addVelocity(const float x0, const float y0, const float Vx0, const float Vy0,
                               const float x1, const float y1, const float Vx1, const float Vy1);
        FluidGrid& setDensityDistortion(const float f) {ShaderGroundDistortion_.setDistortion(f); return *this;}
        FluidGrid& setGammaCorrection(const float f) {ShaderDensityDisplay_.setGamma(f); return *this;}
        FluidGrid& setVelocityAdvectionFactor(const float f) {ShaderVelocityAdvection_.setAdvectionFactor(f); return *this;}
        FluidGrid& setVelocityDiffusionGain(const float f) {ShaderVelocityDiffusion_.setGain(f); return *this;}
        FluidGrid& setVelocityDiffusionRate(const float f) {ShaderVelocityDiffusion_.setDiff(f); return *this;}
        FluidGrid& setVelocityDisplayScale(const float f) {ShaderVelocityDisplay_.setScale(f); return *this;}
        FluidGrid& setVelocityDisplayShowOnlyMagnitude(const bool b) {ShaderVelocityDisplay_.setShowOnlyMagnitude(b); return *this;}
        FluidGrid& setDensityBase(std::vector<float>* const DensityBase);
        
        void display(const Matrix3 CameraProjection,
                     const FluidBufferE Buffer = FluidBufferE::FINAL_COMPOSITION);
        void init();
        void process(const double);
        
    private:
        
        int I(const int x, const int y) const {return (y << FLUID_GRID_SIZE_X_BITS) + x;}
        
        GL::Framebuffer* FBODensitiesBack_{nullptr};
        GL::Framebuffer* FBODensitiesFront_{nullptr};
        GL::Framebuffer* FBOVelocitiesBack_{nullptr};
        GL::Framebuffer* FBOVelocitiesFront_{nullptr};
        GL::Framebuffer FBOBoundaries_{NoCreate};
        GL::Framebuffer FBODensitySources_{NoCreate};
        GL::Framebuffer FBODensities0_{NoCreate};
        GL::Framebuffer FBODensities1_{NoCreate};
        GL::Framebuffer FBOFluidFinalComposition_{NoCreate};
        GL::Framebuffer FBOGroundDistorted_{NoCreate};
        GL::Framebuffer FBOVelocitySources_{NoCreate};
        GL::Framebuffer FBOVelocities0_{NoCreate};
        GL::Framebuffer FBOVelocities1_{NoCreate};

        // BoundariesShader ShaderBoundaries_{NoCreate};
        DensityAdvectionShader ShaderDensityAdvection_{NoCreate};
        DensityDiffusionShader ShaderDensityDiffusion_{NoCreate};
        DensityDisplayShader ShaderDensityDisplay_{NoCreate};
        DensitySourcesShader ShaderDensitySources_{NoCreate};
        GroundDistortionShader ShaderGroundDistortion_{NoCreate};
        FluidFinalCompositionShader ShaderFluidFinalComposition_{NoCreate};
        VelocityAdvectionShader ShaderVelocityAdvection_{NoCreate};
        VelocityDiffusionShader ShaderVelocityDiffusion_{NoCreate};
        VelocityDisplayShader ShaderVelocityDisplay_{NoCreate};
        VelocitySourcesShader ShaderVelocitySources_{NoCreate};

        GL::Mesh MeshBoundaries_{NoCreate};
        GL::Mesh MeshDensityAdvection_{NoCreate};
        GL::Mesh MeshDensityDisplay_{NoCreate};
        GL::Mesh MeshDensityDiffusion_{NoCreate};
        GL::Mesh MeshFluidFinalComposition_{NoCreate};
        GL::Mesh MeshGroundDistorted_{NoCreate};
        GL::Mesh MeshVelocities_{NoCreate};
        GL::Mesh MeshVelocityAdvection_{NoCreate};
        
        GL::Texture2D* TexVelocitiesBack_{nullptr};
        GL::Texture2D* TexVelocitiesFront_{nullptr};
        GL::Texture2D* TexDensitiesBack_{nullptr};
        GL::Texture2D* TexDensitiesFront_{nullptr};
        GL::Texture2D TexBoundaries_{NoCreate};
        GL::Texture2D TexDensityBase_{NoCreate};        // Heightmap
        GL::Texture2D TexDensitySources_{NoCreate};     // Density sources
        GL::Texture2D TexDensities0_{NoCreate};         // Density buffer
        GL::Texture2D TexDensities1_{NoCreate};         // Density buffer
        GL::Texture2D TexFluidFinalComposition_{NoCreate};
        GL::Texture2D TexGroundDistorted_{NoCreate};
        GL::Texture2D TexVelocitySources_{NoCreate};    // Velocity sources
        GL::Texture2D TexVelocities0_{NoCreate};        // Velocities back buffer
        GL::Texture2D TexVelocities1_{NoCreate};        // Velocities front buffer

        std::vector<float>* DensityBase_{nullptr};
        std::vector<float> DensitySources_;
        std::vector<float> VelocitySourcesPoints_;
        std::vector<float> VelocitySourcesLines_;
        
};

#endif // FLUID_GRID_H
