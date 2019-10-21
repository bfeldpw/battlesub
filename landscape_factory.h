#ifndef LANDSCAPE_FACTORY_H
#define LANDSCAPE_FACTORY_H

#include "game_object_factory.h"
#include "landscape.h"

constexpr int POOL_SIZE_LANDSCAPES = 100;

typedef GameObjectFactory<Landscape, POOL_SIZE_LANDSCAPES> LandscapeFactory;

LandscapeFactory GlobalLandscapeFactory;

#endif // LANDSCAPE_FACTORY_H
