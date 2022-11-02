#ifndef BOID_COMPONENT_HPP
#define BOID_COMPONENT_HPP

#include <cstdint>

struct BoidComponent
{
    // Keep arrays constant size to support ECS performance
    static constexpr int NEIGHBOURS_MAX{128};

    uint32_t Neighbours[NEIGHBOURS_MAX];
    uint32_t NrOfNeighbours{0};

    float NeighbourPosAvgX{0.f};
    float NeighbourPosAvgY{0.f};
};

#endif // BOID_COMPONENT_HPP
