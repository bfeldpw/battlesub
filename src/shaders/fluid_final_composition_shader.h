#ifndef FLUID_FINAL_COMPOSITION_SHADER_H
#define FLUID_FINAL_COMPOSITION_SHADER_H

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

class FluidFinalCompositionShader : public GL::AbstractShaderProgram
{

    public:
        
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;

        explicit FluidFinalCompositionShader(NoCreateT): GL::AbstractShaderProgram{NoCreate} {}
        
        explicit FluidFinalCompositionShader()
        {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            GL::Shader Vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader Frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            Vert.addFile(Path_+"texture_base_shader.vert");
            Frag.addFile(Path_+"fluid_final_composition_shader.frag");

            CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({Vert, Frag}));

            attachShaders({Vert, Frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());

            setUniform(uniformLocation("u_tex_density_base"), TexUnitDensityBase);
            setUniform(uniformLocation("u_tex_density_buffer"), TexUnitDensityBuffer);
            setUniform(uniformLocation("u_tex_ground_distorted"), TexUnitGroundDistorted);
            setUniform(uniformLocation("u_tex_velocities"), TexUnitVelocities);
            TransformationUniform_ = uniformLocation("u_matrix");
        }

        FluidFinalCompositionShader& setTransformation(const Matrix3& t)
        {
            setUniform(TransformationUniform_, t);
            return *this;
        }

        FluidFinalCompositionShader& bindTextures(GL::Texture2D& TexDensityBase,
                                                  GL::Texture2D& TexDensityBuffer,
                                                  GL::Texture2D& TexGroundDistorted,
                                                  GL::Texture2D& TexVelocities)
        {
            TexDensityBase.bind(TexUnitDensityBase);
            TexDensityBuffer.bind(TexUnitDensityBuffer);
            TexGroundDistorted.bind(TexUnitGroundDistorted);
            TexVelocities.bind(TexUnitVelocities);
            return *this;
        }

    private:
        
        enum: Int
        {
            TexUnitDensityBase = 0,
            TexUnitDensityBuffer = 1,
            TexUnitGroundDistorted = 2,
            TexUnitVelocities = 3
        };

        std::string Path_{SHADER_PATH};

        Int     TransformationUniform_;
};

#endif // FLUID_FINAL_COMPOSITON_SHADER_H
