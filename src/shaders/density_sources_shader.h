#ifndef DENSITY_SOURCES_SHADER_H
#define DENSITY_SOURCES_SHADER_H

#include <string>

#include <Corrade/Containers/Reference.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Matrix3.h>

#include "shader_path.h"

using namespace Magnum;

class DensitySourcesShader : public GL::AbstractShaderProgram
{

    public:
        
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector3> AmountRGB;

        explicit DensitySourcesShader(NoCreateT): GL::AbstractShaderProgram{NoCreate} {}
        
        explicit DensitySourcesShader()
        {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            GL::Shader Vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader Frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            Vert.addFile(Path_+"density_sources_shader.vert");
            Frag.addFile(Path_+"density_sources_shader.frag");

            CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({Vert, Frag}));

            attachShaders({Vert, Frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());
            
            TransformationMatrixUniform_ = uniformLocation("u_matrix");
        }
        
        DensitySourcesShader& setTransformation(const Matrix3& t)
        {
            setUniform(TransformationMatrixUniform_, t);
            return *this;
        }
        
    private:
        
        std::string Path_{SHADER_PATH};
        Int TransformationMatrixUniform_;

};

#endif // DENSITY_SOURCES_SHADER_H
