/**************************************************************************************************
* M1_card_proc.c
*
* func: M1���Ĵ��������߱���װ��
*
* auth:howard.joe
*
* ����: ���Ժ��ά����һ��Ҫ��֤M1���ĺ��Ĵ������Ķ����Ժͷ�װ�ԣ���֤������ʱ����ͨ�������ļ��ķ�ʽ
*       �Ϳ��Է������ֲ����Ĺ�����
*
**************************************************************************************************/
#include "posapi.h"
#include "../PosLib.h"
#include "m1_card_proc.h"
#include "../showDebug/showDebug.h" // version: v1.0.0
//#include "../posparams.h"
#include "../posmacro.h"
#include "../MathLib.h"

static M1_INTERNAL_MEMBER g_stM1InternalMember = {
	0
};

static M1_DISP_ERRLIST g_stM1DispInfoList[] = {
	{M1_ERR_MULTICARD,       "�ж��ſ�"},
	{M1_ERR_RETRY,           "����ʧ��,������"},
	{M1_ERR_USERCANCEL,      "�û�ȡ��"},
	{0, "δ֪����"}
};

void M1_mifOpen(void)
{
	if (!g_stM1InternalMember.ucMifOpenFlag)
	{
		mifOpen();
		g_stM1InternalMember.ucMifOpenFlag = 1;
		showDebugMessage(0, NULL, "open mif module");
	}
}


void M1_mifClose(void)
{
	if (g_stM1InternalMember.ucMifOpenFlag)
	{
		mifClose();
		g_stM1InternalMember.ucMifOpenFlag = 0;
		showDebugMessage(0, NULL, "close mif module");
	}
}

void M1_mifResetRF(UINT32 dur_time)
{
	mifResetRF(dur_time);
	showDebugMessage(0, NULL, "reset mif module dur_time=%d", dur_time);
	if (0 == dur_time)
	{
		g_stM1InternalMember.ucMifOpenFlag = 0; // ���dur_timeΪ0����ô�൱�ڵ�����mifClose
	}
}

// ���øú���ǰ����Ҫ���ǽ�ģ���ʼ��
INT32 M1_WaitCard(INT32 *CardInfoLen, UINT8 *CardInfo)
{
	int iRet = 0;

	if (!CardInfoLen || !CardInfo)
	{
		showDebugMessage(0, NULL, "param erro.");
		return M1_ERR_OTHER;
	}
	
	while(1)
	{
		iRet = mifCheckOnlyOneCard(0x01); // ֻ��Ҫ���TYPE A��
		if(MIF_RET_SUCCESS == iRet)
		{
			// �ڸ�Ӧ���ڲ���ָ���������͵Ŀ�Ƭ�������ؿ�Ƭ�����кŵ������Ϣ
			iRet = InListPassiveTarget(0x00, 0, NULL, CardInfoLen, CardInfo);
			if (MIF_RET_SUCCESS == iRet)
			{
				return 0;
			}
		}
		else if (MIF_RET_MULTICARD == iRet)
		{
			return M1_ERR_MULTICARD;
		}
		else if (MIF_RET_NOCARD != iRet)
		{
			return M1_ERR_RETRY;
		}
		
		if(PubChkKeyInput())
		{
			iRet = kbGetKey();
			if(iRet == KEY_CANCEL)
			{
				return M1_ERR_USERCANCEL;
			}
		}

	}

	return M1_ERR_OTHER;
}

void M1_DispErrInfo(int iErrCode)
{
	int iCnt = 0;
	
	if (!iErrCode) // ͳһ�涨 ����ֵ0����ɹ�
	{
		return;
	}

	iCnt = 0;
	while (0 != g_stM1DispInfoList[iCnt].iErrCode)
	{
		if (g_stM1DispInfoList[iCnt].iErrCode == iErrCode)
		{
			break;
		}
		
		iCnt++;
	}
	
	lcdClrLine(2, 7);
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "%s", g_stM1DispInfoList[iCnt].szInfo1);
	PubBeepErr();
	PubWaitKey(5);
}
UINT8  ProcContEventRead(UINT32 SectorNo, char* cKey, int iKeyType, char* pzTranferData,  UINT8 *Uid)
{
	int			iRet;
//	ulong 		iAmount;
	
	iRet = mifAuthByKey(SectorNo*4+3, iKeyType, cKey, Uid);
	if (iRet != MIF_RET_SUCCESS)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "������֤����!");
		lcdDisplay(0, 6, DISP_CFONT, "������!");
		sysDelayMs(3000);
		return NO_DISP;
	} 
	iRet = mifReadBlock(SectorNo*4, pzTranferData);
	if (iRet != MIF_RET_SUCCESS)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "��������!");
		lcdDisplay(0, 6, DISP_CFONT, "������!");
		sysDelayMs(3000);
		return NO_DISP;
	}
	iRet = mifReadBlock(SectorNo*4+1, pzTranferData+16);
	if (iRet != MIF_RET_SUCCESS)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "��������!");
		lcdDisplay(0, 6, DISP_CFONT, "������!");
		sysDelayMs(3000);
		return NO_DISP;
	}
		iRet = mifReadBlock(SectorNo*4+2, pzTranferData+32);
	if (iRet != MIF_RET_SUCCESS)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "��������!");
		lcdDisplay(0, 6, DISP_CFONT, "������!");
		sysDelayMs(3000);
		return NO_DISP;
	}
	return MIF_RET_SUCCESS;
}
UINT8  ProcContEventWrite(UINT32 BlockNo, char* cKey, int iKeyType, char* pzTranferData, int iMode, UINT8 *Uid)
{
	int			iRet;
	ulong 		iAmount;
	
	iRet = mifAuthByKey(BlockNo, iKeyType, cKey, Uid);
	if (iRet != MIF_RET_SUCCESS)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "д����֤�����!");
		lcdDisplay(0, 6, DISP_CFONT, "iRet=%d",iRet);
		sysDelayMs(3000);
		kbGetKey();
		return NO_DISP;
	} 

    if(iMode == M1_WRITE_CARD)
	{
		iRet = mifWriteBlock(BlockNo, pzTranferData);
		if (iRet != MIF_RET_SUCCESS)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 4, DISP_CFONT, "д������!");
			lcdDisplay(0, 6, DISP_CFONT, "������!");
			sysDelayMs(3000);
			return NO_DISP;
		}
	}
	else if(iMode == M1_DECREMENT)
	{
		PubChar2Long(pzTranferData, 4, &iAmount);
		iRet = mifDecrement(BlockNo, (UINT32)iAmount);
		if (iRet != MIF_RET_SUCCESS)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 4, DISP_CFONT, "д������!");
			lcdDisplay(0, 6, DISP_CFONT, "������!");
			sysDelayMs(3000);
			return NO_DISP;
		}
		iRet = mifTransfer(BlockNo);
		if (iRet != MIF_RET_SUCCESS)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "�ۿ�ʧ��");
			sysDelayMs(3000);
			return NO_DISP;
		}
	}
	else if(iMode == M1_INCREMENT)
	{
		PubChar2LongEx(pzTranferData, 4, &iAmount);
		iRet = mifIncrement(BlockNo, (UINT32)iAmount);
		if (iRet != MIF_RET_SUCCESS)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "��ֵʧ��1");
			sysDelayMs(3000);
			return NO_DISP;
		}
		iRet = mifTransfer(BlockNo);
		if (iRet != MIF_RET_SUCCESS)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "��ֵʧ��2");
			sysDelayMs(3000);
			return NO_DISP;
		}
	}
	else
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "����ģʽ����3");
		sysDelayMs(3000);
		return NO_DISP;
	}
	return MIF_RET_SUCCESS;
}


UINT8  M1_ReadSector(UINT32 SectorNo, char* cKey, int iKeyType,char* pzOutData, UINT8 *Uid)
{
   return ProcContEventRead(SectorNo, cKey, iKeyType, pzOutData,  Uid);
}
UINT8  M1_WriteBlock(UINT32 BlockNo, char* cKey, int iKeyType,char* pzInData, UINT8 *Uid)//
{

   return ProcContEventWrite(BlockNo, cKey, iKeyType, pzInData, M1_WRITE_CARD, Uid);
}

UINT8  M1_Decrement(UINT32 BlockNo, char* cKey, int iKeyType,char* pzInData, UINT8 *Uid)//
{

   return ProcContEventWrite(BlockNo, cKey, iKeyType, pzInData, M1_DECREMENT, Uid);
}

/*UINT8 CalPubKey(UINT32 SectorNo,int iKeyType,char* cKey,UINT8 *Uid)//
{
  if(SectorNo==0 &&iKeyType==MIF_KEYA)
  {
   memcpy(cKey,"\xA0\xA1\xA2\xA3\xA4\xA5",6);
  }
  else if(SectorNo==0 &&iKeyType==MIF_KEYB)
  {
   // PSAM �ɰ�ȫģ�����
  }
  else if(SectorNo==1 &&iKeyType==MIF_KEYA)
  {
     memcpy(cKey,Uid,4);
	 memcpy(cKey+4,Uid,2);
  }
  else if(SectorNo==1 &&iKeyType==MIF_KEYB)
  {
	  // PSAM �ɰ�ȫģ�����

  }
  return 0;
}*/
/****************************************************************************
 ��    ��:      ��λ��ǰ���ַ�����ʾת��Ϊ������
****************************************************************************/
ulong PubChar2LongEx(char *psString, int iStrLen, ulong *pulOut)
{
	int   iCnt;
	char  szBuffer[100];

	memset(szBuffer,0,sizeof(szBuffer));
	for (iCnt=0; iCnt< iStrLen; iCnt++)
	{
		szBuffer[iCnt] = psString[iStrLen-iCnt-1];
	}

	return PubChar2Long(szBuffer,iStrLen,pulOut);
}


