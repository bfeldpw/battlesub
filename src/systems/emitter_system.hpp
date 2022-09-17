#ifndef EMITTER_SYSTEM_HPP
#define EMITTER_SYSTEM_HPP

#include <entt/entity/registry.hpp>

#include "emitter_component.hpp"
#include "fluid_source_component.hpp"
#include "fluid_source_component_lua.hpp"
#include "status_component.hpp"
#include "status_component_lua.hpp"

class EmitterSystem
{

    public:

        explicit EmitterSystem(entt::registry& _Reg) : Reg_(_Reg),
                                                       FldSrcCompLua_(_Reg),
                                                       StatusCompLua_(_Reg){}
        EmitterSystem() = delete;

        const FluidSourceComponent& getConfigDebrisFluidSource() const {return FldSrcCompLua_.get();}
        const StatusComponent& getConfigDebrisStatus() const {return StatusCompLua_.get();}

        void emit();
        void loadConfigDebris();
        void setConfigDebrisFluidSource(const FluidSourceComponent& _c) {FldSrcCompLua_.set(_c);}
        void setConfigDebrisStatus(const StatusComponent& _c) {StatusCompLua_.set(_c);}

    private:

        entt::registry& Reg_;

        FluidSourceComponentLua FldSrcCompLua_;
        StatusComponentLua StatusCompLua_;

};

#endif // EMITTER_SYSTEM_HPP
