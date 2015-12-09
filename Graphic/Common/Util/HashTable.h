#if !defined ( _HASH_TABLE_H_ )
#define _HASH_TABLE_H_

#include "ClassCache.h"
#include <string.h>

namespace Util
{
#define _HASH_TABLE_ENGAGED_    0x1
#define _HASH_TABLE_HAS_NEXT_   0x2

    template<class K, class V, UINT16 indexSize = 1024, UINT16 blockSize = 256>
    class HashTable
    {
    public:
        HashTable();
        ~HashTable();

        BOOL Insert( const K& key, const V& value );
        BOOL Remove( const K& key );
        BOOL Get( const K& key, V& value ) const;
        V& Get( const K& key ) const;
        BOOL Exist( const K& key ) const;
        VOID Clear();

        inline UINT32 GetCount() const { return m_count; }
    private:
        struct _Item;
        struct _ItemBlock;
        struct _IndexBlock;
        union _Block;

        typedef struct _Item        Item;
        typedef struct _ItemBlock   ItemBlock;
        typedef struct _IndexBlock  IndexBlcok;
        typedef union _Block        Block;

        struct _Item
        {
            V           value;
            K           key;
            UINT32      flag;
        };

        struct _ItemBlock
        {
            Item        items[blockSize];
        };

        struct _IndexBlock
        {
            Block*      blocks[indexSize];
        };

        union _Block
        {
            IndexBlcok  indexBlock;
            ItemBlock   itemBlock;
        };

        typedef ClassCache<Block> BlockCache;

        inline static UINT32 hash2( UINT32 key, UINT32 size )
        {
            return 1 + ( (key >> 5) + 1 ) % ( size - 1 );
        }

        // DJB hash
        inline static UINT32 hash1( const K& key )
        {
            UINT32 hash = 5381;
            BYTE* pByte = (BYTE*)&key;
            for( UINT32 i = 0; i < sizeof(key); ++i )
            {
                hash = ((hash << 5) + hash) + pByte[i];
            }
            return hash;
        }

        static inline bool KeyEqual( const K& key1, const K& key2 )
        {
            return memcmp( &key1, &key2, sizeof(K) ) == 0;
        }

        Block*  m_pIndex;
        UINT32  m_count;
#if defined ( _HASH_TABLE_ANALYSE_ )
    public:
        UINT32 m_uAccess;
        UINT32 m_uCircuitCost;
        UINT32 m_uConflict;
#endif
    };

    template<class K, class V, UINT16 indexSize, UINT16 blockSize>
    HashTable<K, V, indexSize, blockSize>::HashTable()
        : m_pIndex( NULL )
        , m_count( 0 )
#if defined ( _HASH_TABLE_ANALYSE_ )
        , m_uAccess( 0 )
        , m_uCircuitCost( 0 )
        , m_uConflict( 0 )
#endif
    {
    }

    template<class K, class V, UINT16 indexSize, UINT16 blockSize>
    HashTable<K, V, indexSize, blockSize>::~HashTable()
    {
        Clear();
    }

    template<class K, class V, UINT16 indexSize, UINT16 blockSize>
    BOOL HashTable<K, V, indexSize, blockSize>::Insert( const K& key, const V& value )
    {
#if defined ( _HASH_TABLE_ANALYSE_ )
        ++m_uAccess;
#endif
        UINT32 capacity = indexSize * blockSize;
        if ( m_count == capacity )
        {
            return FALSE;
        }
        if ( NULL == m_pIndex )
        {
            m_pIndex = BlockCache::Alloc();
            if ( NULL == m_pIndex )
            {
                return FALSE;
            }
            memset( m_pIndex, 0, sizeof(Block) );
        }
        Item* pItem = NULL;
        UINT32 hashKey = hash1( key );
        for ( UINT32 i = 0; i < capacity; ++i )
        {
            UINT32 index = hashKey % capacity;
            UINT32 blockIndex = index / blockSize;
            Block* &pItemBlock = m_pIndex->indexBlock.blocks[blockIndex];
            if ( NULL == pItemBlock )
            {
                pItemBlock = BlockCache::Alloc();
                if ( NULL == pItemBlock )
                {
                    return FALSE;
                }
                memset( pItemBlock, 0, sizeof(Block) );
            }
            UINT32 offset = index % blockSize;
            pItem = &pItemBlock->itemBlock.items[offset];
            
            if ( pItem->flag & _HASH_TABLE_ENGAGED_ )
            {
                if ( KeyEqual( pItem->key, key ) )
                {
                    return FALSE;
                }
                pItem->flag |= _HASH_TABLE_HAS_NEXT_;
            }
            else
            {
                pItem->flag |= _HASH_TABLE_ENGAGED_;
                pItem->key = key;
                pItem->value = value;
                ++m_count;
                return TRUE;
            }
#if defined ( _HASH_TABLE_ANALYSE_ )
            ++m_uCircuitCost;
            ++m_uConflict;
#endif
            hashKey += hash2( hashKey, capacity );
        }
        return FALSE;
    }

    template<class K, class V, UINT16 indexSize, UINT16 blockSize>
    BOOL HashTable<K, V, indexSize, blockSize>::Remove( const K& key )
    {
#if defined ( _HASH_TABLE_ANALYSE_ )
        ++m_uAccess;
#endif
        UINT32 capacity = indexSize * blockSize;
        if ( NULL == m_pIndex )
        {
            return FALSE;
        }
        Item* pItem = NULL;
        UINT32 hashKey = hash1( key );
        for ( UINT32 i = 0; i < capacity; ++i )
        {
            UINT32 index = hashKey % capacity;
            UINT32 blockIndex = index / blockSize;
            Block* pItemBlock = m_pIndex->indexBlock.blocks[blockIndex];
            if ( NULL == pItemBlock )
            {
                return FALSE;
            }
            UINT32 offset = index % blockSize;
            pItem = &pItemBlock->itemBlock.items[offset];
            if ( pItem->flag & _HASH_TABLE_ENGAGED_ )
            {
                if ( KeyEqual( pItem->key, key ) )
                {
                    pItem->flag &= ~_HASH_TABLE_ENGAGED_;
                    --m_count;
                    return TRUE;
                }
            }
            if ( !(pItem->flag & _HASH_TABLE_HAS_NEXT_) )
            {
                return FALSE;
            }
#if defined ( _HASH_TABLE_ANALYSE_ )
            ++m_uCircuitCost;
#endif
            hashKey += hash2( hashKey, capacity );
        }
        return FALSE;
    }

    template<class K, class V, UINT16 indexSize, UINT16 blockSize>
    BOOL HashTable<K, V, indexSize, blockSize>::Get( const K& key, V& value ) const
    {
#if defined ( _HASH_TABLE_ANALYSE_ )
        ++m_uAccess;
#endif
        UINT32 capacity = indexSize * blockSize;
        if ( NULL == m_pIndex )
        {
            return FALSE;
        }
        Item* pItem = NULL;
        UINT32 hashKey = hash1( key );
        for ( UINT32 i = 0; i < capacity; ++i )
        {
            UINT32 index = hashKey % capacity;
            UINT32 blockIndex = index / blockSize;
            Block* pItemBlock = m_pIndex->indexBlock.blocks[blockIndex];
            if ( NULL == pItemBlock )
            {
                return FALSE;
            }
            UINT32 offset = index % blockSize;
            pItem = &pItemBlock->itemBlock.items[offset];
            if ( pItem->flag & _HASH_TABLE_ENGAGED_ )
            {
                if ( KeyEqual( pItem->key, key ) )
                {
                    value = pItem->value;
                    return TRUE;
                }
            }
            if ( !(pItem->flag & _HASH_TABLE_HAS_NEXT_) )
            {
                return FALSE;
            }
#if defined ( _HASH_TABLE_ANALYSE_ )
            ++m_uCircuitCost;
#endif
            hashKey += hash2( hashKey, capacity );
        }
        return FALSE;
    }

    template<class K, class V, UINT16 indexSize, UINT16 blockSize>
    V& HashTable<K, V, indexSize, blockSize>::Get( const K& key ) const
    {
#if defined ( _HASH_TABLE_ANALYSE_ )
        ++m_uAccess;
#endif
        Item* pItem = NULL;
        UINT32 capacity = indexSize * blockSize;
        UINT32 hashKey = hash1( key );
        for ( UINT32 i = 0; i < capacity; ++i )
        {
            UINT32 index = hashKey % capacity;
            UINT32 blockIndex = index / blockSize;
            Block* pItemBlock = m_pIndex->indexBlock.blocks[blockIndex];
            UINT32 offset = index % blockSize;
            pItem = &pItemBlock->itemBlock.items[offset];
            if ( pItem->flag & _HASH_TABLE_ENGAGED_ )
            {
                if ( KeyEqual( pItem->key, key ) )
                {
                    break;
                }
            }
            ASSERT( pItem->flag & _HASH_TABLE_HAS_NEXT_ );
#if defined ( _HASH_TABLE_ANALYSE_ )
            ++m_uCircuitCost;
#endif
            hashKey += hash2( hashKey, capacity );
        }
        ASSERT( FALSE );
        return pItem->value;
    }

    template<class K, class V, UINT16 indexSize, UINT16 blockSize>
    BOOL HashTable<K, V, indexSize, blockSize>::Exist( const K& key ) const
    {
#if defined ( _HASH_TABLE_ANALYSE_ )
        ++m_uAccess;
#endif
        UINT32 capacity = indexSize * blockSize;
        if ( NULL == m_pIndex )
        {
            return FALSE;
        }
        Item* pItem = NULL;
        UINT32 hashKey = hash1( key );
        for ( UINT32 i = 0; i < capacity; ++i )
        {
            UINT32 index = hashKey % capacity;
            UINT32 blockIndex = index / blockSize;
            Block* pItemBlock = m_pIndex->indexBlock.blocks[blockIndex];
            if ( NULL == pItemBlock )
            {
                return FALSE;
            }
            UINT32 offset = index % blockSize;
            pItem = &pItemBlock->itemBlock.items[offset];
            if ( pItem->flag & _HASH_TABLE_ENGAGED_ )
            {
                if ( KeyEqual( pItem->key, key ) )
                {
                    return TRUE;
                }
            }
            if ( !(pItem->flag & _HASH_TABLE_HAS_NEXT_) )
            {
                return FALSE;
            }
#if defined ( _HASH_TABLE_ANALYSE_ )
            ++m_uCircuitCost;
#endif
            hashKey += hash2( hashKey, capacity );
        }
        return FALSE;
    }

    template<class K, class V, UINT16 indexSize, UINT16 blockSize>
    VOID HashTable<K, V, indexSize, blockSize>::Clear()
    {
        if ( NULL != m_pIndex )
        {
            for ( UINT32 i = 0; i < indexSize; ++i )
            {
                Block* pBlock = m_pIndex->indexBlock.blocks[i];
                if ( NULL != pBlock )
                {
                    BlockCache::Free( pBlock );
                }
            }
            BlockCache::Free( m_pIndex );
            m_pIndex = NULL;
        }
        m_count = 0;
    }
}

#endif
