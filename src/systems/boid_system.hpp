#ifndef BOID_SYSTEM_HPP
#define BOID_SYSTEM_HPP

#include <array>
#include <random>
#include <vector>

#include <entt/entity/registry.hpp>

#include "boid_system_config.hpp"
#include "boid_system_config_lua.hpp"
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

class BoidSystem
{
    public:

        explicit BoidSystem(entt::registry& Reg) : Reg_(Reg), Conf_(Reg) {}
        BoidSystem() = delete;

        const BoidSystemConfig& getConfig() const {return Conf_.get();}
        void setConfig(const BoidSystemConfig& _Conf) {Conf_.set(_Conf); this->resetBoidDebug();}

        void init();
        void loadConfig();
        void update();

    private:

        entt::registry& Reg_;

        BoidSystemConfigLua Conf_;

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
        void applyConfinement();

        // Visual boid debugging
        entt::entity EntityDebug_;

        void resetBoidDebug();
};

#endif // BOID_SYSTEM_HPP
