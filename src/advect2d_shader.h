#ifndef ADVECT2D_SHADER_H
#define ADVECT2D_SHADER_H

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

class Advect2dShader : public GL::AbstractShaderProgram
{

    public:
        
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;

        explicit Advect2dShader(NoCreateT): GL::AbstractShaderProgram{NoCreate} {}
        
        explicit Advect2dShader()
        {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            GL::Shader Vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader Frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            Vert.addFile(Path_+"texture_base_shader.vert");
            Frag.addFile(Path_+"advect2d_shader.frag");

            CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({Vert, Frag}));

            attachShaders({Vert, Frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());

            setUniform(uniformLocation("u_tex_buffer"), TexUnitBuffer);
            AdvectionFactorUniform_ = uniformLocation("u_advection_factor");
            DeltaTUniform_ = uniformLocation("u_dt");
            GridResolutionUniform_ = uniformLocation("u_grid_res");
            TransformationUniform_ = uniformLocation("u_matrix");
            
            setUniform(AdvectionFactorUniform_, 100.0f);
            setUniform(DeltaTUniform_, 1.0f/60.0f);
            setUniform(GridResolutionUniform_, 2.0f);
        }

        Advect2dShader& setAdvectionFactor(const Float f)
        {
            setUniform(AdvectionFactorUniform_, f);
            return *this;
        }

        Advect2dShader& setDeltaT(const Float dt)
        {
            setUniform(DeltaTUniform_, dt);
            return *this;
        }
        
        Advect2dShader& setGridRes(const Float r)
        {
            setUniform(GridResolutionUniform_, r);
            return *this;
        }
        
        Advect2dShader& setTransformation(const Matrix3& t)
        {
            setUniform(TransformationUniform_, t);
            return *this;
        }

        Advect2dShader& bindTexture(GL::Texture2D& TexBuffer)
        {
            TexBuffer.bind(TexUnitBuffer);
            return *this;
        }

    private:
        
        enum: Int
        {
            TexUnitBuffer = 0
        };

        std::string Path_{SHADER_PATH};

        Float   AdvectionFactorUniform_;
        Float   DeltaTUniform_;
        Float   GridResolutionUniform_;
        Int     TransformationUniform_;
};

#endif // ADVECT2D_SHADER_H
