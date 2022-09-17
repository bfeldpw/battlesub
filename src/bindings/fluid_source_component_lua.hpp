#ifndef FLUID_SOURCE_COMPONENT_LUA_HPP
#define FLUID_SOURCE_COMPONENT_LUA_HPP

#include <entt/entity/registry.hpp>

#include "fluid_source_component.hpp"
#include "lua_manager.hpp"

class FluidSourceComponentLua
{
    public:

        explicit FluidSourceComponentLua(entt::registry& _Reg) : Reg_(_Reg) {}
        FluidSourceComponentLua() = delete;

        void read(const std::string& _s)
        {
            auto& Lua = Reg_.ctx().at<LuaManager>();
            Conf_.DensityBackProjection_ = Lua.get<float>(Lua.State_[_s]["fluid_source_component"]["density_back_projection"]);
            Conf_.DensityDynamicR_ = Lua.get<float>(Lua.State_[_s]["fluid_source_component"]["density_dynamic_r"]);
            Conf_.DensityDynamicG_ = Lua.get<float>(Lua.State_[_s]["fluid_source_component"]["density_dynamic_g"]);
            Conf_.DensityDynamicB_ = Lua.get<float>(Lua.State_[_s]["fluid_source_component"]["density_dynamic_b"]);
            Conf_.DensityStaticR_ = Lua.get<float>(Lua.State_[_s]["fluid_source_component"]["density_static_r"]);
            Conf_.DensityStaticG_ = Lua.get<float>(Lua.State_[_s]["fluid_source_component"]["density_static_g"]);
            Conf_.DensityStaticB_ = Lua.get<float>(Lua.State_[_s]["fluid_source_component"]["density_static_b"]);
            Conf_.VelocityBackProjection_ = Lua.get<float>(Lua.State_[_s]["fluid_source_component"]["velocity_back_projection"]);
        }

        const FluidSourceComponent& get() const
        {
            return Conf_;
        }

        void set(const FluidSourceComponent& _c)
        {
            Conf_ = _c;
        }

    private:

        entt::registry& Reg_;

        FluidSourceComponent Conf_;
};

#endif // FLUID_SOURCE_COMPONENT_LUA_HPP
