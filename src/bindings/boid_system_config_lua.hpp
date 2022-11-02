#ifndef BOID_SYSTEM_CONFIG_LUA_HPP
#define BOID_SYSTEM_CONFIG_LUA_HPP

#include <entt/entity/registry.hpp>

#include "boid_component.hpp"
#include "boid_system_config.hpp"
#include "lua_manager.hpp"
#include "message_handler.hpp"

class BoidSystemConfigLua
{
    public:

        explicit BoidSystemConfigLua (entt::registry& _Reg) : Reg_(_Reg) {}
        BoidSystemConfigLua() = delete;

        void read(const std::string& _s)
        {
            auto& Lua = Reg_.ctx().at<LuaManager>();
            Conf_.n = Lua.get<int>(Lua.State_[_s]["n"]);
            Conf_.NeighboursMax = Lua.get<int>(Lua.State_[_s]["neighbours_max"]);
            if (Conf_.NeighboursMax > BoidComponent::NEIGHBOURS_MAX)
            {
                Reg_.ctx().at<MessageHandler>().report("bds", "Too many neighbours, restricting to 32", MessageHandler::WARNING);
                Conf_.NeighboursMax = BoidComponent::NEIGHBOURS_MAX;
            }
            Conf_.GridSizeX = Lua.get<int>(Lua.State_[_s]["grid_size_x"]);
            Conf_.GridSizeY = Lua.get<int>(Lua.State_[_s]["grid_size_y"]);

            Conf_.BoidForce = Lua.get<float>(Lua.State_[_s]["boid_force"]);
            Conf_.BoidTorqueMax = Lua.get<float>(Lua.State_[_s]["boid_torque_max"]);
            Conf_.BoidVelMax = Lua.get<float>(Lua.State_[_s]["boid_vel_max"]);

            Conf_.IsBoidDebugActive = Lua.get<bool>(Lua.State_[_s]["boid_debug"]);
        }

        const BoidSystemConfig& get() const
        {
            return Conf_;
        }

        void set(const BoidSystemConfig& _c)
        {
            Conf_ = _c;
        }

    private:

        entt::registry& Reg_;

        BoidSystemConfig Conf_;
};

#endif // BOID_SYSTEM_CONFIG_LUA_HPP
