#pragma once

namespace hh
{
    template<typename T>
    class tree
    {
    public:
        struct node
        {
            node* m_pLeft;
            node* m_pParent;
            node* m_pRight;

            T m_Value;

            int8_t m_Color;
            bool m_IsNil;

            T* next()
            {
                T* ptr = this;

                if (ptr->m_pRight->m_IsNil)
                {
                    T* node;
                    while (!(node = ptr->m_pParent)->m_IsNil && ptr == node->m_pRight)
                        ptr = node;

                    ptr = node;
                }
                else
                {
                    ptr = ptr->m_pRight;

                    while (!ptr->m_pLeft->m_IsNil)
                        ptr = ptr->m_pLeft;
                }

                return ptr;
            }
        };

        node* m_pHead;
        size_t m_Count;

        node* begin() const
        {
            return m_Count ? m_pHead->m_pLeft : m_pHead;
        }

        node* end() const
        {
            return m_pHead;
        }
    };
}