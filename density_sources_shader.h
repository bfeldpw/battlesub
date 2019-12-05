#ifndef DENSITY_SOURCES_SHADER_H
#define DENSITY_SOURCES_SHADER_H

#include <Corrade/Containers/Reference.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Matrix3.h>

using namespace Magnum;

class DensitySourcesShader : public GL::AbstractShaderProgram
{

    public:
        
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, float> Amount;

        explicit DensitySourcesShader(NoCreateT): GL::AbstractShaderProgram{NoCreate} {}
        
        explicit DensitySourcesShader()
        {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            GL::Shader Vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader Frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            Vert.addFile("density_sources_shader.vert");
            Frag.addFile("density_sources_shader.frag");

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
        
        Int TransformationMatrixUniform_;

};

#endif // DENSITY_SOURCES_SHADER_H
