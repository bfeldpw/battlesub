#ifndef PHYS_OBJECT_ENTITY_MAP_H
#define PHYS_OBJECT_ENTITY_MAP_H

#include <vector>

#include "phys_obj.h"

struct PhysObjEntityMap
{
    PhysObjEntityMap() {PhysObjs.resize(1000);}
    
    std::vector<PhysObj*> PhysObjs;
};

#endif // PHYS_OBJECT_ENTITY_MAP_H
