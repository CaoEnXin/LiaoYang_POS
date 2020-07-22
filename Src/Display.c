
/********************** Internal macros declaration ************************/
/********************** Internal structure declaration *********************/
#include "Display.h"
#include "glbvar.h"


/********************** Internal functions declaration *********************/
/********************** Internal variables declaration *********************/
/********************** external reference declaration *********************/
/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/


void   Display2Strings(uint8_t *pszString1, uint8_t *pszString2)
{
	lcdClrLine(2, 7);
	lcdFlip();	
	PubDisplayMsg(3, pszString1);
	PubDisplayMsg(5, pszString2);
	DrawRect(0, 17, 127, 63);
}

void   Display2StringInRect(uint8_t *pszString1,  uint8_t *pszString2)
{
	Display2Strings(pszString1, pszString2);
	DrawRect(0, 17, 127, 63);
}


void   DrawRect(uint32_t uiBegRow, uint32_t uiBegCol, uint32_t uiEndRow, uint32_t uiEndCol)
{
	UINT32	uiPointx, uiPointy; 

	if (uiBegRow>=127)
	{
		uiEndRow = 127;
	}
	if (uiEndRow>=127)
	{
		uiEndRow = 127;
	}
	if (uiEndRow<=uiBegRow)
	{
		uiEndRow = uiBegRow;
	}

	if (uiBegCol>=63)
	{
		uiBegCol = 63;
	}
	if (uiEndCol>=63)
	{
		uiEndCol = 63;
	}
	if (uiEndCol<=uiBegCol)
	{
		uiEndCol = uiBegCol;
	}

	for (uiPointx=uiBegRow; uiPointx<=uiEndRow;uiPointx++)
	{
		lcdStipple(uiPointx, uiBegCol, ON);
		lcdStipple(uiPointx, uiEndCol, ON);
	}

	for (uiPointy=uiBegCol; uiPointy<=uiEndCol; uiPointy++)
	{
		lcdStipple(uiBegRow, uiPointy, ON);
		lcdStipple(uiEndRow, uiPointy, ON);
	}

	return;
}

void DisplayProcessing(void)
{
	lcdClrLine(2, 7);
	lcdFlip();	
	PubDispString(" �� �� ��",       3|DISP_LINE_LEFT);
	PubDispString(" PROCESSING....", 5|DISP_LINE_LEFT);
	DrawRect(0, 17, 127, 63);
}


void DisplayMainPosOperFailed(char* dispBuf, unsigned int ulWaitSecond)
{
	PubBeepErr();
	lcdClrLine(2, 7);
	if (dispBuf != NULL)
	{
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "%s", dispBuf);//�����ʾһ����Ϣ������16�ֽڽ�ֹ
	}
	else
	{
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "����ʧ��");
	}
	lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "��������");
	PubWaitKey(ulWaitSecond);
	
	return;
}
int DisplayMainPosErrMsg(unsigned char ucMsgCode)
{
	switch(ucMsgCode)
	{
	case 0x01:
	case 0xFF:
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT, "û���ҵ���Ӧ��Կ");
		lcdDisplay(0, 4, DISP_CFONT, "ȷ���̻����ն˺�");
		lcdDisplay(0, 6, DISP_CFONT, "������");
		PubBeepErr();
		kbFlush();
		PubWaitKey(5);
		break;
	case 0x06:
		lcdDisplay(0, 2, DISP_MEDIACY|DISP_CFONT|DISP_CLRLINE, "CMD�����");
		PubBeepErr();
		kbFlush();
		PubWaitKey(5);
		//return OK;
		break;
	case 0x07:
		lcdDisplay(0, 2, DISP_MEDIACY|DISP_CFONT|DISP_CLRLINE, "���ȳ�����Χ");
		PubBeepErr();
		kbFlush();
		PubWaitKey(5);
		//return OK;
		break;
	case 0x08:
		lcdDisplay(0, 2, DISP_MEDIACY|DISP_CFONT|DISP_CLRLINE, "����������");
		PubBeepErr();
		kbFlush();
		PubWaitKey(5);
		//return OK;
		break;
	case 0x09:
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_MEDIACY|DISP_CFONT|DISP_CLRLINE, "У�������");
		PubBeepErr();
		kbFlush();
		PubWaitKey(5);
		//return OK;
		break;
	case 0x0A:
		lcdDisplay(0, 2, DISP_MEDIACY|DISP_CFONT|DISP_CLRLINE, "���ĸ�ʽ����");
		PubBeepErr();
		kbFlush();
		PubWaitKey(5);
		//return OK;
		break;
	default:
		lcdDisplay(0, 2, DISP_MEDIACY|DISP_CFONT|DISP_CLRLINE, "���ձ��Ĵ�");
		PubBeepErr();
		kbFlush();
		PubWaitKey(5);
		//return OK;
		break;
	}
	
	return OK;
}

// END OF FILE



