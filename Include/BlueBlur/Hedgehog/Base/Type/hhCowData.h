#pragma once

#include <Hedgehog/Base/System/hhAllocator.h>

namespace Hedgehog::Base
{
	template<typename T, size_t TFooter = 0>
	class CCowData
	{
	protected:
		class CData
		{
		public:
			uint16_t m_RefCount;
			uint16_t m_Length;
			T m_Data[1];
		};

		static constexpr const T* ms_memStatic = (const T*)0x13E0DC0;

		const T* m_ptr;

		bool IsMemStatic() const
		{
			return !m_ptr || m_ptr == ms_memStatic;
		}

		CData* GetData() const
		{
			return (CData*)((char*)m_ptr - offsetof(CData, m_Data));
		}

		void SetData(CData* pData)
		{
			m_ptr = (const T*)((char*)pData + offsetof(CData, m_Data));
		}

	public:
		const T* Get() const
		{
			return IsMemStatic() ? ms_memStatic : m_ptr;
		}

		void Unset()
		{
			if (!IsMemStatic() && InterlockedDecrement16((short*)&GetData()->m_RefCount) == 0)
				__HH_FREE(GetData());

			m_ptr = ms_memStatic;
		}

		void Set(const CCowData& other)
		{
			m_ptr = other.m_ptr;

			if (!IsMemStatic())
				InterlockedIncrement16((short*)&GetData()->m_RefCount);
		}

		void Set(const T* pPtr, const size_t length)
		{
			Unset();

			if (!length) 
				return;

			CData* pData = (CData*)__HH_ALLOC(offsetof(CData, m_Data) + (length + TFooter) * sizeof(T));
			pData->m_RefCount = 1;
			pData->m_Length = (uint16_t)length;

			if (pPtr)
			{
				memcpy(pData->m_Data, pPtr, length * sizeof(T));
				memset(&pData->m_Data[length], 0, TFooter * sizeof(T));
			}

			SetData(pData);
		}

		CCowData() : m_ptr(ms_memStatic)
		{

		}

		CCowData(CCowData&& other)
		{
			m_ptr = other.m_ptr;
			other.m_ptr = ms_memStatic;
		}

		CCowData(const CCowData& other)
		{
			Set(other);
		}

		~CCowData()
		{
			Unset();
		}
	};
}