#include "posapi.h"
#include "posmacro.h"
#include "terstruc.h"
#include "glbvar.h"
#include "tranfunclist.h"

void GetKeyDescription(int ucKey, char *buf);

void PosDeviceDetect(void)
{
	int iRet;

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    POS检测     ", "   DETECT POS   ");

	iRet = prnStatus();
	switch(iRet) 
	{
	case PRN_PAPEROUT:
		DispMulLanguageString(0, 4, DISP_CFONT, "打印机......缺纸", "PRINT...NO PAPER");
		ErrorBeep();
		kbGetKey();
		break;
	case PRN_OK:
		DispMulLanguageString(0, 4, DISP_CFONT, "打印机......正常", "PRINT...OK");
		break;
	default:
		DispMulLanguageString(0, 4, DISP_CFONT, "打印机......故障", "PRINT...ERROR");
		ErrorBeep();
		kbGetKey();
		break;
	}

}

void GetKeyDescription(int ucKey, char *buf)
{
	switch(ucKey)
	{
	case KEY1:
		strcpy(buf, "按键: 1");
		break;
	case KEY2:
		strcpy(buf, "按键: 2");
		break;
	case KEY3:
		strcpy(buf, "按键: 3");
		break;
	case KEY4:
		strcpy(buf, "按键: 4");
		break;
	case KEY5:
		strcpy(buf, "按键: 5");
		break;
	case KEY6:
		strcpy(buf, "按键: 6");
		break;
	case KEY7:
		strcpy(buf, "按键: 7");
		break;
	case KEY8:
		strcpy(buf, "按键: 8");
		break;
	case KEY9:
		strcpy(buf, "按键: 9");
		break;
	case KEY0:
		strcpy(buf, "按键: 0");
		break;
	case KEY_ALPHA:
		strcpy(buf, "按键: ALPHA");
		break;
	case KEY_ENTER:
		strcpy(buf, "按键: ENTER");
		break;
	case KEY_BACKSPACE:
		strcpy(buf, "按键: BACKSPACE");
		break;
	case KEY_CLEAR:
		strcpy(buf, "按键: CLEAR");
		break;		
	case KEY_MENU:
		strcpy(buf, "按键: MENU");
		break;
	case KEY_FN:
		strcpy(buf, "按键: FUNCT");
		break;
	case KEY_UP:
		strcpy(buf, "按键: 上箭头");
		break;
	case KEY_DOWN:
		strcpy(buf, "按键: 下箭头");
		break;
	case KEY_PRNUP:
		strcpy(buf, "按键: 进纸");
		break;
	}
}

int KeyboardTest(void)
{	
//	int i, j, ucKey;
	int ucKey;
//	int TestAll[256];
	uint8_t buf[20];
	
//	for(i=0; i<256; i++) 
//		TestAll[i] = 0;
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "按键测试,请按键", "TEST KEY,[ENTER]");
	kbSetSlipFW(OFF);
	while(1)
	{	
		ucKey = kbGetKey();
		if( ucKey==KEY_CANCEL )
		{
//			j = 0;
//			for(i=0; i<256; i++) 
//			{
//				if( TestAll[i] ) 
//					j++;
//			}
//			if( j==22 ) 
//				return 0;
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 2, DISP_CFONT, "未完成测试!", "TEST NOT OVER");
			DispMulLanguageString(0, 4, DISP_CFONT, "取消-退出测试", "CANCEL-EXIST");
			DispMulLanguageString(0, 6, DISP_CFONT, "确认-继续测试", "ENTER-CONTINUE");
			if( kbGetKey()==KEY_CANCEL ) 
				return 0;
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 2, DISP_CFONT, "按键测试,请按键", "PRESS KEY");
			continue;
		}
		GetKeyDescription(ucKey, (char *)buf);
		lcdClrLine(6, 7);
		lcdFlip();	
		lcdDisplay(0, 6, DISP_CFONT, (char *)buf);
//		TestAll[ucKey] = 1;
	}
}

int LcdTest(int flag)
{
	int i, x, y;
	int iKey;

	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  POS LCD测试   ", "   LCD TEST     ");

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "L C D 显示屏测试", "LCD TEST");
	DispMulLanguageString(0, 4, DISP_CFONT, "小西文字符...", "LITTLE CHAR");
	iKey = kbGetKeyMs(1000);
	if( iKey==KEY_CANCEL )	
		return 0;

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_ASCII, "abcdefghijklmnopqrstuvwxyz!@#$%&*()_-+=[]{},;.<>|\\/?1234567890ABCDEFGHIJKLMNOPQRSTUVWXWXYZ");
	iKey = kbGetKeyMs(2000);
	if( iKey==KEY_CANCEL )	
		return 0;

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "L C D 显示屏测试", "LCD TEST");
	DispMulLanguageString(0, 4, DISP_CFONT, "大西文字符...", "BIG CHAR");
	iKey = kbGetKeyMs(1000);
	if( iKey==KEY_CANCEL )	
		return 0;

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "abcdefghijklmnopqrstuvwxyz!@#$%&*()_-+=[]{},;.<>");
	iKey = kbGetKeyMs(2000);
	if( iKey==KEY_CANCEL )	
		return 0;
	
	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "|\\/?1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	iKey = kbGetKeyMs(2000);
	if( iKey==KEY_CANCEL )	
		return 0;
	
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "L C D 显示屏测试", "LCD TEST");
	lcdDisplay(0, 7, DISP_ASCII, "NEW POS TECHNOLOGY");
	
	lcdClrLine(2, 7);
	lcdFlip();
	lcdLight(LIGHT_OFF_MODE);
	DispMulLanguageString(40, 4, DISP_CFONT, "关背光", "CLOSE LIGHT");
	iKey = kbGetKeyMs(2000);
	if( iKey==KEY_CANCEL )	
		return 0;
	
	lcdClrLine(2, 7);
	lcdFlip();
	lcdLight(LIGHT_TIMER_MODE);
	DispMulLanguageString(40, 4, DISP_CFONT, "开背光", "OPEN LIGHT");
	iKey = kbGetKeyMs(2000);
	if( iKey==KEY_CANCEL )	
		return 0;

	DispMulLanguageString(0, 4, DISP_CFONT, "点亮所有图标...", "LIGHT ALL ICON");
	for(i=1; i<10; i++)
	{
		if( i==2 ) 
			lcdSetIcon(i, INITSIGNAL+5);
		else  
			lcdSetIcon(i, OPENICON);
	}
	lcdSetIcon(1, CLOSEICON);
	lcdSetIcon(2, INITSIGNAL);
	sysDelayMs(300);
	lcdSetIcon(2, INITSIGNAL+1);
	sysDelayMs(300);
	lcdSetIcon(2, INITSIGNAL+2);
	sysDelayMs(300);
	lcdSetIcon(2, INITSIGNAL+3);
	sysDelayMs(300);
	lcdSetIcon(2, INITSIGNAL+4);
	sysDelayMs(300);
	lcdSetIcon(2, INITSIGNAL+5);
	sysDelayMs(300);
	iKey = kbGetKeyMs(2000);
	if( iKey==KEY_CANCEL )	
		return 0;
	
	DispMulLanguageString(0, 4, DISP_CFONT, "    全 黑 屏    ", "  BACK SCREEN   ");
	for(x=0; x<128; x++)
	  for(y=0; y<64; y++)
		 lcdStipple(x, y, ON);
	iKey = kbGetKeyMs(2000);
	if( iKey==KEY_CANCEL )	
		return 0;
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  POS LCD测试   ", "    LCD TEST    ");
	DispMulLanguageString(0, 4, DISP_CFONT, "关闭所有图标...", "CLOSE ALL ICON");
	for(i=1; i<10; i++) 
		lcdSetIcon(i, CLOSEICON);
	iKey = kbGetKeyMs(2000);
	if( iKey==KEY_CANCEL )	
		return 0;
	
	DispMulLanguageString(0, 6, DISP_CFONT, "LCD测试完毕     ", "TEST END");
	iKey = kbGetKeyMs(2000);
	if( iKey==KEY_CANCEL )	
		return 0;

	OkBeep();
	return 0;
}

void MagTest(int ShowFlag)
{
	uint8_t ucRet;
	uint8_t Track1[100], Track2[50], Track3[120];

	magOpen();
	while(1)
	{
		lcdCls();
		lcdFlip();	
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "    读卡测试    ", "  MAGCARD TEST  ");
		DispMulLanguageString(0, 2, DISP_CFONT, "    读卡测试    ", "PLS SWIPE<<<");
		
		while(1)
		{
			if( magSwiped()==YES ) 
				break;
			if( kbhit()==YES )
			{
				if( kbGetKey()==KEY_CANCEL )
				{
					magClose();
					return;
				}
			}
		}
		memset(Track1, 0, sizeof(Track1));
		memset(Track2, 0, sizeof(Track2));
		memset(Track3, 0, sizeof(Track3));
		ucRet = magRead(Track1, Track2, Track3);
		if( (ucRet&0x70)!=0x00 )
		{
			ErrorBeep();
			DispMulLanguageString(0, 6, DISP_CFONT, "读卡错!", "READ ERROR!");
		}
		if( ucRet==0x06 )
		{
			OkBeep();
			DispMulLanguageString(0, 6, DISP_CFONT, "读2、3磁正确.", "2/3 TRACK OK");
		}
		if( ucRet==0x03 )
		{
			OkBeep();
			DispMulLanguageString(0, 6, DISP_CFONT, "读1、2磁正确.", "1/2 TRACK OK");
		}
		if( ucRet==0x01 )
		{
			OkBeep();
			DispMulLanguageString(0, 6, DISP_CFONT, "读1磁正确.", "1 TRACK OK");
		}
		if( ucRet==0x02 )
		{
			OkBeep();
			DispMulLanguageString(0, 6, DISP_CFONT, "读2磁正确.", "2 TRACK OK");
		}
		if( ucRet==0x04 )
		{
			OkBeep();
			DispMulLanguageString(0, 6, DISP_CFONT, "读3磁正确.", "3 TRACK OK");
		}
		if( ucRet==0x07 )
		{
			OkBeep();
			DispMulLanguageString(0, 6, DISP_CFONT, "读1/2/3磁正确.", "1/2/3 TRACK OK");
		}
		if( ucRet==0x05 )
		{
			OkBeep();
			DispMulLanguageString(0, 6, DISP_CFONT, "读1/3磁正确.", "1/3 TRACK OK");
		}

		kbGetKey();
		
		if( ShowFlag )
		{
			if( ucRet & 0x01 )
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				lcdDisplay(0, 2, DISP_ASCII, (char *)Track1);

				kbGetKey();
			}
			if( ucRet & 0x02 )
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				lcdDisplay(0, 2, DISP_ASCII, (char *)Track2);
				kbGetKey();
			}
			if( ucRet & 0x04 )
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				lcdDisplay(0, 2, DISP_ASCII, (char *)Track3);
				kbGetKey();
			}
		}
	}
}

