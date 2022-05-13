#ifndef FLUID_PROBES_COMPONENT_HPP
#define FLUID_PROBES_COMPONENT_HPP

struct FluidProbeComponent
{
    float M_ = 0.1f;
    float X_ = 0.0f;
    float Y_ = 0.0f;
};

template<int N>
struct FluidProbesComponent
{
    int N_ = 0;
    float M_ = 0.1f;

    float X_[N];
    float Y_[N];
    float NormX_[N];
    float NormY_[N];

};

#endif // FLUID_PROBES_COMPONENT_HPP
