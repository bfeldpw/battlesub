#ifndef FLUID_PROBES_COMPONENT_HPP
#define FLUID_PROBES_COMPONENT_HPP

struct FluidProbeComponent
{
    float Mass_ = 0.1f;
    float ProbeX_ = 0.0f;
    float ProbeY_ = 0.0f;
};

template<int n>
struct FluidProbesComponent
{
    float Mass_ = 0.1f;

    float N_ = 0;
    float ProbeX_[n];
    float ProbeY_[n];
    float ProbeNormX_[n];
    float ProbeNormY_[n];

};

#endif // FLUID_PROBES_COMPONENT_HPP
