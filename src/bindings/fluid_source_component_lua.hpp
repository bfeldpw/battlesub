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
            Conf.DensityBackProjection_ = Lua.get<float>(Lua.State_[_s]["fluid_source_component"]["density_back_projection"]);
            Conf.DensityDynamicR_ = Lua.get<float>(Lua.State_[_s]["fluid_source_component"]["density_dynamic_r"]);
            Conf.DensityDynamicG_ = Lua.get<float>(Lua.State_[_s]["fluid_source_component"]["density_dynamic_g"]);
            Conf.DensityDynamicB_ = Lua.get<float>(Lua.State_[_s]["fluid_source_component"]["density_dynamic_b"]);
            Conf.DensityStaticR_ = Lua.get<float>(Lua.State_[_s]["fluid_source_component"]["density_static_r"]);
            Conf.DensityStaticG_ = Lua.get<float>(Lua.State_[_s]["fluid_source_component"]["density_static_g"]);
            Conf.DensityStaticB_ = Lua.get<float>(Lua.State_[_s]["fluid_source_component"]["density_static_b"]);
            Conf.VelocityBackProjection_ = Lua.get<float>(Lua.State_[_s]["fluid_source_component"]["velocity_back_projection"]);
        }
        void copyTo(FluidSourceComponent& _c)
        {
            _c = Conf;
        }

        FluidSourceComponent Conf;

    private:

        entt::registry& Reg_;
};

#endif // FLUID_SOURCE_COMPONENT_LUA_HPP
