#ifndef PROJECTILE_FACTORY_H
#define PROJECTILE_FACTORY_H

#include "entity_factory.h"
#include "game_object.h"

constexpr int POOL_SIZE_PROJECTILES = 10000;

typedef EntityFactory<GameObject, POOL_SIZE_PROJECTILES> ProjectileFactory;

#endif // PROJECTILE_FACTORY_H
