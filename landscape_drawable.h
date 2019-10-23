#ifndef LANDSCAPE_DRAWABLE_H
#define LANDSCAPE_DRAWABLE_H

#include "common.h"

class LandscapeDrawable: public SceneGraph::Drawable2D
{

    public:
        
        explicit LandscapeDrawable(Object2D& Obj,
                                   GL::Mesh& Mesh,
                                   Shaders::Flat2D& Shader,
                                   const Color4& Color,
                                   SceneGraph::DrawableGroup2D& Drawables) : 
                                   SceneGraph::Drawable2D{Obj, &Drawables}, Mesh_(Mesh), Shader_(Shader), Color_{Color}
        {
//             const struct
//             {
//                 Vector2 pos;
//             } data[]{{{-0.1f, 0.0f}},
//                     {{  0.1f, 0.0f}},
//                     {{ -0.1f, 0.8f}},
//                     {{ -0.1f, 0.8f}},
//                     {{  0.1f, 0.0f}},
//                     {{  0.1f, 0.8f}},
//                     {{  0.1f, 0.8f}},
//                     {{  0.0f, 1.0f}},
//                     {{ -0.1f, 0.8f}}};
// 
// //             GL::Mesh Mesh;
//             GL::Buffer buffer;
//             buffer.setData(data, GL::BufferUsage::StaticDraw);
//             MeshProjectile_ = GL::Mesh{};
//             MeshProjectile_.setCount(9)
//                 .addVertexBuffer(std::move(buffer), 0,
//                     Shaders::VertexColor2D::Position{});
        }

private:
        
        void draw(const Matrix3& TransformationMatrix, SceneGraph::Camera2D& Camera) override
        {
            Shader_
                .setTransformationProjectionMatrix(Camera.projectionMatrix()*TransformationMatrix)
                .setColor(Color_);
            Mesh_.draw(Shader_);
        }

        GL::Mesh&           Mesh_;
        Shaders::Flat2D&    Shader_;
        Color4              Color_;
        
};

#endif // LANDSCAPE_DRAWABLE_H
