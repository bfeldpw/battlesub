#ifndef STATUS_COMPONENT_CONFIG_GUI_ADAPTER_HPP
#define STATUS_COMPONENT_CONFIG_GUI_ADAPTER_HPP

#include <cstring>

#include <Magnum/ImGuiIntegration/Context.hpp>
#include <imgui.h>

#include "status_component.hpp"

class StatusComponentConfigGuiAdapter
{
    public:

        explicit StatusComponentConfigGuiAdapter(const std::string& _s) : s_(_s) {}
        StatusComponentConfigGuiAdapter() = delete;

        bool place()
        {
            bool r{false};

            ImGui::Text((s_+" Config").c_str());
            if (ImGui::SliderInt("Max Age (s)", &Conf_.AgeMax_, -1, 100))
                r = true;
            if (ImGui::SliderInt("Sink Duration (s)", &Conf_.SinkDuration_, 0, 100))
                r = true;

           return r;
        }

        const StatusComponent& get() const
        {
            return Conf_;
        }

        void set(const StatusComponent& _c)
        {
            Conf_ = _c;
        }

    private:

        StatusComponent Conf_;
        std::string s_;

};

#endif // STATUS_COMPONENT_CONFIG_GUI_ADAPTER_HPP
