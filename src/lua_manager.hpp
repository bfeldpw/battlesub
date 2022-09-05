#ifndef LUA_MANAGER_HPP
#define LUA_MANAGER_HPP

#include <string>

#include "error_handler.h"
#include "lua_path.hpp"
#include "message_handler.h"

#define SOL_ALL_SAFETIES_ON 1
#include <limits> // Needed for sol, seemed to be forgotten
#include "sol.hpp"

#include <entt/entity/registry.hpp>

class LuaManager
{
    public:

        explicit LuaManager(entt::registry& _Reg) : Reg_(_Reg)
        {
            Lua_.open_libraries(sol::lib::base);
        }

        bool loadFile(const std::string& _f)
        {
            sol::protected_function_result r = Lua_.safe_script_file(LUA_PATH+_f);
            if (r.valid())
            {
                Reg_.ctx<MessageHandler>().report("Loading "+_f+" successful");
                return true;
            }
            else
            {
                sol::error e = r;
                Reg_.ctx<ErrorHandler>().reportError("Loading "+_f+" failed: "+e.what());
                return false;
            }
        }

        int getInt(sol::protected_function_result _r)
        {
            if (_r.valid())
            {
                int r = _r;
                return r;
            }
            else
            {
                sol::error e = _r;
                Reg_.ctx<ErrorHandler>().reportError("Lua error aquiering value: " + std::string(e.what()));
                return 0;
            }
        }

        sol::state Lua_;

    private:

        entt::registry& Reg_;
};

#endif // LUA_MANAGER_HPP
