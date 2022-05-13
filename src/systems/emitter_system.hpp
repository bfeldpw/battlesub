#ifndef EMITTER_SYSTEM_HPP
#define EMITTER_SYSTEM_HPP

#include <entt/entity/registry.hpp>

#include "emitter_component.hpp"

class EmitterSystem
{

    public:

        explicit EmitterSystem(entt::registry& _Reg) : Reg_(_Reg) {}
        EmitterSystem() = delete;

        void emit();

    private:

        entt::registry& Reg_;

};

#endif // EMITTER_SYSTEM_HPP
