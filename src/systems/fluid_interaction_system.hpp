#ifndef FLUID_INTERACTION_SYSTEM_HPP
#define FLUID_INTERACTION_SYSTEM_HPP

#include <entt/entity/registry.hpp>

#include "fluid_grid.h"

class FluidInteractionSystem
{

    public:

        explicit FluidInteractionSystem(entt::registry& _Reg, FluidGrid& _FluidGrid) : Reg_(_Reg),
                                                                                       FluidGrid_(_FluidGrid) {}
        FluidInteractionSystem() = delete;

        void addSources();
        void applyForces();


    private:

        entt::registry& Reg_;
        FluidGrid& FluidGrid_;

};

#endif // FLUID_INTERACTION_SYSTEM_HPP
