#ifndef GAMEOBJECT_FACTORY_H
#define GAMEOBJECT_FACTORY_H

#include <unordered_map>

#include "memory_pool.h"

template<class T, size_t S>
class GameObjectFactory
{
    public:
        
        const std::unordered_map<EntityIDType, T*>& getEntities() const
        {
            return Entities;
        }
        
        
        T* create()
        {
            T* NewEntity = EntityPool.allocate();
            Entities[NewEntity->ID] = NewEntity;
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

#endif // GAMEOBJECT_FACTORY_H
