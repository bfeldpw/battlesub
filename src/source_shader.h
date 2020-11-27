#ifndef SOURCE_SHADER_H
#define SOURCE_SHADER_H

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

enum class SourceShaderVersionE : int
{
    SCALAR = 0,
    VECTOR = 1
};

class SourceShader : public GL::AbstractShaderProgram
{

    public:
        
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;

        explicit SourceShader(NoCreateT): GL::AbstractShaderProgram{NoCreate} {}
        
        explicit SourceShader(const SourceShaderVersionE _ShaderVersion)
        {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            GL::Shader Vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader Frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            Vert.addFile(Path_+"texture_base_shader.vert");
            if (_ShaderVersion == SourceShaderVersionE::SCALAR)
                Frag.addFile(Path_+"source1d_shader.frag");
            else
                Frag.addFile(Path_+"source2d_shader.frag");

            CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({Vert, Frag}));

            attachShaders({Vert, Frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());

            setUniform(uniformLocation("u_tex_buffer"), TexUnitBuffer);
            setUniform(uniformLocation("u_tex_sources"), TexUnitSources);
            TransformationUniform_ = uniformLocation("u_matrix");
        }

        SourceShader& setTransformation(const Matrix3& t)
        {
            setUniform(TransformationUniform_, t);
            return *this;
        }

        SourceShader& bindTextures(GL::Texture2D& TexBuffer,
                                   GL::Texture2D& TexSources)
        {
            TexBuffer.bind(TexUnitBuffer);
            TexSources.bind(TexUnitSources);
            return *this;
        }

    private:
        
        enum: Int
        {
            TexUnitBuffer = 0,
            TexUnitSources = 1
        };

        std::string Path_{SHADER_PATH};

        Int     TransformationUniform_;
};

#endif // SOURCE_SHADER_H
