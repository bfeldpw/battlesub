#ifndef DIFFUSION_SHADER_H
#define DIFFUSION_SHADER_H

#include <Corrade/Containers/Reference.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Matrix3.h>

using namespace Magnum;

class DiffusionShader : public GL::AbstractShaderProgram
{

    public:
        
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;

        explicit DiffusionShader(NoCreateT): GL::AbstractShaderProgram{NoCreate} {}
        
        explicit DiffusionShader()
        {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            GL::Shader Vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader Frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            Vert.addFile("texture_base_shader.vert");
            Frag.addFile("diffusion_shader.frag");

            CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({Vert, Frag}));

            attachShaders({Vert, Frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());

            setUniform(uniformLocation("u_tex_densities"), TexUnitDensities);
            setUniform(uniformLocation("u_tex_density_base"), TexUnitDensityBase);
            setUniform(uniformLocation("u_tex_diffusion"), TexUnitDiffusion);
            setUniform(uniformLocation("u_tex_velocities"), TexUnitVelocities);
            TransformationUniform_ = uniformLocation("u_matrix");
        }

        DiffusionShader& setTransformation(const Matrix3& t)
        {
            setUniform(TransformationUniform_, t);
            return *this;
        }

        DiffusionShader& bindTextures(GL::Texture2D& TexDensities,
                                      GL::Texture2D& TexDensityBase,
                                      GL::Texture2D& TexVelocities,
                                      GL::Texture2D& TexDiffusion)
        {
            TexDensities.bind(TexUnitDensities);
            TexDensityBase.bind(TexUnitDensityBase);
            TexVelocities.bind(TexUnitVelocities);
            TexDiffusion.bind(TexUnitDiffusion);
            return *this;
        }

    private:
        
        enum: Int
        {
            TexUnitDensities = 0,
            TexUnitDensityBase = 1,
            TexUnitDiffusion = 2,
            TexUnitVelocities = 3
        };

        Int TransformationUniform_;
};

#endif // DIFFUSION_SHADER_H
