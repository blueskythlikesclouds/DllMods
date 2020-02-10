#include "Configuration.h"
#include "PlayerInfo.h"

void** const PLAYER_CONTEXT = (void**)0x1E5E2F0;

bool PlayerInfo::visible = false;

void PlayerInfo::update()
{
    if (!visible)
        return;

    if (!*PLAYER_CONTEXT)
        return;

    const uint32_t result = *(uint32_t*)((uint32_t) * (void**)((uint32_t)*PLAYER_CONTEXT + 0x110) + 172);
    if (!result)
        return;

    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_NoTitleBar;
    flags |= ImGuiWindowFlags_NoResize;
    flags |= ImGuiWindowFlags_NoMove;
    flags |= ImGuiWindowFlags_NoCollapse;
    flags |= ImGuiWindowFlags_AlwaysAutoResize;
    flags |= ImGuiWindowFlags_NoFocusOnAppearing;

    ImGui::SetNextWindowBgAlpha(Configuration::bgAlpha);
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    ImGui::Begin("Player Info", nullptr, flags);

    float* position = (float*)(*(uint32_t*)(result + 16) + 112);
    float* velocity = (float*)(result + 656);

    ImGui::Text("Position: %.3f %.3f %.3f", position[0], position[1], position[2]);
    ImGui::Text("Speed: %.3f", sqrtf(velocity[0] * velocity[0] + velocity[1] * velocity[1] + velocity[2] * velocity[2]));

    ImGui::End();
}
