#ifndef VELOCITY_PROCESSING_SHADER_H
#define VELOCITY_PROCESSING_SHADER_H

#include <Corrade/Containers/Reference.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Matrix3.h>

using namespace Magnum;

class VelocityProcessingShader : public GL::AbstractShaderProgram
{

    public:
        
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;

        explicit VelocityProcessingShader(NoCreateT): GL::AbstractShaderProgram{NoCreate} {}
        
        explicit VelocityProcessingShader()
        {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            GL::Shader Vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader Frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            Vert.addFile("texture_base_shader.vert");
            Frag.addFile("velocity_processing_shader.frag");

            CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({Vert, Frag}));

            attachShaders({Vert, Frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());

            setUniform(uniformLocation("u_tex_velocities"), TexUnitVelocities);
            setUniform(uniformLocation("u_tex_velocity_buffer"), TexUnitVelocityBuffer);
            DeltaTUniform_ = uniformLocation("u_dt");
            GridSizeUniform_ = uniformLocation("u_size");
            TransformationUniform_ = uniformLocation("u_matrix");
            
            setUniform(DeltaTUniform_, 1.0f/60.0f);
            setUniform(GridSizeUniform_, 2048*1024);
        }

        VelocityProcessingShader& setDeltaT(const Float dt)
        {
            setUniform(DeltaTUniform_, dt);
            return *this;
        }
        
        VelocityProcessingShader& setGridSize(const Int s)
        {
            setUniform(GridSizeUniform_, s);
            return *this;
        }
        
        VelocityProcessingShader& setTransformation(const Matrix3& t)
        {
            setUniform(TransformationUniform_, t);
            return *this;
        }

        VelocityProcessingShader& bindTextures(GL::Texture2D& TexVelocities,
                                               GL::Texture2D& TexVelocityBuffer)
        {
            TexVelocities.bind(TexUnitVelocities);
            TexVelocityBuffer.bind(TexUnitVelocityBuffer);
            return *this;
        }

    private:
        
        enum: Int
        {
            TexUnitVelocities = 0,
            TexUnitVelocityBuffer = 1
        };

        Float   DeltaTUniform_;
        Int     GridSizeUniform_;
        Int     TransformationUniform_;
};

#endif // VELOCITY_PROCESSING_SHADER_H
