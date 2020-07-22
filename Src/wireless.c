
#define  _NEWPOS_WIRELESS_C

#include "posapi.h"
//#include "posixlib.h"
#include "terstruc.h"
#include "glbvar.h"
#include "wireless.h"
#include "arpa/inet.h"
#include "posappapi.h"
#include "tranfunclist.h"

int SetIpAddr(uint8_t x, uint8_t y, char *ipAddr)
{
	int pos = 0;
	int ch;
	char tmp_buf[20];
	
	strcpy(tmp_buf, "___.___.___.___");
	lcdDisplay(x, y, DISP_CFONT, (char*)tmp_buf);
	
	while(1)
	{
//		lcdDisplay((x+pos*8), y, DISP_CFONT, " ");
		lcdDisplay(x, y, DISP_CFONT | DISP_CLRLINE, tmp_buf);
		
		ch = kbGetKey();
		if( ch==KEY_CANCEL )
			return -1;
		
		if( (ch>=KEY0) && (ch<=KEY9) && (pos<15) )
		{
//			disp_buf[0] = ch;
//			disp_buf[1] = 0;
//			lcdDisplay((x+pos*8), y, DISP_CFONT, (char *)disp_buf);
			
			tmp_buf[pos++] = ch;
			
			if( (pos==3) || (pos==7) || (pos==11) )
				pos++;
		}
		else if( (ch==KEY_ENTER) && (pos==15) )
		{
			strcpy((char*)ipAddr, (char*)tmp_buf);
			return 0;
		}
		else if( (ch==KEY_BACKSPACE) && (pos>0) )
		{
//			lcdDisplay((x+pos*8), y, DISP_CFONT, "_");
			pos--;
			if( (pos==3) || (pos==7) || (pos==11) )
				pos--;
			tmp_buf[pos] = '_';

		}
	}
}

uint8_t SetWirelessIPPort(void)
{
	uint8_t tmpbuf[32];
	int iRet;

	memset(tmpbuf,0,sizeof(tmpbuf));
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "主机IP地址", "SETUP HOST IP");
	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, (char *)(stPosParam.stTxnCommCfg.stWirlessPara.szRemoteIP));
	if( SetIpAddr(0, 6, (char*)tmpbuf)==0 ) 
	{
		strcpy((char*)stPosParam.stTxnCommCfg.stWirlessPara.szRemoteIP, (char*)tmpbuf);
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "主机IP端口", "SETUP HOST PORT");
	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, (char *)(stPosParam.stTxnCommCfg.stWirlessPara.szRemotePort));
	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 1, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)tmpbuf);
	if( iRet>0 )
	{
		strcpy((char*)(stPosParam.stTxnCommCfg.stWirlessPara.szRemotePort), (char*)tmpbuf);
		stPosParam.stTxnCommCfg.stWirlessPara.szRemotePort[iRet] = 0;
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "设置用户名", "SETUP USER NAME");
	strcpy((char *)tmpbuf, (char *)stPosParam.stTxnCommCfg.stWirlessPara.szUID);
	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, (char *)tmpbuf);
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CHAR+KB_EN_REVDISP, 1, 40, 
		(stPosParam.ucOprtLimitTime*1000), (char *)tmpbuf);
	if( iRet>=0 )
	{
		strcpy((char *)stPosParam.stTxnCommCfg.stWirlessPara.szUID, (char *)tmpbuf);
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "设置网络密码", "SETUP USER PWD");
	strcpy((char*)tmpbuf, (char*)(stPosParam.stTxnCommCfg.stWirlessPara.szPwd));
	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, (char *)tmpbuf);
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CHAR+KB_EN_REVDISP, 1, 30, 
		(stPosParam.ucOprtLimitTime*1000), (char *)tmpbuf);
	if( iRet>=0 )
	{
		strcpy((char*)stPosParam.stTxnCommCfg.stWirlessPara.szPwd, (char*)tmpbuf);
	}

	return OK;

}


int    SetCurCommParam(int iMode)
{
	ST_COMM_CONFIG  stCommCfgBak;
	
	if (iMode == DOWNLAOD_COMMTYPE)
	{
		stCommCfgBak = stPosParam.stTxnCommCfg;
		stPosParam.stTxnCommCfg = stPosParam.stDownParamCommCfg;
	}
	
	switch( stPosParam.stTxnCommCfg.ucCommType )
	{
	case CT_RS232:
		SetRS232Param();
		break;
		
	case CT_MODEM:
		SetAcqTransTelNo(iMode);
		SetModemParam();
		break;
		
	case CT_TCPIP:
		SetTcpIpParam();
		break;
		
	case CT_GPRS:
	case CT_CDMA:
		SetWirelessParam();
		CommOnHook(TRUE);
		lcdClrLine(2, 7);
		break;
		
	case CT_WIFI:
		SetWIFIParam(iMode);
		CommOnHook(TRUE);
		break;
		
	default:
		break;
	}
	
	CommInitModule(&stPosParam.stTxnCommCfg);

	if (iMode == DOWNLAOD_COMMTYPE)
	{
		stPosParam.stDownParamCommCfg = stPosParam.stTxnCommCfg;
		stPosParam.stTxnCommCfg = stCommCfgBak;
	}
	
	SaveAppParam();

	return 0;
}


uint8_t SetWirelessParam()
{
	char   tmpbuf[100];
	int    iRet;

	lcdCls();
	lcdFlip();	
	if( stPosParam.stTxnCommCfg.ucCommType==CT_GPRS )
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "GPRS设置        ", "GPRS SETUP");
	else
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "CDMA设置        ", "CDMA SETUP");

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "APN名称", "SETUP APN:");
	strcpy((char*)tmpbuf, (char*)stPosParam.stTxnCommCfg.stWirlessPara.szAPN);
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
		strcpy((char*)stPosParam.stTxnCommCfg.stWirlessPara.szAPN, (char*)tmpbuf);
		SaveAppParam();
	}
	

	lcdCls();
	lcdFlip();	
	if( stPosParam.stTxnCommCfg.ucCommType==CT_GPRS )
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "GPRS设置        ", "GPRS SETUP");
	else
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "CDMA设置        ", "CDMA SETUP");

	memset(tmpbuf,0,sizeof(tmpbuf));
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "主机IP地址", "SETUP HOST IP");
	lcdDisplay(0, 4, DISP_CFONT, (char *)(stPosParam.stTxnCommCfg.stWirlessPara.szRemoteIP));
	if( SetIpAddr(0, 6, (char*)tmpbuf)==0 ) 
	{
		strcpy((char*)stPosParam.stTxnCommCfg.stWirlessPara.szRemoteIP, (char*)tmpbuf);
		SaveAppParam();
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "主机端口        ", "SETUP HOST PORT");
	lcdDisplay(0, 4, DISP_CFONT, (char *)(stPosParam.stTxnCommCfg.stWirlessPara.szRemotePort));
	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 0, 6,
		(stPosParam.ucOprtLimitTime*1000), (char *)tmpbuf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return KEY_CANCEL;
	}
	if( iRet>0 )
	{
		strcpy((char*)(stPosParam.stTxnCommCfg.stWirlessPara.szRemotePort), (char*)tmpbuf);
		stPosParam.stTxnCommCfg.stWirlessPara.szRemotePort[iRet] = 0;
		SaveAppParam();
	}

	SaveAppParam();
	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "需要用户名");
	lcdDisplay(0, 4, DISP_CFONT, "0-否   1-是");
//	lcdDisplay(0, 6, DISP_CFONT, "              0");
	iRet = kbGetKey();
	if( iRet==KEY0 || iRet==KEY_CANCEL )
	{
		return OK;
	}


	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "设置用户名", "SETUP USER NAME");
	strcpy((char *)tmpbuf, (char *)stPosParam.stTxnCommCfg.stWirlessPara.szUID);
	lcdDisplay(0, 4, DISP_CFONT, tmpbuf);
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CHAR+KB_EN_REVDISP, 0, 40, 
		(stPosParam.ucOprtLimitTime*1000), (char *)tmpbuf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return KEY_CANCEL;
	}
	if( iRet>0 )
	{
		strcpy((char *)stPosParam.stTxnCommCfg.stWirlessPara.szUID, (char *)tmpbuf);
		SaveAppParam();
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "设置用户密码", "SETUP USER PWD");
	strcpy((char*)tmpbuf, (char*)(stPosParam.stTxnCommCfg.stWirlessPara.szPwd));
	lcdDisplay(0, 4, DISP_CFONT, tmpbuf);
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CHAR+KB_EN_REVDISP, 0, 30, 
		(stPosParam.ucOprtLimitTime*1000), (char *)tmpbuf);
	if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
	{
		return KEY_CANCEL;
	}
	if( iRet>0 )
	{
		strcpy((char*)stPosParam.stTxnCommCfg.stWirlessPara.szPwd, (char*)tmpbuf);
	}
	SaveAppParam();

	return OK;
}


int    SelectAndSetWIFIAP(void)
{
	int   iRet, iKeyIndex, iApNum,iMenuNo;
	char  szBuffer[200];
    WiFiConfig_t    stWIFIConfig;
    WiFiAPInfo_t    stApInfoList[64];
    WiFiAPx_t       stApxInfo;   // 
    WiFiDefAP_t     stDefApInfo;
	MenuItem        stSelectMenu[64+1];

	memset(szBuffer,0,sizeof(szBuffer));
	memset((char*)&stWIFIConfig,0,sizeof(stWIFIConfig));
	memset((char*)&stApxInfo,0,sizeof(stApxInfo));
	memset((char*)&stDefApInfo,0,sizeof(stDefApInfo));

	lcdCls();
	lcdFlip();	
	lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,"设置接入AP");
    lcdDisplay(0, 2, DISP_CFONT, "1.搜索并选择AP");
    lcdDisplay(0, 4, DISP_CFONT, "2.手工设置");
    iRet = kbGetKeyMs(60*1000);
	if (iRet != KEY1 && iRet != KEY2)
	{
		return NO_DISP;
	}
    if(iRet == KEY2)
    {
        lcdClrLine(2, 7);
		lcdFlip();	
        lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "请输入SSID:");
        memset(szBuffer,0,sizeof(szBuffer));
		strcpy(szBuffer,stPosParam.stWIFIAPInfo.szSSID);
		iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CHAR+KB_EN_REVDISP, 0, 24, 
			(stPosParam.ucOprtLimitTime*1000), (char *)szBuffer);
		if( iRet >0 && strcmp((char *)stPosParam.stWIFIAPInfo.szSSID, (char *)szBuffer))
		{
			strcpy((char *)stPosParam.stWIFIAPInfo.szSSID, (char *)szBuffer);
		}
        else
        {
            return NO_DISP;
        }
    }
    else
    {
        lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "请 稍 候");
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "正在搜索...");
		DrawRect(0, 17, 127, 63);
        iApNum = WifiScanAP(64, stApInfoList);
        if(iApNum < 0)
        {
            lcdClrLine(2, 7);
			lcdFlip();	
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "搜索AP失败");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请稍候重试");
			DrawRect(0, 17, 127, 63);
			lcdDisplay(96, 7, DISP_ASCII, "%d", iApNum);
			kbGetKeyMs(3000);
			return NO_DISP;
        }

        if(iApNum == 0)
        {
            lcdClrLine(2, 7);
			lcdFlip();	
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "搜索AP失败");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请稍候重试");
			DrawRect(0, 17, 127, 63);
			lcdDisplay(96, 7, DISP_ASCII, "%d", iApNum);
			kbGetKeyMs(3000);
			return NO_DISP;
        }

        for(iMenuNo=0; iMenuNo< iApNum; iMenuNo++)
        {
			stSelectMenu[iMenuNo].bVisible = TRUE;
			stSelectMenu[iMenuNo].pfMenuFunc = NULL;
			strcpy(stSelectMenu[iMenuNo].szMenuName,stApInfoList[iMenuNo].SSID);
        }
		stSelectMenu[iMenuNo].bVisible = FALSE;
		stSelectMenu[iMenuNo].pfMenuFunc = NULL;
		memset(stSelectMenu[iMenuNo].szMenuName,0,sizeof(stSelectMenu[iMenuNo].szMenuName));

        iMenuNo = PubGetMenu((uint8_t *)"请选择AP", stSelectMenu, MENU_AUTOSNO, 60);
        if(iMenuNo < 0 || iMenuNo>= iApNum)
        {
            return NO_DISP;
        }
        strcpy(stApxInfo.SSID, stApInfoList[iMenuNo].SSID);
    }

	lcdCls();
	lcdFlip();	
	lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,"设置接入AP");
	while(1)
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 2, DISP_CFONT, "通讯信道(0-13):");
		memset(szBuffer,0,sizeof(szBuffer));
		sprintf(szBuffer,"%d",stPosParam.stWIFIAPInfo.iChannel);
		iRet = PubGetString(NUM_IN|ECHO_IN,1,2,(uint8_t *)szBuffer,60);
		if(iRet != 0)
		{
			return NO_DISP;
		}

		iRet = atoi(szBuffer);
		if (iRet>=13)
		{
            lcdClrLine(2, 7);
			lcdFlip();	
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "无效信道数值");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "有效范围:0--13");
			DrawRect(0, 17, 127, 63);
			kbGetKeyMs(2000);
			continue;
		}

		stWIFIConfig.Channel = iRet;
		break;
	}

	lcdCls();
	lcdFlip();	
	lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,"设置加密模式");
    lcdDisplay(0, 2, DISP_ASCII, "1.NONE *");
    lcdDisplay(0, 3, DISP_ASCII, "2.WEP64 ");
    lcdDisplay(0, 4, DISP_ASCII, "3.WEP128");
    lcdDisplay(0, 5, DISP_ASCII, "4.WPA-TKIP");
    lcdDisplay(0, 6, DISP_ASCII, "5.WPA2-AES");
    iRet = PubWaitKey(60);
    switch(iRet)
    {
    case KEY_CANCEL:
        return(KB_CANCEL);
    case KEY1:
        stApxInfo.SecurityType = WIFI_SECURITY_NONE;
        break;
    case KEY2:
        stApxInfo.SecurityType = WIFI_SECURITY_WEP64;
        break;
    case KEY3:
        stApxInfo.SecurityType = WIFI_SECURITY_WEP128;
        break;
    case KEY4:
        stApxInfo.SecurityType = WIFI_SECURITY_WPATKIP;
        break;
    case KEY5:
        stApxInfo.SecurityType = WIFI_SECURITY_WPA2AES;
        break;
    default:
        break;
    }

	lcdCls();
	lcdFlip();	
	lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,"设置密钥");
	switch (stApxInfo.SecurityType)
	{
    case WIFI_SECURITY_WEP64:
    case WIFI_SECURITY_WEP128:
        strncpy(stDefApInfo.SSID, stApxInfo.SSID, sizeof(stApxInfo.SSID));
        stDefApInfo.WEPMode = stApxInfo.SecurityType;

		while(1)
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "输入WEP密钥组:");
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "有效范围:[1..4]");
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer,"%d",stPosParam.stWIFIAPInfo.iWEPKeyIdx);
			iRet = PubGetString(NUM_IN|ECHO_IN,1,1,(uint8_t *)szBuffer,60);
			if(iRet != 0)
			{
				return NO_DISP;
			}

			iRet = atoi(szBuffer);
			if (iRet>5)
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "无效信道数值");
				lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "有效范围:1--4");
				DrawRect(0, 17, 127, 63);
				kbGetKeyMs(2000);
				continue;
			}

			stDefApInfo.WEPKeyIdx = iRet;
			break;
		}
		
        lcdClrLine(2, 7);
		lcdFlip();	
        lcdDisplay(0, 2, DISP_CFONT, "设置WEP密钥[%d]:", stDefApInfo.WEPKeyIdx);
        memset(szBuffer, 0, sizeof(szBuffer));
		strcpy(szBuffer,stPosParam.stWIFIAPInfo.szListWEPKey[stDefApInfo.WEPKeyIdx]);
		iRet = PubGetString(ALPHA_IN|ECHO_IN,1,13,(uint8_t *)szBuffer,60);
        if(iRet != 0)
        {
			return NO_DISP;
        }
		strcpy(stApxInfo.WEPKey, szBuffer);
		strcpy(stDefApInfo.WEPKey[stDefApInfo.WEPKeyIdx-1], szBuffer);
		break;
    case WIFI_SECURITY_WPATKIP:
    case WIFI_SECURITY_WPA2AES:
        lcdClrLine(2, 7);
		lcdFlip();	
        lcdDisplay(0, 2, DISP_CFONT, "设置WPA PSK:");
        memset(szBuffer, 0, sizeof(szBuffer));
		sprintf(szBuffer,"%s",stPosParam.stWIFIAPInfo.szWPAPSK);
        iRet = PubGetString(ALPHA_IN|ECHO_IN,1,32,(uint8_t *)szBuffer,60);
        if(iRet != 0)
        {
			return NO_DISP;
        }
		strcpy(stApxInfo.WPAPSK, szBuffer);
        break;

	case WIFI_SECURITY_NONE:
	default:
		break;
	}

    stWIFIConfig.IchipPowerSave = 5;
    stWIFIConfig.WLANPowerSave = 5;
    stWIFIConfig.RoamingMode = 0;
    stWIFIConfig.PeriodicScanInt = 5;
    stWIFIConfig.RoamingLowSNR = 10;
    stWIFIConfig.RoamingHighSNR = 30;

	lcdCls();
	lcdFlip();	
	lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,"设置接入AP");
	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "请稍候...");
	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "正在激活设置");
	DrawRect(0, 17, 127, 63);
	if(stApxInfo.SecurityType == WIFI_SECURITY_WEP64 || stApxInfo.SecurityType == WIFI_SECURITY_WEP128)
    {
		iRet = WifiSetDefAP(&stDefApInfo);
    }
    else
    {
 	    iRet = WifiSetAP(0, &stApxInfo);
    }
    if(iRet != NET_OK)
    {
		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "设置AP配置失败");
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请稍候重试");
		DrawRect(0, 17, 127, 63);
		kbGetKeyMs(5000);
	    return NO_DISP;
    }

    iRet = WifiSetParms(&stWIFIConfig);
    if(iRet != NET_OK)
    {
		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "设置AP配置失败");
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请稍候重试");
		DrawRect(0, 17, 127, 63);
		lcdDisplay(96, 7, DISP_ASCII, "%d", iRet);
		kbGetKeyMs(5000);
	    return NO_DISP;
    }

	SetWiFiLoaclIPCfg();

	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "请稍候...");
	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "正在复位 WI-FI");
	DrawRect(0, 17, 127, 63);
    iRet = WifiSoftReset(0);
    if(iRet != NET_OK)
    {
		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "WIFI复位失败");
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请稍候重试");
		DrawRect(0, 17, 127, 63);
		lcdDisplay(96, 7, DISP_ASCII, "%d", iRet);
		kbGetKeyMs(5000);
	    return NO_DISP;
    }

	stPosParam.stWIFIAPInfo.iChannel = stWIFIConfig.Channel;
	stPosParam.stWIFIAPInfo.iSecurityType = stApxInfo.SecurityType;
	iKeyIndex = stPosParam.stWIFIAPInfo.iWEPKeyIdx = stDefApInfo.WEPKeyIdx; 
	strcpy(stPosParam.stWIFIAPInfo.szListWEPKey[iKeyIndex],stDefApInfo.WEPKey[iKeyIndex]);
	strcpy(stPosParam.stWIFIAPInfo.szWPAPSK,stApxInfo.WPAPSK);
	strcpy(stPosParam.stWIFIAPInfo.szSSID,stApxInfo.SSID);
	SaveAppParam();

	return 0;
}

int    SetWIFIParam(int iMode)
{
	int		iRet, iFlag;
	WiFiStatus_t    stWiFiStatus;
	char    szlocalIP[20];
	uint8_t tmpbuf[32];
	
	if (!(gstPosCapability.uiCommType & CT_WIFI))
	{
		return ERR_COMM_INV_TYPE;
	}
	
	PubDisplayTitle(TRUE, "设置WIFI参数");
	DisplayProcessing();

	WnetInit(20000);
	iFlag = 1;
	while(iFlag)
	{
		iRet = WifiGetCurConnect(&stWiFiStatus);
		PubDisplayTitle(TRUE, "设置WIFI参数");
		lcdDisplay(0,2,DISP_ASCII|DISP_MEDIACY,"%16.16s",stWiFiStatus.SSID);
		if (stWiFiStatus.Status != WIFI_STATUS_CONNECTED)
			lcdDisplay(0, 3, DISP_ASCII, "STATUS: DISCONNECT");
		else
			lcdDisplay(0, 3, DISP_ASCII, "STATUS: CONNECTED");
		lcdDisplay(0, 4, DISP_ASCII, "SIG: %d%",stWiFiStatus.SigLevel);
		if (stWiFiStatus.LnkQual > 75)
		{
			lcdDisplay(0, 4, DISP_ASCII, "QUALITY: EXCELLED");
		}
		else if (stWiFiStatus.LnkQual > 50)
		{
			lcdDisplay(0, 4, DISP_ASCII, "QUALITY: GOOD");
		}
		else
		{
			lcdDisplay(0, 4, DISP_ASCII, "QUALITY: BAD");
		}
		
		memset(szlocalIP, 0, sizeof(szlocalIP));
		iRet = WifiGetLocalIP(szlocalIP);	
		if (iRet !=0 || strcmp(szlocalIP,"0.0.0.0")==0 )
			lcdDisplay(0, 5, DISP_ASCII, "LOCAL IP: 0.0.0.0");
		else
			lcdDisplay(0, 5, DISP_ASCII, "LOCAL:%s",szlocalIP);	
		lcdDisplay(0,6,DISP_CFONT,"[FN/MENU] MODIFY");
		iRet = PubWaitKey(10);
		switch (iRet)
		{
		case KEY_FN:
		case KEY_MENU:
			SelectAndSetWIFIAP();
			break;
		default:
			iFlag = 0;
			break;
		}
	}

	iRet = InitWiFiModule(TRUE);

	PubDisplayTitle(TRUE, "设置WIFI参数");	
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "设置服务器IP", "SETUP HOST IP");
	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, (char *)(stPosParam.stTxnCommCfg.stWIFIIpPara.szRemoteIP));	
	if( SetIpAddr(0, 6, (char*)tmpbuf)==0 ) 
	{
		strcpy((char*)stPosParam.stTxnCommCfg.stWIFIIpPara.szRemoteIP, (char*)tmpbuf);		
	}
	
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "设置服务器端口", "SETUP HOST PORT");
	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, (char *)(stPosParam.stTxnCommCfg.stWIFIIpPara.szRemotePort));
	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 1, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)tmpbuf);
	if( iRet>0 )
	{
		strcpy((char*)(stPosParam.stTxnCommCfg.stWIFIIpPara.szRemotePort), (char*)tmpbuf);		
	}
	
	return 0;
}


int    SetWiFiLoaclIPCfg(void)
{
    int         iRet;
    tcpipcfg_t  stdevcfg;
    char        szIpaddr[64];

    PubDisplayTitle(TRUE,"SETUP LOACL IP");
    memset(&stdevcfg, 0, sizeof(stdevcfg));
    iRet = NetGetTCPIPCfg(OPT_DEVWIFI, &stdevcfg);
    if(iRet != NET_OK)
    {
        lcdDisplay(0, 2, DISP_CFONT|DISP_CLRLINE, "Get CFG error");
        lcdDisplay(0, 4, DISP_CFONT|DISP_CLRLINE, "RET = %d", iRet);
        PubWaitKey(60);
        return(iRet);
    }

    lcdDisplay(0, 0, DISP_ASCII|DISP_CLRLINE, "DHCP: %s", stdevcfg.dhcp==NO ? "DISABLE" : "ENABLE");
    memset(szIpaddr, 0, sizeof(szIpaddr));
    inet_ntop(AF_INET, &(stdevcfg.localip), szIpaddr, sizeof(szIpaddr));
	lcdDisplay(0, 1, DISP_ASCII|DISP_CLRLINE, "IP  : %s", szIpaddr);

    memset(szIpaddr, 0, sizeof(szIpaddr));
    inet_ntop(AF_INET, &(stdevcfg.subnet), szIpaddr, sizeof(szIpaddr));
	lcdDisplay(0, 2, DISP_ASCII|DISP_CLRLINE, "MASK: %s", szIpaddr);

    memset(szIpaddr, 0, sizeof(szIpaddr));
    inet_ntop(AF_INET, &(stdevcfg.gateway), szIpaddr, sizeof(szIpaddr));
	lcdDisplay(0, 3, DISP_ASCII|DISP_CLRLINE, "GATE: %s", szIpaddr);

    memset(szIpaddr, 0, sizeof(szIpaddr));
    inet_ntop(AF_INET, &(stdevcfg.dns1), szIpaddr, sizeof(szIpaddr));
	lcdDisplay(0, 4, DISP_ASCII|DISP_CLRLINE, "DNS1: %s", szIpaddr);

    memset(szIpaddr, 0, sizeof(szIpaddr));
    inet_ntop(AF_INET, &(stdevcfg.dns2), szIpaddr, sizeof(szIpaddr));
	lcdDisplay(0, 5, DISP_ASCII|DISP_CLRLINE, "DNS2: %s", szIpaddr);

    lcdDisplay(0, 6, DISP_CFONT, "[0]--Modify");

    kbFlush();
    iRet = kbGetKey();
    if(iRet == KEY0)
    {
		PubDisplayTitle(TRUE,"SETUP LOACL IP");
        lcdDisplay(0, 2, DISP_CFONT, "SET DHCP:");
        if(stdevcfg.dhcp == NO)
        {
            lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "DISABLE");
        }
        else
        {
            lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "ENABLE");
        }
		lcdDisplay(0, 6, DISP_CFONT, "1:ENABLE*  0:DIS");
        kbFlush();
        iRet = PubWaitKey(60);
        if(iRet == KEY0)
        {
            stdevcfg.dhcp = NO;
        }
        else if(iRet == KEY1)
        {
            stdevcfg.dhcp = YES;
        }
        else 
        {
            return(KB_CANCEL);
        }

        if(stdevcfg.dhcp == NO)
        {
            lcdClrLine(2, 7);
			lcdFlip();	
			iRet = SetIpAddr(0, 6, (char*)stPosParam.stTxnCommCfg.stWIFIIpPara.szLocalIP);
			if( iRet!=0 )
			{
				return iRet;
			}
			inet_pton(AF_INET, (char *)stPosParam.stTxnCommCfg.stWIFIIpPara.szLocalIP, &stdevcfg.localip);

            lcdClrLine(2, 7);
			lcdFlip();	
			iRet = SetIpAddr(0, 6, (char*)stPosParam.stTxnCommCfg.stWIFIIpPara.szNetMask);
			if( iRet!=0 )
			{
				return iRet;
			}
			inet_pton(AF_INET, (char *)stPosParam.stTxnCommCfg.stWIFIIpPara.szNetMask, &stdevcfg.subnet);

            lcdClrLine(2, 7);
			lcdFlip();	
			iRet = SetIpAddr(0, 6, (char*)stPosParam.stTxnCommCfg.stWIFIIpPara.szGatewayIP);
			if( iRet!=0 )
			{
				return iRet;
			}
			inet_pton(AF_INET, (char *)stPosParam.stTxnCommCfg.stWIFIIpPara.szGatewayIP, &stdevcfg.gateway);

            lcdClrLine(2, 7);
			lcdFlip();	
			iRet = SetIpAddr(0, 6, (char*)stPosParam.stTxnCommCfg.stWIFIIpPara.szDNS);
			if( iRet!=0 )
			{
				return iRet;
			}
			inet_pton(AF_INET, (char *)stPosParam.stTxnCommCfg.stWIFIIpPara.szDNS, &stdevcfg.dns1);
        }
        lcdClrLine(2, 7);
		lcdFlip();	
        lcdDisplay(0, 3, DISP_CFONT, "SAVE CFG...");
        iRet = NetSetTCPIPCfg(OPT_DEVWIFI, &stdevcfg);
        if(iRet != NET_OK)
        {
            lcdDisplay(0, 3, DISP_CFONT|DISP_CLRLINE, "保存配置错误", "Save CFG error");
            lcdDisplay(0, 5, DISP_CFONT|DISP_CLRLINE, "RET = %d", iRet);
            PubWaitKey(60);
            return(iRet);
        }
    }
    return(OK);
}


// 设置TCP/IP参数
int SetTcpIpParam(void)
{
	uint8_t tmpbuf[32];
	int iRet;

// 不能设置本机IP,没有权限
/*
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "以太网设置      ", "TCPIP SETUP");
	lcdClrLine(2, 7);
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "本机IP地址", "SETUP LOCAL IP");
	lcdDisplay(0, 4, DISP_CFONT, (char *)(stPosParam.stTxnCommCfg.stTcpIpPara.szLocalIP));	
	if( SetIpAddr(0, 6, (char*)tmpbuf)==0 ) 
	{
		strcpy((char*)stPosParam.stTxnCommCfg.stTcpIpPara.szLocalIP, (char*)tmpbuf);		
	}

	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "子网掩码", "SETUP IP MASK");
	lcdDisplay(0, 4, DISP_CFONT, (char *)(stPosParam.stTxnCommCfg.stTcpIpPara.szNetMask));	
	if( SetIpAddr(0, 6, (char*)tmpbuf)==0 ) 
	{
		strcpy((char*)stPosParam.stTxnCommCfg.stTcpIpPara.szNetMask, (char*)tmpbuf);		
	}


	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "网关", "SETUP DNS");
	lcdDisplay(0, 4, DISP_CFONT, (char *)(stPosParam.stTxnCommCfg.stTcpIpPara.szGatewayIP));	
	if( SetIpAddr(0, 6, (char*)tmpbuf)==0 ) 
	{
		strcpy((char*)stPosParam.stTxnCommCfg.stTcpIpPara.szGatewayIP, (char*)tmpbuf);		
	}
*/
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "设置主机IP", "SETUP HOST IP");
	lcdDisplay(0, 4, DISP_CFONT, (char *)(stPosParam.stTxnCommCfg.stTcpIpPara.szRemoteIP));	
	if( SetIpAddr(0, 6, (char*)tmpbuf)==0 ) 
	{
		strcpy((char*)stPosParam.stTxnCommCfg.stTcpIpPara.szRemoteIP, (char*)tmpbuf);		
	}
	
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "设置主机端口", "SETUP HOST PORT");
	lcdDisplay(0, 4, DISP_CFONT, (char *)(stPosParam.stTxnCommCfg.stTcpIpPara.szRemotePort));
	lcdGoto(80, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 0, 6, 
		(stPosParam.ucOprtLimitTime*1000), (char *)tmpbuf);
	if( iRet>0 )
	{
		strcpy((char*)(stPosParam.stTxnCommCfg.stTcpIpPara.szRemotePort), (char*)tmpbuf);		
	}

	SaveAppParam();
	return OK;
}


int    SetRS232Param(void)
{
	int iKey ,ucFlag ;
	uint8_t ucCommType, szBuffer[30];
	
	ucCommType = stPosParam.stTxnCommCfg.stRS232Para.ucSendMode;
	ucFlag = 1;
	while(ucFlag)
	{
		lcdCls();
		lcdFlip();	
		lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  串口通讯方式: ");
		if( ucCommType==CM_RAW )
		{
			lcdDisplay(0, 2, DISP_CFONT, "%.18s",  "=>原报文发送");
			lcdDisplay(0, 4, DISP_CFONT, "%.18s",  "  打包发送");
		}
		else
		{
			lcdDisplay(0, 2, DISP_CFONT, "%.18s",  "  原报文发送");
			lcdDisplay(0, 4, DISP_CFONT, "%.18s",  "=>打包发送");
		}
		
		iKey = kbGetKeyMs(30000);
		switch (iKey)
		{
		case KEY_CANCEL:			
		case KEY_ENTER:	
			ucFlag = 0;  // 退出while
			break;
		case KEY1 :			       
			ucCommType = CM_RAW;
			ucFlag = 0;
			break;
		case KEY2 :			       
			ucCommType = CM_SYNC;
			ucFlag = 0;
			break;			       			       
		case KEY_UP :
			if(ucCommType != CM_RAW)
			{
				ucCommType = CM_RAW;
			}
			else
			{
				ucCommType = CM_SYNC;
			}
			break;	
		case KEY_DOWN :
			if(ucCommType != CM_RAW)
			{
				ucCommType = CM_RAW;
			}
			else
			{
				ucCommType = CM_SYNC;
			}
			break;
		default:
			break;
		}
	}
	stPosParam.stTxnCommCfg.stRS232Para.ucSendMode = ucCommType;
	
	lcdCls();
	lcdFlip();	
	lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY|DISP_REVERSE,"  串口通讯参数  ");
	lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY, (char *)stPosParam.stTxnCommCfg.stRS232Para.szAttr);
	lcdGoto(0, 6);
	memset(szBuffer, 0, sizeof(szBuffer));
	iKey = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CHAR+KB_EN_REVDISP, 0, 24, 
		(stPosParam.ucOprtLimitTime*1000), (char *)szBuffer);
	if( iKey >0 && strcmp((char *)stPosParam.stTxnCommCfg.stRS232Para.szAttr, (char *)szBuffer))
	{
		strcpy((char *)stPosParam.stTxnCommCfg.stRS232Para.szAttr, (char *)szBuffer);
	}
	
	stPosParam.stTxnCommCfg.stRS232Para.ucPortNo = PORT_COM1;
	
	return 0;		
}


// 设置当前收单行交易电话号码
int SetAcqTransTelNo(int iMode)
{
	uint8_t buf[50];
	int     iRet;
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "通讯参数设置    ", "SETUP DAIL PARA ");
	DispMulLanguageString(0, 2, DISP_CFONT, "外线号码:", "INPUT PABX:");
	strcpy((char *)buf, (char *)stTransCtrl.szExtNum);
	lcdDisplay(0, 4, DISP_CFONT, (char *)buf);
	lcdGoto(56, 6);
	iRet = kbGetString(KB_EN_BIGFONT+KB_EN_NUM+KB_EN_CHAR+KB_EN_REVDISP, 0, 4, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet>=0 )
	{
		strcpy((char *)stTransCtrl.szExtNum, (char *)buf);
		SaveCtrlParam();
	}

	if (iMode == DOWNLAOD_COMMTYPE)
	{
		lcdCls();
		lcdFlip();	
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  远程更新电话  ", "REMOTE DOWNLOAD TEL");
		DispMulLanguageString(0, 2, DISP_CFONT, "请输入更新电话:", "DOWNLOAD TEL:");
		strcpy((char *)buf, (char *)stPosParam.szDownloadTel);
		lcdDisplay(0, 4, DISP_CFONT, (char *)buf);
		lcdGoto(0, 6);
		iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CHAR+KB_EN_REVDISP, 1, 14, 
			(stPosParam.ucOprtLimitTime*1000), (char *)buf);
		if( iRet>0 && strcmp((char *)stPosParam.szDownloadTel, (char *)buf))
		{
			strcpy((char *)stPosParam.szDownloadTel, (char *)buf);
		}
		
		strcpy((char *)stPosParam.stDownParamCommCfg.stPSTNPara.szTxnTelNo, (char *)stPosParam.szDownloadTel);
		strcpy((char *)stPosParam.stTxnCommCfg.stPSTNPara.szTxnTelNo, (char *)stPosParam.stDownParamCommCfg.stPSTNPara.szTxnTelNo);
		return 0;
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "通讯参数设置    ", "SETUP DAIL PARA ");
	DispMulLanguageString(0, 2, DISP_CFONT, "中心交换电话1:", "INPUT NUM1:");
	strcpy((char *)buf, (char *)stPosParam.szPhoneNum1);
	lcdDisplay(0, 4, DISP_CFONT, (char *)buf);
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CHAR+KB_EN_REVDISP, 0, 14, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet>0 && strcmp((char *)stPosParam.szPhoneNum1, (char *)buf))
	{
		strcpy((char *)stPosParam.szPhoneNum1, (char *)buf);
	}
	
	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "通讯参数设置    ", "SETUP DAIL PARA ");
	DispMulLanguageString(0, 2, DISP_CFONT, "中心交换电话2:", "INPUT NUM2:");
	strcpy((char *)buf, (char *)stPosParam.szPhoneNum2);
	lcdDisplay(0, 4, DISP_CFONT, (char *)buf);
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CHAR+KB_EN_REVDISP, 0, 14, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet>0 && strcmp((char *)stPosParam.szPhoneNum2, (char *)buf))
	{
		strcpy((char *)stPosParam.szPhoneNum2, (char *)buf);						  
	}

	lcdCls();
	lcdFlip();	
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "通讯参数设置    ", "SETUP DAIL PARA ");
	DispMulLanguageString(0, 2, DISP_CFONT, "中心交换电话3:", "INPUT NUM3:");
	strcpy((char *)buf, (char *)stPosParam.szPhoneNum3);
	lcdDisplay(0, 4, DISP_CFONT, (char *)buf);
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_CHAR+KB_EN_REVDISP, 0, 14, 
		(stPosParam.ucOprtLimitTime*1000), (char *)buf);
	if( iRet>0 && strcmp((char *)stPosParam.szPhoneNum3, (char *)buf))
	{
		strcpy((char *)stPosParam.szPhoneNum3, (char *)buf);						  
	}
	
	SaveAppParam();
	
	return 0;
}

// 修改Modem参数
int SetModemParam(void)
{
//	int   iKey;
	//iTemp;
//	int   iCnt;
//	uint8_t szBuff[20], ucTemp;
//	int   sBateRateList[20];

	PubDisplayTitle(TRUE,"通讯参数设置    ");

//	PubDisplayTitle(TRUE,"MODEM拨号参数");
/*
	ucTemp = stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.connect_mode;
	while( 2 )
	{
		lcdClrLine(2, 7);
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"通讯模式");
		sprintf((char *)szBuff, "(%s)", ucTemp ? "同步" : "异步");
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,szBuff);
		lcdDisplay(0,6,DISP_CFONT|DISP_MEDIACY,"1.同步  0.异步");
		iKey = kbGetKeyMs(60000);
		if( iKey==KEY_CANCEL || iKey==KEY_INVALID )
		{
			return ERR_USERCANCEL;
		}
		else if( iKey== KEY_ENTER_POS )
		{
			break;
		}
		else if ( iKey==KEY0 )
		{
			ucTemp = MODEM_COMM_ASYNC;
		}
		else if ( iKey==KEY1 )
		{
			ucTemp = MODEM_COMM_SYNC;
		}
	}
	stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.connect_mode = ucTemp;
*/
	stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.connect_mode = MODEM_COMM_SYNC;

/*
	PubDisplayTitle(TRUE,"MODEM拨号参数");
	sBateRateList[0] = MODEM_CONNECT_1200BPS;		
	sBateRateList[1] = MODEM_CONNECT_2400BPS;
	sBateRateList[2] = MODEM_CONNECT_4800BPS;
	sBateRateList[3] = MODEM_CONNECT_7200BPS;
	sBateRateList[4] = MODEM_CONNECT_9600BPS;
	sBateRateList[5] = MODEM_CONNECT_12000BPS;
	sBateRateList[6] = MODEM_CONNECT_14400BPS;
	sBateRateList[7] = MODEM_CONNECT_16800BPS;
	sBateRateList[8] = MODEM_CONNECT_19200BPS;
	sBateRateList[9] = MODEM_CONNECT_21600BPS;
	sBateRateList[10] = MODEM_CONNECT_24000BPS;
	sBateRateList[11] = MODEM_CONNECT_26400BPS;
	sBateRateList[12] = MODEM_CONNECT_28800BPS;
	sBateRateList[13] = MODEM_CONNECT_31200BPS;
	sBateRateList[14] = MODEM_CONNECT_33600BPS;
	sBateRateList[15] = MODEM_CONNECT_48000BPS;
	sBateRateList[16] = MODEM_CONNECT_56000BPS;
	while( 2 )
	{
		lcdClrLine(2, 7);
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"通讯速率: %ld",stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.connect_speed);
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"请输入通讯速率:");
		lcdGoto(80, 6);
		memset(szBuff,0,sizeof(szBuff));
		iKey = kbGetString(KB_EN_BIGFONT+KB_EN_NUM, 0, 6, (stPosParam.ucOprtLimitTime*1000), (char *)szBuff);
		if( iKey<0 )
			return (E_TRANS_CANCEL);
		iTemp = atoi(szBuff);
		for (iCnt=0; iCnt<= 16; iCnt++)
		{
			if (sBateRateList[iCnt] == iTemp)
			{
				break;
			}
		}
		if (iCnt > 16)
		{
			sysBeef(1,200);
			lcdClrLine(2,7);
			DispMulLanguageString(0, 3, DISP_CFONT|DISP_MEDIACY, " 输入值无效 ", "INVALID RATE");
			kbGetKeyMs(3000);
			continue;
		}
		stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.connect_speed = iTemp;
		break;
	}
*/
	stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.connect_speed = MODEM_CONNECT_1200BPS;
/*
	PubDisplayTitle(TRUE,"MODEM拨号参数");
	ucTemp = stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.dial_mode;
	while( 2 )
	{
		lcdClrLine(2, 7);
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"拨号模式");
		sprintf((char *)szBuff, "(%s)", ucTemp ? "脉冲拨号" : "双音多频拨号");
		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,szBuff);
		lcdDisplay(0,6,DISP_CFONT|DISP_MEDIACY,"0.DTMF 1.PULSE");
		iKey = kbGetKeyMs(60000);
		if( iKey==KEY_CANCEL || iKey==KEY_INVALID )
		{
			return ERR_USERCANCEL;
		}
		else if( iKey== KEY_ENTER_POS )
		{
			break;
		}
		else if ( iKey==KEY0 )
		{
			ucTemp = MODEM_DAIL_DTMF;
		}
		else if ( iKey==KEY1 )
		{
			ucTemp = MODEM_DAIL_PULSE;
		}
	}
	stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.connect_mode = ucTemp;
*/
	stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.dial_mode = MODEM_DAIL_DTMF;

//	PubDisplayTitle(TRUE,"MODEM拨号参数");
//	while( 2 )
//	{
//		lcdClrLine(2, 7);
//		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"外线暂停时间:");
//		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"  %ld",stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.dial_pause);
//		lcdGoto(80, 6);
//		memset(szBuff,0,sizeof(szBuff));
//		iKey = kbGetString(KB_EN_BIGFONT+KB_EN_NUM, 0, 3, (stPosParam.ucOprtLimitTime*1000), (char *)szBuff);
//		if( iKey<0 )
//			return (E_TRANS_CANCEL);
//		iTemp = atoi(szBuff);
//		if (iTemp ==0 )
//		{
//			break;
//		}
//		stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.dial_pause = iTemp;
//		break;
//	}
	stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.dial_pause = 1;

//	PubDisplayTitle(TRUE,"MODEM拨号参数");
//	while( 2 )
//	{
//		lcdClrLine(2, 7);
//		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"拨号等待超时时间");
//		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"  %ld",stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.dial_timeo);
//		lcdGoto(80, 6);
//		memset(szBuff,0,sizeof(szBuff));
//		iKey = kbGetString(KB_EN_BIGFONT+KB_EN_NUM, 0, 3, (stPosParam.ucOprtLimitTime*1000), (char *)szBuff);
//		if( iKey<0 )
//			return (E_TRANS_CANCEL);
//		iTemp = atoi(szBuff);
//		if (iTemp ==0 )
//		{
//			break;
//		}
//		stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.dial_timeo = iTemp;
//		break;
//	}
	stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.dial_timeo = 30;

//	PubDisplayTitle(TRUE,"MODEM拨号参数");
//	while( 2 )
//	{
//		lcdClrLine(2, 7);
//		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"空闲超时时间");
//		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,"  %ld",stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.idle_timeo);
//		lcdGoto(80, 6);
//		memset(szBuff,0,sizeof(szBuff));
//		iKey = kbGetString(KB_EN_BIGFONT+KB_EN_NUM, 0, 3, (stPosParam.ucOprtLimitTime*1000), (char *)szBuff);
//		if( iKey<0 )
//			return (E_TRANS_CANCEL);
//		iTemp = atoi(szBuff);
//		if (iTemp ==0 )
//		{
//			break;
//		}
//		stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.idle_timeo = iTemp;
//		break;
//	}
	stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.idle_timeo = 60;

//	PubDisplayTitle(TRUE,"MODEM拨号参数");
//	ucTemp = stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.extension_chk;
//	while( 2 )
//	{
//		lcdClrLine(2, 7);
//		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"是否检测线路");
//		sprintf((char *)szBuff, "(%s)", ucTemp ? "检测" : "不检测");
//		lcdDisplay(0,4,DISP_CFONT|DISP_MEDIACY,szBuff);
//		lcdDisplay(0,6,DISP_CFONT|DISP_MEDIACY,"0.YES   1.NO");
//		iKey = kbGetKeyMs(60000);
//		if( iKey==KEY_CANCEL || iKey==KEY_INVALID )
//		{
//			return ERR_USERCANCEL;
//		}
//		else if( iKey== KEY_ENTER_POS )
//		{
//			break;
//		}
//		else if ( iKey==KEY0 )
//		{
//			ucTemp = 1;
//			break;
//		}
//		else if ( iKey==KEY1 )
//		{
//			ucTemp = 0;
//			break;
//		}
//	}
	stPosParam.stTxnCommCfg.stPSTNPara.stDialPara.extension_chk = 1;

	stPosParam.stTxnCommCfg.ucCommType = CT_MODEM;
	CommInitModule(&stPosParam.stTxnCommCfg);
	return 0;
}


// 预拨号
// pre-connect to host
int PreDial(void)
{
	int		iRet;
	
	/*if( !stPosParam.bPreDial )
	{
		return 0;
	}*/
	
	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 3, DISP_CFONT, "  预拨号中... ");
	lcdDisplay(0, 5, DISP_CFONT, " PREDIALING..."); 
	DrawRect(0, 17, 127, 63);
	
	iRet = AdjustCommParam();
	if( iRet!=0 )
	{
		return iRet;
	}
	
	if (gstCurCommCfg.ucCommType ==CT_WIFI )
	{
		iRet = InitWiFiModule(FALSE);
		if (iRet != 0)
		{
			return iRet;
		}
	}
	
	iRet = CommDial(PREDIAL_MODE);
	iRet = CheckPreDial(iRet);
	if(iRet !=0)
	{
		return NO_DISP;
	}

	return iRet;
}


int    CheckPreDial(int iPreDialRet)
{
	int iRet;
	
	iRet = iPreDialRet;
	iPreDialRet = iPreDialRet & MASK_ERR_CODE;
	
	if (iRet == ERR_COMM_INV_TYPE)
	{
		DispCommErrMsg(iRet);
		return NO_DISP; 
	}
	
	switch( gstCurCommCfg.ucCommType )
	{
	case CT_RS232:		
	case CT_MODEM:		
	case CT_TCPIP:
	case CT_WIFI:
		if (iPreDialRet != 0)
		{
			DispCommErrMsg(iRet);
			return NO_DISP;
		}
		break;
		
	case CT_CDMA:
	case CT_GPRS:
		if (iPreDialRet== NET_ERR_NOSIM ||
			iPreDialRet == NET_ERR_SIMDESTROY)
		{
			DispCommErrMsg(iRet);
			return NO_DISP;
		}
		break;
		
	default:
		DispCommErrMsg(ERR_COMM_INV_TYPE);
		return NO_DISP;
		break;
	}
	
	return 0;
}

void NacTxd(uint8_t *psTxdData, uint32_t uiDataLen)
{
	uint8_t	*psTemp, sWorkBuf[1024*50+10];
	
	if( uiDataLen>1024*50 )
	{
		Display2StringInRect((uint8_t *)"数据长度非法", (uint8_t *)"INVALID PACK");
		PubWaitKey(2);
		return;
	}
	
	sWorkBuf[0] = STX;
	sWorkBuf[1] = (uiDataLen/1000)<<4    | (uiDataLen/100)%10;	// convert to BCD
	sWorkBuf[2] = ((uiDataLen/10)%10)<<4 | uiDataLen%10;
	memcpy(&sWorkBuf[3], psTxdData, uiDataLen);
	sWorkBuf[3+uiDataLen]   = ETX;
	sWorkBuf[3+uiDataLen+1] = PubCalcLRC((char *)psTxdData, uiDataLen, (uint8_t)(sWorkBuf[1] ^ sWorkBuf[2] ^ ETX));
	uiDataLen += 5; 
	
	portClose(PORT_COM1);
	portOpen(PORT_COM1, (void *)"9600,8,n,1");
	psTemp = sWorkBuf;
	while( uiDataLen-->0 )
	{
		if( portSends(PORT_COM1, psTemp,1)!=0 )
		{
			psTemp++;
			break;
		}
		psTemp++;
	}
	portClose(PORT_COM1);
}



