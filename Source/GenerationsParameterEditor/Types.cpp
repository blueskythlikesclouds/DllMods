#include "Types.h"

void ParamValueFuncData::update()
{
    if (!field10)
        return;
        
    ((int(**)(void*, int32_t))(field10 & ~1))[1](&updater, true);
}