#ifndef VISUALS_COMPONENT
#define VISUALS_COMPONENT

#include <Magnum/GL/Mesh.h>

#include <vector>

// TODO Replace vector, no dynamic allocations within components
typedef std::vector<GL::Mesh>   MeshesType;

struct VisualsComponent
{
    MeshesType*                     Meshes_         = nullptr;
    Shaders::Flat2D*                Shader_         = nullptr;
    Object2D*                       Visuals_        = nullptr;
    Scene2D*                        Scene_          = nullptr;
    Color4                          Color_{1.0f, 1.0f, 1.0f, 1.0f};
    DrawableGeneric*                Drawable_       = nullptr;
    SceneGraph::DrawableGroup2D*    DrawableGrp_    = nullptr;
    float                           ScaleX_         = 1.0f;
    float                           ScaleY_         = 1.0f;
    float                           ScaleSinkColor  = 1.0f;
};

#endif // VISUALS_COMPONENT
