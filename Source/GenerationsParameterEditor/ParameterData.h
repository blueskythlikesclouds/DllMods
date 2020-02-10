#pragma once

#include "Types.h"
#include "XmlWriter.h"

#include <map>
#include <vector>

class ParameterData
{
protected:
    bool override;
    std::string name;
    std::string description;

    static std::string getConvertedString(const char* value);

public:
    ParameterData(const char* unconvertedName, const char* unconvertedDescription);
    virtual ~ParameterData() = default;

    virtual bool getOverride() const;
    virtual void setOverride(bool value);

    virtual void showDescription();

    virtual void restore() = 0;
    virtual void preUpdate() = 0;
    virtual void update() = 0;
    virtual void postUpdate() = 0;
    virtual void write(XmlWriter& writer) = 0;
};

class ParamValueData : public ParameterData
{
    ParamValue* paramValue;
    uint32_t overrideValue;
    bool doNotUpdate; // Stupid hack for ModelScaleInPlayableMenu

    void setOverrideValue();

public:
    ParamValueData(ParamValue* paramValue);

    void setOverride(bool value) override;

    void restore() override;
    void preUpdate() override;
    void update() override;
    void postUpdate() override;
    void write(XmlWriter& writer) override;
};

class ParamTypeListData : public ParameterData
{
    ParamTypeList* paramTypeList;
    std::map<uint32_t, std::string> typeMap;
    uint32_t overrideValue;

    void setOverrideValue();

public:
    ParamTypeListData(ParamTypeList* paramTypeList);

    void setOverride(bool value) override;

    void restore() override;
    void preUpdate() override;
    void update() override;
    void postUpdate() override;
    void write(XmlWriter& writer) override;
};

class EditParamData : public ParameterData
{
    EditParam* editParam;
    std::vector<std::unique_ptr<ParameterData>> parameters;

public:
    EditParamData(EditParam* editParam);

    void setOverride(bool value) override;

    void restore() override;
    void preUpdate() override;
    void update() override;
    void postUpdate() override;
    void write(XmlWriter& writer) override;
};

class AbstractParameterData : public ParameterData
{
    AbstractParameter* abstractParameter;

    std::unique_ptr<EditParamData> editParamData;
    std::vector<std::unique_ptr<AbstractParameterData>> parameters;

public:
    AbstractParameterData(AbstractParameter* abstractParameter);

    void setOverride(bool value) override;

    void restore() override;
    void preUpdate() override;
    void update() override;
    void postUpdate() override;
    void update(bool collapsingHeader, bool overrideButton);
    void write(XmlWriter& writer) override;
    void save();
};