/*******************************************************************************************
*  version: V1.0.0
*******************************************************************************************/

#ifndef _SHOWDEBUG_H
#define _SHOWDEBUG_H

//fortest
#define __DEBUG
#define  _SendToComm
//#define _ShowOnScreen
//#define _Debug_Save_Log

#define _DEBUG_STR 0x01
#define _DEBUG_HEX 0x02


#ifdef __cplusplus
extern "C" {
#endif

void showDebugMessage(uint32_t ulHexLen, const uint8_t *pHexHead, char *FmtBuf, ...);
void ListenComm(int iPort);
void SaveToDebugLog(char *pszIn);
#ifdef __cplusplus
}
#endif

#endif
//end of file
