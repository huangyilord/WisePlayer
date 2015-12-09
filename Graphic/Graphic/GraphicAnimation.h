#if !defined ( _GRAPHIC_ANIMATION_H_ )
#define _GRAPHIC_ANIMATION_H_

#include "GraphicTypes.h"
#include <string>
#include <vector>

namespace Graphic
{
    class IGraphicAnimation
    {
    public:
        /**
         *  Start the animation
         */
        virtual VOID Start() = 0;

        /**
         *  Update the animation by interval.
         *
         *  @param interval: The milliseconds from last update.
         *  @return: Returns false if this animation has finished. No more update after returning a false value.
         */
        virtual BOOL Update( UINT32 interval ) = 0;
    };

    class GraphicAnimationController
        : public IGraphicAnimation
    {
    public:
        GraphicAnimationController();
        ~GraphicAnimationController();

        // IGraphicAnimation
        virtual VOID Start();
        virtual BOOL Update( UINT32 interval );

        VOID Pause();
        VOID Resume();
        VOID SetTimeLine( UINT32 timeLine );

        VOID AddAnimation( UINT32 timeStamp, IGraphicAnimation* pAnimation );
        VOID ClearAnimations();

        inline VOID SetSpeed( FLOAT32 speed ) { m_speed = speed; }
        inline VOID SetLoop( BOOL bLoop ) { m_bLoop = bLoop; }
    private:
        std::list<IGraphicAnimation*>                   m_liveAnimations;
        std::multimap<UINT32, IGraphicAnimation*>       m_timeLine;
        UINT32                                          m_currentTimeLine;
        UINT32                                          m_lastAnimationTime;
        FLOAT32                                         m_speed;
        BOOL                                            m_bPause;
        BOOL                                            m_bLoop;
    };

    class Object3DTransformAnimation
        : public IGraphicAnimation
    {
    public:
        Object3DTransformAnimation( Object3D* pObject, const Matrix16& from, const Matrix16& to, UINT32 duration);
        virtual ~Object3DTransformAnimation();

        // override IGraphicAnimation
        virtual VOID Start();
        virtual BOOL Update( UINT32 interval );
    private:
        Object3D*       m_pObject;
        Matrix16        m_from;
        Matrix16        m_to;
        Matrix16        m_current;
        UINT32          m_time;
        UINT32          m_duration;
    };

    class Object3DKeyFrameAnimation
        : public IGraphicAnimation
    {
    public:
        Object3DKeyFrameAnimation( Object3D* pObject );
        virtual ~Object3DKeyFrameAnimation();

        inline VOID SetObject( Object3D* pObject ) { m_pObject = pObject; }
        inline Object3D* GetObject() const { return m_pObject; }

        VOID AddKeyFrame( const Matrix16& transform, UINT32 time );
        VOID ClearKeyFrames();

        // override IGraphicAnimation
        virtual VOID Start();
        virtual BOOL Update( UINT32 interval );
    private:
        typedef struct Frame
        {
            Matrix16    transform;
            UINT32      time;
        } Frame;

        static VOID DeleteFrame( Frame* pFrame );

        Object3D*                   m_pObject;
        std::vector<Frame*>         m_keyFrames;
        Frame                       m_fromFrame;
        UINT32                      m_nextFrame;
        UINT32                      m_time;
    };

    class Skeleton
    {
    private:
        class Bone
            : public Object3D
        {
        public:
            Bone( const Matrix16& initTransform ) : m_initTransform( initTransform ) { LoadTransform( initTransform ); }
            ~Bone() {}

            VOID Reset() { LoadTransform( m_initTransform ); }

            Object3D    m_boneObject;
            Matrix16    m_initTransform;
        };
    public:
        Skeleton();
        ~Skeleton();

        BOOL Update( UINT32 interval );
        VOID UpdateTransform();

        BOOL CreateBone( const CHAR* key, const Matrix16& initTransform );
        VOID Clear();

        BOOL AttachObjectToBone( Object3D* pObject, const CHAR* key );
        BOOL AttachBoneToBone( const CHAR* childKey, const CHAR* parentKey );
        BOOL AttachBoneToObject( Object3D* pObject, const CHAR* key );

        BOOL AddActionToAnimation( const CHAR* animationkey, const CHAR* boneKey, Object3DKeyFrameAnimation* pAction, UINT32 startTime );

        BOOL CreateAnimation( const CHAR* key );
        VOID DestroyAnimation( const CHAR* key );
        VOID ClearAnimations();

        VOID PlayAnimation( const CHAR* key );
        VOID StopAnimation();
    private:
        std::map<std::string, Bone*>                                        m_bones;
        std::map< std::string, std::list<Object3DKeyFrameAnimation*> >      m_actions;
        std::map<std::string, GraphicAnimationController*>                  m_animations;
        GraphicAnimationController*                                         m_pCurrentAnimation;
    };
}

#endif /* defined(_GRAPHIC_ANIMATION_H_) */
