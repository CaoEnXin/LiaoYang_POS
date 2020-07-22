
#include "posapi.h"
#include "posmacro.h"
#include "terstruc.h"
#include "glbvar.h"
#include "tranfunclist.h"


uint8_t TellerLogon(uint8_t flag)
{
	uint8_t  buf[20], buf1[20], nowOper[4];
	int      j;
	int      iRet;
	
	lcdCls();
	lcdFlip();
	if( flag==1 )
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    签    到    ", "     LOGON      ");
	}
	else if( flag==0 )
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    终端解锁    ", "  POS UNLOCK    ");
	}
	DispMulLanguageString(0, 2, DISP_CFONT, "操作员号:  ", "OPER NO.");
	kbFlush();
	lcdGoto(80, 2);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP+KB_EN_SHIFTLEFT, 2, 2, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf1);
	if( iRet!=2 ) 
	{
		return NO_DISP;
	}
	sprintf((char *)nowOper, "0%2s", buf1);
	if( strcmp((char *)nowOper, operStrc[0].szTellerNo)==0 ) //主管功能
	{
		SupTellerFunc();
		return NO_DISP;
	}
	if( strcmp((char *)nowOper, SYS_TELLER_NO)==0 ) //系统管理员功能
	{
		SetParaFunc();
		return NO_DISP;
	}

	if( flag==0 && strcmp((char *)nowOper, operStrc[0].szTellerNo)!=0x00 ) 
	{
		if( strcmp((char *)nowOper, (char *)stTransCtrl.szOldTellerNo)!=0x00 )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  操作员号不符  ", " OPER.NO DIFFER ");
			ErrorBeep();
			kbGetKeyMs(1000);
			return NO_DISP;
		}
	}
	

	for(j=0; j<MAX_OPER_NUM; j++) 
	{
		if( strcmp((char *)nowOper, operStrc[j].szTellerNo)==0x00 ) 
		{
			break;
		}
	}
	if( j==MAX_OPER_NUM )
	{ 
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "   无此操作员   ", "   OPER EXIST   ");
		ErrorBeep();
		kbGetKeyMs(1000);
		return NO_DISP;
	}
	
//	lcdCls();
	lcdClrLine(2, 7);
	lcdFlip();
	if( flag==1 )
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    签    到    ", "     LOGON      ");
	if( flag==0 ) 
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    终端解锁    ", "   POS UNLOCK   ");
		lcdSetIcon (ICON_LOCK, OPENICON);
	}
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入密码    ", "INPUT PWD:");
	lcdGoto(96, 6);
	lcdGoto(96, 6);
	iRet = kbGetString(KB_EN_CRYPTDISP+KB_EN_BIGFONT+KB_EN_REVDISP+KB_EN_NUM, 4, 4, (stPosParam.ucOprtLimitTime*1000), buf);
	if( iRet!=4 )
	{
		return NO_DISP;
	}
	
	if( strcmp((char *)buf, operStrc[j].szPassword) ) 
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "    密码错!!    ", "  PWD ERROR!  ");
		ErrorBeep();
		kbGetKeyMs(1000);
		return NO_DISP;
	}
	strcpy((char *)stTransCtrl.szNowTellerNo, (char *)nowOper);
	strcpy((char *)stTransCtrl.szNowTellerPwd, (char *)operStrc[j].szPassword);
	
	if( flag==0 )
	{
		lcdSetIcon(ICON_LOCK, CLOSEICON);	
	}
	return OK;
}


void TellerAdd()
{
	int  j;
	int  iRet;
	uint8_t  buf[20], buf1[20], buf2[20], buf3[20];
	uint8_t  szDisp[17];

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_REVERSE|DISP_CFONT, "增加操作员      ", "    ADD OPER    ");
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT, "请输入主管密码: ", "INPUT MANAGE PWD");
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 6, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet!=6 )
		return ;

	if( strcmp((char *)buf, operStrc[0].szPassword)!=0x00 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "   主管密码错   ", "PWD ERROR");
		ErrorBeep();
		kbGetKeyMs(1000);
		return ;
	}
		
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_REVERSE|DISP_CFONT, "增加操作员      ", "    ADD OPER    ");
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入操作员编号", "OPER NO:");
	kbFlush();
	lcdGoto(0, 4);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 2, 2, (stPosParam.ucOprtLimitTime*1000), (char *)buf1);
	if( iRet!=2 ) 
	{
		return ;
	}
	sprintf((char *)buf, "0%2s", buf1);

	if( strcmp(operStrc[0].szTellerNo, (char *)buf)==0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " 不能与主管相同 ", "SAME WITH MANAGE");
		ErrorBeep();
		kbGetKeyMs(1000);

		return;
	}
	if( strcmp(SYS_TELLER_NO, (char *)buf)==0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "不能与管理员相同", "SAME WITH SYSTEM");
		ErrorBeep();
		kbGetKeyMs(1000);

		return;
	}

	for(j=1; j<MAX_OPER_NUM; j++) 
	{
		if( strcmp(operStrc[j].szTellerNo, (char *)buf)==0 )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " 该操作员已存在 ", "SAME WITH OPER");
			ErrorBeep();
			kbGetKeyMs(1000);
			return;
		}
	}

	sprintf((char *)szDisp, "操作员编号: %.2s", buf1);
	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, (char *)szDisp);
	DispMulLanguageString(0, 4, DISP_CFONT, "请输入操作员密码", "INPUT PWD:");
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 4, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf2);
	if( iRet!=4 )
		return ;

	sprintf((char *)szDisp, "操作员编号: %.2s", buf1);
	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, (char *)szDisp);
	DispMulLanguageString(0, 4, DISP_CFONT, "请再输一次密码", "INPUT PWD:");
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 4, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf3);
	if( iRet!=4 )
		return;

	if( memcmp(buf2, buf3, 4)!=0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT, "两次输入操作员", "NOT THE SAME");
		DispMulLanguageString(0, 6, DISP_CFONT, "密码不一致", "NOT THE SAME");
		ErrorBeep();
		kbGetKeyMs(1000);
		return;
	}

	for(j=1; j<MAX_OPER_NUM; j++) 
	{
		if( strcmp(operStrc[j].szTellerNo, TELLER_NOBODY)==0 ) 
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT, "操作员增加成功  ", "  ADD SUCC!  ");
			OkBeep();
			strcpy(operStrc[j].szTellerNo, (char *)buf);
			strcpy(operStrc[j].szPassword, (char *)buf2);
			kbGetKeyMs(3000);
			WriteOperFile();
			return;
		}
	}

	if( j==MAX_OPER_NUM ) 
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  操作员数已满  ", "TOO MANY OPER ");
		ErrorBeep();
		kbGetKeyMs(1000);

		return;
	}

	return;	
}

void TellerDel()
{
	int j;
	int iRet;
	uint8_t buf[20], buf1[20];
	uint8_t szDisp[17];

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_REVERSE|DISP_CFONT, "删除操作员      ", "    DEL OPER    ");
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT, "请输入主管密码: ", "INPUT MANAGE PWD");
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 6, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet!=6 )
		return ;

	if( strcmp((char *)buf, operStrc[0].szPassword)!=0x00 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "   主管密码错   ", "PWD ERROR");
		ErrorBeep();
		kbGetKeyMs(1000);
		return ;
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_REVERSE|DISP_CFONT, "   删除操作员   ", "  DELETE OPER   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入操作员编号", "OPER NO:");

	kbFlush();
	lcdGoto(0, 4);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 2, 2, (stPosParam.ucOprtLimitTime*1000), (char *)buf1);
	if( iRet!=2 ) 
	{
		return;
	}
	sprintf((char *)buf, "0%.2s", buf1);
	if( strcmp(SYS_TELLER_NO, (char *)buf)==0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "不能与管理员相同", "SAME WITH SYSTEM");
		ErrorBeep();
		kbGetKeyMs(1000);

		return;
	}
	if( strcmp((char *)stTransCtrl.szNowTellerNo, (char *)buf)==0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " 操作员已经签到 ", " LOGONED ");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "    不能删除    ", "DELETE FORBID");
		ErrorBeep();
		kbGetKeyMs(1000);
		return;
	}
	for(j=1; j<MAX_OPER_NUM; j++) 
	{
		if( strcmp(operStrc[j].szTellerNo, (char *)buf)==0 ) 
		{
			lcdCls();
			lcdFlip();	
			DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_REVERSE|DISP_CFONT, "删除操作员      ", "    DEL OPER    ");
			sprintf((char *)szDisp, "操作员编号: %.2s", buf1);
			lcdDisplay(0, 2, DISP_CFONT, (char *)szDisp);
			DispMulLanguageString(0, 4, DISP_CFONT, "是否删除?", "DELETE?");
			DispMulLanguageString(0, 6, DISP_CFONT, "是-确认 否-取消", "ENTER CANCEL");
			iRet = kbGetKeyMs(30000);
			if( iRet==KEY_CANCEL )
				return;
			strcpy(operStrc[j].szTellerNo, TELLER_NOBODY);
			strcpy((char *)operStrc[j].szPassword, "0000");
			WriteOperFile();
			break;
		}
	}

	if( j==MAX_OPER_NUM ) 
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "   无此操作员   ", " OPER EXIST ");
		ErrorBeep();
		kbGetKeyMs(1000);

		return;
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT, "    删除成功    ", "DELETE SUCC");
	OkBeep();
	kbGetKeyMs(3000);
	return;

}

void TellerShow()
{
	int  j, operNum=0;
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_REVERSE|DISP_CFONT, "查询操作员      ", " VIEW OPER INFO ");
	lcdGoto(0, 2);

	for(j=1; j<MAX_OPER_NUM; j++) 
	{
		if( strcmp(operStrc[j].szTellerNo, TELLER_NOBODY)==0 ) 
		{
			continue;
		}
		lcdPrintf(" %.2s ", operStrc[j].szTellerNo+1);  //都显示2位
		operNum++;
		if( operNum%4==0 ) 
			lcdPrintf("\n");
		
		if( operNum%12==0 )
		{
			DispMulLanguageString(0, 6, DISP_CFONT, "按任意键查看更多", "MORE PRESS KEY");
			kbGetKey();
			lcdClrLine(2, 7);
			lcdFlip();	
			lcdGoto(0, 2);
		}
	}
	
	kbGetKey();
	return;
}

void TellerManageMenu(void)
{
	int ikey;	
	const char *teller_man_main[] = 
	{
		"柜员管理        ",
		"1 主管改密      ",
		"2 增加    3 删除",
		"4查询5操作员改密"
	};

	while(1) 
	{
		ikey = DispMenu(teller_man_main, 4, 1, KEY1, KEY5, stPosParam.ucOprtLimitTime);
		switch(ikey) 
		{
		case KEY1:
			SupChgPwd();
			break;
		case KEY2:
			TellerAdd();
			break;
		case KEY3:
			TellerDel();
			break;
		case KEY4:
			TellerShow();
			break;
		case KEY5:
			TellerChgPwd();
			break;

		default:
			return ;
			break;
		}
	}//while
	
	return ;
}

void TellerChgPwd()
{
	
	int     j;
	uint8_t buf[20], buf1[20], nowOper[4];	
	int iRet;
	uint8_t szDisp[17];
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_REVERSE|DISP_CFONT, "修改操作员密码  ", "OPER CHANGE PWD ");
	sprintf((char *)szDisp, "操作员编号: %.2s", &stTransCtrl.szNowTellerNo[1]);
	lcdDisplay(0, 2, DISP_CFONT, (char *)szDisp);
	DispMulLanguageString(0, 6, DISP_CFONT, "是-确认 否-取消", "ENTER CANCEL");
	iRet = kbGetKeyMs(30000);
	if( iRet==KEY_CANCEL )
		return;
	

	sprintf((char *)nowOper, "0%2s", &stTransCtrl.szNowTellerNo[1]);
	for(j=1; j<MAX_OPER_NUM; j++) 
	{
		if( strcmp((char *)nowOper, operStrc[j].szTellerNo)==0x00 ) 
		{
			break;
		}
	}
	if( j==MAX_OPER_NUM ) 
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "   无此操作员   ", " OPER EXIST ");
		ErrorBeep();
		kbGetKeyMs(1000);
		return;
	}
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入原密码    ", "ORIGINAL PWD:");
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 4, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet!=4 )
		return;
	
	if( strcmp((char *)buf, operStrc[j].szPassword) ) 
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "原密码不正确    ", "   PWD ERROR! ");
		ErrorBeep();
		kbGetKeyMs(1000);

		return;
	}
	
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入新密码    ", "INPUT NEW PWD:");
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 4, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet!=4 )
		return ;
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "请再输一遍新密码", "INPUT AGAIN:");
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 4, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf1);
	if( iRet!=4 )
		return;
	if( strcmp((char *)buf, (char *)buf1) )  
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "两次输入密码不符", "NOT THE SAME");
		ErrorBeep();
		kbGetKeyMs(1000);
		return;
	}
	strcpy((char *)operStrc[j].szPassword ,(char *)buf);
	WriteOperFile();
	if( strcmp((char *)stTransCtrl.szNowTellerNo, (char *)nowOper)==0 )
	{
		strcpy((char *)stTransCtrl.szNowTellerPwd, (char *)buf);
		SaveCtrlParam();
	}
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT, "修改密码成功    ", "  CHANGED SUCC! ");
	OkBeep();
	kbGetKeyMs(3000);
	return;
	
}

void TellerRunLock()
{
	uint8_t buf[20], pass[20], nowOper[4];
	int iRet;

	kbFlush();
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_REVERSE|DISP_CFONT, "终端解锁        ", "LOCK THE SCREEN ");
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入操作员号", "PRESS [ENTER]");
	lcdGoto(0, 4);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 2, 2, (stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet!=2 ) 
	{
		return ;
	}

	memset(nowOper, 0, sizeof(nowOper));
	sprintf((char *)nowOper, "0%2s", buf);
	if( strcmp((char *)nowOper, operStrc[0].szTellerNo)==0 ) //主管功能
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "请输入主管密码", "INPUT PWD:");
		lcdGoto(0, 6);
		iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 6, 6, 
			(stPosParam.ucOprtLimitTime*1000), (char *)pass);
		if( iRet!=6 )
			return ;

		if( (memcmp((char *)pass, operStrc[0].szPassword, 6)!=0x00) ) 
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "    密码不符    ", "PWD ERROR");
			ErrorBeep();
			kbGetKeyMs(1000);
			return;
		}
	}
	else if( strcmp((char *)nowOper, (char *)stTransCtrl.szOldTellerNo)==0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "请输入操作员密码", "INPUT PWD:");
		lcdGoto(0, 4);
		iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 4, 4, 
			(stPosParam.ucOprtLimitTime*1000), (char *)pass);
		if( iRet!=4 )
			return ;

		if( (strcmp((char *)pass, (char *)stTransCtrl.szNowTellerPwd)!=0x00) ) 
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "    密码不符    ", "PWD ERROR");
			ErrorBeep();
			kbGetKeyMs(1000);

			return;
		}

	}
	else
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "非当前操作员    ", "PWD ERROR");
		ErrorBeep();
		kbGetKeyMs(1000);
		return ;

	}


	if( (strcmp((char *)nowOper, operStrc[0].szTellerNo)==0) )
	{
		strcpy((char *)stTransCtrl.szNowTellerNo, TELLER_NOBODY);
		SaveCtrlParam();		
	}
	else
	{
		strcpy((char *)stTransCtrl.szNowTellerNo, (char *)stTransCtrl.szOldTellerNo);
		SaveCtrlParam();
		lcdSetIcon(ICON_LOCK, CLOSEICON);	
	}
	
	return;
}

uint8_t CheckSupPwd(void)
{
	char  buf[20];
	int iRet;

	while(1)
	{	
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "请输入主管密码: ", "INPUT MANAGE PWD");
		lcdGoto(80, 6);  
		iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_REVDISP, 6, 6, 
			(stPosParam.ucOprtLimitTime*1000), (char *)buf);
		if( iRet!=6 )
			return NO_DISP;

		if( strcmp(buf, operStrc[0].szPassword)!=0x00 )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "   主管密码错   ", "PWD ERROR");
			ErrorBeep();
			kbGetKeyMs(1000);
			continue;
		}
		return OK;
	}	
}

uint8_t CheckTellerPwd()
{
	char  buf[20];
	int iRet;
	
	while(1)
	{		
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "请输入操作员密码", "INPUT PWD:");
		lcdGoto(96, 6);
		iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 4, 4, 
			(stPosParam.ucOprtLimitTime*1000), (char *)buf);
		if( iRet!=4 )
			return NO_DISP;
		
		if( strcmp((char *)buf, (char *)stTransCtrl.szNowTellerPwd)!=0x00 )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "     密码错     ", "PWD ERROR");
			ErrorBeep();
			kbGetKeyMs(1000);
			continue;
		}
		return OK;
	}
}


void SupTellerFunc(void)
{
	uint8_t ret;
	int ikey;
	
	const char * teller_man_main[] = 
	{
		"  主管管理功能  ",
		"1 增加    2 删除",
		"3 查询    4 改密"
	};


	ret = CheckSupPwd();
	if( ret )
		return;
	
	while(1) 
	{
		ikey = DispMenu(teller_man_main, 3, 1, KEY1, KEY4, stPosParam.ucOprtLimitTime);
		switch(ikey) 
		{
		case KEY1:
			TellerAdd();
			break;
		case KEY2:
			TellerDel();
			break;
		case KEY3:
			TellerShow();
			break;
		case KEY4:
			SupChgPwd();
			break;

		default:
			return ;
			break;
		}
	}//while
	
	return ;
}

void SupChgPwd()
{
	uint8_t  buf[20], tmp[20];
	int  iRet;
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "修改主管改密    ", " CHANGE MAG PWD ");
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入原密码: ", "INPUT MANAGE PWD");
	lcdGoto(0, 4);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 6, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet!=6 )
		return ;

	if( strcmp((char *)buf, operStrc[0].szPassword)!=0x00 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "原密码不正确    ", "PWD ERROR");
		ErrorBeep();
		kbGetKeyMs(1000);
		return ;
	}
	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "修改主管改密    ", " CHANGE MAG PWD ");
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入新密码:   ", "PLS INPUT PWD:");
	lcdGoto(0, 4);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 6, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet!=6 )
		return;

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "请再输一遍新密码", "INPUT PWD AGAIN");
	lcdGoto(0, 4);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 6, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)tmp);
	if( iRet!=6 )
		return;
	
	if( strcmp((char *)tmp, (char *)buf)!=0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " 两次密码不一致 ", "NOT THE SAME");
		ErrorBeep();
		kbGetKeyMs(1000);
		return;
	}
	strcpy((char *)operStrc[0].szPassword, (char *)buf);
	WriteOperFile();
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT, "修改密码成功    ", "CHG PWD SUCC");
	kbGetKeyMs(2000);

	return;
}
