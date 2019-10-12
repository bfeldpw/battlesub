#ifndef ENTITY_HOOK_H
#define ENTITY_HOOK_H

#include "entity.h"

class EntityHook
{
    public:
        
        EntityHook(EntityIDType ID) : ID_(ID) {}
        
        EntityIDType  getHookID() const {return ID_;}
        void          setHookID(EntityIDType ID) {ID_ = ID;}
    
    private:
        
        EntityIDType ID_ = 0;
        
};

#endif // ENTITY_HOOK_H
