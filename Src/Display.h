
/****************************************************************************
NAME
    Disp.h - ��������ʵ�ú���(���Ӧ�÷�װ������)

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    pyming     2008.04.10      - created
****************************************************************************/

#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "posapi.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void   Display2Strings(uint8_t *pszString1, uint8_t *pszString2);     // ����Ļ�м���ʾ3��5����ʾ  
void   Display2StringInRect(uint8_t *pszString1,  uint8_t *pszString2);
void   DrawRect(uint32_t uiBegRow, uint32_t uiBegCol, uint32_t uiEndRow, uint32_t uiEndCol);
void   DisplayProcessing(void);
void   DisplayMainPosOperFailed(char* dispBuf, unsigned int ulWaitSecond);
int    DisplayMainPosErrMsg(unsigned char ucMsgCode);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _UITL_H

// end of file
