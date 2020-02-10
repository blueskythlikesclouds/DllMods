#pragma once

#include "Std.h"

template<typename T>
struct SharedPtr
{
    T* value;
    void* implementation; // boost::detail::sp_counted_impl_p<T>
};

struct RefCountObject
{
    void* vtable;
    size_t refCount;
};

struct ParamBase : RefCountObject
{
    static const size_t VTABLE_BOOL = 0x16E58E4;
    static const size_t VTABLE_FLOAT = 0x16E5CDC;
    static const size_t VTABLE_INT = 0x16E5E14;
    static const size_t VTABLE_LONG = 0x16E5DAC;
    static const size_t VTABLE_TYPE_LIST = 0x16E5C74;
    static const size_t VTABLE_ULONG = 0x16E5D44;

    int8_t field8;
    const char* name;
    int8_t field10;
};

struct ParamValueFuncData : RefCountObject
{
    uint32_t* valueAddress;
    uint32_t value;
    int32_t field10;
    int32_t field14;
    void (*updater)();
    int32_t field1C;
    void* field20;
    int32_t field24;
    void* field28;
    void *field2C;
    const char* description;
    int32_t field34;

    void update();
};

struct ParamValue : ParamBase
{
    ParamValueFuncData* funcData;
    uint32_t defaultValue;
};

struct ParamTypeListMemberFuncData : RefCountObject
{
    int32_t field8;
    Map<uint32_t, const char*> map;
    int32_t field14;
    int32_t field18; // 0
    int32_t field1C;
    int32_t field20;
    int32_t field24;
    int32_t field28;
    int32_t field2C;
    int32_t field30;
    int32_t field34;
    int32_t field38; // 0
    int32_t field3C;
    int32_t field40;
    int32_t field44;
    int32_t field48;
    int32_t field4C;
    int32_t field50;
    int32_t field54;
    uint32_t* valueAddress;
    uint32_t defaultValue;
    const char* description;
    int32_t field64;
    int32_t field68;
    int32_t field6C;
};

struct ParamTypeListSubData
{
    ParamTypeListMemberFuncData* funcData;
    uint32_t value;

    uint32_t defaultValue;
    const char* defaultValueName;
};

struct ParamTypeList : ParamBase
{
    ParamTypeListSubData* subData;
};

struct EditParam : RefCountObject
{
    int32_t field8;
    Vector<ParamBase*> children;
    int32_t field18;
    Map<void*, void*> field1C;
    int32_t field24;
    int32_t field28;
    Map<void*, void*> field2C;
    int32_t field34;
};

struct AbstractParameter
{
    static const size_t VTABLE_ABSTRACT_PARAMETER = 0x16E5EA4;
    static const size_t VTABLE_PARAMETER_FILE = 0x16E5E88;
    static const size_t VTABLE_PARAMETER_GROUP = 0x16E5FA4;

    void* vtable;
    EditParam* editParam;
    const char* name;
    const char* description;
    int32_t field10;
    Vector<SharedPtr<AbstractParameter>> children;
    int32_t field20;
    Map<const char*, void*> childMap;
    int32_t field2C;
    int32_t field30;
};

struct ParameterCategory : AbstractParameter
{
    int32_t field34;
    int32_t field38;
    int32_t field3C;
    Map<void*, void*> field40;
};