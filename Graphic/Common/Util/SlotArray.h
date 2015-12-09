#if !defined ( _SLOT_ARRAY_H_ )
#define _SLOT_ARRAY_H_

#include "ClassCache.h"

namespace Util
{
#define _SLOT_ARRAY_DEFAULT_BLOCK_SIZE_ 32
#define _SLOT_ARRAY_DEFAULT_INDEX_SIZE_ 253

#define _SLOT_CONTENT_FLAG_FREE_        0x1      

    template <class T, UINT32 uBlockSize = _SLOT_ARRAY_DEFAULT_BLOCK_SIZE_, UINT32 uIndexBlockSize = _SLOT_ARRAY_DEFAULT_INDEX_SIZE_>
    class SlotArray
	{
    public:
        SlotArray();
        ~SlotArray();

        INT32 GenSlot();
        VOID FreeSlot( INT32 slot );
        VOID Clear();
        T* GetSlotItem( INT32 slot );
        const T* GetSlotItem( INT32 slot ) const;
        inline UINT32 GetCount() const { return m_uActiveSlotCount; }
    private:
        struct _FreeSlotInfo;
        union _SlotContent;
        struct _SlotItem;
        struct _SlotItemBlock;
        struct _SlotIndexBlock;
        union _SlotBlock;

        typedef struct _FreeSlotInfo    FreeSlotInfo;
        typedef union _SlotContent      SlotContent;
        typedef struct _SlotItem        SlotItem;
        typedef struct _SlotItemBlock   SlotItemBlock;
        typedef struct _SlotIndexBlock  SlotIndexBlock;
        typedef union _SlotBlock        SlotBlock;

        struct _FreeSlotInfo
        {
            INT32               slot;
            SlotBlock*          pItemBlock;
            SlotItem*           next;
        };

        union _SlotContent
        {
            T                   value;
            FreeSlotInfo        freeInfo;
        };

        struct _SlotItem
        {
            UINT32              flag;
            SlotContent         content;
        };

        struct _SlotItemBlock
        {
            UINT32              itemCount;
            SlotItem            items[uBlockSize];
        };

        struct _SlotIndexBlock
        {
            SlotBlock*          blocks[uIndexBlockSize];
            SlotBlock**         freeList;
            UINT32              freeCount;
            SlotBlock*          next;
        };

        union _SlotBlock
        {
            SlotItemBlock       itemBlock;
            SlotIndexBlock      indexBlock;
        };

        typedef ClassCache<SlotBlock> BlockCache;
        
        UINT32          m_uTotalSlotCount;
        UINT32          m_uActiveSlotCount;
        SlotBlock*      m_pIndex;
        SlotItem*       m_pFreeList;
    };

    template <class T, UINT32 uBlockSize, UINT32 uIndexBlockSize>
    SlotArray<T, uBlockSize, uIndexBlockSize>::SlotArray()
        : m_pIndex( NULL )
        , m_pFreeList( NULL )
        , m_uTotalSlotCount( 0 )
        , m_uActiveSlotCount( 0 )
    {
    }

    template <class T, UINT32 uBlockSize, UINT32 uIndexBlockSize>
    SlotArray<T, uBlockSize, uIndexBlockSize>::~SlotArray()
    {
        Clear();
    }

    template <class T, UINT32 uBlockSize, UINT32 uIndexBlockSize>
    INT32 SlotArray<T, uBlockSize, uIndexBlockSize>::GenSlot()
    {
        if ( m_uTotalSlotCount >= 0x7FFFFFFF && m_uActiveSlotCount == m_uTotalSlotCount )
        {
            return -1;
        }
        // get a slot from free list
        if ( NULL != m_pFreeList )
        {
            INT32 ret = m_pFreeList->content.freeInfo.slot;
            m_pFreeList->flag &= ~_SLOT_CONTENT_FLAG_FREE_;
            SlotBlock* pItemBlock = m_pFreeList->content.freeInfo.pItemBlock;
            m_pFreeList = m_pFreeList->content.freeInfo.next;
            ++pItemBlock->itemBlock.itemCount;
            ++m_uActiveSlotCount;
            return ret;
        }

        // alloc new block
        SlotBlock* pItemBlock = BlockCache::Alloc();
        if ( NULL == pItemBlock )
        {
            return -1;
        }

        // find a index block
        SlotBlock* pIndex = m_pIndex;
        SlotBlock* pPrevIndex = NULL;
        UINT32 offset = 0;
        UINT32 indexCount = 0;
        while ( NULL != pIndex )
        {
            if ( NULL != pIndex->indexBlock.freeList )
            {
                offset = (UINT32)( (BYTE*)(pIndex->indexBlock.freeList) - (BYTE*)pIndex ) / sizeof(SlotBlock*);
                break;
            }
            pPrevIndex = pIndex;
            pIndex = pIndex->indexBlock.next;
            ++ indexCount;
        }
        // no index block is available, create new index block
        if ( NULL == pIndex )
        {
            pIndex = BlockCache::Alloc();
            if ( NULL == pIndex )
            {
                BlockCache::Free( pItemBlock );
                return -1;
            }
            if ( NULL == pPrevIndex )
            {
                m_pIndex = pIndex;
            }
            else
            {
                pPrevIndex->indexBlock.next = pIndex;
            }

            pIndex->indexBlock.next = NULL;
            pIndex->indexBlock.freeCount = uIndexBlockSize;
            // init free list
            pIndex->indexBlock.freeList = &pIndex->indexBlock.blocks[0];
            UINT32 count = uIndexBlockSize - 1;
            for ( UINT32 i = 0; i < count; ++i )
            {
                pIndex->indexBlock.blocks[i] = (SlotBlock*)&pIndex->indexBlock.blocks[i+1];
            }
            pIndex->indexBlock.blocks[count] = NULL;
        }
        pIndex->indexBlock.freeList = (SlotBlock**)*pIndex->indexBlock.freeList;
        pIndex->indexBlock.blocks[offset] = pItemBlock;
        --pIndex->indexBlock.freeCount;

        UINT32 baseSlot = indexCount * uIndexBlockSize * uBlockSize + offset * uBlockSize;
        pItemBlock->itemBlock.itemCount = 1;
        pItemBlock->itemBlock.items[0].flag = 0;
        // add free blocks to free list
        for ( UINT32 i = uBlockSize - 1; i > 0; --i )
        {
            SlotItem& item = pItemBlock->itemBlock.items[i];
            item.flag = _SLOT_CONTENT_FLAG_FREE_;
            item.content.freeInfo.next = m_pFreeList;
            item.content.freeInfo.pItemBlock = pItemBlock;
            item.content.freeInfo.slot = baseSlot + i; 
            m_pFreeList = &item;
        }

        ++ m_uActiveSlotCount;
        m_uTotalSlotCount += uBlockSize;

        return baseSlot;
    }

    template <class T, UINT32 uBlockSize, UINT32 uIndexBlockSize>
    VOID SlotArray<T, uBlockSize, uIndexBlockSize>::FreeSlot( INT32 slot )
    {
        if ( slot >= 0 )
        {
            SlotBlock* pPrevIndex = NULL;
            SlotBlock* pIndex = m_pIndex;
            UINT32 index = slot;
            while ( NULL != pIndex && index >= uIndexBlockSize * uBlockSize )
            {
                pPrevIndex = pIndex;
                pIndex = pIndex->indexBlock.next;
                index -= uIndexBlockSize * uBlockSize;
            }
            if ( NULL != pIndex )
            {
                UINT32 offset = index / uBlockSize;
                SlotBlock* pBlock = pIndex->indexBlock.blocks[offset];
                if ( NULL != pBlock )
                {
                    UINT32 n = index % uBlockSize;
                    SlotItem& item = pBlock->itemBlock.items[n];
                    // if slot is not freed
                    if ( !( item.flag & _SLOT_CONTENT_FLAG_FREE_ ) )
                    {
                        item.flag |= _SLOT_CONTENT_FLAG_FREE_;

                        if ( pBlock->itemBlock.itemCount > 1 )
                        {
                            --pBlock->itemBlock.itemCount;
                            // add to free list
                            FreeSlotInfo& freeInfo = item.content.freeInfo;
                            freeInfo.slot = slot;
                            freeInfo.pItemBlock = pBlock;
                            freeInfo.next = m_pFreeList;
                            m_pFreeList = &item;
                        }
                        else
                        {
                            // free this block
                            BlockCache::Free( pBlock );
                            pIndex->indexBlock.blocks[offset] = (SlotBlock*)pIndex->indexBlock.freeList;
                            pIndex->indexBlock.freeList = &pIndex->indexBlock.blocks[offset];
                            ++pIndex->indexBlock.freeCount;
                            m_uTotalSlotCount -= uBlockSize;

                            if ( pIndex->indexBlock.freeCount == uIndexBlockSize && NULL == pIndex->indexBlock.next )
                            {
                                // free this index block
                                BlockCache::Free( pIndex );
                                if ( NULL == pPrevIndex )
                                {
                                    m_pIndex = NULL;
                                }
                                else
                                {
                                    pPrevIndex->indexBlock.next = NULL;
                                }
                                m_pFreeList = NULL;
                            }
                        }
                        -- m_uActiveSlotCount;
                    }
                }
            }
        }
    }

    template <class T, UINT32 uBlockSize, UINT32 uIndexBlockSize>
    VOID SlotArray<T, uBlockSize, uIndexBlockSize>::Clear()
    {
        SlotBlock* pIndex = m_pIndex;
        while ( NULL != pIndex )
        {
            SlotBlock* pNext = pIndex->indexBlock.next;
            // clear free list
            SlotBlock** pFreeItem = pIndex->indexBlock.freeList;
            while ( NULL != pFreeItem )
            {
                SlotBlock** pNext = (SlotBlock**)*pFreeItem;
                *pFreeItem = NULL;
                pFreeItem = pNext;
            }

            // clear content
            for ( UINT32 i = 0; i < uIndexBlockSize; ++i )
            {
                SlotBlock* pBlock = pIndex->indexBlock.blocks[i];
                if ( NULL != pBlock )
                {
                    BlockCache::Free( pBlock );
                }
            }
            BlockCache::Free( pIndex );
            pIndex = pNext;
        }
        m_uActiveSlotCount = 0;
        m_uTotalSlotCount = 0;
        m_pIndex = NULL;
        m_pFreeList = NULL;
    }

    template <class T, UINT32 uBlockSize, UINT32 uIndexBlockSize>
    T* SlotArray<T, uBlockSize, uIndexBlockSize>::GetSlotItem( INT32 slot )
    {
        if ( slot >= 0 )
        {
            SlotBlock* pPrevIndex = NULL;
            SlotBlock* pIndex = m_pIndex;
            UINT32 index = slot;
            while ( NULL != pIndex && index >= uIndexBlockSize * uBlockSize )
            {
                pPrevIndex = pIndex;
                pIndex = pIndex->indexBlock.next;
                index -= uIndexBlockSize * uBlockSize;
            }
            if ( NULL != pIndex )
            {
                UINT32 offset = index / uBlockSize;
                SlotBlock* pBlock = pIndex->indexBlock.blocks[offset];
                if ( NULL != pBlock )
                {
                    UINT32 n = index % uBlockSize;
                    SlotItem& item = pBlock->itemBlock.items[n];
                    if ( !( item.flag & _SLOT_CONTENT_FLAG_FREE_ ) )
                    {
                        return &item.content.value;
                    }
                }
            }
        }
        return NULL;
    }

    template <class T, UINT32 uBlockSize, UINT32 uIndexBlockSize>
    const T* SlotArray<T, uBlockSize, uIndexBlockSize>::GetSlotItem( INT32 slot ) const
    {
        if ( slot >= 0 )
        {
            SlotBlock* pPrevIndex = NULL;
            SlotBlock* pIndex = m_pIndex;
            UINT32 index = slot;
            while ( NULL != pIndex && index >= uIndexBlockSize * uBlockSize )
            {
                pPrevIndex = pIndex;
                pIndex = pIndex->indexBlock.next;
                index -= uIndexBlockSize * uBlockSize;
            }
            if ( NULL != pIndex )
            {
                UINT32 offset = index / uBlockSize;
                SlotBlock* pBlock = pIndex->indexBlock.blocks[offset];
                if ( NULL != pBlock )
                {
                    UINT32 n = index % uBlockSize;
                    SlotItem& item = pBlock->itemBlock.items[n];
                    if ( !( item.flag & _SLOT_CONTENT_FLAG_FREE_ ) )
                    {
                        return &item.content.value;
                    }
                }
            }
        }
        return NULL;
    }
}

#endif
