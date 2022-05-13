#ifndef VELOCITY_GRADIENT_SUBSTRACTION_SHADER_H
#define VELOCITY_GRADIENT_SUBSTRACTION_SHADER_H

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

class VelocityGradientSubstractionShader : public GL::AbstractShaderProgram
{

    public:
        
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;

        explicit VelocityGradientSubstractionShader(NoCreateT): GL::AbstractShaderProgram{NoCreate} {}
        
        explicit VelocityGradientSubstractionShader()
        {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            GL::Shader Vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader Frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            Vert.addFile(Path_+"texture_base_shader.vert");
            Frag.addFile(Path_+"velocity_gradient_substraction_shader.frag");

            CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({Vert, Frag}));

            attachShaders({Vert, Frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());

            setUniform(uniformLocation("u_tex_velocity_buffer"), TexUnitVelocityBuffer);
            setUniform(uniformLocation("u_tex_velocity_divergence_buffer"), TexUnitVelocityDivergenceBuffer);
            TransformationUniform_ = uniformLocation("u_matrix");
        }
        
        VelocityGradientSubstractionShader& setTransformation(const Matrix3& t)
        {
            setUniform(TransformationUniform_, t);
            return *this;
        }

        VelocityGradientSubstractionShader& bindTextures(GL::Texture2D& TexVelocityBuffer,
                                                         GL::Texture2D& TexVelocityDivergenceBuffer)
        {
            TexVelocityBuffer.bind(TexUnitVelocityBuffer);
            TexVelocityDivergenceBuffer.bind(TexUnitVelocityDivergenceBuffer);
            return *this;
        }

    private:
        
        enum: Int
        {
            TexUnitVelocityBuffer = 0,
            TexUnitVelocityDivergenceBuffer = 1
        };

        std::string Path_{SHADER_PATH};

        Int     TransformationUniform_;
};

#endif // VELOCITY_GRADIENT_SUBSTRACTION_SHADER_H
