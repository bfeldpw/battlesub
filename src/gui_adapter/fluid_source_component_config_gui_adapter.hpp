#ifndef FLUID_SOURCE_COMPONENT_CONFIG_GUI_ADAPTER_HPP
#define FLUID_SOURCE_COMPONENT_CONFIG_GUI_ADAPTER_HPP

#include <cstring>

#include <Magnum/ImGuiIntegration/Context.hpp>
#include <imgui.h>

#include "fluid_source_component.hpp"

class FluidSourceComponentConfigGuiAdapter
{
    public:

        explicit FluidSourceComponentConfigGuiAdapter(const std::string& _s) {s_ = _s;}
        FluidSourceComponentConfigGuiAdapter() = delete;

        bool place()
        {
            ImGui::Text((s_+" Densities").c_str());
            bool r = false;
            float ColDyn[3];
            ColDyn[0] = Conf.DensityDynamicR_;
            ColDyn[1] = Conf.DensityDynamicG_;
            ColDyn[2] = Conf.DensityDynamicB_;
            float ColStat[3];
            ColStat[0] = Conf.DensityStaticR_;
            ColStat[1] = Conf.DensityStaticG_;
            ColStat[2] = Conf.DensityStaticB_;
            if (ImGui::ColorEdit3("Dynamic", ColDyn))
            {
                Conf.DensityDynamicR_ = ColDyn[0];
                Conf.DensityDynamicG_ = ColDyn[1];
                Conf.DensityDynamicB_ = ColDyn[2];
                r = true;
            }
            if (ImGui::ColorEdit3("Static", ColStat))
            {
                Conf.DensityStaticR_ = ColStat[0];
                Conf.DensityStaticG_ = ColStat[1];
                Conf.DensityStaticB_ = ColStat[2];
                r = true;
            }
            return r;
        }

        void copyFrom(const FluidSourceComponent& _c)
        {
            Conf = _c;
        }

        void copyTo(FluidSourceComponent& _c) const
        {
            _c = Conf;
        }

        FluidSourceComponent Conf;

    private:

        std::string s_;

};

#endif // FLUID_SOURCE_COMPONENT_CONFIG_GUI_ADAPTER_HPP
