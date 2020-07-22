#include "posapi.h"
#include "posparams.h"
#include "glbvar.h"
#include "tranfunclist.h"

static void str_convert(uint8_t *dest, uint8_t *srcStr, uint8_t len)
{
	memset(dest, 0x00, len);
	if( strlen((char *)srcStr)<len )
	{
		strcpy((char *)dest, (char *)srcStr);
	}
	else
	{
		memcpy((char *)dest, (char *)srcStr, len-1);
	}
}

static void ascii_convert(uint8_t *dest, uint8_t *srcStr, uint8_t len)
{
	memset(dest, 0x00, len);
	memcpy(dest, srcStr, len);
}


static uint8_t char_to_bin(uint8_t bchar)
{
	
	if( (bchar>='0')&&(bchar<='9') )
	{
		return(bchar-'0');
	}
	else if( (bchar>='A')&&(bchar<='F') )
	{
		return(bchar-'A'+10);
	}
	else if( (bchar>='a')&&(bchar<='f') )
	{
		return(bchar-'a'+10);
	}
	else
	{
		return 0;
	}
}

static char hex_convert(uint8_t *dest, uint8_t *srcStr, uint8_t len)
{
	uint8_t buff[5];
	uint8_t i, k, *ptr, bin[2], achar;
	uint8_t srclen;

	srclen = strlen((char *)srcStr);
	if( srclen==0 )
	{
		memset(dest, 0x00, len);
		return 0;
	}
	ptr = srcStr;
	for(i=0; (i<len)&&(ptr<srcStr+srclen); )
	{
		memset(buff, 0x00, 5);
		for(k=0; (k<4)&&(ptr<srcStr+srclen); ptr++)
		{
			if( (*ptr!=',') && (*ptr!=' ') )
			{
				buff[k++] = *ptr;
			}
		}
		if( strlen((char *)buff)!=4 )
		{
			continue;
		}
		if( memcmp(buff, "0x", 2) )
		{
			continue;
		}
		bin[0] = char_to_bin(buff[2]);
		bin[0] = bin[0]<< 4;
		bin[1] = char_to_bin(buff[3]);
		achar = bin[0] + bin[1];
		dest[i++] = achar;
	}
	return 0;
}

static void bin_convert(uint8_t *dest, uint8_t *srcStr, uint8_t len)
{
	uint8_t  achar;
	int16_t  i;
	int  l;

	switch(len)
	{
	case sizeof(achar):
		achar = (uint8_t)atoi((char *)srcStr);
		*dest = achar;
		break;
	case sizeof(i):
		i = atoi((char *)srcStr); 
		memcpy(dest, (uint8_t *)&i, len); 
		break;
	case sizeof(l):
		l = atol((char *)srcStr);
		memcpy(dest, (uint8_t *)&l, len);
		break;
	default:
		break;
	}
}


static void AllTrim(uint8_t *str)
{
	int len, count;
	uint8_t *p_str;

	len = strlen((char *)str);
	p_str = str;
	p_str += len-1;
	while( (*p_str==' ')||(*p_str==0x09)||(*p_str==0x0d)||(*p_str==0x0a) )
	{	
		*p_str-- = 0x00;
	}
	p_str = str;
	len = strlen((char *)str);
	count = 0;
	while( (*p_str==' ')||(*p_str==0x09)||(*p_str==0x0d)||(*p_str==0x0a) )
	{
		if( p_str<(str+len) )
		{
			p_str++;
			count++;
		}
		else
		{
			break;
		}
	}
	if( count>0 )
	{
		memmove(str, p_str, len-count);
		memset(str+(len-count), 0x00, count);
	}
}


static int read_cnt;
static int readline(int fd, void *vptr, int maxlen)
{
	int n;
	uint8_t c, *ptr;
	static uint8_t *read_ptr, read_buf[400];

	ptr = vptr;
	for(n=1; n<maxlen; n++)
	{
		if( read_cnt<=0 )
		{
			if( (read_cnt=fileRead(fd, read_buf, sizeof(read_buf)))<0 )
			{
				return -1;
			} 
			else if( read_cnt==0 )
			{
				return 0;
			}
			read_ptr = read_buf;
		}
		read_cnt--;
		c = *read_ptr++;
	
		*ptr++ = c;
		if( c==0x0a )
		{
			break;
		}
	}
	*ptr = 0x00;
	return (n);
}



static int NextParam(int fid, uint8_t *name, uint8_t *value)
{
	uint8_t aline[200+1];
	uint8_t *ptr;
	int ret;

	memset(aline, 0x00, sizeof(aline));
	ret = readline(fid, aline, sizeof(aline)-1);
	if( ret<=0 )
	{
		return -1;
	}
	if( aline[0]=='&' )
	{
		return -1;
	}

	ptr = memchr(aline, '#', strlen((char *)aline));
	if ( ptr!=NULL )
	{
		*ptr = 0x00;
	}
	
	AllTrim(aline);
	ptr = memchr(aline, '=', strlen((char *)aline));
	if( ptr==NULL )
	{
		return 1;
	}

	memcpy(name, aline, ptr-aline);
	strcpy((char *)value, (char *)(ptr+1));
	AllTrim(name);
	AllTrim(value);
	return 0;	
}


int GetParams(char *filename, PARAMS *myParams)
{
	int i, j;
	PARAMS *q;
	uint8_t name[80], value[80];
	int ParamsFid;	

	ParamsFid = fileOpen(filename, O_RDWR);
	if( ParamsFid<0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "文件打开错误", "FILE OPEN ERROR");
		kbFlush();
		kbGetKey();
		return 1;
	}
	read_cnt = 0;//very important.

	for(;;)
	{
		memset(name, 0x00, sizeof(name));
		memset(value, 0x00, sizeof(value));
		j = NextParam(ParamsFid, name, value);
		if( j<0 )
		{
			break;
		}
		if( j>0 )
		{
			continue;
		}
		if( value[0]==0x0a )
		{
			continue;
		}
		for(i=0, q=myParams; ;i++, q++)
		{
			if( q->name[0]=='*' )
			{
				break;
			}
			if( strcmp((char *)name, (char *)q->name) )
			{
				continue;
			}

			switch(q->type)
			{
			case STRING_ITEM:	
				str_convert((uint8_t *)q->ini_ptr, value, q->len);
				break;
			case ASSCII_ITEM:
				ascii_convert((uint8_t *)q->ini_ptr, value, q->len);
				break;
			case HEX_ITEM:
				hex_convert((uint8_t *)q->ini_ptr, value, q->len);
				break;
			case BIN_ITEM:	
				bin_convert((uint8_t *)q->ini_ptr, value, q->len);
				break;
			default:
				break;
			}
			break;
		}
	}
	fileClose(ParamsFid);
	return 0;
}


int  GetParamfileName(void)
{
	int   iRet, iFileNums , iCnt;
	FILE_INFO  stFileInfoList[356];
	char  *pTemp,*pTemp1;
	
	memset(gszParamFileName,0,sizeof(gszParamFileName));
	strcpy(gszParamFileName,"appparam.txt");
	memset(stFileInfoList,0,sizeof(stFileInfoList));
	iFileNums = fileGetInfo(stFileInfoList);
	if (iFileNums <= 0)
	{
		return iFileNums;
	}
	
	for (iCnt=0; iCnt<iFileNums; iCnt++)
	{
		if (stFileInfoList[iCnt].type != FILE_TYPE_DATA)
		{
			continue;
		}
		
		pTemp = strstr(stFileInfoList[iCnt].name,".TXT");
		pTemp1 = strstr(stFileInfoList[iCnt].name,".txt");
		if (pTemp == NULL && pTemp1 == NULL)
		{
			continue;
		}
		
		iRet = fileOpen(stFileInfoList[iCnt].name,O_RDWR);
		if (iRet >= 0)
		{
			fileClose(iRet);
			memset(gszParamFileName,0,sizeof(gszParamFileName));
			memcpy(gszParamFileName,stFileInfoList[iCnt].name,sizeof(stFileInfoList[iCnt].name));
			return 0;
		}
		fileClose(iRet);
	}
	
	return 0;
}

