#if !defined ( _TRANSITION_H_ )
#define _TRANSITION_H_

#include "ClassCache.h"

namespace Util
{
    typedef VOID* TransitionHandler;
    class Transition
    {
    public:
        static FLOAT32 Bezier( UINT32 i, UINT32 n, FLOAT32 f );
    public:
        typedef FLOAT32 (*TRANSITION_FUNC)( FLOAT32 f, FLOAT32 interval, VOID* data );
        typedef VOID (*TRANSITION_CALLBACK)( VOID* data );
        
        Transition();
        ~Transition();
        
        VOID Update( FLOAT32 interval );
        
        TransitionHandler AddTransition( VOID* data, TRANSITION_FUNC func, TRANSITION_CALLBACK callback );
        VOID RemoveTransition( TransitionHandler handler );

        VOID Clear();
    private:
        typedef struct TransitionItem
        {
            VOID*                   data;
            TRANSITION_FUNC         func;
            TRANSITION_CALLBACK     callback;
            FLOAT32                 f;
            TransitionItem*         prev;
            TransitionItem*         next;
        } TransitionItem;
        typedef ClassCache<TransitionItem> ItemCache;
        
        TransitionItem*             m_transitions;
    };
}

#endif

