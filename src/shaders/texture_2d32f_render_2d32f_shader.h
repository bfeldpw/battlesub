#ifndef TEXTURE_2D32F_RENDER_2D32F_SHADER_H
#define TEXTURE_2D32F_RENDER_2D32F_SHADER_H

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

class Texture2D32FRender2D32FShader: public GL::AbstractShaderProgram
{

    public:
        
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;

        explicit Texture2D32FRender2D32FShader(NoCreateT): GL::AbstractShaderProgram{NoCreate} {}
        
        explicit Texture2D32FRender2D32FShader()
        {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            GL::Shader Vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader Frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            Vert.addFile(Path_+"texture_base_shader.vert");
            Frag.addFile(Path_+"texture_2d32f_render_2d32f_shader.frag");

            CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({Vert, Frag}));

            attachShaders({Vert, Frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());

            setUniform(uniformLocation("u_texture"), TextureUnit);
            TransformationMatrixUniform_ = uniformLocation("u_matrix");
        }

        Texture2D32FRender2D32FShader& setTransformation(const Matrix3& t)
        {
            setUniform(TransformationMatrixUniform_, t);
            return *this;
        }
        
        Texture2D32FRender2D32FShader& bindTexture(GL::Texture2D& Texture)
        {
            Texture.bind(TextureUnit);
            return *this;
        }

    private:
        
        enum: Int { TextureUnit = 0 };

        std::string Path_{SHADER_PATH};

        Int TransformationMatrixUniform_;
};

#endif // TEXTURE_2D32F_RENDER_2D32F_SHADER_H
