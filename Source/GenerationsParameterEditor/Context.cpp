#include "Configuration.h"
#include "Context.h"
#include "ParameterEditor.h"
#include "PlayerInfo.h"
#include "Status.h"

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

    RECT rect;
    GetClientRect(window, &rect);

    const float fontSize = max(Configuration::fontSize, (rect.bottom - rect.top) / 1080.0f * Configuration::fontSize);

    if ((font = io.Fonts->AddFontFromFileTTF((modDirectoryPath + "/Fonts/DroidSans.ttf").c_str(), fontSize, nullptr, io.Fonts->GetGlyphRangesDefault())) == nullptr)
    {
        Status::addLabel("Failed to load DroidSans.ttf");
        font = io.Fonts->AddFontDefault();
    }

    ImFontConfig fontConfig;
    fontConfig.MergeMode = true;

    if (!io.Fonts->AddFontFromFileTTF((modDirectoryPath + "/Fonts/DroidSansJapanese.ttf").c_str(), fontSize * 1.25f, &fontConfig, io.Fonts->GetGlyphRangesJapanese()))
        Status::addLabel("Failed to load DroidSansJapanese.ttf");

    io.Fonts->Build();

    Status::addLabel("Press Esc to hide/show the UI.");
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
    Status::update();

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
