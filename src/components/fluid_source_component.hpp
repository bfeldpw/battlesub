#ifndef FLUID_SOURCE_COMPONENT_HPP
#define FLUID_SOURCE_COMPONENT_HPP

enum class FluidSourceShapeE : int
{
    POINT,
    LINE,
    RECT
};

struct FluidSourceComponent
{
    FluidSourceShapeE DensityShape_ = FluidSourceShapeE::POINT;
    float DensityBackProjection_ = 0.0f; // Linear Backprojection in seconds
    float DensityDynamic_ = 0.0f;
    float DensitySourceSize_ = 1.0f;
    float DensityStatic_ = 1.0f;

    FluidSourceShapeE VelocityShape_ = FluidSourceShapeE::POINT;
    float VelocityX_[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float VelocityY_[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float VelocitySourceSize_ = 1.0f;
    float VelocityBackProjection_ = 0.0f; // Linear Backprojection in seconds
    float VelocityWeight_ = 0.0f;
};

#endif // FLUID_SOURCE_COMPONENT_HPP
