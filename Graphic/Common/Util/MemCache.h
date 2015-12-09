#if !defined ( _MEM_CACHE_H )
#define _MEM_CACHE_H

#if defined(ANDROID) || defined(LINUX) || defined(MAC_OS)
#include <stdlib.h>
#else
#include <new.h>
#endif

namespace Util
{
	template <UINT32 uMemSize, UINT32 uDefaultSize = 32>
	class MemCache
	{
	private:
		MemCache();
		MemCache( const MemCache& );
		~MemCache();

		static MemCache s_Instance;

		UINT32          m_uMaxSize;
        VOID*           m_pList;
        UINT32          m_uCount;
        UINT32          m_uBlockSize;
	public:
		inline static VOID* Alloc() { return s_Instance._Alloc(); }
		inline static VOID Free( VOID* p ) { s_Instance._Free( p ); }
		inline static VOID SetMaxSize( UINT32 uSize ) { s_Instance._SetMaxSize( uSize ); }

	private:
		inline VOID _SetMaxSize( UINT32 uSize ) { m_uMaxSize = uSize; }
		inline VOID* _Alloc();
		inline VOID _Free( VOID* pNode );
	};

	template <UINT32 uMemSize, UINT32 uDefaultSize>
	MemCache<uMemSize, uDefaultSize> MemCache<uMemSize, uDefaultSize>::s_Instance;

	template <UINT32 uMemSize, UINT32 uDefaultSize>
	MemCache<uMemSize, uDefaultSize>::MemCache()
		: m_uMaxSize( uDefaultSize )
        , m_pList( NULL )
        , m_uCount( 0 )
        , m_uBlockSize( uMemSize > sizeof(VOID*) ? uMemSize : sizeof(VOID*) )
	{
	}
	template <UINT32 uMemSize, UINT32 uDefaultSize>
	MemCache<uMemSize, uDefaultSize>::~MemCache()
	{
        VOID* pBlock = m_pList;
        while ( NULL != pBlock )
		{
            VOID* pNext = *(VOID**)pBlock;
            free( pBlock );
            pBlock = pNext;
		}
        m_uCount = 0;
	}
	template <UINT32 uMemSize, UINT32 uDefaultSize>
	inline void* MemCache<uMemSize, uDefaultSize>::_Alloc()
	{
        if ( NULL != m_pList )
		{
            VOID* ret = m_pList;
            m_pList = *(VOID**)m_pList;
            --m_uCount;
			return ret;
		}

        return malloc( m_uBlockSize );
	}

	template <UINT32 uMemSize, UINT32 uDefaultSize>
	inline VOID MemCache<uMemSize, uDefaultSize>::_Free( VOID* p )
	{
		if ( NULL != p )
		{
            *(VOID**)p = m_pList;
            m_pList = p;
            ++ m_uCount;
            if ( m_uCount > m_uMaxSize )
            {
                UINT32 halfSize = m_uMaxSize >> 1;
                VOID* pBlock = m_pList;
                while ( m_uCount > halfSize )
                {
                    VOID* pNext = *(VOID**)pBlock;
                    free( pBlock );
                    pBlock = pNext;
                    -- m_uCount;
                }
                m_pList = pBlock;
            }
		}
	}
}

#endif
