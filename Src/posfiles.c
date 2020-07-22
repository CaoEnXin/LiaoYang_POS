

#include "posapi.h"
#include "posmacro.h"
#include "terstruc.h"
#include "glbvar.h"
#include "tranfunclist.h" 

#define FILE_APP_LOG    "SysParam.log"
#define FILE_CTRL_LOG	"ctrl.log"
#define FILE_COMM_PAR	"comm.par"
#define FILE_OPER_LOG	"oper.log"
#define FILE_TOTAL_LMT	"total.lmt"
#define FILE_LAST_TOTAL	"lasttotal"
#define FILE_M1ENRICH_LAST_TOTAL	"m1enrichlasttotal"

#define FILE_EMV_AID    "emv.aid"
#define FILE_EMV_CAPK   "emv.capk"
#define FILE_BLKCARD_LOG   "BLKCARD.DAT"

uint8_t DispOrignalTrans(NEWPOS_LOG_STRC *pstLog);

uint8_t SaveCtrlParam()
{
	int fd;
	int ret;
	
	fd = fileOpen(FILE_CTRL_LOG, O_RDWR|O_CREAT);
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
	ret = fileWrite(fd, (uint8_t *)&stTransCtrl, sizeof(struct _TRANS_CTRL_PARAM));
	if( ret!=sizeof(struct _TRANS_CTRL_PARAM) )
	{
		DispFileErrInfo();
		fileClose(fd);	
		return (E_MEM_ERR);
	}
	fileClose(fd);
	return (OK);
}


uint8_t ReadCtrlParam(void)
{
	int fd;
	int ret;
	
	fd = fileOpen(FILE_CTRL_LOG, O_RDWR);
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
	
	ret = fileRead(fd, (uint8_t *)&stTransCtrl, sizeof(struct _TRANS_CTRL_PARAM));
	fileClose(fd);
	
	if( ret!=sizeof(struct _TRANS_CTRL_PARAM) )
	{
		DispFileErrInfo();
		return (E_MEM_ERR);
	}
	
	return (OK);
}



uint8_t ReadOperFile()
{
	int fd;
	int ret;
	
	fd = fileOpen(FILE_OPER_LOG, O_RDWR);
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
	ret = fileRead(fd, (uint8_t *)&operStrc, MAX_OPER_NUM*sizeof(struct _oper_structure));
	if( ret!=MAX_OPER_NUM*sizeof(struct _oper_structure) )
	{
		DispFileErrInfo();
		fileClose(fd);
		return (E_MEM_ERR);
	}
	fileClose(fd);
	
	return OK;
}

uint8_t WriteOperFile()
{
	int fd;
	int ret;
	
	fd = fileOpen(FILE_OPER_LOG, O_RDWR | O_CREAT);
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
	ret = fileWrite(fd, (uint8_t *)&operStrc, MAX_OPER_NUM*sizeof(struct _oper_structure));
	if( ret!=MAX_OPER_NUM*sizeof(struct _oper_structure) )
	{
		DispFileErrInfo();
		fileClose(fd);
		return (E_MEM_ERR);
	}
	fileClose(fd);
	return (OK);
}
uint8_t WriteEnrichFtpFile(void* pdata, int size, char* filename)
{
	//----
	int fd;
	int ret;

	fd = fileOpen(filename, O_RDWR|O_CREAT);
	if( fd<0 ) 
	{
		return (E_MEM_ERR);
	}

	ret = fileSeek(fd, (int)0, SEEK_END);
	if( ret<0 ) 
	{
		fileClose(fd);
		return (E_MEM_ERR);
	}
	ret = fileWrite(fd, (uint8_t *)pdata, size);
	if( ret!=size )
	{
		fileClose(fd);
		return (E_MEM_ERR);
	}
	fileClose(fd);
	return (OK);

	//----
}

uint8_t SaveLimitTotal(void)
{
	int fd;
	int ret;
	
	fd = fileOpen(FILE_TOTAL_LMT, O_RDWR|O_CREAT);
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
	ret = fileWrite(fd, (uint8_t *)&stLimitTotal, sizeof(struct _limit_total_strc));
	if( ret!=sizeof(struct _limit_total_strc) )
	{
		DispFileErrInfo();
		fileClose(fd);
		return (E_MEM_ERR);
	}
	fileClose(fd);
	return (OK);
}

uint8_t ReadLimitTotal(void)
{
	int fd;
	int ret;
	
	fd = fileOpen(FILE_TOTAL_LMT, O_RDWR);
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
	ret = fileRead(fd, (uint8_t *) & stLimitTotal, sizeof(struct _limit_total_strc));
	if( ret!=sizeof(struct _limit_total_strc) )
	{
		DispFileErrInfo();
		fileClose(fd);
		return (E_MEM_ERR);
	}
	fileClose(fd);
	return (OK);
}
uint8_t ReadM1EnrichLastTotal(void)
{
	int fd;
	int ret;
	
	fd = fileOpen(FILE_M1ENRICH_LAST_TOTAL, O_RDWR);
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
	ret = fileRead(fd, (uint8_t *)&stM1EnrichTotal, sizeof(struct _total_m1Enrich_strc));
	if( ret!=sizeof(struct _total_m1Enrich_strc) )
	{
		DispFileErrInfo();
		fileClose(fd);
		return (E_MEM_ERR);
	}
	fileClose(fd);
	return (OK);
}

uint8_t SaveM1EnrichLastTotal(void)
{
	int fd;
	int ret;
	
	fd = fileOpen(FILE_M1ENRICH_LAST_TOTAL, O_RDWR|O_CREAT);
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
	ret = fileWrite(fd, (uint8_t *)&stM1EnrichTotal, sizeof(struct _total_m1Enrich_strc));
	if( ret!=sizeof(struct _total_m1Enrich_strc) )
	{
		DispFileErrInfo();
		fileClose(fd);
		return (E_MEM_ERR);
	}
	fileClose(fd);
	return (OK);
}

uint8_t ReadLastTotal(void)
{
	int fd;
	int ret;
	
	fd = fileOpen(FILE_LAST_TOTAL, O_RDWR);
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
	ret = fileRead(fd, (uint8_t *)&stM1EnrichTotal, sizeof(struct _total_strc));
	if( ret!=sizeof(struct _total_strc) )
	{
		DispFileErrInfo();
		fileClose(fd);
		return (E_MEM_ERR);
	}
	fileClose(fd);
	return (OK);
}

uint8_t SaveLastTotal(void)
{
	int fd;
	int ret;
	
	fd = fileOpen(FILE_LAST_TOTAL, O_RDWR|O_CREAT);
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
	ret = fileWrite(fd, (uint8_t *)&stTotal, sizeof(struct _total_strc));
	if( ret!=sizeof(struct _total_strc) )
	{
		DispFileErrInfo();
		fileClose(fd);
		return (E_MEM_ERR);
	}
	fileClose(fd);
	return (OK);
}

void DispFileErrInfo(void)
{
	int err;
	char  buf[32];
	
	err = errno;
	
	switch(err) 
	{
	case FILE_EXIST: 
		if( stPosParam.iDispLanguage==1 )
			strcpy(buf, "文件已存在");
		else
			strcpy(buf, "FILE EXISTED");
		break;
	case FILE_NOEXIST: 
		if( stPosParam.iDispLanguage==1 )
			strcpy(buf, "文件不存在");
		else
			strcpy(buf, "FILE NO EXIST");
		break;
	case MEM_OVERFLOW: 
		if( stPosParam.iDispLanguage==1 )
			strcpy(buf, "空间不足");
		else
			strcpy(buf, "MEM OVERFLOW");
		break;
	case TOO_MANY_FILES: 
		if( stPosParam.iDispLanguage==1 )
			strcpy(buf, "文件太多");
		else
			strcpy(buf, "TOO MANY FILES");
		break;
	case INVALID_HANDLE: 
		if( stPosParam.iDispLanguage==1 )
			strcpy(buf, "无效句柄");
		else
			strcpy(buf, "INVALID HANDLE");
		break;
	case INVALID_MODE: 
		if( stPosParam.iDispLanguage==1 )
			strcpy(buf, "无效模式");
		else
			strcpy(buf, "INVALID MODE");
		break;
	case FILE_NOT_OPENED: 
		if( stPosParam.iDispLanguage==1 )
			strcpy(buf, "文件未打开");
		else
			strcpy(buf, "NOT OPENED");
		break;
	case END_OVERFLOW: 
		if( stPosParam.iDispLanguage==1 )
			strcpy(buf, "后移溢出");
		else
			strcpy(buf, "END OVERFLOW");
		break;
	case TOP_OVERFLOW: 
		if( stPosParam.iDispLanguage==1 )
			strcpy(buf, "前移溢出");
		else
			strcpy(buf, "TOP OVERFLOW");
		break;
	case NO_PERMISSION: 
		if( stPosParam.iDispLanguage==1 )
			strcpy(buf, "权限不作");
		else
			strcpy(buf, "NO_PERMISSION");
		break;
	default: 
		if( stPosParam.iDispLanguage==1 )
			strcpy(buf, "其它错误");
		else
			strcpy(buf, "OTHER");
		break;
	}
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_MEDIACY|DISP_CFONT, "    文件错误    ", "  FILE ERROR    ");
	lcdDisplay(0, 6, DISP_MEDIACY|DISP_CFONT, buf);
	FailBeep();
	kbGetKeyMs(3000);
}

uint8_t SaveLogFile(void)
{
	int		iRet, iLogFile;
	
	iLogFile = fileOpen(POS_LOG_FILE, O_RDWR|O_CREAT);
	if( iLogFile<0 )
	{
		DispFileErrInfo();
		return E_FILE_OPEN;
	}

	iRet = fileSeek(iLogFile, (int)(stTransCtrl.iTransNum*LOG_RECORD_LEN), SEEK_SET);
	if( iRet<0 ) 
	{
		DispFileErrInfo();
		fileClose(iLogFile);
		return E_FILE_SEEK;
	}

	PosCom.stTrans.uiTxnRecIndex = stTransCtrl.iTransNum;
	iRet = fileWrite(iLogFile, (uint8_t *)&PosCom.stTrans, LOG_RECORD_LEN);
	fileClose(iLogFile);
	if( iRet!=LOG_RECORD_LEN )
	{
		DispFileErrInfo();
		return E_FILE_WRITE;
	}
	
	stTransCtrl.iTransNum++;
	if( stTemp.iTransNo==POS_SALE    || stTemp.iTransNo==POS_PREAUTH ||
		stTemp.iTransNo==POS_AUTH_CM || stTemp.iTransNo==POS_QUE     ||
		stTemp.iTransNo==OFF_ADJUST  || stTemp.iTransNo==ADJUST_TIP  ||
		stTemp.iTransNo==OFF_SALE    || stTemp.iTransNo==POS_REFUND  ||
		stTemp.iTransNo==ICC_OFFSALE || stTemp.iTransNo==POS_OFF_CONFIRM ||
		(stTemp.iTransNo>=EC_QUICK_SALE && stTemp.iTransNo<=POS_LYT_LOGON))
	{	// 最后一笔交易日期 040719
		sprintf((char *)stTransCtrl.szLastTransDate, "%.8s", PosCom.stTrans.szDate);
	}
	SaveCtrlParam();

	// 保存磁道数据，用于批上送 051207
	if( stTemp.iTransNo==POS_REFUND || stTemp.iTransNo==POS_OFF_CONFIRM )
	{
		SaveRefundDataFile();
	}

	return OK;
} 

/**********************************************
仅用于保留退货的磁道信息，该信息仅用于退货批上送
**********************************************/
uint8_t SaveRefundDataFile(void)
{
	int fd, ret;
	struct _REFUND_TRACK_STRUCT  stLog;
	
	memset(&stLog, 0, sizeof(struct _REFUND_TRACK_STRUCT));
	stLog.lTraceNo = PosCom.stTrans.lTraceNo;
	memcpy(stLog.szTrack2 ,PosCom.szTrack2, TRACK2_LEN+1);
	memcpy(stLog.szTrack3 ,PosCom.szTrack3, TRACK3_LEN+1);
	strcpy((char *)stLog.szOldDate, (char *)PosCom.stTrans.szOldTxnDate);

	fd = fileOpen(REFUND_TRACK_FILE, O_RDWR|O_CREAT);
	if( fd<0 )
	{
		DispFileErrInfo();
		return(E_FILE_OPEN);
	}
	ret = fileSeek(fd, 0, SEEK_END);
	if( ret<0 ) 
	{
		DispFileErrInfo();
		fileClose(fd);
		return (E_FILE_SEEK);
	}
	ret = fileWrite(fd, (uint8_t *)&stLog, sizeof(struct _REFUND_TRACK_STRUCT));
	fileClose(fd);
	if( ret!=sizeof(struct _REFUND_TRACK_STRUCT) ) 
	{
		DispFileErrInfo();
		return(E_FILE_WRITE);
	}
	return(OK);
} 

uint8_t CheckRefundDataFile(int lTrace)
{
	int  fd, ret;
	struct _REFUND_TRACK_STRUCT  stLog;
	
	memset(&stLog, 0, sizeof(struct _REFUND_TRACK_STRUCT));

	fd = fileOpen(REFUND_TRACK_FILE, O_RDWR);
	if( fd<0 )
	{
		return OK;
	}
	ret = fileSeek(fd, 0, SEEK_SET);
	if( ret<0 ) 
	{
		DispFileErrInfo();
		fileClose(fd);
		return (E_FILE_SEEK);
	}
	while(1) 
	{
		ret = fileRead(fd, (uint8_t *)&stLog, sizeof(struct _REFUND_TRACK_STRUCT));
		if( ret!=sizeof(struct _REFUND_TRACK_STRUCT) ) 
		{
			fileClose(fd);
			DispFileErrInfo();
			return(E_FILE_READ);
		}
		if( stLog.lTraceNo==lTrace )
		{
			memcpy(PosCom.szTrack2, stLog.szTrack2, TRACK2_LEN+1);
			memcpy(PosCom.szTrack3, stLog.szTrack3, TRACK3_LEN+1);
			strcpy((char *)PosCom.stTrans.szOldTxnDate, (char *)stLog.szOldDate);
			break;
		}
	}
	fileClose(fd);
	return(OK);
} 

/* 撤销、调整等交易更新日志中原流水 */
uint8_t   UpdateLogFile()
{
	int  ret, fd;
	NEWPOS_LOG_STRC stLog;
	
	fd = fileOpen(POS_LOG_FILE, O_RDWR);
	if( fd<0 )
	{
		return(E_MEM_ERR);
	}
	ret = fileSeek(fd, (int)(stTemp.iLogFilePostion*LOG_RECORD_LEN), SEEK_SET);
	if( ret<0 ) 
	{
		fileClose(fd);
		return (E_MEM_ERR);
	}
	
   	ret = fileRead(fd, (uint8_t *)&stLog, LOG_RECORD_LEN);
	if( ret!=LOG_RECORD_LEN ) 
	{
		fileClose(fd);
		return(E_MEM_ERR);
	}
	if( stTemp.iTransNo==ADJUST_TIP )  //
		stLog.ucAdjusted = TRUE;
	else if( stTemp.iTransNo==OFF_ADJUST )
	{
		if( stLog.iTransNo!=OFF_SALE ) 
		{
			fileClose(fd);
			return(OK);
		}
		//离线结算的调整只能调整一次,如果交易已经上送不修改原金额
		if( stLog.ucSendFlag!=TRUE )
		{
			memcpy(stLog.sAmount, PosCom.stTrans.sAmount, 6);
			stLog.ucAdjusted = TRUE;
			stLog.iTransNo = OFF_SALE;
			stLog.ucChgFrmAdjust = TRUE; /*表明本笔交易是由离线结算改变而来*/
			strcpy((char *)stLog.szDate, (char *)PosCom.stTrans.szDate); //更新LOG离线结算的时间
			strcpy((char *)stLog.szTime, (char *)PosCom.stTrans.szTime);
		}
		else
		{
			stLog.ucAdjusted = TRUE;
		}
	}
	else 
	{
		stLog.ucVoidId = TRUE; ///更改被撤销交易
	}
	
	///写入到原交易的位置
	ret = fileSeek(fd, (int)(stTemp.iLogFilePostion*LOG_RECORD_LEN), SEEK_SET);
	if( ret<0 ) 
	{
		fileClose(fd);
		return (E_MEM_ERR);
	}
    ret = fileWrite(fd, (uint8_t *)&stLog, LOG_RECORD_LEN);
	if( ret!=LOG_RECORD_LEN )
	{
		fileClose(fd);
		return(E_MEM_ERR);
	}
		
	fileClose(fd);
	return(OK);
}


/*查询是否有原交易*/
uint8_t CheckData(int lPosStan, NEWPOS_LOG_STRC *pstOutLog)
{
	uint8_t		    ucRet;
    int			iCnt;
	NEWPOS_LOG_STRC	stLog;
	
	if( stTransCtrl.iTransNum==0 )
	{
		return E_NO_TRANS;
	}

	for(iCnt=0; iCnt<stTransCtrl.iTransNum; iCnt++)
	{
		memset(&stLog, 0, sizeof(stLog));
		ucRet = LoadTranLog(iCnt, &stLog);
		if( ucRet!=OK )
		{
			return ucRet;
		}
		if( lPosStan==stLog.lTraceNo )
		{
			break;
		}
	}
	if( iCnt>=stTransCtrl.iTransNum )
	{
		return E_NO_OLD_TRANS;
	}

	if( stLog.ucVoidId==TRUE )
	{
		return E_TRANS_VOIDED;
	}
	if( stLog.ucAdjusted==TRUE )
	{
		return E_TRANS_HAVE_ADJUESTED;		
	}
	if( stLog.ucSendFlag!=TRUE )
	{
		stTemp.bSendId = FALSE;	
	}

	ucRet = DispOrignalTrans(&stLog);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	stTemp.iLogFilePostion = iCnt;
	
	strcpy((char *)PosCom.stTrans.szCardNo, (char *)stLog.szCardNo);	
	strcpy((char *)PosCom.stTrans.szAuthNo, (char *)stLog.szAuthNo);
	PosCom.stTrans.iOldTransNo     = stLog.iTransNo;
	PosCom.stTrans.lOldBatchNumber = stLog.lBatchNumber;
	memcpy(PosCom.stTrans.sAmount, stLog.sAmount, 6);
	strcpy((char *)PosCom.stTrans.szCardUnit, (char *)stLog.szCardUnit);
	strcpy((char *)PosCom.stTrans.szSysReferNo, (char *)stLog.szSysReferNo);
	strcpy((char *)PosCom.stTrans.szOldTxnDate, (char *)stLog.szDate+4);
	sprintf((char *)PosCom.stTrans.szEntryMode, "%.3s", stLog.szEntryMode);
	PosCom.stTrans.uiTxnRecIndex = stLog.uiTxnRecIndex;

	if (stLog.iTransNo >= POS_INSTALLMENT || stLog.iTransNo <= POS_MAGTOPUP_ACCT)
	{
		memcpy(PosCom.stTrans.sTipAmount, stLog.sTipAmount, 6);
		memcpy(PosCom.stTrans.sPreAddAmount, stLog.sPreAddAmount, 6);
		memcpy(PosCom.stTrans.sLoyaltAmount, stLog.sLoyaltAmount, 6);
		PosCom.stTrans.ucInstallmentNum     = stLog.ucInstallmentNum;
		PosCom.stTrans.ucInstalPayMode = stLog.ucInstalPayMode;
		memcpy(PosCom.stTrans.szInstalCode, stLog.szInstalCode, 30);
		memcpy(PosCom.stTrans.szInstalCurrCode,stLog.szInstalCurrCode, 3);
	}

	// 预授权完成撤销：上送38域要同原来的预授权完成交易的请求
	if( stTemp.iTransNo==POS_AUTH_VOID && stLog.iTransNo==POS_AUTH_CM )
	{
		strcpy((char *)PosCom.stTrans.szAuthNo, (char *)stLog.szPrtOldAuthNo);
		strcpy((char *)PosCom.stTrans.szPrtOldAuthNo, (char *)stLog.szPrtOldAuthNo);
	}

	if( stTemp.iTransNo==POS_MOTO_AUTH_VOID || stTemp.iTransNo==POS_MOBILE_VOID_AUTH)
	{
		strcpy((char *)PosCom.stTrans.szPrtOldAuthNo, (char *)stLog.szAuthNo);
	}

	if (stTemp.iTransNo==POS_MOBILE_VOID || stTemp.iTransNo==POS_MOBILE_VOID_AUTH 
		|| stTemp.iTransNo == EC_VOID_TOPUP)
	{
		PosCom.stTrans.iIccDataLen = stLog.iIccDataLen;
		memcpy(PosCom.stTrans.sIccData,stLog.sIccData,PosCom.stTrans.iIccDataLen);
	}

	if( stTemp.iTransNo==OFF_ADJUST ) 
	{
		strcpy((char *)PosCom.stTrans.szAuthMode, (char *)stLog.szAuthMode);
		strcpy((char *)PosCom.stTrans.szAuthUnit, (char *)stLog.szAuthUnit);
		strcpy((char *)PosCom.stTrans.szExpDate,  (char *)stLog.szExpDate);
		if( stLog.iTransNo==POS_SALE )  ///本交易为小费
		{
			strcpy((char *)PosCom.stTrans.szAuthMode, "00"); /*小费交易原交易此域没有，故赋值为00*/
			strcpy((char *)PosCom.stTrans.szCardUnit, (char *)stLog.szCardUnit);
		}

		strcpy(PosCom.stTrans.szIssuerResp,stLog.szIssuerResp);
		strcpy(PosCom.stTrans.szCenterResp,stLog.szCenterResp);
		strcpy(PosCom.stTrans.szRecvBankResp,stLog.szRecvBankResp);
	}

	if( pstOutLog!=NULL )
	{
		memcpy(pstOutLog, &stLog, sizeof(NEWPOS_LOG_STRC));
	}

	return(OK);	
}

uint8_t ComputeTransTotal(int flag)
{
	int  i, fd, ret;
	NEWPOS_LOG_STRC stLog;
	
	lcdCls();
	lcdFlip();	
	if( stTransCtrl.iTransNum==0 )
		return(E_NO_TRANS);
	memset(&stTotal, 0, TOTAL_STRC_SIZE);
	
	fd = fileOpen(POS_LOG_FILE, O_RDWR);
	if( fd<0 ) 
		return(E_MEM_ERR);
	ret = fileSeek(fd, (int)0, SEEK_SET);
	if( ret<0 )
	{
		fileClose(fd);
		return(E_MEM_ERR);
	}
	
	for(i=0; i<stTransCtrl.iTransNum; i++)
	{
		if( fileRead(fd, (uint8_t *)&stLog, LOG_RECORD_LEN)!=LOG_RECORD_LEN )
		{
			fileClose(fd);	
			return(E_MEM_ERR);
		}
		
		/*借记统计*/
		if(ChkIfDebitTxn(stLog.iTransNo) || 
		  ((stLog.iTransNo==POS_SALE || stLog.iTransNo==ICC_OFFSALE) && stLog.ucAdjusted!=TRUE )) 
		{ 	
			if( stLog.iTransNo==OFF_SALE && stLog.ucAdjusted==TRUE && stLog.ucChgFrmAdjust!=TRUE )
				continue;
			
//20080804 将脱机消费统计到内卡PBOC
#ifdef PBOC_VERSION
			if( (memcmp(stLog.szCardUnit, "CUP", 3)==0) || (stLog.iTransNo==ICC_OFFSALE) ) 
#else
			if( memcmp(stLog.szCardUnit, "CUP", 3)==0 ) 
#endif
			{	
				stTotal.iRmbDebitNum ++;
				BcdAdd(stTotal.sRmbDebitAmt, stLog.sAmount, 6);

				if (stLog.iTransNo == EC_QUICK_SALE || stLog.iTransNo == EC_NORMAL_SALE)
				{
					stTotal.iRmbEcNum++;
					BcdAdd(stTotal.sRmbEcAmt, stLog.sAmount, 6);
				}
			} 
			else 
			{	
				stTotal.iFrnDebitNum ++;
				BcdAdd(stTotal.sFrnDebitAmt, stLog.sAmount, 6);
				
				if( stLog.iTransNo==ADJUST_TIP )
					BcdAdd(stTotal.sFrnDebitAmt, stLog.sTipAmount, 6);

				if (stLog.iTransNo == EC_QUICK_SALE || stLog.iTransNo == EC_NORMAL_SALE)
				{
					stTotal.iFrnEcNum++;
					BcdAdd(stTotal.sFrnEcAmt, stLog.sAmount, 6);
				}
			}
		}
//		if( stLog.iTransNo==POS_SALE_VOID || stLog.iTransNo==POS_REFUND || stLog.iTransNo==POS_AUTH_VOID ) 
		if(ChkIfCreditTxn(stLog.iTransNo))
		{	
			if (stLog.iTransNo== EC_TOPUP_CASH && stLog.ucVoidId== TRUE)
			{
				continue;
			}
//20080805 退货统计后台返回000时也要统计到内卡
#ifdef PBOC_VERSION
			if( (memcmp(stLog.szCardUnit, "CUP", 3)==0) || ((stLog.iTransNo==POS_REFUND)&&(memcmp(stLog.szCardUnit, "000", 3)==0)) ) 
#else
			if( memcmp(stLog.szCardUnit, "CUP", 3)==0 ) 
#endif
			{	
				stTotal.iRmbCreditNum ++;
				BcdAdd(stTotal.sRmbCreditAmt, stLog.sAmount, 6);
				
			} 
			else 
			{	
				stTotal.iFrnCreditNum++;
				BcdAdd(stTotal.sFrnCreditAmt, stLog.sAmount, 6);
			}

			if (stLog.iTransNo== EC_TOPUP_CASH || stLog.iTransNo== ED_TOPUP_CASH || stLog.iTransNo== POS_MAGTOPUP_CASH)
			{
				stTotal.iTopUpNum++;
				BcdAdd(stTotal.sTopUpAmt, stLog.sAmount, 6);
			}
		}
		
//		if( stLog.iTransNo==POS_SALE || stLog.iTransNo==ICC_OFFSALE || stLog.iTransNo==ADJUST_TIP )
		if(ChkIfSaleTxn(stLog.iTransNo))
		{
			if( stLog.ucVoidId==TRUE || stLog.ucAdjusted==TRUE )
				continue;
//20080804 将脱机消费统计到内卡PBOC
#ifdef PBOC_VERSION
			if( (memcmp(stLog.szCardUnit, "CUP", 3)==0) || (stLog.iTransNo==ICC_OFFSALE) ) 
#else
			if( memcmp(stLog.szCardUnit, "CUP", 3)==0 ) 
#endif
			{	
				stTotal.iRmbSaleNum++;
				BcdAdd(stTotal.sRmbSaleAmt, stLog.sAmount, 6);
			} 
			else 
			{	
				stTotal.iFrnSaleNum++;
				BcdAdd(stTotal.sFrnSaleAmt, stLog.sAmount, 6);
				BcdAdd(stTotal.sFrnSaleAmt, stLog.sTipAmount, 6);
			}			
		}
		if( stLog.iTransNo==POS_AUTH_CM || stLog.iTransNo==POS_MOTO_AUTH_CM ||
			stLog.iTransNo==POS_MOBILE_AUTH)
		{
			if( stLog.ucVoidId==TRUE )
				continue;
			if( memcmp(stLog.szCardUnit, "CUP", 3)==0 ) 
			{	
				stTotal.iRmbConfirmNum++;
				BcdAdd(stTotal.sRmbConfirmAmt, stLog.sAmount, 6);
			} 
			else 
			{	
				stTotal.iFrnConfirmNum++;
				BcdAdd(stTotal.sFrnConfirmAmt, stLog.sAmount, 6);
			}
		}
		if( stLog.iTransNo==POS_OFF_CONFIRM || stLog.iTransNo==POS_MOTO_AUTH_OFF ||
			stLog.iTransNo==POS_MOBILE_AUTH_OFF)
		{
			if( stLog.ucVoidId==TRUE )
				continue;
			if( memcmp(stLog.szCardUnit, "CUP", 3)==0 ) 
			{	
				stTotal.iRmbOffConfirmNum++;
				BcdAdd(stTotal.sRmbOffConfirmAmt, stLog.sAmount, 6);
			} 
			else 
			{	
				stTotal.iFrnOffConfirmNum++;
				BcdAdd(stTotal.sFrnOffConfirmAmt, stLog.sAmount, 6);
			}
		}
		if( stLog.iTransNo==POS_REFUND || stLog.iTransNo==POS_MOTO_REFUND ||
			stLog.iTransNo==POS_MOBILE_REFUND)
		{
			if( stLog.ucVoidId==TRUE )
				continue;
//20080804 当后台返回000的时候也需要统计到内卡
#ifdef PBOC_VERSION
			if( (memcmp(stLog.szCardUnit, "CUP", 3)==0) || (memcmp(stLog.szCardUnit, "000", 3)==0) ) 
#else
			if( memcmp(stLog.szCardUnit, "CUP", 3)==0 ) 
#endif
			{	
				stTotal.iRmbRefundNum++;
				BcdAdd(stTotal.sRmbRefundAmt, stLog.sAmount, 6);
			} 
			else 
			{	
				stTotal.iFrnRefundNum++;
				BcdAdd(stTotal.sFrnRefundAmt, stLog.sAmount, 6);
			}			
		}
		if( stLog.iTransNo==OFF_SALE || stLog.iTransNo==OFF_ADJUST )
		{
			if( stLog.ucVoidId==TRUE )
				continue;
			if( (stLog.ucAdjusted==TRUE) && (stLog.ucChgFrmAdjust!=TRUE) )
				continue;
			if( memcmp(stLog.szCardUnit, "CUP", 3)==0 ) 
			{	
				stTotal.iRmbOffNum++;
				BcdAdd(stTotal.sRmbOffAmt, stLog.sAmount, 6);
			} 
			else 
			{	
				stTotal.iFrnOffNum++;
				BcdAdd(stTotal.sFrnOffAmt, stLog.sAmount, 6);
				if( stLog.iTransNo==ADJUST_TIP ) 
					BcdAdd(stTotal.sFrnOffAmt, stLog.sTipAmount, 6);
			}			
		}
	}
	
	fileClose(fd);
	return(OK);
}

void update_limit_total()
{			
	if( stTemp.iTransNo==POS_SALE || stTemp.iTransNo==ICC_OFFSALE || stTemp.iTransNo==POS_AUTH_CM || 
		stTemp.iTransNo==POS_OFF_CONFIRM || stTemp.iTransNo==OFF_SALE || stTemp.iTransNo==OFF_ADJUST 
		|| stTemp.iTransNo==ADJUST_TIP ) 
	{ 
		if (memcmp(PosCom.stTrans.szCardUnit, "CUP", 3)==0 ) 
		{	
			BcdAdd(stLimitTotal.sRmbDebitAmt, PosCom.stTrans.sAmount, 6);
		} 
		else 
		{	
			BcdAdd(stLimitTotal.sFrnDebitAmt, PosCom.stTrans.sAmount , 6);
			if( stTemp.iTransNo==ADJUST_TIP ) 
				BcdAdd(stLimitTotal.sFrnDebitAmt, PosCom.stTrans.sTipAmount, 6);  
		}
	}
	if( stTemp.iTransNo==POS_SALE_VOID || stTemp.iTransNo==POS_REFUND || stTemp.iTransNo==POS_AUTH_VOID ) 
	{	
		if( memcmp(PosCom.stTrans.szCardUnit, "CUP", 3)==0 ) 
		{	
			BcdAdd(stLimitTotal.sRmbCreditAmt, PosCom.stTrans.sAmount, 6);
		} 
		else 
		{	
			BcdAdd(stLimitTotal.sFrnCreditAmt, PosCom.stTrans.sAmount, 6);
		}
	}
	SaveLimitTotal();
	return;
}
uint8_t M1reprintTrans(uint8_t prt_bz)
{
	uint8_t buf[17];
	int i, ret, fd;
	int lTraceNo, iRet;
	NEWPOS_LOG_STRC stLog;
	
	lcdCls();
	lcdFlip();	

	if( stTransCtrl.iTransNum==0 ) 
       	return(E_NO_TRANS);
	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    重打凭单    ", "    REPRINT     ");
	
    if( prt_bz==0 )
	{	
        fd = fileOpen(POS_LOG_FILE, O_RDWR);
		if( fd<0 )
		{
			return(E_MEM_ERR);
		}
		for(i=1; i<stTransCtrl.iTransNum+1; i++)
		{
			ret = fileSeek(fd, (int)((stTransCtrl.iTransNum-i)*LOG_RECORD_LEN), SEEK_SET);
			if( ret<0 ) 
			{
				fileClose(fd);
				return (E_MEM_ERR);
			}
			ret = fileRead(fd, (uint8_t *)&stLog, LOG_RECORD_LEN);
			if( ret<0 ) 
			{
				fileClose(fd);
				return(E_MEM_ERR);
			}
			if(1)  ///这里可以添加判断，以实现打印最后一笔正交易
				break;
		}
		fileClose(fd);
		if( i==stTransCtrl.iTransNum+1 ) 
			return(E_NO_TRANS);
	}
	else 
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 2, DISP_CFONT, "输入凭证号");
		lcdGoto(80, 6);
		iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 1, 6, 
			(stPosParam.ucOprtLimitTime*1000), (char *)buf);
		if( iRet<0 )
			return E_TRANS_CANCEL;

		lTraceNo = atol((char *)buf);		
		fd = fileOpen(POS_LOG_FILE, O_RDWR);
		if( fd<0 ) 
		{
			return(E_MEM_ERR);
		}
		ret = fileSeek(fd, (int)0, SEEK_SET);
		if( ret<0 ) 
		{
			fileClose(fd);
			return (E_MEM_ERR);
		}
		
        for(i=0; i<stTransCtrl.iTransNum; i++)
		{
			ret = fileRead(fd, (uint8_t *)&stLog, LOG_RECORD_LEN);
			if( ret!=LOG_RECORD_LEN ) 
			{
				i = stTransCtrl.iTransNum;
				break;
			}
			if( stLog.lTraceNo==lTraceNo ) 
				break;				   
		}
		fileClose(fd);
		if( i==stTransCtrl.iTransNum ) 
			return(E_NO_OLD_TRANS);
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	memcpy(&PosCom.stTrans, &stLog, sizeof(stLog));
	PrtTranTicket(REPRINT);
	return(OK);
	
}

uint8_t reprintTrans(uint8_t prt_bz)
{
	uint8_t buf[17];
	int i, ret, fd;
	int lTraceNo, iRet;
	NEWPOS_LOG_STRC stLog;
	
	lcdCls();
	lcdFlip();	

	if( stTransCtrl.iTransNum==0 ) 
       	return(E_NO_TRANS);
	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    重打凭单    ", "    REPRINT     ");
	
    if( prt_bz==0 )
	{	
        fd = fileOpen(POS_LOG_FILE, O_RDWR);
		if( fd<0 )
		{
			return(E_MEM_ERR);
		}
		for(i=1; i<stTransCtrl.iTransNum+1; i++)
		{
			ret = fileSeek(fd, (int)((stTransCtrl.iTransNum-i)*LOG_RECORD_LEN), SEEK_SET);
			if( ret<0 ) 
			{
				fileClose(fd);
				return (E_MEM_ERR);
			}
			ret = fileRead(fd, (uint8_t *)&stLog, LOG_RECORD_LEN);
			if( ret<0 ) 
			{
				fileClose(fd);
				return(E_MEM_ERR);
			}
			if(1)  ///这里可以添加判断，以实现打印最后一笔正交易
				break;
		}
		fileClose(fd);
		if( i==stTransCtrl.iTransNum+1 ) 
			return(E_NO_TRANS);
	}
	else 
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 2, DISP_CFONT, "输入凭证号");
		lcdGoto(80, 6);
		iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 1, 6, 
			(stPosParam.ucOprtLimitTime*1000), (char *)buf);
		if( iRet<0 )
			return E_TRANS_CANCEL;

		lTraceNo = atol((char *)buf);		
		fd = fileOpen(POS_LOG_FILE, O_RDWR);
		if( fd<0 ) 
		{
			return(E_MEM_ERR);
		}
		ret = fileSeek(fd, (int)0, SEEK_SET);
		if( ret<0 ) 
		{
			fileClose(fd);
			return (E_MEM_ERR);
		}
		
        for(i=0; i<stTransCtrl.iTransNum; i++)
		{
			ret = fileRead(fd, (uint8_t *)&stLog, LOG_RECORD_LEN);
			if( ret!=LOG_RECORD_LEN ) 
			{
				i = stTransCtrl.iTransNum;
				break;
			}
			if( stLog.lTraceNo==lTraceNo ) 
				break;				   
		}
		fileClose(fd);
		if( i==stTransCtrl.iTransNum ) 
			return(E_NO_OLD_TRANS);
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	memcpy(&PosCom.stTrans, &stLog, sizeof(stLog));
	PrtTranTicket(REPRINT);
	return(OK);
	
}

uint8_t PrintDetail(uint8_t bPrintAll,uint8_t ucTitleFlag)
{
	uint8_t		ucRet;
	int		iCnt, iNumOfOnePage;
	NEWPOS_LOG_STRC	stLog;

	if( bPrintAll )
	{
		lcdCls();
		lcdFlip();	
	}
	if( stTransCtrl.iTransNum==0 )
	{
		return E_NO_TRANS;
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "    打印明细    ", "   PRN DETAIL   ");
	
   	prnInit();
	prnPrintf((char *)"\n");
	PrnSmallFontData("-------------------------------------------\n");
	if (ucTitleFlag)
	{
		PrnBigFontChnData("    结 算 明 细 单\n");
	}
	else
	{
		PrnBigFontChnData("    交 易 明 细 单\n");
	}
	PrnSmallFontData("-------------------------------------------\n");
    PrtTranDetailHead();
	
	iNumOfOnePage = 0;
	for(iCnt=0; iCnt<stTransCtrl.iTransNum; iCnt++)
	{
		memset(&stLog, 0, sizeof(stLog));
		ucRet = LoadTranLog(iCnt, &stLog);
		if( ucRet!=OK )
		{
			break;
		}

		if( !bPrintAll )
		{
/*			if( stLog.iTransNo!=POS_SALE    && stLog.iTransNo!=ICC_OFFSALE     &&
				stLog.iTransNo!=POS_AUTH_CM && stLog.iTransNo!=POS_OFF_CONFIRM &&
				stLog.iTransNo!=POS_REFUND  && stLog.iTransNo!=OFF_SALE        &&
				stLog.iTransNo!=OFF_ADJUST  && stLog.iTransNo!=ADJUST_TIP )*/
			if(!ChkIfPrtDetill(stLog.iTransNo))
			{
				continue;
			}
			if( stLog.ucVoidId==TRUE )
			{
				continue;
			}
			if( stLog.ucAdjusted && stLog.ucChgFrmAdjust!=TRUE )
			{
				continue;
			}
		}

		ucRet = PrtOneTranTxn(&stLog);
		if( ucRet!=OK )
		{
			return ucRet;
		}
		iNumOfOnePage++;	
		if( iNumOfOnePage>=NUM_PER_PAGE )
		{
			ucRet = PrintData();
			if( ucRet!=OK )
			{
				return ucRet;
			}
			iNumOfOnePage = 0;
			prnInit();
		}
	}
	prnPrintf("\n\n\n\n");
	
	if( iNumOfOnePage>0 )
	{
		ucRet = PrintData();
		if( ucRet!=OK )
		{
			return ucRet;
		}
	}

	return OK;
}
uint8_t Prt_Last_LS(void)
{
	    INT32 iTotalNum,lCurIndex;
		INT32  iTranNum,iTotalTranAmt;
		INT32	 iCnt, i;//iRet,
		lcdCls();
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  打印最后一笔  ", "  QUERY TRANS   ");
		kbFlush();
		// PubDisplayTitle(TRUE,"交易明细查询");
		//memset(szDisp, 0, sizeof(szDisp));
		lCurIndex = 0;
		iCnt=0;
		iTranNum=0;
		iTotalTranAmt=0;
		 memset(&gstYktEnrichLog, 0, sizeof(gstYktEnrichLog));
		iTotalNum= ReadYtkEnrichLog(&iCnt);
		if( iTotalNum==0 )
		{
		   lcdCls();
		   PubDisplayTitle(TRUE,"当批交易查询");
		   PubDispString("无交易", 3|DISP_LINE_CENTER);
		   PubWaitKey(10);
		   return NO_DISP;
		}
		
		PrtM1TranEnrichTicket(REPRINT);
		return OK;

}
uint8_t Prt_Any_LS(void)
{
	 // int iRet;
		UINT8 buf[50],szDate[7+1],szDisp[50];
		INT32 iTotalNum,lCurIndex;
		INT32  iTranNum,iTotalTranAmt;
		INT32	 iCnt, iRet,i;
		int 	lTraceNo;
		uint8_t 	ucRet, szTraceNo[6+1];
		
		lcdCls();
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  打印任意一笔  ", "  QUERY TRANS   ");	
		
	//----------------------
	DispMulLanguageString(0, 2, DISP_CFONT, "输入凭证号", "INPUT TRACE:");
	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 1, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)szTraceNo);
	if( iRet<0 )
	{
		return E_TRANS_CANCEL;
	}
	lTraceNo = atol((char *)szTraceNo);
		lCurIndex = 0;
		iCnt=0;
		iTranNum=0;
		iTotalTranAmt=0;
		memset(&gstYktEnrichLog, 0, sizeof(gstYktEnrichLog));
		iTotalNum= ReadYtkEnrichLog(&iCnt);
		if( iTotalNum==0 )
		{
		   lcdCls();
		   PubDisplayTitle(TRUE,"当批交易查询");
		   PubDispString("无交易", 3|DISP_LINE_CENTER);
		   PubWaitKey(10);
		   return NO_DISP;
		}
		for(i=0;i<iTotalNum;i++)
		{
		  ReadYtkEnrichLog(&iCnt);
		  if(lTraceNo==gstYktEnrichLog.lEnrichTraceNo)
		  {
		  
			break;
		  }
		  else
		  {
			iCnt--;
			if (iCnt == 0)
			{
				iCnt=iTotalNum;
	
			}
	
		  }
		}
		  if(i==iTotalNum)
		  {
				FailBeep();
				lcdClrLine(2,7);
				lcdDisplay(0, 4, DISP_CFONT, "无相应的交易");
				kbGetKeyMs(3*1000);
				return NO_DISP;
		  }
		  PrtM1TranEnrichTicket(REPRINT);
		return OK;

}
uint8_t Prt_Detail_LS(uint8_t bPrintAll,uint8_t ucTitleFlag)
{
	    uint8_t 	ucRet;
		int 	 iNumOfOnePage;
		//NEWPOS_LOG_STRC stLog;
	uint8_t szBuf[1024];
//--
	INT32  iTotalNum,lCurIndex;
		INT32  iTranNum,iTotalTranAmt;
		INT32  iTran_COM_Num,iTotalTran_COM_Amt;
		INT32  iTran_Stu_Num,iTotalTran_Stu_Amt;
		INT32  iCnt, iRet,i;
		UINT8  szFileName[30];
		UINT8  szFileName_settle[30];
		UINT8  sztime[20];
		UINT8  sztime_settle[20];
		UINT8  szEnrichdata[500];
		int    iEnrichDatalen;
		UINT8  uRet;
		INT32  fileLen;
		int iposition;
		UINT8 sSAM[10+1];//10位卡号
	//	UINT8  sbuf[100];

	//-----------------------------
	        lCurIndex = 0;
			iCnt=0;
			iTranNum=0;
			iTotalTranAmt=0;
			iTran_COM_Num=0;
			iTotalTran_COM_Amt=0;
			iTran_Stu_Num=0;
			iTotalTran_Stu_Amt=0;
			//---------
			lcdClrLine(0, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "	 打印明细	 ", "	PRN DETAIL	 ");
		
		prnInit();
		prnPrintf((char *)"\n");
		PrnSmallFontData("-------------------------------------------\n");
		if (ucTitleFlag)
		{
			PrnBigFontChnData("    结 算 明 细 单\n");
		}
		else
		{
			PrnBigFontChnData("    交 易 明 细 单\n");
		}
		PrnSmallFontData("-------------------------------------------\n");
		PrtTranDetailHead();
		//--------------------------------
		iNumOfOnePage = 0;
			//--------------------------
			 memset(&gstYktEnrichLog, 0, sizeof(gstYktEnrichLog));
			iTotalNum= ReadYtkEnrichLog(&iCnt);
			if( iTotalNum==0 )
			{
			   lcdCls();
			   PubDisplayTitle(TRUE,"当批交易查询");
			   PubDispString("无交易", 3|DISP_LINE_CENTER);
			   PubWaitKey(10);
			   return NO_DISP;
			}
			for(i=0;i<iTotalNum;i++)
			{
				ReadYtkEnrichLog(&iCnt);
				if(gstYktEnrichLog.uiStatus==0x01)//结算了
				{
					iCnt--;
					if (iCnt == 0)
					{
						iCnt=iTotalNum;
					}
					continue;
					//	break;	 //注意，很有可能有问题
					 // iCnt=iCnt%iTotalNum;
				 }
				 else
				 {
					iTranNum++;
					sprintf((char *)szBuf, "%06d     %d.%02d    %16.16s\n", gstYktEnrichLog.lEnrichTraceNo, gstYktEnrichLog.ulTransAmt/100,gstYktEnrichLog.ulTransAmt%100,gstYktEnrichLog.sCardSerialNo);

	                PrnSmallFontData((char *)szBuf);
					iNumOfOnePage++;	
					if( iNumOfOnePage>=NUM_PER_PAGE )
					{
						ucRet = PrintData();
						if( ucRet!=OK )
						{
							return ucRet;
						}
						iNumOfOnePage = 0;
						prnInit();
					}
					
					iCnt--;
					if (iCnt == 0)
					{
					 iCnt=iTotalNum;
					}
				  }
				}

	prnPrintf("\n\n\n\n");
		
		if( iNumOfOnePage>0 )
		{
			ucRet = PrintData();
			if( ucRet!=OK )
			{
				return ucRet;
			}
		}

	//-------------------------------
		return OK;

}
uint8_t Prt_Total_LS(void)
{
		INT32  iTotalNum,lCurIndex;
		INT32  iTranNum,iTotalTranAmt;
		INT32  iTran_COM_Num,iTotalTran_COM_Amt;
		INT32  iTran_Stu_Num,iTotalTran_Stu_Amt;
		INT32  iTran_Old_Num,iTotalTran_Old_Amt;
		INT32  iTran_Sta_Num,iTotalTran_Sta_Amt;
		INT32  iTran_Tel_Num,iTotalTran_Tel_Amt;
		INT32  iTran_Per_Num,iTotalTran_Per_Amt;
		INT32  iTran_Nul_Num,iTotalTran_Nul_Amt;
		INT32  iTran_Tong_Num,iTotalTran_Tong_Amt;
		INT32  iCnt, iRet,i;
		UINT8  szFileName[30];
		UINT8  szFileName_settle[30];
		UINT8  sztime[20];
		UINT8  sztime_settle[20];
		UINT8  szEnrichdata[500];
		int    iEnrichDatalen;
		UINT8  uRet;
		INT32  fileLen;
		int iposition;
		UINT8 sSAM[10+1];//10位卡号
		UINT8  sbuf[100];
		memset(sztime,0,sizeof(sztime));
		memset(szFileName,0,sizeof(szFileName));
		GetEnrichFileName(szFileName);
		memcpy(szFileName_settle,szFileName,sizeof(szFileName));
		szFileName_settle[6]='b';
		memcpy(sztime,szFileName+7,14);//交易记录
		sztime[14]='c';
		sztime[15]=0;
		
		memcpy(sztime_settle,szFileName_settle+7,14);//结算
		sztime_settle[14]='b';
		sztime_settle[15]=0;
	
		lcdCls();
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  打印交易汇总  ", "  QUERY TRANS   ");
		kbFlush();
		lCurIndex = 0;
		iCnt=0;
		iTranNum=0;
		iTotalTranAmt=0;
		iTran_COM_Num=0;
		iTotalTran_COM_Amt=0;
		iTran_Stu_Num=0;
		iTotalTran_Stu_Amt=0;
		iTran_Old_Num=0;iTotalTran_Old_Amt=0;
		iTran_Sta_Num=0;iTotalTran_Sta_Amt=0;
		iTran_Tel_Num=0;iTotalTran_Tel_Amt=0;
		iTran_Per_Num=0;iTotalTran_Per_Amt=0;
		iTran_Nul_Num=0;iTotalTran_Nul_Amt=0;
		iTran_Tong_Num=0;iTotalTran_Tong_Amt=0;
	
		//--------------------------
		 memset(&gstYktEnrichLog, 0, sizeof(gstYktEnrichLog));
		iTotalNum= ReadYtkEnrichLog(&iCnt);
		if( iTotalNum==0 )
		{
		   lcdCls();
		   PubDisplayTitle(TRUE,"当批交易查询");
		   PubDispString("无交易", 3|DISP_LINE_CENTER);
		   PubWaitKey(10);
		   return NO_DISP;
		}
		for(i=0;i<iTotalNum;i++)
		{
			ReadYtkEnrichLog(&iCnt);
			if(gstYktEnrichLog.uiStatus==0x01)//结算了
			{
				iCnt--;
				if (iCnt == 0)
				{
					iCnt=iTotalNum;
				}
				continue;
				//	break;	 //注意，很有可能有问题
				 // iCnt=iCnt%iTotalNum;
			 }
			 else
			 {
				iTranNum++;
				iTotalTranAmt=iTotalTranAmt+ gstYktEnrichLog.ulTransAmt;
				if(gstYktEnrichLog.uCardtype==0x00)//普通卡
				{
					iTran_COM_Num++;
					iTotalTran_COM_Amt=iTotalTran_COM_Amt+ gstYktEnrichLog.ulTransAmt;
				}
				else if(gstYktEnrichLog.uCardtype==0x02)//老年卡
				{
					iTran_Old_Num++;
					iTotalTran_Old_Amt=iTotalTran_Old_Amt+ gstYktEnrichLog.ulTransAmt;
				}
				else if(gstYktEnrichLog.uCardtype==0x01)//学生卡
				{
					iTran_Stu_Num++;
					iTotalTran_Stu_Amt=iTotalTran_Stu_Amt+ gstYktEnrichLog.ulTransAmt;
				}
				else if(gstYktEnrichLog.uCardtype==0x03)//员工卡
				{
					iTran_Sta_Num++;
					iTotalTran_Sta_Amt=iTotalTran_Sta_Amt+ gstYktEnrichLog.ulTransAmt;
				}
				else if(gstYktEnrichLog.uCardtype==0x04)//电信卡
				{
					iTran_Tel_Num++;
					iTotalTran_Tel_Amt=iTotalTran_Tel_Amt+ gstYktEnrichLog.ulTransAmt;
				}
				else if(gstYktEnrichLog.uCardtype==0x05)//个性卡
				{
					iTran_Per_Num++;
					iTotalTran_Per_Amt=iTotalTran_Per_Amt+ gstYktEnrichLog.ulTransAmt;
				}
				else if(gstYktEnrichLog.uCardtype==0x06)//员工通勤卡
				{
					iTran_Tong_Num++;
					iTotalTran_Tong_Amt=iTotalTran_Tong_Amt+ gstYktEnrichLog.ulTransAmt;
				}
				else	//未定义
				{
					iTran_Nul_Num++;
					iTotalTran_Nul_Amt=iTotalTran_Nul_Amt+ gstYktEnrichLog.ulTransAmt;

				}
				iCnt--;
				if (iCnt == 0)
				{
				 iCnt=iTotalNum;
				}
			  }
			}
			memset(&stM1EnrichTotal,0,TOTAL_M1ENRICH_STRC_SIZE);
			stM1EnrichTotal.iRmbM1EnrichNum=iTranNum;
			stM1EnrichTotal.iRmbM1EnrichAmt=iTotalTranAmt;	
			stM1EnrichTotal.iRmbM1Enrich_common_Num=iTran_COM_Num;
			stM1EnrichTotal.iRmbM1Enrich_common_Amt=iTotalTran_COM_Amt;
			stM1EnrichTotal.iRmbM1Enrich_student_Num=iTran_Stu_Num;
			stM1EnrichTotal.iRmbM1Enrich_student_Amt=iTotalTran_Stu_Amt;

			stM1EnrichTotal.iRmbM1Enrich_old_Amt=iTotalTran_Old_Amt;
			stM1EnrichTotal.iRmbM1Enrich_old_Num=iTran_Old_Num;
			stM1EnrichTotal.iRmbM1Enrich_staff_Amt=iTotalTran_Sta_Amt;
			stM1EnrichTotal.iRmbM1Enrich_staff_Num=iTran_Sta_Num;
			stM1EnrichTotal.iRmbM1Enrich_tong_Amt=iTotalTran_Tong_Amt;
			stM1EnrichTotal.iRmbM1Enrich_tong_Num=iTran_Tong_Num;
			stM1EnrichTotal.iRmbM1Enrich_pers_Amt=iTotalTran_Per_Amt;
			stM1EnrichTotal.iRmbM1Enrich_pers_Num=iTran_Per_Num;
			stM1EnrichTotal.iRmbM1Enrich_tel_Amt=iTotalTran_Tel_Amt;
			stM1EnrichTotal.iRmbM1Enrich_tel_Num=iTran_Tel_Num;
			stM1EnrichTotal.iRmbM1Enrich_null_Amt=iTotalTran_Nul_Amt;
			stM1EnrichTotal.iRmbM1Enrich_null_Num=iTran_Nul_Num;
			//memcpy(stM1EnrichTotal.pointofsaleNo,,3);
			memcpy(stM1EnrichTotal.sDateTime,sztime,14);
			memcpy(stM1EnrichTotal.szLSDepartmentNo,stPosParam.szLSDepartmentNo,strlen(stPosParam.szLSDepartmentNo));
			memcpy(stM1EnrichTotal.szLSM1TellNo,stPosParam.szLSM1TellNo,strlen(stPosParam.szLSM1TellNo));
			memcpy(stM1EnrichTotal.szLSPosId,stPosParam.szLSPosId,strlen(stPosParam.szLSPosId));
			strcpy((char *)stM1EnrichTotal.szNowTellerNo, (char *)stTransCtrl.szNowTellerNo);
	
			stM1EnrichTotal.u8Settlemode=1;
	
	        //SaveM1EnrichLastTotal();
			//PrtM1EnrichTotal_1(1);
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT, "交易笔数: %d", iTranNum);
			lcdDisplay(0, 4, DISP_CFONT, "交易金额: %d.%02d", iTotalTranAmt/100,iTotalTranAmt%100);
			kbGetKeyMs(10*1000);
			kbFlush();
			return 0;

}

uint8_t M1_queryDetail(void)
{

     char szDisp[50], szBuffer[50];//,szBuffer1[50],szResultAmt[6];
	 int  iRet,  iTotalNum=0;
	 int  lCurIndex, iCnt=0;
	// char szCustom[50];
 
 
 
	 kbFlush();
	 PubDisplayTitle(TRUE,"交易明细查询");
	 memset(szDisp, 0, sizeof(szDisp));
	 lCurIndex = 0;
	 iCnt=1;
	 memset(&gstYktEnrichLog, 0, YKTENRICH_LOG_SIZE);
	 iTotalNum= ReadYtkEnrichLog(&lCurIndex);
	 if( iTotalNum==0 )
	 {
		 lcdCls();
		 PubDisplayTitle(TRUE,"当批交易查询");
		 PubDispString("无交易", 3|DISP_LINE_CENTER);
		 PubWaitKey(10);
		 return 0;
	 }
	 while(1)
	 {
	     
		 sprintf(szDisp, "%04d/%04d", iCnt, iTotalNum);
		 memset(szBuffer, 0, sizeof(szBuffer));
		 sprintf(szBuffer, "明细 %9.9s", szDisp);
		 PubDisplayTitle(TRUE, szBuffer);
		 lcdDisplay(0, 2, DISP_ASCII, "No:%s", gstYktEnrichLog.sCardSerialNo+8);
		 // lcdDisplay(0, 3, DISP_CFONT, "%s", YktRecord.lTraceNo);
		 lcdDisplay(0, 3, DISP_ASCII, "CSN:%s", gstYktEnrichLog.sCSN);
		 lcdDisplay(0, 4, DISP_ASCII, "AMOUNT: %d.%02d", gstYktEnrichLog.ulTransAmt/100,gstYktEnrichLog.ulTransAmt%100);
		 lcdDisplay(0, 5, DISP_ASCII, "TraceNo: %d", gstYktEnrichLog.lEnrichTraceNo);

		memset(szDisp, 0, sizeof(szDisp));// sTranDateTime
		 sprintf((char *)szDisp, "%02X%02X%02X%02X %02X%02X%02X",
		gstYktEnrichLog.sTranDateTime[0], gstYktEnrichLog.sTranDateTime[1],
		gstYktEnrichLog.sTranDateTime[2],gstYktEnrichLog.sTranDateTime[3], 
		gstYktEnrichLog.sTranDateTime[4], gstYktEnrichLog.sTranDateTime[5],
		gstYktEnrichLog.sTranDateTime[6]);
	    lcdDisplay(0, 6, DISP_ASCII, "TIME:%s",szDisp);
		if(gstYktEnrichLog.uiStatus==1)
		{//已经结算
		   lcdDisplay(0, 7, DISP_ASCII, "Status: settle");//settlement

		}
		else
		{
			lcdDisplay(0, 7, DISP_ASCII, "Status: no settle");

		}

	
//----------------------

	

		/*	 memset(szBuffer, 0, sizeof(szBuffer));
			 sprintf(szBuffer, "%4.4s/%2.2s/%2.2s:%2.2s/%2.2s", YktRecord.szDate, YktRecord.szDate+4, YktRecord.szDate+6,
			 YktRecord.szTime, YktRecord.szTime+2);
			 lcdDisplay(0, 5, DISP_ASCII, "TIME:%s",szBuffer);
			 lcdDisplay(0, 6, DISP_ASCII, "RRN :%s", YktRecord.szSysReferNo);
			 memset(szBuffer, 0, sizeof(szBuffer));
	         PubBcd2Asc0(YktRecord.szTAC,4,szBuffer);
	        // lcdDisplay(0, 7, DISP_ASCII, "TAC :%s", szBuffer);
		//----------
		PubBcdAdd(YktRecord.sAmount,YktRecord.sPollAmount,6,szResultAmt);
	    PubBcdAdd(szResultAmt,YktRecord.sBalance,6,szResultAmt);
		if(memcmp(szResultAmt,YktRecord.sTransBalance,6)==0)
		   lcdDisplay(0, 7, DISP_ASCII, "TAC :%s", szBuffer);
	    else
		  lcdDisplay(0, 7, DISP_ASCII, "Status: Fail ", szBuffer);*/


		//-----------
		 iRet = PubWaitKey(30);
		 switch (iRet)
		 {
		 case KEY_DOWN:
		 case KEY_ENTER:
			 lCurIndex--;
			 if (lCurIndex == 0)
			 {
				 lCurIndex=iTotalNum;
 
			 }
 
			 iCnt++;
			 if (iCnt >iTotalNum)
			 {
				 iCnt = 1;
			 }
			 break;
		 case KEY_UP:
			 lCurIndex++;
			 if (lCurIndex > iTotalNum)
			 {
				 lCurIndex= 1;
 
			 }
			 iCnt--;
			 if (iCnt <= 0)
			 {
				 iCnt = iTotalNum;
			 }
			 break;
		 case KEY_CANCEL:
		 case KEY_TIMEOUT:
			 return NO_DISP;
			 break;
		 case KEY_FN:
			 PubDisplayTitle(TRUE,"重 打 印");
			 lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"是否重印该记录");
			 PubDispString("[取消]	[确认]", DISP_LINE_CENTER|6);
			 if( PubYesNo(30) )
			 {
			  PrtM1TranEnrichTicket(REPRINT);
				// PrtYktTranEnrichTicket(REPRINT);//PrnChkTicketReceipt(PRN_REPRINT);
				 PubDisplayTitle(TRUE,"查询交易明细");
			 }
			 break;
		 default:
			 PubBeepErr();
			 break; 		 
		 }
		 memset(&gstYktEnrichLog, 0, YKTENRICH_LOG_SIZE);
		 ReadYtkEnrichLog(&lCurIndex);
	 }
 
	 return 0;
	/*uint8_t		ucRet, ucKey;
	int		iRecNo;
	NEWPOS_LOG_STRC	stLog;
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "交易查询        ", "   VIEW DETAIL  ");
	
	if( stTransCtrl.iTransNum==0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "     无交易     ", "NO TRANS");
		ErrorBeep();
		kbGetKeyMs(2000);					
		return E_NO_TRANS;
	}
	
	for(iRecNo=stTransCtrl.iTransNum-1; iRecNo>=0; iRecNo--)
	{
		memset(&stLog, 0, sizeof(stLog));
		ucRet = LoadTranLog(iRecNo, &stLog);
		if( ucRet!=OK )
		{
			lcdCls();
			lcdFlip();	
			return ucRet;
		}
		DispOneTrans(&stLog);
		
		ucKey = kbGetKeyMs(stPosParam.ucOprtLimitTime*1000);
		if( ucKey==KEY_CANCEL )
		{
			lcdCls();
			lcdFlip();	
			return E_TRANS_CANCEL;
		}
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 3, DISP_MEDIACY|DISP_CFONT, "    显示完毕    ", "   END     ");
	kbGetKeyMs(1000);

	return NO_DISP;*/
}

uint8_t queryDetail(int flag)
{
	uint8_t		ucRet, ucKey;
	int		iRecNo;
	NEWPOS_LOG_STRC	stLog;
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "交易查询        ", "   VIEW DETAIL  ");
	
	if( stTransCtrl.iTransNum==0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "     无交易     ", "NO TRANS");
		ErrorBeep();
		kbGetKeyMs(2000);					
		return E_NO_TRANS;
	}
	
	for(iRecNo=stTransCtrl.iTransNum-1; iRecNo>=0; iRecNo--)
	{
		memset(&stLog, 0, sizeof(stLog));
		ucRet = LoadTranLog(iRecNo, &stLog);
		if( ucRet!=OK )
		{
			lcdCls();
			lcdFlip();	
			return ucRet;
		}
		DispOneTrans(&stLog);
		
		ucKey = kbGetKeyMs(stPosParam.ucOprtLimitTime*1000);
		if( ucKey==KEY_CANCEL )
		{
			lcdCls();
			lcdFlip();	
			return E_TRANS_CANCEL;
		}
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 3, DISP_MEDIACY|DISP_CFONT, "    显示完毕    ", "   END     ");
	kbGetKeyMs(1000);

	return NO_DISP;
}

void DispOneTrans(NEWPOS_LOG_STRC *pstLog)
{
	uint8_t	  szBuff[45], sTranAmt[6]; 
	uint8_t   szCardTemp[20], szStatuInfo[20];
	
	lcdCls();
	lcdFlip();	
	GetTransName(pstLog->iTransNo, szBuff);
	memset(szStatuInfo,0,sizeof(szStatuInfo));
	GetTransStatusInfo(pstLog,(char*)szStatuInfo);
	if ((pstLog->iTransNo == EC_QUICK_SALE && pstLog->ucSendFlag == TRUE) ||
		(pstLog->iTransNo == EC_NORMAL_SALE && pstLog->ucSendFlag == TRUE) ||
		(pstLog->iTransNo == EC_QUICK_SALE && pstLog->ucSendFail == TS_NOT_CODE) ||
		(pstLog->iTransNo == EC_NORMAL_SALE && pstLog->ucSendFail == TS_NOT_CODE))
	{
		lcdDisplay(0, 0, DISP_CFONT, "EC消费(已上送)");
	}
	else
	{
		if (pstLog->iTransNo == EC_QUICK_SALE)
		{
			lcdDisplay(0, 0, DISP_CFONT, "EC消费");
			if (pstLog->ucSendFail == TS_NOT_CODE)
			{
				lcdClrLine(0,2);
				lcdDisplay(0, 0, DISP_CFONT, "EC消费(已上送)");
			}
		}
		else
		{
			if (pstLog->iTransNo == EC_TOPUP_CASH)
			{
				lcdDisplay(0, 0, DISP_CFONT, "EC现金充值");
				if (pstLog->ucVoidId == TRUE)
					lcdDisplay(0, 0, DISP_CFONT|DISP_CLRLINE, "EC现金充值(已撤)");
			}
			else
			{
				if (pstLog->iTransNo == EC_VOID_TOPUP)
				{
					lcdDisplay(0, 0, DISP_CFONT, "EC现金充值撤销  ");
				}
				else
				{
					if (strlen((char *)szBuff)+strlen((char *)szStatuInfo)>16)
						lcdDisplay(0, 0, DISP_CFONT, "%s%8.8s",(char *)szBuff,szStatuInfo+1);
					else
						lcdDisplay(0, 0, DISP_CFONT, "%s%s",(char *)szBuff,szStatuInfo);
				}
			}
		}

		if (pstLog->iTransNo ==POS_AUTH_CM && pstLog->ucVoidId == TRUE)
		{
			lcdClrLine(0,2);
			lcdDisplay(0, 0, DISP_CFONT, "完成请求(已撤销)");
		}

		if (pstLog->iTransNo ==EC_TOPUP_NORAML)
		{
			lcdClrLine(0,2);
			lcdDisplay(0, 0, DISP_CFONT, "EC非指定账户圈存");
		}

		if (pstLog->iTransNo ==EC_TOPUP_SPEC)
		{
			lcdClrLine(0,2);
			lcdDisplay(0, 0, DISP_CFONT, "EC指定账户圈存");
		}

		if (pstLog->iTransNo == ICC_OFFSALE && pstLog->ucSendFail == TS_NOT_CODE)
		{
			lcdClrLine(0,2);
			lcdDisplay(0, 0, DISP_CFONT, "消费(已上送)");
		}
	}

	lcdClrLine(2,7);
	lcdFlip();
	if( (pstLog->iTransNo==POS_PREAUTH) || (pstLog->iTransNo==POS_PREAUTH_ADD) ||
		(pstLog->iTransNo==OFF_SALE) || (pstLog->iTransNo==OFF_ADJUST) || 
		(pstLog->iTransNo==EC_QUICK_SALE) ||  (pstLog->iTransNo==EC_NORMAL_SALE) ||
		(pstLog->iTransNo==ICC_OFFSALE) )
	{
		strcpy((char *)szCardTemp, (char *)pstLog->szCardNo);
	}
	else
	{
		MaskPan(pstLog->szCardNo, szCardTemp);
	}

	lcdDisplay(0, 2, DISP_ASCII, "%.20s", szCardTemp);
	lcdDisplay(0, 3, DISP_ASCII, "VOUCHER NO:%06ld", pstLog->lTraceNo);
	lcdDisplay(0, 4, DISP_ASCII, "AUTH NO   :%s",    pstLog->szAuthNo);	
	
	memcpy(sTranAmt, pstLog->sAmount, 6);
	if( pstLog->iTransNo==ADJUST_TIP )
	{
		BcdAdd(sTranAmt, pstLog->sTipAmount, 6);
	}

	ConvBcdAmount(sTranAmt, szBuff);
	lcdDisplay(0, 5, DISP_ASCII, "AMOUNT    :%s", szBuff);
	lcdDisplay(0, 6, DISP_ASCII, "%.4s/%.2s/%.2s %.2s:%.2s:%.2s",
			&pstLog->szDate[0], 
			&pstLog->szDate[4], &pstLog->szDate[6],
			&pstLog->szTime[0], &pstLog->szTime[2], &pstLog->szTime[4]);
	lcdDisplay(0, 7, DISP_ASCII, "BATCH NO  :%06ld", pstLog->lBatchNumber);
}

void  GetTransStatusInfo(NEWPOS_LOG_STRC *pstLog,char *pszStatus)
{
	char  szBuffer[20];

	memset(szBuffer,0,sizeof(szBuffer));

	if (pstLog->ucVoidId == TRUE)
	{
		strcpy(szBuffer,"已撤");
	}
	else
	{
		if (pstLog->ucSendFlag == TRUE && pstLog->ucAdjusted == TRUE)
		{
			strcpy(szBuffer,"已调已送");
		}
		else
		{
			if (pstLog->ucSendFlag == TRUE || pstLog->ucSendFail == TS_NOT_CODE)
			{
				strcpy(szBuffer,"已上送");
			}
			
			if (pstLog->ucAdjusted == TRUE)
			{
				strcpy(szBuffer,"已调整");
			}
		}
	}

	if (strlen(szBuffer))
	{
		sprintf(pszStatus,"(%s)",szBuffer);
	}
}

uint8_t queryTrans(void)
{
	uint8_t		ucRet, szTraceNo[6+1];
	int		iCnt, iRet;
	int		lTraceNo;
	NEWPOS_LOG_STRC	stLog;
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "交易查询        ", "  QUERY TRANS   ");

	if( stTransCtrl.iTransNum==0 )
	{
		return E_NO_TRANS;
	}
	
	DispMulLanguageString(0, 2, DISP_CFONT, "输入凭证号", "INPUT TRACE:");
	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 1, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)szTraceNo);
	if( iRet<0 )
	{
		return E_TRANS_CANCEL;
	}
	lTraceNo = atol((char *)szTraceNo);
	
	for(iCnt=0; iCnt<stTransCtrl.iTransNum; iCnt++)
	{
		memset(&stLog, 0, sizeof(stLog));
		ucRet = LoadTranLog(iCnt, &stLog);
		if( ucRet!=OK )
		{
			return ucRet;
		}
		if( stLog.lTraceNo==lTraceNo )
		{
			break;
		}
	}
	if( iCnt>=stTransCtrl.iTransNum )
	{
		return E_NO_OLD_TRANS;
	}
	
	DispOneTrans(&stLog);
	kbGetKeyMs(stPosParam.ucOprtLimitTime*1000);
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "交易查询        ", "  QUERY TRANS   ");
	
	return OK;
}
uint8_t M1_queryTrans(void)  
{
   // int iRet;
	UINT8 buf[50],szDate[7+1],szDisp[50];
	INT32 iTotalNum,lCurIndex;
	INT32  iTranNum,iTotalTranAmt;
    INT32	 iCnt, iRet,i;
	int		lTraceNo;
	uint8_t		ucRet, szTraceNo[6+1];
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    交易查询    ", "  QUERY TRANS   ");	
	
//----------------------
DispMulLanguageString(0, 2, DISP_CFONT, "输入凭证号", "INPUT TRACE:");
lcdGoto(80, 6);
iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 1, 6, 
	(stPosParam.ucOprtLimitTime*1000), (char *)szTraceNo);
if( iRet<0 )
{
	return E_TRANS_CANCEL;
}
lTraceNo = atol((char *)szTraceNo);


//-----------------------------
  
 /*  if( iRet==14 )
   {
	   //memcpy(szDate, buf, 8);
	   buf[14] = 0x00;
	   if( CheckDate((char *)buf)!=0 )
	   {
		   FailBeep();
		   lcdClrLine(4, 7);
		   lcdFlip();  
		   DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "日期格式错误", "DATE FORMAT ERR");
		   kbGetKeyMs(2000);
		   return NO_DISP;
	   }
	   PubAsc2Bcd(buf,14,szDate);//4 				   //4 位bcd
	// memcpy(szDate, buf, 14);
   }
   else
   {
		FailBeep();
		lcdClrLine(2,7);
		lcdDisplay(0, 4, DISP_CFONT, "销售日期补采取消");
		kbGetKeyMs(3*1000);
		return NO_DISP;

   }*/
	lCurIndex = 0;
	iCnt=0;
	iTranNum=0;
	iTotalTranAmt=0;
	memset(&gstYktEnrichLog, 0, sizeof(gstYktEnrichLog));
	iTotalNum= ReadYtkEnrichLog(&iCnt);
	if( iTotalNum==0 )
	{
	   lcdCls();
	   PubDisplayTitle(TRUE,"当批交易查询");
	   PubDispString("无交易", 3|DISP_LINE_CENTER);
	   PubWaitKey(10);
	   return 0;
	}
	for(i=0;i<iTotalNum;i++)
	{
	  ReadYtkEnrichLog(&iCnt);
	  if(lTraceNo==gstYktEnrichLog.lEnrichTraceNo)
	  {
	  
	  	break;
	  }
	  else
	  {
		iCnt--;
		if (iCnt == 0)
		{
		 	iCnt=iTotalNum;

		}

	  }
	}
	  if(i==iTotalNum)
	  {
			FailBeep();
			lcdClrLine(2,7);
			lcdDisplay(0, 4, DISP_CFONT, "无相应的交易");
			kbGetKeyMs(3*1000);
			return NO_DISP;
	  }
//-----------------------
		lcdClrLine(2,7);
	    lcdDisplay(0, 2, DISP_ASCII, "No:%s", gstYktEnrichLog.sCardSerialNo+8);
		 // lcdDisplay(0, 3, DISP_CFONT, "%s", YktRecord.lTraceNo);
		 lcdDisplay(0, 3, DISP_ASCII, "CSN:%s", gstYktEnrichLog.sCSN);
		 lcdDisplay(0, 4, DISP_ASCII, "AMOUNT: %d.%02d", gstYktEnrichLog.ulTransAmt/100,gstYktEnrichLog.ulTransAmt%100);
		 lcdDisplay(0, 5, DISP_ASCII, "TraceNo: %d", gstYktEnrichLog.lEnrichTraceNo);

		memset(szDisp, 0, sizeof(szDisp));// sTranDateTime
		 sprintf((char *)szDisp, "%02X%02X%02X%02X %02X%02X%02X",
		gstYktEnrichLog.sTranDateTime[0], gstYktEnrichLog.sTranDateTime[1],
		gstYktEnrichLog.sTranDateTime[2],gstYktEnrichLog.sTranDateTime[3], 
		gstYktEnrichLog.sTranDateTime[4], gstYktEnrichLog.sTranDateTime[5],
		gstYktEnrichLog.sTranDateTime[6]);
	    lcdDisplay(0, 6, DISP_ASCII, "TIME:%s",szDisp);
		if(gstYktEnrichLog.uiStatus==1)
		{//已经结算
		   lcdDisplay(0, 7, DISP_ASCII, "Status: settle");//settlement

		}
		else
		{
			lcdDisplay(0, 7, DISP_ASCII, "Status: no settle");

		}
		//-----------
		 iRet = PubWaitKey(30);
		 switch (iRet)
		 {
		
		 case KEY_FN:
			 PubDisplayTitle(TRUE,"重 打 印");
			 lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"是否重印该记录");
			 PubDispString("[取消]	[确认]", DISP_LINE_CENTER|6);
			 if( PubYesNo(30) )
			 {
			  PrtM1TranEnrichTicket(REPRINT);
				// PrtYktTranEnrichTicket(REPRINT);//PrnChkTicketReceipt(PRN_REPRINT);
				 PubDisplayTitle(TRUE,"查询交易明细");
			 }
			 break;
		 default:
			 PubBeepErr();
			 break; 
		 }

    return 0;


}

uint8_t  M1_QueryTransTotal(void)
{
	INT32 iTotalNum,lCurIndex;
	INT32  iTranNum,iTotalTranAmt;
    INT32	 iCnt, i;//iRet,
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    交易结算    ", "  QUERY TRANS   ");
	kbFlush();
	// PubDisplayTitle(TRUE,"交易明细查询");
	//memset(szDisp, 0, sizeof(szDisp));
	lCurIndex = 0;
	iCnt=0;
	iTranNum=0;
	iTotalTranAmt=0;
	 memset(&gstYktEnrichLog, 0, sizeof(gstYktEnrichLog));
	iTotalNum= ReadYtkEnrichLog(&iCnt);
	if( iTotalNum==0 )
	{
	   lcdCls();
	   PubDisplayTitle(TRUE,"当批交易查询");
	   PubDispString("无交易", 3|DISP_LINE_CENTER);
	   PubWaitKey(10);
	   return 0;
	}
	for(i=0;i<iTotalNum;i++)
	{
	  ReadYtkEnrichLog(&iCnt);
	  if(gstYktEnrichLog.uiStatus==0x01)//结算了
	  {
		  iCnt--;
		  if (iCnt == 0)
			 {
				 iCnt=iTotalNum;
 
			 }
		 // iCnt=iCnt%iTotalNum;
	  }
	  else
	  {
	    iTranNum++;
		iTotalTranAmt=iTotalTranAmt+ gstYktEnrichLog.ulTransAmt;
		iCnt--;
		if (iCnt == 0)
		{
		 iCnt=iTotalNum;

		}
	  }
	}
	lcdClrLine(2,7);
	lcdDisplay(0, 2, DISP_CFONT, "交易笔数: %d", iTranNum);
	lcdDisplay(0, 4, DISP_CFONT, "交易金额: %d.%02d", iTotalTranAmt/100,iTotalTranAmt%100);
	kbGetKeyMs(10*1000);
	kbFlush();
    return 0;
	//	return 0;



}

uint8_t QueryTransTotal()
{
	uint8_t	ucRet, szBuff[30];
	uint8_t   szTotal[10];
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "查询交易汇总    ", "  QUERY TOTAL   ");

	ucRet = ComputeTransTotal(1);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "查询交易汇总    ", "  QUERY TOTAL   ");
	sprintf((char *)szBuff, "%03d", stTotal.iRmbDebitNum);
	DispMulLanguageString(0, 2, DISP_CFONT, "内卡销售总计:", "SALE TOTAL:");
	lcdDisplay(0, 4, DISP_CFONT, "内卡笔数: %s", szBuff);
	
	memset(szTotal, 0, sizeof(szTotal));
	memcpy(szTotal, stTotal.sRmbDebitAmt, 6);
	ConvBcdAmount(szTotal, szBuff);
	lcdDisplay(0, 6, DISP_CFONT, "金额:%s", szBuff);
	ucRet = kbGetKeyMs(stPosParam.ucOprtLimitTime*1000);
	if( ucRet==KEY_CANCEL )
	{
		return NO_DISP;
	}
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "查询交易汇总    ", "  QUERY TOTAL   ");
	lcdClrLine(2, 7);
	lcdFlip();	
	sprintf((char *)szBuff, "%03d", stTotal.iRmbCreditNum);
	DispMulLanguageString(0, 2, DISP_CFONT, "内卡退货总计:", "SALE TOTAL:");
	lcdDisplay(0, 4, DISP_CFONT, "内卡笔数: %s", szBuff);
	memset(szTotal, 0, sizeof(szTotal));
	memcpy(szTotal, stTotal.sRmbCreditAmt, 6);
	ConvBcdAmount(szTotal, szBuff);
	lcdDisplay(0, 6, DISP_CFONT, "金额:%s", szBuff);
	ucRet = kbGetKeyMs(stPosParam.ucOprtLimitTime*1000);
	if( ucRet==KEY_CANCEL )
	{
		return NO_DISP;
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "查询交易汇总    ", "  QUERY TOTAL   ");
	sprintf((char *)szBuff, "%03d", stTotal.iFrnDebitNum);
	DispMulLanguageString(0, 2, DISP_CFONT, "外卡销售总计:", "SALE TOTAL:");
	lcdDisplay(0, 4, DISP_CFONT, "外卡笔数: %s", szBuff);
	
	memset(szTotal, 0, sizeof(szTotal));
	memcpy(szTotal, stTotal.sFrnDebitAmt, 6);
	ConvBcdAmount(szTotal, szBuff);
	lcdDisplay(0, 6, DISP_CFONT, "金额:%s", szBuff);
	ucRet = kbGetKeyMs(stPosParam.ucOprtLimitTime*1000);
	if( ucRet==KEY_CANCEL )
	{
		return NO_DISP;
	}
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "查询交易汇总    ", "  QUERY TOTAL   ");
	lcdClrLine(2, 7);
	lcdFlip();	
	sprintf((char *)szBuff, "%03d", stTotal.iFrnCreditNum);
	DispMulLanguageString(0, 2, DISP_CFONT, "外卡退货总计:", "SALE TOTAL:");
	lcdDisplay(0, 4, DISP_CFONT, "外卡笔数: %s", szBuff);
	memset(szTotal, 0, sizeof(szTotal));
	memcpy(szTotal, stTotal.sFrnCreditAmt, 6);
	ConvBcdAmount(szTotal, szBuff);
	lcdDisplay(0, 6, DISP_CFONT, "金额:%s", szBuff);
	ucRet = kbGetKeyMs(stPosParam.ucOprtLimitTime*1000);
	if( ucRet==KEY_CANCEL )
	{
		return NO_DISP;
	}

	return OK;
}

uint8_t DispOrignalTrans(NEWPOS_LOG_STRC *pstLog)
{
	uint8_t	szBuff[33];
	uint8_t szMaskCardNo[50];

	memset(szMaskCardNo,0,sizeof(szMaskCardNo));
	MaskPan(pstLog->szCardNo,szMaskCardNo);

	lcdClrLine(2, 7);
	lcdFlip();	
	GetTransName(pstLog->iTransNo, szBuff);
	lcdDisplay(0, 2, DISP_CFONT, (char *)szBuff);
	lcdDisplay(0, 4, DISP_ASCII, "%s", szMaskCardNo);
	lcdDisplay(0, 5, DISP_ASCII, "VOUCHER NO:    %06ld", pstLog->lTraceNo);
	ConvBcdAmount(pstLog->sAmount, szBuff);
	lcdDisplay(0, 6, DISP_ASCII, "AMOUNT    :%10.10s", szBuff);
	lcdDisplay(0, 7, DISP_ASCII, "REF. NO: %s", pstLog->szSysReferNo);

	if( kbGetKeyMs(stPosParam.ucOprtLimitTime*1000)==KEY_ENTER )
	{
		return OK;
	}

	return E_TRANS_CANCEL;
}

/************************************************************************/
/* 从filename文件中读取size大小的数据到pdata中                          */
/************************************************************************/
uint8_t read_data(void* pdata, int size, char* filename)
{
	int fd;
	int ret;
	
	fd = fileOpen(filename, O_RDWR);
	if( fd<0 ) 
	{
		return (E_MEM_ERR);
	}
	ret = fileSeek(fd, (int)0, SEEK_SET);
	if( ret<0 ) 
	{
		fileClose(fd);
		return (E_MEM_ERR);
	}
	
	ret = fileRead(fd, (uint8_t *)pdata, size);
	fileClose(fd);
	
	if( ret!=size )
		return (E_MEM_ERR);
	
	return (OK);
}


/************************************************************************/
/* 把pdata指向的size大小的数据存入filename文件中                        */
/************************************************************************/
uint8_t write_data(void* pdata, int size, char* filename)
{
	int fd;
	int ret;
	
	fd = fileOpen(filename, O_RDWR|O_CREAT);
	if( fd<0 ) 
	{
		return (E_MEM_ERR);
	}
	
	ret = fileSeek(fd, (int)0, SEEK_SET);
	if( ret<0 ) 
	{
		fileClose(fd);
		return (E_MEM_ERR);
	}
	ret = fileWrite(fd, (uint8_t *)pdata, size);
	if( ret!=size )
	{
		fileClose(fd);
		return (E_MEM_ERR);
	}
	fileClose(fd);
	return (OK);
}

uint8_t ReadTermAidList(void)
{
	int		iRet, iAidFile;

	glTermAidNum = 0;
	memset(&glTermAidList[0], 0, sizeof(glTermAidList));
	iAidFile = fileOpen(TERM_AIDLIST, O_RDWR);
	if( iAidFile<0 )
	{
		return E_MEM_ERR;
	}

	iRet = fileRead(iAidFile, (uint8_t *)&glTermAidList[0], sizeof(glTermAidList));
	fileClose(iAidFile);
	if( iRet<0 )
	{
		return E_MEM_ERR;
	}
	glTermAidNum = iRet/sizeof(TermAidList);

	return OK;
}

uint8_t WriteScriptFile(void)
{
	int		fd;
		
	fd = fileOpen(ICC_SCRIPT_FILE, O_RDWR|O_CREAT);
	if( fd<0 )
	{
		return E_MEM_ERR;
	}
	
	if( fileWrite(fd, (uint8_t *)&PosCom, COM_SIZE)!=COM_SIZE )
	{
		fileClose(fd);
		return E_MEM_ERR;
	}
	fileClose(fd);
	
	return OK;
}

uint8_t ReadScriptFile(void)
{
	int		fd;
	
	fd = fileOpen(ICC_SCRIPT_FILE, O_RDWR);
	if( fd<0 )
	{
		return E_MEM_ERR;
	}
	
	if( fileRead(fd, (uint8_t *)&PosCom, COM_SIZE)!=COM_SIZE )
	{
		fileClose(fd);
		return E_MEM_ERR;
	}
	fileClose(fd);
	
	return OK;
}

uint8_t LoadTranLog(int iRecNo, NEWPOS_LOG_STRC *pstTranLog)
{
	int		iRet, iLogFile;
	
	iLogFile = fileOpen(POS_LOG_FILE, O_RDWR);
	if( iLogFile<0 )
	{
		return E_MEM_ERR;
	}
	
	iRet = fileSeek(iLogFile, (int)(iRecNo * LOG_RECORD_LEN), SEEK_SET);
	if( iRet<0 )
	{
		fileClose(iLogFile);
		return E_MEM_ERR;
	}
	
	iRet = fileRead(iLogFile, (uint8_t *)pstTranLog, LOG_RECORD_LEN);
	fileClose(iLogFile);
	if( iRet!=LOG_RECORD_LEN )
	{
		return E_MEM_ERR;
	}
	
	return OK;
}

uint8_t UpdateTranLog(int iRecNo, NEWPOS_LOG_STRC *pstTranLog)
{
	int		iRet, iLogFile;
	
	iLogFile = fileOpen(POS_LOG_FILE, O_RDWR);
	if( iLogFile<0 )
	{
		return E_MEM_ERR;
	}
	
	iRet = fileSeek(iLogFile, (int)(iRecNo * LOG_RECORD_LEN), SEEK_SET);
	if( iRet<0 )
	{
		fileClose(iLogFile);
		return E_MEM_ERR;
	}
	
	iRet = fileWrite(iLogFile, (uint8_t *)pstTranLog, LOG_RECORD_LEN);
	fileClose(iLogFile);
	if( iRet!=LOG_RECORD_LEN )
	{
		return E_MEM_ERR;
	}
	
	return OK;
}

void  LoadDefaultAppParam(void)
{
	ST_COMM_CONFIG      stCommCfgBak;
	unsigned long       ulTimeOut;

	memset(&stPosParam,0,sizeof(stPosParam));
	
	stCommCfgBak = stPosParam.stDownParamCommCfg;
	ulTimeOut = stPosParam.ucCommWaitTime;

	strcpy((char *)stPosParam.szPhoneNum1,"0625");
	strcpy((char *)stPosParam.szPhoneNum2,"0625");
	strcpy((char *)stPosParam.szPhoneNum3,"0625");
	strcpy((char *)stPosParam.szMPhone,"0625");
	strcpy((char *)stPosParam.szDownloadTel,"0625");
	strcpy((char *)stPosParam.szUnitNum,"000000000000000");
	strcpy((char *)stPosParam.szPosId,"00000000");
	stPosParam.ucSendOffFlag = PARAM_OPEN;
	memcpy((char *)stPosParam.sTpdu,"\x60\x30\x00\x00\x00\x00",6);
	stPosParam.ucTipOpen  = PARAM_OPEN ;
	memcpy((char *)stPosParam.sTransOpen,"\xFF\xFD\xFF\xFF",4);	 // 默认电子钱包关闭
	stPosParam.ucAutoLogoff = PARAM_CLOSE;	
	stPosParam.ucManualInput = PARAM_CLOSE ;
	stPosParam.ucErrorPrt = PARAM_CLOSE;
	stPosParam.ucDetailPrt = PARAM_CLOSE;
	stPosParam.ucPrnEng = PARAM_OPEN;
	stPosParam.ucOfflineSettleFlag = PARAM_OPEN;
	strcpy((char *)stPosParam.szUnitChnName,"NEWPOS测试商户");
	strcpy((char *)stPosParam.szEngName,"NEWPOS TEST MERCHANT");	
	stPosParam.ucTestFlag = PARAM_CLOSE; 	
	stPosParam.ucVoidSwipe  = PARAM_OPEN;	
	stPosParam.ucAuthVoidSwipe  = PARAM_OPEN;	
	stPosParam.ucConfirm = '1';	
	stPosParam.ucVoidPin =  PARAM_OPEN;	
	stPosParam.ucPreVoidPin = PARAM_OPEN;
	stPosParam.ucPreComVoidPin = PARAM_OPEN;
	stPosParam.ucPreComPin = PARAM_OPEN;
	stPosParam.ucKeyMode = KEY_MODE_DES;    
	stPosParam.ucEmvSupport = PARAM_CLOSE ;
	stPosParam.ucqPbocSupport = PARAM_CLOSE ;
	stPosParam.ucSelectCardType = PARAM_CLOSE ;
	stPosParam.ucPartPaySupport = PARAM_CLOSE ;
	strcpy((char *)stPosParam.szParamsPwd,"00000000");
	strcpy((char *)stPosParam.szSafepwd, "00000000");
 	strcpy((char *)stPosParam.sTpdu,"6022000000");  
	strcpy((char *)stPosParam.szTransCtl,"FFFFFFFF"); 
	stPosParam.bPreDial = TRUE ;       // 是否预拨号	
	stPosParam.ucTicketNum = '2';	
	stPosParam.ucOprtLimitTime = 60;  
	stPosParam.ucCommWaitTime = 30 ; 
	stPosParam.ucTipper = 10;		
	stPosParam.ucAdjustPercent = 10;	
	stPosParam.ucResendTimes = 2;	
	stPosParam.ucDialRetryTimes = 2;	
	stPosParam.ucKeyIndex = 0;		
	stPosParam.iMaxTransTotal = 500;  
	stPosParam.lNowTraceNo = 12; 	
	stPosParam.lNowBatchNum = 2;	
	stPosParam.lMaxRefundAmt = 1000000;
	stPosParam.iDispLanguage = 1;   // 显示语言
	stPosParam.ucMaxOfflineTxn = 10;  // 
	strcpy((char *)stPosParam.szMerBankId, "0000");
	strcpy((char *)stPosParam.szLocalId, "0000");
	stPosParam.ucPrnTitleFlag = PARAM_OPEN;
	strcpy(stPosParam.szPrnTitle,"银联POS");

	// 默认通讯类型
	stPosParam.stTxnCommCfg.ucCommType  = gstPosCapability.uiCommType;
	stPosParam.stTxnCommCfg.stRS232Para.ucPortNo   = PORT_COM1;
	stPosParam.stTxnCommCfg.stRS232Para.ucSendMode = CM_SYNC;
	strcpy((char *)stPosParam.stTxnCommCfg.stRS232Para.szAttr, "9600,8,n,1");
	// TCP/IP缺省参数
	strcpy((char *)stPosParam.stTxnCommCfg.stTcpIpPara.szRemoteIP,  "210.12.207.186");
	strcpy((char *)stPosParam.stTxnCommCfg.stTcpIpPara.szRemotePort, "9000");
	
	strcpy((char *)stPosParam.stTxnCommCfg.stWirlessPara.szAPN, "CMNET");
	strcpy((char *)stPosParam.stTxnCommCfg.stWirlessPara.szRemoteIP,   "210.12.207.186");
	strcpy((char *)stPosParam.stTxnCommCfg.stWirlessPara.szRemotePort, "9000");
	strcpy((char *)stPosParam.stTxnCommCfg.stWirlessPara.szUID, "CARD");
	strcpy((char *)stPosParam.stTxnCommCfg.stWirlessPara.szPwd, "CARD");

	strcpy((char *)stPosParam.stTxnCommCfg.stWIFIIpPara.szRemoteIP,   "210.12.207.188");
	strcpy((char *)stPosParam.stTxnCommCfg.stWIFIIpPara.szRemotePort, "9000");

	strcpy((char *)stPosParam.stTxnCommCfg.stPSTNPara.szTxnTelNo,   "82790625");
	stPosParam.stTxnCommCfg.stPSTNPara.ucSendMode = MODEM_COMM_SYNC;
	stPosParam.stTxnCommCfg.stPSTNPara.ucSendMode = MODEM_COMM_SYNC;
	stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.connect_mode = MODEM_COMM_SYNC;
	stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.connect_speed = 1200;
	stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.dial_mode = MODEM_DAIL_DTMF;
	stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.dial_pause = 1;
	stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.dial_timeo = 30;
	stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.extension_chk = 1;
	stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.idle_timeo = 60;

	if( !(stCommCfgBak.ucCommType & CT_ALLCOMM) || (stCommCfgBak.ucCommType > CT_ALLCOMM) )
	{
		stPosParam.stDownParamCommCfg.ucCommType = gstPosCapability.uiCommType;
		strcpy((char *)stPosParam.stDownParamCommCfg.stWirlessPara.szAPN, "CMNET");
		strcpy((char *)stPosParam.stDownParamCommCfg.stWirlessPara.szRemoteIP,   "210.22.14.210");
		strcpy((char *)stPosParam.stDownParamCommCfg.stWirlessPara.szRemotePort, "9006");
		strcpy((char *)stPosParam.stDownParamCommCfg.stWirlessPara.szUID, "CARD");
		strcpy((char *)stPosParam.stDownParamCommCfg.stWirlessPara.szPwd, "CARD");
		stPosParam.stDownParamCommCfg.stRS232Para.ucPortNo   = PORT_COM1;
		stPosParam.stDownParamCommCfg.stRS232Para.ucSendMode = CM_SYNC;
		strcpy((char *)stPosParam.stDownParamCommCfg.stRS232Para.szAttr, "9600,8,n,1");
	}
	else
	{
		stPosParam.stDownParamCommCfg = stCommCfgBak;
		stPosParam.ucCommWaitTime = ulTimeOut;
	}

	SaveAppParam();
}


void  AdjustAppParam(void)
{
	stPosParam.iDispLanguage = 1;
	if (stPosParam.ucOprtLimitTime < 5)
	{
		stPosParam.ucOprtLimitTime = 5;
	}

	AscToBcd(stPosParam.sTpdu,stPosParam.szTpdu,10);
	AscToBcd(stPosParam.sTransOpen,stPosParam.szTransCtl,8);
	stPosParam.ucTicketNum |= 0x30;
	stPosParam.ucDialRetryTimes = stPosParam.ucDialRetryTimes & 0x0F;

	switch (stPosParam.stTxnCommCfg.ucCommType)
	{
	case 'G':
	case 'g':
		stPosParam.stTxnCommCfg.ucCommType = CT_GPRS;
		break;
	case 'C':
	case 'c':
		stPosParam.stTxnCommCfg.ucCommType = CT_CDMA;
		break;
	case 't':
	case 'T':
		stPosParam.stTxnCommCfg.ucCommType = CT_TCPIP;
		break;
	case 'r':
	case 'R':
		stPosParam.stTxnCommCfg.ucCommType = CT_RS232;
		break;
	case 'w':
	case 'W':
		stPosParam.stTxnCommCfg.ucCommType = CT_WIFI;
		break;
	case 'm':
	case 'M':
		stPosParam.stTxnCommCfg.ucCommType = CT_MODEM;
		break;
	default:
		stPosParam.stTxnCommCfg.ucCommType = gstPosCapability.uiCommType;
		stPosParam.stTxnCommCfg.ucCommType = stPosParam.stTxnCommCfg.ucCommType & ~CT_RS232;
		break;
	}
	
	if (!(stPosParam.stTxnCommCfg.ucCommType & gstPosCapability.uiCommType))
	{
		stPosParam.stTxnCommCfg.ucCommType = gstPosCapability.uiCommType;
		stPosParam.stTxnCommCfg.ucCommType = stPosParam.stTxnCommCfg.ucCommType & ~CT_RS232;
	}

	if (gstPosCapability.uiCommType & CT_MODEM)
	{
		stPosParam.stTxnCommCfg.stPSTNPara.ucSendMode = MODEM_COMM_SYNC;
		stPosParam.stTxnCommCfg.stPSTNPara.ucSendMode = MODEM_COMM_SYNC;
		stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.connect_mode = MODEM_COMM_SYNC;
		stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.connect_speed = 1200;
		stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.dial_mode = MODEM_DAIL_DTMF;
		stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.dial_pause = 1;
		stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.dial_timeo = 30;
		stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.extension_chk = 1;
		stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.idle_timeo = 60;
		stPosParam.stDownParamCommCfg.stPSTNPara = stPosParam.stTxnCommCfg.stPSTNPara;
	}
	
	strcpy((char *)stPosParam.stTxnCommCfg.stTcpIpPara.szRemoteIP, (char *)stPosParam.stTxnCommCfg.stWirlessPara.szRemoteIP);
	strcpy((char *)stPosParam.stTxnCommCfg.stTcpIpPara.szRemotePort, (char *)stPosParam.stTxnCommCfg.stWirlessPara.szRemotePort);
	stPosParam.stTxnCommCfg.stWIFIIpPara  = stPosParam.stTxnCommCfg.stTcpIpPara;
	stPosParam.stDownParamCommCfg.ucCommType = stPosParam.stTxnCommCfg.ucCommType;
	if (strlen(stPosParam.szFtpPath) > 0 && stPosParam.szFtpPath[strlen(stPosParam.szFtpPath) - 1] != '/' && 
		stPosParam.szFtpPath[strlen(stPosParam.szFtpPath) - 1] != '\\')
	{
		stPosParam.szFtpPath[strlen(stPosParam.szFtpPath)] = '/';
		stPosParam.szFtpPath[strlen(stPosParam.szFtpPath) + 1] = 0;
	}
	return ;
}


int  SaveAppParam(void)
{
	int fd;
	int ret;
	
	fd = fileOpen(FILE_APP_LOG, O_RDWR|O_CREAT);
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
	ret = fileWrite(fd, (uint8_t *)&stPosParam, sizeof(struct  _NEWPOS_PARAM_STRC));
	if( ret!=sizeof(struct  _NEWPOS_PARAM_STRC) )
	{
		DispFileErrInfo();
		fileClose(fd);	
		return (E_MEM_ERR);
	}
	fileClose(fd);
	return (OK);
}


int   LoadAppParam(void)
{
	int fd;
	int ret;
	
	fd = fileOpen(FILE_APP_LOG, O_RDWR);
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
	
	ret = fileRead(fd, (uint8_t *)&stPosParam, sizeof(struct _NEWPOS_PARAM_STRC));
	fileClose(fd);
	
	if( ret!=sizeof(struct _NEWPOS_PARAM_STRC) )
	{
		DispFileErrInfo();
		return (E_MEM_ERR);
	}
	
	return (OK);
}


int  emvSaveAIDList(int iNo, const struct terminal_aid_info *info)
{
	int		iRet, iLogFile;

	iLogFile = fileOpen(FILE_EMV_AID, O_RDWR|O_CREAT);
	if( iLogFile<0 )
	{
		DispFileErrInfo();
		return (E_MEM_ERR);
	}

	iRet = fileSeek(iLogFile, (iNo*sizeof(*info)), SEEK_SET);
	if( iRet<0 ) 
	{
		DispFileErrInfo();
		fileClose(iLogFile);
		return (E_MEM_ERR);
	}

	iRet = fileWrite(iLogFile, (uint8_t *)info, sizeof(*info));
	fileClose(iLogFile);
	if( iRet!=sizeof(*info) )
	{
		DispFileErrInfo();
		return (E_MEM_ERR);
	}
	

	return OK;
} 

int emvLoadAIDList(int iNo, struct terminal_aid_info *info)
{
	int		iRet, iLogFile;
	
	iLogFile = fileOpen(FILE_EMV_AID, O_RDWR);
	if( iLogFile<0 )
	{
		DispFileErrInfo();
		return (E_MEM_ERR);
	}

	iRet = fileSeek(iLogFile, iNo*sizeof(*info), SEEK_SET);
	if( iRet<0 ) 
	{
		DispFileErrInfo();
		fileClose(iLogFile);
		return (E_MEM_ERR);
	}

	iRet = fileRead(iLogFile, (uint8_t *)info, sizeof(*info));
	fileClose(iLogFile);
	if( iRet!=sizeof(*info) )
	{
		DispFileErrInfo();
		return (E_MEM_ERR);
	}
	

	return OK;
} 

int  emvSaveCAPKList(int iNo, const struct issuer_ca_public_key *info)
{
	int		iRet, iLogFile;

	
	iLogFile = fileOpen(FILE_EMV_CAPK, O_RDWR|O_CREAT);
	if( iLogFile<0 )
	{
		DispFileErrInfo();
		return (E_MEM_ERR);
	}

	iRet = fileSeek(iLogFile, (iNo*sizeof(*info)), SEEK_SET);
	if( iRet<0 ) 
	{
		DispFileErrInfo();
		fileClose(iLogFile);
		return (E_MEM_ERR);
	}

	iRet = fileWrite(iLogFile, (uint8_t *)info, sizeof(*info));
	fileClose(iLogFile);
	if( iRet!=sizeof(*info) )
	{
		DispFileErrInfo();
		return (E_MEM_ERR);
	}
	

	return OK;
} 

int emvLoadCAPKList(int iNo, struct issuer_ca_public_key *info)
{
	int		iRet, iLogFile;
	
	iLogFile = fileOpen(FILE_EMV_CAPK, O_RDWR);
	if( iLogFile<0 )
	{
		DispFileErrInfo();
		return (E_MEM_ERR);
	}

	iRet = fileSeek(iLogFile, iNo*sizeof(*info), SEEK_SET);
	if( iRet<0 ) 
	{
		DispFileErrInfo();
		fileClose(iLogFile);
		return (E_MEM_ERR);
	}

	iRet = fileRead(iLogFile, (uint8_t *)info, sizeof(*info));
	fileClose(iLogFile);
	if( iRet!=sizeof(*info) )
	{
		DispFileErrInfo();
		return (E_MEM_ERR);
	}
	

	return OK;
} 

//所有交易状态为无效
void   ClearAllTxnStatus(void)
{
	int iCnt;

	for (iCnt=0; iCnt<MAX_TRANLOG; iCnt++)
	{
		stTransCtrl.sTxnRecIndexList[iCnt] = INV_TXN_INDEX;
	}
}

// 设置交易状态
void   SetTxnCtrlStatus(POS_COM *pstTxnLog)
{
	uint32_t  uiStatus  = 0;

	// 根据各个交易类型设置交易对应的状态

	if( pstTxnLog->stTrans.iTransNo ==OFF_SALE || pstTxnLog->stTrans.iTransNo ==OFF_ADJUST ||
		pstTxnLog->stTrans.iTransNo ==ADJUST_TIP )
	{
		uiStatus |= TS_TXN_OFFLINE;
	}

	stTransCtrl.sTxnRecIndexList[pstTxnLog->stTrans.uiTxnRecIndex] = uiStatus;
	SaveCtrlParam();
}

// 更新交易状态
void   UpdataTxnCtrlStatus(uint32_t uiTxnIndex,uint32_t uiStatus)
{
	if (uiTxnIndex >= MAX_TRANLOG)
	{
		return;
	}

	stTransCtrl.sTxnRecIndexList[uiTxnIndex] = uiStatus;
	SaveCtrlParam();
}

// 获取脱机交易的笔数
int    GetOfflineTxnNums(void)
{
	int iCnt , iTotalNum, iSendTimes;

	iTotalNum = 0;
	for(iCnt=0; iCnt<=stTransCtrl.iTransNum; iCnt++)
	{
		if (stTransCtrl.sTxnRecIndexList[iCnt] == INV_TXN_INDEX)
		{
			continue;
		}

		iSendTimes = stTransCtrl.sTxnRecIndexList[iCnt] & 0x0000000F;

		if (iSendTimes>=stPosParam.ucResendTimes+1)
		{
			continue;
		}

		if (stTransCtrl.sTxnRecIndexList[iCnt]&TS_TXN_OFFLINE)
		{
			iTotalNum++;
		}

		if (stTransCtrl.sTxnRecIndexList[iCnt]&TS_ICC_OFFLINE)
		{
			iTotalNum++;
		}
	}

	return iTotalNum;
}

int     SaveBlkCard(ST_BLACKLIST_TOTAL *pstBlkCard)
{
	int fd;
	int ret;
	
	fd = fileOpen(FILE_BLKCARD_LOG, O_RDWR|O_CREAT);
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
	ret = fileWrite(fd, (uint8_t *)pstBlkCard, sizeof(ST_BLACKLIST_TOTAL));
	if( ret!=sizeof(ST_BLACKLIST_TOTAL) )
	{
		DispFileErrInfo();
		fileClose(fd);	
		return (E_MEM_ERR);
	}
	fileClose(fd);
	return (OK);
}


int     LoadBlkCard(ST_BLACKLIST_TOTAL *pstBlkCard)
{
	int fd;
	int ret;
	
	fd = fileOpen(FILE_BLKCARD_LOG, O_RDWR);
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
	
	ret = fileRead(fd, (uint8_t *)pstBlkCard, sizeof(ST_BLACKLIST_TOTAL));
	fileClose(fd);
	
	if( ret!=sizeof(ST_BLACKLIST_TOTAL) )
	{
		DispFileErrInfo();
		return (E_MEM_ERR);
	}
	
	return (OK);
}

// 获取记录中上送次数最少的交易, ucTxnType 代表是磁条类还是芯片卡类
int    GetMinSendTimesTxn(uint32_t uiTxnType)
{
	int iCnt , iMinSends, iSendTimes;
	int  iTxnIndex;
	
	iTxnIndex = -1;
	iMinSends = 0x0F;
	for(iCnt=0; iCnt<=stTransCtrl.iTransNum; iCnt++)
	{
		if (stTransCtrl.sTxnRecIndexList[iCnt] == INV_TXN_INDEX)
		{
			continue;
		}

		if (!(stTransCtrl.sTxnRecIndexList[iCnt] & uiTxnType))
		{
			continue;
		}
		
		iSendTimes = stTransCtrl.sTxnRecIndexList[iCnt] & 0x0000000F;
		if (iSendTimes >= stPosParam.ucResendTimes+1)
		{
			continue;
		}

		if (iMinSends > iSendTimes)
		{
			iMinSends = iSendTimes;
			iTxnIndex = iCnt;
		}
	}
	
	return iTxnIndex;
}
//文件信息///////
//前8字节是4记录个数+4当前最后一个记录位置如1(cong 1kaishi)
//最大MAX_YKT_TRANLOG个记录
//接调用该函数即可保存该结构的记录，
#define REC_BYTE_NUM    4  //局部配置--前2个字段信息长度
#define MAX_LOG_NUM     5000

//函数说明：
//函数名：根据功能定义
//输入：如果是0，则表示保存下一笔记录，否则iOffSet为保存记录的位置范围是1～MAX_YKT_TRANLOG
//      需要保存的结构体(全局变量)
//输出：0成功 -其他失败
INT32 SaveM1EnrichLog(int iOffSet)//如果是制定的iOffSet，iCurIndex不更形
{
	INT32		iRet, iLogFile;
    int         iRecordNum, iCurIndex;
    UINT8       szBuff[2*REC_BYTE_NUM+1], szHome[REC_BYTE_NUM+1];

    
	memset(szHome, 0, sizeof(szHome));
	memset(szBuff, 0, sizeof(szBuff));
	iLogFile = fileOpen(POS_M1_LOG_FILE, O_RDWR|O_CREAT);
	if( iLogFile<0 )
	{
		return E_FILE_OPEN;
	}
    memset(szBuff, '0', sizeof(szBuff));
    szBuff[2*REC_BYTE_NUM] = 0;
    if(fileSize(POS_M1_LOG_FILE)==0)
    {
        iRet = fileWrite(iLogFile, szBuff, 2*REC_BYTE_NUM);
        if( iRet!=2*REC_BYTE_NUM )
        {
            fileClose(iLogFile);
            return E_FILE_WRITE;
        }
    }
	iRet = fileSeek(iLogFile, 0, SEEK_SET);
	if( iRet<0 ) 
	{
		fileClose(iLogFile);
		return E_FILE_SEEK;
	}
	iRet = fileRead(iLogFile, (UINT8 *)szBuff, 2*REC_BYTE_NUM);//前8字节用于保存索引
    PubAsc2Long(szBuff, REC_BYTE_NUM, (ulong *)&iRecordNum);
    PubAsc2Long(&szBuff[REC_BYTE_NUM], REC_BYTE_NUM, (ulong *)&iCurIndex);
    if(iOffSet == 0)//保存的是下一个记录
    {
        iRecordNum++;
        if(iRecordNum >= MAX_LOG_NUM)//此时已经有一个循环的记录了所以......
            iRecordNum =MAX_LOG_NUM;
    	iCurIndex  = iCurIndex %MAX_LOG_NUM+1;//计算当前记录保存位置
		iOffSet=iCurIndex;

	}
    else if((iOffSet > MAX_LOG_NUM)||(iOffSet < 0))//错误记录索引
    {
    	return E_FILE_READ;
    }
    else //要保存的位置
    {   
           // iCurIndex = iOffSet;
           ;
    }

	iRet = fileSeek(iLogFile, (INT32)((iOffSet-1)*YKTENRICH_LOG_SIZE)+2*REC_BYTE_NUM, SEEK_SET);
	if( iRet<0 ) 
	{
		fileClose(iLogFile);
		return E_FILE_SEEK;
	}
	iRet = fileWrite(iLogFile, (UINT8 *)&gstYktEnrichLog, YKTENRICH_LOG_SIZE);
	if( iRet!=YKTENRICH_LOG_SIZE )
	{
        fileClose(iLogFile);
		return E_FILE_WRITE;
	}

    memset(szBuff, 0, sizeof(szBuff));
    sprintf(szBuff, "%04d%04d", iRecordNum, iCurIndex);
	iRet = fileSeek(iLogFile, 0, SEEK_SET);
	if( iRet<0 ) 
	{
		fileClose(iLogFile);
		return E_FILE_SEEK;
	}
	iRet = fileWrite(iLogFile, (UINT8 *)szBuff, 2*REC_BYTE_NUM);
	if( iRet!=2*REC_BYTE_NUM )
	{
        fileClose(iLogFile);
		return E_FILE_WRITE;
	}
	fileClose(iLogFile);
	return OK;
} 
//读记录提供两种方式://最后一笔(0)和所有记录(先读最后)
//返回当前记录位置,所有记录从1开始
INT32 ReadYtkEnrichLog(int *iRecNo)
{
	INT32		iRet, iLogFile;
    UINT32      iRecordNum=0, iCurIndex;
    UINT8       szBuff[REC_BYTE_NUM*2+1];

    
	iLogFile = fileOpen(POS_M1_LOG_FILE, O_RDWR|O_CREAT);
	if( iLogFile<0 )
	{
		return E_FILE_OPEN;
	}


    iRet = fileRead(iLogFile, szBuff, REC_BYTE_NUM*2);//前8字节用于保存索引
    PubAsc2Long(szBuff, REC_BYTE_NUM, (ulong*)&iRecordNum);
    PubAsc2Long(&szBuff[REC_BYTE_NUM], REC_BYTE_NUM, (ulong*)&iCurIndex);
    if(iRecordNum == 0)//无记录则返回
    {
        return 0;
    }
    if(*iRecNo != 0)//纠正要读的索引号
    {
        iCurIndex  = *iRecNo;
    }
    iRet = fileSeek(iLogFile, REC_BYTE_NUM*2, SEEK_SET);
    if( iRet<0 ) 
    {
    
        fileClose(iLogFile);
        return E_FILE_SEEK;
    }
	iRet = fileSeek(iLogFile, (INT32)((iCurIndex-1)*YKTENRICH_LOG_SIZE), SEEK_CUR);
	if( iRet<0 ) 
	{

		fileClose(iLogFile);
		return E_FILE_SEEK;
	}

	iRet = fileRead(iLogFile, (UINT8 *)&gstYktEnrichLog, YKTENRICH_LOG_SIZE);
	fileClose(iLogFile);
	if( iRet!=YKTENRICH_LOG_SIZE )
	{
		return E_FILE_READ;
	}
    
	*iRecNo = iCurIndex;
	return iRecordNum;
} 


// end of file
