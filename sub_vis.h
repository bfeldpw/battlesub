#ifndef SUB_VIS_H
#define SUB_VIS_H

#include "entity.h"
#include "entity_hook.h"
#include "phys_obj_entity_map.h"
#include "sub_drawable.h"

class SubVis : public Entity, EntityHook
{
    public:
        
        SubVis(EntityIDType IDHook,
               PhysObjEntityMap& EntityMap,
               Scene2D& Scene,
               Color4 Color,
               GL::Mesh& Mesh,
               Shaders::Flat2D& Shader,
               SceneGraph::DrawableGroup2D& Drawables
              ) : EntityHook(IDHook), EntityMap_(EntityMap), Vis_(&Scene)
        {
            std::cout << "SubVis::SubVis()\n  ID hook: " << IDHook << std::endl;
            std::cout << "  ID: " << ID << std::endl;
            
//             Vis_.setScaling({1.0f, 1.0f});
            
            new SubDrawable(Vis_, Mesh, Shader, Color, Drawables);
        }
        
        void update()
        {
            
            const auto Body = EntityMap_.PhysObjs[this->getHookID()]->getBody();
            
            if (Body->IsActive())
            {
                Vis_.setTranslation({Body->GetPosition().x, Body->GetPosition().y})
                    .setRotation(Complex::rotation(Rad(Body->GetAngle())));
            }
        }
        
    private:
        
        Object2D Vis_;
        PhysObjEntityMap& EntityMap_;
};

#endif // SUB_VIS_H
