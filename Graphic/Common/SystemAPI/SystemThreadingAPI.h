#if !defined ( _SYSTEM_THREADING_API_H_ )
#define _SYSTEM_THREADING_API_H_

#if defined ( LINUX ) || defined ( ANDROID )
#include <pthread.h>
#define THREAD_HANDLE       INT32
#define LOCK_HANDLE         pthread_mutex_t*
#define EVENT_HANDLE        EventStructure*

#elif defined ( MAC_OS )
#include <pthread.h>
#define THREAD_HANDLE       pthread_t
#define LOCK_HANDLE         pthread_mutex_t*
#define EVENT_HANDLE        EventStructure*

#elif defined ( WIN32 )
#define THREAD_HANDLE       VOID*
#define LOCK_HANDLE         VOID*
#define EVENT_HANDLE        HANDLE
#endif

namespace SystemAPI
{

#if defined ( LINUX ) || defined ( MAC_OS ) || defined ( ANDROID )
    struct EventStructure
    {
        pthread_mutex_t mutex;
        pthread_cond_t cond;
        bool flag;
    };
#endif

	typedef UINT32 (*THREAD_FUNCTION)( VOID* userData );

    class ThreadingAPI
    {
    public:
        /////////////////////////////////////////////////////////////////////
        /// Thread
        /////////////////////////////////////////////////////////////////////
        /**
         *  Create a thread
         *
         *  @param func: The thread function
         *  @param userData: The parameter.
         *  @return: Returns the handle if new thread. Returns NULL if failed.
         */
        static THREAD_HANDLE CreateThread( THREAD_FUNCTION func, VOID* userData );

        /**
         *  Wait for the specific thread to finish.
         *  If the thread doesn't exist, return immediately.
         *
         *  @param thread: The thread to wait for.
         */
        static VOID WaitForThread( THREAD_HANDLE thread );

        /**
        *  Get current thread handler
        *
        *  @return: Return the thread handler.
        */
        static THREAD_HANDLE GetCurrentThread();


        /////////////////////////////////////////////////////////////////////
        /// Lock
        /////////////////////////////////////////////////////////////////////
        /**
         *  Create a lock
         *
         *  @return: Returns the new created lock. Retruns NULL if failed.
         */
        static LOCK_HANDLE CreateLock();

        /**
         *  Destroy the lock created by CreateLock.
         *
         *  @param lock: The lock to be destroy.
         */
        static VOID DestroyLock( LOCK_HANDLE lock );

        /**
         *  Wait infinite to lock a specific lock.
         *
         *  @param lock: The lock handle.
         */
        static VOID Lock( LOCK_HANDLE lock );

        /**
         *  Try to lock a specific lock.
         *
         *  @param lock: The lock handle.
         *  @return: Returns FALSE if failed to lock
         */
        static BOOL TryLock( LOCK_HANDLE lock );

        /**
         *  Release a lock
         *
         *  @param lock: The lock handle.
         */
        static VOID Unlock( LOCK_HANDLE lock );


        /////////////////////////////////////////////////////////////////////
        /// Event
        /////////////////////////////////////////////////////////////////////
        /**
         *  Create a event
         *  The new created event is initialized to be unset.
         *
         *  @return: Returns the new created event. Retruns NULL if failed.
         */
        static EVENT_HANDLE CreateEvent( BOOL );

        /**
         *  Destroy the event created by CreateEvent.
         *
         *  @param event: The event to be destroy.
         */
        static VOID DestroyEvent( EVENT_HANDLE event );

        /**
         *  Wait infinite for a specific event.
         *
         *  @param event: The event handle.
         */
        static VOID WaitEvent( EVENT_HANDLE event );

        /**
         *  Trigger an event
         *  When trigger an event, only one 'WaitEvent' will return successfully.
         *
         *  @param event: The event handle.
         */
        static VOID SetEvent( EVENT_HANDLE event );

        /**
         *  Sleep for some time
         *
         *  @param event: The event handle.
         */
        static VOID Sleep( UINT32 microSecond);

        /**
         *  Call thread to relinquish the CPU
         *
         */
        static VOID ThreadYield();
    };
}


#endif
