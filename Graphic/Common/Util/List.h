#if !defined ( _LIST_H_ )
#define _LIST_H_

#include "ArrayList.h"

namespace Util
{
    template<class T>
    class List
    {
    public:
        List();
        ~List();

        BOOL PushBack( const T& val );
        BOOL PushFront( const T& val );

        VOID PopBack();
        VOID PopFront();

        T& Front();
        T& Back();

        inline BOOL IsEmpty() const { return m_count == 0; }
        inline UINT32 Size() const { return m_count; }

        VOID Clear();
    public:
        typedef struct _ListNode
        {
            _ListNode*  prev;
            _ListNode*  next;
            T           val;
        } ListNode;

        class Iterator
        {
        public:
            Iterator( ListNode* pNode ) : m_pListNode( pNode ) {}
            Iterator() : m_pListNode( NULL ) {}
            ~Iterator() {}
            inline BOOL IsValid() const { return m_pListNode != NULL; }
            inline VOID Next() { m_pListNode = m_pListNode->next; }
            inline VOID Prev() { m_pListNode = m_pListNode->prev; }
            inline T& Get() const { return m_pListNode->val; }

            ListNode*   m_pListNode;
            friend class List<T>;
        };

        Iterator Begin() const { return Iterator(m_pFirst); }
        Iterator End() const { return Iterator(m_pLast); }
        VOID Erase( Iterator& iter );
    private:
        typedef ArrayList<ListNode, 63> ListNodePool;

        UINT32          m_count;
        ListNode*       m_pFirst;
        ListNode*       m_pLast;
        ListNode*       m_pFirstFreeNode;
        ListNodePool    m_nodePool;
    };

    template<class T>
    List<T>::List()
        : m_count( 0 )
        , m_pFirst( NULL )
        , m_pLast( NULL )
        , m_pFirstFreeNode( NULL )
    {
    }

    template<class T>
    List<T>::~List()
    {
        Clear();
    }

    template<class T>
    VOID List<T>::PopBack()
    {
        if ( NULL != m_pLast )
        {
            ListNode* pLast = m_pLast;
            m_pLast = m_pLast->prev;
            if ( NULL == m_pLast )
            {
                // only one element
                m_pFirst = NULL;
            }
            else
            {
                m_pLast->next = NULL;
            }
            pLast->next = m_pFirstFreeNode;
            m_pFirstFreeNode = pLast;
            -- m_count;
        }
    }

    template<class T>
    BOOL List<T>::PushBack( const T& val )
    {
        ListNode* pNewNode = NULL;
        if ( NULL != m_pFirstFreeNode )
        {
            pNewNode = m_pFirstFreeNode;
            m_pFirstFreeNode = pNewNode->next;
        }
        else
        {
            if ( !m_nodePool.PushBack() )
            {
                return FALSE;
            }
            pNewNode = &m_nodePool.Back();
        }
        pNewNode->val = val;
        pNewNode->next = NULL;
        pNewNode->prev = m_pLast;
        m_pLast = pNewNode;
        ++ m_count;
        if ( NULL == m_pFirst )
        {
            m_pFirst = pNewNode;
        }
        return TRUE;
    }

    template<class T>
    BOOL List<T>::PushFront( const T& val )
    {
        ListNode* pNewNode = NULL;
        if ( NULL != m_pFirstFreeNode )
        {
            pNewNode = m_pFirstFreeNode;
            m_pFirstFreeNode = pNewNode->next;
        }
        else
        {
            if ( !m_nodePool.PushBack() )
            {
                return FALSE;
            }
            pNewNode = &m_nodePool.Back();
        }
        pNewNode->val = val;
        pNewNode->next = m_pFirst;
        pNewNode->prev = NULL;
        m_pFirst = pNewNode;
        ++ m_count;
        if ( NULL == m_pLast )
        {
            m_pLast = pNewNode;
        }
        return TRUE;
    }

    template<class T>
    T& List<T>::Front()
    {
        return m_pFirst->val;
    }

    template<class T>
    T& List<T>::Back()
    {
        return m_pLast->val;
    }

    template<class T>
    VOID List<T>::PopFront()
    {
        if ( NULL != m_pFirst )
        {
            ListNode* pFirst = m_pFirst;
            m_pFirst = m_pFirst->next;
            if ( NULL == m_pFirst )
            {
                // only one element
                m_pLast = NULL;
            }
            else
            {
                m_pFirst->prev = NULL;
            }
            pFirst->next = m_pFirstFreeNode;
            m_pFirstFreeNode = pFirst;
            -- m_count;
        }
    }

    template<class T>
    VOID List<T>::Clear()
    {
        m_count = 0;
        m_pFirst = NULL;
        m_pLast = NULL;
        m_pFirstFreeNode = NULL;
        m_nodePool.Clear();
    }

    template<class T>
    VOID List<T>::Erase( Iterator& iter )
    {
        ListNode* pListNode = iter.m_pListNode;
        iter.m_pListNode = pListNode->next;
        if ( NULL != pListNode->next )
        {
            pListNode->next->prev = pListNode->prev;
        }
        if ( NULL != pListNode->prev )
        {
            pListNode->prev->next = pListNode->next;
        }
        if ( m_pFirst == pListNode )
        {
            m_pFirst = pListNode->next;
        }
        if ( m_pLast == pListNode )
        {
            m_pLast = pListNode->prev;
        }
        pListNode->next = m_pFirstFreeNode;
        m_pFirstFreeNode = pListNode;
        -- m_count;
    }
}

#endif
