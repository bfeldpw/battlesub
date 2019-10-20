#ifndef SUBMARINE_FACTORY_H
#define SUBMARINE_FACTORY_H

#include "memory_pool.h"
#include "sub.h"

constexpr int POOL_SIZE_SUBS = 5;

class SubmarineFactory
{
    public:
        
        Sub* createSubmarine()
        {
            Sub* NewSub = SubPool.allocate();
            return NewSub;
        }
        
        void destroySubmarine(Sub* S)
        {
            S->destroy();
            SubPool.free(S);
        }
        
    private:
        
        MemoryPool<Sub, POOL_SIZE_SUBS> SubPool;
        
};

/*static */SubmarineFactory GlobalSubmarineFactory;

#endif // SUBMARINE_FACTORY_H
