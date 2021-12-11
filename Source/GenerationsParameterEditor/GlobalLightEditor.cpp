#include "Configuration.h"
#include "GlobalLightEditor.h"

#include <fstream>

#include "Path.h"
#include "DebugDrawTextImpl.h"

Eigen::Vector3f currentDirection;
boost::shared_ptr<Hedgehog::Mirage::CLightData> spCurrentGlobalLightData;

bool GlobalLightEditor::visible = false;

bool inputFloat(float* data, float valueStepAmount)
{
    ImGui::PushID(data);
    const bool result = ImGui::InputFloat("", data, valueStepAmount, 0, "%g");
    ImGui::PopID();
    ImGui::SameLine();

    return result;
}

bool inputFloat3(const char* text, float* data, float valueStepAmount)
{
    bool result = false;

    result |= inputFloat(&data[0], valueStepAmount);
    result |= inputFloat(&data[1], valueStepAmount);
    result |= inputFloat(&data[2], valueStepAmount);

    ImGui::TextUnformatted(text);

    return result;
}

template<typename T>
void write(std::ofstream& stream, const T value)
{
    unsigned long swapped = _byteswap_ulong(*(unsigned long*)&value);
    stream.write((const char*)&swapped, sizeof(unsigned long));
}

void GlobalLightEditor::update()
{
    if (!visible)
        return;

    Sonic::CGameDocument* pGameDocument = *Sonic::CGameDocument::ms_pInstance;

    if (pGameDocument == nullptr || pGameDocument->m_pMember == nullptr)
        return;

    const boost::shared_ptr<Sonic::CLightManager> spLightManager = pGameDocument->m_pMember->m_spLightManager;

    if (spLightManager == nullptr || spLightManager->m_pStaticLightContext == nullptr)
        return;

    Hedgehog::Mirage::CStaticLightContext* pStaticLightContext = spLightManager->m_pStaticLightContext;

    if (pStaticLightContext->m_spLightListData == nullptr)
        return;

    const boost::shared_ptr<Hedgehog::Mirage::CLightListData> spLightListData = pStaticLightContext->m_spLightListData;
    
    boost::shared_ptr<Hedgehog::Mirage::CLightData> spGlobalLightData;

    for (auto it = spLightListData->m_Lights.m_pBegin; it != spLightListData->m_Lights.m_pEnd; it++)
    {
        if ((*it)->m_Type != Hedgehog::Mirage::eLightType_Directional)
            continue;

        spGlobalLightData = *it;
        break;
    }

    if (spGlobalLightData == nullptr)
        return;

    ImGui::SetNextWindowBgAlpha(Configuration::bgAlpha);
    if (ImGui::Begin("Global Light Editor", &visible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (spGlobalLightData != spCurrentGlobalLightData)
        {
            spCurrentGlobalLightData = spGlobalLightData;
            currentDirection = spGlobalLightData->m_Direction;
        }

        if (ImGui::CollapsingHeader(spGlobalLightData->m_TypeAndName.c_str() + 13, ImGuiTreeNodeFlags_DefaultOpen))
        {
            inputFloat3("Color", spLightManager->m_GlobalLightDiffuse.data(), Configuration::valueStepAmount);
            inputFloat3("Direction", currentDirection.data(), Configuration::valueStepAmount);

            if (ImGui::Button("Restore defaults"))
            {
                spLightManager->m_GlobalLightDiffuse.x() = spGlobalLightData->m_Color.x();
                spLightManager->m_GlobalLightDiffuse.y() = spGlobalLightData->m_Color.y();
                spLightManager->m_GlobalLightDiffuse.z() = spGlobalLightData->m_Color.z();

                spLightManager->m_GlobalLightDirection = spGlobalLightData->m_Direction;
                currentDirection = spGlobalLightData->m_Direction;
            }

            ImGui::SameLine();

            if (ImGui::Button("Save to work folder"))
            {
                // "Mirage.light "
                std::string name = spGlobalLightData->m_TypeAndName.c_str() + 13;

                std::ofstream stream(Path::getGameDirectoryPath() + "/work/" + name + ".light", std::ios::binary | std::ios::out);

                if (!stream.is_open())
                {
                    DebugDrawText::log(("Failed to save " + name).c_str(), Configuration::labelDisplayTime);
                    return;
                }

                write(stream, 0x38); // File Size
                write(stream, 0x01); // Version
                write(stream, 0x1C); // Data Size
                write(stream, 0x18); // Data Offset
                write(stream, 0x34); // Relocation Table Offset
                write(stream, 0x00); // File Name Offset

                write(stream, 0x00); // Light Type

                // Light Direction
                write(stream, spLightManager->m_GlobalLightDirection.x());
                write(stream, spLightManager->m_GlobalLightDirection.y());
                write(stream, spLightManager->m_GlobalLightDirection.z());

                // Light Color
                write(stream, spLightManager->m_GlobalLightDiffuse.x());
                write(stream, spLightManager->m_GlobalLightDiffuse.y());
                write(stream, spLightManager->m_GlobalLightDiffuse.z());

                // Relocation Table Offset Count
                write(stream, 0x00);

                stream.close();

                DebugDrawText::log(("Saved " + name + ".light").c_str(), Configuration::labelDisplayTime);
            }

            ImGui::SameLine();

            bool enableTerrainShadows = *(uint8_t*)0x10C63F3 != 0x40 || *(uint32_t*)0x13DD294 != 0x100;
            const bool previousEnableTerrainShadows = enableTerrainShadows;

            ImGui::Checkbox("Enable real-time terrain shadows", &enableTerrainShadows);

            if (enableTerrainShadows ^ previousEnableTerrainShadows)
            {
                const uint8_t tokenFxPipeline = enableTerrainShadows ? 0xC0 : 0x40;
                const uint32_t tokenMTFx = enableTerrainShadows ? 0x180 : 0x100;

                WRITE_MEMORY(0x10C63F3, uint8_t, tokenFxPipeline);
                WRITE_MEMORY(0x10C641D, uint8_t, tokenFxPipeline);
                WRITE_MEMORY(0x10C65CC, uint8_t, tokenFxPipeline);
                WRITE_MEMORY(0x10C65F6, uint8_t, tokenFxPipeline);
                WRITE_MEMORY(0x13DD294, uint32_t, tokenMTFx);
            }
        }

        ImGui::End();

        spLightManager->m_GlobalLightSpecular = spLightManager->m_GlobalLightDiffuse;
        spLightManager->m_GlobalLightDirection = currentDirection.normalized();
    }
}
