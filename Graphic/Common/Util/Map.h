#if !defined ( _MAP_H_ )
#define _MAP_H_

#include "List.h"
#include "HashTable.h"

namespace Util
{
    template <class K, class V, UINT16 indexSize = 1024, UINT16 blockSize = 256>
    class Map
    {
    public:
        Map() {}
        ~Map() { Clear(); }

        BOOL Insert( const K& key, const V& val );
        VOID Remove( const K& key );

        inline BOOL Get( const K& key, V& val ) const;
        inline V& Get( const K& key ) const { return m_table.Get( key ).val; }
        inline BOOL Exist( const K& key ) const { return m_table.Exist( key ); }
        inline VOID Clear();

        typedef typename List<K>::Iterator Iterator;

        inline Iterator Begin() const { return m_list.Begin(); }
        inline Iterator End() const { return m_list.End(); }
        inline VOID Erase( Iterator& iter );
        inline UINT32 Size() const { return m_list.Size(); }
        inline BOOL IsEmpty() const { return m_list.IsEmpty(); }
        inline BOOL Get( const Iterator& iter, V& val ) const { return Get( iter.Get(), val ); }
        inline V& Get( const Iterator& iter ) const { return Get( iter.Get() ); }
    private:
        typedef List<K> KeyList;
        typedef struct _Item
        {
            typename KeyList::ListNode*     listNode;
            V                               val;
        } Item;
        typedef HashTable<K, Item, indexSize, blockSize> ItemTable;
        KeyList             m_list;
        ItemTable           m_table;
    };

    template <class K, class V, UINT16 indexSize, UINT16 blockSize>
    BOOL Map<K, V, indexSize, blockSize>::Insert( const K& key, const V& val )
    {
        if ( !m_list.PushBack( key ) )
        {
            return FALSE;
        }
        Item item = { m_list.End().m_pListNode, val };
        if ( !m_table.Insert( key, item ) )
        {
            m_list.PopBack();
            return FALSE;
        }
        return TRUE;
    }

    template <class K, class V, UINT16 indexSize, UINT16 blockSize>
    VOID Map<K, V, indexSize, blockSize>::Remove( const K& key )
    {
        Item item = { 0 };
        if ( m_table.Get( key, item ) )
        {
            typename KeyList::Iterator iter( item.listNode );
            m_table.Remove( key );
            m_list.Erase( iter );
        }
    }

    template <class K, class V, UINT16 indexSize, UINT16 blockSize>
    BOOL Map<K, V, indexSize, blockSize>::Get( const K& key, V& val ) const
    {
        Item item = { 0 };
        if ( m_table.Get( key, item ) )
        {
            val = item.val;
            return TRUE;
        }
        return FALSE;
    }

    template <class K, class V, UINT16 indexSize, UINT16 blockSize>
    VOID Map<K, V, indexSize, blockSize>::Erase( Iterator& iter )
    {
        m_table.Remove( iter.Get() );
        m_list.Erase( iter );
    }

    template <class K, class V, UINT16 indexSize, UINT16 blockSize>
    VOID Map<K, V, indexSize, blockSize>::Clear()
    {
        m_list.Clear();
        m_table.Clear();
    }
}

#endif
