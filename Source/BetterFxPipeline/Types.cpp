#include "Types.h"

SharedString::SharedString(const char* value)
{
    constructString(this, value);
}

SharedString::~SharedString()
{
    destructString(this);
}
