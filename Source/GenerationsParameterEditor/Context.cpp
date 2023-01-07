#include "Context.h"
#include "Configuration.h"
#include "CsdImGui.h"
#include "DebugDrawTextImpl.h"
#include "GlobalLightEditor.h"
#include "MaterialEditor.h"
#include "ParameterEditor.h"
#include "PlayerInfo.h"

const uint32_t* const WIDTH = (uint32_t*)0x1DFDDDC;
const uint32_t* const HEIGHT = (uint32_t*)0x1DFDDE0;

std::string Context::modDirectoryPath;
std::string Context::imGuiIniPath = "ImGui.ini";

HWND Context::window;
IUnknown* Context::device;
ImFont* Context::font;

std::string Context::getModDirectoryPath()
{
    return modDirectoryPath;
}

void Context::setModDirectoryPath(const std::string& value)
{
    modDirectoryPath = value;
    imGuiIniPath = modDirectoryPath + "/ImGui.ini";
}

bool Context::isInitialized()
{
    return window && device;
}

void Context::initialize(HWND window, IUnknown* device)
{
    Context::window = window;
    Context::device = device;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    CsdImGui::init();
    ImGui_ImplWin32_Init(window);

    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = true;
    io.IniFilename = imGuiIniPath.c_str();
    io.DisplaySize = { (float)*WIDTH, (float)*HEIGHT };

    const float fontSize = max(Configuration::fontSize, *HEIGHT / 1080.0f * Configuration::fontSize);

    if ((font = io.Fonts->AddFontFromFileTTF((modDirectoryPath + "/Fonts/DroidSans.ttf").c_str(), fontSize, nullptr, io.Fonts->GetGlyphRangesDefault())) == nullptr)
    {
        DebugDrawText::log("Failed to load DroidSans.ttf", Configuration::labelDisplayTime);
        font = io.Fonts->AddFontDefault();
    }

    ImFontConfig fontConfig;
    fontConfig.MergeMode = true;

    if (!io.Fonts->AddFontFromFileTTF((modDirectoryPath + "/Fonts/DroidSansJapanese.ttf").c_str(), fontSize * 1.25f, &fontConfig, io.Fonts->GetGlyphRangesJapanese()))
        DebugDrawText::log("Failed to load DroidSansJapanese.ttf", Configuration::labelDisplayTime);

    io.Fonts->Build();

    DebugDrawText::log("Press Esc to hide/show the UI.", Configuration::labelDisplayTime);
}

void Context::update()
{
    ImGui::NewFrame();

    if (GetAsyncKeyState(VK_ESCAPE) & 1)
        ParameterEditor::visible ^= true;

    if (!ParameterEditor::visible)
        Configuration::setVisible(false);

    ImGui::PushFont(font);

    ParameterEditor::update();
    Configuration::update();
    PlayerInfo::update();
    GlobalLightEditor::update();
    DebugDrawTextImpl::update();

    ImGui::PopFont();

    ImGui::EndFrame();
    ImGui::Render();

    CsdImGui::render(ImGui::GetDrawData());
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Context::wndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);
}
