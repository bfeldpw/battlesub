#include "debug_render_system.hpp"

#include <Corrade/Containers/ArrayViewStl.h>
#include <Magnum/Primitives/Circle.h>
#include <Magnum/Primitives/Line.h>
#include <Magnum/Trade/MeshData.h>

#include "fluid_grid.h"
#include "fluid_probes_component.hpp"
#include "physics_component.hpp"

DebugRenderSystem::DebugRenderSystem(entt::registry& _Reg) :
                                     Reg_(_Reg),
                                     MeshCircle_(MeshTools::compile(Primitives::circle2DSolid(16))),
                                     MeshLine_(MeshTools::compile(Primitives::line2D()))
{}

void DebugRenderSystem::renderVelocityProbes(const Matrix3& _ProjectionMatrix)
{
    const auto& Fluid = Reg_.ctx().at<FluidGrid>();
    Reg_.view<FluidProbeComponent, PhysicsComponent>().each(
        [&](const auto& _ProbeComp, const auto& _PhysComp)
    {
        Matrix3 MatTransformation;
        Matrix3 MatRotation;
        b2Body* Body = _PhysComp.Body_;

        auto ProbePos = Body->GetWorldPoint({_ProbeComp.X_, _ProbeComp.Y_});
        auto VelF = Fluid.getVelocity(ProbePos.x, ProbePos.y);
        auto VelB = Body->GetLinearVelocityFromLocalPoint({_ProbeComp.X_, _ProbeComp.Y_});
        b2Vec2 Vel = {VelF.x() - VelB.x, VelF.y() - VelB.y};

        MatTransformation = Matrix3::translation({ProbePos.x, ProbePos.y})*
                            Matrix3::scaling({0.1f+Vel.Length()*0.2f, 0.1f+Vel.Length()*0.2f});
        MatRotation = Matrix3::rotation(Magnum::Rad(std::atan2(VelF.y(), VelF.x())));

        Shader_.setTransformationProjectionMatrix(_ProjectionMatrix*MatTransformation)
               .setColor({1.0f, 0.0f, 0.0f, 0.2f})
               .draw(MeshCircle_);
        Shader_.setTransformationProjectionMatrix(_ProjectionMatrix*MatTransformation*MatRotation)
               .setColor({1.0f, 0.0f, 0.0f, 0.5f})
               .draw(MeshLine_);
    });
    Reg_.view<FluidProbesComponent<8>, PhysicsComponent>().each([&](const auto& _ProbesComp, const auto& _PhysComp)
    {
        for (int i=0; i<_ProbesComp.N_; ++i)
        {
            Matrix3 MatTransformation;
            Matrix3 MatRotation;
            b2Body* Body = _PhysComp.Body_;

            auto ProbePos = Body->GetWorldPoint({_ProbesComp.X_[i], _ProbesComp.Y_[i]});
            auto VelF = Fluid.getVelocity(ProbePos.x, ProbePos.y);
            auto VelB = Body->GetLinearVelocityFromLocalPoint({_ProbesComp.X_[i], _ProbesComp.Y_[i]});
            auto NormB = Body->GetWorldVector({_ProbesComp.NormX_[i], _ProbesComp.NormY_[i]});
            b2Vec2 VelR = {VelF.x() - VelB.x, VelF.y() - VelB.y};
            b2Vec2 Vel = b2Dot(VelR, NormB)*NormB;

            MatTransformation = Matrix3::translation({ProbePos.x, ProbePos.y})*
                                Matrix3::scaling({0.1f+Vel.Length()*0.2f, 0.1f+Vel.Length()*0.2f});
            MatRotation = Matrix3::rotation(Magnum::Rad(std::atan2(VelF.y(), VelF.x())));

            Shader_.setTransformationProjectionMatrix(_ProjectionMatrix*MatTransformation)
                   .setColor({1.0f, 0.0f, 0.0f, 0.2f})
                   .draw(MeshCircle_);
            Shader_.setTransformationProjectionMatrix(_ProjectionMatrix*MatTransformation*MatRotation)
                   .setColor({1.0f, 0.0f, 0.0f, 0.5f})
                   .draw(MeshLine_);
        }
    });
}

void DebugRenderSystem::renderVelocityVectors(const Matrix3& _ProjectionMatrix)
{
    GL::Mesh Mesh;
    GL::Buffer Buffer;

    constexpr auto GRID_X = FLUID_GRID_SIZE_X >> FluidGrid::VELOCITY_READBACK_SUBSAMPLE;
    constexpr auto GRID_Y = FLUID_GRID_SIZE_Y >> FluidGrid::VELOCITY_READBACK_SUBSAMPLE;
    constexpr auto CELL_X = WORLD_SIZE_DEFAULT_X/GRID_X;
    constexpr auto CELL_Y = WORLD_SIZE_DEFAULT_Y/GRID_Y;

    const auto& Fluid = Reg_.ctx().at<FluidGrid>();

    for (auto y=0; y<GRID_Y; y+=2)
    {
        for (auto x=0; x<GRID_X; x+=2)
        {
            int IndFlat = GRID_X*y+x;
            float PosX0 = float(x)*CELL_X-0.5f*WORLD_SIZE_DEFAULT_X+CELL_X*0.5f;
            float PosY0 = float(y)*CELL_Y-0.5f*WORLD_SIZE_DEFAULT_Y+CELL_Y*0.5f;
            float PosX1 = PosX0 + Fluid.getVelocityReadback()[IndFlat*2]*0.3f;
            float PosY1 = PosY0 + Fluid.getVelocityReadback()[IndFlat*2+1]*0.3f;
            VelocityVectors_[IndFlat*4] = PosX0;
            VelocityVectors_[IndFlat*4+1] = PosY0;
            VelocityVectors_[IndFlat*4+2] = PosX1;
            VelocityVectors_[IndFlat*4+3] = PosY1;
        }
    }

    Buffer.setData(VelocityVectors_, GL::BufferUsage::StreamDraw);
    Mesh.setCount(VelocityVectors_.size())
        .setPrimitive(GL::MeshPrimitive::Lines)
        .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});

    Shader_.setTransformationProjectionMatrix(_ProjectionMatrix)
           .setColor({0.7f, 0.7f, 0.7f, 0.5f})
           .draw(Mesh);
}
