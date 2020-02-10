#pragma once

template<typename TValue>
struct TreeNode
{
    TreeNode* left;
    TreeNode* parent;
    TreeNode* right;

    TValue value;

    int8_t color;
    bool isNil;

    TreeNode* next()
    {
        TreeNode* ptr = this;

        if (ptr->right->isNil)
        {
            TreeNode* node;
            while (!(node = ptr->parent)->isNil && ptr == node->right)
                ptr = node;

            ptr = node;
        }
        else
        {
            ptr = ptr->right;

            while (!ptr->left->isNil)
                ptr = ptr->left;
        }

        return ptr;
    }
};

template<typename TValue>
struct Tree
{
    TreeNode<TValue>* head;
    size_t count;

    TreeNode<TValue>* begin() const
    {
        return count ? head->left : head;
    }

    TreeNode<TValue>* end() const
    {
        return head;
    }
};

template<typename TKey, typename TValue>
struct Pair
{
    TKey key;
    TValue value;
};

template<typename TKey, typename TValue>
struct Map : Tree<Pair<TKey, TValue>>
{
};

template<typename T>
struct Vector
{
    T* begin;
    T* end;
    T* capacityEnd;
};

template<typename T>
struct ListNode
{
    ListNode* next;
    ListNode* previous;
    T value;
};

template<typename T>
struct List
{
    ListNode<T>* head;
    size_t count;

    ListNode<T>* begin() const
    {
        return count ? head->next : head;
    }

    ListNode<T>* end() const
    {
        return head;
    }

    ListNode<T>* find(const T& value) const
    {
        for (auto it = begin(); it != end(); it = it->next)
        {
            if (it->value == value)
                return it;
        }

        return head;
    }
};