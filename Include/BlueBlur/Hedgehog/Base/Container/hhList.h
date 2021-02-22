#pragma once

namespace hh
{
    template<typename T>
    class list
    {
    public:
        struct node
        {
            node* m_pNext;
            node* m_pPrevious;
            T m_Value;
        };

        INSERT_PADDING(0x4);
        node* m_pHead;
        size_t m_Count;

        node* begin() const
        {
            return m_Count ? m_pHead->m_pNext : m_pHead;
        }

        node* end() const
        {
            return m_pHead;
        }

        node* find(const T& value) const
        {
            for (auto it = begin(); it != end(); it = it->m_pNext)
            {
                if (it->m_Value == value)
                    return it;
            }

            return m_pHead;
        }
    };
}