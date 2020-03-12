#ifndef GLOBAL_FACTORIES_H
#define GLOBAL_FACTORIES_H

#include "debris_factory.h"
#include "landscape_factory.h"
#include "projectile_factory.h"
#include "submarine_factory.h"

struct GlobalFactories
{
    static DebrisFactory     Debris;
    static LandscapeFactory  Landscapes;
    static ProjectileFactory Projectiles;
    static SubmarineFactory  Submarines;
};

#endif // GLOBAL_FACTORIES_H
