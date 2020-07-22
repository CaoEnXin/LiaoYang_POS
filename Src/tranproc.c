
#include "posapi.h"
#include "posmacro.h"
#include "terstruc.h"
#include "glbvar.h"
#include "tranfunclist.h"
#include "st8583.h"
#include "posappapi.h"
#include "emvtranproc.h"
#include "emvapi.h"
#include "qpbocProc.h"
#include "otherTxn.h"

uint8_t SaleTranGetData(void);
uint8_t SaleVoidGetData(void);
uint8_t RefundTranGetData(void);
uint8_t AuthComfirmGetData(void);
uint8_t PreAuthVoidGetData(void);
uint8_t OffSaleGetData(void);
uint8_t AdjustTranGetData(void);
uint8_t PreAuthGetData(void);

static int    iAidNum, iCapkNum;
static struct terminal_aid_info	*pstCurApp;			
static struct terminal_aid_info	*pstAppList;		
static TermAidList	*pstCurTermAid;		
static struct issuer_ca_public_key	*pstCurCapk;		
static CapkInfo		*pstCapkInfoList;	
static uint8_t		bRidOk, bKeyIDOk, bExpiryOk;


uint8_t PosOnlineLogon(uint8_t flag)
{
	uint8_t	ucRet, sBuff[10],sTempBuff[60];
	int	lBatchNo;
	int  ret;
	char szSN[20];

	ret = 0;
	memset(sTempBuff,0,sizeof(sTempBuff));
	lcdCls();
	lcdFlip();	
	InitCommData();
	
	if( flag==0 || flag==1 )
	{
		ucRet = TellerLogon(1);
		if( ucRet!=OK )
		{
			return ucRet;
		}
	}

	if( strcmp((char *)stTransCtrl.szNowTellerNo, operStrc[0].szTellerNo)==0 )
	{
		SaveCtrlParam();
		return OK;
	}

	if( flag==0 && stTransCtrl.ucLogonFlag==LOGGEDON )
	{
		SaveCtrlParam();
		return OK;
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    签    到    ", "     LOGON      ");
	DispMulLanguageString(0, 2, DISP_CFONT, "已连接银联中心", "CONNECT BANK");
	
	InitCommData();
    stTemp.iTransNo = POS_LOGON;
	PosCom.stTrans.iTransNo = POS_LOGON;
	GetPosTime(stTemp.szDate, stTemp.szTime);		
	strcpy((char *)PosCom.stTrans.szDate, (char *)stTemp.szDate);
	strcpy((char *)PosCom.stTrans.szTime, (char *)stTemp.szTime);

	SetCommReqField((uint8_t *)"0800", (uint8_t *)"");
	sprintf((char *)glSendPack.szField63, "%.2s ", stTransCtrl.szNowTellerNo+1);
	ShortToByte(glSendPack.sField62, 25);

	memset(sTempBuff,0,sizeof(sTempBuff));
	memset(szSN,0,sizeof(szSN));
	sysReadSN(szSN);
#ifdef VERSION_NEW6110
	sprintf(sTempBuff,"Sequence No123125%8.8s",szSN);  // 6110的证书编号是P3125
#else
	sprintf(sTempBuff,"Sequence No123136%8.8s",szSN);  // 8110的序列号是:P3136
#endif
	sprintf((char*)glSendPack.sField62+2,"%s",sTempBuff);

	ucRet = SendRecvPacket();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	if( memcmp(PosCom.szRespCode, "00", 2)!=0 )
    {
 		dispRetCode();
	    return NO_DISP;
    }

	sprintf((char *)sBuff, "%.6s", &glRecvPack.szField60[2]);
	lBatchNo = atol((char *)sBuff);
	if( lBatchNo!=stPosParam.lNowBatchNum && lBatchNo>0 && lBatchNo<999999L )
	{
		stPosParam.lNowBatchNum = lBatchNo;
		SaveAppParam();
	}

	if( LogonDivKey(&glRecvPack.sField62[2]) )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "下载工作密钥失败", "LOAD KEY FAIL");
		DispMulLanguageString(0, 4, DISP_CFONT, "请确认主密钥下载", "LOAD MASTER KEY");
		kbGetKey();
		return E_PINPAD_KEY;
	}
	

	if( fileExist("stat.tm")<0 )
	{
		write_stat_time();
		memset(&dial_stat, 0, sizeof(DIAL_STAT));
		write_data(&dial_stat, sizeof(DIAL_STAT), "dial.sta");
	}

	// 检查时间格式
	if( CheckDate((char *)PosCom.stTrans.szDate)==0 && CheckTime((char *)PosCom.stTrans.szTime)==0 )
	{
		AscToBcd(sBuff,   PosCom.stTrans.szDate+2, 6);
		AscToBcd(sBuff+3, PosCom.stTrans.szTime,   6);
		sysSetTime(sBuff);
	}

	stTransCtrl.ucLogonFlag = LOGGEDON;
	SaveCtrlParam();

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "签到成功", "SUCCESS");
	OkBeep();

	OnlineDownOrUp();  // 联机下载或者上送参数等
	if( stTransCtrl.bEmvAppLoaded==FALSE )
	{
		DownLoadEmvParam(FALSE);
	}
	if( stTransCtrl.bEmvCapkLoaded==FALSE )
	{
		DownLoadCAPK(FALSE);
	}

	return OK;
}

uint8_t PosOperLogon(uint8_t flag)
{
	uint8_t	ucRet;

	lcdCls();
	lcdFlip();	
	InitCommData();
	
	if( flag==0 || flag==1 )
	{
		ucRet = TellerLogon(1);
		if( ucRet!=OK )
		{
			return ucRet;
		}
	}

	if( strcmp((char *)stTransCtrl.szNowTellerNo, operStrc[0].szTellerNo)==0 )
	{
		SaveCtrlParam();
		return OK;
	}

	if( flag==0 && stTransCtrl.ucLogonFlag==LOGGEDON )
	{
		SaveCtrlParam();
		return OK;
	}

	stTransCtrl.ucLogonFlag = LOGGEDON;
	SaveCtrlParam();

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "签到成功", "SUCCESS");
	OkBeep();

	return OK;
}

uint8_t PosOnlineLogoff(uint8_t flag)
{
	uint8_t	ucRet;

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    终端签退    ", "    LOGOUT     ");

	InitCommData();
	stTemp.iTransNo = POS_LOGOFF;
	PosCom.stTrans.iTransNo = POS_LOGOFF;
	
	SetCommReqField((uint8_t *)"0820", (uint8_t *)"");

	ucRet = OnlineCommProc();
	if( flag && ucRet==OK )
	{
		stTransCtrl.ucLogonFlag = LOGGEDOFF;
		SaveCtrlParam();
	}

	return ucRet;
}

uint8_t PosSettleTran(void)
{
	uint8_t	ucRet, ucKey, szBuff[4][12+1];
	int	lBatchNo;

	kbFlush();
	lcdCls(); 
	lcdFlip();	
	lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    批    结    ");

	lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "是否批结算?");
	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "按[确认]键继续");
	lcdDisplay(0, 6, DISP_CFONT|DISP_MEDIACY, "  [其它]键取消");
	ucKey = kbGetKeyMs(stPosParam.ucOprtLimitTime*1000);
	if( ucKey!=KEY_ENTER )
	{
		return NO_DISP;
	}

	ucRet = QueryTransTotal();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    批    结    ", "     SETTLE     ");
/*
//20080801 先上送磁条卡离线类交易,需要打印未成功上送的交易
	SendOffLineTrans(2, 0);
	// 上送IC卡脱机交易及磁卡脱机交易
	SendICCOffLineTrans(0, 0);*/
	// 连续循环处理
	while(GetOfflineTxnNums())
	{
		ucRet = TrickleFeedOffLineTxn(FALSE);
		if (ucRet != 0)
		{
			break;
		}
		ucRet = TrickleFeedICOffLineTxn(FALSE);
		if (ucRet != 0)
		{
			break;
		}
	}

	InitCommData();
	GetPosTime(stTemp.szDate, stTemp.szTime);
    strcpy((char *)PosCom.stTrans.szDate, (char *)stTemp.szDate);
	strcpy((char *)PosCom.stTrans.szTime, (char *)stTemp.szTime);

	stTemp.iTransNo = POS_SETT;
	PosCom.stTrans.iTransNo = POS_SETT;
	PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;

	SetCommReqField((uint8_t *)"0500", (uint8_t *)"");
	BcdToAsc0(szBuff[0], stTotal.sRmbDebitAmt,  12);
	BcdToAsc0(szBuff[1], stTotal.sRmbCreditAmt, 12);
	BcdToAsc0(szBuff[2], stTotal.sFrnDebitAmt,  12);
	BcdToAsc0(szBuff[3], stTotal.sFrnCreditAmt, 12);
	sprintf((char *)glSendPack.szField48, "%s%03d%s%03d0%s%03d%s%03d0",
			szBuff[0], stTotal.iRmbDebitNum, szBuff[1], stTotal.iRmbCreditNum,
			szBuff[2], stTotal.iFrnDebitNum, szBuff[3], stTotal.iFrnCreditNum);
	sprintf((char *)glSendPack.szCurrencyCode, "156");
	sprintf((char *)glSendPack.szField63, "%.2s ", stTransCtrl.szNowTellerNo+1);
	
	ucRet = OnlineCommProc();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	stTransCtrl.ucPosStatus = BATCH_STATUS;
	stTransCtrl.ucBatchUpStatus = ALLLOG;

	stTransCtrl.ucRmbSettSaveRsp = stTemp.ucRmbSettRsp;
	stTransCtrl.ucFrnSettSaveRsp = stTemp.ucFrnSettRsp;

	if( stTemp.ucRmbSettRsp=='2' && stTemp.ucFrnSettRsp=='2' )
	{ 
		stTransCtrl.ucBatchUpStatus = ALLLOG;
	}
	else if( stTemp.ucRmbSettRsp=='2' && stTemp.ucFrnSettRsp=='1' )
	{	
		stTransCtrl.ucBatchUpStatus = RMBLOG;
	}
	else if( stTemp.ucRmbSettRsp=='1' && stTemp.ucFrnSettRsp=='2' )
	{	
		stTransCtrl.ucBatchUpStatus = FRNLOG;
	}
	else if( stTemp.ucRmbSettRsp=='1' && stTemp.ucFrnSettRsp=='1' )
	{	
		stTransCtrl.ucBatchUpStatus = EMVLOG;
	}
	SaveCtrlParam();
	
	ucRet = PosBatchUp(stTransCtrl.ucBatchUpStatus);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	stTransCtrl.ucPosStatus = WORK_STATUS;
	SaveCtrlParam();		
	
	lBatchNo = 0L;
	if( stPosParam.ucAutoLogoff==PARAM_OPEN )
	{
		lBatchNo = stPosParam.lNowBatchNum;
		stPosParam.lNowBatchNum++;
		PosOnlineLogoff(0);
		SaveCtrlParam();
	}

	CommHangUp(FALSE);
	
	ucRet = ComputeTransTotal(1);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	stTransCtrl.ucClearLog = TRUE;
	SaveCtrlParam();

	ucRet = PrtTransTotal(1, lBatchNo);
	if( ucRet!=OK )
	{
//20080516 当打印缺纸的时候也要增加批次号
		if( ucRet==PRN_PAPEROUT )
		{
			AfterSettleTran();
		}
		return ucRet;
	}
	
	AfterSettleTran();  // 防止总计单批次号不匹配

	kbFlush();
	if( stPosParam.ucDetailPrt==PARAM_OPEN )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 2, DISP_CFONT, "是否打印明细?");
		lcdDisplay(0, 4, DISP_CFONT, "0.否");
		lcdDisplay(0, 6, DISP_CFONT, "1.是");
		ucKey = kbGetKeyMs(30000);
		if( ucKey==KEY1 )
		{
			ucRet = PrintDetail(FALSE,TRUE);
		}
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "是否打印失败明细");
	lcdDisplay(0, 4, DISP_CFONT, "0.否");
	lcdDisplay(0, 6, DISP_CFONT, "1.是");
	ucKey = kbGetKeyMs(30000);
	if( ucKey==KEY1 )
	{
		ucRet = PrintAllErrOffTrans(); 
	}
		
	return OK;
}

void AfterSettleTran()
{
	uint8_t buf[16];

	memset(&stLimitTotal, 0, sizeof(struct _limit_total_strc));
	SaveLimitTotal();

	if( stPosParam.ucAutoLogoff==PARAM_OPEN )	
	{
		stTransCtrl.ucLogonFlag = LOGGEDOFF;
		strcpy((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY);
		stTotal.lBatchNum = stPosParam.lNowBatchNum - 1;
	}
	else
	{
		stPosParam.lNowBatchNum += 1; 
		stTotal.lBatchNum = stPosParam.lNowBatchNum;
	}
	strcpy((char *)stTotal.sDateTime, (char *)stTemp.szDate);
	strcpy((char *)(stTotal.sDateTime+8), (char *)stTemp.szTime);
	SaveLastTotal(); 

	sprintf((char*)buf, "%06d", stPosParam.lNowBatchNum);
	SaveAppParam();
}

uint8_t  PosBatchSendTran(void)
{
	uint8_t  result, key;
	int  ret;

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "POS批上送", "POS UPLOAD");
	DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "请稍侯", "WAITING");

	if( stTransCtrl.ucBatchUpStatus==0 )
	{
		return OK;
	}

	ret = PosBatchUp(stTransCtrl.ucBatchUpStatus);
	if( OK!=ret ) 
		return ret;

	stTransCtrl.ucPosStatus = WORK_STATUS;
	SaveCtrlParam();

	CommHangUp(FALSE);
	
	GetPosTime(stTemp.szDate, stTemp.szTime);		
	result = ComputeTransTotal(1);
	if( result!=OK ) 
	{
		return result;
	}

	result = PrtTransTotal(1, 0);
	if( result!=OK )
		return (result);

	AfterSettleTran();  ///防止总计单批次号不匹配
	
	kbFlush();
	if( stPosParam.ucDetailPrt==PARAM_OPEN )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 2, DISP_CFONT, "是否打印明细?");
		lcdDisplay(0, 4, DISP_CFONT, "0.否");
		lcdDisplay(0, 6, DISP_CFONT, "1.是");
		key = kbGetKeyMs(30000);
		if( key==KEY1 )
		{
			result = PrintDetail(FALSE,TRUE);
		}
	}
	
	PrintAllErrOffTrans(); 	

	return (OK);
}


uint8_t PosBatchUp(uint8_t flag)
{
	uint8_t	ucRet;
	int	iTotalNum;

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "     批上送     ", "     UPLOAD      ");
	DispMulLanguageString(0, 2, DISP_CFONT, "已连接银联中心  ", "CONNECT BANK");

	iTotalNum = 0;		

//20080801 最新上送所有离线类交易(通知类)
	// 逐笔批上送所有离线交易(通知类)
	ucRet = BatchSendOffTrans1(&iTotalNum, flag);
	printf("totnum1: %d \n",iTotalNum);
	if( ucRet!=OK )
	{
		return ucRet;
	}

#ifdef PBOC_VERSION
	if( flag==ALLLOG || flag==RMBLOG || flag==FRNLOG )
	{
		ucRet = BatchICCOffLineTrans(&iTotalNum, flag);
		printf("totnum2: %d \n",iTotalNum);
		if( ucRet!=OK )
		{
			return ucRet;
		}
	}
#endif
	
	// 上送磁卡联机交易
	ucRet = BatchMagOnLineTrans(&iTotalNum, flag);
	printf("totnum3: %d \n",iTotalNum);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	//上送磁卡联机交易 -- 先退货
	ucRet = BatchSendOffTrans2_Refund(&iTotalNum, flag);
	printf("totnum4: %d \n",iTotalNum);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	//上送磁卡联机交易 -- 燃烧预授权通知
	ucRet = BatchSendOffTrans2_CM(&iTotalNum, flag);
	printf("totnum4: %d \n",iTotalNum);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	// 逐笔批上送所有IC卡联机交易
	ucRet = BatchSendICCOnLineTrans(&iTotalNum, flag);
	printf("totnum5: %d \n",iTotalNum);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	// 上送失败的ICC交易，目前只有QPBOC的失败交易
	ucRet = TransBatchUpLoadFailureTxn(&iTotalNum, flag);
	printf("totnum6: %d \n",iTotalNum);
	if( ucRet!=OK )
	{
		return ucRet;
	}
		
	// 批上送结束
/*	if( iTotalNum==0 ) // 二代平台对账平也需要批结束报文
	{
		return OK;
	}*/
	
	InitCommData();
	stTemp.iTransNo = POS_BATCH_UP;
	PosCom.stTrans.iTransNo = POS_BATCH_UP;
	PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;
	SetCommReqField((uint8_t *)"0320", (uint8_t *)"");
	sprintf((char *)glSendPack.szField48, "%04d", iTotalNum); 
	if( flag==EMVLOG )
	{
		sprintf((char *)&glSendPack.szField60[8], "207");
	}
	else
	{
		sprintf((char *)&glSendPack.szField60[8], "202");
	}
	
	ucRet = SendRecvPacket();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return OK;
}

uint8_t BatchMagOnLineTrans(int *piTotalNum, uint8_t ucSendFalg)
{
	uint8_t		ucRet, sBuff[30], sField48[512];
	int		iCnt, iLength, iItemNum, k, iField48Len;
	int		aiInsertPoint[8];	//用于置本包上送数据的原交易的上送标志
	NEWPOS_LOG_STRC	stLog;
	int     iMytestAmunt;

	if( ucSendFalg==EMVLOG )
	{
		return OK;
	}
	if( stTransCtrl.iTransNum==0 )
	{
		return E_NO_TRANS;
	}

	for(k=0; k<8; k++)
	{
		aiInsertPoint[k] = 60000;
	}

	iItemNum = 0;		//一个数据包中48域的交易笔数
	for(iCnt=0; iCnt<stTransCtrl.iTransNum; iCnt++)
	{
		memset(&stLog, 0, sizeof(stLog));
		ucRet = LoadTranLog(iCnt, &stLog);
		if( ucRet!=OK )
		{
			return ucRet;
		}
		
		if( stLog.iTransNo==POS_PREAUTH     ||
			stLog.iTransNo==POS_PREAUTH_VOID || stLog.iTransNo==POS_PREAUTH_ADD ||
			stLog.iTransNo==OFF_SALE         || stLog.iTransNo==OFF_ADJUST      ||
			stLog.iTransNo==ADJUST_TIP       || stLog.iTransNo==POS_REFUND      ||
			stLog.iTransNo==POS_OFF_CONFIRM )
		{
			continue;
		}
		
		if (stLog.ucSwipeFlag==CARD_INSERTED )
		{
			if (stLog.iTransNo != POS_SALE_VOID)
			{
				continue;
			}
		}

		if (stLog.ucSwipeFlag & CARD_PASSIVE)
		{
			continue;
		}
		
		if( (ucSendFalg==RMBLOG) && (memcmp(stLog.szCardUnit, "CUP", 3)!=0) )
		{
			continue;	
		}
		if( (ucSendFalg==FRNLOG) && (memcmp(stLog.szCardUnit, "CUP", 3)==0) )
		{
			continue;	
		}

		if( stLog.ucBatchSendFlag==TRUE )
		{
			(*piTotalNum)++;
			printf("type: %d %d\n",stLog.iTransNo, stLog.ucSwipeFlag);
			iMytestAmunt = PubBcd2Long(stLog.sAmount,6,NULL);
			printf("totnum3-1: %d \n",iMytestAmunt);
			printf("card: %s \n",stLog.szCardNo);
			printf("totnum3-1: %d \n",*piTotalNum);
			continue;
		}

		aiInsertPoint[iItemNum] = iCnt; 
		iItemNum++;
		printf("totnum3-2: %d \n",iItemNum);

		iLength = (iItemNum-1)*40 + 2;
		if( memcmp(stLog.szCardUnit, "CUP", 3)==0 )
		{
			memcpy(&sField48[iLength], "00", 2);
		}
		else
		{
			memcpy(&sField48[iLength], "01", 2);
		}
		sprintf((char *)&sField48[iLength+2], "%06d", stLog.lTraceNo);
		memset(&sField48[iLength+8],'0',20);
		sprintf((char *)&sField48[iLength+8+(20-strlen((char *)stLog.szCardNo))], "%s", stLog.szCardNo);
		BcdToAsc0(&sField48[iLength+28], stLog.sAmount,12);
		
		if( iItemNum==8 )
		{
			*piTotalNum += iItemNum;
			printf("totnum3-3: %d %d\n",*piTotalNum,iItemNum);
			iField48Len = iItemNum*40 + 2;
			sprintf((char *)sBuff, "%02d", iItemNum);
			memcpy(sField48, sBuff, 2);
			iItemNum = 0;	 

			stTemp.iTransNo = POS_BATCH_UP;
			PosCom.stTrans.iTransNo = POS_BATCH_UP;
			PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;
			SetCommReqField((uint8_t *)"0320", (uint8_t *)"");
			sprintf((char *)glSendPack.szField48, "%.*s", LEN_FIELD48, sField48);

			ucRet = SendRecvPacket();
			if( ucRet!=OK )
			{
				return ucRet;
			}
			for(k=0; k<8; k++)
			{
				if( aiInsertPoint[k]==60000 )
				{
					break;
				}
				memset(&stLog, 0, sizeof(stLog));
				ucRet = LoadTranLog(aiInsertPoint[k], &stLog);
				if( ucRet!=OK )
				{
					return ucRet;
				}
				stLog.ucBatchSendFlag = TRUE;
				ucRet = UpdateTranLog(aiInsertPoint[k], &stLog);
				if( ucRet!=OK )
				{
					return ucRet;
				}
				aiInsertPoint[k] = 60000;
			}			
		}
	}

	if( iItemNum!=0 )
	{	
		*piTotalNum += iItemNum;
		printf("totnum3-4: %d %d\n",*piTotalNum,iItemNum);
		iField48Len = iItemNum*40 + 2;
		sprintf((char *)sBuff, "%02d", iItemNum);
		memcpy(sField48, sBuff, 2);
		iItemNum = 0;	

		stTemp.iTransNo = POS_BATCH_UP;
		PosCom.stTrans.iTransNo = POS_BATCH_UP;
		PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;
		SetCommReqField((uint8_t *)"0320", (uint8_t *)"");
		sprintf((char *)glSendPack.szField48, "%.*s", LEN_FIELD48, sField48);

		ucRet = SendRecvPacket();
		if( ucRet!=OK )
		{
			return ucRet;
		}

		for(k=0; k<8; k++)
		{
			if( aiInsertPoint[k]==60000 )
			{
				break;
			}
			memset(&stLog, 0, sizeof(stLog));
			ucRet = LoadTranLog(aiInsertPoint[k], &stLog);
			if( ucRet!=OK )
			{
				return ucRet;
			}
			stLog.ucBatchSendFlag = TRUE;
			ucRet = UpdateTranLog(aiInsertPoint[k], &stLog);
			if( ucRet!=OK )
			{
				return ucRet;
			}
			aiInsertPoint[k] = 60000;
		}			
	}

	printf("totnum3-56: %d %d\n",*piTotalNum,iItemNum);
	return OK;

}

uint8_t SendOffLineTrans(uint8_t flag, int maxSendTimes)
{
	uint8_t		ucRet;
	int		iRecNo, iRecIndex;
	NEWPOS_LOG_STRC	stLog;
	uint32_t  uiStatus;
	int       iCurUploadNum = 1;
	char      szBuffer[40];

	for(iRecNo=0; iRecNo<stTransCtrl.iTransNum; iRecNo++)
	{
		if (flag)
		{
			if( kbGetKeyMs(150) == KEY_CANCEL )
			{
				return NO_DISP;
			}
		}
		
		memset(&stLog, 0, sizeof(stLog));
		iRecIndex = GetMinSendTimesTxn(TS_TXN_OFFLINE);
		if (iRecIndex == -1)
		{
			return 0;
		}
		ucRet = LoadTranLog(iRecIndex, &stLog);
		if( ucRet )
		{
			return ucRet;
		}

		if(stLog.ucSendFlag || (stLog.iTransNo!=OFF_SALE && stLog.iTransNo!=OFF_ADJUST && stLog.iTransNo!=ADJUST_TIP))
		{
			uiStatus = stTransCtrl.sTxnRecIndexList[iRecIndex];
			uiStatus &= ~TS_TXN_OFFLINE;
			UpdataTxnCtrlStatus(iRecIndex,uiStatus);
			continue;
		}

		lcdClrLine(2,7);
		DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "正在上送交易    ", "Upload Txn...");
		if (stPosParam.iDispLanguage==1)
		{
			sprintf(szBuffer,"第 %d 笔",iCurUploadNum);
		}
		else
		{
			sprintf(szBuffer,"Upload %d ",iCurUploadNum);
		}
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, szBuffer, szBuffer);
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "请稍候  ", "PLS WAIT....");
		sysDelayMs(500); // 延迟等
		iCurUploadNum++;

		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "已连接银联中心  ", "CONNECT BANK");

		InitCommData();
		stTemp.iTransNo = stLog.iTransNo;
		memcpy(&PosCom.stTrans, &stLog, sizeof(stLog));
		PosCom.ucSwipedFlag = stLog.ucSwipeFlag;
		SetCommReqField((uint8_t *)"0220", (uint8_t *)"000000");
		sprintf((char *)glSendPack.szAuthCode, "%.6s", stLog.szAuthNo);
		sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s%s%s",
				PosCom.stTrans.lOldBatchNumber, PosCom.stTrans.lOldTraceNo,
				PosCom.stTrans.szOldTxnDate, stLog.szAuthMode, stLog.szAuthUnit);
		sprintf((char *)glSendPack.szField63, "%s", stLog.szCardUnit);
		if( stLog.iTransNo==ADJUST_TIP || stLog.iTransNo==OFF_ADJUST || stLog.iTransNo==OFF_SALE )
			sprintf((char *)glSendPack.szRRN, "%.12s", stLog.szSysReferNo);

		PosCom.bOnlineTxn = FALSE;  // 先设置没有联机成功 
		ucRet = SendRecvPacket();
		if( ucRet!=OK || strlen((char*)PosCom.szRespCode)==0 )
		{
			// continue;
			if (PosCom.bOnlineTxn)
			{
				uiStatus = stTransCtrl.sTxnRecIndexList[stLog.uiTxnRecIndex];
				uiStatus++;  // 状态1，即相当余发送次数加1
				UpdataTxnCtrlStatus(stLog.uiTxnRecIndex,uiStatus); 
			}
			stLog.ucSendFail = TS_NOT_RECV;
			ucRet = UpdateTranLog(iRecIndex, &stLog);
			if( ucRet!=OK )
			{
				return ucRet;
			}
		}
		else
		{
			if( memcmp(PosCom.szRespCode, "00", 2)==0 || memcmp(PosCom.szRespCode, "94", 2)==0 )
			{
				uiStatus = stTransCtrl.sTxnRecIndexList[stLog.uiTxnRecIndex];
				uiStatus &= ~(TS_TXN_OFFLINE);
				UpdataTxnCtrlStatus(stLog.uiTxnRecIndex,uiStatus);
				stLog.ucSendFlag = TRUE;
				stLog.ucSendFail = FALSE;
				sprintf((char *)stLog.szSysReferNo, "%.12s", PosCom.stTrans.szSysReferNo);
				if( strlen((char *)PosCom.stTrans.szAuthNo) )  
				{
					strcpy((char *)stLog.szAuthNo, (char *)PosCom.stTrans.szAuthNo);
				}
				ucRet = UpdateTranLog(iRecIndex, &stLog);
				if( ucRet!=OK )
				{
					return ucRet;
				}
			}
			else
			{
				uiStatus = stTransCtrl.sTxnRecIndexList[stLog.uiTxnRecIndex];
				uiStatus |= 0x0F;  // 状态改成最大，不要再发送了
				UpdataTxnCtrlStatus(stLog.uiTxnRecIndex,uiStatus);

				stLog.ucSendFail = TS_NOT_CODE;  // 非00
				ucRet = UpdateTranLog(iRecIndex, &stLog);
				if( ucRet!=OK )
				{
					return ucRet;
				}
			}
		}
	}	//for

	return OK;
}

/*选择卡组织*/
uint8_t SelectCardUnit(uint8_t *szCardUnit)
{
	uint8_t ucKey;

	kbFlush();
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "选择国际组织代码", "   ISSURE CODE  ");
	lcdDisplay(0, 4, DISP_ASCII, "1 CUP 2 VIS 3 MCC");
	lcdDisplay(0, 5, DISP_ASCII, "4 MAE 5 JCB 6 DCC");
	lcdDisplay(0, 6, DISP_ASCII, "7 AEX            ");
	
	while( 1 )
	{
		ucKey = kbGetKeyMs(stPosParam.ucOprtLimitTime*1000);
		if( (ucKey>=KEY1 && ucKey<=KEY7) || ucKey==KEY_ENTER || ucKey==KEY_CANCEL )
		{
			break;
		}
	}
	switch(ucKey) {  //增加了如果为人民币卡，按确认处理 051124
	case KEY1:
	case KEY_ENTER:
		strcpy((char *)szCardUnit, "CUP");
		break;

	case KEY2:
		strcpy((char *)szCardUnit, "VIS");
		break;

	case KEY3:
		strcpy((char *)szCardUnit, "MCC");
		break;

	case KEY4:
		strcpy((char *)szCardUnit, "MAE");
		break;

	case KEY5:
		strcpy((char *)szCardUnit, "JCB");
		break;

	case KEY6:
		strcpy((char *)szCardUnit, "DCC");
		break;

	case KEY7:
		strcpy((char *)szCardUnit, "AEX");
		break;

	case KEY_CANCEL:
		return E_TRANS_CANCEL;

	default:
		break;
	}

	return(OK);
}


/****************************************************************
功能: 所有通知类交易的批上送
****************************************************************/
//20080801
uint8_t BatchSendOffTrans1(int *piTotalNum, uint8_t ucSendFlag)
{
	uint8_t		ucRet;
	int		iCnt;
	NEWPOS_LOG_STRC	stLog;

	if( ucSendFlag==EMVLOG )
	{
		return OK;
	}

	for(iCnt=0; iCnt<stTransCtrl.iTransNum; iCnt++)
	{
		memset(&stLog, 0, sizeof(stLog));
		ucRet = LoadTranLog(iCnt, &stLog);
		if( ucRet!=OK )
		{
			return ucRet;
		}

		if( stLog.iTransNo!=OFF_SALE   && stLog.iTransNo!=OFF_ADJUST &&
		    stLog.iTransNo!=ADJUST_TIP )	
		{
			continue;
		}

		if( (ucSendFlag==RMBLOG) && (memcmp(stLog.szCardUnit, "CUP", 3)!=0) )
		{
			continue;	
		}
		if( (ucSendFlag==FRNLOG) && (memcmp(stLog.szCardUnit, "CUP", 3)==0) )
		{
			continue;	
		}
		if( stLog.ucBatchSendFlag==TRUE )
		{	//
			(*piTotalNum)++;
			continue;
		}

		InitCommData();
		memcpy(&PosCom.stTrans, &stLog, sizeof(stLog));
		switch( stLog.iTransNo ){
		case OFF_SALE:
			stTemp.iTransNo = OFF_SALE_BAT;
			break;

		case OFF_ADJUST:
			stTemp.iTransNo = OFF_ADJUST_BAT;
			break;

		case ADJUST_TIP:
			stTemp.iTransNo = ADJUST_TIP_BAT;
			break;


		default:
			continue;
		}

		PosCom.stTrans.iTransNo = stTemp.iTransNo;
		PosCom.ucSwipedFlag = stLog.ucSwipeFlag; // 使SetCommReqField()设置适当的数据域
		SetCommReqField((uint8_t *)"0320", (uint8_t *)(stLog.iTransNo==POS_REFUND ? "200000" : "000000"));
		if( stTemp.iTransNo==OFF_ADJUST_BAT || stTemp.iTransNo==ADJUST_TIP_BAT )
		{
			sprintf((char *)glSendPack.szRRN, "%.12s", stLog.szSysReferNo);
		}

//20110330 都需要上送授权码
//		if( (stLog.iTransNo==OFF_SALE) && (memcmp(stLog.szAuthMode, "02", 2)==0) )
//		{
//			glSendPack.szAuthCode[0] = 0;
//		}
//		else
//		{
		sprintf((char *)glSendPack.szAuthCode, "%.6s", stLog.szAuthNo);
//		}
		
		sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s%s%s",
				PosCom.stTrans.lOldBatchNumber, PosCom.stTrans.lOldTraceNo,
				PosCom.stTrans.szOldTxnDate, stLog.szAuthMode, stLog.szAuthUnit);
		if( stLog.iTransNo==OFF_SALE || stLog.iTransNo==OFF_ADJUST || stLog.iTransNo==ADJUST_TIP )
		{
			sprintf((char *)glSendPack.szField63, "%3s", stLog.szCardUnit);
			glSendPack.szField60[13] = 0; // 60.6-7不送
		}

		ucRet = SendRecvPacket();
		if( ucRet!=OK )
		{
			return ucRet;
		}

		stLog.ucBatchSendFlag = TRUE;
		ucRet = UpdateTranLog(iCnt, &stLog);
		if( ucRet!=OK )
		{
			return ucRet;
		}
		(*piTotalNum)++;
	}

	return OK;
}

//20080801
uint8_t BatchSendOffTrans2_Refund(int *piTotalNum, uint8_t ucSendFlag)
{
	uint8_t		ucRet;
	int		iCnt;
	NEWPOS_LOG_STRC	stLog;

	if( ucSendFlag==EMVLOG )
	{
		return OK;
	}

	for(iCnt=0; iCnt<stTransCtrl.iTransNum; iCnt++)
	{
		memset(&stLog, 0, sizeof(stLog));
		ucRet = LoadTranLog(iCnt, &stLog);
		if( ucRet!=OK )
		{
			return ucRet;
		}

//		if( stLog.iTransNo!=POS_REFUND &&
//			stLog.iTransNo!=POS_OFF_CONFIRM )
		if( stLog.iTransNo!=POS_REFUND)
		{
			continue;
		}

		if( (ucSendFlag==RMBLOG) && (memcmp(stLog.szCardUnit, "CUP", 3)!=0) )
		{
			continue;	
		}
		if( (ucSendFlag==FRNLOG) && (memcmp(stLog.szCardUnit, "CUP", 3)==0) )
		{
			continue;	
		}
		if( stLog.ucBatchSendFlag==TRUE )
		{	//
			(*piTotalNum)++;
			continue;
		}

		InitCommData();
		memcpy(&PosCom.stTrans, &stLog, sizeof(stLog));
		if( stLog.iTransNo==POS_REFUND || stLog.iTransNo==POS_OFF_CONFIRM )
		{
			CheckRefundDataFile(stLog.lTraceNo);  // 读取退货磁道数据等
		}
		switch( stLog.iTransNo ){

		case POS_REFUND:
			stTemp.iTransNo = POS_REFUND_BAT;
			break;

		case POS_OFF_CONFIRM:
			stTemp.iTransNo = OFF_CONFIRM_BAT;
			break;

		default:
			continue;
		}

		PosCom.stTrans.iTransNo = stTemp.iTransNo;
		PosCom.ucSwipedFlag = stLog.ucSwipeFlag; // 使SetCommReqField()设置适当的数据域
		SetCommReqField((uint8_t *)"0320", (uint8_t *)(stLog.iTransNo==POS_REFUND ? "200000" : "000000"));
		if( stTemp.iTransNo==POS_REFUND_BAT )
		{
			sprintf((char *)glSendPack.szRRN, "%.12s", stLog.szSysReferNo);
		}

// 20110328 说要送
//		glSendPack.szTrack2[0] = 0;	// 规范要求不需要送track2/3
//		glSendPack.szTrack3[0] = 0;

//20080731
		if( (stLog.iTransNo==OFF_SALE) && (memcmp(stLog.szAuthMode, "02", 2)==0) )
		{
			glSendPack.szAuthCode[0] = 0;
		}
		else
		{
			sprintf((char *)glSendPack.szAuthCode, "%.6s", stLog.szAuthNo);
		}
		
		sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s%s%s",
				PosCom.stTrans.lOldBatchNumber, PosCom.stTrans.lOldTraceNo,
				PosCom.stTrans.szOldTxnDate, stLog.szAuthMode, stLog.szAuthUnit);
		strcpy((char *)glSendPack.szField63, stLog.szCardUnit);
		if (!strlen(stLog.szCardUnit))
		{
			strcpy((char *)glSendPack.szField63, "000");
		}
// 		if( stLog.iTransNo==POS_REFUND )
// 		{
// 			strcpy((char *)glSendPack.szField63, "000");
// 		}

		ucRet = SendRecvPacket();
		if( ucRet!=OK )
		{
			return ucRet;
		}

		stLog.ucBatchSendFlag = TRUE;
		ucRet = UpdateTranLog(iCnt, &stLog);
		if( ucRet!=OK )
		{
			return ucRet;
		}
		(*piTotalNum)++;
	}

	return OK;
}


uint8_t BatchSendOffTrans2_CM(int *piTotalNum, uint8_t ucSendFlag)
{
	uint8_t		ucRet;
	int		iCnt;
	NEWPOS_LOG_STRC	stLog;

	if( ucSendFlag==EMVLOG )
	{
		return OK;
	}

	for(iCnt=0; iCnt<stTransCtrl.iTransNum; iCnt++)
	{
		memset(&stLog, 0, sizeof(stLog));
		ucRet = LoadTranLog(iCnt, &stLog);
		if( ucRet!=OK )
		{
			return ucRet;
		}

//		if( stLog.iTransNo!=POS_REFUND &&
//			stLog.iTransNo!=POS_OFF_CONFIRM )	
		if( stLog.iTransNo!=POS_OFF_CONFIRM )
		{
			continue;
		}

		if( (ucSendFlag==RMBLOG) && (memcmp(stLog.szCardUnit, "CUP", 3)!=0) )
		{
			continue;	
		}
		if( (ucSendFlag==FRNLOG) && (memcmp(stLog.szCardUnit, "CUP", 3)==0) )
		{
			continue;	
		}
		if( stLog.ucBatchSendFlag==TRUE )
		{	//
			(*piTotalNum)++;
			continue;
		}

		InitCommData();
		memcpy(&PosCom.stTrans, &stLog, sizeof(stLog));
		if( stLog.iTransNo==POS_REFUND || stLog.iTransNo==POS_OFF_CONFIRM )
		{
			CheckRefundDataFile(stLog.lTraceNo);  // 读取退货磁道数据等
		}
		switch( stLog.iTransNo ){

		case POS_REFUND:
			stTemp.iTransNo = POS_REFUND_BAT;
			break;

		case POS_OFF_CONFIRM:
			stTemp.iTransNo = OFF_CONFIRM_BAT;
			break;

		default:
			continue;
		}

		PosCom.stTrans.iTransNo = stTemp.iTransNo;
		PosCom.ucSwipedFlag = stLog.ucSwipeFlag; // 使SetCommReqField()设置适当的数据域
		SetCommReqField((uint8_t *)"0320", (uint8_t *)(stLog.iTransNo==POS_REFUND ? "200000" : "000000"));
		if( stTemp.iTransNo==POS_REFUND_BAT )
		{
			sprintf((char *)glSendPack.szRRN, "%.12s", stLog.szSysReferNo);
		}

// 20110328 说要送
//		glSendPack.szTrack2[0] = 0;	// 规范要求不需要送track2/3
//		glSendPack.szTrack3[0] = 0;

//20080731
		if( (stLog.iTransNo==OFF_SALE) && (memcmp(stLog.szAuthMode, "02", 2)==0) )
		{
			glSendPack.szAuthCode[0] = 0;
		}
		else
		{
			sprintf((char *)glSendPack.szAuthCode, "%.6s", stLog.szAuthNo);
		}
		
		sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s%s%s",
				PosCom.stTrans.lOldBatchNumber, PosCom.stTrans.lOldTraceNo,
				PosCom.stTrans.szOldTxnDate, stLog.szAuthMode, stLog.szAuthUnit);
// 		if( stLog.iTransNo==POS_REFUND )
// 		{
// 			strcpy((char *)glSendPack.szField63, "000");
// 		}
		if (stLog.iTransNo == POS_OFF_CONFIRM)
		{
			glSendPack.szField60[13] = 0; // 60.6-7不送
		}

		ucRet = SendRecvPacket();
		if( ucRet!=OK )
		{
			return ucRet;
		}

		stLog.ucBatchSendFlag = TRUE;
		ucRet = UpdateTranLog(iCnt, &stLog);
		if( ucRet!=OK )
		{
			return ucRet;
		}
		(*piTotalNum)++;
	}

	return OK;
}

/****************************************************************************
 功能: 输入原交易流水号
****************************************************************************/
uint8_t GetOldTraceNo(void)
{
	uint8_t buf[10];
	int iRet;

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入原凭证号:", "INPUT TRACE:");
//	lcdDisplay(0, 2, DISP_CFONT, "请输入原凭证号:");
	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_BIGFONT+KB_EN_NUM+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 1, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet<0 )
		return (E_TRANS_CANCEL);
	PosCom.stTrans.lOldTraceNo = atol((char *)buf);
	sprintf((char *)PosCom.stTrans.szPrtOldPosSer, "%06ld", atol((char *)buf));	

	return OK;
}

/****************************************************************************
 功能: 输入获取原交易的主机参考号
****************************************************************************/

uint8_t GetSysRefNo(void)
{
	uint8_t	szbuf[13];
	int   iRet;

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入原参考号:", "INPUT REF.NO:");
	lcdGoto(32, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CHAR+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 12, 12, 
		(stPosParam.ucOprtLimitTime*1000), (char *)szbuf);
	if( iRet!=12 )
	{
		return E_TRANS_CANCEL;
	}

	sprintf((char *)PosCom.stTrans.szSysReferNo, "%.12s", szbuf);	
	strcpy((char *)PosCom.stTrans.szPrtOldHostSer, (char *)PosCom.stTrans.szSysReferNo);	
	return OK;
}


uint8_t GetOrignTxnDate(void)
{
	uint8_t buf[6];
	uint8_t chkDate[9];
	int iRet;

	while(1) 
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "请输入原交易日期", "INPUT DATE:");
		DispMulLanguageString(0, 4, DISP_CFONT, "(月月日日):", "(MMDD)");

//		lcdGoto(0, 6);
		lcdGoto(80, 6);
		iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 4, 4, 
			(stPosParam.ucOprtLimitTime*1000), (char *)buf);
		if( iRet!=4 )	
			return (E_TRANS_CANCEL);
		memcpy(chkDate, stTemp.szDate, 4);
		memcpy(chkDate+4, buf, 4);
		if( CheckDate((char *)chkDate)==0 )
		{
			strcpy((char *)PosCom.stTrans.szOldTxnDate, (char *)buf);
			break;
		}
		else
		{
			FailBeep();
			lcdClrLine(4, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT, "日期格式错", "FORMAT DATE");
			kbGetKeyMs(2000);
		}
	}
	
	return OK;
}

uint8_t GetAuthNo(void)
{
	uint8_t buf[10];
	int iRet;
	
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入授权码:", "INPUT AUTH NO:");
	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_BIGFONT+KB_EN_NUM+KB_EN_CHAR+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 6, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet<0 )	
		return (E_TRANS_CANCEL);

	sprintf((char *)PosCom.stTrans.szAuthNo, "%.6s", buf);
	sprintf((char *)PosCom.stTrans.szPrtOldAuthNo, "%s", PosCom.stTrans.szAuthNo);

	return OK;
}


void UpdateParameters(uint8_t *psPosParam, uint16_t iMaxLen)
{
	uint8_t macbuf[9];
	int i, j;
	int bm;
	uint8_t tmpbuf[45];
	uint8_t env_buf[24];

	i = 0;
	do 
	{
		memcpy(macbuf, psPosParam+i, 2);
		macbuf[2] = 0;
		bm = atoi((char *)macbuf);
		i += 2;
		switch(bm)
		{
		case 11:
			i = i+2;
			break;

		case 12:
			memcpy(macbuf, psPosParam+i, 2);
			macbuf[2] = 0;
			stPosParam.ucCommWaitTime = atoi((char *)macbuf); 
			i += 2;
			break;

		case 13:
			stPosParam.ucDialRetryTimes = psPosParam[i] - 0x30;
			sprintf((char*)env_buf, "%d", (int)stPosParam.ucDialRetryTimes);
			i++;
			break;

		case 14:
			memcpy(tmpbuf, psPosParam+i, 14);
			for(j=0; j<14; j++)
			{
				if( tmpbuf[j]==0x20 )
				{
					break;
				}
				stPosParam.szPhoneNum1[j] = tmpbuf[j];
			}					
			stPosParam.szPhoneNum1[j] = 0;
			i += 14;
			break;

		case 15:
			memcpy(tmpbuf, psPosParam+i, 14);
			for(j=0; j<14; j++)
			{
				if( tmpbuf[j]==0x20 )
				{
					break;
				}
				stPosParam.szPhoneNum2[j] = tmpbuf[j];
			}					
			stPosParam.szPhoneNum2[j] = 0;					
			i += 14;
			break;

		case 16:
			memcpy(tmpbuf, psPosParam+i, 14);
			for(j=0; j<14; j++)
			{
				if( tmpbuf[j]==0x20 )
				{
					break;
				}
				stPosParam.szPhoneNum3[j] = tmpbuf[j];
			}					
			stPosParam.szPhoneNum3[j] = 0;
			i += 14;
			break;

		case 17:
			memcpy(tmpbuf, psPosParam+i, 14);
			for(j=0; j<14; j++)
			{
				if( tmpbuf[j]==0x20 )
				{
					break;
				}
				stPosParam.szMPhone[j] = tmpbuf[j];
			}					
			stPosParam.szMPhone[j] = 0;
			i += 14;
			break;

		case 18:
			stPosParam.ucTipOpen = psPosParam[i];
			env_buf[0] = psPosParam[i];
			env_buf[1] = 0;
			i++;
			break;

		case 19:
			memcpy(macbuf, psPosParam+i, 2);
			macbuf[2] = 0;
			stPosParam.ucTipper = atoi((char *)macbuf);
			sprintf((char*)env_buf, "%d", (int)stPosParam.ucTipper);			
			i += 2;
			break;

		case 20:
			stPosParam.ucManualInput = psPosParam[i];
			env_buf[0] = stPosParam.ucManualInput;
			env_buf[1] = 0;
			i++;
			break;

		case 21:
			stPosParam.ucAutoLogoff = psPosParam[i];
			env_buf[0] = stPosParam.ucAutoLogoff;
			env_buf[1] = 0;
			i++;
			break;

		case 22:
			memcpy(tmpbuf, psPosParam+i, 40);
			for(j=0; j<40; j++)
			{
				if( tmpbuf[j]==0x20 )
				{							
					break;
				}
				stPosParam.szUnitChnName[j] = tmpbuf[j];
			}
			stPosParam.szUnitChnName[j] = 0;
			i += 40;
			break;

		case 23:
			stPosParam.ucResendTimes = psPosParam[i] - 0x30;
			sprintf((char*)env_buf, "%d", (int)stPosParam.ucResendTimes);
			i++;
			break;

		case 24:
			stPosParam.ucSendOffFlag = psPosParam[i];
			env_buf[0] = stPosParam.ucSendOffFlag;
			env_buf[1] = 0;
			i++;
			break;

		case 25:
			stPosParam.ucKeyIndex = psPosParam[i] - 0x30;
			sprintf((char*)env_buf, "%d", (int) stPosParam.ucKeyIndex);
			i++;
			break;

		case 26:
			memcpy(stPosParam.sTransOpen, psPosParam+i, 4);
//			stPosParam.sTransOpen[2] = 0;
			BcdToAsc(env_buf, stPosParam.sTransOpen, 8);
			env_buf[8] = 0x00; 
			i += 4;
			break;
		}
	} while( i<(int)iMaxLen );

	SaveAppParam();
	stTransCtrl.ucLoadTask = 0;
	SaveCtrlParam();
}

uint8_t GetAuthType(void)
{
	uint8_t result;
	uint8_t inBuf[17];
	int iKey, iRet;

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "选择授权方式:   ", "   AUTH MODE    ");
	DispMulLanguageString(0, 4, DISP_CFONT, "  1 POS 2 电话  ", "1 POS   2 PHONE");
	DispMulLanguageString(0, 6, DISP_CFONT, "  3 小额代授权  ", "3 DAI-AUTH     ");
	do {
		iKey = kbGetKey();
		if( iKey==KEY_CANCEL )
			return (E_TRANS_CANCEL);
	}while( iKey!=KEY1 && iKey!=KEY2 && iKey!=KEY3 );
	
	if( iKey==KEY1 )
		strcpy((char *)PosCom.stTrans.szAuthMode, "00");
	if( iKey==KEY3 )
	{
		if (stPosParam.ucOfflineSettleFlag == PARAM_CLOSE)
		{
			lcdCls();
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " 暂不支持该交易 ", " NOT SUPPORT ");
			ErrorBeep();
			kbGetKeyMs(2000);					
			return NO_DISP;
		}
		strcpy((char *)PosCom.stTrans.szAuthMode, "02");
	}
	if( iKey==KEY2 )
	{
		strcpy((char *)PosCom.stTrans.szAuthMode, "01");			
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "输入授权机构代码", "AUTH CODE");
		lcdGoto(40, 6);
		iRet = kbGetString(KB_EN_BIGFONT+KB_EN_NUM+KB_EN_CHAR+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 11, 11, 
			(stPosParam.ucOprtLimitTime*1000), (char *)inBuf);
		if( iRet!=11 ) 
			return E_TRANS_CANCEL;
		strcpy((char *)PosCom.stTrans.szAuthUnit, (char *)inBuf);
	}
	if( iKey==KEY1 || iKey==KEY2 )  ////pos和电话授权要输入 051124
	{
		result = GetAuthNo();
		if( result!=OK ) 
			return result;
	}

	return OK;
}

uint8_t GetTxnTableIndex(int txn)
{
	uint8_t	i;

	for(i=0; i<TABLES_LEN; i++)
	{
		if( txn==TrTable[i].iTransNo )
		{
			return i;
		}
	}

	FailBeep();
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "取交易表失败!", "GET TABLE FAIL");
	while(1){};

	return 0xff;
}

void OnlineDownOrUp(void)
{
	uint8_t	ucRet;
	
	switch( stTemp.sPacketHead[5] )
	{
	case '1':
		PosDownLoadParam();
		break;

	case '2':
		PosUpLoadParam();
		break;

	case '3':
		PosOnlineLogon(1);
		break;

	case '4':	// IC卡CAPK下载
		ucRet = DownLoadCAPK(FALSE);
		if( ucRet!=OK )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 3, DISP_CFONT, "IC卡公钥下载失败", "LOAD IC KEY");
			sysDelayMs(1000);
		}
		break;

	case '5':	// IC卡参数下载
		ucRet = DownLoadEmvParam(FALSE);
		if( ucRet!=OK )
		{
			DispMulLanguageString(0, 3, DISP_CFONT, "IC卡参数下载失败", "LOAD IC PARA");
			sysDelayMs(1000);
		}
		break;
		
	case '6':
		stTransCtrl.ucDownAppFlag = 1;
		SaveCtrlParam();
		break;

	case '7':
		ucRet = TransDownloadBlkCard();
		break;

	default:
		break;
	}

	if( stTransCtrl.ucLoadTask==1 )
	{
		PosDownLoadParam();
	}

}

uint8_t NeedPIN(int iTxnNo)
{
	if( iTxnNo==POS_SALE_VOID || iTxnNo == POS_VOID_INSTAL || 
		iTxnNo == POS_VOID_LOYATL_ISSUER || iTxnNo == POS_VOID_LOYATL_ALL||
		iTxnNo == POS_MOBILE_VOID || iTxnNo == POS_RESER_VOID ||
		iTxnNo == POS_MOTO_SALE_VOID)
	{	// 撤销类交易输入密码参数状态
		return (stPosParam.ucVoidPin==PARAM_OPEN);
	}
	if( iTxnNo==POS_PREAUTH_VOID || iTxnNo==POS_MOBILE_VOID_PRE ||
		iTxnNo==POS_MOTO_PREAUTH_VOID)
	{
		return (stPosParam.ucPreVoidPin==PARAM_OPEN);
	}

	if( iTxnNo==POS_AUTH_VOID || iTxnNo==POS_MOBILE_VOID_AUTH 
		|| iTxnNo==POS_MOTO_AUTH_VOID)
	{
		return (stPosParam.ucPreComVoidPin==PARAM_OPEN);
	}

	if( iTxnNo==POS_AUTH_CM || iTxnNo==POS_MOBILE_AUTH 
		|| iTxnNo==POS_MOTO_AUTH_CM)
	{
		return (stPosParam.ucPreComPin==PARAM_OPEN);
	}

	if (iTxnNo >= POS_INSTALLMENT && iTxnNo <= POS_QUE_LOYATL_ALL)
	{
		return TRUE;
	}

	if (iTxnNo == EC_TOPUP_SPEC || iTxnNo == EC_TOPUP_NORAML)
	{
		return TRUE;
	}

	if( PosCom.ucSwipedFlag==CARD_INSERTED && stTemp.iTransNo== POS_QUE)
	{
		if (!PosCom.ucPinEntry)
		{
			return FALSE;
		}
	}

	if( PosCom.ucSwipedFlag==CARD_INSERTED || stTemp.iTransNo==POS_OFF_CONFIRM 
		|| stTemp.iTransNo==POS_MOBILE_AUTH_OFF || stTemp.iTransNo==POS_MOTO_AUTH_OFF )
	{
		return FALSE;
	}

	return TRUE;
}

uint8_t ICNeedPIN(int iTxnNo)
{
	if( iTxnNo==POS_SALE_VOID )
	{	// 撤销类交易输入密码参数状态
		return (stPosParam.ucVoidPin==PARAM_OPEN);
	}
	if( iTxnNo==POS_PREAUTH_VOID )
	{
		return (stPosParam.ucPreVoidPin==PARAM_OPEN);
	}

	if( iTxnNo==POS_AUTH_VOID )
	{
		return (stPosParam.ucPreComVoidPin==PARAM_OPEN);
	}

	if( iTxnNo==POS_AUTH_CM )
	{
		return (stPosParam.ucPreComPin==PARAM_OPEN);
	}

	return TRUE;
}

uint8_t SaleTranGetData(void)
{
	uint8_t ucRet;

	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    消    费    ", "      SALE      ");
	if( stPosParam.ucEmvSupport==PARAM_OPEN )
		ucRet = PosGetCard(CARD_SWIPED|CARD_INSERTED);
	else
		ucRet = PosGetCard(CARD_SWIPED);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucRet = AppGetAmount(9, TRAN_AMOUNT);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucRet = EnterPIN(0);
	if( ucRet!=OK )
	{
		if( ucRet==PED_RET_NOKEY )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 2, DISP_CFONT, "请做联机签到", "LOGON ONLINE");
			kbGetKey();
		}
		return ucRet;
	}

	return OK;
}

uint8_t SaleVoidGetData(void)
{
	uint8_t	ucRet, ucSwipeFlag;
	
	if( stTemp.iTransNo==POS_SALE_VOID )
	{
		if( stPosParam.ucEmvSupport==PARAM_OPEN )	
			ucSwipeFlag = CARD_SWIPED|CARD_INSERTED|0x80;
		else
			ucSwipeFlag = CARD_SWIPED|0x80;
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    消费撤销    ", "  SALE VOID     ");
	}
	else
	{
		ucSwipeFlag = CARD_SWIPED|0x80;
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " 预授权完成撤销 ", "  PRE-CMP VOID  ");
	}

	ucRet = CheckSupPwd();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucRet = GetOldTraceNo();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucRet = CheckData(PosCom.stTrans.lOldTraceNo, NULL);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	lcdClrLine(2, 7);
	lcdFlip();
	if( stTemp.iTransNo==POS_SALE_VOID && PosCom.stTrans.iOldTransNo!=POS_SALE &&
		PosCom.stTrans.iOldTransNo!=ICC_OFFSALE && PosCom.stTrans.iOldTransNo!=QPBOC_ONLINE_SALE
		&& PosCom.stTrans.iOldTransNo!=OFF_SALE )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  原交易非消费  ", "NOT SALE");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "    不能撤销    ", "NOT VOID");
		ErrorBeep();
		kbGetKeyMs(3000);

		return NO_DISP;
	}
	if( stTemp.iTransNo==POS_AUTH_VOID && PosCom.stTrans.iOldTransNo!=POS_AUTH_CM)
//		(PosCom.stTrans.iOldTransNo!=POS_AUTH_CM && PosCom.stTrans.iOldTransNo!=POS_OFF_CONFIRM))
	{//预授权完成通知不能撤销case10806
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "原交易非授权完成", "NOT PREAUTH");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "    不能撤销    ", "NOT VOID");
		ErrorBeep();
		kbGetKeyMs(3000);

		return NO_DISP;
	}

	if( stTemp.iTransNo==POS_AUTH_VOID && PosCom.stTrans.szEntryMode[1] == '5')
	{
		ucSwipeFlag |= CARD_INSERTED;
	}

	if( (stTemp.iTransNo==POS_SALE_VOID && stPosParam.ucVoidSwipe!=PARAM_OPEN) ||
		(stTemp.iTransNo==POS_AUTH_VOID && stPosParam.ucAuthVoidSwipe!=PARAM_OPEN) )
	{
		PosCom.ucSwipedFlag = CARD_KEYIN;
	} 
	else
	{
		ucRet = PosGetCard(ucSwipeFlag);
		if( ucRet!=OK )
		{
			return ucRet;
		}
	}

	if( (PosCom.stTrans.iOldTransNo==ICC_OFFSALE || PosCom.stTrans.iOldTransNo==OFF_SALE) &&
		!stTemp.bSendId )
	{	// 脱机批准的IC卡消费交易(还没有上送过的)
		return OK;
	}
	
	ucRet = EnterPIN(0);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	return OK;
}

uint8_t RefundTranGetData(void)
{
	uint8_t ucRet, ucSwipeMode;

	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    退    货    ", "     REFUND     ");
	ucRet = CheckSupPwd();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	if( stPosParam.ucEmvSupport==PARAM_OPEN )
		ucSwipeMode = CARD_SWIPED|CARD_INSERTED;
	else
		ucSwipeMode = CARD_SWIPED;
	
	ucRet = PosGetCard(ucSwipeMode);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	ucRet = GetSysRefNo();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucRet = GetOrignTxnDate();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
REINPUT:	
	ucRet = AppGetAmount(9, TRAN_AMOUNT);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	if( (stPosParam.lMaxRefundAmt!=0) &&
		(BcdToLong(PosCom.stTrans.sAmount, 6)>stPosParam.lMaxRefundAmt) )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  输入金额无效  ", "INVALID AMOUNT");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "   超退货额度   ", "EXLIMITED");
		ErrorBeep();
		kbGetKeyMs(2000);		
		return NO_DISP;
	}

	if (PosCom.ucSwipedFlag != CARD_INSERTED)
	{
		ucRet = AppConfirmAmount();
		if( ucRet==KEY_CANCEL )
		{
			goto REINPUT;
		}
	}

	if( PosCom.stTrans.szCardUnit[0]==0 )
	{
		strcpy((char *)PosCom.stTrans.szCardUnit, "000");
	}
	PosCom.stTrans.lOldTraceNo = 0;
	PosCom.stTrans.lOldBatchNumber = 0;

	// 根据AID来判断设置一下PosCom.stTrans.szCardUnit 的值是否是CUP
	if (PosCom.ucSwipedFlag == CARD_INSERTED && memcmp(PosCom.stTrans.szAID,STR_PBOC_AID,10) == 0)
	{
		strcpy((char *)PosCom.stTrans.szCardUnit, "CUP");
	}

	return OK;
}

uint8_t AuthComfirmGetData(void)
{
	uint8_t ucRet, ucSwipeMode;

	if( stPosParam.ucEmvSupport==PARAM_OPEN )
		ucSwipeMode = CARD_SWIPED|CARD_INSERTED;
//		ucSwipeMode = CARD_SWIPED|CARD_INSERTED|CARD_PASSIVE;
	else
		ucSwipeMode = CARD_SWIPED;
	if( stPosParam.ucManualInput==PARAM_OPEN )
	{
		ucSwipeMode |= CARD_KEYIN;
	}

	if( stTemp.iTransNo==POS_AUTH_CM )
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "预授权完成(请求)", "PRE CMP(ONLINE)");
//		ucSwipeMode &= ~CARD_INSERTED;	// 不允许插卡
	}
	else
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "预授权完成(通知)", "PRE CMP(OFFLINE)");
		PosCom.stTrans.szOldTxnDate[0] = 0;
	}

	ucRet = PosGetCard(ucSwipeMode);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	ucRet = GetOrignTxnDate();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucRet = GetAuthNo();
	if( ucRet!=OK )
	{
		return ucRet;
	}

REINPUT1:	
	ucRet = AppGetAmount(8, TRAN_AMOUNT);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	if( stTemp.iTransNo!=POS_AUTH_CM )
	{
		if (PosCom.ucSwipedFlag != CARD_INSERTED)
		{
			ucRet = AppConfirmAmount();
			if( ucRet==KEY_CANCEL )
			{
				goto REINPUT1;
			}
		}
	}

	ucRet = EnterPIN(0);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	PosCom.stTrans.lOldBatchNumber = 0;
	PosCom.stTrans.lOldTraceNo = 0;

	return OK;
}

uint8_t PreAuthVoidGetData(void)
{
	uint8_t ucRet, ucSwipeMode;

	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "   预授权撤销   ", "  PREAUTH VOID  ");
	ucRet = CheckSupPwd();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	if( stPosParam.ucEmvSupport==PARAM_OPEN )
		ucSwipeMode = CARD_SWIPED|CARD_INSERTED;
//		ucSwipeMode = CARD_SWIPED|CARD_INSERTED|CARD_PASSIVE;
	else
		ucSwipeMode = CARD_SWIPED;
	if( stPosParam.ucManualInput==PARAM_OPEN )
	{
		ucSwipeMode |= CARD_KEYIN;
	}
	ucRet = PosGetCard(ucSwipeMode);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	strcpy((char *)PosCom.stTrans.szOldTxnDate, "0000");
	ucRet = GetOrignTxnDate();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucRet = GetAuthNo();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucRet = AppGetAmount(8, TRAN_AMOUNT);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucRet = EnterPIN(0);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	PosCom.stTrans.lOldBatchNumber = 0;
	PosCom.stTrans.lOldTraceNo = 0;

	return OK;	
}

uint8_t PreAuthGetData(void)
{
	uint8_t	ucRet;

	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "     预授权     ", "    PREAUTH     ");
	if( stPosParam.ucEmvSupport==PARAM_OPEN ) 
		ucRet = PosGetCard(CARD_SWIPED|CARD_INSERTED);
	else
		ucRet = PosGetCard(CARD_SWIPED);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucRet = AppGetAmount(9, TRAN_AMOUNT);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	ucRet = EnterPIN(0);
	if( ucRet!=OK)
	{
		return ucRet;			
	}
	strcpy((char *)PosCom.stTrans.szAuthMode, "00");

	return OK;
}

uint8_t PreAuthAddInput(void)
{
	uint8_t	ucRet, ucSwipeMode;
	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "   预授权追加   ", "   PREAUTH ADD  ");
	ucSwipeMode = CARD_SWIPED|0x80;		// DO NOT check service code
	ucRet = PosGetCard(ucSwipeMode);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	ucRet = GetAuthNo();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	ucRet = AppGetAmount(8, TRAN_AMOUNT);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	ucRet = EnterPIN(0);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	PosCom.stTrans.lOldBatchNumber = 0;
	PosCom.stTrans.lOldTraceNo = 0;
	return OK;
}

uint8_t OffSaleGetData(void)
{
	uint8_t	ucRet;

	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    离线结算    ", " SETTLE OFFLINE ");
	ucRet = PosGetCard(CARD_KEYIN); 
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	ucRet = GetAuthType(); 
	if( ucRet )
	{
		return ucRet;
	}
	
	ucRet = SelectCardUnit(PosCom.stTrans.szCardUnit);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	ucRet = AppGetAmount(8, TRAN_AMOUNT);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	sprintf((char *)PosCom.stTrans.szOldTxnDate, "0000");
	sprintf((char *)PosCom.stTrans.szEntryMode, "012");	
	PosCom.stTrans.iTransNo = stTemp.iTransNo;

	IncreaseTraceNo();

	return OK;	
}

uint8_t AdjustTranGetData(void)
{
	uint8_t		ucRet;
	NEWPOS_LOG_STRC	stOldLog;

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    结算调整    ", "  SETTLE ADJUST ");
	ucRet = GetOldTraceNo();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	ucRet = CheckData(PosCom.stTrans.lOldTraceNo, &stOldLog);	
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	lcdClrLine(2, 7);
	lcdFlip();	
	if( PosCom.stTrans.iOldTransNo!=OFF_SALE && PosCom.stTrans.iOldTransNo!=POS_SALE &&
		PosCom.stTrans.iOldTransNo!=ICC_OFFSALE )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " 该交易不能调整 ", "   NOT ADJUST    ");
		ErrorBeep();
		kbGetKeyMs(3000);

		return NO_DISP;
	}
	sprintf((char *)PosCom.stTrans.szEntryMode, "012");	/*置成手工输入且无密码*/
	PosCom.stTrans.ucSwipeFlag = CARD_KEYIN;	// 为了批上送时使用
	
	if( PosCom.stTrans.iOldTransNo==OFF_SALE )
	{
		ucRet = AppGetAmount(8, ADJUST_AMOUNT);
		if( ucRet!=OK )
		{
			return ucRet;
		}
		ucRet = CheckAdjustAmt(PosCom.stTrans.sAmount, stOldLog.sAmount, stPosParam.ucAdjustPercent);
		if( ucRet!=OK )
		{
			return ucRet;
		}
	}
	else 
	{
		if( memcmp(PosCom.stTrans.szCardUnit, "CUP", 3)==0 )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " 仅外卡能做小费 ", "ONLY FRN CARD");
			ErrorBeep();
			kbGetKeyMs(3000);

			return NO_DISP;
		}
		if( stPosParam.ucTipOpen!=PARAM_OPEN ) 
		{
			return E_TIP_NO_OPEN;
		}
		stTemp.iTransNo =  ADJUST_TIP;
		PosCom.stTrans.iTransNo = stTemp.iTransNo;
		PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;
		
		ucRet = AppGetAmount(8, TIP_AMOUNT);
		if( ucRet!=OK )
		{
			return ucRet;
		}
	}
	
	if( stOldLog.ucSendFlag==TRUE || stTemp.iTransNo==ADJUST_TIP )
//	if( stOldLog.ucSendFlag==TRUE)
	{
		IncreaseTraceNo();
	}
	PosCom.stTrans.iTransNo = stTemp.iTransNo;

	return OK;
}

uint8_t CheckAdjustAmt(uint8_t *sAmount, uint8_t *sOldAmount, uint8_t ucPercent)
{
	int oldAmt,newAmt;
	int64_t tmp;
	
	if( stPosParam.ucAdjustPercent==0 ) 
		return OK;

	oldAmt = BcdToLong(sOldAmount, 6);
	newAmt = BcdToLong(sAmount, 6);
	
	tmp = oldAmt * stPosParam.ucAdjustPercent; 
	if( newAmt>(tmp/100+oldAmt) )	
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  输入金额无效  ", "INVALID AMOUNT");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "   超调整额度   ", "EXLIMITED");
		ErrorBeep();
		kbGetKeyMs(2000);
		return NO_DISP;
	}

	return OK;
}

uint8_t OnlineSale(void)
{
	uint8_t	ucRet;

	ucRet = SaleTranGetData();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	SetCommReqField((uint8_t *)"0200", (uint8_t *)"000000");
	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		return ucRet;
	}

	return NO_DISP;
}

uint8_t VoidSaleAuth(void)
{
	uint8_t	ucRet;
	
	ucRet = SaleVoidGetData();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	if( (PosCom.stTrans.iOldTransNo==ICC_OFFSALE || PosCom.stTrans.iOldTransNo==OFF_SALE) && 
		!stTemp.bSendId )
	{	// 脱机批准的IC卡消费交易
		sprintf((char *)PosCom.szRespCode, "00");
		return AfterCommProc();
	}
	
	// 设置交易请求数据
	SetCommReqField((uint8_t *)"0200", (uint8_t *)"200000");
	if( PosCom.stTrans.szSysReferNo[0]==0 )
	{
		sprintf((char *)glSendPack.szRRN, "%0*ld", LEN_RRN, 0L);
	}
	else
	{
		sprintf((char *)glSendPack.szRRN, "%.*s", LEN_RRN, PosCom.stTrans.szSysReferNo);
	}
	sprintf((char *)glSendPack.szAuthCode, "%.*s", LEN_AUTH_CODE, PosCom.stTrans.szAuthNo);
	if( stTemp.iTransNo==POS_AUTH_VOID )
	{
		sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s", PosCom.stTrans.lOldBatchNumber,
				PosCom.stTrans.lOldTraceNo, PosCom.stTrans.szOldTxnDate);
	}
	else
	{
		sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d", PosCom.stTrans.lOldBatchNumber,
				PosCom.stTrans.lOldTraceNo);
	}

	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		return ucRet;
	}
		
	return NO_DISP;
}

uint8_t RefundTran(void)
{
	uint8_t	ucRet;
	
	ucRet = RefundTranGetData();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	SetCommReqField((uint8_t *)"0220", (uint8_t *)"200000");
	sprintf((char *)glSendPack.szRRN, "%.*s", LEN_RRN, PosCom.stTrans.szSysReferNo);
	sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s", PosCom.stTrans.lOldBatchNumber,
			PosCom.stTrans.lOldTraceNo, PosCom.stTrans.szOldTxnDate);
	sprintf((char *)glSendPack.szField63, "%.3s", PosCom.stTrans.szCardUnit);
	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		return ucRet;
	}
		
	return NO_DISP;
}

uint8_t PreAuth(void)
{
	uint8_t	ucRet;

	ucRet = PreAuthGetData();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	// 设置交易请求数据
	SetCommReqField((uint8_t *)"0100", (uint8_t *)"030000");
	
	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		return ucRet;
	}
		
	return NO_DISP;
}

uint8_t VoidPreAuth(void)
{
	uint8_t	ucRet;
	
	ucRet = PreAuthVoidGetData();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	SetCommReqField((uint8_t *)"0100", (uint8_t *)"200000");
	sprintf((char *)glSendPack.szAuthCode, "%.6s", PosCom.stTrans.szAuthNo);
	sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s", PosCom.stTrans.lOldBatchNumber,
			PosCom.stTrans.lOldTraceNo, PosCom.stTrans.szOldTxnDate);
	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		return ucRet;
	}
		
	return NO_DISP;
}

uint8_t PreAuthAdd(void)
{
	uint8_t	ucRet;
	
	ucRet = PreAuthAddInput();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	SetCommReqField((uint8_t *)"0100", (uint8_t *)"030000");
	sprintf((char *)glSendPack.szAuthCode, "%.6s", PosCom.stTrans.szAuthNo);
	sprintf((char *)glSendPack.szOrginalMsg, "%016d", 0);	// 16个0
	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		return ucRet;
	}
		
	return NO_DISP;
}

uint8_t OnlineAuthConfirm(void)
{
	uint8_t	ucRet;
	
	ucRet = AuthComfirmGetData();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	// 设置交易请求数据
	SetCommReqField((uint8_t *)"0200", (uint8_t *)"000000");
	sprintf((char *)glSendPack.szAuthCode, "%.*s", LEN_AUTH_CODE, PosCom.stTrans.szAuthNo);
	sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s", PosCom.stTrans.lOldBatchNumber,
			PosCom.stTrans.lOldTraceNo, PosCom.stTrans.szOldTxnDate);
	
	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;
}

uint8_t OfflineAuthConfirm(void)
{
	uint8_t	ucRet;
	
	// 交易数据收集
	ucRet = AuthComfirmGetData();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	// 设置交易请求数据
	SetCommReqField((uint8_t *)"0220", (uint8_t *)"000000");
	sprintf((char *)glSendPack.szAuthCode, "%.*s", LEN_AUTH_CODE, PosCom.stTrans.szAuthNo);
	sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s", PosCom.stTrans.lOldBatchNumber,
			PosCom.stTrans.lOldTraceNo, PosCom.stTrans.szOldTxnDate);
	
	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		return ucRet;
	}
		
	return NO_DISP;
}

uint8_t QueryBalance(void)
{
	uint8_t	ucRet;
	
	// 交易数据收集
	DispMulLanguageString(0, 0, DISP_REVERSE|DISP_MEDIACY|DISP_CFONT, "     查余额     ", "     BALANCE    ");
	if( stPosParam.ucEmvSupport==PARAM_OPEN )
		ucRet = PosGetCard(CARD_SWIPED|CARD_INSERTED);
	else
		ucRet = PosGetCard(CARD_SWIPED);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucRet = EnterPIN(0);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	// 设置交易请求数据
	SetCommReqField((uint8_t *)"0200", (uint8_t *)"310000");
	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		return ucRet;
	}
		
	return NO_DISP;
}

uint8_t OfflineSale(void)
{
	uint8_t	ucRet;
	
	// 交易数据收集
	ucRet = OffSaleGetData();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	strcpy((char *)PosCom.szRespCode, "00"); // 不联机
	ucRet = AfterCommProc();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	return NO_DISP;
}

uint8_t OfflineAdjust(void)
{
	uint8_t	ucRet;
	
	// 交易数据收集
	ucRet = AdjustTranGetData();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 3, DISP_CFONT|DISP_MEDIACY, "调整成功", "  ADJUST SUCC  ");
	sysDelayMs(1000);
	strcpy((char *)PosCom.szRespCode, "00"); //不联机
	ucRet = AfterCommProc();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;
}


uint8_t TestResponse(void)
{	
	uint8_t	ucRet;
	
	if( stPosParam.iDispLanguage==1 )
		ucRet = OneTwoSelect("回响测试", "测试", "取消");
	else
		ucRet = OneTwoSelect("RESPONE", "TEST", "CANCEL");

	if( ucRet!=KEY1 )
	{
		return NO_DISP;
	}

	// 设置交易请求数据
	InitCommData();
	stTemp.iTransNo = RESPONSE_TEST;
	SetCommReqField((uint8_t *)"0820", (uint8_t *)"");
	ucRet = SendRecvPacket();   // 联机通讯过程
	CommHangUp(FALSE);
	DispResult(ucRet);

	return NO_DISP;
}

uint8_t AfterCommProc(void)
{
	uint8_t	ucRet, ucTransOk, ucPrintSlip;
//	int   iRet;
	int   iStanBak;
	uint8_t sPacketHeadBak[7];  //备份

	if( stTemp.iTransNo==POS_SETT )	// 结算响应没有bit 39
	{
		return OK;
	}

	if (stTemp.iTransNo == POS_MAGTOPUP_CASH && memcmp(PosCom.szRespCode, "98", 2)==0 )
	{
		return NO_DISP;
	}

	if (stTemp.iTransNo == POS_MAGTOPUP_CM && memcmp(PosCom.szRespCode, "A6", 2)==0 )
	{
		return NO_DISP;
	}
	
	// 显示交易结果
	if (ChkAcceptTxnCode((char*)PosCom.szRespCode))
	{
		ucTransOk = TRUE; // 交易成功
		lcdClrLine(2, 7);
		lcdFlip();
		if( memcmp((char*)PosCom.szRespCode, "00", 2)==0 )
		{
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "交易成功", "SUCCESS");
		}
		else if( memcmp((char*)PosCom.szRespCode, "10", 2)==0 )
		{
			// 部分扣款处理
			if(stTemp.iTransNo == POS_SALE || stTemp.iTransNo == POS_LOYATL_ALL)
			{
				ucRet = TransPartSale();
				return ucRet;
			}
			else
			{
				dispRetCode();
				ucTransOk = FALSE;
			}
		}
		else
		{
			dispRetCode();
			strcpy((char*)PosCom.szRespCode, "00");
		}
	}
	else
	{
		dispRetCode();
		ucTransOk = FALSE;
	}
	if( stTemp.sPacketHead[5]=='0' )
	{
		CommHangUp(FALSE);
	}

	if( ucTransOk==FALSE )
	{
		OnlineDownOrUp();  // 联机下载或者上送参数等
		return OK;
	}
	
	if( stTemp.iTransNo==POS_PREAUTH_ADD )
	{
		AscToBcd(PosCom.stTrans.sPreAddAmount, &glRecvPack.szExtAmount[2], 12);
	}

	// 交易成功，记录日志
	ucPrintSlip = FALSE;
	if (ChkIfSaveLog())
	{
		if (PosCom.bOnlineTxn &&(stTemp.iTransNo == EC_QUICK_SALE || stTemp.iTransNo == EC_NORMAL_SALE))
		{
			PosCom.stTrans.ucSendFlag = TRUE;
		}
		if (PosCom.bOnlineTxn && stTemp.iTransNo == QPBOC_ONLINE_SALE)
		{
			PosCom.stTrans.ucSendFlag = TRUE;
		}
		ucRet = SaveLogFile();
		if( ucRet!=OK )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  文件操作失败  ", "FILE ERROR");
			ErrorBeep();
			kbGetKeyMs(2000);
		}
		update_limit_total(); // 交易金额统计到限制总帐，以免统计错
		ucPrintSlip = TRUE;	
		
		if( stTemp.iTransNo==OFF_SALE || stTemp.iTransNo == OFF_ADJUST || stTemp.iTransNo == ADJUST_TIP)
		{	// 离线结算
			stTransCtrl.sTxnRecIndexList[PosCom.stTrans.uiTxnRecIndex] = TS_TXN_OFFLINE;
			SaveCtrlParam();
		}
		if (stTemp.iTransNo==ICC_OFFSALE || 
			(stTemp.iTransNo == EC_QUICK_SALE && !PosCom.bOnlineTxn) || 
			(stTemp.iTransNo == EC_NORMAL_SALE && !PosCom.bOnlineTxn) )
		{
			stTransCtrl.sTxnRecIndexList[PosCom.stTrans.uiTxnRecIndex] = TS_ICC_OFFLINE;
			SaveCtrlParam();
		}
	}
	else if( stTemp.iTransNo==OFF_ADJUST && stTemp.bSendId==FALSE )
	{
		PosCom.stTrans.lTraceNo = PosCom.stTrans.lOldTraceNo; // 覆盖原交易使用原流水号
		PosCom.stTrans.iTransNo = OFF_SALE; // 这种情况打印时候打印离线结算
		stTransCtrl.sTxnRecIndexList[PosCom.stTrans.uiTxnRecIndex] = TS_TXN_OFFLINE; 
		SaveCtrlParam();
		ucPrintSlip = TRUE;
	}

	if( ChkIfUpdateLog())
	{
		ucRet = UpdateLogFile();
		if( ucRet!=OK )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  文件操作失败  ", "FILE ERROR");
			ErrorBeep();
			kbGetKeyMs(2000);
		}
	}

	if( stTemp.iTransNo==POS_QUE || stTemp.iTransNo == POS_MOBILE_QUE)
	{
		DispBalance();
	}
	
	if( stTemp.iTransNo==POS_QUE_LOYATL_ALL )
	{
		DisplayLoyaltBalance();
	}

	// 打印单据等
	if( ucPrintSlip==TRUE )
	{
		PrtTranTicket(0);
	}

	// 打印后再离线上送
	// 离线之前先备份状态
	memcpy(sPacketHeadBak,stTemp.sPacketHead,sizeof(stTemp.sPacketHead));
	if (stTemp.iTransNo !=POS_LOGON && stTemp.iTransNo !=POS_LOGOFF)
	{
		if (PosCom.ucSwipedFlag == CARD_INSERTED)
		{
			DispRemoveICC();
		}

		kbFlush();
		iStanBak = PosCom.stTrans.lTraceNo; // 先备份，再回复
		if (PosCom.bOnlineTxn || GetOfflineTxnNums()>=stPosParam.ucMaxOfflineTxn)
		{
			while(GetOfflineTxnNums())
			{
				ucRet = TrickleFeedOffLineTxn(TRUE);
				if (ucRet != 0)
				{
					break;
				}
				ucRet = TrickleFeedICOffLineTxn(TRUE);
				if (ucRet != 0)
				{
					break;
				}

				if( kbGetKeyMs(150) == KEY_CANCEL )
				{
					break;
				}
			}

			kbFlush(); // 清除按键
		}		
		PosCom.stTrans.lTraceNo = iStanBak;
	}
	// 离线后进行回复
	memcpy(stTemp.sPacketHead,sPacketHeadBak,sizeof(stTemp.sPacketHead));
	// 联机下载或者上送参数等
	OnlineDownOrUp(); 

	return OK;
}

void SetCommReqField(uint8_t *pszMsgCode, uint8_t *pszProcCode)
{
	uint8_t	ucIndex, sBuff[200];
	uint32_t uiDatalen, uiAmount;
	char     *psTemp;

	// set common bits
	memset(&glSendPack, 0, sizeof(STISO8583));
	sprintf((char *)glSendPack.szMsgCode,    "%.*s", LEN_MSG_CODE,  pszMsgCode);
	sprintf((char *)glSendPack.szProcCode,   "%.*s", LEN_PROC_CODE, pszProcCode);
	sprintf((char *)glSendPack.szTermID,     "%.*s", LEN_TERM_ID,   PosCom.stTrans.szPosId);
	sprintf((char *)glSendPack.szMerchantID, "%.*s", LEN_MERCHANT_ID, PosCom.szUnitNum);

	// set bit 11
	if( stTemp.iTransNo!=RESPONSE_TEST      && stTemp.iTransNo!=POS_DOWNLOAD_PARAM &&
		stTemp.iTransNo!=POS_UPLOAD_PARAM   && stTemp.iTransNo!=QUERY_EMV_CAPK     &&
		stTemp.iTransNo!=QUERY_EMV_PARAM    && stTemp.iTransNo!=DOWNLOAD_EMV_CAPK  &&
		stTemp.iTransNo!=DOWNLOAD_EMV_PARAM && stTemp.iTransNo!=END_EMV_CAPK       &&
		stTemp.iTransNo!=END_EMV_PARAM && stTemp.iTransNo != POS_LYT_LOGON &&
		stTemp.iTransNo!=END_DOWNLOADBLK)    
	{
		sprintf((char *)glSendPack.szSTAN, "%0*d", LEN_POS_STAN,  PosCom.stTrans.lTraceNo);
	}

	// set bit 60
	SetCommField60();
	if( stTemp.iTransNo==POS_LOGON && stPosParam.ucKeyMode==KEY_MODE_3DES )
	{
		return;
	}
/*	if( stTemp.iTransNo==POS_LOGON && stPosParam.ucKeyMode==KEY_MODE_3DES )
	{	// format: 00 + batch no(6) + 003
//		sprintf((char *)glSendPack.szField60, "00%06d003", PosCom.stTrans.lBatchNumber);
		sprintf((char *)glSendPack.szField60, "00%06d004", PosCom.stTrans.lBatchNumber);
		return;
	}
	else
	{
		ucIndex = GetTxnTableIndex(stTemp.iTransNo);
		if( PosCom.ucSwipedFlag==FALLBACK_SWIPED )
		{
			if (stTemp.iTransNo == POS_LOYATL_ALL)
			{
				sprintf((char *)glSendPack.szField60, "%s%06d%s52065", TrTable[ucIndex].szMessTypeCode,
					PosCom.stTrans.lBatchNumber, TrTable[ucIndex].szNetManInfoCode);
			}
			else if (stTemp.iTransNo == POS_LOYATL_ISSUER)
			{
				sprintf((char *)glSendPack.szField60, "%s%06d%s52048", TrTable[ucIndex].szMessTypeCode,
					PosCom.stTrans.lBatchNumber, TrTable[ucIndex].szNetManInfoCode);
			}
			else
			{
				sprintf((char *)glSendPack.szField60, "%s%06d%s52", TrTable[ucIndex].szMessTypeCode,
					PosCom.stTrans.lBatchNumber, TrTable[ucIndex].szNetManInfoCode);
			}
		}
		else
		{
//20080730 	Pboc应该要加上去
#ifdef PBOC_VERSION
			if (stTemp.iTransNo == POS_LOYATL_ALL)
			{
				sprintf((char *)glSendPack.szField60, "%s%06d%s500065", TrTable[ucIndex].szMessTypeCode,
					PosCom.stTrans.lBatchNumber, TrTable[ucIndex].szNetManInfoCode);
			}
			else if (stTemp.iTransNo == POS_LOYATL_ISSUER)
			{
				sprintf((char *)glSendPack.szField60, "%s%06d%s500048", TrTable[ucIndex].szMessTypeCode,
					PosCom.stTrans.lBatchNumber, TrTable[ucIndex].szNetManInfoCode);
			}
			else
			{
				sprintf((char *)glSendPack.szField60, "%s%06d%s500", TrTable[ucIndex].szMessTypeCode,
					PosCom.stTrans.lBatchNumber, TrTable[ucIndex].szNetManInfoCode);
			}
#else
			sprintf((char *)glSendPack.szField60, "%s%06d", TrTable[ucIndex].szMessTypeCode,
					PosCom.stTrans.lBatchNumber);
#endif
		}
	} */
	if( stTemp.iTransNo==ICC_BATCHUP || stTemp.iTransNo==ICC_FAIL_BATCHUP)
	{
		return;
	}
	
	ucIndex = GetTxnTableIndex(stTemp.iTransNo);
	// 部分管理类交易只存在部分关键数据域
	if( stTemp.iTransNo==POS_LOGON          || stTemp.iTransNo==POS_LOGOFF         ||
		stTemp.iTransNo==RESPONSE_TEST      || stTemp.iTransNo==POS_DOWNLOAD_PARAM ||
		stTemp.iTransNo==POS_UPLOAD_PARAM   || stTemp.iTransNo==QUERY_EMV_CAPK     ||
		stTemp.iTransNo==QUERY_EMV_PARAM    || stTemp.iTransNo==DOWNLOAD_EMV_CAPK  ||
		stTemp.iTransNo==DOWNLOAD_EMV_PARAM || stTemp.iTransNo==END_EMV_CAPK       ||
		stTemp.iTransNo==END_EMV_PARAM      || stTemp.iTransNo==POS_SETT           ||
		stTemp.iTransNo==POS_BATCH_UP       || stTemp.iTransNo==END_DOWNLOADBLK)     
	{	// 重新设置bit 60 (删除60.4和60.5)
		sprintf((char *)glSendPack.szField60, "%s%06d%s", TrTable[ucIndex].szMessTypeCode,
				stPosParam.lNowBatchNum, TrTable[ucIndex].szNetManInfoCode);
		return;
	}

	// set bit 4, txn amount
	if( PosCom.stTrans.iTransNo==ADJUST_TIP || PosCom.stTrans.iTransNo==ADJUST_TIP_BAT )
	{
		memcpy(sBuff, PosCom.stTrans.sAmount, 6);
		BcdAdd(sBuff, PosCom.stTrans.sTipAmount, 6);
		BcdToAsc0(glSendPack.szTranAmt, sBuff, 12);
	}
	else if( !AmountIsZero(PosCom.stTrans.sAmount, 6) )
	{
		BcdToAsc0(glSendPack.szTranAmt, PosCom.stTrans.sAmount, 12);
	}

	// set entry mode(bit 22) & track 2/3
	sprintf((char *)PosCom.stTrans.szEntryMode, "000");
	switch( PosCom.ucSwipedFlag ){
	case CARD_KEYIN:
		memcpy(PosCom.stTrans.szEntryMode, "01", 2);
		sprintf((char *)glSendPack.szPan,     "%.*s", LEN_PAN,      PosCom.stTrans.szCardNo);
		sprintf((char *)glSendPack.szExpDate, "%.*s", LEN_EXP_DATE, PosCom.stTrans.szExpDate);
		break;
		
	case CARD_SWIPED:
		memcpy(PosCom.stTrans.szEntryMode, "02", 2);
//		sprintf((char *)glSendPack.szTrack2, "%.*s", LEN_TRACK2, PosCom.szTrack2);
//		sprintf((char *)glSendPack.szTrack3, "%.*s", LEN_TRACK3, PosCom.szTrack3);
		EncrypteFieldTrac23(PosCom.szTrack2,glSendPack.szTrack2);
		EncrypteFieldTrac23(PosCom.szTrack3,glSendPack.szTrack3);
		break;
		
	case CARD_INSERTED:
		memcpy(PosCom.stTrans.szEntryMode, "05", 2);
		if (PosCom.stTrans.iTransNo == EC_TOPUP_NORAML)
		{
			EncrypteFieldTrac23(PosCom.szTrack2,glSendPack.szTrack2);
			EncrypteFieldTrac23(PosCom.szTrack3,glSendPack.szTrack3);
			sprintf((char *)glSendPack.szPan,      "%.*s", LEN_PAN,    PosCom.stTrans.szTranferCardNo);
			if( PosCom.stTrans.bPanSeqNoOk )	// ADVT case 43 
			{	// TIP interoper.04-T02-SC01 
				sprintf((char *)glSendPack.szPanSeqNo, "%0*X", LEN_PAN_SEQ_NO, PosCom.stTrans.ucPanSeqNo);
			}
		}
		else
		{
	//		sprintf((char *)glSendPack.szTrack2,   "%.*s", LEN_TRACK2, PosCom.szTrack2);
			if (PosCom.stTrans.iTransNo != EC_TOPUP_SPEC) // 电子现金指定账户圈存不需要35
			{
				EncrypteFieldTrac23(PosCom.szTrack2,glSendPack.szTrack2);
			}

			sprintf((char *)glSendPack.szPan,      "%.*s", LEN_PAN,    PosCom.stTrans.szCardNo);
			sprintf((char *)glSendPack.szExpDate,  "%.*s", LEN_EXP_DATE,   PosCom.stTrans.szExpDate);
			if( PosCom.stTrans.bPanSeqNoOk )	// ADVT case 43 
			{	// TIP interoper.04-T02-SC01 
				sprintf((char *)glSendPack.szPanSeqNo, "%0*X", LEN_PAN_SEQ_NO, PosCom.stTrans.ucPanSeqNo);
			}
		}
		break;
		
	case FALLBACK_SWIPED:
		// memcpy(PosCom.stTrans.szEntryMode, "95", 2);
		memcpy(PosCom.stTrans.szEntryMode, "02", 2);
//		sprintf((char *)glSendPack.szTrack2, "%.*s", LEN_TRACK2, PosCom.szTrack2);
//		sprintf((char *)glSendPack.szTrack3, "%.*s", LEN_TRACK3, PosCom.szTrack3);
		EncrypteFieldTrac23(PosCom.szTrack2,glSendPack.szTrack2);
		EncrypteFieldTrac23(PosCom.szTrack3,glSendPack.szTrack3);
		PosCom.stTrans.ucFallBack = TRUE;
		break;

	case CARD_PASSIVE:
		memcpy(PosCom.stTrans.szEntryMode, "07", 2);
		if (PosCom.stTrans.iTransNo>=POS_MOBILE_SALE && PosCom.stTrans.iTransNo<=POS_MOBILE_QUE)
		{
			memcpy(PosCom.stTrans.szEntryMode, "96", 2);
			sprintf((char *)glSendPack.szPan,     "%.*s", LEN_PAN,      PosCom.stTrans.szCardNo);
			sprintf((char *)glSendPack.szTrack2, "%.*s", LEN_TRACK2, PosCom.szTrack2);
			sprintf((char *)glSendPack.szTrack3, "%.*s", LEN_TRACK3, PosCom.szTrack3);
		}
		else
		{
			sprintf((char *)glSendPack.szPan,      "%.*s", LEN_PAN,    PosCom.stTrans.szCardNo);
			EncrypteFieldTrac23(PosCom.szTrack2,glSendPack.szTrack2);
			EncrypteFieldTrac23(PosCom.szTrack3,glSendPack.szTrack3);
			if( PosCom.stTrans.bPanSeqNoOk )	// ADVT case 43 
			{	// TIP interoper.04-T02-SC01 
				sprintf((char *)glSendPack.szPanSeqNo, "%0*X", LEN_PAN_SEQ_NO, PosCom.stTrans.ucPanSeqNo);
			}
		}
		break;
	}
	UpdateEntryModeByPIN();	// 处理联机PIN输入

	if( stTemp.iTransNo == EC_TOPUP_NORAML && PosCom.stTrans.ucEcTopOldSwipeFlag == CARD_SWIPED) //电子现金圈存根据原转出卡的标识
	{
		glSendPack.szEntryMode[1] = '2';
	}

	if( stTemp.iTransNo == POS_LYT_LOGON)
	{
		sprintf((char *)glSendPack.szField60, "%s%06d%s", TrTable[ucIndex].szMessTypeCode,
			stPosParam.lNowBatchNum, TrTable[ucIndex].szNetManInfoCode);
		return ;
	}

	// set bit 25 & 49
	sprintf((char *)glSendPack.szSvrCode, "%s", TrTable[ucIndex].szSevPointCode);
	sprintf((char *)glSendPack.szCurrencyCode, "156");
	
	// set bit 48, tip amount
	if( PosCom.stTrans.iTransNo==OFF_SALE       || PosCom.stTrans.iTransNo==OFF_ADJUST   ||
		PosCom.stTrans.iTransNo==ADJUST_TIP     || PosCom.stTrans.iTransNo==OFF_SALE_BAT ||
		PosCom.stTrans.iTransNo==OFF_ADJUST_BAT || PosCom.stTrans.iTransNo==ADJUST_TIP_BAT )
	{
		// 处理离线交易的小费
		if( !AmountIsZero(PosCom.stTrans.sTipAmount, 6) )
		{
			BcdToAsc0(glSendPack.szField48, PosCom.stTrans.sTipAmount, 12);
		}
	}

	// 手机芯片卡类交易设置55域
	if (PosCom.stTrans.iTransNo>=POS_MOBILE_SALE && PosCom.stTrans.iTransNo<=POS_MOBILE_QUE)
	{
		uiDatalen = 0;
		psTemp = (char *)&PosCom.stTrans.sIccData[0];
		memcpy(psTemp,"\xDF\x32",2);
		uiDatalen += 2;
		psTemp+=2;
		PubLong2Char(strlen((char *)PosCom.stTrans.szAID),1,psTemp);
		psTemp+=1;
		uiDatalen += 1;
		strcpy(psTemp,(char *)PosCom.stTrans.szAID);
		uiDatalen += strlen((char *)PosCom.stTrans.szAID);
		psTemp+=strlen((char *)PosCom.stTrans.szAID);

		memcpy(psTemp,"\xDF\x33\x08",3);
		uiDatalen += 3;
		psTemp+=3;
		memcpy(psTemp,PosCom.stTrans.sAppCrypto,8);
		uiDatalen += 8;
		psTemp+=8;

		memcpy(psTemp,"\xDF\x34\x07",3);
		uiDatalen += 3;
		psTemp+=3;
		memcpy(psTemp,PosCom.stTrans.szIssureID,7);
		uiDatalen += 7;
		psTemp+=7;

		PosCom.stTrans.iIccDataLen = uiDatalen;		
		ShortToByte(glSendPack.sICCData, PosCom.stTrans.iIccDataLen);
		memcpy(&glSendPack.sICCData[2], PosCom.stTrans.sIccData, PosCom.stTrans.iIccDataLen);
	}

	// 设置58域
	if (stTemp.iTransNo == ED_TOPUP_CASH || stTemp.iTransNo == ED_TOPUP_NORAML 
		|| stTemp.iTransNo == ED_TOPUP_NORAML)
	{
		uiDatalen = 2;
		memcpy(glSendPack.szEPData+uiDatalen,"RQ",2);
		uiDatalen += 2;
		memcpy(glSendPack.szEPData+uiDatalen,PosCom.stTrans.szIssureID,8);
		uiDatalen += 8;
		PubAsc2Bcd((char *)PosCom.stTrans.szCardNo,20,(char *)glSendPack.szEPData+uiDatalen);
		uiDatalen += 10;
		memcpy(glSendPack.szEPData+uiDatalen,PosCom.stTrans.sICRandom,4);
		uiDatalen += 4;
		glSendPack.szEPData[uiDatalen] = PosCom.stTrans.ucKeyIndex;
		uiDatalen += 1;
		glSendPack.szEPData[uiDatalen] = PosCom.stTrans.ucKeyVersion;
		uiDatalen += 1;
		memcpy(glSendPack.szEPData+uiDatalen,PosCom.stTrans.sIcTxnSeq,2);
		uiDatalen += 2;
		memcpy(glSendPack.szEPData+uiDatalen,PosCom.stTrans.sBalanceAmount,4);
		uiDatalen += 4;
		uiAmount = PubBcd2Long((uint8_t *)PosCom.stTrans.sAmount,6,NULL);
		PubLong2Char(uiAmount,4,(char *)glSendPack.szEPData+uiDatalen);
		uiDatalen += 4;
		glSendPack.szEPData[uiDatalen] = PosCom.stTrans.ucIcTransType;
		uiDatalen += 1;
		memcpy(glSendPack.szEPData+uiDatalen,PosCom.stTrans.sPsamTid,6);
		uiDatalen += 6;
		memcpy(glSendPack.szEPData+uiDatalen,PosCom.stTrans.sMac1,4);
		uiDatalen += 4;	
		PubLong2Char(uiDatalen-2,2,(char *)glSendPack.szEPData);
	}

	if (PosCom.stTrans.iTransNo == EC_TOPUP_NORAML)
	{
		strcpy((char*)glSendPack.szField48,"0520");
/*		if (PosCom.ucPinEntry)// 转入卡没有输入密码
		{
			glSendPack.szField48[2] = HAVE_INPUT_PIN;
		}*/
		
		ShortToByte(glSendPack.sField62, strlen((char*)PosCom.stTrans.szCardNo));	// 二进制的域,头2个字节存放数据长度
		strcpy((char *)&glSendPack.sField62[2],(char*)PosCom.stTrans.szCardNo);
	}

	// set length bytes of bit 64 for iPack8583()
	if( HaveMacData(stTemp.iTransNo) )
	{	// 二进制的域,头2个字节存放数据长度
		ShortToByte(glSendPack.sMacData, LEN_MAC_DATA);
	}
}

uint8_t DownLoadCAPK(uint8_t ucNeedBak)
{
	uint8_t		ucRet, szProcInfo[16+1];
	int		iCnt;
	struct issuer_ca_public_key	stNewCapkList[MAX_KEY_NUM];
	CapkInfo	stCapkInfoList[MAX_KEY_NUM];

	if (ucNeedBak)
	{
		PosCom.stTrans.iTransNo = stTemp.iTransNo;
		memcpy(&PosCom.stOrgPack, &glSendPack, sizeof(glSendPack));	// 保存请求报文
		memcpy(&BackPosCom, &PosCom, COM_SIZE);
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_CFONT|DISP_REVERSE, "  IC卡公钥下载  ", "   LOAD IC KEY  ");

	if( stPosParam.ucEmvSupport!=PARAM_OPEN )
	{
		stTransCtrl.bEmvCapkLoaded = TRUE;
		SaveCtrlParam();
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_MEDIACY|DISP_CFONT, "无需下载IC卡公钥", "NOT IC KEY");
		kbGetKeyMs(2000);
		return NO_DISP;
	}

	// 初始化全局临时变量
	iCapkNum        = 0;
	pstCapkInfoList = &stCapkInfoList[0];
	memset(&stCapkInfoList[0], 0, sizeof(stCapkInfoList));
	memset(&stNewCapkList[0],  0, sizeof(stNewCapkList));
	
	// 查询是否有CAPK下载
	ucRet = QueryEmvCapk();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	emv_delete_all_ca_public_keys();

	// 更新EMV内核中的CAPK信息(删除查询信息中不存在的CAPK & 更新有效期)
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_MEDIACY|DISP_CFONT, "已连接银联中心  ", "CONNECT BANK");
	ucRet = UpdateCoreCapkInfo();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	stTransCtrl.bEmvCapkLoaded = FALSE;
	SaveCtrlParam();

	// 下载CAPK
	for(iCnt=0; iCnt<iCapkNum; iCnt++)
	{
		sprintf((char *)szProcInfo, "%d/%d", iCnt+1, iCapkNum);
		lcdDisplay(96, 3, DISP_ASCII, "%5.5s", szProcInfo);

		if( stCapkInfoList[iCnt].bNeedDownLoad==FALSE )
		{	// 忽略掉终端已经存在的CAPK
			continue;
		}

		InitCommData();
		stTemp.iTransNo = DOWNLOAD_EMV_CAPK;
		SetCommReqField((uint8_t *)"0800", (uint8_t *)"");
		ShortToByte(glSendPack.sField62, (uint16_t)(3+5+3+1));	// 二进制的域,头2个字节存放数据长度
		memcpy(&glSendPack.sField62[2],     "\x9F\x06\x05", 3);
		memcpy(&glSendPack.sField62[2+3],   stCapkInfoList[iCnt].sRid, 5);
		memcpy(&glSendPack.sField62[2+3+5], "\x9F\x22\x01", 3);
		glSendPack.sField62[2+3+5+3] = stCapkInfoList[iCnt].ucKeyID;

		ucRet = SendRecvPacket();
		if( ucRet!=OK )
		{
			return ucRet;
		}

		if( memcmp(glRecvPack.szRspCode, "00", 2)!=0 || glRecvPack.sField62[2]!='1' )
		{
			continue;	
		}

		// 解析CAPK参数
		pstCurCapk = &stNewCapkList[iCnt];
		
		ucRet = UnpackTlvData(SaveEmvCapkItem);
		if( ucRet!=OK )
		{
			return ucRet;
		}
		sysBeef(3, 100);
	}

	ucRet = SaveNewEmvCapk(&stNewCapkList[0]);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	stTransCtrl.bEmvCapkLoaded = TRUE;
	SaveCtrlParam();

	ucRet = FinishDownLoad(END_EMV_CAPK);

	if (ucNeedBak)
	{
		memcpy(&PosCom, &BackPosCom, COM_SIZE);
		memcpy(&glSendPack, &PosCom.stOrgPack, sizeof(glSendPack));	// 恢复请求报文
		stTemp.iTransNo = PosCom.stTrans.iTransNo;
		PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;
		sprintf((char *)glSendPack.szSTAN, "%0*d", LEN_POS_STAN,  PosCom.stTrans.lTraceNo);
	}

	return ucRet;
}

uint8_t DownLoadEmvParam(uint8_t ucNeedBak)
{
	int			iCnt;
	uint8_t			ucRet, szProcInfo[16+1];
	struct terminal_aid_info stAppList[MAX_APP_NUM];
	TermAidList		stTermAidList[MAX_APP_NUM];

	if (ucNeedBak)
	{
		PosCom.stTrans.iTransNo = stTemp.iTransNo;
		memcpy(&PosCom.stOrgPack, &glSendPack, sizeof(glSendPack));	// 保存请求报文
		memcpy(&BackPosCom, &PosCom, COM_SIZE);
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_CFONT|DISP_REVERSE, "  EMV参数下载   ", " LOAD EMV PARA  ");

	if( stPosParam.ucEmvSupport!=PARAM_OPEN )
	{
		stTransCtrl.bEmvAppLoaded = TRUE;
		SaveCtrlParam();
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_MEDIACY|DISP_CFONT, "无需下载IC卡参数", "NO IC PARA");
		kbGetKeyMs(2000);
		return NO_DISP;
	}

	// 初始化全局临时变量
	iAidNum    = 0;
	pstAppList = &stAppList[0];
	memset(&stAppList[0], 0, sizeof(stAppList));
	memset(&stTermAidList[0], 0, sizeof(stTermAidList));

	// 查询是否有EMV参数下载
	ucRet = QueryEmvParam();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	stTransCtrl.bEmvAppLoaded = FALSE;
	SaveCtrlParam();

//20080804 下载前先清除AID
	emv_delete_all_terminal_aids();

	// 下载EMV参数
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "已连接银联中心  ", "CONNECT BANK");
	for(iCnt=0; iCnt<iAidNum; iCnt++)
	{
		sprintf((char *)szProcInfo, "%d/%d", iCnt+1, iAidNum);
		lcdDisplay(96, 3, DISP_ASCII, "%5.5s", szProcInfo);
		
		InitCommData();
		stTemp.iTransNo = DOWNLOAD_EMV_PARAM;
		SetCommReqField((uint8_t *)"0800", (uint8_t *)"");
		ShortToByte(glSendPack.sField62, (uint16_t)(stAppList[iCnt].aid_len+2+1));	// 二进制的域,头2个字节存放数据长度
		memcpy(&glSendPack.sField62[2], "\x9F\x06", 2);
		glSendPack.sField62[2+2] = stAppList[iCnt].aid_len;
		memcpy(&glSendPack.sField62[2+2+1], stAppList[iCnt].aid, stAppList[iCnt].aid_len);

		ucRet = SendRecvPacket();
		if( ucRet!=OK )
		{
			return ucRet;
		}
		if( memcmp(glRecvPack.szRspCode, "00", 2)!=0 || glRecvPack.sField62[2]!='1' )
		{	// POSP返回失败或者AID列表存在,但是下载时候没有参数,则交易失败
			return E_TRANS_FAIL;
		}

		// 解析EMV应用参数
		pstCurApp     = &stAppList[iCnt];
		pstCurTermAid = &stTermAidList[iCnt];
		ucRet = UnpackTlvData(SaveAppParamItem);
		if( ucRet!=OK )
		{
			return ucRet;
		}

		// 同步到本地应用列表
		memcpy(pstCurTermAid->sAID, pstCurApp->aid, pstCurApp->aid_len);
		pstCurTermAid->ucAidLen = pstCurApp->aid_len;
		sysBeef(3, 100);
	}

	// 保存EMV参数到EMV内核
	ucRet = SaveEmvAppParam(&stTermAidList[0]);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucRet =  FinishDownLoad(END_EMV_PARAM);

	if (ucNeedBak)
	{
		memcpy(&PosCom, &BackPosCom, COM_SIZE);
		memcpy(&glSendPack, &PosCom.stOrgPack, sizeof(glSendPack));	// 恢复请求报文
		stTemp.iTransNo = PosCom.stTrans.iTransNo;
		PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;
		sprintf((char *)glSendPack.szSTAN, "%0*d", LEN_POS_STAN,  PosCom.stTrans.lTraceNo);
	}

	return ucRet;
}

// 查询EMV应用参数信息
uint8_t QueryEmvParam(void)
{
	uint8_t	ucRet;

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "查询中,稍候", "QUERYING,WAIT");
	while(1)
	{
		InitCommData();
		stTemp.iTransNo = QUERY_EMV_PARAM;
		SetCommReqField((uint8_t *)"0820", (uint8_t *)"");
		ShortToByte(glSendPack.sField62, 3);		// 二进制的域,头2个字节存放数据长度
		sprintf((char *)&glSendPack.sField62[2], "1%02d", iAidNum);
		
		ucRet = SendRecvPacket();
		if( ucRet!=OK )
		{
			return ucRet;
		}
		if( memcmp(glRecvPack.szRspCode, "00", 2)!=0 )
		{
			return E_TRANS_FAIL;
		}

		// 解析EMV应用ID列表
		ucRet = UnpackTlvData(SaveOneAid);
		if( ucRet!=OK )
		{
			return ucRet;
		}

		if( glRecvPack.sField62[2]!='2' )
		{
			break;
		}
	}
	if( iAidNum==0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 3, DISP_CFONT, "无参数下载", "NO PARAM");
		kbGetKeyMs(2000);
		stTransCtrl.bEmvAppLoaded = TRUE;
		SaveCtrlParam();
		return NO_DISP;
	}

	return OK;
}

// 解析TLV参数
uint8_t UnpackTlvData(SaveTLVData pfSaveData)
{
	int	iRet, iTotalLen;
	uint8_t	*psTemp, *psTLVData;
	
	iTotalLen = ByteToShort(glRecvPack.sField62) - 1;
	psTLVData = &glRecvPack.sField62[2+1];	// bit 62: len(2) + control(1) + TLV Data(n)

	for(psTemp=psTLVData; psTemp<psTLVData+iTotalLen; )
	{
		iRet = GetTlvValue(&psTemp, psTLVData+iTotalLen-psTemp, pfSaveData, FALSE);
		if( iRet<0 )
		{
			return E_TRANS_FAIL;
		}
	}
	
	return OK;
}

// 保存EMV应用ID
void SaveOneAid(uint16_t uiTag, uint8_t *psData, int iDataLen)
{
	if( uiTag==0x9F06 && iAidNum<MAX_APP_NUM )
	{
		memcpy(pstAppList[iAidNum].aid, psData, MIN(16, iDataLen));
		pstAppList[iAidNum].aid_len = MIN(16, iDataLen);
		iAidNum++;
	}
}

// 保存一个应用参数的TLV要素
void SaveAppParamItem(uint16_t uiTag, uint8_t *psData, int iDataLen)
{
	switch( uiTag )
	{
	case 0x9F06:	// AID
		if( (uint8_t)iDataLen!=pstCurApp->aid_len ||
			memcmp(pstCurApp->aid, psData, iDataLen)!=0 )
		{	// Oops!下载的AID与请求的AID不匹配(POSP的Bug?)!删除AID!(实际中可能不存在,这里只是为了容错)
			pstCurApp->aid_len = 0;
		}
		break;
	
	case 0xDF01:
		pstCurApp->support_partial_aid_select = (*psData==0) ?  FULL_MATCH: PART_MATCH;
		break;
	
	case 0x9F08:
		memcpy(pstCurApp->application_version, psData, MIN(iDataLen, 2));
		break;
	
	case 0xDF11:
		memcpy(pstCurApp->terminal_action_code_default, psData, MIN(iDataLen, 5));
		break;
	
	case 0xDF12:
		memcpy(pstCurApp->terminal_action_code_online, psData, MIN(iDataLen, 5));
		break;
		
	case 0xDF13:
		memcpy(pstCurApp->terminal_action_code_denial, psData, MIN(iDataLen, 5));
		break;
		
	case 0x9F1B:
		Char2Long(psData, (uint16_t)MIN(iDataLen, 4), &pstCurApp->terminal_floor_limit);
		break;
		
	case 0xDF15:
		Char2Long(psData, (uint16_t)MIN(iDataLen, 4), &pstCurApp->threshold_value);
		break;
		
	case 0xDF16:
		pstCurApp->maximum_target_percentage = (psData[0]>>4)*10 + (psData[0] & 0x0F);
		break;
		
	case 0xDF17:
		pstCurApp->target_percentage = (psData[0]>>4)*10 + (psData[0] & 0x0F);
		break;
		
	case 0xDF14:
		pstCurApp->default_tdol[0] = (uint8_t)iDataLen;
		memcpy(&pstCurApp->default_tdol[1], psData, MIN(iDataLen, (int)sizeof(pstCurApp->default_tdol)-1));
		break;
		
	case 0xDF18:
		pstCurTermAid->bOnlinePin = *psData;
		break;

	case 0x9F7B:
		pstCurTermAid->m_EcMaxTxnAmount = PubBcd2Long(psData, (uint16_t)MIN(iDataLen, 6),NULL);
		break;
		
	case 0xDF19:
		pstCurApp->cl_Floor_Limit = PubBcd2Long(psData, (uint16_t)MIN(iDataLen, 6),NULL);
		break;

	case 0xDF20:
		pstCurApp->cl_ReaderMaxTransAmount = PubBcd2Long(psData, (uint16_t)MIN(iDataLen, 6),NULL);
		break;

	case 0xDF21:
		pstCurApp->cl_CVM_Amount = PubBcd2Long(psData, (uint16_t)MIN(iDataLen, 6),NULL);
		break;
		
	default:
		break;
	}
}

// 保存EMV应用参数到EMV内核
uint8_t SaveEmvAppParam(TermAidList *pstTermAidList)
{
	int	iRet, iCnt, iAidFile, iErrNo;
	struct terminal_aid_info stTempApp;

	// 删除core的AID,然后重新添加
	for(iCnt=0; iCnt<MAX_APP_NUM; iCnt++)
	{
		memset(&stTempApp, 0, sizeof(stTempApp));
		iRet = emv_get_one_aid_info(iCnt, &stTempApp);
		
		emv_del_one_aid_info(stTempApp.aid_len, stTempApp.aid);
	}

	for(iCnt=0; iCnt<iAidNum; iCnt++)
	{
		iRet = emv_add_one_aid_info(&pstAppList[iCnt]);
		iErrNo = iRet?errno:0;

		if( iErrNo!=EMV_RET_SUCCESS )
		{
			return E_TRANS_FAIL;
		}
		emvSaveAIDList(iCnt, &pstAppList[iCnt]);
	}

//	emvSaveAIDList();

	// 删除终端的AID,并重新添加
	fileRemove(TERM_AIDLIST);
	iAidFile = fileOpen(TERM_AIDLIST, O_RDWR|O_CREAT);
	if( iAidFile<0 )
	{
		return E_TRANS_FAIL;
	}
	for(iCnt=0; iCnt<iAidNum; iCnt++)
	{
		if( pstTermAidList[iCnt].ucAidLen==0 )
		{
			continue;
		}
		iRet = fileWrite(iAidFile, (uint8_t *)&pstTermAidList[iCnt], sizeof(TermAidList));
		if( iRet!=sizeof(TermAidList) )
		{
			fileClose(iAidFile);
			return E_TRANS_FAIL;
		}
	}
	fileClose(iAidFile);

	return ReadTermAidList();
}

// 参数/CAPK下载结束交易
uint8_t FinishDownLoad(int iTranNo)
{
	uint8_t	ucRet;

	// 参数下载完成
	lcdCls();
	lcdFlip();	
	if( iTranNo==END_EMV_PARAM )
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  参数下载结束  ", "  LOAD PARA END ");
		stTransCtrl.bEmvAppLoaded = TRUE;
	}
	else if (iTranNo== END_DOWNLOADBLK)
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, " 黑名单下载结束 ", "LOAD KEY END");
		stTransCtrl.bEmvCapkLoaded = TRUE;
	}
	else 
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  公钥下载结束  ", "LOAD KEY END");
		stTransCtrl.bEmvCapkLoaded = TRUE;
	}
	SaveCtrlParam();
	DispMulLanguageString(0, 2, DISP_CFONT, "已连接银联中心  ", "CONNECT BANK");
	
	InitCommData();
	stTemp.iTransNo = iTranNo;
	SetCommReqField((uint8_t *)"0800", (uint8_t *)"");
	ucRet = SendRecvPacket();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	if( memcmp(glRecvPack.szRspCode, "00", 2)!=0 )
	{
		return E_TRANS_FAIL;
	}
		
	return OK;
}

// 查询POSP的CAPK信息
uint8_t QueryEmvCapk(void)
{
	uint8_t	ucRet;

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "查询中,稍候", "QUERYING,WAIT");
	while(1)
	{
		InitCommData();
		stTemp.iTransNo = QUERY_EMV_CAPK;
		SetCommReqField((uint8_t *)"0820", (uint8_t *)"");
		ShortToByte(glSendPack.sField62, 3);	// 二进制的域,头2个字节存放数据长度
		sprintf((char *)&glSendPack.sField62[2], "1%02d", iCapkNum);
		
		ucRet = SendRecvPacket();
		if( ucRet!=OK )
		{
			return ucRet;
		}
		if( memcmp(glRecvPack.szRspCode, "00", 2)!=0 )
		{
			return E_TRANS_FAIL;
		}
		
		// 解析CAPK信息
		bRidOk = bKeyIDOk = bExpiryOk = FALSE;
		ucRet = UnpackTlvData(SaveCapkInfoItem);
		if( ucRet!=OK )
		{
			return ucRet;
		}
		
		if( glRecvPack.sField62[2]!='2' )
		{
			break;
		}
	}

	if( iCapkNum==0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 3, DISP_CFONT|DISP_MEDIACY, "无公钥下载", "NO KEY");
		kbGetKeyMs(2000);
		stTransCtrl.bEmvCapkLoaded = TRUE;
		SaveCtrlParam();
		return NO_DISP;
	}
	
	return OK;
}

// 保存CAPK信息的一个TLV元素
void SaveCapkInfoItem(uint16_t uiTag, uint8_t *psData, int iDataLen)
{
	if( iCapkNum>=MAX_KEY_NUM )
	{
		return;
	}

	switch( uiTag )
	{
	case 0x9F06:
		memcpy(pstCapkInfoList[iCapkNum].sRid, psData, MIN(iDataLen, 5));
		bRidOk = TRUE;
		break;
	
	case 0x9F22:
		pstCapkInfoList[iCapkNum].ucKeyID = *psData;
		bKeyIDOk = TRUE;
		break;
	
	case 0xDF05:
		if( iDataLen==8 )	// 目前CUP是按照这个ASCII数字串返回的
		{
			AscToBcd(pstCapkInfoList[iCapkNum].sExpDate, psData, 8);
		}
		else
		{
			memcpy(pstCapkInfoList[iCapkNum].sExpDate, psData, MIN(iDataLen, 4));
		}
		bExpiryOk = TRUE;
		break;
	}

	if( bRidOk && bKeyIDOk && bExpiryOk )	// 三个要素都设置了,则继续分析组CAPK信息
	{
		bRidOk = bKeyIDOk = bExpiryOk = FALSE;
		pstCapkInfoList[iCapkNum].bNeedDownLoad = TRUE;
		iCapkNum++;
	}
}

// 更新CORE的CAPK信息
uint8_t UpdateCoreCapkInfo(void)
{
	int		iRet, iCnt, iErrNo;
	struct issuer_ca_public_key	stTempCapk;

    for(iCnt=0; iCnt<iCapkNum; iCnt++)
    {
        memset(&stTempCapk, 0, sizeof(stTempCapk));
        iRet = emv_get_one_ca_public_key(pstCapkInfoList[iCnt].sRid, pstCapkInfoList[iCnt].ucKeyID, &stTempCapk);
		iErrNo = iRet?errno:0;
        if( iErrNo!=EMV_RET_SUCCESS )
        {
            continue;
        }

        // CAPK在EMV内核和POSP中存在
        if( memcmp(stTempCapk.expiration_date, &pstCapkInfoList[iCnt].sExpDate[1], 3) ) // !需要更新有效期!
        {
            pstCapkInfoList[iCnt].bNeedDownLoad = FALSE;	// 忽略该CAPK
//            iRet = emvChgCAExpDate(pstCapkInfoList[iCnt].sRid, pstCapkInfoList[iCnt].ucKeyID, &pstCapkInfoList[iCnt].sExpDate[1]);
            iRet = emv_add_one_ca_public_key(&stTempCapk);
			iErrNo = iRet?errno:0;
        }

        if( iErrNo!=EMV_RET_SUCCESS )
        {
            return E_TRANS_FAIL;
        }
    }

	return OK;
}

// 查找查询的CAPK信息
int SearchCapkInfo(uint8_t *psRid, uint8_t ucKeyID)
{
	int		iCnt;

	for(iCnt=0; iCnt<iCapkNum; iCnt++)
	{
		if( ucKeyID==pstCapkInfoList[iCnt].ucKeyID   &&
			memcmp(psRid, pstCapkInfoList[iCnt].sRid, 5)==0 )
		{
			return iCnt;
		}
	}

	return -1;
}

void SaveEmvCapkItem(uint16_t uiTag, uint8_t *psData, int iDataLen)
{
	if( iDataLen<=0 )
	{
		return;
	}

	switch( uiTag )
	{
	case 0x9F06:
		memcpy(pstCurCapk->RID, psData, MIN(iDataLen, 5));
		break;
	
	case 0x9F22:
		pstCurCapk->Index   = *psData;
		break;
	
	case 0xDF05:
		if( iDataLen==8 )	// 目前CUP是按照这个ASCII数字串返回的
		{
			AscToBcd(pstCurCapk->expiration_date, psData+2, 6);
		}
		else
		{
			memcpy(pstCurCapk->expiration_date, psData+1, 3);
		}
		break;
			
	case 0xDF02:
		pstCurCapk->len_of_modulus = (uint8_t)MIN(iDataLen, 248);
		memcpy(pstCurCapk->modulus, psData, pstCurCapk->len_of_modulus);
		break;
		
	case 0xDF04:
		pstCurCapk->len_of_exponent = (uint8_t)MIN(iDataLen, 3);
		memcpy(pstCurCapk->exponent, psData, pstCurCapk->len_of_exponent);
		break;
		
	case 0xDF03:
		memcpy(pstCurCapk->checksum, psData, MIN(iDataLen, 20));
		break;
	}
}

uint8_t SaveNewEmvCapk(struct issuer_ca_public_key *pstNewCapkList)
{
	int		iRet, iCnt;
    uint8_t       szDate[9], szTime[7];

    uint8_t       szBuff[20];
	int    ifileindex;

    memset(szDate, 0, sizeof(szDate));
    memset(szTime, 0, sizeof(szTime));
    GetPosTime(szDate, szTime); 

    
	for(iCnt=0; iCnt<iCapkNum; iCnt++)
	{
		if( pstCapkInfoList[iCnt].bNeedDownLoad==FALSE )
		{
			continue;
		}
        memset(szBuff, 0, sizeof(szBuff));
        BcdToAsc0(szBuff, pstNewCapkList[iCnt].expiration_date, 6);
 //           prnPrintf("Expiry: 20%.2s-%.2s-%.2s\n", szBuff, &szBuff[2], &szBuff[4]);
        
        
//        prnPrintf("\n\n\n");
//        PrintData();
		if( memcmp(&szDate[2], szBuff, 6) >0 )
		{
			continue;
		}		
        iRet = emv_add_one_ca_public_key(pstNewCapkList+iCnt);
		if( iRet!=EMV_RET_SUCCESS )
		{
			return E_TRANS_FAIL;
		}
//		emvSaveCAPKList(iCnt, pstNewCapkList+iCnt); 
		// 防止有效期过了，iCnt的大小超过了文件大小
		if (fileExist("emv.capk")>=0)
		{
			ifileindex = fileSize("emv.capk");
		}
		else
		{
			ifileindex = 0;
		}
		ifileindex = ifileindex/(sizeof(struct issuer_ca_public_key));
		ifileindex = MIN(ifileindex,iCnt);
		emvSaveCAPKList(ifileindex, pstNewCapkList+iCnt); // 
	}
//	emvSaveCAPKList();

	return OK;
}

uint8_t PosDownLoadParam(void)
{
	uint8_t	ucRet;

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    参数下载    ", "   LOAD PARA    ");
	DispMulLanguageString(0, 2, DISP_CFONT, "已连接银联中心  ", "CONNECT BANK  ");

	stTransCtrl.ucLoadTask = 1;
	SaveCtrlParam();

	InitCommData();
	stTemp.iTransNo = POS_DOWNLOAD_PARAM;
	SetCommReqField((uint8_t *)"0800", (uint8_t *)"");
	ucRet = SendRecvPacket();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	if( memcmp(glRecvPack.szRspCode, "00", 2)!=0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 3, DISP_CFONT, "参数下载失败", "LOAD PARAM FAIL");
		sysDelayMs(1000);
		return NO_DISP;
	}
	UpdateParameters(&glRecvPack.sField62[2], ByteToShort(glRecvPack.sField62));

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "参数下载成功", "SUCCESS");
	kbGetKeyMs(3000);

	return OK;
}

uint8_t PosUpLoadParam(void)
{
	uint8_t	ucRet;
	int		iLen;

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    状态上送    ", "  STATUS UPLOAD ");
	DispMulLanguageString(0, 2, DISP_CFONT, "已连接银联中心  ", "CONNECT BANK");

	InitCommData();
	stTemp.iTransNo = POS_UPLOAD_PARAM;
	SetCommReqField((uint8_t *)"0820", (uint8_t *)"");
	memset(glSendPack.sField62,0,sizeof(glSendPack.sField62));
	iLen = MakeTermStatusData(&glSendPack.sField62[2]);
	ShortToByte(glSendPack.sField62, (uint16_t)iLen);

	ucRet = SendRecvPacket();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	if( memcmp(glRecvPack.szRspCode, "00", 2)!=0 )
	{
		return E_TRANS_FAIL;
	}
	init_dial_statics();

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "状态上送成功", "SUCCESS");
	kbGetKeyMs(3000);

	return OK;
}

// 生产POS状态上送信息
int MakeTermStatusData(uint8_t *pStatus) 
{
	strcpy((char *)pStatus, "011021031041051");	
	strcpy((char *)pStatus+15, "1160");	
	sprintf((char *)pStatus+19, "12%02d", (int)stPosParam.ucCommWaitTime);
	sprintf((char *)pStatus+23, "13%d", (int)stPosParam.ucDialRetryTimes);
	strcpy((char *)pStatus+26, "14              ");
	memcpy(pStatus+28, stPosParam.szPhoneNum1, strlen((char *)stPosParam.szPhoneNum1));					
	strcpy((char *)pStatus+42, "15              ");
	memcpy(pStatus+44, stPosParam.szPhoneNum2, strlen((char *)stPosParam.szPhoneNum2));					
	strcpy((char *)pStatus+58, "16              ");
	memcpy(pStatus+60, stPosParam.szPhoneNum3, strlen((char *)stPosParam.szPhoneNum3));
	strcpy((char *)pStatus+74, "17              ");
	memcpy(pStatus+76, stPosParam.szMPhone, strlen((char *)stPosParam.szMPhone));					
	
	strcpy((char *)pStatus+90, "18");					
	pStatus[92] = stPosParam.ucTipOpen;
	sprintf((char *)pStatus+93,"19%02d", (int)stPosParam.ucTipper);
	
	strcpy((char *)pStatus+97, "20");
	pStatus[99] = stPosParam.ucManualInput;
	
	strcpy((char *)pStatus+100, "21");
	pStatus[102] = stPosParam.ucAutoLogoff;
	
	sprintf((char *)pStatus+103, "23%1d", (int)stPosParam.ucResendTimes);	
//	memcpy(pStatus+106, "24",2);
//	pStatus[108] = stPosParam.ucSendOffFlag;
	sprintf((char *)pStatus+106, "25%1d", (int)stPosParam.ucKeyIndex);
	sprintf((char *)pStatus+109,"27%2d", (int)stPosParam.ucMaxOfflineTxn);
	
	if( calculate_dial_statics()!=OK ) 
		memcpy(pStatus + 113 ,"51007200999980",14);
	else 
		sprintf((char*)pStatus+113, "51%04d%05d%03d", dial_stat.hours, dial_stat.dials, dial_stat.suc_rate);
	pStatus[127] = 0;
	
	return 127;
}

/************************************************************************
 只上送IC卡离线交易(SALE),并且根据CUP要求,必须在结算前上送
************************************************************************/
uint8_t SendICCOffLineTrans(uint8_t flag, int maxSendTimes)
{
	uint8_t		ucRet;
	int		iRecNo, iRecIndex;
	NEWPOS_LOG_STRC	stLog;
	uint32_t uiStatus;
	int      iCurUploadNum = 1;
	char     szBuffer[40];

	for(iRecNo=0; iRecNo<stTransCtrl.iTransNum; iRecNo++)
	{
		if (flag)
		{
			if( kbGetKeyMs(150) == KEY_CANCEL )
			{
				return NO_DISP;
			}
		}

		memset(&stLog, 0, sizeof(stLog));
		iRecIndex = GetMinSendTimesTxn(TS_ICC_OFFLINE);
		if (iRecIndex == -1)
		{
			return 0;
		}

		ucRet = LoadTranLog(iRecIndex, &stLog);
		if( ucRet )
		{
			return ucRet;
		}

		if( (stLog.iTransNo!=ICC_OFFSALE && stLog.iTransNo != EC_NORMAL_SALE && stLog.iTransNo != EC_QUICK_SALE) 
			|| stLog.ucSendFlag ||	stLog.ucVoidId )
		{
			uiStatus = stTransCtrl.sTxnRecIndexList[iRecIndex];
			uiStatus &= ~TS_ICC_OFFLINE;
			UpdataTxnCtrlStatus(iRecIndex,uiStatus);
			continue;
		}

		InitCommData();
		stTemp.iTransNo = stLog.iTransNo;
		PosCom.ucSwipedFlag = CARD_INSERTED;
		if (stLog.iTransNo == EC_QUICK_SALE)
		{
			PosCom.ucSwipedFlag = CARD_PASSIVE;
			PosCom.stTrans.ucSwipeFlag = CARD_PASSIVE;
		}
		memcpy(&PosCom.stTrans, &stLog, sizeof(stLog));
		SetCommReqField((uint8_t *)"0200", (uint8_t *)"000000");

		stLog.iIccDataLen += 4;
		ShortToByte(glSendPack.sICCData, (uint16_t)stLog.iIccDataLen);
		memcpy(&glSendPack.sICCData[2], stLog.sIccData, stLog.iIccDataLen-4);
		memcpy(&glSendPack.sICCData[2+stLog.iIccDataLen-4], "\x8A\x02\x59\x31", 4);
		stLog.iIccDataLen -= 4;
		strcpy((char *)glSendPack.szField63, "CUP");
		glSendPack.szField60[13] = 0; // 60.6-7不送

		lcdClrLine(2,7);
		DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "正在上送交易    ", "Upload Txn...");
		if (stPosParam.iDispLanguage==1)
		{
			sprintf(szBuffer,"第 %d 笔",iCurUploadNum);
		}
		else
		{
			sprintf(szBuffer,"Upload %d ",iCurUploadNum);
		}
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, szBuffer, szBuffer);
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "请稍候  ", "PLS WAIT....");
		sysDelayMs(500); // 延迟等
		iCurUploadNum++;
		
		PosCom.bOnlineTxn = FALSE;  // 先设置没有联机成功 
		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "已连接银联中心  ");
		ucRet = SendRecvPacket();
		if( ucRet!=OK || strlen((char*)PosCom.szRespCode)==0)
		{
//				continue; /*离线交易上送收不到应答，继续上送*/
			if (PosCom.bOnlineTxn)
			{
				uiStatus = stTransCtrl.sTxnRecIndexList[stLog.uiTxnRecIndex];
				uiStatus++;  // 状态1，即相当余发送次数加1
				UpdataTxnCtrlStatus(stLog.uiTxnRecIndex,uiStatus);					
			}
			stLog.ucSendFail = TS_NOT_RECV;
			ucRet = UpdateTranLog(iRecIndex, &stLog);
			if( ucRet!=OK )
			{
				return ucRet;
			}
		}
		else
		{
			/*此处判断返回码*/
			if( memcmp(PosCom.szRespCode, "00", 2)==0 || memcmp(PosCom.szRespCode, "94", 2)==0 )
			{
				//break;
				uiStatus = stTransCtrl.sTxnRecIndexList[stLog.uiTxnRecIndex];
				uiStatus &= ~TS_ICC_OFFLINE;
				UpdataTxnCtrlStatus(stLog.uiTxnRecIndex,uiStatus);

				stLog.ucSendFlag = TRUE;
				stLog.ucSendFail = FALSE;  //成功 
				ucRet = UpdateTranLog(iRecIndex, &stLog);
				if( ucRet!=OK )
				{
					return ucRet;
				}
			}
			else
			{
				uiStatus = stTransCtrl.sTxnRecIndexList[stLog.uiTxnRecIndex];
				uiStatus |= 0x0F;  //  状态改成最大，不要再发送了
				UpdataTxnCtrlStatus(stLog.uiTxnRecIndex,uiStatus);

				stLog.ucSendFail = TS_NOT_CODE;  // 非00
				ucRet = UpdateTranLog(iRecIndex, &stLog);
				if( ucRet!=OK )
				{
					return ucRet;
				}
			}
		}
		
	}	//for

	return OK;
}

//20080801 增加批上送所有的IC脱机交易  暂时这么处理，需要看规范
uint8_t BatchICCOffLineTrans(int *piTotalNum, uint8_t ucSendFlag)
{
	uint8_t		ucRet;
	int		iRecNo;
	NEWPOS_LOG_STRC	stLog;

	for(iRecNo=0; iRecNo<stTransCtrl.iTransNum; iRecNo++)
	{
		memset(&stLog, 0, sizeof(stLog));
		ucRet = LoadTranLog(iRecNo, &stLog);
		if( ucRet!=OK )
		{
			return ucRet;
		}

		if( !(stLog.iTransNo ==ICC_OFFSALE || stLog.iTransNo == EC_QUICK_SALE || stLog.iTransNo == EC_NORMAL_SALE ))
		{
			continue;
		}

		if( stLog.ucBatchSendFlag==TRUE )
		{
			(*piTotalNum)++;
			continue;
		}

		InitCommData();
		memcpy(&PosCom.stTrans, &stLog, sizeof(stLog));
		stTemp.iTransNo = ICC_BATCHUP;
		PosCom.stTrans.iTransNo = stTemp.iTransNo;
		if (stLog.iTransNo == EC_QUICK_SALE)
		{
			PosCom.ucSwipedFlag = CARD_PASSIVE;
			PosCom.stTrans.ucSwipeFlag = CARD_PASSIVE;
		}
		SetCommReqField((uint8_t *)"0320", (uint8_t *)"000000");
		sprintf((char *)glSendPack.szPan, "%.19s", PosCom.stTrans.szCardNo);
		BcdToAsc0(glSendPack.szTranAmt, PosCom.stTrans.sAmount, 12);
//20080805 有效期需要上送(有则上送)
		sprintf((char *)glSendPack.szExpDate,  "%.*s", LEN_EXP_DATE, PosCom.stTrans.szExpDate);
		sprintf((char *)glSendPack.szEntryMode, "%s0",  PosCom.stTrans.szEntryMode);
		sprintf((char *)glSendPack.szSvrCode, "00");
		sprintf((char *)glSendPack.szCurrencyCode, "156");

		if( PosCom.stTrans.bPanSeqNoOk )	// ADVT case 43 
		{
			sprintf((char *)glSendPack.szPanSeqNo, "%0*X", LEN_PAN_SEQ_NO, PosCom.stTrans.ucPanSeqNo);
		}
//20080804 增加
		stLog.iIccDataLen += 4;
		ShortToByte(glSendPack.sICCData, (uint16_t)stLog.iIccDataLen);
		memcpy(&glSendPack.sICCData[2], stLog.sIccData, stLog.iIccDataLen-4);
		memcpy(&glSendPack.sICCData[2+stLog.iIccDataLen-4], "\x8A\x02\x59\x31", 4);
		
		stLog.iIccDataLen -= 4;

		strcpy((char *)glSendPack.szField63, "CUP");

		if (stLog.iTransNo == EC_QUICK_SALE || PosCom.stTrans.ucSwipeFlag & CARD_PASSIVE)
		{
			sprintf((char *)glSendPack.szField60, "36%06d00060", PosCom.stTrans.lBatchNumber);
		}
		else
		{
			sprintf((char *)glSendPack.szField60, "36%06d00050", PosCom.stTrans.lBatchNumber);
		}

		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "已连接银联中心  ");
		ucRet = SendRecvPacket();
		if( ucRet!=OK )
		{
			return ucRet; 
		}
		
		stLog.ucBatchSendFlag = TRUE;
		ucRet = UpdateTranLog(iRecNo, &stLog);
		if( ucRet!=OK )
		{
			return ucRet;
		}
		(*piTotalNum)++;

	}	//for

	return OK;
}
/****************************************************************
功能: 所有IC卡联机交易的批上送
返回: 上送笔数
****************************************************************/
uint8_t BatchSendICCOnLineTrans(int *piTotalNum, uint8_t ucSendFlag)
{
	uint8_t		ucRet;
	int		iCnt;
	NEWPOS_LOG_STRC	stLog;

	for(iCnt=0; iCnt<stTransCtrl.iTransNum; iCnt++)
	{
		memset(&stLog, 0, sizeof(stLog));
		ucRet = LoadTranLog(iCnt, &stLog);
		if( ucRet!=OK )
		{
			return ucRet;
		}

		if( stLog.ucSwipeFlag!=CARD_INSERTED ||
			(stLog.iTransNo!=POS_SALE && stLog.iTransNo!=POS_PREAUTH && stLog.iTransNo != EC_TOPUP_CASH) )
		{
			continue;
		}

		if((stLog.sTVR[4] & 0x40) ) // 发卡行认证失败的时候放到另外的失败记录上送
		{
			continue;
		}

		if( stLog.ucBatchSendFlag==TRUE )
		{
			(*piTotalNum)++;
			continue;
		}

		InitCommData();
		memcpy(&PosCom.stTrans, &stLog, sizeof(stLog));
		stTemp.iTransNo = ICC_BATCHUP;
		PosCom.stTrans.iTransNo = stTemp.iTransNo;
		SetCommReqField((uint8_t *)"0320", (uint8_t *)"");
		sprintf((char *)glSendPack.szPan, "%.19s", PosCom.stTrans.szCardNo);
		BcdToAsc0(glSendPack.szTranAmt, PosCom.stTrans.sAmount, 12);
		sprintf((char *)glSendPack.szEntryMode, "%s0",  PosCom.stTrans.szEntryMode);
		if( PosCom.stTrans.bPanSeqNoOk )	// ADVT case 43 
		{
			sprintf((char *)glSendPack.szPanSeqNo, "%0*X", LEN_PAN_SEQ_NO, PosCom.stTrans.ucPanSeqNo);
		}
		ShortToByte(glSendPack.sICCData, PosCom.stTrans.iIccDataLen);
		memcpy(&glSendPack.sICCData[2], PosCom.stTrans.sIccData, PosCom.stTrans.iIccDataLen);

		memcpy(&glSendPack.sICCData[5], PosCom.stTrans.sAppCrypto, 8); // 改变9F26
		memcpy(&glSendPack.sICCData[5+8+3], "\x40", 1); // 改变9F27

//20080804 不平送205，平送203
		if( ucSendFlag!=EMVLOG )
			memcpy((char *)&glSendPack.szField60[8], "205", 3);
		memcpy(glSendPack.sField62, "\x00\x15\x36\x31", 4);
		sprintf((char *)&glSendPack.sField62[4], "0000%s156", glSendPack.szTranAmt);
		if( memcmp(PosCom.stTrans.szCardUnit, "CUP", 3)!=0 )
		{
			memcpy(&glSendPack.sField62[4], "01", 2);
		}

		glSendPack.szField60[13] = 0; // 60.6-7不送

		ucRet = SendRecvPacket();
		if( ucRet!=OK )
		{
			return ucRet;
		}

		stLog.ucBatchSendFlag = TRUE;
		ucRet = UpdateTranLog(iCnt, &stLog);
		if( ucRet!=OK )
		{
			return ucRet;
		}
		(*piTotalNum)++;
	}

	return OK;
}

void UpdateEntryModeByPIN(void)
{
	if( PosCom.ucPinEntry )
	{
		PosCom.stTrans.szEntryMode[2] = HAVE_INPUT_PIN;
		// set bit 52, pin block
//20080730
		sprintf((char *)glSendPack.szCaptureCode, "12"); // 有密码才送26域
		ShortToByte(glSendPack.sPINData, LEN_PIN_DATA);	// 二进制的域,头2个字节存放数据长度
		memcpy(&glSendPack.sPINData[2], PosCom.sPIN, LEN_PIN_DATA);
	
	}
	else
	{
		PosCom.stTrans.szEntryMode[2] = NOT_INPUT_PIN;
	}

	if (PosCom.stTrans.iTransNo == POS_RESER_SALE || PosCom.stTrans.iTransNo == POS_RESER_VOID)
	{
		memcpy((char*)PosCom.stTrans.szEntryMode,"92",2);
	}

	if ( PosCom.ucPinEntry || strlen((char *)glSendPack.szTrack2)!=0 || strlen((char *)glSendPack.szTrack3)!=0)
	{
		strcpy((char *)glSendPack.szSecurityInfo, "0000000000000000");
		if (PosCom.ucPinEntry)
		{
			glSendPack.szSecurityInfo[0] = '2';
			if (PosCom.stTrans.iTransNo == POS_RESER_SALE || //PosCom.stTrans.iTransNo == POS_RESER_VOID ||
				(PosCom.stTrans.iTransNo>=POS_MOBILE_SALE && PosCom.stTrans.iTransNo <= POS_MOBILE_QUE))
			{
				glSendPack.szSecurityInfo[0] = '1';
			}
		}

		// set bit 53
		if( stPosParam.ucKeyMode!=KEY_MODE_DES )
		{
			glSendPack.szSecurityInfo[1] = '6';
		}

		if (!(PosCom.stTrans.iTransNo>=POS_MOBILE_SALE && PosCom.stTrans.iTransNo <= POS_MOBILE_QUE))
		{
			if (strlen((char *)glSendPack.szTrack2)!=0 || strlen((char *)glSendPack.szTrack3)!=0)
			{
				glSendPack.szSecurityInfo[2] = '1';
			}
		}
	}

	// CUP要求bit 22左对齐,因此在后面补充一个'0'(因为iPack8583()是右靠齐的)
	sprintf((char *)glSendPack.szEntryMode, "%s0", PosCom.stTrans.szEntryMode);
}


void  EncrypteFieldTrac23(uint8_t *pszTrack, uint8_t *pszEncrypData)
{
	char  szTemp[300],szBuffer[300],szData[10];
	int   iRet, iDatalen, iCnt;
	
	memset(szBuffer,0,sizeof(szBuffer));
	memset(szTemp,0,sizeof(szTemp));
	memset(szData,0,sizeof(szData));
	if (pszTrack == NULL || strlen((char *)pszTrack)==0)
	{
		return;
	}

	iDatalen = strlen((char *)pszTrack);
	strcpy((char *)szTemp,(char *)pszTrack);
	if (iDatalen < 17)
	{
		PubAddHeadChars(szTemp,'F',17);
		iDatalen = 17;
	}

	if (iDatalen%2==0)
		memcpy(szBuffer,&szTemp[iDatalen-18],16);
	else
		memcpy(szBuffer,&szTemp[iDatalen-17],16);

	for (iCnt=0; iCnt<8; iCnt++)
	{
		szData[iCnt] = ((szBuffer[2*iCnt]&0x0F)<<4) | (szBuffer[2*iCnt+1]&0x0F);
	}

	iRet = PedGetMac(DOUBLE_TDK_KEY_ID, szData, 8, MAC_MODE_1, szBuffer);
	if( iRet!=PED_RET_OK )
			return ;

	if (iDatalen%2==0)
		PubBcd2Asc(szBuffer,8,&szTemp[iDatalen-18]);
	else
		PubBcd2Asc(szBuffer,8,&szTemp[iDatalen-17]);

	if (pszEncrypData != NULL)
		strcpy((char *)pszEncrypData,szTemp);
}


void  SetCommField60(void)
{
	uint8_t	ucIndex;

	if( stTemp.iTransNo==POS_LOGON )
	{	
		if (stPosParam.ucKeyMode==KEY_MODE_3DES)
			sprintf((char *)glSendPack.szField60, "00%06d004", PosCom.stTrans.lBatchNumber);
		else
			sprintf((char *)glSendPack.szField60, "00%06d001", PosCom.stTrans.lBatchNumber);
		return;
	}
	else
	{
		ucIndex = GetTxnTableIndex(stTemp.iTransNo);
		sprintf((char *)glSendPack.szField60, "%s%06d%s500", TrTable[ucIndex].szMessTypeCode,
					PosCom.stTrans.lBatchNumber, TrTable[ucIndex].szNetManInfoCode);

		if( PosCom.ucSwipedFlag==FALLBACK_SWIPED )
		{
			glSendPack.szField60[12] = '2';
		}

		if (PosCom.stTrans.ucSwipeFlag & CARD_PASSIVE)
		{
			glSendPack.szField60[11] = '6';
		}

		if (stPosParam.ucPartPaySupport == PARAM_OPEN)
		{
			glSendPack.szField60[13] = '1';
		}

		if (stTemp.iTransNo >= POS_LOYATL_ISSUER && stTemp.iTransNo <= POS_QUE_LOYATL_ALL)
		{
			sprintf((char*)glSendPack.szField60,"%s048",glSendPack.szField60);
			if (stTemp.iTransNo == POS_LOYATL_ALL || stTemp.iTransNo == POS_VOID_LOYATL_ALL 
				|| stTemp.iTransNo == POS_REFUND_LOYATL_ALL || stTemp.iTransNo == POS_QUE_LOYATL_ALL )
			{
				memcpy(&glSendPack.szField60[14],"065",3);
			}
		}

		if(ChkSetPartSaleFlag())
		{
			glSendPack.szField60[13] = 0; // 60.6-7不送
		}
	}
}

uint8_t TransDownloadBlkCard(void)
{
	uint8_t		ucRet,  ucFirstTime;
	int		iCurTotalNum, iTotalNum, uiCardlen;
	ST_BLACKLIST_TOTAL stBlkCard;
	uint8_t  *psTemp;
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_CFONT|DISP_REVERSE, "   黑名单下载   ", "   LOAD IC KEY  ");

	PreDial();

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_CFONT|DISP_REVERSE, "   黑名单下载   ", "   LOAD IC KEY  ");
	DispMulLanguageString(0, 2, DISP_CFONT, "已连接银联中心", "CONNECT BANK");
	
	memset((char*)&stBlkCard,0,sizeof(stBlkCard));
	stBlkCard.ulBlackListNum = 0;
	ucFirstTime = TRUE;
	iCurTotalNum = iTotalNum = 0;
	while(1)
	{
		InitCommData();
		stTemp.iTransNo = POS_DOWNLOADBLK;
		PosCom.stTrans.iTransNo = POS_DOWNLOADBLK;
		GetPosTime(stTemp.szDate, stTemp.szTime);		
		strcpy((char *)PosCom.stTrans.szDate, (char *)stTemp.szDate);
		strcpy((char *)PosCom.stTrans.szTime, (char *)stTemp.szTime);
		
		SetCommReqField((uint8_t *)"0800", (uint8_t *)"");
		memset(glSendPack.szSTAN,0,sizeof(glSendPack.szSTAN));
		memset(glSendPack.szEntryMode,0,sizeof(glSendPack.szEntryMode));
		memset(glSendPack.szSvrCode,0,sizeof(glSendPack.szSvrCode));
		memset(glSendPack.szCurrencyCode,0,sizeof(glSendPack.szCurrencyCode));
		sprintf((char *)glSendPack.szField60, "00%06d390",stPosParam.lNowBatchNum);
/*		if (!ucFirstTime)
		{
			glSendPack.szField60[10] = '1';
		}*/
		ShortToByte(glSendPack.sField62, 3);
		if (iTotalNum)
			sprintf((char*)glSendPack.sField62+2,"%03d",iTotalNum+1);	
		else
			sprintf((char*)glSendPack.sField62+2,"%03d",iTotalNum);		
		ucRet = SendRecvPacket();
		if( ucRet!=OK )
		{
			return ucRet;
		}

		if( memcmp(glRecvPack.szRspCode, "00", 2)!=0 )
		{
 			dispRetCode();
			return NO_DISP;
		}

		psTemp = &glRecvPack.sField62[2];
		psTemp++;
		iCurTotalNum = PubAsc2Long((char*)psTemp,3,NULL);
		psTemp += 3;

		while(iTotalNum<iCurTotalNum)
		{
			if (psTemp == NULL)
			{
				lcdClrLine(2,7);
				lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"数据错误");
				lcdDisplay(0,5,DISP_CFONT|DISP_MEDIACY,"请 重 试");
				PubBeepErr();
				PubWaitKey(5);
				return NO_DISP;
			}

			if (iTotalNum >= MAX_BLACKLIST_NUM)
			{
				lcdClrLine(2,7);
				lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"黑名单数量超限");
				lcdDisplay(0,5,DISP_CFONT|DISP_MEDIACY,"请 重 试");
				PubBeepErr();
				PubWaitKey(5);
				return NO_DISP;
			}

			uiCardlen = PubAsc2Long((char*)psTemp,2,NULL);
			psTemp += 2;
			stBlkCard.szBlackList[iTotalNum][0] = uiCardlen;
			PubAsc2Bcd((char*)psTemp,uiCardlen,&stBlkCard.szBlackList[iTotalNum][1]);
			psTemp+=14;
			iTotalNum++;
		}

		stBlkCard.ulBlackListNum = iTotalNum;
		if (glRecvPack.sField62[2] != '2')
		{
			break;
		}
	}

	if (stBlkCard.ulBlackListNum != 0)
	{
		//保存
		SaveBlkCard(&stBlkCard);
	}

	ucRet = FinishDownLoad(END_DOWNLOADBLK);
	if (ucRet != 0)
	{
		return ucRet;
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_CFONT|DISP_REVERSE, "   黑名单下载   ", "   LOAD IC KEY  ");
	lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"下载黑名单成功");
	PubBeepErr();
	PubWaitKey(5);
	return OK;	
}


// 逐步上送所有的失败的ICC卡QPBOC卡交易记录
uint8_t TransBatchUpLoadFailureTxn(int *piTotalNum, uint8_t ucSendFlag)
{
	uint8_t ucRet;

	// AAC失败交易先上送
	ucRet = TransBatchUpLoadIccAACTxn(piTotalNum, ucSendFlag);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	// 其他失败交易后上送
	ucRet = TransBatchUpLoadIccFailureTxn(piTotalNum, ucSendFlag);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucRet = TransBatchUploadQpbocFailureTxnLog(piTotalNum, ucSendFlag);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	return OK;
}

// 逐步上送所有的失败的ICC发卡行认证失败的交易记录
uint8_t TransBatchUpLoadIccFailureTxn(int *piTotalNum, uint8_t ucSendFlag)
{
	uint8_t		ucRet;
	int		iCnt;
	NEWPOS_LOG_STRC	stLog;

	for(iCnt=0; iCnt<stTransCtrl.iTransNum; iCnt++)
	{
		memset(&stLog, 0, sizeof(stLog));
		ucRet = LoadTranLog(iCnt, &stLog);
		if( ucRet!=OK )
		{
			return ucRet;
		}

		if( stLog.ucSwipeFlag!=CARD_INSERTED ||
			(stLog.iTransNo!=POS_SALE && stLog.iTransNo!=POS_PREAUTH) )
		{
			continue;
		}

		if(!(stLog.sTVR[4] & 0x40) )
		{
			continue;
		}

		if( stLog.ucBatchSendFlag==TRUE )
		{
			(*piTotalNum)++;
			continue;
		}

		InitCommData();
		memcpy(&PosCom.stTrans, &stLog, sizeof(stLog));
		stTemp.iTransNo = ICC_FAIL_BATCHUP;
		PosCom.stTrans.iTransNo = stTemp.iTransNo;
//		PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;
		SetCommReqField((uint8_t *)"0320", (uint8_t *)"");
		sprintf((char *)glSendPack.szPan, "%.19s", PosCom.stTrans.szCardNo);
		BcdToAsc0(glSendPack.szTranAmt, PosCom.stTrans.sAmount, 12);
		sprintf((char *)glSendPack.szEntryMode, "%s0",  PosCom.stTrans.szEntryMode);
		if( PosCom.stTrans.bPanSeqNoOk )	// ADVT case 43 
		{
			sprintf((char *)glSendPack.szPanSeqNo, "%0*X", LEN_PAN_SEQ_NO, PosCom.stTrans.ucPanSeqNo);
		}
		else
		{
			strcpy((char *)glSendPack.szPanSeqNo, "000");
		}
		ShortToByte(glSendPack.sICCData, PosCom.stTrans.iIccDataLen);
		memcpy(&glSendPack.sICCData[2], PosCom.stTrans.sIccData, PosCom.stTrans.iIccDataLen);

		memcpy(&glSendPack.sICCData[5], PosCom.stTrans.sAppCrypto, 8); // 改变9F26
		memcpy(&glSendPack.sICCData[5+8+3], "\x40", 1); // 改变9F27

//20080804 不平送205，平送203
		if( ucSendFlag!=EMVLOG )
			memcpy((char *)&glSendPack.szField60[8], "206", 3);
		memcpy(glSendPack.sField62, "\x00\x17\x37\x31", 4);
		sprintf((char *)&glSendPack.sField62[4], "0005%s15622", glSendPack.szTranAmt);
		if( memcmp(PosCom.stTrans.szCardUnit, "CUP", 3)!=0 )
		{
			memcpy(&glSendPack.sField62[4], "01", 2);
		}
		ucRet = SendRecvPacket();
		if( ucRet!=OK )
		{
			return ucRet;
		}

		stLog.ucBatchSendFlag = TRUE;
		ucRet = UpdateTranLog(iCnt, &stLog);
		if( ucRet!=OK )
		{
			return ucRet;
		}
		(*piTotalNum)++;
	}

	return OK;
}


uint8_t TransBatchUpLoadIccAACTxn(int *piTotalNum, uint8_t ucSendFlag)
{
	uint8_t		ucRet;
	int		iCnt, iTotalRecNum, iTransTypeBak;
	int     iFilesize = 0;
	
	iFilesize = fileSize(ICC_FAILUR_TXN_FILE);
	if (iFilesize <=0 )
	{
		return 0;
	}	
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "批上送失败交易", "  QUERY TRANS   ");
	iTotalRecNum = iFilesize/COM_SIZE;
	for(iCnt=0; iCnt<iTotalRecNum; iCnt++)
	{
		InitCommData();
		memset(&PosCom, 0, sizeof(PosCom));
		ucRet = ReadIccFailureFile(iCnt);
		if( ucRet!=OK )
		{
			return ucRet;
		}

		if (!(PosCom.stTrans.ucSwipeFlag & CARD_INSERTED))
		{
			continue;
		}
		
		if( PosCom.stTrans.ucBatchSendFlag==TRUE )
		{	//
			(*piTotalNum)++;
			continue;
		}
		
		iTransTypeBak = PosCom.stTrans.iTransNo;
		stTemp.iTransNo = ICC_FAIL_BATCHUP;
		PosCom.stTrans.iTransNo = stTemp.iTransNo;
//		PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;
		SetCommReqField((uint8_t *)"0320", (uint8_t *)"");
		BcdToAsc0(glSendPack.szTranAmt, PosCom.stTrans.sAmount, 12);
		sprintf((char *)glSendPack.szEntryMode, "%3.3s0",  PosCom.stTrans.szEntryMode);
		if( PosCom.stTrans.bPanSeqNoOk )	// ADVT case 43 
		{
			sprintf((char *)glSendPack.szPanSeqNo, "%0*X", LEN_PAN_SEQ_NO, PosCom.stTrans.ucPanSeqNo);
		}
		else
		{
			strcpy((char *)glSendPack.szPanSeqNo, "000");			
		}
				
		ShortToByte(glSendPack.sICCData, PosCom.stTrans.iIccDataLen);
		memcpy(&glSendPack.sICCData[2], PosCom.stTrans.sIccData, PosCom.stTrans.iIccDataLen);		
		memcpy(&glSendPack.sICCData[5], PosCom.stTrans.sAppCrypto, 8); // 改变9F26
		memcpy(&glSendPack.sICCData[5+8+3], "\x00", 1); // 改变9F27
		
		//20080804 不平送205，平送203
		if( ucSendFlag!=EMVLOG )
			memcpy((char *)&glSendPack.szField60[8], "206", 3);
		memcpy(glSendPack.sField62, "\x00\x17\x37\x31", 4);
		sprintf((char *)&glSendPack.sField62[4], "0011%s15610", glSendPack.szTranAmt);
		glSendPack.szField60[13] = 0; // 60.6-7不送
		sprintf((char *)glSendPack.szPan,     "%.*s", LEN_PAN,      PosCom.stTrans.szCardNo);
		ucRet = SendRecvPacket();
		if( ucRet!=OK )
		{
			return ucRet;
		}
		
		PosCom.stTrans.ucBatchSendFlag = TRUE;
		PosCom.stTrans.iTransNo = iTransTypeBak;
		ucRet = UpDateIccFailureFile(iCnt);
		if( ucRet!=OK )
		{
			return ucRet;
		}
		(*piTotalNum)++;
	}
	
	return OK;
}

// end of file
