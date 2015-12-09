#include "SystemHttpAPI.h"


#define MAXHEADERSIZE 1024
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace SystemAPI
{

    //SystemHttpAPI::SystemHttpAPI()
    //{
    //    m_phostent=NULL;
    //    m_port = 80;

    //    m_bConnected=FALSE;

    //    for(int i=0;i<256;i++)
    //        m_ipaddr[i]='\0';
    //    memset(m_requestheader,0,MAXHEADERSIZE);
    //    memset(m_ResponseHeader,0,MAXHEADERSIZE);

    //    m_nCurIndex = 0;
    //    m_bResponsed = FALSE;
    //    m_nResponseHeaderSize = -1;
    //    /*
    //    m_nBufferSize = nBufferSize;
    //    m_pBuffer = new char[nBufferSize];
    //    memset(m_pBuffer,0,nBufferSize);*/
    //}

    //SystemHttpAPI::~SystemHttpAPI()
    //{
    //    hy_socket_close(m_socketHandle);
    //}

    //BOOL SystemHttpAPI::Socket()
    //{
    //    if(m_bConnected)return FALSE;

    //    m_socketHandle = hy_tcp_socket();
    //    if(m_socketHandle == INVALID_SOCKET)
    //    {
    //        return FALSE;
    //    }

    //    return TRUE;
    //}

    //BOOL SystemHttpAPI::Connect(BYTE ip[4], UINT16 port)
    //{
    //    if(szHostName == NULL)
    //        return FALSE;

    //    HY_SOCKADDR socketAddr;
    //    ip_to_hy_addr( &socketAddr, ip, port );
  
    //    if(hy_socket_connect(m_socketHandle,HY_SOCKADDR&destaddr,sizeof(destaddr))!=0)
    //    {
    //        return FALSE;
    //    }

    //    m_bConnected=TRUE;
    //    return TRUE;
    //}

    //const char *SystemHttpAPI::FormatRequestHeader(char *pServer,char *pObject, long &Length,
    //    char *pCookie,char *pReferer,long nFrom,
    //    long nTo,int nServerType)
    //{
    //    char szTemp[20];
    //    memset(m_requestheader,'\0',1024);

    //    /// line1: method, path, version
    //    strcat(m_requestheader,"GET ");
    //    strcat(m_requestheader,pObject);
    //    strcat(m_requestheader," HTTP/1.1");
    //    strcat(m_requestheader,"\r\n");

    //    /// line2: host
    //    strcat(m_requestheader,"Host:");
    //    strcat(m_requestheader,pServer);
    //    strcat(m_requestheader,"\r\n");

    //    /// line3: referer
    //    if(pReferer != NULL)
    //    {
    //        strcat(m_requestheader,"Referer:");
    //        strcat(m_requestheader,pReferer);
    //        strcat(m_requestheader,"\r\n");		
    //    }

    //    /// line4: receive data type
    //    strcat(m_requestheader,"Accept:*/*");
    //    strcat(m_requestheader,"\r\n");

    //    /// line5: Browser type
    //    strcat(m_requestheader,"User-Agent:Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)");
    //    strcat(m_requestheader,"\r\n");

    //    /// line6: Connect setting, keep-alive
    //    strcat(m_requestheader,"Connection:Keep-Alive");
    //    strcat(m_requestheader,"\r\n");

    //    /// line7: Cookie
    //    if(pCookie != NULL)
    //    {
    //        strcat(m_requestheader,"Set Cookie:0");
    //        strcat(m_requestheader,pCookie);
    //        strcat(m_requestheader,"\r\n");
    //    }

    //    /// line8: the start/end position of request data
    //    if(nFrom > 0)
    //    {
    //        strcat(m_requestheader,"Range: bytes=");
    //        _ltoa(nFrom,szTemp,10);
    //        strcat(m_requestheader,szTemp);
    //        strcat(m_requestheader,"-");
    //        if(nTo > nFrom)
    //        {
    //            _ltoa(nTo,szTemp,10);
    //            strcat(m_requestheader,szTemp);
    //        }
    //        strcat(m_requestheader,"\r\n");
    //    }

    //    //blank line
    //    strcat(m_requestheader,"\r\n");

    //    Length=strlen(m_requestheader);
    //    return m_requestheader;
    //}

    //BOOL SystemHttpAPI::SendRequest(const char *pRequestHeader, long Length)
    //{
    //    if(!m_bConnected)return FALSE;

    //    if(pRequestHeader==NULL)
    //        pRequestHeader=m_requestheader;
    //    if(Length==0)
    //        Length=strlen(m_requestheader);

    //    if(send(m_s,pRequestHeader,Length,0)==SOCKET_ERROR)
    //    {
    //        return FALSE;
    //    }
    //    int nLength;
    //    GetResponseHeader(nLength);
    //    return TRUE;
    //}

    //long SystemHttpAPI::Receive(char* pBuffer,long nMaxLength)
    //{
    //    if(!m_bConnected)return NULL;

    //    long nLength;
    //    nLength=recv(m_s,pBuffer,nMaxLength,0);

    //    if(nLength <= 0)
    //    {
    //        CloseSocket();
    //    }
    //    return nLength;
    //}

    //BOOL SystemHttpAPI::CloseSocket()
    //{
    //    if(m_s != NULL)
    //    {
    //        if(closesocket(m_s)==SOCKET_ERROR)
    //        {
    //            return FALSE;
    //        }
    //    }
    //    m_s = NULL;
    //    m_bConnected=FALSE;
    //    return TRUE;
    //}

    //int SystemHttpAPI::GetRequestHeader(char *pHeader, int nMaxLength) const
    //{
    //    int nLength;
    //    if(int(strlen(m_requestheader))>nMaxLength)
    //    {
    //        nLength=nMaxLength;
    //    }
    //    else
    //    {
    //        nLength=strlen(m_requestheader);
    //    }
    //    memcpy(pHeader,m_requestheader,nLength);
    //    return nLength;
    //}

    //BOOL SystemHttpAPI::SetTimeout(int nTime, int nType)
    //{
    //    if(nType == 0)
    //    {
    //        nType = SO_RCVTIMEO;
    //    }
    //    else
    //    {
    //        nType = SO_SNDTIMEO;
    //    }

    //    DWORD dwErr;
    //    dwErr=setsockopt(m_s,SOL_SOCKET,nType,(char*)&nTime,sizeof(nTime)); 
    //    if(dwErr)
    //    {
    //        return FALSE;
    //    }
    //    return TRUE;
    //}

    //const char* SystemHttpAPI::GetResponseHeader(int &nLength)
    //{
    //    if(!m_bResponsed)
    //    {
    //        char c = 0;
    //        int nIndex = 0;
    //        BOOL bEndResponse = FALSE;
    //        while(!bEndResponse && nIndex < MAXHEADERSIZE)
    //        {
    //            recv(m_s,&c,1,0);
    //            m_ResponseHeader[nIndex++] = c;
    //            if(nIndex >= 4)
    //            {
    //                if(m_ResponseHeader[nIndex - 4] == '\r' && m_ResponseHeader[nIndex - 3] == '\n'
    //                    && m_ResponseHeader[nIndex - 2] == '\r' && m_ResponseHeader[nIndex - 1] == '\n')
    //                    bEndResponse = TRUE;
    //            }
    //        }
    //        m_nResponseHeaderSize = nIndex;
    //        m_bResponsed = TRUE;
    //    }

    //    nLength = m_nResponseHeaderSize;
    //    return m_ResponseHeader;
    //}

    //INT32 SystemHttpAPI::GetResponseLine(char *pLine, int nMaxLength)
    //{
    //    if(m_nCurIndex >= m_nResponseHeaderSize)
    //    {
    //        m_nCurIndex = 0;
    //        return -1;
    //    }

    //    int nIndex = 0;
    //    char c = 0;
    //    do 
    //    {
    //        c = m_ResponseHeader[m_nCurIndex++];
    //        pLine[nIndex++] = c;
    //    } while(c != '\n' && m_nCurIndex < m_nResponseHeaderSize && nIndex < nMaxLength);

    //    return nIndex;
    //}

    //INT32 SystemHttpAPI::GetField(const char *szSession, char *szValue, int nMaxLength)
    //{
    //    if(!m_bResponsed) return -1;

    //    CString strRespons;
    //    strRespons = m_ResponseHeader;
    //    int nPos = -1;
    //    nPos = strRespons.Find(szSession,0);
    //    if(nPos != -1)
    //    {
    //        nPos += strlen(szSession);
    //        nPos += 2;
    //        int nCr = strRespons.Find("\r\n",nPos);
    //        CString strValue = strRespons.Mid(nPos,nCr - nPos);
    //        strcpy(szValue,strValue);
    //        return (nCr - nPos);
    //    }
    //    else
    //    {
    //        return -1;
    //    }
    //}

    //INT32 SystemHttpAPI::GetServerState()
    //{
    //    if(!m_bResponsed) return -1;

    //    char szState[4];
    //    szState[0] = m_ResponseHeader[9];
    //    szState[1] = m_ResponseHeader[10];
    //    szState[2] = m_ResponseHeader[11];
    //    szState[3] = '\0';

    //    return atoi(szState);
    //}


}