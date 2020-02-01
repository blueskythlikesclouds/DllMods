#include "String.h"

FUNCTION_PTR(void, __thiscall, clearString, 0x661550, String* This);
FUNCTION_PTR(void*, __thiscall, copyString, 0x6621A0, String* This, const char* value);

void String::clear()
{
    clearString(this);
}

String::String() : value(nullptr), length(0)
{
}

String::String(const char* value) : String()
{
    copyString(this, value);
}

String::~String()
{
    clearString(this);
}
