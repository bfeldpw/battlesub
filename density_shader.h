#ifndef DENSITY_SHADER_H
#define DENSITY_SHADER_H

#include <Corrade/Containers/Reference.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>

class DensityShader : public GL::AbstractShaderProgram
{

    public:
        
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;

        explicit DensityShader(NoCreateT): GL::AbstractShaderProgram{NoCreate} {}
        
        explicit DensityShader()
        {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            GL::Shader Vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader Frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            Vert.addFile("density_shader.vert");
            Frag.addFile("density_shader.frag");

            CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({Vert, Frag}));

            attachShaders({Vert, Frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());

            setUniform(uniformLocation("u_texture"), TextureUnit);
            TransformationMatrixUniform = uniformLocation("u_matrix");
        }

        DensityShader& setTransformation(const Matrix3& t)
        {
            setUniform(TransformationMatrixUniform, t);
            return *this;
        }

        DensityShader& bindTexture(GL::Texture2D& Texture)
        {
            Texture.bind(TextureUnit);
            return *this;
        }

    private:
        
        enum: Int { TextureUnit = 0 };

        Int TransformationMatrixUniform;
};

#endif // DENSITY_SHADER_H
