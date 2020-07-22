
#include <openssl/sha.h>
#include "posapi.h"
#include "posmacro.h"
#include "terstruc.h"
#include "glbvar.h"
#include "tranfunclist.h"
#include "emvtranproc.h"
#include "Menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "New8110_ICC.h"

/********************** Internal macros declaration ************************/
/********************** Internal structure declaration *********************/
/********************** Internal variables declaration *********************/
/********************** Internal functions declaration *********************/
uint8_t  PosGetMobileCard(void);
int      GetMobileCardInfo(void);
/********************** external reference declaration *********************/
extern int gl_FirstRunSelApp;
/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

// ��ȡ�ֻ�����Ϣ
uint8_t  PosGetMobileCard(void)
{
    uint8_t	ucRet; 
	int     iRet;
	char    szTransName[40];
	
	memset(szTransName,0,sizeof(szTransName));
	while(1)
	{
		lcdClrLine(2,7);
		lcdFlip();
		lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,szTransName);
//		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"�����IC����ӿ�");
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"��ӿ�");
		ucRet = DetectCardEvent(CARD_PASSIVE);
		if( ucRet==CARD_KEYIN )
		{
			contactness_close(imif_fd);
			imif_fd = -1;
			if( kbGetKey()==KEY_CANCEL )
			{
				return E_TRANS_CANCEL;
			}
		}
		else if( ucRet == CARD_PASSIVE )
		{// �ֻ���������
			PubBeepErr();
			iRet = GetMobileCardInfo();
			contactness_close(imif_fd);
			imif_fd = -1;
			if (iRet != 0)
			{
				PubBeepErr();
				continue;
			}

			return 0;
		}
		else 
		{  
			PubBeepErr();
			continue;
		}
	}
}


int      GetMobileCardInfo(void)
{
	int		iRet;
	uint8_t szSendData[300], szRecvData[300];
	uint8_t szData[300];
	uint32_t iSendLen, iRecvLen;

	iSendLen = iRecvLen = 0;  
    memset(szSendData, 0x00, sizeof(szSendData));
    memset(szRecvData, 0x00, sizeof(szRecvData));
    // �������������ݵ���Ӧ����ѡ��DF	
	memcpy(szSendData,"\x00\xA4\x04\x00\x0F\xA0\x00\x00\x03\x33\x43\x55\x50\x2D\x4D\x4F\x42\x49\x4C\x45\x00",21);
    iSendLen = 21;
	iRet = contactness_exchangeapdu(imif_fd,iSendLen,szSendData,&iRecvLen,szRecvData);
	if (iRet != 0 || !((szRecvData[iRecvLen-2] == 0x90 && szRecvData[iRecvLen-1] == 0x00)))
	{
		lcdClrLine(2,7);
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"��ȡʧ�ܣ�������");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}

	iSendLen = iRecvLen = 0;  
    memset(szSendData, 0x00, sizeof(szSendData));
    memset(szRecvData, 0x00, sizeof(szRecvData));
    // ��ȡ�����к�	
	memcpy(szSendData,"\x00\xB0\x82\x00\x00\x00",5);
    iSendLen = 5;
	iRet = contactness_exchangeapdu(imif_fd,iSendLen,szSendData,&iRecvLen,szRecvData);	
	if (iRet != 0 || !((szRecvData[iRecvLen-2] == 0x90 && szRecvData[iRecvLen-1] == 0x00)))
	{
		lcdClrLine(2,7);
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"��ȡʧ�ܣ�������");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
	memcpy(szData,szRecvData,iRecvLen-2);
    memset(szRecvData, 0x00, sizeof(szRecvData));
	PubBcd2Asc0((uint8_t*)szData,iRecvLen-2,(char*)szRecvData);
	PubTrimTailChars((char*)szRecvData,'F');
	PubAsc2Bcd((char*)szRecvData,strlen((char*)szRecvData),(char*)PosCom.stTrans.szAID);
	
	iSendLen = iRecvLen = 0;  
    memset(szSendData, 0x00, sizeof(szSendData));
    memset(szRecvData, 0x00, sizeof(szRecvData));
    // �������������ݵ���Ӧ����ѡ��DF	
	memcpy(szSendData,"\x00\xB0\x83\x00\x00\x00",5);
    iSendLen = 5;
	iRet = contactness_exchangeapdu(imif_fd,iSendLen,szSendData,&iRecvLen,szRecvData);	
	if (iRet != 0 || !((szRecvData[iRecvLen-2] == 0x90 && szRecvData[iRecvLen-1] == 0x00)))
	{
		lcdClrLine(2,7);
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"��ȡʧ�ܣ�������");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
	
	iSendLen = iRecvLen = 0;  
    memset(szSendData, 0x00, sizeof(szSendData));
    memset(szRecvData, 0x00, sizeof(szRecvData));
    // �������������ݵ���Ӧ����ѡ��DF	
	memcpy(szSendData,"\x80\xF8\x02\x00\x08\x00\x00\x00\x00\x00\x00\x00\x80\x00",14);
	GetPosTime(stTemp.szDate, stTemp.szTime);	
	sprintf((char*)szData,"%8.8s%6.6s",stTemp.szDate,stTemp.szTime);
	PubAsc2Bcd((char*)szData,17,(char*)PosCom.stTrans.szIssureID);  // ����ʱ��DF34��Ҫ����
	memcpy(&szSendData[5],PosCom.stTrans.szIssureID,7);
    iSendLen = 14;
	iRet = contactness_exchangeapdu(imif_fd,iSendLen,szSendData,&iRecvLen,szRecvData);
	if (iRet != 0 || iRecvLen < 118 || !((szRecvData[iRecvLen-2] == 0x90 && szRecvData[iRecvLen-1] == 0x00)))
	{
		lcdClrLine(2,7);
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"��ȡʧ�ܣ�������");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}

	memset(PosCom.stTrans.szInstalCode,0,sizeof(PosCom.stTrans.szInstalCode));
	memcpy(PosCom.stTrans.szInstalCode,szRecvData,20);
	memset(szData,0,sizeof(szData));
	memcpy(szData,szRecvData+20,10);
	memset(PosCom.stTrans.szCardNo,0,sizeof(PosCom.stTrans.szCardNo));
	PubBcd2Asc0(szData,10,(char*)PosCom.stTrans.szCardNo);
	PubTrimTailChars((char*)PosCom.stTrans.szCardNo,'F');

	memset(szData,0,sizeof(szData));
	iSendLen = PubBcd2Long(szRecvData+30,1,NULL);
	memcpy(szData,szRecvData+31,19);
	memset(PosCom.szTrack2,0,sizeof(PosCom.szTrack2));
	PubBcd2Asc0(szData,19,(char*)szSendData);
	memcpy((char*)PosCom.szTrack2,szSendData,iSendLen);
//	PubTrimTailChars((char*)PosCom.szTrack2,'F');

	memset(szData,0,sizeof(szData));
	iSendLen = PubBcd2Long(szRecvData+50,2,NULL);
	memcpy(szData,szRecvData+52,52);
	memset(PosCom.szTrack3,0,sizeof(PosCom.szTrack3));
	PubBcd2Asc0(szData,52,(char*)szSendData);
	memcpy((char*)PosCom.szTrack3,szSendData,iSendLen);
	
	memset(szData,0,sizeof(szData));
	memcpy(szData,szRecvData+104,2);
	memset(PosCom.stTrans.szExpDate,0,sizeof(PosCom.stTrans.szExpDate));
	PubBcd2Asc0(szData,2,(char*)PosCom.stTrans.szExpDate);

	memcpy(PosCom.stTrans.sAppCrypto,szRecvData+108,8);

	PosCom.ucSwipedFlag = CARD_PASSIVE;
	PosCom.stTrans.ucSwipeFlag = PosCom.ucSwipedFlag;	// ����ˢ����־
	PosCom.stTrans.ucFallBack = FALSE;

	return 0;
}

// �����ֽ�˵�
uint8_t SelectEcFunc(void)
{
	int keySub; //�����˵����ؽ�ֵ
	// �����ֽ�˵�
	const char *Ec_menu[] = 
	{
		"1 ����֧��      ",
		"2 ��֧ͨ��      ",
		"3 Ȧ��          ",
		"4 ����ѯ      ",
		"5 ��ϸ��ѯ      ",
		"6 �ѻ��˻�      "
	};

	const char *Ec_Topup_menu[] = 
	{
		"1 �ֽ��ֵ      ",
		"2 ָ���˻�Ȧ��  ",
		"3 ��ָ���˻�Ȧ��",
		"4 �ֽ��ֵ����  "
	};

	keySub = DispMenu(Ec_menu, 6, 0, KEY1, KEY6, stPosParam.ucOprtLimitTime);
	if( keySub==KEY1 ) 
		stTemp.iTransNo = EC_QUICK_SALE;
	else if( keySub==KEY2 ) 
		stTemp.iTransNo = EC_NORMAL_SALE;
	else if( keySub==KEY3 ) 
	{
		keySub = DispMenu(Ec_Topup_menu, 4, 0, KEY1, KEY4, stPosParam.ucOprtLimitTime);
		if( keySub==KEY1 )
			stTemp.iTransNo = EC_TOPUP_CASH;		
		else if( keySub==KEY2 )
			stTemp.iTransNo = EC_TOPUP_SPEC;
		else if( keySub==KEY3 )
			stTemp.iTransNo = EC_TOPUP_NORAML;
		else if( keySub==KEY4 )
			stTemp.iTransNo = EC_VOID_TOPUP;
		else
			return NO_DISP;
	}
	else if( keySub==KEY4 ) 
		stTemp.iTransNo = EC_BALANCE;
	else if( keySub==KEY5 ) 
		stTemp.iTransNo = EC_QUE_TXN;
	else if( keySub==KEY6 ) 
		stTemp.iTransNo = EC_REFUND;
	else 
		return NO_DISP;

	return OK;
}

// ����Ǯ���˵�
uint8_t SelectEdFunc(void)
{
	int keySub; //�����˵����ؽ�ֵ
	// �����ֽ�Ȧ��˵�
	const char *Ed_menu[] = 
	{
		"1 ����      ",
		"2 Ȧ��          ",
		"3 ����ѯ      ",
		"4 ��ϸ��ѯ      "
	};
	
	const char *Ed_Topup_menu[] = 
	{
		"1 �ֽ��ֵ      ",
		"2 ָ���˻�Ȧ��  ",
		"3 ��ָ���˻�Ȧ��"
	};
	
	keySub = DispMenu(Ed_menu, 4, 0, KEY1, KEY4, stPosParam.ucOprtLimitTime);
	if( keySub==KEY1 ) 
		stTemp.iTransNo = ED_SALE;
	else if( keySub==KEY2 ) 
	{
		keySub = DispMenu(Ed_Topup_menu, 3, 0, KEY1, KEY3, stPosParam.ucOprtLimitTime);
		if( keySub==KEY1 )
			stTemp.iTransNo = ED_TOPUP_CASH;		
		else if( keySub==KEY2 )
			stTemp.iTransNo = ED_TOPUP_SPEC;
		else if( keySub==KEY3 )
			stTemp.iTransNo = ED_TOPUP_NORAML;
		else
			return NO_DISP;
	}
	else if( keySub==KEY3 ) 
		stTemp.iTransNo = ED_BALANCE;
	else if( keySub==KEY4 ) 
		stTemp.iTransNo = ED_QUE_TXN;
	else 
		return NO_DISP;
	
	return OK;
}


// ���ڲ˵�
uint8_t SelectInstalFunc(void)
{
	int keySub; //�����˵����ؽ�ֵ	
	// ���ڸ���˵�
	const char *Instal_menu[] = 
	{
		"1 ��������      ",
		"2 �������ѳ���  "
	};

	keySub = DispMenu(Instal_menu, 2, 0, KEY1, KEY2, stPosParam.ucOprtLimitTime);
	if( keySub==KEY1 ) 
		stTemp.iTransNo = POS_INSTALLMENT;
	else if( keySub==KEY2 ) 
		stTemp.iTransNo = POS_VOID_INSTAL;
	else 
		return NO_DISP;
	
	return OK;
}

// ���ֲ˵�
uint8_t SelectLoyaltFunc(void)
{
	int keySub; //�����˵����ؽ�ֵ	
	// ���ֲ˵�
	const char *Loyalt_menu[] = 
	{
		"1 ��������      ",
		"2 �������ѳ���  ",
		"3 ���˻��ֲ�ѯ  ",
		"4 ���˻����˻�  "
	};
	
	// �������Ѳ˵�
	const char *Loyalt_Sale_menu[] = 
	{
		"1 �����л�������",
		"2 ���˻�������  "
	};
	
	// ���ֳ����˵�
	const char *Loyalt_Void_menu[] = 
	{
		"1 �����л��ֳ���",
		"2 ���˻��ֳ���  "
	};


	keySub = DispMenu(Loyalt_menu, 4, 0, KEY1, KEY4, stPosParam.ucOprtLimitTime);
	if( keySub==KEY1 ) 
	{
		keySub = DispMenu(Loyalt_Sale_menu, 2, 0, KEY1, KEY2, stPosParam.ucOprtLimitTime);
		if( keySub==KEY1 )
			stTemp.iTransNo = POS_LOYATL_ISSUER;		
		else if( keySub==KEY2 )
			stTemp.iTransNo = POS_LOYATL_ALL;
		else
			return NO_DISP;
	}
	else if( keySub==KEY2 ) 
	{
		keySub = DispMenu(Loyalt_Void_menu, 2, 0, KEY1, KEY2, stPosParam.ucOprtLimitTime);
		if( keySub==KEY1 )
			stTemp.iTransNo = POS_VOID_LOYATL_ISSUER;		
		else if( keySub==KEY2 )
			stTemp.iTransNo = POS_VOID_LOYATL_ALL;
		else
			return NO_DISP;
	}
	else if( keySub==KEY3 ) 
		stTemp.iTransNo = POS_QUE_LOYATL_ALL;
	else if( keySub==KEY4 ) 
		stTemp.iTransNo = POS_REFUND_LOYATL_ALL;
	else 
		return NO_DISP;
	
	return OK;
}

// �ֻ�оƬ�˵�
uint8_t SelectMobileFunc(void)
{
	int keySub; //�����˵����ؽ�ֵ
	const char *Moblie_menu[] = 
	{
		"1 �ֻ�����      ",
		"2 �ֻ����ѳ���  ",
		"3 �ֻ�оƬ�˻�  ",
		"4 �ֻ�оƬԤ��Ȩ",
		"5 �ֻ�Ԥ��Ȩ����",
		"6 Ԥ��Ȩ�������",
		"7 Ԥ��Ȩ���֪ͨ",
		"8 Ԥ��Ȩ��ɳ���",
		"9 ����ѯ      "
	};
	
	keySub = DispMenu(Moblie_menu, 9, 0, KEY1, KEY9, stPosParam.ucOprtLimitTime);
	if( keySub==KEY1 ) 
		stTemp.iTransNo = POS_MOBILE_SALE;
	else if( keySub==KEY2 ) 
		stTemp.iTransNo = POS_MOBILE_VOID;
	else if( keySub==KEY3 ) 
		stTemp.iTransNo = POS_MOBILE_REFUND;
	else if( keySub==KEY4 ) 
		stTemp.iTransNo = POS_MOBILE_PREAUTH;
	else if( keySub==KEY5 ) 
		stTemp.iTransNo = POS_MOBILE_VOID_PRE;
	else if( keySub==KEY6 ) 
		stTemp.iTransNo = POS_MOBILE_AUTH;
	else if( keySub==KEY7 ) 
		stTemp.iTransNo = POS_MOBILE_AUTH_OFF;
	else if( keySub==KEY8 ) 
		stTemp.iTransNo = POS_MOBILE_VOID_AUTH;
	else if( keySub==KEY9 ) 
		stTemp.iTransNo = POS_MOBILE_QUE;
	else 
		return NO_DISP;
	
	return OK;
}

// �����ֽ�����
uint8_t TransEcSale(int iTransType)
{
	uint8_t	ucRet;
	uint8_t  szBuffer[200];
	uint8_t  uLen;
	int      iCnt, iRet;
	
	emv_pboc_ec_enable(1);	// ���õ����ֽ���
	emv_pboc_set_ec_ttl(stPosParam.iEcMaxAmount); // ���������

	while(1)
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "   �����ֽ�����   ", "      SALE      ");
		ucRet = AppGetAmount(9, TRAN_AMOUNT);
		if( ucRet!=OK )
		{
			return ucRet;
		}
		
		if( (stPosParam.iEcMaxAmount!=0) &&
			(BcdToLong(PosCom.stTrans.sAmount, 6)>stPosParam.iEcMaxAmount) )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  ��������Ч  ", "INVALID AMOUNT");
			DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, " �������ֽ��� ", "EXLIMITED");
			ErrorBeep();
			kbGetKeyMs(2000);		
			continue;
		}

		break;
	}
	
	ucRet = PosGetCard(CARD_INSERTED);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	memset(szBuffer,0,sizeof(szBuffer));
	emv_get_data_element("\x4F", 1, &uLen, szBuffer);	// AID
	BcdToAsc0(PosCom.stTrans.szAID, (uint8_t*)szBuffer, uLen*2);
	RemoveTailChars(PosCom.stTrans.szAID, 'F');	
	for (iCnt=0; iCnt< MAX_APP_NUM; iCnt++)
	{
		if (memcmp(szBuffer,glTermAidList[iCnt].sAID,uLen) == 0)
		{
			break;
		}
	}
	if (iCnt >= MAX_APP_NUM)
	{
		emv_pboc_set_ec_ttl(stPosParam.iEcMaxAmount); // ���������
	}
	else
	{	
		emv_pboc_set_ec_ttl(glTermAidList[iCnt].m_EcMaxTxnAmount); // ���������
		if (glTermAidList[iCnt].m_EcMaxTxnAmount == 0)
		{
			emv_pboc_set_ec_ttl(stPosParam.iEcMaxAmount);
		}
	}

	if (ChkIfBlkCard((char*)PosCom.stTrans.szCardNo))
	{
		lcdClrLine(2,7);
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"�ÿ������������");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}

	if (ChkOnlyEcCard(PosCom.stTrans.szAID) && stTemp.iTransNo == EC_NORMAL_SALE)
	{
		memset(szBuffer,0,sizeof(szBuffer));
		//	iRet = emv_get_data_element("\x9F\x79", 2, &iLen, szBuffer);	// �����ֽ����
		iRet = GetEcElementData((uint8_t*)"\x9F\x79", &uLen, (uint8_t*)szBuffer);	// �����ֽ����
		if (iRet == 0)
		{
			iRet = PubBcd2Long((uint8_t*)szBuffer,uLen,NULL);
			if (memcmp(PosCom.stTrans.sAmount,szBuffer,6) > 0)
			{
				lcdClrLine(2,7);
				lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"�������");
				lcdDisplay(0,5,DISP_CFONT|DISP_MEDIACY,"���������ֽ����");
				PubBeepErr();
				PubWaitKey(5);
				return E_TRANS_FAIL;
			}
		}
	}
	
	SetCommReqField((uint8_t *)"0200", (uint8_t *)"000000");
	// �Ƚ����߽��׵Ŀ����͹̶����ڿ�
	strcpy((char *)PosCom.stTrans.szCardUnit, "CUP");
	return ProcEmvTran();
}

//  �����ֽ��ֵ
uint8_t TransEcTopup(int iTransType)
{
	uint8_t	ucRet, iLen, ucSwipedFlagBak;
	char    szProcCode[7];
	int     iRet;
	char    szBuffer[100];
	uint8_t  szTrack2Bak[TRACK2_LEN+1];
	uint8_t  szTrack3Bak[TRACK3_LEN+1];
	
	emv_pboc_ec_enable(1);	// ���õ����ֽ���
	emv_pboc_set_ec_ttl(stPosParam.iEcMaxAmount); // ���������

	if(iTransType == EC_TOPUP_CASH)
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "   �����ֽ��ֵ   ", "     EC LOAD    ");
	}
	else if(iTransType == EC_TOPUP_SPEC)
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  ָ���ʻ�Ȧ��  ", "     EC LOAD    ");
	}
	else
	{
		// ��ָ���˻�Ȧ���һ��Ҳ���Բ忨��ֻ��ȡ����,������ı�־��ʶ�ǵ�һ�β忨
		PosCom.ucEdOrEpFlag = TRUE;
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " ��ָ���ʻ�Ȧ�� ", "     EC LOAD    ");
		ucRet = PosGetCard(CARD_SWIPED|CARD_INSERTED);
		if( ucRet!=OK )
		{
			return ucRet;
		}

		PosCom.stTrans.ucEcTopOldSwipeFlag = ucSwipedFlagBak = PosCom.ucSwipedFlag;
		strcpy((char*)PosCom.stTrans.szTranferCardNo,(char*)PosCom.stTrans.szCardNo);
		PosCom.ucSwipedFlag = NO_SWIPE_INSERT;
		strcpy((char*)szTrack2Bak,(char*)PosCom.szTrack2);
		strcpy((char*)szTrack3Bak,(char*)PosCom.szTrack3);
		PosCom.ucEdOrEpFlag = FALSE;
		if (ucSwipedFlagBak == CARD_INSERTED)
		{
			DispRemoveICC();
		}
	}
	
	if(iTransType == EC_TOPUP_CASH)
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "   �����ֽ��ֵ   ", "     EC LOAD    ");
	}
	else if(iTransType == EC_TOPUP_SPEC)
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  ָ���ʻ�Ȧ��  ", "     EC LOAD    ");
	}
	else
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " ��ָ���ʻ�Ȧ�� ", "     EC LOAD    ");
	}
	ucRet = PosGetCard(CARD_INSERTED);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	// �����ཻ�׿�����Ҫ�жϺ�����
/*	if (ChkIfBlkCard((char*)PosCom.stTrans.szCardNo))
	{
		lcdClrLine(2,7);
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"�ÿ������������");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}*/

	ucRet = AppGetAmount(9, TRAN_AMOUNT);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	if(iTransType != EC_TOPUP_CASH )
	{
		ucRet = EnterPIN(0);
		if( ucRet!=OK )
		{
			if( ucRet==PED_RET_NOKEY )
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				DispMulLanguageString(0, 2, DISP_CFONT, "��������ǩ��", "LOGON ONLINE");
				kbGetKey();
			}
			return ucRet;
		}
	}
	
	memset(szBuffer,0,sizeof(szBuffer));
//	iRet = emv_get_data_element("\x9F\x79", 2, &iLen, szBuffer);	// �����ֽ����
	iRet = GetEcElementData((uint8_t*)"\x9F\x79", &iLen, (uint8_t*)szBuffer);	// �����ֽ����
	if (iRet == 0)
	{
		iRet = PubBcd2Long((uint8_t*)szBuffer,iLen,NULL);
		PubLong2Char(iRet,4,(char*)PosCom.stTrans.sBalanceAmount);
	}

//	iRet = emv_get_data_element("\x9F\x77", 2, &iLen, szBuffer);	// �����ֽ��������
	iRet = GetEcElementData((uint8_t*)"\x9F\x77", &iLen, (uint8_t*)szBuffer);	// �����ֽ��������
	if (iRet == 0)
	{
		iRet = PubBcd2Long((uint8_t*)szBuffer,iLen,NULL);
		PubLong2Char(iRet,4,(char*)PosCom.stTrans.sAfterBalanceAmt);  // 
	}		
	
	memset(szProcCode,0,sizeof(szProcCode));
	if (iTransType == EC_TOPUP_CASH)
	{
		strcpy(szProcCode,"630000");
	}
	else if (iTransType == EC_TOPUP_NORAML)
	{
		strcpy((char*)szProcCode,"620000");
		strcpy((char*)PosCom.szTrack2,(char*)szTrack2Bak);
		strcpy((char*)PosCom.szTrack3,(char*)szTrack3Bak);
	}
	else
	{
		strcpy((char*)szProcCode,"600000");
	}

	SetCommReqField((uint8_t *)"0200", (uint8_t *)szProcCode);
	glSendPack.szField60[13] = 0; // 60.6-7����
	return ProcEmvTran();
}

//  �����ֽ��ֵ����
uint8_t TransEcVoidTopup(int iTransType)
{	
	uint8_t	ucRet, ucSwipeFlag;
	uint32_t	ulICCDataLen;
	uint8_t	*psICCData, *psTemp;
	int     iRet;
		
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "�����ֽ��ֵ����", "VOID INSTALLMENT");
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
	if(PosCom.stTrans.iOldTransNo != EC_TOPUP_CASH )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "ԭ���׷��ֽ��ֵ", "NOT PREAUTH");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "    ���ܳ���    ", "NOT VOID");
		ErrorBeep();
		kbGetKeyMs(3000);	
		return NO_DISP;
	}

	ucSwipeFlag = CARD_INSERTED|0x80;
	ucRet = PosGetCard(ucSwipeFlag);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	psICCData = PosCom.stTrans.sIccData;
	ulICCDataLen = PosCom.stTrans.iIccDataLen;
	memset((char*)&gstIccPrintData,0,sizeof(gstIccPrintData));
	for(psTemp=psICCData; psTemp<psICCData+ulICCDataLen; )
	{
		iRet = GetTlvValue(&psTemp, psICCData+ulICCDataLen-psTemp, SavePrintICCData, TRUE);
		if( iRet<0 )
		{	
			break;
		}
	}
	if (gstIccPrintData.ucIssureAppDatalen!=0)
	{
		emv_set_data_element("\x9F\x10", 2, gstIccPrintData.ucIssureAppDatalen, gstIccPrintData.sIssureAppData);
	}
	emv_set_data_element("\x9F\x36", 2, 1, gstIccPrintData.sAppAtc);
	if (gstIccPrintData.ucTVRoK)
	{
		emv_set_data_element("\x95", 1, 5, gstIccPrintData.sTVR);
	}
	if (gstIccPrintData.ucTIDSnoK)
	{
		emv_set_data_element("\x9F\x1E", 2, 8, gstIccPrintData.sTIDSn);
	}
	if (gstIccPrintData.ucScritptRetlen!=0)
	{
		emv_set_data_element("\xDF\x31", 2, gstIccPrintData.ucScritptRetlen, gstIccPrintData.sScritptRet);
	}
	
	SetCommReqField((uint8_t *)"0200", (uint8_t *)"170000");
	if( PosCom.stTrans.szSysReferNo[0]==0 )
	{
		sprintf((char *)glSendPack.szRRN, "%0*ld", LEN_RRN, 0L);
	}
	else
	{
		sprintf((char *)glSendPack.szRRN, "%.*s", LEN_RRN, PosCom.stTrans.szSysReferNo);
	}
	sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d", PosCom.stTrans.lOldBatchNumber,
		PosCom.stTrans.lOldTraceNo);
	glSendPack.szField60[13] = 0; // 60.6-7����
	ShortToByte(glSendPack.sICCData, PosCom.stTrans.iIccDataLen);
	memcpy(&glSendPack.sICCData[2], PosCom.stTrans.sIccData, PosCom.stTrans.iIccDataLen);

	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;

//	return ProcEmvTran();
}

// �����ֽ��˻�
uint8_t TransEcRefund(void)
{	
	uint8_t ucRet, ucSwipeMode;
	uint8_t buf[10];
	int iRet;

	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  �����ֽ��˻�  ", "   EC REFUND    ");
	ucRet = CheckSupPwd();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	ucSwipeMode = CARD_INSERTED|CARD_PASSIVE;	
	ucRet = PosGetCard(ucSwipeMode);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	memset((char*)buf,0,sizeof(buf));
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  �����ֽ��˻�  ", "   EC REFUND    ");		
	DispMulLanguageString(0, 2, DISP_CFONT, "������ԭ�ն˺�:", "INPUT TID:");
	iRet = PubGetString(NUM_IN,8,8,(uint8_t*)buf,stPosParam.ucOprtLimitTime);
	if( iRet != 0 )	
		return (E_TRANS_CANCEL);
	memset((char*)PosCom.stTrans.szInstalCode,0,sizeof(PosCom.stTrans.szInstalCode));
	strcpy((char*)PosCom.stTrans.szInstalCode,(char*)buf);

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  �����ֽ��˻�  ", "   EC REFUND    ");	
	DispMulLanguageString(0, 2, DISP_CFONT, "������", "PLS INPUT:");
	DispMulLanguageString(0, 4, DISP_CFONT, "ԭ�������κ�:", "ORG BATCH NO.:");
	iRet = PubGetString(NUM_IN,1,6,(uint8_t*)buf,stPosParam.ucOprtLimitTime);
	if( iRet != 0 )	
		return (E_TRANS_CANCEL);
	PosCom.stTrans.lOldBatchNumber = atol((char *)buf);	
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  �����ֽ��˻�  ", "   EC REFUND    ");	
	DispMulLanguageString(0, 2, DISP_CFONT, "������", "PLS INPUT:");
	DispMulLanguageString(0, 4, DISP_CFONT, "ԭ������ˮ��:", "ORG BATCH NO.:");
	iRet = PubGetString(NUM_IN,1,6,(uint8_t*)buf,stPosParam.ucOprtLimitTime);
	if( iRet != 0 )	
		return (E_TRANS_CANCEL);
	PosCom.stTrans.lOldTraceNo = atol((char *)buf);
	sprintf((char *)PosCom.stTrans.szPrtOldPosSer, "%06ld", atol((char *)buf));
	
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
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  ��������Ч  ", "INVALID AMOUNT");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "   ���˻����   ", "EXLIMITED");
		ErrorBeep();
		kbGetKeyMs(2000);		
		return NO_DISP;
	}
	ucRet = AppConfirmAmount();
	if( ucRet==KEY_CANCEL )
	{
		goto REINPUT;
	}

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  �����ֽ��˻�  ", "   EC REFUND    ");	
	if( PosCom.stTrans.szCardUnit[0]==0 )
	{
		strcpy((char *)PosCom.stTrans.szCardUnit, "CUP");
	}
	strcpy((char *)PosCom.stTrans.szOldTxnDate, "0000");
	
	SetCommReqField((uint8_t *)"0220", (uint8_t *)"200000");
	sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s", PosCom.stTrans.lOldBatchNumber,
		PosCom.stTrans.lOldTraceNo, PosCom.stTrans.szOldTxnDate);
	sprintf((char *)glSendPack.szField63, "%.3s", PosCom.stTrans.szCardUnit);
	ShortToByte(glSendPack.sField62, 8);		// �����Ƶ���,ͷ2���ֽڴ�����ݳ���
	sprintf((char *)&glSendPack.sField62[2], "%8.8s", PosCom.stTrans.szInstalCode);
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;
}

// �����ֽ����
uint8_t TransEcBalance(void)
{
	int  iRet;
	APDU_SEND stSendApdu;
	APDU_RESP stRespApdu;
	uint8_t	sBalance[6], szBuff[20];
	uint8_t szBuf[17];
	uint8_t Atr[500];
	int     iLen;

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "�����ֽ�����ѯ", "   EC BALANCE   ");
	while(1)
	{
		ShowGetCardScreen(CARD_INSERTED);
		iRet = DetectCardEvent(CARD_INSERTED);
		if( iRet ==CARD_KEYIN )
		{
			if( kbGetKey()==KEY_CANCEL )
			{
				return NO_DISP;
			}
		}
		else if( iRet==CARD_INSERTED )
		{
			break;
		}
	}

	iccSetPara(ICC_USERCARD,ICC_VOL_5V,ICC_ISO);
	iRet = iccPowerUp(ICC_USERCARD, &iLen, Atr);
	if( iRet )
	{
		lcdClrLine(2,7);
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"��ȡIC������");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;		
	}
	gl_FirstRunSelApp = 0x01;
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "�����ֽ�����ѯ", "   EC BALANCE   ");
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��ȡ�����...");
	iRet = emv_application_select_no_gpo(icc_fd);
	if(iRet != 0)
	{
		lcdClrLine(2,7);
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"��������ʧ��");
		PubWaitKey(5);
		return NO_DISP;
	}
	
	memset((char*)&stSendApdu,0,sizeof(stSendApdu));
	memset((char*)&stRespApdu,0,sizeof(stRespApdu));	
	stSendApdu.CLA = 0x80;
	stSendApdu.INS = 0xCA;
	stSendApdu.P1 = 0x9F;
	stSendApdu.P2 = 0x79;
	stSendApdu.Lc = 0;
	stSendApdu.Le = 0;
	stSendApdu.LeFlag = 0x01;	
	iRet = iccExchangeAPDU(icc_fd,&stSendApdu,&stRespApdu);
	if(iRet != ICC_SUCCESS
		|| stRespApdu.SWA != 0x90
		|| stRespApdu.SWB != 0x00)
	{
		lcdCls();
		lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"%02x %02x ����ʧ��",stRespApdu.SWA,stRespApdu.SWB);
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
	
	if(stRespApdu.LenOut != 0x09
		|| stRespApdu.DataOut[0] != 0x9F
		|| stRespApdu.DataOut[1] != 0x79
		|| stRespApdu.DataOut[2] != 0x06)
	{
		lcdCls();
		lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,"    ����ʧ��    ");
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"%02x %02x ����ʧ��",stRespApdu.SWA,stRespApdu.SWB);
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
	

	lcdCls();
	lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,"�����ֽ�����ѯ");
	lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"��ѯ�ɹ�");
	lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��鿴�������");
	lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"���ο����ÿ�");
	
	// �����˽�������ı�־֮�����в���ʵ��
	memset(sBalance,0,sizeof(sBalance));
	memset(szBuff,0,sizeof(szBuff));
	memcpy(sBalance,stRespApdu.DataOut+3,6);
	szBuff[0] = ' ';
	ConvBcdAmount(sBalance, &szBuff[1]);
	
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_MEDIACY|DISP_CFONT, "    ��ѯ�ɹ�    ", " QUERY SUCCESS  ");
	DispMulLanguageString(0, 4, DISP_CFONT, "�������:", "BALANCE:");
	DispMulLanguageString(0, 6, DISP_CLRLINE|DISP_CFONT, " ��鿴������� ", " LOOK PINPAD    ");
	memset(szBuf, 0x20, 16);
	iLen = strlen((char *)szBuff);
	memcpy(&szBuf[16-iLen-1], szBuff, iLen);
	szBuf[15] = 0;
	lcdDisplay(0, 6, DISP_CFONT, (char *)szBuf);	
	PubBeepErr();	
	kbGetKeyMs(10000);		// �ȴ�4����
	
	return (OK);	
}


// �����ֽ��ѯ��¼
uint8_t TransEcViewRecord(void)
{
	int  iRet, iCurRec, iAtcNum;
	uint8_t     szBuffer[50], szAmout[20],szotherAmt[20], szCurCode[10], szTxnCurCode[10];
	uint8_t     szDataTime[30], szTxnAtc[4], szMerchantName[30], szTxnType[4];
	uint8_t     iTotalRecNum, iDatalen;
	int       iLen;
	char      Atr[500];

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "�����ֽ��¼��ѯ", "   EC RECORD   ");	
	while(1)
	{
		ShowGetCardScreen(CARD_INSERTED);
		iRet = DetectCardEvent(CARD_INSERTED);
		if( iRet ==CARD_KEYIN )
		{
			if( kbGetKey()==KEY_CANCEL )
			{
				return NO_DISP;
			}
		}
		else if( iRet==CARD_INSERTED )
		{
			break;
		}
	}

	iccSetPara(ICC_USERCARD,ICC_VOL_5V,ICC_ISO);
	iRet = iccPowerUp(ICC_USERCARD, &iLen, Atr);
	if( iRet )
	{
		lcdClrLine(2,7);
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"��ȡIC������");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;		
	}
	gl_FirstRunSelApp = 0x01;
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "�����ֽ��¼��ѯ", "   EC RECORD   ");
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��ȡIC����¼��..");
	iRet = emv_application_select_no_gpo(icc_fd);
	if(iRet != 0)
	{
		lcdClrLine(2,7);
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
		lcdDisplay(0,5,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}

	iRet = emv_check_data_element("\x9F\x4D",2);
	if( iRet!= 0 )
	{
		lcdClrLine(2,7);
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
		lcdDisplay(0,5,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}

	iTotalRecNum = 0;
	iCurRec = 1;
	memset(szBuffer,0,sizeof(szBuffer));
	emv_get_data_element("\x9F\x4D",2,&iTotalRecNum,szBuffer);
	iTotalRecNum = szBuffer[1];
	while(1)
	{
		lcdCls();
		sprintf((char*)szBuffer,"%d/%d",iCurRec,(int)iTotalRecNum);
		lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_INVLINE, "������ϸ%8.8s",szBuffer);
		iRet = emv_read_log_record(icc_fd,iCurRec);
		if(iRet)
		{			
			if(emv_check_data_element("\x9F\x4F",2) != 1)
			{
				lcdClrLine(2,7);
				lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"��������ʧ��");
				PubWaitKey(5);
				iCurRec++;
				if (iCurRec > iTotalRecNum)
				{
					iCurRec = 1;
				}
				return NO_DISP;
			}
			else if((emv_get_last_iccard_sw() == 0x6A83) || (iRet == EMV_ERRNO_DATA))
			{
				lcdClrLine(2,7);
				lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"��¼β");
				PubWaitKey(3);
				iCurRec = 1;
				return NO_DISP;
			}
			else
			{
				lcdClrLine(2,7);
				lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"��������ʧ��");
				PubWaitKey(5);
				iCurRec++;
				if (iCurRec > iTotalRecNum)
				{
					iCurRec = 1;
				}
				return NO_DISP;
			}			
		}
		
		memset(szBuffer,0,sizeof(szBuffer));
		memset(szAmout,0,sizeof(szAmout));
		memset(szDataTime,0,sizeof(szDataTime));
		memset(szTxnAtc,0,sizeof(szTxnAtc));
		memset(szMerchantName,0,sizeof(szMerchantName));
		memset(szTxnType,0,sizeof(szTxnType));
		// ʱ��
		emv_get_log_item("\x9A",1, szBuffer, &iDatalen);
		memcpy(szDataTime,"20",2);
		PubBcd2Asc0(szBuffer,3,(char*)szDataTime+2);
		memset(szBuffer,0x00,sizeof(szBuffer));
	    emv_get_log_item("\x9F\x21",2, szBuffer, &iDatalen);
		PubBcd2Asc0(szBuffer,3,(char*)szDataTime+8);
		// ����
		memset(szBuffer,0x00,sizeof(szBuffer));
	    emv_get_log_item("\x9F\x36",2, szTxnAtc, &iDatalen);		
		// �̻�����
		memset(szMerchantName,0x00,sizeof(szMerchantName));
		emv_get_log_item("\x9F\x4E",2, szMerchantName, &iDatalen);	
		// ��������
		memset(szTxnType,0x00,sizeof(szTxnType));
		emv_get_log_item("\x9C",1, szTxnType, &iDatalen);
		// ���
		memset(szBuffer,0x00,sizeof(szBuffer));
		emv_get_log_item("\x9F\x02",2, szBuffer, &iDatalen);
		PubBcd2Asc0(szBuffer,6,(char*)szAmout);
		// ��Ȩ���
		memset(szBuffer,0x00,sizeof(szBuffer));
		emv_get_log_item("\x9F\x03",2, szBuffer, &iDatalen);
		PubBcd2Asc0(szBuffer,6,(char*)szotherAmt);
		// ���Ҵ���
		memset(szBuffer,0x00,sizeof(szBuffer));
		emv_get_log_item("\x9F\x1A",2, szBuffer, &iDatalen);
		PubBcd2Asc0(szBuffer,2,(char*)szCurCode);
		// ���׻��Ҵ���
		memset(szBuffer,0x00,sizeof(szBuffer));
		emv_get_log_item("\x5F\x2A",2, szBuffer, &iDatalen);
		PubBcd2Asc0(szBuffer,2,(char*)szTxnCurCode);

		lcdClrLine(2,7);
		memset(szBuffer,0x00,sizeof(szBuffer));
		PubConvAmount(NULL,szAmout,2,szBuffer,0);
		lcdDisplay(0,2,DISP_ASCII,"AMT:");
		lcdDisplay(0,3,DISP_ASCII|DISP_MEDIACY,"%s",szBuffer);
		memset(szBuffer,0x00,sizeof(szBuffer));
		iAtcNum = PubChar2Long(szTxnAtc,2,NULL);
		switch (szTxnType[0])
		{
		case 0x01:
			sprintf((char*)szBuffer,"��������:ȡ��");
			break;
		case 0x09:
			sprintf((char*)szBuffer,"��������:����");
			break;
		case 0x60:
		case 0x62:
		case 0x63:
			sprintf((char*)szBuffer,"��������:Ȧ��");
			break;
		default:
			sprintf((char*)szBuffer,"��������:����");
			break;
		}
		lcdDisplay(0,4,DISP_CFONT,(char*)szBuffer);
		lcdDisplay(0,6,DISP_ASCII,"ATC: %02d",iAtcNum);
		lcdDisplay(0,7, DISP_ASCII, "%4.4s/%2.2s/%2.2s %2.2s:%2.2s:%2.2s", szDataTime,
		szDataTime+4,szDataTime+6,szDataTime+8,szDataTime+10,szDataTime+12);
		
		// ��ʾ�ڶ�ҳ
		while(1)
		{
			iRet = PubWaitKey(60);
			if( iRet==KEY_CANCEL || iRet==KEY_INVALID )
			{
				return 0;
			}
			else if( iRet==KEY_DOWN || iRet== KEY_ENTER )
			{
				lcdClrLine(2,7);
				memset(szBuffer,0x00,sizeof(szBuffer));
				PubConvAmount(NULL,szotherAmt,2,szBuffer,0);
				lcdDisplay(0,2,DISP_ASCII,"OTHER AMT: %s",szBuffer);
				lcdDisplay(0,3,DISP_ASCII,"CUR CODE:%s",(char*)&szCurCode[1]);
				lcdDisplay(0,4,DISP_ASCII,"TXN CODE:%s",(char*)&szTxnCurCode[1]);
				lcdDisplay(0,5,DISP_ASCII,"MERCHANT NAME:");
				lcdDisplay(0,6,DISP_ASCII,"%s",szMerchantName);
				break;
			}
			else
			{
				PubBeepErr();
				continue;
			}
		}

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
//				iCurRec = 1;
				// ����10������˳���
				lcdClrLine(2,7);
				lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"�� ¼ β");
				PubBeepErr();
				PubWaitKey(5);
				return 0;
			}
		}
		else if( iRet==KEY_UP || iRet==KEY_CLEAR )
		{
			if (iCurRec > 1)
				iCurRec -= 1;
			else
				iCurRec = iTotalRecNum;
		}
		else
		{
			PubBeepErr();
		}	
	}

	return 0;	
}

// ��ȡ�����ֽ���� ���� �����ֽ�������� ������
uint8_t GetEcElementData(uint8_t *tag, uint8_t *len, uint8_t *val)
{
	int  iRet;
	APDU_SEND stSendApdu;
	APDU_RESP stRespApdu;
	
	memset((char*)&stSendApdu,0,sizeof(stSendApdu));
	memset((char*)&stRespApdu,0,sizeof(stRespApdu));	
	stSendApdu.CLA = 0x80;
	stSendApdu.INS = 0xCA;
	stSendApdu.P1 = tag[0];
	stSendApdu.P2 = tag[1];
	stSendApdu.Lc = 0;
	stSendApdu.Le = 0;
	stSendApdu.LeFlag = 0x01;	
	iRet = iccExchangeAPDU(icc_fd,&stSendApdu,&stRespApdu);
	if(iRet != ICC_SUCCESS
		|| stRespApdu.SWA != 0x90
		|| stRespApdu.SWB != 0x00)
	{
		return NO_DISP;
	}
	
	if(stRespApdu.LenOut != 0x09
		|| stRespApdu.DataOut[0] != tag[0]
		|| stRespApdu.DataOut[1] != tag[1] 
		|| stRespApdu.DataOut[2] != 0x06)
	{
		return NO_DISP;
	}
		
	if (val != NULL)
	{
		*len = stRespApdu.DataOut[2];
		memcpy((char*)val,(char*)stRespApdu.DataOut+3,stRespApdu.DataOut[2]);
	}

	return (OK);
}

// ����Ǯ������
uint8_t TransEdSale(int iTransType)
{
	uint8_t ucRet;
	uint32_t uiAmount, uiBalance;
	APDU_SEND stApduSend;
	APDU_RESP stApduResp;  
	int   iRet;
	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  ����Ǯ������  ", "      SALE      ");
	ucRet = AppGetAmount(9, TRAN_AMOUNT);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucRet = PosGetEdCard(CARD_INSERTED);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	uiAmount = PubBcd2Long(PosCom.stTrans.sAmount,6,NULL);
	uiBalance = PubChar2Long(PosCom.stTrans.sBalanceAmount,4,NULL);
	if (uiAmount > uiBalance)
	{
		lcdCls();
		lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"�� �� �� ��");
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}

	ucRet = CheckPSamCard();
	if (ucRet != 0)
	{
		lcdCls();
		lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"����PSAM��");
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}

	// ���ѳ�ʼ��
	memset((uint8_t*)&stApduSend, 0x00, sizeof(stApduSend));
	memset((uint8_t*)&stApduResp, 0x00, sizeof(stApduResp));
	stApduSend.CLA = 0x80;
	stApduSend.INS = 0x50;
	stApduSend.P1 = 0x01;
	if (PosCom.ucEdOrEpFlag)
	{
		stApduSend.P2 = 0x02;
	}
	else
	{
		stApduSend.P2 = 0x01;
	}
	stApduSend.Lc = 0x0B;
	stApduSend.DataIn[0] = PosCom.stTrans.ucKeyIndex;   // ������Կ����
	uiAmount = PubBcd2Long(PosCom.stTrans.sAmount, 6, NULL); // ���ѽ��
	PubLong2Char(uiAmount,4,(char*)&stApduSend.DataIn[1]);
	memcpy(&stApduSend.DataIn[5], PosCom.stTrans.szPsamTid, 6);	// �ն˱��	
	stApduSend.Le = 0x0F;
	stApduSend.LeFlag = 0x01;
	iRet = iccExchangeAPDU(icc_fd, &stApduSend, &stApduResp);
	if(iRet != 0
		|| stApduResp.SWA != 0x90
		|| stApduResp.SWB != 0x00
		|| stApduResp.LenOut != 0x0F)
	{
		lcdCls();
		lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"%02x%02x ��ʼ����",stApduResp.SWA,stApduResp.SWB);
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
	memcpy(PosCom.stTrans.sBalanceAmount, stApduResp.DataOut, 4);			// �������
	memcpy(PosCom.stTrans.sIcTxnSeq, stApduResp.DataOut+4, 2);		// ���ڽ������к�
//	memcpy(over_amount, stApduResp.DataOut+6, 3);			// ͸֧�޶�
	PosCom.stTrans.ucKeyVersion = stApduResp.DataOut[9];						// DPK��Կ�汾 
	PosCom.stTrans.ucKeyAlgFlag = stApduResp.DataOut[10];							// DPK��Կ�㷨��ʶ
	memcpy(PosCom.stTrans.sICRandom, stApduResp.DataOut+11, 4);			// α�����

	// psam �� MAC1����	
	memset((uint8_t*)&stApduSend, 0x00, sizeof(stApduSend));
	memset((uint8_t*)&stApduResp, 0x00, sizeof(stApduResp));
	stApduSend.CLA = 0x80;
	stApduSend.INS = 0x70;
	stApduSend.P1 = 0x00;
	stApduSend.P2 = 0x00;
	stApduSend.Lc = 0x1C;
	memcpy(stApduSend.DataIn, PosCom.stTrans.sICRandom, 4);		// IC��α�����
	memcpy(stApduSend.DataIn+4, PosCom.stTrans.sIcTxnSeq, 2);// �������к�
	PubLong2Char(uiAmount,4,(char*)&stApduSend.DataIn[6]);
	stApduSend.DataIn[10] = 0x06;					// �������ͱ�ʶ
//	stApduSend.DataIn[11] = DPK_Version;				// ��Կ�汾��
	PubAsc2Bcd((char*)PosCom.stTrans.szDate,8,(char*)stApduSend.DataIn+11); // �ն˽���������ʱ��
	PubAsc2Bcd((char*)PosCom.stTrans.szTime,6,(char*)stApduSend.DataIn+15); // �ն˽���������ʱ��
	stApduSend.DataIn[18] = PosCom.stTrans.ucKeyVersion;
	stApduSend.DataIn[19] = PosCom.stTrans.ucKeyAlgFlag;					// ��Կ�㷨��ʶ	
//	memcpy(stApduSend.DataIn+20, gstTransInfo.stTranLog.szPan+12, 8);		// �û�����
	PubAsc2Bcd((char*)PosCom.stTrans.szCardNo,16,(char*)stApduSend.DataIn+20);
	stApduSend.Le = 0x08;
	stApduSend.LeFlag = 0x01;
	iRet = iccExchangeAPDU(iPsam_fd, &stApduSend, &stApduResp);
	if(iRet != 0
		|| stApduResp.SWA != 0x90
		|| stApduResp.SWB != 0x00
		|| stApduResp.LenOut != 0x08)
	{
		lcdCls();
		lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"%02x%02x ȡMAC1��",stApduResp.SWA,stApduResp.SWB);
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
	memcpy(PosCom.stTrans.sPsamTxnSeq, stApduResp.DataOut, 4);		// �ն��ѻ��������к�
	memcpy(PosCom.stTrans.sMac1, stApduResp.DataOut+4, 4);		// MAC1ֵ 

	// �û�������,��ȡTAC��MAC2
	memset((uint8_t*)&stApduSend, 0x00, sizeof(stApduSend));
	memset((uint8_t*)&stApduResp, 0x00, sizeof(stApduResp));
	stApduSend.CLA = 0x80;
	stApduSend.INS = 0x54;
	stApduSend.P1 = 0x01;
	stApduSend.P2 = 0x00;
	stApduSend.Lc = 0x0F;
	memcpy(stApduSend.DataIn, PosCom.stTrans.sPsamTxnSeq, 4);		// �ն˽������к�
	PubAsc2Bcd((char*)PosCom.stTrans.szDate,8,(char*)stApduSend.DataIn+4); // �ն˽���������ʱ��
	PubAsc2Bcd((char*)PosCom.stTrans.szTime,6,(char*)stApduSend.DataIn+8); // �ն˽���������ʱ��
	memcpy(stApduSend.DataIn+11, PosCom.stTrans.sMac1, 4);
	stApduSend.Le = 0x08;
	stApduSend.LeFlag = 0x01;
	iRet = iccExchangeAPDU(icc_fd, &stApduSend, &stApduResp);
	if(iRet != 0
		|| stApduResp.SWA != 0x90
		|| stApduResp.SWB != 0x00
		|| stApduResp.LenOut != 0x08)
	{
		lcdCls();
		lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"%02x%02x ȡMAC2��",stApduResp.SWA,stApduResp.SWB);
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
	memcpy(PosCom.stTrans.sTac, stApduResp.DataOut, 4); // TAC
	memcpy(PosCom.stTrans.sMac2, stApduResp.DataOut+4, 4); // MAC2

	// PSAM������,��֤MAC2
	memset((uint8_t*)&stApduSend, 0x00, sizeof(stApduSend));
	memset((uint8_t*)&stApduResp, 0x00, sizeof(stApduResp));
	stApduSend.CLA = 0x80;
	stApduSend.INS = 0x72;
	stApduSend.P1 = 0x00;
	stApduSend.P2 = 0x00;
	stApduSend.Lc = 0x04;
	memcpy(stApduSend.DataIn, PosCom.stTrans.sMac2, 4);		// MAC2
	stApduSend.Le = 0x00;
	stApduSend.LeFlag = 0x00;
	iRet = iccExchangeAPDU(iPsam_fd, &stApduSend, &stApduResp);
	if(iRet != 0
		|| stApduResp.SWA != 0x90
		|| stApduResp.SWB != 0x00)
	{
		lcdCls();
		lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"%02x%02x ��֤MAC��",stApduResp.SWA,stApduResp.SWB);
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
		
	strcpy((char *)PosCom.szRespCode, "00"); // ������
	ucRet = AfterCommProc();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;	
}

// ����Ǯ����ֵ
uint8_t TransEdTopup(int iTransType)
{
	uint8_t ucRet;
	uint32_t uiAmount;
	APDU_SEND stApduSend;
	APDU_RESP stApduResp;  
	int   iRet, iDatalen;
	char   szProcCode[7];

	if (iTransType == ED_TOPUP_SPEC)
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  ָ���˻�Ȧ��  ", "      SALE      ");
	}
	else if (iTransType == ED_TOPUP_NORAML)
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " ��ָ���˻�Ȧ�� ", "      SALE      ");
	}
	else
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "����Ǯ���ֽ��ֵ", "      SALE      ");
	}

	if (iTransType == ED_TOPUP_NORAML)
	{
		ucRet = PosGetCard(CARD_SWIPED);
		if( ucRet!=OK )
		{
			return ucRet;
		}
		
		strcpy((char*)PosCom.stTrans.szTranferCardNo,(char*)PosCom.stTrans.szCardNo);
		PosCom.ucSwipedFlag = NO_SWIPE_INSERT;
		PosCom.stTrans.ucFallBack = FALSE;
	}

	ucRet = PosGetEdCard(CARD_INSERTED);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucRet = AppGetAmount(9, TRAN_AMOUNT);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	PosCom.bIsFirstGAC = TRUE;
	PosCom.ucFallBack = FALSE;
	PosCom.stTrans.ucKeyIndex = 0x01;
	while(PosCom.bIsFirstGAC)
	{
		ucRet = CheckPSamCard();
		if (ucRet != 0)
		{
			lcdCls();
			lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
			lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"����PSAM��");
			lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
			PubBeepErr();
			PubWaitKey(5);
			return NO_DISP;
		}

		// ��ֵ��ʼ��
		memset((uint8_t*)&stApduSend, 0x00, sizeof(stApduSend));
		memset((uint8_t*)&stApduResp, 0x00, sizeof(stApduResp));
		stApduSend.CLA = 0x80;
		stApduSend.INS = 0x50;
		stApduSend.P1 = 0x00;
		if (PosCom.ucEdOrEpFlag)
			stApduSend.P2 = 0x02;
		else
			stApduSend.P2 = 0x01;
		stApduSend.Lc = 0x0B;
		stApduSend.DataIn[0] = PosCom.stTrans.ucKeyIndex;   // ��Կ����
		uiAmount = PubBcd2Long(PosCom.stTrans.sAmount, 6, NULL); // ���ѽ��
		PubLong2Char(uiAmount,4,(char*)&stApduSend.DataIn[1]);
		memcpy(&stApduSend.DataIn[5], PosCom.stTrans.sPsamTid, 6);	// �ն˱��	
		stApduSend.Le = 0x10;
		stApduSend.LeFlag = 0x01;
		iRet = iccExchangeAPDU(icc_fd, &stApduSend, &stApduResp);
		if(iRet != 0
			|| stApduResp.SWA != 0x90
			|| stApduResp.SWB != 0x00
			|| stApduResp.LenOut != 0x0F)
		{
			lcdCls();
			lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
			lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"%02x%02x ��ʼ����",stApduResp.SWA,stApduResp.SWB);
			lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
			PubBeepErr();
			PubWaitKey(5);
			return NO_DISP;
		}
		memcpy(PosCom.stTrans.sBalanceAmount, stApduResp.DataOut, 4);			// �������
		memcpy(PosCom.stTrans.sIcTxnSeq, stApduResp.DataOut+4, 2);		// ���ڽ������к�	
		PosCom.stTrans.ucKeyVersion = stApduResp.DataOut[6];						// DPK��Կ�汾 
		PosCom.stTrans.ucKeyAlgFlag = stApduResp.DataOut[7];							// DPK��Կ�㷨��ʶ
		memcpy(PosCom.stTrans.sICRandom, stApduResp.DataOut+8, 4);				// α�����
		memcpy(PosCom.stTrans.sMac1, stApduResp.DataOut+12, 4);					// MAC1

		// ��������
		memset(szProcCode,0,sizeof(szProcCode));
		if (iTransType == ED_TOPUP_CASH)
		{
			strcpy(szProcCode,"630000");
		}
		else if (iTransType == ED_TOPUP_NORAML)
		{
			strcpy(szProcCode,"620000");
		}
		else
			strcpy(szProcCode,"600000");

		SetCommReqField((uint8_t *)"0200", (uint8_t *)szProcCode);

		// �Ƚ�����������������
		SendScriptResult();
		ReverseTranProc();
		
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "��������������", "CONNECT BANK");
		ucRet = SendRecvPacket();
		if( ucRet!=OK )
		{
			UpdateReversalFile(ucRet); // ���¼�¼��ԭ�����ļ�				
			return ucRet;
		}
		
		// ��ʾ���׽��
		if( memcmp(PosCom.szRespCode, "00", 2) !=0 )
		{
			fileRemove(REVERSAL_FILE);   // ʧ������ɾ������
			if (PosCom.bIsFirstGAC && memcmp(PosCom.szRespCode, "01", 2)==0 
				&& glRecvPack.szEPData[22] != 99)
			{
				PosCom.bIsFirstGAC = FALSE;
				PosCom.ucFallBack = TRUE; 
				PosCom.stTrans.ucKeyIndex = glRecvPack.szEPData[22];
				continue;
			}
			else
			{
				fileRemove(REVERSAL_FILE);   // ͨѶ�ɹ�
				dispRetCode();
				return NO_DISP;
			}
		}

		// �ɹ�����к�̴���,
		iDatalen = PubChar2Long(glRecvPack.szEPData,2,NULL);
		if (iDatalen < 40 || memcmp(glRecvPack.szEPData,"RP",2) != 0)
		{
			lcdCls();
			lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
			lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"MAC2��������");
			lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
			PubBeepErr();
			PubWaitKey(5);
			return NO_DISP;
		}		
		memcpy(PosCom.stTrans.sMac2,&glRecvPack.szEPData[48],4);

		// �û�������,��ɳ�ֵ����
		memset((uint8_t*)&stApduSend, 0x00, sizeof(stApduSend));
		memset((uint8_t*)&stApduResp, 0x00, sizeof(stApduResp));
		stApduSend.CLA = 0x80;
		stApduSend.INS = 0x52;
		stApduSend.P1 = 0x00;
		stApduSend.P2 = 0x00;
		stApduSend.Lc = 0x0B;
		PubAsc2Bcd((char*)PosCom.stTrans.szDate,8,(char*)stApduSend.DataIn+4); // �ն˽���������ʱ��
		PubAsc2Bcd((char*)PosCom.stTrans.szTime,6,(char*)stApduSend.DataIn+8); // �ն˽���������ʱ��
		memcpy(stApduSend.DataIn+7, PosCom.stTrans.sMac2, 4);	// MAC2
		stApduSend.Le = 0x04;
		stApduSend.LeFlag = 0x01;
		iRet = iccExchangeAPDU(icc_fd, &stApduSend, &stApduResp);
		if(iRet != 0
			|| stApduResp.SWA != 0x90
			|| stApduResp.SWB != 0x00
			|| stApduResp.LenOut != 0x04)
		{
			lcdCls();
			lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
			lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"%02x%02x ����TAC��",stApduResp.SWA,stApduResp.SWB);
			lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
			PubBeepErr();
			PubWaitKey(5);
			return NO_DISP;
		}
		if (memcmp(stApduResp.DataOut,"\x00\x00\x00\x00",4) == 0)
		{
			lcdCls();
			lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
			lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"%02x%02x ����TAC��",stApduResp.SWA,stApduResp.SWB);
			lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
			PubBeepErr();
			PubWaitKey(5);
			return NO_DISP;
		}
		memcpy(PosCom.stTrans.sTac, stApduResp.DataOut, 4);	// TAC

		fileRemove(REVERSAL_FILE);
		return AfterCommProc();  // �ɹ�
	}
	
	return NO_DISP;		
}

// ����Ǯ�����
uint8_t TransEdBalance(void)
{
	uint8_t ucRet;
	uint32_t uiAmount;
	char    szBuff[40],sBalance[20], szBuf[40]; 
	int   iLen;
	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "����Ǯ������ѯ", "      SALE      ");
	ucRet = PosGetEdCard(CARD_INSERTED);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	lcdCls();
	lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,"����Ǯ������ѯ");
	lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"��ѯ�ɹ�");
	lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��鿴�������");
	lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"���ο����ÿ�");
	
	// �����˽�������ı�־֮�����в���ʵ��
	uiAmount = PubChar2Long(PosCom.stTrans.sBalanceAmount,4,NULL);
	memset(sBalance,0,sizeof(sBalance));
	memset(szBuff,0,sizeof(szBuff));
	PubLong2Bcd(uiAmount,6,sBalance);
	szBuff[0] = ' ';
	ConvBcdAmount((uint8_t*)sBalance, (uint8_t*)&szBuff[1]);
	
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_MEDIACY|DISP_CFONT, "    ��ѯ�ɹ�    ", " QUERY SUCCESS  ");
	DispMulLanguageString(0, 4, DISP_CFONT, "�������:", "BALANCE:");
	DispMulLanguageString(0, 6, DISP_CLRLINE|DISP_CFONT, " ��鿴������� ", " LOOK PINPAD    ");
	memset(szBuf, 0x20, 16);
	iLen = strlen((char *)szBuff);
	memcpy(&szBuf[16-iLen-1], szBuff, iLen);
	szBuf[15] = 0;
	lcdDisplay(0, 6, DISP_CFONT, (char *)szBuf);
	PubBeepErr();	
	
	kbGetKeyMs(10000);		// �ȴ�4����
	
	return (OK);
}

// ����Ǯ����ѯ��¼
uint8_t TransEdViewRecord(void)
{
	uint8_t ucRet;
	int   iRet, iLen, iCurRec;
	APDU_SEND stApduSend;
	APDU_RESP stApduResp;
	uint8_t     szBuffer[50], szAmout[20];
	uint8_t     szDataTime[30], szTxnAtc[4], szMerchantName[30], szTxnType[4];	
	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "����Ǯ����ѯ��¼", "      SALE      ");
	ucRet = PosGetEdCard(CARD_INSERTED);
	if( ucRet!=OK )
	{
		return ucRet;
	}	

	iCurRec = 1;
	while(1)
	{
		memset((uint8_t*)&stApduSend, 0x00, sizeof(stApduSend));
		memset((uint8_t*)&stApduResp, 0x00, sizeof(stApduResp));
		stApduSend.CLA = 0x00;
		stApduSend.INS = 0xB2;
		stApduSend.P1 = iCurRec;
		stApduSend.P2 = 0x1D;  //0x18+0x04;
		stApduSend.Lc = 0x00;
		stApduSend.Le = 0x00;
		stApduSend.LeFlag = 0x01;
		iRet = iccExchangeAPDU(icc_fd, &stApduSend, &stApduResp);
		if(iRet != 0
			|| stApduResp.SWA != 0x90
			|| stApduResp.SWB != 0x00)
		{
			lcdClrLine(2,7);
			lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"��¼β");
			PubWaitKey(3);
			iCurRec = 1;
			return NO_DISP;
		}

		iCurRec++;
		
		memset(szBuffer,0,sizeof(szBuffer));
		memset(szAmout,0,sizeof(szAmout));
		memset(szDataTime,0,sizeof(szDataTime));
		memset(szTxnAtc,0,sizeof(szTxnAtc));
		memset(szMerchantName,0,sizeof(szMerchantName));
		memset(szTxnType,0,sizeof(szTxnType));
		// ����
		memcpy(szTxnAtc,stApduResp.DataOut,2);
		// ���
		memcpy(szAmout,stApduResp.DataOut+2,4);
		// ��������
		szTxnType[0] = stApduResp.DataOut[6];
		// �ն˱��
		memcpy(szMerchantName,stApduResp.DataOut+7,6);
		// ʱ��
		memset(szBuffer,0x00,sizeof(szBuffer));
		memcpy(szBuffer,stApduResp.DataOut+13,7);
		PubBcd2Asc0(szBuffer,7,(char*)szDataTime);
		
		lcdClrLine(2,7);
		memset(szBuffer,0x00,sizeof(szBuffer));
		PubConvAmount(NULL,szAmout,2,szBuffer,0);
		lcdDisplay(0,2,DISP_CFONT,"���:%s",szBuffer);
		memset(szBuffer,0x00,sizeof(szBuffer));
		iLen = PubChar2Long(szTxnAtc,2,NULL);
		switch (szTxnType[0])
		{
		case 0x01:
			sprintf((char*)szBuffer,"��������:ȡ��");
			break;
		case 0x09:
			sprintf((char*)szBuffer,"��������:����");
			break;
		default:
			sprintf((char*)szBuffer,"��������:����");
			break;
		}
		lcdDisplay(0,4,DISP_CFONT,(char*)szBuffer);
		lcdDisplay(0,6,DISP_ASCII,"ATC: %d",iLen);
		lcdDisplay(0,7, DISP_ASCII, "%4.4s/%2.2s/%2.2s %2.2s:%2.2s:%2.2s", szDataTime,
		szDataTime+4,szDataTime+6,szDataTime+8,szDataTime+10,szDataTime+12);

		iRet = PubWaitKey(60);
		if( iRet==KEY_CANCEL || iRet==KEY_INVALID )
		{
			break;
		}
		else if( iRet==KEY_DOWN || iRet== KEY_ENTER )
		{
			continue;
		}
		else if( iRet==KEY_UP || iRet==KEY_CLEAR )
		{
			if (iCurRec > 1)
				iCurRec -= 1;
			else
				iCurRec = 1;
		}
		else
		{
			PubBeepErr();
		}	
	}

	return 0;
}

// ���ڸ���
uint8_t TransInstallment(void)
{
	uint8_t ucRet;
	int     iRet;
	char    szBuffer[100];

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ���ڸ���    ", "   INSTALLMENT  ");
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

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ���ڸ���    ", "   INSTALLMENT  ");
	memset(szBuffer,0,sizeof(szBuffer));	
	DispMulLanguageString(0, 2, DISP_CFONT, "�������������:", "INPUT INSTAL NO:");
/*	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_BIGFONT+KB_EN_NUM+KB_EN_CHAR+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 1, 2, 
		(stPosParam.ucOprtLimitTime*1000), (char *)szBuffer);*/
	iRet = PubGetString(NUM_IN,1,2,(uint8_t*)szBuffer,stPosParam.ucOprtLimitTime);
	if( iRet != 0 )	
		return (E_TRANS_CANCEL);
	
	PosCom.stTrans.ucInstallmentNum = atoi(szBuffer);

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ���ڸ���    ", "   INSTALLMENT  ");
	memset(szBuffer,0,sizeof(szBuffer));	
	DispMulLanguageString(0, 2, DISP_CFONT, "������", "INPUT INSTAL NO:");
	DispMulLanguageString(0, 4, DISP_CFONT, "��Ʒ��Ŀ����:", "INPUT INSTAL NO:");
/*	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_BIGFONT+KB_EN_NUM+KB_EN_CHAR+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 1, 30, 
		(stPosParam.ucOprtLimitTime*1000), (char *)szBuffer);*/
	iRet = PubGetString(NUM_IN|ALPHA_IN,1,30,(uint8_t*)szBuffer,stPosParam.ucOprtLimitTime);
	if( iRet != 0 )	
		return (E_TRANS_CANCEL);
	strcpy((char*)PosCom.stTrans.szInstalCode,szBuffer);

	ucRet = 1;
	while(ucRet)
	{
		lcdCls();
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " ������֧����ʽ ", "   INSTALLMENT  ");
		lcdDisplay(0,2,DISP_CFONT,"1.һ����֧��");
		lcdDisplay(0,4,DISP_CFONT,"2.����֧��");
		lcdDisplay(0,6,DISP_CFONT,"��ѡ��1��2");
		iRet = PubWaitKey(stPosParam.ucOprtLimitTime);
		switch(iRet)
		{
		case KEY1: 
			PosCom.stTrans.ucInstalPayMode = 0; 
			ucRet = 0;
			break;
		case KEY2: 
			PosCom.stTrans.ucInstalPayMode = 1; 
			ucRet = 0;
			break;
		case KEY_CANCEL: 
			return NO_DISP;
		default:
			PubBeepErr();
			break;
		}
	}
	
	ucRet = EnterPIN(0);
	if( ucRet!=OK )
	{
		if( ucRet==PED_RET_NOKEY )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 2, DISP_CFONT, "��������ǩ��", "LOGON ONLINE");
			kbGetKey();
		}
		return ucRet;
	}
	
	SetCommReqField((uint8_t *)"0200", (uint8_t *)"000000");
	ShortToByte(glSendPack.sField62, 62);	// �����Ƶ���,ͷ2���ֽڴ�����ݳ���
	memset((char *)&glSendPack.sField62[2],' ',62);
	sprintf((char *)&glSendPack.sField62[2], "%02d", PosCom.stTrans.ucInstallmentNum);
	memcpy((char *)&glSendPack.sField62[4],PosCom.stTrans.szInstalCode,strlen((char*)PosCom.stTrans.szInstalCode));
	sprintf(szBuffer,"1%d",PosCom.stTrans.ucInstalPayMode);
	memcpy((char *)&glSendPack.sField62[34],szBuffer,strlen(szBuffer));
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;	
}

// ���ڸ����
uint8_t TransVoidInstal(void)
{
	uint8_t	ucRet, ucSwipeFlag;
	
	if( stPosParam.ucEmvSupport==PARAM_OPEN )	
		ucSwipeFlag = CARD_SWIPED|CARD_INSERTED|0x80;
	else
		ucSwipeFlag = CARD_SWIPED|0x80;
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "���ڸ������ѳ���", "VOID INSTALLMENT");
	
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
	if(PosCom.stTrans.iOldTransNo != POS_INSTALLMENT )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "ԭ���׷Ƿ��ڸ���", "NOT PREAUTH");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "    ���ܳ���    ", "NOT VOID");
		ErrorBeep();
		kbGetKeyMs(3000);	
		return NO_DISP;
	}
	
	if( stTemp.iTransNo==POS_VOID_INSTAL && stPosParam.ucVoidSwipe!=PARAM_OPEN)
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
	
	ucRet = EnterPIN(0);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	// ���ý�����������
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
	sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d", PosCom.stTrans.lOldBatchNumber,
		PosCom.stTrans.lOldTraceNo);
	
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;	
}

// ��������
uint8_t TransLoyalt(int iTransType)
{
	uint8_t ucRet;
	int     iRet;
	char    szBuffer[100];
	
	lcdCls();
	if (iTransType == POS_LOYATL_ISSUER)
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " �����л������� ", "   INSTALLMENT  ");
	}
	else
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  ���˻�������  ", "   INSTALLMENT  ");
	}
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
		
	lcdCls();
	if (iTransType == POS_LOYATL_ISSUER)
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " �����л������� ", "   INSTALLMENT  ");
	}
	else
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  ���˻�������  ", "   INSTALLMENT  ");
	}
	memset(szBuffer,0,sizeof(szBuffer));	
	DispMulLanguageString(0, 2, DISP_CFONT, "������", "INPUT INSTAL NO:");
	DispMulLanguageString(0, 4, DISP_CFONT, "��Ʒ��Ŀ����:", "INPUT INSTAL NO:");
/*	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_BIGFONT+KB_EN_NUM+KB_EN_CHAR+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 1, 30, 
		(stPosParam.ucOprtLimitTime*1000), (char *)szBuffer);*/
	iRet = PubGetString(NUM_IN|ALPHA_IN,1,30,(uint8_t*)szBuffer,stPosParam.ucOprtLimitTime);
	if( iRet != 0 )	
		return (E_TRANS_CANCEL);
	strcpy((char*)PosCom.stTrans.szInstalCode,szBuffer);
		
	ucRet = EnterPIN(0);
	if( ucRet!=OK )
	{
		if( ucRet==PED_RET_NOKEY )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 2, DISP_CFONT, "��������ǩ��", "LOGON ONLINE");
			kbGetKey();
		}
		return ucRet;
	}
	
	SetCommReqField((uint8_t *)"0200", (uint8_t *)"000000");
	ShortToByte(glSendPack.sField62, 62);	// �����Ƶ���,ͷ2���ֽڴ�����ݳ���
	memset((char *)&glSendPack.sField62[2],' ',60);
	memcpy((char *)&glSendPack.sField62[2],PosCom.stTrans.szInstalCode,strlen((char*)PosCom.stTrans.szInstalCode));
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;		
}

// ���ֳ���
uint8_t TransVoidLoyalt(int iTransType)
{
	uint8_t	ucRet, ucSwipeFlag;
	
	if( stPosParam.ucEmvSupport==PARAM_OPEN )	
		ucSwipeFlag = CARD_SWIPED|CARD_INSERTED|0x80;
	else
		ucSwipeFlag = CARD_SWIPED|0x80;
	lcdCls();
	if (iTransType == POS_VOID_LOYATL_ISSUER)
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " �����л��ֳ��� ", "VOID INSTALLMENT");
	}
	else
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  ���˻��ֳ���  ", "VOID INSTALLMENT");
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
	if( stTemp.iTransNo==POS_VOID_LOYATL_ISSUER && PosCom.stTrans.iOldTransNo!=POS_LOYATL_ISSUER )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "ԭ���׷ǻ�������", "NOT SALE");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "    ���ܳ���    ", "NOT VOID");
		ErrorBeep();
		kbGetKeyMs(3000);	
		return NO_DISP;
	}
	if( stTemp.iTransNo==POS_VOID_LOYATL_ALL && PosCom.stTrans.iOldTransNo!=POS_LOYATL_ALL )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "ԭ���׷ǻ�������", "NOT SALE");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "    ���ܳ���    ", "NOT VOID");
		ErrorBeep();
		kbGetKeyMs(3000);	
		return NO_DISP;
	}
	
	if( (stTemp.iTransNo==POS_VOID_LOYATL_ISSUER && stPosParam.ucVoidSwipe!=PARAM_OPEN) ||
		(stTemp.iTransNo==POS_VOID_LOYATL_ALL && stPosParam.ucVoidSwipe!=PARAM_OPEN) )
	{
		PosCom.ucSwipedFlag = CARD_KEYIN;
	} 
	else

	
	if( stTemp.iTransNo==POS_VOID_INSTAL && stPosParam.ucVoidSwipe!=PARAM_OPEN)
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
	
	ucRet = EnterPIN(0);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	// ���ý�����������
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
	sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d", PosCom.stTrans.lOldBatchNumber,
		PosCom.stTrans.lOldTraceNo);
	
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;	
}

// ���ֲ�ѯ
uint8_t TransQueryLoyalt(void)
{
	uint8_t	ucRet;
	
	// ���������ռ�
	DispMulLanguageString(0, 0, DISP_REVERSE|DISP_MEDIACY|DISP_CFONT, "    ���ֲ�ѯ    ", "     BALANCE    ");
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
	
	// ���ý�����������
	SetCommReqField((uint8_t *)"0200", (uint8_t *)"310000");
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;	
}

// �����˻�
uint8_t TransRefundLoyalt(void)
{
	uint8_t ucRet, ucSwipeMode;
	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  ���˻����˻�  ", "     REFUND     ");
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
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  ��������Ч  ", "INVALID AMOUNT");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "   ���˻����   ", "EXLIMITED");
		ErrorBeep();
		kbGetKeyMs(2000);		
		return NO_DISP;
	}
	ucRet = AppConfirmAmount();
	if( ucRet==KEY_CANCEL )
	{
		goto REINPUT;
	}
	
	if( PosCom.stTrans.szCardUnit[0]==0 )
	{
		strcpy((char *)PosCom.stTrans.szCardUnit, "000");
	}
	PosCom.stTrans.lOldTraceNo = 0;
	PosCom.stTrans.lOldBatchNumber = 0;
	
	SetCommReqField((uint8_t *)"0220", (uint8_t *)"200000");
	sprintf((char *)glSendPack.szRRN, "%.*s", LEN_RRN, PosCom.stTrans.szSysReferNo);
	sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s", PosCom.stTrans.lOldBatchNumber,
		PosCom.stTrans.lOldTraceNo, PosCom.stTrans.szOldTxnDate);
	sprintf((char *)glSendPack.szField63, "%.3s", PosCom.stTrans.szCardUnit);
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;	
}


// �ֻ�оƬ����
uint8_t TransMobileSale(void)
{
	uint8_t ucRet;
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  �ֻ�оƬ����  ", "      SALE      ");
	ucRet = AppGetAmount(9, TRAN_AMOUNT);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	ucRet = PosGetMobileCard();
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
			DispMulLanguageString(0, 2, DISP_CFONT, "��������ǩ��", "LOGON ONLINE");
			kbGetKey();
		}
		return ucRet;
	}
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  �ֻ�оƬ����  ", "      SALE      ");

	SetCommReqField((uint8_t *)"0200", (uint8_t *)"000000");
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;	
}

// �ֻ�оƬ���ѣ�Ԥ��Ȩ��ɳ���
uint8_t TransVoidMobileSale(void)
{
	uint8_t	ucRet;
	char    szTransName[40], szBuffer[100];
	int     ilen;

	memset(szTransName,0,sizeof(szTransName));
	memset(szBuffer,0,sizeof(szBuffer));
	GetTransName(stTemp.iTransNo,(uint8_t*)szBuffer);
	ilen = strlen(szBuffer);	
	ilen = (16-ilen)/2;
	memset(szTransName,' ',16);
	memcpy(szTransName+ilen,szBuffer,strlen(szBuffer));

	lcdCls();
	lcdFlip();
	lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,"%-16.16s",szTransName);
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

	lcdCls();
	lcdFlip();
	lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,szTransName);	
	if( stTemp.iTransNo==POS_MOBILE_VOID && PosCom.stTrans.iOldTransNo!=POS_MOBILE_SALE )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "ԭ���׷��ֻ�����", "NOT SALE");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "    ���ܳ���    ", "NOT VOID");
		ErrorBeep();
		kbGetKeyMs(3000);		
		return NO_DISP;
	}
	if( stTemp.iTransNo==POS_MOBILE_VOID_AUTH && 
		(PosCom.stTrans.iOldTransNo!=POS_MOBILE_AUTH && PosCom.stTrans.iOldTransNo!=POS_MOBILE_AUTH_OFF) )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "ԭ���׷���Ȩ���", "NOT PREAUTH");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "    ���ܳ���    ", "NOT VOID");
		ErrorBeep();
		kbGetKeyMs(3000);
		
		return NO_DISP;
	}
	
	if( (stTemp.iTransNo==POS_MOBILE_VOID && stPosParam.ucVoidSwipe!=PARAM_OPEN) ||
		(stTemp.iTransNo==POS_MOBILE_VOID_AUTH && stPosParam.ucAuthVoidSwipe!=PARAM_OPEN) )
	{
		PosCom.ucSwipedFlag = CARD_KEYIN;
	} 
	else
	{
		ucRet = PosGetMobileCard();
		if( ucRet!=OK )
		{
			return ucRet;
		}
	}	
	
	PosCom.ucPinEntry = FALSE;
	ucRet = EnterPIN(0);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	lcdCls();
	lcdFlip();
	lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,szTransName);

	// ���ý�����������
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
	if( stTemp.iTransNo==POS_MOBILE_VOID )
	{
// 		sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s", PosCom.stTrans.lOldBatchNumber,
// 			PosCom.stTrans.lOldTraceNo, PosCom.stTrans.szOldTxnDate);
		sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d", PosCom.stTrans.lOldBatchNumber,
			PosCom.stTrans.lOldTraceNo);
	}
	else
	{
		sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s", PosCom.stTrans.lOldBatchNumber,
			PosCom.stTrans.lOldTraceNo, PosCom.stTrans.szOldTxnDate);
	}
	
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;		
}

// �ֻ�оƬԤ��Ȩ
uint8_t TransMobilePreAuth(void)
{
	uint8_t	ucRet;
	char    szTransName[40], szBuffer[100];
	int     ilen;

	memset(szTransName,0,sizeof(szTransName));
	memset(szBuffer,0,sizeof(szBuffer));
	GetTransName(stTemp.iTransNo,(uint8_t*)szBuffer);
	ilen = strlen(szBuffer);	
	ilen = (16-ilen)/2;
	memset(szTransName,' ',16);
	memcpy(szTransName+ilen,szBuffer,strlen(szBuffer));
	
	lcdCls();
	lcdFlip();
	lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,"%-16.16s",szTransName);
	
	ucRet = AppGetAmount(9, TRAN_AMOUNT);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	ucRet = PosGetMobileCard();
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
			DispMulLanguageString(0, 2, DISP_CFONT, "��������ǩ��", "LOGON ONLINE");
			kbGetKey();
		}
		return ucRet;
	}
	
	lcdCls();
	lcdFlip();
	lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,szTransName);	
	// ���ý�����������
	SetCommReqField((uint8_t *)"0100", (uint8_t *)"030000");	
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;
}

// �ֻ�оƬԤ��Ȩ�������,֪ͨ
uint8_t TransMobileAuth(void)
{
	uint8_t ucRet;	
	char    szTransName[40], szBuffer[100];
	int     ilen;

	memset(szTransName,0,sizeof(szTransName));
	memset(szBuffer,0,sizeof(szBuffer));
	GetTransName(stTemp.iTransNo,(uint8_t*)szBuffer);
	ilen = strlen(szBuffer);	
	ilen = (16-ilen)/2;
	memset(szTransName,' ',16);
	memcpy(szTransName+ilen,szBuffer,strlen(szBuffer));
	
	lcdCls();
	lcdFlip();
	lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,"%-16.16s",szTransName);

	ucRet = PosGetMobileCard();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	lcdCls();
	lcdFlip();
	lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,szTransName);
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
	
	if( stTemp.iTransNo!=POS_MOBILE_AUTH )
	{
		ucRet = AppConfirmAmount();
		if( ucRet==KEY_CANCEL )
		{
			goto REINPUT1;
		}
	}

	ucRet = EnterPIN(0);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	PosCom.stTrans.lOldBatchNumber = 0;
	PosCom.stTrans.lOldTraceNo = 0;
	
	// ���ý�����������
	if (stTemp.iTransNo == POS_MOBILE_AUTH)
	{
		SetCommReqField((uint8_t *)"0200", (uint8_t *)"000000");
	}
	else
	{
		SetCommReqField((uint8_t *)"0220", (uint8_t *)"000000");
	}
	sprintf((char *)glSendPack.szAuthCode, "%.*s", LEN_AUTH_CODE, PosCom.stTrans.szAuthNo);
	sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s", PosCom.stTrans.lOldBatchNumber,
		PosCom.stTrans.lOldTraceNo, PosCom.stTrans.szOldTxnDate);
	
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;
}

// �ֻ�оƬԤ��Ȩ����
uint8_t TransMobileVoidPreAuth(void)
{
	uint8_t ucRet;
	char    szTransName[40], szBuffer[100];
	int     ilen;

	memset(szTransName,0,sizeof(szTransName));
	memset(szBuffer,0,sizeof(szBuffer));
	GetTransName(stTemp.iTransNo,(uint8_t*)szBuffer);
	ilen = strlen(szBuffer);	
	ilen = (16-ilen)/2;
	memset(szTransName,' ',16);
	memcpy(szTransName+ilen,szBuffer,strlen(szBuffer));
	
	lcdCls();
	lcdFlip();
	lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,"%-16.16s",szTransName);
	ucRet = CheckSupPwd();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	lcdCls();
	lcdFlip();
	lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,szTransName);
	ucRet = PosGetMobileCard();
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
	
	SetCommReqField((uint8_t *)"0100", (uint8_t *)"200000");
	sprintf((char *)glSendPack.szAuthCode, "%.6s", PosCom.stTrans.szAuthNo);
	sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s", PosCom.stTrans.lOldBatchNumber,
		PosCom.stTrans.lOldTraceNo, PosCom.stTrans.szOldTxnDate);
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;

}

// �ֻ�оƬ�˻�
uint8_t TransMobileRefund(void)
{
	uint8_t ucRet;
	char    szTransName[40], szBuffer[100];
	int     ilen;

	memset(szTransName,0,sizeof(szTransName));
	memset(szBuffer,0,sizeof(szBuffer));
	GetTransName(stTemp.iTransNo,(uint8_t*)szBuffer);
	ilen = strlen(szBuffer);	
	ilen = (16-ilen)/2;
	memset(szTransName,' ',16);
	memcpy(szTransName+ilen,szBuffer,strlen(szBuffer));
	
	lcdCls();
	lcdFlip();
	lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,"%-16.16s",szTransName);	
	ucRet = CheckSupPwd();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	lcdCls();
	lcdFlip();
	lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,szTransName);
	ucRet = PosGetMobileCard();
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
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  ��������Ч  ", "INVALID AMOUNT");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "   ���˻����   ", "EXLIMITED");
		ErrorBeep();
		kbGetKeyMs(2000);		
		return NO_DISP;
	}
	ucRet = AppConfirmAmount();
	if( ucRet==KEY_CANCEL )
	{
		goto REINPUT;
	}
	
	if( PosCom.stTrans.szCardUnit[0]==0 )
	{
		strcpy((char *)PosCom.stTrans.szCardUnit, "000");
	}
	PosCom.stTrans.lOldTraceNo = 0;
	PosCom.stTrans.lOldBatchNumber = 0;
	
	SetCommReqField((uint8_t *)"0220", (uint8_t *)"200000");
	sprintf((char *)glSendPack.szRRN, "%.*s", LEN_RRN, PosCom.stTrans.szSysReferNo);
	sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s", PosCom.stTrans.lOldBatchNumber,
		PosCom.stTrans.lOldTraceNo, PosCom.stTrans.szOldTxnDate);
	sprintf((char *)glSendPack.szField63, "%.3s", PosCom.stTrans.szCardUnit);
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;
}

// �ֻ�оƬ����ѯ
uint8_t TransMobileBalance(void)
{
	uint8_t ucRet;
	char    szTransName[40], szBuffer[100];
	int     ilen;

	memset(szTransName,0,sizeof(szTransName));
	memset(szBuffer,0,sizeof(szBuffer));
	GetTransName(stTemp.iTransNo,(uint8_t*)szBuffer);
	ilen = strlen(szBuffer);	
	ilen = (16-ilen)/2;
	memset(szTransName,' ',16);
	memcpy(szTransName+ilen,szBuffer,strlen(szBuffer));
	
	lcdCls();
	lcdFlip();
	lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,"%-16.16s",szTransName);		
	ucRet = PosGetMobileCard();
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	ucRet = EnterPIN(0);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	// ���ý�����������
	SetCommReqField((uint8_t *)"0200", (uint8_t *)"310000");
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;
}


// ����ǩ��
uint8_t TransLytLogon(void)
{
	uint8_t	ucRet;
	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " ����Ա����ǩ�� ", "      SALE      ");
	if( stPosParam.ucEmvSupport==PARAM_OPEN )
		ucRet = PosGetCard(CARD_SWIPED|CARD_INSERTED|CARD_KEYIN);
//		ucRet = PosGetCard(CARD_SWIPED|CARD_INSERTED|CARD_PASSIVE|CARD_KEYIN);
	else
		ucRet = PosGetCard(CARD_SWIPED|CARD_KEYIN);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	SetCommReqField((uint8_t *)"0820", (uint8_t *)"");
	memset(glSendPack.szPanSeqNo,0,sizeof(glSendPack.szPanSeqNo));
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "ǩ���ɹ�", "SUCCESS");
	OkBeep();	
	return NO_DISP;
}      

// end of file
