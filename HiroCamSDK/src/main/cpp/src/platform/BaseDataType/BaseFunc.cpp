#include "BaseDataType.h"
#include "AutoLock.h"
namespace HiroCamSDK{
// 获取本地DNS服务器信息
//param : ddns1, ddns2 为传入参数，传入参数空间长度不能小于17
HR_BOOL GetDDnsServer(HR_CHAR* ddns1, HR_CHAR* ddns2) 
{
#ifdef _WIN32
#ifndef _WP8
	// 声明变量
	FIXED_INFO * FixedInfo;     // 定义保存本地计算机网络参数信息的结构体指针
	ULONG    ulOutBufLen;       // 保存获取到的本地计算机网络参数信息结构体链表的长度
	IP_ADDR_STRING * pIPAddr;   // 保存所有DNS服务器的IP地址列表
	
	FixedInfo = (FIXED_INFO *) GlobalAlloc( GPTR, sizeof( FIXED_INFO ) );
	ulOutBufLen = sizeof( FIXED_INFO );
	if( ERROR_BUFFER_OVERFLOW == GetNetworkParams( FixedInfo, &ulOutBufLen ) ) //第一次获取长度
	{
		GlobalFree( FixedInfo );
		FixedInfo = (FIXED_INFO *) GlobalAlloc( GPTR, ulOutBufLen );
	}

	if ( ERROR_SUCCESS != GetNetworkParams( FixedInfo, &ulOutBufLen ) )
	{
		 GlobalFree( FixedInfo );
		 HRLOG_E("GetDDnsServer failed: GetNetworkParams failed!\n");
		 return HR_FALSE;
	}
	else
	{
		char* ddns[2] = {ddns1, ddns2};
		pIPAddr = &(FixedInfo->DnsServerList);
		for (int i=0; i<2 && pIPAddr; i++, pIPAddr = pIPAddr->Next)
		{
			strcpy(ddns[i], pIPAddr->IpAddress.String);
		}
	}
	return HR_TRUE;
#else
	return HR_TRUE;
#endif
#else
#ifdef _MAC
#ifndef _ONLYMAC
    return HR_FALSE;
#else
    HR_BOOL bOk = HR_FALSE;
    FILE *fp = fopen( "/etc/resolv.conf", "rb" );
    if( NULL == fp )
    {
        return bOk;
    }
    char buf[512] = {0};
    if( fread(buf, 1, 512, fp) > 0 )
    {
        int dns[2][10] = {0};
        char* pDDns1 = strstr(buf, "nameserver ");
        if ( pDDns1 )
        {
            if ( 4 == sscanf(pDDns1, "nameserver %d.%d.%d.%d", &dns[0][0], &dns[0][1], &dns[0][2], &dns[0][3]) )
            {
                bOk = HR_TRUE;
                sprintf( ddns1, "%d.%d.%d.%d", dns[0][0], dns[0][1], dns[0][2], dns[0][3] );
            }
            
            char* pDDns2 = strstr(pDDns1 + strlen("nameserver"), "nameserver ");
            if ( pDDns2 )
            {
                if ( 4 == sscanf(pDDns2, "nameserver %d.%d.%d.%d", &dns[1][0], &dns[1][1], &dns[1][2], &dns[1][3]) )
                {
                    bOk = HR_TRUE;
                    sprintf( ddns2, "%d.%d.%d.%d", dns[1][0], dns[1][1], dns[1][2], dns[1][3] );
                }
            }
        }
    }
    fclose( fp );
    return bOk;
#endif
#else
	HR_BOOL bOk = HR_FALSE;
	FILE *fp = fopen( "/etc/resolv.conf", "rb" );
	if( NULL == fp )
    {
        return bOk;
    }
    char buf[512] = {0};
    if( fread(buf, 1, 512, fp) > 0 )
    {
        int dns[2][10] = {0};
		char* pDDns1 = strstr(buf, "nameserver ");
        if ( pDDns1 )
        {
			if ( 4 == sscanf(pDDns1, "nameserver %d.%d.%d.%d", &dns[0][0], &dns[0][1], &dns[0][2], &dns[0][3]) )
			{
				bOk = HR_TRUE;
				sprintf( ddns1, "%d.%d.%d.%d", dns[0][0], dns[0][1], dns[0][2], dns[0][3] );
			}

			char* pDDns2 = strstr(pDDns1 + strlen("nameserver"), "nameserver ");
			if ( pDDns2 )
			{
				if ( 4 == sscanf(pDDns2, "nameserver %d.%d.%d.%d", &dns[1][0], &dns[1][1], &dns[1][2], &dns[1][3]) )
				{
					bOk = HR_TRUE;
					sprintf( ddns2, "%d.%d.%d.%d", dns[1][0], dns[1][1], dns[1][2], dns[1][3] );
				}
			}
        }
    }
    fclose( fp );
    return bOk;
#endif
#endif 
}  

HR_BOOL StrNCpy(HR_CHAR *dst, HR_CHAR *src, HR_U32 dstlen)
{
	if (!dstlen || dst == NULL || src == NULL) return HR_FALSE;
	dst[--dstlen] = '\0';
	strncpy(dst, src, dstlen);
	return HR_TRUE;
}

unsigned char char_to_hex( unsigned char x )
{
	return (unsigned char)(x > 9 ? x + 55: x + 48);
}

int is_alpha_number_char( unsigned char c )
{
	if ( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') )
		return 1;
	return 0;
}

static unsigned char hexchars[] = "0123456789ABCDEF"; 

static int htoi(char *s) 
{ 
	int value; 
	int c; 

	c = ((unsigned char *)s)[0]; 
	if (isupper(c)) 
		c = tolower(c); 
	value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16; 

	c = ((unsigned char *)s)[1]; 
	if (isupper(c)) 
		c = tolower(c); 
	value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10; 
	return (value); 
}

HR_S32  URL_Encode( const HR_CHAR * src, HR_S32  src_len,  HR_CHAR * dest, HR_S32  dest_len )
{
	unsigned char ch;
	int  len = 0;

	while (len < src_len && len < (dest_len - 4) && *src)
	{
		ch = (unsigned char)*src;
		if (*src == ' ') 
		{
			*dest++ = '+';
		}
		else if (is_alpha_number_char(ch) || strchr("-_.", ch)) 
		{
			*dest++ = *src;
		}
		else 
		{
			*dest++ = '%';
			*dest++ = char_to_hex( (unsigned char)(ch >> 4) );
			*dest++ = char_to_hex( (unsigned char)(ch % 16) );
		}  
		++src;
		++len;
	}
	*dest = 0;
	return len;
}

HR_S32 URL_Decode(HR_CHAR *str, HR_S32 len)
{ 
   char *dest = str; 
   char *data = str; 

   while (len--) 
   { 
      if (*data == '+') 
      { 
         *dest = ' '; 
      } 
      else
      if (*data == '%' && len>= 2 && isxdigit((int) *(data + 1)) && isxdigit((int) *(data + 2))) 
      { 
         *dest = (char) htoi(data + 1); 
         data += 2; 
         len -= 2; 
      } 
      else 
      { 
         *dest = *data; 
      } 
      data++; 
      dest++; 
   } 
   *dest = '\0'; 
   return dest - str; 
}

HR_S32 CheckSpecialSymbol(HR_CHAR *str, HR_U32 len)
{
	if (!str && len<=0)
	{
		return -1;
	}
	
	for (int i = 0;i < len;i++)
	{
		if (str[i] == '\'' || str[i] == '@' || str[i] == '#' || str[i] == '%' || str[i] == '^' || str[i] == '*'
			|| str[i] == '(' || str[i] == ')' || str[i] == '_' || str[i] == '+' || str[i] == '-' || str[i] == '{'
			|| str[i] == '}' || str[i] == ';' || str[i] == ':' || str[i] == '"' || str[i] == '|' || str[i] == '<'
			|| str[i] == '>' || str[i] == '?' || str[i] == ',' || str[i] == '.' || str[i] == '\\' || str[i] == '/'
			|| str[i] == '`' )
		{
			return -1;
		}
	}
	return 0;
	
}


HR_S32 GetLastErr()
{
#if _WIN32
	return GetLastError();
#else
	return errno;
#endif
}

}



