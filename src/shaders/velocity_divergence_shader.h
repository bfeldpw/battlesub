#ifndef VELOCITY_DIVERGENCE_SHADER_H
#define VELOCITY_DIVERGENCE_SHADER_H

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

class VelocityDivergenceShader : public GL::AbstractShaderProgram
{

    public:
        
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;

        explicit VelocityDivergenceShader(NoCreateT): GL::AbstractShaderProgram{NoCreate} {}
        
        explicit VelocityDivergenceShader()
        {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            GL::Shader Vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader Frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            Vert.addFile(Path_+"texture_base_shader.vert");
            Frag.addFile(Path_+"velocity_divergence_shader.frag");

            CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({Vert, Frag}));

            attachShaders({Vert, Frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());

            setUniform(uniformLocation("u_tex_velocity_buffer"), TexUnitVelocityBuffer);
            TransformationUniform_ = uniformLocation("u_matrix");
        }
        
        VelocityDivergenceShader& setTransformation(const Matrix3& t)
        {
            setUniform(TransformationUniform_, t);
            return *this;
        }

        VelocityDivergenceShader& bindTexture(GL::Texture2D& TexVelocityBuffer)
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

        Int     TransformationUniform_;
};

#endif // VELOCITY_DIVERGENCE_SHADER_H
