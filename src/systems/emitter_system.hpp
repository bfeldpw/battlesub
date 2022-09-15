#ifndef EMITTER_SYSTEM_HPP
#define EMITTER_SYSTEM_HPP

#include <entt/entity/registry.hpp>

#include "emitter_component.hpp"
#include "fluid_source_component_lua.hpp"
#include "status_component_lua.hpp"

class EmitterSystem
{

    public:

        explicit EmitterSystem(entt::registry& _Reg) : Reg_(_Reg),
                                                       FldSrcCompLua_(_Reg),
                                                       StatusCompLua_(_Reg){}
        EmitterSystem() = delete;

        void emit();
        void loadConfig();

    private:

        entt::registry& Reg_;

        FluidSourceComponentLua FldSrcCompLua_;
        StatusComponentLua StatusCompLua_;

};

#endif // EMITTER_SYSTEM_HPP
