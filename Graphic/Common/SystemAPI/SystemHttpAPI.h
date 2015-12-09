#if !defined ( _SYSTEM_HTTP_API_H_ )
#define _SYSTEM_HTTP_API_H_

#include "Connection/hy_conn_api.h"

namespace SystemAPI
{
    class SystemHttpAPI
    {
    public:
        BOOL            GetFile(const CHAR* pURL, CHAR* fileName);

    private:
        INT32           GetServerState();
        INT32           GetField(const CHAR* szSession,char *szValue,int nMaxLength);
        INT32			GetResponseLine(CHAR *pLine,int nMaxLength);
        const CHAR*     GetResponseHeader(INT32 &Length);
        const CHAR*	    FormatRequestHeader(CHAR *pServer,CHAR *pObject,INT32 &Length,
                                            CHAR* pCookie=NULL,CHAR *pReferer=NULL,
                                            INT32 nFrom=0,INT32 nTo=0,
                                            INT32 nServerType=0);
        INT32           GetRequestHeader(CHAR *pHeader,INT32 nMaxLength) const;
        BOOL            SendRequest(const CHAR* pRequestHeader = NULL, INT32 Length = 0);

        SystemHttpAPI();
        virtual ~SystemHttpAPI();

        BOOL            SetTimeout(int nTime,int nType=0);
        INT32           Receive(char* pBuffer,long nMaxLength);
        BOOL            Connect(char* szHostName,int nPort=80);
        BOOL            Socket();
        BOOL            CloseSocket();

        CHAR                m_requestheader[1024];
        CHAR                m_ResponseHeader[1024];
        int                 m_port;
        CHAR                m_ipaddr[256];
        BOOL                m_bConnected;
        //HY_SOCKET_HANDLE    m_socketHandle;
        
        int                 m_nCurIndex;
        BOOL                m_bResponsed;
        int                 m_nResponseHeaderSize;
    };
}

#endif
