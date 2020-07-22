#include <openssl/sha.h>

#include "posapi.h"
#include "posmacro.h"
#include "terstruc.h"
#include "glbvar.h"
#include "tranfunclist.h"
#include "emvtranproc.h"
#include "Menu.h"
#include "otherTxn.h"
#include "qpbocProc.h"
#include "EMVCallBack.h"
#include "New8110_ICC.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STX             0x02
#define ETX             0x03
#define ENQ             0x05
#define ACK             0x06
#define NAK             0x15

#define LEN_MAX_RS232   1024        

int time_diff(STAT_TIME *old_time, STAT_TIME *new_time);
extern uint32_t	GetEpoch(uint8_t YYYYMMDDhhmmss[14]);

extern void LongToStr(uint32_t ldat, uint8_t *str);

extern int gl_SupEn;  // IC�����ն˶�֧��Ӣ��
extern int gl_SupCN;  // IC�����ն˶�֧������

#define STAR_YEAR 1950
uint8_t IsLeapYear(uint32_t inyear);

extern int gl_FirstRunSelApp;

/* 
	��õ���ǰ���ڵ��������
	��������1980��1��1�տ�ʼ����
	
��ڣ� ������YYYYMMDD
����:  ����

ע�⣺ ���������ʱ��������Ҫ��,
		�ú���û��ʶ������ʱ���Ƿ���Ч
		����ʹ�б���Ҳ�޷���֪�����
*/
const uint16_t monthday[12]=
{
	0,		
	31,		//1 
	31+28,		//2 
	31+28+31,		//3 
	31+28+31+30,		//4 
	31+28+31+30+31,		//5 
	31+28+31+30+31+30,		//6 
	31+28+31+30+31+30+31,		//7 
	31+28+31+30+31+30+31+31,		//8 
	31+28+31+30+31+30+31+31+30,		//9 
	31+28+31+30+31+30+31+31+30+31,		//10
	31+28+31+30+31+30+31+31+30+31+30,		//11
};
uint8_t AppEnrichGetAmount(int length, uint8_t flag)//��λԪ
{
	uint8_t	buf[17], szAmount[16];
	int	amt, iRet;
	//uint32_t	tmp;
	uint8_t   szDisp1[17], szDisp2[17];

	if( flag==0 && (PosCom.ucSwipedFlag==CARD_INSERTED || PosCom.bSkipEnterAmt) )
	{
		if (stTemp.iTransNo != EC_REFUND)
		{
			return OK;
		}
	}

	if( flag==3 && (PosCom.ucSwipedFlag==CARD_INSERTED || PosCom.bSkipEnterAmt) )
	{
		return OK;
	}
	
	lcdClrLine(2, 7);
	lcdFlip();	
	if( flag==TRAN_AMOUNT )
	{
		DispMulLanguageString(0, 2, DISP_CFONT, "��������:(Ԫ) ", "INPUT AMOUNT:");
		DispMulLanguageString(0, 4, DISP_CFONT, " ��Ч���5������", "INPUT AMOUNT:");
	}
	else if( flag==IC_AMOUNT )
	{
		if( gl_SupEn==1 )
			lcdDisplay(0, 2, DISP_CFONT, "ENTER AMOUNT:");
		else	
			lcdDisplay(0, 2, DISP_CFONT, "��������:  ");
	}
    else if( flag==ADJUST_AMOUNT ) 
	{
		memset(szDisp1, 0, sizeof(szDisp1));
		memset(szDisp2, 0, sizeof(szDisp2));
		ConvBcdAmount(PosCom.stTrans.sAmount, szDisp1);
		sprintf((char *)szDisp2, "ԭ���:%s", szDisp1);
		lcdDisplay(0, 2, DISP_CFONT, (char *)szDisp2);
		DispMulLanguageString(0, 4, DISP_CFONT, "�����������: ", "INPUT AMOUNT:");
	}
	else
	{
		DispMulLanguageString(0, 2, DISP_CFONT, "������С�ѽ��: ", "INPUT FEE:");
	}
	memset(szAmount, 0, 16);
    
	memset(buf, 0, sizeof(buf));
	do{	
		memset(buf, 0, sizeof(buf));
		lcdClrLine(6,7);
		lcdGoto(48, 6);
		iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 1, length, (stPosParam.ucOprtLimitTime*1000), buf);	
		if( iRet<0 )		
			return E_TRANS_CANCEL;
		amt = atol((char *)buf);
		
	}while( amt==0 && flag!=TIP_AMOUNT || (amt%5 !=0));
	   // if
        amt=amt*100;
		sprintf((char *)szAmount, "%012d", amt);
		AscToBcd(PosCom.stTrans.sAmount, szAmount, 12);
	

	return(OK);
}

uint8_t AppGetAmount(int length, uint8_t flag)
{
	uint8_t	buf[17], szAmount[16];
	int	amt, iRet;
	uint32_t	tmp;
	uint8_t   szDisp1[17], szDisp2[17];

	if( flag==0 && (PosCom.ucSwipedFlag==CARD_INSERTED || PosCom.bSkipEnterAmt) )
	{
		if (stTemp.iTransNo != EC_REFUND)
		{
			return OK;
		}
	}

	if( flag==3 && (PosCom.ucSwipedFlag==CARD_INSERTED || PosCom.bSkipEnterAmt) )
	{
		return OK;
	}
	
	lcdClrLine(2, 7);
	lcdFlip();	
	if( flag==TRAN_AMOUNT )
	{
		DispMulLanguageString(0, 2, DISP_CFONT, "��������:  ", "INPUT AMOUNT:");
	}
	else if( flag==IC_AMOUNT )
	{
		if( gl_SupEn==1 )
			lcdDisplay(0, 2, DISP_CFONT, "ENTER AMOUNT:");
		else	
			lcdDisplay(0, 2, DISP_CFONT, "��������:  ");
	}
    else if( flag==ADJUST_AMOUNT ) 
	{
		memset(szDisp1, 0, sizeof(szDisp1));
		memset(szDisp2, 0, sizeof(szDisp2));
		ConvBcdAmount(PosCom.stTrans.sAmount, szDisp1);
		sprintf((char *)szDisp2, "ԭ���:%s", szDisp1);
		lcdDisplay(0, 2, DISP_CFONT, (char *)szDisp2);
		DispMulLanguageString(0, 4, DISP_CFONT, "�����������:  ", "INPUT AMOUNT:");
	}
	else
	{
		DispMulLanguageString(0, 2, DISP_CFONT, "������С�ѽ��:  ", "INPUT FEE:");
	}
	memset(szAmount, 0, 16);
    
	memset(buf, 0, sizeof(buf));
	do{	
		lcdGoto(48, 6);
		iRet = kbGetString(KB_EN_NUM+KB_EN_FLOAT+KB_EN_BIGFONT+KB_EN_REVDISP, 1, length, (stPosParam.ucOprtLimitTime*1000), buf);	
		if( iRet<0 )		
			return E_TRANS_CANCEL;
		amt = atol((char *)buf);
	}while( amt==0 && flag!=TIP_AMOUNT );
	
    if( flag==TIP_AMOUNT )
	{
		tmp = BcdToLong(PosCom.stTrans.sAmount, 6);
		tmp = tmp * stPosParam.ucTipper; 
		if( amt>(tmp/100) )	
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  ��������Ч  ", "INVALID AMOUNT");
			DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "   ��С�Ѷ��   ", "EXCEED FEE");
			ErrorBeep();
			kbGetKeyMs(2000);	
			return NO_DISP;
			
		}
		sprintf((char *)szAmount, "%012d", amt);
		AscToBcd(PosCom.stTrans.sTipAmount, szAmount, 12);
	}
	else
	{
		sprintf((char *)szAmount, "%012d", amt);
		AscToBcd(PosCom.stTrans.sAmount, szAmount, 12);
	}

	return(OK);
}

// ȷ�Ͻ��
uint8_t AppConfirmAmount(void)
{
	uint8_t	buf[17];
	int	iRet;
	
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "��ȷ�Ͻ��:  ", "INPUT AMOUNT:");
	memset(buf, 0, sizeof(buf));
	ConvBcdAmount(PosCom.stTrans.sAmount, buf);
	lcdDisplay(0, 4, DISP_CFONT, (char *)buf);
	lcdDisplay(0, 6, DISP_CFONT, "��[ȡ��]���޸�");
	iRet = kbGetKey();
	if( iRet==KEY_CANCEL )
	{
		return iRet;
	}
    
	return(OK);
}

void GetPosTime(uint8_t *pszDate, uint8_t *pszTime)
{
	uint8_t sDateTime[8];
	
	sysGetTime(sDateTime);

	memcpy(pszDate, "20", 2);

	BcdToAsc0(&pszDate[2], sDateTime, 6);
	BcdToAsc0(pszTime, &sDateTime[3], 6);
}

// �ȴ��������¼�
int  WaitEvent(void)
{
	uint8_t	 ucReadCnt;//ucRet,
	uint8_t	szLastTime[6+1];
	uint8_t	szDate[8+1];	
	uint8_t	szTime[6+1];
	//uint8_t   ucRet1;
	//int   iKey=0;
//	int   iKeyValue;
	int   iRet;//, iCnt = 0;
	//int   timeout;
	
	magClose();
	magOpen();
	magReset();

	if (stPosParam.ucqPbocSupport == PARAM_OPEN && imif_fd < 0)
	{	
		imif_fd = contactness_open();
		iRet = contactness_poll(imif_fd);
	}
	
	ucReadCnt = 0;
	kbFlush();
	sprintf((char *)szLastTime, "000000");
	while( 1 )
	{
		if( kbhit()==YES )
		{
			if (stPosParam.ucqPbocSupport == PARAM_OPEN)
			{
				contactness_close(imif_fd);
				imif_fd = -1;
			}
			return kbGetKey();
		}
		
		/*if( magSwiped()==YES )
		{
			if (stPosParam.ucqPbocSupport == PARAM_OPEN)
			{
				contactness_close(imif_fd);
				imif_fd = -1;
			}
			if( stTransCtrl.ucDefaltTxn==1 )
			{
				DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    ��    ��    ", "      SALE      ");
			}
			else
			{
				stTemp.iTransNo = POS_PREAUTH;
				DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "     Ԥ��Ȩ     ", "    PREAUTH     ");
			}

			ucRet = SwipeCardProc(TRUE);
			if( ucRet!=OK )
			{
				if( ucRet==E_ERR_SWIPE )
				{
					DispResult(E_ERR_SWIPE);
					return NO_DISP;
				}
				if( ucRet==E_NEED_INSERT )
				{
REINSERT:
					ShowGetCardScreen(CARD_INSERTED);
					ucRet1 = DetectCardEvent(CARD_INSERTED);
					if( ucRet1==CARD_KEYIN )
					{
						iKey = kbGetKey();
						if( iKey==KEY_CANCEL )
						{
							return E_TRANS_CANCEL;
						}
						else
						{
							goto REINSERT;
						}
					}					
					else if( ucRet1==CARD_INSERTED )
					{
						emv_pboc_ec_enable(0);	// �����ֽ��ܹر�
						ucRet1 = InsertCardProc(TRUE);
						if( ucRet1==E_NEED_FALLBACK )
						{
							PosCom.ucFallBack = TRUE;
							ShowGetCardScreen(CARD_SWIPED);
							ucRet1 = DetectCardEvent(CARD_SWIPED);
							if (ucRet1 != CARD_SWIPED)
							{
								return NO_DISP;
							}

							continue;
						}
						else if( ucRet1==E_CARD_BLOCKER )
						{
							lcdClrLine(2, 7);
							lcdFlip();	
							lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "��������");
							ErrorBeep();
							kbGetKeyMs(2000);		

							DispRemoveICC();
							return NO_DISP;
						}
						else if( ucRet1==E_APP_BLOCKED )
						{
							lcdClrLine(2, 7);
							lcdFlip();	
							lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "Ӧ������");
							ErrorBeep();
							kbGetKeyMs(2000);		

							DispRemoveICC();
							return NO_DISP;
						}
						else if( ucRet1==E_TRANS_FAIL )
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
							return ucRet1;
						}
					}					
				}
				return ucRet;
			}
			return DispCardNo(PosCom.stTrans.szCardNo);
		}*/

		/*if( (stPosParam.ucEmvSupport==PARAM_OPEN) && (PosCom.ucFallBack==FALSE) &&
			(iccDetect(ICC_USER)==ICC_SUCCESS) )
		{
			if (stPosParam.ucqPbocSupport == PARAM_OPEN)
			{
				contactness_close(imif_fd);
				imif_fd = -1;
			}
			if( stTransCtrl.ucDefaltTxn==1 )
			{
				DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    ��    ��    ", "      SALE      ");
			}
			else
			{
				stTemp.iTransNo = POS_PREAUTH;
				DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "     Ԥ��Ȩ     ", "    PREAUTH     ");
			}

			ucRet = InsertCardProc(TRUE);
			if( ucRet==E_NEED_FALLBACK )
			{
				ucReadCnt++;
				lcdClrLine(2, 7);
				lcdFlip();	
				PosCom.ucFallBack = TRUE;
				DispMulLanguageString(0, 2, DISP_CFONT, "��ˢ��-->", "PLS SWIPE CARD->");
				if (ucReadCnt > 2)
				{
					ucReadCnt = 0;
					return NO_DISP;
				}
				continue;
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
				DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "    ���׾ܾ�    ", " TRANS REJECT");
				ErrorBeep();
				kbGetKeyMs(2000);					

				DispRemoveICC();
				return NO_DISP;
			}
			return ucRet;

		}
		if ( stPosParam.ucqPbocSupport == PARAM_OPEN)
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
						PosCom.stTrans.iTransNo = EC_QUICK_SALE;
						stTemp.iTransNo = EC_QUICK_SALE;
						iRet = ContackCardProc(TRUE);  // �ǻ������¼�
						contactness_close(imif_fd);
						imif_fd = -1;
						return iRet;
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
			}
			else
			{
				if (iCnt < 1 )
				{ // ��ü��2��
					iCnt++;
					contactness_poll(imif_fd);
					continue;
				}

				PosCom.stTrans.iTransNo = EC_QUICK_SALE;
				stTemp.iTransNo = EC_QUICK_SALE;
				iRet = ContackCardProc(TRUE);  // �ǻ������¼�
				contactness_close(imif_fd);
				imif_fd = -1;
				return iRet;
			}			
		}*/
		
		GetPosTime(szDate, szTime);
		if( memcmp(szLastTime, szTime, 3)!=0 )
		{
			magClose();
			magOpen();
			magReset();
		}
		if( memcmp(szLastTime, szTime, 6)!=0 )
		{
			lcdDisplay(0, 7, DISP_ASCII|DISP_CLRLINE, "%.4s/%.2s/%.2s  %.2s:%.2s:%.2s",
					szDate, szDate+4, szDate+6, szTime, szTime+2, szTime+4);
			sprintf((char *)szLastTime, "%.6s", szTime);
		}
	}
}

uint8_t SelectTrans(void)
{
    int key;
	int keySub; //�����˵����ؽ�ֵ
	int iRet;
	UINT8 ucRet;
	//int  iNowTime;

	/*const char *mainMenu[] = 
	{
		"1 ����  2 ��  ��",
		"3 �˻�  4 Ԥ��Ȩ",
		"5 ����  6 ��  ӡ",
		"7 ����  8 ��  ��"
	};*/
	/*const char *mainMenu[] = 
	{
		"1 �³�    2 �ͳ�",
		"3 �ۿ�    4 ����",
		"5 ���ݴ���6 ���",
		"7 ��¼��ѯ8 ����"

	};*/
		const char *mainMenu[] = 
	{
	    "��������IC����ֵ",
		"1 �ͳ�    2 ���",
		"3 ����    4 ��ӡ",  //"3 ���ݴ���4 ����",    4  ��ӡ
		"5 ��¼��ѯ6 ����"
		

	};
	const char* dataProcessingMenu[] =
	{
     "1 ��ѯ�������  ",
	 "2 ��ֵ�����    ",
	 "3 ��ӡ�ϴν��㵥",
	 "4 �����ϴ�      ",
	 //"5 ����ֵ���ڲ���",
	 //"6 ���������ڲ���",
	 //"7 �鿴δ�ϴ��ļ�"
	};
	/*const char* manMenu[] = 
	{
		"1 ǩ��    2 ǩ��",
		"3 ���ײ�ѯ4 ��Ա",
		"5 ���ߺ���6 ����",
		"7 �����ն�8 �汾"
	};*/
		const char* manMenu[] = 
	{
		"1 ǩ��    2 �汾",
		"3 ʱ��    4 ����"
	//	"5 �ش��ϱʽ��㵥"
		//"5 ���ߺ���6 ����",
		//"7 �����ն�8 �汾"
	};
	const char *query_menu[] = 
	{
		"���ײ�ѯ        ",
		"1 ��ѯ������ϸ  ",
		"2 ��ѯ���׻���  ",
		"3 ����ˮ�Ų�ѯ  "
	};

	/*const char *off_menu[] = 
	{
		"    ���߽���    ",
		"1 ���߽���      ",
		"2 �������      "
	};*/

	/*const char *else_menu[] = 
	{
		"1 �����ֽ�      ",
		"2 ����Ǯ��      ",
		"3 ���ڸ���      ",
		"4 ����ѯ      ",
		"5 ���ֶһ�      ",
		"6 �ֻ�оƬ      ",
		"7 ԤԼ����      ",
		"8 ����          ",
		"9 ��������ֵ    "
	};*/

	/*const char* pre_menu[] = 
	{
		"1 Ԥ��Ȩ        ",
		"2 Ԥ��Ȩ�������",
		"3 Ԥ��Ȩ���֪ͨ",			
		"4 Ԥ��Ȩ����    ",
		"5 Ԥ��Ȩ��ɳ���"
	};*/

	const char* after_menu[] = 
	{
		"1 ��ѯ���      ",
		"2 ����          "
	};
	
	while(1) 
	{	
		//iNowTime= sysGetTimerCount();
		//if( (iNowTime-stPosParam.OldTime) >= (UINT32)(1*60*1000) )	// ��鶨ʱ��
		//{
			//iRet = ERR_COMM_TIMEOUT;
			//sysPowerOff();
		//}
//NacTxd("1234",4);

		key = DispMenu(mainMenu, 4, 1, KEY1, KEY6, stPosParam.ucOprtLimitTime);
		switch(key)
		{
		/*case KEY1:
			//stTemp.iTransNo = M1_MONTHENRICH;
			lcdCls();
			lcdFlip();	
			lcdClrLine(2, 7);
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " �ݲ�֧�ָý��� ", "NOT SUPPORT");
			ErrorBeep();
			kbGetKeyMs(2000);	
			
			return NO_DISP;
			break;	*/			  			
		case KEY1://�ͳ�
			stTemp.iTransNo =M1_ENRICH;//stTemp.iTransNo = POS_SALE_VOID;				
			return OK;
			break;				  
		/*case KEY3:
			//stTemp.iTransNo = M1_SELL;
			lcdCls();
			lcdFlip();	
			lcdClrLine(2, 7);
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " �ݲ�֧�ָý��� ", "NOT SUPPORT");
			ErrorBeep();
			kbGetKeyMs(2000);	
			
			return NO_DISP;
			break;	*/
			//return OK;
			//break ;
		case KEY2://���
			keySub = DispMenu(after_menu, 2, 0, KEY1, KEY2, stPosParam.ucOprtLimitTime);
			if( keySub==KEY1 )//��ѯ���
			{
				stTemp.iTransNo = M1_RAEDCARD;
			}
			else if( keySub==KEY2 )//����
			{
				stTemp.iTransNo = POS_REVERSE;
			}
			else
			{
				continue;
			}
			return OK;
			break;						
		case KEY3://����
			/*keySub = DispMenu(dataProcessingMenu, 7, 0, KEY1, KEY7, stPosParam.ucOprtLimitTime);
			if( keySub==KEY1 )
				stTemp.iTransNo = M1_QUE_SETT;		
			else if( keySub==KEY2 )
				stTemp.iTransNo = M1_SETT;
			else if( keySub==KEY3 )
				stTemp.iTransNo = M1_SETT_PRT;
			else if( keySub==KEY4 )
				stTemp.iTransNo = M1_DATA_UPLOAD;
			else if( keySub==KEY5 )
				stTemp.iTransNo = M1_SLAEDATE_UPLOAD;
			else if( keySub==KEY6 )
				stTemp.iTransNo = M1_SETTDATE_UPLOAD;
			else if( keySub==KEY7 )
				stTemp.iTransNo = M1_QUERY_NOT_UPLOAD;
			else 
				continue;*/
			stTemp.iTransNo =M1_SETT;
			return OK;
			break;			
		/*case KEY6:
			lcdCls();
			lcdFlip();	
			lcdClrLine(2, 7);
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "���޴˹���", "NOT SUPPORT");
			ErrorBeep();
			kbGetKeyMs(2000);	
			//M1SelectPrintFunc();
			return NO_DISP;//M1SelectPrintFunc();
			break;	*/
			//if( SelectPrintFunc() ) 
			//	continue;
			//else 
			//	return OK;
			//break;
		case KEY5://��¼��ѯ
			keySub = DispMenu(query_menu, 4, 1, KEY1, KEY3, stPosParam.ucOprtLimitTime);
				if( keySub==KEY1 )//��ѯ������ϸ
					stTemp.iTransNo = M1_QUE_BATCH_MX;
				else if( keySub==KEY2 )//��ѯ���׻���
					stTemp.iTransNo = M1_QUE_BATCH_TOTAL;
				else if( keySub==KEY3 )//����ˮ�Ų�ѯ
					stTemp.iTransNo = M1_QUE_BATCH_ANY;
				else 
				continue;
			return OK;
			break;
		case KEY6://����
			keySub = DispMenu(manMenu, 2, 0, KEY1, KEY4, stPosParam.ucOprtLimitTime);
			if( keySub==KEY1 )//ǩ��
				stTemp.iTransNo = POS_LOGON;
			else if( keySub==KEY2 )//�汾
				stTemp.iTransNo = OPER_QUE_TERM_INFO;
			else if( keySub==KEY3 )//ʱ��
			{
                DispDateTime();
			  	continue;
				//  DispMenu(const char * menu [ ],int iLines,int iTitle,int iStartKey,int iEndKey,int iTimeOut)
			}
			else if( keySub==KEY4 )//����
			{
				iRet = CheckParamPass();
				if( iRet!=1 )
				{
					continue;
				}
		
				SetupPosPara();
				return NO_DISP;
			}
			else
			{
				continue;
			}
			//stTemp.iTransNo = M1_QUE_BATCH_ANY;
		
			//stTemp.iTransNo =OPER_QUE_TERM_INFO; 

			return OK;
			break;

		case KEY4:   // ��ӡ		
	    	ucRet=	M1SelectPrintFunc();
			/*iRet = CheckParamPass();
			if( iRet!=1 )
			{
				continue;
			}

			SetupPosPara();*/

			return ucRet;
			/*keySub = DispMenu(else_menu, 9, 0, KEY1, KEY9, stPosParam.ucOprtLimitTime);
			if( keySub==KEY1 ) 
			{

				if( SelectEcFunc() ) 
					continue;
				else 
					return OK;
			}
			else if( keySub==KEY2 )
			{
				lcdCls();
				lcdFlip();	
				lcdClrLine(2, 7);
				DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " �ݲ�֧�ָý��� ", "NOT SUPPORT");
				ErrorBeep();
				kbGetKeyMs(2000);					
				continue;

			}
			else if( keySub==KEY3 )
			{
				if( SelectInstalFunc() ) 
					continue;
				else 
					return OK;
			}			
			else if( keySub==KEY4 )
			{
				stTemp.iTransNo = POS_QUE;	
			}
			else if( keySub==KEY5 )
			{

				if( SelectLoyaltFunc() ) 
					continue;
				else 
					return OK;
			}
			else if( keySub==KEY6 )
			{

				if( SelectMobileFunc() ) 
					continue;
				else 
					return OK;
			}
			else if( keySub==KEY7 )
			{

				if( SelectBookFunc() ) 
					continue;
				else 
					return OK;
			}
			else if( keySub==KEY8 )
			{

				if( SelectMotoFunc() ) 
					continue;
				else 
					return OK;
			}
			else if( keySub==KEY9 )
			{
				if( SelectMagTopupFunc() ) 
					continue;
				else 
					return OK;
			}
			else 
				continue;
			return OK;*/
			break;  			
		case KEY_ENTER: //1~2
			break;
		default:
			return NO_DISP;
			break;
		}
	}
	return(NO_DISP);
}

uint8_t M1SelectPrintFunc(void)
{
	int keySub;
	const char* print_menu[] = {
		"1 �ش����һ��  ",
		"2 �ش�����һ��  ",
		"3 ��ӡ���׻���  ",
		"4 �ش��ϱʽ��㵥"
	};

	keySub = DispMenu(print_menu, 4, 0, KEY1, KEY4, stPosParam.ucOprtLimitTime);
	if( keySub==KEY1 ) 
		stTemp.iTransNo = M1_PRT_LAST;
	else if( keySub==KEY2 ) 
		stTemp.iTransNo = M1_PRT_ANY;
	/*else if( keySub==KEY3 ) 
		stTemp.iTransNo = M1_PRT_DETAIL;*/
	else if( keySub==KEY3 ) 
		stTemp.iTransNo = M1_PRT_TOTAL;
	else if( keySub==KEY4 ) 
		stTemp.iTransNo = M1_SETT_PRT;
	else 
		return NO_DISP;
	return OK;
}

uint8_t SelectPrintFunc(void)
{
	int keySub;
	const char* print_menu[] = {
		"1 �ش����һ��  ",
		"2 �ش�����һ��  ",
		"3 ��ӡ������ϸ  ",
		"4 ��ӡ���׻���  "
	};

	keySub = DispMenu(print_menu, 4, 0, KEY1, KEY4, stPosParam.ucOprtLimitTime);
	if( keySub==KEY1 ) 
		stTemp.iTransNo = PRT_LAST;
	else if( keySub==KEY2 ) 
		stTemp.iTransNo = PRT_ANY;
	else if( keySub==KEY3 ) 
		stTemp.iTransNo = PRT_DETAIL;
	else if( keySub==KEY4 ) 
		stTemp.iTransNo = PRT_TOTAL;
	else 
		return NO_DISP;
	return OK;
}

uint8_t LogonFunc(void)
{
	int keySub;
	const char* logon_menu[] = {
		"ǩ��            ",
		"1 POSǩ��       ",
		"2 ����Աǩ��    ",
		"3 ����Ա����ǩ��"
	};

	keySub = DispMenu(logon_menu, 4, 1, KEY1, KEY3, stPosParam.ucOprtLimitTime);

	if( keySub==KEY1 )
	{
		stTemp.iTransNo = POS_LOGON;
	}
	else if( keySub==KEY2 ) 
	{
		PosOperLogon(1);
	}
	else if( keySub==KEY3 ) 
	{
/*		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " �ݲ�֧�ָý��� ", "NOT SUPPORT");
		ErrorBeep();
		kbGetKeyMs(2000);*/
		stTemp.iTransNo = POS_LYT_LOGON;
	}
	else 
		return NO_DISP;
	return OK;
}


uint8_t SelectPosManFunc(void)
{
	const char *manmenu[]={
		"    �ն˹���    ",
		"1 �������      ",
		"2 ��������      ",
		"3 ״̬����      ",
		"4 ��Կ����      ",
		"5 EMV��������   ",
		"6 ϵͳ����      ",
		"7 �鿴EMV���   ",
		"8 ����������    ",
		"9 ʧ�ܼ�¼��ѯ  ",
//#ifdef _EMV_TEST_VERSION
//		"8 ��ӡͨѶ����  ",
//		"9 ����ͨѶ����  ",
//#endif
	};
	int key;


//#ifdef _EMV_TEST_VERSION
//		key = DispMenu(manmenu, 10, 1, KEY1, KEY9, stPosParam.ucOprtLimitTime);
//#else
		key = DispMenu(manmenu, 10, 1, KEY1, KEY9, stPosParam.ucOprtLimitTime);
//#endif
	switch(key) {
	case KEY1:
		stTemp.iTransNo = RESPONSE_TEST;
		break;
	case KEY2:
//20080804 PBOC����������ʱ����
#ifdef PBOC_VERSION
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " �ݲ�֧�ָý��� ", "NOT SUPPORT");
		ErrorBeep();
		kbGetKeyMs(2000);					
#else		
		stTemp.iTransNo = POS_DOWNLOAD_PARAM;
#endif
		break;
	case KEY3:
		stTemp.iTransNo = POS_UPLOAD_PARAM;
		break;
	case KEY4:
		if( CheckParamPass() != 1)
		{
			return NO_DISP;
		}
		stTemp.iTransNo = DOWNLOAD_EMV_CAPK;
		break;
	case KEY5:
		if( CheckParamPass() != 1)
		{
			return NO_DISP;
		}
		stTemp.iTransNo = DOWNLOAD_EMV_PARAM;
		break;
	case KEY6:
		SetParaFunc();
		return NO_DISP;
		break;
	case KEY7:
		if( CheckParamPass() != 1)
		{
			return NO_DISP;
		}
		ViewEmvResult();
		return NO_DISP;
		break;
	case KEY8:
		if( CheckParamPass() != 1)
		{
			return NO_DISP;
		}
		TransDownloadBlkCard();
		return NO_DISP;
		break;
	case KEY9:
		ViewIccFailureLog();
		return NO_DISP;
	break;

//20080715 for Pboc
//#ifdef _EMV_TEST_VERSION
//	case KEY8:
//		PrnCommLog();
//		break;
//
//	case KEY9:
//		SndCommLog();
//		break;
//#endif
		break;
	default:
		return NO_DISP;
	}
	return OK;
}

void dispRetCode(void)
{
	int i;
	uint8_t tmpbuf[20], dispStr1[20], szDispstr2[20];
	
	lcdClrLine(2, 7);	
	lcdFlip();	
	if( (memcmp(PosCom.szRespCode, "00", 2)==0) )
	{
		return;
	}
	
	for(i=0; i<ERR_INFO_NUM; i++) 
	{
		if( memcmp(sRetErrInfo[i].ucRetCode, (char*)PosCom.szRespCode, 2)==0 )
			break;
	}
	memset(szDispstr2,0,sizeof(szDispstr2));
	memset(dispStr1,0,sizeof(dispStr1));
	memset(tmpbuf,0,sizeof(tmpbuf));
	if( i==ERR_INFO_NUM )
	{
		if( stPosParam.iDispLanguage==1 )
		{
			strcpy((char *)dispStr1, (char *)"����ʧ��");
			memcpy(tmpbuf, "������:", 7);
			sprintf((char *)(tmpbuf+7), "[%.2s]", PosCom.szRespCode);
			tmpbuf[11] = 0;
		}
		else
		{
			strcpy((char *)dispStr1, (char *)"TRAN FAIL");
			memcpy(tmpbuf, "ERRCODE:", 8);
			sprintf((char *)(tmpbuf+8), "[%.2s]", PosCom.szRespCode);
			tmpbuf[12] = 0;
		}
	}
	else
	{
		strcpy((char *)dispStr1, (char *)sRetErrInfo[i].szMessage);
		if (strlen((char *)sRetErrInfo[i].szOperate)>16)
		{
			memcpy((char *)tmpbuf, (char *)sRetErrInfo[i].szOperate,16);
			strcpy((char *)szDispstr2, (char *)sRetErrInfo[i].szOperate+16);
		}
		else
		{
			strcpy((char *)tmpbuf, (char *)sRetErrInfo[i].szOperate);
		}
	}

	lcdDisplay(0, 2, DISP_MEDIACY|DISP_CFONT, (char *)dispStr1);
	lcdDisplay(0, 4, DISP_MEDIACY|DISP_CFONT, (char *)tmpbuf);
	lcdDisplay(0, 6, DISP_MEDIACY|DISP_CFONT, (char *)szDispstr2);
	sysBeef(3, 1000);
	kbGetKey(); ///�ȴ�����Աȷ��
	
	return;
}

void ConvErrCode(uint8_t errCode, char *szErrInfo)
{	
	switch(errCode) 
	{
	case	0:
		strcpy((char *)szErrInfo, "���׳ɹ�"); 
		break;
	case	1:	
		strcpy((char *)szErrInfo, "����ȡ��"); 
		break;
	case	2:	
		strcpy((char *)szErrInfo, "����ʧ��"); 
		break;
	case	3:	
		strcpy((char *)szErrInfo, "�޽���"); 
		break;
	case	4:	
		strcpy((char *)szErrInfo, "�������"); 
		break;
	case	5:	
		strcpy((char *)szErrInfo, "����ʧ��"); 
		break;
	case	6:	
		strcpy((char *)szErrInfo, "���ݷ���ʧ��"); 
		break;
	case	7:	
		strcpy((char *)szErrInfo, "���ݽ���ʧ��"); 
		break;
	case	8:	
		strcpy((char *)szErrInfo, "�������"); 
		break;
	case	9:	
		strcpy((char *)szErrInfo, "����ʧ��"); 
		break;
	case	10:	
		strcpy((char *)szErrInfo, "ԭ���ײ�����"); 
		break;
	case	11:	
		strcpy((char *)szErrInfo, "ԭ�����ѳ���"); 
		break;
	case	12:	
		strcpy((char *)szErrInfo, "ˢ������"); 
		break;
	case	13:	
		strcpy((char *)szErrInfo, "�ļ�����ʧ��"); 
		break;
	case	14:	
		strcpy((char *)szErrInfo, "��Կ����"); 
		break;
	case	15:	
		strcpy((char *)szErrInfo, "С�ѽ���"); 
		break;
	case	16:	
		strcpy((char *)szErrInfo, "С�ѽ���δ��ͨ"); 
		break;
	case	17:	
		strcpy((char *)szErrInfo, "�����Ѿ�������"); 
		break;
	case	18:	
		if( stPosParam.iDispLanguage==1 )
			strcpy((char *)szErrInfo, "���ļ���"); 
		else
			strcpy((char *)szErrInfo, "OPEN FILE ERROR"); 
		break;
	case	19:	
		strcpy((char *)szErrInfo, "��λ�ļ���"); 
		break;
	case	20:	
		strcpy((char *)szErrInfo, "���ļ���"); 
		break;
	case	21:	
		strcpy((char *)szErrInfo, "д�ļ���"); 
		break;
	case	22:	
		strcpy((char *)szErrInfo, "�հ�MACУ���"); 
		break;
	case	23:	
		strcpy((char *)szErrInfo, "��оƬ��"); 
		break;
	case    25:
		strcpy((char *)szErrInfo, "���׾ܾ�"); 
		break;

	default	:	
		strcpy((char *)szErrInfo, "��������"); 
		break;
	}	
	return;
}

void GetTransName(int tCode, uint8_t *szName)
{	
	switch(tCode) 
	{        
	case POS_SALE:
	case ICC_OFFSALE:
		if( stPosParam.iDispLanguage==1 )
			strcpy((char *)szName, "����");
		else
			strcpy((char *)szName, "SALE");
		break;	//1
	case POS_PREAUTH:
		if( stPosParam.iDispLanguage==1 )
			strcpy((char *)szName, "Ԥ��Ȩ");
		else
			strcpy((char *)szName, "PREAUTH");
		break;
	case POS_AUTH_CM:
		if( stPosParam.iDispLanguage==1 )
			strcpy((char *)szName, "Ԥ��Ȩ���(����)");
		else
			strcpy((char *)szName, "PRE CMP(ONLINE)");
		break;
	case POS_OFF_CONFIRM:
		if( stPosParam.iDispLanguage==1 )
			strcpy((char *)szName, "Ԥ��Ȩ���(֪ͨ)");
		else
			strcpy((char *)szName, "PRE CMP(OFFLINE)");
		break;			
	case POS_PREAUTH_VOID:
		if( stPosParam.iDispLanguage==1 )
			strcpy((char *)szName, "Ԥ��Ȩ����");
		else
			strcpy((char *)szName, "PRE VOID");
		break;
	case POS_AUTH_VOID:
		if( stPosParam.iDispLanguage==1 )
			strcpy((char *)szName, "Ԥ��Ȩ��ɳ���");
		else
			strcpy((char *)szName, "PRE CMP VOID");
		break;
	case POS_SALE_VOID:
		if( stPosParam.iDispLanguage==1 )
			strcpy((char *)szName, "���ѳ���");
		else
			strcpy((char *)szName, "SALE VOID");
		break;
	case POS_PREAUTH_ADD:
		if( stPosParam.iDispLanguage==1 )
			strcpy((char *)szName, "׷��Ԥ��Ȩ");
		else
			strcpy((char *)szName, "PREAUTH ADD");
		break;
	case POS_REFUND:
		if( stPosParam.iDispLanguage==1 )
			strcpy((char *)szName, "�˻�");
		else
			strcpy((char *)szName, "REFUND");
		break;
	case POS_QUE:
		if( stPosParam.iDispLanguage==1 )
			strcpy((char *)szName, "�����");
		else
			strcpy((char *)szName, "BALANCE");
		break;
	case OFF_SALE:
		if( stPosParam.iDispLanguage==1 )
			strcpy((char *)szName, "���߽���");
		else
			strcpy((char *)szName, "SETTLE(OFFLINE)");
		break;
	case ADJUST_TIP:
	case OFF_ADJUST:
		if( stPosParam.iDispLanguage==1 )
			strcpy((char *)szName, "�������");
		else
			strcpy((char *)szName, "ADJUST");
		break;	
		
	case EC_QUICK_SALE:
	case QPBOC_ONLINE_SALE:
			strcpy((char *)szName, "�����ֽ����֧��");
		break;

	case EC_NORMAL_SALE:
		strcpy((char *)szName, "�����ֽ���֧ͨ��");
		break;

	case EC_TOPUP_CASH:
		strcpy((char *)szName, "�����ֽ��ֽ�Ȧ��");
		break;

	case EC_TOPUP_SPEC:
		strcpy((char *)szName, "ָ���ʻ�Ȧ��");
		break;

	case EC_TOPUP_NORAML:
		strcpy((char *)szName, "��ָ���˻�Ȧ��");
		break;

	case EC_VOID_TOPUP:
		strcpy((char *)szName, "�ֽ�Ȧ�泷��");
		break;

	case EC_REFUND:
		strcpy((char *)szName, "�����ֽ��˻�");
		break;

	case ED_SALE:
		strcpy((char *)szName, "����Ǯ��֧��");
		break;

	case ED_TOPUP_CASH:
		strcpy((char *)szName, "����Ǯ���ֽ�Ȧ��");
		break;

	case ED_TOPUP_SPEC:
		strcpy((char *)szName, "Ǯ��ָ���ʻ�Ȧ��");
		break;

	case ED_TOPUP_NORAML:
		strcpy((char *)szName, "��ָ���ʻ�Ȧ��");
		break;
		
	case POS_INSTALLMENT:
		strcpy((char *)szName, "��������");
		break;
		
	case POS_VOID_INSTAL:
		strcpy((char *)szName, "�������ѳ���");
		break;

	case POS_LOYATL_ISSUER:
		strcpy((char *)szName, "�����л�������");
		break;
		
	case POS_LOYATL_ALL:
		strcpy((char *)szName, "���˻�������");
		break;

	case POS_VOID_LOYATL_ISSUER:
		strcpy((char *)szName, "�����л��ֳ���");
		break;
		
	case POS_VOID_LOYATL_ALL:
		strcpy((char *)szName, "���˻��ֳ���");
		break;

	case POS_REFUND_LOYATL_ALL:
		strcpy((char *)szName, "���˻����˻�");
		break;

	case POS_MOBILE_SALE:
//		strcpy((char *)szName, "�ֻ�����");
		strcpy((char *)szName, "����");
		break;
		
	case POS_MOBILE_VOID:
//		strcpy((char *)szName, "�ֻ����ѳ���");
		strcpy((char *)szName, "���ѳ���");
		break;
		
	case POS_MOBILE_REFUND:
//		strcpy((char *)szName, "�ֻ�оƬ�˻�");
		strcpy((char *)szName, "�˻�");
		break;
		
	case POS_MOBILE_PREAUTH:
//		strcpy((char *)szName, "�ֻ�оƬԤ��Ȩ");
		strcpy((char *)szName, "Ԥ��Ȩ");
		break;

	case POS_MOBILE_VOID_PRE:
//		strcpy((char *)szName, "�ֻ�Ԥ��Ȩ����");
		strcpy((char *)szName, "Ԥ��Ȩ����");
		break;
		
	case POS_MOBILE_AUTH:
//		strcpy((char *)szName, "�ֻ�Ԥ��Ȩ���");
		strcpy((char *)szName, "Ԥ��Ȩ���");
		break;
		
	case POS_MOBILE_AUTH_OFF:
//		strcpy((char *)szName, "�ֻ�Ԥ��Ȩ֪ͨ");
		strcpy((char *)szName, "Ԥ��Ȩ֪ͨ");
		break;
		
	case POS_MOBILE_VOID_AUTH:
		strcpy((char *)szName, "Ԥ��Ȩ��ɳ���");
		break;

	case POS_MOBILE_QUE:
		strcpy((char *)szName, "����ѯ");
		break;

	case POS_RESER_SALE:
		strcpy((char *)szName, "ԤԼ����");
		break;
		
	case POS_RESER_VOID:
		strcpy((char *)szName, "ԤԼ���ѳ���");
		break;
		
	case POS_MAGTOPUP_CASH:
	case POS_MAGTOPUP_CM:
		strcpy((char *)szName, "�������ֽ��ֵ");
		break;
		
	case POS_MAGTOPUP_ACCT:
		strcpy((char *)szName, "�������ʻ���ֵ");
		break;

	case POS_MOTO_SALE:
		strcpy((char *)szName, "��������");
		break;
		
	case POS_MOTO_PREAUTH:
		strcpy((char *)szName, "����Ԥ��Ȩ");
		break;
		
	case POS_MOTO_AUTH_CM:
		strcpy((char *)szName, "����Ԥ��Ȩ���");
		break;
		
	case POS_MOTO_PREAUTH_VOID:
		strcpy((char *)szName, "����Ԥ��Ȩ����");
		break;
		
	case POS_MOTO_AUTH_VOID:
		strcpy((char *)szName, "������Ȩ��ɳ���");
		break;
		
	case POS_MOTO_SALE_VOID:
		strcpy((char *)szName, "�������ѳ���");
		break;
		
	case POS_MOTO_REFUND:
		strcpy((char *)szName, "�����˻�");
		break;
		
	case POS_MOTO_AUTH_OFF:
		strcpy((char *)szName, "Ԥ��Ȩ���֪ͨ");
		break;

	default	:
		strcpy((char *)szName, "    ");
		break;
	}	
	return;
}


void init_dial_statics(void)
{
	dial_stat.dials = 0;
	dial_stat.fails = 0;
	dial_stat.hours = 0;
	dial_stat.suc_rate = 0;
	write_stat_time();
}
/************************************************************************/
/* ͳ�Ʋ��ųɹ���                                                       */
/************************************************************************/
uint8_t calculate_dial_statics(void)
{
	STAT_TIME stat_time;
	STAT_TIME now_time;
	uint8_t buf[9], B_date[9], B_time[7];
	
	if( read_data(&dial_stat, sizeof(DIAL_STAT), "dial.sta")!=OK ) 
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 3, DISP_CFONT|DISP_MEDIACY, "�����ļ�ʧ��!", "DIAL ERROR");
		kbGetKey();
		return 1;

	}
	
	if( read_data(&stat_time, sizeof(STAT_TIME), "stat.tm")!=OK ) 
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 3, DISP_CFONT|DISP_MEDIACY, "ʱ���ļ�ʧ��!", "TIME ERROR");
		kbGetKey();
		return 1;
	}
	
	GetPosTime(B_date, B_time);
	
	memcpy(buf, B_date, 4);
	buf[4] = 0;
	now_time.year = atoi((char*)buf);
	
	memcpy(buf, B_date+4, 2);
	buf[2] = 0;
	now_time.month = atoi((char*)buf);
	
	memcpy(buf, B_date+6, 2);
	buf[2] = 0;
	now_time.day = atoi((char*)buf);
	
	memcpy(buf, B_time, 2);
	buf[2] = 0;
	now_time.hour = atoi((char*)buf);
	
	dial_stat.hours = time_diff(&stat_time, & now_time);
	
	if( dial_stat.hours<=0 ) 
		dial_stat.hours = 1; 
	if( dial_stat.hours>9999 ) 
		dial_stat.hours = 9999; /*��ֻ֤����λ*/
	
	if( dial_stat.dials==0 ) 
	{
		dial_stat.dials = 0;
		dial_stat.fails = 0;
		dial_stat.suc_rate = 100;
		return OK;
	}
	else
	{
		dial_stat.suc_rate = (int) ((dial_stat.dials - dial_stat.fails) * 100 / dial_stat.dials);
	}
	if( dial_stat.suc_rate>100 ) 
		dial_stat.suc_rate = 100; /*��֤�����λ*/
	return OK;
	
}

/************************************************************************/
/* ��¼����ͳ�ƵĿ�ʼʱ��                                               */
/************************************************************************/
uint8_t write_stat_time(void)
{
	uint8_t B_date[9], B_time[7];
	uint8_t buf[5];
	STAT_TIME stat_time;
	
	GetPosTime(B_date, B_time);
	
	memcpy(buf, B_date, 4);
	buf[4] = 0;
	stat_time.year = atoi((char*)buf);
	
	memcpy(buf, B_date + 4, 2);
	buf[2] = 0;
	stat_time.month = atoi((char*)buf);
	
	memcpy(buf, B_date + 6, 2);
	buf[2] = 0;
	stat_time.day = atoi((char*)buf);
	
	memcpy(buf, B_time, 2);
	buf[2] = 0;
	stat_time.hour = atoi((char*)buf);
	
	return write_data(&stat_time, sizeof(STAT_TIME), "stat.tm");
	
}
         
uint32_t GetDays(uint8_t *szDate)
{
	int16_t  usyear, mymonth, mydate, nowyear;
	uint32_t rsl;
	
	nowyear = (int16_t)AscToLong(&szDate[0], 4);
	
	usyear = nowyear - STAR_YEAR;

//	/*�����һ�� */
	rsl = 0;
	
	rsl = (nowyear/4)-(STAR_YEAR/4);
	rsl = rsl+(STAR_YEAR/100)-(nowyear/100);
	rsl = rsl+(nowyear/400)-(STAR_YEAR/400);
	
	rsl = usyear*365l+rsl;
	mymonth = (int16_t) AscToLong(&szDate[4], 2);

	mydate = monthday[mymonth-1];
	rsl += mydate;
	if( IsLeapYear((uint32_t)nowyear) )
	{
		if( IsLeapYear((uint32_t)STAR_YEAR) )
		{
			if( mymonth>=3 )
			{
				/*����Ϊ���꣬Ӧ�ü���1��*/
				rsl++;
			}
		}
		else
		{
			if( mymonth<3 )
			{
				/*����Ϊ���꣬��ʼ�겻������Ҫ��1��*/
				rsl--;
			}
			
		}
	}
	else if( IsLeapYear((uint32_t)STAR_YEAR) )
	{
//		/*���겻Ϊ���꣬��ʼ��Ϊ��Ӧ�ü���1��*/
		rsl++;
	}
	
	mydate = (int16_t)AscToLong(&szDate[6], 2); 
	mydate--;
	rsl += mydate;
	return rsl;
}

/*
��õ�ǰ��Ե����� 
��1980��1��1��0ʱ��ʼ��

��ڣ� ������ʱ���� YYYYMMDDhhmmss
����:  ����ǰ������

*/
uint32_t GetEpoch(uint8_t *szToDateTime)
{
	uint32_t rsl;
	uint32_t mydate;
	
	rsl = GetDays(szToDateTime);
	rsl *= 24;
	
	mydate = AscToLong(&szToDateTime[8], 2);
	rsl += mydate;
	/*����ɷ���*/
	rsl *= 60l;
	
	mydate = AscToLong(&szToDateTime[10], 2);
	rsl += mydate;
	/*�������*/
	rsl *= 60l;
	
	mydate = AscToLong(&szToDateTime[12], 2);
	rsl += mydate;
	return rsl;	
}
/*
���ܣ� �ж��Ƿ�Ϊ����
��ڣ� ���
���ڣ� 1 Ϊ����
	   0 �������� 
*/
uint8_t IsLeapYear(uint32_t inyear)
{
	if( (inyear%400)==0 )
	{
		return 1;
	}
	else if( (inyear%100)==0 )
	{
		return 0;
	}
	else if( (inyear%4)==0 )
	{
		return 1;
	}
	return 0;		
}	


/************************************************************************/
/* ����old_time��new_time֮���ʱ����СʱΪ��λ                     */
/************************************************************************/
int time_diff(STAT_TIME * old_time, STAT_TIME * new_time) 
{
	uint8_t old_time_buf[15];
	uint8_t new_time_buf[15];
	int t_old, t_new;
	int diff;
	
	sprintf((char*) old_time_buf, "%04d%02d%02d%02d0000",
		old_time->year, (int)old_time->month, (int)old_time->day, (int)old_time->hour);
	sprintf((char*) new_time_buf, "%04d%02d%02d%02d0000",
		new_time->year, (int)new_time->month, (int)new_time->day, (int)new_time->hour);
	
	t_old = GetEpoch(old_time_buf);
	t_new = GetEpoch(new_time_buf);
	
	diff = t_new - t_old;
	
	if( diff<0 )
		diff = 0 - diff;
	
	diff /= 3600;
	
	return diff;
}



uint8_t AmountIsZero(uint8_t *sBcdAmt, uint8_t ucAmtLen)
{
	uint8_t	i;

	for(i=0; i<ucAmtLen; i++)
	{
		if( sBcdAmt[i]!=0 )
		{
			return FALSE;
		}
	}

	return TRUE;
}

/************************************************************************
 * ˢ���¼���������
 ************************************************************************/
uint8_t SwipeCardProc(uint8_t bCheckICC)
{
	uint8_t	ucRet;

	// ���ŵ�����
	PosCom.ucSwipedFlag = NO_SWIPE_INSERT;
	ucRet = GetTrackData(PosCom.szTrack2, PosCom.szTrack3, PosCom.stTrans.szCardNo);
	if( ucRet!=OK )
	{
		return ucRet;
	}

	// �������˻���ֵ���ж�IC��
	if (PosCom.stTrans.iTransNo == POS_MAGTOPUP_ACCT)
	{
		if (!PosCom.stTrans.ucEcTopOldSwipeFlag)
		{
			PosCom.ucFallBack   = FALSE;
			PosCom.ucSwipedFlag = CARD_SWIPED;
			return OK;
		}
	}

	// ���ײ�Ҫ���жϿ�Ƭ���ͻ���Ϊ��EMV�ն�,ֱ�ӷ���
	if( !bCheckICC || stPosParam.ucEmvSupport!=PARAM_OPEN )
	{
		PosCom.ucSwipedFlag = CARD_SWIPED;
		return OK;
	}

	// EMV�ն�,�������
	if( PosCom.ucFallBack==TRUE && IsEmvCard(PosCom.szTrack2) )
	{	// fallback������IC��,�򷵻سɹ�
		PosCom.ucSwipedFlag = FALLBACK_SWIPED;
		return OK;
	}

	// ��ͨ�ſ�ˢ������FALLBACK���EMV��ˢ��
	if( !IsEmvCard(PosCom.szTrack2) )
	{
		PosCom.ucFallBack   = FALSE;
		PosCom.ucSwipedFlag = CARD_SWIPED;
		return OK;
	}

	// оƬ��,�ж��Ƿ����ò���Աѡ������
/*	if( stPosParam.ucSelectCardType==PARAM_OPEN )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "�Ƿ�ΪIC��?", "  IS THIS CARD  ");
		DispMulLanguageString(0, 4, DISP_CFONT, "0.��   ", " IC CARD?");
		DispMulLanguageString(0, 6, DISP_CFONT, "1.��   ", "1.YES   0.NO");

		kbFlush();
		while( 1 )
		{
			ucKey = kbGetKeyMs((stPosParam.ucOprtLimitTime*1000));
			if( ucKey==KEY_CANCEL )
			{
				return E_TRANS_CANCEL;
			}
			if( ucKey==KEY1 )
			{
				break;	// ��ʾΪоƬ��,�ò���Ա���²忨����
			}
			if( ucKey==KEY0 )
			{
				PosCom.ucSwipedFlag = CARD_SWIPED;	// �Ǳ�׼�ſ�ˢ��()
				return OK;
			}
		}
	}

	// �Զ�ѡ������
	lcdClrLine(2, 7);
	lcdFlip();	
	if( stTemp.iTransNo==POS_AUTH_CM )	// ��Ȩ���(����)
	{
		DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, " оƬ��,����Ԥ��", "IC CARD");
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " Ȩ���֪ͨ     ", "PRE CMP OFFLINE");
	}
	else
	{
		DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "  �ÿ�ΪоƬ��  ", "IC CARD");
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  ��忨����    ", "PLEASE INSERT");
	}
	kbGetKeyMs(2000); */

	return E_NEED_INSERT;
}


/************************************************************************
 * ICC�忨�¼���������
 ************************************************************************/
extern int(*__ICCARD_EXCHANGE_APDU)(int fd, uint32_t sendlen, const void *senddata, uint32_t *recvlen, void *recvdat);
uint8_t InsertCardProc(uint8_t bShowDefTranName)
{
	int		iRet, iErrNo;
	uint8_t Atr[500];
	int     len;

	// ����Ѿ�FALLBACK,����IC���忨����
	if( PosCom.ucFallBack==TRUE )
	{
		return E_NEED_FALLBACK;
	}
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
	kbFlush();

	// ����EMV����
	emv_get_mck_configure(&tConfig);
	tEMVCoreInit.transaction_type = EMV_GOODS;

	if( stTemp.iTransNo==POS_QUE || stTemp.iTransNo==POS_PREAUTH || stTemp.iTransNo == POS_SALE 
		|| stTemp.iTransNo == EC_TOPUP_CASH || stTemp.iTransNo == EC_TOPUP_SPEC 
		|| stTemp.iTransNo == EC_TOPUP_NORAML)  // �����ֽ�Ȧ��һ��Ҫ����
	{
		tConfig.support_forced_online_capability = 1;
	}
	else
	{
		tConfig.support_forced_online_capability = 0;
	}

//	Hash(&tConfig.terminal_type, sizeof(tConfig)-20, tConfig.checksum);
	SHA1((uint8_t*)&tConfig, sizeof(struct terminal_mck_configure)-20, tConfig.checksum);

	emv_set_mck_configure(&tConfig);
	switch(stTemp.iTransNo)
	{
	case EC_TOPUP_CASH:
		tEMVCoreInit.transaction_type = 0x63;
		break;
	case EC_TOPUP_SPEC:
		tEMVCoreInit.transaction_type = 0x60;
		break;
	case EC_TOPUP_NORAML:
		tEMVCoreInit.transaction_type = 0x62;
		break;
	default:
		tEMVCoreInit.transaction_type = EMV_GOODS;
		break;		
	}
	emv_set_core_init_parameter(&tEMVCoreInit);	
    gl_FirstRunSelApp = 0x01;
	
//20080804 ��Ҫ�޸�
	ModifyTermCapForApp();	// Ӧ����GPO֮ǰ�޸�,����ĿǰEMV Core��֧��!!!
	iccSetPara(ICC_USERCARD,ICC_VOL_5V,ICC_ISO);
	iRet = iccPowerUp(ICC_USERCARD, &len, Atr);
	if( iRet )
	{
		return E_NEED_FALLBACK;		
	}

	if( stTemp.iTransNo==POS_SALE || stTemp.iTransNo==POS_QUE)
	{
		emv_pboc_ec_enable(0);	// �����ֽ��ܹر�
	}

	icc_fd = ICC_USERCARD;
	__ICCARD_EXCHANGE_APDU = New8110_icc_exchange_apdu;
	// Ӧ��ѡ��
	iRet = emv_application_select(icc_fd, stPosParam.lNowTraceNo);
	iErrNo = iRet?errno:0;
	DispTransName();
	// ADVT case 26 
	if( iErrNo==EMV_ERRNO_DATA || iErrNo==EMV_ERRNO_NO_APP ||
		iErrNo==EMV_ERRNO_SW )
	{
		PosCom.ucFallBack = TRUE;
		if (iErrNo==EMV_ERRNO_NO_APP)
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "    �޴�AID    ");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "  ��ˢ������    ");
			WaitRemoveICC();
		}
		else
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "    �忨����    ");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "  ��ˢ������    ");
			WaitRemoveICC();
		}

		return E_NEED_FALLBACK;
	}
	if( iErrNo==EMV_ERRNO_CANCEL )
	{
		return E_TRANS_CANCEL;
	}
	if( iErrNo==EMV_ERRNO_CARD_BLOCKED )
	{
		return E_CARD_BLOCKER;
	}
	if( iErrNo==EMV_ERRNO_APP_BLOCKED )
	{
		return E_APP_BLOCKED;
	}
	if( iErrNo!=EMV_RET_SUCCESS )
	{
		return E_TRANS_FAIL;
	}

	// ��ȡӦ������
	DispICCProcessing();
	sysDelayMs(100);
	
	iRet = emv_read_app_data(icc_fd);
	iErrNo = iRet?errno:0;
	if( iErrNo==EMV_ERRNO_CANCEL )
	{
		return E_TRANS_CANCEL;
	}
	if( iErrNo!=EMV_RET_SUCCESS )
	{
		return E_TRANS_FAIL;
	}
	SetEmvTranType();	// set emv transaction type, tag 9C

//20080717 TIP
	emv_set_data_element("\x9F\x53", 2, 1, "\x52");

	// ��Ƭ������֤(ֻ��Sale/Balance/Pre-Auth��Ҫ����EMV����)
	if( stTemp.iTransNo==POS_SALE || stTemp.iTransNo==POS_QUE ||
		stTemp.iTransNo==POS_PREAUTH || 
		(stTemp.iTransNo>=EC_NORMAL_SALE && stTemp.iTransNo <= EC_TOPUP_NORAML) )
	{
		// ��ָ���˻�Ȧ���һ��Ҳ���Բ忨��ֻ��ȡ����,��������֤����
		if (!(stTemp.iTransNo == EC_TOPUP_NORAML && PosCom.ucEdOrEpFlag)) 
		{
			iRet = emv_offline_data_authentication(icc_fd);
			iErrNo = iRet?errno:0;
			if( iErrNo!=EMV_RET_SUCCESS )
			{
				return E_TRANS_FAIL;
			}
		}
	}

	// Read Track 2 and/or Pan
	iRet = GetEmvTrackData();	
	if( iRet<0 )
	{
		return E_TRANS_FAIL;
	}

	if (ChkOnlyEcCard(PosCom.stTrans.szAID) && 
		(stTemp.iTransNo==POS_PREAUTH || stTemp.iTransNo==POS_QUE))
	{
		lcdClrLine(2,7);
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"�������ֽ�");
		lcdDisplay(0,5,DISP_CFONT|DISP_MEDIACY,"��֧�ִ˽���");
		PubBeepErr();
		PubWaitKey(5);
		return E_TRANS_FAIL;
	}

	if (ChkOnlyEcCard(PosCom.stTrans.szAID) && stTemp.iTransNo == EC_TOPUP_SPEC)
	{
		lcdClrLine(2,7);
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"�������ֽ�");
		lcdDisplay(0,5,DISP_CFONT|DISP_MEDIACY,"��֧�ִ˽���");
		PubBeepErr();
		PubWaitKey(5);
		return E_TRANS_FAIL;
	}

	PosCom.ucSwipedFlag = CARD_INSERTED;
	
	return DispCardNo(PosCom.stTrans.szCardNo);
}

int GetEmvTrackData(void)
{
	int	iRet, iErrNo;
	uint8_t	sTemp[30], szCardNo[20+1], iLength;
	int	i, bReadTrack2, bReadPan;

	// Read Track 2 Equivalent Data
	bReadTrack2 = FALSE;
	memset(sTemp, 0, sizeof(sTemp));
	
	iRet = emv_get_data_element("\x57", 1, &iLength, sTemp);
	iErrNo = iRet?errno:0;
	if( iErrNo==EMV_RET_SUCCESS )
	{
		bReadTrack2 = TRUE;
		BcdToAsc0(PosCom.szTrack2, sTemp, iLength*2);
		RemoveTailChars(PosCom.szTrack2, 'F');	// erase padded 'F' chars
		for(i=0; PosCom.szTrack2[i]!='\0'; i++)		// convert 'D' to '='
		{
			if( PosCom.szTrack2[i]=='D' )
			{
				PosCom.szTrack2[i] = '=';
				break;
			}
		}
	}

	// read PAN
	bReadPan = FALSE;
	memset(sTemp, 0, sizeof(sTemp));
	iRet = emv_get_data_element("\x5A", 1, &iLength, sTemp);
	iErrNo = iRet?errno:0;
	if( iErrNo==EMV_RET_SUCCESS )
	{
		BcdToAsc0(szCardNo, sTemp, iLength*2);
		RemoveTailChars(szCardNo, 'F');		// erase padded 'F' chars
		if( bReadTrack2 && !MatchTrack2AndPan(PosCom.szTrack2, szCardNo) )
		{	// ���Track2 & PAN ͬʱ���ڣ������ƥ��
			return -1;
		}
		sprintf((char *)PosCom.stTrans.szCardNo, "%.19s", szCardNo);
		bReadPan = TRUE;
	}
	else if( !bReadTrack2 )	// ���Track 2 �� PAN ��û��,����ʧ��
	{
		return -2;
	}
	if( !bReadPan )	// û�ж�ȡPAN��������track 2
	{
		iRet = GetCardNoFromTrack(PosCom.stTrans.szCardNo, PosCom.szTrack2, (uint8_t *)"");
		if( iRet!=OK )
		{
			return -3;
		}
	}

	// read PAN sequence number
	PosCom.stTrans.bPanSeqNoOk = FALSE;
	iRet = emv_get_data_element("\x5F\x34", 2, &iLength, &PosCom.stTrans.ucPanSeqNo);
	iErrNo = iRet?errno:0;
	if( iErrNo==EMV_RET_SUCCESS )
	{
		PosCom.stTrans.bPanSeqNoOk = TRUE;
	}

	// read Application Expiration Date
	if( bReadPan )
	{
		memset(sTemp, 0, sizeof(sTemp));
		iRet = emv_get_data_element("\x5F\x24", 2, &iLength, sTemp);
		iErrNo = iRet?errno:0;
		if( iErrNo==EMV_RET_SUCCESS )
		{
			BcdToAsc0(PosCom.stTrans.szExpDate, sTemp, 4);
		}
	}

	// read other data for print slip
	emv_get_data_element("\x50", 1, &iLength, PosCom.stTrans.szAppLable);	// application label
	iRet = emv_get_data_element("\x4F", 1, &iLength, sTemp);	// AID
	iErrNo = iRet?errno:0;
	if( iErrNo==EMV_RET_SUCCESS )
	{
		BcdToAsc0(PosCom.stTrans.szAID, sTemp, iLength*2);
		RemoveTailChars(PosCom.stTrans.szAID, 'F');
	}

	// ���÷�����֯����(��EMV�������̽���)
	if( stTemp.iTransNo!=POS_SALE && stTemp.iTransNo!=POS_QUE &&
		stTemp.iTransNo!=POS_PREAUTH && 
		!(stTemp.iTransNo>=EC_NORMAL_SALE && stTemp.iTransNo <= EC_TOPUP_NORAML))
	{
		iRet = SetIssuerName();
		if( iRet!=OK )
		{
			return -4;
		}
	}

	return OK;
}

void RemoveTailChars(uint8_t *pszString, uint8_t ucRemove)
{
	int	i, iLen;

	iLen = strlen((char *)pszString);
	for(i=iLen-1; i>0; i--)
	{
		if( pszString[i]!=ucRemove )
		{
			break;
		}
		pszString[i] = 0;
	}
}

uint8_t MatchTrack2AndPan(uint8_t *pszTrack2, uint8_t *pszPan)
{
	int	i;
	uint8_t	szTemp[19+1];

	for(i=0; i<19 && pszTrack2[i]!='\0'; i++)
	{
		if( pszTrack2[i]=='=' )
		{
			break;
		}
		szTemp[i] = pszTrack2[i];
	}
	szTemp[i] = 0;

	if( strcmp((char *)szTemp, (char *)pszPan)==0 )
	{
		return TRUE;
	}

	return FALSE;
}

void ModifyTermCapForApp(void)
{
	int	iRet, iCnt, iErrNo;
	uint8_t	sAid[16], iLength;

	emv_get_mck_configure(&tConfig);
	memset(sAid, 0, sizeof(sAid));
	iRet = emv_get_data_element("\x9F\x06", 2,  &iLength, sAid);	// should use this 
	iErrNo = iRet?errno:0;
	if( iErrNo!=EMV_RET_SUCCESS )
	{
		memcpy(tConfig.terminal_capabilities, "\xE0\xF1\xC8", 3);	// use default value
		emv_set_data_element("\x9F\x33", 2, 3, tConfig.terminal_capabilities);	// core will not modify this tag 
//		Hash(&tConfig.terminal_type, sizeof(tConfig)-20, tConfig.checksum);
		SHA1((uint8_t*)&tConfig, sizeof(struct terminal_mck_configure)-20, tConfig.checksum);

		emv_set_mck_configure(&tConfig);
		return;
	}

	memcpy(tConfig.terminal_capabilities, "\xE0\xF1\xC8", 3);	// set default value
	for(iCnt=0; iCnt<glTermAidNum; iCnt++)
	{
		if( glTermAidList[iCnt].ucAidLen==(uint8_t)iLength &&
			memcmp(glTermAidList[iCnt].sAID, sAid, iLength)==0 )
		{
			break;
		}
	}
	if( iCnt<glTermAidNum && glTermAidList[iCnt].bOnlinePin )	// �ҵ��˶�Ӧ��AID,�޸��ն�����!
	{
		tConfig.terminal_capabilities[1] |= 0x40;
	}
	else
	{
		tConfig.terminal_capabilities[1] &= ~0x40;
	}
//	Hash(&tConfig.terminal_capabilities, sizeof(tConfig)-20, tConfig.checksum);
	SHA1((uint8_t*)&tConfig, sizeof(struct terminal_mck_configure)-20, tConfig.checksum);

	emv_set_mck_configure(&tConfig);
	emv_set_data_element("\x9F\x33", 2, 3, tConfig.terminal_capabilities);	// core will not modify this tag 
//#endif
}

void DispTransName(void)
{
	lcdCls();
	lcdFlip();	
	switch( stTemp.iTransNo )
	{
	case M1_ENRICH:
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    ��    ��    ", "      SALE      ");
		break;
	case SEND_TAC :
	    DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    TAC ����    ", "      SALE      ");
	break;	
	case POS_SALE:
	
	case ICC_OFFSALE:
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    ��    ��    ", "      SALE      ");
		break;
	
	case POS_PREAUTH:
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "     Ԥ��Ȩ     ", "    PREAUTH     ");
		break;
	
	case POS_LOGON:
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    ǩ    ��    ", "     LOGON      ");
	    break;
		
	case POS_LOGOFF:
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    �ն�ǩ��    ", "     LOGOUT     ");
	    break;

	case POS_SETT:
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    ��    ��    ", "     SETTLE     ");
		break;
		
	case POS_BATCH_UP:
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "     ������     ", "     UPLOAD     ");
		break;

	case POS_SALE_VOID:
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    ���ѳ���    ", "      VOID      ");
	    break;
	
	case POS_AUTH_VOID:
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, " Ԥ��Ȩ��ɳ��� ", "  PRE CMP VOID  ");
	    break;

	case POS_PREAUTH_VOID:
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "   Ԥ��Ȩ����   ", "  PREAUTH VOID  ");
		break;

	case POS_REFUND:
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "     ��    ��   ", "     REFUND     ");
		break;

	case POS_AUTH_CM:
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "Ԥ��Ȩ���(����)", " PRE CMP(ONLINE)");
		break;
		
	case POS_OFF_CONFIRM:
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "Ԥ��Ȩ���(֪ͨ)", "PRE CMP(OFFLINE)");
		break;
		
	case POS_QUE:
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "     �����     ", "     BALANCE    ");
		break;

	case RESPONSE_TEST:
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    �������    ", "    RSP TEST    ");
		break;

	case OFF_SALE:
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    ���߽���    ", " SETTLE(OFFLINE)");
		break;
		
	case OFF_ADJUST:
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    �������    ", "    ADJUST      ");
		break;

	case DOWNLOAD_EMV_CAPK:
        DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    ��Կ����    ", "   DOWN PUBKEY  ");
        break;
    case DOWNLOAD_EMV_PARAM:
        DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "   EMV��������  ", " DOWN EMV PARAM ");
        break;
		
	default:
	    break;
	}
}

void SetEmvTranType()
{
	switch( stTemp.iTransNo )
	{
	case POS_SALE:
	case EC_NORMAL_SALE:
	case EC_QUICK_SALE:
    case QPBOC_ONLINE_SALE:
		emv_set_data_element("\x9C", 1, 1, "\x00");
		break;
		
	case POS_PREAUTH:
		emv_set_data_element("\x9C", 1, 1, "\x03");
		break;
		
	case POS_QUE:
		emv_set_data_element("\x9C", 1, 1, "\x31");
		break;

	case EC_TOPUP_CASH:
		emv_set_data_element("\x9C", 1, 1, "\x63");
		break;
		
	case EC_TOPUP_NORAML:
		emv_set_data_element("\x9C", 1, 1, "\x62");
		break;
		
	case EC_TOPUP_SPEC:
		emv_set_data_element("\x9C", 1, 1, "\x60");
		break;		
		
	default:
		break;
	}
}


//20080715 for Pboc test
void ViewEmvResult(void)
{
	uint8_t	sTVR[5], sTSI[2], iLength;

	memset(sTVR, 0, sizeof(sTVR));
	emv_get_data_element("\x95", 1, &iLength, sTVR);
	memset(sTSI, 0, sizeof(sTSI));
	emv_get_data_element("\x9B", 1, &iLength, sTSI);

	lcdCls();
	lcdFlip();	
	lcdDisplay(0, 0, DISP_ASCII, "TVR: %02X %02X %02X %02X %02X",
			sTVR[0], sTVR[1], sTVR[2], sTVR[3], sTVR[4]);
	lcdDisplay(0, 1, DISP_ASCII, "TSI: %02X %02X", sTSI[0], sTSI[1]);
	kbGetKeyMs(60*1000);
}

#ifdef _EMV_TEST_VERSION

void PrnCommLog(void)
{
	uint16_t	i;

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  ��ӡͨѶ����  ", "  PRN PACKAGE   ");
	DispMulLanguageString(0, 3, DISP_CFONT, "   ��  ��  ��   ", "    WAITING     ");
	if( usSendPacketLen>MAX_PACKET_LEN || usRecvPacketLen>MAX_PACKET_LEN )
	{
		return;
	}

	prnInit();
	prnPrintf("\nSend Pack:\n");
	for(i=0; i<usSendPacketLen; i++)
	{
		prnPrintf("%02X ", sSendPacketBuff[i]);
	}
	prnPrintf("\n\nRecv Pack:\n");

	for(i=0; i<usRecvPacketLen; i++)
	{
		prnPrintf("%02X ", sRecvPacketBuff[i]);
	}
	prnPrintf("\n\n");
	prnStart();
}




void SndCommLog(void)
{
	uint8_t	ucKey, ucPortNo;

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  ����ͨѶ����  ", "  SEND PACKAGE  ");
	if( usSendPacketLen>MAX_PACKET_LEN || usRecvPacketLen>MAX_PACKET_LEN )
	{
		return;
	}

	while( 1 )
	{
		lcdDisplay(0, 3, DISP_CFONT, "   1.COM1");
		ucKey = kbGetKeyMs(30*1000);
		if( ucKey==KEY_CANCEL )
		{
			return;
		}
		else if( ucKey==KEY1 || ucKey==KEY_ENTER )
		{
			ucPortNo = PORT_COM1;
			break;
		}
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  ���Ժ�", "  PLS WAITING  ");
	portOpen(ucPortNo, "9600,8,n,1");
	vNacSend(ucPortNo, sSendPacketBuff, usSendPacketLen);
	vNacSend(ucPortNo, sRecvPacketBuff, usRecvPacketLen);
	portClose(ucPortNo);
}
#endif


uint16_t uiPortRecvBuf(uint8_t ucPort, uint8_t *psBuff, uint16_t nLength, uint32_t nTimeOut)
{
	uint8_t   ucRet;
#define TIMER_TEMPORARY	4
	
//20080723 ��ʱʱ�䲻��Ҫ*100
	sysTimerSet(TIMER_TEMPORARY, (uint16_t)(nTimeOut));
	while( nLength>0 )
	{
		if( sysTimerCheck(TIMER_TEMPORARY)==0 )
		{
			return 0xFF;
		}
		ucRet = portRecv(ucPort, psBuff, 4000);
		if( ucRet==PORT_RET_OK )
		{
			psBuff++;
			nLength--;
		}
		else if( ucRet!=0xFF )
		{
			return 1;
		}
	}   // end of while( nLength>0
	
	return 0;
}

/****************************************************************************
Function:       ������ת��ΪBCD��ʽ�Ĵ����,1234 --> 0x12 0x34
Param In:
ulSource    ��ת��������
uiTCnt      Ŀ�껺�����Ĵ�С(�ֽ���)
Param Out:
psTarget    Ŀ�껺����
Return:         none
****************************************************************************/
void vLong2Bcd(uint32_t ulSource, uint16_t uiTCnt, uint8_t *psTarget)
{
	char    szFmt[30], szBuf[30];
	
	sprintf(szFmt, "%%0%dlu", uiTCnt*2);
	sprintf(szBuf, szFmt, ulSource);
	if( psTarget!=NULL )
	{
		vTwo2One((uint8_t *)szBuf, uiTCnt*2, psTarget);
	}
}

void vOne2Two(uint8_t *psIn, uint16_t nLength, uint8_t *psOut)
{
	static const uint8_t ucHexToChar[16] = {"0123456789ABCDEF"};
	uint16_t   nCnt;
	
	for(nCnt=0; nCnt<nLength; nCnt++)
	{
		psOut[2*nCnt]   = ucHexToChar[(psIn[nCnt]>>4)];
		psOut[2*nCnt+1] = ucHexToChar[(psIn[nCnt]&0x0F)];
	}
}

/****************************************************************************
Function:      ͬvOneTwo()����,����Ŀ�괮����һ '\0'
Param In:
psIn        Դ��
nLength     Դ������
Param Out:
psOut       Ŀ�괮
Return Code:   none
****************************************************************************/
void vOne2Two0(uint8_t *psIn, uint16_t nLength, uint8_t *psOut)
{
	vOne2Two(psIn, nLength, psOut);
	psOut[2*nLength] = 0;
}

/****************************************************************************
Function:      ���ɶ���16���ƴ��ϲ�����һ�볤�ȵĶ����ƴ�,"12AB"-->0x12AB
Param In:
psIn        Դ��
nLength     Դ������
Param Out:
psOut       Ŀ�괮
Return Code:   none
****************************************************************************/
void vTwo2One(uint8_t *psIn, uint16_t nLength, uint8_t *psOut)
{
	uint8_t   tmp;
	uint16_t  i;
	
	for(i=0; i<nLength; i+=2)
	{
		tmp = psIn[i];
		if( tmp>'9' )
		{
			tmp = toupper(tmp) - 'A' + 0x0A;
		}
		else
		{
			tmp &= 0x0F;
		}
		psOut[i/2] = (tmp<<4);
		
		tmp = psIn[i+1];
		if( tmp>'9' )
		{
			tmp = toupper(tmp) - 'A' + 0x0A;
		}
		else
		{
			tmp &= 0x0F;
		}
		psOut[i/2] |= tmp;
	}
}

/****************************************************************************
Function:       ת��BCD��Ϊ����
Param In:
psSource    ��ת����BCD���ݸ�ʽ�Ĵ�
uiSCnt      Դ���ĳ���
Param Out:
*pulTarget  ת��������
Return:         none
****************************************************************************/
void vBcd2Long(uint8_t *psSource, uint16_t uiSCnt, uint32_t *pulTarget)
{
	uint8_t   szBuf[30];
	
	vOne2Two0(psSource, uiSCnt, szBuf);
	if( pulTarget!=NULL )
	{
		*pulTarget = atol((char *)szBuf);
	}
}

/****************************************************************************
Function:      Send data from com port.
Param In:
ucPort      Port number to read data. valid value: COM1, COM2.
Port must be opened before send data.
psBuff      Data to be send.
nLength     Bytes of data.
Param Out:
none
Return Code:
none
Protocol:      STX+Len1+Len2+strings+ETX+CKS, CKS = Len1 -- ETX (^)
****************************************************************************/
void vNacSend(uint8_t ucPort, uint8_t *psBuff, uint16_t nLength)
{
	uint8_t   ucLRC, sBuff[2];
	uint16_t  i;
	int   iRet;
	uint8_t   szBuf[1024];
	
	// prepare data
	vLong2Bcd((uint32_t)nLength, 2, sBuff);
	ucLRC = sBuff[0] ^ sBuff[1] ^ ETX;
	for(i=0; i<nLength; i++)
	{
		ucLRC ^= psBuff[i];
	}
	
	portClose(PORT_COM1);
	portOpen(PORT_COM1, (char *)"9600, 8, n, 1");

	memset(szBuf, 0, sizeof(szBuf));
	szBuf[0] = STX;
	szBuf[1] = sBuff[0];
	szBuf[2] = sBuff[1];
	memcpy(&szBuf[3], psBuff, nLength);
	szBuf[3+nLength] = ETX;
	szBuf[4+nLength] = ucLRC;
	iRet = portSends(PORT_COM1, szBuf, nLength+5);                  
	if( iRet!=PORT_RET_OK )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 2, DISP_CFONT, "Send error!");
		return;
	}
	portClose(PORT_COM1);


}

uint16_t uiNacRecv(uint8_t ucPort, uint8_t *psBuff, uint32_t *puiLen, uint32_t nTimeOut)
{
	uint8_t   ucLRC, ucSTX, sLength[2];
	uint16_t  i, uiRet;
	uint32_t  ulLen;
	
	portClose(PORT_COM1);
	portOpen(PORT_COM1, (char *)"9600, 8, n, 1");

	// get STX
	while(1)
	{
		uiRet = uiPortRecvBuf(ucPort, &ucSTX, 1, nTimeOut);
		if( uiRet!=0 )
		{
			return uiRet;
		}
		if( ucSTX==STX )
		{
			break;
		}
	}

	// get L1 & L2
	uiRet = uiPortRecvBuf(ucPort, sLength, 2, nTimeOut);
	if( uiRet!=0 )
	{
		return uiRet;
	}
	ucLRC = sLength[0] ^ sLength[1];
	vBcd2Long(sLength, 2, &ulLen);
	if( ulLen>LEN_MAX_RS232 )
	{
		return 0xFE;
	}
	*puiLen = (uint32_t)ulLen;
	ulLen   += 2;

	
	// get body & ETX & LRC
	uiRet = uiPortRecvBuf(ucPort, psBuff, (uint16_t)ulLen, nTimeOut);
	if( uiRet!=0 )
	{
		return uiRet;
	}	
	
	if( psBuff[ulLen-2]!=ETX )
	{
		return 0xFE;
	}
	
	for(i=0; i<ulLen; i++)
	{
		ucLRC ^= psBuff[i];
	}
	if( ucLRC!=0 )
	{
		return 0xFE;
	}
	
	portClose(PORT_COM1);

	return 0;
}
//#endif

// �ǽӴ�ʽ������
extern int(*__ICCARD_EXCHANGE_APDU)(int fd, uint32_t sendlen, const void *senddata, uint32_t *recvlen, void *recvdat);
uint8_t ContackCardProc(uint8_t bShowDefTranName)
{// Ŀǰֻ����QBPOC����
	int ierrorno = EMV_QPBOC_OK;
	struct emv_core_init Init;
    EMV_APP_TYPE AppType;	
	uint8_t TransType;
	uint32_t Amount, AmountOther, iReadCnt;
	int  iOldTime,iNowTime, istatus, iNeedSaveLog;
	uint8_t  szBuffer[200];
	uint8_t  uLen;
//	int      iCnt;
	struct qpboc_parameters  gstQpbocParamters;

	istatus = 0;
	PosCom.ucSwipedFlag = NO_SWIPE_INSERT;	
	if( bShowDefTranName )
	{
		lcdCls();
		lcdFlip();	
		stTemp.iTransNo = POS_SALE;
	}
	DispTransName();
	DispContackLessProcessing();
	
	TransType = 0x00;
	memcpy(gstQpbocParamters.m_TransactionProperty, "\xA6\x00", 2);
	gstQpbocParamters.m_StatusCheckSupported = 0;
	ierrorno = emv_qpboc_set_parameter(&gstQpbocParamters);
	if (EMV_QPBOC_OK != ierrorno)
	{
		lcdCls();
		lcdDisplay(0 ,3 , DISP_CFONT | DISP_MEDIACY , "���ò���ʧ��");
		kbGetKey();
		return NO_DISP;
	}
	
	// ����Ԥ�������ҵ�AID�����ò���������������.
/*	printf("transqpboc proc before \n");
    ierrorno = TransQpbocPreProc();
	printf("transqpboc proc end :%d \n",ierrorno);
	if (ierrorno != 0)
	{
		return ierrorno;
	}*/

	TransType = 0x00;
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

	Amount = 0;
	AmountOther = 0;	
	if (memcmp(PosCom.stTrans.sAmount,"\x00\x00\x00\x00\x00\x00",6) == 0)
	{
		if (0 != emv_get_amount(&Amount, NULL))
			return NO_DISP;
	}
	else
	{
		Amount = PubBcd2Long(PosCom.stTrans.sAmount,6,NULL);
	}

	contactness_poll(imif_fd);
	iOldTime = sysGetTimerCount();
	while(1)
	{
		iNowTime= sysGetTimerCount();
		if( (iNowTime-iOldTime) >= (uint32_t)(1000) )	// ��鶨ʱ��
		{
			lcdClrLine(2,7);
			lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"������");
			PubBeepErr();
			PubWaitKey(5);
			return NO_DISP;
		}

		emv_contactless_obtain_status(imif_fd, &istatus);
		if (istatus == NFC_OP_EXCHANGE_APDU)
		{
			break;
		}
		sysDelayMs(10);
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

	if (ierrorno == EMV_QPBOC_CARD_IN_BLACK_LIST)
	{
		contactness_close(imif_fd);
		imif_fd = -1;
		lcdClrLine(2,7);
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"�ÿ������������");
		PubBeepErr();
		PubWaitKey(5);
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

//	printf("read qpboc data 2:%d \n",ierrorno);
	if (ierrorno != EMV_QPBOC_OK)
	{
		contactness_close(imif_fd);
		imif_fd = -1;

		emv_qpboc_get_app_type(&AppType);
		if (EMV_MSD == AppType)
		{
			emv_set_data_element("\x9F\x39", 2, 1, "\x91");
		}
		else if (EMV_QPBOC == AppType)
		{
			emv_set_data_element("\x9F\x39", 2, 1, "\x07");
		}
		else if (EMV_PBOC == AppType)
		{
			emv_set_data_element("\x9F\x39", 2, 1, "\x98");
		}

//		iNeedSaveLog = emv_qpboc_need_save_failed_log();
//		printf("check need save log: %d \n", iNeedSaveLog);
		iNeedSaveLog = TRUE; // ȫ������
		if (iNeedSaveLog) // ��������ռ�
		{
			PosCom.stTrans.ucSwipeFlag = PosCom.ucSwipedFlag = CARD_PASSIVE;	// ����ˢ����־
			GetAndSetQpbocElement();
			SaveIccFailureTranData();
			SaveIccFailureFile();
		}

		DisplayQpbocError(ierrorno);
		return NO_DISP;		
	}

//	contactness_close(imif_fd);
//	imif_fd = -1;
	sysBeef(6, 400);
	lcdCls();
	lcdDisplay(0, 2, DISP_MEDIACY | DISP_CFONT, "�� �� ��");

	memset(szBuffer,0,sizeof(szBuffer));
	if (0 == emv_get_data_element("\x9F\x5D", 2, &uLen, szBuffer))
	{
		Amount = PubBcd2Long(szBuffer,uLen,NULL);
		lcdDisplay(0, 4, DISP_CFONT, "�������:");
		lcdDisplay(0, 6, DISP_MEDIACY|DISP_CFONT,"%d.%02d", Amount/100, Amount%100);
	}
	sysBeef(6, 200);
	PubWaitKey(1);
	contactness_poweroff(imif_fd);  // �ú�������ֱ�����ƿ�
	contactness_close(imif_fd);
	imif_fd = -1;
	
	emv_qpboc_get_app_type(&AppType);
	if (EMV_MSD == AppType)
	{
		emv_set_data_element("\x9F\x39", 2, 1, "\x91");
	}
	else if (EMV_QPBOC == AppType)
	{
		emv_set_data_element("\x9F\x39", 2, 1, "\x07");
	}
	else if (EMV_PBOC == AppType)
	{
		emv_set_data_element("\x9F\x39", 2, 1, "\x98");
	}	

	PosCom.ucSwipedFlag = CARD_PASSIVE;
	PosCom.stTrans.ucSwipeFlag = PosCom.ucSwipedFlag;	// ����ˢ����־
	PosCom.bSkipEnterAmt = TRUE;
	return 0;
}

// end file
