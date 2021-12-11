#include "Configuration.h"
#include "Context.h"
#include "ParameterEditor.h"
#include "PlayerInfo.h"
#include "GlobalLightEditor.h"
#include "DebugDrawTextImpl.h"

const uint32_t* const WIDTH = (uint32_t*)0x1DFDDDC;
const uint32_t* const HEIGHT = (uint32_t*)0x1DFDDE0;

std::string Context::modDirectoryPath;
std::string Context::imGuiIniPath = "ImGui.ini";

HWND Context::window;
IDirect3DDevice9* Context::device;

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

void Context::initialize(HWND window, IDirect3DDevice9* device)
{
    Context::window = window;
    Context::device = device;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplDX9_Init(device);
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
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
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
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void Context::reset()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
}

LRESULT Context::wndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);
}
