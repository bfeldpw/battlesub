#ifndef PROJECTILE_FACTORY_H
#define PROJECTILE_FACTORY_H

#include "memory_pool.h"
#include "projectile.h"

constexpr int POOL_SIZE_PROJECTILES = 10000;

class ProjectileFactory
{
    public:
        
        Projectile* createProjectile()
        {
            Projectile* NewProjectile = ProjectilePool.allocate();
            return NewProjectile;
        }
        
        void destroyProjectile(Projectile* P)
        {
            ProjectilePool.free(P);
        }
        
    private:
        
        MemoryPool<Projectile, POOL_SIZE_PROJECTILES>   ProjectilePool;
        
};

/*static */ProjectileFactory GlobalProjectileFactory;

#endif // PROJECTILE_FACTORY_H
