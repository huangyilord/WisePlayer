#include "SystemThreadingAPI.h"

#if defined ( LINUX ) || defined ( MAC_OS ) || defined ( ANDROID )
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#elif defined( WIN32 )
#include <process.h>
#include <windows.h>
#endif

#include "Util/Output.h"
namespace SystemAPI
{

#if defined ( LINUX ) || defined ( MAC_OS ) || defined ( ANDROID )

	typedef void* (*PTHREAD_FUNCTION)(void*);

    THREAD_HANDLE ThreadingAPI::CreateThread( THREAD_FUNCTION func, VOID* userData )
    {
        pthread_t thread;
        if (0 != pthread_create(&thread, NULL, (PTHREAD_FUNCTION)func, userData))
        {
            return 0;
        }
        return thread;
    }

    VOID ThreadingAPI::WaitForThread( THREAD_HANDLE thread )
    {
        pthread_join(thread, NULL);
    }

    THREAD_HANDLE ThreadingAPI::GetCurrentThread()
    {
        return pthread_self();
    }

    LOCK_HANDLE ThreadingAPI::CreateLock()
    {
        pthread_mutex_t* pMutex = new pthread_mutex_t;
        if (pthread_mutex_init(pMutex, NULL)==0)
        {
            return pMutex;
        }
        else
        {
            Util::Output::Write( Util::Output::EChannel_StdErr, "Fatal error: CreateLock fail\r\n");
            abort();
        }
    }

    VOID ThreadingAPI::DestroyLock( LOCK_HANDLE lock )
    {
        pthread_mutex_destroy(lock);
        delete lock;
    }

    VOID ThreadingAPI::Lock( LOCK_HANDLE lock )
    {
        if (pthread_mutex_lock(lock) != 0)
        {
            Util::Output::Write( Util::Output::EChannel_StdErr, "Fatal error: Lock fail\r\n");
            abort();
        }
    }

    BOOL ThreadingAPI::TryLock( LOCK_HANDLE lock )
    {
        return pthread_mutex_trylock(lock) == EBUSY ? FALSE : TRUE;
    }

    VOID ThreadingAPI::Unlock( LOCK_HANDLE lock )
    {
        if (pthread_mutex_unlock(lock) != 0)
        {
            Util::Output::Write( Util::Output::EChannel_StdErr, "Fatal error: Unlock fail\r\n");
            abort();
        }
    }

    EVENT_HANDLE ThreadingAPI::CreateEvent( BOOL initialState )
    {
        EventStructure* pEvent = (EventStructure*)malloc(sizeof(EventStructure));
        if (pEvent == NULL)
        {
            Util::Output::Write( Util::Output::EChannel_StdErr, "Fatal error: Unlock fail\r\n");
            abort();
        }

        memset(pEvent, 0, sizeof(EventStructure));

        if (pthread_mutex_init(&(pEvent->mutex), NULL) != 0)
        {
            Util::Output::Write( Util::Output::EChannel_StdErr, "Fatal error: pthread_mutex_init fail\r\n");
            abort();
        }

        if (pthread_cond_init(&(pEvent->cond), NULL) != 0)
        {
            Util::Output::Write( Util::Output::EChannel_StdErr, "Fatal error: pthread_cond_init fail\r\n");
            abort();
        }
        pEvent->flag = initialState;

        return pEvent;
    }

    VOID ThreadingAPI::DestroyEvent( EVENT_HANDLE event )
    {
        if (event == NULL)
        {
            return;
        }

        if (pthread_mutex_destroy(&(event->mutex)) != 0)
        {
            Util::Output::Write( Util::Output::EChannel_StdErr, "Fatal error: pthread_mutex_destroy fail\r\n");
            abort();
        }

        if (pthread_cond_destroy(&(event->cond)) != 0)
        {
            Util::Output::Write( Util::Output::EChannel_StdErr, "Fatal error: pthread_cond_destroy fail\r\n");
            abort();
        }

        free(event);
    }

    VOID ThreadingAPI::WaitEvent( EVENT_HANDLE event )
    {
        if (event == NULL)
        {
            return;
        }

        if (pthread_mutex_lock(&(event->mutex)) != 0)
        {
            Util::Output::Write( Util::Output::EChannel_StdErr, "Fatal error: pthread_mutex_lock fail\r\n");
            abort();
        }

        if (!event->flag)
        {
            if (pthread_cond_wait(&(event->cond), &(event->mutex)) != 0)
            {
                Util::Output::Write( Util::Output::EChannel_StdErr, "Fatal error: pthread_cond_wait fail\r\n");
                abort();
            }
        }

        event->flag = false;
        if (pthread_mutex_unlock(&(event->mutex)) != 0)
        {
            Util::Output::Write( Util::Output::EChannel_StdErr, "Fatal error: pthread_mutex_unlock fail\r\n");
            abort();
        }
    }

    VOID ThreadingAPI::SetEvent( EVENT_HANDLE event )
    {
        if (event == NULL)
        {
            return;
        }

        if (pthread_mutex_lock(&(event->mutex)) != 0)
        {
            Util::Output::Write( Util::Output::EChannel_StdErr, "Fatal error: pthread_mutex_lock fail\r\n");
            abort();
        }
        event->flag = true;
        if (pthread_cond_broadcast(&(event->cond)) != 0)
        {
            Util::Output::Write( Util::Output::EChannel_StdErr, "Fatal error: pthread_cond_broadcast fail\r\n");
            abort();
        }
        if (pthread_mutex_unlock(&(event->mutex)) != 0)
        {
            Util::Output::Write( Util::Output::EChannel_StdErr, "Fatal error: pthread_mutex_unlock fail\r\n");
            abort();
        }
    }

    VOID ThreadingAPI::Sleep( UINT32 microSecond )
    {
#if defined (MAC_OS)
        Sleep(microSecond);
#else
        sleep((UINT32)((FLOAT32)microSecond / 1000.0f));
#endif
    }

    VOID ThreadingAPI::ThreadYield()
    {
#if defined (MAC_OS)
        pthread_yield_np();
#else
        if (pthread_yield() != 0)
        {
            Util::Output::Write( Util::Output::EChannel_StdErr, "Fatal error: pthread_yield fail\r\n");
            abort();
        }
#endif
    }


#elif defined(WIN32)

    typedef unsigned int (__stdcall * PTHREAD_FUNCTION)(void *);

    THREAD_HANDLE ThreadingAPI::CreateThread( THREAD_FUNCTION func, VOID* userData )
    {
        return (HANDLE)_beginthreadex(NULL, 0, (PTHREAD_FUNCTION)func, userData, 0, NULL);
    }

    VOID ThreadingAPI::WaitForThread( THREAD_HANDLE thread )
    {
        WaitForSingleObject( thread, INFINITE );
    }

    LOCK_HANDLE ThreadingAPI::CreateLock()
    {
        return CreateMutex(NULL, FALSE, NULL);
    }

    VOID ThreadingAPI::DestroyLock( LOCK_HANDLE lock )
    {
        CloseHandle(lock);
    }

    VOID ThreadingAPI::Lock( LOCK_HANDLE lock )
    {
        WaitForSingleObject(lock, INFINITE);
    }

    BOOL ThreadingAPI::TryLock( LOCK_HANDLE lock )
    {
        return WaitForSingleObject(lock, 0) == WAIT_OBJECT_0;
    }

    VOID ThreadingAPI::Unlock( LOCK_HANDLE lock )
    {
        ReleaseMutex(lock);
    }

    EVENT_HANDLE ThreadingAPI::CreateEvent( BOOL initialState)
    {
        return ::CreateEvent(NULL, FALSE, initialState, NULL);
    }

    VOID ThreadingAPI::DestroyEvent( EVENT_HANDLE event )
    {
        CloseHandle(event);
    }

    VOID ThreadingAPI::WaitEvent( EVENT_HANDLE event )
    {
        WaitForSingleObject(event, INFINITE);
    }

    VOID ThreadingAPI::SetEvent( EVENT_HANDLE event )
    {
        if (event)
        {
            ::SetEvent(event);
        }
    }
	
    VOID ThreadingAPI::Sleep( UINT32 microSecond )
    {
        ::Sleep(microSecond);
    }

    VOID ThreadingAPI::ThreadYield()
    {
        ::Sleep(0);        
    }

#endif


}
