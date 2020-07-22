/****************************************************************************
	���������°汾�淶��д(NEW POS)
****************************************************************************/
#include <openssl/sha.h>
#include "posapi.h"
#include "posmacro.h"
#include "terstruc.h"
#include "glbvar.h"
#include "tranfunclist.h"
#include "emvtrantest.h"
#include "emvapi.h"
#include "wireless.h"
#include "posparams.h"
#include "EMVCallBack.h"
#include "Menu.h"
#include "otherTxn.h"
#include "qpbocProc.h"
#include <alloc.h>

#define DYNAMIC_MEMORY_SIZE		(1024 * 512)

int  ReadParamsFile(void);
int  EMVCoreInit(void);
void   GetPosCapablity(void);
extern int(*__ICCARD_EXCHANGE_APDU)(int fd, uint32_t sendlen, const void *senddata, uint32_t *recvlen, void *recvdat);

const APPINFO AppInfo = {
	"���ſ�С���",	//Ӧ������Ψһ��
	"V1.1.4",		//Ӧ�ð汾��
	"NEW POS",		//Ӧ���ṩ��
	__DATE__,
//	"2010/07/12",	//Ӧ������ʱ�䣬��װʱȷ����
	"������׼�汾",	//Ӧ��������Ϣ
	0				//Ӧ�÷���ţ�����װʱȷ����
};

int  TxtToEnv(void);

PARAMS myParams[]=
{
	{"�̻���",		STR_PARAMS(stPosParam.szUnitNum),	  STRING_ITEM},	
	{"�ն˺�",		STR_PARAMS(stPosParam.szPosId),	      STRING_ITEM},
	{"�̻�����",    STR_PARAMS(stPosParam.szUnitChnName), STRING_ITEM},
	{"�绰����1",	STR_PARAMS(stPosParam.szPhoneNum1),	  STRING_ITEM},
	{"�绰����2",	STR_PARAMS(stPosParam.szPhoneNum2),	  STRING_ITEM},	
	{"�绰����3",	STR_PARAMS(stPosParam.szPhoneNum3),	  STRING_ITEM},		
	{"POS��ʼ��ˮ��",	LONG_PARAMS(stPosParam.lNowTraceNo),  BIN_ITEM},
	{"POS��ʼ���κ�",	LONG_PARAMS(stPosParam.lNowBatchNum), BIN_ITEM},
	{"������������", 	STR_PARAMS(stPosParam.szParamsPwd),	STRING_ITEM},
	{"��ȫ��������", 	STR_PARAMS(stPosParam.szSafepwd),	STRING_ITEM},
	{"�����ط�����",    INT_PARAMS(stPosParam.ucResendTimes), BIN_ITEM},
//	{"���߽�������ģʽ", CHAR_PARAMS(stPosParam.ucSendOffFlag), ASSCII_ITEM}, // 1���±�����ʱ���� 0������ǰͳһ����
	{"С�Ѱٷֱ�", 	INT_PARAMS(stPosParam.ucTipper), BIN_ITEM},
	{"С���Ƿ�ͨ", 	CHAR_PARAMS(stPosParam.ucTipOpen), ASSCII_ITEM},
	{"����Կ��", 	INT_PARAMS(stPosParam.ucKeyIndex), BIN_ITEM},
	{"TPDU",		STR_PARAMS(stPosParam.szTpdu),	STRING_ITEM},
	{"���׿��Ʋ���",STR_PARAMS(stPosParam.szTransCtl),	STRING_ITEM},
	{"ͨѶ��ȴ�ʱ��", 	INT_PARAMS(stPosParam.ucCommWaitTime), BIN_ITEM},
	{"�������",   	STR_PARAMS(stPosParam.szMPhone),	STRING_ITEM},
	{"�Զ���ǩ��", 	CHAR_PARAMS(stPosParam.ucAutoLogoff), ASSCII_ITEM},
	{"�Ƿ�ͨ���俨��", CHAR_PARAMS(stPosParam.ucManualInput), ASSCII_ITEM},
	{"�Ƿ��ӡ���ϵ���", CHAR_PARAMS(stPosParam.ucErrorPrt), ASSCII_ITEM},
	{"�Ƿ��ӡ��ϸ", CHAR_PARAMS(stPosParam.ucDetailPrt), ASSCII_ITEM},
	{"���д���",    STR_PARAMS(stPosParam.szMerBankId), STRING_ITEM},
	{"���ص�����",    STR_PARAMS(stPosParam.szLocalId), STRING_ITEM},
	{"�Ƿ��ӡӢ��", CHAR_PARAMS(stPosParam.ucPrnEng), ASSCII_ITEM},
	{"����������", 	INT_PARAMS(stPosParam.iMaxTransTotal), BIN_ITEM},
	{"��ӡƾ��������", INT_PARAMS(stPosParam.ucTicketNum), BIN_ITEM},
	{"���ѳ���ˢ����־", CHAR_PARAMS(stPosParam.ucVoidSwipe), ASSCII_ITEM},
	{"Ԥ��Ȩ����ˢ����־", CHAR_PARAMS(stPosParam.ucAuthVoidSwipe), ASSCII_ITEM},
	{"����˻���",	LONG_PARAMS(stPosParam.lMaxRefundAmt), BIN_ITEM},
	{"�����ֽ�����޶�",	LONG_PARAMS(stPosParam.iEcMaxAmount), BIN_ITEM},
	{"�ǽ��ѻ�����޶�",	LONG_PARAMS(stPosParam.iqPbocMinAmt), BIN_ITEM},
//	{"�ǽ���߽����޶�",	LONG_PARAMS(stPosParam.iqPbocMaxAmt), BIN_ITEM},
	{"��󲦺Ŵ���", INT_PARAMS(stPosParam.ucDialRetryTimes), BIN_ITEM},
	{"���ѳ��������Ƿ���������", CHAR_PARAMS(stPosParam.ucVoidPin), ASSCII_ITEM},
	{"Ԥ��Ȩ���������Ƿ���������", CHAR_PARAMS(stPosParam.ucPreVoidPin), ASSCII_ITEM},
	{"Ԥ��Ȩ��ɳ����Ƿ���������", CHAR_PARAMS(stPosParam.ucPreComVoidPin), ASSCII_ITEM},
	{"Ԥ��Ȩ��������Ƿ���������", CHAR_PARAMS(stPosParam.ucPreComPin), ASSCII_ITEM},
	{"������ʱʱ��", INT_PARAMS(stPosParam.ucOprtLimitTime), BIN_ITEM},
	{"Ԥ��Ȩ��ɷ�ʽ", CHAR_PARAMS(stPosParam.ucConfirm), ASSCII_ITEM},
	{"DES�㷨", 	CHAR_PARAMS(stPosParam.ucKeyMode), ASSCII_ITEM},
	{"�����ٷֱ�", 	INT_PARAMS(stPosParam.ucAdjustPercent), BIN_ITEM},
	{"EMV����", 	CHAR_PARAMS(stPosParam.ucEmvSupport), ASSCII_ITEM},
	{"QPBOC����", 	CHAR_PARAMS(stPosParam.ucqPbocSupport), ASSCII_ITEM},
	{"֧�ֲ��ֿۿ��",   CHAR_PARAMS(stPosParam.ucPartPaySupport), ASSCII_ITEM},
	{"����Ա�ж�IC��", CHAR_PARAMS(stPosParam.ucSelectCardType), ASSCII_ITEM},
	{"������Զ��IP",	STR_PARAMS(stPosParam.stTxnCommCfg.stWirlessPara.szRemoteIP),	STRING_ITEM},
	{"�������˿�", 	STR_PARAMS(stPosParam.stTxnCommCfg.stWirlessPara.szRemotePort),	STRING_ITEM},
	{"�û���",      STR_PARAMS(stPosParam.stTxnCommCfg.stWirlessPara.szUID), STRING_ITEM},
	{"��������",    STR_PARAMS(stPosParam.stTxnCommCfg.stWirlessPara.szPwd), STRING_ITEM},
	{"���ź���",    STR_PARAMS(stPosParam.stTxnCommCfg.stWirlessPara.szAPN), STRING_ITEM},
	{"�Ƿ����״̬", 	CHAR_PARAMS(stPosParam.ucTestFlag), ASSCII_ITEM},
	{"ͨѶ��ʽ", 	    CHAR_PARAMS(stPosParam.stTxnCommCfg.ucCommType), ASSCII_ITEM},
	{"Զ�����ص绰����",	STR_PARAMS(stPosParam.szDownloadTel),	  STRING_ITEM},
	{"�Զ����ͱ�������", INT_PARAMS(stPosParam.ucMaxOfflineTxn), BIN_ITEM},
	{"Ĭ�ϴ�ӡǩ��̧ͷ", CHAR_PARAMS(stPosParam.ucPrnTitleFlag), ASSCII_ITEM},
	{"�Զ���ǩ��̧ͷ",	 STR_PARAMS(stPosParam.szPrnTitle),	  STRING_ITEM},
//--------------------------------------------------------------
	{"��ֵ����޶�",	LONG_PARAMS(stPosParam.ulMaxBalaceAmt), BIN_ITEM},

	{"FTP�������˿ں�", 	STR_PARAMS(stPosParam.szFtpServPort), 	STRING_ITEM},
	{"FTP������IP��ַ", 	STR_PARAMS(	 stPosParam.szFtpServIP),	STRING_ITEM},
	{"FTP��¼��",		  STR_PARAMS( stPosParam.szFtpUserName),	STRING_ITEM},
	{"FTP��¼����", 		STR_PARAMS( stPosParam.szFtpPwd),		STRING_ITEM},
	{"FTP�ļ�·��", 		STR_PARAMS( stPosParam.szFtpPath), 	   STRING_ITEM},
	
	{"�ն˱��",		STR_PARAMS(stPosParam.sM1TellNo), 	  STRING_ITEM},
	{"���ű��",		STR_PARAMS(stPosParam.szCompanyNo),	  STRING_ITEM},
	{"��ֵ���",		STR_PARAMS(stPosParam.pointofsaleNo),	  STRING_ITEM},
	{"*****************", NULL, 0,							0}
};

//���۵��
uint8_t logo[] = 
{
    0x42, 0x4D, 0x3E, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x28, 0x00, 
    0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x80, 0x00, 
    0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x7F, 0xDF, 0xFF, 0x7F, 0xFF, 0x7F, 0x00, 0x00, 0x7F, 0xDF, 
    0xFF, 0x7F, 0xFF, 0xBF, 0x00, 0x00, 0x7F, 0xDF, 0xFF, 0x7F, 0xFF, 0xDF, 0x00, 0x00, 0x7F, 0xEF, 
    0xFF, 0x7F, 0xFF, 0xDF, 0x00, 0x00, 0x7F, 0xEF, 0xFF, 0xBF, 0x9D, 0xDF, 0x00, 0x00, 0x7F, 0xEF, 
    0xF1, 0x0C, 0x2B, 0xCF, 0x00, 0x00, 0xBF, 0xEF, 0xF3, 0x5C, 0xB3, 0xEF, 0x00, 0x00, 0xBF, 0xF7, 
    0xF0, 0x56, 0x20, 0xEF, 0x00, 0x00, 0xBF, 0xF7, 0xFA, 0x06, 0x9B, 0xEF, 0x00, 0x00, 0xBF, 0xF7, 
    0xF0, 0xB6, 0x10, 0xEF, 0x00, 0x00, 0xDF, 0xF7, 0xF7, 0x87, 0x5F, 0xF7, 0x00, 0x00, 0xDF, 0xFB, 
    0xF8, 0xBB, 0x1A, 0xF7, 0x00, 0x00, 0xDF, 0xFB, 0xFD, 0x83, 0x06, 0xF7, 0x00, 0x00, 0xDF, 0xFB, 
    0xFF, 0xDF, 0xFF, 0x77, 0x00, 0x00, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0x77, 0x00, 0x00, 0xEC, 0x36, 
    0xB1, 0x6B, 0xE3, 0x37, 0x00, 0x00, 0xEC, 0x92, 0xB5, 0x29, 0xCB, 0x3B, 0x00, 0x00, 0xED, 0xD0, 
    0x95, 0x29, 0xE9, 0x5B, 0x00, 0x00, 0xEC, 0xC8, 0xF1, 0x8C, 0x21, 0x4B, 0x00, 0x00, 0xF4, 0xCE, 
    0xDF, 0xF5, 0x9F, 0xFB, 0x00, 0x00, 0xF4, 0xEE, 0xFF, 0xF4, 0xDF, 0xFB, 0x00, 0x00, 0xF6, 0xEE, 
    0xFF, 0xF4, 0x1F, 0xFD, 0x00, 0x00, 0xF7, 0xFE, 0xFF, 0xF7, 0xFF, 0xFD, 0x00, 0x00, 0xFB, 0xFE, 
    0xFF, 0xFB, 0xFF, 0xFD, 0x00, 0x00, 0xFB, 0xFF, 0x7F, 0xFB, 0xFF, 0xFD, 0x00, 0x00, 0xFB, 0xFF, 
    0x7F, 0xFB, 0xFF, 0xFD, 0x00, 0x00, 0xFB, 0xFF, 0x7F, 0xFB, 0xFF, 0xFE, 0x00, 0x00, 0xFD, 0xFF, 
    0x7F, 0xFD, 0xFF, 0xFE, 0x00, 0x00, 0xFD, 0xFF, 0xBF, 0xFD, 0xFF, 0xFE, 0x00, 0x00, 0xFC, 0x7F, 
    0xBF, 0xFE, 0xFF, 0xFD, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 
};


void CheckPedStatus(void)
{
	tPedConfig  stPedInfo;
	int  iRet;

	memset((char*)&stPedInfo,0,sizeof(stPedInfo));

	iRet = PedGetConfig(&stPedInfo);
	if (iRet!= 0)
	{
		return;
	}

	if (stPedInfo.Status == PED_RET_LOCKED)
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  ��Կ���̽���  ", " UNLOCK PINPAD  ");
		DispMulLanguageString(0, 2, DISP_CFONT, "�Ƿ����������Կ", " CLEAR ALL KEY? ");
		iRet = kbGetKey();
		if( iRet==KEY_ENTER )
		{
			lcdClrLine(2,7);
			PedRebuild();
		}
	}
}

int main(int argc, char *argv[])
{
	uint8_t	ucRet;
	struct terminal_aid_info sTempAid;
	struct issuer_ca_public_key sTempCapk;
	int    iCnt=0;
	int emv_ret;
	INT8     sTimeStr[15];
	char szSN[20];
	int n;

	// ����һ���ڴ�
	static uint8_t theMemoryPool[DYNAMIC_MEMORY_SIZE] __attribute__((aligned(4)));	
	_HeapInit((ulong)theMemoryPool, sizeof(theMemoryPool));
	
	__ICCARD_EXCHANGE_APDU = New8110_icc_exchange_apdu;
	CheckPedStatus();
	//  ��ȡ�ն���������
	GetPosCapablity();

	//ϵͳ��ʼ���ͱ�����ʼ��
	InitPos();

	memset(szSN,0,sizeof(szSN));
	sysReadSN(szSN);
	memcpy(stPosParam.sM1TellNo,szSN,8);
	SaveAppParam();
//NacTxd("111111",6);
//NacTxd(stPosParam.sM1TellNo,8);
//NacTxd(stPosParam.szCompanyNo,4);

	if( fileExist("emv.aid")>=0 )
	{
		for(iCnt=0; iCnt<(fileSize("emv.aid")/sizeof(struct terminal_aid_info)); iCnt++)
		{	
			emvLoadAIDList(iCnt, &sTempAid);
			emv_ret = emv_add_one_aid_info(&sTempAid);
		}
	}
	if( fileExist("emv.capk")>=0 )
	{
		for(iCnt=0; iCnt<(fileSize("emv.capk")/sizeof(struct issuer_ca_public_key)); iCnt++)
		{	
			emvLoadCAPKList(iCnt, &sTempCapk);
			emv_add_one_ca_public_key(&sTempCapk);
		}
	}	

/*	if (stPosParam.ucqPbocSupport == PARAM_OPEN)
	{	// ��Ҫ�������Ҫʱ�Ű��豸��
		imif_fd = contactness_open();
		contactness_poll(imif_fd);
	}*/
	stTransCtrl.ucLogonFlag=LOGGEDON;
	/*while(1)
	{
	    
		ResetPsam();
		lcdCls();
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ǩ    ��    ", "     LOGON      ");
		//--------����ISAM����֤------------
		ucRet =EnrichAuthenticate();
		if( ucRet==OK )
		{
		    lcdCls();
  		    DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ǩ���ɹ�    ", "     LOGON      ");
  		    DispRemoveICC();
  			break; 
		}

	}*/
	

	//stPosParam.maxAgentAmt=500000;

	// �������22��֮�󣬻�������6��֮ǰ����������
	sysGetTime(&sTimeStr[1]);
	//if(sTimeStr[4]>34 || sTimeStr[4]<6)
	//{
	//	sysPowerOff();
	//}
	n = 0;
	while(1)
	{
		// �����4��û�ػ������¶�ȣ����غ�����
		//sysGetTime(&sTimeStr[1]);
		//if(sTimeStr[4]==4 && sTimeStr[5]<30)
		//{
		//	lcdCls();			
		//	ucRet = DownloadBlack(); //���º�����
		//	if( ucRet!=OK )
		//	{
		//		strcpy((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY);
		//	}
		//	sysPowerOff();
		//}
	
		//��ʼ���й�ȫ�ֱ���
		InitAllTransData(); 

		 //�ն����������������ȼ��
		if( TermCheck() )
		{
			continue;
		}
		
		if( strcmp((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY)==0 || stTransCtrl.ucLogonFlag==LOGGEDOFF )
		{
			ucRet = PosOnlineLogon(0);
			CommHangUp(FALSE);

			if(ucRet == OK)
			{
				lcdCls();
				DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ��    ¼    ", "     LOGON      ");
				ucRet = Login();
				if( ucRet==OK )
				{
				    lcdCls();
		  		    DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ��¼�ɹ�    ", "     LOGON      ");
		  			n = 0;
				}
				else
				{
					n ++;
					if(n>3)
					{
						lcdCls();
						DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ��¼ʧ��    ", " 	LOGON	   ");
						sysPowerOff();
					}
					strcpy((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY);
					continue;	
				}
				
				//ucRet = DownloadBlack(); //���º�����
				//if( ucRet!=OK )
				//{
				//	strcpy((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY);
				//	continue;	
				//}

				//stPosParam.maxAgentAmt=100000;//��
				//SaveAppParam();
				
			}
			
			continue;
		}

		// ��ʾ�����沢�ȴ��û�����(�磬������ˢ�����忨��)
	//	DispMainScreen();
	//	ucRet = ProcEvent();	// ������سɹ������Ѿ���������ȷ�Ľ��״���
		GetPosTime(stTemp.szDate, stTemp.szTime);	
			strcpy(PosCom.stTrans.szDate, stTemp.szDate);
			strcpy(PosCom.stTrans.szTime, stTemp.szTime);

		ucRet = SelectTrans();	

		if( ucRet==OK )
		{
			ucRet = ProcSelectTran();	// ����ѡ��Ľ���/����
			// for TIP fallback when 1st GAC return 6985 
			if( ucRet==E_NEED_FALLBACK )
			{
				DispMulLanguageString(0, 3, DISP_CFONT|DISP_MEDIACY, "�忨����", "INSERT ERROR");
				DispMulLanguageString(0, 5, DISP_CFONT|DISP_MEDIACY, "��ˢ������", "PLS SWIPE");
				WaitRemoveICC();
				PosCom.ucFallBack    = TRUE;
				PosCom.bSkipEnterAmt = TRUE;
				PosCom.ucSwipedFlag  = NO_SWIPE_INSERT;
				ucRet = PosGetCard(CARD_SWIPED);
				if( ucRet==OK )
				{
					ucRet = ProcSelectTran();
				}
			}
			// for TIP fallback when 1st GAC return 6985 
		}
		
		DispResult(ucRet);	// ��ʾ���׽�� 
		DispRemoveICC();
		CommOnHook(FALSE);
	} // end while(1)   

}

void DispResult(uint8_t rej)
{
	uint8_t szDispBuf[17];
	
	if( rej<NO_DISP )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		ConvErrCode(rej, (char *)szDispBuf);
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, (char *)szDispBuf);
		if( rej!=OK ) 
		{
			sysBeef(3, 1000);
		}
		else
		{
			OkBeep();
		}
		kbGetKeyMs(2000);
	}

	DispRemoveICC();
}

// POS �ṹ��ʼ�� 
void InitAllTransData(void)
{
	memset(&stTemp, 0, sizeof(stTemp));
	stTemp.iTransNo   = NO_TRANS_SELECT;
	stTemp.bSendId    = TRUE;
	stTemp.ucPinEnterTime = 1;
	InitCommData();
}

void InitCommData(void)
{	
	uint8_t	buf[7];
	
	memset(&PosCom, 0, COM_SIZE);	
	memcpy(PosCom.szUnitNum, stPosParam.szUnitNum, 15);
	PosCom.szUnitNum[15] = 0;
	memcpy(PosCom.stTrans.szPosId, stPosParam.szPosId, 8);
	PosCom.stTrans.szPosId[8] = 0;
	
	if( stPosParam.lNowTraceNo<=0 || stPosParam.lNowTraceNo>999999L )
	{
		stPosParam.lNowTraceNo = 1;
		sprintf((char*)buf, "%06d", stPosParam.lNowTraceNo);
	}
	if(stPosParam.lNowBatchNum<=0 || stPosParam.lNowBatchNum>999999L )
	{
		stPosParam.lNowBatchNum = 1;
		sprintf((char*)buf, "%06d", stPosParam.lNowBatchNum);
	}
	SaveAppParam();
	PosCom.stTrans.lTraceNo     = stPosParam.lNowTraceNo;
	PosCom.stTrans.lBatchNumber = stPosParam.lNowBatchNum;
	
	strcpy((char *)PosCom.stTrans.szTellerNo, (char *)stTransCtrl.szNowTellerNo);
	memset(stTemp.sPacketHead, 0, 7);

	PosCom.ucSwipedFlag = NO_SWIPE_INSERT;
	PosCom.ucPinEntry   = FALSE;
	PosCom.bOnlineTxn   = FALSE;
	PosCom.bIsFirstGAC  = TRUE;
	PosCom.stTrans.ucFallBack = FALSE;
	PosCom.bSkipEnterAmt = FALSE;
}


void InitPos(void)
{
	int iRet;
	
	iRet = sysCheckFirstRun();
// 	if (iRet)
// 	{
// 		iRet = PedWriteMasterKey(PARITY_NONE+TDEA_NONE, 1, 0, 16, "\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11");
// 		iRet = PedWriteMasterKey(PARITY_NONE+TDEA_NONE, 1, 1, 16, "\x11\x11\x11\x11\x11\x11\x11\x11\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
// 	}

	GetParamfileName();  // �ڴλ�ȡ�����ļ����ļ�����

	if (fileExist(gszParamFileName)<0)
	{
		if (fileExist("SysParam.log")<0)
		{
			LoadDefaultAppParam();
		}
		else
		{
			LoadAppParam();
		}
	}
	else
	{
		TxtToEnv();
		fileRemove(gszParamFileName);
	}

	if( fileExist("ctrl.log")<0 )  // ��һ������
	{
		AppFirstRun();
	}
	
	EMVCoreInit();
//	LoadQpbocParam();
	
	//ÿ�ο����󣬶��뱣��ı��� 
	ReadCtrlParam();

	if( stTransCtrl.ucClearLog==TRUE )
	{
		stTransCtrl.iTransNum = 0;
		ClearAllTxnStatus();
		fileRemove(POS_LOG_FILE); 
		fileRemove(REFUND_TRACK_FILE);
		fileRemove(ICC_FAILUR_TXN_FILE);
		stTransCtrl.ucClearLog = FALSE;
		SaveCtrlParam();
	}

	ReadOperFile();
	ReadLimitTotal();
	ReadTermAidList();	
	
//	kbSound(3, 80); 
	kbFlush();
	
	PosDeviceDetect(); //���Ӳ��״̬����ӡ��
	CommInitModule(&stPosParam.stTxnCommCfg);
	
	// �ػ��ؿ��������Ȳ���Աǩ���������Ҫ������ǩ��
	strcpy((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY);

	SaveCtrlParam();
}


int EMVCoreInit(void)
{
	int iRet;

	emv_set_kernel_type(KERNEL_TYPE_PBOC);
		
	emv_init_data_element();

// �����ֽ������
//	emv_pboc_ec_enable(1);
//	
//	emv_pboc_set_ec_ttl(1000);  // ���õ����ֽ�Ĺ���,10Ԫ���ѻ�


	// ����EMV�����
	emv_get_mck_configure(&tConfig);
//20080715 Pboc����ΪE0 E1 C8 ����
	memcpy(tConfig.terminal_capabilities, "\xE0\xF1\xC8", 3);
	memcpy(tConfig.additional_terminal_capabilities, "\xE0\x00\xF0\xA0\x01", 5);
	tConfig.terminal_type = 0x22;

	tConfig.support_forced_online_capability = 0;
	tConfig.support_get_pin_try_counter = 1;
	tConfig.support_pse_selection_method = 1;
	tConfig.support_cardholder_confirmation = 1;
	tConfig.support_floor_limit_checking = 1;
	tConfig.support_advices = 1;
	tConfig.support_batch_data_capture = 1;
	tConfig.support_bypass_pin_entry = 1;
	tConfig.support_default_ddol = 1;
	tConfig.support_forced_acceptance_capability = 0;
	tConfig.support_issuer_initiated_voice_referrals = 1;
	tConfig.support_multi_language = 1;
	tConfig.support_online_data_capture = 1;
	tConfig.support_random_transaction_selection = 1;
	tConfig.support_default_tdol = 1;
	tConfig.support_terminal_action_codes = 1;
	tConfig.support_transaction_log = 1;
	tConfig.support_trm_based_on_aip = 1;
	tConfig.support_velocity_checking = 1;
	
 	tEMVCoreInit.transaction_type = EMV_GOODS;
	
	memcpy(tEMVCoreInit.terminal_id, "19790414", 8);                 // �ն�ID��
    memcpy(tEMVCoreInit.merchant_id,"123456789ABCDEF", 15);     // �̻�ID��
    memcpy(tEMVCoreInit.merchant_cate_code, "\x00\x01", 2);         // �̻�������
    tEMVCoreInit.merchant_name_loc_len = 28;         // �̻����Ƽ�λ����Ϣ����
    memcpy(tEMVCoreInit.merchant_name_loc, "NEW POS TECH SHEN ZHEN CHINA", 28);    
    
	memcpy(tEMVCoreInit.terminal_country_code,   "\x01\x56", 2);    //�ն˹��Ҵ���
	memcpy(tEMVCoreInit.transacion_currency_code, "\x01\x56", 2);    //���׻��Ҵ���
	memcpy(tEMVCoreInit.refer_currency_code, "\x01\x56", 2);    //�ο����Ҵ���
    tEMVCoreInit.transacion_currency_exponent = 0x02;        // ���׻���ָ��
    tEMVCoreInit.refer_currency_exponent = 0x02;        // �ο�����ָ��
    tEMVCoreInit.refer_currency_coefficient = 1000;       // �ο����Ҵ���ͽ��׻��Ҵ����ת��ϵ��(���׻��ҶԲο����ҵĻ���*1000)

//	Hash(&tConfig.terminal_type, sizeof(tConfig)-20, tConfig.checksum);
	SHA1((uint8_t*)&tConfig, sizeof(struct terminal_mck_configure)-20, tConfig.checksum);
	emv_set_mck_configure(&tConfig);
	
	emv_set_core_init_parameter(&tEMVCoreInit);

    tEMVCallBackFunc.emv_candidate_apps_selection = emv_candidate_apps_selection;
    tEMVCallBackFunc.emv_advice_process = emv_advice_process;
    tEMVCallBackFunc.emv_check_certificate = emv_check_certificate;
	
	tEMVCallBackFunc.emv_get_pin = emv_get_pin;	
	tEMVCallBackFunc.emv_return_pin_verify_result = emv_return_pin_verify_result;

    tEMVCallBackFunc.emv_check_online_pin = emv_check_online_pin;

	tEMVCallBackFunc.emv_get_amount = emv_get_amount;

    tEMVCallBackFunc.emv_online_transaction_process = emv_online_transaction_process;
    tEMVCallBackFunc.emv_issuer_referral_process = emv_issuer_referral_process;
    tEMVCallBackFunc.emv_multi_language_selection = emv_multi_language_selection;

	tEMVCallBackFunc.emv_check_revocation_certificate = emvCheckCRL;
	tEMVCallBackFunc.emv_check_exception_file = emvCheckExceptPAN;
	tEMVCallBackFunc.emv_get_transaction_log_amount = emvGetTransLogAmount;

	iRet = emv_init_callback_func(&tEMVCallBackFunc);
	if( iRet!=EMV_RET_SUCCESS )
	{
		return iRet;
	}

	emv_delete_all_terminal_aids();		
	emv_delete_all_ca_public_keys();

	return EMV_RET_SUCCESS;
}

void AppFirstRun(void)
{
	int		i;
	ST_BLACKLIST_TOTAL stBlkCard;
	
	// ��ʼ��POS״̬
	memset(&stTransCtrl, 0, sizeof(struct _TRANS_CTRL_PARAM));
	
	stTransCtrl.ucPosStatus = WORK_STATUS;
	strcpy((char *)stTransCtrl.szLastTransDate, "99999999");
	stTransCtrl.ucClearLog = FALSE;
	stTransCtrl.bEmvCapkLoaded = FALSE;
	stTransCtrl.bEmvAppLoaded  = FALSE;
	stTransCtrl.ucDefaltTxn = 1;
	stTransCtrl.szExtNum[0] = 0;
	ClearAllTxnStatus(); //���ý���״̬Ϊ��Ч
	SaveCtrlParam();  
	stPosParam.lEnrichTraceNo=1;
	stPosParam.lEnrichCumulativeBalance=0;
	SaveAppParam();
	// ��ʼ������Ա״̬ 
	memset(&operStrc, 0, MAX_OPER_NUM*sizeof(struct _oper_structure));
	sprintf(operStrc[0].szTellerNo, SUP_TELLER_NO);
	sprintf(operStrc[0].szPassword, SUP_TELLER_PWD);
	
	for(i=1; i<5; i++)
	{
		sprintf(operStrc[i].szTellerNo, "%03d", i);
		sprintf(operStrc[i].szPassword, "0000");
	}
	
	for(i=5; i<MAX_OPER_NUM; i++)
	{
		sprintf(operStrc[i].szTellerNo, TELLER_NOBODY);
		sprintf(operStrc[i].szPassword, "0000");
	}
	WriteOperFile();
	
	dial_stat.dials = 0;
	dial_stat.fails = 0;
	dial_stat.hours = 0;
	dial_stat.suc_rate = 0;
	if( write_data(&dial_stat, sizeof(DIAL_STAT), "dial.sta")!=OK )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 3, DISP_CFONT|DISP_MEDIACY, "�ļ���������!", "FILE ERROR");
		kbGetKey();
		return ;

	}
	if( write_stat_time()!=OK )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 3, DISP_CFONT|DISP_MEDIACY, "�ļ���������!", "FILE ERROR");
		kbGetKey();
		return ;
	}
	
	// ��ʼ���ܶ������ļ� 
	memset(&stLimitTotal, 0, sizeof(struct _limit_total_strc));
	SaveLimitTotal();
	memset(&stTotal, 0, TOTAL_STRC_SIZE);	
	strcpy((char *)stTotal.sDateTime, "00000000000000");
	SaveLastTotal();
	memset(&stM1EnrichTotal, 0, TOTAL_M1ENRICH_STRC_SIZE);
	strcpy((char *)stM1EnrichTotal.sDateTime, "00000000000000");

	SaveM1EnrichLastTotal();

	// ��ʼ��������
	memset((char*)&stBlkCard,0,sizeof(stBlkCard));
	SaveBlkCard(&stBlkCard);

#ifdef _EMV_TEST_VERSION
	InitTestApps();
	InitTestKeys();
//	LoadDefaultQpbocParam();
#endif
}


// �����ֽ����Ƿ�֧��  
uint8_t CheckSupport(int iTransNo)
{
	switch( iTransNo )
	{
	case POS_QUE:
		if( stPosParam.sTransOpen[0] & 0x80 )
		{
			return TRUE;
		}
		break;

	case POS_PREAUTH:
		if( stPosParam.sTransOpen[0] & 0x40 )
		{
			return TRUE;
		}
		break;

	case POS_PREAUTH_VOID:
		if( stPosParam.sTransOpen[0] & 0x20 )
		{
			return TRUE;
		}
		break;

	case POS_AUTH_CM:
		if( stPosParam.sTransOpen[0] & 0x10 )
		{
			return TRUE;
		}
		break;

	case POS_AUTH_VOID:
		if( stPosParam.sTransOpen[0] & 0x08 )
		{
			return TRUE;
		}
		break;

	case POS_SALE:
		if( stPosParam.sTransOpen[0] & 0x04 )
		{
			return TRUE;
		}
		break;

	case POS_SALE_VOID:
		if( stPosParam.sTransOpen[0] & 0x02 )
		{
			return TRUE;
		}
		break;

	case POS_REFUND:
		if( stPosParam.sTransOpen[0] & 0x01 )
		{
			return TRUE;
		}
		break;

	case OFF_SALE:
		if( stPosParam.sTransOpen[1] & 0x80 )
		{
			return TRUE;
		}
		break;

	case OFF_ADJUST:
		if( stPosParam.sTransOpen[1] & 0x40 )
		{
			return TRUE;
		}
		break;

	case POS_OFF_CONFIRM:  //11
		if( stPosParam.sTransOpen[1] & 0x20 )
		{
			return TRUE;
		}
		break;

	case UPLOAD_SCRIPT_RESULT:  //12
		if( stPosParam.sTransOpen[1] & 0x10 )
		{
			return TRUE;
		}
		break;

	case EC_QUICK_SALE:  //13
	case EC_NORMAL_SALE:
		if( stPosParam.sTransOpen[1] & 0x08 )
		{
			return TRUE;
		}
		break;

	case POS_PREAUTH_ADD: //14
		if( stPosParam.sTransOpen[1] & 0x04 )
		{
			return TRUE;
		}
		break;

	case ED_TOPUP_CASH: //15
	case ED_TOPUP_SPEC: //15
	case ED_TOPUP_NORAML: //15
		if( stPosParam.sTransOpen[1] & 0x02 )
		{
			return TRUE;
		}
		break;

	case POS_INSTALLMENT: //16
		if( stPosParam.sTransOpen[1] & 0x01 )
		{
			return TRUE;
		}
		break;

	case POS_VOID_INSTAL: //17
		if( stPosParam.sTransOpen[2] & 0x80 )
		{
			return TRUE;
		}
		break;

	case POS_LOYATL_ALL: //18
	case POS_LOYATL_ISSUER:
		if( stPosParam.sTransOpen[2] & 0x40 )
		{
			return TRUE;
		}
		break;
		
	case POS_VOID_LOYATL_ALL: //19
	case POS_VOID_LOYATL_ISSUER: //19
		if( stPosParam.sTransOpen[2] & 0x20 )
		{
			return TRUE;
		}
		break;

	case EC_TOPUP_CASH: //20
	case EC_TOPUP_SPEC: //20
	case EC_TOPUP_NORAML: //20
	case EC_VOID_TOPUP:
		if( stPosParam.sTransOpen[2] & 0x10 )
		{
			return TRUE;
		}
		break;

	case POS_RESER_SALE: //21
		if( stPosParam.sTransOpen[2] & 0x08 )
		{
			return TRUE;
		}
		break;

	case POS_RESER_VOID: //22
		if( stPosParam.sTransOpen[2] & 0x04 )
		{
			return TRUE;
		}
		break;

	case POS_MOTO_SALE: //23
		if( stPosParam.sTransOpen[2] & 0x02 )
		{
			return TRUE;
		}
		break;

	case POS_MOTO_SALE_VOID: //24
		if( stPosParam.sTransOpen[2] & 0x01 )
		{
			return TRUE;
		}
		break;

	case POS_MAGTOPUP_ACCT: //25
	case POS_MAGTOPUP_CASH:
		if( stPosParam.sTransOpen[3] & 0x80 )
		{
			return TRUE;
		}
		break;

	default:
		return TRUE;
//		break;
	}

	return FALSE;
}


void DispMainScreen(void)
{
	lcdCls();
	if( stTransCtrl.ucLogonFlag==LOGGEDOFF )
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  �� �� �� ��   ", " CHINA UNIONPAY ");
		DispMulLanguageString(0, 2, DISP_CFONT, "δǩ��", "NOT LOGON");
		DispMulLanguageString(0, 4, DISP_CFONT, "�������ǩ��...", "PRESS ANY KEY");
//		lcdDisplay(0, 6, DISP_ASCII, " %.16s", AppInfo.AppVer);
		lcdFlip();	
		kbFlush();
		kbGetKey();
	}
	else
	{ 
		/*lcdDrLogo(0, 0, logo);
		DispMulLanguageString(58, 0, DISP_CFONT, "�й�����", "UNIONPAY");
		lcdDisplay(58, 2, DISP_CFONT, VERSION_STRING);

		if( stPosParam.ucEmvSupport==PARAM_OPEN )
		{
			DispMulLanguageString(0, 4, DISP_CFONT, " ��ˢ��/��IC��  ", "SWIPE/INSERT");
		}
		else
		{
			DispMulLanguageString(0, 4, DISP_CFONT, "   ��ˢ��-->    ", "PLS SWIPE-->");
		}*/
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ���ݹ���    ", "UNIONPAY");
		DispMulLanguageString(0, 3, DISP_CFONT|DISP_MEDIACY, "IC����ֵ�ն�","IONPAY");
		//DispMulLanguageString(0, 3, DISP_CFONT|DISP_MEDIACY, "IC����ֵ�ն�","IONPAY");
	}
	lcdFlip();	
}


void CheckLimitTotal(void) 
{
	if( memcmp(stLimitTotal.sFrnCreditAmt, "\x99\x99\x00\x00\x00\x00", 6)>=0 ||
		memcmp(stLimitTotal.sFrnDebitAmt,  "\x99\x99\x00\x00\x00\x00", 6)>=0 ||
		memcmp(stLimitTotal.sRmbCreditAmt, "\x99\x99\x00\x00\x00\x00", 6)>=0 ||
		memcmp(stLimitTotal.sRmbDebitAmt,  "\x99\x99\x00\x00\x00\x00", 6)>=0 )
	{
		lcdCls();
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  �ܼƽ��̫��  ", "AMOUNT EXCEED");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "   ����������!  ", "SETTLE NOW!");
		ErrorBeep();
		kbGetKey();					
	}
}


//��ʾӦ����Ϣ��Ӧ�ñ��;���ṩ���ý�������
void SetParaFunc(void)
{
	int iRet;

	iRet = CheckParamPass();
	if( iRet!=1 )
	{
		return ;
	}

	SetupPosPara();

	return ;
} 

//�쿴�ն˰汾���ն˺���Ϣ
uint8_t DispTermInfo(void)
{
    uint8_t	szBuff[20];

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "�ն���Ϣ        ", "    VERSION     ");
	sprintf((char *)szBuff, VERSION_STRING);
	lcdDisplay(0, 2, DISP_CFONT, (char *)szBuff);
	lcdDisplay(0, 4, DISP_CFONT, (char *)stPosParam.szUnitNum);
	lcdDisplay(0, 6, DISP_CFONT, (char *)stPosParam.szPosId);
	kbGetKeyMs(5000);	
	return NO_DISP;
}


int TermCheck(void)
{
	uint8_t ucRet;
	int     iRet;

	if( strcmp((char *)stTransCtrl.szNowTellerNo, TELLER_LOCK)==0 )
	{
		TellerRunLock();
		return 1;
	}
	if( stTransCtrl.ucPosStatus==BATCH_STATUS )
	{
		lcdCls();
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " �ϴ��������ж� ", "LAST UPLOAD HALT");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "  ������������  ", "CONTINUE");
		ErrorBeep();
		iRet = kbGetKeyMs(2000);
		if (iRet == KEY_FN)
		{
			SelectPosManFunc();
		}
		
		ucRet = PosBatchSendTran();
		if( ucRet==OK )
		{
			stTransCtrl.iTransNum = 0;
			ClearAllTxnStatus();
			fileRemove(POS_LOG_FILE); 
			fileRemove(REFUND_TRACK_FILE);
			fileRemove(ICC_FAILUR_TXN_FILE);
			stTransCtrl.ucClearLog = FALSE;				 
			SaveCtrlParam();
		}  

		return 1;
	}

	if( stTransCtrl.iTransNum>=stPosParam.iMaxTransTotal )
	{
		lcdCls();
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " ��־�洢���޶� ", "SAVE LOG EXCEED ");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "     �����     ", "SETTLE FIRST");
		ErrorBeep();
		kbGetKeyMs(2000);					

	}
	CheckLimitTotal();
	return 0;
}

//����޸Ĳ���������
int CheckParamPass(void)
{
	uint8_t sysPass[16], passWd[16];
	int     iRet;
	
	memset(sysPass, 0, sizeof(sysPass));
	memcpy(sysPass, stPosParam.szParamsPwd, 8); 
	
	while(1) 
	{
		lcdCls();
		lcdFlip();
		memset(passWd, 0, sizeof(passWd));
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ϵͳ����    ", "   SETUP PARA   ");
		DispMulLanguageString(0, 2, DISP_CFONT, "���������Ա����", "INPUT SYS PWD:");
		lcdGoto(56, 6);
		iRet = kbGetString(KB_EN_CRYPTDISP+KB_EN_BIGFONT+KB_EN_NUM+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 8, 8, 
			(stPosParam.ucOprtLimitTime*1000), (char *)passWd);
		if( iRet==8 )
		{
			if( strcmp((char *)passWd, (char *)sysPass)==0 )
			{
				return 1;
			}
			lcdClrLine(2, 7);
			lcdFlip();
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "    �������    ", "PWD ERROR!");
			ErrorBeep();
			kbGetKeyMs(1000);					

		}
		else
		{
			return 0;
		}
	}
}

uint8_t ProcEvent(void)
{
	int 	ucRet, ucEvent;

	GetPosTime(stTemp.szDate, stTemp.szTime);	
	strcpy((char *)PosCom.stTrans.szDate, (char *)stTemp.szDate);
	strcpy((char *)PosCom.stTrans.szTime, (char *)stTemp.szTime);

	ucEvent = WaitEvent();	//��ʾ��Ļ��8��ʱ�䣬���ȴ���������ˢ��	
	switch( ucEvent ) 
	{
	case OK:
		if( PosCom.ucSwipedFlag!=NO_SWIPE_INSERT ) // ��ˢ�����忨�¼�����
		{
			if( stTransCtrl.ucDefaltTxn==1 )
			{
				stTemp.iTransNo = POS_SALE;
			}
			else
			{
				stTemp.iTransNo = POS_PREAUTH;
			}

			if (PosCom.ucSwipedFlag == CARD_PASSIVE)
			{
				PosCom.stTrans.iTransNo = EC_QUICK_SALE;
				stTemp.iTransNo = EC_QUICK_SALE;
			}
		}
		return OK;

	case KEY_MENU:
	case KEY_ENTER:
		ucRet = SelectTrans();	
		if( ucRet!=OK )
		{
			return ucRet;
		}
		break;

	case KEY_FN:
		//ucRet = SelectPosManFunc();
		SetParaFunc();
		//if( ucRet!=OK )
		//{
			//return ucRet;
		//}
		break;

	default:
		return NO_DISP;
	}
	
	return OK;
}

uint8_t ProcSelectTran(void)
{
	uint8_t	ucRet=0;

	PosCom.stTrans.iTransNo = stTemp.iTransNo;	
	if( CheckSupport(stTemp.iTransNo)==FALSE )
	{
		lcdCls();
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " �ݲ�֧�ָý��� ", " NOT SUPPORT ");
		ErrorBeep();
		kbGetKeyMs(2000);					
		return NO_DISP;
	}
	
	switch( stTemp.iTransNo )
	{
	case POS_SALE:            
		ucRet = M1_Sale();//OnlineSale();//
		break;
	case M1_ENRICH:
		ucRet =M1_Enrich();//��ֵ
		break;
	case M1_RAEDCARD:
		ucRet =M1_ReadCard();//���� ��ѯ
		//ucRet = ftpDemo(PreDial);
		break;
		//----
	case M1_QUE_SETT://��ѯ�������
		ucRet =M1_Que_Sett();
		break;
	case M1_SETT: //��ֵ����㣬���ɽ����¼
		ucRet =M1_Sett();
		break;
	case M1_SETT_PRT://�����ӡ
		ucRet =M1_Sett_Prt();
		break;
	case M1_DATA_UPLOAD:  //�����ϴ�
		//ucRet =M1_Data_UpLoad();
		ucRet =M1_Data_UpLoad_TCP();
		break;
	case M1_SLAEDATE_UPLOAD: //���������ڲ���
		ucRet =M1_SaleDate_Upload();
		break;
	case M1_SETTDATE_UPLOAD:  //���������ڲ���
		ucRet =M1_SettDate_Upload();
		break;
	case M1_QUERY_NOT_UPLOAD:
		ucRet =M1_Que_NotUploadFile();
		break;
	case M1_DATA_UPLOAD_COM:  //�����ϴ�[����]
		ucRet =M1_Data_UpLoad_COM();
		break;
	case POS_DOWNLOADBLK: //���غ�����
		//ucRet =DownloadBlack();
		break;
	case POS_PREAUTH:         
		ucRet = PreAuth();
		break;		
	case POS_AUTH_CM:          
		ucRet = OnlineAuthConfirm();
		break;		
	case POS_OFF_CONFIRM:
		ucRet = OfflineAuthConfirm();
		break;		
	case POS_PREAUTH_VOID:     
		ucRet = VoidPreAuth();
		break;		
	case POS_SALE_VOID:		
	case POS_AUTH_VOID:		
		ucRet = VoidSaleAuth();
		break;		
	case POS_REFUND:           
		ucRet = RefundTran();
		break;		
	case POS_PREAUTH_ADD:
		ucRet = PreAuthAdd();
		break;		
	case POS_QUE:			
		ucRet = QueryBalance();
		break;		
	case POS_DOWNLOAD_PARAM:   
		ucRet = PosDownLoadParam();
		break;		
	case POS_UPLOAD_PARAM:     
		ucRet = PosUpLoadParam();
		break;
	case RESPONSE_TEST:
		ucRet = TestResponse();
		break;		
	case DOWNLOAD_EMV_CAPK:
		ucRet = DownLoadCAPK(FALSE);
		break;		
	case DOWNLOAD_EMV_PARAM:
		ucRet = DownLoadEmvParam(FALSE);
		break;
	case OFF_SALE:            
		ucRet = OfflineSale();
		break;		
	case OFF_ADJUST:           
		ucRet = OfflineAdjust();
		break;		
	case ADJUST_TIP:          
		ucRet = NO_DISP; 
		break;
	case POS_LOGON:
		ucRet = PosOnlineLogon(0);
		CommHangUp(FALSE);
		break;
	case POS_LOGOFF:
		ucRet = PosOnlineLogoff(1);
		CommHangUp(FALSE);
		break;	
	case PRT_ANY:
		ucRet = reprintTrans(1);
		break;
	case PRT_DETAIL:
		ucRet = PrintDetail(TRUE,FALSE);
		break;
	case PRT_LAST:
		ucRet = reprintTrans(0);
		break;
	case PRT_LAST_TOTAL: //��ӡ��������
		ucRet = ReadLastTotal();
		if( ucRet!=OK )
		{
			return NO_DISP;
		}
		ucRet = PrtTransTotal(0, 0);
		break;
	case PRT_TOTAL:
		ucRet = ComputeTransTotal(1);
		if( ucRet!=OK )
		{
			break;
		}
		ucRet = PrtNowTotal();
		break;
	case SETUP_EXTNUM:		
		PabxSetup();
		ucRet = NO_DISP;
		break;
	case QUE_BATCH_MX:
		ucRet = queryDetail(1);		
		break;
	case QUE_BATCH_ANY:
		ucRet = queryTrans(); 
		break;
	case QUE_BATCH_TOTAL:
		ucRet = QueryTransTotal();	
		if( ucRet==OK )
		{
			ucRet = NO_DISP;
		}
		break;
	case M1_PRT_LAST:		
		break;
	case M1_PRT_ANY:		
		break;
	case M1_PRT_DETAIL:		
		break;
	case M1_PRT_TOTAL:		
		break;
	case M1_QUE_BATCH_MX:
		ucRet = M1_queryDetail();		
		break;
	case M1_QUE_BATCH_ANY:
		ucRet = M1_queryTrans(); 
		break;
	case M1_QUE_BATCH_TOTAL:
		ucRet = M1_QueryTransTotal();	
		if( ucRet==OK )
		{
			ucRet = NO_DISP;
		}
		break;
	case OPER_QUE_TERM_INFO:
		ucRet = DispTermInfo();
		break;
	case POS_SETT:  	
		ucRet = PosSettleTran();
		if( ucRet==OK )
		{
			stTransCtrl.iTransNum = 0;
			ClearAllTxnStatus();
			fileRemove(POS_LOG_FILE); 
			fileRemove(REFUND_TRACK_FILE);
			fileRemove(ICC_FAILUR_TXN_FILE);
			stTransCtrl.ucClearLog = FALSE;				 
			SaveCtrlParam();
		}
		if( stTemp.sPacketHead[5]=='6' )		
		{
			stTransCtrl.ucDownAppFlag = 1;
			SaveCtrlParam();
		}
		break;		
	case EC_QUICK_SALE:
		ucRet = TransEcQuickSale();
		break;
	case EC_NORMAL_SALE:
		ucRet = TransEcSale(stTemp.iTransNo);
		break;
	case EC_TOPUP_CASH:
	case EC_TOPUP_NORAML:
	case EC_TOPUP_SPEC:
		ucRet = TransEcTopup(stTemp.iTransNo);
		break;
	case EC_VOID_TOPUP:
		ucRet = TransEcVoidTopup(stTemp.iTransNo);
		break;
	case EC_BALANCE:
		ucRet = TransEcBalance();
		break;
	case EC_REFUND:
		ucRet = TransEcRefund();
		break;
	case EC_QUE_TXN:
		ucRet = TransEcViewRecord();
		break;
	case ED_SALE:
		ucRet = TransEdSale(stTemp.iTransNo);
		break;
	case ED_TOPUP_CASH:
	case ED_TOPUP_NORAML:
	case ED_TOPUP_SPEC:
		ucRet = TransEdTopup(stTemp.iTransNo);
		break;		
	case ED_BALANCE:
		ucRet = TransEdBalance();
		break;		
	case ED_QUE_TXN:
		ucRet = TransEdViewRecord();
		break;
	case POS_INSTALLMENT:
		ucRet = TransInstallment();
		break;		
	case POS_VOID_INSTAL:
		ucRet = TransVoidInstal();
		break;
	case POS_LOYATL_ISSUER:
	case POS_LOYATL_ALL:
		ucRet = TransLoyalt(stTemp.iTransNo);
		break;		
	case POS_VOID_LOYATL_ISSUER:
	case POS_VOID_LOYATL_ALL:
		ucRet = TransVoidLoyalt(stTemp.iTransNo);
		break;
	case POS_QUE_LOYATL_ALL:
		ucRet = TransQueryLoyalt();
		break;		
	case POS_REFUND_LOYATL_ALL:
		ucRet = TransRefundLoyalt();
		break;
	case POS_MOBILE_SALE:
		ucRet = TransMobileSale();
		break;		
	case POS_MOBILE_VOID:
	case POS_MOBILE_VOID_AUTH:
		ucRet = TransVoidMobileSale();
		break;
	case POS_MOBILE_VOID_PRE:
		ucRet = TransMobileVoidPreAuth();
		break;
	case POS_MOBILE_REFUND:
		ucRet = TransMobileRefund();
		break;		
	case POS_MOBILE_AUTH:
	case POS_MOBILE_AUTH_OFF:
		ucRet = TransMobileAuth();
		break;
	case POS_MOBILE_PREAUTH:
		ucRet = TransMobilePreAuth();
		break;		
	case POS_MOBILE_QUE:
		ucRet = TransMobileBalance();
		break;
	case POS_RESER_SALE:
		ucRet = TransBookSale();
		break;		
	case POS_RESER_VOID:
		ucRet = TransVoidBookSale();
		break;
	case POS_MOTO_SALE:
		ucRet = TransMotoSale();
		break;		
	case POS_MOTO_SALE_VOID:
	case POS_MOTO_AUTH_VOID:
		ucRet = TransVoidMotoSale();
		break;
	case POS_MOTO_PREAUTH_VOID:
		ucRet = TransMotoVoidPreAuth();
		break;		
	case POS_MOTO_REFUND:
		ucRet = TransMotoRefund();
		break;		
	case POS_MOTO_AUTH_CM:
	case POS_MOTO_AUTH_OFF:
		ucRet = TransMotoAuth();
		break;		
	case POS_MOTO_PREAUTH:
		ucRet = TransMotoPreAuth();
		break;		
	case POS_MAGTOPUP_CASH:
		ucRet = TransMagTopupCash();
		break;		
	case POS_MAGTOPUP_ACCT:
		ucRet = TransMagTopupAcct();
		break;
	case POS_LYT_LOGON:
		ucRet = TransLytLogon();
		CommHangUp(FALSE);
		break;
	case NO_TRANS_SELECT:
		ucRet = NO_DISP;
		break;
	default:
		ucRet = E_NO_TRANS;
		break;
	} // switch

	CommOnHook(FALSE);
	return ucRet;
}


int TxtToEnv()
{
	int iRet;

	if( fileExist(gszParamFileName)<0 )
	{
		LoadDefaultAppParam();
		SaveAppParam();
		return 0;
	}

	iRet = ReadParamsFile();
	if( iRet!=0 )
	{
		lcdCls();
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "��ȡ�����ļ�ʧ��", "READ PARA FAIL");
		ErrorBeep();
		kbGetKey();					
		return 1;
	}

	AdjustAppParam();
	SaveAppParam();

	return 0;	
}

int  ReadParamsFile(void)
{
	int  ret;
	
	ret = GetParams(gszParamFileName, myParams);
	if( ret==0 )
	{
		fileRemove(gszParamFileName);
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " ��ȡ�����ɹ�!  ", " READ PARA SUC! ");
		ErrorBeep();
		kbGetKey();	
	}
	else
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " ��ȡ����ʧ��!  ", " READ PARA FAIL");
		ErrorBeep();
		kbGetKey();	
		return ret;
	}

	return 0;
}

//20080730
void PabxSetup()
{
	char  buf[16];
	int iRet;

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "���ߺ���        ", "SETUP DAIL PARA ");
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "��������������: ", "INPUT MANAGE PWD");
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 6, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet!=6 )
		return ;

	if( strcmp(buf, operStrc[0].szPassword)!=0x00 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "   ���������   ", "PWD ERROR");
		ErrorBeep();
		kbGetKeyMs(1000);
		return ;
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "���ߺ���Ϊ:", "INPUT PABX:");
	strcpy(buf, (char *)stTransCtrl.szExtNum);
	lcdDisplay(0, 6, DISP_CFONT, buf);
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_BIGFONT+KB_EN_NUM+KB_EN_CHAR+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 0, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet>=0 )
	{
		strcpy((char *)stTransCtrl.szExtNum, buf);
		SaveCtrlParam();
	}

	return;
}

void GetPosCapablity(void)
{
	char    szPosConfigInfo[40];
	int     iRet;
	
	memset(szPosConfigInfo, 0, sizeof(szPosConfigInfo));
	iRet = sysReadConfig((uint8_t *)szPosConfigInfo);
	if( iRet<=0 )
	{
		return;
	}
	
	memset((char*)&gstPosCapability, 0, sizeof(gstPosCapability));
	gstPosCapability.uiPosType = szPosConfigInfo[0];
	gstPosCapability.uiPrnType = szPosConfigInfo[1];
	gstPosCapability.uiSecuType = szPosConfigInfo[5];

	gstPosCapability.uiCommType |= CT_RS232;

	if (szPosConfigInfo[2]!=0)
	{
		gstPosCapability.uiCommType |= CT_MODEM;
	}
	
	if (szPosConfigInfo[8]!=0)
	{
		gstPosCapability.uiCommType |= CT_TCPIP;
	}
	
	if (szPosConfigInfo[9]!=0)
	{
		gstPosCapability.uiCommType |= CT_GPRS;
	}
	
	if (szPosConfigInfo[10]!=0)
	{
		gstPosCapability.uiCommType |= CT_CDMA;
	}
	
	if (szPosConfigInfo[11]!=0)
	{
		gstPosCapability.uiCommType |= CT_WIFI;
	}
	
	switch (szPosConfigInfo[3])
	{
	case 1:		gstPosCapability.uiMaxSyncPSTNRate = 1200;		break;
	case 2:		gstPosCapability.uiMaxSyncPSTNRate = 2400;		break;
	case 3:		gstPosCapability.uiMaxSyncPSTNRate = 9600;		break;
	case 4:		gstPosCapability.uiMaxSyncPSTNRate = 14400;		break;
	default:		break;
	}
	
	switch (szPosConfigInfo[4])
	{
	case 1:		gstPosCapability.uiMaxAsynPSTNRate = 1200;		break;
	case 2:		gstPosCapability.uiMaxAsynPSTNRate = 2400;		break;
	case 3:		gstPosCapability.uiMaxAsynPSTNRate = 4800;		break;
	case 4:		gstPosCapability.uiMaxAsynPSTNRate = 7200;		break;
	case 5:		gstPosCapability.uiMaxAsynPSTNRate = 9600;		break;
	case 6:		gstPosCapability.uiMaxAsynPSTNRate = 12000;		break;
	case 7:		gstPosCapability.uiMaxAsynPSTNRate = 14400;		break;
	case 8:		gstPosCapability.uiMaxAsynPSTNRate = 19200;		break;
	case 9:		gstPosCapability.uiMaxAsynPSTNRate = 24000;		break;
	case 10:	gstPosCapability.uiMaxAsynPSTNRate = 26400;		break;
	case 11:	gstPosCapability.uiMaxAsynPSTNRate = 28800;		break;
	case 12:	gstPosCapability.uiMaxAsynPSTNRate = 31200;		break;
	case 13:	gstPosCapability.uiMaxAsynPSTNRate = 33600;		break;
	case 14:	gstPosCapability.uiMaxAsynPSTNRate = 48000;		break;
	case 15:	gstPosCapability.uiMaxAsynPSTNRate = 56000;		break;
	default:
		break;
	}
	
	gstPosCapability.uiUnContactlessCard = szPosConfigInfo[12];
}

