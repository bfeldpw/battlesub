#ifndef FLUID_GRID_H
#define FLUID_GRID_H

#include <algorithm>
#include <entt/entity/fwd.hpp>
#include <vector>

#include <entt/entity/registry.hpp>
#include <Magnum/GL/BufferImage.h>
#include <Magnum/GL/Framebuffer.h>

#include "advect_shader.h"
#include "density_display_shader.h"
#include "density_sources_shader.h"
#include "drawable_generic.h"
#include "fluid_final_composition_shader.h"
#include "global_resources.h"
#include "ground_distortion_shader.h"
#include "jacobi_shader.h"
#include "source_shader.h"
#include "texture_2d32f_render_2d32f_shader.h"
#include "velocity_display_shader.h"
#include "velocity_divergence_shader.h"
#include "velocity_gradient_substraction_shader.h"
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
    VELOCITIES_LOW_RES = 6,
    VELOCITY_DIVERGENCE = 7,
    PRESSURE_FRONT = 8,
    PRESSURE_BACK = 9,
    DENSITY_DIFFUSION_FRONT = 10,
    DENSITY_DIFFUSION_BACK = 11,
    GROUND_DISTORTED = 12,
    FINAL_COMPOSITION = 13
};

class FluidGrid
{
    public:

        struct Config
        {
            int IterationsDensityDiffusion_ = 5;
            int IterationsVelocityDiffusion_ = 5;
            int IterationsPressureEquation_ = 20;
        } Config_;

        // Fluid Grid constants to be accessed from outside, too
        static constexpr float FLUID_FREQUENCY = 30.0f;
        static constexpr float FLUID_TIMESTEP = 1.0f/FLUID_FREQUENCY;
        static constexpr int VELOCITY_READBACK_SUBSAMPLE = 2;
        static constexpr int VELOCITY_READBACK_SUBSAMPLE_XY = 3;

        typedef std::array<float, (FLUID_GRID_ARRAY_SIZE >> VELOCITY_READBACK_SUBSAMPLE_XY)> VelocityReadbackDataType;

        explicit FluidGrid(entt::registry& _Reg) : Reg_(_Reg) {}

        void loadConfig();

        Vector2 getVelocity(const float _x, const float _y) const;
        const VelocityReadbackDataType& getVelocityReadback() const {return VelReadback_;};

        FluidGrid& addDensity(const float x, const float y, const float d);
        FluidGrid& addDensity(const float x0, const float y0,
                              const float x1, const float y1, const float d);
        FluidGrid& addVelocity(const float x, const float y, const float Vx, const float Vy,
                               const float w = 1.0f);
        FluidGrid& addVelocity(const float x0, const float y0, const float Vx0, const float Vy0,
                               const float x1, const float y1, const float Vx1, const float Vy1,
                               const float w = 1.0f);
        FluidGrid& setDensityDistortion(const float f) {ShaderGroundDistortion_.setDistortion(f); return *this;}
        FluidGrid& setConfig(const Config& _Config) {Config_ = _Config; return *this;}
        FluidGrid& setIterationsDensityDiffusion(const int n)
        {
            Config_.IterationsDensityDiffusion_ = n;
            ShaderJacobi1d_.setAlpha(getDiffusionAlpha());
            return *this;
        }
        FluidGrid& setScalarFieldDisplayScale(const float f) {ShaderDensityDisplay_.setScale(f); return *this;}
        FluidGrid& setVelocityAdvectionFactor(const float f) {ShaderAdvect2d_.setAdvectionFactor(f); return *this;}
        FluidGrid& setVelocityDisplayScale(const float f) {ShaderVelocityDisplay_.setScale(f); return *this;}
        FluidGrid& setVelocityDisplayShowOnlyMagnitude(const bool b) {ShaderVelocityDisplay_.setShowOnlyMagnitude(b); return *this;}
        FluidGrid& setDensityBase(std::vector<float>* const DensityBase);

        void bindBoundaryMap();
        void display(const Matrix3 CameraProjection,
                     const FluidBufferE Buffer = FluidBufferE::FINAL_COMPOSITION);
        void init();
        void process(const double);
        
    private:

        entt::registry& Reg_;

        static constexpr int VELOCITY_READBACK_FRACTION_SIZE = 3;
        typedef std::array<float, (FLUID_GRID_ARRAY_SIZE >>
                                   (VELOCITY_READBACK_SUBSAMPLE_XY+
                                    VELOCITY_READBACK_FRACTION_SIZE))> VelocityReadbackFractionDataType;
        static constexpr Magnum::Range2Di ViewportFull{{0, 0}, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}};
        static constexpr Magnum::Range2Di ViewportTop{{0, 0}, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y>>1}};
        static constexpr Magnum::Range2Di ViewportBottom{{0, FLUID_GRID_SIZE_Y>>1}, {FLUID_GRID_SIZE_X, FLUID_GRID_SIZE_Y}};
        const Magnum::Range2Di* ViewportCurrent = &ViewportTop;
        const Magnum::Range2Di* ViewportNext = &ViewportBottom;

        float getPressureEquationAlpha()
        {
            // float Res = float(FLUID_GRID_SIZE_X) / WORLD_SIZE_DEFAULT_X;
            float Res = WORLD_SIZE_DEFAULT_X / FLUID_GRID_SIZE_X;
            // return -Res*Res;
            return 2.0f;
        }
        float getDiffusionAlpha()
        {
            float Res = float(FLUID_GRID_SIZE_X) / WORLD_SIZE_DEFAULT_X;
            // float Res = WORLD_SIZE_DEFAULT_X / FLUID_GRID_SIZE_X;
            return Res*Res / Config_.IterationsDensityDiffusion_ * FLUID_FREQUENCY;
        }
        void readbackVelocities(const int _Fraction, const int _SubDivisionBase2);

        void advectDensities();
        void advectVelocities();
        void diffuseDensities();
        void calculateDivergence();
        void calculatePressure();
        void substractPressureGradient();
        void diffuseVelocities();
        void renderDensitySources();
        void renderVelocitySources();

        // int IterationsDensityDiffusion_ = 5;

        VelocityReadbackDataType VelReadback_;

        GL::BufferImage2D* PBOVelocityFront_{nullptr};
        GL::BufferImage2D* PBOVelocityBack_{nullptr};
        GL::BufferImage2D PBOVelocity0_{NoCreate};
        GL::BufferImage2D PBOVelocity1_{NoCreate};
        GL::Framebuffer* FBODensitiesBack_{nullptr};
        GL::Framebuffer* FBODensitiesFront_{nullptr};
        GL::Framebuffer* FBOPressureBack_{nullptr};
        GL::Framebuffer* FBOPressureFront_{nullptr};
        GL::Framebuffer* FBOVelocitiesBack_{nullptr};
        GL::Framebuffer* FBOVelocitiesFront_{nullptr};
        GL::Framebuffer FBOBoundaries_{NoCreate};
        GL::Framebuffer FBODensitySources_{NoCreate};
        GL::Framebuffer FBODensities0_{NoCreate};
        GL::Framebuffer FBODensities1_{NoCreate};
        GL::Framebuffer FBOFluidFinalComposition_{NoCreate};
        GL::Framebuffer FBOGroundDistorted_{NoCreate};
        GL::Framebuffer FBOPressure0_{NoCreate};
        GL::Framebuffer FBOPressure1_{NoCreate};
        GL::Framebuffer FBOVelocitySources_{NoCreate};
        GL::Framebuffer FBOVelocities0_{NoCreate};
        GL::Framebuffer FBOVelocities1_{NoCreate};
        GL::Framebuffer FBOVelocitiesLowRes_{NoCreate};
        GL::Framebuffer FBOVelocityDivergence_{NoCreate};

        AdvectShader ShaderAdvect1d_{NoCreate};
        AdvectShader ShaderAdvect2d_{NoCreate};
        DensityDisplayShader ShaderDensityDisplay_{NoCreate};
        DensitySourcesShader ShaderDensitySources_{NoCreate};
        GroundDistortionShader ShaderGroundDistortion_{NoCreate};
        FluidFinalCompositionShader ShaderFluidFinalComposition_{NoCreate};
        JacobiShader ShaderJacobi1d_{NoCreate};
        JacobiShader ShaderJacobi2d_{NoCreate};
        SourceShader ShaderSource1d_{NoCreate};
        SourceShader ShaderSource2d_{NoCreate};
        Texture2D32FRender2D32FShader ShaderVelocityLowRes_{NoCreate};
        VelocityDisplayShader ShaderVelocityDisplay_{NoCreate};
        VelocityDivergenceShader ShaderVelocityDivergence_{NoCreate};
        VelocityGradientSubstractionShader ShaderVelocityGradientSubstraction_{NoCreate};
        VelocitySourcesShader ShaderVelocitySources_{NoCreate};

        GL::Mesh MeshFluidGridBuffer_{NoCreate};

        GL::Texture2D* TexVelocitiesBack_{nullptr};
        GL::Texture2D* TexVelocitiesFront_{nullptr};
        GL::Texture2D* TexDensitiesBack_{nullptr};
        GL::Texture2D* TexDensitiesFront_{nullptr};
        GL::Texture2D* TexPressureBack_{nullptr};
        GL::Texture2D* TexPressureFront_{nullptr};
        GL::Texture2D TexBoundaries_{NoCreate};
        GL::Texture2D TexDensityBase_{NoCreate};        // Heightmap
        GL::Texture2D TexDensitySources_{NoCreate};     // Density sources
        GL::Texture2D TexDensities0_{NoCreate};         // Density buffer
        GL::Texture2D TexDensities1_{NoCreate};         // Density buffer
        GL::Texture2D TexFluidFinalComposition_{NoCreate};
        GL::Texture2D TexGroundDistorted_{NoCreate};
        GL::Texture2D TexPressure0_{NoCreate};        
        GL::Texture2D TexPressure1_{NoCreate};
        GL::Texture2D TexVelocitySources_{NoCreate};    // Velocity sources
        GL::Texture2D TexVelocities0_{NoCreate};        // Velocities back buffer
        GL::Texture2D TexVelocities1_{NoCreate};        // Velocities front buffer
        GL::Texture2D TexVelocityDivergence_{NoCreate};
        GL::Texture2D TexVelocitiesLowRes_{NoCreate};

        std::vector<float>* DensityBase_{nullptr};
        std::vector<float> DensitySourcesPoints_;
        std::vector<float> DensitySourcesLines_;
        std::vector<float> VelocitySourcesPoints_;
        std::vector<float> VelocitySourcesLines_;
        
};

#endif // FLUID_GRID_H
