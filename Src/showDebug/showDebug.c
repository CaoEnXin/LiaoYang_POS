/*******************************************************************************************
*  version: V1.0.0
*******************************************************************************************/

#include "stdarg.h"
#include "posapi.h"
#include "showDebug.h"

#define DEBUG_LOG "debug_msg.log"

void showDebugMessage(uint32_t ulHexLen, const uint8_t *pHexHead, char *FmtBuf, ...)
{
#define _DEBUG_MSG_LEN 1024 * 3
	char szMessage[_DEBUG_MSG_LEN], sCurTime[7], szBuffer[_DEBUG_MSG_LEN];
	va_list argptr;
	int i;
	const uint8_t *pos;

#ifndef __DEBUG	
	return;
#endif


	//get dateTime
	sysGetTime(sCurTime);
	sprintf((char *)szMessage, "%02X%02X/%02X/%02X %02X:%02X:%02X FILE:%s LINE:%u\n",
			0x20, sCurTime[0], sCurTime[1],
			sCurTime[2], sCurTime[3], sCurTime[4], sCurTime[5], __FILE__, __LINE__);

	//show string
	if (NULL != FmtBuf)
	{
		va_start(argptr, FmtBuf);
		vsprintf(szBuffer, FmtBuf, argptr);
#ifdef _ShowOnScreen
		lcdCls();
		lcdPrintf("%s", szBuffer);
		kbGetKey();
#endif
		sprintf(szMessage + strlen(szMessage), "%s", szBuffer);
		va_end(argptr);	
	}
	
	//show hex
	if (NULL != pHexHead && (ulHexLen < _DEBUG_MSG_LEN - 2 - strlen(szMessage)))
	{
		sprintf(szMessage + strlen(szMessage), "\nHex:\n");
		pos = pHexHead;

		memset(szBuffer, 0, sizeof(szBuffer));
		for (i = 0; i < ulHexLen; ++i)
		{
			sprintf(szBuffer + strlen(szBuffer), "[%02x]", *(pos++));
		}

#ifdef _ShowOnScreen
		lcdCls();
		lcdPrintf("%s", szBuffer);
		kbGetKey();
#endif
		strcat(szMessage + strlen(szMessage), szBuffer);
	}
	
	//the end
	strcat(szMessage, "\n\n");

#ifdef _Debug_Save_Log
		SaveToDebugLog(szMessage);
#endif

#ifdef _SendToComm
	// send msg to com1
	portClose(PORT_COM1);
	portOpen(PORT_COM1, (void *)"115200,8,n,1");
	i = portSends(PORT_COM1, (uint8_t *)szMessage, strlen(szMessage));
#endif
}

void ListenComm(int iPort)
{
	int iRet;
	char szBuffer[1024];
	
	portClose(PORT_MODEM);
	portClose(PORT_COM1);
	iRet = portOpen(iPort, (void *)"9600,8,n,1");
	if (0 != iRet)
	{
		lcdCls();
		lcdPrintf("open port: %d fail", iPort);
		kbGetKey();
		sysReset();
	}
	else
	{
		portReset(iPort);
	}

	memset(szBuffer, 0, sizeof(szBuffer));
	lcdCls();
	lcdPrintf("listening...");
	while(1)
	{
		iRet = portRecv(iPort, szBuffer, 100);
		if (iRet == PORT_RET_OK)	
		{
			lcdCls();
			lcdPrintf("Oh yeah! it comes!");
			kbGetKey();
			portReset(iPort);
			lcdCls();
			lcdPrintf("listening...");
		}
		
	}
	
}

void SaveToDebugLog(char *pszIn)
{
	INT32		iRet, iLogFile;
	
	iLogFile = fileOpen(DEBUG_LOG, O_RDWR|O_CREAT);
	if( iLogFile<0 )
	{
		return;
	}

	iRet = fileSeek(iLogFile, 0, SEEK_END);
	if( iRet<0 ) 
	{
		fileClose(iLogFile);
		return;
	}

	iRet = fileWrite(iLogFile, (UINT8 *)pszIn, strlen(pszIn));
	fileClose(iLogFile);
	if( iRet!=strlen(pszIn) )
	{
		return;
	}
}
//end of file
