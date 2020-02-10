#include "Configuration.h"
#include "Context.h"
#include "ParameterEditor.h"
#include "PlayerInfo.h"
#include "Status.h"

#include <fstream>

bool Configuration::visible = false;

bool Configuration::showDescriptions = false;
bool Configuration::wrapNames = true;
float Configuration::bgAlpha = 0.625f;
float Configuration::fontSize = 16.0f;
float Configuration::valueStepAmount = 0.1f;

void Configuration::load()
{
    const INIReader reader(Context::getModDirectoryPath() + "/" + FILE_NAME);

    if (reader.ParseError() != 0)
    {
        Status::addLabel("Failed to load " + std::string(FILE_NAME));
        return;
    }

    showDescriptions = reader.GetBoolean("Configuration", "ShowDescriptions", false);
    wrapNames = reader.GetBoolean("Configuration", "WrapNames", true);
    bgAlpha = reader.GetFloat("Configuration", "BgAlpha", 0.625f);
    fontSize = reader.GetFloat("Configuration", "FontSize", 16.0f);
    valueStepAmount = reader.GetFloat("Configuration", "ValueStepAmount", 0.1f);
    ParameterEditor::visible = reader.GetBoolean("Configuration", "DisplayParameterEditorWindow", true);
    PlayerInfo::visible = reader.GetBoolean("Configuration", "DisplayPlayerInfoWindow", false);
    Status::visible = reader.GetBoolean("Configuration", "DisplayStatusWindow", true);
    Status::labelDisplayTime = reader.GetFloat("Configuration", "StatusLabelDisplayTime", 5.0f);
}

void Configuration::save()
{
    std::ofstream stream(Context::getModDirectoryPath() + "/" + FILE_NAME, std::ofstream::out);
    if (!stream.is_open())
    {
        Status::addLabel("Failed to save " + std::string(FILE_NAME));
        return;
    }

    stream << "[Configuration]" << std::endl;
    stream << "ShowDescriptions" << "=" << (showDescriptions ? "true" : "false") << std::endl;
    stream << "WrapNames" << "=" << (wrapNames ? "true" : "false") << std::endl;
    stream << "BgAlpha" << "=" << bgAlpha << std::endl;
    stream << "FontSize" << "=" << fontSize << std::endl;
    stream << "ValueStepAmount" << "=" << valueStepAmount << std::endl;
    stream << "DisplayParameterEditorWindow" << "=" << (ParameterEditor::visible ? "true" : "false") << std::endl;
    stream << "DisplayPlayerInfoWindow" << "=" << (PlayerInfo::visible ? "true" : "false") << std::endl;
    stream << "DisplayStatusWindow" << "=" << (Status::visible ? "true" : "false") << std::endl;
    stream << "StatusLabelDisplayTime" << "=" << Status::labelDisplayTime << std::endl;
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
        ImGui::Checkbox("Show parameter descriptions", &showDescriptions);
        ImGui::Checkbox("Wrap parameter names", &wrapNames);
        ImGui::SliderFloat("Window background alpha", &bgAlpha, 0.0f, 1.0f);
        ImGui::InputFloat("Font size (Takes effect after restart)", &fontSize, 1.0f);
        ImGui::InputFloat("Value step amount", &valueStepAmount, 0.1f);
        ImGui::Checkbox("Display parameter editor window", &ParameterEditor::visible);
        ImGui::Checkbox("Display player info window", &PlayerInfo::visible);
        ImGui::Checkbox("Display status window", &Status::visible);
        ImGui::InputFloat("Status label display time", &Status::labelDisplayTime, 1.0f);
    }
    ImGui::End();

    if (previousVisible && !visible)
        save();
}