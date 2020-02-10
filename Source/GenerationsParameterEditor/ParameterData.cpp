#include "Configuration.h"
#include "ParameterData.h"
#include "Path.h"
#include "Status.h"

const char* const EMPTY_STRING = (const char*)0x13E0DC0;

std::string ParameterData::getConvertedString(const char* value)
{
    if (!value || value == EMPTY_STRING)
        return std::string();

    WCHAR wideChar[1024];
    CHAR multiByte[1024];

    MultiByteToWideChar(932, 0, value, -1, wideChar, 1024);
    WideCharToMultiByte(CP_UTF8, 0, wideChar, -1, multiByte, 1024, 0, 0);

    return std::string(multiByte);
}

ParameterData::ParameterData(const char* unconvertedName, const char* unconvertedDescription)
    : override(false), name(getConvertedString(unconvertedName)), description(getConvertedString(unconvertedDescription))
{
}

bool ParameterData::getOverride() const
{
    return override;
}

void ParameterData::setOverride(const bool value)
{
    override = value;
}

void ParameterData::showDescription()
{
    if (!Configuration::showDescriptions || description.empty() || !ImGui::IsItemHovered())
        return;

    ImGui::BeginTooltip();
    ImGui::TextUnformatted(description.c_str());
    ImGui::EndTooltip();
}

void ParamValueData::setOverrideValue()
{
    if (!override)
        return;

    if ((size_t)paramValue->vtable == ParamBase::VTABLE_BOOL)
        *(bool*)&overrideValue = *(bool*)paramValue->funcData->valueAddress;
    else
        overrideValue = *paramValue->funcData->valueAddress;
}

ParamValueData::ParamValueData(ParamValue* paramValue)
    : ParameterData(paramValue->name, paramValue->funcData->description), paramValue(paramValue), overrideValue(0), doNotUpdate(name == "ModelScaleInPlayableMenu")
{
}

void ParamValueData::setOverride(const bool value)
{
    ParameterData::setOverride(value);
    setOverrideValue();
}

void ParamValueData::restore()
{
    switch ((size_t)paramValue->vtable)
    {
    case ParamBase::VTABLE_BOOL:
        *(bool*)paramValue->funcData->valueAddress = *(bool*)&paramValue->funcData->value;
        break;

    case ParamBase::VTABLE_FLOAT:
    case ParamBase::VTABLE_INT:
    case ParamBase::VTABLE_LONG:
    case ParamBase::VTABLE_ULONG:
        *(uint32_t*)paramValue->funcData->valueAddress = *(uint32_t*)&paramValue->funcData->value;
        break;
    }

    setOverrideValue();

    if (!doNotUpdate)
        paramValue->funcData->update();
}

void ParamValueData::preUpdate()
{
    if (!override)
        return;

    if ((size_t)paramValue->vtable == ParamBase::VTABLE_BOOL)
        *(bool*)paramValue->funcData->valueAddress = *(bool*)&overrideValue;
    else
        *paramValue->funcData->valueAddress = overrideValue;

    if (!doNotUpdate)
        paramValue->funcData->update();
}

void ParamValueData::update()
{
    bool result = false;

    if (Configuration::wrapNames)
        ImGui::PushTextWrapPos();

    switch ((size_t)paramValue->vtable)
    {
    case ParamBase::VTABLE_BOOL:
        result = ImGui::Checkbox(name.c_str(), (bool*)paramValue->funcData->valueAddress);
        break;

    case ParamBase::VTABLE_FLOAT:
        result = ImGui::InputFloat(name.c_str(), (float*)paramValue->funcData->valueAddress, Configuration::valueStepAmount);
        break;

    case ParamBase::VTABLE_INT:
    case ParamBase::VTABLE_LONG:
    case ParamBase::VTABLE_ULONG:
        result = ImGui::InputInt(name.c_str(), (int*)paramValue->funcData->valueAddress);
        break;
    }

    if (Configuration::wrapNames)
        ImGui::PopTextWrapPos();

    if (result && !doNotUpdate)
        paramValue->funcData->update();

    showDescription();
}

void ParamValueData::postUpdate()
{
    setOverrideValue();
}

void ParamValueData::write(XmlWriter& writer)
{
    switch ((size_t)paramValue->vtable)
    {
    case ParamBase::VTABLE_BOOL:
        writer.writeElement(name, *(bool*)paramValue->funcData->valueAddress ? "true" : "false");
        break;

    case ParamBase::VTABLE_FLOAT:
        writer.writeElement(name, *(float*)paramValue->funcData->valueAddress);
        break;

    case ParamBase::VTABLE_INT:
        writer.writeElement(name, *(int*)paramValue->funcData->valueAddress);
        break;

    case ParamBase::VTABLE_LONG:
        writer.writeElement(name, *(long*)paramValue->funcData->valueAddress);
        break;

    case ParamBase::VTABLE_ULONG:
        writer.writeElement(name, *(unsigned long*)paramValue->funcData->valueAddress);
        break;
    }
}

void ParamTypeListData::setOverrideValue()
{
    if (!override)
        return;

    overrideValue = *paramTypeList->subData->funcData->valueAddress;
}

ParamTypeListData::ParamTypeListData(ParamTypeList* paramTypeList)
    : ParameterData(paramTypeList->name, paramTypeList->subData->funcData->description), paramTypeList(paramTypeList), overrideValue(0)
{
    for (auto it = paramTypeList->subData->funcData->map.begin(); it != paramTypeList->subData->funcData->map.end(); it = it->next())
        typeMap.emplace(it->value.key, getConvertedString(it->value.value));
}

void ParamTypeListData::setOverride(const bool value)
{
    ParameterData::setOverride(value);
    setOverrideValue();
}

void ParamTypeListData::restore()
{
    *paramTypeList->subData->funcData->valueAddress = paramTypeList->subData->funcData->defaultValue;
    setOverrideValue();
}

void ParamTypeListData::preUpdate()
{
    if (override)
        *paramTypeList->subData->funcData->valueAddress = overrideValue;
}

void ParamTypeListData::update()
{
    if (Configuration::wrapNames)
        ImGui::PushTextWrapPos();

    const bool result = ImGui::BeginCombo(name.c_str(), typeMap[*paramTypeList->subData->funcData->valueAddress].c_str());

    showDescription();

    if (!result)
        return;

    for (auto& pair : typeMap)
    {
        if (!ImGui::Selectable(pair.second.c_str()))
            continue;

        *paramTypeList->subData->funcData->valueAddress = pair.first;
    }

    ImGui::EndCombo();

    if (Configuration::wrapNames)
        ImGui::PopTextWrapPos();
}

void ParamTypeListData::postUpdate()
{
    setOverrideValue();
}

void ParamTypeListData::write(XmlWriter& writer)
{
    writer.writeElement(name, *paramTypeList->subData->funcData->valueAddress);
}

EditParamData::EditParamData(EditParam* editParam)
    : ParameterData(nullptr, nullptr), editParam(editParam)
{
    for (auto it = editParam->children.begin; it != editParam->children.end; it++)
    {   
        switch ((size_t)(*it)->vtable)
        {
        case ParamBase::VTABLE_BOOL:
        case ParamBase::VTABLE_FLOAT:
        case ParamBase::VTABLE_INT:
        case ParamBase::VTABLE_LONG:
        case ParamBase::VTABLE_ULONG:
            parameters.push_back(std::make_unique<ParamValueData>((ParamValue*)*it));
            break;

        case ParamBase::VTABLE_TYPE_LIST:
            parameters.push_back(std::make_unique<ParamTypeListData>((ParamTypeList*)*it));
            break;
        }
    }
}

void EditParamData::setOverride(const bool value)
{
    for (auto& parameter : parameters)
        parameter->setOverride(value);

    ParameterData::setOverride(value);
}

void EditParamData::restore()
{
    for (auto& parameter : parameters)
        parameter->restore();
}

void EditParamData::preUpdate()
{
    for (auto& parameter : parameters)
        parameter->preUpdate();
}

void EditParamData::update()
{
    for (auto& parameter : parameters)
        parameter->update();

    if (ImGui::Button("Restore defaults"))
        restore();
}

void EditParamData::postUpdate()
{
    for (auto& parameter : parameters)
        parameter->postUpdate();
}

void EditParamData::write(XmlWriter& writer)
{
    writer.writeBeginElement("Param");

    for (auto& parameter : parameters)
        parameter->write(writer);

    writer.writeEndElement();
}

AbstractParameterData::AbstractParameterData(AbstractParameter* abstractParameter)
    : ParameterData(abstractParameter->name, abstractParameter->description), abstractParameter(abstractParameter)
{
    if (abstractParameter->editParam)
        editParamData = std::make_unique<EditParamData>(abstractParameter->editParam);

    for (auto it = abstractParameter->children.begin; it != abstractParameter->children.end; ++it)
        parameters.push_back(std::make_unique<AbstractParameterData>(it->value));
}

void AbstractParameterData::setOverride(const bool value)
{
    if (editParamData)
        editParamData->setOverride(value);

    for (auto& parameter : parameters)
        parameter->setOverride(value);

    ParameterData::setOverride(value);
}

void AbstractParameterData::restore()
{
    if (editParamData)
        editParamData->restore();

    for (auto& parameter : parameters)
        parameter->restore();
}

void AbstractParameterData::preUpdate()
{
    if (editParamData)
        editParamData->preUpdate();

    for (auto& parameter : parameters)
        parameter->preUpdate();
}

void AbstractParameterData::update()
{
    update(false, false);
}

void AbstractParameterData::postUpdate()
{
    if (editParamData)
        editParamData->postUpdate();

    for (auto& parameter : parameters)
        parameter->postUpdate();
}

void AbstractParameterData::update(const bool collapsingHeader, const bool overrideButton)
{
    bool result;

    if (collapsingHeader)
        result = ImGui::CollapsingHeader(name.c_str());

    else
        result = ImGui::TreeNode(name.c_str());

    showDescription();

    if (!result)
        return;

    if (collapsingHeader)
        ImGui::PushID(abstractParameter);

    if (editParamData)
        editParamData->update();

    for (auto& parameter : parameters)
        parameter->update();

    if (!collapsingHeader)
        ImGui::TreePop();
    else
    {
        if (ImGui::Button("Restore defaults"))
            restore();

        ImGui::SameLine();

        if (ImGui::Button("Save to work folder"))
            save();

        if (overrideButton)
        {
            ImGui::SameLine();

            if (ImGui::Checkbox("Force override", &override))
                setOverride(override);
        }

        ImGui::PopID();
    }
}

void AbstractParameterData::write(XmlWriter& writer)
{
    writer.writeBeginElement(name);

    if ((size_t)abstractParameter->vtable == AbstractParameter::VTABLE_PARAMETER_GROUP)
        writer.writeBeginElement("Category");

    if (editParamData)
        editParamData->write(writer);

    for (auto& parameter : parameters)
        parameter->write(writer);

    if ((size_t)abstractParameter->vtable == AbstractParameter::VTABLE_PARAMETER_GROUP)
        writer.writeEndElement();

    writer.writeEndElement();
}

void AbstractParameterData::save()
{
    XmlWriter writer(Path::getGameDirectoryPath() + "/work/" + name);
    if (!writer.isOpen())
    {
        Status::addLabel("Failed to save " + std::string(name));
        return;
    }

    writer.writeBeginDocument();
    write(writer);

    Status::addLabel("Saved " + std::string(name));
}
