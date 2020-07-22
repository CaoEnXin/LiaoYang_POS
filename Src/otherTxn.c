
#include <openssl/sha.h>
#include "posapi.h"
#include "posmacro.h"
#include "terstruc.h"
#include "glbvar.h"
#include "tranfunclist.h"
#include "emvtranproc.h"
#include "otherTxn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

uint8_t  GetMotoDate(int iTransType);  // �������׵Ŀ������������������������


uint8_t PosGetOrderCard(uint8_t ucMode)
{
    uint8_t	ucRet;
//	int     iKey;

	if( PosCom.ucSwipedFlag!=NO_SWIPE_INSERT )
	{
		return OK;
	}

	while(1)
	{
		ShowGetCardScreen(ucMode);
		ucRet = DetectCardEvent(ucMode);
		if( ucRet==CARD_KEYIN )
		{
/*			iKey = kbGetKey();
			if( iKey==KEY_CANCEL )
			{
				return E_TRANS_CANCEL;
			}

			if (!(iKey>=KEY1 && iKey<=KEY9))
			{
				PubBeepErr();
				continue;
			}*/

			ucRet = ManualEntryCardNo(PosCom.stTrans.szCardNo);
			if( ucRet!=OK )
			{
				return ucRet;
			}

			return 0;
		}
		else
		{
			continue;
		}
	}
}

// ԤԼ����
uint8_t SelectBookFunc(void)
{
	int keySub; //�����˵����ؽ�ֵ	
	const char *Booking_menu[] = 
	{
		"1 ԤԼ����      ",
		"2 ԤԼ���ѳ���  "
	};
	
	keySub = DispMenu(Booking_menu, 2, 0, KEY1, KEY2, stPosParam.ucOprtLimitTime);
	if( keySub==KEY1 ) 
		stTemp.iTransNo = POS_RESER_SALE;
	else if( keySub==KEY2 ) 
		stTemp.iTransNo = POS_RESER_VOID;
	else 
		return NO_DISP;
	
	return OK;
}

// ����
uint8_t SelectMotoFunc(void)
{
	int keySub; //�����˵����ؽ�ֵ	
	const char *Moto_menu[] = 
	{
		"1 ����          ",
		"2 ���ѳ���      ",
		"3 �˻�          ",
		"4 Ԥ��Ȩ        ",
		"5 Ԥ��Ȩ����    ",
		"6 Ԥ��Ȩ�������",
		"7 Ԥ��Ȩ��ɳ���",
		"8 Ԥ��Ȩ���֪ͨ"
	};

	keySub = DispMenu(Moto_menu, 8, 0, KEY1, KEY8, stPosParam.ucOprtLimitTime);
	if( keySub==KEY1 ) 
		stTemp.iTransNo = POS_MOTO_SALE;
	else if( keySub==KEY2 ) 
		stTemp.iTransNo = POS_MOTO_SALE_VOID;
	else if( keySub==KEY3 ) 
		stTemp.iTransNo = POS_MOTO_REFUND;
	else if( keySub==KEY4 ) 
		stTemp.iTransNo = POS_MOTO_PREAUTH;
	else if( keySub==KEY5 ) 
		stTemp.iTransNo = POS_MOTO_PREAUTH_VOID;
	else if( keySub==KEY6 ) 
		stTemp.iTransNo = POS_MOTO_AUTH_CM;
	else if( keySub==KEY7 ) 
		stTemp.iTransNo = POS_MOTO_AUTH_VOID;
	else if( keySub==KEY8 ) 
		stTemp.iTransNo = POS_MOTO_AUTH_OFF;
	else  
		return NO_DISP;
	
	return OK;		
}

// ��������ֵ
uint8_t SelectMagTopupFunc(void)
{
	int keySub; //�����˵����ؽ�ֵ	
	// ���ڸ���˵�
	const char *Magtopup_menu[] = 
	{
		"1 �ֽ��ֵ      ",
		"2 �˻���ֵ      "
	};
	
	keySub = DispMenu(Magtopup_menu, 2, 0, KEY1, KEY2, stPosParam.ucOprtLimitTime);
	if( keySub==KEY1 ) 
		stTemp.iTransNo = POS_MAGTOPUP_CASH;
	else if( keySub==KEY2 ) 
		stTemp.iTransNo = POS_MAGTOPUP_ACCT;
	else 
		return NO_DISP;

	return OK;	
}


 // ԤԼ����
uint8_t TransBookSale(void)
{
	uint8_t ucRet;
	int     iRet;
	char    szBuffer[100],szTemp[100];
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "�ֻ��޿�ԤԼ����", "   SALE  ");
	ucRet = AppGetAmount(9, TRAN_AMOUNT);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "�ֻ��޿�ԤԼ����", "   SALE  ");
	memset(szBuffer,0,sizeof(szBuffer));	
	DispMulLanguageString(0, 2, DISP_CFONT, "�������ֻ�����:", "INPUT INSTAL NO:");
/*	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_BIGFONT+KB_EN_NUM+KB_EN_CHAR+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 1, 30, 
		(stPosParam.ucOprtLimitTime*1000), (char *)szBuffer);*/
	iRet = PubGetString(NUM_IN,4,11,(uint8_t*)szBuffer,stPosParam.ucOprtLimitTime);
	if( iRet != 0 )	
		return (E_TRANS_CANCEL);

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "�ֻ��޿�ԤԼ����", "   SALE  ");
	memset(szTemp,0,sizeof(szTemp));	
	DispMulLanguageString(0, 2, DISP_CFONT, "������ԤԼ����:", "INPUT INSTAL NO:");
/*	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_BIGFONT+KB_EN_NUM+KB_EN_CHAR+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 1, 30, 
		(stPosParam.ucOprtLimitTime*1000), (char *)szBuffer);*/
	iRet = PubGetString(NUM_IN,6,6,(uint8_t*)szTemp,stPosParam.ucOprtLimitTime);
	if( iRet != 0)	
		return (E_TRANS_CANCEL);

	memset(PosCom.stTrans.szInstalCode,0,sizeof(PosCom.stTrans.szInstalCode));
	memset(PosCom.stTrans.szInstalCode,' ',11);
	memcpy(PosCom.stTrans.szInstalCode,szBuffer,strlen(szBuffer));
	memcpy(&PosCom.stTrans.szInstalCode[11],szTemp,strlen(szTemp));
		
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
	ShortToByte(glSendPack.sField62, 22);	// �����Ƶ���,ͷ2���ֽڴ�����ݳ���
	memset((char *)&glSendPack.sField62[2],' ',22);
	memcpy((char *)&glSendPack.sField62[2], "90",2);
	memcpy((char *)&glSendPack.sField62[4], PosCom.stTrans.szInstalCode,strlen((char*)PosCom.stTrans.szInstalCode));
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;
}

// ԤԼ���ѳ���
uint8_t TransVoidBookSale(void)
{
	uint8_t	ucRet, ucSwipeFlag;
	
	if( stPosParam.ucEmvSupport==PARAM_OPEN )	
		ucSwipeFlag = CARD_SWIPED|CARD_INSERTED|0x80;
	else
		ucSwipeFlag = CARD_SWIPED|0x80;
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  ԤԼ���ѳ���  ", "VOID INSTALLMENT");
	
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
	if(PosCom.stTrans.iOldTransNo != POS_RESER_SALE )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "ԭ���׷�ԤԼ����", "NOT PREAUTH");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "    ���ܳ���    ", "NOT VOID");
		ErrorBeep();
		kbGetKeyMs(3000);	
		return NO_DISP;
	}
	
	PosCom.ucSwipedFlag = CARD_KEYIN;
/*	if( stTemp.iTransNo==POS_RESER_VOID && stPosParam.ucVoidSwipe!=PARAM_OPEN)
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
	}*/
	
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

uint8_t TransValidAcct(void)
{
	int  iTransType;
	uint8_t ucRet;
	char   szBuffer[100];

	iTransType = PosCom.stTrans.iTransNo;
	PosCom.stTrans.iTransNo = POS_QUERY_ACCT;	
	stTemp.iTransNo = POS_QUERY_ACCT;

	if (iTransType == POS_MAGTOPUP_ACCT)
	{
		memset(szBuffer,0,sizeof(szBuffer));
		strcpy(szBuffer, (char*)PosCom.stTrans.szTranferCardNo);
		strcpy((char*)PosCom.stTrans.szTranferCardNo,(char*)PosCom.stTrans.szCardNo);
		strcpy((char*)PosCom.stTrans.szCardNo,szBuffer);
	}

	// ���ý�����������
	SetCommReqField((uint8_t*)"0100", (uint8_t*)"330000");
	ShortToByte(glSendPack.sField62, 22);	// �����Ƶ���,ͷ2���ֽڴ�����ݳ���
	memset((char *)&glSendPack.sField62[2],' ',22);
	memcpy((char *)&glSendPack.sField62[2],PosCom.stTrans.szInstalCode,strlen((char*)PosCom.stTrans.szInstalCode));
	memset((char *)glSendPack.sPINData,0,sizeof(glSendPack.sPINData));
	memset((char *)glSendPack.szCaptureCode,0,sizeof(glSendPack.szCaptureCode));
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}

	if (iTransType == POS_MAGTOPUP_ACCT)
	{
		memset(szBuffer,0,sizeof(szBuffer));
		strcpy(szBuffer, (char*)PosCom.stTrans.szTranferCardNo);
		strcpy((char*)PosCom.stTrans.szTranferCardNo,(char*)PosCom.stTrans.szCardNo);
		strcpy((char*)PosCom.stTrans.szCardNo,szBuffer);
	}

	sprintf((char *)PosCom.stTrans.szSysReferNo,   "%.12s", glRecvPack.szRRN);
	PosCom.stTrans.lOldTraceNo = PosCom.stTrans.lTraceNo;
	PosCom.stTrans.lOldBatchNumber = PosCom.stTrans.lBatchNumber;
	PosCom.stTrans.iTransNo = iTransType;	
	stTemp.iTransNo = iTransType;
	PosCom.stTrans.lTraceNo     = stPosParam.lNowTraceNo;  // ��ˮ�ż�1
	return OK;
}

// �������ֽ��ֵ
uint8_t TransMagTopupCash(void)
{
	uint8_t ucRet;
	int     iRet, iAmount, iBalance, iIdType;
	char    szBuffer[100],szTemp[100],szData[100];
	const char *mainMenu[] = 
	{
		"��ѡ��֤������",
		"1 ���֤        ",
		"2 ����֤        ",
		"3 ����          ",
		"4 ����֤        ",
		"5 ̨��֤        ",
		"6 ̨��֤        ",
		"7 ʿ��֤        ",
		"8 ����֤��      "
	};
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " �������ֽ��ֵ ", "      SALE      ");
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

	while(1) 
	{	
		iIdType = DispMenu(mainMenu, 9, 1, KEY1, KEY8, stPosParam.ucOprtLimitTime);
		if (iIdType>=KEY1 && iIdType<=KEY8)
		{
			break;
		}

		return NO_DISP;
	}

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " �������ֽ��ֵ ", "      SALE      ");
	memset(szBuffer,0,sizeof(szBuffer));	
	DispMulLanguageString(0, 2, DISP_CFONT, "������֤������:", "INPUT INSTAL NO:");
	iRet = PubGetString(NUM_IN|ALPHA_IN,1,20,(uint8_t*)szBuffer,stPosParam.ucOprtLimitTime);
	if( iRet != 0 )	
		return (E_TRANS_CANCEL);

	if (iIdType==KEY8)
	{
		sprintf((char*)PosCom.stTrans.szInstalCode,"99%s",szBuffer);
	}
	else
	{
		sprintf((char*)PosCom.stTrans.szInstalCode,"%02d%s",(iIdType-KEY0),szBuffer);
	}

	ucRet = TransValidAcct();
	if (ucRet != 0)
	{
		return ucRet;
	}

	memset(szTemp,0,sizeof(szTemp));
	ConvBcdAmount(PosCom.stTrans.sAmount,(uint8_t*)szTemp);
	memset(szBuffer,0,sizeof(szBuffer));
//	PubConvAmount(NULL,&glRecvPack.szExtAmount[8],2,(uint8_t*)szBuffer,0);
	memset(szData,0,sizeof(szData));
	memcpy(szData,&glRecvPack.sField62[2],12);
	PubConvAmount(NULL,(uint8_t*)szData,2,(uint8_t*)szBuffer,0);

	iAmount = PubBcd2Long(PosCom.stTrans.sAmount,6,NULL);
//	iBalance= PubAsc2Long((char*)&glRecvPack.szExtAmount[8],12,NULL);
	iBalance= PubAsc2Long((char*)&glRecvPack.sField62[2],12,NULL);
	
	lcdCls();
	lcdFlip();
	sprintf(szData,"�ɳ���:%s", szBuffer);
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_CFONT, szData, szData);
	sprintf(szData,"��ֵ���:%s", szTemp);
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, szData,szData);

	if (iBalance >= iAmount) 
	{
		DispMulLanguageString(0, 4, DISP_CLRLINE|DISP_CFONT, "  ��ȷ�ϼ�����  ", " LOOK PINPAD    ");
	}
	else
	{
		DispMulLanguageString(0, 4, DISP_CLRLINE|DISP_CFONT, "   ��������   ", " LOOK PINPAD    ");
		DispMulLanguageString(0, 6, DISP_CLRLINE|DISP_CFONT, "    �����ؼ�    ", " LOOK PINPAD    ");
	}
	while(1)
	{
		iRet = PubWaitKey(stPosParam.ucOprtLimitTime);
		if (iRet == KEY_CANCEL)
		{
//			return E_TRANS_CANCEL;
			while(1)
			{
				lcdClrLine(2,7);
				memset(PosCom.stTrans.sAmount,0,sizeof(PosCom.stTrans.sAmount));
				ucRet = AppGetAmount(9, TRAN_AMOUNT);
				if( ucRet!=OK )
				{
					return ucRet;
				}
				
				iAmount = PubBcd2Long(PosCom.stTrans.sAmount,6,NULL);
				if (iBalance < iAmount)
				{
					lcdClrLine(2,7);
					DispMulLanguageString(0, 4, DISP_CLRLINE|DISP_CFONT, "  �����ɳ���  ", " LOOK PINPAD    ");
					DispMulLanguageString(0, 6, DISP_CLRLINE|DISP_CFONT, "   ��������   ", " LOOK PINPAD    ");
					PubWaitKey(2);
					continue;
				}
				
				break;
			}
			
			break;
		}
		
		if (iRet == KEY_ENTER)
		{
			if (iBalance >= iAmount) 
			{
				break;
			}
		}

		PubBeepErr();
		continue;
	}
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " �������ֽ��ֵ ", "      SALE      ");
	memset(glRecvPack.szRspCode,0,sizeof(glRecvPack.szRspCode));
	SetCommReqField((uint8_t *)"0200", (uint8_t *)"630000");
	sprintf((char *)glSendPack.szRRN, "%.*s", LEN_RRN, PosCom.stTrans.szSysReferNo);
	sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d", PosCom.stTrans.lOldBatchNumber,
				PosCom.stTrans.lOldTraceNo);
	ShortToByte(glSendPack.sField62, 22);	// �����Ƶ���,ͷ2���ֽڴ�����ݳ���
	memset((char *)&glSendPack.sField62[2],' ',22);
	memcpy((char *)&glSendPack.sField62[2],PosCom.stTrans.szInstalCode,strlen((char*)PosCom.stTrans.szInstalCode));
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		if (memcmp(PosCom.szRespCode, "98", 2)==0 ||
			(PosCom.bOnlineTxn && strlen((char*)glRecvPack.szRspCode) == 0)) // �����˵���Ӧ����Ϊ�գ���û���յ�Ҳ��ȷ��
		{
			ucRet = TransmagTopupConfirm();
			if(ucRet != 0)
			{
				if (memcmp(PosCom.szRespCode, "A6", 2)==0 )
				{
					strcpy((char*)PosCom.szRespCode, "00");
					AfterCommProc();
					return 0;
				}
			}

			dispRetCode();
			return NO_DISP;
		}
		return ucRet;
	}
	
	return NO_DISP;	
}

// �������ʻ���ֵ
uint8_t TransMagTopupAcct(void)
{
	uint8_t ucRet, ucPinEntryBak;
	int     iRet, iAmount, iBalance, iIdType;
	char    szBuffer[100],szTemp[100],szData[100], szDisp[100];
	const char *mainMenu[] = 
	{
		"��ѡ��֤������",
		"1 ���֤        ",
		"2 ����֤        ",
		"3 ����          ",
		"4 ����֤        ",
		"5 ̨��֤        ",
		"6 ̨��֤        ",
		"7 ʿ��֤        ",
		"8 ����֤��      "
	};
	char   szTrack2Bak[TRACK2_LEN+1], szTrack3Bak[TRACK3_LEN+1];
	uint8_t ucSwipedFlagBak,ucSwipedFlagBak2;
	
	PosCom.stTrans.ucEcTopOldSwipeFlag = TRUE; // ���õ�һ��ˢ��ʱ�����ж�IC��
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " �������˻���ֵ ", "      SALE      ");
	ucRet = PosGetCard(CARD_SWIPED|CARD_INSERTED);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucSwipedFlagBak = PosCom.ucSwipedFlag;
	// ���ױ�������ת�����ŵ�
	memset(szTrack2Bak,0,sizeof(szTrack2Bak));
	memset(szTrack3Bak,0,sizeof(szTrack3Bak));
	strcpy(szTrack2Bak,PosCom.szTrack2);
	strcpy(szTrack3Bak,PosCom.szTrack3);
	strcpy((char*)PosCom.stTrans.szTranferCardNo,(char*)PosCom.stTrans.szCardNo);
	PosCom.ucSwipedFlag = NO_SWIPE_INSERT;
	PosCom.stTrans.ucFallBack = FALSE;
	PosCom.stTrans.ucEcTopOldSwipeFlag = FALSE; // �ڶ���ˢ������Ҫ�ж�IC��
	ucRet = PosGetCard(CARD_SWIPED);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucSwipedFlagBak2 = PosCom.ucSwipedFlag;
	memset(szBuffer,0,sizeof(szBuffer));
	strcpy(szBuffer, (char*)PosCom.stTrans.szTranferCardNo);
	strcpy((char*)PosCom.stTrans.szTranferCardNo,(char*)PosCom.stTrans.szCardNo);
	strcpy((char*)PosCom.stTrans.szCardNo,szBuffer);
	
	ucRet = AppGetAmount(9, TRAN_AMOUNT);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	while(1) 
	{	
		iIdType = DispMenu(mainMenu, 9, 1, KEY1, KEY8, stPosParam.ucOprtLimitTime);
		if (iIdType>=KEY1 && iIdType<=KEY7)
		{
			break;
		}
		
		return NO_DISP;
	}
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " �������˻���ֵ ", "      SALE      ");
	memset(szBuffer,0,sizeof(szBuffer));	
	DispMulLanguageString(0, 2, DISP_CFONT, "������֤������:", "INPUT INSTAL NO:");
	iRet = PubGetString(NUM_IN|ALPHA_IN,1,20,(uint8_t*)szBuffer,stPosParam.ucOprtLimitTime);
	if( iRet != 0 )	
		return (E_TRANS_CANCEL);

	if (iIdType==KEY8)
	{
		sprintf((char*)PosCom.stTrans.szInstalCode,"99%s",szBuffer);
	}
	else
	{
		sprintf((char*)PosCom.stTrans.szInstalCode,"%02d%s",(iIdType-KEY0),szBuffer);
	}

// 	ucRet = EnterPIN(0);
// 	if( ucRet!=OK )
// 	{
// 		return ucRet;
// 	}
	
	ucPinEntryBak = PosCom.ucPinEntry;
	PosCom.ucPinEntry = FALSE;
	ucRet = TransValidAcct();
	if (ucRet != 0)
	{
		return ucRet;
	}
	PosCom.ucPinEntry = ucPinEntryBak;	
	
	memset(szTemp,0,sizeof(szTemp));
	ConvBcdAmount(PosCom.stTrans.sAmount,(uint8_t*)szTemp);
	memset(szBuffer,0,sizeof(szBuffer));
//	PubConvAmount(NULL,&glRecvPack.szExtAmount[8],2,(uint8_t*)szBuffer,0);
	sprintf(szData,"%12.12s",&glRecvPack.sField62[2]);
	PubConvAmount(NULL,(uint8_t*)szData,2,(uint8_t*)szBuffer,0);
	
	iAmount = PubBcd2Long(PosCom.stTrans.sAmount,6,NULL);
//	iBalance= PubAsc2Long((char*)&glRecvPack.szExtAmount[8],12,NULL);
	iBalance= PubAsc2Long((char*)&glRecvPack.sField62[2],12,NULL);
	
	lcdCls();
	lcdFlip();
	sprintf(szDisp, "�ɳ���:%s", szBuffer);
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_CFONT, szDisp,szDisp);
	sprintf(szDisp, "��ֵ���:%s", szTemp);
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, szDisp,szDisp);
	
	if (iBalance >= iAmount) 
	{
		DispMulLanguageString(0, 4, DISP_CLRLINE|DISP_CFONT, "  ��ȷ�ϼ�����  ", " LOOK PINPAD    ");
	}
	else
	{
		DispMulLanguageString(0, 4, DISP_CLRLINE|DISP_CFONT, "   ��������   ", " LOOK PINPAD    ");
		DispMulLanguageString(0, 6, DISP_CLRLINE|DISP_CFONT, "    �����ؼ�    ", " LOOK PINPAD    ");
	}
	while(1)
	{
		iRet = PubWaitKey(stPosParam.ucOprtLimitTime);
		if (iRet == KEY_CANCEL)
		{
//			return E_TRANS_CANCEL;
			while(1)
			{
				lcdClrLine(2,7);
				memset(PosCom.stTrans.sAmount,0,sizeof(PosCom.stTrans.sAmount));
				ucRet = AppGetAmount(9, TRAN_AMOUNT);
				if( ucRet!=OK )
				{
					return ucRet;
				}
				
				iAmount = PubBcd2Long(PosCom.stTrans.sAmount,6,NULL);
				if (iBalance < iAmount)
				{
					lcdClrLine(2,7);
					DispMulLanguageString(0, 4, DISP_CLRLINE|DISP_CFONT, "  �����ɳ���  ", " LOOK PINPAD    ");
					DispMulLanguageString(0, 6, DISP_CLRLINE|DISP_CFONT, "   ��������   ", " LOOK PINPAD    ");
					PubWaitKey(2);
					continue;
				}
				
				break;
			}

			break;
		}
		
		if (iRet == KEY_ENTER)
		{
			if (iBalance >= iAmount) 
			{
				break;
			}
		}
		
		PubBeepErr();
		continue;
	}

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " �������˻���ֵ ", "      SALE      ");
	ucRet = EnterPIN(0);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	//����ת�����ŵ���Ϣ
	strcpy(PosCom.szTrack2,szTrack2Bak);
	strcpy(PosCom.szTrack3,szTrack3Bak);
	PosCom.ucSwipedFlag = ucSwipedFlagBak;  // �Ȱ���ת�����ķ�ʽ����
	
	SetCommReqField((uint8_t *)"0200", (uint8_t *)"400000");
	// 60.3-- 60.6 ����Ҫ
	glSendPack.szField60[8] = 0;
	PosCom.ucSwipedFlag = ucSwipedFlagBak2;  // �ٰ���ת�뿨�ķ�ʽ����
	// ��ԭ��֤������ˮ��
	sprintf((char *)glSendPack.szSTAN, "%0*d", LEN_POS_STAN,  PosCom.stTrans.lOldTraceNo);

/*	ShortToByte(glSendPack.sField62, 22);	// �����Ƶ���,ͷ2���ֽڴ�����ݳ���
	memset((char *)&glSendPack.sField62[2],' ',28);
	memcpy((char *)&glSendPack.sField62[2],PosCom.stTrans.szTranferCardNo,strlen((char*)PosCom.stTrans.szTranferCardNo));*/
	//62���ʵ�ʳ���
	ShortToByte(glSendPack.sField62, strlen((char*)PosCom.stTrans.szTranferCardNo));
	strcpy((char *)&glSendPack.sField62[2],PosCom.stTrans.szTranferCardNo);
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;		
}

// �������ֽ��ֵȷ��
uint8_t TransmagTopupConfirm(void)
{
	int  iTransType;
	uint8_t ucRet;
	uint8_t  szRespCodeBak[2+1],szIssuerBankId[9],szRecvBankId[9],szCenterId[9], szAuthNo[7],szSysReferNo[13];
	uint8_t szCardUnit[4];

	//�ȱ�����Ϣ
	sprintf((char *)szSysReferNo,   "%.12s",PosCom.stTrans.szSysReferNo);
	sprintf((char *)szAuthNo,   "%.6s",  PosCom.stTrans.szAuthNo);
	sprintf((char *)szIssuerBankId, "%.8s",  PosCom.stTrans.szIssuerBankId);
	sprintf((char *)szRecvBankId,   "%.8s",  PosCom.stTrans.szRecvBankId);
	sprintf((char *)szCenterId,     "%.8s",  PosCom.stTrans.szCenterId);

	PosCom.stTrans.iTransNo = stTemp.iTransNo;
	memcpy(&PosCom.stOrgPack, &glSendPack, sizeof(glSendPack));	// ����������
	memcpy(&BackPosCom, &PosCom, COM_SIZE);
	strcpy(szCardUnit,PosCom.stTrans.szCardUnit);  // ��ֵȷ�ϲ�����������ǰ���
	if (strlen(szCardUnit)!=0)
	{
		strcpy(szCardUnit,"CUP");
	}
	
	iTransType = PosCom.stTrans.iTransNo;
	PosCom.stTrans.iTransNo = POS_MAGTOPUP_CM;	
	stTemp.iTransNo = POS_MAGTOPUP_CM;
	
	// ���ý�����������
	SetCommReqField((uint8_t*)"0220", (uint8_t*)"630000");
	memset(glSendPack.sField62,0,sizeof(glSendPack.sField62));
	//�����ֽ��ֵȷ�ϱ�����35��36��53����ࣻ
	memset(glSendPack.szTrack2,0,sizeof(glSendPack.szTrack2));
	memset(glSendPack.szTrack3,0,sizeof(glSendPack.szTrack3));
	memset(glSendPack.szSecurityInfo,0,sizeof(glSendPack.szSecurityInfo));
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		strcpy(szRespCodeBak,PosCom.szRespCode);

//		memcpy(&PosCom, &BackPosCom, COM_SIZE);
//		memcpy(&glSendPack, &PosCom.stOrgPack, sizeof(glSendPack));	// �ָ�������
		stTemp.iTransNo = PosCom.stTrans.iTransNo;
//		PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;
		PosCom.stTrans.iTransNo = stTemp.iTransNo = iTransType;
		if (memcmp(szRespCodeBak,"A6",2)==0)
		{
			strcpy(PosCom.szRespCode,szRespCodeBak);
		}


		//�ָ���Ϣ
		if (!strlen(PosCom.stTrans.szSysReferNo))
			sprintf((char *)PosCom.stTrans.szSysReferNo,   "%.12s",szSysReferNo);
		if (!strlen(PosCom.stTrans.szAuthNo))
			sprintf((char *)PosCom.stTrans.szAuthNo,   "%.6s", szAuthNo);
		if (!strlen(PosCom.stTrans.szIssuerBankId))
			sprintf((char *)PosCom.stTrans.szIssuerBankId,   "%.8s",szIssuerBankId);
		if (!strlen(PosCom.stTrans.szRecvBankId))
			sprintf((char *)PosCom.stTrans.szRecvBankId,   "%.8s",szRecvBankId);
		if (!strlen(PosCom.stTrans.szCenterId))
			sprintf((char *)PosCom.stTrans.szCenterId,   "%.8s",szCenterId);

		strcpy(PosCom.stTrans.szCardUnit,szCardUnit);  // ��ֵȷ�ϲ�����������ԭ����

		return ucRet;
	}
	
//	memcpy(&PosCom, &BackPosCom, COM_SIZE);
//	memcpy(&glSendPack, &PosCom.stOrgPack, sizeof(glSendPack));	// �ָ�������
	stTemp.iTransNo = PosCom.stTrans.iTransNo;
//	PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;
	PosCom.stTrans.iTransNo = stTemp.iTransNo = iTransType;

	//�ָ���Ϣ
	if (!strlen(PosCom.stTrans.szSysReferNo))
		sprintf((char *)PosCom.stTrans.szSysReferNo,   "%.12s",szSysReferNo);
	if (!strlen(PosCom.stTrans.szAuthNo))
		sprintf((char *)PosCom.stTrans.szAuthNo,   "%.6s", szAuthNo);
	if (!strlen(PosCom.stTrans.szIssuerBankId))
		sprintf((char *)PosCom.stTrans.szIssuerBankId,   "%.8s",szIssuerBankId);
	if (!strlen(PosCom.stTrans.szRecvBankId))
		sprintf((char *)PosCom.stTrans.szRecvBankId,   "%.8s",szRecvBankId);
	if (!strlen(PosCom.stTrans.szCenterId))
		sprintf((char *)PosCom.stTrans.szCenterId,   "%.8s",szCenterId);
	
	strcpy(PosCom.stTrans.szCardUnit,szCardUnit);  // ��ֵȷ�ϲ�����������ԭ����

	return OK;
}

// ��������
uint8_t TransMotoSale(void)
{
	uint8_t ucRet;
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ��������    ", "      SALE      ");
	ucRet = AppGetAmount(9, TRAN_AMOUNT);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	ucRet = GetMotoDate(stTemp.iTransNo);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ��������    ", "      SALE      ");

	SetCommReqField((uint8_t *)"0200", (uint8_t *)"000000");
	ShortToByte(glSendPack.sField62, 22);	// �����Ƶ���,ͷ2���ֽڴ�����ݳ���
	memset((char *)&glSendPack.sField62[2],' ',22);
	memcpy((char *)&glSendPack.sField62[2], "91",2);
	memcpy((char *)&glSendPack.sField62[4], PosCom.stTrans.szInstalCode,strlen((char*)PosCom.stTrans.szInstalCode));
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;	
}

// �������ѳ���
uint8_t TransVoidMotoSale(void)
{
	uint8_t	ucRet, ucSwipeFlag;
	

	if( stPosParam.ucEmvSupport==PARAM_OPEN )	
		ucSwipeFlag = CARD_SWIPED|CARD_INSERTED|0x80;
	else
		ucSwipeFlag = CARD_SWIPED|0x80;

	if (PosCom.stTrans.iTransNo == POS_MOTO_SALE_VOID)
	{
		lcdCls();
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  �������ѳ���  ", "VOID SALE");	
	}
	else
	{
		lcdCls();
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "������Ȩ��ɳ���", "VOID SALE");	
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
	if( stTemp.iTransNo==POS_MOTO_SALE_VOID && PosCom.stTrans.iOldTransNo!=POS_MOTO_SALE )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "ԭ���׷Ƕ�������", "NOT SALE");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "    ���ܳ���    ", "NOT VOID");
		ErrorBeep();
		kbGetKeyMs(3000);		
		return NO_DISP;
	}
	if( stTemp.iTransNo==POS_MOTO_AUTH_VOID && 
		(PosCom.stTrans.iOldTransNo!=POS_MOTO_AUTH_CM && PosCom.stTrans.iOldTransNo!=POS_MOTO_AUTH_OFF) )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "ԭ���׷���Ȩ���", "NOT PREAUTH");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "    ���ܳ���    ", "NOT VOID");
		ErrorBeep();
		kbGetKeyMs(3000);
		
		return NO_DISP;
	}
	
	ucSwipeFlag = CARD_KEYIN;
	ucRet = PosGetCard(ucSwipeFlag);
	if( ucRet!=OK )
	{
		return ucRet;
	}

/*	if( (stTemp.iTransNo==POS_MOTO_SALE_VOID && stPosParam.ucVoidSwipe!=PARAM_OPEN) ||
		(stTemp.iTransNo==POS_MOTO_AUTH_VOID && stPosParam.ucAuthVoidSwipe!=PARAM_OPEN) )
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
	}*/
	
	if (PosCom.stTrans.iTransNo == POS_MOTO_SALE_VOID)
	{
		lcdCls();
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  �������ѳ���  ", "VOID SALE");	
	}
	else
	{
		lcdCls();
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "������Ȩ��ɳ���", "VOID SALE");	
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
	if( stTemp.iTransNo==POS_MOTO_SALE_VOID )
	{
// 		sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s", PosCom.stTrans.lOldBatchNumber,
// 			PosCom.stTrans.lOldTraceNo, PosCom.stTrans.szOldTxnDate);
		sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d", PosCom.stTrans.lOldBatchNumber,
			PosCom.stTrans.lOldTraceNo);
	}
	else
	{
//		sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d", PosCom.stTrans.lOldBatchNumber,
//			PosCom.stTrans.lOldTraceNo);
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

// ����Ԥ��Ȩ
uint8_t TransMotoPreAuth(void)
{
	uint8_t	ucRet;
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "   ����Ԥ��Ȩ   ", "      SALE      ");
	ucRet = AppGetAmount(9, TRAN_AMOUNT);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	ucRet = GetMotoDate(stTemp.iTransNo);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "   ����Ԥ��Ȩ   ", "      SALE      ");
	
	// ���ý�����������
	SetCommReqField((uint8_t *)"0100", (uint8_t *)"030000");
	ShortToByte(glSendPack.sField62, 22);	// �����Ƶ���,ͷ2���ֽڴ�����ݳ���
	memset((char *)&glSendPack.sField62[2],' ',22);
	memcpy((char *)&glSendPack.sField62[2], "91",2);
	memcpy((char *)&glSendPack.sField62[4], PosCom.stTrans.szInstalCode,strlen((char*)PosCom.stTrans.szInstalCode));	
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;
}

// ����Ԥ��Ȩ�������,֪ͨ
uint8_t TransMotoAuth(void)
{
	uint8_t ucRet;

	lcdCls();
	if( stTemp.iTransNo==POS_MOTO_AUTH_CM )
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "Ԥ��Ȩ���(����)", "PRE CMP(ONLINE)");
	else
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "Ԥ��Ȩ���(֪ͨ)", "PRE CMP(OFFLINE)");	
	ucRet = GetMotoDate(stTemp.iTransNo);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	lcdCls();
	if( stTemp.iTransNo==POS_MOTO_AUTH_CM )
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "Ԥ��Ȩ���(����)", "PRE CMP(ONLINE)");
	else
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "Ԥ��Ȩ���(֪ͨ)", "PRE CMP(OFFLINE)");
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
	
	if( stTemp.iTransNo!=POS_MOTO_AUTH_CM )
	{
		ucRet = AppConfirmAmount();
		if( ucRet==KEY_CANCEL )
		{
			goto REINPUT1;
		}
	}
	
	// ���ý�����������
	if (stTemp.iTransNo == POS_MOTO_AUTH_CM)
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
	if (stTemp.iTransNo !=POS_MOTO_AUTH_OFF)
	{
		ShortToByte(glSendPack.sField62, 22);	// �����Ƶ���,ͷ2���ֽڴ�����ݳ���
		memset((char *)&glSendPack.sField62[2],' ',22);
		memcpy((char *)&glSendPack.sField62[2], "91",2);
		memcpy((char *)&glSendPack.sField62[4], PosCom.stTrans.szInstalCode,strlen((char*)PosCom.stTrans.szInstalCode));	
	}
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;
}


// ����Ԥ��Ȩ����
uint8_t TransMotoVoidPreAuth(void)
{
	uint8_t ucRet;
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " ����Ԥ��Ȩ���� ", "      SALE      ");
	ucRet = CheckSupPwd();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " ����Ԥ��Ȩ���� ", "      SALE      ");
	ucRet = GetMotoDate(stTemp.iTransNo);
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
	
	PosCom.stTrans.lOldBatchNumber = 0;
	PosCom.stTrans.lOldTraceNo = 0;
	
	SetCommReqField((uint8_t *)"0100", (uint8_t *)"200000");
	sprintf((char *)glSendPack.szAuthCode, "%.6s", PosCom.stTrans.szAuthNo);
	sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s", PosCom.stTrans.lOldBatchNumber,
		PosCom.stTrans.lOldTraceNo, PosCom.stTrans.szOldTxnDate);
	memset(glSendPack.sField62,0,sizeof(glSendPack.sField62));	
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;

}

// �����˻�
uint8_t TransMotoRefund(void)
{
	uint8_t ucRet, ucSwipeMode;
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    �����˻�    ", "      SALE      ");
	ucRet = CheckSupPwd();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    �����˻�    ", "      SALE      ");
/*	ucRet = GetMotoDate(stTemp.iTransNo);
	if( ucRet!=OK )
	{
		return ucRet;
	}*/
/*	if( stPosParam.ucEmvSupport==PARAM_OPEN )
		ucSwipeMode = CARD_SWIPED|CARD_INSERTED;
	else
		ucSwipeMode = CARD_SWIPED;*/
	ucSwipeMode = CARD_KEYIN;
	ucRet = PosGetCard(ucSwipeMode);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    �����˻�    ", "      SALE      ");
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
/*	ShortToByte(glSendPack.sField62, 22);	// �����Ƶ���,ͷ2���ֽڴ�����ݳ���
	memset((char *)&glSendPack.sField62[2],' ',22);
	memcpy((char *)&glSendPack.sField62[2], "91",2);
	memcpy((char *)&glSendPack.sField62[4], PosCom.stTrans.szInstalCode,strlen((char*)PosCom.stTrans.szInstalCode));*/
	ucRet = OnlineCommProc();   // ����ͨѶ����
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;
}


uint8_t  GetMotoDate(int iTransType)
{
	uint8_t ucRet;
	int     iRet;
	char    szBuffer[100],szTemp[100], szTransName[20];
	int     ilen;
	
	memset(szTransName,0,sizeof(szTransName));
	memset(szBuffer,0,sizeof(szBuffer));
	GetTransName(iTransType,(uint8_t*)szBuffer);
	ilen = strlen(szBuffer);	
	ilen = (16-ilen)/2;
	memset(szTransName,' ',16);
	memcpy(szTransName+ilen,szBuffer,strlen(szBuffer));

	lcdCls();
	lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,"%-16.16s",szTransName);	
	ucRet = PosGetOrderCard(CARD_KEYIN);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	if (stTemp.iTransNo != POS_MOTO_PREAUTH_VOID)
	{
		lcdCls();
		lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,szTransName);
		memset(szBuffer,0,sizeof(szBuffer));	
		DispMulLanguageString(0, 2, DISP_CFONT, "������", "INPUT ID NO:");
		DispMulLanguageString(0, 4, DISP_CFONT, "���֤�ź���λ:", "INPUT INSTAL NO:");
		lcdGoto(80, 6);
		iRet = kbGetString(KB_EN_BIGFONT+KB_EN_NUM+KB_EN_CHAR+KB_EN_REVDISP, 6, 6, 
			(stPosParam.ucOprtLimitTime*1000), (char *)szBuffer);
		if( iRet<0 )	
			return (E_TRANS_CANCEL);
		
		lcdCls();
		lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,szTransName);	
		memset(szTemp,0,sizeof(szTemp));	
		DispMulLanguageString(0, 2, DISP_CFONT, "������CVN2:", "INPUT CVN2:");
		lcdGoto(80, 6);
		iRet = kbGetString(KB_EN_BIGFONT+KB_EN_NUM+KB_EN_CHAR+KB_EN_REVDISP, 3, 3, 
			(stPosParam.ucOprtLimitTime*1000), (char *)szTemp);
		if( iRet<0 )	
			return (E_TRANS_CANCEL);
		
		sprintf((char*)PosCom.stTrans.szInstalCode,"%s%s",szTemp,szBuffer);
	}
	
	ucRet = ManualEntryExpDate(PosCom.stTrans.szExpDate);
	if (ucRet != 0)
	{
		return ucRet;
	}

	return 0;
}


//  ���ֿۿ�
uint8_t TransPartSale(void)
{
	uint8_t  ucRet;
	char     szBuffer[100], szAmount[13];
	int      iRet,  iTransNoBak, iStanBak;

	PosCom.stTrans.iTransNo = stTemp.iTransNo;
	memcpy(&PosCom.stOrgPack, &glSendPack, sizeof(glSendPack));	// ����������
	memcpy(&BackPosCom, &PosCom, COM_SIZE);
	iTransNoBak = PosCom.stTrans.iTransNo;
	iStanBak = PosCom.stTrans.lTraceNo;

	lcdCls();
	if (stTemp.iTransNo == POS_SALE)
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    ��    ��    ", "      SALE      ");
	}
	else
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  ���˻�������  ", "      SALE      ");
	}
	lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"�� ȷ ��");
	lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"���ֳжҽ���");
	lcdDisplay(0,6,DISP_CFONT|DISP_MEDIACY,"����Y  ȡ��N");
	while(1)
	{
		iRet = PubWaitKey(stPosParam.ucOprtLimitTime);
		if (iRet == KEY_ENTER)
		{
			lcdClrLine(2,7);
			lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"δ�����");
			memset(szBuffer,0,sizeof(szBuffer));
			memset(szAmount,0,sizeof(szAmount));
			PubAscSub((char*)glSendPack.szTranAmt,(char*)glRecvPack.szTranAmt,12,szAmount);
			PubAddHeadChars(szAmount,12,'0');
			PubConvAmount(NULL,(uint8_t*)szAmount,2,(uint8_t*)szBuffer,0);
			lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"%s Ԫ",szBuffer);
			lcdDisplay(0,6,DISP_CFONT|DISP_MEDIACY,"��������ȡ");
			PubBeepErr();
			PubWaitKey(5);
			strcpy((char*)PosCom.szRespCode,"00");  // ���óɹ�
			PubAsc2Bcd((char*)glRecvPack.szTranAmt,12,(char*)PosCom.stTrans.sAmount);  // ��ӡ�жҽ��
			PosCom.stTrans.ucInstalPayMode = 0x03;  // ���ֿۿ��־
			AfterCommProc();
			return 0;
		}
		if (iRet == KEY_CANCEL)
		{
			memcpy(&PosCom, &BackPosCom, COM_SIZE);
			memcpy(&glSendPack, &PosCom.stOrgPack, sizeof(glSendPack));	// �ָ�������
			stTemp.iTransNo = PosCom.stTrans.iTransNo = iTransNoBak;
			PosCom.stTrans.lTraceNo = iStanBak;
			strcpy((char*)PosCom.szRespCode,"00");  // ���óɹ�
			PubAsc2Bcd((char*)glRecvPack.szTranAmt,12,(char*)PosCom.stTrans.sAmount);  // ��ӡ�жҽ��
			PosCom.stTrans.ucInstalPayMode = 0x03;  // ���ֿۿ��־
			AfterCommProc();

			if (stTemp.iTransNo == POS_SALE)
			{
				stTemp.iTransNo = PosCom.stTrans.iTransNo =POS_SALE_VOID; 
			}
			else
			{
				stTemp.iTransNo = PosCom.stTrans.iTransNo = POS_VOID_LOYATL_ALL;
			}
			
			PosCom.ucSwipedFlag = CARD_KEYIN;
			PosCom.ucPinEntry = FALSE;
			PosCom.stTrans.lOldTraceNo = PosCom.stTrans.lTraceNo;
			PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;
			sprintf((char *)PosCom.stTrans.szPrtOldPosSer, "%06d", PosCom.stTrans.lOldTraceNo);
			SetCommReqField((uint8_t *)"0200", (uint8_t *)"200000");
			sprintf((char *)glSendPack.szRRN, "%.*s", LEN_RRN, glRecvPack.szRRN);
			sprintf((char *)glSendPack.szAuthCode, "%.*s", LEN_AUTH_CODE, glRecvPack.szAuthCode);
// 			sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%4.4s", BackPosCom.stTrans.lBatchNumber,
// 					iStanBak, BackPosCom.stTrans.szDate+4);
			sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d", BackPosCom.stTrans.lBatchNumber,
					iStanBak);

			ucRet = OnlineCommProc();   // ����ͨѶ����
			if( ucRet!=OK )
			{
				return ucRet;
			}
			return NO_DISP;
		}

		PubBeepErr();
		continue;
	}

	memcpy(&PosCom, &BackPosCom, COM_SIZE);
	memcpy(&glSendPack, &PosCom.stOrgPack, sizeof(glSendPack));	// �ָ�������
	stTemp.iTransNo = PosCom.stTrans.iTransNo = iTransNoBak;
	strcpy((char*)PosCom.szRespCode,"00");  // ���óɹ�

	return 0;
}

// end of file

