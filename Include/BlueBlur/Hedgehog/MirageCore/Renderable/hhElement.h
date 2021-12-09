#pragma once

#include <BlueBlur.h>
#include <Hedgehog/MirageCore/Renderable/hhRenderable.h>

namespace Hedgehog::Mirage
{
    class CElement;

    static FUNCTION_PTR(CElement*, __thiscall, fpCElementCtor, 0x702050, CElement* This);

    class CElement : public CRenderable
    {
    public:
        INSERT_PADDING(0x80);

        CElement(const null_ctor&) : CRenderable(null_ctor{}) {}

        CElement() : CElement(null_ctor{})
        {
            fpCElementCtor(this);
        }
    };

    ASSERT_SIZEOF(CElement, 0x8C);
}