#ifndef DEBRIS_FACTORY_H
#define DEBRIS_FACTORY_H

#include "entity_factory.h"
#include "game_object.h"

constexpr int POOL_SIZE_DEBRIS = 10000;

typedef EntityFactory<GameObject, POOL_SIZE_DEBRIS> DebrisFactory;

#endif // DEBRIS_FACTORY_H
