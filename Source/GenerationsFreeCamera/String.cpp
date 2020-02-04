#include "String.h"

FUNCTION_PTR(void, __thiscall, destructString, 0x661550, String* This);
FUNCTION_PTR(void*, __thiscall, constructString, 0x6621A0, String* This, const char* value);

void String::clear()
{
    destructString(this);
}

String::String() : value(nullptr)
{
}

String::String(const char* value) : String()
{
    constructString(this, value);
}

String::~String()
{
    destructString(this);
}
