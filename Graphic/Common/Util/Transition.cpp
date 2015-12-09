#include "Transition.h"
#include "ClassCache.h"

namespace Util
{
    FLOAT32 Transition::Bezier( UINT32 i, UINT32 n, FLOAT32 f )
    {
        if ( n < i )
        {
            return 1.f;
        }
        FLOAT32 ret = 1.f;

        // ( 1 - f ) ^ ( n - 1 ) * f ^ i
        UINT32 temp = i;
        while ( temp )
        {
            ret *= f;
            -- temp;
        }
        temp = n - i;
        while ( temp )
        {
            ret *= ( 1.f - f );
            -- temp;
        }

        // C( i, n )
        FLOAT32 modulus = 1.f;
        temp = n - i;
        while ( n > i )
        {
            modulus *= n;
            -- n;
        }
        while ( temp > 1 )
        {
            modulus /= temp;
            -- temp;
        }
        return ret * modulus;
    }

    Transition::Transition()
        : m_transitions( NULL )
    {
    }

    Transition::~Transition()
    {
        Clear();
    }

    VOID Transition::Update( FLOAT32 interval )
    {
        if ( NULL != m_transitions )
        {
            TransitionItem* item = m_transitions;
            do
            {
                item->f = item->func( item->f, interval, item->data );
                if ( item->f >= 1.f )
                {
                    item->callback( item->data );
                    RemoveTransition( item );
                    if ( NULL == m_transitions )
                    {
                        break;
                    }
                }
                item = item->next;
            } while ( item != m_transitions );
        }
    }

    TransitionHandler Transition::AddTransition( VOID* data, TRANSITION_FUNC func, TRANSITION_CALLBACK callback )
    {
        TransitionItem* item = ItemCache::Alloc();
        if ( NULL == item )
        {
            return NULL;
        }
        item->f = 0.f;
        item->data = data;
        item->func = func;
        item->callback = callback;
        if ( NULL == m_transitions )
        {
            m_transitions = item->next = item->prev = item;
        }
        else
        {
            item->prev = m_transitions->prev;
            item->next = m_transitions;
            m_transitions->prev = item;
            item->prev->next = item;
        }
        return item;
    }

    VOID Transition::RemoveTransition( TransitionHandler handler )
    {
        TransitionItem* item = (TransitionItem*)handler;
        item->next->prev = item->prev;
        item->prev->next = item->next;
        if ( m_transitions == item )
        {
            if ( m_transitions == item->next )
            {
                m_transitions = NULL;
            }
            else
            {
                m_transitions = item->next;
            }
        }
        ItemCache::Free( item );
    }

    VOID Transition::Clear()
    {
        if ( NULL != m_transitions )
        {
            TransitionItem* item = m_transitions;
            do
            {
                TransitionItem* next = item->next;
                ItemCache::Free( item );
                item = next;
            } while ( item != m_transitions );
            m_transitions = NULL;
        }
    }
}
