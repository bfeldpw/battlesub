#ifndef DENSITY_DIFFUSION_SHADER_H
#define DENSITY_DIFFUSION_SHADER_H

#include <Corrade/Containers/Reference.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Matrix3.h>

using namespace Magnum;

class DensityDiffusionShader : public GL::AbstractShaderProgram
{

    public:
        
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;

        explicit DensityDiffusionShader(NoCreateT): GL::AbstractShaderProgram{NoCreate} {}
        
        explicit DensityDiffusionShader()
        {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            GL::Shader Vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader Frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            Vert.addFile("texture_base_shader.vert");
            Frag.addFile("density_diffusion_shader.frag");

            CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({Vert, Frag}));

            attachShaders({Vert, Frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());

            setUniform(uniformLocation("u_tex_density_sources"), TexUnitDensitySources);
            setUniform(uniformLocation("u_tex_density_base"), TexUnitDensityBase);
            setUniform(uniformLocation("u_tex_density_buffer"), TexUnitDensityBuffer);
            setUniform(uniformLocation("u_tex_velocities"), TexUnitVelocities);
            DeltaTUniform_ = uniformLocation("u_dt");
            GridSizeUniform_ = uniformLocation("u_size");
            TransformationUniform_ = uniformLocation("u_matrix");
            
            setUniform(DeltaTUniform_, 1.0f/60.0f);
            setUniform(GridSizeUniform_, 2048*1024);
        }

        DensityDiffusionShader& setDeltaT(const Float dt)
        {
            setUniform(DeltaTUniform_, dt);
            return *this;
        }
        
        DensityDiffusionShader& setGridSize(const Int s)
        {
            setUniform(GridSizeUniform_, s);
            return *this;
        }
        
        DensityDiffusionShader& setTransformation(const Matrix3& t)
        {
            setUniform(TransformationUniform_, t);
            return *this;
        }

        DensityDiffusionShader& bindTextures(GL::Texture2D& TexDensitySources,
                                      GL::Texture2D& TexDensityBase,
                                      GL::Texture2D& TexDensityBuffer,
                                      GL::Texture2D& TexVelocities)
        {
            TexDensitySources.bind(TexUnitDensitySources);
            TexDensityBase.bind(TexUnitDensityBase);
            TexDensityBuffer.bind(TexUnitDensityBuffer);
            TexVelocities.bind(TexUnitVelocities);
            return *this;
        }

    private:
        
        enum: Int
        {
            TexUnitDensitySources = 0,
            TexUnitDensityBase = 1,
            TexUnitDensityBuffer = 2,
            TexUnitVelocities = 3
        };

        Float   DeltaTUniform_;
        Int     GridSizeUniform_;
        Int     TransformationUniform_;
};

#endif // DENSITY_DIFFUSION_SHADER_H
