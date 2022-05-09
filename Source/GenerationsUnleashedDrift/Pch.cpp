#include "Pch.h"

#if !_DEBUG
namespace boost
{
    void __cdecl boost::throw_exception(class stdext::exception const&) {}
}
#endif