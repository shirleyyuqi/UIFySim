/*
* Copyright (c) 2014,foscam 研发体系
* All rights reserved.
*
* 文件名称：BaseDataType.h
* 文件标识：
* 摘 要：跨平台类型与函数重定义
*
* 当前版本：1.0
* 作 者：luoc
* 完成日期：2014年7月23日
*
* 取代版本：
* 原作者 ：
* 完成日期：
*/

#ifndef __HIROCAMSDK_SRC_PLATFORM_BASEDATATYPE__
#define __HIROCAMSDK_SRC_PLATFORM_BASEDATATYPE__

#ifdef _WIN32
#include <windows.h>
//#include <ws2def.h>
#include <WinSock2.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")

#ifndef _WP8
#include <IPTypes.h>
#include <IPHlpApi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#else
#include <thread>
#include <mutex>
#endif

#else
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#ifdef ANDROID //Android NDK下的包含路径不一样
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#endif
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#endif

#include <iostream>
#include <vector>
#include <map>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
namespace HiroCamSDK
{
	/*基本类型*/
	typedef unsigned char           HR_U8;
	typedef unsigned short          HR_U16;
	typedef unsigned int            HR_U32;

	typedef char                    HR_S8;
	typedef short                   HR_S16;
	typedef int                     HR_S32;
	typedef char                    HR_CHAR;
    
    typedef const char              HR_CS8;
	
	typedef void					HR_VOID;

	typedef enum {
		HR_FALSE = 0,
		HR_TRUE  = 1,
	} HR_BOOL;

#ifndef _M_IX86
	typedef unsigned long long		HR_U64;
	typedef long long				HR_S64;
#else
	typedef unsigned __int64		HR_U64;
	typedef __int64					HR_S64;
#endif
    
#define HRLOG_I        printf
#define HRLOG_W        printf
#define HRLOG_E        printf

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

#ifndef NULL
#define NULL			0L
#endif
#define HR_NULL		0L
#define HR_SUCCESS		0
#define HR_FAILURE		(-1)
	

#ifndef _WIN32
	#define INFINITE            0xFFFFFFFF  // Infinite timeout
#endif

#define HR_SAFACE_FREE(obj)	if(obj){free(obj);obj=NULL;}
#define HR_SAFACE_DELETE(obj)	if(obj){delete obj;obj=NULL;}
#define HR_SAFACE_DELETE_ARRAY(obj)	if(obj){delete []obj;obj=NULL;}

#define HR_RETURN_VOID
#define _HR_USEASSERT	0
#define _HR_USE_ASSERTASIF	1
#if _HR_USEASSERT
#define HR_ASSERT(a, exec, ret)	assert(a)
#elif _HR_USE_ASSERTASIF
#define HR_ASSERT(a, exec, ret)	if(!(a))\
									{\
										exec;\
										return ret;\
									} 
#else
#define HR_ASSERT(a, exec, ret)
#endif

#ifdef _WIN32
#if defined(_WP8) && !defined(_WIN_APP)

	/*线程相关*/
	typedef HR_U32 (__stdcall *THREADFUNC)(void* lpThreadParameter);
	typedef std::thread				HR_THREAD;
	typedef DWORD					HR_THREADID;
#define THREAD_RETURN(x)		return (x)
#define HR_THREADMETHOD(func, lpParam)	DWORD WINAPI func(void* lpParam)
#define HR_THREADMETHOD_DEC(func, lpParam)	static DWORD WINAPI func(void* lpParam)
#define SLEEP(t)				std::this_thread::sleep_for(std::chrono::milliseconds(10));
#else
	/*线程相关*/
	typedef HR_U32 ( __stdcall *THREADFUNC )(void* lpThreadParameter);
	typedef HANDLE				HR_THREAD;
	typedef DWORD				HR_THREADID;
#define THREAD_RETURN(x)		return (x)
#define HR_THREADMETHOD(func, lpParam)	DWORD WINAPI func(void* lpParam)
#define HR_THREADMETHOD_DEC(func, lpParam)	static DWORD WINAPI func(void* lpParam)
#define SLEEP(t)				Sleep(t)
#endif

#ifdef _WP8
	/*锁相关*/
	typedef std::mutex		MUTEX;
#define INIT_MUTEX(x)				
#define RELEASE_MUTEX(x)			
#define TRYLOCK_MUTEX(x)			(*x).try_lock()
#define LOCK_MUTEX(x)				(*x).lock()
#define UNLOCK_MUTEX(x)				(*x).unlock()
#else
	/*锁相关*/
	typedef CRITICAL_SECTION		MUTEX;
#define INIT_MUTEX(x)				InitializeCriticalSection(x)
#define RELEASE_MUTEX(x)			DeleteCriticalSection(x)
#define TRYLOCK_MUTEX(x)			(!TryEnterCriticalSection(x))
#define LOCK_MUTEX(x)				EnterCriticalSection(x)
#define UNLOCK_MUTEX(x)				LeaveCriticalSection(x)
#endif

/*文件相关*/
#define HR_FILE									FILE
#define HR_FILE_OPEN(name, mode)					fopen(name, mode)
#define HR_FILE_CLOSE(fp)							fclose(fp)
#define HR_FILE_WRITE(buf, size, n, fp)			fwrite(buf, size, n, fp)
#define HR_FILE_READ(buf, size, n, fp)				fread(buf, size, n, fp)
#define HR_FILE_SEEK(fp, offset, origin)			fseek(fp, offset, origin)
#define HR_FILE_TELL(fp)							ftell(fp)
#define HR_FILE_GETERR()							GetLastError();

/*socket相关*/
	typedef	SOCKET									HR_SOCKET;
	typedef	int										HR_SOCKLEN;
//socket 初始化与反初始化,整个SDK只调用一次
#define HR_SOCK_INIT		WSADATA data;\
							WSAStartup(MAKEWORD(2, 2), &data);
#define HR_SOCK_DEINIT		WSACleanup();
#define HR_SOCK_IOCTL(x, y, z)						::ioctlsocket(x, y, z)
#define HR_SOCK_CLOSE(x)							::closesocket(x)
#define HR_SOCK_CREATE(af, type, protocol)			::socket(af, type, protocol)
#define HR_SOCK_LISTEN(s, backlog)					::listen(s, backlog)
#define HR_SOCK_BIND(s, addr, namelen)				::bind(s, addr, namelen)
#define HR_SOCK_ACCEPT(s, addr, addrlen)			::accept(s, addr, addrlen)
#define HR_SOCK_CONNECT(s, name, namelen)			::connect(s, name, namelen)
#define HR_SOCK_SEND(s, buf, len, flags)			::send(s, buf, len, flags)
#define HR_SOCK_RECV(s, buf, len, flags)			::recv(s, buf, len, flags)
#define HR_SOCK_SENDTO(s, buf, len, flags, to, tolen)		::sendto(s, buf, len, flags, to, tolen)
#define HR_SOCK_RECVFROM(s, buf, len, flags, from, fromlen)		::recvfrom(s, buf, len, flags, from, fromlen)
#define HR_SOCK_SELECT(nfds, readfds, writefds, exceptfds, timeout)	::select(nfds, readfds, writefds, exceptfds, timeout)
#define HR_SOCK_SETOPT(s, level, optname, optval, optlen)	::setsockopt(s, level, optname, optval, optlen)
#define HR_SOCK_GETOPT(s, level, optname, optval, optlen)	::getsockopt(s, level, optname, optval, optlen)
#define HR_HTONL(hostlong)		::htonl(hostlong)
#define HR_NTOHL(hostlong)		::ntohl(netlong)
#define HR_HTONS(hostshort)	::htons(hostshort)
#define HR_NTOHS(hostshort)	::ntohs(netshort)
#define HR_INET_ADDR(cp)		inet_addr(cp)
#define HR_INADDR_NONE			INADDR_NONE
#else	/* linux or mac */

/*线程相关*/
	typedef void* (*THREADFUNC)(void*);
	typedef pthread_t			HR_THREAD;
	typedef void*				HR_THREADID;
#define	THREAD_FUNC_TO			void *(*)(void *)
#define WAIT_TIMEOUT			0
#define THREAD_RETURN(x)		return NULL
#define HR_THREADMETHOD(func, lpParam)	void* func(void* lpParam)
#define HR_THREADMETHOD_DEC(func, lpParam)	static void* func(void* lpParam)
#define SLEEP(t)				usleep(t*1000)

/*锁相关*/
	typedef pthread_mutex_t			MUTEX;
#define INIT_MUTEX(x)				pthread_mutex_init(x, NULL)
#define RELEASE_MUTEX(x)			pthread_mutex_destroy(x)
#define TRYLOCK_MUTEX(x)			pthread_mutex_trylock(x)
#define LOCK_MUTEX(x)				pthread_mutex_lock(x)
#define UNLOCK_MUTEX(x)				pthread_mutex_unlock(x)

/*文件相关*/
#define HR_FILE									FILE
#define HR_FILE_OPEN(name, mode)					fopen(name, mode)
#define HR_FILE_CLOSE(fp)							fclose(fp)
#define HR_FILE_WRITE(buf, size, n, fp)			fwrite(buf, size, n, fp)
#define HR_FILE_READ(buf, size, n, fp)				fread(buf, size, n, fp)
#define HR_FILE_SEEK(fp, offset, origin)			fseek(fp, offset, origin)
#define HR_FILE_TELL(fp)							ftell(fp)
#define HR_FILE_GETERR()							errno;

/*socket相关*/
	typedef	int										HR_SOCKET;
	typedef	socklen_t								HR_SOCKLEN;
#define HR_SOCK_INIT
#define HR_SOCK_DEINIT
#define HR_SOCK_IOCTL(x, y, z)						::fcntl((x), (y), (z))
#define HR_SOCK_CLOSE(x)							::close((x))
#define HR_SOCK_CREATE(af, type, protocol)			::socket(af, type, protocol)
#define HR_SOCK_LISTEN(s, backlog)					::listen(s, backlog)
#define HR_SOCK_BIND(s, addr, namelen)				::bind(s, addr, namelen)
#define HR_SOCK_ACCEPT(s, addr, addrlen)			::accept(s, addr, addrlen)
#define HR_SOCK_CONNECT(s, name, namelen)			::connect(s, name, namelen)
#define HR_SOCK_SEND(s, buf, len, flags)			::send(s, buf, len, flags)
#define HR_SOCK_RECV(s, buf, len, flags)			::recv(s, buf, len, flags)
#define HR_SOCK_SENDTO(s, buf, len, flags, to, tolen)		::sendto(s, buf, len, flags, to, tolen)
#define HR_SOCK_RECVFROM(s, buf, len, flags, from, fromlen)				::recvfrom(s, buf, len, flags, from, fromlen)
#define HR_SOCK_SELECT(nfds, readfds, writefds, exceptfds, timeout)	::select(nfds, readfds, writefds, exceptfds, timeout)
#define HR_SOCK_SETOPT(s, level, optname, optval, optlen)	::setsockopt(s, level, optname, optval, optlen)
#define HR_SOCK_GETOPT(s, level, optname, optval, optlen)	::getsockopt(s, level, optname, optval, optlen)
#define HR_HTONL(hostlong)		::htonl(hostlong)
#define HR_NTOHL(hostlong)		::ntohl(netlong)
#define HR_HTONS(hostshort)	::htons(hostshort)
#define HR_NTOHS(hostshort)	::ntohs(netshort)
#define HR_INET_ADDR(cp)		inet_addr(cp)
#define HR_INADDR_NONE			INADDR_NONE
#endif
}
#endif
