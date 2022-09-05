#ifndef GAME_OBJECT_FACTORY_HPP
#define GAME_OBJECT_FACTORY_HPP

#include <any>

#include <box2d.h>
#include <entt/entity/registry.hpp>

#include "drawable_generic.h"
#include "global_resources.h"
#include "physics_component.hpp"
#include "status_component.hpp"
#include "visuals_component.hpp"


class GameObjectFactory
{
    public:

        explicit GameObjectFactory(entt::registry& Reg) : Reg_(Reg) {}
        GameObjectFactory() = delete;

        void create(entt::entity e, std::any _Parent, const GameObjectTypeE _GameObjectType,
                    const int _AgeMax,
                    const DrawableGroupsTypeE _DrawableGroupsType, Color4 _Col,
                    const b2BodyDef& _BodyDef);

        void createShapes(b2Body* Body, ShapesType* Shapes);
        void updateStatus();
        void updateVisuals();

    private:

        entt::registry& Reg_;

        // A static memory storage for entities is needed in order to have a
        // consistent storage for SetUserData of Box2D. AFAIK, there is no
        // way to get a persistent memory address from entt::registry. Hence,
        // an intermediate structure is needed to allow for user data if using
        // only entities.
        struct
        {
            std::array<entt::id_type, 10000> IDs_;
            std::array<entt::id_type, 10000> IDMap_;
            std::array<entt::id_type, 10000> FreeIDs_;
            int Count_ = 0;
            int CountFree_ = 0;
        } EntityIDs;

        entt::id_type id(entt::entity _e) {return entt::id_type(Reg_.entity(_e));}
        DBLK(
            void printInternalEntityLists() const;
        )
};

#endif // GAME_OBJECT_FACTORY_H
