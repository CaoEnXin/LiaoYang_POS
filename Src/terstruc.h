
#ifndef _NEWPOS_TERSTRUC_H
#define _NEWPOS_TERSTRUC_H

#include "posapi.h"
#include "posmacro.h"
#include "st8583.h"
#include "Commlib.h"

typedef struct _POS_8583PACKET_STRC {
	int  iTransNo;			    // ���״���
	uint8_t  szSevPointCode[2+1];		// �����������   
	uint8_t  szMessTypeCode[2+1];		// ��Ϣ������     
	uint8_t  szNetManInfoCode[3+1];	// ���������Ϣ�� 
} NEWPOS_8583PACKET_STRC;

#define   TS_NOT_RECV    0x02    // ���͵���û���յ�����
#define   TS_NOT_CODE    0x03    // ���͵��Ƿ������00
#define   TS_INST_SPAC   0x01    // ���ڲ���ӡ�ո�
#define   TS_FEE_SPAC    0x02    // ÿ�ڲ���ӡ�ո�

typedef struct   POS_LOG_STRUCT{
	uint8_t	ucBatchSendFlag;       
	uint8_t   ucVoidId;       
	uint8_t	ucSendFlag;
	uint8_t	ucSendFail;    // 
	uint8_t	ucAdjusted;	
	uint8_t 	ucChgFrmAdjust; 

	uint8_t   ucInstalAmtFlag; // ��ʶ��Ǯ�������Ƿ��ӡ�ո��ǲ���ӡ
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
	uint32_t    uiTxnRecIndex;       // ���׼�¼������

	uint8_t   ucInstallmentNum;      // ��������
	uint8_t   ucInstalPayMode;       // ���ڸ���֧����ʽ
	uint8_t   szInstalCode[30+1];    // ��Ʒ��Ŀ����
	uint8_t   szInstalCurrCode[3+1]; // ���ڻ�������
	uint8_t   szInstalIssurResp[13+1]; // ���ڷ����ж����ʽ���ն˿�ԭ����ӡ�ڱ�ע��

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
	uint8_t sBalanceAmount[4];  // ����Ǯ���������,����ǰ
	uint8_t ucKeyIndex;
	uint8_t szPsamTid[9];
	uint8_t szIssureID[8+1];              //�����б�ʶ
	uint8_t sICRandom[4];
	uint8_t   ucKeyVersion;
	uint8_t   ucKeyAlgFlag;            // �㷨��ʶ
	uint8_t   sIcTxnSeq[2];            // �û����������к�
	uint8_t   ucIcTransType;
	uint8_t   sPsamTid[6];
	uint8_t   sMac1[4];
	uint8_t   sMac2[4];
//	uint8_t   sHostDateTime[7];
	uint8_t   sTac[4];
	uint8_t   sPsamTxnSeq[4];           // psam���������к�
	uint8_t   sAfterBalanceAmt[4+1];    // ���׺����, 4byte
	uint8_t   szTranferCardNo[21];      // ת�ʿ���
	uint8_t  ucEcTopOldSwipeFlag;       // ��ָ���˻�Ȧ��ת���������뷽ʽ

	// EMV��������
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
	uint8_t ucNeedSignature;   // qpboc�Ƿ���Ҫǩ��
	//------���ſ�CPU----------
	UINT8 sCardSerialNo[16+1];//��Ӧ�����к� 
	ulong ulPOSTraceNo;//2.	��ֵ����,Ǯ��������
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
	uint8_t  ucEdOrEpFlag;  // �ǵ���Ǯ�����ǵ��Ӵ���
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
	int iRmbM1Enrich_common_Num;//��ͨ������
	int iRmbM1Enrich_common_Amt;//��ͨ�����
	int iRmbM1Enrich_student_Num;//ѧ��������
	int iRmbM1Enrich_student_Amt;//ѧ�������
	int iRmbM1Enrich_old_Num;//���꿨����
	int iRmbM1Enrich_old_Amt;//���꿨���
	int iRmbM1Enrich_staff_Num;//Ա��������
	int iRmbM1Enrich_staff_Amt;//Ա�������
	int iRmbM1Enrich_tong_Num;//Ա��ͨ�ڿ�����
	int iRmbM1Enrich_tong_Amt;//Ա��ͨ�ڿ����
	int iRmbM1Enrich_tel_Num;//���ſ�����
	int iRmbM1Enrich_tel_Amt;//���ſ����
	int iRmbM1Enrich_pers_Num;//���Կ�����
	int iRmbM1Enrich_pers_Amt;//���Կ����
	int iRmbM1Enrich_null_Num;//δ���忨����
	int iRmbM1Enrich_null_Amt;//δ���忨���
	int iRmbM1SaleNum;
	int iRmbM1SaleAmt;
	
	uint8_t sM1TellNo[4+1];//����Ա���ţ�4λASC��
	uint8_t szCompanyNo[4+1];//��˾���?
	uint8_t pointofsaleNo[3+10+1];//���۵��
	uint8_t szNowTellerNo[4];//��¼�Ĳ���Ա��
	
	uint8_t szLSDepartmentNo[2*2+1] ;      //���ű��
	uint8_t szLSM1TellNo[4*2+1];//��ɽ����Ա
	uint8_t szLSPosId[12+1];  //�ն˱��
	
	UINT8 u8Settlemode;//1��ͨ���㣬2�������ڲ��ɣ�3�������ڲ���
	UINT8 szDate[8+1];//�������ڻ��߽��㲹������
	//int lBatchNum; 
	uint8_t sDateTime[15]; // ����ʱ��
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
	uint8_t ucqPbocSupport;      // �Ƿ�֧��Qpboc
	uint8_t ucPartPaySupport;    // ֧�ֲ��ֿۿ�
	uint8_t ucSelectCardType;	
	uint8_t szParamsPwd[9];   
	uint8_t szSafepwd[9];
	uint8_t szTpdu[11];       
	uint8_t szTransCtl[9]; 
	uint8_t bPreDial;       // �Ƿ�Ԥ����
	uint8_t szDownloadTel[20]; 
	uint8_t ucDetailPrt;   
	uint8_t szMerBankId[5];
	uint8_t szLocalId[5];
	uint8_t ucPrnEng; 
	uint8_t ucMaxOfflineTxn;  // �����Զ����͵��ۼƱ���
	uint8_t ucOfflineSettleFlag;  // С�����Ȩ�Ľ������Ƿ�񿪷�
	uint8_t ucPrnTitleFlag; 
	uint8_t szPrnTitle[60];       // ��ӡƱ�ݵ�̧ͷ
	//----
	uint8_t szLSDepartmentNo[2*2+1] ;      //���ű��
	uint8_t szLSM1TellNo[4*2+1];//��ɽ����Ա
	uint8_t szLSPosId[12+1];
	uint8_t szMacKey[16+1];
	uint8_t szMsgBodyKey[32+1];
	//----
	uint8_t sM1TellNo[4+1];//����Ա���ţ�4λASC��
	uint8_t szCompanyNo[4+1];//��˾���?
	uint8_t pointofsaleNo[3+10+1];//���۵��
	char	 szFtpServIP[15+1];    // FTP������IP
	char	 szFtpServPort[5+1];   // FTP�������˿�
	char	 szFtpUserName[32];    // FTP��¼�û���, Ĭ��anonymous
	char	 szFtpPwd[32];		   // FTP��¼����, Ĭ��123456
	char	 szFtpPath[100];	   // FTP���������ļ����ڵ��ļ��е�·��
	int ulMaxBalaceAmt;  //����ֵ�����
	int dayMaxBalaceAmt;  //�����ճ�ֵ�����
	ulong iTotalTranAmt;  //�����ѳ�ֵ���(��λ: ��)
	ulong lSaleCumulativeBalance;

	//----
	int  uiRever;     // Ϊ�˶��뱣��
	int ucTicketNum;	
	int ucOprtLimitTime;  
	int ucCommWaitTime; 
	int ucTipper;		
	int ucAdjustPercent;	
	int ucResendTimes;	      // �����ط�����
	int ucDialRetryTimes;	
	int ucKeyIndex;		
	int iMaxTransTotal;  
	int lNowTraceNo; 	
	int lNowBatchNum;
	int ifsendTC;  // 1�ǣ�0����
	int lEnrichTraceNo;  //��ֵ��ˮ��
	int lUnSettTraceNum;
	int lUnSettFileNum;
	
	ulong lEnrichCumulativeBalance;//��ֵ�ۻ����
	int lMaxRefundAmt;
	int iEcMaxAmount;       // �����ն˵����ֽ����޶�
	int iqPbocMinAmt;      // �ǽӴ�ʽ�ѻ�����޶�
	int iqPbocMaxAmt;      // �ǽӴ�ʽ�����޶�
	
	int iDispLanguage;   // ��ʾ����
	int iCommType;       // 1-�ѻ� 2-���� 3-��������

	int OldTime; //��¼���һ�β���ʱ��

	ST_COMM_CONFIG		stTxnCommCfg;	        // �ն�ͨѶ���� ����
	ST_COMM_CONFIG		stDownParamCommCfg;		// �ն˲�������ͨѶ����		
	ST_WIFI_AP_INFO     stWIFIAPInfo;           // wifi AP ������Ϣ Ŀǰֻ����һ̨����ֻ����һ���ȵ�
}POS_PARAM_STRC;


#define  MAX_TRANLOG			   1000		      // must <65535
#define  INV_TXN_INDEX			   0xFFFFFFFF	  // ��Ч��������һ���ֽ�8bit��ʶ��������
//˵�����������õ�4λ����4λ������������ʧ�ܵĴ���
#define TS_NEEDTC			    0x4000		// EMV������ҪTC����
#define TS_NEEDUPLOAD			0x2000		// û����������
#define TS_TXN_OFFLINE			0x1000		// �����������߽�����Ҫ���� 
#define TS_ICC_OFFLINE          0x0800      // ICC�������߽�����Ҫ����
typedef struct _TRANS_CTRL_EnrichFile{
	uint8_t  uflag;//  1����Ҫ�ϴ��ġ�
	char	 szRemotefile[256]; // ���������ļ�·��
	char	 szLocalfile[256]; // ���ص��ļ�·������Ҫע�����pos���ļ�����󳤶�Ϊ16���ֽ�	
  	int iRmbM1EnrichNum;//����
  	int iRmbM1SaleNum;
	int iRmbM1EnrichAmt;//���
	int iRmbM1SaleAmt;
	int iRmbM1Enrich_common_Num;//����
	int iRmbM1Enrich_common_Amt;//���
	int iRmbM1Enrich_student_Num;//����
	int iRmbM1Enrich_student_Amt;//���
	UINT8 u8Settlemode;//1��ͨ���㣬2�������ڲ��ɣ�3�������ڲ���
	UINT8 szDate[8+1];//�������ڻ��߽��㲹������

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
	uint32_t  sTxnRecIndexList[MAX_TRANLOG];		// ���׼�¼��
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
	uint8_t szBankId[9];       //�յ��к�
	uint8_t szBankName[13];    //�յ���������
}ACQUIRERIDNAME;

//typedef struct 
//{ 
//	uint8_t AppNo[APP_NUM+1];                 
//	uint8_t MenuItem[APP_NUM+1][APP_NAME];   
//}APPMENU;

typedef struct temp_global_var_strc {
	int iTransNo;			
	int iLogFilePostion;	 	      //��־�ļ�λ��
	uint8_t ucPinEnterTime;             //PIN�������
	uint8_t ucRmbSettRsp;	              //����ҽ��㷵��ƽ��ƽ��־
	uint8_t ucFrnSettRsp;	              //�⿨���㷵��ƽ��ƽ��־
	uint8_t szDate[9];
	uint8_t szTime[7];
	uint8_t sPacketHead[7];             //����ͷ
	uint8_t bSendId; 
	// for m1 -----edited by longlee
	uint8_t       sCardDataList[64][16];      //S50������������Ϣ 64��block��ÿ��block16�ֽ�
	uint8_t	   sCardDataListSub[64][16];
	uint8_t       sKeyAList[16][6];           //S50������������keya��֤����
	uint8_t       sKeyBList[16*6];           //S50������������keyb��֤����
	uint8_t       sCardUid[16];               //S50��UID
} STRUCT_TEMP_VAR;


// EMV AID�����б�
typedef  struct _tagTermAidList 
{
	uint8_t	ucAidLen;			// AID�ĳ���
	uint8_t	sAID[17];			// Ӧ�ñ�־
	uint8_t	bOnlinePin;		    // CUPҪ��ÿ��AID��Ҫ�������
	uint32_t    m_EcMaxTxnAmount;       // �����ֽ�9F7B
	uint32_t 	m_ReaderMaxTransAmount; // DF20�ǽӴ����׵Ľ����ڻ���ڴ���ֵ��������ֹ
	uint32_t 	m_FloorLimiteAmount;    // DF19�ǽӴ����׵�����޶�
	uint32_t 	m_CVMAmount;            // DF21�ǽӴ����׳�����ֵ����д��Ҫ��һ���ֿ�����֤����
}TermAidList;

// EMV CAPK�б�
typedef struct _tagCapkInfo 
{
	uint8_t	bNeedDownLoad;
	uint8_t	sRid[5];
	uint8_t	ucKeyID;
	uint8_t	sExpDate[4];
}CapkInfo;

#define    MAX_BLACKLIST_NUM    40000
#define    ST_BLACKLIST_USED         0x01   // ���������� 
#define    ST_BLACKLIST_NEEDDOWN     0x02   // ��Ҫ��������
typedef struct _tag_BLACKLIST_TOTAL
{
	uint8_t            ucFlag;          // �Ƿ���Ҫ��������
	char               szVersion[10+1]; // �汾�ţ���10byte
	uint32_t           ulBlackListNum;  // ʵ�ʿ�����
	char               szBlackList[MAX_BLACKLIST_NUM][8];  // 1len + 7 data���ṹ����
}ST_BLACKLIST_TOTAL;

// �ն�����������Ϣ�������ӡ���ͣ�ͨѶ����
typedef struct _tag_POS_CONFIG_INFO
{
	uint32_t   uiPosType;            // �ն��ͺ�
	uint32_t   uiSecuType;           // ��ȫģ�����ͣ��Ƿ�֧��PCI
	uint32_t   uiPrnType;            // �ն˴�ӡ�����ͣ���򣭣�����
	uint32_t   uiCommType;           // �ն����õ�ͨѶ����
	uint32_t   uiMaxSyncPSTNRate;    // ͬ�����ŵ���߲�����
	uint32_t   uiMaxAsynPSTNRate;    // �첽���ŵ���߲�����
	uint32_t   uiUnContactlessCard;  // �ǽӴ�ʽ��
}ST_POS_CONFIG_INFO;

#endif
