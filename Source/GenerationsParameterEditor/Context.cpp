#include "Context.h"
#include "Configuration.h"
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
Backend Context::backend;

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

Backend Context::getBackend()
{
    return backend;
}

void Context::initialize(HWND window, IUnknown* device)
{
    Context::window = window;
    Context::device = device;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    backend = Backend::Unknown;
    {
        IDirect3DDevice9* d3d9Device = nullptr;

        if (SUCCEEDED(device->QueryInterface(&d3d9Device)))
        {
            backend = Backend::DX9;

            ImGui_ImplDX9_Init(d3d9Device);

            d3d9Device->Release();
        }
    }

    if (backend == Backend::Unknown)
    {
        ID3D11Device* d3d11Device = nullptr;

        if (SUCCEEDED(device->QueryInterface(&d3d11Device)))
        {
            backend = Backend::DX11;

            ID3D11DeviceContext* d3d11Context = nullptr;
            d3d11Device->GetImmediateContext(&d3d11Context);

            ImGui_ImplDX11_Init(d3d11Device, d3d11Context);

            d3d11Device->Release();
            d3d11Context->Release();
        }
    }

    if (backend == Backend::Unknown)
    {
        MessageBoxW(window, L"Failed to initialize Parameter Editor", L"Parameter Editor", MB_ICONERROR);
        exit(-1);
    }

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
    switch (backend)
    {
    case Backend::DX9:
        ImGui_ImplDX9_NewFrame();
        break;

    case Backend::DX11:
        ImGui_ImplDX11_NewFrame();
        break;
    }

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
    MaterialEditor::update();
    DebugDrawTextImpl::update();

    ImGui::PopFont();

    ImGui::EndFrame();
    ImGui::Render();

    switch (backend)
    {
    case Backend::DX9:
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        break;

    case Backend::DX11:
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        break;
    }
}

void Context::reset()
{
    switch (backend)
    {
    case Backend::DX9:
        ImGui_ImplDX9_InvalidateDeviceObjects();
        break;

    case Backend::DX11:
        ImGui_ImplDX11_InvalidateDeviceObjects();
        break;
    }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Context::wndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);
}
