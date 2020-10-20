#ifndef EMITTER_COMPONENT_HPP
#define EMITTER_COMPONENT_HPP

#include <random>

#include "status_component.hpp"

struct EmitterComponent
{

    static std::mt19937 Generator_;

    GameObjectTypeE Type_ = GameObjectTypeE::DEBRIS_LANDSCAPE;
    float  Angle_ = 0.0f;
    float  AngleStdDev_ = 0.0f;
    float  Frequency_ = 0.0f;
    int    Counter_ = 0;
    int    Number_ = 1;
    float  OriginX_ = 0.0f;
    float  OriginY_ = 0.0f;
    float  Scale_ = 1.0f;
    float  ScaleStdDev_ = 0.0f;
    float  Velocity_ = 1.0f;
    float  VelocityStdDev_ = 0.0f;
};

#endif // EMITTER_COMPONENT_HPP
