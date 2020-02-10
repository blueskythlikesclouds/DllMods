#include "Configuration.h"
#include "Status.h"

std::list<Status::Label> Status::labels;

bool Status::visible = true;

float Status::labelDisplayTime = 5.0f;

void Status::addLabel(const std::string& label)
{
    labels.emplace_front(label, labelDisplayTime);
}

void Status::update()
{
    ImGuiIO& io = ImGui::GetIO();

    for (auto it = labels.begin(); it != labels.end();)
    {
        it->time -= io.DeltaTime;

        if (it->time < 0.0f)
            it = labels.erase(it);
        else
            ++it;
    }

    if (labels.empty() || !visible)
        return;

    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_NoTitleBar;
    flags |= ImGuiWindowFlags_NoResize;
    flags |= ImGuiWindowFlags_NoMove;
    flags |= ImGuiWindowFlags_NoCollapse;
    flags |= ImGuiWindowFlags_AlwaysAutoResize;
    flags |= ImGuiWindowFlags_NoFocusOnAppearing;

    ImGui::SetNextWindowBgAlpha(Configuration::bgAlpha);
    ImGui::Begin("Status", nullptr, flags);
    ImGui::SetWindowPos(ImVec2(io.DisplaySize.x - ImGui::GetWindowSize().x, 0));

    for (auto it = labels.begin(); it != labels.end();)
    {
        ImGui::TextUnformatted(it++->label.c_str());

        if (it != labels.end())
            ImGui::Separator();
    }

    ImGui::End();
}