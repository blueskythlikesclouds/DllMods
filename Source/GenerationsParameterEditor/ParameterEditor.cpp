#include "Configuration.h"
#include "ParameterEditor.h"
#include "PlayerInfo.h"
#include "GlobalLightEditor.h"
#include "MaterialEditor.h"

bool ParameterEditor::visible = true;

ParameterEditor::Map ParameterEditor::staticParameters;
ParameterEditor::Map ParameterEditor::dynamicParameters;

void ParameterEditor::updateLazyAbstractParameter(Map::value_type& it, const bool isStatic)
{
    if (!it.second)
    {
        if (visible && ImGui::CollapsingHeader(it.first->name))
            it.second = std::make_unique<AbstractParameterData>(it.first);
    }
    else 
    {
        it.second->preUpdate();

        if (visible)
            it.second->update(true, isStatic);

        it.second->postUpdate();
    }
}

void ParameterEditor::saveLazyAbstractParameter(Map::value_type& it)
{
    if (!it.second)
        it.second = std::make_unique<AbstractParameterData>(it.first);

    it.second->save();
}

void ParameterEditor::updateParameters(List<AbstractParameter*>* staticParameters,
    List<AbstractParameter*>* dynamicParameters)
{
    for (auto it = staticParameters->begin(); it != staticParameters->end(); it = it->next)
    {
        auto it2 = ParameterEditor::staticParameters.find(it->value);

        if (it2 == ParameterEditor::staticParameters.end())
            ParameterEditor::staticParameters.emplace(it->value, std::unique_ptr<AbstractParameterData>());
    }

    for (auto it = ParameterEditor::dynamicParameters.begin(); it != ParameterEditor::dynamicParameters.end();)
    {
        if (dynamicParameters->find(it->first) == dynamicParameters->end())
            it = ParameterEditor::dynamicParameters.erase(it);
        else
            ++it;
    }

    for (auto it = dynamicParameters->begin(); it != dynamicParameters->end(); it = it->next)
    {
        auto it2 = ParameterEditor::dynamicParameters.find(it->value);

        if (it2 == ParameterEditor::dynamicParameters.end())
            ParameterEditor::dynamicParameters.emplace(it->value, std::unique_ptr<AbstractParameterData>());
    }
}

void ParameterEditor::update()
{
    const bool end = visible;

    ImGui::SetNextWindowBgAlpha(Configuration::bgAlpha);
    if (visible && ImGui::Begin("Parameter Editor", &visible, ImGuiWindowFlags_MenuBar))
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Save all"))
                {
                    for (auto& parameter : staticParameters)
                        saveLazyAbstractParameter(parameter);

                    for (auto& parameter : dynamicParameters)
                        saveLazyAbstractParameter(parameter);
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Close"))
                    visible = false;

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Configuration"))
                    Configuration::setVisible(!Configuration::getVisible());

                if (ImGui::MenuItem("Player info"))
                    PlayerInfo::visible ^= true;

                if (ImGui::MenuItem("Global light editor"))
                    GlobalLightEditor::visible ^= true;

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

    for (auto& parameter : staticParameters)
        updateLazyAbstractParameter(parameter, true);

    for (auto& parameter : dynamicParameters)
        updateLazyAbstractParameter(parameter, false);

    if (end)
        ImGui::End();
}
