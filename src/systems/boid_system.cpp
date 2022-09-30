#include "boid_system.hpp"

#include "b2_math.h"
#include "message_handler.hpp"
#include "physics_component.hpp"
#include "visuals_component.hpp"
#include "world_def.h"
#include <entt/entity/entity.hpp>

void BoidSystem::init()
{
    Reg_.ctx().at<MessageHandler>().report("bds", "Initialising boid system for fish", MessageHandler::INFO);

    std::normal_distribution<float> DistPos(0.0, 10.0f);

    auto ResX = WORLD_SIZE_DEFAULT_X / Conf_.GridSizeX;
    auto ResY = WORLD_SIZE_DEFAULT_Y / Conf_.GridSizeY;

    DBLK(
        std::ostringstream Str;
        Str << "Grid cell size: " << ResX << "x" << ResY << "m²";
        Reg_.ctx().at<MessageHandler>().report("bds", Str.str(),MessageHandler::DEBUG_L1);
    )

    GridCount_.resize(Conf_.GridSizeX*Conf_.GridSizeY);
    GridEntities_.resize(Conf_.GridSizeX*Conf_.GridSizeY*Conf_.n);

    for (auto i=0; i < Conf_.n; ++i)
    {
        auto Boid = Reg_.create();
        Magnum::Math::Color3 Col = {0.3f, 0.3f, 0.1f};
        b2BodyDef BodyDef;
        BodyDef.type = b2_dynamicBody;
        BodyDef.enabled = true;
        BodyDef.angularDamping = 1.0f;
        BodyDef.linearDamping = 0.0f;
        BodyDef.position.Set(100.f+DistPos(Generator_), 20.f+DistPos(Generator_));
        Reg_.ctx().at<GameObjectSystem>().create(Boid, this, GameObjectTypeE::BOID, -1,
                                                DrawableGroupsTypeE::BOIDS, Col, BodyDef);
        Reg_.emplace<BoidComponent>(Boid);
        Reg_.emplace<FluidSourceComponent>(Boid);

        auto& FldProbesComp = Reg_.emplace<FluidProbeComponent>(Boid);
        Reg_.ctx().at<FluidInteractionSystem>().addFluidProbe(FldProbesComp, 0.001f, 0.0f, 0.0f);

        auto& Vis = Reg_.get<VisualsComponent>(Boid);
        // auto Scale = 0.5f+DistPos(Generator_)*0.01f;
        auto Scale = 1.0f;
        Vis.Visuals_->setScaling({Scale, Scale});

        if (i==0)
        {
            EntityDebug_ = Boid;
        }
    }
    Reg_.ctx().at<MessageHandler>().report("bds", "Populated world with "+std::to_string(Conf_.n)+" fish", MessageHandler::INFO);
}

void BoidSystem::update()
{
    static std::normal_distribution<float> DistPos(0.0, 0.05);

    this->updateGrid();
    if (IsBoidDebugActive_) this->resetBoidDebug();
    this->updateNeighbours();

    Reg_.view<BoidComponent, PhysicsComponent>().each(
        [&,this](auto _e, auto& _BoidComp, auto& _PhysComp)
        {
            b2Vec2 Sum;
            for (auto i=0; i<_BoidComp.NrOfNeighbours; ++i)
            {
                Sum += Reg_.get<PhysicsComponent>(entt::entity(_BoidComp.Neighbours[i])).Body_->GetPosition();
            }
            if (_BoidComp.NrOfNeighbours > 0)
            {
                _BoidComp.NeighbourPosAvgX = Sum.x / _BoidComp.NrOfNeighbours;
                _BoidComp.NeighbourPosAvgY = Sum.y / _BoidComp.NrOfNeighbours;
            }
            else
            {
                _BoidComp.NeighbourPosAvgX = 0.0f;
                _BoidComp.NeighbourPosAvgY = 0.0f;
            }
            // _PhysComp.Body_->ApplyTorque(DistPos(Generator_), true);
            // _PhysComp.Body_->ApplyForceToCenter(0.001f*Sum, true);
            // _PhysComp.Body_->ApplyForceToCenter({_PhysComp.Body_->GetWorldVector({0.f, 0.5f})}, true);
        });
    Reg_.view<BoidComponent, PhysicsComponent>().each(
        [&,this](auto _e, auto& _BoidComp, auto& _PhysComp)
        {
            b2Vec2 Vec = b2Vec2(_BoidComp.NeighbourPosAvgX, _BoidComp.NeighbourPosAvgY) -
                        _PhysComp.Body_->GetPosition();
            if (Vec.Length() != 0.0f && _PhysComp.Body_->GetLinearVelocity().Length() < 2.0f)
                _PhysComp.Body_->ApplyForceToCenter(10.f/Vec.Length()*Vec, true);

            // _PhysComp.Body_->ApplyTorque(DistPos(Generator_), true);
            // _PhysComp.Body_->ApplyForceToCenter({_PhysComp.Body_->GetWorldVector({0.f, 0.5f})}, true);
        });
}

int BoidSystem::getGridIndexFromFloatPosition(float _x, float _y)
{
    auto f_x = float(Conf_.GridSizeX) / WORLD_SIZE_DEFAULT_X;
    auto f_y = float(Conf_.GridSizeY) / WORLD_SIZE_DEFAULT_Y;

    auto x = int((_x + WORLD_SIZE_DEFAULT_X*0.5f) * f_x);
    auto y = int((_y + WORLD_SIZE_DEFAULT_Y*0.5f) * f_y);

    return y*Conf_.GridSizeX + x;
}

NeighbourArrayType& BoidSystem::getGridNeighbourIndecesFromIndex(int _i)
{
    static NeighbourArrayType r;

    DBLK(
        if (_i < 1 || _i > Conf_.GridSizeX*Conf_.GridSizeY-1)
            Reg_.ctx().at<MessageHandler>().report("bds", "Neighbour index out of bounds.");
    )

    r[0] = _i - Conf_.GridSizeX-1;
    r[1] = _i - Conf_.GridSizeX;
    r[2] = _i - Conf_.GridSizeX+1;
    r[3] = _i - 1;
    r[4] = _i;
    r[5] = _i + 1;
    r[6] = _i + Conf_.GridSizeX-1;
    r[7] = _i + Conf_.GridSizeX;
    r[8] = _i + Conf_.GridSizeX+1;

    return r;
}

void BoidSystem::updateGrid()
{
    // Update grid
    std::fill(GridCount_.begin(), GridCount_.end(), 0);
    Reg_.view<BoidComponent, PhysicsComponent>().each(
        [&,this](auto _e, auto& _BoidComp, auto& _PhysComp)
        {
            auto p = _PhysComp.Body_->GetPosition();

            auto i = this->getGridIndexFromFloatPosition(p.x, p.y);

            GridCount_[i]++;
            GridEntities_[i*Conf_.n+GridCount_[i]-1] = entt::to_integral(_e);
        });
}

void BoidSystem::updateNeighbours()
{
    // Register neigbours
    Reg_.view<BoidComponent, PhysicsComponent>().each(
        [&,this](auto _e, auto& _BoidComp, auto& _PhysComp)
        {
            auto p = _PhysComp.Body_->GetPosition();
            _BoidComp.NrOfNeighbours = 0;

            auto i = this->getGridIndexFromFloatPosition(p.x, p.y);
            auto n_i = this->getGridNeighbourIndecesFromIndex(i);

            // For all local cells in the grid
            for (const auto j : n_i)
            {
                // Gather entities per cell
                for (auto k=0; k<GridCount_[j]; ++k)
                {
                    if (_BoidComp.NrOfNeighbours < BoidComponent::NEIGHBOURS_MAX-1)
                    {
                        auto e = GridEntities_[j*Conf_.n+k];
                        if (e != entt::to_integral(_e)) _BoidComp.Neighbours[_BoidComp.NrOfNeighbours++] = e;
                    }
                    else
                    {
                        // Reg_.ctx().at<MessageHandler>().report("bds", "Too many neighbours to register "+std::to_string(GridCount_[i]), MessageHandler::WARNING);
                        break;
                    }
                }
            }

            if (IsBoidDebugActive_ && _e == EntityDebug_)
            {
                for (auto i=0; i<_BoidComp.NrOfNeighbours; ++i)
                {
                    auto& Vis = Reg_.get<VisualsComponent>(entt::entity(_BoidComp.Neighbours[i]));
                    Vis.Drawable_->setColor({0.5f, 0.25f, 0.0f, 1.0f});
                }
                auto& VisDeb = Reg_.get<VisualsComponent>(_e);
                VisDeb.Drawable_->setColor({1.0f, 0.0f, 0.0f, 1.0f});
            }
        });
}

void BoidSystem::resetBoidDebug()
{
    Reg_.view<BoidComponent, VisualsComponent>().each(
        [&,this](auto _e, auto& _BoidComp, auto& _VisComp)
        {
            _VisComp.Drawable_->setColor({0.3f, 0.3f, 0.1f, 1.0f});
        });
}
