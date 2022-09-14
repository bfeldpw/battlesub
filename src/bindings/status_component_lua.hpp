#ifndef STATUS_COMPONENT_LUA_HPP
#define STATUS_COMPONENT_LUA_HPP

#include <entt/entity/registry.hpp>

#include "status_component.hpp"
#include "lua_manager.hpp"

class StatusComponentLua
{
    public:

        explicit StatusComponentLua(entt::registry& _Reg) : Reg_(_Reg) {}
        StatusComponentLua() = delete;

        void read(const std::string& _s)
        {
            auto& Lua = Reg_.ctx().at<LuaManager>();
            Conf_.AgeMax_ = Lua.get<float>(Lua.State_[_s]["status_component"]["age_max"]);
        }
        void copyTo(StatusComponent& _c)
        {
            _c = Conf_;
        }
        StatusComponent Conf_;

    private:

        entt::registry& Reg_;
};

#endif // STATUS_COMPONENT_LUA_HPP
