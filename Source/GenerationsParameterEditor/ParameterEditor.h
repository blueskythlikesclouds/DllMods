#pragma once

#include "ParameterData.h"

class ParameterEditor
{
public:
    struct CmpByName
    {
        bool operator()(const AbstractParameter* lhs, const AbstractParameter* rhs) const
        {
            return strcmp(lhs->name, rhs->name) < 0;
        }
    };

    typedef std::map<AbstractParameter*, std::unique_ptr<AbstractParameterData>, CmpByName> Map;

private:
    static void updateLazyAbstractParameter(Map::value_type& it, bool isStatic);
    static void saveLazyAbstractParameter(Map::value_type& it);

public:
    static bool visible;

    static Map staticParameters;
    static Map dynamicParameters;

    static void updateParameters(
        List<AbstractParameter*>* staticParameters, List<AbstractParameter*>* dynamicParameters);

    static void update();
};
