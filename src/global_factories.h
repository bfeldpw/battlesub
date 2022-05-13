#ifndef GLOBAL_FACTORIES_H
#define GLOBAL_FACTORIES_H

#include "entity_factory.h"
#include "submarine_factory.h"

struct GlobalFactories
{
    static SubmarineFactory  Submarines;
};

#endif // GLOBAL_FACTORIES_H
