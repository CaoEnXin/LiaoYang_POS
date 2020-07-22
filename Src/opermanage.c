
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
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ǩ    ��    ", "     LOGON      ");
	}
	else if( flag==0 )
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    �ն˽���    ", "  POS UNLOCK    ");
	}
	DispMulLanguageString(0, 2, DISP_CFONT, "����Ա��:  ", "OPER NO.");
	kbFlush();
	lcdGoto(80, 2);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP+KB_EN_SHIFTLEFT, 2, 2, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf1);
	if( iRet!=2 ) 
	{
		return NO_DISP;
	}
	sprintf((char *)nowOper, "0%2s", buf1);
	if( strcmp((char *)nowOper, operStrc[0].szTellerNo)==0 ) //���ܹ���
	{
		SupTellerFunc();
		return NO_DISP;
	}
	if( strcmp((char *)nowOper, SYS_TELLER_NO)==0 ) //ϵͳ����Ա����
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
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  ����Ա�Ų���  ", " OPER.NO DIFFER ");
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
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "   �޴˲���Ա   ", "   OPER EXIST   ");
		ErrorBeep();
		kbGetKeyMs(1000);
		return NO_DISP;
	}
	
//	lcdCls();
	lcdClrLine(2, 7);
	lcdFlip();
	if( flag==1 )
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ǩ    ��    ", "     LOGON      ");
	if( flag==0 ) 
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    �ն˽���    ", "   POS UNLOCK   ");
		lcdSetIcon (ICON_LOCK, OPENICON);
	}
	DispMulLanguageString(0, 2, DISP_CFONT, "����������    ", "INPUT PWD:");
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
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "    �����!!    ", "  PWD ERROR!  ");
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
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_REVERSE|DISP_CFONT, "���Ӳ���Ա      ", "    ADD OPER    ");
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT, "��������������: ", "INPUT MANAGE PWD");
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 6, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet!=6 )
		return ;

	if( strcmp((char *)buf, operStrc[0].szPassword)!=0x00 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "   ���������   ", "PWD ERROR");
		ErrorBeep();
		kbGetKeyMs(1000);
		return ;
	}
		
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_REVERSE|DISP_CFONT, "���Ӳ���Ա      ", "    ADD OPER    ");
	DispMulLanguageString(0, 2, DISP_CFONT, "���������Ա���", "OPER NO:");
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
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " ������������ͬ ", "SAME WITH MANAGE");
		ErrorBeep();
		kbGetKeyMs(1000);

		return;
	}
	if( strcmp(SYS_TELLER_NO, (char *)buf)==0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "���������Ա��ͬ", "SAME WITH SYSTEM");
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
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " �ò���Ա�Ѵ��� ", "SAME WITH OPER");
			ErrorBeep();
			kbGetKeyMs(1000);
			return;
		}
	}

	sprintf((char *)szDisp, "����Ա���: %.2s", buf1);
	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, (char *)szDisp);
	DispMulLanguageString(0, 4, DISP_CFONT, "���������Ա����", "INPUT PWD:");
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 4, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf2);
	if( iRet!=4 )
		return ;

	sprintf((char *)szDisp, "����Ա���: %.2s", buf1);
	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, (char *)szDisp);
	DispMulLanguageString(0, 4, DISP_CFONT, "������һ������", "INPUT PWD:");
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 4, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf3);
	if( iRet!=4 )
		return;

	if( memcmp(buf2, buf3, 4)!=0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT, "�����������Ա", "NOT THE SAME");
		DispMulLanguageString(0, 6, DISP_CFONT, "���벻һ��", "NOT THE SAME");
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
			DispMulLanguageString(0, 4, DISP_CFONT, "����Ա���ӳɹ�  ", "  ADD SUCC!  ");
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
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "  ����Ա������  ", "TOO MANY OPER ");
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
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_REVERSE|DISP_CFONT, "ɾ������Ա      ", "    DEL OPER    ");
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT, "��������������: ", "INPUT MANAGE PWD");
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 6, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet!=6 )
		return ;

	if( strcmp((char *)buf, operStrc[0].szPassword)!=0x00 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "   ���������   ", "PWD ERROR");
		ErrorBeep();
		kbGetKeyMs(1000);
		return ;
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_REVERSE|DISP_CFONT, "   ɾ������Ա   ", "  DELETE OPER   ");
	DispMulLanguageString(0, 2, DISP_CFONT, "���������Ա���", "OPER NO:");

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
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "���������Ա��ͬ", "SAME WITH SYSTEM");
		ErrorBeep();
		kbGetKeyMs(1000);

		return;
	}
	if( strcmp((char *)stTransCtrl.szNowTellerNo, (char *)buf)==0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " ����Ա�Ѿ�ǩ�� ", " LOGONED ");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "    ����ɾ��    ", "DELETE FORBID");
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
			DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_REVERSE|DISP_CFONT, "ɾ������Ա      ", "    DEL OPER    ");
			sprintf((char *)szDisp, "����Ա���: %.2s", buf1);
			lcdDisplay(0, 2, DISP_CFONT, (char *)szDisp);
			DispMulLanguageString(0, 4, DISP_CFONT, "�Ƿ�ɾ��?", "DELETE?");
			DispMulLanguageString(0, 6, DISP_CFONT, "��-ȷ�� ��-ȡ��", "ENTER CANCEL");
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
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "   �޴˲���Ա   ", " OPER EXIST ");
		ErrorBeep();
		kbGetKeyMs(1000);

		return;
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT, "    ɾ���ɹ�    ", "DELETE SUCC");
	OkBeep();
	kbGetKeyMs(3000);
	return;

}

void TellerShow()
{
	int  j, operNum=0;
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_REVERSE|DISP_CFONT, "��ѯ����Ա      ", " VIEW OPER INFO ");
	lcdGoto(0, 2);

	for(j=1; j<MAX_OPER_NUM; j++) 
	{
		if( strcmp(operStrc[j].szTellerNo, TELLER_NOBODY)==0 ) 
		{
			continue;
		}
		lcdPrintf(" %.2s ", operStrc[j].szTellerNo+1);  //����ʾ2λ
		operNum++;
		if( operNum%4==0 ) 
			lcdPrintf("\n");
		
		if( operNum%12==0 )
		{
			DispMulLanguageString(0, 6, DISP_CFONT, "��������鿴����", "MORE PRESS KEY");
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
		"��Ա����        ",
		"1 ���ܸ���      ",
		"2 ����    3 ɾ��",
		"4��ѯ5����Ա����"
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
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_REVERSE|DISP_CFONT, "�޸Ĳ���Ա����  ", "OPER CHANGE PWD ");
	sprintf((char *)szDisp, "����Ա���: %.2s", &stTransCtrl.szNowTellerNo[1]);
	lcdDisplay(0, 2, DISP_CFONT, (char *)szDisp);
	DispMulLanguageString(0, 6, DISP_CFONT, "��-ȷ�� ��-ȡ��", "ENTER CANCEL");
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
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "   �޴˲���Ա   ", " OPER EXIST ");
		ErrorBeep();
		kbGetKeyMs(1000);
		return;
	}
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "������ԭ����    ", "ORIGINAL PWD:");
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 4, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet!=4 )
		return;
	
	if( strcmp((char *)buf, operStrc[j].szPassword) ) 
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "ԭ���벻��ȷ    ", "   PWD ERROR! ");
		ErrorBeep();
		kbGetKeyMs(1000);

		return;
	}
	
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "������������    ", "INPUT NEW PWD:");
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 4, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet!=4 )
		return ;
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "������һ��������", "INPUT AGAIN:");
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 4, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf1);
	if( iRet!=4 )
		return;
	if( strcmp((char *)buf, (char *)buf1) )  
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "�����������벻��", "NOT THE SAME");
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
	DispMulLanguageString(0, 4, DISP_CFONT, "�޸�����ɹ�    ", "  CHANGED SUCC! ");
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
	DispMulLanguageString(0, 0, DISP_MEDIACY|DISP_REVERSE|DISP_CFONT, "�ն˽���        ", "LOCK THE SCREEN ");
	DispMulLanguageString(0, 2, DISP_CFONT, "���������Ա��", "PRESS [ENTER]");
	lcdGoto(0, 4);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 2, 2, (stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet!=2 ) 
	{
		return ;
	}

	memset(nowOper, 0, sizeof(nowOper));
	sprintf((char *)nowOper, "0%2s", buf);
	if( strcmp((char *)nowOper, operStrc[0].szTellerNo)==0 ) //���ܹ���
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "��������������", "INPUT PWD:");
		lcdGoto(0, 6);
		iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 6, 6, 
			(stPosParam.ucOprtLimitTime*1000), (char *)pass);
		if( iRet!=6 )
			return ;

		if( (memcmp((char *)pass, operStrc[0].szPassword, 6)!=0x00) ) 
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "    ���벻��    ", "PWD ERROR");
			ErrorBeep();
			kbGetKeyMs(1000);
			return;
		}
	}
	else if( strcmp((char *)nowOper, (char *)stTransCtrl.szOldTellerNo)==0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "���������Ա����", "INPUT PWD:");
		lcdGoto(0, 4);
		iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 4, 4, 
			(stPosParam.ucOprtLimitTime*1000), (char *)pass);
		if( iRet!=4 )
			return ;

		if( (strcmp((char *)pass, (char *)stTransCtrl.szNowTellerPwd)!=0x00) ) 
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "    ���벻��    ", "PWD ERROR");
			ErrorBeep();
			kbGetKeyMs(1000);

			return;
		}

	}
	else
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "�ǵ�ǰ����Ա    ", "PWD ERROR");
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
		DispMulLanguageString(0, 2, DISP_CFONT, "��������������: ", "INPUT MANAGE PWD");
		lcdGoto(80, 6);  
		iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_REVDISP, 6, 6, 
			(stPosParam.ucOprtLimitTime*1000), (char *)buf);
		if( iRet!=6 )
			return NO_DISP;

		if( strcmp(buf, operStrc[0].szPassword)!=0x00 )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "   ���������   ", "PWD ERROR");
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
		DispMulLanguageString(0, 2, DISP_CFONT, "���������Ա����", "INPUT PWD:");
		lcdGoto(96, 6);
		iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 4, 4, 
			(stPosParam.ucOprtLimitTime*1000), (char *)buf);
		if( iRet!=4 )
			return NO_DISP;
		
		if( strcmp((char *)buf, (char *)stTransCtrl.szNowTellerPwd)!=0x00 )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "     �����     ", "PWD ERROR");
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
		"  ���ܹ�����  ",
		"1 ����    2 ɾ��",
		"3 ��ѯ    4 ����"
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "�޸����ܸ���    ", " CHANGE MAG PWD ");
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "������ԭ����: ", "INPUT MANAGE PWD");
	lcdGoto(0, 4);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 6, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet!=6 )
		return ;

	if( strcmp((char *)buf, operStrc[0].szPassword)!=0x00 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "ԭ���벻��ȷ    ", "PWD ERROR");
		ErrorBeep();
		kbGetKeyMs(1000);
		return ;
	}
	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "�޸����ܸ���    ", " CHANGE MAG PWD ");
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "������������:   ", "PLS INPUT PWD:");
	lcdGoto(0, 4);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 6, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet!=6 )
		return;

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "������һ��������", "INPUT PWD AGAIN");
	lcdGoto(0, 4);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CRYPTDISP+KB_EN_SHIFTLEFT+KB_EN_REVDISP, 6, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)tmp);
	if( iRet!=6 )
		return;
	
	if( strcmp((char *)tmp, (char *)buf)!=0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " �������벻һ�� ", "NOT THE SAME");
		ErrorBeep();
		kbGetKeyMs(1000);
		return;
	}
	strcpy((char *)operStrc[0].szPassword, (char *)buf);
	WriteOperFile();
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 4, DISP_CFONT, "�޸�����ɹ�    ", "CHG PWD SUCC");
	kbGetKeyMs(2000);

	return;
}
