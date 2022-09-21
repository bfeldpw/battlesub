#ifndef BOID_SYSTEM_HPP
#define BOID_SYSTEM_HPP

#include <unordered_map>

#include <entt/entity/registry.hpp>

#include "message_handler.hpp"

struct BoidComponent
{
    float Test_;
};

struct BoidSystemConfig
{
    int n{100};
    int CellGridX{64};
    int CellGridY{32};
};

class BoidSystem
{
    public:

        explicit BoidSystem(entt::registry& Reg) : Reg_(Reg) {}
        BoidSystem() = delete;

        void init()
        {
            for (auto i=0; i < Conf_.n; ++i)
            {
                auto Boid = Reg_.create();
                Reg_.emplace<BoidComponent>(Boid);
            }
            Reg_.ctx().at<MessageHandler>().report("bds", "Populated world with "+std::to_string(Conf_.n)+" fish", MessageHandler::INFO);
        }

        void update()
        {
            // Update cells
            Reg_.view<BoidComponent>().each(
                [this](auto _e, auto& _BoidComp)
                {
                    // Cells_.insert({22,_e});
                });
        }

    private:

        entt::registry& Reg_;

        BoidSystemConfig Conf_;
        std::unordered_multimap<int, entt::entity> Cells_;
};

#endif // BOID_SYSTEM_HPP
