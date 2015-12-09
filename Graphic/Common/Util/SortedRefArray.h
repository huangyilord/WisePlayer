#if !defined ( _SORTED_REF_ARRAY_H_ )
#define _SORTED_REF_ARRAY_H_

#include "SlotArray.h"

namespace Util
{
    template <class T>
    class SortedRefArray
    {
    public:
        SortedRefArray();
        ~SortedRefArray();

        BOOL InsertItem( T* pItem );
        VOID RemoveItem( T* pItem );

        inline VOID Clear() { m_items.Clear(); }
        inline const T* GetItem( UINT32 index ) const { return *m_items.GetSlotItem( index ); }
        inline UINT32 GetItemIndex( const T* pItem ) const { return pItem->_sortedRefArrayItem.index; }

        inline UINT32 GetCount() const { return m_items.GetCount(); }

        typedef struct SortedRefArrayItem
        {
            UINT32  index;
            T*      pPrev;
            T*      pNext;
        } SortedRefArrayItem;
    private:
        typedef Util::SlotArray<T*, 256>    TRefArray;
        TRefArray                           m_items;
        T*                                  m_pFirstIndex;
    };

    template <class T>
    SortedRefArray<T>::SortedRefArray()
        : m_pFirstIndex( NULL )
    {
    }

    template <class T>
    SortedRefArray<T>::~SortedRefArray()
    {
        Clear();
    }

    template <class T>
    BOOL SortedRefArray<T>::InsertItem( T* pItem )
    {
        INT32 slot = m_items.GenSlot();
        if ( slot < 0 )
        {
            return FALSE;
        }
        ASSERT( (UINT32)slot == (m_items.GetCount() - 1) );
        
        if ( NULL == m_pFirstIndex )
        {
            pItem->_sortedRefArrayItem.index = slot;
            *m_items.GetSlotItem( slot ) = pItem;
            m_pFirstIndex = pItem->_sortedRefArrayItem.pNext = pItem->_sortedRefArrayItem.pPrev = pItem;
        }
        else
        {
            T* pCurrentIndex = m_pFirstIndex;
            do
            {
                T* pNext = pCurrentIndex->_sortedRefArrayItem.pNext;
                INT32 compare = *pItem - *pCurrentIndex;
                if ( 0 > compare )
                {
                    // create new index
                    pItem->_sortedRefArrayItem.pPrev = pCurrentIndex->_sortedRefArrayItem.pPrev;
                    pItem->_sortedRefArrayItem.pNext = pCurrentIndex;
                    pItem->_sortedRefArrayItem.pPrev->_sortedRefArrayItem.pNext = pItem;
                    pCurrentIndex->_sortedRefArrayItem.pPrev = pItem;
                    if ( m_pFirstIndex == pCurrentIndex )
                    {
                        m_pFirstIndex = pItem;
                    }
                    break;
                }
                else if ( 0 == compare )
                {
                    pCurrentIndex = pNext;
                    break;
                }

                pCurrentIndex = pNext;
            } while ( pCurrentIndex != m_pFirstIndex );

            if ( pCurrentIndex == m_pFirstIndex )
            {
                // add to the end of array
                pItem->_sortedRefArrayItem.index = slot;
                *m_items.GetSlotItem( slot ) = pItem;

                if ( (*pItem - *pCurrentIndex->_sortedRefArrayItem.pPrev) > 0 )
                {
                    // create new index if pItem bigger than all
                    pItem->_sortedRefArrayItem.pPrev = pCurrentIndex->_sortedRefArrayItem.pPrev;
                    pItem->_sortedRefArrayItem.pNext = pCurrentIndex;
                    pItem->_sortedRefArrayItem.pPrev->_sortedRefArrayItem.pNext = pItem;
                    pCurrentIndex->_sortedRefArrayItem.pPrev = pItem;
                }
                else
                {
                    pItem->_sortedRefArrayItem.pPrev = pItem->_sortedRefArrayItem.pNext = NULL;
                }
                return TRUE;
            }

            do
            {
                T* pNext = pCurrentIndex->_sortedRefArrayItem.pNext;
                // pItem take pCurrentIndex's position, and hold pCurrentIndex
                pItem->_sortedRefArrayItem.index = pCurrentIndex->_sortedRefArrayItem.index;
                *m_items.GetSlotItem( pItem->_sortedRefArrayItem.index ) = pItem;
                pItem = pCurrentIndex;

                T** nextSlotItem = NULL;
                // next slot is not the added slot
                if ( pItem->_sortedRefArrayItem.index + 2 < m_items.GetCount() )
                {
                    nextSlotItem = m_items.GetSlotItem( pItem->_sortedRefArrayItem.index + 1 );
                }
                if ( NULL != nextSlotItem && *nextSlotItem != pCurrentIndex->_sortedRefArrayItem.pNext )
                {
                    // replace pCurrentIndex with next item
                    ASSERT( NULL == (*nextSlotItem)->_sortedRefArrayItem.pNext );
                    ASSERT( NULL == (*nextSlotItem)->_sortedRefArrayItem.pPrev );
                    (*nextSlotItem)->_sortedRefArrayItem.pNext = pCurrentIndex->_sortedRefArrayItem.pNext;
                    pCurrentIndex->_sortedRefArrayItem.pNext->_sortedRefArrayItem.pPrev = *nextSlotItem;
                    (*nextSlotItem)->_sortedRefArrayItem.pPrev = pCurrentIndex->_sortedRefArrayItem.pPrev;
                    pCurrentIndex->_sortedRefArrayItem.pPrev->_sortedRefArrayItem.pNext = *nextSlotItem;
                    pCurrentIndex->_sortedRefArrayItem.pNext = pCurrentIndex->_sortedRefArrayItem.pPrev = NULL;
                }
                pCurrentIndex = pNext;
            } while ( pCurrentIndex != m_pFirstIndex );

            // put pItem at the end of array
            pItem->_sortedRefArrayItem.index = slot;
            *m_items.GetSlotItem( slot ) = pItem;
        }
        return TRUE;
    }

    template <class T>
    VOID SortedRefArray<T>::RemoveItem( T* pItem )
    {
        if ( NULL != pItem->_sortedRefArrayItem.pNext )
        {
            // pItem is used as a index
            // find the next item to replace this index
            T** pNextSlot = m_items.GetSlotItem( pItem->_sortedRefArrayItem.index + 1 );
            if ( NULL == pNextSlot || *pNextSlot == pItem->_sortedRefArrayItem.pNext )
            {
                // pItem is the only one of this index
                // remove this index
                pItem->_sortedRefArrayItem.pPrev->_sortedRefArrayItem.pNext = pItem->_sortedRefArrayItem.pNext;
                pItem->_sortedRefArrayItem.pNext->_sortedRefArrayItem.pPrev = pItem->_sortedRefArrayItem.pPrev;
                if ( m_pFirstIndex == pItem )
                {
                    if ( pItem == pItem->_sortedRefArrayItem.pNext )
                    {
                        // pItem is the only one item
                        ASSERT( 1 == m_items.GetCount() );
                        m_items.FreeSlot( pItem->_sortedRefArrayItem.index );
                        ASSERT( 0 == m_items.GetCount() );
                        m_pFirstIndex = NULL;
                        return;
                    }
                    else
                    {
                        m_pFirstIndex = pItem->_sortedRefArrayItem.pNext;
                    }
                }
            }
            else
            {
                // use the next item replace this index
                pItem->_sortedRefArrayItem.pPrev->_sortedRefArrayItem.pNext = (*pNextSlot);
                (*pNextSlot)->_sortedRefArrayItem.pPrev = pItem->_sortedRefArrayItem.pPrev;
                (*pNextSlot)->_sortedRefArrayItem.pNext = pItem->_sortedRefArrayItem.pNext;
                pItem->_sortedRefArrayItem.pNext->_sortedRefArrayItem.pPrev = (*pNextSlot);
                if ( m_pFirstIndex == pItem )
                {
                    m_pFirstIndex = (*pNextSlot);
                }
            }
            pItem->_sortedRefArrayItem.pNext = pItem->_sortedRefArrayItem.pPrev = NULL;
        }

        UINT32 uEmptySlot = pItem->_sortedRefArrayItem.index;
        if ( uEmptySlot == m_items.GetCount() - 1 )
        {
            m_items.FreeSlot( uEmptySlot );
            return;
        }
        T* pCurrentIndex = m_pFirstIndex;
        do
        {
            T* pNext = pCurrentIndex->_sortedRefArrayItem.pNext;
            if ( pCurrentIndex->_sortedRefArrayItem.index > pItem->_sortedRefArrayItem.index )
            {
                if ( pCurrentIndex->_sortedRefArrayItem.index - pItem->_sortedRefArrayItem.index > 1 )
                {
                    // move last one of previous index to the empty slot
                    T** pEmptySlot = m_items.GetSlotItem( uEmptySlot );
                    T** pSwapSlot = m_items.GetSlotItem( pCurrentIndex->_sortedRefArrayItem.index - 1 );
                    ASSERT( *pEmptySlot == pItem );
                    ASSERT( NULL != pSwapSlot );
                    *pEmptySlot = *pSwapSlot;
                    (*pSwapSlot)->_sortedRefArrayItem.index = uEmptySlot;
                    uEmptySlot = pCurrentIndex->_sortedRefArrayItem.index - 1;
                }
                break;
            }

            pCurrentIndex = pNext;
        } while ( pCurrentIndex != m_pFirstIndex );

        if ( pCurrentIndex == m_pFirstIndex && pCurrentIndex->_sortedRefArrayItem.index < pItem->_sortedRefArrayItem.index )
        {
            // empty slot is behind all index, just swap empty slot with last one
            T** pEmptySlot = m_items.GetSlotItem( uEmptySlot );
            T** pSwapSlot = m_items.GetSlotItem( m_items.GetCount() - 1 );
            ASSERT( NULL != pEmptySlot );
            ASSERT( NULL != pSwapSlot );
            *pEmptySlot = *pSwapSlot;
            (*pSwapSlot)->_sortedRefArrayItem.index = uEmptySlot;
            m_items.FreeSlot( m_items.GetCount() - 1 );
            return;
        }

        do
        {
            T* pNext = pCurrentIndex->_sortedRefArrayItem.pNext;
            UINT32 uSwapSlot = pNext->_sortedRefArrayItem.index - 1;
            if ( pNext == m_pFirstIndex )
            {
                uSwapSlot = m_items.GetCount() - 1;
            }
            // move last one of previous index to the empty slot
            T** pEmptySlot = m_items.GetSlotItem( uEmptySlot );
            T** pSwapSlot = m_items.GetSlotItem( uSwapSlot );
            ASSERT( NULL != pEmptySlot );
            ASSERT( NULL != pSwapSlot );
            *pEmptySlot = *pSwapSlot;

            // replace pCurrentIndex with moved item
            if ( uSwapSlot != pCurrentIndex->_sortedRefArrayItem.index )
            {
                (*pSwapSlot)->_sortedRefArrayItem.pPrev = pCurrentIndex->_sortedRefArrayItem.pPrev;
                pCurrentIndex->_sortedRefArrayItem.pPrev->_sortedRefArrayItem.pNext = (*pSwapSlot);
                (*pSwapSlot)->_sortedRefArrayItem.pNext = pCurrentIndex->_sortedRefArrayItem.pNext;
                pCurrentIndex->_sortedRefArrayItem.pNext->_sortedRefArrayItem.pPrev = (*pSwapSlot);
                pCurrentIndex->_sortedRefArrayItem.pPrev = pCurrentIndex->_sortedRefArrayItem.pNext = NULL;
            }
            (*pSwapSlot)->_sortedRefArrayItem.index = uEmptySlot;
            uEmptySlot = uSwapSlot;

            pCurrentIndex = pNext;
        } while ( pCurrentIndex != m_pFirstIndex );

        m_items.FreeSlot( m_items.GetCount() - 1 );
    }
}

#endif
