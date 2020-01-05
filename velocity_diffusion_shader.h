#ifndef VELOCITY_DIFFUSION_SHADER_H
#define VELOCITY_DIFFUSION_SHADER_H

#include <Corrade/Containers/Reference.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Matrix3.h>

using namespace Magnum;

class VelocityDiffusionShader : public GL::AbstractShaderProgram
{

    public:
        
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;

        explicit VelocityDiffusionShader(NoCreateT): GL::AbstractShaderProgram{NoCreate} {}
        
        explicit VelocityDiffusionShader()
        {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            GL::Shader Vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader Frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            Vert.addFile("texture_base_shader.vert");
            Frag.addFile("velocity_diffusion_shader.frag");

            CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({Vert, Frag}));

            attachShaders({Vert, Frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());

            setUniform(uniformLocation("u_tex_velocity_sources"), TexUnitVelocitySources);
            setUniform(uniformLocation("u_tex_velocity_buffer"), TexUnitVelocityBuffer);
            DeltaTUniform_ = uniformLocation("u_dt");
            GainUniform_ = uniformLocation("u_gain");
            GridResUniform_ = uniformLocation("u_grid_res");
            TransformationUniform_ = uniformLocation("u_matrix");
            
            setUniform(DeltaTUniform_, 1.0f/60.0f);
            setUniform(GainUniform_, 1.0e5f);
            setUniform(GridResUniform_, 2);
        }

        VelocityDiffusionShader& setDeltaT(const Float dt)
        {
            setUniform(DeltaTUniform_, dt);
            return *this;
        }
        
        VelocityDiffusionShader& setGain(const Float f)
        {
            setUniform(GainUniform_, f);
            return *this;
        }
        
        VelocityDiffusionShader& setGridRes(const Int r)
        {
            setUniform(GridResUniform_, r);
            return *this;
        }
        
        VelocityDiffusionShader& setTransformation(const Matrix3& t)
        {
            setUniform(TransformationUniform_, t);
            return *this;
        }

        VelocityDiffusionShader& bindTextures(GL::Texture2D& TexVelocitySources,
                                              GL::Texture2D& TexVelocityBuffer)
        {
            TexVelocitySources.bind(TexUnitVelocitySources);
            TexVelocityBuffer.bind(TexUnitVelocityBuffer);
            return *this;
        }

    private:
        
        enum: Int
        {
            TexUnitVelocitySources = 0,
            TexUnitVelocityBuffer = 1
        };

        Float   DeltaTUniform_;
        Float   GainUniform_;
        Int     GridResUniform_;
        Int     TransformationUniform_;
};

#endif // VELOCITY_DIFFUSION_SHADER_H
