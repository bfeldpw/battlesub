#ifndef COMP_PHYSICS_H
#define COMP_PHYSICS_H

#include <Box2D/Box2D.h>

enum class ShapeEnumType : int
{
    CIRCLE,
    CHAIN,
    POLYGON
};

typedef std::vector<b2Vec2>     ShapeType;
typedef struct
{
    std::vector<ShapeType>    ShapeDefs;
    std::vector<b2FixtureDef> FixtureDefs;
    ShapeEnumType             Type = ShapeEnumType::POLYGON;

} ShapesType;

struct CompPhysics
{
        b2Body*     Body_   = nullptr;
        b2World*    World_  = nullptr;
        ShapesType* Shapes_ = nullptr;
};

#endif // COMP_PHYSICS_H
