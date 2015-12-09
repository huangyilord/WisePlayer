#include "GraphicAnimation.h"
#include "GraphicMath.h"

#include "Util/ClassCache.h"

#include <algorithm>

namespace Graphic
{
    GraphicAnimationController::GraphicAnimationController()
        : m_currentTimeLine( 0 )
        , m_lastAnimationTime( 0 )
        , m_speed( 1.f )
        , m_bPause( FALSE )
        , m_bLoop( TRUE )
    {
        
    }

    GraphicAnimationController::~GraphicAnimationController()
    {
        
    }

    VOID GraphicAnimationController::Start()
    {
        SetTimeLine( 0 );
    }

    BOOL GraphicAnimationController::Update( UINT32 interval )
    {
        if ( !m_bPause )
        {
            interval = (UINT32)((FLOAT32)interval * m_speed);
            UINT32 newTimeLine = m_currentTimeLine + interval;
            // update live animations
            std::list<IGraphicAnimation*>::iterator iterLive = m_liveAnimations.begin();
            while ( iterLive != m_liveAnimations.end() )
            {
                IGraphicAnimation* pAnimation = *iterLive;
                if ( !pAnimation->Update( interval ) )
                {
                    m_liveAnimations.erase( iterLive ++ );
                }
                else
                {
                    iterLive ++;
                }
            }

            // start new animations
            std::multimap<UINT32, IGraphicAnimation*>::iterator from = m_timeLine.upper_bound( m_currentTimeLine );
            std::multimap<UINT32, IGraphicAnimation*>::iterator to = m_timeLine.upper_bound( newTimeLine );
            while ( from != to )
            {
                IGraphicAnimation* pAnimation = from->second;
                pAnimation->Start();
                if ( pAnimation->Update( newTimeLine - from->first ) )
                {
                    m_liveAnimations.push_back( pAnimation );
                }
                from ++;
            }
            m_currentTimeLine = newTimeLine;
        }
        BOOL bContinue = !m_liveAnimations.empty() || m_currentTimeLine < m_lastAnimationTime;
        if ( !bContinue && m_bLoop )
        {
            SetTimeLine( 0 );
            bContinue = TRUE;
        }
        return bContinue;
    }

    VOID GraphicAnimationController::Pause()
    {
        m_bPause = TRUE;
    }

    VOID GraphicAnimationController::Resume()
    {
        m_bPause = FALSE;
    }

    VOID GraphicAnimationController::SetTimeLine( UINT32 timeLine )
    {
        m_currentTimeLine = timeLine;
        m_liveAnimations.clear();
        std::multimap<UINT32, IGraphicAnimation*>::iterator from = m_timeLine.begin();
        std::multimap<UINT32, IGraphicAnimation*>::iterator to = m_timeLine.upper_bound( m_currentTimeLine );
        while ( from != to )
        {
            IGraphicAnimation* pAnimation = from->second;
            pAnimation->Start();
            if ( pAnimation->Update( m_currentTimeLine ) )
            {
                m_liveAnimations.push_back( pAnimation );
            }
            from ++;
        }
    }

    VOID GraphicAnimationController::AddAnimation( UINT32 timeStamp, IGraphicAnimation* pAnimation )
    {
        m_timeLine.insert( std::make_pair(timeStamp, pAnimation) );
        if (timeStamp > m_lastAnimationTime)
        {
            m_lastAnimationTime = timeStamp;
        }
    }

    VOID GraphicAnimationController::ClearAnimations()
    {
        m_timeLine.clear();
        m_lastAnimationTime = 0;
    }

    Object3DTransformAnimation::Object3DTransformAnimation( Object3D* pObject, const Matrix16& from, const Matrix16& to, UINT32 duration)
        : m_pObject( pObject )
        , m_from( from )
        , m_to( to )
        , m_duration( duration )
    {
    }

    Object3DTransformAnimation::~Object3DTransformAnimation()
    {
        
    }

    VOID Object3DTransformAnimation::Start()
    {
        m_time = 0;
    }

    BOOL Object3DTransformAnimation::Update( UINT32 interval )
    {
        m_time += interval;
        if (m_time >= m_duration)
        {
            m_current = m_to;
            m_pObject->LoadTransform( m_current );
            return FALSE;
        }
        FLOAT32 f = (FLOAT32)m_time / (FLOAT32)m_duration;
        MatrixInterpolate( &m_current, &m_from, &m_to, f );
        m_pObject->LoadTransform( m_current );
        return TRUE;
    }

    Object3DKeyFrameAnimation::Object3DKeyFrameAnimation( Object3D* pObject )
        : m_pObject( pObject )
        , m_nextFrame( 0 )
        , m_time( 0 )
    {
    }

    Object3DKeyFrameAnimation::~Object3DKeyFrameAnimation()
    {
        
    }

    VOID Object3DKeyFrameAnimation::AddKeyFrame( const Matrix16& transform, UINT32 time )
    {
        Frame* pFrame = Util::ClassCache<Frame>::Alloc();
        if ( NULL != pFrame )
        {
            pFrame->transform = transform;
            pFrame->time = time;
            m_keyFrames.push_back( pFrame );
        }
    }

    VOID Object3DKeyFrameAnimation::DeleteFrame( Frame* pFrame )
    {
        Util::ClassCache<Frame>::Free( pFrame );
    }

    VOID Object3DKeyFrameAnimation::ClearKeyFrames()
    {
        std::for_each( m_keyFrames.begin(), m_keyFrames.end(), DeleteFrame );
        m_keyFrames.clear();
    }

    VOID Object3DKeyFrameAnimation::Start()
    {
        m_time = 0;
        m_nextFrame = 0;
        m_fromFrame.time = 0;
        if ( !m_keyFrames.empty() )
        {
            m_fromFrame = *(m_keyFrames[0]);
        }
    }

    BOOL Object3DKeyFrameAnimation::Update( UINT32 interval )
    {
        m_time += interval;
        while ( m_nextFrame < m_keyFrames.size() )
        {
            const Frame& frame = *m_keyFrames.at( m_nextFrame );
            if ( m_time < frame.time )
            {
                FLOAT32 f = 1.f;
                if ( frame.time > m_fromFrame.time )
                {
                    f = (FLOAT32)(m_time - m_fromFrame.time) / (FLOAT32)(frame.time - m_fromFrame.time);
                }
                Matrix16 transform;
                MatrixInterpolate( &transform, &m_fromFrame.transform, &frame.transform, f );
                m_pObject->LoadTransform( transform );
                return TRUE;
            }
            ++m_nextFrame;
            m_fromFrame = frame;
        }
        m_pObject->LoadTransform( m_fromFrame.transform );
        return FALSE;
    }

    Skeleton::Skeleton()
        : m_pCurrentAnimation( NULL )
    {
    }

    Skeleton::~Skeleton()
    {
        Clear();
    }

    BOOL Skeleton::Update( UINT32 interval )
    {
        BOOL ret = TRUE;
        if ( NULL != m_pCurrentAnimation )
        {
            ret = m_pCurrentAnimation->Update( interval );
        }
        UpdateTransform();
        return ret;
    }

    VOID Skeleton::UpdateTransform()
    {
        Matrix16 absoluteTransform;
        std::map<std::string, Bone*>::iterator iter = m_bones.begin();
        while ( iter != m_bones.end() )
        {
            Bone* pBone = iter->second;
            pBone->GetAbsoluteTransform( absoluteTransform );
            pBone->m_boneObject.LoadTransform( absoluteTransform );
            iter ++;
        }
    }

    BOOL Skeleton::CreateBone( const CHAR* key, const Matrix16& initTransform )
    {
        if ( m_bones.find( key ) != m_bones.end() )
        {
            return FALSE;
        }
        Bone* pBone = new(std::nothrow) Bone( initTransform );
        if ( NULL == pBone )
        {
            return FALSE;
        }
        m_bones.insert( std::make_pair(key, pBone) );
        pBone->m_boneObject.LoadTransform( initTransform );
        return TRUE;
    }

    VOID Skeleton::Clear()
    {
        std::map<std::string, Bone*>::iterator iterBone = m_bones.begin();
        while ( iterBone != m_bones.end() )
        {
            delete iterBone->second;
            iterBone ++;
        }
        m_bones.clear();
        ClearAnimations();
    }

    BOOL Skeleton::AttachObjectToBone( Object3D* pObject, const CHAR* key )
    {
        std::map<std::string, Bone*>::iterator iterBone = m_bones.find( key );
        if ( iterBone == m_bones.end() )
        {
            return FALSE;
        }
        Bone* pBone = iterBone->second;
        pObject->SetParent( &pBone->m_boneObject );
        return TRUE;
    }

    BOOL Skeleton::AttachBoneToBone( const CHAR* childKey, const CHAR* parentKey )
    {
        std::map<std::string, Bone*>::iterator iterBone = m_bones.find( childKey );
        if ( iterBone == m_bones.end() )
        {
            return FALSE;
        }
        Bone* pChildBone = iterBone->second;
        iterBone = m_bones.find( parentKey );
        if ( iterBone == m_bones.end() )
        {
            return FALSE;
        }
        Bone* pParentBone = iterBone->second;
        pChildBone->SetParent( pParentBone );
        return TRUE;
    }

    BOOL Skeleton::AttachBoneToObject( Object3D* pObject, const CHAR* key )
    {
        std::map<std::string, Bone*>::iterator iterBone = m_bones.find( key );
        if ( iterBone == m_bones.end() )
        {
            return FALSE;
        }
        Bone* pBone = iterBone->second;
        pBone->SetParent( pObject );
        return TRUE;
    }

    BOOL Skeleton::AddActionToAnimation( const CHAR* animationkey, const CHAR* boneKey, Object3DKeyFrameAnimation* pAction, UINT32 startTime )
    {
        std::map<std::string, GraphicAnimationController*>::iterator iterAnimation = m_animations.find( animationkey );
        if ( iterAnimation == m_animations.end() )
        {
            return FALSE;
        }
        GraphicAnimationController* pController = iterAnimation->second;
        std::map<std::string, Bone*>::iterator iterBone = m_bones.find( boneKey );
        if ( iterBone == m_bones.end() )
        {
            return FALSE;
        }
        Bone* pBone = iterBone->second;
        pAction->SetObject( pBone );
        pController->AddAnimation( startTime, pAction );
        std::list<Object3DKeyFrameAnimation*>& actionList = m_actions[animationkey];
        actionList.push_back( pAction );
        return TRUE;
    }

    BOOL Skeleton::CreateAnimation( const CHAR* key )
    {
        std::map<std::string, GraphicAnimationController*>::iterator iterAnimation = m_animations.find( key );
        if ( iterAnimation != m_animations.end() )
        {
            return TRUE;
        }
        GraphicAnimationController* pController = new(std::nothrow) GraphicAnimationController();
        if ( NULL == pController )
        {
            return FALSE;
        }
        m_animations.insert( std::make_pair(key, pController) );
        return TRUE;
    }

    static VOID DeleteAnimation( Object3DKeyFrameAnimation* pAnimation )
    {
        delete pAnimation;
    }

    VOID Skeleton::DestroyAnimation( const CHAR* key )
    {
        std::map<std::string, GraphicAnimationController*>::iterator iterAnimation = m_animations.find( key );
        if ( iterAnimation == m_animations.end() )
        {
            return;
        }
        GraphicAnimationController* pController = iterAnimation->second;
        delete pController;
        m_animations.erase( iterAnimation );
        std::list<Object3DKeyFrameAnimation*>& actionList = m_actions[key];
        std::for_each( actionList.begin(), actionList.end(), DeleteAnimation );
        m_actions.erase( key );
    }

    VOID Skeleton::ClearAnimations()
    {
        std::map<std::string, GraphicAnimationController*>::iterator iterAnimation = m_animations.begin();
        while ( iterAnimation != m_animations.end() )
        {
            GraphicAnimationController* pController = iterAnimation->second;
            delete pController;
            iterAnimation++;
        }
        m_animations.clear();
        std::map< std::string, std::list<Object3DKeyFrameAnimation*> >::iterator iterAction = m_actions.begin();
        while ( iterAction != m_actions.end() )
        {
            std::list<Object3DKeyFrameAnimation*>& actionList = iterAction->second;
            std::for_each( actionList.begin(), actionList.end(), DeleteAnimation );
            iterAction ++;
        }
        m_actions.clear();
    }

    VOID Skeleton::PlayAnimation( const CHAR* key )
    {
        std::map<std::string, Bone*>::iterator iter = m_bones.begin();
        while ( iter != m_bones.end() )
        {
            Bone* pBone = iter->second;
            pBone->Reset();
            iter ++;
        }
        std::map<std::string, GraphicAnimationController*>::iterator iterAnimation = m_animations.find( key );
        if ( iterAnimation != m_animations.end() )
        {
            m_pCurrentAnimation = iterAnimation->second;
            m_pCurrentAnimation->Start();
        }
    }

    VOID Skeleton::StopAnimation()
    {
        m_pCurrentAnimation = NULL;
        std::map<std::string, Bone*>::iterator iter = m_bones.begin();
        while ( iter != m_bones.end() )
        {
            Bone* pBone = iter->second;
            pBone->Reset();
            Matrix16 absoluteTransform;
            pBone->GetAbsoluteTransform( absoluteTransform );
            pBone->m_boneObject.LoadTransform( absoluteTransform );
            iter ++;
        }
    }
}
