#ifndef LUA_MANAGER_HPP
#define LUA_MANAGER_HPP

#include <string>

#include "lua_path.hpp"
#include "message_handler.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include <limits> // Needed for sol, seemed to be forgotten
#include "sol.hpp"

#include <entt/entity/registry.hpp>

class LuaManager
{
    public:

        explicit LuaManager(entt::registry& _Reg) : Reg_(_Reg)
        {
            State_.open_libraries(sol::lib::base);
        }
        LuaManager() = delete;

        bool loadFile(const std::string& _f)
        {
            sol::protected_function_result r = State_.safe_script_file(LUA_PATH+_f);
            if (r.valid())
            {
                Reg_.ctx().at<MessageHandler>().report("lua", "Loading "+_f+" successful", MessageHandler::INFO);
                return true;
            }
            else
            {
                sol::error e = r;
                Reg_.ctx().at<MessageHandler>().report("lua","Loading "+_f+" failed: "+e.what());
                return false;
            }
        }

        template<class T>
        T get(const sol::optional<T>& _r, T _Default = 0)
        {
            if (_r)
            {
                return _r.value();
            }
            else
            {
                Reg_.ctx().at<MessageHandler>().report("lua", "Lua error aquiring value.");
                return _Default;
            }
        }

        sol::state State_;

    private:

        entt::registry& Reg_;
};

#endif // LUA_MANAGER_HPP
