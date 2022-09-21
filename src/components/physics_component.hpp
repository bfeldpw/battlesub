#ifndef PHYSICS_COMPONENT_HPP
#define PHYSICS_COMPONENT_HPP

#include <box2d.h>

struct PhysicsComponent
{
    b2Body*      Body_   = nullptr;
};

#endif // PHYSICS_COMPONENT_HPP
