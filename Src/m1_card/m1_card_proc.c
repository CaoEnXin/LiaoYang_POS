/**************************************************************************************************
* M1_card_proc.c
*
* func: M1卡的处理函数，具备封装性
*
* auth:howard.joe
*
* 留言: 请以后的维护者一定要保证M1卡的核心处理函数的独立性和封装性，保证可以随时仅仅通过拷贝文件的方式
*       就可以方便地移植到别的工程上
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
	{M1_ERR_MULTICARD,       "有多张卡"},
	{M1_ERR_RETRY,           "读卡失败,请重试"},
	{M1_ERR_USERCANCEL,      "用户取消"},
	{0, "未知错误"}
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
		g_stM1InternalMember.ucMifOpenFlag = 0; // 如果dur_time为0，那么相当于调用了mifClose
	}
}

// 调用该函数前，需要将非接模块初始化
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
		iRet = mifCheckOnlyOneCard(0x01); // 只需要检测TYPE A卡
		if(MIF_RET_SUCCESS == iRet)
		{
			// 在感应区内查找指定调制类型的卡片，并返回卡片的序列号等相关信息
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
	
	if (!iErrCode) // 统一规定 返回值0代表成功
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
		lcdDisplay(0, 4, DISP_CFONT, "读卡验证错误!");
		lcdDisplay(0, 6, DISP_CFONT, "请重试!");
		sysDelayMs(3000);
		return NO_DISP;
	} 
	iRet = mifReadBlock(SectorNo*4, pzTranferData);
	if (iRet != MIF_RET_SUCCESS)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "读卡错误!");
		lcdDisplay(0, 6, DISP_CFONT, "请重试!");
		sysDelayMs(3000);
		return NO_DISP;
	}
	iRet = mifReadBlock(SectorNo*4+1, pzTranferData+16);
	if (iRet != MIF_RET_SUCCESS)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "读卡错误!");
		lcdDisplay(0, 6, DISP_CFONT, "请重试!");
		sysDelayMs(3000);
		return NO_DISP;
	}
		iRet = mifReadBlock(SectorNo*4+2, pzTranferData+32);
	if (iRet != MIF_RET_SUCCESS)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "读卡错误!");
		lcdDisplay(0, 6, DISP_CFONT, "请重试!");
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
		lcdDisplay(0, 4, DISP_CFONT, "写卡认证错错误!");
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
			lcdDisplay(0, 4, DISP_CFONT, "写卡错误!");
			lcdDisplay(0, 6, DISP_CFONT, "请重试!");
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
			lcdDisplay(0, 4, DISP_CFONT, "写卡错误!");
			lcdDisplay(0, 6, DISP_CFONT, "请重试!");
			sysDelayMs(3000);
			return NO_DISP;
		}
		iRet = mifTransfer(BlockNo);
		if (iRet != MIF_RET_SUCCESS)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "扣款失败");
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
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "充值失败1");
			sysDelayMs(3000);
			return NO_DISP;
		}
		iRet = mifTransfer(BlockNo);
		if (iRet != MIF_RET_SUCCESS)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "充值失败2");
			sysDelayMs(3000);
			return NO_DISP;
		}
	}
	else
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "输入模式错误3");
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
   // PSAM 由安全模块计算
  }
  else if(SectorNo==1 &&iKeyType==MIF_KEYA)
  {
     memcpy(cKey,Uid,4);
	 memcpy(cKey+4,Uid,2);
  }
  else if(SectorNo==1 &&iKeyType==MIF_KEYB)
  {
	  // PSAM 由安全模块计算

  }
  return 0;
}*/
/****************************************************************************
 功    能:      低位在前的字符串表示转换为整型数
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


