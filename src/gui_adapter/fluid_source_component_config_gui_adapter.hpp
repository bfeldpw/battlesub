#ifndef FLUID_SOURCE_COMPONENT_CONFIG_GUI_ADAPTER_HPP
#define FLUID_SOURCE_COMPONENT_CONFIG_GUI_ADAPTER_HPP

#include <cstring>

#include <Magnum/ImGuiIntegration/Context.hpp>
#include <imgui.h>

#include "fluid_source_component.hpp"

class FluidSourceComponentConfigGuiAdapter
{
    public:

        explicit FluidSourceComponentConfigGuiAdapter(const std::string& _s) : s_(_s) {}
        FluidSourceComponentConfigGuiAdapter() = delete;

        bool place()
        {
            ImGui::Text((s_+" Densities").c_str());
            bool r = false;
            float ColDyn[3];
            ColDyn[0] = Conf_.DensityDynamicR_;
            ColDyn[1] = Conf_.DensityDynamicG_;
            ColDyn[2] = Conf_.DensityDynamicB_;
            float ColStat[3];
            ColStat[0] = Conf_.DensityStaticR_;
            ColStat[1] = Conf_.DensityStaticG_;
            ColStat[2] = Conf_.DensityStaticB_;
            if (ImGui::ColorEdit3("Dynamic", ColDyn))
            {
                Conf_.DensityDynamicR_ = ColDyn[0];
                Conf_.DensityDynamicG_ = ColDyn[1];
                Conf_.DensityDynamicB_ = ColDyn[2];
                r = true;
            }
            if (ImGui::ColorEdit3("Static", ColStat))
            {
                Conf_.DensityStaticR_ = ColStat[0];
                Conf_.DensityStaticG_ = ColStat[1];
                Conf_.DensityStaticB_ = ColStat[2];
                r = true;
            }
            return r;
        }

        const FluidSourceComponent& get() const
        {
            return Conf_;
        }

        void set(const FluidSourceComponent& _c)
        {
            Conf_ = _c;
        }

    private:

        FluidSourceComponent Conf_;
        std::string s_;

};

#endif // FLUID_SOURCE_COMPONENT_CONFIG_GUI_ADAPTER_HPP
