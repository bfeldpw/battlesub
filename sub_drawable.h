#ifndef SUB_DRAWABLE_H
#define SUB_DRAWABLE_H

#include "common.h"

class SubDrawable: public SceneGraph::Drawable2D
{

    public:
        
        explicit SubDrawable(Object2D& Obj,
                             GL::Mesh& Mesh,
                             Shaders::Flat2D& Shader,
                             const Color4& Color,
                             SceneGraph::DrawableGroup2D& Drawables) : 
                             SceneGraph::Drawable2D{Obj, &Drawables}, Mesh_(Mesh), Shader_(Shader), Color_{Color} {}

    private:
        
        void draw(const Matrix3& TransformationMatrix, SceneGraph::Camera2D& Camera) override
        {
            Shader_
                .setTransformationProjectionMatrix(Camera.projectionMatrix()*TransformationMatrix)
                .setColor(Color_);
            Mesh_.draw(Shader_);
        }

        Color4              Color_;
        GL::Mesh&           Mesh_;
        Shaders::Flat2D&    Shader_;
        
};

#endif // SUB_DRAWABLE_H
