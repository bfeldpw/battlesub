#ifndef PROJECTILE_DRAWABLE_H
#define PROJECTILE_DRAWABLE_H

#include "common.h"

class ProjectileDrawable: public SceneGraph::Drawable2D
{

    public:
        
        explicit ProjectileDrawable(Object2D* Obj,
                                    GL::Mesh* Mesh,
                                    Shaders::Flat2D* Shader,
                                    const Color4& Color,
                                    SceneGraph::DrawableGroup2D* Drawables) : 
                                    SceneGraph::Drawable2D{*Obj, Drawables}, Mesh_(Mesh), Shader_(Shader), Color_{Color} {}
                                    
        void setColor(const Color4 Color)
        {
            Color_ = Color;
        }

    private:
        
        void draw(const Matrix3& TransformationMatrix, SceneGraph::Camera2D& Camera) override
        {
            assert(Mesh_ != nullptr);
            assert(Shader_ != nullptr);
            Shader_
                ->setTransformationProjectionMatrix(Camera.projectionMatrix()*TransformationMatrix)
                 .setColor(Color_);
            Mesh_->draw(*Shader_);
        }

        GL::Mesh*           Mesh_;
        Shaders::Flat2D*    Shader_;
        Color4              Color_;
        
};

#endif // PROJECTILE_DRAWABLE_H
