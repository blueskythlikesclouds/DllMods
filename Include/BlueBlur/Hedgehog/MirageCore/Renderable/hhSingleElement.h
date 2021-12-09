#pragma once

#include <BlueBlur.h>
#include <Hedgehog/MirageCore/Renderable/hhElement.h>

namespace Hedgehog::Mirage
{
    class CInstanceInfo;
    class CSingleElement;
    class CModelData;

    static FUNCTION_PTR(CSingleElement*, __thiscall, fpCSingleElementCtor, 0x701D00,
        CSingleElement* This, const boost::shared_ptr<CModelData>& spModelData);

    class CSingleElement : public CElement
    {
    public:
        boost::shared_ptr<CInstanceInfo> m_spInstanceInfo;
        INSERT_PADDING(0x14);

        CSingleElement(const null_ctor&) : CElement(null_ctor{}) {}

        CSingleElement(const boost::shared_ptr<CModelData>& spModelData) : CSingleElement(null_ctor{})
        {
            fpCSingleElementCtor(this, spModelData);
        }
    };

    ASSERT_OFFSETOF(CSingleElement, m_spInstanceInfo, 0x8C);
    ASSERT_SIZEOF(CSingleElement, 0xA8);
}