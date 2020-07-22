
#ifndef _NEWPOS_TERSTRUC_H
#define _NEWPOS_TERSTRUC_H

#include "posapi.h"
#include "posmacro.h"
#include "st8583.h"
#include "Commlib.h"

typedef struct _POS_8583PACKET_STRC {
	int  iTransNo;			    // 交易代号
	uint8_t  szSevPointCode[2+1];		// 服务点条件码   
	uint8_t  szMessTypeCode[2+1];		// 消息类型码     
	uint8_t  szNetManInfoCode[3+1];	// 网络管理信息码 
} NEWPOS_8583PACKET_STRC;

#define   TS_NOT_RECV    0x02    // 发送但是没有收到返回
#define   TS_NOT_CODE    0x03    // 发送但是返回码非00
#define   TS_INST_SPAC   0x01    // 首期不打印空格
#define   TS_FEE_SPAC    0x02    // 每期不打印空格

typedef struct   POS_LOG_STRUCT{
	uint8_t	ucBatchSendFlag;       
	uint8_t   ucVoidId;       
	uint8_t	ucSendFlag;
	uint8_t	ucSendFail;    // 
	uint8_t	ucAdjusted;	
	uint8_t 	ucChgFrmAdjust; 

	uint8_t   ucInstalAmtFlag; // 标识分钱手续费是否打印空格还是不打印
	uint8_t   sAmount[6];
	uint8_t   sTipAmount[6];
	uint8_t   sPreAddAmount[6];
	uint8_t   sLoyaltAmount[6];

	int   lTraceNo;
	int	lOldTraceNo; 
	int	lBatchNumber;
	int	lOldBatchNumber;    
	int   iTransNo;        
	int   iOldTransNo;
	uint32_t    uiTxnRecIndex;       // 交易记录索引号

	uint8_t   ucInstallmentNum;      // 分期期数
	uint8_t   ucInstalPayMode;       // 分期付款支付方式
	uint8_t   szInstalCode[30+1];    // 商品项目编码
	uint8_t   szInstalCurrCode[3+1]; // 分期货币种类
	uint8_t   szInstalIssurResp[13+1]; // 分期发卡行定义格式，终端可原样打印在备注栏

	uint8_t   szPosId[9];	
	uint8_t   szCardNo[21];
	uint8_t   szDate[9];
	uint8_t   szTime[7];
	uint8_t   szSysReferNo[13]; 
	uint8_t   szAuthNo[7];     
	uint8_t   szExpDate[5];
	uint8_t   szOldTxnDate[5]; 
	uint8_t   szEntryMode[4];
	uint8_t	szTellerNo[4];
	uint8_t	szCardUnit[4];  
	uint8_t	szAuthMode[3]; 
	uint8_t	szAuthUnit[12]; 
	uint8_t	szIssuerResp[21];
	uint8_t	szCenterResp[21];
	uint8_t	szRecvBankResp[21];
	uint8_t	szIssuerBankId[9];
	uint8_t	szRecvBankId[9];
	uint8_t	szCenterId[9];
	uint8_t	szPrtOldPosSer[7]; 
	uint8_t	szPrtOldAuthNo[7]; 
	uint8_t	szPrtOldHostSer[13]; 
	uint8_t sBalanceAmount[4];  // 电子钱包存折余额,交易前
	uint8_t ucKeyIndex;
	uint8_t szPsamTid[9];
	uint8_t szIssureID[8+1];              //发卡行标识
	uint8_t sICRandom[4];
	uint8_t   ucKeyVersion;
	uint8_t   ucKeyAlgFlag;            // 算法标识
	uint8_t   sIcTxnSeq[2];            // 用户卡交易序列号
	uint8_t   ucIcTransType;
	uint8_t   sPsamTid[6];
	uint8_t   sMac1[4];
	uint8_t   sMac2[4];
//	uint8_t   sHostDateTime[7];
	uint8_t   sTac[4];
	uint8_t   sPsamTxnSeq[4];           // psam卡交易序列号
	uint8_t   sAfterBalanceAmt[4+1];    // 交易后余额, 4byte
	uint8_t   szTranferCardNo[21];      // 转帐卡号
	uint8_t  ucEcTopOldSwipeFlag;       // 非指定账户圈存转出卡的输入方式

	// EMV交易数据
	uint8_t	sIccData[255];
	uint8_t	iIccDataLen;
	uint8_t	bPanSeqNoOk;		// ADVT case 43 
	uint8_t	ucPanSeqNo;			// App. PAN sequence number
	uint8_t	sAppCrypto[8];		// app. cryptogram
	uint8_t	sAuthRspCode[2];
	uint8_t	sTVR[5];
	uint8_t	szAID[32+1];
	uint8_t	szAppLable[16+1];
	uint8_t	sTSI[2];
	uint8_t	sATC[2];
	uint8_t	ucFallBack;		
	uint8_t	ucSwipeFlag;
	uint8_t ucNeedSignature;   // qpboc是否需要签名
	//------天津磁卡CPU----------
	UINT8 sCardSerialNo[16+1];//卡应用序列号 
	ulong ulPOSTraceNo;//2.	充值次数,钱包计数器
} NEWPOS_LOG_STRC;
#define LOG_RECORD_LEN sizeof(struct POS_LOG_STRUCT)

typedef struct NEWPOS_COM_STRUCT {    
	uint8_t  szRespCode[2+1];       
	uint8_t  szUnitNum[15+1];		
	uint8_t  sPIN[8];			
	uint8_t  szTrack2[TRACK2_LEN+1];
	uint8_t  szTrack3[TRACK3_LEN+1];
	NEWPOS_LOG_STRC stTrans;		
	NEWPOS_8583PACKET_STRC stTrnTbl;  
	uint8_t  ucFallBack;	
	uint8_t  ucSwipedFlag;
	uint8_t  ucPinEntry;	
	uint8_t  sReversalDE55[128];	
	int  iReversalDE55Len;
	uint8_t  sScriptDE55[128];
	int  iScriptDE55Len;
	uint8_t  bOnlineTxn;	
	uint8_t  szCertData[2+20+1];
	uint8_t  bIsFirstGAC;
	uint8_t  bSkipEnterAmt;	
	uint8_t  ucEdOrEpFlag;  // 是电子钱包还是电子存折
	STISO8583 stOrgPack;
}POS_COM;
#define COM_SIZE  sizeof(struct NEWPOS_COM_STRUCT)

typedef struct   _REFUND_TRACK_STRUCT {
	int    lTraceNo;   
	uint8_t    szTrack2[TRACK2_LEN+1];
	uint8_t	 szTrack3[TRACK3_LEN+1];
	uint8_t	 szOldDate[5];	
} REFUND_TRACK_STRC;
#define REFUND_LOG_STRC_LEN sizeof(struct _REFUND_TRACK_STRUCT)
typedef struct _total_m1Enrich_strc {	
	int iRmbM1EnrichNum;
	int iRmbM1EnrichAmt;	
	int iRmbM1Enrich_common_Num;//普通卡笔数
	int iRmbM1Enrich_common_Amt;//普通卡金额
	int iRmbM1Enrich_student_Num;//学生卡笔数
	int iRmbM1Enrich_student_Amt;//学生卡金额
	int iRmbM1Enrich_old_Num;//老年卡笔数
	int iRmbM1Enrich_old_Amt;//老年卡金额
	int iRmbM1Enrich_staff_Num;//员工卡笔数
	int iRmbM1Enrich_staff_Amt;//员工卡金额
	int iRmbM1Enrich_tong_Num;//员工通勤卡笔数
	int iRmbM1Enrich_tong_Amt;//员工通勤卡金额
	int iRmbM1Enrich_tel_Num;//电信卡笔数
	int iRmbM1Enrich_tel_Amt;//电信卡金额
	int iRmbM1Enrich_pers_Num;//个性卡笔数
	int iRmbM1Enrich_pers_Amt;//个性卡金额
	int iRmbM1Enrich_null_Num;//未定义卡笔数
	int iRmbM1Enrich_null_Amt;//未定义卡金额
	int iRmbM1SaleNum;
	int iRmbM1SaleAmt;
	
	uint8_t sM1TellNo[4+1];//操作员卡号（4位ASC）
	uint8_t szCompanyNo[4+1];//公司编号?
	uint8_t pointofsaleNo[3+10+1];//销售点号
	uint8_t szNowTellerNo[4];//登录的操作员号
	
	uint8_t szLSDepartmentNo[2*2+1] ;      //部门编号
	uint8_t szLSM1TellNo[4*2+1];//乐山操作员
	uint8_t szLSPosId[12+1];  //终端编号
	
	UINT8 u8Settlemode;//1普通结算，2销售日期补采；3结算日期补采
	UINT8 szDate[8+1];//销售日期或者结算补采日期
	//int lBatchNum; 
	uint8_t sDateTime[15]; // 结算时间
}TOTAL_M1ENRICH_STRC;

#define TOTAL_M1ENRICH_STRC_SIZE  sizeof(struct _total_m1Enrich_strc)

typedef struct _total_strc {	
	int iRmbCreditNum;
	int iRmbDebitNum;	
	int iRmbSaleNum;		
	int iRmbConfirmNum;	
	int iRmbOffConfirmNum;
	int iRmbRefundNum;	
	int iRmbOffNum;	
	int iRmbEcNum;

	int iFrnCreditNum;	
	int iFrnDebitNum;		
	int iFrnSaleNum;	
	int iFrnConfirmNum;
	int iFrnOffConfirmNum;
	int iFrnRefundNum;
	int iFrnOffNum;	
	int iTopUpNum;
	int iFrnEcNum;

	uint8_t sRmbDebitAmt[6];
	uint8_t sRmbCreditAmt[6];
	uint8_t sFrnCreditAmt[6];	
	uint8_t sFrnDebitAmt[6];	
	uint8_t sRmbSaleAmt[6];	
	uint8_t sRmbConfirmAmt[6];	
	uint8_t sRmbOffConfirmAmt[6];	
	uint8_t sRmbRefundAmt[6];	
	uint8_t sRmbOffAmt[6];	
	uint8_t sFrnSaleAmt[6];	
	uint8_t sFrnConfirmAmt[6];	
	uint8_t sFrnOffConfirmAmt[6];	
	uint8_t sFrnRefundAmt[6];	
	uint8_t sFrnOffAmt[6];	
	uint8_t sTopUpAmt[6];
	uint8_t sRmbEcAmt[6];
	uint8_t sFrnEcAmt[6];

	int lBatchNum; 
	uint8_t sDateTime[15]; 
}TOTAL_STRC;

#define TOTAL_STRC_SIZE  sizeof(struct _total_strc)

typedef struct _oper_structure {
	char szTellerNo[4];
	char szPassword[13];	
}OPER_STRC;
#define OPER_STRC_SIZE sizeof(struct _oper_structure)

typedef struct _NEWPOS_PARAM_STRC {
	uint8_t szPhoneNum1[20];
	uint8_t szPhoneNum2[20];
	uint8_t szPhoneNum3[20];
	uint8_t szMPhone[20];
	uint8_t szUnitNum[16];
	uint8_t szPosId[9];
	uint8_t ucSendOffFlag;	
	uint8_t sTpdu[6];
	uint8_t ucTipOpen;		
	uint8_t sTransOpen[4];
	uint8_t ucAutoLogoff;	
	uint8_t ucManualInput;
	uint8_t ucErrorPrt;	
	uint8_t szUnitChnName[41];
	uint8_t szEngName[41];
	uint8_t ucTestFlag; 	
	uint8_t ucVoidSwipe;	
	uint8_t ucAuthVoidSwipe;	
	uint8_t ucConfirm;	
	uint8_t ucVoidPin;	
	uint8_t ucPreVoidPin;
	uint8_t ucPreComVoidPin;
	uint8_t ucPreComPin;  
	uint8_t ucKeyMode;    
	uint8_t ucEmvSupport;
	uint8_t ucqPbocSupport;      // 是否支持Qpboc
	uint8_t ucPartPaySupport;    // 支持部分扣款
	uint8_t ucSelectCardType;	
	uint8_t szParamsPwd[9];   
	uint8_t szSafepwd[9];
	uint8_t szTpdu[11];       
	uint8_t szTransCtl[9]; 
	uint8_t bPreDial;       // 是否预拨号
	uint8_t szDownloadTel[20]; 
	uint8_t ucDetailPrt;   
	uint8_t szMerBankId[5];
	uint8_t szLocalId[5];
	uint8_t ucPrnEng; 
	uint8_t ucMaxOfflineTxn;  // 满足自动上送的累计笔数
	uint8_t ucOfflineSettleFlag;  // 小额代授权的结算是是否否开放
	uint8_t ucPrnTitleFlag; 
	uint8_t szPrnTitle[60];       // 打印票据的抬头
	//----
	uint8_t szLSDepartmentNo[2*2+1] ;      //部门编号
	uint8_t szLSM1TellNo[4*2+1];//乐山操作员
	uint8_t szLSPosId[12+1];
	uint8_t szMacKey[16+1];
	uint8_t szMsgBodyKey[32+1];
	//----
	uint8_t sM1TellNo[4+1];//操作员卡号（4位ASC）
	uint8_t szCompanyNo[4+1];//公司编号?
	uint8_t pointofsaleNo[3+10+1];//销售点号
	char	 szFtpServIP[15+1];    // FTP服务器IP
	char	 szFtpServPort[5+1];   // FTP服务器端口
	char	 szFtpUserName[32];    // FTP登录用户名, 默认anonymous
	char	 szFtpPwd[32];		   // FTP登录密码, 默认123456
	char	 szFtpPath[100];	   // FTP服务器中文件所在的文件夹的路径
	int ulMaxBalaceAmt;  //卡充值最大金额
	int dayMaxBalaceAmt;  //结算日充值最大金额
	ulong iTotalTranAmt;  //当日已充值金额(单位: 分)
	ulong lSaleCumulativeBalance;

	//----
	int  uiRever;     // 为了对齐保留
	int ucTicketNum;	
	int ucOprtLimitTime;  
	int ucCommWaitTime; 
	int ucTipper;		
	int ucAdjustPercent;	
	int ucResendTimes;	      // 冲正重发次数
	int ucDialRetryTimes;	
	int ucKeyIndex;		
	int iMaxTransTotal;  
	int lNowTraceNo; 	
	int lNowBatchNum;
	int ifsendTC;  // 1是，0不是
	int lEnrichTraceNo;  //充值流水号
	int lUnSettTraceNum;
	int lUnSettFileNum;
	
	ulong lEnrichCumulativeBalance;//充值累积金额
	int lMaxRefundAmt;
	int iEcMaxAmount;       // 电子终端电子现金交易限额
	int iqPbocMinAmt;      // 非接触式脱机最低限额
	int iqPbocMaxAmt;      // 非接触式交易限额
	
	int iDispLanguage;   // 显示语言
	int iCommType;       // 1-脱机 2-串口 3-联机无线

	int OldTime; //记录最后一次操作时间

	ST_COMM_CONFIG		stTxnCommCfg;	        // 终端通讯配置 －－
	ST_COMM_CONFIG		stDownParamCommCfg;		// 终端参数下载通讯配置		
	ST_WIFI_AP_INFO     stWIFIAPInfo;           // wifi AP 配置信息 目前只考虑一台机器只连接一个热点
}POS_PARAM_STRC;


#define  MAX_TRANLOG			   1000		      // must <65535
#define  INV_TXN_INDEX			   0xFFFFFFFF	  // 无效的最后后面一个字节8bit标识交易类型
//说明，请勿暂用低4位，低4位用来表明上送失败的次数
#define TS_NEEDTC			    0x4000		// EMV交易需要TC上送
#define TS_NEEDUPLOAD			0x2000		// 没有批次上送
#define TS_TXN_OFFLINE			0x1000		// 磁条卡的离线交易需要上送 
#define TS_ICC_OFFLINE          0x0800      // ICC卡的离线交易需要上送
typedef struct _TRANS_CTRL_EnrichFile{
	uint8_t  uflag;//  1是需要上传的。
	char	 szRemotefile[256]; // 服务器的文件路径
	char	 szLocalfile[256]; // 本地的文件路径，需要注意的是pos的文件名最大长度为16个字节	
  	int iRmbM1EnrichNum;//笔数
  	int iRmbM1SaleNum;
	int iRmbM1EnrichAmt;//金额
	int iRmbM1SaleAmt;
	int iRmbM1Enrich_common_Num;//笔数
	int iRmbM1Enrich_common_Amt;//金额
	int iRmbM1Enrich_student_Num;//笔数
	int iRmbM1Enrich_student_Amt;//金额
	UINT8 u8Settlemode;//1普通结算，2销售日期补采；3结算日期补采
	UINT8 szDate[8+1];//销售日期或者结算补采日期

}TRANS_CTRL_EnrichFile;


typedef struct _TRANS_CTRL_PARAM {
	uint8_t szNowTellerNo[4];	
	uint8_t szNowTellerPwd[13];
	uint8_t szOldTellerNo[4];
	uint8_t ucPosStatus; 
	uint8_t ucBatchUpStatus; 
	uint8_t szExtNum[10];	 
	uint8_t ucLoadTask;  
	uint8_t szLastTransDate[9]; 
	uint8_t ucLogonFlag;	
	uint8_t ucClearLog;	
	uint8_t ucDownAppFlag; 
	uint8_t ucDefaltTxn;   
	uint8_t bEmvCapkLoaded;	
	uint8_t bEmvAppLoaded;	
	uint8_t ucRmbSettSaveRsp;
	uint8_t ucFrnSettSaveRsp;
	uint8_t ucCurSamNo;
	int iTransNum;       
	uint32_t  sTxnRecIndexList[MAX_TRANLOG];		// 交易记录表
	TRANS_CTRL_EnrichFile stEnrichFile[50];
}TRANS_CTRL_PARAM;


typedef struct errinfostru{
	uint8_t ucRetCode[3];	
	uint8_t szMessage[17]; 
	uint8_t szOperate[33]; 
}ERR_INFO_STRC;

typedef struct DIAL_STAT_STRUC {
	int hours;		
	int dials;		
	int fails;		
	int  suc_rate;	
}DIAL_STAT;


typedef struct STAT_TIME_STRUC {
	int	year;
	char	month;
	char	day;
	char	hour;
}STAT_TIME;


typedef struct _limit_total_strc {	
	uint8_t sRmbDebitAmt[6];	
	uint8_t sRmbCreditAmt[6];
	uint8_t sFrnCreditAmt[6];	
	uint8_t sFrnDebitAmt[6];	
}LIMIT_TOTAL_STRC;


typedef struct {
	uint8_t szBankId[9];       //收单行号
	uint8_t szBankName[13];    //收单行中文名
}ACQUIRERIDNAME;

//typedef struct 
//{ 
//	uint8_t AppNo[APP_NUM+1];                 
//	uint8_t MenuItem[APP_NUM+1][APP_NAME];   
//}APPMENU;

typedef struct temp_global_var_strc {
	int iTransNo;			
	int iLogFilePostion;	 	      //日志文件位置
	uint8_t ucPinEnterTime;             //PIN输入次数
	uint8_t ucRmbSettRsp;	              //人民币结算返回平不平标志
	uint8_t ucFrnSettRsp;	              //外卡结算返回平不平标志
	uint8_t szDate[9];
	uint8_t szTime[7];
	uint8_t sPacketHead[7];             //报文头
	uint8_t bSendId; 
	// for m1 -----edited by longlee
	uint8_t       sCardDataList[64][16];      //S50卡所有数据信息 64个block，每个block16字节
	uint8_t	   sCardDataListSub[64][16];
	uint8_t       sKeyAList[16][6];           //S50卡所有扇区的keya验证数据
	uint8_t       sKeyBList[16*6];           //S50卡所有扇区的keyb验证数据
	uint8_t       sCardUid[16];               //S50卡UID
} STRUCT_TEMP_VAR;


// EMV AID参数列表
typedef  struct _tagTermAidList 
{
	uint8_t	ucAidLen;			// AID的长度
	uint8_t	sAID[17];			// 应用标志
	uint8_t	bOnlinePin;		    // CUP要求每个AID需要这个参数
	uint32_t    m_EcMaxTxnAmount;       // 电子现金9F7B
	uint32_t 	m_ReaderMaxTransAmount; // DF20非接触交易的金额大于或等于此数值，则交易终止
	uint32_t 	m_FloorLimiteAmount;    // DF19非接触交易的最低限额
	uint32_t 	m_CVMAmount;            // DF21非接触交易超过此值，读写器要求一个持卡人验证方法
}TermAidList;

// EMV CAPK列表
typedef struct _tagCapkInfo 
{
	uint8_t	bNeedDownLoad;
	uint8_t	sRid[5];
	uint8_t	ucKeyID;
	uint8_t	sExpDate[4];
}CapkInfo;

#define    MAX_BLACKLIST_NUM    40000
#define    ST_BLACKLIST_USED         0x01   // 黑名单可用 
#define    ST_BLACKLIST_NEEDDOWN     0x02   // 需要继续下载
typedef struct _tag_BLACKLIST_TOTAL
{
	uint8_t            ucFlag;          // 是否需要继续下载
	char               szVersion[10+1]; // 版本号－－10byte
	uint32_t           ulBlackListNum;  // 实际卡数量
	char               szBlackList[MAX_BLACKLIST_NUM][8];  // 1len + 7 data卡结构数据
}ST_BLACKLIST_TOTAL;

// 终端能力配置信息，例如打印类型，通讯类型
typedef struct _tag_POS_CONFIG_INFO
{
	uint32_t   uiPosType;            // 终端型号
	uint32_t   uiSecuType;           // 安全模块类型，是否支持PCI
	uint32_t   uiPrnType;            // 终端打印机类型，针打－－热敏
	uint32_t   uiCommType;           // 终端配置的通讯类型
	uint32_t   uiMaxSyncPSTNRate;    // 同步拨号的最高波特率
	uint32_t   uiMaxAsynPSTNRate;    // 异步拨号的最高波特率
	uint32_t   uiUnContactlessCard;  // 非接触式卡
}ST_POS_CONFIG_INFO;

#endif
