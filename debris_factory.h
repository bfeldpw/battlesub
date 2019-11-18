#ifndef DEBRIS_FACTORY_H
#define DEBRIS_FACTORY_H

#include "game_object.h"
#include "game_object_factory.h"

constexpr int POOL_SIZE_DEBRIS = 10000;

typedef GameObjectFactory<GameObject, POOL_SIZE_DEBRIS> DebrisFactory;

#endif // DEBRIS_FACTORY_H
