#ifndef FLUID_INTERACTION_SYSTEM_HPP
#define FLUID_INTERACTION_SYSTEM_HPP

#include <entt/entity/registry.hpp>

#include "fluid_grid.h"
#include "fluid_probes_component.hpp"

class FluidInteractionSystem
{

    public:

        explicit FluidInteractionSystem(entt::registry& _Reg) : Reg_(_Reg) {}
        FluidInteractionSystem() = delete;

        void addFluidProbe(FluidProbeComponent& _Probe, const float _M, const float _X, const float _Y);

        template<int N>
        void addFluidProbe(FluidProbesComponent<N>& _Probe, const int _N, const float _M, const int _I,
                           const float _X, const float _Y,
                           const float _NormX, const float _NormY, const bool Normalize = true);
        template<int N>
        void addFluidProbe(FluidProbesComponent<N>& _Probe, const int _I,
                           const float _X, const float _Y,
                           const float _NormX, const float _NormY, const bool Normalize = true);
        void addSources();
        void applyForces();


    private:

        entt::registry& Reg_;

};

template<int N>
void FluidInteractionSystem::addFluidProbe(FluidProbesComponent<N>& _Probe, const int _N, const float _M, const int _I,
                                           const float _X, const float _Y,
                                           const float _NormX, const float _NormY, const bool Normalize)
{
    _Probe.N_ = _N;
    _Probe.M_ = _M;
    this->addFluidProbe<N>(_Probe, _I, _X, _Y, _NormX, _NormY, Normalize);
}

template<int N>
void FluidInteractionSystem::addFluidProbe(FluidProbesComponent<N>& _Probe, const int _I,
                                           const float _X, const float _Y,
                                           const float _NormX, const float _NormY, const bool Normalize)
{
    assert(_I < N);
    _Probe.X_[_I] = _X;
    _Probe.Y_[_I] = _Y;
    if (Normalize)
    {
        float Length = std::sqrt(_NormX*_NormX + _NormY*_NormY);
        if (Length > 0.0f)
        {
            float InvLength = 1.0f/Length;
            _Probe.NormX_[_I] = _NormX * InvLength;
            _Probe.NormY_[_I] = _NormY * InvLength;
        }
    }
    else
    {
        _Probe.NormX_[_I] = _NormX;
        _Probe.NormY_[_I] = _NormY;
    }
}

#endif // FLUID_INTERACTION_SYSTEM_HPP
