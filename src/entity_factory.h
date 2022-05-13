#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include <unordered_map>

#include "entity.h"
#include "memory_pool.h"

template<class T, size_t S>
class EntityFactory
{
    public:
        
        const std::unordered_map<EntityIDType, T*>& getEntities() const
        {
            return Entities;
        }
        
        T* create()
        {
            T* NewEntity = EntityPool.allocate();
            if (NewEntity != nullptr) Entities[NewEntity->ID] = NewEntity;
            return NewEntity;
        }
        
        void destroy(T* P)
        {
            P->destroy();
            Entities.erase(P->ID);
            EntityPool.free(P);
        }
        
    private:
        
        MemoryPool<T, S>                     EntityPool;
        std::unordered_map<EntityIDType, T*> Entities;
        
};

#endif // ENTITY_FACTORY_H
