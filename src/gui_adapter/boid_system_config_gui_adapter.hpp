#ifndef BOID_SYSTEM_CONFIG_GUI_ADAPTER_HPP
#define BOID_SYSTEM_CONFIG_GUI_ADAPTER_HPP

#include <cstring>

#include <Magnum/ImGuiIntegration/Context.hpp>
#include <imgui.h>

#include "boid_system_config.hpp"

class BoidSystemConfigGuiAdapter
{
    public:

        explicit BoidSystemConfigGuiAdapter(const std::string& _s) : s_(_s) {}
        BoidSystemConfigGuiAdapter() = delete;

        bool place()
        {
            bool r{false};

            ImGui::Text((s_+" Config").c_str());
            if (ImGui::SliderFloat("Boid Force (N)", &Conf_.BoidForce, 0.0f, 10.0f))
                r = true;
            if (ImGui::SliderFloat("Boid Torque Max (Nm)", &Conf_.BoidTorqueMax, 0.0f, 1.0f))
                r = true;
            if (ImGui::SliderFloat("Boid Velocity Max (m/s)", &Conf_.BoidVelMax, 0.f, 10.0))
                r = true;
            if (ImGui::Checkbox("Show boid debug", &Conf_.IsBoidDebugActive))
                r = true;

           return r;
        }

        const BoidSystemConfig& get() const
        {
            return Conf_;
        }

        void set(const BoidSystemConfig& _c)
        {
            Conf_ = _c;
        }

    private:

        BoidSystemConfig Conf_;
        std::string s_;

};

#endif // BOID_SYSTEM_CONFIG_GUI_ADAPTER_HPP
