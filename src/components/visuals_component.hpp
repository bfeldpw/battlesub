#ifndef VISUALS_COMPONENT
#define VISUALS_COMPONENT

#include <Magnum/GL/Mesh.h>
#include <vector>

#include "drawable_generic.h"

typedef std::vector<GL::Mesh>   MeshesType;

struct VisualsComponent
{
    MeshesType*                     Meshes_         = nullptr;
    Shaders::Flat2D*                Shader_         = nullptr;
    Object2D*                       Visuals_        = nullptr;
    Scene2D*                        Scene_          = nullptr;
    DrawableGeneric*                Drawable_       = nullptr;
    SceneGraph::DrawableGroup2D*    DrawableGrp_    = nullptr;
};

#endif // VISUALS_COMPONENT
