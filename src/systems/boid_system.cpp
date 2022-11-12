#include "boid_system.hpp"

#include "b2_math.h"
#include "boid_component.hpp"
#include "boid_system_config.hpp"
#include "math_helper.hpp"
#include "message_handler.hpp"
#include "physics_component.hpp"
#include "visuals_component.hpp"
#include "world_def.h"
#include <entt/entity/entity.hpp>
#include <random>

using namespace bfe::mh;

void BoidSystem::init()
{
    Reg_.ctx().at<MessageHandler>().report("bds", "Initialising boid system for fish", MessageHandler::INFO);

    std::uniform_real_distribution<float> DistPos(0.0, 40.0f);
    std::uniform_real_distribution<float> DistAngle(-MATH_PI_F, MATH_PI_F);

    DBLK(
        auto ResX = WORLD_SIZE_DEFAULT_X / Conf_.get().GridSizeX;
        auto ResY = WORLD_SIZE_DEFAULT_Y / Conf_.get().GridSizeY;
        std::ostringstream Str;
        Str << "Grid cell size: " << ResX << "x" << ResY << "m²";
        Reg_.ctx().at<MessageHandler>().report("bds", Str.str(),MessageHandler::DEBUG_L1);
    )

    // Setup vectors, enlargen grid for neighbour operations
    auto GridIndexMax = (Conf_.get().GridSizeX+2) * (Conf_.get().GridSizeY+2);
    GridCount_.resize(GridIndexMax);
    GridEntities_.resize(GridIndexMax*Conf_.get().n);

    for (auto i=0; i < Conf_.get().n; ++i)
    {
        auto Boid = Reg_.create();
        Magnum::Math::Color3 Col = {0.3f, 0.3f, 0.1f};
        b2BodyDef BodyDef;
        BodyDef.type = b2_dynamicBody;
        BodyDef.enabled = true;
        BodyDef.angularDamping = 1.0f;
        BodyDef.linearDamping = 0.0f;
        BodyDef.position.Set(100.f+DistPos(Generator_), 20.f+DistPos(Generator_));
        BodyDef.angle = DistAngle(Generator_);
        Reg_.ctx().at<GameObjectSystem>().create(Boid, this, GameObjectTypeE::BOID, -1,
                                                 DrawableGroupsTypeE::BOIDS, Col, BodyDef);
        Reg_.emplace<BoidComponent>(Boid);
        Reg_.emplace<FluidSourceComponent>(Boid);

        auto& FldProbesComp = Reg_.emplace<FluidProbeComponent>(Boid);
        Reg_.ctx().at<FluidInteractionSystem>().addFluidProbe(FldProbesComp, 0.1f, 0.0f, 0.0f);

        auto& Vis = Reg_.get<VisualsComponent>(Boid);
        auto Scale = 1.0f;
        Vis.Visuals_->setScaling({Scale, Scale});

        if (i==0)
        {
            EntityDebug_ = Boid;
        }
    }
    Reg_.ctx().at<MessageHandler>().report("bds", "Populated world with "+std::to_string(Conf_.get().n)+" fish", MessageHandler::INFO);
}

void BoidSystem::loadConfig()
{
    Conf_.read("boid_system");
}

void BoidSystem::update()
{
    // First, assign all entities to their respective grid cell
    this->updateGrid();
    // Reset visual information from debug
    // (some boids are coloured differently in debug visualisation)
    if (Conf_.get().IsBoidDebugActive)
        this->resetBoidDebug();
    // Gather neighbouring entities for each entity
    // This information is stored in the boid component
    this->updateNeighbours();
    // Gather all information needed for boid rules
    // This might be average positions, orientation, etc.
    this->updateLocalFeatures();

    // Now apply all rules
    this->applySeparation();
    this->applyAlignment();
    this->applyCohesion();
    this->applyConfinement();
    this->applyDynamics();
}

int BoidSystem::getGridIndexFromFloatPosition(float _x, float _y)
{
    auto f_x = float(Conf_.get().GridSizeX) / WORLD_SIZE_DEFAULT_X;
    auto f_y = float(Conf_.get().GridSizeY) / WORLD_SIZE_DEFAULT_Y;

    // Grid is slightly larger to make neighbour calculations easier
    auto x = int((_x + WORLD_SIZE_DEFAULT_X*0.5f) * f_x)+1;
    auto y = int((_y + WORLD_SIZE_DEFAULT_Y*0.5f) * f_y)+1;

    return y*Conf_.get().GridSizeX + x;
}

NeighbourArrayType& BoidSystem::getGridNeighbourIndecesFromIndex(int _i)
{
    static NeighbourArrayType r;

    DBLK(
        if (_i < 1 || _i > (Conf_.get().GridSizeX+2)*(Conf_.get().GridSizeY+2)-1)
            Reg_.ctx().at<MessageHandler>().report("bds", "Neighbour index out of bounds.");
    )

    r[0] = _i - Conf_.get().GridSizeX-1;
    r[1] = _i - Conf_.get().GridSizeX;
    r[2] = _i - Conf_.get().GridSizeX+1;
    r[3] = _i - 1;
    r[4] = _i;
    r[5] = _i + 1;
    r[6] = _i + Conf_.get().GridSizeX-1;
    r[7] = _i + Conf_.get().GridSizeX;
    r[8] = _i + Conf_.get().GridSizeX+1;

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
            GridEntities_[i*Conf_.get().n+GridCount_[i]-1] = entt::to_integral(_e);
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
                    if (_BoidComp.NrOfNeighbours < Conf_.get().NeighboursMax)
                    {
                        auto e = GridEntities_[j*Conf_.get().n+k];
                        if (e != entt::to_integral(_e)) _BoidComp.Neighbours[_BoidComp.NrOfNeighbours++] = e;
                    }
                    else
                    {
                        break;
                    }
                }
            }

            if (Conf_.get().IsBoidDebugActive && _e == EntityDebug_)
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

void BoidSystem::updateLocalFeatures()
{
    Reg_.view<BoidComponent>().each(
        [&,this](auto& _BoidComp)
        {
            b2Vec2 Pos{0.f, 0.f};
            float Angle{0.f};
            for (auto i=0; i<_BoidComp.NrOfNeighbours; ++i)
            {
                Pos   += Reg_.get<PhysicsComponent>(entt::entity(_BoidComp.Neighbours[i])).Body_->GetPosition();
                Angle += Reg_.get<PhysicsComponent>(entt::entity(_BoidComp.Neighbours[i])).Body_->GetAngle();
            }
            if (_BoidComp.NrOfNeighbours > 0)
            {
                _BoidComp.NeighbourPosAvgX = Pos.x / _BoidComp.NrOfNeighbours;
                _BoidComp.NeighbourPosAvgY = Pos.y / _BoidComp.NrOfNeighbours;
                _BoidComp.NeighbourAngle = Angle / _BoidComp.NrOfNeighbours;
            }
            else
            {
                _BoidComp.NeighbourPosAvgX = 0.0f;
                _BoidComp.NeighbourPosAvgY = 0.0f;
                _BoidComp.NeighbourAngle = 0.0f;
            }
        });
}

void BoidSystem::applySeparation()
{
    Reg_.view<BoidComponent, PhysicsComponent>().each(
        [&](const auto _e, auto& _BoidComp, auto& _PhysComp)
        {
            b2Vec2 p = _PhysComp.Body_->GetPosition();
            b2Vec2 d_v{WORLD_SIZE_DEFAULT_X, WORLD_SIZE_DEFAULT_Y};
            auto d = d_v.LengthSquared();
            int n{0};

            // Get nearest neighbour
            for (auto i=0; i<_BoidComp.NrOfNeighbours; ++i)
            {
                auto p_i = Reg_.get<PhysicsComponent>(entt::entity(_BoidComp.Neighbours[i])).Body_->GetPosition();
                auto d_i = (p-p_i).LengthSquared();
                if (d_i < d)
                {
                    d_v = p_i-p;
                    d = d_i;
                    n = i;
                }
            }

            // Turn away
            float Torque = Conf_.get().BoidTorqueMax / MATH_PI_F;
            auto AngSelf = _PhysComp.Body_->GetAngle() + MATH_PI_F*0.5f;
            AngSelf = constrainAngle(AngSelf);
            auto Angle = -AngSelf + std::atan2(-d_v.y, -d_v.x);
            Angle = constrainAngle(Angle);
            if (std::abs(Angle) > MATH_PI_F * 0.8f)
            {
                _BoidComp.Torque += Torque*Angle;
            }
        });
}

void BoidSystem::applyAlignment()
{
    float Torque = Conf_.get().BoidTorqueMax / MATH_PI_F;
    Reg_.view<BoidComponent, PhysicsComponent>().each(
        [&](auto& _BoidComp, auto& _PhysComp)
        {
            auto Angle = _BoidComp.NeighbourAngle - _PhysComp.Body_->GetAngle();
            Angle = constrainAngle(Angle);
            _BoidComp.Torque += Torque*Angle;
        });
}

void BoidSystem::applyCohesion()
{
    float Force  = Conf_.get().BoidForce;
    float Torque = Conf_.get().BoidTorqueMax / MATH_PI_F;
    Reg_.view<BoidComponent, PhysicsComponent>().each(
        [&](auto& _BoidComp, auto& _PhysComp)
        {
            b2Vec2 Vec = b2Vec2(_BoidComp.NeighbourPosAvgX, _BoidComp.NeighbourPosAvgY) -
                        _PhysComp.Body_->GetPosition();
            auto Angle = std::atan2(Vec.y, Vec.x) -
                std::atan2(_PhysComp.Body_->GetWorldVector({0.f, 1.f}).y, _PhysComp.Body_->GetWorldVector({0.f, 1.f}).x);
            Angle = constrainAngle(Angle);
            _BoidComp.Torque += Torque*0.5f*Angle;
            _BoidComp.Force  += Force;
        });
}

void BoidSystem::applyConfinement()
{
    float BoundaryMinX{80.f};
    float BoundaryMaxX{160.f};
    float BoundaryMinY{0.f};
    float BoundaryMaxY{80.f};
    float Torque = Conf_.get().BoidTorqueMax / MATH_PI_F;
    Reg_.view<BoidComponent, PhysicsComponent>().each(
        [&](auto& _BoidComp, auto& _PhysComp)
        {
            auto Pos = _PhysComp.Body_->GetPosition();
            if (Pos.x < BoundaryMinX || Pos.x > BoundaryMaxX ||
                Pos.y < BoundaryMinY || Pos.y > BoundaryMaxY)
            {
                b2Vec2 Vec = b2Vec2((BoundaryMaxX+BoundaryMinX) * 0.5f,
                                    (BoundaryMaxY+BoundaryMinY) * 0.5f) -
                             Pos;
                auto Angle = std::atan2(Vec.y, Vec.x) -
                    std::atan2(_PhysComp.Body_->GetWorldVector({0.f, 1.f}).y, _PhysComp.Body_->GetWorldVector({0.f, 1.f}).x);
                Angle = constrainAngle(Angle);
                _BoidComp.Torque += Torque*2.0f*Angle;
            }
        });
}

void BoidSystem::applyDynamics()
{
    float VelMax = Conf_.get().BoidVelMax;
    Reg_.view<BoidComponent, PhysicsComponent>().each(
        [&](auto& _BoidComp, auto& _PhysComp)
        {
            if (std::abs(_PhysComp.Body_->GetAngularVelocity()) < MATH_PI_F)
            {
                _PhysComp.Body_->ApplyTorque(_BoidComp.Torque, true);
            }
            if (_PhysComp.Body_->GetLinearVelocity().Length() < VelMax)
                // _PhysComp.Body_->ApplyForceToCenter({_PhysComp.Body_->GetWorldVector({0.f, _BoidComp.Force})}, true);
                _PhysComp.Body_->ApplyForceToCenter({_PhysComp.Body_->GetWorldVector({0.f, Conf_.get().BoidForce})}, true);
            _BoidComp.Force = 0.0f;
            _BoidComp.Torque = 0.0f;
        });
}

void BoidSystem::resetBoidDebug()
{
    Reg_.view<BoidComponent, VisualsComponent>().each(
        [](auto& _BoidComp, auto& _VisComp)
        {
            _VisComp.Drawable_->setColor({0.3f, 0.3f, 0.1f, 1.0f});
        });
}
