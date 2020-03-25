#ifndef GROUND_DISTORTION_SHADER_H
#define GROUND_DISTORTION_SHADER_H

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

class GroundDistortionShader: public GL::AbstractShaderProgram
{

    public:
        
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;

        explicit GroundDistortionShader(NoCreateT): GL::AbstractShaderProgram{NoCreate} {}
        
        explicit GroundDistortionShader()
        {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            GL::Shader Vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader Frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            Vert.addFile(Path_+"texture_base_shader.vert");
            Frag.addFile(Path_+"ground_distortion_shader.frag");

            CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({Vert, Frag}));

            attachShaders({Vert, Frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());

            setUniform(uniformLocation("u_tex_ground"), TexUnitGround);
            setUniform(uniformLocation("u_tex_velocities"), TexUnitVelocities);
            DistortionUniform_ = uniformLocation("u_distortion");
            DeltaTUniform_ = uniformLocation("u_dt");
            GridResolutionUniform_ = uniformLocation("u_grid_res");
            TransformationUniform_ = uniformLocation("u_matrix");
            
            setUniform(DistortionUniform_, 100.0f);
            setUniform(DeltaTUniform_, 1.0f/60.0f);
            setUniform(GridResolutionUniform_, 2.0f);
        }

        GroundDistortionShader& setDistortion(const Float f)
        {
            setUniform(DistortionUniform_, f);
            return *this;
        }

        GroundDistortionShader& setDeltaT(const Float dt)
        {
            setUniform(DeltaTUniform_, dt);
            return *this;
        }
        
        GroundDistortionShader& setGridRes(const Float r)
        {
            setUniform(GridResolutionUniform_, r);
            return *this;
        }
        
        GroundDistortionShader& setTransformation(const Matrix3& t)
        {
            setUniform(TransformationUniform_, t);
            return *this;
        }

        GroundDistortionShader& bindTextures(GL::Texture2D& TexGround,
                                             GL::Texture2D& TexVelocities)
        {
            TexGround.bind(TexUnitGround);
            TexVelocities.bind(TexUnitVelocities);
            return *this;
        }

    private:
        
        enum: Int
        {
            TexUnitGround = 0,
            TexUnitVelocities = 1
        };

        std::string Path_{SHADER_PATH};

        Float   DistortionUniform_;
        Float   DeltaTUniform_;
        Float   GridResolutionUniform_;
        Int     TransformationUniform_;
};

#endif // GROUND_DISTORTION_SHADER_H
