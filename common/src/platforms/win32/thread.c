#include "thread.h"
#include <windows.h>

boolean thread_create_thread( thread_type* thread, thread_function func, void* user_data )
{
    *thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, user_data, 0, NULL);
    if ( NULL == *thread )
    {
        return FALSE;
    }
    return TRUE;
}

void thread_wait_for_thread( thread_type* thread )
{
    WaitForSingleObject( *thread, INFINITE );
}

void thread_get_current_thread( thread_type* thread )
{
    *thread = GetCurrentThread();
}

boolean thread_equal( const thread_type* thread1, const thread_type* thread2 )
{
    return *thread1 == *thread2;
}

void thread_sleep( uint32 milliseconds )
{
    Sleep( milliseconds );
}

void thread_yield()
{
    Sleep( 0 );
}
