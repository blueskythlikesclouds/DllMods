#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Database/System/hhDatabaseData.h>

namespace Hedgehog::Motion
{
    class CLightSubMotionValueData
    {
    public:
        float m_Data[28];
    };

    ASSERT_SIZEOF(CLightSubMotionValueData, 0x70);

    class CLightSubMotionData
    {
    public:
        INSERT_PADDING(0x24);
        float m_FrameRate;
        float m_StartFrame;
        float m_EndFrame;
        CLightSubMotionValueData m_DefaultValueData;
        INSERT_PADDING(0x8);
    };

    ASSERT_OFFSETOF(CLightSubMotionData, m_FrameRate, 0x24);
    ASSERT_OFFSETOF(CLightSubMotionData, m_StartFrame, 0x28);
    ASSERT_OFFSETOF(CLightSubMotionData, m_EndFrame, 0x2C);
    ASSERT_OFFSETOF(CLightSubMotionData, m_DefaultValueData, 0x30);
    ASSERT_SIZEOF(CLightSubMotionData, 0xA8);

    class CLightMotionData;

    static inline FUNCTION_PTR(void, __cdecl, fpCLightMotionDataStep, 0x758640, 
        CLightMotionData* This, uint32_t index, float frame, CLightSubMotionValueData& valueData, const Math::CMatrix& matrix);

    class CLightMotionData : public Hedgehog::Database::CDatabaseData
    {
    public:
        uint32_t m_SubMotionCount;
        CLightSubMotionData* m_SubMotions;
        INSERT_PADDING(0xC);

        void Step(uint32_t index, float frame, CLightSubMotionValueData& valueData, const Math::CMatrix& matrix = Math::CMatrix::Identity())
        {
            fpCLightMotionDataStep(this, index, frame, valueData, matrix);
        }
    };

    ASSERT_OFFSETOF(CLightMotionData, m_SubMotionCount, 0xC);
    ASSERT_OFFSETOF(CLightMotionData, m_SubMotions, 0x10);
    ASSERT_SIZEOF(CLightMotionData, 0x20);
}
