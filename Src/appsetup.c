
#include "posapi.h"
#include "posmacro.h"
#include "terstruc.h"
#include "glbvar.h"
#include "tranfunclist.h"
#include "wireless.h"
#include "emvapi.h"
#include "tmslib.h"
UINT8 FTPParam(void)
{

    char   tmpbuf[100];
	int    iRet;
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "FTP����        ", "CDMA SETUP");

	memset(tmpbuf,0,sizeof(tmpbuf));
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "FTP������IP:", "SETUP HOST IP");
	lcdDisplay(0, 4, DISP_CFONT, (char *)(stPosParam.szFtpServIP));
	if( SetIpAddr(0, 6, (char*)tmpbuf)==0 ) 
	{
		strcpy((char*)stPosParam.szFtpServIP, (char*)tmpbuf);
		SaveAppParam();
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "FTP�������˿�", "SETUP HOST PORT");
	lcdDisplay(0, 4, DISP_CFONT, (char *)(stPosParam.szFtpServPort));
	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 0, 6,
		(stPosParam.ucOprtLimitTime*1000), (char *)tmpbuf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return KEY_CANCEL;
	}
	if( iRet>0 )
	{
		strcpy((char*)(stPosParam.szFtpServPort), (char*)tmpbuf);
		stPosParam.szFtpServPort[iRet] = 0;
		SaveAppParam();
	}
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "FTP��¼��:", "SETUP APN:");
	strcpy((char*)tmpbuf, (char*)stPosParam.szFtpUserName);
	lcdDisplay(0, 4, DISP_CFONT, tmpbuf);
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_CHAR+KB_EN_BIGFONT+KB_EN_REVDISP, 0, 30, 
		(stPosParam.ucOprtLimitTime*1000), (char *)tmpbuf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return KEY_CANCEL;
	}
	else if( iRet>0 )
	{
		strcpy((char*)stPosParam.szFtpUserName, (char*)tmpbuf);
		SaveAppParam();
	}
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "FTP��¼����:", "SETUP APN:");
	strcpy((char*)tmpbuf, (char*)stPosParam.szFtpPwd);
	lcdDisplay(0, 4, DISP_CFONT, tmpbuf);
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_CHAR+KB_EN_BIGFONT+KB_EN_REVDISP, 0, 30, 
		(stPosParam.ucOprtLimitTime*1000), (char *)tmpbuf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return KEY_CANCEL;
	}
	else if( iRet>0 )
	{
		strcpy((char*)stPosParam.szFtpPwd, (char*)tmpbuf);
		SaveAppParam();
	}
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "FTP�ļ�·��:", "SETUP APN:");
	strcpy((char*)tmpbuf, (char*)stPosParam.szFtpPath);
	lcdDisplay(0, 4, DISP_CFONT, tmpbuf);
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_CHAR+KB_EN_BIGFONT+KB_EN_REVDISP, 0, 255, 
		(stPosParam.ucOprtLimitTime*1000), (char *)tmpbuf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return KEY_CANCEL;
	}
	else if( iRet>0 )
	{
		strcpy((char*)stPosParam.szFtpPath, (char*)tmpbuf);
		
		if (strlen(stPosParam.szFtpPath) > 0 && stPosParam.szFtpPath[strlen(stPosParam.szFtpPath) - 1] != '/' 
				&& stPosParam.szFtpPath[strlen(stPosParam.szFtpPath) - 1] != '\\')
			{		
				stPosParam.szFtpPath[strlen(stPosParam.szFtpPath)] = '/';
				stPosParam.szFtpPath[strlen(stPosParam.szFtpPath) + 1] = 0;
			}
		SaveAppParam();
	}
	
return 0;

}




// ����ϵͳ����
void SetupPosPara(void)
{
	int     iKey;
	/*const char *setupMenu[] = 
	{
		"ϵͳ����        ",
		"1 �ն˲�������  ",
		"2 ͨѶ��������  ",
		"3 ���׹�������  ",
		"4 �ն���Կ����  ",
		"5 �޸�����      ",
		"6 ��������      ",
		"7 FTP����       "
	};*/
	
	const char *setupMenu[] = 
	{
		"ϵͳ����        ",
		"1 �ն˲�������  ",
		"2 ���׹�������  ",
		"3 �޸�����      ",
		"4 ��������      ",
	};
	
	while(1) 
	{
		iKey = DispMenu(setupMenu, 5, 1, KEY1, KEY4, stPosParam.ucOprtLimitTime);
		switch(iKey) 
		{
		case KEY1:
			SetupTerminalPara();
			break;
		/*case KEY2:
			SetCurCommType();
			SetCurCommParam(TRANS_COMMTYPE);
			SetTpduOutTime();
			break;*/
		case KEY2:
			SetupTranFunc();
			break;
		/*case KEY4:
			SetMasterKeys();
			break;*/
		case KEY3:
			ChangePwdFunc();
			break;
		case KEY4:
			OtherFunc();
			break;
		/*case KEY7:
			FTPParam();
			break;*/
		case KEY_CANCEL:
			SaveCtrlParam();
			return;
		case KEY_UP:
		case KEY_ENTER:
			break;
		default:
			continue;
		}

		SaveAppParam();
		SaveCtrlParam();	
	}
}

void SetupTranFunc(void)
{
	int   iKey;
	const char *setupMenu[] = 
	{
		"���׹�������    ",
		"1 ���ν���      ",
		"2 ǩ������      ",
		"3 С�ѱ���      ",
		"4 �����ط�����  ",
		"5 ��ӡ����      ",
		"6 ����ױ���  "
	};

	
	while(1) 
	{
		iKey = DispMenu(setupMenu, 7, 1, KEY1, KEY6, stPosParam.ucOprtLimitTime);
		switch(iKey) 
		{
		case KEY1:
			SetTransMask();
			break;
		case KEY2:
			AutoLogoutSetup();
			break;
		case KEY3:
			TipperSetup();
			break;
		case KEY4:
			SetTranResendTime();	
			break;
		case KEY5:
			SetPrintTicketNum();
			break;
		case KEY6:
			SetTranMaxNumber();
			SetMaxOfflineNums();
			break;
		case KEY_CANCEL:
			SaveCtrlParam();
			return;
		case KEY_UP:
		case KEY_ENTER:
			break;
		default:
			continue;
		}

		SaveAppParam();
		SaveCtrlParam();	
	}
}

void ChangePwdFunc(void)
{
	int   iKey;
	const char *setupMenu[] = 
	{
		"�޸�����        ",
		"1 �޸Ĺ���Ա����",
		"2 �޸İ�ȫ����  "
	};

	
	while(1) 
	{
		iKey = DispMenu(setupMenu, 3, 1, KEY1, KEY2, stPosParam.ucOprtLimitTime);
		switch(iKey) 
		{
		case KEY1:
			ChangeSysPwd();
			break;
		case KEY2:
			ChangeSafePwd();
			break;
		case KEY_CANCEL:
			return;
		case KEY_UP:
		case KEY_ENTER:
			break;
		default:
			continue;
		}

		SaveAppParam();
		SaveCtrlParam();	
	}
}

void OtherFunc(void)
{
	int iKey;
	const char *setupMenu[] = 
	{
		"��������        ",
		"1 ���������ˮ  ",
		"2 ������������  ",
		"3 ���ϵ�����    ",
		"4 ������ӡ      ",
		"5 ���߲���      ",
		"6 ״̬����      "
	};

	
	while(1) 
	{
		iKey = DispMenu(setupMenu, 7, 1, KEY1, KEY6, stPosParam.ucOprtLimitTime);
		switch(iKey) 
		{
		case KEY1:
			ClearFile();
			break;
		case KEY2:
			SetTransFlow();
			break;
		case KEY3:
			SetErrPrnTicketNum();
			break;
		case KEY4:
			PrintParam(1); // ��ӡȫ������
			break;
		case KEY5:
			DeviceTest();
			break;
		case KEY6:
			SetTestStatus();
			break;
		case KEY_CANCEL:
		case KEY_ENTER:
			return;
		case KEY_UP:
			break;
		default:
			continue;
		}

		SaveAppParam();
		SaveCtrlParam();	
	}
}

void SetupTerminalPara(void) 
{
	int     iKey;
	int     iRet;
	uint8_t buf[61], buf1[61], buf2[2];
	uint8_t szTime[15];
	uint8_t szDate[9];
	uint8_t szDateTime[7];
	long    lTmpLong;
	uint8_t szpwd[10];

	kbFlush();
	/*lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ն˲�������    ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "POS�����:", "PLS INPUT MER NO");
	strcpy((char *)buf, (char *)stPosParam.sM1TellNo);
	lcdDisplay(8, 4, DISP_CFONT, (char*)buf);
	lcdGoto(8, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 4, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==4 && strcmp((char *)stPosParam.sM1TellNo, (char *)buf))
	{
	
		strcpy((char *)stPosParam.sM1TellNo, (char *)buf); 
		//strcpy((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY);	
		SaveAppParam();
	}
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ն˲�������    ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "��˾���:", "PLS INPUT MER NO");
	strcpy((char *)buf, (char *)stPosParam.szCompanyNo);
	lcdDisplay(8, 4, DISP_CFONT, (char*)buf);
	lcdGoto(8, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 4, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==4 && strcmp((char *)stPosParam.szCompanyNo, (char *)buf))
	{
	
		strcpy((char *)stPosParam.szCompanyNo, (char *)buf); 
		//strcpy((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY);	
		SaveAppParam();
	}*/
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ն˲�������    ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "���ű��:", "PLS INPUT MER NO");
	strcpy((char *)buf, (char *)stPosParam.szLSDepartmentNo);
	lcdDisplay(0, 4, DISP_CFONT, (char*)buf);
	lcdGoto(8, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 4, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==4 && strcmp((char *)stPosParam.szLSDepartmentNo, (char *)buf))
	{
	
		strcpy((char *)stPosParam.szLSDepartmentNo, (char *)buf); 
		//strcpy((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY);	
		SaveAppParam();
	}
	//-------����Ա���---
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ն˲�������	  ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "����Ա���:", "PLS INPUT MER NO");
	strcpy((char *)buf, (char *)stPosParam.szLSM1TellNo);
	lcdDisplay(0, 4, DISP_CFONT, (char*)buf);
	lcdGoto(8, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 8, 8, 
	(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==8 && strcmp((char *)stPosParam.szLSM1TellNo, (char *)buf))
	{

	strcpy((char *)stPosParam.szLSM1TellNo, (char *)buf); 
	//strcpy((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY); 
	SaveAppParam();
	}
/*/---------------------
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ն˲�������    ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "�ն˱��:", "PLS INPUT MER NO");
	strcpy((char *)buf, (char *)stPosParam.szLSPosId);
	lcdDisplay(0, 4, DISP_CFONT, (char*)buf);
	lcdGoto(8, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 12,12, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==12 && strcmp((char *)stPosParam.szLSPosId, (char *)buf))
	{
	
		strcpy((char *)stPosParam.szLSPosId, (char *)buf); 
		//strcpy((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY);	
		SaveAppParam();
	}
    //----------------------*/
   /* lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ն˲�������    ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "MAC��Կ:", "PLS INPUT MER NO");
	strcpy((char *)buf, (char *)stPosParam.szMacKey);
//	lcdDisplay(8, 4, DISP_CFONT, (char*)buf);
	lcdGoto(0, 4);*/
	/*lcdGoto(0, 4);
				memset(buf,0,sizeof(buf));
				iRet = kbGetString(0x175, ucLen, ucLen, 
					(stPosParam.ucOprtLimitTime*1000), (char *)buf);
				if( iRet!=ucLen )
					break;*/

	/*iRet = kbGetString(0x175, 16,16, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==16 )
	{
	
		strcpy((char *)stPosParam.szMacKey, (char *)buf); 
		//strcpy((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY);	
		SaveAppParam();
	}
	//----------------------
		lcdCls();
		lcdFlip();	
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ն˲�������	  ", "SETUP PARA   ");
		DispMulLanguageString(0, 2, DISP_CFONT, "��������Կ:", "PLS INPUT MER NO");
		strcpy((char *)buf, (char *)stPosParam.szMsgBodyKey);
	//	lcdDisplay(8, 4, DISP_CFONT, (char*)buf);
		lcdGoto(0, 4);
		iRet = kbGetString(0x175, 32,32, 
			(stPosParam.ucOprtLimitTime*1000), (char *)buf);
		if( iRet==32 )
		{
		
			strcpy((char *)stPosParam.szMsgBodyKey, (char *)buf); 
			//strcpy((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY); 
			SaveAppParam();
		}

	//-*/
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ն˲�������    ", " SETUP TRACE.NO "); 
	DispMulLanguageString(0, 2, DISP_CFONT, "����ֵ�޶�(Ԫ):", "INPUT TRACE.NO:");
	sprintf((char *)buf, "%d", stPosParam.ulMaxBalaceAmt);
	lcdDisplay(0, 4, DISP_CFONT, (char *)buf);
	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 0, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return ;
	}
	lTmpLong = atol((char *)buf);
	if( iRet>0 && stPosParam.ulMaxBalaceAmt!=lTmpLong )
	{
		stPosParam.ulMaxBalaceAmt = lTmpLong;
		SaveAppParam();
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ն˲�������    ", " SETUP TRACE.NO "); 
	DispMulLanguageString(0, 2, DISP_CFONT, "���ճ�ֵ�޶�(Ԫ):", "INPUT TRACE.NO:");
	sprintf((char *)buf, "%d", stPosParam.dayMaxBalaceAmt);
	lcdDisplay(0, 4, DISP_CFONT, (char *)buf);
	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 0, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return ;
	}
	lTmpLong = atol((char *)buf);
	if( iRet>0 && stPosParam.dayMaxBalaceAmt!=lTmpLong )
	{
		stPosParam.dayMaxBalaceAmt = lTmpLong;
		SaveAppParam();
	}
	//---------------------------pointofsaleNo
	
	/*kbFlush();
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ն˲�������    ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "�������̻����:", "PLS INPUT MER NO");
	strcpy((char *)buf, (char *)stPosParam.szUnitNum);
	lcdDisplay(8, 4, DISP_CFONT, (char*)buf);
	lcdGoto(8, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 0, 15, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return ;
	}
	else if( iRet==15 && strcmp((char *)stPosParam.szUnitNum, (char *)buf))
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 2, DISP_CFONT, "�����밲ȫ����");
		lcdGoto(64, 6);
		iRet = kbGetString(KB_EN_CRYPTDISP+KB_EN_BIGFONT+KB_EN_NUM+KB_EN_REVDISP, 8, 8, 
			(stPosParam.ucOprtLimitTime*1000), (char *)szpwd);
		if( iRet==8 )
		{
			if( strcmp((char *)szpwd, (char *)stPosParam.szSafepwd)!=0 )
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "��ȫ���벻��ȷ  ", "PWD ERROR!");
				ErrorBeep();
				kbGetKeyMs(1000);	

			}
			else
			{
				strcpy((char *)stPosParam.szUnitNum, (char *)buf); 
				strcpy((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY);	
				SaveAppParam();
			}
		}
	}

	
	kbFlush();
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ն˲�������    ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "�������ն˱��:", "PLS INPUT TEM NO");
	strcpy((char *)buf, (char *)stPosParam.szPosId);
	lcdDisplay(64, 4, DISP_CFONT, (char *)buf);
	lcdGoto(64, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 0, 8, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return ;
	}
	if( iRet==8 && strcmp((char *)stPosParam.szPosId, (char *)buf))
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 2, DISP_CFONT, "�����밲ȫ����");
		lcdGoto(64, 6);
		iRet = kbGetString(KB_EN_CRYPTDISP+KB_EN_BIGFONT+KB_EN_NUM+KB_EN_REVDISP, 8, 8, 
			(stPosParam.ucOprtLimitTime*1000), (char *)szpwd);
		if( iRet==8 )
		{
			if( strcmp((char *)szpwd, (char *)stPosParam.szSafepwd)!=0 )
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "��ȫ���벻��ȷ  ", "PWD ERROR!");
				ErrorBeep();
				kbGetKeyMs(1000);	

			}
			else
			{
				strcpy((char *)stPosParam.szPosId, (char *)buf);
				strcpy((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY);	
				SaveAppParam();
			}
		}
	}

	kbFlush();
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ն˲�������    ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, (char *)stPosParam.szUnitChnName, (char *)stPosParam.szUnitChnName);
	DispMulLanguageString(0, 6, DISP_CFONT, "����?(ȷ��/ȡ��)", "CHG?ENTER/CANCEL");
	do {
		iKey = kbGetKey();
	} while( iKey!=KEY_CANCEL && iKey!=KEY_ENTER );
	if( iKey==KEY_ENTER )
	{
		kbFlush();
		memset(buf, 0, sizeof(buf));
		lcdGoto(0, 2);
		iRet = kbGetHZ(0, 40, 0, (char *)buf);
		if( iRet>0 && strcmp((char *)stPosParam.szUnitChnName, (char *)buf) )
		{
			strcpy((char *)stPosParam.szUnitChnName, (char *)buf);
			SaveAppParam();
		}
	}
	*/
	/*GetPosTime(szDate, szTime);

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ն˲�������    ", "   SETUP DATE   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "�����뵱ǰ���", "YYYY");
	lcdDisplay(0, 4, DISP_CFONT, "YYYY");
	memset(buf, 0, sizeof(buf));
	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 0, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return ;
	}
	if( iRet==4 )
	{
		memcpy(szDate, buf, 4);
		szDate[8] = 0x00;
		if( CheckDate((char *)szDate)!=0 )
		{
			FailBeep();
			lcdClrLine(4, 7);
			lcdFlip();	
			DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "���ڸ�ʽ����", "DATE FORMAT ERR");
			kbGetKeyMs(2000);
			return;
		}
		
		AscToBcd(szDateTime, &szDate[2], 6);
		AscToBcd(szDateTime+3, szTime, 6);
		sysSetTime(szDateTime);
		write_stat_time();
		memset(&dial_stat, 0, sizeof(DIAL_STAT));
		write_data(&dial_stat, sizeof(DIAL_STAT), "dial.sta");
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ն˲�������    ", " SETUP TRACE.NO "); 
	DispMulLanguageString(0, 2, DISP_CFONT, "��ǰ��ˮ��:", "INPUT TRACE.NO:");
	sprintf((char *)buf, "%06d", stPosParam.lNowTraceNo);
	lcdDisplay(0, 4, DISP_CFONT, (char *)buf);
	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 0, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return ;
	}
	lTmpLong = atol((char *)buf);
	if( iRet>0 && stPosParam.lNowTraceNo!=lTmpLong )
	{
		stPosParam.lNowTraceNo = lTmpLong;
		SaveAppParam();
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ն˲�������    ", " SETUP BATCH NO ");
	DispMulLanguageString(0, 2, DISP_CFONT, "��ǰ���κ�:", "INPUT BATCH NO:");
	sprintf((char *)buf, "%06d", stPosParam.lNowBatchNum);
	lcdDisplay(0, 4, DISP_CFONT, (char *)buf);
	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 0, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return ;
	}
	lTmpLong = atol((char *)buf);
	if( iRet>0 && stPosParam.lNowBatchNum!=lTmpLong )
	{
		stPosParam.lNowBatchNum = lTmpLong;
		SaveAppParam();
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "����˻��������", " SETUP BATCH NO ");
	lcdDisplay(0, 2, DISP_CFONT, "��������˻����");
	LongToAmount(buf, stPosParam.lMaxRefundAmt);
	sprintf((char *)buf1, "%s", buf);
	lcdDisplay(0, 4, DISP_CFONT, (char *)buf1);
	lcdGoto(56, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_FLOAT+KB_EN_REVDISP, 0, 8, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return ;
	}
	if( iRet>0 && stPosParam.lMaxRefundAmt!=atol((char *)buf) )
	{
		stPosParam.lMaxRefundAmt= atol((char *)buf);
		SaveAppParam();
	}
*/
/*	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�����ֽ����޶�", " SETUP BATCH NO ");
	lcdDisplay(0, 2, DISP_CFONT, "��������ֽ���");
	LongToAmount(buf, stPosParam.iEcMaxAmount);
	sprintf((char *)buf1, "%s", buf);
	lcdDisplay(0, 4, DISP_CFONT, (char *)buf1);
	lcdGoto(56, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_FLOAT+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 0, 8, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return ;
	}
	if( iRet>0 && stPosParam.iEcMaxAmount!=atol((char *)buf) )
	{
		stPosParam.iEcMaxAmount= atol((char *)buf);
		SaveAppParam();
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ǽ��ѻ�����޶�", " SETUP BATCH NO ");
	lcdDisplay(0, 2, DISP_CFONT, "����ǽ�����޶�");
	LongToAmount(buf, stPosParam.iqPbocMinAmt);
	sprintf((char *)buf1, "%s", buf);
	lcdDisplay(0, 4, DISP_CFONT, (char *)buf1);
	lcdGoto(56, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_FLOAT+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 0, 8, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return ;
	}
	if( iRet>0 && stPosParam.iqPbocMinAmt!=atol((char *)buf) )
	{
		stPosParam.iqPbocMinAmt= atol((char *)buf);
		SaveAppParam();
	}*/

/*	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ǽ�������޶�", " SETUP BATCH NO ");
	lcdDisplay(0, 2, DISP_CFONT, "�������ǽӽ��");
	LongToAmount(buf, stPosParam.iqPbocMaxAmt);
	sprintf((char *)buf1, "%s", buf);
	lcdDisplay(0, 4, DISP_CFONT, (char *)buf1);
	lcdGoto(56, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_FLOAT+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 0, 8, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return ;
	}
	if( iRet>0 && stPosParam.iqPbocMaxAmt!=atol((char *)buf) )
	{
		stPosParam.iqPbocMaxAmt= atol((char *)buf);
		SaveAppParam();
	}*/

/*
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�����ӡ��ϸ����", " SETUP BATCH NO ");
	lcdDisplay(0, 2, DISP_CFONT, "�Ƿ���ʾ��ӡ��ϸ");
	lcdDisplay(0, 4, DISP_CFONT, "   0-��    1-�� ");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = stPosParam.ucDetailPrt;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.ucDetailPrt = PARAM_CLOSE;
		SaveAppParam();
	}
	else if( iRet==1 )
	{
		stPosParam.ucDetailPrt = PARAM_OPEN;
		SaveAppParam();
	}
	else if( iRet<0 )
	{
		return ;
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "���д�������    ", " MER BANK SETUP ");
	DispMulLanguageString(0, 2, DISP_CFONT, "���������д���:", "INPUT NUM1:");
	strcpy((char *)buf, (char *)stPosParam.szMerBankId);
	lcdDisplay(0, 4, DISP_CFONT, (char *)buf);
	lcdGoto(96, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CHAR+KB_EN_REVDISP, 0, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return ;
	}
	if( iRet>0 && strcmp((char *)stPosParam.szMerBankId, (char *)buf))
	{
		strcpy((char *)stPosParam.szMerBankId, (char *)buf);
		SaveAppParam();
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "���ص�����      ", "LOCAL ID SETUP ");
	DispMulLanguageString(0, 2, DISP_CFONT, "�����뱾�ص�����", "INPUT NUM1:");
	strcpy((char *)buf, (char *)stPosParam.szLocalId);
	lcdDisplay(0, 4, DISP_CFONT, (char *)buf);
	lcdGoto(96, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CHAR+KB_EN_REVDISP, 0, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return ;
	}
	if( iRet>0 && strcmp((char *)stPosParam.szLocalId, (char *)buf))
	{
		strcpy((char *)stPosParam.szLocalId, (char *)buf);
		SaveAppParam();
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "Ӣ������        ", " SETUP BATCH NO ");
	lcdDisplay(0, 2, DISP_CFONT, "ǩ�����Ƿ��Ӣ��");
	lcdDisplay(0, 4, DISP_CFONT, "   0-��    1-�� ");
    memset(buf2, 0, sizeof(buf2));
	buf2[0] = stPosParam.ucPrnEng;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.ucPrnEng = PARAM_CLOSE;
		SaveAppParam();
	}
	else if( iRet==1 )
	{
		stPosParam.ucPrnEng = PARAM_OPEN;
		SaveAppParam();
	}
	else if( iRet<0 )
	{
		return ;
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "ǩ����̧ͷ����  ", " SETUP PRN TITLE ");
	lcdDisplay(0, 2, DISP_CFONT, "̧ͷ����ѡ��");
	lcdDisplay(0, 4, DISP_CFONT, "0-����    1-Ĭ��");
    memset(buf2, 0, sizeof(buf2));
	buf2[0] = stPosParam.ucPrnTitleFlag;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.ucPrnTitleFlag = PARAM_CLOSE;
		SaveAppParam();
	}
	else if( iRet==1 )
	{
		stPosParam.ucPrnTitleFlag = PARAM_OPEN;
		SaveAppParam();
	}
	else if( iRet<0 )
	{
		return ;
	}

	if (stPosParam.ucPrnTitleFlag == PARAM_CLOSE)
	{
		kbFlush();
		lcdCls();
		lcdFlip();	
		DispMulLanguageString(0, 0, DISP_CFONT, (char *)stPosParam.szPrnTitle, (char *)stPosParam.szPrnTitle);
		DispMulLanguageString(0, 4, DISP_CFONT, "0--ȡ��  1--ȷ��", "0-CANCEL 1-ENTER");
		do {
			iKey = kbGetKey();
		} while( iKey!=KEY_CANCEL && iKey!=KEY_ENTER && iKey!=KEY0 && iKey!=KEY1);
		if( iKey==KEY_CANCEL || iKey==KEY0 )
		{
			kbFlush();
			memset(buf, 0, sizeof(buf));
			lcdGoto(0, 2);
			iRet = kbGetHZ(0, 20, 0, (char *)buf);
			if( iRet>0 && strcmp((char *)stPosParam.szPrnTitle, (char *)buf) )
			{
				strcpy((char *)stPosParam.szPrnTitle, (char *)buf);
				SaveAppParam();
			}
		}
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "Ĭ�Ͻ�������    ", " SETUP BATCH NO ");
	lcdDisplay(0, 2, DISP_CFONT, "1-����");
	lcdDisplay(0, 4, DISP_CFONT, "0-Ԥ��Ȩ");
    memset(buf2, 0, sizeof(buf2));
	buf2[0] = stTransCtrl.ucDefaltTxn+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stTransCtrl.ucDefaltTxn = 0;
		SaveAppParam();
	}
	else if( iRet==1 )
	{
		stTransCtrl.ucDefaltTxn = 1;
		SaveAppParam();
	}
	else if( iRet<0 )
	{
		return ;
	}
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "��Կ�㷨����    ", " SETUP BATCH NO ");
	lcdDisplay(0, 2, DISP_CFONT, "1-������");
	lcdDisplay(0, 4, DISP_CFONT, "0-˫����");
    memset(buf2, 0, sizeof(buf2));
	buf2[0] = stPosParam.ucKeyMode;

	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.ucKeyMode = KEY_MODE_3DES;
		SaveAppParam();
	}
	else if( iRet==1 )
	{
		stPosParam.ucKeyMode = KEY_MODE_DES;
		SaveAppParam();
	}
	else if( iRet<0 )
	{
		return ;
	}
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "���ѳ����Ƿ�ˢ��", " SETUP BATCH NO ");
	lcdDisplay(0, 4, DISP_CFONT, "   0-��    1-�� ");
    memset(buf2, 0, sizeof(buf2));
	buf2[0] = stPosParam.ucVoidSwipe;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.ucVoidSwipe = PARAM_CLOSE;
		SaveAppParam();
	}
	else if( iRet==1 )
	{
		stPosParam.ucVoidSwipe = PARAM_OPEN;
		SaveAppParam();
	}
	else if( iRet<0 )
	{
		return ;
	}

	lcdCls();
	lcdFlip();	
	lcdDisplay(0, 0, DISP_CFONT|DISP_REVERSE, "Ԥ��Ȩ��ɳ�����");
	lcdDisplay(0, 2, DISP_CFONT|DISP_REVERSE, "��ˢ��");
	lcdDisplay(0, 4, DISP_CFONT, "   0-��    1-�� ");
    memset(buf2, 0, sizeof(buf2));
	buf2[0] = stPosParam.ucAuthVoidSwipe;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.ucAuthVoidSwipe = PARAM_CLOSE;
		SaveAppParam();
	}
	else if( iRet==1 )
	{
		stPosParam.ucAuthVoidSwipe = PARAM_OPEN;
		SaveAppParam();
	}
	else if( iRet<0 )
	{
		return ;
	}

	lcdCls();
	lcdFlip();	
	lcdDisplay(0, 0, DISP_CFONT|DISP_REVERSE, "���ѳ����Ƿ�����");
	lcdDisplay(0, 2, DISP_CFONT|DISP_REVERSE, "��");
	lcdDisplay(0, 4, DISP_CFONT, "   0-��    1-�� ");
    memset(buf2, 0, sizeof(buf2));
	buf2[0] = stPosParam.ucVoidPin;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.ucVoidPin = PARAM_CLOSE;
		SaveAppParam();
	}
	else if( iRet==1 )
	{
		stPosParam.ucVoidPin = PARAM_OPEN;
		SaveAppParam();
	}
	else if( iRet<0 )
	{
		return ;
	}

	lcdCls();
	lcdFlip();	
	lcdDisplay(0, 0, DISP_CFONT|DISP_REVERSE, "Ԥ��Ȩ�����Ƿ���");
	lcdDisplay(0, 2, DISP_CFONT|DISP_REVERSE, "����");
	lcdDisplay(0, 4, DISP_CFONT, "   0-��    1-�� ");
    memset(buf2, 0, sizeof(buf2));
	buf2[0] = stPosParam.ucPreVoidPin;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.ucPreVoidPin = PARAM_CLOSE;
		SaveAppParam();
	}
	else if( iRet==1 )
	{
		stPosParam.ucPreVoidPin = PARAM_OPEN;
		SaveAppParam();
	}
	else if( iRet<0 )
	{
		return ;
	}

	lcdCls();
	lcdFlip();	
	lcdDisplay(0, 0, DISP_CFONT|DISP_REVERSE, "Ԥ��Ȩ��ɳ�����");
	lcdDisplay(0, 2, DISP_CFONT|DISP_REVERSE, "��������");
	lcdDisplay(0, 4, DISP_CFONT, "   0-��    1-�� ");
    memset(buf2, 0, sizeof(buf2));
	buf2[0] = stPosParam.ucPreComVoidPin;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.ucPreComVoidPin = PARAM_CLOSE;
		SaveAppParam();
	}
	else if( iRet==1 )
	{
		stPosParam.ucPreComVoidPin = PARAM_OPEN;
		SaveAppParam();
	}	
	else if( iRet<0 )
	{
		return ;
	}


	lcdCls();
	lcdFlip();	
	lcdDisplay(0, 0, DISP_CFONT|DISP_REVERSE, "Ԥ��Ȩ���������");
	lcdDisplay(0, 2, DISP_CFONT|DISP_REVERSE, "��������");
	lcdDisplay(0, 4, DISP_CFONT, "   0-��    1-�� ");
    memset(buf2, 0, sizeof(buf2));
	buf2[0] = stPosParam.ucPreComPin;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.ucPreComPin = PARAM_CLOSE;
		SaveAppParam();
	}
	else if( iRet==1 )
	{
		stPosParam.ucPreComPin = PARAM_OPEN;
		SaveAppParam();
	}	
	else if( iRet<0)
	{
		return ;
	}
	*/
	
	return ;
}

void SetCurCommType(void)
{
	uint8_t  ucCommType;
	int  iKey;

	ucCommType = stPosParam.stTxnCommCfg.ucCommType;
	if( !(ucCommType & CT_ALLCOMM) || ucCommType>CT_ALLCOMM )
	{
		ucCommType = gstPosCapability.uiCommType;
		ucCommType = ucCommType & ~CT_RS232;
	}
RESTART:
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "ͨѶ��������    ", "COMM SETUP");
	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "1.����   2.����");
	lcdDisplay(0, 4, DISP_CFONT, "3.GPRS   4.CDMA");
	lcdDisplay(0, 6, DISP_CFONT, "5.��̫��       ");
		
	iKey = kbGetKeyMs(30000);
	if( iKey==KEY_CANCEL )
	{
		return ;
	}
	else if( iKey==KEY1 )
	{
		ucCommType = CT_MODEM;
	}
	else if( iKey==KEY2 )
	{
		ucCommType = CT_RS232;
	}
	else if( iKey==KEY3 )
	{
		ucCommType = CT_GPRS;
	}
	else if( iKey==KEY4 )
	{
		ucCommType = CT_CDMA;
	}
	else if( iKey==KEY5 )
	{
		ucCommType = CT_TCPIP;
	}

	if ( (ucCommType>=CT_RS232) && (ucCommType<=CT_WIFI) )
	{
		if( gstPosCapability.uiCommType & ucCommType )
		{
			stPosParam.stTxnCommCfg.ucCommType = ucCommType;
		}
		else
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			lcdDisplay(0, 2, DISP_CFONT, "����û�и�ģ��  ");
			lcdDisplay(0, 4, DISP_CFONT, "����ѡ��ͨѶ��ʽ");
			OkBeep();
			kbGetKeyMs(3000);
			goto RESTART;
		}
	}
	
	return;
}

void SetTpduOutTime(void)
{
	uint8_t buf[32], buf1[32];
	int     iTmpInt, iRet;
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "ͨѶ��������    ", "MANAGE PHONENUM ");
	DispMulLanguageString(0, 2, DISP_CFONT, "���Ĺ���绰:", "INPUT MAG NUM:  ");
	strcpy((char *)buf, (char *)stPosParam.szMPhone);
	lcdDisplay(0, 4, DISP_CFONT, (char *)buf);
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 0, 14, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return ;
	}
	if( iRet>0 )
	{
		strcpy((char *)stPosParam.szMPhone, (char *)buf);
		SaveAppParam();
	}

	memset(buf1, 0, sizeof(buf1));
	BcdToAsc(buf1, stPosParam.sTpdu, 10);
	buf1[10] = 0;

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "ͨѶ��������    ", "   SETUP TPDU   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "TPDU", "TPDU:  ");
	DispMulLanguageString(0, 4, DISP_CFONT, (char *)buf1, (char *)buf1);
	lcdGoto(48, 6);
	iRet = kbGetString(KB_EN_BIGFONT+KB_EN_NUM+KB_EN_REVDISP, 0, 10, (stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return ;
	}
	if( iRet==10 && strcmp((char *)buf1, (char *)buf) ) 
	{
		AscToBcd(stPosParam.sTpdu, buf, 10);
	} 	

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "�Ƿ�Ԥ����");
	lcdDisplay(0, 4, DISP_CFONT, "0-��   1-��");
	iRet = kbGetKeyMs(30000);
	if( iRet==KEY0 )
	{
		stPosParam.bPreDial = PARAM_CLOSE;
		SaveAppParam();
	}
	else if( iRet==KEY1 )
	{
		stPosParam.bPreDial = PARAM_OPEN;
		SaveAppParam();
	}
	else if( iRet==KEY_CANCEL || iRet==KEY_TIMEOUT )
	{
		return ;
	}

	memset(buf1, 0, sizeof(buf1));
	sprintf((char *)buf1, "(20~90S): %d", stPosParam.ucCommWaitTime);

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "ͨѶ��������    ", "   SETUP TPDU   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "���׳�ʱʱ��    ", "SETUP COMM TIME ");
	DispMulLanguageString(0, 4, DISP_CFONT, (char *)buf1, (char *)buf1);
	lcdGoto(112, 6);
	iRet = kbGetString(KB_EN_BIGFONT+KB_EN_NUM+KB_EN_REVDISP, 0, 2, (stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return ;
	}
	iTmpInt = atoi((char *)buf);
	if( iRet>0 && iTmpInt>=20 && iTmpInt<=90 && stPosParam.ucCommWaitTime!=iTmpInt )
	{
		stPosParam.ucCommWaitTime = iTmpInt; 
		SaveAppParam();
	}
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "ͨѶ��������    ", "   SETUP TPDU   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "�����ز�����    ", "   REDAIL TIMES ");
	sprintf((char *)buf, "%d", stPosParam.ucDialRetryTimes);
	lcdDisplay(0, 4, DISP_CFONT, (char *)buf);
	lcdGoto(120, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 0, 1, (stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return ;
	}
	iTmpInt = atoi((char *)buf);
	if( iRet==1 && iTmpInt<=6 && iTmpInt>=1 && stPosParam.ucDialRetryTimes!=iTmpInt )
	{
		stPosParam.ucDialRetryTimes = iTmpInt;
	}

	SaveAppParam();
}



void ClearFile()
{
	int iRet, i;
	int  lCurIndex, iCnt=0,  iTotalNum=0;

	lcdCls();
	lcdFlip();	
	lcdDisplay(0, 0, DISP_CFONT|DISP_REVERSE, "���������ˮ    ");
	lcdDisplay(0, 2, DISP_CFONT, "���������ˮ");
	lcdDisplay(0, 4, DISP_CFONT, "��-ȡ��  ��-ȷ��");
	iRet = kbGetKey();
	if( iRet==KEY_ENTER )
	{
		lCurIndex = 0;
		iCnt=0;
		memset(&gstYktEnrichLog, 0, YKTENRICH_LOG_SIZE);
		iTotalNum= ReadYtkEnrichLog(&lCurIndex);
		
		for(i=0; i<iTotalNum; i++){
			if(gstYktEnrichLog.uiStatus==1)
			{//�Ѿ�����
			   ;
			}
			else
			{
				iCnt = 1;
				lcdCls();
				lcdFlip();	
				lcdDisplay(0, 2, DISP_CFONT, "��δ�����¼");
				lcdDisplay(0, 4, DISP_CFONT, "���Ƚ���");
				PubBeepErr();
				PubWaitKey(3);
				break;
			}
			
			 lCurIndex++;
			 if (lCurIndex > iTotalNum)
			 {
				 break;
			 }

			 memset(&gstYktEnrichLog, 0, YKTENRICH_LOG_SIZE);
			 ReadYtkEnrichLog(&lCurIndex);
		}

		if( iTotalNum==0 || iCnt == 0)
		{
			fileRemove(POS_LOG_FILE);
			fileRemove(REFUND_TRACK_FILE);
			fileRemove(ICC_FAILUR_TXN_FILE);
			fileRemove(POS_M1_LOG_FILE);
			stPosParam.lEnrichTraceNo = 0;
			stPosParam.lNowTraceNo = 0;
			stTransCtrl.iTransNum = 0;
			ClearAllTxnStatus();
			stTransCtrl.ucPosStatus = WORK_STATUS;
			SaveCtrlParam();
		}
		
		
	}

	return ;
}


void SetTestStatus(void)
{
	uint8_t key1;

	key1 = OptionsSelect("    ����״̬    ", stPosParam.ucTestFlag==PARAM_CLOSE,
		"��ǰ״̬:����", "��ǰ״̬:����", "[1].����", "[2].����", stPosParam.ucOprtLimitTime);
	if( (stTransCtrl.iTransNum!=0) && (key1==KEY1 || key1==KEY2) )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT, "    ���Ƚ���    ", "  SETTLE FIRST  ");
		sysBeef(3, 1000);
		kbGetKeyMs(1000);
		return;					
	}
	
	if( key1==KEY2 && stPosParam.ucTestFlag==PARAM_CLOSE )
	{
		stPosParam.ucTestFlag = PARAM_OPEN;
		strcpy((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY);
		stTransCtrl.ucLogonFlag = LOGGEDOFF;
		stTransCtrl.ucPosStatus = TEST_STATUS;
	}
	else if( key1==KEY1 && stPosParam.ucTestFlag==PARAM_OPEN )
	{
		stPosParam.ucTestFlag = PARAM_CLOSE;
		strcpy((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY);
		stTransCtrl.ucLogonFlag = LOGGEDOFF;
		stTransCtrl.ucPosStatus = WORK_STATUS;
	}
	SaveCtrlParam();
	return ;

}

int GenDownloadKeyFromMainPos(unsigned char * pszMid, unsigned char * pszTid, unsigned char ucMasterKeyID)
{
	int iRet;
	unsigned int uiBeginTime=0;
	unsigned int uiCurTime= 0;
	unsigned short	usCRC=0;
	////unsigned char szBuffer[]="0123456789012345";
	//unsigned char szOutBuf[16+1];
	//int i=0;
	int iKey=0;
	//����Ϊ�ֲ��������Լ��ٶԻ���������
	unsigned short  usSendPacketLen;  
	unsigned short  usRecvPacketLen;
	uint8_t	sSendPacketBuff[MAX_PACKET_LEN];	
	uint8_t	sRecvPacketBuff[MAX_PACKET_LEN];
    
	if (ucMasterKeyID > 99)
	{
		ucMasterKeyID = 0;
	}
	
	PubDisplayTitle(TRUE, "װ����Կ");
	if (pszMid == NULL)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "��������");
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "�̻��Ų�ӦΪ��");
		PubWaitKey(0);
	}
	if (pszTid == NULL)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "��������");
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "�ն˺Ų�ӦΪ��");
		PubWaitKey(0);
	}

	memset(sSendPacketBuff, 0, sizeof(sSendPacketBuff));
	usSendPacketLen = 0;

	memcpy(sSendPacketBuff, "\x02\x05\x01\x00\x19", 5);
	usSendPacketLen += 5;
	memcpy(&sSendPacketBuff[usSendPacketLen], pszMid, 15);
	usSendPacketLen += 15;
	memcpy(&sSendPacketBuff[usSendPacketLen], pszTid, 8);
	usSendPacketLen += 8;
	sSendPacketBuff[usSendPacketLen] = ucMasterKeyID;
	usSendPacketLen += 1;
	sSendPacketBuff[usSendPacketLen] = 0x03;
	usSendPacketLen += 1;

	usCRC = PubGetCRC((char *)&sSendPacketBuff[1], usSendPacketLen-1);
	sSendPacketBuff[usSendPacketLen] = (uint8_t)(usCRC/256);
	usSendPacketLen += 1;
	sSendPacketBuff[usSendPacketLen] = (uint8_t)(usCRC%256);//PubCalcLRC(sSendPacketBuff,usSendPacketLen, 0x00);
	usSendPacketLen += 1;

	Rs232Close();
	Rs232Open(PORT_COM1, STR_RS232_PARAM);
	Rs232ResetPurge(RS232_TX_PURGE|RS232_RX_PURGE);

	lcdClrLine(2, 7);
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��������..", 0);
	iKey = kbGetKeyMs(500);
	if(iKey == KEY_CANCEL)
	{
		DisplayMainPosOperFailed("����ȡ��", 5);
		return NO_DISP;
	}
	iRet = MainPosRS232Txd(sSendPacketBuff, usSendPacketLen, 60);
	if (iRet != OK)
	{
		DisplayMainPosOperFailed("����ʧ��", 5);
		return NO_DISP;
	}
	
	kbFlush();
	uiBeginTime = sysGetTimerCount();
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��������(%d)", 0);
	while (Rs232CheckPurge(RS232_RX_PURGE) != ERR_RS232_OK)
	{
		// һֱ��GenPOS�·�����,����180S
		uiCurTime = sysGetTimerCount();
		if (uiCurTime - uiBeginTime > 2*60*1000)//2����
		{
			DisplayMainPosOperFailed("�������ݳ�ʱ", 5);
			return NO_DISP;
		}
		lcdDisplay(0, 3, DISP_CFONT|DISP_CLRLINE|DISP_MEDIACY, "��������(%d)", (uiCurTime - uiBeginTime)/1000);
		iRet = kbGetKeyMs(0);
		if(iRet == KEY_CANCEL)
		{
			DisplayMainPosOperFailed("����ȡ��", 5);
			return NO_DISP;
		}
	}

	memset(sRecvPacketBuff,0,sizeof(sRecvPacketBuff));
	usRecvPacketLen = 0;
	iRet = MainPosRS232Rxd(sRecvPacketBuff, MAX_PACKET_LEN, 60, (unsigned int*)&usRecvPacketLen);
	if (iRet != ERR_RS232_OK)
	{
		DisplayMainPosOperFailed("�������ݴ�", 5);
		return NO_DISP;
	}
	
	//���ص��Ǵ����룬��ʾ������Ϣ������
//	if (memcmp(sRecvPacketBuff, "\x02\x40", 2) == 0)
//	{
//		DisplayMainPosErrMsg(sRecvPacketBuff[2]);
//		return NO_DISP;
//	}
	
	if (memcmp(sRecvPacketBuff, "\x02\x05\x01\x00", 4) != 0)
	{
		DisplayMainPosOperFailed("CMD��ƥ��", 5);
		return NO_DISP;
	}

	if (memcmp(&sRecvPacketBuff[5], "\x00\x00\x00\x00", 4) != 0)
	{
		DisplayMainPosErrMsg(sRecvPacketBuff[8]);
		return NO_DISP;
	}
	
	if (memcmp(&sRecvPacketBuff[9], pszMid, 15) != 0)
	{
		DisplayMainPosOperFailed("�̻��Ų�ƥ��", 5);
		return NO_DISP;
	}
	
	if (memcmp(&sRecvPacketBuff[24], pszTid, 8) != 0)
	{
		DisplayMainPosOperFailed("�ն˺Ų�ƥ��", 5);
		return NO_DISP;
	}

	if( stPosParam.ucKeyMode==KEY_MODE_DES )
	{
		memcpy(&sRecvPacketBuff[32+8], &sRecvPacketBuff[32], 8);
	}	

	iRet = PedWriteMasterKey(PARITY_NONE+TDEA_NONE, 1, ucMasterKeyID+1, 16, &sRecvPacketBuff[32]);
	if( iRet!=PED_RET_OK )
    {
        lcdClrLine(2, 7);
        lcdFlip();  
        DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  װ����Կ����  ", " LOAD KEY ERROR ");
        lcdDisplay(0, 6, DISP_CFONT, "iRet=%d", iRet);
        ErrorBeep();
        kbGetKeyMs(3000);
        return 3;
    }
	stPosParam.ucKeyIndex = ucMasterKeyID;
	SaveAppParam();	
	//װ�سɹ����ͳɹ���Ϣ��ȥĸPOS
	Rs232ResetPurge(RS232_TX_PURGE|RS232_RX_PURGE);

	memset(sSendPacketBuff, 0, sizeof(sSendPacketBuff));
	usSendPacketLen = 0;

	memcpy(sSendPacketBuff, "\x02\x80\x01\x00\x01", 5);
	usSendPacketLen += 5;
	sSendPacketBuff[usSendPacketLen] = 0x03;
	usSendPacketLen += 1;

	usCRC = PubGetCRC((char *)&sSendPacketBuff[1], usSendPacketLen-1);
	sSendPacketBuff[usSendPacketLen] = (uint8_t)(usCRC/256);
	usSendPacketLen += 1;
	sSendPacketBuff[usSendPacketLen] = (uint8_t)(usCRC%256);//0x80;//PubCalcLRC(sSendPacketBuff,usSendPacketLen, 0x00);
	usSendPacketLen += 1;	
	iRet = MainPosRS232Txd(sSendPacketBuff, usSendPacketLen, 60);
	//if (iRet != OK)
	{
		//���Ͳ��ɹ�Ҳʲô������
		//DisplayMainPosOperFailed("����ʧ��", 5);
		//return NO_DISP;
	}
	Rs232Close();
/*
	lcdCls();
	PubDisplayTitle(TRUE, "���Լ��ܽ��");
	//PubGetString(ECHO_IN|ALPHA_IN, 16, 16, szBuffer, USER_OPER_TIMEOUT);
	for(i=0; i<(sizeof(szBuffer)); ++i)
	{
		lcdDisplay((i%8)*16, 4+(i/8)*2, DISP_CFONT, "%02u", szBuffer[i]);
	}
	memset(szOutBuf, 0, sizeof(szOutBuf));
	PedTDEA(TDEA_ENCRYPT, TYPE_MASTERKEY, ucMasterKeyID+1, 16, szBuffer, szOutBuf);
	for(i=0; i<(sizeof(szOutBuf)); ++i)
	{
		lcdDisplay(0, 2, DISP_CFONT|DISP_CLRLINE, "%d,%x", i, szOutBuf[i]);
		PubWaitKey(0);
	}
*/
	return OK;
}

void SetMasterKeys(void)
{
	uint8_t buf[64];
	uint8_t buf1[64];
	uint8_t ucTmpKeyId;
	uint8_t ucLen,ucMode;
	int iTmpInt, iRet, iKey;
	const char* KeyMenu[] = 
	{
		"�ն���Կ����    ",
		"1 ������Կ      ",
		"2 �ֹ�������Կ  ",
        "3 ĸPOS������Կ ",
		"4 ������Կ������"
	};

	while(1)
	{
		iKey = DispMenu(KeyMenu, 5, 1, KEY1, KEY4, stPosParam.ucOprtLimitTime);
		switch(iKey)
		{
		case KEY1:
			break;

		case KEY2:
			if( stPosParam.ucKeyMode==KEY_MODE_DES )
			{
				ucLen = 16;
				ucMode = 0x01;
			}
			else  ///3DES
			{
				ucLen = 32;
				ucMode = 0x03; 
			}
			lcdCls();
			lcdFlip();	
			DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ֹ�������Կ    ", " INPUT KEY DATA ");
			DispMulLanguageString(0, 2, DISP_CFONT, "����Կ������:", "INPUT KEY INDEX:");
			DispMulLanguageString(0, 4, DISP_CFONT, "����:(00--99)", "INPUT:(00--99)");
			lcdGoto(112, 6);
			memset(buf,0,sizeof(buf));
			iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 1, 2, (stPosParam.ucOprtLimitTime*1000), 
				(char *)buf);	
			if( iRet>0 )
			{
				iTmpInt = atoi((char *)buf);
				if( iTmpInt>99 || iTmpInt<0 )
				{
					lcdClrLine(6, 7);
					lcdFlip();	
					DispMulLanguageString(0, 6, DISP_CFONT, "��Կ���������", "KEY INDEX ERROR");
					kbGetKeyMs(1000);
					break;
				}
				ucTmpKeyId = iTmpInt+1;
			}
			else 
				break;
			lcdCls();
			lcdFlip();	
			DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "�ֹ�������Կ    ", " INPUT KEY DATA ");
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 2, DISP_CFONT, "����������Կ  ", "INPUT KEY DATA:");

			lcdGoto(0, 4);
			memset(buf,0,sizeof(buf));
			iRet = kbGetString(0x175, ucLen, ucLen, 
				(stPosParam.ucOprtLimitTime*1000), (char *)buf);
			if( iRet!=ucLen )
				break;

			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 2, DISP_CFONT, "����������Կ  ", "INPUT KEY DATA:");

			lcdGoto(0, 4);
			memset(buf1,0,sizeof(buf1));
			iRet = kbGetString(0x175, ucLen, ucLen, 
				(stPosParam.ucOprtLimitTime*1000), (char *)buf1);
			if( iRet!=ucLen )
				break;

			if( memcmp(buf, buf1, ucLen)!=0 )
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				lcdDisplay(0, 2, DISP_CFONT, "�������벻һ��");
				kbGetKeyMs(20000);
				return ;
			}
			
			if( stPosParam.ucKeyMode==KEY_MODE_DES )
			{
				AscToBcd(buf1, buf, ucLen);
				memcpy(buf1+8, buf1, 8);
			}
			else
			{
				AscToBcd(buf1, buf, ucLen);
			}			

			iRet = PedWriteMasterKey(PARITY_NONE+TDEA_NONE, 1, ucTmpKeyId, 16, buf1);
			if( iRet!=PED_RET_OK )  
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  װ����Կ����  ", " LOAD KEY ERROR ");
				lcdDisplay(0, 6, DISP_CFONT, "iRet=%d", iRet);
				ErrorBeep();
				kbGetKeyMs(3000);
			}
			break;
			
		case KEY3:
            lcdCls();
            PubDisplayTitle(TRUE, "װ����Կ");
            DispMulLanguageString(0, 2, DISP_CFONT, "����Կ������:", "INPUT KEY INDEX:");
            DispMulLanguageString(0, 4, DISP_CFONT, "����:(00--99)", "INPUT:(00--99)");
            lcdGoto(112, 6);
            memset(buf,0,sizeof(buf));
            iRet = kbGetString(KB_EN_REVDISP+KB_EN_NUM+KB_EN_BIGFONT, 1, 2, (stPosParam.ucOprtLimitTime*1000), (char*)buf);    
            if( iRet>0 )
            {
                iTmpInt = atoi((char *)buf);
                if( iTmpInt>99 || iTmpInt<0 )
                {
                    lcdClrLine(6, 7);
                    DispMulLanguageString(0, 6, DISP_CFONT, "��Կ���������", "KEY INDEX ERROR");
                    kbGetKeyMs(1000);
                    break;
                }
                //ucTmpKeyId = iTmpInt+1;
            }
            else 
                break;

            iRet = GenDownloadKeyFromMainPos(stPosParam.szUnitNum, stPosParam.szPosId, iTmpInt);
            if (iRet == OK)
            {
//              stPosParam.ucKeyIndex = iTmpInt;
//              SaveAppParam();
                
                PubLongBeep();
                lcdClrLine(2, 7);
                lcdDisplay(0, 3, DISP_CFONT, "��Կװ�سɹ�");
                PubWaitKey(0);
            }
            break;

		case KEY4:
			lcdCls();
			lcdFlip();	
			DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "������Կ������  ", "   KEY INDEX    ");
			iTmpInt = stPosParam.ucKeyIndex;
			sprintf((char *)buf, "��Կ������:%02d", iTmpInt);
			lcdDisplay(0, 2, DISP_CFONT, (char *)buf);
			DispMulLanguageString(0, 4, DISP_CFONT, "����:(00--99)", "INPUT:(00--99)  ");
			sprintf((char *)buf, "%02d", iTmpInt);
			lcdGoto(112, 6);
			memset(buf,0,sizeof(buf));
			iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 1, 2, (stPosParam.ucOprtLimitTime*1000), 
				(char *)buf);
			if( iRet>0 && (iTmpInt!=atoi((char *)buf)) )
			{
				iTmpInt = atoi((char *)buf);
				if( iTmpInt>99 || iTmpInt<0 )
				{
					lcdClrLine(6, 7);
					lcdFlip();	
					DispMulLanguageString(0, 6, DISP_CFONT, "��Կ���������", "KEY INDEX ERROR");
					kbGetKeyMs(1000);
					break;
				}
				stPosParam.ucKeyIndex = iTmpInt;
				SaveAppParam();
			}
			break;

		default:
			return; 
		}
	}
}

void SetTranResendTime(void)
{
	uint8_t buf[32];
	int     iTmpInt;
	int     iRet;
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "�����ط�����    ", "SETUP RETRY TIME");
	iTmpInt = stPosParam.ucResendTimes;
	memset(buf, 0, sizeof(buf));
	sprintf((char *)buf, "�ط�����:%d", iTmpInt);
	lcdDisplay(0, 2, DISP_CFONT, (char *)buf);
	
	sprintf((char *)buf, "%d", iTmpInt);
	lcdGoto(120, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 1, 1, (stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==1 && iTmpInt!=atoi((char *)buf) )
	{
		iTmpInt = atoi((char *)buf);
		stPosParam.ucResendTimes = iTmpInt;
	}

	SaveAppParam();
	return ;
}


void SetTransFlow(void)
{
	uint8_t ucKey;
	uint8_t buf2[2];
	int iRet;
	
/*	if( stPosParam.ucEmvSupport==PARAM_OPEN )
	{
		ucKey = OptionsSelect(" ����Ա�ж�IC�� ", stPosParam.ucSelectCardType==PARAM_OPEN,
			"��ǰ����:��", "��ǰ����:��", "[1].��", "[2].��", stPosParam.ucOprtLimitTime);
		if( ucKey==KEY2 )
		{
			stPosParam.ucSelectCardType = PARAM_OPEN;
		}
		else
		{
			stPosParam.ucSelectCardType = PARAM_CLOSE;
		}
	}*/
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "���俨������    ", "SUPPORT EMV TRAN");
	lcdDisplay(0, 2, DISP_CFONT, "�Ƿ�֧�����俨��");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
	buf2[0] = stPosParam.ucManualInput;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.ucManualInput = PARAM_CLOSE;
	}
	else if( iRet==1 )
	{
		stPosParam.ucManualInput = PARAM_OPEN;
	}

	SaveAppParam();

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "��Ȩ���ʵ�ַ�ʽ", "  PRE-CMP MODE  ");

	DispMulLanguageString(0, 2, DISP_CFONT, "0 ͬʱ֧��      ", "0 BOTH          ");
	DispMulLanguageString(0, 4, DISP_CFONT, "1 Ԥ��Ȩ�������", "1 PRE-CMP ONLINE");
	DispMulLanguageString(0, 6, DISP_CFONT, "2 Ԥ��Ȩ���֪ͨ", "2 PRECMP OFFLINE");
	kbFlush();
	ucKey = kbGetKey();
	if( ucKey==KEY0 ) 
		stPosParam.ucConfirm = '0';
	else if( ucKey==KEY1 ) 
		stPosParam.ucConfirm = '1';
	else if( ucKey==KEY2 ) 
		stPosParam.ucConfirm = '2';
	if( stPosParam.ucConfirm=='0' )
	{
		MaskTrans(POS_AUTH_CM, 4, "Ԥ��Ȩ���(����)", KEY2);
		MaskTrans(POS_OFF_CONFIRM, 11, "Ԥ��Ȩ���(֪ͨ)", KEY2);
	}
	else if( stPosParam.ucConfirm=='1' )
	{
		MaskTrans(POS_AUTH_CM, 4, "Ԥ��Ȩ���(����)", KEY2);
		MaskTrans(POS_OFF_CONFIRM, 11, "Ԥ��Ȩ���(֪ͨ)", KEY1);
	}
	else if( stPosParam.ucConfirm=='2' )
	{
		MaskTrans(POS_AUTH_CM, 4, "Ԥ��Ȩ���(����)", KEY1);
		MaskTrans(POS_OFF_CONFIRM, 11, "Ԥ��Ȩ���(֪ͨ)", KEY2);
	}
	
/*	ucKey = OptionsSelect("    ��    ��    ", stPosParam.ucSendOffFlag==PARAM_OPEN,
		"״̬:��������", "״̬:��������", "[1].��������  ", "[2].��������", stPosParam.ucOprtLimitTime);
	if( ucKey==KEY1 && stPosParam.ucSendOffFlag==PARAM_CLOSE ) 
	{
		stPosParam.ucSendOffFlag = PARAM_OPEN;
	}
	else if( ucKey==KEY2 && stPosParam.ucSendOffFlag==PARAM_OPEN ) 
	{
		stPosParam.ucSendOffFlag = PARAM_CLOSE;
	} 
	else if( ucKey==KEY_CANCEL )
		return; */

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "С�ѽ���        ", "SUPPORT EMV TRAN");
	lcdDisplay(0, 2, DISP_CFONT, "�Ƿ�֧��С��");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
	buf2[0] = stPosParam.ucTipOpen;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.ucTipOpen = PARAM_CLOSE;
	}
	else if( iRet==1 )
	{
		stPosParam.ucTipOpen = PARAM_OPEN;
	}

	SaveAppParam();

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�Ƿ��Զ�ǩ��    ", "SUPPORT EMV TRAN");
	lcdDisplay(0, 2, DISP_CFONT, "�Ƿ��Զ�ǩ��");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
	buf2[0] = stPosParam.ucAutoLogoff;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.ucAutoLogoff = PARAM_CLOSE;
	}
	else if( iRet==1 )
	{
		stPosParam.ucAutoLogoff = PARAM_OPEN;
	}

	SaveAppParam();
}

void AutoLogoutSetup(void)
{
	int     iRet;
    uint8_t buf2[2];
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "ǩ������        ", "SUPPORT EMV TRAN");
	lcdDisplay(0, 2, DISP_CFONT, "������Զ�ǩ��");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
	buf2[0] = stPosParam.ucAutoLogoff;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.ucAutoLogoff = PARAM_CLOSE;
	}
	else if( iRet==1 )
	{
		stPosParam.ucAutoLogoff = PARAM_OPEN;
	}
	SaveAppParam();
	return ;
}

void TipperSetup(void)
{
	int     iRet, iTmpInt;
	uint8_t dispbuf[20], buf[20];
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "С�ѱ�������    ", "SUPPORT EMV TRAN");

	sprintf((char *)dispbuf, "С�Ѱٷֱ�(%d%%)", stPosParam.ucTipper);
	lcdDisplay(0, 2, DISP_CFONT, "%s", dispbuf);
	sprintf((char *)buf, "%d", stPosParam.ucTipper);
	lcdGoto(112, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 1, 2, (stPosParam.ucOprtLimitTime*1000), (char *)buf);
	iTmpInt = atoi((char *)buf);
	if( iRet>0 && iTmpInt!=stPosParam.ucTipper ) 
	{
		stPosParam.ucTipper = iTmpInt;
	}
	SaveAppParam();
	return ;
}

void SetTransMask(void)
{
	int     iRet;
	uint8_t buf2[2];

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "���ν���        ", "SUPPORT EMV TRAN");
	lcdDisplay(0, 2, DISP_CFONT, "����");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[0]&0x02)>>1)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[0] &= ~0x02;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[0] |= 0x02;
	}
    else if(iRet<0)
    {
        return ;
    }

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "����");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[0]&0x04)>>2)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[0] &= ~0x04;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[0] |= 0x04;
	}
    else if(iRet<0)
    {
        return ;
    }

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "��ѯ");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[0]&0x80)>>7)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[0] &= ~0x80;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[0] |= 0x80;
	}
    else if(iRet<0)
    {
        return ;
    }

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "Ԥ��Ȩ");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[0]&0x40)>>6)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[0] &= ~0x40;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[0] |= 0x40;
	}
    else if(iRet<0)
    {
        return ;
    }

    
	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "����");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[1]&0x80)>>7)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[1] &= ~0x80;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[1] |= 0x80;
	}
    else if(iRet<0)
    {
        return ;
    }

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "�������");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[1]&0x40)>>6)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[1] &= ~0x40;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[1] |= 0x40;
	}
    else if(iRet<0)
    {
        return ;
    }
    
	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "�˻�");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = (stPosParam.sTransOpen[0]&0x01)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[0] &= ~0x01;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[0] |= 0x01;
	}
    else if(iRet<0)
    {
        return ;
    }


	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "Ԥ��Ȩ����");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
	buf2[0] = ((stPosParam.sTransOpen[0]&0x20)>>5)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[0] &= ~0x20;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[0] |= 0x20;
	}
    else if(iRet<0)
    {
        return ;
    }

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "Ԥ��Ȩ�������");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[0]&0x10)>>4)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[0] &= ~0x10;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[0] |= 0x10;
	}
    else if(iRet<0)
    {
        return ;
    }


	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "Ԥ��Ȩ��ɳ���");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[0]&0x08)>>3)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[0] &= ~0x08;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[0] |= 0x08;
	}
    else if(iRet<0)
    {
        return ;
    }
    

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "����Ǯ��Ȧ��");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[1]&0x04)>>2)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[1] &= ~0x04;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[1] |= 0x04;
	}
    else if(iRet<0)
    {
        return ;
    }


	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "Ԥ��Ȩ���֪ͨ");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[1]&0x20)>>5)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[1] &= ~0x20;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[1] |= 0x20;
	}
    else if(iRet<0)
    {
        return ;
    }

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "�ű�������֪ͨ");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[1]&0x10)>>4)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[1] &= ~0x10;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[1] |= 0x10;
	}
    else if(iRet<0)
    {
        return ;
    }

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "�����ֽ��ѻ�����");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[1]&0x08)>>3)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[1] &= ~0x08;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[1] |= 0x08;
	}
    else if(iRet<0)
    {
        return ;
    }

/*	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "����Ǯ������");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[1]&0x02)>>1)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[1] &= ~0x02;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[1] |= 0x02;
	}
    else if(iRet<0)
    {
        return ;
    }*/

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "���ڸ�������");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[1]&0x01))+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[1] &= ~0x01;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[1] |= 0x01;
	}
    else if(iRet<0)
    {
        return ;
    }

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "���ڸ������ѳ���");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[2]&0x80)>>7)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[2] &= ~0x80;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[2] |= 0x80;
	}
    else if(iRet<0)
    {
        return ;
    }

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "��������");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[2]&0x40)>>6)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[2] &= ~0x40;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[2] |= 0x40;
	}
    else if(iRet<0)
    {
        return ;
    }

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "�������ѳ���");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[2]&0x20)>>5)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[2] &= ~0x20;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[2] |= 0x20;
	}
    else if(iRet<0)
    {
        return ;
    }

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "�����Ȧ���ཻ��");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[2]&0x10)>>4)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[2] &= ~0x10;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[2] |= 0x10;
	}
    else if(iRet<0)
    {
        return ;
    }

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "ԤԼ����");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[2]&0x08)>>3)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[2] &= ~0x08;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[2] |= 0x08;
	}
    else if(iRet<0)
    {
        return ;
    }

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "ԤԼ���ѳ���");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[2]&0x04)>>2)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[2] &= ~0x04;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[2] |= 0x04;
	}
    else if(iRet<0)
    {
        return ;
    }

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "��������");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[2]&0x02)>>1)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[2] &= ~0x02;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[2] |= 0x02;
	}
    else if(iRet<0)
    {
        return ;
    }

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "�������ѳ���");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[2]&0x01))+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[2] &= ~0x01;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[2] |= 0x01;
	}
    else if(iRet<0)
    {
        return ;
    }

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "��������ֵ");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
    buf2[0] = ((stPosParam.sTransOpen[2]&0x80)>>7)+0x30;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.sTransOpen[3] &= ~0x80;
	}
	else if( iRet==1 )
	{
		stPosParam.sTransOpen[3] |= 0x80;
	}
    else if(iRet<0)
    {
        return ;
    }

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "���ν���        ", "SUPPORT EMV TRAN");
	lcdDisplay(0, 2, DISP_CFONT, "С�����Ȩ�Ľ���");
	lcdDisplay(0, 4, DISP_CFONT, "0-��֧��  1-֧��");
    memset(buf2, 0, sizeof(buf2));
	if (stPosParam.ucOfflineSettleFlag!=PARAM_CLOSE && stPosParam.ucOfflineSettleFlag != PARAM_OPEN)
	{
		stPosParam.ucOfflineSettleFlag = PARAM_OPEN;
	}
    buf2[0] = stPosParam.ucOfflineSettleFlag;
	iRet = SelectSetTwo(buf2);
	if( iRet==0 )
	{
		stPosParam.ucOfflineSettleFlag = PARAM_CLOSE;
	}
	else if( iRet==1 )
	{
		stPosParam.ucOfflineSettleFlag = PARAM_OPEN;
	}
    else if(iRet<0)
    {
        return ;
    }

	SaveAppParam();
}

void SetTranMaxNumber(void)
{
	uint8_t buf[18];
	int iTmpInt, iRet;
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "����ױ�������", "  TRAN MAX NUM  ");
	sprintf((char *)buf, "����ױ���:%d", stPosParam.iMaxTransTotal);

	lcdDisplay(0, 2, DISP_CFONT, (char *)buf);
	while(1) 
	{
		lcdClrLine(4, 7);
		lcdFlip();	
		sprintf((char *)buf, "%d", stPosParam.iMaxTransTotal);
		lcdGoto(104, 6);
		iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 1, 3, (stPosParam.ucOprtLimitTime*1000), (char *)buf);
		if( iRet<0 )
			return;
		iTmpInt = atoi((char *)buf);
		if( iTmpInt>500 || iTmpInt==0 )
		{
			lcdClrLine(6, 7);
			lcdFlip();	
			DispMulLanguageString(0, 6, DISP_CFONT, "    ��������    ", "   NUM ERROR   ");
			ErrorBeep();
			continue;
		}
		if( iRet>0 && iTmpInt!=stPosParam.iMaxTransTotal )
		{
			stPosParam.iMaxTransTotal = iTmpInt;
			SaveAppParam();
			break;
		}
		else
			return;
	}
	
}


void  SetMaxOfflineNums(void)
{
	uint8_t buf[18];
	int iTmpInt, iRet;
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "�Զ������ۼƱ���", "  TRAN MAX NUM  ");
	sprintf((char *)buf, "����������:%d", stPosParam.ucMaxOfflineTxn);
	
	lcdDisplay(0, 2, DISP_CFONT, (char *)buf);
	while(1) 
	{
		lcdClrLine(4, 7);
		lcdFlip();	
		sprintf((char *)buf, "%d", stPosParam.ucMaxOfflineTxn);
		lcdGoto(104, 6);
		iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 1, 2, (stPosParam.ucOprtLimitTime*1000), (char *)buf);
		if( iRet<0 )
			return;
		iTmpInt = atoi((char *)buf);
		if( iTmpInt>10 || iTmpInt==0 )
		{
			lcdClrLine(6, 7);
			lcdFlip();	
			DispMulLanguageString(0, 6, DISP_CFONT, "    ��������    ", "   NUM ERROR   ");
			ErrorBeep();
			continue;
		}
		if( iRet>0 && iTmpInt!=stPosParam.ucMaxOfflineTxn )
		{
			stPosParam.ucMaxOfflineTxn = iTmpInt;
			SaveAppParam();
			break;
		}
		else
			return;
	}
}

void TestPrint(void)
{
	if( OneTwoSelect("��ӡ����", "��ӡ", "ȡ��")!=KEY1 )
		return;
	
	prnInit();
	PrnNormalFontData("\n\n\n\n");
	prnPrintf("��ӡ����\n");
	prnPrintf("����������\n");
	prnPrintf("����������\n");
	prnPrintf("����������\n");
	prnPrintf("����������\n");
	prnPrintf("����������\n");
	prnPrintf("����������\n");
	prnPrintf("����������\n");
	prnPrintf("����������\n");
	prnPrintf("����������\n");
	prnPrintf("����������\n");
//	prnPrintf("\f");
	
	PrintData();
}


uint8_t MaskTrans(int iTransId, int iMaskBit, char *pFuncName, uint8_t ucInKey)
{
	uint8_t ucKey;
	uint8_t ucEnv_buf[16];
	uint8_t ucMask = 0x80;
	int iIndex = 0;
	
	if( iMaskBit>8 ) 
		iIndex = 1;
	
	ucMask = ucMask >> (iMaskBit - 1) % 8;
	if( ucInKey )
		ucKey = ucInKey;
	else
	{
		ucKey = OptionsSelect(pFuncName, CheckSupport(iTransId), "��ǰ����:֧��", "��ǰ����:��֧��", "[1].��֧��" , 
			"[2].֧��", stPosParam.ucOprtLimitTime);
	}
	switch(ucKey) 
	{
	case KEY2:
		stPosParam.sTransOpen[iIndex] |= ucMask;
		BcdToAsc(ucEnv_buf, stPosParam.sTransOpen, 4);
		ucEnv_buf[4] = 0;
		return KEY2;
		break;
	case KEY1:
		stPosParam.sTransOpen[iIndex] &= ~ucMask;
		BcdToAsc(ucEnv_buf, stPosParam.sTransOpen, 4);
		ucEnv_buf[4] = 0;
		return KEY1;
		break;
	case KEY_CANCEL:
		return KEY_CANCEL;
	}
	return OK;
}

void DispMulLanguageString(int Col, int Line, uint32_t Mode, char *pstChBuf, char *pstEnBuf)
{
	if( stPosParam.iDispLanguage==1 )
	{
		lcdDisplay(Col, Line, Mode, pstChBuf);		
	}
	else
	{
		lcdDisplay(Col, Line, Mode, pstEnBuf);		
	}
}

void SetDispLanguage(void)
{
	int  iRet;
	uint8_t  szBuf[10];
		
	lcdCls();
	lcdFlip();	
	kbFlush();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  ������ʾ����  ", " SETUP LANGUAGE ");
	DispMulLanguageString(0, 2, DISP_CFONT, "[1]:����", "[1]:CHINESE");
	DispMulLanguageString(0, 4, DISP_CFONT, "[2]:Ӣ��", "[2]:ENGLISH");
	
	do {
		iRet = kbGetKeyMs(stPosParam.ucOprtLimitTime*1000);
	} while( iRet!=KEY1 && iRet!=KEY2 && iRet!=KEY_ENTER && iRet!=KEY_CANCEL );
	
	if( iRet==KEY1 )
	{
		stPosParam.iDispLanguage = 1;
		memset(szBuf, 0, sizeof(szBuf));
		sprintf((char *)szBuf, "%d", stPosParam.iDispLanguage);
	}
	else if( iRet==KEY2 )
	{
		stPosParam.iDispLanguage = 2;
		memset(szBuf, 0, sizeof(szBuf));
		sprintf((char *)szBuf, "%d", stPosParam.iDispLanguage);
	}

	SaveAppParam();
	return ;
}

void SetPrintTicketNum(void)
{
	uint8_t  buf[32];
	int  tmpInt, iRet;
	int  key;
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "��ӡ��������    ", "SETUP PRINT NUMS");
	tmpInt = stPosParam.ucTicketNum - '0';
	sprintf((char *)buf, "��ӡ����: %d", tmpInt);
	lcdDisplay(0, 2, DISP_CFONT, (char *)buf);
	lcdGoto(120, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 1, 1, (stPosParam.ucOprtLimitTime*1000), (char *)buf);
	tmpInt = atoi((char *)buf);
	key = 0x30 + tmpInt;
	if( iRet==1 && tmpInt<=3 && tmpInt>=1 && key!=stPosParam.ucTicketNum )
	{
		stPosParam.ucTicketNum = key; 
		SaveAppParam();
	}
	
	return ;
}

void SetErrPrnTicketNum(void)
{
	int  key;

	key = OptionsSelect(" ���ϵ���ӡ���� ", stPosParam.ucErrorPrt==PARAM_OPEN,
		"��ǰ����:��ӡ", "��ǰ����:����ӡ",	"[1].����ӡ", "[2].��ӡ", stPosParam.ucOprtLimitTime);

	switch(key) 
	{
	case KEY2:
		stPosParam.ucErrorPrt = PARAM_OPEN;
		break;
	case KEY1:
		stPosParam.ucErrorPrt = PARAM_CLOSE;
		break;
	case KEY_CANCEL:
		return;
	}
	SaveAppParam();

}


void PrintParam(uint8_t prtFlag)
{
   	int	i;
    uint8_t   buf[60];
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    ������ӡ    ", "   PIRNT PARA   ");
	DispMulLanguageString(0, 4, DISP_CFONT, "���Ժ�... ", "WAITTING...");
	prnInit();
	PrnBigFontChnData("��������������Ϣ\n");
	memset(buf, 0, sizeof(buf));
	sprintf((char *)buf, "�̻�����:%s\n", stPosParam.szUnitChnName);
	PrnSmallFontData((char *)buf);
	prnPrintf("�̻����: %s\n", stPosParam.szUnitNum);
	prnPrintf("�ն˱��: %s\n", stPosParam.szPosId);
	prnPrintf("�ز�����: %d\n", stPosParam.ucDialRetryTimes);
	BcdToAsc(buf, stPosParam.sTpdu, 10);
	buf[10] = 0x00;
	prnPrintf("TPDU����: %s\n", buf);
	prnPrintf("�����ط�����: %d\n", stPosParam.ucResendTimes);
	i = stPosParam.ucTicketNum - '0';
	prnPrintf("��ӡƾ����: %d\n", i);

	if( stPosParam.ucErrorPrt==PARAM_OPEN )
	{
		prnPrintf("���ϵ�: ��ӡ\n");
	}
	else
	{
		prnPrintf("���ϵ�: ����ӡ\n");
	}
	if( CheckSupport(POS_PREAUTH)==TRUE )
	{
		prnPrintf("Ԥ��Ȩ�ཻ��: ֧��\n");
	}
	else
	{
		prnPrintf("Ԥ��Ȩ�ཻ��: ��֧��\n");
	}

	if( CheckSupport(POS_REFUND)==TRUE )
	{
		prnPrintf("�˻�����: ֧��\n");
	}
	else
	{
		prnPrintf("�˻�����: ��֧��\n");
	}

	prnPrintf("ͨѶ��ʱʱ��: %d��\n", stPosParam.ucCommWaitTime);
	prnPrintf("���κ�:    %ld\n", stPosParam.lNowBatchNum);
	prnPrintf("ƾ֤��:    %ld\n", stPosParam.lNowTraceNo);
	prnPrintf("����ױ���: %d��\n", stPosParam.iMaxTransTotal);
	prnPrintf("�Զ����ͱ���: %d��\n", stPosParam.ucMaxOfflineTxn);
	prnPrintf("������ʱʱ��: %d��\n", stPosParam.ucOprtLimitTime);
	if( stPosParam.ucManualInput==PARAM_OPEN )
	{
		prnPrintf("���俨��: ֧��\n");
	}
	else
	{
		prnPrintf("���俨��: ��֧��\n");
	}
	if( stPosParam.ucVoidSwipe==PARAM_OPEN )
	{
		prnPrintf("�����ཻ���Ƿ�ˢ��: ��\n");
	}
	else
	{
		prnPrintf("�����ཻ���Ƿ�ˢ��: ��\n");
	}
	if( stPosParam.ucVoidPin==PARAM_OPEN )
	{
		prnPrintf("���ѳ��������Ƿ�������: ��\n");
	}
	else
	{
		prnPrintf("���ѳ��������Ƿ�������: ��\n");
	}
	if( stPosParam.ucPreVoidPin==PARAM_OPEN )
	{
		prnPrintf("Ԥ��Ȩ���������Ƿ�������: ��\n");
	}
	else
	{
		prnPrintf("Ԥ��Ȩ���������Ƿ�������: ��\n");
	}
	if( stPosParam.ucPreComVoidPin==PARAM_OPEN )
	{
		prnPrintf("Ԥ��Ȩ��ɳ��������Ƿ�������: ��\n");
	}
	else
	{
		prnPrintf("Ԥ��Ȩ��ɳ��������Ƿ�������: ��\n");
	}
	if( stPosParam.ucPreComPin==PARAM_OPEN )
	{
		prnPrintf("Ԥ��Ȩ����������Ƿ�������: ��\n");
	}
	else
	{
		prnPrintf("Ԥ��Ȩ����������Ƿ�������: ��\n");
	}

/*	if( stPosParam.ucSendOffFlag==PARAM_OPEN )
		prnPrintf("�������ͷ�ʽ: ��������\n");
	else 
		prnPrintf("�������ͷ�ʽ: ��������\n"); */
	if( stPosParam.ucAutoLogoff==PARAM_OPEN )
	{
		prnPrintf("�Ƿ��Զ�ǩ��: ��\n");
	}
	else
	{
		prnPrintf("�Ƿ��Զ�ǩ��: ��\n");
	}

	i = stPosParam.ucKeyIndex;
	prnPrintf("����Կ����: %d\n", i);
/*	if( stPosParam.ucSelectCardType==PARAM_OPEN )
	{
		prnPrintf("����Ա�ж�IC��: ��\n");
	}
	else
	{
		prnPrintf("����Ա�ж�IC��: ��\n");
	}*/

	prnPrintf("\n\n\n\n\n");
	PrintData();

	if( stPosParam.ucEmvSupport==PARAM_OPEN )
		PrnEmvParam();	// ��ӡEMV����
}

uint8_t CheckParamOpen(char * item, char condition)
{
	if( condition ) 
	{
		prnPrintf("%s:��\n", item);
	}
	else 
	{
		prnPrintf("%s:��\n", item);
	}
	return OK;
}

void DeviceTest()
{
	int ikey;
	const char * test_main[] = 
	{
		"    ���߲���    ",
		"1 ��ӡ��        ",
		"2 ��ʾ��        ",
		"3 ������        ",
		"4 ����          ",
		"5 �������      "
	};

	while(1) 
	{
		ikey = DispMenu(test_main, 6, 1, KEY1, KEY5, stPosParam.ucOprtLimitTime);
		switch(ikey) {
		case KEY1:
			TestPrint();
			prnInit(); //��ֹ������ӡ��,��Ĳ����ٴ�ӡ����
			break;
		case KEY2:
			LcdTest(1);
			break;
		case KEY3:
			MagTest(1);
			break;
		case KEY4:
			KeyboardTest();
			kbSetSlipFW(ON);
			break;
		case KEY5:
			TestResponse();
			break;

		default:
			return;
		}
	}
}

void ChangeSysPwd(void)
{
	uint8_t buf[16], buf1[16];
	int iRet;

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "�޸Ĺ���Ա����  ", " CHG SYSTEM PWD ");
	DispMulLanguageString(0, 2, DISP_CFONT, "������ԭ����: ", "INPUT SYS PWD:");
	lcdGoto(64, 6);
	iRet = kbGetString(KB_EN_CRYPTDISP+KB_EN_BIGFONT+KB_EN_NUM+KB_EN_REVDISP, 8, 8, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==8 )
	{
		if( strcmp((char *)buf, (char *)stPosParam.szParamsPwd)!=0 )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "ԭ���벻��ȷ    ", "PWD ERROR!");
			ErrorBeep();
			kbGetKeyMs(1000);	
			return ;
		}
	}
	else
	{
		return ;
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "������������: ", "INPUT NEW PWD:");
	lcdGoto(64, 6);
	iRet = kbGetString(KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_REVDISP, 8, 8, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==8 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "������һ��������", "INPUT PWD AGAIN");
		lcdGoto(64, 6);
		iRet = kbGetString(KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_REVDISP, 8, 8, 
			(stPosParam.ucOprtLimitTime*1000), (char *)buf1);
		if( iRet==8 )
		{
			if( strcmp((char *)buf, (char *)buf1)==0 )
			{
				memset(stPosParam.szParamsPwd,0,sizeof(stPosParam.szParamsPwd));
				memcpy(stPosParam.szParamsPwd,buf1,8);
				SaveAppParam();
			}
			else
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "�����������벻��", "NOT THE SAME");
				ErrorBeep();
				kbGetKeyMs(3000);
				return ;
			}			
		}
		else
		{
			return ;
		}
	}
	else 
	{
		return ;
	}
	
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT, "�޸�����ɹ�", "NOT THE SAME");
	kbGetKeyMs(3000);

	return ;
}

void ChangeSafePwd(void)
{
	uint8_t buf[16], buf1[16];
	int iRet;

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "�޸İ�ȫ����    ", " CHG SYSTEM PWD ");
	DispMulLanguageString(0, 2, DISP_CFONT, "������ԭ����: ", "INPUT SYS PWD:");
	lcdGoto(64, 6);
	iRet = kbGetString(KB_EN_CRYPTDISP+KB_EN_BIGFONT+KB_EN_NUM+KB_EN_REVDISP, 8, 8, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==8 )
	{
		if( strcmp((char *)buf, (char *)stPosParam.szSafepwd)!=0 )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "ԭ���벻��ȷ    ", "PWD ERROR!");
			ErrorBeep();
			kbGetKeyMs(1000);	
			return ;
		}
	}
	else 
	{
		return ;
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "������������: ", "INPUT NEW PWD:");
	lcdGoto(64, 6);
	iRet = kbGetString(KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_REVDISP, 8, 8, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==8 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "������һ��������", "INPUT PWD AGAIN");
		lcdGoto(64, 6);
		iRet = kbGetString(KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_REVDISP, 8, 8, 
			(stPosParam.ucOprtLimitTime*1000), (char *)buf1);
		if( iRet==8 )
		{
			if( strcmp((char *)buf, (char *)buf1)==0 )
			{
				memcpy(stPosParam.szSafepwd,buf1,8);
				SaveAppParam();
			}
			else
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "�����������벻��", "NOT THE SAME");
				ErrorBeep();
				kbGetKeyMs(3000);
				return ;
			}			
		}
		else
		{
			return ;
		}
	}
	else 
	{
		return ;
	}
	
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT, "�޸�����ɹ�", "NOT THE SAME");
	kbGetKeyMs(3000);

	return ;
}
//˫��ѡ������ʽ
//�ڵ�����������ʾѡ���Ӧ����
//��ȡ������ʱ����ϵͳ�������
//ֱ�Ӱ�ȷ�Ͻ����¸�����
//������Ϸ�������°�ȷ�ϲŽ����¸�����ѡ��
uint32_t    SelectSetTwo(uint8_t *buf)
{
    int iRet;    
    while(1)
    {
        lcdClrLine(6,6);
    	lcdGoto(120, 6);
    	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP+KB_EN_PRESETSTR, 0, 1, (stPosParam.ucOprtLimitTime*1000), buf);
    	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
    	{
    		return -1;
    	}
        else if(iRet==0)
        {
            return 99;
        }
        else if((iRet==1)&&(strcmp("1",buf)>=0))
        {
            return atoi(buf);
        }
    }
}

