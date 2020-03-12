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

#endif // ENTITY_H
