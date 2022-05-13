#ifndef SUBMARINE_FACTORY_H
#define SUBMARINE_FACTORY_H

#include "entity.h"
#include "submarine.h"

constexpr int POOL_SIZE_SUBS = 5;

typedef EntityFactory<Submarine, POOL_SIZE_SUBS> SubmarineFactory;

#endif // SUBMARINE_FACTORY_H
