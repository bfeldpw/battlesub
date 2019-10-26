#ifndef PROJECTILE_FACTORY_H
#define PROJECTILE_FACTORY_H

#include "game_object_factory.h"
#include "projectile.h"

constexpr int POOL_SIZE_PROJECTILES = 10000;

typedef GameObjectFactory<Projectile, POOL_SIZE_PROJECTILES> ProjectileFactory;

#endif // PROJECTILE_FACTORY_H
