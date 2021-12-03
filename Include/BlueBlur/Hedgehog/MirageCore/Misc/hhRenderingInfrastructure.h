#pragma once

#include <BlueBlur.h>

#include <Hedgehog/Base/hhObject.h>
#include <Hedgehog/MirageCore/Misc/hhRenderingDevice.h>

namespace Hedgehog::Mirage
{
    class CRenderingInfrastructure : public Hedgehog::Base::CObject
    {
    public:
        virtual ~CRenderingInfrastructure() = default;

        INSERT_PADDING(0x5C);
        CRenderingDevice m_RenderingDevice;
    };

    ASSERT_OFFSETOF(CRenderingInfrastructure, m_RenderingDevice, 0x60);
    ASSERT_SIZEOF(CRenderingInfrastructure, 0x7650);
}