#ifndef DEBRIS_FACTORY_H
#define DEBRIS_FACTORY_H

#include "game_object_factory.h"
#include "debris.h"

constexpr int POOL_SIZE_DEBRIS = 10000;

typedef GameObjectFactory<Debris, POOL_SIZE_DEBRIS> DebrisFactory;

#endif // DEBRIS_FACTORY_H
