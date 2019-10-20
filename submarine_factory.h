#ifndef SUBMARINE_FACTORY_H
#define SUBMARINE_FACTORY_H

#include "game_object_factory.h"
#include "sub.h"

constexpr int POOL_SIZE_SUBS = 5;

typedef GameObjectFactory<Sub, POOL_SIZE_SUBS> SubmarineFactory;

SubmarineFactory GlobalSubmarineFactory;

#endif // SUBMARINE_FACTORY_H
