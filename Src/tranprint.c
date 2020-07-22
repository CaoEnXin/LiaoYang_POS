
#include "posapi.h"
#include "posmacro.h"
#include "terstruc.h"
#include "glbvar.h"
#include "tranfunclist.h"
#include "font.h"




void PrnBigFontChnData(char *pBuf)
{
	INT32 iLanguage, iWidth, iRet;
	UINT8 sFontName[40], sFontChar[40];
	
	prnGetCurFont(sFontName, sFontChar, &iLanguage, &iWidth, SET_PRIMARY_FONT);
	
	iRet = prnSetFont(sFontName, sFontChar, 0, 24, SET_PRIMARY_FONT);
	if( iRet==ERROR )
	{
		lcdClrLine(2, 7);
		DispMulLanguageString(0, 2, DISP_CFONT, "字库中无该字体!!", "FONT NOT EXIST ");
		kbGetKey();
		return ;
	}
	
	prnGetCurFont(sFontName, sFontChar, &iLanguage, &iWidth, SET_SECONDARY_FONT);
	
	iRet = prnSetFont(sFontName, sFontChar, 0, 24, SET_SECONDARY_FONT);
	if( iRet==ERROR )
	{
		lcdClrLine(2, 7);
		DispMulLanguageString(0, 2, DISP_CFONT, "字库中无该字体!!", "FONT NOT EXIST");
		kbGetKey();
		return ;
	}
	
	prnPrintf(pBuf);
	
	return ;	
}

void PrnBigFontEngData(char *pBuf)
{
	return PrnBigFontChnData(pBuf);
}

void PrnNormalFontData(char *pBuf)
{	
	return PrnBigFontChnData(pBuf);
	//prnPrintf(pBuf);
	
	//return ;	
}


void PrnSmallFontData(char *pBuf)
{
	INT32 iLanguage, iWidth, iRet;
	UINT8 sFontName[40], sFontChar[40];
		
	prnGetCurFont(sFontName, sFontChar, &iLanguage, &iWidth, SET_PRIMARY_FONT);

	iRet = prnSetFont(sFontName, sFontChar, 0, 16, SET_PRIMARY_FONT);
	if( iRet==ERROR )
	{
		lcdClrLine(2, 7);
		DispMulLanguageString(0, 2, DISP_CFONT, "字库中无该字体!!", "FONT NOT EXIST");
		kbGetKey();
		return ;
	}

	prnGetCurFont(sFontName, sFontChar, &iLanguage, &iWidth, SET_SECONDARY_FONT);
	
	iRet = prnSetFont(sFontName, sFontChar, 0, 16, SET_SECONDARY_FONT);
	if( iRet==ERROR )
	{
		lcdClrLine(2, 7);
		DispMulLanguageString(0, 2, DISP_CFONT, "字库中无该字体!!", "FONT NOT EXIST");
		kbGetKey();
		return ;
	}
	prnPrintf(pBuf);
	
	return ;
	
}

void PrtTranTicket(int flag)
{
	char  buf[200], buf1[200],szPrnTitle[200];
	int   iRet, j;
	uint8_t szCardTemp[20];
	uint32_t	ulICCDataLen;
	uint8_t	*psICCData, *psTemp;

	memset(buf,0,sizeof(buf));
	memset(buf1,0,sizeof(buf1));
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "   打印凭单中   ", "PRINT...");
	for(j=0; j<3; j++)
	{
		if( flag!=2 )
		{
			lcdClrLine(6, 7);
			lcdFlip();	
			DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "   打印凭单中   ", "PRINT...");
		}

		if( j==1 && stPosParam.ucTicketNum=='2' ) //不打印银行存根
			continue;
		
		iRet = prnInit();
		if( iRet!=OK )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 3, DISP_CFONT, "打印机初始化失败", "INIT PRINT FAIL");
			kbGetKey();
			return ;
		}
		prnStep(2);
		if( flag==2)
		{
			PrnBigFontChnData("\n***故障报告单,非凭证联***\n------------------------\n");
		}

		if( stPosParam.ucTestFlag==PARAM_OPEN )
		{
			PrnBigFontChnData("\n**测试/TESTING,凭证无效**\n");
		}
			
		PrnSmallFontData("\n-----------------------------------------------\n");
		if( stPosParam.ucPrnEng==PARAM_OPEN )
		{
			memset(szPrnTitle,0,sizeof(szPrnTitle));
			if(stPosParam.ucPrnTitleFlag == PARAM_CLOSE)
			{
				sprintf(szPrnTitle,"%s签购单/POS SALES SLIP\n",stPosParam.szPrnTitle);
				PrnBigFontChnData(szPrnTitle);
			}
			else
			{
				PrnBigFontChnData("银联POS签购单/POS SALES SLIP\n");
			}
		}
		else
		{
			memset(szPrnTitle,0,sizeof(szPrnTitle));
			if(stPosParam.ucPrnTitleFlag == PARAM_CLOSE)
			{
				sprintf(szPrnTitle,"%s签购单\n",stPosParam.szPrnTitle);
				PrnBigFontChnData(szPrnTitle);
			}
			else
			{
				PrnBigFontChnData("       银联POS签购单      \n");
			}
		}
		if( j==0 )
		{
			PrnSmallFontData("商户存根                     MERCHANT COPY");
		}
		else if( j==1 )
		{
			PrnSmallFontData("银行存根                         BANK COPY");
		}
		else if( j==2 )
		{
			PrnSmallFontData("持卡人存根                 CARDHOLDER COPY");
		}
		PrnSmallFontData("\n-----------------------------------------------\n");

		if( stPosParam.ucPrnEng==PARAM_OPEN )
			PrnNormalFontData("商户名称(MERCHANT NAME): \n");
		else
			PrnNormalFontData("商户名称: \n");

		prnPrintf("%s\n", stPosParam.szUnitChnName);

		if( stPosParam.ucPrnEng==PARAM_OPEN )
			prnPrintf("商户编号(MERCHANT NO): \n");	
		else
			prnPrintf("商户编号: \n");	

		prnPrintf("%s\n", PosCom.szUnitNum);

		if( stPosParam.ucPrnEng==PARAM_OPEN )
			prnPrintf("终端编号(TERMINAL NO):\n");
		else
			prnPrintf("终端编号:\n");

		prnPrintf("%s\n", PosCom.stTrans.szPosId);

		if( stPosParam.ucPrnEng==PARAM_OPEN )
			prnPrintf("操作员号(OPERATOR NO):");
		else
			prnPrintf("操作员号:");

		prnPrintf("  %s\n", stTransCtrl.szNowTellerNo+1);
		
		PrnSmallFontData("-----------------------------------------------\n");

		if( memcmp(stPosParam.szLocalId, "0000", 4)==0 )
		{
			if( memcmp(stPosParam.szMerBankId, PosCom.stTrans.szIssuerBankId, 4)==0 )
			{
				GetBankChnName(PosCom.stTrans.szIssuerBankId, (uint8_t *)buf);
				if( stPosParam.ucPrnEng==PARAM_OPEN )
				{
					snprintf(buf1, sizeof(buf1), "发卡行(ISSUER):%s\n", buf);
					PrnNormalFontData(buf1);
				}
				else
				{
					snprintf(buf1, sizeof(buf1), "发卡行: %s\n", buf);
					PrnNormalFontData(buf1);
				}
			}
			else
			{
				GetBankChnName(PosCom.stTrans.szIssuerBankId, (uint8_t *)buf);
				if( stPosParam.ucPrnEng==PARAM_OPEN )
				{
//					sprintf(buf1, "发卡行(ISSUER):%s %s\n", PosCom.stTrans.szIssuerBankId, buf);
					sprintf(buf1, "发卡行(ISSUER):%s\n", buf);
					PrnNormalFontData(buf1);
				}
				else
				{
//					sprintf(buf1, "发卡行:%s %s\n", PosCom.stTrans.szIssuerBankId, buf);
					sprintf(buf1, "发卡行:%s\n", buf);
					PrnNormalFontData(buf1);
				}
			}
		}
		else if( memcmp(stPosParam.szLocalId, &PosCom.stTrans.szIssuerBankId[4], 2)!=0 )
		{
			GetBankChnName(PosCom.stTrans.szIssuerBankId, (uint8_t *)buf);
			if( stPosParam.ucPrnEng==PARAM_OPEN )
			{
				sprintf(buf1, "发卡行(ISSUER):%s 银联\n", buf);
				PrnNormalFontData(buf1);
			}
			else
			{
				sprintf(buf1, "发卡行:%s 银联\n", buf);
				PrnNormalFontData(buf1);
			}
		}
		else
		{
			GetBankChnName(PosCom.stTrans.szIssuerBankId, (uint8_t *)buf);
			if( stPosParam.ucPrnEng==PARAM_OPEN )
			{
				snprintf(buf1, sizeof(buf1), "发卡行(ISSUER):%s\n", buf);
				PrnNormalFontData(buf1);
			}
			else
			{
				snprintf(buf1, sizeof(buf1), "发卡行:%s \n", buf);
				PrnNormalFontData(buf1);
			}
		}				

		if( memcmp(PosCom.stTrans.szCardUnit, "CUP", 3)!=0 )
		{
			if( stPosParam.ucPrnEng==PARAM_OPEN )
				prnPrintf("外卡组织:%s\n", PosCom.stTrans.szCardUnit);
			else
				prnPrintf("外卡组织:%s\n", PosCom.stTrans.szCardUnit);
		}

		if( stPosParam.ucPrnEng==PARAM_OPEN )
			prnPrintf("卡号(CARD NO):\n");
		else
			prnPrintf("卡号:\n");

		memset(buf, 0, sizeof(buf));
		memset(buf1, 0, sizeof(buf1));
		if( (PosCom.stTrans.iTransNo==POS_PREAUTH) || (PosCom.stTrans.iTransNo==POS_PREAUTH_ADD) ||
			(PosCom.stTrans.iTransNo==OFF_SALE) || (PosCom.stTrans.iTransNo==OFF_ADJUST) || 
			(PosCom.stTrans.iTransNo==ICC_OFFSALE) || (PosCom.stTrans.iTransNo==EC_QUICK_SALE) ||
			(PosCom.stTrans.iTransNo==EC_NORMAL_SALE) || (PosCom.stTrans.iTransNo==POS_MOBILE_PREAUTH) ||
			(PosCom.stTrans.iTransNo==POS_MOTO_PREAUTH))
		{
			MakeFormatCardNo((char *)PosCom.stTrans.szCardNo, buf1);
		}
		else
		{
//			if (PosCom.stTrans.iTransNo==POS_MAGTOPUP_ACCT)
			if(0)
			{
//				MakeFormatCardNo((char *)PosCom.stTrans.szTranferCardNo, buf1);
			}
			else
			{
				if (PosCom.stTrans.iTransNo==EC_TOPUP_NORAML)
				{
//					MakeFormatCardNo((char *)PosCom.stTrans.szCardNo, buf1);
					memset(szCardTemp, 0, sizeof(szCardTemp));
					MaskPan(PosCom.stTrans.szTranferCardNo, szCardTemp);
					MakeFormatCardNo((char *)szCardTemp, buf1);
				}
				else
				{
					if (PosCom.stTrans.iTransNo==POS_RESER_SALE || PosCom.stTrans.iTransNo==POS_RESER_VOID)
					{
						memset(szCardTemp, 0, sizeof(szCardTemp));
						memset(buf,0,sizeof(buf));
	//					memcpy(buf,"000000000000000000000",strlen((char *)PosCom.stTrans.szCardNo));
						strcpy(buf,(char *)PosCom.stTrans.szCardNo);
						MaskPan((uint8_t *)buf, szCardTemp);
						MakeFormatCardNo((char *)szCardTemp, buf1);
					}
					else
					{
						memset(szCardTemp, 0, sizeof(szCardTemp));
						MaskPan(PosCom.stTrans.szCardNo, szCardTemp);
						MakeFormatCardNo((char *)szCardTemp, buf1);
					}
				}
			}
		}
		sprintf(buf, "%s %c\n", buf1, GetEntryMode());
		PrnBigFontEngData(buf);
		
		memset(buf, 0, sizeof(buf));
		memset(buf1, 0, sizeof(buf1));
		GetBankChnName(PosCom.stTrans.szRecvBankId, (uint8_t *)buf1);
		if( stPosParam.ucPrnEng==PARAM_OPEN )
		{
//			sprintf(buf, "收单行(ACQ):%s %s\n", PosCom.stTrans.szRecvBankId, buf1);
            sprintf(buf, "收单行(ACQ):%s\n", buf1);
		}
		else
		{
//			sprintf(buf, "收单行:%s %s\n", PosCom.stTrans.szRecvBankId, buf1);
			sprintf(buf, "收单行:%s\n",  buf1);
		}

		PrnNormalFontData(buf);

		if( stPosParam.ucPrnEng==PARAM_OPEN )
			prnPrintf("交易类型(TXN. TYPE):\n");
		else
			prnPrintf("交易类型:\n");
		memset(buf1,0,sizeof(buf1));
		GetTransPrtName(PosCom.stTrans.iTransNo,buf1);
		PrnBigFontChnData(buf1);
	
		if( strlen((char *)PosCom.stTrans.szExpDate)==4 )
		{
			if( memcmp(PosCom.stTrans.szExpDate, "0000", 4)!=0 )
			{
				memset(buf1, 0, sizeof(buf1));
				memset(buf, 0, sizeof(buf));
				if( memcmp(PosCom.stTrans.szExpDate, "50", 2)>=0 )
				{
					memcpy((char*)buf1, "19", 2);
				}
				else
				{
					strcpy((char*)buf1, "20");
				}
				memcpy(buf1+2, PosCom.stTrans.szExpDate, 2);
				buf1[4] = '/';
				memcpy(buf1+5, PosCom.stTrans.szExpDate+2, 2);
				buf1[7] = 0;
				if( stPosParam.ucPrnEng==PARAM_OPEN )
					sprintf(buf, "卡有效期(EXP. DATE):   %s\n", buf1);
				else
					sprintf(buf, "卡有效期:   %s\n", buf1);

				PrnNormalFontData(buf);
			}
		}
		PrnSmallFontData("-----------------------------------------------\n");
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			PrnNormalFontData("批次号(BATCH NO)  :");
		else
			PrnNormalFontData("批次号:");

		prnPrintf("%06ld\n", PosCom.stTrans.lBatchNumber);

		if( stPosParam.ucPrnEng==PARAM_OPEN )
			prnPrintf("凭证号(VOUCHER NO):");
		else
			prnPrintf("凭证号:");

		prnPrintf("%06ld\n", PosCom.stTrans.lTraceNo);

		if( stPosParam.ucPrnEng==PARAM_OPEN )
			prnPrintf("授权码(AUTH NO)   :");
		else
			prnPrintf("授权码:");

		prnPrintf("%s\n", PosCom.stTrans.szAuthNo);

		if( stPosParam.ucPrnEng==PARAM_OPEN )
			prnPrintf("日期/时间(DATE/TIME):\n");	
		else
			prnPrintf("日期/时间:\n");	

		prnPrintf("   %.4s/%.2s/%.2s %.2s:%.2s:%.2s\n",
			&PosCom.stTrans.szDate[0], &PosCom.stTrans.szDate[4], &PosCom.stTrans.szDate[6],
			&PosCom.stTrans.szTime[0], &PosCom.stTrans.szTime[2], &PosCom.stTrans.szTime[4]);

		if( stPosParam.ucPrnEng==PARAM_OPEN )
			prnPrintf("交易参考号(REF. NO):");
		else
			prnPrintf("交易参考号:");

		prnPrintf("%s\n", PosCom.stTrans.szSysReferNo);
		// 打印金额
		PrintAmount();
		if( flag==2 )
		{	
			PrnSmallFontData("冲正不成功,请人工处理\n");
			PrnSmallFontData("***故障单,非凭证联***\n\n\n\n\n");
			prnPrintf("\f");
			PrintData();
			return;
		}

		if( stPosParam.ucPrnEng==PARAM_OPEN )
			PrnSmallFontData("备注/REFERENCE\n");
		else
			PrnSmallFontData("备注\n");
//		if( PosCom.stTrans.iTransNo==POS_SALE_VOID || PosCom.stTrans.iTransNo==POS_AUTH_VOID )
		if( PosCom.stTrans.iTransNo==POS_SALE_VOID || PosCom.stTrans.iTransNo==POS_MOBILE_VOID
			|| PosCom.stTrans.iTransNo==POS_MOTO_SALE_VOID || PosCom.stTrans.iTransNo==POS_RESER_VOID ||
			PosCom.stTrans.iTransNo==POS_MOTO_AUTH_VOID || PosCom.stTrans.iTransNo==POS_VOID_INSTAL ||
			PosCom.stTrans.iTransNo==POS_VOID_LOYATL_ALL || PosCom.stTrans.iTransNo==POS_VOID_LOYATL_ISSUER
			|| PosCom.stTrans.iTransNo==POS_AUTH_VOID || PosCom.stTrans.iTransNo==EC_VOID_TOPUP ||
			PosCom.stTrans.iTransNo==POS_MOBILE_VOID_AUTH )
		{
			if( stPosParam.ucPrnEng==PARAM_OPEN )
				prnPrintf("原凭证号(VOUCHER):%s\n", PosCom.stTrans.szPrtOldPosSer);
			else
				prnPrintf("原凭证号:%s\n", PosCom.stTrans.szPrtOldPosSer);

		}
//		else if( PosCom.stTrans.iTransNo==POS_AUTH_CM || PosCom.stTrans.iTransNo==POS_OFF_CONFIRM )
		else if( PosCom.stTrans.iTransNo==POS_AUTH_CM || PosCom.stTrans.iTransNo==POS_MOBILE_AUTH
			|| PosCom.stTrans.iTransNo==POS_MOTO_AUTH_CM )
		{
			if( stPosParam.ucPrnEng==PARAM_OPEN )
				prnPrintf("预授权码(AUTH NO):%s\n", PosCom.stTrans.szPrtOldAuthNo);
			else
				prnPrintf("预授权码:%s\n", PosCom.stTrans.szPrtOldAuthNo);

		}
		else if( PosCom.stTrans.iTransNo==POS_REFUND || PosCom.stTrans.iTransNo== POS_MOBILE_REFUND ||
			PosCom.stTrans.iTransNo==POS_MOTO_REFUND || PosCom.stTrans.iTransNo==POS_REFUND_LOYATL_ALL)
		{
			if( stPosParam.ucPrnEng==PARAM_OPEN )
			{
				prnPrintf("原参考号(REFER NO): %s\n", PosCom.stTrans.szPrtOldHostSer);
				prnPrintf("原交易日期(DATE): %s\n", PosCom.stTrans.szOldTxnDate);
			}
			else
			{
				prnPrintf("原参考号: %s\n", PosCom.stTrans.szPrtOldHostSer);
				prnPrintf("原交易日期: %s\n", PosCom.stTrans.szOldTxnDate);
			}

		}
		else if( PosCom.stTrans.iTransNo==POS_PREAUTH_VOID )
		{
			if( stPosParam.ucPrnEng==PARAM_OPEN )
				prnPrintf("原预授权码(AUTH NO):%s\n", PosCom.stTrans.szPrtOldAuthNo);
			else
				prnPrintf("原预授权码:%s\n", PosCom.stTrans.szPrtOldAuthNo);
		}

		if( PosCom.stTrans.iTransNo==POS_AUTH_VOID || PosCom.stTrans.iTransNo == POS_MOTO_PREAUTH_VOID 
			|| PosCom.stTrans.iTransNo == POS_MOTO_AUTH_VOID || PosCom.stTrans.iTransNo == POS_MOBILE_VOID_AUTH ||
			PosCom.stTrans.iTransNo == POS_MOBILE_VOID_PRE || PosCom.stTrans.iTransNo == POS_MOTO_AUTH_CM)
		{
			if( stPosParam.ucPrnEng==PARAM_OPEN )
				prnPrintf("原预授权码(AUTH NO):%s\n", PosCom.stTrans.szPrtOldAuthNo);
			else
				prnPrintf("原预授权码:%s\n", PosCom.stTrans.szPrtOldAuthNo);
		}

		PrintAddtionalInfo();
		
		if( strlen((char *)PosCom.stTrans.szIssuerResp)!=0 && memcmp(PosCom.stTrans.szIssuerResp, "          ", 10)!=0 )
			prnPrintf("发卡行返回:%s\n", PosCom.stTrans.szIssuerResp);
		if( strlen((void *)PosCom.stTrans.szCenterResp)!=0 && memcmp(PosCom.stTrans.szCenterResp, "          ", 10)!=0 )
			prnPrintf("中心返回:%s\n", PosCom.stTrans.szCenterResp);
		if( strlen((void *)PosCom.stTrans.szRecvBankResp)!=0 && memcmp(PosCom.stTrans.szRecvBankResp, "          ", 10)!=0 )
			prnPrintf("收单行返回:%s\n", PosCom.stTrans.szRecvBankResp);

		if (PosCom.stTrans.iTransNo==POS_MAGTOPUP_ACCT)
		{
			prnPrintf("转入卡号:%s\n", PosCom.stTrans.szTranferCardNo);
		}

		if( memcmp(PosCom.stTrans.szEntryMode, "05", 2)==0 )
		{
			if( PosCom.stTrans.iTransNo==POS_SALE || PosCom.stTrans.iTransNo==ICC_OFFSALE ||
				PosCom.stTrans.iTransNo==POS_PREAUTH )
			{
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
				BcdToAsc0((uint8_t *)buf, gstIccPrintData.sARQC, 16);
				prnPrintf("ARQC: %s\n", buf);

//				BcdToAsc0((uint8_t *)buf, PosCom.stTrans.sAppCrypto, 16);
//				prnPrintf("TC: %s\n", buf);
				BcdToAsc0((uint8_t *)buf, PosCom.stTrans.sTVR, 10);
				prnPrintf("TVR: %s\n", buf);
				prnPrintf("AID: %.32s\n", PosCom.stTrans.szAID);
				if( PosCom.stTrans.bPanSeqNoOk )	
				{	// TIP interoper.04-T02-SC01 
					prnPrintf("PAN SN: %02d\n",PosCom.stTrans.ucPanSeqNo);
				}
				BcdToAsc0((uint8_t *)buf, PosCom.stTrans.sTSI, 4);
				prnPrintf("TSI: %s\n", buf);
				BcdToAsc0((uint8_t *)buf, PosCom.stTrans.sATC, 4);
				prnPrintf("ATC: %s\n", buf);
				prnPrintf("APPLAB: %.16s\n", PosCom.stTrans.szAppLable);
				prnPrintf("应用首选名: %.16s\n", PosCom.stTrans.szAppLable);
			}
		}

		if( PosCom.stTrans.iTransNo== EC_QUICK_SALE || PosCom.stTrans.iTransNo== EC_NORMAL_SALE 
			|| PosCom.stTrans.iTransNo == QPBOC_ONLINE_SALE)
		{
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

			if (PosCom.stTrans.iTransNo == QPBOC_ONLINE_SALE)
			{
				BcdToAsc0((uint8_t *)buf, PosCom.stTrans.sAppCrypto, 16);
				prnPrintf("ARQC: %s\n", buf);
			}
			else
			{
				BcdToAsc0((uint8_t *)buf, PosCom.stTrans.sAppCrypto, 16);
				prnPrintf("TC: %s\n", buf);
				BcdToAsc0((uint8_t *)buf, gstIccPrintData.sTerminalCapab, 6);
				prnPrintf("TermCap: %s\n", buf);
			}
			BcdToAsc0((uint8_t *)buf, PosCom.stTrans.sTVR, 10);
			prnPrintf("TVR: %s\n", buf);
			prnPrintf("AID: %.32s\n", PosCom.stTrans.szAID);
			BcdToAsc0((uint8_t *)buf, PosCom.stTrans.sTSI, 4);
			prnPrintf("TSI: %s\n", buf);
			BcdToAsc0((uint8_t *)buf, PosCom.stTrans.sATC, 4);
			prnPrintf("ATC: %s\n", buf);
			if( PosCom.stTrans.bPanSeqNoOk )	
			{	// TIP interoper.04-T02-SC01 
				prnPrintf("PAN SN: %02d\n",PosCom.stTrans.ucPanSeqNo);
			}
			BcdToAsc0((uint8_t *)buf, gstIccPrintData.sAip, 4);
			prnPrintf("AIP: %s\n",buf);
			BcdToAsc0((uint8_t *)buf, gstIccPrintData.sTerminalRand, 8);
			prnPrintf("UNPRNUM: %s \n",buf);
			if (memcmp(gstIccPrintData.sEcIssureAuthCode,"\x00\x00\x00\x00\x00\x00",6) != 0)
			{
				//				BcdToAsc0(buf, gstIccPrintData.sEcIssureAuthCode, 12);
				memset(buf,0,sizeof(buf));
				memcpy(buf,gstIccPrintData.sEcIssureAuthCode,6);
				prnPrintf("电子现金发卡行授权码: %s\n",buf);
			}
			if (gstIccPrintData.ucIssureAppDatalen != 0)
			{
				BcdToAsc0((uint8_t *)buf, gstIccPrintData.sIssureAppData, gstIccPrintData.ucIssureAppDatalen*2);
				prnPrintf("IAD: %s\n", buf);
			}
			prnPrintf("APPLAB: %.16s\n", PosCom.stTrans.szAppLable);
			prnPrintf("应用首选名: %.16s\n", PosCom.stTrans.szAppLable);

			if (PosCom.stTrans.iTransNo== EC_NORMAL_SALE)
			{
				iRet = PubChar2Long(PosCom.stTrans.sAfterBalanceAmt,4,NULL);
				prnPrintf("余额: %d.%02d\n", (iRet/100), (iRet%100));
			}
			else
			{
				ConvBcdAmount(PosCom.stTrans.sLoyaltAmount,(uint8_t *)buf1);
				sprintf(buf, "余额: %s\n",buf1);
				PrnSmallFontData(buf);
			}
		}

		if( PosCom.stTrans.iTransNo==EC_TOPUP_CASH || PosCom.stTrans.iTransNo==EC_TOPUP_NORAML ||
			PosCom.stTrans.iTransNo==EC_TOPUP_SPEC )
		{
			prnPrintf("AID: %.32s\n", PosCom.stTrans.szAID);
			BcdToAsc0((uint8_t *)buf, PosCom.stTrans.sATC, 4);
			prnPrintf("ATC: %s\n", buf);
			iRet = PubChar2Long(PosCom.stTrans.sAfterBalanceAmt,4,NULL);
			prnPrintf("充值后卡片余额: %d.%02d\n", (iRet/100), (iRet%100));
			if (PosCom.stTrans.iTransNo==EC_TOPUP_NORAML)
			{
				prnPrintf("转出卡卡号:\n  %s\n", PosCom.stTrans.szCardNo);
/*				memset(buf1, 0, sizeof(buf1));
				memset(szCardTemp, 0, sizeof(szCardTemp));
				strcpy((char *)szCardTemp,(char *)PosCom.stTrans.szTranferCardNo);
				iRet = strlen((char *)szCardTemp);
				memset(szCardTemp+(iRet-5),'*',4);
				MakeFormatCardNo((char *)szCardTemp, buf1);
				prnPrintf("转出卡卡号:\n  %s\n", buf1);*/
			}
		}

		if( flag==REPRINT ) 
		{
			if( stPosParam.ucPrnEng==PARAM_OPEN )
				PrnNormalFontData("****重打印/DUPLICATED****\n");
			else
				PrnNormalFontData("****重打印****\n");

		}
		if( stPosParam.ucTestFlag==PARAM_OPEN )
		{
			PrnNormalFontData("***测试/TESTING,凭证无效***\n");
		}	
		
		if( j!=2 )
		{
			if( stPosParam.ucPrnEng==PARAM_OPEN )
			{
				PrnNormalFontData("持卡人签名(CARDHOLDER SIGNATURE)\n\n\n\n");
				PrnSmallFontData("-----------------------------------------------\n");			
				prnPrintf("本人确认以上交易,同意将其计入本卡账户\n");
				prnPrintf("I ACKNOWLEDGE SATISFACTORY RECEIPT OF RELATIVE  GOODS/SERVICE");
			}
			else
			{
				PrnNormalFontData("持卡人签名\n\n\n\n");
				PrnSmallFontData("-----------------------------------------------\n");			
				prnPrintf("本人确认以上交易,同意将其计入本卡账户\n");
			}
		}
		
		prnPrintf("\n\n\n\n\n");
		PrintData();
		if( j==0 && stPosParam.ucTicketNum=='1' ) //只打印商户存根
			break;
		if( (stPosParam.ucTicketNum=='2' && j==0) || (stPosParam.ucTicketNum=='3' && j==1) )
		{
			lcdClrLine(6, 7);
			lcdFlip();	
			DispMulLanguageString(0, 6, DISP_CFONT, "按确认继续打印", "[ENTER]-PRINT");
			kbGetKeyMs(2000);
		}
	}
	
	return;
}


void MakeFormatCardNo(const char *card, char *fmtCard)
{
	int i;

	memset(fmtCard, ' ', 24);
	fmtCard[24] = 0;

	for(i=0; i<strlen(card); i++)
	{
		if( i%4==0 )
			memcpy(fmtCard+5*(i/4), card+i, 4);
	}
}


uint8_t PrtTranDetailHead()
{
	if( stPosParam.ucTestFlag==PARAM_OPEN )
		PrnBigFontChnData("\n**测试/TESTING，凭证无效**\n");

	if( stPosParam.ucPrnEng==PARAM_OPEN )
	{
		PrnSmallFontData("凭证号  类型 授权码   金额  卡号\n");
		PrnSmallFontData("VOUCHER TYPE AUTH NO AMOUNT CARD NO.\n");
	}
	else
	{
		PrnSmallFontData("凭证号  类型 授权码   金额  卡号\n");
	}
	PrnSmallFontData("------------------------------------\n");

	return(OK);
}

uint8_t PrtOneTranTxn(NEWPOS_LOG_STRC *pstLog)
{
	uint8_t	ucTranType, szBuff[100], szBuf[100];
	uint8_t   szCardTemp[50];

	memset((char*)szCardTemp,0,sizeof(szCardTemp));
	memset((char*)szBuff,0,sizeof(szBuff));
	memset((char*)szBuf,0,sizeof(szBuf));
	switch( pstLog->iTransNo )
	{
	case POS_SALE:
	case ICC_OFFSALE:
	case ADJUST_TIP: 
		ucTranType = 'S';
		break;

	case POS_AUTH_CM:
		ucTranType = 'P';
		break;

	case POS_REFUND:
		ucTranType = 'R';
		break;

	case OFF_SALE:
	case OFF_ADJUST:		
		ucTranType = 'L';
		break;

	case POS_OFF_CONFIRM:
		ucTranType = 'C';
		break;

	case POS_INSTALLMENT:
		ucTranType = 'T';
		break;

	case POS_LOYATL_ALL:
	case POS_LOYATL_ISSUER:
		ucTranType = 'B';
		break;

	case EC_QUICK_SALE:
	case EC_NORMAL_SALE:
	case QPBOC_ONLINE_SALE:
	case ED_SALE:
		ucTranType = 'E';
		break;

	case EC_TOPUP_CASH:
	case EC_TOPUP_NORAML:
	case EC_TOPUP_SPEC:
	case ED_TOPUP_CASH:
	case ED_TOPUP_NORAML:
	case ED_TOPUP_SPEC:
	case POS_MAGTOPUP_CASH:
	case POS_MAGTOPUP_ACCT:
		ucTranType = 'Q';
		break;

	default:
		ucTranType = ' ';
		break;
	}

	if( pstLog->iTransNo==ADJUST_TIP )
	{
		BcdAdd(pstLog->sAmount, pstLog->sTipAmount, 6);
	}
	ConvBcdAmount(pstLog->sAmount, szBuff);

	memset(szBuf, 0, sizeof(szBuf));
	if( (pstLog->iTransNo==POS_PREAUTH) || (pstLog->iTransNo==POS_PREAUTH_ADD) ||
		(pstLog->iTransNo==OFF_SALE) || (pstLog->iTransNo==OFF_ADJUST) ||
		(pstLog->iTransNo==ICC_OFFSALE) || ( pstLog->iTransNo == EC_QUICK_SALE) ||
		( pstLog->iTransNo == EC_NORMAL_SALE) || (pstLog->iTransNo == QPBOC_ONLINE_SALE))
	{
		strcpy((char *)szCardTemp, (char *)pstLog->szCardNo);
	}
	else
	{
		MaskPan(pstLog->szCardNo, szCardTemp);
	}
	sprintf((char *)szBuf, "%06d %c %6.6s %12.12s %.19s\n", pstLog->lTraceNo, ucTranType, pstLog->szAuthNo,
			szBuff, szCardTemp);

	PrnSmallFontData((char *)szBuf);

	return OK;
}


uint8_t PrtM1EnrichTotal(uint8_t flag)
{
   	int	ret;//i, 
  //  uint8_t   buf1[40];
//	uint8_t   sRmbTotalSaleAmt[20], szRmbAmout[13],szRmbEcAmount[13];
	

	lcdClrLine(0, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, " 正在打印结算单 ", "PRINTING...");
	DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "     请稍候     ", "WAITING...");
	
	prnInit();
	PrnSmallFontData("\n\n");
	
	PrnSmallFontData("-------------------------------------------\n");
	PrnBigFontChnData("    结 算 总 计 单\n");
	PrnSmallFontData("-------------------------------------------\n");
	PrnBigFontChnData("");
	prnPrintf("公司编号:%s\n", stM1EnrichTotal.szCompanyNo);
	prnPrintf("充值点号:%s\n", stM1EnrichTotal.pointofsaleNo);
	prnPrintf("POS机编号:%s\n", stPosParam.sM1TellNo);
	if(stM1EnrichTotal.u8Settlemode==1)
	{
		prnPrintf("结算方式:普通结算\n");

	}
	else if(stM1EnrichTotal.u8Settlemode==2)
	{
		prnPrintf("结算方式:按销售日期补采\n");
		prnPrintf("销售日期补采:%s\n",stM1EnrichTotal.szDate);
	}
	else if(stM1EnrichTotal.u8Settlemode==3)
	{
		prnPrintf("结算方式:按结算日期补采\n");
		prnPrintf("结算日期补采:%s\n",stM1EnrichTotal.szDate);

	}
	prnPrintf("结算日期:%.4s/%.2s/%.2s %.2s:%.2s:%.2s\n",
			&stM1EnrichTotal.sDateTime[0], &stM1EnrichTotal.sDateTime[4], &stM1EnrichTotal.sDateTime[6],
			&stM1EnrichTotal.sDateTime[8], &stM1EnrichTotal.sDateTime[10], &stM1EnrichTotal.sDateTime[12]);
	
		PrnBigFontChnData("  类型        笔数     金额\n");

	    PrnSmallFontData("------------------------------------------\n");	

	if( stM1EnrichTotal.iRmbM1EnrichNum !=0 || stM1EnrichTotal.iRmbM1SaleNum != 0)
	{
	    PrnBigFontChnData("");
        prnPrintf("  充值        %3d     %d.%02d\n", stM1EnrichTotal.iRmbM1EnrichNum, stM1EnrichTotal.iRmbM1EnrichAmt/100,stM1EnrichTotal.iRmbM1EnrichAmt%100);	
		prnPrintf("  消费        %3d     %d.%02d\n", stM1EnrichTotal.iRmbM1SaleNum, stM1EnrichTotal.iRmbM1SaleAmt/100,stM1EnrichTotal.iRmbM1SaleAmt%100);	
				
//		i = ConvBcdAmount(stTotal.sRmbSaleAmt, buf1);
//		prnPrintf("消费/SALE                  %3d  %12s\n", stTotal.iRmbSaleNum, buf1);		
	}
	if( flag==0 ) 
	{
		PrnSmallFontData("****重打印/DUPLICATED****\n");
	}

	
		PrnBigFontChnData("\n\n\n\n");

	ret = PrintData();
	return(ret);
}


uint8_t PrtTransTotal(uint8_t flag, int batchNo)
{
   	int	i, ret;
    uint8_t   buf1[40];
	uint8_t   sRmbTotalSaleAmt[20], szRmbAmout[13],szRmbEcAmount[13];
	
	if( flag ) //打印本批总计
	{
		if( stTransCtrl.iTransNum==0 ) 
			return(E_NO_TRANS);
	}
	
	
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, " 正在打印结算单 ", "PRINTING...");
	DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "     请稍候     ", "WAITING...");
	
	prnInit();
	if( stPosParam.ucTestFlag==PARAM_OPEN )
	{
		PrnBigFontChnData("\n**测试/TESTING, 凭证无效**\n");
	}
	else 
		PrnSmallFontData("\n\n");
	
	PrnSmallFontData("-------------------------------------------\n");
	PrnBigFontChnData("    结 算 总 计 单\n");
	PrnSmallFontData("-------------------------------------------\n");
	if( stPosParam.ucPrnEng==PARAM_OPEN )
	{
		prnPrintf("商户名称(MERCHANT NAME):%s\n", stPosParam.szUnitChnName );	
		prnPrintf("商户编号(MERCHANT NO):%s\n", stPosParam.szUnitNum);	
		prnPrintf("终端编号(TERMINAL NO):%s\n", stPosParam.szPosId);
	}
	else
	{
		prnPrintf("商户名称:%s\n", stPosParam.szUnitChnName );	
		prnPrintf("商户编号:%s\n", stPosParam.szUnitNum);	
		prnPrintf("终端编号:%s\n", stPosParam.szPosId);
	}
	if( flag )
	{
		if( batchNo!=0 ) 
		{
			if( stPosParam.ucPrnEng==PARAM_OPEN )
				prnPrintf("批次号(BATCH NO):%06ld\n", batchNo);
			else
				prnPrintf("批次号:%06ld\n", batchNo);
		}
		else 
		{
			if( stPosParam.ucPrnEng==PARAM_OPEN )
				prnPrintf("批次号(BATCH NO):%06ld\n", stPosParam.lNowBatchNum);
			else
				prnPrintf("批次号:%06ld\n", stPosParam.lNowBatchNum);

		}
		prnPrintf("日期/时间:%.4s/%.2s/%.2s %.2s:%.2s:%.2s\n",
			&stTemp.szDate[0], &stTemp.szDate[4], &stTemp.szDate[6],
			&stTemp.szTime[0], &stTemp.szTime[2], &stTemp.szTime[4]);

	}
	else
	{
		prnPrintf("批次号:%06ld\n", stTotal.lBatchNum);
		prnPrintf("日期/时间:%.4s/%.2s/%.2s %.2s:%.2s:%.2s\n",
			&stTotal.sDateTime[0], &stTotal.sDateTime[4], &stTotal.sDateTime[6],
			&stTotal.sDateTime[8], &stTotal.sDateTime[10], &stTotal.sDateTime[12]);
	}
	if( stPosParam.ucPrnEng==PARAM_OPEN )
		PrnSmallFontData("类型/TYPE           笔数/SUM    金额/AMOUNT\n");
	else
		PrnSmallFontData("     类型               笔数           金额\n");

	PrnSmallFontData("-------------------------------------------\n");	
	if( flag )
	{
//		if( stTemp.ucRmbSettRsp=='2' )
//			prnPrintf("内卡: 对帐不平\n");
//		else if( stTemp.ucRmbSettRsp=='3' )
//			prnPrintf("内卡: 对帐出错\n");
//		else
//			prnPrintf("内卡: 对帐平\n");
		if( stTransCtrl.ucRmbSettSaveRsp=='2' )
			prnPrintf("内卡: 对帐不平\n");
		else if( stTransCtrl.ucRmbSettSaveRsp=='3' )
			prnPrintf("内卡: 对帐出错\n");
		else
			prnPrintf("内卡: 对帐平\n");

	}
	else 
		prnPrintf("内卡:\n");
	if( !AmountIsZero(stTotal.sRmbSaleAmt, 6) )
	{
		memset(szRmbAmout,0,sizeof(szRmbAmout));
		memset(szRmbEcAmount,0,sizeof(szRmbEcAmount));
		memset(sRmbTotalSaleAmt,0,sizeof(sRmbTotalSaleAmt));
		PubBcd2Asc0(stTotal.sRmbSaleAmt,6,szRmbAmout);
		PubBcd2Asc0(stTotal.sRmbEcAmt,6,szRmbEcAmount);
		PubAscSub(szRmbAmout,szRmbEcAmount,12,sRmbTotalSaleAmt);
		PubAsc2Bcd(sRmbTotalSaleAmt,12,szRmbAmout);
		i = ConvBcdAmount(szRmbAmout, buf1);
		prnPrintf("消费/SALE                  %3d  %12s\n", stTotal.iRmbSaleNum-stTotal.iRmbEcNum, buf1);		

//		i = ConvBcdAmount(stTotal.sRmbSaleAmt, buf1);
//		prnPrintf("消费/SALE                  %3d  %12s\n", stTotal.iRmbSaleNum, buf1);		
	}

	if( !AmountIsZero(stTotal.sRmbEcAmt, 6) )
	{
		i = ConvBcdAmount(stTotal.sRmbEcAmt, buf1);
		prnPrintf("电子现金消费/SALE          %3d  %12s\n", stTotal.iRmbEcNum, buf1);		
		
		//		i = ConvBcdAmount(stTotal.sRmbSaleAmt, buf1);
		//		prnPrintf("消费/SALE                  %3d  %12s\n", stTotal.iRmbSaleNum, buf1);		
	}

	if( !AmountIsZero(stTotal.sRmbRefundAmt, 6) )
	{
		i = ConvBcdAmount(stTotal.sRmbRefundAmt, buf1);
		prnPrintf("退货/REFUND                %3d  %12s\n", stTotal.iRmbRefundNum, buf1);
	}
	if( !AmountIsZero(stTotal.sRmbConfirmAmt, 6) )
	{
		i = ConvBcdAmount(stTotal.sRmbConfirmAmt, buf1);
		prnPrintf("完成/AUTH COMPLETE         %3d  %12s\n", stTotal.iRmbConfirmNum, buf1);
	}
	if( !AmountIsZero(stTotal.sRmbOffConfirmAmt, 6) )
	{
		i = ConvBcdAmount(stTotal.sRmbOffConfirmAmt, buf1);
		prnPrintf("完成(通知)/AUTH SETTLEMENT %3d  %12s\n", stTotal.iRmbOffConfirmNum, buf1);
	}
	if( !AmountIsZero(stTotal.sRmbOffAmt, 6) )
	{
		i = ConvBcdAmount(stTotal.sRmbOffAmt, buf1);
		prnPrintf("离线/OFFLINE               %3d  %12s\n", stTotal.iRmbOffNum, buf1);
	}
	if( !AmountIsZero(stTotal.sTopUpAmt, 6) )
	{
		i = ConvBcdAmount(stTotal.sTopUpAmt, buf1);
		prnPrintf("圈存/LOAD                  %3d  %12s\n", stTotal.iTopUpNum, buf1);		
	}
	prnPrintf("-------------------------------------------\n");	
	if( flag )
	{
//		if( stTemp.ucFrnSettRsp=='2' )
//			prnPrintf("外卡:  对帐不平\n");
//		else if( stTemp.ucFrnSettRsp=='3' )
//			prnPrintf("外卡:  对帐出错\n");
//		else
//			prnPrintf("外卡:  对帐平\n");
		if( stTransCtrl.ucFrnSettSaveRsp=='2' )
			prnPrintf("外卡:  对帐不平\n");
		else if( stTransCtrl.ucFrnSettSaveRsp=='3' )
			prnPrintf("外卡:  对帐出错\n");
		else
			prnPrintf("外卡:  对帐平\n");
	} 
	else 
		prnPrintf("外卡:\n");
	if( !AmountIsZero(stTotal.sFrnSaleAmt, 6) )
	{
		memset(szRmbAmout,0,sizeof(szRmbAmout));
		memset(szRmbEcAmount,0,sizeof(szRmbEcAmount));
		memset(sRmbTotalSaleAmt,0,sizeof(sRmbTotalSaleAmt));
		PubBcd2Asc0(stTotal.sFrnSaleAmt,6,szRmbAmout);
		PubBcd2Asc0(stTotal.sFrnEcAmt,6,szRmbEcAmount);
		PubAscSub(szRmbAmout,szRmbEcAmount,12,sRmbTotalSaleAmt);
		PubAsc2Bcd(sRmbTotalSaleAmt,12,szRmbAmout);
		i = ConvBcdAmount(szRmbAmout, buf1);
		prnPrintf("消费/SALE                  %3d  %12s\n", stTotal.iFrnSaleNum-stTotal.iFrnEcNum, buf1);		

//		i = ConvBcdAmount(stTotal.sFrnSaleAmt, buf1);
//		prnPrintf("消费/SALE                  %3d  %12s\n", stTotal.iFrnSaleNum, buf1);
	}
	
	if( !AmountIsZero(stTotal.sFrnEcAmt, 6) )
	{
		i = ConvBcdAmount(stTotal.sFrnEcAmt, buf1);
		prnPrintf("电子现金消费/SALE          %3d  %12s\n", stTotal.iFrnEcNum, buf1);		
		
		//		i = ConvBcdAmount(stTotal.sRmbSaleAmt, buf1);
		//		prnPrintf("消费/SALE                  %3d  %12s\n", stTotal.iRmbSaleNum, buf1);		
	}

	if( !AmountIsZero(stTotal.sFrnRefundAmt, 6) )
	{
		i = ConvBcdAmount(stTotal.sFrnRefundAmt, buf1);
		prnPrintf("退货/REFUND                %3d  %12s\n", stTotal.iFrnRefundNum, buf1);
	}	
	if( !AmountIsZero(stTotal.sFrnConfirmAmt, 6) )
	{	
		i = ConvBcdAmount(stTotal.sFrnConfirmAmt, buf1);
		prnPrintf("完成/AUTH COMPLETE         %3d  %12s\n", stTotal.iFrnConfirmNum, buf1);
	}
	if( !AmountIsZero(stTotal.sFrnOffConfirmAmt, 6) )
	{	
		i = ConvBcdAmount(stTotal.sFrnOffConfirmAmt, buf1);
		prnPrintf("完成(通知)/AUTH SETTLEMENT %3d  %12s\n", stTotal.iFrnOffConfirmNum, buf1);
	}
	if( !AmountIsZero(stTotal.sFrnOffAmt, 6) )
	{	
		i = ConvBcdAmount(stTotal.sFrnOffAmt, buf1);
		prnPrintf("离线/OFFLINE               %3d  %12s\n", stTotal.iFrnOffNum, buf1);
	}

	if( flag==0 ) 
	{
		prnPrintf("****重打印/DUPLICATED****\n");
	}

	if( stPosParam.ucTicketNum=='0' ) 
		prnPrintf("\f");
	else 
		prnPrintf("\n\n\n\n\n\n");

	ret = PrintData();
	return(ret);
}

uint8_t PrtNowTotal(void)
{
   	int	i, ret;
    uint8_t   buf[50], buf1[50];
	uint8_t   amtBuf[20];

	lcdCls();
	lcdFlip();	
	if( stTransCtrl.iTransNum==0 ) 
		return(E_NO_TRANS);
	
	lcdCls();
	lcdFlip();	
	lcdClrLine(2, 7);
	DispMulLanguageString(0, 3, DISP_CFONT|DISP_MEDIACY, "打印总计...", "PRINT TOTAL..");
	
	prnInit();
	prnPrintf("\n\n");
	PrnBigFontChnData("    交 易 总 计 单\n");
	
	PrnSmallFontData("-------------------------------------------\n");	
	memset(buf, 0, sizeof(buf));
	sprintf((char *)buf, "商户名称(MERCHANT NAME):%s\n", stPosParam.szUnitChnName);
	PrnSmallFontData((char *)buf);
	prnPrintf("商户编号(MERCHANT NO):%s\n", PosCom.szUnitNum);		
	prnPrintf("终端编号(TERMINAL NO):%s\n", PosCom.stTrans.szPosId);	
	prnPrintf("操作员号(OPERATOR NO):%s\n", stTransCtrl.szNowTellerNo+1);	
	prnPrintf("批次号(BATCH NO):%06ld\n", stPosParam.lNowBatchNum);
	
	prnPrintf("日期/时间:%.4s/%.2s/%.2s %.2s:%.2s:%.2s\n",
		&stTemp.szDate[0], &stTemp.szDate[4], &stTemp.szDate[6],
		&stTemp.szTime[0], &stTemp.szTime[2], &stTemp.szTime[4]);
	
	prnPrintf("--------------------------\n");	
	prnPrintf("类型  笔数     金额\n");
	prnPrintf("--------------------------\n");
	
	memset(buf, ' ', 24);
	memcpy(buf, "内卡借记  ", 10);
	sprintf((char *)buf+11, "%3d", stTotal.iRmbDebitNum);
	buf[15] = '\0';
	prnPrintf((char *)buf);
	memcpy(amtBuf, stTotal.sRmbDebitAmt, 6);
	i = ConvBcdAmount(amtBuf, buf1);
	buf1[i] = '\0';
	prnPrintf("   ");
	prnPrintf((char *)buf1);
	prnPrintf("\n");

	memset(buf, ' ', 24);
	memcpy(buf, "内卡贷记  ", 10);
	sprintf((char *)buf+11, "%3d", stTotal.iRmbCreditNum);
	buf[15] = '\0';
	prnPrintf((char *)buf);
	memcpy(amtBuf, stTotal.sRmbCreditAmt, 6);
	i = ConvBcdAmount(amtBuf, buf1);	
	buf1[i] = '\0';
	prnPrintf("   ");
	prnPrintf((char *)buf1);
	prnPrintf("\n");

	memset(buf, ' ', 24);
	memcpy(buf, "外卡借记  ", 10);
	sprintf((char *)buf+11, "%3d", stTotal.iFrnDebitNum);
	buf[15] = '\0';
	prnPrintf((char *)buf);
	memcpy(amtBuf, stTotal.sFrnDebitAmt, 6);
	i = ConvBcdAmount(amtBuf, buf1);
	buf1[i] = '\0';
	prnPrintf("   ");
	prnPrintf((char *)buf1);
	prnPrintf("\n");
		
	memset(buf, ' ', 24);
	memcpy(buf, "外卡贷记  ", 10);
	sprintf((char *)buf+11, "%3d", stTotal.iFrnCreditNum);
	buf[15] = '\0';
	prnPrintf((char *)buf);
	memcpy(amtBuf, stTotal.sFrnCreditAmt, 6);
	i = ConvBcdAmount(amtBuf, buf1);	
	buf1[i] = '\0';
	prnPrintf("   ");
	prnPrintf((char *)buf1);
	prnPrintf("\n");

	prnPrintf("--------------------------\n");
	if( stPosParam.ucTicketNum=='0' ) 
		prnPrintf("\f");
	else 
		prnPrintf("\n\n\n\n\n\n\n\n");
	
	ret = PrintData();
    return(ret);
}

uint8_t PrintOneErrOffTrans(NEWPOS_LOG_STRC *stLog)
{
	uint8_t ret;
	uint8_t buf1[30];
	int i;
	
	prnInit();
	prnPrintf("\n\n");
	PrnBigFontChnData("   故 障 报 告 单\n");
	
	PrnSmallFontData("未成功上送的离线交易\n");
	prnPrintf("凭证号: %06ld    授权号: %s", stLog->lTraceNo, stLog->szAuthNo);
	
	switch (stLog->iTransNo)
	{
	case OFF_SALE:
		prnPrintf(" 离线结算\n");
		break;
	default:
		prnPrintf(" 结算调整\n");
		break;
	}
	prnPrintf("卡号:%s\n", stLog->szCardNo);
	
	i = ConvBcdAmount(stLog->sAmount, buf1);
	prnPrintf("金额: %s\n", buf1);
	if( !AmountIsZero(stLog->sTipAmount, 6) )
	{		
		i = ConvBcdAmount(stLog->sTipAmount, buf1);
		prnPrintf("小费:%s\n", buf1);
	}

	prnPrintf("--------------------------\n");
	if( stPosParam.ucTicketNum=='0' ) 
		prnPrintf("\f");
	else 
		prnPrintf("\n\n\n\n\n\n");
	ret = PrintData();
	return(ret);
}

uint8_t PrintAllErrOffTrans(void)
{
/*	uint8_t ret;
	uint8_t hasPrt = 0;
	uint8_t buf1[30];
	NEWPOS_LOG_STRC stLog;

	int i, result;
	int fd;
	
	fd = fileOpen(POS_LOG_FILE, O_RDWR);
	if( fd<0 ) 
	{
		return (E_MEM_ERR);
	}
	
	while(1) 
	{
		result = fileRead(fd, (uint8_t *)&stLog, LOG_RECORD_LEN);
		if( result!=LOG_RECORD_LEN ) 
		{
			fileClose(fd);
			break;
		}
		
		if( stLog.ucSendFlag!=TRUE
			&& (stLog.iTransNo==OFF_SALE || stLog.iTransNo==OFF_ADJUST || stLog.iTransNo==ADJUST_TIP) ) 
		{
			prnInit();  //初始化，否则会重复打印
			if( hasPrt==0x00 )
			{			
				prnPrintf("\n\n");
				prnPrintf("  故 障 报 告 单\n");				
				prnPrintf("未成功上送的离线交易\n");
				hasPrt = 0x01;
			}
			prnPrintf("凭证号: %06ld\n授权号: %s\n", stLog.lTraceNo, stLog.szAuthNo);
			
			switch(stLog.iTransNo) 
			{
			case OFF_SALE:
				prnPrintf(" 离线结算\n");
				break;
			default:
				prnPrintf(" 结算调整\n");
				break;
			}
			prnPrintf("卡号: %s\n", stLog.szCardNo);
			
			i = ConvBcdAmount(stLog.sAmount, buf1);
			
			prnPrintf("金额: %s\n", buf1);
			if( !AmountIsZero(stLog.sTipAmount, 6) )
			{				
				i = ConvBcdAmount(stLog.sTipAmount, buf1);
				prnPrintf("小费: %s\n", buf1);
			}
			prnPrintf("\n----------------------------\n\n\n\n\n");
			ret = PrintData();
			if( ret!=OK )
			{
				fileClose(fd);
				return(ret);
			}
		}
		else 
			continue;			
	}		
	fileClose(fd);
	
	return(OK); */
	
	uint8_t ret, ucRet;
	uint8_t hasPrt = 0;
//	uint8_t buf1[30];
	NEWPOS_LOG_STRC stLog;	
	int  iRecNo, iNumOfOnePage;
	int fd;

	fd = fileOpen(POS_LOG_FILE, O_RDWR);
	if( fd<0 ) 
	{
		return (E_MEM_ERR);
	}

//	prnPrintf("TOTAL NUM :%d \n", stTransCtrl.iTransNum);

	iNumOfOnePage = 0;
	for(iRecNo=0; iRecNo<stTransCtrl.iTransNum; iRecNo++)
	{
		if (stTransCtrl.sTxnRecIndexList[iRecNo] == INV_TXN_INDEX)
		{
			continue;
		}
		
//		prnPrintf("printt stautas :%d \n", stTransCtrl.sTxnRecIndexList[iRecNo]);

		if (!(stTransCtrl.sTxnRecIndexList[iRecNo] & TS_TXN_OFFLINE) 	
			&& !(stTransCtrl.sTxnRecIndexList[iRecNo] & TS_ICC_OFFLINE))
		{
			continue;
		}

		memset(&stLog, 0, sizeof(stLog));
		ret = LoadTranLog(iRecNo, &stLog);
		if( ret )
		{
			continue;
		}
		
//		prnPrintf("printt fail code :%d \n", stLog.ucSendFail);

//		if( stLog.ucSendFlag!=TRUE
//			&& (stLog.iTransNo==OFF_SALE || stLog.iTransNo==OFF_ADJUST || stLog.iTransNo==ADJUST_TIP) ) 
		if( stLog.ucSendFail != TS_NOT_RECV)
		{
			continue;
		}

		// 打印故障单
		if( hasPrt==0x00 )
		{			
   			prnInit();
			prnPrintf((char *)"\n");
			if( stPosParam.ucPrnEng==PARAM_OPEN )
			{
				PrnSmallFontData("未成功上送交易明细/UNSUCCESSSFUL LIST\n");
			}
			else
			{
				PrnSmallFontData("      未成功上送交易明细\n");
			}
			PrtTranDetailHead();
			hasPrt = 0x01;
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

	fileClose(fd);

	// 打印应答码非00
	fd = fileOpen(POS_LOG_FILE, O_RDWR);
	if( fd<0 ) 
	{
		return (E_MEM_ERR);
	}
	
	iNumOfOnePage = 0;
	for(iRecNo=0; iRecNo<stTransCtrl.iTransNum; iRecNo++)
	{
		if (stTransCtrl.sTxnRecIndexList[iRecNo] == INV_TXN_INDEX)
		{
			continue;
		}
		
		if (!(stTransCtrl.sTxnRecIndexList[iRecNo] & TS_TXN_OFFLINE) 	
			&& !(stTransCtrl.sTxnRecIndexList[iRecNo] & TS_ICC_OFFLINE))
		{
			continue;
		}
		
		memset(&stLog, 0, sizeof(stLog));
		ret = LoadTranLog(iRecNo, &stLog);
		if( ret )
		{
			continue;
		}
		
		//		if( stLog.ucSendFlag!=TRUE
		//			&& (stLog.iTransNo==OFF_SALE || stLog.iTransNo==OFF_ADJUST || stLog.iTransNo==ADJUST_TIP) ) 
		if( stLog.ucSendFail != TS_NOT_CODE)
		{
			continue;
		}
		
		// 打印故障单
		if( hasPrt==0x00 )
		{			
			prnInit();
			prnPrintf((char *)"\n");
			if( stPosParam.ucPrnEng==PARAM_OPEN )
			{
				PrnSmallFontData("上送后被平台拒绝交易明细/DENIED LIST\n");
			}
			else
			{
				PrnSmallFontData("     上送后被平台拒绝交易明细  \n");
			}
			PrtTranDetailHead();
			hasPrt = 0x01;
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
	
	fileClose(fd);


	return 0;
}

void DispPrnErrInfo(uint8_t err)
{
	uint8_t dispStr1[20], dispStr2[20];
	
	lcdClrLine(2, 7);
	lcdFlip();	
	strcpy((char *)dispStr1, "打印失败");
	
	switch(err)
	{
	case PRN_WRONG_PACKAGE:
		strcpy((char *)dispStr2, "打印数据包格式错");
		break;
	case PRN_FAULT:
		strcpy((char *)dispStr2, "打印机故障");
		break;
	case PRN_TOOHEAT:
		strcpy((char *)dispStr2, "打印机过热");
		break;
	case PRN_UNFINISHED:
		strcpy((char *)dispStr2, "打印未完成");
		break;
	case PRN_NOFONTLIB:
		strcpy((char *)dispStr2, "打印机未装字库");
		break;
	case PRN_OUTOFMEMORY:
		strcpy((char *)dispStr2, "数据包过长");
		break;
	default:
		strcpy((char *)dispStr2, "其他错误");
		break;
	}
	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, (char *)dispStr1);
	lcdDisplay(0, 6, DISP_CFONT|DISP_MEDIACY, (char *)dispStr2);
	sysBeef(3, 1000);

}


uint8_t PrintData(void)
{
	uint8_t ret;
	uint8_t key;
	
	while(1)
	{
        ret = prnStart();
		if( ret==PRN_OK )
			return(OK);
		if( ret!=PRN_OK )
		{		
			if( ret==PRN_PAPEROUT )
			{
				CommHangUp(FALSE);
				lcdCls();
				lcdFlip();	
				DispMulLanguageString(0, 2, DISP_MEDIACY|DISP_CFONT, "打印机缺纸...", "PAPEROUT...");
				DispMulLanguageString(0, 4, DISP_MEDIACY|DISP_CFONT, "按<确认>打印", "[ENTER]-PRINT");
				DispMulLanguageString(0, 6, DISP_MEDIACY|DISP_CFONT, "按<取消>退出", "[CANCEL]-EXIT");
				sysBeef(2, 1000);				
				do{
					key = kbGetKey();
				}while( key!=KEY_ENTER && key!=KEY_CANCEL );
				if( key==KEY_ENTER )
					continue;
				else 
					return(PRN_PAPEROUT);
			}
			
			DispPrnErrInfo(ret);
			kbGetKey();
			return(NO_DISP);
		}
	}
}

//依据银行编码得到该银行的中文名称
int GetBankChnName(uint8_t * id, uint8_t *bankName)
{
	int i;

	for(i=0; i<MAX_ACQUIRER_NUM; i++) 
	{
		if( memcmp(sAcqList[i].szBankId, id, 4)==0 ) 
			break;
	}
	if( i==MAX_ACQUIRER_NUM ) 
		strcpy((char *)bankName, (char *)id);
	else
		strcpy((char *)bankName, (char *) sAcqList[i].szBankName);
	return OK;
}

uint8_t GetEntryMode(void)
{
	if (PosCom.stTrans.iTransNo == EC_TOPUP_NORAML)
	{
		if (PosCom.stTrans.ucEcTopOldSwipeFlag== CARD_INSERTED)
		{
			return 'I';
		}
		else
		{
			return 'S';
		}
	}
	if (PosCom.stTrans.iTransNo==POS_RESER_SALE || PosCom.stTrans.iTransNo==POS_RESER_VOID)
	{
		return 'N';
	}
	if( PosCom.stTrans.ucFallBack )
	{
//		return 'F';
		return 'S';
	}
	if( PosCom.stTrans.szEntryMode[1]=='1' )
	{
		return 'M';
	}
	if( PosCom.stTrans.szEntryMode[1]=='2' )
	{
		return 'S';
	}
	if( PosCom.stTrans.szEntryMode[1]=='5' )
	{
		return 'I';
	}
	if (PosCom.stTrans.szEntryMode[1]=='7')
	{
		return 'C';
	}

	if (memcmp(PosCom.stTrans.szEntryMode,"96",2)==0)
	{
		return 'C';
	}

	return ' ';
}

void PrnEmvParam(void)
{
	int		iRet, iCnt;
	uint8_t		szBuff[40];
	struct terminal_aid_info stEmvApp;
	struct issuer_ca_public_key	stEmvCapk;
	int       fd, fileLen, Num;

	prnInit();
	PrnBigFontChnData("本机银联EMV参数信息\n");
	emv_get_mck_configure(&tConfig);
	memset(szBuff, 0, sizeof(szBuff));
	sprintf((char *)szBuff, "商户类型: %02X %02X\n", tEMVCoreInit.merchant_cate_code[0], tEMVCoreInit.merchant_cate_code[1]);
	PrnSmallFontData((char *)szBuff);
	prnPrintf("终端类型: %02X\n", tConfig.terminal_type);
	prnPrintf("终端能力: %02X %02X %02X\n", tConfig.terminal_capabilities[0], tConfig.terminal_capabilities[1], 
		tConfig.terminal_capabilities[2]);
	prnPrintf("终端扩展能力: %02X %02X %02X %02X %02X\n", tConfig.additional_terminal_capabilities[0], 
		tConfig.additional_terminal_capabilities[1], tConfig.additional_terminal_capabilities[2], 
		tConfig.additional_terminal_capabilities[3], tConfig.additional_terminal_capabilities[4]);
	prnPrintf("国家代码: %02X %02X\n", tEMVCoreInit.terminal_country_code[0], tEMVCoreInit.terminal_country_code[1]);

	prnPrintf("EMV应用列表\n");
	for(iCnt=0; iCnt<MAX_APP_NUM; iCnt++)
	{
		memset(&stEmvApp, 0, sizeof(stEmvApp));
		iRet = emv_get_one_aid_info(iCnt, &stEmvApp);
		if( iRet<0 )
		{
			continue;
		}
		BcdToAsc0(szBuff, stEmvApp.aid, stEmvApp.aid_len*2);
		prnPrintf("AID: %s\n", szBuff);
		prnPrintf("Floor Limit: %d.%02d\n", stEmvApp.terminal_floor_limit/100, stEmvApp.terminal_floor_limit%100);
		BcdToAsc0(szBuff, stEmvApp.terminal_action_code_denial, 10);
		prnPrintf("TAC Denial:  %s\n", szBuff);
		BcdToAsc0(szBuff, stEmvApp.terminal_action_code_online, 10);
		prnPrintf("TAC Online:  %s\n", szBuff);
		BcdToAsc0(szBuff, stEmvApp.terminal_action_code_default, 10);
		prnPrintf("TAC Default: %s\n", szBuff);
		prnPrintf("EC MAX AMT: %d.%02d\n", stEmvApp.cl_ReaderMaxTransAmount/100,stEmvApp.cl_ReaderMaxTransAmount%100);
		prnPrintf("EC Floor limit: %d.%02d\n", stEmvApp.cl_Floor_Limit/100,stEmvApp.cl_Floor_Limit%100);
		prnPrintf("EC CVM Amt: %d.%02d\n\n", stEmvApp.cl_CVM_Amount/100,stEmvApp.cl_CVM_Amount%100);
	}
	
	prnPrintf("\nEMV公钥列表\n");
    fd = fileOpen("emv.capk", O_RDWR);
    if( fd>=0 )
    {
        fileLen = fileSize("emv.capk");
        Num = fileLen/sizeof(struct issuer_ca_public_key);

        for(iCnt=0; iCnt<Num; iCnt++)
        {
            fileSeek(fd, iCnt*sizeof(struct issuer_ca_public_key), SEEK_SET);
            fileRead(fd, (uint8_t*)&stEmvCapk, sizeof(struct issuer_ca_public_key));
            BcdToAsc0(szBuff, stEmvCapk.RID, 10);
    		prnPrintf("RID: %s, Key ID:%02X, ", szBuff, stEmvCapk.Index);
    		BcdToAsc0(szBuff, stEmvCapk.expiration_date, 6);
    		prnPrintf("Expiry: 20%.2s-%.2s-%.2s\n", szBuff, &szBuff[2], &szBuff[4]);
        }
        fileClose(fd);
    }


	prnPrintf("\n\n\n");

	PrintData();
}


void  GetTransPrtName(int iTransType, char *pszTransName)
{
	char  szBuffer[100];

	memset(szBuffer,0,sizeof(szBuffer));
	switch(iTransType) 
	{
	case POS_SALE:
	case ICC_OFFSALE:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"  消费(SALE)\n");
		else
			strcpy(szBuffer,"  消费\n");				
		break;
	case POS_SALE_VOID:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"  消费撤销(VOID)\n");
		else
			strcpy(szBuffer,"  消费撤销\n");				
		break;
	case POS_AUTH_VOID:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"预授权完成撤销(COMPLETE VOID)\n");
		else
			strcpy(szBuffer,"  预授权完成撤销\n");				
		break;
	case POS_PREAUTH_VOID:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"   预授权撤销(CANCEL)\n");
		else
			strcpy(szBuffer,"  预授权撤销\n");				
		break;
	case POS_OFF_CONFIRM:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"预授权完成通知(AUTH SETTLEMENT)\n");
		else
			strcpy(szBuffer,"  预授权完成通知\n");				
		break;
	case POS_AUTH_CM:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"预授权完成请求(AUTH COMPLETE)\n");
		else
			strcpy(szBuffer,"  预授权完成请求\n");				
		break;
	case POS_REFUND:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"  退货(REFUND)\n");
		else
			strcpy(szBuffer,"  退货\n");				
		break;
	case POS_PREAUTH:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer," 预授权(AUTH)\n");
		else
			strcpy(szBuffer,"  预授权\n");				
		break;
	case POS_PREAUTH_ADD:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"追加预授权(ADD-AUTH)\n");
		else
			strcpy(szBuffer,"  追加预授权\n");				
		break;
	case OFF_SALE:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"离线结算(OFFLINE)\n");
		else
			strcpy(szBuffer,"  离线结算\n");				
		break;
	case OFF_ADJUST:
	case ADJUST_TIP:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"结算调整(ADJUST)\n");
		else
			strcpy(szBuffer,"  结算调整\n");				
		break;

	case ED_SALE:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"电子钱包消费(EP SALE)\n");
		else
			strcpy(szBuffer,"  电子钱包消费\n");				
		break;

	case POS_INSTALLMENT:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"分期付款消费(INSTALLMENT)\n");
		else
			strcpy(szBuffer,"  分期付款消费\n");				
		break;

	case POS_VOID_INSTAL:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"分期付款消费撤销(VOID)\n");
		else
			strcpy(szBuffer,"  分期付款消费撤销\n");				
		break;

	case POS_LOYATL_ALL:
	case POS_LOYATL_ISSUER:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"积分消费(BONUS)\n");
		else
			strcpy(szBuffer,"  积分消费\n");				
		break;
		
	case POS_VOID_LOYATL_ALL:
	case POS_VOID_LOYATL_ISSUER:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"积分消费撤销(VOID)\n");
		else
			strcpy(szBuffer,"  积分消费撤销\n");				
		break;

	case EC_QUICK_SALE:
	case EC_NORMAL_SALE:
//	case QPBOC_ONLINE_SALE:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"电子现金消费(EC SALE)\n");
		else
			strcpy(szBuffer,"  电子现金消费\n");				
		break;

	case QPBOC_ONLINE_SALE:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"  消费(SALE)\n");
		else
			strcpy(szBuffer,"  消费\n");				
		break;

	case POS_RESER_SALE:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"预约消费(RESERVATION SALE)\n");
		else
			strcpy(szBuffer,"  预约消费\n");				
		break;

	case POS_RESER_VOID:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"预约消费撤销(VOID)\n");
		else
			strcpy(szBuffer,"  预约消费撤销\n");				
		break;

	case POS_MOTO_SALE:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"订购消费(MOTO SALE)\n");
		else
			strcpy(szBuffer,"  订购消费\n");				
		break;
		
	case POS_MOTO_SALE_VOID:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"订购消费撤销(VOID)\n");
		else
			strcpy(szBuffer,"  订购消费撤销\n");				
		break;

	case POS_MOTO_AUTH_OFF:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"订购预授权完成通知(AUTH SETTLEMENT)\n");
		else
			strcpy(szBuffer,"  订购预授权完成通知\n");				
		break;

	case POS_MOTO_AUTH_CM:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"订购预授权完成请求(AUTH COMPLETE)\n");
		else
			strcpy(szBuffer,"  订购预授权完成请求\n");				
		break;

	case POS_MOTO_AUTH_VOID:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"订购预授权完成撤销(COMPLETE VOID)\n");
		else
			strcpy(szBuffer,"  订购预授权完成撤销\n");				
		break;

	case POS_MOTO_REFUND:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"订购退货(REFUND)\n");
		else
			strcpy(szBuffer,"  订购退货\n");				
		break;

	case EC_TOPUP_NORAML:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"电子现金非指定账户圈存(EC LOAD)\n");
		else
			strcpy(szBuffer,"  电子现金非指定账户圈存\n");				
		break;

	case EC_TOPUP_SPEC:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"电子现金指定账户圈存(EC LOAD)\n");
		else
			strcpy(szBuffer,"  电子现金指定账户圈存\n");				
		break;

	case EC_TOPUP_CASH:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"电子现金现金充值(EC LOAD)\n");
		else
			strcpy(szBuffer,"  电子现金现金充值\n");				
		break;
		
	case EC_VOID_TOPUP:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"电子现金充值撤销(EC LOAD VOID)\n");
		else
			strcpy(szBuffer,"  电子现金充值撤销\n");				
		break;

	case POS_MAGTOPUP_CASH:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"磁条卡现金充值(ACCOUNT LOAD)\n");
		else
			strcpy(szBuffer,"  磁条卡现金充值\n");				
		break;
		
	case POS_MAGTOPUP_ACCT:
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			strcpy(szBuffer,"磁条卡账户充值(ACCOUNT LOAD)\n");
		else
			strcpy(szBuffer,"  磁条卡账户充值\n");				
		break;

	default:
		if (iTransType >= EC_QUICK_SALE && iTransType <= POS_LYT_LOGON)
		{
			GetTransName(iTransType,(uint8_t*)szBuffer);
		}
		break;
	}

	if (pszTransName != NULL)
	{
		sprintf(pszTransName,"%s\n",szBuffer);
	}

	return ;
}


void  PrintAmount(void)
{
	char  buf[200], buf1[200];

	memset(buf1,0,sizeof(buf1));
	memset(buf,0,sizeof(buf));
	if (PosCom.stTrans.iTransNo == POS_INSTALLMENT || PosCom.stTrans.iTransNo == POS_VOID_INSTAL)
	{
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			prnPrintf("金额(AMOUNT):\n");
		else
			prnPrintf("金额:\n");
		
		memset(buf1, 0, sizeof(buf1));
		memset(buf, 0, sizeof(buf));
		ConvBcdAmount(PosCom.stTrans.sAmount, (uint8_t *)buf1);
		sprintf(buf, "RMB:%s\n", buf1);
		PrnBigFontEngData(buf);

/*		sprintf(buf,"分期期数: %d\n",PosCom.stTrans.ucInstallmentNum);
		PrnNormalFontData(buf);
		if (!PosCom.stTrans.ucInstalPayMode)
		{
			prnPrintf("分期还款金额:\n");
			memset(buf1, 0, sizeof(buf1));
			memset(buf, 0, sizeof(buf));
			ConvBcdAmount(PosCom.stTrans.sTipAmount,(uint8_t *)buf1);
			sprintf(buf, "RMB%s\n", buf1);
			PrnBigFontEngData(buf);	

			sprintf(buf,"分期币种: %3.3s\n",PosCom.stTrans.szInstalCurrCode);
			PrnNormalFontData(buf);
		}
		else
		{
			prnPrintf("分期付款首期金额:\n");
			memset(buf1, 0, sizeof(buf1));
			memset(buf, 0, sizeof(buf));
			ConvBcdAmount(PosCom.stTrans.sTipAmount,(uint8_t *)buf1);
			sprintf(buf, "RMB%s\n", buf1);
			PrnBigFontEngData(buf);	
			sprintf(buf,"分期币种: %3.3s\n",PosCom.stTrans.szInstalCurrCode);
			PrnNormalFontData(buf);
			prnPrintf("持卡人手续费:\n");
			memset(buf1, 0, sizeof(buf1));
			memset(buf, 0, sizeof(buf));
			ConvBcdAmount(PosCom.stTrans.sTipAmount,(uint8_t *)buf1);
			sprintf(buf, "%3.3s%s\n", PosCom.stTrans.szInstalCurrCode,buf1);
			PrnBigFontEngData(buf);	
		}*/

		PrnSmallFontData("-----------------------------------------------\n");
		return;
	}

	if (PosCom.stTrans.iTransNo == POS_LOYATL_ALL || PosCom.stTrans.iTransNo == POS_LOYATL_ISSUER)
	{
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			prnPrintf("金额(AMOUNT):\n");
		else
			prnPrintf("金额:\n");
		
		memset(buf1, 0, sizeof(buf1));
		memset(buf, 0, sizeof(buf));
		ConvBcdAmount(PosCom.stTrans.sAmount, (uint8_t *)buf1);
		sprintf(buf, "RMB:%s\n", buf1);
		PrnBigFontEngData(buf);		

/*		sprintf(buf,"商品代码: %s\n",PosCom.stTrans.szInstalCode);
		PrnNormalFontData(buf);
		prnPrintf("兑换积分分数:\n");
		memset(buf1, 0, sizeof(buf1));
		memset(buf, 0, sizeof(buf));
		PubBcd2Asc0(PosCom.stTrans.sTipAmount,5,buf);
		PubAddHeadChars(buf,12,'0');
		PubConvAmount(NULL,(uint8_t *)buf,0,(uint8_t *)buf1,0);
		sprintf(buf, "   %s\n", buf1);
		PrnBigFontEngData(buf);	
		PrnNormalFontData("自付金金额:\n");
		memset(buf1, 0, sizeof(buf1));
		memset(buf, 0, sizeof(buf));
		ConvBcdAmount(PosCom.stTrans.sPreAddAmount,(uint8_t *)buf1);
		sprintf(buf, "RMB%s\n", buf1);
		PrnBigFontEngData(buf);
		PrnNormalFontData("积分余额:\n");
		memset(buf1, 0, sizeof(buf1));
		memset(buf, 0, sizeof(buf));
		PubBcd2Asc0(PosCom.stTrans.sLoyaltAmount,5,buf);
		PubAddHeadChars(buf,12,'0');
		PubConvAmount(NULL,(uint8_t *)buf,0,(uint8_t *)buf1,0);
		sprintf(buf, "   %s\n", buf1);
		PrnBigFontEngData(buf);	*/					
		PrnSmallFontData("-----------------------------------------------\n");
		return;
	}

	if (PosCom.stTrans.iTransNo == POS_VOID_LOYATL_ALL || PosCom.stTrans.iTransNo == POS_VOID_LOYATL_ISSUER)
	{
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			prnPrintf("金额(AMOUNT):\n");
		else
			prnPrintf("金额:\n");
		
		memset(buf1, 0, sizeof(buf1));
		memset(buf, 0, sizeof(buf));
		ConvBcdAmount(PosCom.stTrans.sAmount, (uint8_t *)buf1);
		sprintf(buf, "RMB:%s\n", buf1);
		PrnBigFontEngData(buf);		

/*		PrnNormalFontData("兑换积分分数:\n");
		memset(buf1, 0, sizeof(buf1));
		memset(buf, 0, sizeof(buf));
		PubBcd2Asc0(PosCom.stTrans.sTipAmount,5,buf);
		PubAddHeadChars(buf,12,'0');
		PubConvAmount(NULL,(uint8_t *)buf,0,(uint8_t *)buf1,0);
		sprintf(buf, "   %s\n", buf1);
		PrnBigFontEngData(buf);	
		PrnNormalFontData("自付金金额:\n");
		memset(buf1, 0, sizeof(buf1));
		memset(buf, 0, sizeof(buf));
		ConvBcdAmount(PosCom.stTrans.sPreAddAmount,(uint8_t *)buf1);
		sprintf(buf, "RMB%s\n", buf1);
		PrnBigFontEngData(buf);*/
		PrnSmallFontData("-----------------------------------------------\n");
		return;
	}

	if ((PosCom.stTrans.iTransNo==POS_SALE || PosCom.stTrans.iTransNo==POS_LOYATL_ALL)
		&& PosCom.stTrans.ucInstalPayMode == 0x03)
	{
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			prnPrintf("承兑金额(AMOUNT):\n");
		else
			prnPrintf("承兑金额:\n");
	}
	else
	{
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			prnPrintf("金额(AMOUNT):\n");
		else
			prnPrintf("金额:\n");
	}
	
	memset(buf1, 0, sizeof(buf1));
	memset(buf, 0, sizeof(buf));
	ConvBcdAmount(PosCom.stTrans.sAmount, (uint8_t *)buf1);
	sprintf(buf, "RMB:%s\n", buf1);
	PrnBigFontEngData(buf);
	if( PosCom.stTrans.iTransNo==POS_PREAUTH_ADD )
	{
		if( !AmountIsZero(PosCom.stTrans.sPreAddAmount, 6) )
		{
			if( stPosParam.ucPrnEng==PARAM_OPEN )
				PrnNormalFontData("累计金额(TOTAL)  :\n");
			else
				PrnNormalFontData("累计金额:\n");
			
			memset(buf1, 0, sizeof(buf1));
			memset(buf, 0, sizeof(buf));
			ConvBcdAmount(PosCom.stTrans.sPreAddAmount, (uint8_t *)buf1);
			sprintf(buf, "RMB:%s\n", buf1);
			PrnBigFontEngData(buf);
		}
	}
	
	if( !AmountIsZero(PosCom.stTrans.sTipAmount, 6) )
	{
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			PrnNormalFontData("小费(TIPS)  :\n");
		else
			PrnNormalFontData("TIPS  :\n");
		
		memset(buf1, 0, sizeof(buf1));
		memset(buf, 0, sizeof(buf));
		ConvBcdAmount(PosCom.stTrans.sTipAmount, (uint8_t *)buf1);
		sprintf(buf, "RMB:%s\n", buf1);
		PrnBigFontEngData(buf);
		if( stPosParam.ucPrnEng==PARAM_OPEN )
			PrnNormalFontData("总计(TOTAL) :\n");
		else
			PrnNormalFontData("TOTAL :\n");
		
		memcpy(buf1, PosCom.stTrans.sAmount, 6);
		BcdAdd((uint8_t *)buf1, (uint8_t *)PosCom.stTrans.sTipAmount, 6);
		ConvBcdAmount((uint8_t *)buf1, (uint8_t *)buf);
		sprintf(buf1, "RMB:%s\n", buf);
		PrnBigFontEngData(buf1);
	}
	
	PrnSmallFontData("-----------------------------------------------\n");
}


void  PrintAddtionalInfo(void)
{
	char  buf[200], buf1[200];
	int   ilen;
	uint8_t szCardTemp[20],szFlag[3];

	memset(buf1,0,sizeof(buf1));
	memset(buf,0,sizeof(buf));
	if (PosCom.stTrans.iTransNo == POS_INSTALLMENT || PosCom.stTrans.iTransNo == POS_VOID_INSTAL)
	{
		sprintf(buf,"分期期数: %d\n",PosCom.stTrans.ucInstallmentNum);
		PrnSmallFontData(buf);
		if (!PosCom.stTrans.ucInstalPayMode)
		{
			PrnSmallFontData("分期付款首期金额: ");
			memset(buf1, 0, sizeof(buf1));
			memset(buf, 0, sizeof(buf));
			ConvBcdAmount(PosCom.stTrans.sTipAmount,(uint8_t *)buf1);
			sprintf(buf, "%s\n", buf1);
			PrnSmallFontData(buf);	

			sprintf(buf,"分期币种: %3.3s\n",PosCom.stTrans.szInstalCurrCode);
			PrnSmallFontData(buf);

			PrnSmallFontData("持卡人手续费: ");
			memset(buf1, 0, sizeof(buf1));
			memset(buf, 0, sizeof(buf));
			ConvBcdAmount(PosCom.stTrans.sPreAddAmount,(uint8_t *)buf1);
			sprintf(buf, "%s\n", buf1);
			PrnSmallFontData(buf);
		}
		else
		{
			PrnSmallFontData("分期付款首期金额: ");
			memset(buf1, 0, sizeof(buf1));
			memset(buf, 0, sizeof(buf));
			ConvBcdAmount(PosCom.stTrans.sTipAmount,(uint8_t *)buf1);
			sprintf(buf, "%s\n", buf1);
			PrnSmallFontData(buf);
			sprintf(buf,"分期币种: %3.3s\n",PosCom.stTrans.szInstalCurrCode);
			PrnSmallFontData(buf);
			PrnSmallFontData("首期手续费:");
			memset(buf1, 0, sizeof(buf1));
			memset(buf, 0, sizeof(buf));
			if (!(PosCom.stTrans.ucInstalAmtFlag & TS_INST_SPAC))
			{
				ConvBcdAmount(PosCom.stTrans.sPreAddAmount,(uint8_t *)buf1);
				sprintf(buf, "%s\n", buf1);
				PrnSmallFontData(buf);				
			}
			else
			{
				sprintf(buf, "\n");
				PrnSmallFontData(buf);
			}
			PrnSmallFontData("每期手续费:");
			memset(buf1, 0, sizeof(buf1));
			memset(buf, 0, sizeof(buf));

			if (!(PosCom.stTrans.ucInstalAmtFlag & TS_FEE_SPAC))
			{
				PubBcd2Asc0(PosCom.stTrans.sLoyaltAmount,6,buf);
				ConvBcdAmount(PosCom.stTrans.sLoyaltAmount,(uint8_t *)buf1);
				sprintf(buf, "%s\n",buf1);
				PrnSmallFontData(buf);	
			}
			else
			{
				sprintf(buf, "\n");
				PrnSmallFontData(buf);
			}
		}

/*		if (strlen((char *)PosCom.stTrans.szInstalIssurResp)!=0)
		{
			sprintf(buf, "%s\n", PosCom.stTrans.szInstalIssurResp);
			PrnSmallFontData(buf);	
		}*/

		return;
	}

	if (PosCom.stTrans.iTransNo == POS_LOYATL_ALL || PosCom.stTrans.iTransNo == POS_LOYATL_ISSUER)
//	if (PosCom.stTrans.iTransNo == POS_LOYATL_ISSUER)
	{		
		sprintf(buf,"商品代码: %s\n",PosCom.stTrans.szInstalCode);
		PrnSmallFontData(buf);
		PrnSmallFontData("兑换积分分数:");
		memset(buf1, 0, sizeof(buf1));
		memset(buf, 0, sizeof(buf));
		PubBcd2Asc0(PosCom.stTrans.sTipAmount,5,buf);
		PubAddHeadChars(buf,12,'0');
		PubConvAmount(NULL,(uint8_t *)buf,0,(uint8_t *)buf1,0);
		sprintf(buf, "   %s\n", buf1);
		PrnSmallFontData(buf);	
		PrnSmallFontData("自付金金额:");
		memset(buf1, 0, sizeof(buf1));
		memset(buf, 0, sizeof(buf));
		ConvBcdAmount(PosCom.stTrans.sPreAddAmount,(uint8_t *)buf1);
		sprintf(buf, "%s\n", buf1);
		PrnSmallFontData(buf);
		PrnSmallFontData("积分余额:");
		memset(buf1, 0, sizeof(buf1));
		memset(buf, 0, sizeof(buf));
		PubBcd2Asc0(PosCom.stTrans.sLoyaltAmount,6,buf);
		PubAddHeadChars(buf,12,'0');
		PubConvAmount(NULL,(uint8_t *)buf,0,(uint8_t *)buf1,0);

		memset(szFlag,0,sizeof(szFlag));
		szFlag[0] = (PosCom.stTrans.ucInstalAmtFlag!='C') ? '-' : ' ';
		sprintf(buf, "  %s%s\n", szFlag,buf1);
		PrnSmallFontData(buf);						
		return;
	}

	if (PosCom.stTrans.iTransNo == POS_VOID_LOYATL_ALL || PosCom.stTrans.iTransNo == POS_VOID_LOYATL_ISSUER)
//	if (PosCom.stTrans.iTransNo == POS_VOID_LOYATL_ISSUER)
	{		
		PrnSmallFontData("兑换积分分数:");
		memset(buf1, 0, sizeof(buf1));
		memset(buf, 0, sizeof(buf));
		PubBcd2Asc0(PosCom.stTrans.sTipAmount,5,buf);
		PubAddHeadChars(buf,12,'0');
		PubConvAmount(NULL,(uint8_t *)buf,0,(uint8_t *)buf1,0);
		sprintf(buf, "   %s\n", buf1);
		PrnSmallFontData(buf);	
		PrnSmallFontData("自付金金额:");
		memset(buf1, 0, sizeof(buf1));
		memset(buf, 0, sizeof(buf));
		ConvBcdAmount(PosCom.stTrans.sPreAddAmount,(uint8_t *)buf1);
		sprintf(buf, "RMB%s\n", buf1);
		PrnSmallFontData(buf);
		return;
	}

	if (PosCom.stTrans.iTransNo == POS_RESER_SALE || PosCom.stTrans.iTransNo == POS_RESER_VOID)
	{
		memset(szCardTemp, 0, sizeof(szCardTemp));
		memset(buf1, 0, sizeof(buf1));
		memcpy(szCardTemp,PosCom.stTrans.szInstalCode,11);
		PubTrimTailChars((char *)szCardTemp,' ');
		ilen = strlen((char *)szCardTemp);
		memset(szCardTemp+3,'*',ilen-6);
		MakeFormatCardNo((char *)szCardTemp, buf1);
		prnPrintf("预约手机号: %s\n",buf1);
	}

/*	if (PosCom.stTrans.iTransNo == POS_LOYATL_ISSUER)
	{		
		PrnSmallFontData("积分余额:");
		memset(buf1, 0, sizeof(buf1));
		memset(buf, 0, sizeof(buf));
		PubBcd2Asc0(PosCom.stTrans.sLoyaltAmount,5,buf);
		PubAddHeadChars(buf,12,'0');
		PubConvAmount(NULL,(uint8_t *)buf,0,(uint8_t *)buf1,0);
		sprintf(buf, "   %s\n", buf1);
		PrnSmallFontData(buf);						
		return;
	}*/
}
void PrtM1TranEnrichTicket(INT32 flag)
{
	UINT8 j;
	UINT8 uRet;
	INT32 iRet;
	unsigned long ultemp;
	long ltmp;
	INT8  buf[30];
	INT8 sTimeStr[15],pszDate[20];
	UINT8 buf_1[50], buf_2[50];
	uint8_t buf_3[50]; 

	lcdClrLine(2, 7);
	DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "	 打印凭单中   ", "PRINT...");
	sysGetTime(&sTimeStr[1]);
	sTimeStr[0]=0x20;
	BcdToAsc0(&pszDate[0], sTimeStr, 14);
	for(j=0; j<3; j++)
	{
		if( flag!=2 )
		{
			lcdClrLine(6, 7);
			DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "	 打印凭单中   ", "PRINT...");
		}

		if( j==1 && stPosParam.ucTicketNum=='2' ) //不打印银行存根
			continue;
		
		iRet = prnInit();
		if( iRet!=OK )
		{
			lcdClrLine(2, 7);
			DispMulLanguageString(0, 3, DISP_CFONT, "打印机初始化失败", "INIT PRINT FAIL");
			kbGetKey();
			return ;
		}
		if( flag==2 )
		{
			PrnNormalFontData("\n**充值故障报告单,非凭证联**\n------------------------\n");
		}

		if( stPosParam.ucTestFlag==PARAM_OPEN )
		{
			PrnNormalFontData("\n**测试/TESTING,凭证无效**\n");
		}

		PrnNormalFontData("      银川公交IC卡收费系统  \n");
		PrnNormalFontData("   脱机充值移动POS机充值凭证\n");
		prnPrintf("\n操  作: 客票充值\n");
		
		prnPrintf("卡  号: ");
		memset(buf_1, 0, sizeof(buf_1));
		memcpy(buf_1,gstYktEnrichLog.sCardSerialNo+8,8);
		PubAsc2Bcd(buf_1, 8, buf_3);
		ltmp = buf_3[0]*256*256*256 + buf_3[1]*256*256 + buf_3[2]*256 + buf_3[3];
		sprintf((INT8 *)buf, "%08d", ltmp);
		prnPrintf("%s\n", buf);
		
		prnPrintf("芯片号: ");
		memset(buf, 0, sizeof(buf));
NacTxd(gstYktEnrichLog.sCSN,4);
	 	sprintf((char *)buf, "%02X%02X%02X%02X",
			gstYktEnrichLog.sCSN[0], gstYktEnrichLog.sCSN[1],gstYktEnrichLog.sCSN[2],gstYktEnrichLog.sCSN[3]);
		prnPrintf("%s\n", buf);

		prnPrintf("卡类型：");
		if(gstYktEnrichLog.uCardtype==0x00)
		{
			lcdDisplay(0, 4, DISP_CFONT, "卡类型:普通卡");
			prnPrintf("普通卡\n");
		}
		 else if(gstYktEnrichLog.uCardtype==1)
		{
			lcdDisplay(0, 4, DISP_CFONT, "卡类型:老人卡 A");
			prnPrintf("老人卡 A\n");
		}
		 else if(gstYktEnrichLog.uCardtype==2)
		{
			lcdDisplay(0, 4, DISP_CFONT, "卡类型:老人卡 B");
			prnPrintf("老人卡 B\n");
		}
		 else if(gstYktEnrichLog.uCardtype==3)
		{
			lcdDisplay(0, 4, DISP_CFONT, "卡类型:学生卡 A");
			prnPrintf("学生卡 A\n");
		}
		 else if(gstYktEnrichLog.uCardtype==4)
		{
			lcdDisplay(0, 4, DISP_CFONT, "卡类型:学生卡 B");
			prnPrintf("学生卡 B\n");
		}
		else if(gstYktEnrichLog.uCardtype==5)
		{
			lcdDisplay(0, 4, DISP_CFONT, "卡类型:预留卡 A");
			prnPrintf("预留卡 A\n");
		}
		else if(gstYktEnrichLog.uCardtype==6)
		{
			lcdDisplay(0, 4, DISP_CFONT, "卡类型:预留卡 B");
			prnPrintf("预留卡 B\n");
		}
		else if(gstYktEnrichLog.uCardtype==7)
		{
			lcdDisplay(0, 4, DISP_CFONT, "卡类型:CPU卡");
			prnPrintf("CPU卡\n");
		}
		else if(gstYktEnrichLog.uCardtype==8)
		{
			lcdDisplay(0, 4, DISP_CFONT, "卡类型:员工卡");
			prnPrintf("员工卡\n");
		}
		else if(gstYktEnrichLog.uCardtype==9)
		{
			lcdDisplay(0, 4, DISP_CFONT, "卡类型:爱心卡");
			prnPrintf("爱心卡\n");
		}
		else if(gstYktEnrichLog.uCardtype==10)
		{
			lcdDisplay(0, 4, DISP_CFONT, "卡类型:优待卡");
			prnPrintf("优待卡\n");
		}
		else if(gstYktEnrichLog.uCardtype==11)
		{
			lcdDisplay(0, 4, DISP_CFONT, "卡类型:集团卡");
			prnPrintf("集团卡\n");
		}
		else if(gstYktEnrichLog.uCardtype==12)
		{
			lcdDisplay(0, 4, DISP_CFONT, "卡类型:福利卡");
			prnPrintf("福利卡\n");
		}
		else
		{
			lcdDisplay(0, 4, DISP_CFONT, "卡类型:未知定义");
			prnPrintf("未知定义\n");

		}
	
		prnPrintf("电子钱包信息:\n");
		prnPrintf("  充值时间: ");
		memset(buf, 0, sizeof(buf));// sTranDateTime
	 sprintf((char *)buf, "%02X%02X-%02X-%02X %02X:%02X:%02X",
	gstYktEnrichLog.sTranDateTime[0], gstYktEnrichLog.sTranDateTime[1],
	gstYktEnrichLog.sTranDateTime[2],gstYktEnrichLog.sTranDateTime[3], 
	gstYktEnrichLog.sTranDateTime[4], gstYktEnrichLog.sTranDateTime[5],
	gstYktEnrichLog.sTranDateTime[6]);
	//	PubBcd2Asc0(gstYktEnrichLog.sTranDateTime,7,buf);
		prnPrintf("%s\n", buf);
		prnPrintf("  充值金额: ");
	//	PubConvAmount(uint8_t * pszPrefix,uint8_t * pszIn,uint8_t ucDeciPos,uint8_t * pszOut,uint8_t ucMisc)
	//	gstYktEnrichLog.ulTransAmt;
	    memset(buf,0,sizeof(buf));
		sprintf((INT8 *)buf, "%ld.%02ld元", gstYktEnrichLog.ulTransAmt/100,gstYktEnrichLog.ulTransAmt%100);
		prnPrintf("%s\n", buf);
		prnPrintf("  实充金额: ");
		  memset(buf,0,sizeof(buf));
		sprintf((INT8 *)buf, "%ld.%02ld元", gstYktEnrichLog.ulPaidupAmt/100,gstYktEnrichLog.ulPaidupAmt%100);
		prnPrintf("%s\n", buf);
		prnPrintf("  卡内余额: ");
		  memset(buf,0,sizeof(buf));
		sprintf((INT8 *)buf, "%ld.%02ld元", gstYktEnrichLog.ulNowBalaceAmt/100,gstYktEnrichLog.ulNowBalaceAmt%100);
		prnPrintf("%s\n", buf);
		prnPrintf("费用总计: ");
			  memset(buf,0,sizeof(buf));
		sprintf((INT8 *)buf, "%ld.%02ld元", gstYktEnrichLog.ulPaidupAmt/100,gstYktEnrichLog.ulPaidupAmt%100);
		prnPrintf("%s\n", buf);
	    			
		prnPrintf("操作员号: ");
		prnPrintf("%s\n", gstYktEnrichLog.sTellNo+1);
		prnPrintf("部门编号: ");
		prnPrintf("%s\n", stPosParam.szCompanyNo);
		prnPrintf("充值点号: ");
		prnPrintf("%s\n", stPosParam.pointofsaleNo);
		prnPrintf("POS机编号: ");
		prnPrintf("%s\n", stPosParam.sM1TellNo);
		
		//-----
		memset(buf, 0, sizeof(buf));// sTranDateTime
	 	sprintf((char *)buf, "%02X%02X-%02X-%02X %02X:%02X:%02X",
		sTimeStr[0], sTimeStr[1],sTimeStr[2],sTimeStr[3], 
		sTimeStr[4], sTimeStr[5],sTimeStr[6]);
		prnPrintf("操作时间: %s\n",buf);//sTimeStr

		if( j!=2 )
		{
			PrnNormalFontData("持卡人签名\n\n\n\n");
			PrnSmallFontData("-----------------------------------------------\n");			
			prnPrintf("本人确认以上交易,同意将其计入本卡账户\n");
		}

		if( flag==REPRINT ) 
		{
		 prnPrintf("\n****重打印****\n");
			   
		}
		prnPrintf("\n");
		prnPrintf("\n");
		prnPrintf("\n");
		prnPrintf("\n");

		uRet=PrintData();
		if(uRet!=OK)
		 {
		   lcdClrLine(2, 7);
		   DispMulLanguageString(0, 2, DISP_MEDIACY|DISP_CFONT, "打印凭证失败", " ");
		   kbGetKeyMs(1000);
		 }
		 if( j==0 && stPosParam.ucTicketNum=='1' ) //只打印商户存根
			break;
		if( (stPosParam.ucTicketNum=='2' && j==0) || (stPosParam.ucTicketNum=='3' && j==1) )
		{
			lcdClrLine(6, 7);
			lcdFlip();	
			DispMulLanguageString(0, 6, DISP_CFONT, "按确认继续打印", "[ENTER]-PRINT");
			kbGetKeyMs(2000);
		}
	}
	
	return;


}

// end of file


void PrtM1TranSaleTicket(INT32 flag)
{
		UINT8 j;
		UINT8 uRet;
	//	INT8  szResultAmt[6];
		INT32 iRet;
	//	UINT8 szCardTemp[20];
	//    UINT8 szAmount[30];
	//    unsigned long ultemp;
		INT8  buf[30];
        INT8     sTimeStr[15],pszDate[20];
		lcdClrLine(2, 7);
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "	 打印凭单中   ", "PRINT...");
	      sysGetTime(&sTimeStr[1]);
			sTimeStr[0]=0x20;
			BcdToAsc0(&pszDate[0], sTimeStr, 14);
		for(j=0; j<3; j++)
		{
			if( flag!=2 )
			{
				lcdClrLine(6, 7);
				DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "	 打印凭单中   ", "PRINT...");
			}
	
			if( j==1 && stPosParam.ucTicketNum=='2' ) //不打印银行存根
				continue;
			
			iRet = prnInit();
			if( iRet!=OK )
			{
				lcdClrLine(2, 7);
				DispMulLanguageString(0, 3, DISP_CFONT, "打印机初始化失败", "INIT PRINT FAIL");
				kbGetKey();
				return ;
			}
			if( flag==2 )
			{
				PrnNormalFontData("\n**充值故障报告单,非凭证联**\n------------------------\n");
			}
	
			if( stPosParam.ucTestFlag==PARAM_OPEN )
			{
				PrnNormalFontData("\n**测试/TESTING,凭证无效**\n");
			}
				
			//PrnSmallFontData("\n------------------------------------------\n");
              //  PrnBigFontData("	  粤通卡充值凭证单	  ");
			
			//PrnSmallFontData("\n------------------------------------------\n");
			//PrnNormalFontData("            ");
		//	prnLogo(EM_BMP_ROTATE0,YTKLOGObaiye);
		//	prnLogo(EM_BMP_ROTATE0,YTKLOGO);
			PrnNormalFontData("      银川公交IC卡收费系统  \n");
			PrnNormalFontData("   小额消费移动POS机扣款凭证\n");
			prnPrintf("\n操  作: 客票消费\n");
			prnPrintf("卡  号: ");
			prnPrintf("%s\n", gstYktEnrichLog.sCardSerialNo);
			prnPrintf("芯片号: ");
			//PubChar2Long(gstYktEnrichLog.sCSN,4,&ultemp);
			//memset(buf,0,sizeof(buf));
			//sprintf((INT8 *)buf, "%010d", ultemp);
			prnPrintf("%s\n", gstYktEnrichLog.sCSN);
			//prnPrintf("ISAM号：");
			//prnPrintf("%s\n", gstYktEnrichLog.sPsamId);
			
			prnPrintf("卡类型：");
//prnPrintf(gstYktEnrichLog.uCardtype);
			if(gstYktEnrichLog.uCardtype==0x00)
			{
				lcdDisplay(0, 4, DISP_CFONT, "卡类型:普通卡");
				prnPrintf("普通卡\n");
			}
			 else if(gstYktEnrichLog.uCardtype==1)
			{
				lcdDisplay(0, 4, DISP_CFONT, "卡类型:老人卡 A");
				prnPrintf("老人卡 A\n");
			}
			 else if(gstYktEnrichLog.uCardtype==2)
			{
				lcdDisplay(0, 4, DISP_CFONT, "卡类型:老人卡 B");
				prnPrintf("老人卡 B\n");
			}
			 else if(gstYktEnrichLog.uCardtype==3)
			{
				lcdDisplay(0, 4, DISP_CFONT, "卡类型:学生卡 A");
				prnPrintf("学生卡 A\n");
			}
			 else if(gstYktEnrichLog.uCardtype==4)
			{
				lcdDisplay(0, 4, DISP_CFONT, "卡类型:学生卡 B");
				prnPrintf("学生卡 B\n");
			}
			else if(gstYktEnrichLog.uCardtype==5)
			{
				lcdDisplay(0, 4, DISP_CFONT, "卡类型:预留卡 A");
				prnPrintf("预留卡 A\n");
			}
			else if(gstYktEnrichLog.uCardtype==6)
			{
				lcdDisplay(0, 4, DISP_CFONT, "卡类型:预留卡 B");
				prnPrintf("预留卡 B\n");
			}
			else if(gstYktEnrichLog.uCardtype==7)
			{
				lcdDisplay(0, 4, DISP_CFONT, "卡类型:CPU卡");
				prnPrintf("CPU卡\n");
			}
			else if(gstYktEnrichLog.uCardtype==8)
			{
				lcdDisplay(0, 4, DISP_CFONT, "卡类型:员工卡");
				prnPrintf("员工卡\n");
			}
			else if(gstYktEnrichLog.uCardtype==9)
			{
				lcdDisplay(0, 4, DISP_CFONT, "卡类型:爱心卡");
				prnPrintf("爱心卡\n");
			}
			else if(gstYktEnrichLog.uCardtype==10)
			{
				lcdDisplay(0, 4, DISP_CFONT, "卡类型:优待卡");
				prnPrintf("优待卡\n");
			}
			else if(gstYktEnrichLog.uCardtype==11)
			{
				lcdDisplay(0, 4, DISP_CFONT, "卡类型:集团卡");
				prnPrintf("集团卡\n");
			}
			else if(gstYktEnrichLog.uCardtype==12)
			{
				lcdDisplay(0, 4, DISP_CFONT, "卡类型:福利卡");
				prnPrintf("福利卡\n");
			}
			else
			{
				lcdDisplay(0, 4, DISP_CFONT, "卡类型:未知定义");
				prnPrintf("未知定义\n");

			}
		
			prnPrintf("电子钱包信息:\n");
			prnPrintf("  消费时间: ");
				memset(buf, 0, sizeof(buf));// sTranDateTime
		 sprintf((char *)buf, "%02X%02X-%02X-%02X %02X:%02X:%02X",
		gstYktEnrichLog.sTranDateTime[0], gstYktEnrichLog.sTranDateTime[1],
		gstYktEnrichLog.sTranDateTime[2],gstYktEnrichLog.sTranDateTime[3], 
		gstYktEnrichLog.sTranDateTime[4], gstYktEnrichLog.sTranDateTime[5],
		gstYktEnrichLog.sTranDateTime[6]);
		//	PubBcd2Asc0(gstYktEnrichLog.sTranDateTime,7,buf);
			prnPrintf("%s\n", buf);
			prnPrintf("  消费金额: ");
		//	PubConvAmount(uint8_t * pszPrefix,uint8_t * pszIn,uint8_t ucDeciPos,uint8_t * pszOut,uint8_t ucMisc)
		//	gstYktEnrichLog.ulTransAmt;
		    memset(buf,0,sizeof(buf));
			sprintf((INT8 *)buf, "%ld.%02ld元", gstYktEnrichLog.ulTransAmt/100,gstYktEnrichLog.ulTransAmt%100);
			prnPrintf("%s\n", buf);
			prnPrintf("  实扣金额: ");
			  memset(buf,0,sizeof(buf));
			sprintf((INT8 *)buf, "%ld.%02ld元", gstYktEnrichLog.ulPaidupAmt/100,gstYktEnrichLog.ulPaidupAmt%100);
			prnPrintf("%s\n", buf);
			prnPrintf("  卡内余额: ");
			  memset(buf,0,sizeof(buf));
			sprintf((INT8 *)buf, "%ld.%02ld元", gstYktEnrichLog.ulNowBalaceAmt/100,gstYktEnrichLog.ulNowBalaceAmt%100);
			prnPrintf("%s\n", buf);
			prnPrintf("费用总计: ");
				  memset(buf,0,sizeof(buf));
			sprintf((INT8 *)buf, "%ld.%02ld元", gstYktEnrichLog.ulPaidupAmt/100,gstYktEnrichLog.ulPaidupAmt%100);
			prnPrintf("%s\n", buf);
			
			prnPrintf("操作员号: ");
			prnPrintf("%s\n", gstYktEnrichLog.sTellNo+1);
			prnPrintf("部门编号: ");
			prnPrintf("%s\n", stPosParam.szCompanyNo);
			prnPrintf("充值点号: ");
			prnPrintf("%s\n", stPosParam.pointofsaleNo);
			prnPrintf("POS机编号: ");
			prnPrintf("%s\n", stPosParam.sM1TellNo);
			//-----
			memset(buf, 0, sizeof(buf));// sTranDateTime
		 sprintf((char *)buf, "%02X%02X-%02X-%02X %02X:%02X:%02X",
		sTimeStr[0], sTimeStr[1],sTimeStr[2],sTimeStr[3], 
		sTimeStr[4], sTimeStr[5],sTimeStr[6]);
		//	PubBcd2Asc0(gstYktEnrichLog.sTranDateTime,7,buf);
			//prnPrintf("%s\n", buf);
			prnPrintf("操作时间: %s\n",buf);//sTimeStr
   // gstYktEnrichLog.sTranDateTime[0]=0x20;
   // memcpy(&gstYktEnrichLog.sTranDateTime[1],sTimeStr,6);
          
			//-----
			//GetDays(uint8_t * szDate)
			//prnPrintf("操作时间: ");
			//gstYktEnrichLog.sCSN
		//	prnPrintf("商户名称: ");
		//	prnPrintf("%s\n",stPosParam.szUnitChnName);
			//prnPrintf("交易类型: 充值 \n");
		
			/*prnPrintf("交易银行卡：");
			prnPrintf("%s\n", YktRecord.szbankCardNo);
			prnPrintf("系统参考号：");
		    prnPrintf("%s\n", YktRecord.szSysReferNo);
			prnPrintf("粤通卡卡号: ");
			prnPrintf(" %s\n",YktRecord.szCardNo);
			prnPrintf("充值前余额: ");
			memset(szAmount,0,sizeof(szAmount));
			ConvBcdAmount( YktRecord.sBalance, szAmount);
			prnPrintf("RMB:%s \n",szAmount);
			prnPrintf("充值金额: ");
			memset(szAmount,0,sizeof(szAmount));
			ConvBcdAmount( YktRecord.sAmount, szAmount);
			prnPrintf("RMB:%s \n",szAmount);
			prnPrintf("回退金额: ");
			memset(szAmount,0,sizeof(szAmount));
			ConvBcdAmount( YktRecord.sPollAmount, szAmount);
			prnPrintf("RMB:%s \n",szAmount);
			prnPrintf("充值后余额: ");
			memset(szAmount,0,sizeof(szAmount));
			ConvBcdAmount( YktRecord.sTransBalance, szAmount);
			prnPrintf("RMB:%s \n",szAmount);
			prnPrintf("交易时间: ");
			memset(szAmount,0,sizeof(szAmount));
			memcpy(szAmount,YktRecord.szDate,8);
			memcpy(szAmount+8,YktRecord.szTime,6);
			prnPrintf("%s \n",szAmount);
			prnPrintf("终端编号: ");
			prnPrintf("%s\n", stPosParam.szYtkPosId);
			prnPrintf("交易TAC码 : ");
			memset(szAmount,0,sizeof(szAmount));
			PubBcd2Asc(YktRecord.szTAC,4,szAmount);
			prnPrintf("%s \n",szAmount);
			if( flag==REPRINT)
			{
			prnPrintf("MAC1 : ");
			memset(szAmount,0,sizeof(szAmount));
			PubBcd2Asc(YktRecord.szMac1,4,szAmount);
			prnPrintf("%s \n",szAmount);
						prnPrintf("MAC2 : ");
			memset(szAmount,0,sizeof(szAmount));
			PubBcd2Asc(YktRecord.szMac2,4,szAmount);
			prnPrintf("%s \n",szAmount);
			}*/
		//	prnPrintf("\n");
		//	prnPrintf("客服热线：96533");
		if( j!=2 )
		{
			PrnNormalFontData("持卡人签名\n\n\n\n");
			PrnSmallFontData("-----------------------------------------------\n");			
			prnPrintf("本人确认以上交易,同意将其计入本卡账户\n");
		}
		//	PrnSmallFontData("\n***********************************************\n");
		//	PrnBigFontData("感谢您使用广东百业信息技术有限公司百银缴费通\n");
		
	
	//		PrnSmallFontData("------------------------------------------\n");  
			if( flag==REPRINT ) 
			{
			 prnPrintf("\n****重打印****\n");
				   
			}
			prnPrintf("\n");
			prnPrintf("\n");
			prnPrintf("\n");
			prnPrintf("\n");
			
            //PrintData();
			 uRet=PrintData();
		 if(uRet!=OK)
		 {
		   lcdClrLine(2, 7);
		   DispMulLanguageString(0, 2, DISP_MEDIACY|DISP_CFONT, "打印凭证失败", " ");
		//   DispMulLanguageString(0, 4, DISP_MEDIACY|DISP_CFONT, "如需凭证请联系:", " ");
		 //  DispMulLanguageString(0, 6, DISP_MEDIACY|DISP_CFONT, "020-38811228", " ");
		   kbGetKeyMs(1000);

		 }
		 if( j==0 && stPosParam.ucTicketNum=='1' ) //只打印商户存根
			break;
		if( (stPosParam.ucTicketNum=='2' && j==0) || (stPosParam.ucTicketNum=='3' && j==1) )
		{
			lcdClrLine(6, 7);
			lcdFlip();	
			DispMulLanguageString(0, 6, DISP_CFONT, "按确认继续打印", "[ENTER]-PRINT");
			kbGetKeyMs(2000);
		}
		}
		
		return;


}

// end of file

