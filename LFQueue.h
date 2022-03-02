#ifndef LFQUEUE_H
#define LFQUEUE_H

#include <atomic>

namespace MaxLib
{

template<typename T>
class LockFreeQueue
{
protected:
    struct QueueNode
    {
        T* data;
        QueueNode* next;

        QueueNode(): data(nullptr), next(nullptr){}
    };

    QueueNode* m_head;
    QueueNode* m_tail;
    int m_limit;
    std::atomic<int> m_len;

public:
    LockFreeQueue(const LockFreeQueue& obj) = delete;
    LockFreeQueue& operator= (const LockFreeQueue& obj) = delete;
    
    LockFreeQueue(int capacity): 
        m_head(), m_tail(new QueueNode()), m_limit(capacity), m_len(0)
    {
        m_head = m_tail;
        m_head->next = nullptr;
    }

    bool push(T* data)
    {
        bool ret = capacity() >= size();

        if(ret)
        {
            QueueNode* node = new QueueNode();
            node->data = data;
            node->next = nullptr;
            
            QueueNode* p = m_tail;
            QueueNode* old = p;

            do
            {
                while(p && (p->next != nullptr))
                {
                    p = p->next;
                }
                
            } while (!__sync_bool_compare_and_swap(&(p->next), nullptr, node));

            __sync_bool_compare_and_swap(&m_tail, old, node);

            m_len++;
        }

        return ret;
    }


    T* pop()
    {
        T* ret = nullptr;

        if(size() > 0)
        {
            QueueNode* p = nullptr;

            do{
                p = m_head->next;

                if(p == nullptr)
                {
                    return nullptr;
                }

            }while(!__sync_bool_compare_and_swap(&m_head->next, p, p->next));   

            ret = p->data;
            m_len--;

            if(m_len == 0)
            {
                while(!__sync_bool_compare_and_swap(&m_tail, p, m_head));
            }

            delete p;
        }

        return ret;
    }

    void clear()
    {
        while(size() > 0)
        {
            pop();
        }
    }

    int capacity()
    {
        return m_limit;
    }

    int size()
    {
        return m_len;
    }

    bool isFull()
    {
        return capacity() == size() ? true : false;
    }

    virtual ~LockFreeQueue()
    {
        clear();
    }
};

}


#endif