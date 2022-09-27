#ifndef BOID_SYSTEM_HPP
#define BOID_SYSTEM_HPP

#include <array>
#include <random>

#include <entt/entity/registry.hpp>

#include "fluid_source_component.hpp"
#include "game_object_system.hpp"
#include "message_handler.hpp"
#include "status_component.hpp"
#include "physics_component.hpp"
#include "visuals_component.hpp"

struct BoidComponent
{
    float Test_;
};

struct BoidSystemConfig
{
    int n{999};
    int CellGridX{32};
    int CellGridY{16};
};

class BoidSystem
{
    public:

        explicit BoidSystem(entt::registry& Reg) : Reg_(Reg) {}
        BoidSystem() = delete;

        void init()
        {
            std::normal_distribution<float> DistPos(0.0, 10.0f);

            for (auto i=0; i < Conf_.n; ++i)
            {
                auto Boid = Reg_.create();
                Magnum::Math::Color3 Col = {0.7f, 0.7f, 0.3f};
                b2BodyDef BodyDef;
                BodyDef.type = b2_dynamicBody;
                BodyDef.enabled = true;
                BodyDef.angularDamping = 1.0f;
                BodyDef.linearDamping = 0.0f;
                BodyDef.position.Set(200.f+DistPos(Generator_), 20.f+DistPos(Generator_));
                Reg_.ctx().at<GameObjectSystem>().create(Boid, this, GameObjectTypeE::BOID, -1,
                                                      DrawableGroupsTypeE::WEAPON, Col, BodyDef);
                Reg_.emplace<BoidComponent>(Boid);
                Reg_.emplace<FluidSourceComponent>(Boid);


                auto& Vis = Reg_.get<VisualsComponent>(Boid);
                Vis.Scale_ = 1.0f+DistPos(Generator_)*0.025f;
                Vis.Visuals_->setScaling({Vis.Scale_, Vis.Scale_});
            }
            Reg_.ctx().at<MessageHandler>().report("bds", "Populated world with "+std::to_string(Conf_.n)+" fish", MessageHandler::INFO);
        }

        void update()
        {
            std::normal_distribution<float> DistPos(0.0, 0.01);

            // Update cells
            Reg_.view<BoidComponent, PhysicsComponent>().each(
                [&,this](auto _e, auto& _BoidComp, auto& _PhysComp)
                {
                    _PhysComp.Body_->ApplyTorque(DistPos(Generator_), true);
                    _PhysComp.Body_->ApplyForceToCenter({_PhysComp.Body_->GetWorldVector({0.f, 0.1f})}, true);
                    auto p = _PhysComp.Body_->GetPosition();
                    // std::cout << (int(p.x+256.f) / 16 + int(p.y+128.f) / 16 * 16) << std::endl;
                    // Cells_.insert({22, _e});
                });
        }

    private:

        entt::registry& Reg_;

        BoidSystemConfig Conf_;

        std::mt19937 Generator_;

        // Definition of a spatial grid for boids behaviour. A rough
        // approximation should be enough, so the number of boids per
        // cell is restricted. This simplifies the data structure and
        // shouldn't have too much of an impact for the algorithm.
        std::array<entt::entity, 64*32*10> Grid_;
};

#endif // BOID_SYSTEM_HPP
