#ifndef CONTACT_LISTENER_H
#define CONTACT_LISTENER_H

#include <Box2D/Box2D.h>
#include <entt/entity/registry.hpp>

#include "global_emitter_factory.h"

class ContactListener : public b2ContactListener
{

    public:

        ContactListener(entt::registry& _Reg) : Reg_{_Reg} {}

        void PreSolve(b2Contact* Contact, const b2Manifold* OldManifold) override;

    private:

        void emitLandscapeDebris(b2Contact* const _Contact,
                                 b2Body* const _Body1,
                                 b2Body* const _Body2);
        void emitSubmarineDebris(b2Contact* const _Contact,
                                 b2Body* const _Body1,
                                 b2Body* const _Body2,
                                 const GameObjectTypeE _TypeOther);
        void testGameObjectTypes(b2Contact* const _Contact,
                                 const GameObjectTypeE _Type1,
                                 const GameObjectTypeE _Type2,
                                 b2Body* const _Body1,
                                 b2Body* const _Body2);

        entt::registry& Reg_;

};

#endif // CONTACT_LISTENER_H
