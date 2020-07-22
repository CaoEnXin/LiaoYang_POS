
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "FTP设置        ", "CDMA SETUP");

	memset(tmpbuf,0,sizeof(tmpbuf));
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "FTP服务器IP:", "SETUP HOST IP");
	lcdDisplay(0, 4, DISP_CFONT, (char *)(stPosParam.szFtpServIP));
	if( SetIpAddr(0, 6, (char*)tmpbuf)==0 ) 
	{
		strcpy((char*)stPosParam.szFtpServIP, (char*)tmpbuf);
		SaveAppParam();
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "FTP服务器端口", "SETUP HOST PORT");
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
	DispMulLanguageString(0, 2, DISP_CFONT, "FTP登录名:", "SETUP APN:");
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
	DispMulLanguageString(0, 2, DISP_CFONT, "FTP登录密码:", "SETUP APN:");
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
	DispMulLanguageString(0, 2, DISP_CFONT, "FTP文件路径:", "SETUP APN:");
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




// 设置系统参数
void SetupPosPara(void)
{
	int     iKey;
	/*const char *setupMenu[] = 
	{
		"系统管理        ",
		"1 终端参数设置  ",
		"2 通讯参数设置  ",
		"3 交易功能设置  ",
		"4 终端密钥管理  ",
		"5 修改密码      ",
		"6 其他功能      ",
		"7 FTP设置       "
	};*/
	
	const char *setupMenu[] = 
	{
		"系统管理        ",
		"1 终端参数设置  ",
		"2 交易功能设置  ",
		"3 修改密码      ",
		"4 其他功能      ",
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
		"交易功能设置    ",
		"1 屏蔽交易      ",
		"2 签退设置      ",
		"3 小费比例      ",
		"4 交易重发次数  ",
		"5 打印张数      ",
		"6 最大交易笔数  "
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
		"修改密码        ",
		"1 修改管理员密码",
		"2 修改安全密码  "
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
		"其他功能        ",
		"1 清除交易流水  ",
		"2 交易流程设置  ",
		"3 故障单设置    ",
		"4 参数打印      ",
		"5 机具测试      ",
		"6 状态设置      "
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
			PrintParam(1); // 打印全部参数
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "终端参数设置    ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "POS机编号:", "PLS INPUT MER NO");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "终端参数设置    ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "公司编号:", "PLS INPUT MER NO");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "终端参数设置    ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "部门编号:", "PLS INPUT MER NO");
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
	//-------操作员编号---
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "终端参数设置	  ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "操作员编号:", "PLS INPUT MER NO");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "终端参数设置    ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "终端编号:", "PLS INPUT MER NO");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "终端参数设置    ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "MAC密钥:", "PLS INPUT MER NO");
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
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "终端参数设置	  ", "SETUP PARA   ");
		DispMulLanguageString(0, 2, DISP_CFONT, "报文体密钥:", "PLS INPUT MER NO");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "终端参数设置    ", " SETUP TRACE.NO "); 
	DispMulLanguageString(0, 2, DISP_CFONT, "卡充值限额(元):", "INPUT TRACE.NO:");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "终端参数设置    ", " SETUP TRACE.NO "); 
	DispMulLanguageString(0, 2, DISP_CFONT, "当日充值限额(元):", "INPUT TRACE.NO:");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "终端参数设置    ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入商户编号:", "PLS INPUT MER NO");
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
		lcdDisplay(0, 2, DISP_CFONT, "请输入安全密码");
		lcdGoto(64, 6);
		iRet = kbGetString(KB_EN_CRYPTDISP+KB_EN_BIGFONT+KB_EN_NUM+KB_EN_REVDISP, 8, 8, 
			(stPosParam.ucOprtLimitTime*1000), (char *)szpwd);
		if( iRet==8 )
		{
			if( strcmp((char *)szpwd, (char *)stPosParam.szSafepwd)!=0 )
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "安全密码不正确  ", "PWD ERROR!");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "终端参数设置    ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入终端编号:", "PLS INPUT TEM NO");
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
		lcdDisplay(0, 2, DISP_CFONT, "请输入安全密码");
		lcdGoto(64, 6);
		iRet = kbGetString(KB_EN_CRYPTDISP+KB_EN_BIGFONT+KB_EN_NUM+KB_EN_REVDISP, 8, 8, 
			(stPosParam.ucOprtLimitTime*1000), (char *)szpwd);
		if( iRet==8 )
		{
			if( strcmp((char *)szpwd, (char *)stPosParam.szSafepwd)!=0 )
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "安全密码不正确  ", "PWD ERROR!");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "终端参数设置    ", "SETUP PARA   ");
	DispMulLanguageString(0, 2, DISP_CFONT, (char *)stPosParam.szUnitChnName, (char *)stPosParam.szUnitChnName);
	DispMulLanguageString(0, 6, DISP_CFONT, "更改?(确认/取消)", "CHG?ENTER/CANCEL");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "终端参数设置    ", "   SETUP DATE   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入当前年份", "YYYY");
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
			DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "日期格式错误", "DATE FORMAT ERR");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "终端参数设置    ", " SETUP TRACE.NO "); 
	DispMulLanguageString(0, 2, DISP_CFONT, "当前流水号:", "INPUT TRACE.NO:");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "终端参数设置    ", " SETUP BATCH NO ");
	DispMulLanguageString(0, 2, DISP_CFONT, "当前批次号:", "INPUT BATCH NO:");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "最大退货金额设置", " SETUP BATCH NO ");
	lcdDisplay(0, 2, DISP_CFONT, "请输最大退货金额");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "电子现金交易限额", " SETUP BATCH NO ");
	lcdDisplay(0, 2, DISP_CFONT, "请输最大现金金额");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "非接脱机最低限额", " SETUP BATCH NO ");
	lcdDisplay(0, 2, DISP_CFONT, "请输非接最低限额");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "非接最大交易限额", " SETUP BATCH NO ");
	lcdDisplay(0, 2, DISP_CFONT, "请输最大非接金额");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "结算打印明细设置", " SETUP BATCH NO ");
	lcdDisplay(0, 2, DISP_CFONT, "是否提示打印明细");
	lcdDisplay(0, 4, DISP_CFONT, "   0-否    1-是 ");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "商行代码设置    ", " MER BANK SETUP ");
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入商行代码:", "INPUT NUM1:");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "本地地区码      ", "LOCAL ID SETUP ");
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入本地地区码", "INPUT NUM1:");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "英文设置        ", " SETUP BATCH NO ");
	lcdDisplay(0, 2, DISP_CFONT, "签购单是否打英文");
	lcdDisplay(0, 4, DISP_CFONT, "   0-否    1-是 ");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "签购单抬头设置  ", " SETUP PRN TITLE ");
	lcdDisplay(0, 2, DISP_CFONT, "抬头内容选择");
	lcdDisplay(0, 4, DISP_CFONT, "0-输入    1-默认");
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
		DispMulLanguageString(0, 4, DISP_CFONT, "0--取消  1--确认", "0-CANCEL 1-ENTER");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "默认交易设置    ", " SETUP BATCH NO ");
	lcdDisplay(0, 2, DISP_CFONT, "1-消费");
	lcdDisplay(0, 4, DISP_CFONT, "0-预授权");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "密钥算法设置    ", " SETUP BATCH NO ");
	lcdDisplay(0, 2, DISP_CFONT, "1-单倍长");
	lcdDisplay(0, 4, DISP_CFONT, "0-双倍长");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "消费撤销是否刷卡", " SETUP BATCH NO ");
	lcdDisplay(0, 4, DISP_CFONT, "   0-否    1-是 ");
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
	lcdDisplay(0, 0, DISP_CFONT|DISP_REVERSE, "预授权完成撤销是");
	lcdDisplay(0, 2, DISP_CFONT|DISP_REVERSE, "否刷卡");
	lcdDisplay(0, 4, DISP_CFONT, "   0-否    1-是 ");
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
	lcdDisplay(0, 0, DISP_CFONT|DISP_REVERSE, "消费撤销是否输密");
	lcdDisplay(0, 2, DISP_CFONT|DISP_REVERSE, "码");
	lcdDisplay(0, 4, DISP_CFONT, "   0-否    1-是 ");
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
	lcdDisplay(0, 0, DISP_CFONT|DISP_REVERSE, "预授权撤销是否输");
	lcdDisplay(0, 2, DISP_CFONT|DISP_REVERSE, "密码");
	lcdDisplay(0, 4, DISP_CFONT, "   0-否    1-是 ");
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
	lcdDisplay(0, 0, DISP_CFONT|DISP_REVERSE, "预授权完成撤销是");
	lcdDisplay(0, 2, DISP_CFONT|DISP_REVERSE, "否输密码");
	lcdDisplay(0, 4, DISP_CFONT, "   0-否    1-是 ");
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
	lcdDisplay(0, 0, DISP_CFONT|DISP_REVERSE, "预授权完成请求是");
	lcdDisplay(0, 2, DISP_CFONT|DISP_REVERSE, "否输密码");
	lcdDisplay(0, 4, DISP_CFONT, "   0-否    1-是 ");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "通讯参数设置    ", "COMM SETUP");
	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "1.拨号   2.串口");
	lcdDisplay(0, 4, DISP_CFONT, "3.GPRS   4.CDMA");
	lcdDisplay(0, 6, DISP_CFONT, "5.以太网       ");
		
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
			lcdDisplay(0, 2, DISP_CFONT, "机器没有该模块  ");
			lcdDisplay(0, 4, DISP_CFONT, "重新选择通讯方式");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "通讯参数设置    ", "MANAGE PHONENUM ");
	DispMulLanguageString(0, 2, DISP_CFONT, "中心管理电话:", "INPUT MAG NUM:  ");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "通讯参数设置    ", "   SETUP TPDU   ");
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
	lcdDisplay(0, 2, DISP_CFONT, "是否预拨号");
	lcdDisplay(0, 4, DISP_CFONT, "0-否   1-是");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "通讯参数设置    ", "   SETUP TPDU   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "交易超时时间    ", "SETUP COMM TIME ");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "通讯参数设置    ", "   SETUP TPDU   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "交易重拨次数    ", "   REDAIL TIMES ");
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
	lcdDisplay(0, 0, DISP_CFONT|DISP_REVERSE, "清除交易流水    ");
	lcdDisplay(0, 2, DISP_CFONT, "清除交易流水");
	lcdDisplay(0, 4, DISP_CFONT, "否-取消  是-确认");
	iRet = kbGetKey();
	if( iRet==KEY_ENTER )
	{
		lCurIndex = 0;
		iCnt=0;
		memset(&gstYktEnrichLog, 0, YKTENRICH_LOG_SIZE);
		iTotalNum= ReadYtkEnrichLog(&lCurIndex);
		
		for(i=0; i<iTotalNum; i++){
			if(gstYktEnrichLog.uiStatus==1)
			{//已经结算
			   ;
			}
			else
			{
				iCnt = 1;
				lcdCls();
				lcdFlip();	
				lcdDisplay(0, 2, DISP_CFONT, "有未结算记录");
				lcdDisplay(0, 4, DISP_CFONT, "请先结算");
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

	key1 = OptionsSelect("    测试状态    ", stPosParam.ucTestFlag==PARAM_CLOSE,
		"当前状态:正常", "当前状态:测试", "[1].正常", "[2].测试", stPosParam.ucOprtLimitTime);
	if( (stTransCtrl.iTransNum!=0) && (key1==KEY1 || key1==KEY2) )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT, "    请先结帐    ", "  SETTLE FIRST  ");
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
	//以下为局部变量，以减少对环境的依赖
	unsigned short  usSendPacketLen;  
	unsigned short  usRecvPacketLen;
	uint8_t	sSendPacketBuff[MAX_PACKET_LEN];	
	uint8_t	sRecvPacketBuff[MAX_PACKET_LEN];
    
	if (ucMasterKeyID > 99)
	{
		ucMasterKeyID = 0;
	}
	
	PubDisplayTitle(TRUE, "装载密钥");
	if (pszMid == NULL)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "参数错误");
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "商户号不应为空");
		PubWaitKey(0);
	}
	if (pszTid == NULL)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "参数错误");
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "终端号不应为空");
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
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "发送数据..", 0);
	iKey = kbGetKeyMs(500);
	if(iKey == KEY_CANCEL)
	{
		DisplayMainPosOperFailed("操作取消", 5);
		return NO_DISP;
	}
	iRet = MainPosRS232Txd(sSendPacketBuff, usSendPacketLen, 60);
	if (iRet != OK)
	{
		DisplayMainPosOperFailed("发送失败", 5);
		return NO_DISP;
	}
	
	kbFlush();
	uiBeginTime = sysGetTimerCount();
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "接收数据(%d)", 0);
	while (Rs232CheckPurge(RS232_RX_PURGE) != ERR_RS232_OK)
	{
		// 一直等GenPOS下发数据,暂设180S
		uiCurTime = sysGetTimerCount();
		if (uiCurTime - uiBeginTime > 2*60*1000)//2分钟
		{
			DisplayMainPosOperFailed("接收数据超时", 5);
			return NO_DISP;
		}
		lcdDisplay(0, 3, DISP_CFONT|DISP_CLRLINE|DISP_MEDIACY, "接收数据(%d)", (uiCurTime - uiBeginTime)/1000);
		iRet = kbGetKeyMs(0);
		if(iRet == KEY_CANCEL)
		{
			DisplayMainPosOperFailed("操作取消", 5);
			return NO_DISP;
		}
	}

	memset(sRecvPacketBuff,0,sizeof(sRecvPacketBuff));
	usRecvPacketLen = 0;
	iRet = MainPosRS232Rxd(sRecvPacketBuff, MAX_PACKET_LEN, 60, (unsigned int*)&usRecvPacketLen);
	if (iRet != ERR_RS232_OK)
	{
		DisplayMainPosOperFailed("接收数据错", 5);
		return NO_DISP;
	}
	
	//返回的是错误码，显示错误信息并返回
//	if (memcmp(sRecvPacketBuff, "\x02\x40", 2) == 0)
//	{
//		DisplayMainPosErrMsg(sRecvPacketBuff[2]);
//		return NO_DISP;
//	}
	
	if (memcmp(sRecvPacketBuff, "\x02\x05\x01\x00", 4) != 0)
	{
		DisplayMainPosOperFailed("CMD不匹配", 5);
		return NO_DISP;
	}

	if (memcmp(&sRecvPacketBuff[5], "\x00\x00\x00\x00", 4) != 0)
	{
		DisplayMainPosErrMsg(sRecvPacketBuff[8]);
		return NO_DISP;
	}
	
	if (memcmp(&sRecvPacketBuff[9], pszMid, 15) != 0)
	{
		DisplayMainPosOperFailed("商户号不匹配", 5);
		return NO_DISP;
	}
	
	if (memcmp(&sRecvPacketBuff[24], pszTid, 8) != 0)
	{
		DisplayMainPosOperFailed("终端号不匹配", 5);
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
        DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  装主密钥错误  ", " LOAD KEY ERROR ");
        lcdDisplay(0, 6, DISP_CFONT, "iRet=%d", iRet);
        ErrorBeep();
        kbGetKeyMs(3000);
        return 3;
    }
	stPosParam.ucKeyIndex = ucMasterKeyID;
	SaveAppParam();	
	//装载成功后发送成功信息过去母POS
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
		//发送不成功也什么都不做
		//DisplayMainPosOperFailed("发送失败", 5);
		//return NO_DISP;
	}
	Rs232Close();
/*
	lcdCls();
	PubDisplayTitle(TRUE, "测试加密结果");
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
		"终端密钥管理    ",
		"1 导入密钥      ",
		"2 手工输入密钥  ",
        "3 母POS导入密钥 ",
		"4 交易密钥索引号"
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
			DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "手工输入密钥    ", " INPUT KEY DATA ");
			DispMulLanguageString(0, 2, DISP_CFONT, "主密钥索引号:", "INPUT KEY INDEX:");
			DispMulLanguageString(0, 4, DISP_CFONT, "输入:(00--99)", "INPUT:(00--99)");
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
					DispMulLanguageString(0, 6, DISP_CFONT, "密钥号输入错误", "KEY INDEX ERROR");
					kbGetKeyMs(1000);
					break;
				}
				ucTmpKeyId = iTmpInt+1;
			}
			else 
				break;
			lcdCls();
			lcdFlip();	
			DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "手工输入密钥    ", " INPUT KEY DATA ");
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 2, DISP_CFONT, "请输入主密钥  ", "INPUT KEY DATA:");

			lcdGoto(0, 4);
			memset(buf,0,sizeof(buf));
			iRet = kbGetString(0x175, ucLen, ucLen, 
				(stPosParam.ucOprtLimitTime*1000), (char *)buf);
			if( iRet!=ucLen )
				break;

			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 2, DISP_CFONT, "再输入主密钥  ", "INPUT KEY DATA:");

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
				lcdDisplay(0, 2, DISP_CFONT, "两次输入不一致");
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
				DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  装主密钥错误  ", " LOAD KEY ERROR ");
				lcdDisplay(0, 6, DISP_CFONT, "iRet=%d", iRet);
				ErrorBeep();
				kbGetKeyMs(3000);
			}
			break;
			
		case KEY3:
            lcdCls();
            PubDisplayTitle(TRUE, "装载密钥");
            DispMulLanguageString(0, 2, DISP_CFONT, "主密钥索引号:", "INPUT KEY INDEX:");
            DispMulLanguageString(0, 4, DISP_CFONT, "输入:(00--99)", "INPUT:(00--99)");
            lcdGoto(112, 6);
            memset(buf,0,sizeof(buf));
            iRet = kbGetString(KB_EN_REVDISP+KB_EN_NUM+KB_EN_BIGFONT, 1, 2, (stPosParam.ucOprtLimitTime*1000), (char*)buf);    
            if( iRet>0 )
            {
                iTmpInt = atoi((char *)buf);
                if( iTmpInt>99 || iTmpInt<0 )
                {
                    lcdClrLine(6, 7);
                    DispMulLanguageString(0, 6, DISP_CFONT, "密钥号输入错误", "KEY INDEX ERROR");
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
                lcdDisplay(0, 3, DISP_CFONT, "密钥装载成功");
                PubWaitKey(0);
            }
            break;

		case KEY4:
			lcdCls();
			lcdFlip();	
			DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "交易密钥索引号  ", "   KEY INDEX    ");
			iTmpInt = stPosParam.ucKeyIndex;
			sprintf((char *)buf, "密钥索引号:%02d", iTmpInt);
			lcdDisplay(0, 2, DISP_CFONT, (char *)buf);
			DispMulLanguageString(0, 4, DISP_CFONT, "输入:(00--99)", "INPUT:(00--99)  ");
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
					DispMulLanguageString(0, 6, DISP_CFONT, "密钥号输入错误", "KEY INDEX ERROR");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "交易重发次数    ", "SETUP RETRY TIME");
	iTmpInt = stPosParam.ucResendTimes;
	memset(buf, 0, sizeof(buf));
	sprintf((char *)buf, "重发次数:%d", iTmpInt);
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
		ucKey = OptionsSelect(" 操作员判断IC卡 ", stPosParam.ucSelectCardType==PARAM_OPEN,
			"当前设置:是", "当前设置:否", "[1].否", "[2].是", stPosParam.ucOprtLimitTime);
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "手输卡号设置    ", "SUPPORT EMV TRAN");
	lcdDisplay(0, 2, DISP_CFONT, "是否支持手输卡号");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "授权完成实现方式", "  PRE-CMP MODE  ");

	DispMulLanguageString(0, 2, DISP_CFONT, "0 同时支持      ", "0 BOTH          ");
	DispMulLanguageString(0, 4, DISP_CFONT, "1 预授权完成请求", "1 PRE-CMP ONLINE");
	DispMulLanguageString(0, 6, DISP_CFONT, "2 预授权完成通知", "2 PRECMP OFFLINE");
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
		MaskTrans(POS_AUTH_CM, 4, "预授权完成(请求)", KEY2);
		MaskTrans(POS_OFF_CONFIRM, 11, "预授权完成(通知)", KEY2);
	}
	else if( stPosParam.ucConfirm=='1' )
	{
		MaskTrans(POS_AUTH_CM, 4, "预授权完成(请求)", KEY2);
		MaskTrans(POS_OFF_CONFIRM, 11, "预授权完成(通知)", KEY1);
	}
	else if( stPosParam.ucConfirm=='2' )
	{
		MaskTrans(POS_AUTH_CM, 4, "预授权完成(请求)", KEY1);
		MaskTrans(POS_OFF_CONFIRM, 11, "预授权完成(通知)", KEY2);
	}
	
/*	ucKey = OptionsSelect("    离    线    ", stPosParam.ucSendOffFlag==PARAM_OPEN,
		"状态:立即上送", "状态:批结上送", "[1].立即上送  ", "[2].批结上送", stPosParam.ucOprtLimitTime);
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "小费交易        ", "SUPPORT EMV TRAN");
	lcdDisplay(0, 2, DISP_CFONT, "是否支持小费");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "是否自动签退    ", "SUPPORT EMV TRAN");
	lcdDisplay(0, 2, DISP_CFONT, "是否自动签退");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "签退设置        ", "SUPPORT EMV TRAN");
	lcdDisplay(0, 2, DISP_CFONT, "结算后自动签退");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "小费比例设置    ", "SUPPORT EMV TRAN");

	sprintf((char *)dispbuf, "小费百分比(%d%%)", stPosParam.ucTipper);
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "屏蔽交易        ", "SUPPORT EMV TRAN");
	lcdDisplay(0, 2, DISP_CFONT, "撤销");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "消费");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "查询");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "预授权");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "离线");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "结算调整");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "退货");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "预授权撤销");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "预授权完成请求");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "预授权完成撤销");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "电子钱包圈存");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "预授权完成通知");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "脚本处理结果通知");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "电子现金脱机消费");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "电子钱包消费");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "分期付款消费");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "分期付款消费撤销");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "积分消费");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "积分消费撤销");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "借贷记圈存类交易");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "预约消费");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "预约消费撤销");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "订购消费");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "订购消费撤销");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	lcdDisplay(0, 2, DISP_CFONT, "磁条卡充值");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "屏蔽交易        ", "SUPPORT EMV TRAN");
	lcdDisplay(0, 2, DISP_CFONT, "小额代授权的结算");
	lcdDisplay(0, 4, DISP_CFONT, "0-不支持  1-支持");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "最大交易笔数设置", "  TRAN MAX NUM  ");
	sprintf((char *)buf, "最大交易笔数:%d", stPosParam.iMaxTransTotal);

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
			DispMulLanguageString(0, 6, DISP_CFONT, "    笔数错误    ", "   NUM ERROR   ");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "自动上送累计笔数", "  TRAN MAX NUM  ");
	sprintf((char *)buf, "上送最大笔数:%d", stPosParam.ucMaxOfflineTxn);
	
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
			DispMulLanguageString(0, 6, DISP_CFONT, "    笔数错误    ", "   NUM ERROR   ");
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
	if( OneTwoSelect("打印测试", "打印", "取消")!=KEY1 )
		return;
	
	prnInit();
	PrnNormalFontData("\n\n\n\n");
	prnPrintf("打印测试\n");
	prnPrintf("国国国国国\n");
	prnPrintf("国国国国国\n");
	prnPrintf("国国国国国\n");
	prnPrintf("国国国国国\n");
	prnPrintf("国国国国国\n");
	prnPrintf("国国国国国\n");
	prnPrintf("国国国国国\n");
	prnPrintf("国国国国国\n");
	prnPrintf("国国国国国\n");
	prnPrintf("国国国国国\n");
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
		ucKey = OptionsSelect(pFuncName, CheckSupport(iTransId), "当前设置:支持", "当前设置:不支持", "[1].不支持" , 
			"[2].支持", stPosParam.ucOprtLimitTime);
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  设置显示语言  ", " SETUP LANGUAGE ");
	DispMulLanguageString(0, 2, DISP_CFONT, "[1]:中文", "[1]:CHINESE");
	DispMulLanguageString(0, 4, DISP_CFONT, "[2]:英文", "[2]:ENGLISH");
	
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "打印张数设置    ", "SETUP PRINT NUMS");
	tmpInt = stPosParam.ucTicketNum - '0';
	sprintf((char *)buf, "打印张数: %d", tmpInt);
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

	key = OptionsSelect(" 故障单打印设置 ", stPosParam.ucErrorPrt==PARAM_OPEN,
		"当前设置:打印", "当前设置:不打印",	"[1].不打印", "[2].打印", stPosParam.ucOprtLimitTime);

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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    参数打印    ", "   PIRNT PARA   ");
	DispMulLanguageString(0, 4, DISP_CFONT, "请稍候... ", "WAITTING...");
	prnInit();
	PrnBigFontChnData("本机银联参数信息\n");
	memset(buf, 0, sizeof(buf));
	sprintf((char *)buf, "商户名称:%s\n", stPosParam.szUnitChnName);
	PrnSmallFontData((char *)buf);
	prnPrintf("商户编号: %s\n", stPosParam.szUnitNum);
	prnPrintf("终端编号: %s\n", stPosParam.szPosId);
	prnPrintf("重拨次数: %d\n", stPosParam.ucDialRetryTimes);
	BcdToAsc(buf, stPosParam.sTpdu, 10);
	buf[10] = 0x00;
	prnPrintf("TPDU设置: %s\n", buf);
	prnPrintf("交易重发次数: %d\n", stPosParam.ucResendTimes);
	i = stPosParam.ucTicketNum - '0';
	prnPrintf("打印凭单数: %d\n", i);

	if( stPosParam.ucErrorPrt==PARAM_OPEN )
	{
		prnPrintf("故障单: 打印\n");
	}
	else
	{
		prnPrintf("故障单: 不打印\n");
	}
	if( CheckSupport(POS_PREAUTH)==TRUE )
	{
		prnPrintf("预授权类交易: 支持\n");
	}
	else
	{
		prnPrintf("预授权类交易: 不支持\n");
	}

	if( CheckSupport(POS_REFUND)==TRUE )
	{
		prnPrintf("退货交易: 支持\n");
	}
	else
	{
		prnPrintf("退货交易: 不支持\n");
	}

	prnPrintf("通讯超时时间: %d秒\n", stPosParam.ucCommWaitTime);
	prnPrintf("批次号:    %ld\n", stPosParam.lNowBatchNum);
	prnPrintf("凭证号:    %ld\n", stPosParam.lNowTraceNo);
	prnPrintf("最大交易笔数: %d笔\n", stPosParam.iMaxTransTotal);
	prnPrintf("自动上送笔数: %d笔\n", stPosParam.ucMaxOfflineTxn);
	prnPrintf("操作超时时间: %d秒\n", stPosParam.ucOprtLimitTime);
	if( stPosParam.ucManualInput==PARAM_OPEN )
	{
		prnPrintf("手输卡号: 支持\n");
	}
	else
	{
		prnPrintf("手输卡号: 不支持\n");
	}
	if( stPosParam.ucVoidSwipe==PARAM_OPEN )
	{
		prnPrintf("撤销类交易是否刷卡: 是\n");
	}
	else
	{
		prnPrintf("撤销类交易是否刷卡: 否\n");
	}
	if( stPosParam.ucVoidPin==PARAM_OPEN )
	{
		prnPrintf("消费撤销交易是否输密码: 是\n");
	}
	else
	{
		prnPrintf("消费撤销交易是否输密码: 否\n");
	}
	if( stPosParam.ucPreVoidPin==PARAM_OPEN )
	{
		prnPrintf("预授权撤销交易是否输密码: 是\n");
	}
	else
	{
		prnPrintf("预授权撤销交易是否输密码: 否\n");
	}
	if( stPosParam.ucPreComVoidPin==PARAM_OPEN )
	{
		prnPrintf("预授权完成撤销交易是否输密码: 是\n");
	}
	else
	{
		prnPrintf("预授权完成撤销交易是否输密码: 否\n");
	}
	if( stPosParam.ucPreComPin==PARAM_OPEN )
	{
		prnPrintf("预授权完成请求交易是否输密码: 是\n");
	}
	else
	{
		prnPrintf("预授权完成请求交易是否输密码: 否\n");
	}

/*	if( stPosParam.ucSendOffFlag==PARAM_OPEN )
		prnPrintf("离线上送方式: 立即上送\n");
	else 
		prnPrintf("离线上送方式: 批结上送\n"); */
	if( stPosParam.ucAutoLogoff==PARAM_OPEN )
	{
		prnPrintf("是否自动签退: 是\n");
	}
	else
	{
		prnPrintf("是否自动签退: 否\n");
	}

	i = stPosParam.ucKeyIndex;
	prnPrintf("主密钥索引: %d\n", i);
/*	if( stPosParam.ucSelectCardType==PARAM_OPEN )
	{
		prnPrintf("操作员判断IC卡: 是\n");
	}
	else
	{
		prnPrintf("操作员判断IC卡: 否\n");
	}*/

	prnPrintf("\n\n\n\n\n");
	PrintData();

	if( stPosParam.ucEmvSupport==PARAM_OPEN )
		PrnEmvParam();	// 打印EMV参数
}

uint8_t CheckParamOpen(char * item, char condition)
{
	if( condition ) 
	{
		prnPrintf("%s:开\n", item);
	}
	else 
	{
		prnPrintf("%s:关\n", item);
	}
	return OK;
}

void DeviceTest()
{
	int ikey;
	const char * test_main[] = 
	{
		"    机具测试    ",
		"1 打印机        ",
		"2 显示器        ",
		"3 读卡器        ",
		"4 键盘          ",
		"5 回响测试      "
	};

	while(1) 
	{
		ikey = DispMenu(test_main, 6, 1, KEY1, KEY5, stPosParam.ucOprtLimitTime);
		switch(ikey) {
		case KEY1:
			TestPrint();
			prnInit(); //防止参数打印中,别的参数再打印出来
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "修改管理员密码  ", " CHG SYSTEM PWD ");
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入原密码: ", "INPUT SYS PWD:");
	lcdGoto(64, 6);
	iRet = kbGetString(KB_EN_CRYPTDISP+KB_EN_BIGFONT+KB_EN_NUM+KB_EN_REVDISP, 8, 8, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==8 )
	{
		if( strcmp((char *)buf, (char *)stPosParam.szParamsPwd)!=0 )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "原密码不正确    ", "PWD ERROR!");
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
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入新密码: ", "INPUT NEW PWD:");
	lcdGoto(64, 6);
	iRet = kbGetString(KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_REVDISP, 8, 8, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==8 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "请再输一遍新密码", "INPUT PWD AGAIN");
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
				DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "两次输入密码不符", "NOT THE SAME");
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
	DispMulLanguageString(0, 4, DISP_CFONT, "修改密码成功", "NOT THE SAME");
	kbGetKeyMs(3000);

	return ;
}

void ChangeSafePwd(void)
{
	uint8_t buf[16], buf1[16];
	int iRet;

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "修改安全密码    ", " CHG SYSTEM PWD ");
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入原密码: ", "INPUT SYS PWD:");
	lcdGoto(64, 6);
	iRet = kbGetString(KB_EN_CRYPTDISP+KB_EN_BIGFONT+KB_EN_NUM+KB_EN_REVDISP, 8, 8, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==8 )
	{
		if( strcmp((char *)buf, (char *)stPosParam.szSafepwd)!=0 )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "原密码不正确    ", "PWD ERROR!");
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
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入新密码: ", "INPUT NEW PWD:");
	lcdGoto(64, 6);
	iRet = kbGetString(KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_REVDISP, 8, 8, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet==8 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "请再输一遍新密码", "INPUT PWD AGAIN");
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
				DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "两次输入密码不符", "NOT THE SAME");
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
	DispMulLanguageString(0, 4, DISP_CFONT, "修改密码成功", "NOT THE SAME");
	kbGetKeyMs(3000);

	return ;
}
//双向选择题解答方式
//在第四行输入显示选择对应数字
//按取消、超时返回系统管理界面
//直接按确认进入下个界面
//在输入合法的情况下按确认才进入下个设置选项
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

