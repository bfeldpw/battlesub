#ifndef VELOCITY_ADVECTION_SHADER_H
#define VELOCITY_ADVECTION_SHADER_H

#include <string>

#include <Corrade/Containers/Reference.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Matrix3.h>

#include "shader_path.h"

using namespace Magnum;

class VelocityAdvectionShader : public GL::AbstractShaderProgram
{

    public:
        
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;

        explicit VelocityAdvectionShader(NoCreateT): GL::AbstractShaderProgram{NoCreate} {}
        
        explicit VelocityAdvectionShader()
        {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            GL::Shader Vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader Frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            Vert.addFile(Path_+"texture_base_shader.vert");
            Frag.addFile(Path_+"velocity_advection_shader.frag");

            CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({Vert, Frag}));

            attachShaders({Vert, Frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());

            setUniform(uniformLocation("u_tex_velocity_buffer"), TexUnitVelocityBuffer);
            AdvectionFactorUniform_ = uniformLocation("u_advection_factor");
            DeltaTUniform_ = uniformLocation("u_dt");
            GridResolutionUniform_ = uniformLocation("u_grid_res");
            TransformationUniform_ = uniformLocation("u_matrix");
            
            setUniform(AdvectionFactorUniform_, 0.5);
            setUniform(DeltaTUniform_, 1.0f/60.0f);
            setUniform(GridResolutionUniform_, 2.0f);
        }

        VelocityAdvectionShader& setAdvectionFactor(const Float a)
        {
            setUniform(AdvectionFactorUniform_, a);
            return *this;
        }

        VelocityAdvectionShader& setDeltaT(const Float dt)
        {
            setUniform(DeltaTUniform_, dt);
            return *this;
        }
        
        VelocityAdvectionShader& setGridRes(const Float r)
        {
            setUniform(GridResolutionUniform_, r);
            return *this;
        }
        
        VelocityAdvectionShader& setTransformation(const Matrix3& t)
        {
            setUniform(TransformationUniform_, t);
            return *this;
        }

        VelocityAdvectionShader& bindTexture(GL::Texture2D& TexVelocityBuffer)
        {
            TexVelocityBuffer.bind(TexUnitVelocityBuffer);
            return *this;
        }

    private:
        
        enum: Int
        {
            TexUnitVelocityBuffer = 0
        };

        std::string Path_{SHADER_PATH};

        Float   AdvectionFactorUniform_;
        Float   DeltaTUniform_;
        Float   GridResolutionUniform_;
        Int     TransformationUniform_;
};

#endif // VELOCITY_ADVECTION_SHADER_H
