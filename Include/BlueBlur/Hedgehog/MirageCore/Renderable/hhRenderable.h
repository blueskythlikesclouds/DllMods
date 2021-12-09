#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Mirage
{
    class CRenderable;

    static FUNCTION_PTR(CRenderable*, __thiscall, fpCRenderableCtor, 0x6F49C0, CRenderable* This);

    class CRenderable : public Base::CObject
    {
    public:
        INSERT_PADDING(0x8);

        CRenderable(const null_ctor&) : CObject(null_ctor{}) {}

        CRenderable() : CRenderable(null_ctor{})
        {
            fpCRenderableCtor(this);
        }

        virtual ~CRenderable() = default;
    };

    ASSERT_SIZEOF(CRenderable, 0xC);
}