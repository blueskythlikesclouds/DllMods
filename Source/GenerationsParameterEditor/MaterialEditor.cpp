#include "MaterialEditor.h"
#include "Configuration.h"
#include "Context.h"

bool MaterialEditor::visible;

struct DatabaseSorter
{
    bool operator()(const boost::shared_ptr<hh::db::CDatabaseData>& left,
        const boost::shared_ptr<hh::db::CDatabaseData>& right) const
    {
        return left->m_TypeAndName < right->m_TypeAndName;
    }
};

struct MaterialEditor::Impl
{
    CriticalSection criticalSection;

    std::set<boost::shared_ptr<hh::mr::CShaderListData>, DatabaseSorter> shaders;
    std::set<boost::shared_ptr<hh::mr::CMaterialData>, DatabaseSorter> materials;

    char materialSearch[1024]{};
    boost::shared_ptr<Hedgehog::Mirage::CMaterialData> curMaterial;

    bool prevShaderBeginCombo{};
    float prevShaderScrollY{};

    float textureSize{};
};

std::unique_ptr<MaterialEditor::Impl> MaterialEditor::impl;

HOOK(boost::shared_ptr<hh::mr::CShaderListData>*, __fastcall, CMirageDatabaseWrapperGetShaderListData, hh::mr::fpCMirageDatabaseWrapperGetShaderListData,
    hh::mr::CMirageDatabaseWrapper* This, void* _, boost::shared_ptr<hh::mr::CShaderListData>& out_spShaderListData, 
    const Hedgehog::Base::CSharedString& in_rName, size_t in_Unknown)
{
    const auto result = originalCMirageDatabaseWrapperGetShaderListData(This, _, out_spShaderListData, in_rName, in_Unknown);

    if (out_spShaderListData)
    {
        std::lock_guard lock(MaterialEditor::impl->criticalSection);
        MaterialEditor::impl->shaders.insert(out_spShaderListData);
    }

    return result;
}

HOOK(boost::shared_ptr<hh::mr::CMaterialData>*, __fastcall, CMirageDatabaseWrapperGetMaterialData, hh::mr::fpCMirageDatabaseWrapperGetMaterialData,
    hh::mr::CMirageDatabaseWrapper* This, void* _, boost::shared_ptr<hh::mr::CMaterialData>& out_spMaterialData, 
    const Hedgehog::Base::CSharedString& in_rName, size_t in_Unknown)
{
    const auto result = originalCMirageDatabaseWrapperGetMaterialData(This, _, out_spMaterialData, in_rName, in_Unknown);

    if (out_spMaterialData)
    {
        std::lock_guard lock(MaterialEditor::impl->criticalSection);
        MaterialEditor::impl->materials.insert(out_spMaterialData);
    }

    return result;
}

void MaterialEditor::init()
{
    impl = std::make_unique<Impl>();

    INSTALL_HOOK(CMirageDatabaseWrapperGetShaderListData);
    INSTALL_HOOK(CMirageDatabaseWrapperGetMaterialData);
}

namespace
{
    char buf[256] = "##";

    const char* imguiLabelBeforeProperty(const char* label)
    {
        strcpy(buf + 2, label);

        ImGui::TableNextColumn();
        ImGui::TextUnformatted(label);
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_EPSILON);

        return buf;
    }

    template<typename T>
    void imguiParameters(const char* label, hh::vector<boost::shared_ptr<T>>& parameters, ImGuiTreeNodeFlags flags)
    {
        strcpy(buf + 2, label);

        ImGui::Separator();

        if (ImGui::TreeNodeEx(label, flags))
        {
            if (!parameters.empty())
            {
                if (ImGui::BeginTable(buf, 2, ImGuiTableFlags_SizingStretchSame, { -FLT_EPSILON, 0 }))
                {
                    for (const auto& param : parameters)
                    {
                        ImGui::PushID(param.get());
                        {
                            char paramName[256]{};
                            strcpy(paramName, param->m_Name.GetValue());

                            ImGui::TableNextColumn();
                            ImGui::SetNextItemWidth(-FLT_EPSILON);
                            if (ImGui::InputText("##ParamName", paramName, sizeof(paramName)))
                                param->m_Name = paramName;

                            ImGui::TableNextColumn();

                            for (size_t i = 0; i < param->m_ValueNum; i++)
                            {
                                char valueName[256]{};
                                sprintf(valueName, "##ParamValue%d", i);

                                ImGui::SetNextItemWidth(-FLT_EPSILON);

                                if constexpr (std::is_same_v<T, hh::mr::CParameterFloat4Element>)
                                    ImGui::InputFloat4(valueName, &param->m_spValue[i * 4], "%g");

                                else if constexpr (std::is_same_v<T, hh::mr::CParameterInt4Element>)
                                    ImGui::InputInt4(valueName, &param->m_spValue[i * 4]);

                                else if constexpr (std::is_same_v<T, hh::mr::CParameterBoolElement>)
                                    ImGui::Checkbox(valueName, (bool*)&param->m_spValue[i]);
                            }
                        }
                        ImGui::PopID();
                    }

                    ImGui::EndTable();
                }
            }
            else
            {
                ImGui::TextUnformatted("There are no parameters available.");
            }

            if (ImGui::Button("Add"))
            {
                auto param = boost::make_shared<T>();

                param->m_ValueNum = 1;

                if constexpr (std::is_same_v<T, hh::mr::CParameterBoolElement>)
                    param->m_spValue = boost::make_shared<typename T::ValueType[]>(1);
                else
                    param->m_spValue = boost::make_shared<typename T::ValueType[]>(4);

                parameters.push_back(param);
            }

            ImGui::TreePop();
        }
    }

    ImTextureID imguiTex(const boost::shared_ptr<hh::mr::CPictureData>& picture)
    {
        if (picture && picture->IsMadeAll() && picture->m_pD3DTexture)
            return *(ImTextureID*)((char*)picture->m_pD3DTexture + 20);
        else
            return nullptr;
    }

    void imguiAddressMode(const char* name, D3DTEXTUREADDRESS& value)
    {
        const char* label = "None";
        switch (value)
        {
        case D3DTADDRESS_WRAP: label = "Wrap"; break;
        case D3DTADDRESS_MIRROR: label = "Mirror"; break;
        case D3DTADDRESS_CLAMP: label = "Clamp"; break;
        case D3DTADDRESS_BORDER: label = "Border"; break;
        case D3DTADDRESS_MIRRORONCE: label = "Mirror Once"; break;
        }

        if (ImGui::BeginCombo(name, label))
        {
            if (ImGui::Selectable("Wrap", value == D3DTADDRESS_WRAP)) value = D3DTADDRESS_WRAP;
            if (ImGui::Selectable("Mirror", value == D3DTADDRESS_MIRROR)) value = D3DTADDRESS_MIRROR;
            if (ImGui::Selectable("Clamp", value == D3DTADDRESS_CLAMP)) value = D3DTADDRESS_CLAMP;
            if (ImGui::Selectable("Border", value == D3DTADDRESS_BORDER)) value = D3DTADDRESS_BORDER;
            if (ImGui::Selectable("Mirror Once", value == D3DTADDRESS_MIRRORONCE)) value = D3DTADDRESS_MIRRORONCE;

            ImGui::EndCombo();
        }
    }
}

void MaterialEditor::update()
{
    if (!visible || !**Sonic::CGameDocument::GetInstance())
        return;

    ImGui::SetNextWindowBgAlpha(Configuration::bgAlpha);

    if (!ImGui::Begin("Material Editor", &visible))
        return ImGui::End();

    ImGui::SetNextItemWidth(-FLT_EPSILON);
    ImGui::InputText("##MaterialSearch", impl->materialSearch, sizeof(impl->materialSearch));

    if (ImGui::BeginListBox("##Material", { -FLT_EPSILON, 0 }))
    {
        std::lock_guard lock(impl->criticalSection);

        for (auto it = impl->materials.begin(); it != impl->materials.end(); )
        {
            if (it->unique())
            {
                it = impl->materials.erase(it);
            }
            else
            {
                if ((*it)->IsMadeAll() && strstr((*it)->m_TypeAndName.c_str(), impl->materialSearch))
                {
                    if (ImGui::Selectable((*it)->m_TypeAndName.c_str() + sizeof("Mirage.material"), *it == impl->curMaterial))
                    {
                        impl->curMaterial = *it;
                        impl->prevShaderScrollY = -1.0f;
                    }

                    if (!(*it)->m_spTexsetData->m_TextureList.empty())
                    {
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();

                            for (auto& tex : (*it)->m_spTexsetData->m_TextureList)
                            {
                                const auto imTex = imguiTex(tex->m_spPictureData);
                                if (imTex)
                                {
                                    ImGui::Image(imTex, { 128.0f, 128.0f });
                                    ImGui::SameLine();
                                }
                            }

                            ImGui::EndTooltip();
                        }
                    }
                }

                ++it;
            }
        }

        ImGui::EndListBox();
    }

    ImGui::Separator();

    if (impl->curMaterial && ImGui::CollapsingHeader(impl->curMaterial->m_TypeAndName.c_str() + sizeof("Mirage.material"), ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::BeginTable("##MaterialProperties", 2, ImGuiTableFlags_SizingStretchSame, {-FLT_EPSILON, 0}))
        {
            ImGui::PushID(impl->curMaterial.get());

            const bool shaderBeginCombo = ImGui::BeginCombo(imguiLabelBeforeProperty("Shader"), impl->curMaterial->m_spShaderListData->m_TypeAndName.c_str() + sizeof("Mirage.shader-list"));

            if (shaderBeginCombo)
            {
                for (auto& shader : impl->shaders)
                {
                    if (!shader->IsMadeAll())
                        continue;

                    const bool selected = shader == impl->curMaterial->m_spShaderListData;

                    if (ImGui::Selectable(shader->m_TypeAndName.c_str() + sizeof("Mirage.shader-list"), selected))
                        impl->curMaterial->m_spShaderListData = shader;

                    if (selected && impl->prevShaderScrollY < 0.0f)
                    {
                        ImGui::SetScrollHereY();
                        impl->prevShaderBeginCombo = true;
                    }
                }

                if (impl->prevShaderBeginCombo ^ shaderBeginCombo)
                    ImGui::SetScrollY(impl->prevShaderScrollY);

                impl->prevShaderScrollY = ImGui::GetScrollY();

                ImGui::EndCombo();
            }

            impl->prevShaderBeginCombo = shaderBeginCombo;

            ImGui::InputScalar(imguiLabelBeforeProperty("Alpha Threshold"), ImGuiDataType_U8, &impl->curMaterial->m_AlphaThreshold);
            ImGui::Checkbox(imguiLabelBeforeProperty("Double Sided"), &impl->curMaterial->m_DoubleSided);
            ImGui::Checkbox(imguiLabelBeforeProperty("Additive"), &impl->curMaterial->m_Additive);

            ImGui::PopID();
            ImGui::EndTable();
        }

        ImGui::Separator();

        if (ImGui::TreeNodeEx("Textures", ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto& textureList = impl->curMaterial->m_spTexsetData->m_TextureList;
            auto& textureNameList = impl->curMaterial->m_spTexsetData->m_TextureNameList;

            enum
            {
                NONE,
                MOVE_UP,
                MOVE_DOWN,
                REMOVE
            } action{ NONE };

            size_t index{};

            for (size_t i = 0; i < textureList.size(); i++)
            {
                auto& texture = textureList[i];

                ImGui::PushID(texture.get());
                {
                    if (ImGui::BeginTable("##MaterialTextures", 2, ImGuiTableFlags_SizingStretchProp, { -FLT_EPSILON, 0 }))
                    {
                        ImGui::TableNextColumn();

                        if (const auto imTex = imguiTex(texture->m_spPictureData))
                        {
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + max(0.0f, ImGui::GetContentRegionAvail().x - impl->textureSize) / 2.0f);
                            ImGui::Image(imTex, { impl->textureSize, impl->textureSize });
                        }

                        ImGui::TableNextColumn();

                        const float y = ImGui::GetCursorPos().y;

                        if (ImGui::BeginTable("##MaterialTextureParams", 2, ImGuiTableFlags_SizingStretchSame, { -FLT_EPSILON, 0 }))
                        {
                            char name[256]{};
                            strcpy(name, textureNameList[i].c_str());

                            if (ImGui::InputText(imguiLabelBeforeProperty("Name"), name, sizeof(name)))
                                texture->m_TypeAndName = textureNameList[i] = (texture->m_TypeAndName = name);

                            char pictureName[256 + sizeof("Mirage.picture")]{};

                            if (texture->m_spPictureData)
                                strcpy(pictureName, texture->m_spPictureData->m_TypeAndName.c_str());

                            if (ImGui::InputText(imguiLabelBeforeProperty("Picture Name"), pictureName + sizeof("Mirage.picture"), sizeof(pictureName) - sizeof("Mirage.picture")))
                                texture->m_spPictureData->m_TypeAndName = pictureName;

                            constexpr uint8_t step = 1;
                            ImGui::InputScalar(imguiLabelBeforeProperty("Texture Coordinate Index"), ImGuiDataType_U8, &texture->m_TexcoordIndex, &step);
                            imguiAddressMode(imguiLabelBeforeProperty("Address U"), texture->m_SamplerState.AddressU);
                            imguiAddressMode(imguiLabelBeforeProperty("Address V"), texture->m_SamplerState.AddressV);

                            char type[256]{};
                            strcpy(type, texture->m_Type.GetValue());

                            if (ImGui::InputText(imguiLabelBeforeProperty("Type"), type, sizeof(type)))
                                texture->m_Type = type;

                            impl->textureSize = ImGui::GetCursorPos().y - y;

                            ImGui::EndTable();
                        }

                        if (ImGui::Button("Move Up"))
                        {
                            action = MOVE_UP;
                            index = i;
                        }

                        ImGui::SameLine();
                        if (ImGui::Button("Move Down"))
                        {
                            action = MOVE_DOWN;
                            index = i;
                        }

                        ImGui::SameLine();
                        if (ImGui::Button("Remove"))
                        {
                            action = REMOVE;
                            index = i;
                        }

                        ImGui::EndTable();
                    }

                    if (i < textureList.size() - 1)
                        ImGui::Separator();
                }
                ImGui::PopID();
            }

            if (action == REMOVE || (action == MOVE_UP && index != 0) || (action == MOVE_DOWN && index != (textureList.size() - 1)))
            {
                const auto texture = textureList[index];
                const auto textureName = textureNameList[index];

                auto it0 = textureList.erase(textureList.begin() + index);
                auto it1 = textureNameList.erase(textureNameList.begin() + index);

                if (action == MOVE_UP)
                {
                    textureList.insert(--it0, texture);
                    textureNameList.insert(--it1, textureName);
                }
                else if (action == MOVE_DOWN)
                {
                    textureList.insert(++it0, texture);
                    textureNameList.insert(++it1, textureName);
                }
            }

            ImGui::TreePop();
        }

        imguiParameters("Float4 Parameters", impl->curMaterial->m_Float4Params, ImGuiTreeNodeFlags_DefaultOpen);

        if (impl->curMaterial->m_Float4Params.empty())
            impl->curMaterial->m_MaterialFlags |= hh::mr::eMaterialFlags_Float4ParamsEmpty;
        else
            impl->curMaterial->m_MaterialFlags &= hh::mr::eMaterialFlags_Float4ParamsEmpty;

        imguiParameters("Int4 Parameters", impl->curMaterial->m_Int4Params, 0);

        if (impl->curMaterial->m_Int4Params.empty())
            impl->curMaterial->m_MaterialFlags |= hh::mr::eMaterialFlags_Int4ParamsEmpty;
        else
            impl->curMaterial->m_MaterialFlags &= hh::mr::eMaterialFlags_Int4ParamsEmpty;

        imguiParameters("Bool Parameters", impl->curMaterial->m_Bool4Params, 0);

        if (impl->curMaterial->m_Bool4Params.empty())
            impl->curMaterial->m_MaterialFlags |= hh::mr::eMaterialFlags_BoolParamsEmpty;
        else
            impl->curMaterial->m_MaterialFlags &= hh::mr::eMaterialFlags_BoolParamsEmpty;

    }

    ImGui::End();
}
