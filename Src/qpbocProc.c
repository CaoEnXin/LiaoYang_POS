
#include "qpbocProc.h"
#include "MathLib.h"
#include "PosLib.h"
#include "glbvar.h"
#include "tranfunclist.h"
#include "New8110_ICC.h"

/********************** Internal macros declaration ************************/
#define FILE_QPBOC_PARAM    "qbpoc.log"
/********************** Internal structure declaration *********************/
// 余额查询/消费/预授权,55域EMV标签
static Def55EmvTag stIccFailTxnTagList[] =
{
	{0x9F26, DE55_OPT_SET},
	{0x9F27, DE55_OPT_SET},
	{0x9F10, DE55_OPT_SET},
	{0x9F37, DE55_OPT_SET},
	{0x9F36, DE55_OPT_SET},
	{0x95,   DE55_OPT_SET},
	{0x9A,   DE55_OPT_SET},
	{0x9C,   DE55_OPT_SET},
	{0x9F02, DE55_OPT_SET},
	{0x5F2A, DE55_OPT_SET},
	{0x82,   DE55_OPT_SET},
	{0x9F1A, DE55_OPT_SET},
	{0x9F03, DE55_OPT_SET},
	{0x9F33, DE55_OPT_SET},
	{0x9F34, DE55_OPT_SET},
	{0x9F35, DE55_OPT_SET},
	{0x9F1E, DE55_COND_SET},
	{0x84,   DE55_OPT_SET},
	{0x9F09, DE55_OPT_SET},
	{0x9F41, DE55_OPT_SET},
	{0x9F63, DE55_COND_SET},
	{0x9F74, DE55_COND_SET},  // 电子现金类的
	{NULL_TAG_1},
};
/********************** Internal variables declaration *********************/
//struct qpboc_parameters  gstQpbocParamters;
/********************** Internal functions declaration *********************/
/********************** external reference declaration *********************/
extern int(*__ICCARD_EXCHANGE_APDU)(int fd, uint32_t sendlen, const void *senddata, uint32_t *recvlen, void *recvdat);
/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

/*
int    LoadQpbocParam(void)
{
	int fd;
	int ret;
	
	fd = fileOpen(FILE_QPBOC_PARAM, O_RDWR);
	if( fd<0 ) 
	{
		DispFileErrInfo();
		return (E_MEM_ERR);
	}
	ret = fileSeek(fd, (int)0, SEEK_SET);
	if( ret<0 ) 
	{
		DispFileErrInfo();
		fileClose(fd);
		return (E_MEM_ERR);
	}
	
	ret = fileRead(fd, (uint8_t *)&gstQpbocParamters, sizeof(struct qpboc_parameters));
	fileClose(fd);
	
	if( ret!=sizeof(struct qpboc_parameters) )
	{
		DispFileErrInfo();
		return (E_MEM_ERR);
	}
	
	return (OK);
}

void  LoadDefaultQpbocParam(void)
{
	memset(&gstQpbocParamters,0,sizeof(gstQpbocParamters));

	memcpy(gstQpbocParamters.m_TransactionProperty, "\xA6\x00", 2);
	gstQpbocParamters.m_ReaderMaxTransAmount = 2000;
	gstQpbocParamters.m_FloorLimiteAmount    = 1000;
	gstQpbocParamters.m_CVMAmount			= 500;
	gstQpbocParamters.m_StatusCheckSupported = 0;

	SaveQpbocParam();

}

int    SaveQpbocParam(void)
{
	int fd;
	int ret;
	
	fd = fileOpen(FILE_QPBOC_PARAM, O_RDWR|O_CREAT);
	if( fd<0 ) 
	{
		DispFileErrInfo();
		return (E_MEM_ERR);
	}
	
	ret = fileSeek(fd, (int)0, SEEK_SET);
	if( ret<0 ) 
	{
		DispFileErrInfo();
		fileClose(fd);
		return (E_MEM_ERR);
	}
	ret = fileWrite(fd, (uint8_t *)&gstQpbocParamters, sizeof(struct  qpboc_parameters));
	if( ret!=sizeof(struct  qpboc_parameters) )
	{
		DispFileErrInfo();
		fileClose(fd);	
		return (E_MEM_ERR);
	}
	fileClose(fd);
	return (OK);
} */

void   DisplayQpbocError(int iErrorCode)
{	
	uint8_t Buffer[255];
	uint8_t uiSize;
	char szTemp[255];

	lcdCls();
	switch (iErrorCode)
	{
		case EMV_QPBOC_OK:
			lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"交易成功");
			break;
 		case EMV_QPBOC_TRANSACTION_DECLIEN:
			lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"交易拒绝");
			break;
		case EMV_QPBOC_CARD_EXPIRED:
			lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"交易拒绝");
			lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"卡片已过期");
			break;
		
		case EMV_QPBOC_APP_BLOCKED:
			lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"应用被锁定");
			break;
		case EMV_QPBOC_CONNECT_TRANSACTION_NEEDED:
			lcdDisplay(0,2, DISP_CFONT|DISP_MEDIACY, "交易终止"); 
			lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"请使用其它界面");
			break;
			
		case EMV_QPBOC_NEED_PBOC_TRANSACTION:
			lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"按非接触式PBOC交易");
			break;
		
		case EMV_QPBOC_NEED_RETRY:
			lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"请重试");
			break;
		case EMV_QPBOC_CANCELED:
//		case EMV_QPBOC_TRANSACTION_TERMINATED:
		case EMV_QPBOC_ICC_ERROR:
		case EMV_QPBOC_INVALID_DATA:
		case EMV_QPBOC_DATA_MISSING:
 		case EMV_QPBOC_DATA_DUPLICATE:
		case EMV_QPBOC_MEMORY_OVERFLOW:
		case EMV_QPBOC_MEMORY_NO_ENOUGH:
		case EMV_QPBOC_PROGRAMING_ERROR:
			lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"交易终止");
			break;
		case EMV_QPBOC_TRANSACTION_TERMINATED:
			lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"交易拒绝");
			break;
	}

	memset(Buffer, 0, sizeof(Buffer));
	uiSize = sizeof(Buffer);
	//Tag = 0x9F5D;
	memset(Buffer, 0, sizeof(Buffer));
	if (EMV_QPBOC_TRANSACTION_DECLIEN == iErrorCode)
	{
		if (0 == emv_get_data_element("\x9F\x5D", 2, &uiSize, Buffer))
		{
			memset(szTemp,0,sizeof(szTemp));
			PubBcd2Asc0(Buffer,uiSize,szTemp);
			PubAddHeadChars(szTemp,12,'0');
			memset(Buffer, 0, sizeof(Buffer));
			PubConvAmount(NULL,(uint8_t*)szTemp,2,Buffer,0);
			lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"脱机余额:%s", Buffer);
		}
	}

	PubBeepErr();
	PubWaitKey(5);
	return;
}


void   DisplayQpbocBalance(void)
{
	uint8_t Buffer[255];
	uint8_t uiSize;
	char szTemp[255];
		
	memset(Buffer, 0, sizeof(Buffer));
	uiSize = sizeof(Buffer);
	//Tag = 0x9F5D;
	memset(Buffer, 0, sizeof(Buffer));
	if (0 == emv_get_data_element("\x9F\x5D", 2, &uiSize, Buffer))
	{
		memset(szTemp,0,sizeof(szTemp));
		PubBcd2Asc0(Buffer,uiSize,szTemp);
		PubAddHeadChars(szTemp,12,'0');
		memset(Buffer, 0, sizeof(Buffer));
		PubConvAmount(NULL,(uint8_t*)szTemp,2,Buffer,0);
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"脱机余额:%s", Buffer);
	}
}


uint8_t ProcQpbocTran(void)
{
	int	iRet, iNeedSaveLog;

	PosCom.bOnlineTxn = FALSE; // 先设置成没有联机 
	sprintf((char *)PosCom.szRespCode, "00");
	PosCom.stTrans.iTransNo = stTemp.iTransNo;
	PosCom.stTrans.ucSwipeFlag = PosCom.ucSwipedFlag;	// 保存刷卡标志

	iNeedSaveLog = FALSE;
	GetAndSetQpbocElement();

	iRet = emv_qpboc_complete();
	if ( iRet != EMV_QPBOC_OK)
	{
		iNeedSaveLog = emv_qpboc_need_save_failed_log();
		if (iNeedSaveLog) // 保存错误日记
		{
			GetAndSetQpbocElement();
			SaveIccFailureTranData();
			SaveIccFailureFile();
		}

		DisplayQpbocError(iRet);
		return NO_DISP;
	}

	GetAndSetQpbocElement();
	if( PosCom.bOnlineTxn && memcmp(PosCom.szRespCode, "00", 2)!=0 )
	{	
		return AfterCommProc();
	}

	if( PosCom.bOnlineTxn)
	{
		fileRemove(REVERSAL_FILE);
	}

	SaveEmvTranData();

	iRet = AfterCommProc();
	return iRet;
}


// 电子现金快速支付 走qpboc消费流程
uint8_t TransEcQuickSale(void)
{
	uint8_t	ucRet;
	
	while(1)
	{
		if (memcmp(PosCom.stTrans.sAmount,"\x00\x00\x00\x00\x00\x00",6) != 0)
		{
			break;
		}

		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "电子现金快速消费", "      SALE      ");
		ucRet = AppGetAmount(9, TRAN_AMOUNT);
		if( ucRet!=OK )
		{
			return ucRet;
		}
		
//		if( (stPosParam.iEcMaxAmount!=0) &&
//			(BcdToLong(PosCom.stTrans.sAmount, 6)>stPosParam.iEcMaxAmount) )
/*		iAmount = BcdToLong(PosCom.stTrans.sAmount, 6);
		if (CheckQpbocMaxAmount(iAmount))
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  输入金额无效  ", "INVALID AMOUNT");
			DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, " 超电子现金额度 ", "EXLIMITED");
			ErrorBeep();
			kbGetKeyMs(2000);		
			continue;
		}*/
		
		break;
	}
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "电子现金快速消费", "      SALE      ");
	ucRet = PosGetCard(CARD_PASSIVE);
	if( ucRet!=OK )
	{
		__ICCARD_EXCHANGE_APDU = New8110_icc_exchange_apdu; // 切换回调函数
		return ucRet;
	}

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "电子现金快速消费", "      SALE      ");
	SetCommReqField((uint8_t *)"0200", (uint8_t *)"000000");
	// 先将离线交易的卡类型固定成内卡
	strcpy((char *)PosCom.stTrans.szCardUnit, "CUP");
	ucRet = OnlineCommProc();   // 
	__ICCARD_EXCHANGE_APDU = New8110_icc_exchange_apdu;  // 切换回调函数
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;
}


uint8_t CheckQpbocMaxAmount(uint32_t uiTxnAmount)
{
/*	int  iCnt;

	for (iCnt=0; iCnt< MAX_APP_NUM; iCnt++)
	{
		if (uiTxnAmount <= glTermAidList[iCnt].m_ReaderMaxTransAmount)
		{
			return FALSE;
		}
	}

	return TRUE;*/
	return FALSE;
}


uint32_t GetMaxQpbocCfgAmount(void)
{
//	int  iCnt;
	uint32_t uiMaxAmount = 0;
	
/*	for (iCnt=0; iCnt< MAX_APP_NUM; iCnt++)
	{
		if (uiMaxAmount <= glTermAidList[iCnt].m_ReaderMaxTransAmount)
		{
			uiMaxAmount = glTermAidList[iCnt].m_ReaderMaxTransAmount;
		}
	}*/
	
	return uiMaxAmount;
}


uint8_t  TransQpbocPreProc(void)
{
	int ierrorno = EMV_QPBOC_OK;
	struct emv_core_init Init;
 //   EMV_APP_TYPE AppType;	
	uint8_t TransType;
	uint32_t Amount, iReadCnt;
	int  iOldTime,iNowTime, istatus;
	uint8_t  szBuffer[200];
	uint8_t  uLen;
	int      iCnt;
	struct qpboc_parameters  gstQpbocParamters;

	istatus = 0;
	TransType = 0x00;
	memcpy(gstQpbocParamters.m_TransactionProperty, "\xA6\x00", 2);
//	gstQpbocParamters.m_ReaderMaxTransAmount = GetMaxQpbocCfgAmount();
// 	if (gstQpbocParamters.m_ReaderMaxTransAmount == 0)
// 	{
// 		gstQpbocParamters.m_ReaderMaxTransAmount = 10000;
// 	}
//	gstQpbocParamters.m_FloorLimiteAmount    = 1000;
//	gstQpbocParamters.m_CVMAmount			= 500;
	gstQpbocParamters.m_StatusCheckSupported = 0;
	ierrorno = emv_qpboc_set_parameter(&gstQpbocParamters);
	if (EMV_QPBOC_OK != ierrorno)
	{
		lcdCls();
		lcdDisplay(0 ,3 , DISP_CFONT | DISP_MEDIACY , "设置参数失败");
		kbGetKey();
		return NO_DISP;
	}

	emv_get_core_init_parameter(&Init);	
	switch(Init.transaction_type)
	{
	  case EMV_CASH:     TransType = 0x01; break;
	  case EMV_CASHBACK: TransType = 0x09; break;
	  case EMV_GOODS:    TransType = 0x00; break;
	  case EMV_SERVICE:  TransType = 0x00; break;
	  case EMV_INQUIRY:  TransType = 0x10; break;
	  case EMV_TRANSFER: TransType = 0x11; break;
	  case EMV_PAYMENT:  TransType = 0x12; break;
	  case EMV_ADMIN:    TransType = 0x13; break;
	}

	Amount = PubBcd2Long(PosCom.stTrans.sAmount,6,NULL);
//	contactness_poll(imif_fd);
	iOldTime = sysGetTimerCount();
	while(1)
	{
		iNowTime= sysGetTimerCount();
		if( (iNowTime-iOldTime) >= (uint32_t)(1000) )	// 检查定时器
		{
			lcdClrLine(2,7);
			lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"请重试");
			PubBeepErr();
			PubWaitKey(5);
			return NO_DISP;
		}

		istatus = contactness_poll(imif_fd);
		if (istatus == 0)
		{
			break;
		}
//		usleep(10*1000);
	}

	ierrorno = emv_qpboc_pre_transaction(stPosParam.lNowTraceNo, TransType, Amount);
	if (ierrorno != EMV_QPBOC_OK)
	{
		DisplayQpbocError(ierrorno);
		return NO_DISP;
	}

	iReadCnt = 0;
	while(1)
	{
		iReadCnt++;
		if (iReadCnt >= 3)
		{
			DisplayQpbocError(ierrorno);
			return NO_DISP;
		}
		__ICCARD_EXCHANGE_APDU = contactness_exchangeapdu;
		emv_set_data_element("\x9F\x39",2,1, "\x07");		
		ierrorno = emv_qpboc_read_data(imif_fd);
		if (EMV_QPBOC_NEED_RETRY == ierrorno)
		{
			continue;
		}

		break;
	}

	if (ierrorno != EMV_QPBOC_OK)
	{
		contactness_close(imif_fd);
		imif_fd = -1;
		DisplayQpbocError(ierrorno);
		return NO_DISP;		
	}

	memset(szBuffer,0,sizeof(szBuffer));
	memset(PosCom.stTrans.szCardNo,0,sizeof(PosCom.stTrans.szCardNo));
	emv_get_data_element("\x5A", 1, &uLen, szBuffer);
	PubBcd2Asc((char*)szBuffer,uLen,(char*)PosCom.stTrans.szCardNo);
	PubTrimTailChars((char*)PosCom.stTrans.szCardNo,'F');
//	printf("read qpboc card 2:%s \n",PosCom.stTrans.szCardNo);
	
	memset(szBuffer,0,sizeof(szBuffer));
	emv_get_data_element("\x4F", 1, &uLen, szBuffer);	// AID
	BcdToAsc0(PosCom.stTrans.szAID, (uint8_t*)szBuffer, uLen*2);
	RemoveTailChars(PosCom.stTrans.szAID, 'F');
//	printf("read qpboc aid 2:%s \n",PosCom.stTrans.szAID);
	
	for (iCnt=0; iCnt< MAX_APP_NUM; iCnt++)
	{
		if (memcmp(szBuffer,glTermAidList[iCnt].sAID,uLen) == 0)
		{
			break;
		}
	}
	if (iCnt >= MAX_APP_NUM)
	{
		memcpy(gstQpbocParamters.m_TransactionProperty, "\xA6\x00", 2);
// 		gstQpbocParamters.m_ReaderMaxTransAmount = 10000;
// 		gstQpbocParamters.m_FloorLimiteAmount    = 1000;
// 		gstQpbocParamters.m_CVMAmount			= 500;
		gstQpbocParamters.m_StatusCheckSupported = 0;
	}
	else
	{
		memcpy(gstQpbocParamters.m_TransactionProperty, "\xA6\x00", 2);
// 		gstQpbocParamters.m_ReaderMaxTransAmount = glTermAidList[iCnt].m_ReaderMaxTransAmount;
// 		gstQpbocParamters.m_FloorLimiteAmount    = glTermAidList[iCnt].m_FloorLimiteAmount;
// 		gstQpbocParamters.m_CVMAmount			= glTermAidList[iCnt].m_CVMAmount;
		gstQpbocParamters.m_StatusCheckSupported = 0;
	}

/*	if (Amount > gstQpbocParamters.m_ReaderMaxTransAmount)
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  输入金额无效  ", "INVALID AMOUNT");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, " 超电子现金额度 ", "EXLIMITED");
		ErrorBeep();
		return NO_DISP;
	}*/

	ierrorno = emv_qpboc_set_parameter(&gstQpbocParamters);
	if (EMV_QPBOC_OK != ierrorno)
	{
		lcdCls();
		lcdDisplay(0 ,3 , DISP_CFONT | DISP_MEDIACY , "设置参数失败");
		kbGetKey();
		return NO_DISP;
	}

	if (ChkIfBlkCard((char*)PosCom.stTrans.szCardNo))
	{
		lcdClrLine(2,7);
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"该卡被列入黑名单");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
		
	return 0;
}


uint8_t  GetAndSetQpbocElement(void)
{
	int	iRet, iErrNo, i;
	uint8_t  uLen;
	char     szBuffer[100];

	emv_get_data_element("\x9F\x26", 2, &uLen, PosCom.stTrans.sAppCrypto);
	emv_get_data_element("\x8A", 1, &uLen, PosCom.stTrans.sAuthRspCode);
	emv_get_data_element("\x95", 1, &uLen, PosCom.stTrans.sTVR);
	emv_get_data_element("\x9B", 1, &uLen, PosCom.stTrans.sTSI);
	emv_get_data_element("\x9F\x36", 2, &uLen, PosCom.stTrans.sATC);
	PosCom.stTrans.bPanSeqNoOk = FALSE;
	iRet = emv_get_data_element("\x5F\x34", 2, &uLen, &PosCom.stTrans.ucPanSeqNo);
	iErrNo = iRet?errno:0;
	if( iErrNo==EMV_RET_SUCCESS )
	{
		PosCom.stTrans.bPanSeqNoOk = TRUE;
	}
	emv_get_data_element("\x50", 1, &uLen, PosCom.stTrans.szAppLable);
	memset(PosCom.stTrans.sLoyaltAmount,0,sizeof(PosCom.stTrans.sLoyaltAmount));
	emv_get_data_element("\x9F\x5D", 2, &uLen, szBuffer);
	if (uLen <= 6)
		memcpy(PosCom.stTrans.sLoyaltAmount+(6-uLen),szBuffer,uLen);
	else
		memcpy(PosCom.stTrans.sLoyaltAmount,szBuffer,6);
	emv_get_data_element("\x9F\x41", 2, &uLen, PosCom.stTrans.sPsamTxnSeq); // 交易序列号
	memset(szBuffer,0,sizeof(szBuffer));
	memset(PosCom.stTrans.szCardNo,0,sizeof(PosCom.stTrans.szCardNo));
	iErrNo = emv_get_data_element("\x5A", 1, &uLen, szBuffer);
	if (iErrNo == EMV_RET_SUCCESS)
	{
		PubBcd2Asc(szBuffer,uLen,(char*)PosCom.stTrans.szCardNo);
		PubTrimTailChars((char*)PosCom.stTrans.szCardNo,'F');
	}

	if (iErrNo != EMV_RET_SUCCESS)
	{
		memset(szBuffer,0,sizeof(szBuffer));
		iRet = emv_get_data_element("\x57", 1, &uLen, szBuffer);
		iErrNo = iRet?errno:0;
		if( iErrNo==EMV_RET_SUCCESS )
		{
			BcdToAsc0(PosCom.szTrack2, (uint8_t*)szBuffer, uLen*2);
			RemoveTailChars(PosCom.szTrack2, 'F');	// erase padded 'F' chars
			for(i=0; PosCom.szTrack2[i]!='\0'; i++)		// convert 'D' to '='
			{
				if( PosCom.szTrack2[i]=='D' )
				{
					PosCom.szTrack2[i] = '=';
					memcpy(PosCom.stTrans.szCardNo,PosCom.szTrack2,i);
					break;
				}
			}
		}
	}

	memset(szBuffer,0,sizeof(szBuffer));
	iRet = emv_get_data_element("\x4F", 1, &uLen, szBuffer);	// AID
	BcdToAsc0(PosCom.stTrans.szAID, (uint8_t*)szBuffer, uLen*2);
	RemoveTailChars(PosCom.stTrans.szAID, 'F');
	memcpy(PosCom.stTrans.szEntryMode, "072", 3);

	iRet = emv_get_data_element("\x9F\x27", 2, &uLen, szBuffer);
	iErrNo = iRet?errno:0;
	if( iErrNo != EMV_RET_SUCCESS )
	{
		memset(szBuffer,0,sizeof(szBuffer));
		szBuffer[0] = 0x40; // 没有就设置成40
		emv_set_data_element("\x9F\x27", 2, 1, szBuffer);
	}

	iRet = emv_get_data_element("\x9F\x10", 2, &uLen, szBuffer);
	iErrNo = iRet?errno:0;
	if( iErrNo!=EMV_RET_SUCCESS )
	{
		memset(szBuffer,0,sizeof(szBuffer));
		emv_set_data_element("\x9F\x10", 2, 8, szBuffer);
	}

	iRet = emv_get_data_element("\x9F\x37", 2, &uLen, szBuffer);
	iErrNo = iRet?errno:0;
	if( iErrNo!=EMV_RET_SUCCESS )
	{
		memset(szBuffer,0,sizeof(szBuffer));
		emv_set_data_element("\x9F\x37", 2, 4, szBuffer);
	}

	iRet = emv_get_data_element("\x95", 1, &uLen, szBuffer);
	iErrNo = iRet?errno:0;
	if( iErrNo!=EMV_RET_SUCCESS )
	{
		memset(szBuffer,0,sizeof(szBuffer));
		emv_set_data_element("\x95", 1, 5, szBuffer);
	}

	return 0;
}

void     SaveIccFailureTranData(void)
{
	int	    iCnt, iRet, iErrNo;
	uint8_t iTemp, iLength;
	uint8_t sBuff[100], sTemp[100];
		
	iCnt = 0;
	while(stIccFailTxnTagList[iCnt].uiTagNum != NULL_TAG_1)
	{
		memset(sBuff, 0, sizeof(sBuff));
		memset(sTemp, 0, sizeof(sTemp));
		if( stIccFailTxnTagList[iCnt].uiTagNum>0xFF )
		{
			sTemp[0] = (uint8_t)(stIccFailTxnTagList[iCnt].uiTagNum>>8);
			sTemp[1] = (uint8_t)(stIccFailTxnTagList[iCnt].uiTagNum);
			iTemp = 2;
		}
		else
		{
			sTemp[0] = (uint8_t)(stIccFailTxnTagList[iCnt].uiTagNum);
			iTemp = 1;
		}

		iRet = emv_get_data_element(sTemp, iTemp, &iLength, sBuff);
		iErrNo = iRet?errno:0;
		if( iErrNo!=EMV_RET_SUCCESS )
		{
			if (stIccFailTxnTagList[iCnt].uiTagNum == 0x9F74)
			{
				iCnt++;
				continue;
			}

			if (stIccFailTxnTagList[iCnt].uiTagNum == 0x9F63)  // 没有也不送
			{
				iCnt++;
				continue;
			}

			memset(sBuff,0,sizeof(sBuff));
			switch(stIccFailTxnTagList[iCnt].uiTagNum)
			{
			case 0x9F26:  iLength = 8; break;
			case 0x9F27:  iLength = 1; break;
			case 0x9F10:  iLength = 8; break;
			case 0x9F37:  iLength = 4; break;
			case 0x9F36:  iLength = 2; break;
			case 0x95:  iLength = 5; break;
			case 0x9A:  iLength = 6; break;
			case 0x9C:  iLength = 2; break;
			case 0x9F02:  iLength = 12; break;
			case 0x5F2A:  iLength = 3; break;
			case 0x82:  iLength = 2; break;
			case 0x9F1A:  iLength = 3; break;
			case 0x9F03:  iLength = 12; break;
			case 0x9F33:  iLength = 3; break;
			case 0x9F34:  iLength = 3; break;
			case 0x9F35:  iLength = 2; break;
			case 0x9F1E:  iLength = 8; break;
			case 0x84:  iLength = 8; break;
			case 0x9F09:  iLength = 2; break;
			case 0x9F41:  iLength = 2; break;
			case 0x9F63:  iLength = 16; break;
			case 0x9F74:  iLength = 6; break;
			default:
				break;
			}
		}

		emv_set_data_element(sTemp, iTemp, iLength, sBuff);
		iCnt++;
	}

	iCnt = 0;
	SetOnlineTranDE55(stIccFailTxnTagList, PosCom.stTrans.sIccData, &iCnt);
	PosCom.stTrans.iIccDataLen = (uint8_t)iCnt;
	strcpy((char*)PosCom.stTrans.szEntryMode,"072");
	if (PosCom.ucPinEntry)
	{
		strcpy((char*)PosCom.stTrans.szEntryMode,"071");
	}

	if (strlen((char*)PosCom.stTrans.szCardNo)==0)
	{
		strcpy((char*)PosCom.stTrans.szCardNo,"0000000000000000");
	}

	PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;
	IncreaseTraceNo();
}

uint8_t SaveIccFailureFile(void)
{
	int		fd;
	int     ret;
	
	fd = fileOpen(ICC_FAILUR_TXN_FILE, O_RDWR|O_CREAT);
	if( fd<0 )
	{
		return E_MEM_ERR;
	}

	ret = fileSeek(fd, 0, SEEK_END);
	if( ret<0 ) 
	{
		DispFileErrInfo();
		fileClose(fd);
		return (E_FILE_SEEK);
	}
	
	if( fileWrite(fd, (uint8_t *)&PosCom, COM_SIZE)!=COM_SIZE )
	{
		fileClose(fd);
		return E_MEM_ERR;
	}
	fileClose(fd);
	
	return OK;
}


uint8_t ReadIccFailureFile(uint32_t uiIndex)
{
	int		fd, ret;
	int     iFilesize = 0;
	
	iFilesize = fileSize(ICC_FAILUR_TXN_FILE);
	if (iFilesize < 0)
	{
		iFilesize = 0;
	}

	if (iFilesize <= uiIndex*COM_SIZE)
	{
		return E_MEM_ERR;
	}

	fd = fileOpen(ICC_FAILUR_TXN_FILE, O_RDWR);
	if( fd<0 )
	{
		return E_MEM_ERR;
	}

	ret = fileSeek(fd, (int)(uiIndex*COM_SIZE), SEEK_SET);
	if( ret<0 ) 
	{
		fileClose(fd);
		return (E_MEM_ERR);
	}
	
	if( fileRead(fd, (uint8_t *)&PosCom, COM_SIZE)!=COM_SIZE )
	{
		fileClose(fd);
		return E_MEM_ERR;
	}
	fileClose(fd);
	
	return OK;
}


uint8_t UpDateIccFailureFile(uint32_t uiIndex)
{
	int		iRet, iLogFile;
	
	iLogFile = fileOpen(ICC_FAILUR_TXN_FILE, O_RDWR);
	if( iLogFile<0 )
	{
		return E_MEM_ERR;
	}
	
	iRet = fileSeek(iLogFile, (int)(uiIndex * COM_SIZE), SEEK_SET);
	if( iRet<0 )
	{
		fileClose(iLogFile);
		return E_MEM_ERR;
	}
	
	iRet = fileWrite(iLogFile, (uint8_t *)&PosCom, COM_SIZE);
	fileClose(iLogFile);
	if( iRet!=COM_SIZE)
	{
		return E_MEM_ERR;
	}
	
	return OK;
}

uint8_t ViewIccFailureLog(void)
{
	int		iRet, iCurRec, iTotalRecNum, iAtcNum;
	int     iFilesize = 0;
	uint8_t ucRet;
	char    szBuffer[100];
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  失败交易查询  ", "  QUERY TRANS   ");

	iFilesize = fileSize(ICC_FAILUR_TXN_FILE);
	if (iFilesize <=0 )
	{
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"无失败交易");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
	
	iTotalRecNum = iFilesize/(sizeof(PosCom));
	iCurRec=0;
    while(iCurRec<iTotalRecNum)
	{
		memset(&PosCom, 0, sizeof(PosCom));
		ucRet = ReadIccFailureFile(iCurRec);
		if( ucRet!=OK )
		{
			return ucRet;
		}

		lcdCls();
		if (PosCom.stTrans.ucSwipeFlag & CARD_INSERTED)
		{
			lcdDisplay(0,0,DISP_CFONT,(char*)"消费");
		}
		else
		{
			lcdDisplay(0,0,DISP_CFONT,(char*)"电子现金快速消费");
		}
		memset(szBuffer,0x00,sizeof(szBuffer));
		ConvBcdAmount(PosCom.stTrans.sAmount,(uint8_t*)szBuffer);
		lcdDisplay(0,2,DISP_CFONT,"金额:%s",szBuffer);
		memset(szBuffer,0x00,sizeof(szBuffer));		
		lcdDisplay(0,4,DISP_ASCII,(char*)PosCom.stTrans.szCardNo);
		iAtcNum = PubChar2Long(PosCom.stTrans.sATC,2,NULL);
		lcdDisplay(0,6,DISP_ASCII,"ATC:%d STAN:%d",iAtcNum,PosCom.stTrans.lTraceNo);
		lcdDisplay(0,7, DISP_ASCII, "%4.4s/%2.2s/%2.2s %2.2s:%2.2s:%2.2s", PosCom.stTrans.szDate,
			PosCom.stTrans.szDate+4,PosCom.stTrans.szDate+6,
			PosCom.stTrans.szTime,PosCom.stTrans.szTime+2,PosCom.stTrans.szTime+4);
		
		iRet = PubWaitKey(60);
		if( iRet==KEY_CANCEL || iRet==KEY_INVALID )
		{
			break;
		}
		else if( iRet==KEY_DOWN || iRet== KEY_ENTER )
		{
			if (iCurRec < iTotalRecNum)
				iCurRec += 1;
			else
			{
				lcdClrLine(2,7);
				lcdDisplay(0,3,DISP_CFONT,"记 录 尾");
				PubBeepErr();
				PubWaitKey(5);
				return NO_DISP;
			}
		}
		else if( iRet==KEY_UP || iRet==KEY_CLEAR )
		{
			if (iCurRec > 1)
				iCurRec -= 1;
			else
				PubBeepErr();
		}
		else
		{
			PubBeepErr();
		}
	}
	
	return OK;
}


int  TransBatchUploadQpbocFailureTxnLog(int *piTotalNum, uint8_t ucSendFlag)
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

		if ((PosCom.stTrans.ucSwipeFlag & CARD_INSERTED))
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
		memcpy(&glSendPack.sICCData[5+8+3], "\x40", 1); // 改变9F27
		
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
