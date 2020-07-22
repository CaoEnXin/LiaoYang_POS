#include "posapi.h"
#include "posmacro.h"
#include "terstruc.h"
#include "glbvar.h"
#include "posappapi.h"
#include "tranfunclist.h"

/*=================================================================*/
/*  �����������ж��Ƿ���һ��EMV��Ƭ								   */
/*  ����ֵ  ��TRUE --> EMV��Ƭ									   */
/*            FALSE--> ��EMV��Ƭ						           */
/*=================================================================*/
uint8_t IsEmvCard(uint8_t *pszTrack2Data)
{
	char	*pszSeperator;
	
	if( *pszTrack2Data==0 )
	{
		return FALSE;
	}
	
	pszSeperator = strchr((char *)pszTrack2Data, '=');
	if( pszSeperator==NULL )
	{
		return FALSE;
	}
	if( (pszSeperator[5]=='2') || (pszSeperator[5]=='6') )
	{
		return TRUE;
	}
	
	return FALSE;
}

/****************************************************************************
 ��������:���ˢ����IC���忨�����俨�ŵ��¼�����������Ӧ���¼���
 ����ֵ:   1: ˢ��
		   2: ����IC��
		   4: ����
****************************************************************************/
uint8_t DetectCardEvent(uint8_t ucMode)
{
	int  iRet, iCnt;
	int  timeout ;

	iRet = 0;
	//��ͷ�ϵ硢�򿪡��建��
	if( ucMode & CARD_SWIPED )
	{
		magClose();
		magOpen();
		magReset();
	}

	if (ucMode & CARD_PASSIVE)
	{
		magClose();
		magOpen();
		magReset();

		contactness_close(imif_fd);
		imif_fd = -1;
		imif_fd = contactness_open();
		contactness_poll(imif_fd);
//		sysDelayMs(50);
	}
	 
	iCnt = 0;			
    kbFlush();
	while(1)
	{
		if( kbhit()==YES )
		{
			return CARD_KEYIN; //�а����¼�
		}
		if( (ucMode & CARD_SWIPED) && (magSwiped()==YES) )
		{
			return CARD_SWIPED; //��ˢ���¼�
		}
		if( (stPosParam.ucEmvSupport==PARAM_OPEN) && (ucMode & CARD_INSERTED) &&
			(iccDetect(ICC_USER)==ICC_SUCCESS) )
		{
			return CARD_INSERTED; //�в���IC���¼�
		}
		if ( (stPosParam.ucqPbocSupport == PARAM_OPEN) && (ucMode & CARD_PASSIVE))
		{
			iRet = contactness_poweredon(imif_fd);
			if (iRet != 0)
			{ // ʧ�ܴ���
				if (iRet == MIF_RET_MULTICARD)
				{
					timeout  = sysGetTimerCount() + 1000;
					while ((iRet = contactness_poweredon(imif_fd))) {
						if (iRet == MIF_RET_NOCARD) {
							timeout  = sysGetTimerCount() + 1000;
						} else if (iRet == MIF_RET_MULTICARD) {
							if (sysGetTimerCount() > timeout )
								break;
						}
						sysDelayMs(50);
					}

					
					if (iRet ==MIF_RET_SUCCESS)
					{
						return CARD_PASSIVE;  // �ǻ������¼�
					}

					if (iRet == MIF_RET_MULTICARD)
					{
						contactness_close(imif_fd);
						imif_fd = -1;
						lcdClrLine(2,7);
						lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"��ʾ��һ��Ƭ");
						PubBeepErr();
						PubWaitKey(5);
						return NO_DISP;
					}
				}
/*				iRet = contactness_poll(imif_fd);
				sysDelayMs(50);
				iRet = contactness_poweredon(imif_fd);
				if (iRet == MIF_RET_MULTICARD)
				{
					contactness_close(imif_fd);
					imif_fd = -1;
					lcdClrLine(2,7);
					lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"��ʾ��һ��Ƭ");
					PubBeepErr();
					PubWaitKey(5);
					return NO_DISP;
				}*/
			}
			else
			{
				if (iCnt < 1 )
				{ // ��ü��2��
					iCnt++;
					contactness_poll(imif_fd);
					continue;
				}
				return CARD_PASSIVE;  // �ǻ������¼�
			}			
		}
	}
}

void ShowGetCardScreen(uint8_t ucMode)
{
	uint8_t	szDispBuf[33],szDispBuf2[33];

	memset(szDispBuf2,0,sizeof(szDispBuf2));
	switch( ucMode & 0x7F )
	{
	case CARD_SWIPED:
		if (PosCom.stTrans.iTransNo == EC_TOPUP_NORAML)
			sprintf((char *)szDispBuf, "��ˢ��(ת����)-->");
		else
			sprintf((char *)szDispBuf, "��ˢ��-->");
		break;

	case CARD_INSERTED:
		sprintf((char *)szDispBuf, "�����IC��>>>");
		break;

	case CARD_KEYIN:
		sprintf((char *)szDispBuf, "�����뿨��:");
	    break;

	case CARD_SWIPED|CARD_KEYIN:
		sprintf((char *)szDispBuf, "��ˢ�������뿨��");
	    break;

	case CARD_SWIPED|CARD_INSERTED:
		if (PosCom.stTrans.iTransNo == POS_MAGTOPUP_ACCT || PosCom.stTrans.iTransNo == EC_TOPUP_NORAML)
		{
			sprintf((char *)szDispBuf, "��ˢת�������");
			sprintf((char *)szDispBuf2, "ת����");			
		}
		else
		{
			sprintf((char *)szDispBuf, "�����IC����ˢ��");
//			sprintf((char *)szDispBuf, "�����IC����ˢ��");
//			sprintf((char *)szDispBuf2, "��ӿ�");
		}
		break;

	case CARD_INSERTED|CARD_KEYIN:
		sprintf((char *)szDispBuf, "���IC�����俨��");
		break;

	case CARD_INSERTED|CARD_KEYIN|CARD_SWIPED:
//		sprintf((char *)szDispBuf, "���/ˢ��/�俨��");
		sprintf((char *)szDispBuf, "�����IC����ˢ��");
		sprintf((char *)szDispBuf2, "�����뿨��");
		break;

	case CARD_PASSIVE:
		sprintf((char *)szDispBuf, "�� �� ��");
		break;

	case CARD_PASSIVE|CARD_INSERTED:
		sprintf((char *)szDispBuf, "�����IC����ӿ�");
		break;

	case CARD_INSERTED|CARD_KEYIN|CARD_SWIPED|CARD_PASSIVE:
		sprintf((char *)szDispBuf, "�����IC����ˢ��");
		sprintf((char *)szDispBuf2, "�����뿨�Ż�ӿ�");
		break;

	default:
	    return;
	}
	lcdClrLine(2, 7);
	lcdFlip();	
	if (strlen((char *)szDispBuf2))
	{
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, (char *)szDispBuf);
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, (char *)szDispBuf2);
	}
	else
	{
		lcdDisplay(0, 2, DISP_CFONT, (char *)szDispBuf);
	}
}


uint8_t PosGetCard(uint8_t ucMode)
{
    uint8_t	ucRet, bCheckICC;

	if( PosCom.ucSwipedFlag!=NO_SWIPE_INSERT )
	{
		return OK;
	}

	bCheckICC = !(ucMode & 0x80);
	while(1)
	{
		ShowGetCardScreen(ucMode);
		ucRet = DetectCardEvent(ucMode);
		if( ucRet==CARD_KEYIN )
		{
			if( ucMode & CARD_KEYIN )	// �������뿨��
			{
				ucRet = ManualEntryCardNo(PosCom.stTrans.szCardNo);
				if( ucRet!=OK )
				{
					return ucRet;
				}
				return ManualEntryExpDate(PosCom.stTrans.szExpDate);
			}
			else if( kbGetKey()==KEY_CANCEL )
			{
				contactness_close(imif_fd);
				imif_fd = -1;
				return E_TRANS_CANCEL;
			}
		}
		else if( ucRet==CARD_SWIPED ) 
		{
			ucRet = SwipeCardProc(bCheckICC);
			if( ucRet==OK )
			{
				return DispCardNo(PosCom.stTrans.szCardNo);
			}
			else if( ucRet==E_ERR_SWIPE )	// ˢ������
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				lcdDisplay(0, 2, DISP_CFONT, "ˢ������");
				lcdDisplay(0, 4, DISP_CFONT, "�����ˢ��-->");
				lcdDisplay(0, 6, DISP_CFONT, "��ȡ�����˳�");
				kbGetKeyMs(2000);
			}
			else if( ucRet==E_NEED_INSERT )	// ��оƬ��
			{
				if( (ucMode & CARD_INSERTED)==0 ) // �����ײ�����忨(����Ȩ�������)
				{
					return E_NEED_INSERT;
				}
				ucMode &= ~CARD_SWIPED;	// ȥ��ˢ�����
			}
			else
			{
				return ucRet;	// ��������
			}
		}
		else if( ucRet==CARD_INSERTED )
		{
			ucRet = InsertCardProc(FALSE);
			if( ucRet==E_NEED_FALLBACK )
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				lcdDisplay(0, 2, DISP_CFONT, "��ˢ��-->");
				ucMode = CARD_SWIPED;
				PosCom.ucFallBack = TRUE;
				bCheckICC = TRUE;
			}
			else if( ucRet==E_CARD_BLOCKER )
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "��������");
				ErrorBeep();
				kbGetKeyMs(2000);		

				DispRemoveICC();
				return NO_DISP;
			}
			else if( ucRet==E_APP_BLOCKED )
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "Ӧ������");
				ErrorBeep();
				kbGetKeyMs(2000);		

				DispRemoveICC();
				return NO_DISP;
			}
			else if( ucRet==E_TRANS_FAIL )
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "    ���׾ܾ�    ");
				ErrorBeep();
				kbGetKeyMs(2000);		

				DispRemoveICC();
				return NO_DISP;
			}
			else
			{
				return ucRet;
			}
		}
		else if (ucMode==CARD_PASSIVE && ucRet==CARD_PASSIVE)
		{
			ucRet = ContackCardProc(FALSE);
			contactness_close(imif_fd);
			imif_fd = -1;
			return ucRet;
		}
	}
}

/****************************************************************************
 ��������:    ������Ч��
****************************************************************************/
uint8_t ManualEntryExpDate(uint8_t *szExpDate)
{
	uint8_t inBuf[6];
	char  chkDate[9];
	int iRet;

	while(1) 
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 2, DISP_CFONT, "�����뿨��Ч��");
		lcdDisplay(0, 4, DISP_CFONT, "(��������):");
		memset(inBuf, 0, sizeof(inBuf));
		lcdGoto(80, 6);
		iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 0, 4, 
			(stPosParam.ucOprtLimitTime*1000), (char *)inBuf);
		if( iRet<0 )
			return (E_TRANS_CANCEL);
		if( strlen((char *)inBuf)==0 ) //��������Ч��
			break;
		if( strlen((char *)inBuf)!=4 )
			continue;
		memcpy(chkDate, inBuf, 4);
		if( CheckYYMM(chkDate)==0 )
		{
			strcpy((char *)PosCom.stTrans.szExpDate, (char *)inBuf);
			break;
		}	
		else
		{
			FailBeep();
			lcdClrLine(6, 7);
			lcdFlip();	
			lcdDisplay(0, 6, DISP_CFONT, "��Ч�ڸ�ʽ��");
			kbGetKeyMs(1000);
		}			
	}
	return OK;
}

/****************************************************************************
 ��������:    �ֹ����뿨�ţ�ռ����Ļ4��7��
****************************************************************************/
uint8_t ManualEntryCardNo(uint8_t *pszCardNo)
{
	uint8_t	sBuff[21];
	int   iRet;

	lcdClrLine(4, 7);
	lcdFlip();	
	lcdGoto(0, 4);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_MAXLEN+KB_EN_REVDISP, 13, 19, 
		(stPosParam.ucOprtLimitTime*1000), (char *)sBuff);
	if( iRet<0 )
	{
		return E_TRANS_CANCEL;
	}
	sprintf((char *)pszCardNo, "%.*s", iRet, sBuff);
	PosCom.ucSwipedFlag = CARD_KEYIN;
	PosCom.stTrans.ucSwipeFlag = CARD_KEYIN;

	return OK;
}

/****************************************************************************
 ��������: ��ˢ������£�ȡ�ŵ���Ϣ
****************************************************************************/
uint8_t GetTrackData(uint8_t *pszTrack2, uint8_t *pszTrack3, uint8_t *pszCardNo)
{
	uint8_t	ucRet;
	int   iCardLen=0;
//	uint8_t szTrack1[100];

	ucRet = magRead(NULL, pszTrack2, pszTrack3);
//	ucRet = magRead(szTrack1, pszTrack2, pszTrack3);
	if( (ucRet&0x70)!=0x00 )
	{
		return E_ERR_SWIPE;
	}
	magClose();

	iCardLen = strlen((char *)pszTrack2);
	if( iCardLen<24 || iCardLen>37 )
	{
		return E_ERR_SWIPE;
	}

	return GetCardNoFromTrack(pszCardNo, pszTrack2, pszTrack3);
}

/****************************************************************************
 ��������: ��ʾ���ţ�����Ļ�ĵ�2��
****************************************************************************/
uint8_t DispCardNo(uint8_t *pszCardNo)
{
	uint8_t szTempCardNo[21];

	MaskPan(pszCardNo, szTempCardNo);
	
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "����   CARD INFO", "       CARD INFO");

	if( strlen((char *)pszCardNo)>16 )
	{
		lcdDisplay(0, 4, DISP_ASCII, "%.20s", pszCardNo);
	}
	else
	{
		lcdDisplay(0, 4, DISP_CFONT, "%s", pszCardNo);
	}

	DispMulLanguageString(0, 6, DISP_CFONT, "    ȷ��(ENTER)?", "        (ENTER)?");
	if( kbGetKeyMs(30*1000)!=KEY_ENTER )
	{
		return E_TRANS_CANCEL;
	}

	PosCom.stTrans.ucSwipeFlag = PosCom.ucSwipedFlag;	// ����ˢ����־

	return OK;
}
/****************************************************************************
��������: �Ӵŵ���ȡ����
****************************************************************************/
uint8_t GetCardNoFromTrack(uint8_t *szCardNo, uint8_t *track2, uint8_t *track3)
{
	int   i;
	
	track2[37] = 0;
	track3[104] = 0;
	
	// POS�˲��濨����2�ŵ���ʼ��'��'
	if( strlen((char *)track2)!=0 )
	{	
		i = 0;
		while( track2[i]!='=' )
		{
			if( i>19 ) 
				return E_ERR_SWIPE;
			i++;
		}
		if( i<13 || i>19 ) 
			return E_ERR_SWIPE;
		memcpy(szCardNo, track2, i);		
		szCardNo[i] = 0;
		return OK;	
	}
	// ֻ��3�ŵ���3�ŵ���ʼ�ڶ�λ��ʼ��'='
	else if( strlen((char *)track3)!=0 ) 
	{
		i = 0;
		while( track3[i]!='=' ) 
		{
			if( i>21 ) 
				return E_ERR_SWIPE;
			i++;
		}			    
		if( i<15 || i>21 )
			return E_ERR_SWIPE;		
		memcpy(szCardNo, track3+2 ,i-2);		
		szCardNo[i-2] = 0;
		return OK;	
	}
	
	return E_ERR_SWIPE;		

}


void DispRemoveICC(void)
{
	/*if( stPosParam.ucEmvSupport==PARAM_CLOSE )
	{
		return;
	}*/

/*
	iccPowerDown(ICC_USER);
	// ���IC���Ѱγ���ֱ�ӷ��ء�
	if( iccDetect(ICC_USER)!=ICC_SUCCESS )
	{
		return;
	}

	// ��ʾ���ȴ�IC���γ�
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "��γ�IC��", "PLS REMOVE CARD");
//	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "��γ�IC��");
//	lcdDisplay(0, 6, DISP_CFONT|DISP_MEDIACY, "PLS REMOVE CARD");
	while( iccDetect(ICC_USER)==ICC_SUCCESS )
	{ 
		sysBeep();
		sysDelayMs(500);
	}*/
}

void WaitRemoveICC(void)
{
	iccPowerDown(ICC_USER);
	while( iccDetect(ICC_USER)==ICC_SUCCESS );
}

void DispICCProcessing(void)
{
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "ICC������,���Ժ�", "ICC PROCESS...");
}

void MaskPan(uint8_t *pszInPan, uint8_t *pszOutPan)
{
	uint8_t   	szBuff[30];
	int		iCnt, iPanLen;
	
	memset(szBuff, 0, sizeof(szBuff));
	iPanLen = strlen((char *)pszInPan);
	for(iCnt=0; iCnt<iPanLen; iCnt++)
	{
		if( (iCnt<6) || (iCnt>=iPanLen-4 && iCnt<iPanLen) )
		{
			szBuff[iCnt] = pszInPan[iCnt];
		}
		else
		{
			szBuff[iCnt] = '*';
		}
	}
	
	sprintf((char *)pszOutPan, "%.*s", LEN_PAN, szBuff);
}


uint8_t PosGetEdCard(uint8_t ucMode)
{
    uint8_t	ucRet;

	while(1)
	{
		ShowGetCardScreen(ucMode);
		ucRet = DetectCardEvent(ucMode);
		if( ucRet==CARD_KEYIN )
		{
			if( kbGetKey()==KEY_CANCEL )
			{
				return E_TRANS_CANCEL;
			}
		}
		else if( ucRet==CARD_INSERTED )
		{
			ucRet = InsertEdCardProc(FALSE);
			if( ucRet != 0 )
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "    ���׾ܾ�    ");
				ErrorBeep();
				kbGetKeyMs(2000);		
				DispRemoveICC();
				return NO_DISP;
			}
			else
			{
				return ucRet;
			}
		}
	}
}

uint8_t InsertEdCardProc(uint8_t bShowDefTranName)
{
	int		iRet, iDatalen;
	uint8_t szBuffer[1000], szTemp[2000], szBegExp[5], szEndExp[5];
	char    szIssureID[9], szAppSN[20];
	uint8_t  ucAppTypeFlag, ucAppVersion;
	APDU_SEND stSendApdu;
	APDU_RESP stRespApdu;
	char      *psTemp;

	PosCom.ucSwipedFlag = NO_SWIPE_INSERT;
	if( bShowDefTranName )
	{
		lcdCls();
		lcdFlip();	
		if( stTransCtrl.ucDefaltTxn==1 )
		{
			stTemp.iTransNo = POS_SALE;
		}
		else
		{
			stTemp.iTransNo = POS_PREAUTH;
		}
	}
	DispTransName();
	DispICCProcessing();

	memset(szBuffer,0,sizeof(szBuffer));
	iccSetPara(ICC_USERCARD,ICC_VOL_5V,ICC_ISO);
	iRet = iccPowerUp(ICC_USERCARD, &iDatalen, szBuffer);
	if( iRet )
	{
		return E_NEED_FALLBACK;		
	}

	// Ӧ��ѡ��
	memset((char*)&stSendApdu,0,sizeof(stSendApdu));
	memset((char*)&stRespApdu,0,sizeof(stRespApdu));	
	stSendApdu.CLA = 0x00;
	stSendApdu.INS = 0xA4;
	stSendApdu.P1 = 0x04;
	stSendApdu.P2 = 0x00;
	stSendApdu.Lc = 7;
	memcpy(stSendApdu.DataIn,"\xA0\x00\x00\x03\x33\x01\x01",7);
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

	memset(szBuffer,0,sizeof(szBuffer));
	memset(szTemp,0,sizeof(szTemp));
	PubBcd2Asc0(stRespApdu.DataOut,stRespApdu.LenOut,(char*)szTemp);
	psTemp = strstr((char*)szTemp,"BF0C");
	if (psTemp == NULL)
	{
		lcdClrLine(2,7);
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"����ʧ��");
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
	
	iDatalen = PubAsc2Long(psTemp+4,2,NULL);
	if (iDatalen & 0x80)
	{
		iDatalen = iDatalen & 0x7F;
		iDatalen = PubAsc2Long(psTemp+6,iDatalen*2,NULL);
		psTemp += (iDatalen +1)*2;
	}
	else
	{
		psTemp += 2;
	}

	PubAsc2Bcd(psTemp,iDatalen*2,(char*)szBuffer);

	memset(szIssureID,0,sizeof(szIssureID));
	memcpy(szIssureID,szBuffer,8);
	ucAppTypeFlag = szBuffer[8];
	ucAppVersion = szBuffer[9];
	memcpy(szAppSN,&szBuffer[10],10);
	memcpy(szBegExp,&szBuffer[20],4);
	memcpy(szEndExp,&szBuffer[24],4);

	GetPosTime(stTemp.szDate, stTemp.szTime);	
	strcpy((char *)PosCom.stTrans.szDate, (char *)stTemp.szDate);
	strcpy((char *)PosCom.stTrans.szTime, (char *)stTemp.szTime);

	if (memcmp(szBegExp,PosCom.stTrans.szDate+2,4)>=0 )
	{
		lcdClrLine(2,7);
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"��δ��Ч");
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}

	if (memcmp(szEndExp,PosCom.stTrans.szDate+2,4) <=0 )
	{
		lcdClrLine(2,7);
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"���ѹ���");
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}

	PosCom.ucSwipedFlag = CARD_INSERTED;
	// �жϲ�ѡ���ǵ���Ǯ�����ǵ��Ӵ���,�д��޸�
	PosCom.ucEdOrEpFlag = FALSE;
	if (ucAppTypeFlag == 1)
	{
		PosCom.ucEdOrEpFlag = TRUE;
	}

	if (PosCom.ucEdOrEpFlag) //���Ӵ�����Ҫ��֤����
	{
		iRet = VerifyUserPwd();
		if (iRet != 0)
		{
			return NO_DISP;
		}
	}

	if (stTemp.iTransNo == ED_BALANCE)
	{
		iRet = VerifyUserPwd();
		if (iRet != 0)
		{
			return NO_DISP;
		}

		return 0;
	}
	
	iRet = GetEdEpBalance(); // ��ȡ���
	if (iRet != 0)
	{
		return NO_DISP;
	}
	// �����д��޸�

	PubBcd2Asc0((uint8_t*)szAppSN,10,(char*)PosCom.stTrans.szCardNo);
	return DispCardNo(PosCom.stTrans.szCardNo);
}


int    VerifyUserPwd(void)
{
	int		iRet, ilen;
	APDU_SEND stApduSend;
	APDU_RESP stApduResp;
	char  szBuffer[10];
	uint8_t   ucMode;

	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));		
	ucMode = FALSE;
	while(1)
	{
		lcdClrLine(2, 7);
		lcdFlip();
		DispMulLanguageString(0, 2, DISP_CFONT, "����������    ", "INPUT PWD:");
		lcdGoto(96, 6);
		memset(szBuffer,0,sizeof(szBuffer));
		iRet = kbGetString(KB_EN_CRYPTDISP+KB_EN_BIGFONT+KB_EN_NUM+KB_EN_REVDISP, 4, 8, 
			(stPosParam.ucOprtLimitTime*1000), (char *)szBuffer);
		if( iRet!=4 )
		{
			return NO_DISP;
		}
	
		ucMode = TRUE;		
		stApduSend.CLA = 0x00;
		stApduSend.INS = 0x20;
		stApduSend.P1 = 0x00;
		stApduSend.P2 = 0x00;
		ilen = strlen(szBuffer);
		stApduSend.Lc = (ilen+1)/2;
		if (ilen%2 != 0)
		{
			szBuffer[ilen] = '0';
			ilen += 1;
		}
		PubAsc2Bcd(szBuffer,ilen,(char*)stApduSend.DataIn);
		stApduSend.Le = 0x00;
		stApduSend.LeFlag = 0x01;
		iRet = iccExchangeAPDU(icc_fd,&stApduSend,&stApduResp);
		if (iRet != 0 || stApduResp.SWA != 0x90|| stApduResp.SWB != 0x00)
		{
			memset(szBuffer,0,sizeof(szBuffer));
			PubBcd2Asc((char*)&stApduResp.SWA,1,szBuffer);
			PubBcd2Asc((char*)&stApduResp.SWB,1,szBuffer+2);
			if (memcmp(szBuffer,"63C",3)==0)
			{
				PubBeepErr();
				lcdClrLine(2,7);
				PubDispString("PIN��֤ʧ��",2|DISP_LINE_CENTER);
				lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ʣ%s�γ��Ի���",szBuffer+3);
				lcdDisplay(0,6,DISP_CFONT|DISP_MEDIACY,"��[ȷ��]������");
				iRet = PubWaitKey(0);
				if (iRet == KEY_ENTER)
				{
					continue;
				}
				return ERR_TRAN_FAIL;
			}
			else
			{
				lcdCls();
				lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
				lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"%02x %02x �������",stApduResp.SWA,stApduResp.SWB);
				lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
				PubBeepErr();
				PubWaitKey(5);
				return NO_DISP;
			}
			return ERR_TRAN_FAIL;
		}
		else
		{
			break;
		}
	}

	return 0;
}


int    GetEdEpBalance(void)
{
	int		iRet;
	APDU_SEND stApduSend;
	APDU_RESP stApduResp;

	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));	
	
	stApduSend.CLA = 0x80;
	stApduSend.INS = 0x5C;
	stApduSend.P1 = 0x00;
	if (PosCom.ucEdOrEpFlag)
	{
		stApduSend.P2 = 0x02;
	}
	else
	{
		stApduSend.P2 = 0x01;
	}
	stApduSend.Lc = 0x00;
	stApduSend.Le = 0x04;
	stApduSend.LeFlag = 0x01;
	iRet = iccExchangeAPDU(icc_fd,&stApduSend,&stApduResp);
	if (iRet != 0 || stApduResp.SWA != 0x90|| stApduResp.SWB != 0x00)
	{
		lcdCls();
		lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"%02x %02x ��������",stApduResp.SWA,stApduResp.SWB);
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
	
	memset(PosCom.stTrans.sBalanceAmount,0,sizeof(PosCom.stTrans.sBalanceAmount));
	memcpy(PosCom.stTrans.sBalanceAmount,stApduResp.DataOut,4);
	return 0;
}


int    CheckPSamCard(void)
{
	int   iRet, iSamNo, iTryCnt, iReadPsamFlag, iStrlen;
	char  szBuffer[300];
	APDU_SEND stApduSend;
	APDU_RESP stApduResp;  
	
	iSamNo = stTransCtrl.ucCurSamNo;
	if (stTransCtrl.ucCurSamNo < 1 || stTransCtrl.ucCurSamNo > 3)
	{
		iSamNo = 1;
	}
	
	iTryCnt = 1;
	iReadPsamFlag = 0;
	while(iTryCnt++<4) // �����4������
	{
		iPsam_fd = iSamNo;
		if( iPsam_fd  < 0 )
		{
			continue;
		}
		
		iccSetPara(iPsam_fd,ICC_VOL_5V,ICC_ISO);
		iRet = iccPowerUp(iPsam_fd, &iStrlen, szBuffer);
		if (iRet == 0) // ���SAM����λ����ȷ���Ƿ���SAM�������APP
		{	
			iRet = SelectSamAPP(iPsam_fd);
			if (iRet == 0)
			{
				stTransCtrl.ucCurSamNo = iSamNo;
				SaveCtrlParam();
				iReadPsamFlag = 1;  // �ɹ�
				break;
			}
		}
		
		// û�п���λ��������һ��SAM�����Ƿ��п�
		iSamNo++;
		if (iSamNo>3)
		{
			iSamNo = 1;
		}
	}
	
	if (iReadPsamFlag == 0)
	{
		stTransCtrl.ucCurSamNo = -1;
		return -1; 
	}

	// ѡ��PSAM��app
	memset((uint8_t*)&stApduSend, 0x00, sizeof(stApduSend));
	memset((uint8_t*)&stApduResp, 0x00, sizeof(stApduResp));
	stApduSend.CLA = 0x00;
	stApduSend.INS = 0xA4;
	stApduSend.P1 = 0x00;
	stApduSend.P2 = 0x00;
	stApduSend.Lc = 0x02;
	stApduSend.DataIn[0] = 0xDF;
	stApduSend.DataIn[1] = 0x01;
	stApduSend.Le = 0x00;
	stApduSend.LeFlag = 0x01;
	iRet = iccExchangeAPDU(iPsam_fd, &stApduSend, &stApduResp);
	if(iRet != 0
		|| stApduResp.SWA != 0x90
		|| stApduResp.SWB != 0x00)
	{
		lcdCls();
		lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"%02x %02x PSAM������",stApduResp.SWA,stApduResp.SWB);
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
	
	// ��ȡ�ն˱�� 
	memset((uint8_t*)&stApduSend, 0x00, sizeof(stApduSend));
	memset((uint8_t*)&stApduResp, 0x00, sizeof(stApduResp));
	stApduSend.CLA = 0x00;
	stApduSend.INS = 0xB0;
	stApduSend.P1 = 0x96;
	stApduSend.P2 = 0x00;
	stApduSend.Lc = 0;
	stApduSend.Le = 0x06;
	stApduSend.LeFlag = 0x01;
	iRet = iccExchangeAPDU(iPsam_fd, &stApduSend, &stApduResp);
	if(iRet != 0
		|| stApduResp.SWA != 0x90
		|| stApduResp.SWB != 0x00
		|| stApduResp.LenOut != 0x06)
	{
		lcdCls();
		lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"%02x %02x PSAM������",stApduResp.SWA,stApduResp.SWB);
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
	memcpy(PosCom.stTrans.szPsamTid, stApduResp.DataOut, 6);
	
	// ��ȡ������Կ���� 	
	memset((uint8_t*)&stApduSend, 0x00, sizeof(stApduSend));
	memset((uint8_t*)&stApduResp, 0x00, sizeof(stApduResp));
	stApduSend.CLA = 0x00;
	stApduSend.INS = 0xB0;
	stApduSend.P1 = 0x97;
	stApduSend.P2 = 0x00;
	stApduSend.Lc = 0;
	stApduSend.Le = 0x01;
	stApduSend.LeFlag = 0x01;
	iRet = iccExchangeAPDU(iPsam_fd, &stApduSend, &stApduResp);
	if(iRet != 0
		|| stApduResp.SWA != 0x90
		|| stApduResp.SWB != 0x00
		|| stApduResp.LenOut != 0x01)
	{
		lcdCls();
		lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"    ����ʧ��    ");
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"%02x %02x PSAM������",stApduResp.SWA,stApduResp.SWB);
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"��ο����ÿ�");
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
	PosCom.stTrans.ucKeyIndex = stApduResp.DataOut[0];
	return 0;
}


int    SelectSamAPP(int ifd)
{
	int   iRet;
	APDU_SEND stApduSend;
	APDU_RESP stApduResp;
		
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0x00;
	stApduSend.INS = 0xB0;
	stApduSend.P1 = 0x96;
	stApduSend.P2 = 0x00;
	stApduSend.Le = 0;
	stApduSend.Lc = 0;
	stApduSend.LeFlag = 1;	
	iRet = iccExchangeAPDU(ifd,&stApduSend,&stApduResp);
	if (iRet!=ICC_SUCCESS)
	{
		return ERR_TRAN_FAIL;
	}
	
	if (!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 ))
	{
		return ERR_TRAN_FAIL;
	}
	
	return 0;
}


void  DispContackLessProcessing(void)
{
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "ICC������,���Ժ�", "ICC PROCESS...");
}
// end of file
