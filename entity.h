#ifndef ENTITY_H
#define ENTITY_H

typedef int EntityIDType;

class Entity
{
    public:
        
        Entity() : ID(s_IDMax++) {}
        
        EntityIDType ID = 0;
        
    
    private:
        
        static EntityIDType s_IDMax;
        
};

EntityIDType Entity::s_IDMax = 1;

#endif // ENTITY_H
