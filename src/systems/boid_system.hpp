#ifndef BOID_SYSTEM_HPP
#define BOID_SYSTEM_HPP

#include <array>
#include <random>
#include <vector>

#include <entt/entity/registry.hpp>

#include "fluid_interaction_system.hpp"
#include "fluid_probes_component.hpp"
#include "fluid_source_component.hpp"
#include "game_object_system.hpp"
#include "message_handler.hpp"
#include "status_component.hpp"
#include "physics_component.hpp"
#include "visuals_component.hpp"
#include "world_def.h"

typedef std::array<int,9> NeighbourArrayType;

struct BoidComponent
{
    // Keep arrays constant size to support ECS performance
    static constexpr int NEIGHBOURS_MAX{32};

    uint32_t Neighbours[NEIGHBOURS_MAX];
    uint32_t NrOfNeighbours{0};

    float NeighbourPosAvgX{0.f};
    float NeighbourPosAvgY{0.f};
};

struct BoidSystemConfig
{
    int n{512};
    int GridSizeX{256};
    int GridSizeY{128};
};

class BoidSystem
{
    public:

        explicit BoidSystem(entt::registry& Reg) : Reg_(Reg) {}
        BoidSystem() = delete;

        void init();
        void update();

    private:

        entt::registry& Reg_;

        BoidSystemConfig Conf_;

        std::mt19937 Generator_;

        std::vector<uint32_t> GridEntities_;
        std::vector<int> GridCount_;

        int getGridIndexFromFloatPosition(float _x, float _y);
        NeighbourArrayType& getGridNeighbourIndecesFromIndex(int _i);
        void updateGrid();
        void updateNeighbours();
        void updateLocalFeatures();
        void applySeparation();
        void applyAlignment();
        void applyCohesion();

        // Visual boid debugging
        bool IsBoidDebugActive_{true};
        entt::entity EntityDebug_;

        void resetBoidDebug();
};

#endif // BOID_SYSTEM_HPP
