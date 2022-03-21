#include "Configuration.h"
#include "DebugDrawText.h"
#include "DebugDrawTextImpl.h"

extern "C" __declspec(dllexport) DebugDrawText* DebugDrawTextGetInstance()
{
    return DebugDrawText::getInstance();
}

DebugDrawText* DebugDrawText::getInstanceImpl()
{
    return new DebugDrawTextImpl();
}

CriticalSection DebugDrawTextImpl::criticalSection;
std::list<DebugDrawTextImpl::Text> DebugDrawTextImpl::texts;
bool DebugDrawTextImpl::visible = true;

void DebugDrawTextImpl::drawImpl(const char* text, const Location& location, float size, const Color& color)
{
    std::lock_guard lock(criticalSection);
    texts.push_back({ TextType::Free, text, 0, 0, location, size, color });
}

void DebugDrawTextImpl::logImpl(const char* text, const float time, const size_t priority, const Color& color)
{
    std::lock_guard lock(criticalSection);
    texts.push_back({ TextType::Grouped, text, time, priority, {0, 0}, 1, color });
}

inline ImU32 getImGuiColor(const DebugDrawText::Color& color)
{
    const uint8_t r = (uint8_t)(color.r * 255);
    const uint8_t g = (uint8_t)(color.g * 255);
    const uint8_t b = (uint8_t)(color.b * 255);
    const uint8_t a = (uint8_t)(color.a * 255);
    return r | (g << 8) | (b << 16) | (a << 24);
}

void DebugDrawTextImpl::update()
{
    std::lock_guard lock(criticalSection);

    ImGuiIO& io = ImGui::GetIO();

    for (auto it = texts.begin(); it != texts.end();)
    {
        if (it->time < 0.0f)
            it = texts.erase(it);
        else
            it++->time -= io.DeltaTime;
    }

    if (texts.empty() || !visible)
        return;

    std::stable_sort(texts.begin(), texts.end(), [](const Text& lhs, const Text& rhs) { return lhs.priority < rhs.priority; });

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

    for (auto it = texts.begin(); it != texts.end();)
    {
        if ((*it).type != TextType::Grouped)
        {
            ++it;
            continue;
        }

        const ImU32 color = getImGuiColor((*it).color);
        if (color != (ImU32)-1)
            ImGui::PushStyleColor(ImGuiCol_Text, color);

        ImGui::TextUnformatted(it++->text.c_str());

        if (color != (ImU32)-1)
            ImGui::PopStyleColor();

        if (it != texts.end())
            ImGui::Separator();
    }

    ImGui::End();

    for (auto it = texts.begin(); it != texts.end(); ++it)
    {
        if ((*it).type != TextType::Free)
            continue;

        ImGui::GetForegroundDrawList()->AddText(nullptr, Configuration::fontSize * (*it).size,
            { (float)(*it).location.x, (float)(*it).location.y }, getImGuiColor((*it).color), (*it).text.c_str());
    }
}
