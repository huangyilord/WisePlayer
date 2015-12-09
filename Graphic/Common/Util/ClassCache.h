#if !defined ( _CLASS_CACHE_H_ )
#define _CLASS_CACHE_H_

#if defined(ANDROID) || defined(LINUX) || defined(MAC_OS)
#include <new>
#include <stdlib.h>
#else
#include <new.h>
#endif

namespace Util
{
	template <class T, UINT32 uDefaultSize = 32>
	class ClassCache
	{
	private:
		ClassCache();
		ClassCache( const ClassCache& );
		~ClassCache();

		static ClassCache s_Instance;

		UINT32          m_uMaxSize;
        T*              m_pList;
        UINT32          m_uCount;
        UINT32          m_uBlockSize;
	public:
		inline static T* Alloc() { return s_Instance._Alloc(); }
		inline static VOID Free( T* p ) { s_Instance._Free( p ); }
		inline static VOID SetMaxSize( UINT32 uSize ) { s_Instance._SetMaxSize( uSize ); }
        inline static VOID Clear() { s_Instance._Clear(); }
	private:
		inline VOID _SetMaxSize( UINT32 uSize ) { m_uMaxSize = uSize; }
		T* _Alloc();
		VOID _Free( T* pNode );
        VOID _Clear();
	};

	template <class T, UINT32 uDefaultSize>
	ClassCache<T,uDefaultSize> ClassCache<T,uDefaultSize>::s_Instance;

	template <class T, UINT32 uDefaultSize>
	ClassCache<T,uDefaultSize>::ClassCache()
		: m_uMaxSize( uDefaultSize )
        , m_pList( NULL )
        , m_uCount( 0 )
        , m_uBlockSize( sizeof(T) > sizeof(T*) ? sizeof(T) : sizeof(T*) )
	{
	}
	template <class T, UINT32 uDefaultSize>
	ClassCache<T,uDefaultSize>::~ClassCache()
	{
        _Clear();
	}
	template <class T, UINT32 uDefaultSize>
	T* ClassCache<T,uDefaultSize>::_Alloc()
	{
        if ( NULL != m_pList )
		{
            T* ret = m_pList;
            m_pList = *(T**)m_pList;
            ::new(ret) T;
            --m_uCount;
			return ret;
		}
        T* ret = (T*)malloc( m_uBlockSize );
        if ( ret )
        {
            ::new(ret) T;
        }
		return ret;
	}

	template <class T, UINT32 uDefaultSize>
	VOID ClassCache<T,uDefaultSize>::_Free( T* p )
	{
		if ( NULL != p )
		{
            p->~T();
            *(T**)p = m_pList;
            m_pList = p;
            ++ m_uCount;
            if ( m_uCount > m_uMaxSize )
            {
                UINT32 halfSize = m_uMaxSize >> 1;
                T* pBlock = m_pList;
                while ( m_uCount > halfSize )
                {
                    T* pNext = *(T**)pBlock;
                    free( pBlock );
                    pBlock = pNext;
                    -- m_uCount;
                }
                m_pList = pBlock;
            }
		}
	}

    template <class T, UINT32 uDefaultSize>
    VOID ClassCache<T,uDefaultSize>::_Clear()
    {
        T* pBlock = m_pList;
        while ( NULL != pBlock )
		{
            T* pNext = *(T**)pBlock;
            free( pBlock );
            pBlock = pNext;
		}
        m_uCount = 0;
    }
}

#endif
