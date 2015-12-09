#if !defined ( _SET_H_ )
#define _SET_H_

#include "List.h"
#include "HashTable.h"

namespace Util
{
    template <class T>
    class Set
    {
    public:
        Set() {}
        ~Set() { Clear(); }

        BOOL Add( const T& val );
        VOID Remove( const T& val );

        inline BOOL Exist( const T& val ) const;
        inline VOID Clear();

        typedef typename List<T>::Iterator Iterator;

        inline Iterator Begin() const { return m_list.Begin(); }
        inline Iterator End() const { return m_list.End(); }
        inline VOID Erase( Iterator& iter );
        inline UINT32 Size() const { return m_list.Size(); }
        inline BOOL IsEmpty() const { return m_list.IsEmpty(); }
    private:
        typedef List<T> ItemList;
        typedef HashTable<T, typename ItemList::ListNode*, 1024, 256> ItemTable;
        ItemList            m_list;
        ItemTable           m_table;
    };

    template <class T>
    BOOL Set<T>::Add( const T& val )
    {
        if ( !m_list.PushBack( val ) )
        {
            return FALSE;
        }
        if ( !m_table.Insert( val, m_list.End().m_pListNode ) )
        {
            m_list.PopBack();
            return FALSE;
        }
        return TRUE;
    }

    template <class T>
    VOID Set<T>::Remove( const T& val )
    {
        typename ItemList::ListNode* node = NULL;
        if ( m_table.Get( val, node ) )
        {
            typename ItemList::Iterator iter( node );
            m_table.Remove( val );
            m_list.Erase( iter );
        }
    }

    template <class T>
    BOOL Set<T>::Exist( const T& val ) const
    {
        typename ItemList::ListNode* node = NULL;
        return m_table.Get( val, node );
    }

    template <class T>
    VOID Set<T>::Erase( Iterator& iter )
    {
        m_table.Remove( iter.Get() );
        m_list.Erase( iter );
    }

    template <class T>
    VOID Set<T>::Clear()
    {
        m_list.Clear();
        m_table.Clear();
    }
}

#endif
