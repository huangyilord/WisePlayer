#if !defined ( _ARRAY_LIST_H_ )
#define _ARRAY_LIST_H_

#include "MemCache.h"

namespace Util
{
    template<class T, UINT32 blockSize = 31>
    class ArrayList
    {
    public:
        ArrayList();
        ~ArrayList();

        BOOL PushBack();

        T& Get( UINT32 index ) const;

        VOID PopBack();

        T& Front() const;
        T& Back() const;

        inline BOOL IsEmpty() const { return m_count == 0; }
        inline UINT32 Size() const { return m_count; }

        VOID Clear();
    private:
        union Item;
        struct BlockInformation
        {
            UINT32  blockUsed;
            Item*   prevBlock;
            Item*   nextBlock;
        };
        union Item
        {
            T                   val;
            BlockInformation    blockInfo;
        };
        typedef MemCache<sizeof(Item) * (blockSize + 1)>  BlockCache;

        Item*           m_pFirstBlock;
        Item*           m_pLastBlock;
        UINT32          m_count;
    };

    template<class T, UINT32 blockSize>
    ArrayList<T, blockSize>::ArrayList()
        : m_pFirstBlock( NULL )
        , m_pLastBlock( NULL )
        , m_count( 0 )
    {
    }

    template<class T, UINT32 blockSize>
    ArrayList<T, blockSize>::~ArrayList()
    {
        Clear();
    }

    template<class T, UINT32 blockSize>
    BOOL ArrayList<T, blockSize>::PushBack()
    {
        if ( NULL != m_pLastBlock )
        {
            Item& item = m_pLastBlock[blockSize];
            if ( item.blockInfo.blockUsed < blockSize )
            {
                ++item.blockInfo.blockUsed;
                ++m_count;
                return TRUE;
            }
            ASSERT( item.blockInfo.nextBlock == NULL );
            item.blockInfo.nextBlock = (Item*)BlockCache::Alloc();
            if ( NULL == item.blockInfo.nextBlock )
            {
                return FALSE;
            }
            
            Item& newItem = item.blockInfo.nextBlock[blockSize];
            newItem.blockInfo.prevBlock = m_pLastBlock;
            newItem.blockInfo.nextBlock = NULL;
            newItem.blockInfo.blockUsed = 1;
            m_pLastBlock = item.blockInfo.nextBlock;
            ++ m_count;
            return TRUE;
        }

        m_pLastBlock = (Item*)BlockCache::Alloc();
        if ( NULL == m_pLastBlock )
        {
            return FALSE;
        }
        m_pFirstBlock = m_pLastBlock;
        Item& newItem = m_pLastBlock[blockSize];
        newItem.blockInfo.prevBlock = m_pLastBlock;
        newItem.blockInfo.nextBlock = NULL;
        newItem.blockInfo.blockUsed = 1;
        ++ m_count;
        return TRUE;
    }

    template<class T, UINT32 blockSize>
    T& ArrayList<T, blockSize>::Get( UINT32 index ) const
    {
        Item* pBlock = m_pFirstBlock;
        while ( NULL != pBlock )
        {
            Item& blockItem = pBlock[blockSize];
            if ( index < blockItem.blockInfo.blockUsed )
            {
                pBlock += index;
                break;
            }
            index -= blockItem.blockInfo.blockUsed;
            pBlock = blockItem.blockInfo.nextBlock;
        }
        return pBlock->val;
    }

    template<class T, UINT32 blockSize>
    VOID ArrayList<T, blockSize>::PopBack()
    {
        if ( NULL != m_pLastBlock )
        {
            Item& blockItem = m_pLastBlock[blockSize];
            ASSERT( blockItem.blockInfo.blockUsed > 0 );
            -- m_count;
            -- blockItem.blockInfo.blockUsed;
            if ( 0 == blockItem.blockInfo.blockUsed )
            {
                Item* pLastBlock = blockItem.blockInfo.prevBlock;
                BlockCache::Free( m_pLastBlock );
                m_pLastBlock = pLastBlock;
                if ( NULL == m_pLastBlock )
                {
                    m_pFirstBlock = NULL;
                    ASSERT( m_count == 0 );
                }
            }
        }
    }

    template<class T, UINT32 blockSize>
    T& ArrayList<T, blockSize>::Front() const
    {
        return m_pFirstBlock[0].val;
    }

    template<class T, UINT32 blockSize>
    T& ArrayList<T, blockSize>::Back() const
    {
        Item& blockItem = m_pLastBlock[blockSize];
        return m_pLastBlock[blockItem.blockInfo.blockUsed - 1].val;
    }

    template<class T, UINT32 blockSize>
    VOID ArrayList<T, blockSize>::Clear()
    {
        Item* pBlock = m_pFirstBlock;
        while ( NULL != pBlock )
        {
            Item* pNextBlock = pBlock[blockSize].blockInfo.nextBlock;
            BlockCache::Free( pBlock );
            pBlock = pNextBlock;
        }
        m_pFirstBlock = NULL;
        m_pLastBlock = NULL;
        m_count = 0;
    }
}

#endif
