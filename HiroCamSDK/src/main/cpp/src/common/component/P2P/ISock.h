/*
* Copyright (c) 2014,foscam �з���ϵ
* All rights reserved.
*
* �ļ����ƣ�ISock.h
* �ļ���ʶ��
* ժ Ҫ��SOCKET������
*
* ��ǰ�汾��1.0
* �� �ߣ�luoc
* ������ڣ�2014��8��7��
*
* ȡ���汾��
* ԭ���� ��
* ������ڣ�
*/

#ifndef __HIROCAMSDK_SRC_PLATFORM_NET_ISOCK__
#define __HIROCAMSDK_SRC_PLATFORM_NET_ISOCK__
#include "BaseDataType.h"
namespace HiroCamSDK {

		typedef enum
		{
			_SOCKET_CLOSE = -0x10,
			_SOCKET_ERROR = -0x9,
			_SOCKET_NODATA = 0x0,
		}SOCKET_STATA;

		enum
		{
			_SOCKET_SUCCESS = -0x100,
			_SOCKET_IPERROR,
			_SOCKET_TIMEOUT,
			_SOCKET_SERVERCLOSED,
			_SOCKET_USERQUIT,
			_SOCKET_TIMEERR,
            _SOCKET_REINIT,
			_SOCKET_CONECTRROR,
		};

class ISock {
public:
    ISock(){};
	virtual ~ISock(){};
	virtual HR_S32 Init() = 0;
	virtual HR_S32 CreateSocket() = 0;
	virtual HR_S32 CloseSocket() = 0;
	virtual HR_S32 Connect(HR_S8* ip, HR_U16 port, HR_S8* usrdata, HR_U32 timeout=3000) = 0;
	virtual HR_S32 Recv(HR_S8* buf, HR_S32 n, HR_U32 channel) = 0;
	virtual HR_S32 Send(HR_S8* buf, HR_S32 n, HR_U32 channel, HR_S32 timeout) = 0;
	virtual HR_S32 UserQuit() = 0;
    virtual HR_BOOL IsQuit() = 0;
    virtual HR_VOID ResetQuit() = 0;
    virtual HR_S32 GetBufInfo(HR_U32 channel, HR_S8** pBuf, HR_U32** ppRecv, HR_U32* pMaxLen) = 0;
};

}


#endif /* __FOSSDK_SRC_COMMON_COMPONENT_PROTOCOL_COMMON_INTERFACE_ISOCK__ */
