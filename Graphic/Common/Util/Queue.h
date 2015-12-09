#ifndef _MY_QUEUE_
#define _MY_QUEUE_

#include "ClassCache.h"
#include "SystemAPI/SystemThreadingAPI.h"
#ifdef WIN32
#pragma warning(disable: 4127)
#endif
namespace Util
{
    template <class T>
    class QueueNode
    {
    public:
        QueueNode() : pData(NULL), next(NULL), prev(NULL)
        {

        }

        T* pData;
        QueueNode* next;
        QueueNode* prev;
    };

    template <class T, bool bThreadSafe = false>
    class Queue
    {
    public:
        Queue() : head(NULL), tail(NULL), size(0)
        {
            if (bThreadSafe)
            {
                m_lock = SystemAPI::ThreadingAPI::CreateLock();
            }
        }

        ~Queue()
        {
            if (bThreadSafe)
            {
                SystemAPI::ThreadingAPI::DestroyLock(m_lock);
            }
        }

        bool PushFront(T* pData)
        {
            if (!pData)
            {
                return true;
            }

            if (bThreadSafe)
            {
                SystemAPI::ThreadingAPI::Lock(m_lock);
            }

            QueueNode<T>* pNewQueueNode = ClassCache<QueueNode<T> >::Alloc();
            bool retValue = true;
            if (pNewQueueNode)
            {
                size++;
                pNewQueueNode->pData = pData;

                QueueNode<T>* originalHead = head;
                head = pNewQueueNode;

                if (originalHead)
                {
                    originalHead->prev = head;
                    head->next = originalHead;
                    head->prev = NULL;
                }
                else
                {
                    head->next = NULL;
                    head->prev = NULL;
                    tail = pNewQueueNode;
                }
            }
            else
            {
                retValue = false;
            }

            if (bThreadSafe)
            {
                SystemAPI::ThreadingAPI::Unlock(m_lock);
            }

            return retValue;
        }

        T* PopBack()
        {
            if (bThreadSafe)
            {
                SystemAPI::ThreadingAPI::Lock(m_lock);
            }

            QueueNode<T>* returnNode = NULL;

            T* returnValue = NULL;
            if (tail)
            {
                size--;
                returnNode = tail;
                tail = returnNode->prev;
                if (tail == NULL)
                {
                    head = NULL;
                }
                else
                {
                    tail->next = NULL;
                }

                returnValue = returnNode->pData;
                ClassCache<QueueNode<T> >::Free(returnNode);
            }
            else
            {
                returnNode = NULL;
            }
          

            if (bThreadSafe)
            {
                SystemAPI::ThreadingAPI::Unlock(m_lock);
            }

            
            return returnValue;
        }

        int GetSizeNoThreadSafe()
        {
            return size;
        }

    private:
        QueueNode<T>* head;
        QueueNode<T>* tail;

        int size;

        LOCK_HANDLE m_lock;
    };


}


#endif