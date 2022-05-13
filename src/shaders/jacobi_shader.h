#ifndef JACOBI_SHADER_H
#define JACOBI_SHADER_H

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

enum class JacobiShaderVersionE : int
{
    SCALAR = 0,
    VECTOR = 1
};

class JacobiShader : public GL::AbstractShaderProgram
{

    public:
        
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;

        explicit JacobiShader(NoCreateT): GL::AbstractShaderProgram{NoCreate} {}
        
        explicit JacobiShader(const JacobiShaderVersionE _ShaderVersion)
        {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            GL::Shader Vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader Frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            Vert.addFile(Path_+"texture_base_shader.vert");
            if (_ShaderVersion == JacobiShaderVersionE::SCALAR)
                Frag.addFile(Path_+"jacobi1d_shader.frag");
            else
                Frag.addFile(Path_+"jacobi2d_shader.frag");

            CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({Vert, Frag}));

            attachShaders({Vert, Frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());

            setUniform(uniformLocation("u_tex_x"), TexUnitX);
            setUniform(uniformLocation("u_tex_b"), TexUnitB);
            AlphaUniform_ = uniformLocation("u_alpha");
            RBetaUniform_ = uniformLocation("u_r_beta");
            DiffusionRateUniform_ = uniformLocation("u_diffusion_rate");
            TransformationUniform_ = uniformLocation("u_matrix");
            
            setUniform(AlphaUniform_, 1.0f);
            setUniform(RBetaUniform_, 1.0f/4.0f);
            setUniform(DiffusionRateUniform_, 1.0f);
        }

        JacobiShader& setAlpha(const Float a)
        {
            setUniform(AlphaUniform_, a);
            return *this;
        }

        JacobiShader& setRBeta(const Float b)
        {
            setUniform(RBetaUniform_, b);
            return *this;
        }

        JacobiShader& setDiffusionRate(const Float d)
        {
            setUniform(DiffusionRateUniform_, d);
            return *this;
        }

        JacobiShader& setTransformation(const Matrix3& t)
        {
            setUniform(TransformationUniform_, t);
            return *this;
        }

        JacobiShader& bindTextures(GL::Texture2D& TexX, GL::Texture2D& TexB)
        {
            TexX.bind(TexUnitX);
            TexB.bind(TexUnitB);
            return *this;
        }

    private:

        enum: Int
        {
            TexUnitX = 0,
            TexUnitB = 1
        };

        std::string Path_{SHADER_PATH};

        Float   AlphaUniform_;
        Float   RBetaUniform_;
        Float   DiffusionRateUniform_;
        Int     TransformationUniform_;
};

#endif // JACOBI_SHADER_H
