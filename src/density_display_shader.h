#ifndef DENSITY_DISPLAY_SHADER_H
#define DENSITY_DISPLAY_SHADER_H

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

class DensityDisplayShader : public GL::AbstractShaderProgram
{

    public:
        
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;

        explicit DensityDisplayShader(NoCreateT): GL::AbstractShaderProgram{NoCreate} {}
        
        explicit DensityDisplayShader()
        {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            GL::Shader Vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader Frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            Vert.addFile(Path_+"texture_base_shader.vert");
            Frag.addFile(Path_+"density_display_shader.frag");

            CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({Vert, Frag}));

            attachShaders({Vert, Frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());

            setUniform(uniformLocation("u_texture"), TextureUnit);
            GammaUniform_ = uniformLocation("u_gamma");
            TransformationMatrixUniform_ = uniformLocation("u_matrix");
            
            setUniform(GammaUniform_, 2.2f);
        }

        DensityDisplayShader& setGamma(const float f)
        {
            setUniform(GammaUniform_, f);
            return *this;
        }
        
        DensityDisplayShader& setTransformation(const Matrix3& t)
        {
            setUniform(TransformationMatrixUniform_, t);
            return *this;
        }

        DensityDisplayShader& bindTexture(GL::Texture2D& Texture)
        {
            Texture.bind(TextureUnit);
            return *this;
        }

    private:
        
        enum: Int { TextureUnit = 0 };

        std::string Path_{SHADER_PATH};

        Float GammaUniform_;
        Int TransformationMatrixUniform_;
};

#endif // DENSITY_DISPLAY_SHADER_H
