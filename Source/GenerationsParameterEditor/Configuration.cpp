#include "Configuration.h"
#include "Context.h"
#include "ParameterEditor.h"
#include "PlayerInfo.h"
#include "GlobalLightEditor.h"
#include "DebugDrawTextImpl.h"

#include <fstream>

bool Configuration::visible = false;

bool Configuration::loadDebugShaders = true;
bool Configuration::showDescriptions = false;
bool Configuration::wrapNames = true;
float Configuration::bgAlpha = 0.625f;
float Configuration::fontSize = 16.0f;
float Configuration::valueStepAmount = 0.1f;
float Configuration::labelDisplayTime = 5.0f;

void Configuration::load()
{
    const INIReader reader(Context::getModDirectoryPath() + "/" + FILE_NAME);

    if (reader.ParseError() != 0)
    {
        DebugDrawText::log(("Failed to load " + std::string(FILE_NAME)).c_str(), labelDisplayTime);
        return;
    }

    loadDebugShaders = reader.GetBoolean("Configuration", "LoadDebugShaders", true);
    showDescriptions = reader.GetBoolean("Configuration", "ShowDescriptions", false);
    wrapNames = reader.GetBoolean("Configuration", "WrapNames", true);
    bgAlpha = reader.GetFloat("Configuration", "BgAlpha", 0.625f);
    fontSize = reader.GetFloat("Configuration", "FontSize", 16.0f);
    valueStepAmount = reader.GetFloat("Configuration", "ValueStepAmount", 0.1f);
    ParameterEditor::visible = reader.GetBoolean("Configuration", "DisplayParameterEditorWindow", true);
    PlayerInfo::visible = reader.GetBoolean("Configuration", "DisplayPlayerInfoWindow", false);
    GlobalLightEditor::visible = reader.GetBoolean("Configuration", "DisplayGlobalLightWindow", false);
    DebugDrawTextImpl::visible = reader.GetBoolean("Configuration", "DisplayStatusWindow", true);
    labelDisplayTime = reader.GetFloat("Configuration", "StatusLabelDisplayTime", 5.0f);
}

void Configuration::save()
{
    std::ofstream stream(Context::getModDirectoryPath() + "/" + FILE_NAME, std::ofstream::out);
    if (!stream.is_open())
    {
        DebugDrawText::log(("Failed to save " + std::string(FILE_NAME)).c_str(), labelDisplayTime);
        return;
    }

    stream << "[Configuration]" << std::endl;
    stream << "LoadDebugShaders" << "=" << (loadDebugShaders ? "true" : "false") << std::endl;
    stream << "ShowDescriptions" << "=" << (showDescriptions ? "true" : "false") << std::endl;
    stream << "WrapNames" << "=" << (wrapNames ? "true" : "false") << std::endl;
    stream << "BgAlpha" << "=" << bgAlpha << std::endl;
    stream << "FontSize" << "=" << fontSize << std::endl;
    stream << "ValueStepAmount" << "=" << valueStepAmount << std::endl;
    stream << "DisplayParameterEditorWindow" << "=" << (ParameterEditor::visible ? "true" : "false") << std::endl;
    stream << "DisplayPlayerInfoWindow" << "=" << (PlayerInfo::visible ? "true" : "false") << std::endl;
    stream << "DisplayGlobalLightWindow" << "=" << (GlobalLightEditor::visible ? "true" : "false") << std::endl;
    stream << "DisplayStatusWindow" << "=" << (DebugDrawTextImpl::visible ? "true" : "false") << std::endl;
    stream << "StatusLabelDisplayTime" << "=" << labelDisplayTime << std::endl;
}

bool Configuration::getVisible()
{
    return visible;
}

void Configuration::setVisible(const bool value)
{
    if (visible && !value)
        save();

    visible = value;
}

void Configuration::update()
{
    if (!visible)
        return;

    const bool previousVisible = visible;

    ImGui::SetNextWindowBgAlpha(bgAlpha);
    if (ImGui::Begin("Configuration", &visible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Checkbox("Load debug shaders (Takes effect after restart)", &loadDebugShaders);
        ImGui::Checkbox("Show parameter descriptions", &showDescriptions);
        ImGui::Checkbox("Wrap parameter names", &wrapNames);
        ImGui::SliderFloat("Window background alpha", &bgAlpha, 0.0f, 1.0f);
        ImGui::InputFloat("Font size (Takes effect after restart)", &fontSize, 1.0f, 0, "%g");
        ImGui::InputFloat("Value step amount", &valueStepAmount, 0.1f, 0, "%g");
        ImGui::Checkbox("Display parameter editor window", &ParameterEditor::visible);
        ImGui::Checkbox("Display player info window", &PlayerInfo::visible);
        ImGui::Checkbox("Display global light editor window", &GlobalLightEditor::visible);
        ImGui::Checkbox("Display status window", &DebugDrawTextImpl::visible);
        ImGui::InputFloat("Status label display time", &labelDisplayTime, 1.0f, 0, "%g");
    }
    ImGui::End();

    if (previousVisible && !visible)
        save();
}