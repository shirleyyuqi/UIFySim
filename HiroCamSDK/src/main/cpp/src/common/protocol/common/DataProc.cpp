#include "DataProc.h"
namespace HiroCamSDK {
    namespace Protocol{
        
HR_S32 HRDataProc_Recv(IParse* pParse, ISock* pSock, HR_S32 channel)
{
	HR_S32 ret = 0;
	HR_S32 times = 1000;
	HR_S8* pBuf = NULL;
	HR_U32* pRcvLen = NULL;
	HR_U32 iMaxLen = 0;
	pSock->GetBufInfo(channel, &pBuf, &pRcvLen, &iMaxLen);
	while(times--){
		ret = pSock->Recv(pBuf+*pRcvLen, iMaxLen-*pRcvLen, channel);
		if (ret < 0){
			if (_SOCKET_SERVERCLOSED == ret){
				return _SOCKET_SERVERCLOSED;
			}
			return _SOCKET_ERROR;
		}else if ( ret == 0 ){
			return _SOCKET_NODATA;
		}else{
			ret = HRDataProc_onParseData(pParse, pSock, channel, pBuf, pRcvLen, iMaxLen);
			if (_SOCKET_USERQUIT == ret) 
				return _SOCKET_USERQUIT;
		}
	}
	return _SOCKET_NODATA;
}

HR_S32 HRDataProc_onParseData(IParse* pParse, ISock* pSock, HR_S32 channel, HR_S8* pBuf, HR_U32* pRcvLen, HR_U32 iMaxLen)
{
	HR_U32 parsePoint = 0;
	while(parsePoint < *pRcvLen)
	{
		if (pSock->IsQuit()){
			return _SOCKET_USERQUIT;
		}
		switch( pParse->onCheckData(pBuf + parsePoint, *pRcvLen-parsePoint) )
		{
		case DATAERROR:
			parsePoint++;
			continue;
		case DATAOK:
			if(parsePoint > 0){
				memmove(pBuf, pBuf+parsePoint, *pRcvLen-parsePoint);
				*pRcvLen -= parsePoint;
				parsePoint = 0;
			}			
			
			parsePoint += pParse->onParseMsg(pSock, pBuf + parsePoint);
			continue;
		case DATANOCOMP:
			pParse->onDataNoComp(pBuf + parsePoint, iMaxLen, pRcvLen);
			break;
		case HEADNOCOMP:
			break;
		}
		break;
	}
	if (parsePoint > 0 && *pRcvLen >= parsePoint){
		memmove(pBuf, pBuf+parsePoint, *pRcvLen-parsePoint);
		*pRcvLen -= parsePoint;
	}
	return parsePoint;
}
        
    }
}
