#ifndef _WIPN_API_H_
#define _WIPN_API_H_

#include <stdio.h>
#include <time.h>
#include <errno.h>

#if defined WINDOWS 
#include <windows.h>
#elif defined LINUX
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#endif // #if defined WINDOWS

#include "NDT_Type.h"
#include "NDT_API.h"
#include "NDT_Error.h"
#include "WiPN_Error.h"
#include "WiPN_StringEncDec.h"

#ifdef WINDOWS
#define SNPRINTF	_snprintf
#else
#define SNPRINTF	snprintf
#endif

// Whether to subscribe, cancel the subscription of API code to participate in the compilation
#define SUBSCRIBE_FLAG		1
// Whether will push API code in compilation
#define POST_FLAG			1

#ifdef __cplusplus
extern "C" {
#endif 

//// for Device && Client		
#define SIZE_DID                	24
#define SIZE_InitString         	200
#define SIZE_AES128Key          	20
//// for Device
#define SIZE_NDTLicense         	12
#define SIZE_iPNLicense         	12
//// for WiPN API Device&&Client 
#define SIZE_UTCTString         	16
#define SIZE_RETString				32
//// for WiPN API Device Post
#define SIZE_PostServerString   	600
#define SIZE_Title					32
#define SIZE_Content            	128
#define SIZE_Payload            	128
#define SIZE_SoundName          	64
//// for WiPN API Client Subscribe
#define SIZE_SubscribeServerString 	600
#define SIZE_DeviceToken            200		

//// Todo: Modify this for your own QueryServer Number
//// Each customer has its own fixed QueryServer DID and Number (Specified in the deployment WiPN server)
//#define QUERY_SERVER_NUMBER  		2
////// Todo: Modify this for your own QueryServer DID
//#define QUERY_SERVER_DID_Test_1 	"PPCS-014143-SBKHR"
//#define QUERY_SERVER_DID_Test_2 	"PPCS-014144-RVDKK"
    
#define QUERY_SERVER_NUMBER 6
//    //// Todo:Modify this for your own QueryServer DID
//#define QUERY_SERVER_DID_Test_1 "LBCS-000003-CJRNC"
//#define QUERY_SERVER_DID_Test_2 "LBCS-000004-TTERH"
//#define QUERY_SERVER_DID_Test_3 "LBCS-000005-PTEZZ"
//#define QUERY_SERVER_DID_Test_4 "LBCS-000006-RDSVJ"
//#define QUERY_SERVER_DID_Test_5 "LBCS-000007-BWDDK"
//#define QUERY_SERVER_DID_Test_6 "LBCS-000008-PWBDD"
    
//// The WiPN key to encrypt/decrypt data required
//#define	STRING_ENC_DEC_KEY			"WiPN@CS2-Network"
#define	STRING_ENC_DEC_KEY			"DuFmNrxLRk973wxP"


//// Time delay unit: ms
void mSecSleep(unsigned int ms);
//// get NDT/WiPN Error Code Info
const char *getErrorCodeInfo(int err);
//// Count Time of use
unsigned int MyGetTickCount();

//// -------------------------------- WiPN API --------------------------------
////  WiPN_Query: Device&&Client
INT32 WiPN_Query(
                 char *encDecKey,
                 const CHAR *pDeviceDID, 			// The target device DID
				const CHAR *QueryServerDID[], 		// QueryServer DID
				CHAR *pPostServerString, 			// Save PostServerString
				UINT32 SizeOfPostServerString, 		// PostServerString Buf Size
				CHAR *pSubscribeServerString, 		// Save SubscribeServerString 
				UINT32 SizeOfSubscribeServerString, // SubscribeServerString Buf Size
				CHAR *pUTCTString, 					// Save UTCT From QueryServer response
				UINT32 SizeOfUTCTString);			// UTCTString Buf Size
				
//// WiPN_QueryByServer: Device&&Client
INT32 WiPN_QueryByServer(
                         char *encDecKey,
				const CHAR *pServerString,			// cross-platform Initstring
				const CHAR *pAES128Key,				// cross-platform AES128Key
				const CHAR *pDeviceDID, 			// The target device DID
				const CHAR *QueryServerDID[], 		// QueryServer DID
				CHAR *pPostServerString, 			// Save PostServerString
				UINT32 SizeOfPostServerString, 		// PostServerString Buf Size
				CHAR *pSubscribeServerString, 		// Save SubscribeServerString 
				UINT32 SizeOfSubscribeServerString, // SubscribeServerString Buf Size
				CHAR *pUTCTString, 					// Save UTCT From QueryServer response
				UINT32 SizeOfUTCTString);			// UTCTString Buf Size
				
//// WiPN_Subscribe: Client
INT32 WiPN_Subscribe(
				const CHAR *pSubscribeServerString,	// Obtained by WiPN_Query
				const CHAR *pSubCmd,				// Subscribe Cmd
				CHAR *pRETString,					// Save SubscribeServer return info
				UINT32 SizeOfRETString,				// RETString Buf Size
				CHAR *pUTCTString,					// Save UTCT From SubscribeServer response
				UINT32 SizeOfUTCTString);			// UTCTString Buf Size

//// WiPN_SubscribeByServer
INT32 WiPN_SubscribeByServer(
				const CHAR *pServerString,			// cross-platform Initstring
				const CHAR *pAES128Key,				// cross-platform AES128Key
				const CHAR *pSubscribeServerString,	// Obtained by WiPN_Query
				const CHAR *pSubCmd,				// Subscribe Cmd
				CHAR *pRETString,					// Save SubscribeServer return info
				UINT32 SizeOfRETString,				// RETString Buf Size
				CHAR *pUTCTString,					// Save UTCT From SubscribeServer response
				UINT32 SizeOfUTCTString);			// UTCTString Buf Size			
				
//// WiPN_UnSubscribe: Client
INT32 WiPN_UnSubscribe(
				const CHAR *pSubscribeServerString,	// Obtained by WiPN_Query
				const CHAR *pSubCmd,				// Subscribe Cmd
				CHAR *pRETString,					// Save SubscribeServer return info
				UINT32 SizeOfRETString,				// RETString Buf Size
				CHAR *pUTCTString,					// Save UTCT From SubscribeServer response
				UINT32 SizeOfUTCTString);			// UTCTString Buf Size

//// WiPN_UnSubscribeByServer
INT32 WiPN_UnSubscribeByServer(
				const CHAR *pServerString,			// cross-platform Initstring
				const CHAR *pAES128Key,				// cross-platform AES128Key
				const CHAR *pSubscribeServerString,	// Obtained by WiPN_Query
				const CHAR *pSubCmd,				// Subscribe Cmd
				CHAR *pRETString,					// Save SubscribeServer return info
				UINT32 SizeOfRETString,				// RETString Buf Size
				CHAR *pUTCTString,					// Save UTCT From SubscribeServer response
				UINT32 SizeOfUTCTString);			// UTCTString Buf Size				
				
//// WiPN_Post: Device
INT32 WiPN_Post(const CHAR *pPostServerString, 		// Obtained by WiPN_Query
				const CHAR *pCmd,					// Push cmd
				CHAR *pRETString, 					// Save PostServer return info
				UINT32 SizeOfRETString, 			// RETString Buf Size
				CHAR *pUTCTString, 					// Save UTCT From PostServer response
				UINT32 SizeOfUTCTString);			// UTCTString Buf Size				
				
//// WiPN_PostByServer: Device
INT32 WiPN_PostByServer(
				const CHAR *pServerString,			// cross-platform Initstring
				const CHAR *pAES128Key,				// cross-platform AES128Key
				const CHAR *pPostServerString, 		// Obtained by WiPN_Query
				const CHAR *pCmd,					// Push cmd
				CHAR *pRETString, 					// Save PostServer return info
				UINT32 SizeOfRETString, 			// RETString Buf Size
				CHAR *pUTCTString, 					// Save UTCT From PostServer response
				UINT32 SizeOfUTCTString);			// UTCTString Buf Size

//// WiPN_ChkSubscribe				
INT32 WiPN_ChkSubscribe(
				const CHAR *pSubscribeServerString,	// Obtained by WiPN_Query
				const CHAR *pChkSubCmd,				// Check Subscribe Cmd
				CHAR *pListString,					// Save Server return List Info
				UINT32 SizeOfListString,			// ListString Buf Size
				CHAR *pUTCTString,					// Save UTCT From Server response
				UINT32 SizeOfUTCTString);			// UTCTString Buf Size

//// WiPN_ChkSubscribeByServer
INT32 WiPN_ChkSubscribeByServer(
				const CHAR *pServerString,			// cross-platform Initstring
				const CHAR *pAES128Key,				// cross-platform AES128Key
				const CHAR *pSubscribeServerString,	// Obtained by WiPN_Query
				const CHAR *pChkSubCmd,				// Check Subscribe Cmd
				CHAR *pListString,					// Save Server return ListInfo
				UINT32 SizeOfListString,			// ListString Buf Size
				CHAR *pUTCTString,					// Save UTCT From Server response
				UINT32 SizeOfUTCTString);			// UTCTString Buf Size

INT32 WiPN_SortForServerDID(CHAR *pServerDIDString);

INT32 WiPN_ResetBadge(	const CHAR *pSubscribeServerString,
						const CHAR *pResetBadgeCmd,
						CHAR *pRETString,
						UINT32 SizeOfRETString,
						CHAR *pUTCTString,
						UINT32 SizeOfUTCTString);
						
INT32 WiPN_ResetBadgeByServer(	const CHAR *pServerString,
								const CHAR *pAES128Key,
								const CHAR *pSubscribeServerString,
								const CHAR *pResetBadgeCmd,
								CHAR *pRETString,
								UINT32 SizeOfRETString,
								CHAR *pUTCTString,
								UINT32 SizeOfUTCTString);
//// ------------------------------ WiPN API End ------------------------------

#ifdef __cplusplus
}
#endif 
#endif 	// _WIPN_API_H_
