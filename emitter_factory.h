#ifndef EMITTER_FACTORY_H
#define EMITTER_FACTORY_H

#include "emitter.h"
#include "game_object_factory.h"

constexpr int POOL_SIZE_EMITTERS = 100;

typedef GameObjectFactory<Emitter, POOL_SIZE_EMITTERS> EmitterFactory;

#endif // EMITTER_FACTORY_H
