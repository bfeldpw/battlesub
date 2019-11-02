#ifndef SUBMARINE_FACTORY_H
#define SUBMARINE_FACTORY_H

#include "game_object_factory.h"
#include "submarine.h"

constexpr int POOL_SIZE_SUBS = 5;

typedef GameObjectFactory<Submarine, POOL_SIZE_SUBS> SubmarineFactory;

#endif // SUBMARINE_FACTORY_H
