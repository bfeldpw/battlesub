#ifndef STATUS_COMPONENT
#define STATUS_COMPONENT

#include "timer.h" 

enum class GameObjectTypeE : int
{
    DEBRIS_LANDSCAPE,
    DEBRIS_SUBMARINE,
    LANDSCAPE,
    PROJECTILE,
    SUBMARINE_HULL,
    SUBMARINE_RUDDER,
    DEFAULT
};

struct StatusComponent
{
    GameObjectTypeE Type_ = GameObjectTypeE::DEFAULT;
    Timer Age_;
    bool  IsSinking_ = false;
    bool  IsSunk_ = false;
    bool  IsToBeDeleted_ = false;
};

#endif // STATUS_COMPONENT
