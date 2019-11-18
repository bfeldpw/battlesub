#ifndef LANDSCAPE_FACTORY_H
#define LANDSCAPE_FACTORY_H

#include "game_object_factory.h"

constexpr int POOL_SIZE_LANDSCAPES = 100;

typedef GameObjectFactory<GameObject, POOL_SIZE_LANDSCAPES> LandscapeFactory;

#endif // LANDSCAPE_FACTORY_H
