
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

uint8_t  GetMotoDate(int iTransType);  // 订购交易的卡号输入和其它公用数据输入


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

// 预约消费
uint8_t SelectBookFunc(void)
{
	int keySub; //二级菜单返回健值	
	const char *Booking_menu[] = 
	{
		"1 预约消费      ",
		"2 预约消费撤销  "
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

// 订购
uint8_t SelectMotoFunc(void)
{
	int keySub; //二级菜单返回健值	
	const char *Moto_menu[] = 
	{
		"1 消费          ",
		"2 消费撤销      ",
		"3 退货          ",
		"4 预授权        ",
		"5 预授权撤销    ",
		"6 预授权完成请求",
		"7 预授权完成撤销",
		"8 预授权完成通知"
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

// 磁条卡充值
uint8_t SelectMagTopupFunc(void)
{
	int keySub; //二级菜单返回健值	
	// 分期付款菜单
	const char *Magtopup_menu[] = 
	{
		"1 现金充值      ",
		"2 账户充值      "
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


 // 预约消费
uint8_t TransBookSale(void)
{
	uint8_t ucRet;
	int     iRet;
	char    szBuffer[100],szTemp[100];
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "手机无卡预约消费", "   SALE  ");
	ucRet = AppGetAmount(9, TRAN_AMOUNT);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "手机无卡预约消费", "   SALE  ");
	memset(szBuffer,0,sizeof(szBuffer));	
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入手机号码:", "INPUT INSTAL NO:");
/*	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_BIGFONT+KB_EN_NUM+KB_EN_CHAR+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 1, 30, 
		(stPosParam.ucOprtLimitTime*1000), (char *)szBuffer);*/
	iRet = PubGetString(NUM_IN,4,11,(uint8_t*)szBuffer,stPosParam.ucOprtLimitTime);
	if( iRet != 0 )	
		return (E_TRANS_CANCEL);

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "手机无卡预约消费", "   SALE  ");
	memset(szTemp,0,sizeof(szTemp));	
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入预约号码:", "INPUT INSTAL NO:");
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
			DispMulLanguageString(0, 2, DISP_CFONT, "请做联机签到", "LOGON ONLINE");
			kbGetKey();
		}
		return ucRet;
	}
	
	SetCommReqField((uint8_t *)"0200", (uint8_t *)"000000");
	ShortToByte(glSendPack.sField62, 22);	// 二进制的域,头2个字节存放数据长度
	memset((char *)&glSendPack.sField62[2],' ',22);
	memcpy((char *)&glSendPack.sField62[2], "90",2);
	memcpy((char *)&glSendPack.sField62[4], PosCom.stTrans.szInstalCode,strlen((char*)PosCom.stTrans.szInstalCode));
	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;
}

// 预约消费撤消
uint8_t TransVoidBookSale(void)
{
	uint8_t	ucRet, ucSwipeFlag;
	
	if( stPosParam.ucEmvSupport==PARAM_OPEN )	
		ucSwipeFlag = CARD_SWIPED|CARD_INSERTED|0x80;
	else
		ucSwipeFlag = CARD_SWIPED|0x80;
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  预约消费撤消  ", "VOID INSTALLMENT");
	
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
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "原交易非预约消费", "NOT PREAUTH");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "    不能撤销    ", "NOT VOID");
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
	sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d", PosCom.stTrans.lOldBatchNumber,
		PosCom.stTrans.lOldTraceNo);
	
	ucRet = OnlineCommProc();   // 联机通讯过程
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

	// 设置交易请求数据
	SetCommReqField((uint8_t*)"0100", (uint8_t*)"330000");
	ShortToByte(glSendPack.sField62, 22);	// 二进制的域,头2个字节存放数据长度
	memset((char *)&glSendPack.sField62[2],' ',22);
	memcpy((char *)&glSendPack.sField62[2],PosCom.stTrans.szInstalCode,strlen((char*)PosCom.stTrans.szInstalCode));
	memset((char *)glSendPack.sPINData,0,sizeof(glSendPack.sPINData));
	memset((char *)glSendPack.szCaptureCode,0,sizeof(glSendPack.szCaptureCode));
	ucRet = OnlineCommProc();   // 联机通讯过程
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
	PosCom.stTrans.lTraceNo     = stPosParam.lNowTraceNo;  // 流水号加1
	return OK;
}

// 磁条卡现金充值
uint8_t TransMagTopupCash(void)
{
	uint8_t ucRet;
	int     iRet, iAmount, iBalance, iIdType;
	char    szBuffer[100],szTemp[100],szData[100];
	const char *mainMenu[] = 
	{
		"请选择证件类型",
		"1 身份证        ",
		"2 军官证        ",
		"3 护照          ",
		"4 回乡证        ",
		"5 台胞证        ",
		"6 台胞证        ",
		"7 士兵证        ",
		"8 其它证件      "
	};
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " 磁条卡现金充值 ", "      SALE      ");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " 磁条卡现金充值 ", "      SALE      ");
	memset(szBuffer,0,sizeof(szBuffer));	
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入证件号码:", "INPUT INSTAL NO:");
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
	sprintf(szData,"可充金额:%s", szBuffer);
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_CFONT, szData, szData);
	sprintf(szData,"充值金额:%s", szTemp);
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, szData,szData);

	if (iBalance >= iAmount) 
	{
		DispMulLanguageString(0, 4, DISP_CLRLINE|DISP_CFONT, "  按确认键继续  ", " LOOK PINPAD    ");
	}
	else
	{
		DispMulLanguageString(0, 4, DISP_CLRLINE|DISP_CFONT, "   请调整金额   ", " LOOK PINPAD    ");
		DispMulLanguageString(0, 6, DISP_CLRLINE|DISP_CFONT, "    按返回键    ", " LOOK PINPAD    ");
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
					DispMulLanguageString(0, 4, DISP_CLRLINE|DISP_CFONT, "  超过可充金额  ", " LOOK PINPAD    ");
					DispMulLanguageString(0, 6, DISP_CLRLINE|DISP_CFONT, "   请调整金额   ", " LOOK PINPAD    ");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " 磁条卡现金充值 ", "      SALE      ");
	memset(glRecvPack.szRspCode,0,sizeof(glRecvPack.szRspCode));
	SetCommReqField((uint8_t *)"0200", (uint8_t *)"630000");
	sprintf((char *)glSendPack.szRRN, "%.*s", LEN_RRN, PosCom.stTrans.szSysReferNo);
	sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d", PosCom.stTrans.lOldBatchNumber,
				PosCom.stTrans.lOldTraceNo);
	ShortToByte(glSendPack.sField62, 22);	// 二进制的域,头2个字节存放数据长度
	memset((char *)&glSendPack.sField62[2],' ',22);
	memcpy((char *)&glSendPack.sField62[2],PosCom.stTrans.szInstalCode,strlen((char*)PosCom.stTrans.szInstalCode));
	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		if (memcmp(PosCom.szRespCode, "98", 2)==0 ||
			(PosCom.bOnlineTxn && strlen((char*)glRecvPack.szRspCode) == 0)) // 发送了但是应答码为空，即没有收到也发确认
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

// 磁条卡帐户充值
uint8_t TransMagTopupAcct(void)
{
	uint8_t ucRet, ucPinEntryBak;
	int     iRet, iAmount, iBalance, iIdType;
	char    szBuffer[100],szTemp[100],szData[100], szDisp[100];
	const char *mainMenu[] = 
	{
		"请选择证件类型",
		"1 身份证        ",
		"2 军官证        ",
		"3 护照          ",
		"4 回乡证        ",
		"5 台胞证        ",
		"6 台胞证        ",
		"7 士兵证        ",
		"8 其它证件      "
	};
	char   szTrack2Bak[TRACK2_LEN+1], szTrack3Bak[TRACK3_LEN+1];
	uint8_t ucSwipedFlagBak,ucSwipedFlagBak2;
	
	PosCom.stTrans.ucEcTopOldSwipeFlag = TRUE; // 设置第一次刷卡时必需判断IC卡
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " 磁条卡账户充值 ", "      SALE      ");
	ucRet = PosGetCard(CARD_SWIPED|CARD_INSERTED);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	ucSwipedFlagBak = PosCom.ucSwipedFlag;
	// 交易报文上送转出卡磁道
	memset(szTrack2Bak,0,sizeof(szTrack2Bak));
	memset(szTrack3Bak,0,sizeof(szTrack3Bak));
	strcpy(szTrack2Bak,PosCom.szTrack2);
	strcpy(szTrack3Bak,PosCom.szTrack3);
	strcpy((char*)PosCom.stTrans.szTranferCardNo,(char*)PosCom.stTrans.szCardNo);
	PosCom.ucSwipedFlag = NO_SWIPE_INSERT;
	PosCom.stTrans.ucFallBack = FALSE;
	PosCom.stTrans.ucEcTopOldSwipeFlag = FALSE; // 第二次刷卡不需要判断IC卡
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " 磁条卡账户充值 ", "      SALE      ");
	memset(szBuffer,0,sizeof(szBuffer));	
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入证件号码:", "INPUT INSTAL NO:");
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
	sprintf(szDisp, "可充金额:%s", szBuffer);
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_CFONT, szDisp,szDisp);
	sprintf(szDisp, "充值金额:%s", szTemp);
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, szDisp,szDisp);
	
	if (iBalance >= iAmount) 
	{
		DispMulLanguageString(0, 4, DISP_CLRLINE|DISP_CFONT, "  按确认键继续  ", " LOOK PINPAD    ");
	}
	else
	{
		DispMulLanguageString(0, 4, DISP_CLRLINE|DISP_CFONT, "   请调整金额   ", " LOOK PINPAD    ");
		DispMulLanguageString(0, 6, DISP_CLRLINE|DISP_CFONT, "    按返回键    ", " LOOK PINPAD    ");
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
					DispMulLanguageString(0, 4, DISP_CLRLINE|DISP_CFONT, "  超过可充金额  ", " LOOK PINPAD    ");
					DispMulLanguageString(0, 6, DISP_CLRLINE|DISP_CFONT, "   请调整金额   ", " LOOK PINPAD    ");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " 磁条卡账户充值 ", "      SALE      ");
	ucRet = EnterPIN(0);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	//上送转出卡磁道信息
	strcpy(PosCom.szTrack2,szTrack2Bak);
	strcpy(PosCom.szTrack3,szTrack3Bak);
	PosCom.ucSwipedFlag = ucSwipedFlagBak;  // 先按照转出卡的方式设置
	
	SetCommReqField((uint8_t *)"0200", (uint8_t *)"400000");
	// 60.3-- 60.6 不需要
	glSendPack.szField60[8] = 0;
	PosCom.ucSwipedFlag = ucSwipedFlagBak2;  // 再按照转入卡的方式设置
	// 用原验证交易流水号
	sprintf((char *)glSendPack.szSTAN, "%0*d", LEN_POS_STAN,  PosCom.stTrans.lOldTraceNo);

/*	ShortToByte(glSendPack.sField62, 22);	// 二进制的域,头2个字节存放数据长度
	memset((char *)&glSendPack.sField62[2],' ',28);
	memcpy((char *)&glSendPack.sField62[2],PosCom.stTrans.szTranferCardNo,strlen((char*)PosCom.stTrans.szTranferCardNo));*/
	//62填充实际长度
	ShortToByte(glSendPack.sField62, strlen((char*)PosCom.stTrans.szTranferCardNo));
	strcpy((char *)&glSendPack.sField62[2],PosCom.stTrans.szTranferCardNo);
	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;		
}

// 磁条卡现金充值确认
uint8_t TransmagTopupConfirm(void)
{
	int  iTransType;
	uint8_t ucRet;
	uint8_t  szRespCodeBak[2+1],szIssuerBankId[9],szRecvBankId[9],szCenterId[9], szAuthNo[7],szSysReferNo[13];
	uint8_t szCardUnit[4];

	//先备份信息
	sprintf((char *)szSysReferNo,   "%.12s",PosCom.stTrans.szSysReferNo);
	sprintf((char *)szAuthNo,   "%.6s",  PosCom.stTrans.szAuthNo);
	sprintf((char *)szIssuerBankId, "%.8s",  PosCom.stTrans.szIssuerBankId);
	sprintf((char *)szRecvBankId,   "%.8s",  PosCom.stTrans.szRecvBankId);
	sprintf((char *)szCenterId,     "%.8s",  PosCom.stTrans.szCenterId);

	PosCom.stTrans.iTransNo = stTemp.iTransNo;
	memcpy(&PosCom.stOrgPack, &glSendPack, sizeof(glSendPack));	// 保存请求报文
	memcpy(&BackPosCom, &PosCom, COM_SIZE);
	strcpy(szCardUnit,PosCom.stTrans.szCardUnit);  // 充值确认不会回这个域，用前面的
	if (strlen(szCardUnit)!=0)
	{
		strcpy(szCardUnit,"CUP");
	}
	
	iTransType = PosCom.stTrans.iTransNo;
	PosCom.stTrans.iTransNo = POS_MAGTOPUP_CM;	
	stTemp.iTransNo = POS_MAGTOPUP_CM;
	
	// 设置交易请求数据
	SetCommReqField((uint8_t*)"0220", (uint8_t*)"630000");
	memset(glSendPack.sField62,0,sizeof(glSendPack.sField62));
	//磁条现金充值确认报文中35、36、53域多余；
	memset(glSendPack.szTrack2,0,sizeof(glSendPack.szTrack2));
	memset(glSendPack.szTrack3,0,sizeof(glSendPack.szTrack3));
	memset(glSendPack.szSecurityInfo,0,sizeof(glSendPack.szSecurityInfo));
	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		strcpy(szRespCodeBak,PosCom.szRespCode);

//		memcpy(&PosCom, &BackPosCom, COM_SIZE);
//		memcpy(&glSendPack, &PosCom.stOrgPack, sizeof(glSendPack));	// 恢复请求报文
		stTemp.iTransNo = PosCom.stTrans.iTransNo;
//		PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;
		PosCom.stTrans.iTransNo = stTemp.iTransNo = iTransType;
		if (memcmp(szRespCodeBak,"A6",2)==0)
		{
			strcpy(PosCom.szRespCode,szRespCodeBak);
		}


		//恢复信息
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

		strcpy(PosCom.stTrans.szCardUnit,szCardUnit);  // 充值确认不会回这个域，用原来的

		return ucRet;
	}
	
//	memcpy(&PosCom, &BackPosCom, COM_SIZE);
//	memcpy(&glSendPack, &PosCom.stOrgPack, sizeof(glSendPack));	// 恢复请求报文
	stTemp.iTransNo = PosCom.stTrans.iTransNo;
//	PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;
	PosCom.stTrans.iTransNo = stTemp.iTransNo = iTransType;

	//恢复信息
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
	
	strcpy(PosCom.stTrans.szCardUnit,szCardUnit);  // 充值确认不会回这个域，用原来的

	return OK;
}

// 订购消费
uint8_t TransMotoSale(void)
{
	uint8_t ucRet;
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    订购消费    ", "      SALE      ");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    订购消费    ", "      SALE      ");

	SetCommReqField((uint8_t *)"0200", (uint8_t *)"000000");
	ShortToByte(glSendPack.sField62, 22);	// 二进制的域,头2个字节存放数据长度
	memset((char *)&glSendPack.sField62[2],' ',22);
	memcpy((char *)&glSendPack.sField62[2], "91",2);
	memcpy((char *)&glSendPack.sField62[4], PosCom.stTrans.szInstalCode,strlen((char*)PosCom.stTrans.szInstalCode));
	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;	
}

// 订购消费撤消
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
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  订购消费撤销  ", "VOID SALE");	
	}
	else
	{
		lcdCls();
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "订购授权完成撤销", "VOID SALE");	
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
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "原交易非订购消费", "NOT SALE");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "    不能撤销    ", "NOT VOID");
		ErrorBeep();
		kbGetKeyMs(3000);		
		return NO_DISP;
	}
	if( stTemp.iTransNo==POS_MOTO_AUTH_VOID && 
		(PosCom.stTrans.iOldTransNo!=POS_MOTO_AUTH_CM && PosCom.stTrans.iOldTransNo!=POS_MOTO_AUTH_OFF) )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "原交易非授权完成", "NOT PREAUTH");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "    不能撤销    ", "NOT VOID");
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
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  订购消费撤销  ", "VOID SALE");	
	}
	else
	{
		lcdCls();
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "订购授权完成撤销", "VOID SALE");	
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
	
	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;		
}

// 订购预授权
uint8_t TransMotoPreAuth(void)
{
	uint8_t	ucRet;
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "   订购预授权   ", "      SALE      ");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "   订购预授权   ", "      SALE      ");
	
	// 设置交易请求数据
	SetCommReqField((uint8_t *)"0100", (uint8_t *)"030000");
	ShortToByte(glSendPack.sField62, 22);	// 二进制的域,头2个字节存放数据长度
	memset((char *)&glSendPack.sField62[2],' ',22);
	memcpy((char *)&glSendPack.sField62[2], "91",2);
	memcpy((char *)&glSendPack.sField62[4], PosCom.stTrans.szInstalCode,strlen((char*)PosCom.stTrans.szInstalCode));	
	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;
}

// 订购预授权完成请求,通知
uint8_t TransMotoAuth(void)
{
	uint8_t ucRet;

	lcdCls();
	if( stTemp.iTransNo==POS_MOTO_AUTH_CM )
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "预授权完成(请求)", "PRE CMP(ONLINE)");
	else
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "预授权完成(通知)", "PRE CMP(OFFLINE)");	
	ucRet = GetMotoDate(stTemp.iTransNo);
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	lcdCls();
	if( stTemp.iTransNo==POS_MOTO_AUTH_CM )
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "预授权完成(请求)", "PRE CMP(ONLINE)");
	else
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "预授权完成(通知)", "PRE CMP(OFFLINE)");
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
	
	// 设置交易请求数据
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
		ShortToByte(glSendPack.sField62, 22);	// 二进制的域,头2个字节存放数据长度
		memset((char *)&glSendPack.sField62[2],' ',22);
		memcpy((char *)&glSendPack.sField62[2], "91",2);
		memcpy((char *)&glSendPack.sField62[4], PosCom.stTrans.szInstalCode,strlen((char*)PosCom.stTrans.szInstalCode));	
	}
	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;
}


// 订购预授权撤消
uint8_t TransMotoVoidPreAuth(void)
{
	uint8_t ucRet;
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " 订购预授权撤消 ", "      SALE      ");
	ucRet = CheckSupPwd();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, " 订购预授权撤消 ", "      SALE      ");
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
	ucRet = OnlineCommProc();   // 联机通讯过程
	if( ucRet!=OK )
	{
		return ucRet;
	}
	
	return NO_DISP;

}

// 订购退货
uint8_t TransMotoRefund(void)
{
	uint8_t ucRet, ucSwipeMode;
	
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    订购退货    ", "      SALE      ");
	ucRet = CheckSupPwd();
	if( ucRet!=OK )
	{
		return ucRet;
	}

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    订购退货    ", "      SALE      ");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    订购退货    ", "      SALE      ");
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
/*	ShortToByte(glSendPack.sField62, 22);	// 二进制的域,头2个字节存放数据长度
	memset((char *)&glSendPack.sField62[2],' ',22);
	memcpy((char *)&glSendPack.sField62[2], "91",2);
	memcpy((char *)&glSendPack.sField62[4], PosCom.stTrans.szInstalCode,strlen((char*)PosCom.stTrans.szInstalCode));*/
	ucRet = OnlineCommProc();   // 联机通讯过程
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
		DispMulLanguageString(0, 2, DISP_CFONT, "请输入", "INPUT ID NO:");
		DispMulLanguageString(0, 4, DISP_CFONT, "身份证号后六位:", "INPUT INSTAL NO:");
		lcdGoto(80, 6);
		iRet = kbGetString(KB_EN_BIGFONT+KB_EN_NUM+KB_EN_CHAR+KB_EN_REVDISP, 6, 6, 
			(stPosParam.ucOprtLimitTime*1000), (char *)szBuffer);
		if( iRet<0 )	
			return (E_TRANS_CANCEL);
		
		lcdCls();
		lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,szTransName);	
		memset(szTemp,0,sizeof(szTemp));	
		DispMulLanguageString(0, 2, DISP_CFONT, "请输入CVN2:", "INPUT CVN2:");
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


//  部分扣款
uint8_t TransPartSale(void)
{
	uint8_t  ucRet;
	char     szBuffer[100], szAmount[13];
	int      iRet,  iTransNoBak, iStanBak;

	PosCom.stTrans.iTransNo = stTemp.iTransNo;
	memcpy(&PosCom.stOrgPack, &glSendPack, sizeof(glSendPack));	// 保存请求报文
	memcpy(&BackPosCom, &PosCom, COM_SIZE);
	iTransNoBak = PosCom.stTrans.iTransNo;
	iStanBak = PosCom.stTrans.lTraceNo;

	lcdCls();
	if (stTemp.iTransNo == POS_SALE)
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    消    费    ", "      SALE      ");
	}
	else
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  联盟积分消费  ", "      SALE      ");
	}
	lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"请 确 认");
	lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"部分承兑交易");
	lcdDisplay(0,6,DISP_CFONT|DISP_MEDIACY,"接收Y  取消N");
	while(1)
	{
		iRet = PubWaitKey(stPosParam.ucOprtLimitTime);
		if (iRet == KEY_ENTER)
		{
			lcdClrLine(2,7);
			lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"未付金额");
			memset(szBuffer,0,sizeof(szBuffer));
			memset(szAmount,0,sizeof(szAmount));
			PubAscSub((char*)glSendPack.szTranAmt,(char*)glRecvPack.szTranAmt,12,szAmount);
			PubAddHeadChars(szAmount,12,'0');
			PubConvAmount(NULL,(uint8_t*)szAmount,2,(uint8_t*)szBuffer,0);
			lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"%s 元",szBuffer);
			lcdDisplay(0,6,DISP_CFONT|DISP_MEDIACY,"请另行收取");
			PubBeepErr();
			PubWaitKey(5);
			strcpy((char*)PosCom.szRespCode,"00");  // 设置成功
			PubAsc2Bcd((char*)glRecvPack.szTranAmt,12,(char*)PosCom.stTrans.sAmount);  // 打印承兑金额
			PosCom.stTrans.ucInstalPayMode = 0x03;  // 部分扣款标志
			AfterCommProc();
			return 0;
		}
		if (iRet == KEY_CANCEL)
		{
			memcpy(&PosCom, &BackPosCom, COM_SIZE);
			memcpy(&glSendPack, &PosCom.stOrgPack, sizeof(glSendPack));	// 恢复请求报文
			stTemp.iTransNo = PosCom.stTrans.iTransNo = iTransNoBak;
			PosCom.stTrans.lTraceNo = iStanBak;
			strcpy((char*)PosCom.szRespCode,"00");  // 设置成功
			PubAsc2Bcd((char*)glRecvPack.szTranAmt,12,(char*)PosCom.stTrans.sAmount);  // 打印承兑金额
			PosCom.stTrans.ucInstalPayMode = 0x03;  // 部分扣款标志
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

			ucRet = OnlineCommProc();   // 联机通讯过程
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
	memcpy(&glSendPack, &PosCom.stOrgPack, sizeof(glSendPack));	// 恢复请求报文
	stTemp.iTransNo = PosCom.stTrans.iTransNo = iTransNoBak;
	strcpy((char*)PosCom.szRespCode,"00");  // 设置成功

	return 0;
}

// end of file

