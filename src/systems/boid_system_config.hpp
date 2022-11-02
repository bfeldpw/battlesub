#ifndef BOID_SYSTEM_CONFIG_HPP
#define BOID_SYSTEM_CONFIG_HPP

struct BoidSystemConfig
{
    int n{512};
    int NeighboursMax{128};
    int GridSizeX{128};
    int GridSizeY{64};

    float BoidForce{10.0f};
    float BoidTorqueMax{0.05f};
    float BoidVelMax{5.0f};

    bool IsBoidDebugActive{true};
};

#endif // BOID_SYSTEM_CONFIG_HPP
