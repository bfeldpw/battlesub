#ifndef LANDSCAPE_FACTORY_H
#define LANDSCAPE_FACTORY_H

#include "entity_factory.h"
#include "game_object.h"

constexpr int POOL_SIZE_LANDSCAPES = 100;

typedef EntityFactory<GameObject, POOL_SIZE_LANDSCAPES> LandscapeFactory;

#endif // LANDSCAPE_FACTORY_H
