#ifndef FLUID_PROBES_COMPONENT_HPP
#define FLUID_PROBES_COMPONENT_HPP

struct FluidProbesComponent
{
    static constexpr int PROBES_MAX = 8;

    float MassFactor_ = 0.1f;

    float N_ = 0;
    float ProbeX_[PROBES_MAX];
    float ProbeY_[PROBES_MAX];
};

#endif // FLUID_PROBES_COMPONENT_HPP
