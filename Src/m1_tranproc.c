/*****************************************************************************************************
* m1_tranproc.c
*
*auth: howard.joe
*
* 留言: 该文件是用来实现M1核心处理函数在本工程的使用的，无需保持独立性，如果要移植到别的工程上，需考虑新工程的
*属性酌情修改。另外，要尽可能保证，当本工程不需要M1卡处理时，可以方便地拆卸。
*****************************************************************************************************/
#include "posmacro.h"
#include "terstruc.h"
#include "glbvar.h"
#include "tranfunclist.h"
#include "m1_card/m1_card_proc.h"
#include "m1_tranproc.h"
#include "LocalFtp.h"
//#include "LocalFtp.h"
#include "EtcDLL.h"


UINT8 InListPassive(INT32 *iCardInfoLen, UINT8 *sCardInfo);

UINT8 ConfirmCardPassword(void)
{
	APDU_SEND stApduSend;
	APDU_RESP stApduResp;
	int   iRet;
	
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS = 0x20;
	stApduSend.P1 = 0;
	stApduSend.P2 = 0x41;
	stApduSend.Lc = 0x08;
	memcpy(stApduSend.DataIn,"\x10\x01",8);//卡的密码
	stApduSend.Le = 0;
	stApduSend.LeFlag = 0;
	iRet = iccExchangeAPDU(ICC_USERCARD,&stApduSend,&stApduResp);
	if (iRet!=ICC_SUCCESS)
	{
	return -1;
	}

	if (!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 ))
	{

	    lcdClrLine(2,7);
		lcdDisplay(0, 2, DISP_CFONT, "%02X %02X", stApduResp.SWA, stApduResp.SWB);
		lcdDisplay(0, 4, DISP_CFONT, "密码错误1");
		kbGetKey();
	return -2;
	}
	return OK;

}
UINT8 ConfirmDevelopersPassword(void)
{
	APDU_SEND stApduSend;
		APDU_RESP stApduResp;
		int   iRet;
		
		memset((char*)&stApduSend,0,sizeof(stApduSend));
		memset((char*)&stApduResp,0,sizeof(stApduResp));
		stApduSend.CLA = 0;
		stApduSend.INS = 0x20;
		stApduSend.P1 = 0;
		stApduSend.P2 = 0x42;
		stApduSend.Lc = 0x08;
		memcpy(stApduSend.DataIn,"\x20\x00\x07\x01\x20\x00\x11\x01",8);//卡的密码
		stApduSend.Le = 0;
		stApduSend.LeFlag = 0;
		iRet = iccExchangeAPDU(ICC_USERCARD,&stApduSend,&stApduResp);
		if (iRet!=ICC_SUCCESS)
		{
		return -1;
		}
	
		if (!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 ))
		{
	
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT, "%02X %02X", stApduResp.SWA, stApduResp.SWB);
			lcdDisplay(0, 4, DISP_CFONT, "密码错误2");
			kbGetKey();
		    return -2;
		}
		return OK;


}
UINT8   SAMGetRandom(UINT8* sRandom)
{
		int    iRet;//, iCnt;
		APDU_SEND stApduSend;
		APDU_RESP stApduResp;
	
	  
		memset((char*)&stApduSend,0,sizeof(stApduSend));
		memset((char*)&stApduResp,0,sizeof(stApduResp));
		stApduSend.CLA = 0;
		stApduSend.INS = 0x84;
		stApduSend.P1 = 0x00;
		stApduSend.P2 = 0x00;
		stApduSend.Lc = 0x00;
	//	memcpy(stApduSend.DataIn,"\x10\x01",2);
	//	stApduSend.Lc = 2;
	//	 memcpy(stApduSend.DataIn,"1PAY.SYS.DDF01",14);
		stApduSend.Le = 0x08;
		stApduSend.LeFlag = 1;
	//	NacTxd(&stApduSend, 15);
		iRet = iccExchangeAPDU(ISAM_SLOT,&stApduSend,&stApduResp);
		if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
		{
	
			lcdClrLine(2, 7);
			PubDispString("取随机数错", 3|DISP_LINE_CENTER);
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		}
		memcpy(sRandom,stApduResp.DataOut,8);
		//NacTxd(stApduResp.DataOut, 50);
		return 0;

}


//输入:inbuf要加密的数据内容
//输入:len要加密的数据长度，输入之前可以先不补0x00
//输出:inbuf加密后的数据内容
int TDEACBCEnry(uint8_t * inbuf, int len,int* poutlen,uint8_t * key)
{
	uint8_t  buf[17];//, tmpbuf[17];
	int 	 l,  ret;
	int      i,k;

	
	memset(buf, 0, 8);//初始化向量
	memset(inbuf+len, 0, 8);
	inbuf[len]=0x80;

	if( len%8 ) 
		l = len/8 + 1;		
	else 
		l = len/8;
	for(k=0; k<8; k++) 
	{
		inbuf[k] = buf[k] ^ inbuf[k];
	}
	ret=TDEA(inbuf,inbuf,key,16,TDEA_ENCRYPT);
	if( ret!=PED_RET_OK )
		return(ret);
	
	for(i=1; i<l; i++) 
	{
		for(k=0; k<8; k++) 
		{
			inbuf[i*8+k] = inbuf[i*8+k] ^ inbuf[(i-1)*8+k];
		}
        ret=TDEA(&inbuf[i*8],&inbuf[i*8],key,16,TDEA_ENCRYPT);
		if( ret!=PED_RET_OK )
			return(ret);
	}
	*poutlen	=l*8;
    return OK;
}
//输入:inbuf要解密的数据内容
//输入:len要解密的数据长度
//输出:inbuf解密后的明文数据
int TDEACBCDecry(uint8_t * inbuf, int len,uint8_t * key)
{
	uint8_t  buf[17],inbuftemp[500];// tmpbuf[17],
	int 	 l,  ret;
	int      i,k;

	memcpy(inbuftemp,inbuf,len+8);
	memset(buf, 0, 8);//初始化向量
//	memset(inbuf+len, 0, 8);

	if( len%8 ) 
		l = len/8 + 1;		
	else 
		l = len/8;
	//memcpy(tmpbuf,inbuf,8);
	ret=TDEA(inbuf,inbuf,key,16,TDEA_DECRYPT);
	if( ret!=PED_RET_OK )
		return(ret);
	for(k=0; k<8; k++) 
	{
		inbuf[k] = buf[k] ^ inbuf[k];
	}

	
	for(i=1; i<l; i++) 
	{
	   // memcpy(tmpbuf,&inbuf[i-*8],8);
        ret=TDEA(&inbuf[i*8],&inbuf[i*8],key,16,TDEA_DECRYPT);
		if( ret!=PED_RET_OK )
			return(ret);
		for(k=0; k<8; k++) 
		{
			inbuf[i*8+k] = inbuf[i*8+k] ^ inbuftemp[(i-1)*8+k];
		}

	}
		
    return OK;
}


UINT8 EnrichAuthenticate(void)
{
	int   iOldTime,iNowTime;
	int   iRet;
	int   iDatalen=0;
	char  szBuffer[300];
	//UINT8 sRandom[8];
	UINT8 result[16];
	APDU_SEND stApduSend;
	APDU_RESP stApduResp;
//	UINT8 ucRet;
	
	kbFlush();

	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS = 0x88;
	stApduSend.P1 = 0x00;
	stApduSend.P2 = 0x60;
	stApduSend.Lc = 0x10;
	memcpy(stApduSend.DataIn,"\x75\x00\xFF\x00\x00\x00\x00\x00",8);
	stApduSend.Le = 0x00;
	stApduSend.LeFlag = 1;
//NacTxd(&stApduSend, 30);
	iRet = iccExchangeAPDU(ISAM_SLOT,&stApduSend,&stApduResp);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{
		lcdClrLine(2, 7);
		PubDispString("ISAM内部认证错", 3|DISP_LINE_CENTER);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	memcpy(result,stApduResp.DataOut,8);
//NacTxd(stApduResp.DataOut, 50);

	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS = 0x88;
	stApduSend.P1 = 0x00;
	stApduSend.P2 = 0x60;
	stApduSend.Lc = 0x10;
	memcpy(stApduSend.DataIn,"\x8A\xFF\x00\xFF\xFF\xFF\xFF\xFF",8);
	stApduSend.Le = 0x00;
	stApduSend.LeFlag = 1;
//NacTxd(&stApduSend, 30);
	iRet = iccExchangeAPDU(ISAM_SLOT,&stApduSend,&stApduResp);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{

		lcdClrLine(2, 7);
		PubDispString("ISAM内部认证错", 3|DISP_LINE_CENTER);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
//NacTxd(stApduResp.DataOut, 50);

	//memcpy(result + 8,stApduResp.DataOut,8);
	//memcpy(gstYktEnrichLog.sIsamKey,result,16);

//NacTxd("9999", 20);
//NacTxd(gstYktEnrichLog.sIsamKey, 20);


/*
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "请插入管理卡"); 
//	lcdDisplay(0, 6, DISP_CFONT|DISP_MEDIACY, "充值中请勿拔卡"); 
	iOldTime = sysGetTimerCount();
	while(1)
	{
		iNowTime= sysGetTimerCount();
		if( (iNowTime-iOldTime) >= (UINT32)(stPosParam.ucOprtLimitTime*1000) )	 // 检查定时器
		{
			 //iRet = ERR_COMM_TIMEOUT;
			  return E_TRANS_CANCEL;
		}
		iRet = iccDetect(ICC_USERCARD);
		if( iRet==ICC_SUCCESS )
		{
			iccSetPara(ICC_USERCARD,ICC_VOL_5V,ICC_EMV );//ICC_38400
			iRet = iccPowerUp(ICC_USERCARD, &iDatalen, szBuffer); 
			if( iRet!=ICC_SUCCESS )
			{
				lcdClrLine(2,7);
				PubDispString("用户卡上电失败", 3|DISP_LINE_CENTER);
				PubDispString("无效卡，请重试", 5|DISP_LINE_CENTER);
				PubWaitKey(5);
				iccPowerDown(ICC_USERCARD);
				return E_TRANS_FAIL;
			} 
		//	stInParam.ucSwipedFlag=CARD_INSERTED;//CARD_PASSIVE;
		  //  CARD_INSERTED
			break;
		}
		if( kbhit()==YES )
		{
			iRet = kbGetKey();
			if( iRet==KEY_CANCEL )
			{
				return E_TRANS_CANCEL;
			}
		}
	}
	//
	/////////对管理卡密码/////////
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS = 0x20;
	stApduSend.P1 = 0;
	stApduSend.P2 = 0x41;
	stApduSend.Lc = 0x08;
	memcpy(stApduSend.DataIn,"\x00\x00\x00\x00\x00\x00\x00\x00",8);//卡的密码
	stApduSend.Le = 0;
	stApduSend.LeFlag = 0;
//	NacTxd(&stApduSend, 20);
	iRet = iccExchangeAPDU(ICC_USERCARD,&stApduSend,&stApduResp);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{

	    lcdClrLine(2,7);
		lcdDisplay(0, 2, DISP_CFONT, "%02X %02X", stApduResp.SWA, stApduResp.SWB);
		lcdDisplay(0, 4, DISP_CFONT, "密码错误1");
		kbGetKey();
		return -2;
	}
	//
	//////////对管理卡认证开发商密钥/////////////
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS = 0x20;
	stApduSend.P1 = 0;
	stApduSend.P2 = 0x42;
	stApduSend.Lc = 0x08;
	memcpy(stApduSend.DataIn,"\x20\x00\x07\x01\x20\x00\x11\x01",8);//卡的密码
	stApduSend.Le = 0;
	stApduSend.LeFlag = 0;
//	NacTxd("2001",4);
//	NacTxd(&stApduSend, 20);
	iRet = iccExchangeAPDU(ICC_USERCARD,&stApduSend,&stApduResp);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{
	
		lcdClrLine(2,7);
		lcdDisplay(0, 2, DISP_CFONT, "%02X %02X", stApduResp.SWA, stApduResp.SWB);
		lcdDisplay(0, 4, DISP_CFONT, "密码错误2");
		kbGetKey();
		return -2;
	}
//	NacTxd("2002",4);

	/////////小卡取随机数//////
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS = 0x84;
	stApduSend.P1 = 0x00;
	stApduSend.P2 = 0x00;
	stApduSend.Lc = 0x00;
	stApduSend.Le = 0x08;
	stApduSend.LeFlag = 1;
//	NacTxd(&stApduSend, 15);
	iRet = iccExchangeAPDU(ISAM_SLOT,&stApduSend,&stApduResp);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{

		lcdClrLine(2, 7);
		PubDispString("取随机数错", 3|DISP_LINE_CENTER);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	memcpy(sRandom,stApduResp.DataOut,8);
//	NacTxd(stApduResp.DataOut, 50);

	///////////////大卡内部认证/////////////
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS = 0x88;
	stApduSend.P1 = 0x00;
	stApduSend.P2 = 0x11;
	stApduSend.Lc = 0x08;
	memcpy(stApduSend.DataIn,sRandom,8);
	stApduSend.Le = 0x00;
	stApduSend.LeFlag = 1;
//	NacTxd(&stApduSend, 15);
	iRet = iccExchangeAPDU(ICC_USERCARD,&stApduSend,&stApduResp);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{

		lcdClrLine(2, 7);
		PubDispString("大卡内部认证错", 3|DISP_LINE_CENTER);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	memcpy(sRandom,stApduResp.DataOut,8);
//	NacTxd(stApduResp.DataOut, 50);
//	NacTxd("2010", 4);

	///////////////小卡外部认证/////////////
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS = 0x82;
	stApduSend.P1 = 0x00;
	stApduSend.P2 = 0x22;
	stApduSend.Lc = 0x08;
	memcpy(stApduSend.DataIn,sRandom,8);
	stApduSend.Le = 0x00;
	stApduSend.LeFlag = 1;
//	NacTxd(&stApduSend, 15);
	iRet = iccExchangeAPDU(ISAM_SLOT,&stApduSend,&stApduResp);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{

		lcdClrLine(2, 7);
		PubDispString("小卡外部认证错", 3|DISP_LINE_CENTER);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	///////////////对管理卡取随机数///////////////
//		NacTxd("2011", 4);
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS = 0x84;
	stApduSend.P1 = 0x00;
	stApduSend.P2 = 0x00;
	stApduSend.Lc = 0x00;
//	memcpy(stApduSend.DataIn,"\x10\x01",2);
//	stApduSend.Lc = 2;
//	 memcpy(stApduSend.DataIn,"1PAY.SYS.DDF01",14);
	stApduSend.Le = 0x08;
	stApduSend.LeFlag = 1;
//	NacTxd(&stApduSend, 15);
	iRet = iccExchangeAPDU(ICC_USERCARD,&stApduSend,&stApduResp);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{

		lcdClrLine(2, 7);
		PubDispString("取随机数错", 3|DISP_LINE_CENTER);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	memcpy(sRandom,stApduResp.DataOut,8);
//	NacTxd(stApduResp.DataOut, 50);
//	NacTxd("2012", 4);

	///////////////ISAM内部认证/////////////
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS = 0x88;
	stApduSend.P1 = 0x00;
	stApduSend.P2 = 0x11;
	stApduSend.Lc = 0x08;
	memcpy(stApduSend.DataIn,sRandom,8);
	stApduSend.Le = 0x00;
	stApduSend.LeFlag = 1;
//	NacTxd(&stApduSend, 15);
	iRet = iccExchangeAPDU(ISAM_SLOT,&stApduSend,&stApduResp);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{

		lcdClrLine(2, 7);
		PubDispString("ISAM内部认证错", 3|DISP_LINE_CENTER);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	memcpy(sRandom,stApduResp.DataOut,8);
//	NacTxd(stApduResp.DataOut, 50);

	///////////////大卡外部认证/////////////
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS = 0x82;
	stApduSend.P1 = 0x00;
	stApduSend.P2 = 0x21;
	stApduSend.Lc = 0x08;
	memcpy(stApduSend.DataIn,sRandom,8);
	stApduSend.Le = 0x00;
	stApduSend.LeFlag = 1;
//	NacTxd(&stApduSend, 15);
	iRet = iccExchangeAPDU(ICC_USERCARD,&stApduSend,&stApduResp);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{

		lcdClrLine(2, 7);
		PubDispString("大卡外部认证错", 3|DISP_LINE_CENTER);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	*/
	return 0;
	 
}

UINT8 CPU_ReadEnrichData(pEtcardInParam_t  pInParam)
{
	int   iRet;
	int        iSendLen, iRecvLen;//iRet,
	UINT8 szSendData[300], szRecvData[300];
	UINT8 sBlance[10];
	
	/////////////////选择公共钱包:00A4040009A00000000386980701/////
	iSendLen = 0;
    iRecvLen = 0;  
	memset(szSendData, 0x00, sizeof(szSendData));
    memset(szRecvData, 0x00, sizeof(szRecvData));
	memcpy(szSendData,"\x00\xA4\x00\x00\x02\x3F\x01\x00",8);//memcpy(szSendData,"\x00\xA4\x00\x00\x02\x3F\x00\x00",8);
	iSendLen = 8;
	iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
	if ((iRet ==MIF_RET_SUCCESS) && (szRecvData[iRecvLen-2] == 0x90 && szRecvData[iRecvLen-1]== 0x00))
	{
		;
	}
	else
   {
        lcdClrLine(2, 7);
	    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "主应用选择错");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
     
	}
//NacTxd("0015", 4);
//NacTxd(szRecvData,iRecvLen);
	
	    iSendLen = 0;
        iRecvLen = 0;  
	    memset(szSendData, 0x00, sizeof(szSendData));
        memset(szRecvData, 0x00, sizeof(szRecvData));
	    memcpy(szSendData,"\x00\xb0\x95\x00\x00",5);
	    iSendLen = 5;
	    iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
//NacTxd("0016", 4);
//NacTxd(szSendData,iSendLen);
//NacTxd(szRecvData,iRecvLen);

		if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
	    {
		    lcdClrLine(2, 7);
		    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "读消费卡文件错");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		// return ERR_CODE_USECARD_SELECT_ADF;
	    }
		//gstYktEnrichLog.uCardtype=szRecvData[30];
		//memcpy(gstYktEnrichLog.sCardSerialNo,&szRecvData[12],8);//卡应用序列号 
		PubBcd2Asc(&szRecvData[12],8,gstYktEnrichLog.sCardSerialNo);//卡应用序列号 

		iSendLen = 0;
        iRecvLen = 0;  
	    memset(szSendData, 0x00, sizeof(szSendData));
        memset(szRecvData, 0x00, sizeof(szRecvData));
	    memcpy(szSendData,"\x00\xb0\x85\x00\x00",5);
	    iSendLen = 5;
	    iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
//NacTxd("0017", 4);
//NacTxd(szSendData,iSendLen);
//NacTxd(szRecvData,iRecvLen);
//NacTxd(gstYktEnrichLog.sCardSerialNo,20);

		if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
	    {
		    lcdClrLine(2, 7);
		    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "读消费卡文件错");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		// return ERR_CODE_USECARD_SELECT_ADF;
	    }
		gstYktEnrichLog.uCardtype=szRecvData[0];
 

/*			
//判断启用标志
       
	   	if(szRecvData[37]==0x01)
	   	{
			; 
		}
		else if(szRecvData[37]==0x02)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT, "卡片状态:");
			lcdDisplay(0, 5, DISP_CFONT, "停用");
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP;

		}
		else if(szRecvData[37]==0x00)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT, "卡片状态:");
			lcdDisplay(0, 5, DISP_CFONT, "未启用");
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP;

		}
		else
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT, "卡片状态:");
			lcdDisplay(0, 5, DISP_CFONT, "未知");
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP;
		}

		
		PubAsc2Bcd(pInParam->szDate,8,szBuffer);
		if (memcmp(&szRecvData[33],szBuffer,4)<0 )
	   	{
		   lcdClrLine(2, 7);
		   lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "年检日期已过期");
		   lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "请年检");
		   PubBeepErr();
		   PubWaitKey(3);
		   return NO_DISP; 
	    }
		 	
		if (memcmp(&szRecvData[24],szBuffer,4)<0 )
	   	{
		   lcdClrLine(2, 7);
		   lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "应用有效期已过");
		  // lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "请年检");
		   PubBeepErr();
		   PubWaitKey(3);
		   return NO_DISP; 
	    }
	   	PubBcd2Asc(&szRecvData[12],8,gstYktEnrichLog.sCardSerialNo);
	    //PubBcd2Asc(&szRecvData[12],8,PosCom.stTrans.sCardSerialNo);
	//   memcpy(PosCom.stTrans.SCardSerialNo,&szRecvData[12],8);//UINT8 SCardSerialNo[8];//卡应用序列号 
	 //  gstYktEnrichLog.uCardtype=szRecvData[8];
*/

		//----选择1001  目录
	/*	iSendLen = 0;
        iRecvLen = 0;  
	    memset(szSendData, 0x00, sizeof(szSendData));
        memset(szRecvData, 0x00, sizeof(szRecvData));
	    memcpy(szSendData,"\x00\xA4\x00\x00\x02\x10\x01\x00",8);
	    iSendLen = 8;
	    iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
	    if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
	    {
	     lcdClrLine(2, 7);
	    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "目录选择错");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
		 //return ERR_CODE_USECARD_SELECT_ADF;
	    }*/
		 
		//-------读年检日期-------------------------
	/*	iSendLen = 0;
        iRecvLen = 0;  
	    memset(szSendData, 0x00, sizeof(szSendData));
        memset(szRecvData, 0x00, sizeof(szRecvData));
	    memcpy(szSendData,"\x00\xb0\x9A\x01\x04",5);
	    iSendLen = 5;
	    iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
	    if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
	    {
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "读年年检日期错");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		// return ERR_CODE_USECARD_SELECT_ADF;
	    }
		//---验证年检---------
         PubAsc2Bcd(pInParam->szDate,8,szBuffer);
	   if (memcmp(szRecvData,szBuffer,4)<0 )
	   	{
		   lcdClrLine(2, 7);
		   lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "年检日期已过期");
		   lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "请年检");
		   PubBeepErr();
		   PubWaitKey(3);
		   return NO_DISP; 


	    }*/
		//----------------
//		NacTxd("0017", 4);
//		NacTxd(szRecvData,iRecvLen);//
		//memcpy(pOutParam->CardInfo.AppInfo.AppInsideNum, &szRecvData[12], 8);//保存票卡卡号

		//----密码验证-----------
	/*	iSendLen = 0;
		iRecvLen = 0;  
		memset(szSendData, 0x00, sizeof(szSendData));
		memset(szRecvData, 0x00, sizeof(szRecvData));
		memcpy(szSendData,"\x00\x20\x00\x00\x04\x47\x54\x43\x43",9);
		iSendLen = 9;
		iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
		if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
		{
		   lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "密码校验错");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		  // return ERR_CODE_VERIFY_PIN;
		}*/
//NacTxd("0018", 4);
//NacTxd(szRecvData,iRecvLen);
		//---余额查询
		iSendLen = 0;
		iRecvLen = 0;	
		memset(szSendData, 0x00, sizeof(szSendData));
		memset(szRecvData, 0x00, sizeof(szRecvData));
		memcpy(szSendData,"\x80\x5c\x00\x02\x04",5);
		iSendLen = 5;
		iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
		if ((iRet ==MIF_RET_SUCCESS) && (szRecvData[iRecvLen-2] == 0x90 && szRecvData[iRecvLen-1]== 0x00))
		{
			memcpy(sBlance, szRecvData, 4);
			gstYktEnrichLog.ulOldBalaceAmt=sBlance[0]*256*256*256+sBlance[1]*256*256+
			sBlance[2]*256+sBlance[3];

		}
		else
		{
		    lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "余额查询错");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		}
		
		//---------------------
//NacTxd("0019", 4);
//NacTxd(szRecvData,iRecvLen);
		return OK;

}

    //复位住建部psam卡指令
	UINT8 ResetPsam2(void)
	{
		int    iRet;
		
		iRet = SocketSet(1, 0);
		if( iRet!=0 )
		{
			return NO_DISP; 	
		}
	
		// SAM卡上电复位
		iRet = SamReset(1, NULL);
		if( iRet==1 )
		{
			return NO_DISP; 	 
		}
		return 0;
	}



UINT8 CPU_ReadSaleCard(pEtcardInParam_t  pInParam)
{
	//	int   iOldTime,iNowTime;
	int   iRet;
//	int   iDatalen=0;
	//char  szBuffer[300];
	//UINT8 sDataIn[300];
	//UINT8 sRandom[8];
	//UINT8 sEncrypt_Random[8];
	//APDU_SEND stApduSend;
	//APDU_RESP stApduResp;
    //	UINT8    sKEY1[8];
	//UINT8	sKEY2[8];
	//--
	int        iSendLen, iRecvLen;//iRet,
	UINT8 szSendData[300], szRecvData[300];
	UINT8 sBlance[10];
    //	int lValue;
	/////////////////选择公共钱包:\x00\xA4\x04\x00\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01/////
	iSendLen = 0;
    iRecvLen = 0;  
	memset(szSendData, 0x00, sizeof(szSendData));
    memset(szRecvData, 0x00, sizeof(szRecvData));
	memcpy(szSendData,"\x00\xA4\x04\x00\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01",14);//memcpy(szSendData,"\x00\xA4\x00\x00\x02\x3F\x00\x00",8);
	iSendLen = 14;
	iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
	if ((iRet ==MIF_RET_SUCCESS) && (szRecvData[iRecvLen-2] == 0x90 && szRecvData[iRecvLen-1]== 0x00))
	{
		;
	
	}
	else
   {
        lcdClrLine(2, 7);
	    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "主应用选择错");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}

	////////////////用户卡读0015基本应用数据文件获取应用序列号:\x00\xB0\x96\x00\x1E（从第24位开始，截取16位字符）
//NacTxd("0015", 4);
//NacTxd(szRecvData,iRecvLen);
	    iSendLen = 0;
        iRecvLen = 0;  
	    memset(szSendData, 0x00, sizeof(szSendData));
        memset(szRecvData, 0x00, sizeof(szRecvData));
	    //memcpy(szSendData,"\x00\xB0\x96\x00\x1E",5);
		memcpy(szSendData,"\x00\xB0\x95\x00\x00",5);
	    iSendLen = 5;
	    iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
//NacTxd("0016", 4);
//NacTxd(szSendData,iSendLen);
//NacTxd(szRecvData,iRecvLen);

		if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
	    {
		    lcdClrLine(2, 7);
		    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "读消费卡文件错");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		// return ERR_CODE_USECARD_SELECT_ADF;
	    }
		//gstYktEnrichLog.uCardtype=szRecvData[30];
		//memcpy(gstYktEnrichLog.sCardSerialNo,&szRecvData[12],8);//卡应用序列号 
		PubBcd2Asc(&szRecvData[12],8,gstYktEnrichLog.sCardSerialNo);//卡应用序列号 

		iSendLen = 0;
        iRecvLen = 0;  
	    memset(szSendData, 0x00, sizeof(szSendData));
        memset(szRecvData, 0x00, sizeof(szRecvData));
	    memcpy(szSendData,"\x00\xB0\x85\x00\x00",5);
	    iSendLen = 5;
	    iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
//NacTxd("0016", 4);
//NacTxd(szSendData,iSendLen);
//NacTxd(szRecvData,iRecvLen);
//NacTxd(gstYktEnrichLog.sCardSerialNo,20);

		if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
	    {
		    lcdClrLine(2, 7);
		    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "读消费卡文件错");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		// return ERR_CODE_USECARD_SELECT_ADF;
	    }
		gstYktEnrichLog.uCardtype=szRecvData[0];
 

/*			
//判断启用标志
       
	   	if(szRecvData[37]==0x01)
	   	{
			; 
		}
		else if(szRecvData[37]==0x02)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT, "卡片状态:");
			lcdDisplay(0, 5, DISP_CFONT, "停用");
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP;

		}
		else if(szRecvData[37]==0x00)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT, "卡片状态:");
			lcdDisplay(0, 5, DISP_CFONT, "未启用");
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP;

		}
		else
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT, "卡片状态:");
			lcdDisplay(0, 5, DISP_CFONT, "未知");
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP;
		}

		
		PubAsc2Bcd(pInParam->szDate,8,szBuffer);
		if (memcmp(&szRecvData[33],szBuffer,4)<0 )
	   	{
		   lcdClrLine(2, 7);
		   lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "年检日期已过期");
		   lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "请年检");
		   PubBeepErr();
		   PubWaitKey(3);
		   return NO_DISP; 
	    }
		 	
		if (memcmp(&szRecvData[24],szBuffer,4)<0 )
	   	{
		   lcdClrLine(2, 7);
		   lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "应用有效期已过");
		  // lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "请年检");
		   PubBeepErr();
		   PubWaitKey(3);
		   return NO_DISP; 
	    }
	   	PubBcd2Asc(&szRecvData[12],8,gstYktEnrichLog.sCardSerialNo);
	    //PubBcd2Asc(&szRecvData[12],8,PosCom.stTrans.sCardSerialNo);
	//   memcpy(PosCom.stTrans.SCardSerialNo,&szRecvData[12],8);//UINT8 SCardSerialNo[8];//卡应用序列号 
	 //  gstYktEnrichLog.uCardtype=szRecvData[8];
*/

		//----选择1001  目录
	/*	iSendLen = 0;
        iRecvLen = 0;  
	    memset(szSendData, 0x00, sizeof(szSendData));
        memset(szRecvData, 0x00, sizeof(szRecvData));
	    memcpy(szSendData,"\x00\xA4\x00\x00\x02\x10\x01\x00",8);
	    iSendLen = 8;
	    iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
	    if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
	    {
	     lcdClrLine(2, 7);
	    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "目录选择错");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
		 //return ERR_CODE_USECARD_SELECT_ADF;
	    }*/
		 
		//-------读年检日期-------------------------
	/*	iSendLen = 0;
        iRecvLen = 0;  
	    memset(szSendData, 0x00, sizeof(szSendData));
        memset(szRecvData, 0x00, sizeof(szRecvData));
	    memcpy(szSendData,"\x00\xb0\x9A\x01\x04",5);
	    iSendLen = 5;
	    iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
	    if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
	    {
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "读年年检日期错");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		// return ERR_CODE_USECARD_SELECT_ADF;
	    }
		//---验证年检---------
         PubAsc2Bcd(pInParam->szDate,8,szBuffer);
	   if (memcmp(szRecvData,szBuffer,4)<0 )
	   	{
		   lcdClrLine(2, 7);
		   lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "年检日期已过期");
		   lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "请年检");
		   PubBeepErr();
		   PubWaitKey(3);
		   return NO_DISP; 


	    }*/
		//----------------
//		NacTxd("0017", 4);
//		NacTxd(szRecvData,iRecvLen);//
		//memcpy(pOutParam->CardInfo.AppInfo.AppInsideNum, &szRecvData[12], 8);//保存票卡卡号

		//----密码验证-----------
	/*	iSendLen = 0;
		iRecvLen = 0;  
		memset(szSendData, 0x00, sizeof(szSendData));
		memset(szRecvData, 0x00, sizeof(szRecvData));
		memcpy(szSendData,"\x00\x20\x00\x00\x04\x47\x54\x43\x43",9);
		iSendLen = 9;
		iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
		if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
		{
		   lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "密码校验错");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		  // return ERR_CODE_VERIFY_PIN;
		}*/
//		NacTxd("0018", 4);
//		NacTxd(szRecvData,iRecvLen);
		//---余额查询
		iSendLen = 0;
		iRecvLen = 0;	
		memset(szSendData, 0x00, sizeof(szSendData));
		memset(szRecvData, 0x00, sizeof(szRecvData));
		memcpy(szSendData,"\x80\x5C\x00\x02\x04",5);
		iSendLen = 5;
		iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
		if ((iRet ==MIF_RET_SUCCESS) && (szRecvData[iRecvLen-2] == 0x90 && szRecvData[iRecvLen-1]== 0x00))
		{
			memcpy(sBlance, szRecvData, 4);
			gstYktEnrichLog.ulOldBalaceAmt=sBlance[0]*256*256*256+sBlance[1]*256*256+
			sBlance[2]*256+sBlance[3];

		}
		else
		{
		    lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "余额查询错");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		}
		
		//---------------------
//NacTxd("0019", 4);
//NacTxd(szRecvData,iRecvLen);

		return OK;
}


INT32 M1_ReadCard(void)
{
    UINT8 szAmount[20];
	int iCardInfoLen,iRet;
	UINT8 sCardInfo[30];
//	UINT8 sCsn[4];
	UINT8 sSect1KeyA[6];
	//UINT8 sSect1KeyB[6];
	UINT8 sSectorData[50];
	UINT8 sCityCode[2];
	UINT8 sTrace[4];
	UINT8 sMac[4];
//	UINT8 szPsamId[6];
	UINT8 ucRet;
	int  iOldTime,iNowTime;
	MIF_TIME_STRUCT stMifTimeInfo;
	unsigned char szCommand[200];
	int keyType,i;
	EtcardInParam_t 	stInParam;

	
	PubDisplayTitle(TRUE, "读  卡");
	//iRet =ResetPsam(ICC_SAM1, ICC_ISO);//复位psam卡,里面提示错误
	iRet =ResetPsam2();
	keyType=-1;
	if(iRet !=0)
	{
		iRet =ResetPsam();
		keyType=0;
	}
	if(iRet !=0)
	{
		lcdClrLine(2, 7);
		PubDispString("SAM上电复位失败", 3|DISP_LINE_CENTER);
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
	/*iRet = GetPsamId(gstYktEnrichLog.sPsamId);
	if(iRet !=0)
	{
		return NO_DISP;
	}*/
	iOldTime = sysGetTimerCount();
	mifClose();
	mifOpen();
	while(1)
    {
       iNowTime= sysGetTimerCount();
	   if( (iNowTime-iOldTime) >= (UINT32)(stPosParam.ucOprtLimitTime*1000) )	// 检查定时器
	   {
			//iRet = ERR_COMM_TIMEOUT;
			 return E_TRANS_CANCEL;
	   }
	    if (gstPosCapability.uiUnContactlessCard)
		{
            mifOpen();
			mifGetTimeOut(&stMifTimeInfo);
			stMifTimeInfo.WaitTime_PollTypeB *= 5;
			stMifTimeInfo.WaitTime_PollTypeA *= 5;
 			stMifTimeInfo.WaitTime_RATS *= 20;
			mifSetTimeOut(&stMifTimeInfo);

			memset(sCardInfo, 0x00, sizeof(sCardInfo));
			memcpy(szCommand,"\x00\xFF\xFF\x01\x00",5);
            // 在感应区内查找指定调制类型的卡片，并返回卡片的序列号等相关信息
			iRet = InListPassiveTarget(0,0,szCommand,&iCardInfoLen, sCardInfo);

			if (iRet == MIF_RET_SUCCESS)
			{
				break;
			}
		}
	   if( kbhit()==YES )
	   {
		   iRet = kbGetKey();
		   if( iRet==KEY_CANCEL )
		   {
			   return E_TRANS_CANCEL;
		   }
	   }
   }
	/*
	while(1)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "请刷卡");
		iNowTime= sysGetTimerCount();
		if( (iNowTime-iOldTime) >= (UINT32)(stPosParam.ucOprtLimitTime*1000) )	// 检查定时器
		{
			//iRet = ERR_COMM_TIMEOUT;
			 return E_TRANS_CANCEL;
		}
		
		iRet =ChkContactlessCardPad(&iCardInfoLen, sCardInfo);
		if (0 == iRet)
		{
			break;
		}
		//M1_DispErrInfo(iRet);
		if (M1_ERR_USERCANCEL == iRet)
		{
			mifClose();
			return NO_DISP;
		}
		mifResetRF(100);
	}
	*/
	memcpy(stTemp.sCardUid,sCardInfo+5, 4);

	if(memcmp(&sCardInfo[3],"\x28",1)==0 || memcmp(&sCardInfo[3],"\x20",1)==0){// CPU卡充值
		iRet = InATR(&iCardInfoLen,sCardInfo);
		if (iRet != 0)
		{
			return E_TRANS_FAIL;
		}
		stInParam.ucSwipedFlag=CARD_PASSIVE;
		GetPosTime(stInParam.szDate, stInParam.szTime);	
		PubAsc2Bcd(stInParam.szDate,8,&gstYktEnrichLog.sTranDateTime[0]);
		PubAsc2Bcd(stInParam.szTime,6,&gstYktEnrichLog.sTranDateTime[4]);
	
		ucRet = CPU_ReadSaleCard(&stInParam);// 处理CPU卡文件

		if(ucRet !=OK)
		{
		   return NO_DISP;
		}
		
	}else if(memcmp(&sCardInfo[3],"\x08",1)==0){//M1卡充值
		memcpy(sSect1KeyA,"\xA0\xA1\xA2\xA3\xA4\xA5",6);
		memcpy(&stTemp.sKeyBList ,sSect1KeyA,6);
		ucRet =M1_ReadSector(0, sSect1KeyA,MIF_KEYA, sSectorData,stTemp.sCardUid);//返回48字节
		if( ucRet!=OK )
		{
		  return E_TRANS_FAIL;
		}
		memcpy(stTemp.sCardDataList,sSectorData,48);
	//NacTxd("9001", 4);
	//NacTxd(sSectorData, 48);
		//----------------
		memcpy(sSect1KeyA, stTemp.sCardUid, 4);
		memcpy(sSect1KeyA+4, stTemp.sCardUid, 2);
		memset(sSectorData, 0, sizeof(sSectorData));
		memcpy(&stTemp.sKeyBList[1*6] ,sSect1KeyA,6);
	   // NacTxd("9001", 4);
		ucRet =M1_ReadSector(1, sSect1KeyA,MIF_KEYA, sSectorData,stTemp.sCardUid);//返回48字节
		if(ucRet !=0)
		{
			return NO_DISP;
		}
		memcpy(stTemp.sCardDataList[4],sSectorData,48);
	//NacTxd("9001", 4);
	//NacTxd(sSectorData, 48);
	
		//保存临时变量
		memcpy(sCityCode,&sSectorData[0],2);//
		memcpy(sTrace,&sSectorData[4],4);// liu shui
		memcpy(sMac,&sSectorData[8],4);//ren zhen ma
		PubBcd2Asc(&sSectorData[0],8,gstYktEnrichLog.sCardSerialNo);
		gstYktEnrichLog.uCardtype=sSectorData[13];//卡类型
	//NacTxd(sMac, 8);
	
	//sSectorOper	
	//	memset(sAllKeyB,0,sizeof(sAllKeyB));
		if(keyType==0)//卡槽1上的小卡
		{
			iRet= GetSecPW(stTemp.sCardUid, sTrace, sMac, sCityCode, "\x00\x01\x10\x03\x03\x03\x06\x11\x12\x13\x14\x15\x16\x17\x18\x19","1111111110000000", 1, stTemp.sKeyBList);
			if(iRet !=0)
			{
				return NO_DISP;
			}
		//NacTxd("0008", 4);
		//NacTxd(stTemp.sKeyBList, 6*16);
			iRet = GetAllDataFromM1Card();//获取卡内所有数据
			if(iRet != 0)
			{
				return iRet;
			}
		//NacTxd("0009", 4);
			iRet = M1_IsCardInfoLegal();//判断卡内数据的合法性
			if(iRet != 0)
			{
				return iRet;
			}
		}
		
		else{  //卡槽2上的小卡
			iRet= M1_GetCalKeyA(stTemp.sCardUid, sTrace, sMac, sCityCode, "\x00\x01\x10\x03\x03\x03\x06\x11\x12\x13\x14\x15\x16\x17\x18\x19","1111111110000000", stTemp.sKeyBList);
			if(iRet !=0)
			{
				return NO_DISP;
			}
		//NacTxd("1008", 4);
		//NacTxd(stTemp.sKeyBList, 6*16);
			
			//获取卡内所有数据
			for(i=0;i<=8;i++)
			{
				memset(sSectorData,0,sizeof(sSectorData));//stTemp.sKeyBList
				ucRet =M1_ReadSector(i, &stTemp.sKeyBList[6*i],MIF_KEYA, sSectorData,stTemp.sCardUid);//返回48字节
				//NacTxd(&i, 4);
				if(ucRet !=0)
				{
					return NO_DISP;
				}
				memcpy(stTemp.sCardDataList[i*4],sSectorData,48);
		//NacTxd(stTemp.sCardDataList[i*4],48);
			}
				
			iRet = M1_IsCardInfoLegal();//判断卡内数据的合法性
			if(iRet != 0)
			{
				return iRet;
			}
			memset(stTemp.sCardDataListSub, 0, sizeof(stTemp.sCardDataListSub));
			memcpy(stTemp.sCardDataListSub, stTemp.sCardDataList, 64*16);
		}
		
		//---- 卡类别stTemp.sCardDataList[4][13]
		gstYktEnrichLog.ulOldBalaceAmt=stTemp.sCardDataList[2*4+1][0]+stTemp.sCardDataList[2*4+1][1]*256
		+stTemp.sCardDataList[2*4+1][2]*256*256+stTemp.sCardDataList[2*4+1][3]*256*256*256;
		LongToAmount(szAmount, gstYktEnrichLog.ulOldBalaceAmt);

	}
	
	LongToAmount(szAmount, gstYktEnrichLog.ulOldBalaceAmt);	
//	gstYktEnrichLog.ulMaxBalaceAmt=stTemp.sCardDataList[2*4][4]*256*256*256+stTemp.sCardDataList[2*4][5]*256*256
//	+stTemp.sCardDataList[2*4][6]*256+stTemp.sCardDataList[2*4][7];
	//	gstYktEnrichLog.ulMaxBalaceAmt=stTemp.sCardDataList[2*4][4]+stTemp.sCardDataList[2*4][5]*256
	//	+stTemp.sCardDataList[2*4][6]*256*256+stTemp.sCardDataList[2*4][7]*256*256*256;
	//gstYktEnrichLog.ulMaxBalaceAmt=gstYktEnrichLog.ulMaxBalaceAmt*100;
	gstYktEnrichLog.ulMaxBalaceAmt= stPosParam.ulMaxBalaceAmt*100;

	lcdClrLine(2,7);
	lcdDisplay(0, 2, DISP_CFONT, "余额:%s", szAmount);
	LongToAmount(szAmount, gstYktEnrichLog.ulMaxBalaceAmt);
	lcdDisplay(0, 6, DISP_CFONT, "限额:%s", szAmount);
	
    if(gstYktEnrichLog.uCardtype==0x00)
	{
		lcdDisplay(0, 4, DISP_CFONT, "卡类型:普通卡");
	}
	 else if(gstYktEnrichLog.uCardtype==1)
	{
		lcdDisplay(0, 4, DISP_CFONT, "卡类型:老人卡 A");
	}
	 else if(gstYktEnrichLog.uCardtype==2)
	{
		lcdDisplay(0, 4, DISP_CFONT, "卡类型:老人卡 B");
	}
	 else if(gstYktEnrichLog.uCardtype==3)
	{
		lcdDisplay(0, 4, DISP_CFONT, "卡类型:学生卡 A");
	}
	 else if(gstYktEnrichLog.uCardtype==4)
	{
		lcdDisplay(0, 4, DISP_CFONT, "卡类型:学生卡 B");
	}
	else if(gstYktEnrichLog.uCardtype==5)
	{
		lcdDisplay(0, 4, DISP_CFONT, "卡类型:预留卡 A");
	}
	else if(gstYktEnrichLog.uCardtype==6)
	{
		lcdDisplay(0, 4, DISP_CFONT, "卡类型:预留卡 B");
	}
	else if(gstYktEnrichLog.uCardtype==7)
	{
		lcdDisplay(0, 4, DISP_CFONT, "卡类型:CPU卡");
	}
	else if(gstYktEnrichLog.uCardtype==8)
	{
		lcdDisplay(0, 4, DISP_CFONT, "卡类型:员工卡");
	}
	else if(gstYktEnrichLog.uCardtype==9)
	{
		lcdDisplay(0, 4, DISP_CFONT, "卡类型:爱心卡");
	}
	else if(gstYktEnrichLog.uCardtype==10)
	{
		lcdDisplay(0, 4, DISP_CFONT, "卡类型:优待卡");
	}
	else if(gstYktEnrichLog.uCardtype==11)
	{
		lcdDisplay(0, 4, DISP_CFONT, "卡类型:集团卡");
	}
	else if(gstYktEnrichLog.uCardtype==12)
	{
		lcdDisplay(0, 4, DISP_CFONT, "卡类型:福利卡");
	}
	else
	{
		lcdDisplay(0, 4, DISP_CFONT, "卡类型:未知定义");

	}
    
   kbGetKeyMs(10*1000);
	return 0;
}
//-----查询当天结算---
uint8_t M1_Que_Sett(void)
{
	//----
	INT32 iTotalNum,lCurIndex;
	INT32  iTranNum,iTotalTranAmt,iTotalSaleAmt;
    INT32	 iCnt,i;//, iRet
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    交易结算    ", "  QUERY TRANS   ");
	kbFlush();
	// PubDisplayTitle(TRUE,"交易明细查询");
	//memset(szDisp, 0, sizeof(szDisp));
	lCurIndex = 0;
	iCnt=0;
	iTranNum=0;
	iTotalTranAmt=0;
	iTotalSaleAmt=0;
	 memset(&gstYktEnrichLog, 0, sizeof(gstYktEnrichLog));
	iTotalNum= ReadYtkEnrichLog(&iCnt);
	if( iTotalNum==0 )
	{
	   lcdCls();
	   PubDisplayTitle(TRUE,"当批交易查询");
	   PubDispString("无交易", 3|DISP_LINE_CENTER);
	   PubWaitKey(10);
	   return 0;
	}
	for(i=0;i<iTotalNum;i++)
	{
	  ReadYtkEnrichLog(&iCnt);
	  if(gstYktEnrichLog.uiStatus==0x01)//结算了
	  {
		  iCnt--;
		  if (iCnt == 0)
			 {
				 iCnt=iTotalNum;
 
			 }
		 // iCnt=iCnt%iTotalNum;
	  }
	  else
	  {
	    iTranNum++;
		//if(gstYktEnrichLog.uMode==0)
		//{
		//	iTotalTranAmt=iTotalTranAmt+ gstYktEnrichLog.ulTransAmt;
		//}else
		//{
		//	iTotalSaleAmt=iTotalSaleAmt+ gstYktEnrichLog.ulTransAmt;
		//}
		iCnt--;
		if (iCnt == 0)
		{
		 iCnt=iTotalNum;

		}
	  }
	}

	if( iTranNum==0 )
	{
	   lcdCls();
	   PubDisplayTitle(TRUE,"当批交易查询");
	   PubDispString("无交易", 3|DISP_LINE_CENTER);
	   PubWaitKey(10);
	   return 0;
	}
	
	lcdClrLine(2,7);
	lcdDisplay(0, 2, DISP_CFONT, "交易笔数: %d", iTranNum);
	lcdDisplay(0, 4, DISP_CFONT, "充值金额: %d.%02d", iTotalTranAmt/100,iTotalTranAmt%100);
	lcdDisplay(0, 6, DISP_CFONT, "消费金额: %d.%02d", iTotalSaleAmt/100,iTotalSaleAmt%100);
	kbGetKeyMs(10*1000);
	kbFlush();
    return 0;
}
UINT8 GetEnrichFileName(UINT8 *pFileName)
{
	pFileName[0]='N';
	memcpy(&pFileName[1],stPosParam.sM1TellNo,8);
	GetPosTime(&pFileName[9],&pFileName[9+8]);
	memcpy(&pFileName[9+14],".ss",3);
	return 0;


}
UINT8 PackFTP(YKTENRICH_LOG* PLog,UINT8* Pfield,int *piCnt,UINT8* szdatatime)
{
	int iCnt=0;
	UINT8 szBuffer[30];
	int  istrlen;
	int i;
	//NacTxd("0001",4);
	memcpy(&Pfield[iCnt],PLog->sCardSerialNo,16); //卡号
	iCnt+=16;
	Pfield[iCnt]=',';
	iCnt+=1;
	//NacTxd("0002",4);
	
	/*/---------------------
	memset(szBuffer,0,sizeof(szBuffer));
	sprintf(szBuffer, "%ld",PLog->ulPOSTraceNo);
	istrlen=strlen(szBuffer);
	memcpy(&Pfield[iCnt],szBuffer,istrlen);
	iCnt+=istrlen;
	Pfield[iCnt]=',';
	iCnt+=1;
	//NacTxd("0003",4);
	memcpy(&Pfield[iCnt],PLog->sCSN,10);
	iCnt+=10;
	Pfield[iCnt]=',';
	iCnt+=1;
	*/
	//NacTxd("0004",4);
	memset(szBuffer,0,sizeof(szBuffer));
	sprintf(szBuffer, "%d",PLog->uCardtype); //卡类型
	istrlen=strlen(szBuffer);
	memcpy(&Pfield[iCnt],szBuffer,istrlen);
	iCnt+=istrlen;
	Pfield[iCnt]=',';
	iCnt+=1;

	memset(szBuffer,0,sizeof(szBuffer));
	sprintf(szBuffer, "%d",PLog->uMode); //充值或消费
	istrlen=strlen(szBuffer);
	memcpy(&Pfield[iCnt],szBuffer,istrlen);
	iCnt+=istrlen;
	Pfield[iCnt]=',';
	iCnt+=1;
	
	//NacTxd("0005",4);
	memset(szBuffer,0,sizeof(szBuffer));
	/*sprintf(szBuffer, "%4.4s/%2.2s/%2.2s:%2.2s/%2.2s", PLog->sTranDateTime, YktRecord.szDate+4, YktRecord.szDate+6,
			YktRecord.szTime, YktRecord.szTime+2);*/
	sprintf((char *)szBuffer, "%02X%02X-%02X-%02X %02X:%02X:%02X",
		PLog->sTranDateTime[0], PLog->sTranDateTime[1],
		PLog->sTranDateTime[2], PLog->sTranDateTime[3], 
		PLog->sTranDateTime[4], PLog->sTranDateTime[5],
		PLog->sTranDateTime[6]);
	istrlen=strlen(szBuffer);
	//NacTxd("1005",4);
	memcpy(&Pfield[iCnt],szBuffer,istrlen);
	iCnt+=istrlen;
	Pfield[iCnt]=',';
	iCnt+=1;

	memset(szBuffer,0,sizeof(szBuffer));//充值金额
	sprintf(szBuffer, "%ld",PLog->ulTransAmt);
	istrlen=strlen(szBuffer);
	memcpy(&Pfield[iCnt],szBuffer,istrlen);
	iCnt+=istrlen;
	Pfield[iCnt]=',';
	iCnt+=1;
	
	//NacTxd("0009",4);
	memset(szBuffer,0,sizeof(szBuffer));//充值后金额
	sprintf(szBuffer, "%ld",PLog->ulNowBalaceAmt);
	istrlen=strlen(szBuffer);
	memcpy(&Pfield[iCnt],szBuffer,istrlen);
	iCnt+=istrlen;
	Pfield[iCnt]=',';
	iCnt+=1;

	//NacTxd("0008",4);
	memset(szBuffer,0,sizeof(szBuffer));//充值前金额
	sprintf(szBuffer, "%ld",PLog->ulOldBalaceAmt);
	istrlen=strlen(szBuffer);
	memcpy(&Pfield[iCnt],szBuffer,istrlen);
	iCnt+=istrlen;
	Pfield[iCnt]=',';
	iCnt+=1;

 	//30 30 31 00 38 31 34 37 39 35 30 39 31 32 33 34 35 30 39 00 00 00 00 00 00 
	//istrlen=strlen(PLog->sTellNo);//操作员号
	//memcpy(&Pfield[iCnt], PLog->sTellNo,istrlen);
	memcpy(&Pfield[iCnt], PLog->sTellNo+1,2);
	iCnt+=2;
	Pfield[iCnt]=',';
	iCnt+=1;

	//NacTxd("1007",4);
	//memcpy(&Pfield[iCnt], PLog->sPsamId,10);//终端号
	memcpy(&Pfield[iCnt], PLog->sPosSN,8);
	iCnt+=8;
	Pfield[iCnt]=',';
	iCnt+=1;

	//NacTxd("1008",4);
	//PubBcd2Asc(&PLog->sNowTellNo[2],2,&Pfield[iCnt]);//线路/ 充值点号
	memcpy(&Pfield[iCnt], PLog->pointofsaleNo,4);
	iCnt+=4;
	Pfield[iCnt]=',';
	iCnt+=1;
	//NacTxd("0007",4);
	
	i = strlen(PLog->sCompanyNo);
	memcpy(&Pfield[iCnt], PLog->sCompanyNo,i);//公司号/ 部门号
	//NacTxd("1006",4);
	//memcpy(&Pfield[iCnt], "0006",4);
	iCnt+=i;
	Pfield[iCnt]=',';
	iCnt+=1;
	
	
	/*/------------------
	//NacTxd("0010",4);
	memset(szBuffer,0,sizeof(szBuffer));
	sprintf(szBuffer, "%4.4s-%2.2s-%2.2s %2.2s:%2.2s:%2.2s", szdatatime, szdatatime+4, szdatatime+6,
			szdatatime+8, szdatatime+10,szdatatime+12);
	istrlen=strlen(szBuffer);
	memcpy(&Pfield[iCnt],szBuffer,istrlen);
	iCnt+=istrlen;
	Pfield[iCnt]=',';
	iCnt+=1;
	*/
	
	//--充值POS流水号--------------
	memset(szBuffer,0,sizeof(szBuffer));
	sprintf(szBuffer, "%d",PLog->lEnrichTraceNo);
	istrlen=strlen(szBuffer);
	memcpy(&Pfield[iCnt],szBuffer,istrlen);
	iCnt+=istrlen;
	Pfield[iCnt]=',';
	iCnt+=1;
	//-----POS充值累积金额-----------------
	memset(szBuffer,0,sizeof(szBuffer));//POS充值累积金额
	if(PLog->uMode==0)
	{
		sprintf(szBuffer, "%ld",PLog->lEnrichCumulativeBalance);
	}else{
		sprintf(szBuffer, "%ld",PLog->lSaleCumulativeBalance);
	}
	istrlen=strlen(szBuffer);
	memcpy(&Pfield[iCnt],szBuffer,istrlen);
	iCnt+=istrlen;
	Pfield[iCnt]=',';
	iCnt+=1;
	Pfield[iCnt]='\n';
	iCnt+=1;
	*piCnt=iCnt;
	return 0;

}
//-----查找可存取的节点。

int  FindEnrichStoreFilePosition(void)
{
  int i;
   for(i=0;i<50;i++)
   {
		if(stTransCtrl.stEnrichFile[i].uflag!=1)
		{
			return i;
		}
			
   }
   return -1;

}

//----充值点结算，生成结算记录-
uint8_t M1_Sett(void)
{
	INT32  iTotalNum,lCurIndex;
	INT32  iTranNum, enrichNum, saleNum;
	INT32  iTotalTranAmt,iTotalSalAmt;
	INT32  iCnt, iRet,i;
	UINT8  szFileName[30];
	UINT8  sztime[20];
	UINT8  szEnrichdata[500];
	int    iEnrichDatalen;
	UINT8  uRet;
	INT32  fileLen;
		
	memset(sztime,0,sizeof(sztime));
	memset(szFileName,0,sizeof(szFileName));
	GetEnrichFileName(szFileName);
	memcpy(sztime,szFileName+9,14);
//	prnInit();
//	prnPrintf("%s",szFileName);
//	prnStart();
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    交易结算    ", "  QUERY TRANS   ");
	kbFlush();
	// PubDisplayTitle(TRUE,"交易明细查询");
	//memset(szDisp, 0, sizeof(szDisp));
	lCurIndex = 0;
	iCnt=0;
	iTranNum=0;
	enrichNum=0;
	saleNum=0;
	iTotalTranAmt=0;
	iTotalSalAmt=0;
	
	memset(&gstYktEnrichLog, 0, sizeof(gstYktEnrichLog));
	iTotalNum= ReadYtkEnrichLog(&iCnt);
	if( iTotalNum==0 )
	{
	  lcdCls();
	   PubDisplayTitle(TRUE,"当批交易查询");
	   PubDispString("无交易", 3|DISP_LINE_CENTER);
	   PubWaitKey(10);
	   return NO_DISP;
	}

	//-----查找可存取的节点�
	iRet=FindEnrichStoreFilePosition();
	if(iRet<0)
	{
	 
	   lcdClrLine(2,7);
	   lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "请先数据上传");
	   lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "再结算");
	   PubWaitKey(10);
	   return NO_DISP;

	}
	 
	for(i=0;i<iTotalNum;i++)
	{
		ReadYtkEnrichLog(&iCnt);
	    if(gstYktEnrichLog.uiStatus==0x01)//结算了
		{
			iCnt--;
			if (iCnt == 0)
			{
				iCnt=iTotalNum;
	 		}
			 // break;   //注意，很有可能有问题
			 // iCnt=iCnt%iTotalNum;
		 }
		 else
		 {
			
			//---打包----
			memset(szEnrichdata,0,sizeof(szEnrichdata));
			PackFTP(&gstYktEnrichLog,szEnrichdata,&iEnrichDatalen,sztime);
			uRet= WriteEnrichFtpFile(szEnrichdata,iEnrichDatalen, sztime);//这里sztime是存储在POS里面的文件名。
			if(uRet==OK)
			{
				//更新日志
				iTranNum++;
				if(gstYktEnrichLog.uMode==0)
				{
					iTotalTranAmt=iTotalTranAmt+ gstYktEnrichLog.ulTransAmt;
					enrichNum++;
				}
				else
				{
					iTotalSalAmt=iTotalSalAmt+ gstYktEnrichLog.ulTransAmt;
					saleNum++;
				}

				gstYktEnrichLog.uiStatus=1;
				PubAsc2Bcd(sztime,14,gstYktEnrichLog.sLogDateTime);
				iRet = SaveM1EnrichLog(iCnt);

				if( iRet!=OK )
				{
					lcdClrLine(2, 7);
					DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "保存文件操作失败", "FILE ERROR");
					ErrorBeep();
					kbGetKeyMs(2000);
					break;
				}

			}

			iCnt--;
			if (iCnt == 0)
			{
			 iCnt=iTotalNum;
			}
		}
	}
	
    iRet =fileExist(sztime);
	if(iRet >= 0)
	{
	   fileLen = fileSize(sztime);
	    if(fileLen>0)
	    {
		   iRet=FindEnrichStoreFilePosition();
		   
		   if(iRet >=0)
		   {
		   
			memcpy(stTransCtrl.stEnrichFile[iRet].szLocalfile,sztime,strlen(sztime));
			memset(stTransCtrl.stEnrichFile[iRet].szRemotefile,0,sizeof(stTransCtrl.stEnrichFile[iRet].szRemotefile));

			memcpy(stTransCtrl.stEnrichFile[iRet].szRemotefile,szFileName,strlen(szFileName));
		    sprintf(&stTransCtrl.stEnrichFile[iRet].szRemotefile[strlen(szFileName)-3],"_%d.ss",iTranNum);
			stTransCtrl.stEnrichFile[iRet].uflag=1;
			stTransCtrl.stEnrichFile[iRet].iRmbM1EnrichAmt=iTotalTranAmt;
			stTransCtrl.stEnrichFile[iRet].iRmbM1SaleAmt=iTotalSalAmt;
			stTransCtrl.stEnrichFile[iRet].iRmbM1EnrichNum=enrichNum;
			stTransCtrl.stEnrichFile[iRet].iRmbM1SaleNum=saleNum;
			stTransCtrl.stEnrichFile[iRet].u8Settlemode=1;
			SaveCtrlParam();

			stPosParam.lUnSettTraceNum=0;
			stPosParam.lUnSettFileNum++;
			SaveAppParam();
		   }
	     }
		else
		{
		 fileRemove(sztime);

		}
	}
	memset(&stM1EnrichTotal,0,TOTAL_M1ENRICH_STRC_SIZE);
	stM1EnrichTotal.iRmbM1EnrichNum=enrichNum;
	stM1EnrichTotal.iRmbM1EnrichAmt=iTotalTranAmt;
	stM1EnrichTotal.iRmbM1SaleNum=saleNum;
	stM1EnrichTotal.iRmbM1SaleAmt=iTotalSalAmt;
	//memcpy(stM1EnrichTotal.pointofsaleNo,,3);
	memcpy(stM1EnrichTotal.sDateTime,sztime,14);
	memcpy(stM1EnrichTotal.sM1TellNo,stPosParam.sM1TellNo,strlen(stPosParam.sM1TellNo));
	memcpy(stM1EnrichTotal.szCompanyNo,stPosParam.szCompanyNo,strlen(stPosParam.szCompanyNo));
	memcpy(stM1EnrichTotal.pointofsaleNo,stPosParam.pointofsaleNo,strlen(stPosParam.pointofsaleNo));
	stM1EnrichTotal.u8Settlemode=1;

	if(iTranNum==0){
		lcdCls();
    	PubDisplayTitle(TRUE,"当批交易结算");
   		PubDispString("无交易", 3|DISP_LINE_CENTER);
   		PubWaitKey(10);
    	return NO_DISP;
	}

	SaveM1EnrichLastTotal();
	PrtM1EnrichTotal(1);
	
	lcdCls();
    PubDisplayTitle(TRUE,"交易结算");
	lcdDisplay(0, 2, DISP_CFONT, "交易笔数: %d", iTranNum);
	lcdDisplay(0, 4, DISP_CFONT, "充值金额: %d.%02d", iTotalTranAmt/100,iTotalTranAmt%100);
	lcdDisplay(0, 6, DISP_CFONT, "消费金额: %d.%02d", iTotalSalAmt/100,iTotalSalAmt%100);
	kbGetKeyMs(10*1000);
	kbFlush();

	M1_Data_UpLoad_TCP();
	
	return 0;

}
//--结算打印----
uint8_t M1_Sett_Prt(void)
{
	 ReadM1EnrichLastTotal();
 	if( stM1EnrichTotal.iRmbM1EnrichNum !=0 || stM1EnrichTotal.iRmbM1SaleNum != 0)
	{
 		PrtM1EnrichTotal(0);
	}
	else
	{
		lcdCls();
	    PubDispString("无数据", 3|DISP_LINE_CENTER);
	    PubWaitKey(10);
	    return NO_DISP;
	}
 	return 0;
}
int GetFtpFiletotal(void)
{
	int i;
	int j=0;
	for(i=0;i<50;i++)
	{
		 if(stTransCtrl.stEnrichFile[i].uflag==1)
		 {
			// return i;
			j++;
		 }
			 
	}
	return j;

}

int GetFtpFilePosition(void)
{
   int i;
   for(i=0;i<50;i++)
   {
		if(stTransCtrl.stEnrichFile[i].uflag==1)
		{
			return i;
		}
			
   }
   return -1;
}
//---数据上传---------------
uint8_t M1_Data_UpLoad_TCP(void)
{
	FTP_PARA stFtpPara;
	int iRet,fd;
	int iposition;
	uint8_t Ret;
	int itotalFile;
	int ioffSet=0;//偏移量
	int iFileSize=0;//文件大小
	int iCnt;   //
	int iMsgSN=1; //数据报序列号
	int iReadLen;
	uint8_t szReadbuf[2048];
	uint8_t utemp;
	int  iuploadCnt=0;

	memset(&stFtpPara, 0, sizeof(FTP_PARA));
	itotalFile=GetFtpFiletotal();

	iRet=GetFtpFilePosition();
	if(iRet<0)
	{//没有上传的数据
		lcdClrLine(0,7);
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "无数据上传");
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请先结算");
		PubWaitKey(10);
		return NO_DISP;
	}
	iposition=iRet;
	strcpy(stFtpPara.szLocalfile,stTransCtrl.stEnrichFile[iposition].szLocalfile);
	sprintf(stFtpPara.szRemotefile, "%s",  stTransCtrl.stEnrichFile[iposition].szRemotefile);

	iFileSize=fileSize(stTransCtrl.stEnrichFile[iposition].szLocalfile);
	if(iFileSize<0)
	{
		lcdDisplay(0, 4, DISP_MEDIACY|DISP_CFONT, "上传之前错误");
	    DispFileErrInfo();
		return (E_MEM_ERR);
	}
	
	iuploadCnt=0;

	while (ioffSet < iFileSize)
	{
	    if(iuploadCnt>3)
	    {
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "结算错误");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请重试");
			FailBeep();
			kbGetKey();
			return NO_DISP;
		}
		iReadLen = MIN(iFileSize - ioffSet, 800);
		fd = fileOpen(stTransCtrl.stEnrichFile[iposition].szLocalfile, O_RDWR);
		if( fd<0 ) 
		{
			lcdDisplay(0, 4, DISP_MEDIACY|DISP_CFONT, "打开文件错误");
			DispFileErrInfo();
			return (E_MEM_ERR);
		}
		iRet = fileSeek(fd, (int)ioffSet, SEEK_SET);
		if( iRet<0 ) 
		{
			lcdDisplay(0, 4, DISP_MEDIACY|DISP_CFONT, "查看文件错误");
			DispFileErrInfo();
			fileClose(fd);
			return (E_MEM_ERR);
		} 
		memset(szReadbuf,0,sizeof(szReadbuf));
		iRet =fileRead(fd,szReadbuf,iReadLen);
		if( iRet<0 ) 
		{
			lcdDisplay(0, 4, DISP_MEDIACY|DISP_CFONT, "读文件错误");
			DispFileErrInfo();
			fileClose(fd);
			return (E_MEM_ERR);
		}
		fileClose(fd);
		iReadLen=iRet;
		memset(sSendPacketBuff, 0, sizeof(sSendPacketBuff));
	    memset(sRecvPacketBuff, 0, sizeof(sRecvPacketBuff));
	    usRecvPacketLen = 0;
		usSendPacketLen=0;
		iCnt=0;
		memcpy(&sSendPacketBuff[iCnt],"0001",4);
		iCnt+=4;
		memcpy(&sSendPacketBuff[iCnt],stPosParam.sM1TellNo,4);//终端编号
		iCnt+=4;
		memset(&sSendPacketBuff[iCnt],' ',30);
		memcpy(&sSendPacketBuff[iCnt],stTransCtrl.stEnrichFile[iposition].szRemotefile,strlen(stTransCtrl.stEnrichFile[iposition].szRemotefile));//终端编号
	    iCnt+=30;	
		sprintf(&sSendPacketBuff[iCnt], "%016d", iFileSize);
		iCnt+=16;
		if(ioffSet+iReadLen<iFileSize)//还有
        {
			sSendPacketBuff[iCnt]='1';
		}
		else
		{
			sSendPacketBuff[iCnt]='9';
		}
		iCnt+=1;
		sprintf(&sSendPacketBuff[iCnt], "%03d", iMsgSN);
		iCnt+=3;
		sprintf(&sSendPacketBuff[iCnt], "%016d", ioffSet);//偏移量
		iCnt+=16;	
		sprintf(&sSendPacketBuff[iCnt], "%04d", iReadLen);//长度
		iCnt+=4;
		memcpy(&sSendPacketBuff[iCnt],szReadbuf,iReadLen);
		iCnt+=iReadLen;
		utemp=PubCalcLRC(sSendPacketBuff,iCnt,0);
		sprintf(&sSendPacketBuff[iCnt],"%03d",utemp);
		iCnt+=3;
		usSendPacketLen=iCnt;
		Ret= SendRecvPacket();
		if(Ret!=0)
		{
		   iuploadCnt++;
		   continue;
		}
		utemp=(uint8_t)PubCalcLRC(sRecvPacketBuff,usRecvPacketLen-1,0);
		if(utemp !=sRecvPacketBuff[usRecvPacketLen-1])
		{
			 iuploadCnt++;
		     continue;
		}
		
		if(memcmp(&sRecvPacketBuff[8],"0000",4)!=0)
		{
			//上传失败 退出
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "结算错误");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请重试");
			FailBeep();
			kbGetKey();
			return NO_DISP;
		}
		if(sRecvPacketBuff[42]=='2')//重传
		{
             iuploadCnt++;
		     continue;
		}
		else if(sRecvPacketBuff[42]=='1')
		{
			if(ioffSet+iReadLen<iFileSize)//还有
		    {
				iuploadCnt=0;
				ioffSet+=iReadLen;
				iMsgSN++;
		    }
			else
			{
				iuploadCnt=0;
				stTransCtrl.stEnrichFile[iposition].uflag=0;
				SaveCtrlParam();
				fileRemove(stTransCtrl.stEnrichFile[iposition].szLocalfile);
				memset(&stFtpPara, 0, sizeof(FTP_PARA));

				iRet=GetFtpFilePosition();
				if(iRet<0)
				{//没有上传的数据
					lcdClrLine(0,7);
					lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "结算文件");
					lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "上传成功");
					OkBeep();
					PubWaitKey(3);
					return NO_DISP;
				}
				iposition=iRet;
				strcpy(stFtpPara.szLocalfile,stTransCtrl.stEnrichFile[iposition].szLocalfile);
				sprintf(stFtpPara.szRemotefile, "%s",  stTransCtrl.stEnrichFile[iposition].szRemotefile);
				iFileSize=fileSize(stTransCtrl.stEnrichFile[iposition].szLocalfile);
				if(iFileSize<0)
				{
					lcdDisplay(0, 4, DISP_MEDIACY|DISP_CFONT, "过程中错误");
					DispFileErrInfo();
					return (E_MEM_ERR);
				}
				iuploadCnt=0;
				ioffSet=0;
				iMsgSN=1;
				continue;
		  }

		}
	}
}

//通过串口上传数据
uint8_t COM_SendRecvData(void)
{
	unsigned long  ulTemp;
	uint8_t buf[10];
	int iRet;
	unsigned int uiBeginTime=0;
	unsigned int uiCurTime= 0;
	int iKey=0;

	lcdClrLine(0,7);
	Rs232Close();
	iRet = Rs232Open(PORT_COM1, (void *)"9600,8,n,1");
	if(iRet != OK)
	{
		DisplayMainPosOperFailed("串口打开失败", 5);
		return NO_DISP;
	}
	
	iRet = Rs232ResetPurge(RS232_TX_PURGE|RS232_RX_PURGE);
	if(iRet != OK)
	{
		DisplayMainPosOperFailed("存在缓存数据", 5);
		return NO_DISP;
	}

	sprintf(buf, "%04d", usSendPacketLen);
	memmove(&sSendPacketBuff[4], sSendPacketBuff, usSendPacketLen);
	memcpy(sSendPacketBuff,buf,4);
	usSendPacketLen += 4;

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

	lcdClrLine(0,7);
	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "正在处理数据");

	memset(sRecvPacketBuff,0,sizeof(sRecvPacketBuff));
	usRecvPacketLen = 0;

	/*iRet = MainPosRS232Rxd_(sRecvPacketBuff, MAX_PACKET_LEN, 60, (unsigned int*)&usRecvPacketLen);
	if (iRet != ERR_RS232_OK)
	{
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "接收数据错 %d", iRet);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "按键返回");
		PubWaitKey(5);
		return NO_DISP;
	}
	*/

	PubChar2Long(sRecvPacketBuff, 2, &ulTemp);
	memmove(sRecvPacketBuff, &sRecvPacketBuff[2], (uint)ulTemp);
	usRecvPacketLen = (uint32_t)ulTemp;

	return OK;	
}


//---数据上传---------------
uint8_t M1_Data_UpLoad_COM(void)
{
	FTP_PARA stFtpPara;
	 int iRet,fd;
	 int iposition;
	 uint8_t Ret;
	 int itotalFile;
	 //int iCurCntFile=0;
	 int ioffSet=0;//偏移量
	 int iFileSize=0;//文件大小
	 int iCnt;   //
	 int iMsgSN=1; //数据报序列号
	 int iReadLen;
	 uint8_t szReadbuf[2048];
	 uint8_t utemp;
	 int  iuploadCnt=0;
	 INT8  serialNo[10];

	memset(&stFtpPara, 0, sizeof(FTP_PARA));
	
	itotalFile=GetFtpFiletotal();
	iRet=GetFtpFilePosition();
	if(iRet<0)
	{//没有上传的数据
		lcdClrLine(0,7);
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "无数据上传");
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请先结算");
		PubWaitKey(10);
		return NO_DISP;
	}
	
	iposition=iRet;
	strcpy(stFtpPara.szLocalfile,stTransCtrl.stEnrichFile[iposition].szLocalfile);
	sprintf(stFtpPara.szRemotefile, "%s",  stTransCtrl.stEnrichFile[iposition].szRemotefile);
	iFileSize=fileSize(stTransCtrl.stEnrichFile[iposition].szLocalfile);
	if(iFileSize<0)
	{
	    DispFileErrInfo();
		return (E_MEM_ERR);
	}

	iuploadCnt=0;
	while (ioffSet < iFileSize)
	{
	    if(iuploadCnt>3)
	    {
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "结算错误");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请重试");
			FailBeep();
			kbGetKey();
			return NO_DISP;
		}
		iReadLen = MIN(iFileSize - ioffSet, 800);
		fd = fileOpen(stTransCtrl.stEnrichFile[iposition].szLocalfile, O_RDWR);
		if( fd<0 ) 
		{
			DispFileErrInfo();
			return (E_MEM_ERR);
		}
		iRet = fileSeek(fd, (int)ioffSet, SEEK_SET);
		if( iRet<0 ) 
		{
			DispFileErrInfo();
			fileClose(fd);
			return (E_MEM_ERR);
		} 
		memset(szReadbuf,0,sizeof(szReadbuf));
		iRet =fileRead(fd,szReadbuf,iReadLen);
		if( iRet<0 ) 
		{
			DispFileErrInfo();
			fileClose(fd);
			return (E_MEM_ERR);
		}
		fileClose(fd);
		iReadLen=iRet;
		memset(sSendPacketBuff, 0, sizeof(sSendPacketBuff));
	    memset(sRecvPacketBuff, 0, sizeof(sRecvPacketBuff));
	    usRecvPacketLen = 0;
		usSendPacketLen=0;
		iCnt=0;
		sysReadSN(serialNo);
		memcpy(&sSendPacketBuff[iCnt],serialNo,8);//终端编号
		iCnt+=8;

		memset(&sSendPacketBuff[iCnt],' ',30);
		memcpy(&sSendPacketBuff[iCnt],stTransCtrl.stEnrichFile[iposition].szRemotefile,strlen(stTransCtrl.stEnrichFile[iposition].szRemotefile));
	    iCnt+=30;	
		sprintf(&sSendPacketBuff[iCnt], "%016d", iFileSize);
		iCnt+=16;
		if(ioffSet+iReadLen<iFileSize)//还有
        {
			sSendPacketBuff[iCnt]='1';
		}
		else
		{
			sSendPacketBuff[iCnt]='9';
		}
		iCnt+=1;
		sprintf(&sSendPacketBuff[iCnt], "%03d", iMsgSN);
		iCnt+=3;
		sprintf(&sSendPacketBuff[iCnt], "%016d", ioffSet);//偏移量
		iCnt+=16;	
		sprintf(&sSendPacketBuff[iCnt], "%04d", iReadLen);//长度
		iCnt+=4;
		memcpy(&sSendPacketBuff[iCnt],szReadbuf,iReadLen);
		iCnt+=iReadLen;
		utemp=PubCalcLRC(sSendPacketBuff,iCnt,0);
		sprintf(&sSendPacketBuff[iCnt],"%03d",utemp);
		iCnt+=3;
		usSendPacketLen=iCnt;
		
		//发送、接收数据
		Ret= COM_SendRecvData(); 
		if(Ret!=0)
		{
		   iuploadCnt++;
		   continue;
		}
		utemp=(uint8_t)PubCalcLRC(sRecvPacketBuff,usRecvPacketLen-1,0);
		if(utemp !=sRecvPacketBuff[usRecvPacketLen-1])
		{
			 iuploadCnt++;
		     continue;
		}
		
		if(memcmp(&sRecvPacketBuff[8],"0000",4)!=0)
		{
               //上传失败 退出
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "结算错误");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请重试");
			FailBeep();
			kbGetKey();
			return NO_DISP;
		}
		
		if(sRecvPacketBuff[42]=='2')//重传
		{
             iuploadCnt++;
		     continue;
		}
		else if(sRecvPacketBuff[42]=='1')
		{
		  if(ioffSet+iReadLen<iFileSize)//还有
		  {
			iuploadCnt=0;
			ioffSet+=iReadLen;
			iMsgSN++;
		  }
		  else
		  {
		      iuploadCnt=0;
		      stTransCtrl.stEnrichFile[iposition].uflag=0;
		      SaveCtrlParam();
			  fileRemove(stTransCtrl.stEnrichFile[iposition].szLocalfile);
             //----------------
             	memset(&stFtpPara, 0, sizeof(FTP_PARA));

				iRet=GetFtpFilePosition();
				if(iRet<0)
				{//没有上传的数据
					lcdClrLine(0,7);
					lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "结算文件");
					lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "上传成功");
					OkBeep();
					PubWaitKey(3);
					return NO_DISP;
				}
				iposition=iRet;
				strcpy(stFtpPara.szLocalfile,stTransCtrl.stEnrichFile[iposition].szLocalfile);
				sprintf(stFtpPara.szRemotefile, "%s",  stTransCtrl.stEnrichFile[iposition].szRemotefile);
				iFileSize=fileSize(stTransCtrl.stEnrichFile[iposition].szLocalfile);
				if(iFileSize<0)
				{
   					 DispFileErrInfo();
					return (E_MEM_ERR);
				}

				iuploadCnt=0;
				ioffSet=0;
				iMsgSN=1;
				continue;
		  }

		}
		//ioffSet+=iReadLen;
		//iMsgSN++;
	}

}

uint8_t M1_Data_UpLoad(void)
{
    FTP_PARA stFtpPara;
	int iRet;
	int iposition;
	uint8_t Ret;
    int itotalFile;
	int iCurCntFile=0;
	//NacTxd("1111",4);
	portClose(PORT_COM1);
	portOpen(PORT_COM1, (void *)"9600,8,n,1");
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    数据上传    ", "  QUERY TRANS   ");
	//通过FTP上送文件
	memset(&stFtpPara, 0, sizeof(FTP_PARA));
	
	strcpy(stFtpPara.szHostip, stPosParam.szFtpServIP);
	strcpy(stFtpPara.szPort, stPosParam.szFtpServPort);
	strcpy(stFtpPara.szUser, stPosParam.szFtpUserName);
	strcpy(stFtpPara.szPwd, stPosParam.szFtpPwd);
	itotalFile=GetFtpFiletotal();

	iRet=GetFtpFilePosition();
	if(iRet<0)
	{//没有上传的数据
		lcdClrLine(0,7);
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "无数据上传");
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请先结算");
		PubWaitKey(10);
		return NO_DISP;

	}
	iposition=iRet;

	strcpy(stFtpPara.szLocalfile,stTransCtrl.stEnrichFile[iposition].szLocalfile);
	sprintf(stFtpPara.szRemotefile, "%s%s", stPosParam.szFtpPath, stTransCtrl.stEnrichFile[iposition].szRemotefile);
	
	if (strlen(stFtpPara.szHostip) < 7 || strlen(stFtpPara.szPort) < 0 ||
		strlen(stFtpPara.szUser) == 0 || strlen(stFtpPara.szPwd) == 0 )//||strlen(stFtpPara.szRemotefile) == 0 || strlen(stFtpPara.szLocalfile) == 0
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT | DISP_MEDIACY, "ftp参数设置出错");
		PubBeepErr();
		PubWaitKey(5);
		return 1; 
	}
	// 拨号
	iRet = PreDial();
	if( iRet!=0 )
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "拨号失败");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "DialErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}

	// 登录ftp
	iRet = NewFtpLogon(&stFtpPara);
	if (iRet != 0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "FTP登录失败");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "LogErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}
    while(1)
    {
		//上送文件
		iRet=GetFtpFilePosition();
		if(iRet<0)
		{//没有上传的数�
		  Ret=0;
		  break;
		}
		memset(stFtpPara.szLocalfile,0,sizeof(stFtpPara.szLocalfile));
		memset(stFtpPara.szRemotefile,0,sizeof(stFtpPara.szRemotefile));
		//memset();
		iposition=iRet;
		strcpy(stFtpPara.szLocalfile,stTransCtrl.stEnrichFile[iposition].szLocalfile);
		sprintf(stFtpPara.szRemotefile, "%s%s", stPosParam.szFtpPath, stTransCtrl.stEnrichFile[iposition].szRemotefile);
		if(strlen(stFtpPara.szRemotefile) == 0 || strlen(stFtpPara.szLocalfile) == 0)
		{
		  lcdClrLine(2, 7);
		  lcdDisplay(0, 3, DISP_CFONT | DISP_MEDIACY, "ftp参数设置出错");
		  PubBeepErr();
		  PubWaitKey(5);
		  Ret=NO_DISP;
		 	break;

		}
		if(fileExist(stFtpPara.szLocalfile) < 0)
		{
				lcdClrLine(2, 7);
				lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "%s", stFtpPara.szLocalfile);
				lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "文件不存在");
				PubBeepErr();
				PubWaitKey(5);
				 Ret=NO_DISP;
				break;
		}
		//itotalFile;
		iCurCntFile++;
		lcdClrLine(2,7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "文件正在上传中");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "总计文件:%d",itotalFile);
		lcdDisplay(0, 6, DISP_CFONT | DISP_MEDIACY, "当前文件:%d",iCurCntFile);

		iRet = NewFtpPut(&stFtpPara);
		if(iRet==0)	
	    {
		  stTransCtrl.stEnrichFile[iposition].uflag=0;
		  SaveCtrlParam();
	    }
		else
		{
		 Ret=NO_DISP;
		  break;

		}
    }
	
	// 登出服务器
	NewFtpQuit();
	return Ret;
}

//--按销售日期补采----
uint8_t M1_SaleDate_Upload(void)
{
	int iRet;
	UINT8  buf[50];
	UINT8  szDate[4+1];//yyyymmdd 销售时间 BCD
	INT32  iTotalNum,lCurIndex;
	INT32  iTranNum, enrichNum, saleNum;
	INT32  iTotalTranAmt,iTotalSalAmt;
	INT32  iCnt,i;
	UINT8  szFileName[30];
	UINT8  sztime[20];//结算时间 ASC
	UINT8  szEnrichdata[500];
	int    iEnrichDatalen;
	UINT8  uRet,Ret;
	FTP_PARA stFtpPara;
	int iposition=0;
	int ioffSet=0;//偏移量
	int iFileSize=0;//文件大小
	int iMsgSN=1; //数据报序列号
	int iReadLen;
	uint8_t szReadbuf[2048];
	uint8_t utemp;
	int  iuploadCnt=0;
	int fd;

	iTranNum=0;
	enrichNum=0;
	saleNum=0;
	iTotalTranAmt=0;
	iTotalSalAmt=0;


	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    数据上传    ", "  QUERY TRANS   ");	
	DispMulLanguageString(0, 2, DISP_CFONT, "请输入销售日期", "YYYY");
	lcdDisplay(0, 4, DISP_CFONT, "YYYYMMDD");
	memset(buf, 0, sizeof(buf));
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 8, 8, (stPosParam.ucOprtLimitTime*1000), (char *)buf);
   if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
   {
       lcdClrLine(2,7);
	   lcdDisplay(0, 4, DISP_CFONT, "销售日期补采取消");
	   kbGetKeyMs(3*1000);
	   return NO_DISP;
   }
   if( iRet==8 )
   {
	   buf[8] = 0x00;
	   if( CheckDate((char *)buf)!=0 )
	   {
		   FailBeep();
		   lcdClrLine(4, 7);
		   lcdFlip();  
		   DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "日期格式错误", "DATE FORMAT ERR");
		   kbGetKeyMs(2000);
		   return NO_DISP;
	   }
	   PubAsc2Bcd(buf,8,szDate);//4                    //4 位bcd
   }
   else
   {
   		FailBeep();
		lcdClrLine(2,7);
		lcdDisplay(0, 4, DISP_CFONT, "销售日期补采取消");
		kbGetKeyMs(3*1000);
		return NO_DISP;

   }
   //----------------------------------------
	memset(sztime,0,sizeof(sztime));
	memset(szFileName,0,sizeof(szFileName));
	GetEnrichFileName(szFileName);
	memcpy(sztime,szFileName+5,14);
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    交易结算    ", "  QUERY TRANS   ");
	kbFlush();

	lCurIndex = 0;
	iCnt=0;
	iTranNum=0;
	iTotalTranAmt=0;
	memset(&gstYktEnrichLog, 0, sizeof(gstYktEnrichLog));
	iTotalNum= ReadYtkEnrichLog(&iCnt);
	if( iTotalNum==0 )
	{
	   lcdClrLine(0,7);
	   lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "请先数据上传");
	   lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "再结算");
	   PubWaitKey(10);
	   return NO_DISP;
	}
	iRet=FindEnrichStoreFilePosition();
	if(iRet<0)
	{
	   lcdCls();
	   PubDisplayTitle(TRUE,"当批交易查询");
	   PubDispString("无交易", 3|DISP_LINE_CENTER);
	   PubWaitKey(10);
	   return NO_DISP;
	}
	for(i=0;i<iTotalNum;i++)
	{
		ReadYtkEnrichLog(&iCnt);
		if(memcmp(gstYktEnrichLog.sTranDateTime,szDate,4)!=0)
		{
			iCnt--;
			if (iCnt == 0)
			{
			iCnt=iTotalNum;
			}
		}
		else
		{
			//---打包----
			memset(szEnrichdata,0,sizeof(szEnrichdata));
			PackFTP(&gstYktEnrichLog,szEnrichdata,&iEnrichDatalen,sztime);
			uRet= WriteEnrichFtpFile(szEnrichdata,iEnrichDatalen, sztime);//这里sztime是存储在POS里面的文件名。
			if(uRet==OK)
			{
				//更新日志
				iTranNum++;
				if(gstYktEnrichLog.uMode==0)
				{
					iTotalTranAmt=iTotalTranAmt+ gstYktEnrichLog.ulTransAmt;
					enrichNum++;
				}
				else
				{
					iTotalSalAmt=iTotalSalAmt+ gstYktEnrichLog.ulTransAmt;
					saleNum++;
				}
				gstYktEnrichLog.uiStatus=1;
				SaveM1EnrichLog(iCnt);
			}
			iCnt--;
			if (iCnt == 0)
			{
				iCnt=iTotalNum;
			}
		}
	}
	
	iRet =fileExist(sztime);
	if(iRet >= 0)
	{
		iRet=FindEnrichStoreFilePosition();
		if(iRet >=0)
		{
			iposition=iRet;
			memcpy(stTransCtrl.stEnrichFile[iposition].szLocalfile,sztime,strlen(sztime));
			memset(stTransCtrl.stEnrichFile[iposition].szRemotefile,0,sizeof(stTransCtrl.stEnrichFile[iposition].szRemotefile));

			memcpy(stTransCtrl.stEnrichFile[iposition].szRemotefile,szFileName,strlen(szFileName));
			sprintf(&stTransCtrl.stEnrichFile[iposition].szRemotefile[strlen(szFileName)-3],"_%d.aa",iTranNum);

			stTransCtrl.stEnrichFile[iposition].uflag=1;
			stTransCtrl.stEnrichFile[iposition].iRmbM1EnrichAmt=iTotalTranAmt;
			stTransCtrl.stEnrichFile[iposition].iRmbM1EnrichNum=iTranNum;
			stTransCtrl.stEnrichFile[iposition].u8Settlemode=3;
			PubBcd2Asc0(szDate,4,stTransCtrl.stEnrichFile[iposition].szDate);
			SaveCtrlParam();
		}
	}
	if(iTranNum==0)
	{
		lcdClrLine(2,7);
		PubBeepOk();
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "没有结算的数据");
		PubWaitKey(10);
		return NO_DISP;
	}

	memset(&stM1EnrichTotal,0,TOTAL_M1ENRICH_STRC_SIZE);
	stM1EnrichTotal.iRmbM1EnrichNum=enrichNum;
	stM1EnrichTotal.iRmbM1EnrichAmt=iTotalTranAmt;
	stM1EnrichTotal.iRmbM1SaleNum=saleNum;
	stM1EnrichTotal.iRmbM1SaleAmt=iTotalSalAmt;
	memcpy(stM1EnrichTotal.sDateTime,sztime,14);
	memcpy(stM1EnrichTotal.sM1TellNo,stPosParam.sM1TellNo,strlen(stPosParam.sM1TellNo));
	memcpy(stM1EnrichTotal.szCompanyNo,stPosParam.szCompanyNo,strlen(stPosParam.szCompanyNo));
	memcpy(stM1EnrichTotal.pointofsaleNo,stPosParam.pointofsaleNo,strlen(stPosParam.pointofsaleNo));
	stM1EnrichTotal.u8Settlemode=2;
	PubBcd2Asc0(szDate,4,stM1EnrichTotal.szDate);

	if(iTranNum==0){
		lcdCls();
    	PubDisplayTitle(TRUE,"当批交易结算");
   		PubDispString("无交易", 3|DISP_LINE_CENTER);
   		PubWaitKey(10);
    	return NO_DISP;
	}

	SaveM1EnrichLastTotal();
	PrtM1EnrichTotal(1);
	lcdClrLine(2,7);
	lcdDisplay(0, 2, DISP_CFONT, "交易笔数: %d", iTranNum);
	lcdDisplay(0, 4, DISP_CFONT, "充值金额: %d.%02d", iTotalTranAmt/100,iTotalTranAmt%100);
	lcdDisplay(0, 6, DISP_CFONT, "消费金额: %d.%02d", iTotalSalAmt/100,iTotalSalAmt%100);
	kbGetKeyMs(3*1000);
	kbFlush();

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    数据上传    ", "  QUERY TRANS   ");
	//通过FTP上送文件
	iposition=iRet;
	strcpy(stFtpPara.szLocalfile,stTransCtrl.stEnrichFile[iposition].szLocalfile);
	sprintf(stFtpPara.szRemotefile, "%s",  stTransCtrl.stEnrichFile[iposition].szRemotefile);

	iFileSize=fileSize(stTransCtrl.stEnrichFile[iposition].szLocalfile);
	if(iFileSize<0)
	{
		DispFileErrInfo();
		return (E_MEM_ERR);
	}

	iuploadCnt=0;

	while (ioffSet < iFileSize)
	{
	    if(iuploadCnt>3)
	    {
			  lcdClrLine(2,7);
			  lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "结算错误");
			  lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请重试");
			  FailBeep();
			  kbGetKey();
			  return NO_DISP;
		}
		iReadLen = MIN(iFileSize - ioffSet, 800);
		fd = fileOpen(stTransCtrl.stEnrichFile[iposition].szLocalfile, O_RDWR);
		if( fd<0 ) 
		{
			DispFileErrInfo();
			return (E_MEM_ERR);
		}
		iRet = fileSeek(fd, (int)ioffSet, SEEK_SET);
		if( iRet<0 ) 
		{
			DispFileErrInfo();
			fileClose(fd);
			return (E_MEM_ERR);
		} 
		memset(szReadbuf,0,sizeof(szReadbuf));
		iRet =fileRead(fd,szReadbuf,iReadLen);
		if( iRet<0 ) 
		{
			DispFileErrInfo();
			fileClose(fd);
			return (E_MEM_ERR);
		}
		fileClose(fd);
		iReadLen=iRet;
		memset(sSendPacketBuff, 0, sizeof(sSendPacketBuff));
	    memset(sRecvPacketBuff, 0, sizeof(sRecvPacketBuff));
	    usRecvPacketLen = 0;
		usSendPacketLen=0;
		iCnt=0;
		memcpy(&sSendPacketBuff[iCnt],"0001",4);
		iCnt+=4;
		memcpy(&sSendPacketBuff[iCnt],stPosParam.sM1TellNo,4);//终端编号
		iCnt+=4;
		memset(&sSendPacketBuff[iCnt],' ',30);
		memcpy(&sSendPacketBuff[iCnt],stTransCtrl.stEnrichFile[iposition].szRemotefile,strlen(stTransCtrl.stEnrichFile[iposition].szRemotefile));//终端编号
	    iCnt+=30;	
		sprintf(&sSendPacketBuff[iCnt], "%016d", iFileSize);
		iCnt+=16;
		if(ioffSet+iReadLen<iFileSize)//还有
        {
			sSendPacketBuff[iCnt]='1';

		}
		else
		{
			sSendPacketBuff[iCnt]='9';
		}
		iCnt+=1;
		sprintf(&sSendPacketBuff[iCnt], "%03d", iMsgSN);
		iCnt+=3;
		sprintf(&sSendPacketBuff[iCnt], "%016d", ioffSet);//偏移量
		iCnt+=16;	
		sprintf(&sSendPacketBuff[iCnt], "%04d", iReadLen);//长度
		iCnt+=4;
		memcpy(&sSendPacketBuff[iCnt],szReadbuf,iReadLen);
		iCnt+=iReadLen;
		utemp=PubCalcLRC(sSendPacketBuff,iCnt,0);
		sprintf(&sSendPacketBuff[iCnt],"%03d",utemp);
		iCnt+=3;
		usSendPacketLen=iCnt;
		Ret= SendRecvPacket();
		if(Ret!=0)
		{
		   iuploadCnt++;
		   continue;
		}
		utemp=(uint8_t)PubCalcLRC(sRecvPacketBuff,usRecvPacketLen-1,0);
		if(utemp !=sRecvPacketBuff[usRecvPacketLen-1])
		{
			 iuploadCnt++;
		     continue;
		}

		if(memcmp(&sRecvPacketBuff[8],"0000",4)!=0)
		{
			//上传失败 退出
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "结算错误");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请重试");
			FailBeep();
			kbGetKey();
			return NO_DISP;
		}
		if(sRecvPacketBuff[42]=='2')//重传
		{
             iuploadCnt++;
		     continue;
		}
		else if(sRecvPacketBuff[42]=='1')
		{
		  	if(ioffSet+iReadLen<iFileSize)//还有
		  	{
		     	iuploadCnt=0;
				ioffSet+=iReadLen;
				iMsgSN++;
		  	}
		  	else
			{
		        iuploadCnt=0;
				stTransCtrl.stEnrichFile[iposition].uflag=0;
				SaveCtrlParam();
				fileRemove(stTransCtrl.stEnrichFile[iposition].szLocalfile);
				iuploadCnt=0;
				ioffSet=0;
				iMsgSN=1;
				lcdClrLine(2,7);
				lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "结算文件");
				lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "上传成功");
				OkBeep();
				PubWaitKey(3);
				return NO_DISP;
			}
		}
	}
	return 0;
}

//--按结算日期补采-----
uint8_t M1_SettDate_Upload(void)
{
	int iRet;
	UINT8  buf[50];
	UINT8  szDate[4+1];//yyyymmdd 销售时间 BCD
	INT32  iTotalNum,lCurIndex;
	INT32  iTranNum, enrichNum, saleNum;
	INT32  iTotalTranAmt,iTotalSalAmt;
	INT32  iCnt,i;
	UINT8  szFileName[30];
	UINT8  sztime[20];//结算时间 ASC
	UINT8  szEnrichdata[500];
	int    iEnrichDatalen;
	UINT8  uRet,Ret;
	FTP_PARA stFtpPara;
	//int iRet;
	int iposition=0;

	iTranNum=0;
	enrichNum=0;
	saleNum=0;
	iTotalTranAmt=0;
	iTotalSalAmt=0;
	
	//int iposition=0;
	int ioffSet=0;//偏移量
	int iFileSize=0;//文件大小
	//int iCnt; //
	int iMsgSN=1; //数据报序列号
	int iReadLen;
	uint8_t szReadbuf[2048];
	uint8_t utemp;
	int  iuploadCnt=0;
	//FTP_PARA stFtpPara;
		 int fd;
	
			lcdCls();
			DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    数据上传    ", "  QUERY TRANS   ");	
			// DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "终端参数设置	 ", "	SETUP DATE	 ");
			DispMulLanguageString(0, 2, DISP_CFONT, "请输入结算日期", "YYYY");
			lcdDisplay(0, 4, DISP_CFONT, "YYYYMMDD");
			memset(buf, 0, sizeof(buf));
			lcdGoto(0, 6);
			iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 8, 8, 
			  (stPosParam.ucOprtLimitTime*1000), (char *)buf);
		   if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
		   {
			   lcdClrLine(2,7);
			   lcdDisplay(0, 4, DISP_CFONT, "结算日期补采取消");
			   kbGetKeyMs(3*1000);
			   return NO_DISP;
		   }
		   if( iRet==8 )
		   {
			   //memcpy(szDate, buf, 8);
			   buf[8] = 0x00;
			   if( CheckDate((char *)buf)!=0 )
			   {
				   FailBeep();
				   lcdClrLine(4, 7);
				   lcdFlip();  
				   DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "日期格式错误", "DATE FORMAT ERR");
				   kbGetKeyMs(2000);
				   return NO_DISP;
			   }
			   PubAsc2Bcd(buf,8,szDate);//4 				   //4 位bcd
			  
		   }
		   else
		   {
				FailBeep();
				lcdClrLine(2,7);
				lcdDisplay(0, 4, DISP_CFONT, "销售日期补采取消");
				kbGetKeyMs(3*1000);
				return NO_DISP;
	
		   }
		   //----------------------------------------
		   memset(sztime,0,sizeof(sztime));
		memset(szFileName,0,sizeof(szFileName));
		GetEnrichFileName(szFileName);
		memcpy(sztime,szFileName+5,14);
	//	prnInit();
	//	prnPrintf("%s",szFileName);
	//	prnStart();
		lcdCls();
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    交易结算    ", "  QUERY TRANS   ");
		kbFlush();
		// PubDisplayTitle(TRUE,"交易明细查询");
		//memset(szDisp, 0, sizeof(szDisp));
		lCurIndex = 0;
		iCnt=0;
		iTranNum=0;
		iTotalTranAmt=0;
		 memset(&gstYktEnrichLog, 0, sizeof(gstYktEnrichLog));
		iTotalNum= ReadYtkEnrichLog(&iCnt);
		if( iTotalNum==0 )
		{
		   lcdClrLine(2,7);
		   lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "请先数据上传");
		   lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "再结算");
		   PubWaitKey(10);
		   return NO_DISP;
		}
		 iRet=FindEnrichStoreFilePosition();
		 if(iRet<0)
		 {
		   lcdCls();
		   PubDisplayTitle(TRUE,"当批交易查询");
		   PubDispString("无交易", 3|DISP_LINE_CENTER);
		   PubWaitKey(10);
		   return NO_DISP;
	
		 }
		for(i=0;i<iTotalNum;i++)
		{
			ReadYtkEnrichLog(&iCnt);
			if(memcmp(gstYktEnrichLog.sLogDateTime,szDate,4)!=0)
		   // if(gstYktEnrichLog.uiStatus==0x01)//结算了
			{
				iCnt--;
				if (iCnt == 0)
				{
					iCnt=iTotalNum;
				}
				//  break;
				 // iCnt=iCnt%iTotalNum;
			 }
			 else
			 {
				//iTranNum++;
				//iTotalTranAmt=iTotalTranAmt+ gstYktEnrichLog.ulTransAmt;
				//---打包----
				memset(szEnrichdata,0,sizeof(szEnrichdata));
				PackFTP(&gstYktEnrichLog,szEnrichdata,&iEnrichDatalen,sztime);
				uRet= WriteEnrichFtpFile(szEnrichdata,iEnrichDatalen, sztime);//这里sztime是存储在POS里面的文件名。
				if(uRet==OK)
				{
				  //更新日志
				  iTranNum++;
				  if(gstYktEnrichLog.uMode==0)
				  {
					iTotalTranAmt=iTotalTranAmt+ gstYktEnrichLog.ulTransAmt;
					enrichNum++;
				  }
				  else
				  {
					iTotalSalAmt=iTotalSalAmt+ gstYktEnrichLog.ulTransAmt;
					saleNum++;
				  }
			  
				  gstYktEnrichLog.uiStatus=1;
				  PubAsc2Bcd(sztime,14,gstYktEnrichLog.sLogDateTime);
				  SaveM1EnrichLog(iCnt);
	
				}
			//	prnInit();
			//	prnPrintf("%s",szEnrichdata);
			//	prnStart();
				
				iCnt--;
				if (iCnt == 0)
				{
				 iCnt=iTotalNum;
				}
			  }
			}
			iRet =fileExist(sztime);
			if(iRet >= 0)
			{
			   iRet=FindEnrichStoreFilePosition();
			   if(iRet >=0)
			   {
			   iposition=iRet;
				memcpy(stTransCtrl.stEnrichFile[iposition].szLocalfile,sztime,strlen(sztime));
				memset(stTransCtrl.stEnrichFile[iposition].szRemotefile,0,sizeof(stTransCtrl.stEnrichFile[iposition].szRemotefile));
				memcpy(stTransCtrl.stEnrichFile[iposition].szRemotefile,szFileName,strlen(szFileName));
				sprintf(&stTransCtrl.stEnrichFile[iposition].szRemotefile[strlen(szFileName)-3],"_%d.aa",iTranNum);
				stTransCtrl.stEnrichFile[iposition].uflag=1;
				//stTransCtrl.stEnrichFile[iRet].uflag=1;
				stTransCtrl.stEnrichFile[iposition].iRmbM1EnrichAmt=iTotalTranAmt;
				stTransCtrl.stEnrichFile[iposition].iRmbM1EnrichNum=iTranNum;
				stTransCtrl.stEnrichFile[iposition].u8Settlemode=2;
				PubBcd2Asc0(szDate,4,stTransCtrl.stEnrichFile[iposition].szDate);
				SaveCtrlParam();
			   }
			}
			if(iTranNum==0)
			{
				lcdClrLine(2,7);
				PubBeepOk();
				lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "没有结算的数据");
				//	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "再结算");
				PubWaitKey(10);
				return NO_DISP;


			}
			memset(&stM1EnrichTotal,0,TOTAL_M1ENRICH_STRC_SIZE);
			stM1EnrichTotal.iRmbM1EnrichNum=enrichNum;
			stM1EnrichTotal.iRmbM1EnrichAmt=iTotalTranAmt;
			stM1EnrichTotal.iRmbM1SaleNum=saleNum;
			stM1EnrichTotal.iRmbM1SaleAmt=iTotalSalAmt;
			//memcpy(stM1EnrichTotal.pointofsaleNo,,3);
			memcpy(stM1EnrichTotal.sDateTime,sztime,14);
			memcpy(stM1EnrichTotal.sM1TellNo,stPosParam.sM1TellNo,strlen(stPosParam.sM1TellNo));
			memcpy(stM1EnrichTotal.szCompanyNo,stPosParam.szCompanyNo,strlen(stPosParam.szCompanyNo));
			memcpy(stM1EnrichTotal.pointofsaleNo,stPosParam.pointofsaleNo,strlen(stPosParam.pointofsaleNo));
			stM1EnrichTotal.u8Settlemode=2;
			PubBcd2Asc0(szDate,4,stM1EnrichTotal.szDate);
	
	
			SaveM1EnrichLastTotal();
			PrtM1EnrichTotal(1);
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT, "交易笔数: %d", iTranNum);
			lcdDisplay(0, 4, DISP_CFONT, "充值金额: %d.%02d", iTotalTranAmt/100,iTotalTranAmt%100);
			lcdDisplay(0, 6, DISP_CFONT, "消费金额: %d.%02d", iTotalSalAmt/100,iTotalSalAmt%100);
			kbGetKeyMs(3*1000);
			kbFlush();
	
			lcdCls();
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    数据上传    ", "  QUERY TRANS   ");
		iposition=iRet;
			strcpy(stFtpPara.szLocalfile,stTransCtrl.stEnrichFile[iposition].szLocalfile);
			sprintf(stFtpPara.szRemotefile, "%s",  stTransCtrl.stEnrichFile[iposition].szRemotefile);
		//int ret;
			iFileSize=fileSize(stTransCtrl.stEnrichFile[iposition].szLocalfile);
			if(iFileSize<0)
			{
				DispFileErrInfo();
				return (E_MEM_ERR);
			}
		
			iuploadCnt=0;
		
		while (ioffSet < iFileSize)
		{
			if(iuploadCnt>3)
			{
				//NacTxd("3333",4);
				   lcdClrLine(2,7);
				  lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "结算错误");
				  lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请重试");
				  //OkBeep(void)
				  FailBeep();
				  kbGetKey();
				  //k
				 // PubWaitKey(3);
				  return NO_DISP;
				//return NO_DISP;
		
			}
			iReadLen = MIN(iFileSize - ioffSet, 800);
			fd = fileOpen(stTransCtrl.stEnrichFile[iposition].szLocalfile, O_RDWR);
			if( fd<0 ) 
			{
				DispFileErrInfo();
				return (E_MEM_ERR);
			}
			iRet = fileSeek(fd, (int)ioffSet, SEEK_SET);
			if( iRet<0 ) 
			{
				DispFileErrInfo();
				fileClose(fd);
				return (E_MEM_ERR);
			} 
			memset(szReadbuf,0,sizeof(szReadbuf));
			iRet =fileRead(fd,szReadbuf,iReadLen);
			if( iRet<0 ) 
			{
				DispFileErrInfo();
				fileClose(fd);
				return (E_MEM_ERR);
			}
			fileClose(fd);
			iReadLen=iRet;
			memset(sSendPacketBuff, 0, sizeof(sSendPacketBuff));
			memset(sRecvPacketBuff, 0, sizeof(sRecvPacketBuff));
			usRecvPacketLen = 0;
			usSendPacketLen=0;
			iCnt=0;
			memcpy(&sSendPacketBuff[iCnt],"0001",4);
			iCnt+=4;
			memcpy(&sSendPacketBuff[iCnt],stPosParam.sM1TellNo,4);//终端编号
			iCnt+=4;
			memset(&sSendPacketBuff[iCnt],' ',30);
			memcpy(&sSendPacketBuff[iCnt],stTransCtrl.stEnrichFile[iposition].szRemotefile,strlen(stTransCtrl.stEnrichFile[iposition].szRemotefile));//终端编号
			iCnt+=30;	
			sprintf(&sSendPacketBuff[iCnt], "%016d", iFileSize);
			iCnt+=16;
			if(ioffSet+iReadLen<iFileSize)//还有
			{
				sSendPacketBuff[iCnt]='1';
		
			}
			else
			{
				sSendPacketBuff[iCnt]='9';
			}
			iCnt+=1;
			sprintf(&sSendPacketBuff[iCnt], "%03d", iMsgSN);
			iCnt+=3;
			sprintf(&sSendPacketBuff[iCnt], "%016d", ioffSet);//偏移量
			iCnt+=16;	
			sprintf(&sSendPacketBuff[iCnt], "%04d", iReadLen);//长度
			iCnt+=4;
			memcpy(&sSendPacketBuff[iCnt],szReadbuf,iReadLen);
			iCnt+=iReadLen;
			utemp=PubCalcLRC(sSendPacketBuff,iCnt,0);
			sprintf(&sSendPacketBuff[iCnt],"%03d",utemp);
			iCnt+=3;
			usSendPacketLen=iCnt;
			//NacTxd(sSendPacketBuff,usSendPacketLen);
			//PubCalcLRC()
			Ret= SendRecvPacket();
			if(Ret!=0)
			{
			   iuploadCnt++;
			   continue;
			  //return NO_DISP;
			}
			utemp=(uint8_t)PubCalcLRC(sRecvPacketBuff,usRecvPacketLen-1,0);
			if(utemp !=sRecvPacketBuff[usRecvPacketLen-1])
			{
			//	 NacTxd("2222",4);
			//	 NacTxd(&utemp,1);
			//	 NacTxd(&sRecvPacketBuff[usRecvPacketLen-1],1);
				 iuploadCnt++;
				 continue;
			}
			//NacTxd(sRecvPacketBuff,usRecvPacketLen);
			
			if(memcmp(&sRecvPacketBuff[8],"0000",4)!=0)
			{
				   //上传失败 退出
				//   NacTxd("3333",4);
				   lcdClrLine(2,7);
				  lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "结算错误");
				  lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请重试");
				   FailBeep();
				  kbGetKey();
				  return NO_DISP;
				  // return NO_DISP;
		
			}
			if(sRecvPacketBuff[42]=='2')//重传
			{
			// NacTxd("4444",4);
				 iuploadCnt++;
				 continue;
			}
			else if(sRecvPacketBuff[42]=='1')
			{
			  if(ioffSet+iReadLen<iFileSize)//还有
			  {
				 iuploadCnt=0;
				ioffSet+=iReadLen;
						iMsgSN++;
			  }
			  else
			  {
				   iuploadCnt=0;
				  stTransCtrl.stEnrichFile[iposition].uflag=0;
				  SaveCtrlParam();
				  fileRemove(stTransCtrl.stEnrichFile[iposition].szLocalfile);
				 //----------------
				
					iuploadCnt=0;
					ioffSet=0;
					iMsgSN=1;
			
				 //--------------
		
				  
				  lcdClrLine(2,7);
		
		
				  
				 lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "结算文件");
				  lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "上传成功");
				  OkBeep();
				  PubWaitKey(3);
				  return NO_DISP;
			  }
		
			}
			//ioffSet+=iReadLen;
			//iMsgSN++;
		}
		
		

		//通过FTP上送文件
		/*memset(&stFtpPara, 0, sizeof(FTP_PARA));
		
		strcpy(stFtpPara.szHostip, stPosParam.szFtpServIP);
		strcpy(stFtpPara.szPort, stPosParam.szFtpServPort);
		strcpy(stFtpPara.szUser, stPosParam.szFtpUserName);
		strcpy(stFtpPara.szPwd, stPosParam.szFtpPwd);
	
		
		strcpy(stFtpPara.szLocalfile,stTransCtrl.stEnrichFile[iposition].szLocalfile);
		sprintf(stFtpPara.szRemotefile, "%s%s", stPosParam.szFtpPath, stTransCtrl.stEnrichFile[iposition].szRemotefile);

	
		iRet = NewFtpPutFile(&stFtpPara, PreDial);
		if(iRet==0) 
		{
			stTransCtrl.stEnrichFile[iposition].uflag=0;
			SaveCtrlParam();
		}*/
			return 0;

}
uint8_t M1_Que_NotUploadFile(void)
{
	int i,j=0;
	 // prnStart();
	 prnInit();
	  PrnSmallFontData("");
	  for(i=0;i<50;i++)
	  {
		   if(stTransCtrl.stEnrichFile[i].uflag==1)
		   {
		       j++;
			   prnPrintf("文件名:%s\n",stTransCtrl.stEnrichFile[i].szRemotefile);
			   prnPrintf("充值笔数:%d\n",stTransCtrl.stEnrichFile[i].iRmbM1EnrichNum);
			   prnPrintf("充值金额:%d.%02d\n",stTransCtrl.stEnrichFile[i].iRmbM1EnrichAmt/100,stTransCtrl.stEnrichFile[i].iRmbM1EnrichAmt%100);
			   prnPrintf("消费笔数:%d\n",stTransCtrl.stEnrichFile[i].iRmbM1SaleNum);
			   prnPrintf("消费金额:%d.%02d\n",stTransCtrl.stEnrichFile[i].iRmbM1SaleAmt/100,stTransCtrl.stEnrichFile[i].iRmbM1SaleAmt%100);
              if(stTransCtrl.stEnrichFile[i].u8Settlemode==1)
				{
					prnPrintf("结算方式:普通结算\n");

				}
				else if(stTransCtrl.stEnrichFile[i].u8Settlemode==2)
				{
					prnPrintf("结算方式:按销售日期补采\n");
					prnPrintf("销售日期补采:%s\n",stTransCtrl.stEnrichFile[i].szDate);
				}
				else if(stTransCtrl.stEnrichFile[i].u8Settlemode==3)
				{
					prnPrintf("结算方式:按结算日期补采\n");
					prnPrintf("结算日期补采:%s\n",stTransCtrl.stEnrichFile[i].szDate);

				}
				prnPrintf("\n");
		   }
			   
	  }
	  if(j==0)
	  {
       lcdCls();
	   lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "没有需要上传的");
	   lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "结算文件");
	   kbGetKeyMs(3*1000);
	   return 0;

	  }
	  prnPrintf("\n\n\n\n");
	  
		   PrintData();
		  return 0;


}


//---更新充值额度---------------
uint8_t Agent_Update(void)
{
	FTP_PARA stFtpPara;
	 uint8_t Ret;
	 int iCnt,iTranNum,iTotalNum;   //
	 uint8_t utemp;
	 int  iuploadCnt=0;
	int lTmpInt = 0;
	int iRet,i;
	uint8_t	buf[10+1];//,szAmount[12];

	iCnt=0;
	iTranNum=0;
	 memset(&gstYktEnrichLog, 0, sizeof(gstYktEnrichLog));
	iTotalNum = ReadYtkEnrichLog(&iCnt);
	if( iTotalNum > 0 )
	{
		for(i=0;i<iTotalNum;i++)
		{
		  ReadYtkEnrichLog(&iCnt);
		  if(gstYktEnrichLog.uiStatus==0x01)//结算了
		  {
			 iCnt--;
			 if (iCnt == 0)
			 {
				 iCnt=iTotalNum;
			 }
		  }
		  else
		  {
		    iTranNum++;
			break;
		  }
		}

		if(iTranNum > 0)
		{
			M1_Sett();
		}
		
	}

	iRet=GetFtpFiletotal();
	if(iRet>0)
	{
		M1_Data_UpLoad_TCP();
	}

	memset(&stFtpPara, 0, sizeof(FTP_PARA));

	iuploadCnt=0;

	while (1)
	{
		iuploadCnt ++;
	    if(iuploadCnt>3)
	    {
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "额度同步错误");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请重试");
			FailBeep();
			kbGetKey();
			return NO_DISP;

		}
		
		memset(sSendPacketBuff, 0, sizeof(sSendPacketBuff));
	    memset(sRecvPacketBuff, 0, sizeof(sRecvPacketBuff));
	    usRecvPacketLen = 0;
		usSendPacketLen=0;
		iCnt=0;
		memcpy(&sSendPacketBuff[iCnt],"0002",4);
		iCnt+=4;
		
//NacTxd("1111",4);
//NacTxd(&stPosParam.sM1TellNo,20);

		sprintf(&sSendPacketBuff[iCnt],"00",2);//部门
		iCnt+=2;

		//PubAsc2Bcd(stPosParam.sM1TellNo,8,&sSendPacketBuff[iCnt]);//终端编号
		sprintf(&sSendPacketBuff[iCnt],stPosParam.sM1TellNo,8);//终端编号
		iCnt+=8;
		
		utemp=PubCalcLRC(sSendPacketBuff,iCnt,0);
		sprintf(&sSendPacketBuff[iCnt],"%03d",utemp);
		iCnt+=3;
		usSendPacketLen=iCnt;
//NacTxd(sSendPacketBuff,usSendPacketLen);
		//PubCalcLRC()
		Ret= SendRecvPacket();
		if(Ret!=0)
		{
		   continue;
          //return NO_DISP;
		}
		utemp=(uint8_t)PubCalcLRC(sRecvPacketBuff,usRecvPacketLen-1,0);
		if(utemp !=sRecvPacketBuff[usRecvPacketLen-1])
		{
//NacTxd("2222",4);
//NacTxd(&utemp,1);
//NacTxd(&sRecvPacketBuff[usRecvPacketLen-1],1);
		     continue;
		}
//NacTxd(sRecvPacketBuff,usRecvPacketLen);
		
		if(memcmp(&sRecvPacketBuff[14],"0000",4)!=0)
		{
            //上传失败 退出
//NacTxd("3333",4);
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "额度同步错误");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "重试....");
			FailBeep();
			kbGetKey();
			continue;
		}

		//memset(szAmount, 0, 16);
		memset(buf, 0, sizeof(buf));
		memcpy(buf,&sRecvPacketBuff[18],10);
//NacTxd(buf,10);		
		lTmpInt = atol((char *)buf);
		
		//prnInit();//打印初始化
		//prnPrintf("%10d",lTmpInt);//字
		//stPosParam.maxAgentAmt=lTmpInt;
		//prnPrintf("%10d",lTmpInt);//字
		//prnStart();//打印开始

		SaveAppParam();
		
		return OK;

	}

}


//---更新充值额度---------------
uint8_t Login(void)
{
	FTP_PARA stFtpPara;
	uint8_t Ret;
	int iCnt,iTranNum,iTotalNum;   //
	uint8_t utemp;
	int  iuploadCnt=0;
	int lTmpInt = 0;
	int iRet,i;
	uint8_t	buf[10+1];//,szAmount[12];
	uint8_t	key[32+1];//,szAmount[12];
	uint8_t sMsgBodyKey[16];
	uint8_t szSendMsgData[500];
	int ioutlen;

	iCnt=0;
	iTranNum=0;
	memset(&gstYktEnrichLog, 0, sizeof(gstYktEnrichLog));
	iTotalNum = ReadYtkEnrichLog(&iCnt);
	if( iTotalNum > 0 )
	{
		for(i=0;i<iTotalNum;i++)
		{
		  ReadYtkEnrichLog(&iCnt);
		  if(gstYktEnrichLog.uiStatus==0x01)//结算了
		  {
			 iCnt--;
			 if (iCnt == 0)
			 {
				 iCnt=iTotalNum;
			 }
		  }
		  else
		  {
		    iTranNum++;
			break;
		  }
		}

		if(iTranNum > 0)
		{
			M1_Sett();
		}
		
	}

	iRet=GetFtpFiletotal();
	if(iRet>0)
	{
		M1_Data_UpLoad_TCP();
	}

	memset(&stFtpPara, 0, sizeof(FTP_PARA));

	iuploadCnt=0;

	while (1)
	{
		iuploadCnt ++;
	    if(iuploadCnt>3)
	    {
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "登录失败");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请重试");
			FailBeep();
			kbGetKey();
			return NO_DISP;

		}
		
		memset(sSendPacketBuff, 0, sizeof(sSendPacketBuff));
	    memset(sRecvPacketBuff, 0, sizeof(sRecvPacketBuff));
	    usRecvPacketLen = 0;
		usSendPacketLen=0;
		iCnt=0;
		memcpy(&sSendPacketBuff[iCnt],"0004",4);
		iCnt+=4;

		memcpy(&sSendPacketBuff[iCnt],"00",2);//部门
		iCnt+=2;

		sprintf(&sSendPacketBuff[iCnt],stPosParam.sM1TellNo,8);//终端编号
		iCnt+=8;

		utemp=PubCalcLRC(sSendPacketBuff,iCnt,0);
		sprintf(&sSendPacketBuff[iCnt],"%03d",utemp);
		iCnt+=3;
		usSendPacketLen=iCnt;
//NacTxd(sSendPacketBuff,usSendPacketLen);
		//PubCalcLRC()
		Ret= SendRecvPacket();
		if(Ret!=0)
		{
		   continue;
          //return NO_DISP;
		}
//NacTxd("1001",4);
//NacTxd(sRecvPacketBuff,usRecvPacketLen);
		utemp=(uint8_t)PubCalcLRC(sRecvPacketBuff,usRecvPacketLen-1,0);
		if(utemp !=sRecvPacketBuff[usRecvPacketLen-1])
		{
		     continue;
		}
		
		if(memcmp(&sRecvPacketBuff[14],"0000",4)!=0)
		{
            //上传失败 退出
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "登录验证失败");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "重试1....");
			FailBeep();
			kbGetKey();
			continue;
		}

		PubAsc2Bcd("544A4851574C2E434F4D323031323036",32,sMsgBodyKey);
		memcpy(&szSendMsgData[0],stPosParam.sM1TellNo,8);
		iRet=TDEACBCEnry(szSendMsgData,8,&ioutlen,sMsgBodyKey);
//NacTxd("1002",4);
//NacTxd(szSendMsgData,ioutlen);
		if(iRet != 0)
		{//加密错误
		    lcdClrLine(2, 7);
		    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "加密错误");
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		}
		
		PubBcd2Asc(szSendMsgData,ioutlen + ioutlen,&key[0]);
//NacTxd("1234",4);

		if(memcmp(&sRecvPacketBuff[18],key,ioutlen + ioutlen)!=0)
		{
            //上传失败 退出
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "登录验证失败");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "重试2....");
			FailBeep();
			kbGetKey();
			continue;
		}


		return OK;

	}

}

//通过1级分散算出圈存密钥，
//拿圈存密钥对通过圈存初始化得到的部分数据进行加密生成8字节的过程密钥SK，
//然后再拿过程密钥SK  对圈存数据按MAC计算生成成MAC2,
//再把MAC2拼接上交易日期时间，作为指令，进行圈存
UINT8 GetMAC(UINT8 initbuf[], UINT8 mac_data[],int mlen, UINT8 mac_data_1[], int mlen_1)
{
	uint8_t key[16];//根密钥
	uint8_t l_data[8];
	uint8_t r_data[8];
	uint8_t data[16];//圈存密钥
	uint8 sk_data[8];//过程密钥
	int i, l, k,sk, ret;
	int iSendLen, iRecvLen;
	UINT8 szSendData[300], szRecvData[300];
	UINT8 mac_init[8];
	APDU_SEND stApduSend;
	APDU_RESP stApduResp;

	uint8 result[8];
	uint8 temp[8];

	/////一级分散计算圈存密钥
	//memcpy(l_data,gstYktEnrichLog.sCardSerialNo,8);
	memcpy(l_data,"\x00\x00\x00\x00",4);
	memcpy(l_data+4,stTemp.sCardUid,4);
//NacTxd(l_data,20);
	for(i=0;i<8;i++){
		r_data[i]=~l_data[i];
	}
	memcpy(key,gstYktEnrichLog.sIsamKey,16);//根密钥
	ret=TDEA(l_data,l_data,key,16,TDEA_ENCRYPT);
	if( ret!=PED_RET_OK )
			return(1);
	memcpy(data,l_data,8);

	
	ret=TDEA(r_data,r_data,key,16,TDEA_ENCRYPT);
	if( ret!=PED_RET_OK )
			return(1);
	memcpy(&data[8],r_data,8);

//NacTxd("4002",6);
//NacTxd(key,20);
//NacTxd(data,20);

	//memcpy(&data,"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",16);
	/////拿圈存密钥对通过圈存初始化得到的部分数据进行加密生成8字节的过程密钥SK，
	memset(sk_data, 0x00, sizeof(sk_data));
	memcpy(sk_data,initbuf+8,4);//伪随机数
	memcpy(sk_data+4,initbuf+4,2);//交易序号
	memcpy(sk_data+6,"\x80\x00",2);
//NacTxd("1006",4);
//NacTxd(sk_data,10);
	ret=TDEA(sk_data,sk_data,data,16,TDEA_ENCRYPT);
	if( ret!=PED_RET_OK )
			return(1);
//NacTxd(sk_data,20);

	/////拿过程密钥和圈存初始化得到的部分数据生成MAC2
	/*//取随机数，得到8    字节的初始值
	iSendLen = 0;
	iRecvLen = 0;  
	memset(szSendData, 0x00, sizeof(szSendData));
	memset(szRecvData, 0x00, sizeof(szRecvData));
	memcpy(szSendData,"\x00\x84\x00\x00\x04",5);
	iSendLen+=5;

	ret = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
	if ((ret!=ICC_SUCCESS)||(szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
	{

		lcdClrLine(2, 7);
		PubDispString("取随机数错", 3|DISP_LINE_CENTER);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
NacTxd("113322",6);
NacTxd(szRecvData,30);
	memcpy(mac_init,szRecvData,4);
	*/
	memcpy(mac_init,"\x00\x00\x00\x00\x00\x00\x00\x00",8);

//NacTxd("450000",6);
//NacTxd(initbuf,30);//圈存初始化结果
//NacTxd(mac_data_1,32);
//NacTxd(mac_data,32);

	//验证MAC1
	memset(temp, 0x00, sizeof(temp));
	memset(result, 0x00, sizeof(result));
	memcpy(result,mac_init,8);
	for(i=0;i<mlen_1/8;i++){
		memcpy(temp,mac_data_1+i*8,8);
		PubXor(result,temp,8);
		ret=TDEA(result,result,sk_data,8,TDEA_ENCRYPT);
	}
//NacTxd("4003",4);
//NacTxd(result,20);
//NacTxd(initbuf,25);

	if(memcmp(result,initbuf+12,4)!=0){
		return NO_DISP;
	}

	//生成MAC2
	memset(temp, 0x00, sizeof(temp));
	memset(result, 0x00, sizeof(result));
	memcpy(result,mac_init,8);
	for(i=0;i<mlen/8;i++){
		memcpy(temp,mac_data+i*8,8);
		PubXor(result,temp,8);
		ret=TDEA(result,result,sk_data,8,TDEA_ENCRYPT);
	}
//NacTxd("4004",4);
//NacTxd(result,20);

	memcpy(gstYktEnrichLog.mac2,result,4);
	
	return 0;

}


UINT8 CPU_EnrichWriteData(pEtcardInParam_t  pInParam)
{
    //---------初始化指令，返回报文体明文-------
	uint8_t szSendMsgData[500];
	uint8_t szRevMsgData[500];
	int iReportlen,ioutlen;
	int iRet,iCnt;
	uint8_t uRet,ucRet;
    uint8_t sMsgBodyKey[16];

	int 	   iSendLen, iRecvLen;
	UINT8 szSendData[300], szRecvData[300];
	int macSendLen, macSendLen_1,macRecvLen;
	UINT8 macSendData[300], macRecvData[300];//生成MAC2  所用到数据
	UINT8 macSendData_1[300],mac1[4];//验证MAC1  所用数据
	UINT8 initbuf[300];//圈存初始化返回结果
	int lValue;
	APDU_SEND stApduSend;
	APDU_RESP stApduResp;
	UINT8 result[16];
	uint8_t time[11];

	////////////////////密码验证//////////////////
	iSendLen = 0;
	iRecvLen = 0;  
	memset(szSendData, 0x00, sizeof(szSendData));
	memset(szRecvData, 0x00, sizeof(szRecvData));
	memcpy(szSendData,"\x00\x20\x00\x00\x03\x12\x34\x56",8);
	iSendLen = 9;
	iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
	if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
	{
	   lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "密码校验错");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	  // return ERR_CODE_VERIFY_PIN;
	}
//NacTxd("1000", 4);
//NacTxd(szRecvData,iRecvLen);

	////////////////////圈存初始化//////////////////
	PubAsc2Bcd(stPosParam.szPosId,12,pInParam->TerminalNum);
	iSendLen = 0;
	iRecvLen = 0;  
	//memset(szSendData, 0x00, sizeof(szSendData));
	//memset(szRecvData, 0x00, sizeof(szRecvData));
	memcpy(szSendData,"\x80\x50\x00\x02\x0B",5);
	iSendLen = 5;
	memcpy(szSendData+iSendLen,"\x01",1);
	iSendLen+=1;
	lValue = PubBcd2Long(PosCom.stTrans.sAmount,6,NULL);
	PubLong2Char(lValue, 4, szSendData+iSendLen);
	iSendLen+=4;  
	memcpy(szSendData+iSendLen, pInParam->TerminalNum, 6);
	iSendLen+=6; 
	memcpy(szSendData+iSendLen, "\x10", 1);
	iSendLen+=1;
//NacTxd("1001",4);
//NacTxd(szSendData, iSendLen);
	iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
//NacTxd(szRecvData, iRecvLen);

	if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
	{
	    lcdClrLine(2, 7);
	    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "圈存初始化错");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		//lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "读卡失败");
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	//return ERR_CODE_INIT_LOAD;
	}
	memcpy(initbuf,szRecvData, iRecvLen);
//NacTxd("1002",4);	
	SaveReversalTranData();
//NacTxd(gstYktEnrichLog.sTranDateTime,70);
	//memcpy(gstYktEnrichLog.sTranDateTime,&sRecvPacketBuff[7+2],7);

	////////////////////圈存//////////////////
	iSendLen = 0;
	iRecvLen = 0;  
	memset(szSendData, 0x00, sizeof(szSendData));
	memset(szRecvData, 0x00, sizeof(szRecvData));
	memcpy(szSendData,"\x80\x52\x00\x00\x0B",5);
	iSendLen = 5;
	//	memcpy(szSendData+iSendLen,"\x01",1);
	//szSendMsgData
	/*PubAsc2Bcd(pInParam->szDate,8,szSendData+iSendLen);
	iSendLen+=4;
	PubAsc2Bcd(pInParam->szTime,6,szSendData+iSendLen);
	iSendLen+=3;*/
	memcpy(time,&gstYktEnrichLog.sTranDateTime,7);
//NacTxd("1003",4);
//NacTxd(time,16);

	memcpy(szSendData+iSendLen,&time,7);
	iSendLen+=(7);

	//认证根密钥
	ucRet =EnrichAuthenticate();
	if( ucRet!=OK )
	{
		return NO_DISP; 
	}
//NacTxd("1005",4);
//NacTxd(gstYktEnrichLog.sIsamKey,16);

	//验证MAC1  的数据
	macSendLen_1=0;
	memset(macSendData_1,0x00, sizeof(macSendData_1));
	memcpy(macSendData_1,initbuf,4);//旧金额
	macSendLen_1+=4;
	lValue = PubBcd2Long(PosCom.stTrans.sAmount,6,NULL); //金额
	PubLong2Char(lValue, 4, macSendData_1+macSendLen_1);
	macSendLen_1+=4;
	memcpy(macSendData_1+macSendLen_1,"\x02",1);
	macSendLen_1+=1;
	memcpy(macSendData_1+macSendLen_1,pInParam->TerminalNum,6);
	macSendLen_1+=6;
	memcpy(macSendData_1+macSendLen_1,"\x80",1);
	macSendLen_1+=1;
	while(macSendLen_1%8!=0){
		memcpy(macSendData+macSendLen_1,"\x00",1);
		macSendLen_1+=1;
	}
	
	
	//获取MAC2
	macSendLen=0;
	memset(macSendData,0x00, sizeof(szSendData));
	lValue = PubBcd2Long(PosCom.stTrans.sAmount,6,NULL); //金额
	PubLong2Char(lValue, 4, macSendData);
	macSendLen+=4;
	memcpy(macSendData+macSendLen,"\x02",1);
	macSendLen+=1;
	memcpy(macSendData+macSendLen,pInParam->TerminalNum,6);
	macSendLen+=6;
	memcpy(macSendData+macSendLen,&time,7);
	macSendLen+=7;
	memcpy(macSendData+macSendLen,"\x80",1);
	macSendLen+=1;
	while(macSendLen%8!=0){
		memcpy(macSendData+macSendLen,"\x00",1);
		macSendLen+=1;
	}

	ucRet=GetMAC(initbuf,macSendData,macSendLen,macSendData_1,macSendLen_1);
	if( ucRet!=OK )
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "MAC不正确");
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}

	memcpy(szSendData+iSendLen,gstYktEnrichLog.mac2,4);
	iSendLen+=4;
	memcpy(szSendData+iSendLen,"\x04",1);
	iSendLen+=1;

//NacTxd("1010",4);
//NacTxd(szSendData, iSendLen);
	iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
//NacTxd(szRecvData, iRecvLen);
	if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "圈存指令错");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		/*	
		iRet=Get_tran_Prove(gstYktEnrichLog.ulPOSTraceNo+1,gstYktEnrichLog.sTac);
			if(iRet!=OK)//沖正
			{

			  return NO_DISP; 

			}
			else
			{
			  ;
		}
		*/
		return NO_DISP; 
	}
	else
	{
		memcpy(gstYktEnrichLog.sTac,szRecvData,4);

	}
	
	if(iRet!=0)
	{//检测是否要沖正
		//NacTxd("1231",4);
		//SaveReversalTranData();
		ReverseTranProc();
		return NO_DISP; 

	}
	else
	{
      fileRemove(REVERSAL_FILE);
	  PubBeepOk();
	}
	
	//----------------------
	/*
	gstYktEnrichLog.ulNowBalaceAmt=gstYktEnrichLog.ulTransAmt+gstYktEnrichLog.ulOldBalaceAmt;
	//----保存控制参数-------------------------
	gstYktEnrichLog.lEnrichTraceNo=stPosParam.lEnrichTraceNo;
	gstYktEnrichLog.lEnrichCumulativeBalance=stPosParam.lEnrichCumulativeBalance+gstYktEnrichLog.ulTransAmt;

	//终端保存记录
	ucRet = SaveM1EnrichLog(0);
	if( ucRet!=OK )
	{
		lcdClrLine(2, 7);
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "保存文件操作失败", "FILE ERROR");
		ErrorBeep();
		kbGetKeyMs(2000);
	}
	stPosParam.lEnrichTraceNo++;
	stPosParam.lEnrichCumulativeBalance=stPosParam.lEnrichCumulativeBalance+gstYktEnrichLog.ulTransAmt;
	SaveAppParam();

	//打印
	PrtM1TranEnrichTicket(NORMAL);
	*/		


	/*SendTAC();

	//------------------------
	NacTxd("1232",4);
    memset(sSendPacketBuff,0,sizeof(sSendPacketBuff));
	iCnt=0;
	sSendPacketBuff[iCnt]=0x01;
	iCnt+=1;
	memcpy(&sSendPacketBuff[iCnt],"\x40\x02",2);
	iCnt+=2;
	PubLong2Char(iReportlen,2,&sSendPacketBuff[iCnt]);
	iCnt+=2;
	iRet=TDEACBCEnry(szSendMsgData,iReportlen,&ioutlen,"\xBF\x6F\x8F\x9E\xB4\xD6\xFE\x19\xA6\xC3\xFF\xEC\x27\x59\x4A\xFF");
	if(iRet!=OK)
	{//加密错误
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "加密错误");
		//lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	memcpy(&sSendPacketBuff[iCnt],szSendMsgData,ioutlen);
	iCnt+=ioutlen;
	NacTxd("9001",4);
	NacTxd(sSendPacketBuff,iCnt);
	usSendPacketLen=iCnt;
	uRet= SendRecvPacket();
	if(uRet!=0)//
	{
	 NacTxd("1238",4);
	 return NO_DISP;
	}
	NacTxd("1239",4);
	NacTxd(sRecvPacketBuff,usRecvPacketLen);
	if(memcmp(&sRecvPacketBuff[7],"\x40\x02",2)==0)
	{
	   ;
	}
	else if(memcmp(&sRecvPacketBuff[7],"\x30\x02",2)==0)
	{
       ;
	}
	else if(memcmp(&sRecvPacketBuff[7],"\x41\x02",2)==0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "未找到该条交易");
		//lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "允许范围");
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	else if(memcmp(&sRecvPacketBuff[7],"\x40\x12",2)==0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "其他错误");
	//	lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "黑名单卡");
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 

	}
	else
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "其他错误");
		//	lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "黑名单卡");
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
*/
	
	return 0;
 
}


//----
INT32 M1_Enrich(void)
{
	INT32 iCardInfoLen, iRet = 0;
	UINT8 ucRet;
	UINT8 sCardInfo[30];
	UINT8 sCsn[4];
	UINT8 sSect1KeyA[6];
	UINT8 sSectorData[50];
	UINT8 sCityCode[2];
	UINT8 sTrace[4];
	UINT8 sMac[4];
	int szTemp[50];
	ulong  reOldBalaceAmt;
	int  iOldTime,iNowTime;
	MIF_TIME_STRUCT stMifTimeInfo;
	unsigned char szCommand[200];
	int i,iCnt;
	UINT8 blkTmp[8];
	uint8_t	buf[1];

	unsigned long ultemp;
	EtcardInParam_t 	stInParam;

	if(stPosParam.lUnSettTraceNum>=100 && stPosParam.lUnSettTraceNum%20==0)
	{
		PubDisplayTitle(TRUE, "提  示");
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "请在空闲时");
		lcdDisplay(0, 6, DISP_CFONT, "结算交易数据");
		ErrorBeep();
		iRet = kbGetString(KB_EN_BIGFONT+KB_EN_CHAR+KB_EN_REVDISP, 0, 1, (3000), buf);
		if( iRet==KB_CANCEL || iRet==KEY_ENTER|| iRet==KB_TIMEOUT  )
		{}
	}

	PubDisplayTitle(TRUE, "客  充");
	memset(&gstYktEnrichLog,0,sizeof(YKTENRICH_LOG));
	strcpy((char *)gstYktEnrichLog.sTellNo, (char *)stTransCtrl.szNowTellerNo);//sTellNo

	iRet =ResetPsam();

	memset(gstYktEnrichLog.sPosSN,0,sizeof(gstYktEnrichLog.sPosSN));
	memcpy(gstYktEnrichLog.sPosSN,stPosParam.sM1TellNo,8);//终端号
	
	memset(gstYktEnrichLog.pointofsaleNo,0,sizeof(gstYktEnrichLog.pointofsaleNo));
	memcpy(gstYktEnrichLog.pointofsaleNo,stPosParam.pointofsaleNo,4);//线路/ 充值点号

	memset(gstYktEnrichLog.sCompanyNo,0,sizeof(gstYktEnrichLog.sCompanyNo));
	memcpy(gstYktEnrichLog.sCompanyNo,stPosParam.szCompanyNo,strlen(stPosParam.szCompanyNo));//公司号

	//ucRet = AppChxGetAmount(9, TRAN_AMOUNT,1);
	//if( ucRet!=OK )
	//{
	//	return ucRet;
	//}
	
//NacTxd("1111",10);

	PubBcd2Long(PosCom.stTrans.sAmount,6,&gstYktEnrichLog.ulTransAmt);//gstYktEnrichLog.ulTransAmt
	gstYktEnrichLog.ulPaidupAmt=gstYktEnrichLog.ulTransAmt;

	//if(gstYktEnrichLog.ulTransAmt<=0 || gstYktEnrichLog.ulTransAmt/100 > stPosParam.ulMaxBalaceAmt)
	//{
	//	//-------------已经超最大充值限额----
	//	lcdClrLine(2, 7);
	//	lcdDisplay(0, 4, DISP_CFONT, "超出充值点限额");
	//	ErrorBeep();
	//	sysDelayMs(3000);
	//	return NO_DISP;
	//}

	memset(szTemp,0,sizeof(szTemp));
	ConvBcdAmount(PosCom.stTrans.sAmount,(uint8_t*)szTemp);
	
	lcdClrLine(2, 7);
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "金额:%s元",szTemp); 
	lcdDisplay(0, 6, DISP_CFONT|DISP_MEDIACY, "请刷公交卡"); 
	iOldTime = sysGetTimerCount();
	mifClose();
	mifOpen();
	while(1)
    {
       iNowTime= sysGetTimerCount();
	   if( (iNowTime-iOldTime) >= (UINT32)(stPosParam.ucOprtLimitTime*1000) )	// 检查定时器
	   {
			//iRet = ERR_COMM_TIMEOUT;
			 return E_TRANS_CANCEL;
	   }
	    if (gstPosCapability.uiUnContactlessCard)
		{
            mifOpen();
			mifGetTimeOut(&stMifTimeInfo);
			stMifTimeInfo.WaitTime_PollTypeB *= 5;
			stMifTimeInfo.WaitTime_PollTypeA *= 5;
 			stMifTimeInfo.WaitTime_RATS *= 20;
			mifSetTimeOut(&stMifTimeInfo);

			memset(sCardInfo, 0x00, sizeof(sCardInfo));
			memcpy(szCommand,"\x00\xFF\xFF\x01\x00",5);
            // 在感应区内查找指定调制类型的卡片，并返回卡片的序列号等相关信息
			iRet = InListPassiveTarget(0,0,szCommand,&iCardInfoLen, sCardInfo);

			if (iRet == MIF_RET_SUCCESS)
			{
				break;
			}
		}
	   if( kbhit()==YES )
	   {
		   iRet = kbGetKey();
		   if( iRet==KEY_CANCEL )
		   {
			   return E_TRANS_CANCEL;
		   }
	   }
   }
	
	memset(sCsn, 0, sizeof(sCsn));
	memcpy(stTemp.sCardUid,sCardInfo+5, 4);

	if(memcmp(&sCardInfo[3],"\x28",1)==0 || memcmp(&sCardInfo[3],"\x20",1)==0)// CPU卡充值
	{
		iRet = InATR(&iCardInfoLen,sCardInfo);
		if (iRet != 0)
		{
			return E_TRANS_FAIL;
		}

		memcpy(gstYktEnrichLog.sCSN,&sCardInfo[12],4);
		
		stInParam.ucSwipedFlag=CARD_PASSIVE;
		GetPosTime(stInParam.szDate, stInParam.szTime);	
		PubAsc2Bcd(stInParam.szDate,8,&gstYktEnrichLog.sTranDateTime[0]);
		PubAsc2Bcd(stInParam.szTime,6,&gstYktEnrichLog.sTranDateTime[4]);
	
		ucRet = CPU_ReadEnrichData(&stInParam);// 处理CPU卡文件
		if(ucRet !=OK)
		{
		   return NO_DISP;
		}

		//gstYktEnrichLog.ulMaxBalaceAmt= stPosParam.ulMaxBalaceAmt*100;
		//if(gstYktEnrichLog.ulMaxBalaceAmt<gstYktEnrichLog.ulOldBalaceAmt+gstYktEnrichLog.ulTransAmt)
		//{
		//	//-------------已经超最大充值金额----
		//	lcdClrLine(2, 7);
		//	lcdDisplay(0, 4, DISP_CFONT, "超出充值限额");
		//	ErrorBeep();
		//	sysDelayMs(3000);
		//	return NO_DISP;
		//}
		
		//NacTxd("2222",4);
		
		ucRet = CPU_EnrichWriteData(&stInParam);
		if(ucRet !=OK)
		{
		   return NO_DISP;
		}
		
	}
	else if(memcmp(&sCardInfo[3],"\x08",1)==0)//M1卡充值
	{
		ultemp=stTemp.sCardUid[0]+stTemp.sCardUid[1]*256+stTemp.sCardUid[2]*256*256+stTemp.sCardUid[3]*256*256*256;
		memset(gstYktEnrichLog.sCSN,0,sizeof(gstYktEnrichLog.sCSN));
		sprintf((INT8 *)gstYktEnrichLog.sCSN, "%010u", ultemp);
		
		memcpy(sSect1KeyA,"\xA0\xA1\xA2\xA3\xA4\xA5",6);
		memcpy(&stTemp.sKeyBList ,sSect1KeyA,6);
		ucRet =M1_ReadSector(0, sSect1KeyA,MIF_KEYA, sSectorData,stTemp.sCardUid);//返回48字节
		if( ucRet!=OK )
		{
		  return E_TRANS_FAIL;
		}
		memcpy(stTemp.sCardDataList,sSectorData,48);

		memcpy(sSect1KeyA, stTemp.sCardUid, 4);
		memcpy(sSect1KeyA+4, stTemp.sCardUid, 2);
		memset(sSectorData, 0, sizeof(sSectorData));
		memcpy(&stTemp.sKeyBList[1*6] ,sSect1KeyA,6);

		ucRet =M1_ReadSector(1, sSect1KeyA,MIF_KEYA, sSectorData,stTemp.sCardUid);//返回48字节
		if(ucRet !=0)
		{
			return NO_DISP;
		}
		memcpy(stTemp.sCardDataList[4],sSectorData,48);

		//保存临时变量
		memcpy(sCityCode,&sSectorData[0],2);//
		memcpy(sTrace,&sSectorData[4],4);// liu shui
		memcpy(sMac,&sSectorData[8],4);//ren zhen ma
		PubBcd2Asc(&sSectorData[0],8,gstYktEnrichLog.sCardSerialNo);
		gstYktEnrichLog.uCardtype=sSectorData[13];//卡类型

		//判断黑名单
		PubBcd2Asc(&sSectorData[4],4,blkTmp);

		iRet = ChkIfBlkCard(blkTmp);
		if(iRet !=0)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 2, DISP_CFONT, "黑名单卡");
			lcdDisplay(0, 4, DISP_CFONT, "请联系管理员!");
			ErrorBeep();
			sysDelayMs(3000);
			return NO_DISP;
		}
		//sSectorOper	
		//	memset(sAllKeyB,0,sizeof(sAllKeyB));
		//算各扇区秘钥
		iRet= GetSecPW(stTemp.sCardUid, sTrace, sMac, sCityCode, "\x00\x01\x10\x03\x03\x03\x06\x11\x12\x13\x14\x15\x16\x17\x18\x19","1111111110000000", 1, stTemp.sKeyBList);
		if(iRet !=0)
		{
			return NO_DISP;
		}
//NacTxd("1008", 4);
//NacTxd(stTemp.sKeyBList, 6*16);
		iRet = GetAllDataFromM1Card();//获取卡内所有数据
		if(iRet != 0)
		{
			return iRet;
		}
		iRet = M1_IsCardInfoLegal();//判断卡内数据的合法性
		if(iRet != 0)
		{
			return iRet;
		}
		memset(stTemp.sCardDataListSub, 0, sizeof(stTemp.sCardDataListSub));
		memcpy(stTemp.sCardDataListSub, stTemp.sCardDataList, 64*16);
		
		//读钱包，读BLOCK9,
		//memcpy(gstYktEnrichLog.uOldBalaceAmt,sCardDataList[2*4+1],4);
		gstYktEnrichLog.ulOldBalaceAmt=stTemp.sCardDataList[2*4+1][0]+stTemp.sCardDataList[2*4+1][1]*256
			+stTemp.sCardDataList[2*4+1][2]*256*256+stTemp.sCardDataList[2*4+1][3]*256*256*256;
		gstYktEnrichLog.ulMaxBalaceAmt= stPosParam.ulMaxBalaceAmt*100;

		////判断，充值最大金额，充值上线
		//if(gstYktEnrichLog.ulMaxBalaceAmt<gstYktEnrichLog.ulOldBalaceAmt+gstYktEnrichLog.ulTransAmt)
		//{
		//	//-------------已经超最大充值金额----
		//	lcdClrLine(2, 7);
		//	lcdDisplay(0, 4, DISP_CFONT, "超出充值限额");
		//	ErrorBeep();
		//	sysDelayMs(3000);
		//	return NO_DISP;
		//}
		
		iRet=M1_RchAmtProcess();//充值
		if(iRet!=0)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 4, DISP_CFONT, "充值失败");
			ErrorBeep();
			sysDelayMs(3000);
			return NO_DISP;
		}

		//再次获取卡内所有数据验证是否充值成功
		memset(sSectorData,0,sizeof(sSectorData));//stTemp.sKeyBList
		ucRet =M1_ReadSector(2, &stTemp.sKeyBList[6*2],MIF_KEYB, sSectorData,stTemp.sCardUid);//返回48字节
		//NacTxd(&i, 4);
		if(ucRet !=0)
		{
			return NO_DISP;
		}
		memcpy(stTemp.sCardDataList[2*4],sSectorData,48);
		reOldBalaceAmt=stTemp.sCardDataList[2*4+1][0]+stTemp.sCardDataList[2*4+1][1]*256
		+stTemp.sCardDataList[2*4+1][2]*256*256+stTemp.sCardDataList[2*4+1][3]*256*256*256;

		if(reOldBalaceAmt-gstYktEnrichLog.ulTransAmt != gstYktEnrichLog.ulOldBalaceAmt)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 4, DISP_CFONT, "充值失败");
			ErrorBeep();
			sysDelayMs(3000);
			return NO_DISP;
		}
	}

	gstYktEnrichLog.ulNowBalaceAmt=gstYktEnrichLog.ulTransAmt+gstYktEnrichLog.ulOldBalaceAmt;
	//----保存控制参数-------------------------
	gstYktEnrichLog.lEnrichTraceNo=stPosParam.lEnrichTraceNo;
	gstYktEnrichLog.lEnrichCumulativeBalance=stPosParam.lEnrichCumulativeBalance+gstYktEnrichLog.ulTransAmt;
	gstYktEnrichLog.uMode=0;

	//终端保存记录
	ucRet = SaveM1EnrichLog(0);
	if( ucRet!=OK )
	{
		lcdClrLine(2, 7);
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "保存文件操作失败", "FILE ERROR");
		ErrorBeep();
		kbGetKeyMs(2000);
	}
	stPosParam.lEnrichTraceNo++;
	stPosParam.lEnrichCumulativeBalance=stPosParam.lEnrichCumulativeBalance+gstYktEnrichLog.ulTransAmt;
	//stPosParam.maxAgentAmt=stPosParam.maxAgentAmt-gstYktEnrichLog.ulTransAmt;
	stPosParam.lUnSettTraceNum++;

	SaveAppParam();

	//打印
	PrtM1TranEnrichTicket(NORMAL);

//提示充值成功

//-----再读取数据。输出--------------------------------------------

  /*NacTxd("OK11",4);
 int i;
 for(i=0;i<=7;i++)
 	{
     ucRet =M1_ReadSector(i, &stTemp.sKeyBList[6*i],MIF_KEYB, sSectorData,stTemp.sCardUid);//返回48字节
	NacTxd("0007", 4);
    
	if(ucRet !=0)
	{
		return NO_DISP;
	}
	NacTxd(&i,4);
	NacTxd(sSectorData, 48);
 	}*/
//--------------------------------------------------
	M1_mifClose();
	PubBeepOk();
	lcdClrLine(2, 7);
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "请移走卡");
	
	//PubWaitKey(0);
//	showDebugMessage(iCardInfoLen, sCardInfo, "read card succ!");

	return 0;
}


UINT8 CPU_SaleWriteData(pEtcardInParam_t  pInParam)
{
    //---------初始化指令，返回报文体明文-------
	//uint8_t szSendMsgData[500];
	//uint8_t szRevMsgData[500];
	int iReportlen,ioutlen;
	int iRet;//,iCnt;
	uint8_t ucRet;//uRet,
    //uint8_t sMsgBodyKey[16];

	int 	   iSendLen, iRecvLen;
	UINT8 szSendData[300], szRecvData[300];
	//int  macSendLen_1,macRecvLen;//macSendLen,
	//UINT8 macSendData[300], macRecvData[300];//生成MAC2  所用到数据
	//UINT8 macSendData_1[300],mac1[4];//验证MAC1  所用数据
	UINT8 initbuf[300];//圈存初始化返回结果
	int lValue;
	APDU_SEND stApduSend;
	APDU_RESP stApduResp;
	UINT8 terminal[6];
	UINT8 keyindex[1];
	uint8_t time[11];
	UINT8 sBlance[10];
	UINT8 city[4];

	/////////////////选择公共钱包:\x00\xA4\x04\x00\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01
	iSendLen = 0;
    iRecvLen = 0;  
	memset(szSendData, 0x00, sizeof(szSendData));
    memset(szRecvData, 0x00, sizeof(szRecvData));
	memcpy(szSendData,"\x00\xA4\x04\x00\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01",14);//memcpy(szSendData,"\x00\xA4\x00\x00\x02\x3F\x00\x00",8);
	iSendLen = 14;
//NacTxd("6002",5);
//NacTxd(szSendData, iSendLen);
	iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
//NacTxd(szRecvData, iRecvLen);
	if ((iRet !=MIF_RET_SUCCESS) && (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
	{
		if ((iRet !=MIF_RET_SUCCESS) && (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "主应用选择错");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		}
	}

	/////////////////////---余额查询///////////////////////////
	iSendLen = 0;
	iRecvLen = 0;	
	memset(szSendData, 0x00, sizeof(szSendData));
	memset(szRecvData, 0x00, sizeof(szRecvData));
	memcpy(szSendData,"\x80\x5C\x00\x02\x04",5);
	iSendLen = 5;
//NacTxd("6003",5);
//NacTxd(szSendData, iSendLen);
	iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
//NacTxd(szRecvData, iRecvLen);
	if ((iRet ==MIF_RET_SUCCESS) && (szRecvData[iRecvLen-2] == 0x90 && szRecvData[iRecvLen-1]== 0x00))
	{
		memcpy(sBlance, szRecvData, 4);
		gstYktEnrichLog.ulOldBalaceAmt=sBlance[0]*256*256*256+sBlance[1]*256*256+
		sBlance[2]*256+sBlance[3];
	}
	else
	{
	    lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "余额查询错");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}

	gstYktEnrichLog.ulMaxBalaceAmt= stPosParam.ulMaxBalaceAmt*100;

	if(gstYktEnrichLog.ulOldBalaceAmt<gstYktEnrichLog.ulTransAmt )
	{
		//-------------余额不足----
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "余额不足");
		ErrorBeep();
		sysDelayMs(3000);
		return NO_DISP;

	}

	//判断，消费最大金额，消费上线
	if(gstYktEnrichLog.ulMaxBalaceAmt<gstYktEnrichLog.ulTransAmt)
	{
		//-------------已经超最大消费金额----
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "超出消费限额");
		ErrorBeep();
		//lcdDisplay(0, 6, DISP_CFONT, "请重试!");
		sysDelayMs(3000);
		return NO_DISP;

	}

	/////////////////////读用户卡0015获取应用序列号: \x00\xB0\x95\x00\x1E/////////////////////
	iRecvLen = 0;  
	memset(szSendData, 0x00, sizeof(szSendData));
	memset(szRecvData, 0x00, sizeof(szRecvData));
	memcpy(szSendData,"\x00\xB0\x95\x00\x1E",5);
	iSendLen = 5;
//NacTxd("7001",5);
//NacTxd(szSendData, iSendLen);
	iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
//NacTxd(szRecvData, iRecvLen);
	if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
	{
	   lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "获取应用序列号错");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	PubBcd2Asc(&szRecvData[12],8,gstYktEnrichLog.sCardSerialNo);//卡应用序列号 
	//gstYktEnrichLog.uCardtype=szRecvData[15];
	memcpy(city,&szRecvData[2],2);
//NacTxd("111111111",10);
//NacTxd(city,10);
	/*
	if(memcmp(&city,"\x31\x31",2)!=0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "外地卡，扣款失败");
		ErrorBeep();
		sysDelayMs(3000);
		return NO_DISP;
	}
	*/
	iSendLen = 0;
    iRecvLen = 0;  
    memset(szSendData, 0x00, sizeof(szSendData));
    memset(szRecvData, 0x00, sizeof(szRecvData));
    memcpy(szSendData,"\x00\xB0\x85\x00\x00",5);
    iSendLen = 5;
    iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
//NacTxd("0016", 4);
//NacTxd(szSendData,iSendLen);
//NacTxd(szRecvData,iRecvLen);
//NacTxd(gstYktEnrichLog.sCardSerialNo,20);

	if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
    {
	    lcdClrLine(2, 7);
	    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "读消费卡文件错");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	// return ERR_CODE_USECARD_SELECT_ADF;
    }
	gstYktEnrichLog.uCardtype=szRecvData[0];

	////////////////////PSAM卡选择文件: \x00\xA4\x00\x00\x02\x3F\x00///////////////
	ioutlen = 0;
	iReportlen = 0;
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS =0xA4;
	stApduSend.P1 = 0x00;
	stApduSend.P2 = 0x00;
	stApduSend.Lc = 0x02;
	memcpy(stApduSend.DataIn,"\x3F\x00",2);
	stApduSend.Le = 0x00;
	stApduSend.LeFlag = 0x01;
	
//NacTxd("8000",4);
//NacTxd(&stApduSend, ioutlen+7);
	iRet = iccExchangeAPDU(ICC_SAM2,&stApduSend,&stApduResp);
//NacTxd(&stApduResp, iReportlen+10);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "MAC错误");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X",  stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}

	////////////////////PSAM卡读取终端编号: 00 B0 96 00 06//////////////////
	ioutlen = 0;
	iReportlen = 0;
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA =0x00;
	stApduSend.INS =0xB0;
	stApduSend.P1 = 0x96;
	stApduSend.P2 = 0x00;
	stApduSend.Lc = 0x00;
	stApduSend.Le = 0x06;
	stApduSend.LeFlag = 0x01;
	
//NacTxd("8001",4);
//NacTxd(&stApduSend, ioutlen+7);
	iRet = iccExchangeAPDU(ICC_SAM2,&stApduSend,&stApduResp);
//NacTxd(&stApduResp, iReportlen+10);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "读取终端编号错");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X",  stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	memcpy(terminal, &stApduResp.DataOut, 6);

/*
	////////////////////PSAM卡密钥索引 00 B0 97 00 01 //////////////////
	ioutlen = 0;
	iReportlen = 0;
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA =0x00;
	stApduSend.INS =0xB0;
	stApduSend.P1 = 0x97;
	stApduSend.P2 = 0x00;
	stApduSend.Lc = 0x00;
	stApduSend.Le = 0x01;
	stApduSend.LeFlag = 0x01;
NacTxd("8002",4);
NacTxd(&stApduSend, ioutlen+7);
	iRet = iccExchangeAPDU(ICC_SAM2,&stApduSend,&stApduResp);
NacTxd(&stApduResp, iReportlen+10);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "读取密钥索引错");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X",  stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	memcpy(keyindex, &stApduResp.DataOut, 1);
	*/

	/*住建部密钥时用到
	////////////////////用户卡获取安全识别码:\x80\xCA\x00\x00\x09//////////////////
	iSendLen = 0;
	iRecvLen = 0;  
	memset(szSendData, 0x00, sizeof(szSendData));
	memset(szRecvData, 0x00, sizeof(szRecvData));
	memcpy(szSendData,"\x80\xCA\x00\x00\x09",5);
	iSendLen = 5;
//NacTxd("8002",5);
//NacTxd(szSendData, iSendLen);
	iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
//NacTxd(szRecvData, iRecvLen);
	if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
	{
	   lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "获取安全识别码错");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	  // return ERR_CODE_VERIFY_PIN;
	}
	*/
	
	////////////////////PSAM卡选择文件:\x00\xA4\x00\x00\x02\x10\x01///////////////
	//	00A4000002ADF1
	ioutlen = 0;
	iReportlen = 0;
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS =0xA4;
	stApduSend.P1 = 0x00;
	stApduSend.P2 = 0x00;
	stApduSend.Lc = 0x02;
	stApduSend.Le = 0x00;
	stApduSend.LeFlag = 0x01;
	memcpy(stApduSend.DataIn,"\xAD\xF1",2);
	
//NacTxd("8003",4);
//NacTxd(&stApduSend, ioutlen+7);
	iRet = iccExchangeAPDU(ICC_SAM2,&stApduSend,&stApduResp);
//NacTxd(&stApduResp, iReportlen+10);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "MAC错误");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X",  stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	
	/*
	////////////PSAM卡验证用户安全识别码:80 CA 00 00 09+安全码///////////////
	ioutlen = 0;
	iReportlen = 0;
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA =0x80;
	stApduSend.INS =0xCA;
	stApduSend.P1 = 0x00;
	stApduSend.P2 = 0x00;
	stApduSend.Lc = 0x09;
	stApduSend.Le = 0x00;
	stApduSend.LeFlag = 0x01;
	memcpy(stApduSend.DataIn,szRecvData,9);
	
//NacTxd("8004",4);
//NacTxd(&stApduSend, 15);
	iRet = iccExchangeAPDU(ICC_SAM2,&stApduSend,&stApduResp);
//NacTxd(&stApduResp, 30);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "验证错误");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X",  stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	*/
	////////////////////普通消费初始化//////////////////
	//\x80\x50\x01\x02\x0B\x01+00000001（消费金额8位）+（测试2返回的终端编号）+0F
	PubAsc2Bcd(stPosParam.szPosId,12,pInParam->TerminalNum);
	iSendLen = 0;
	iRecvLen = 0;  
	//memset(szSendData, 0x00, sizeof(szSendData));
	//memset(szRecvData, 0x00, sizeof(szRecvData));
	memcpy(szSendData,"\x80\x50\x01\x02\x0B",5); //p1=01是普通消费//p1=03是复合消费，PSAM卡初始化中交易类型标识取值要与此一致
	iSendLen = 5;
	memcpy(szSendData+iSendLen, "\x01", 1);//密钥索引PSAM卡00B0970001指令
	iSendLen+=1;
	lValue = PubBcd2Long(PosCom.stTrans.sAmount,6,NULL);
	PubLong2Char(lValue, 4, szSendData+iSendLen);
	iSendLen+=4;  
	memcpy(szSendData+iSendLen, terminal, 6);
	iSendLen+=6; 
	memcpy(szSendData+iSendLen, "\x0F", 1);
	iSendLen+=1;
//NacTxd("8005",4);
//NacTxd(szSendData, iSendLen);
	iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
//NacTxd(szRecvData, iRecvLen);

	if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
	{
	    lcdClrLine(2, 7);
	   /* lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "消费初始化错");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);*/
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "读卡失败");
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	//return ERR_CODE_INIT_LOAD;
	}
	memcpy(initbuf,szRecvData, iRecvLen);
//NacTxd("8006",4);	
	
	//-----------------
	SaveReversalTranData();
//NacTxd(gstYktEnrichLog.sTranDateTime,70);
	//memcpy(gstYktEnrichLog.sTranDateTime,&sRecvPacketBuff[7+2],7);

	memcpy(time,&gstYktEnrichLog.sTranDateTime,7);

	///////////////////////PSAM卡消费初始化(计算MAC1)///////////////////////////
	//\x80\x70\x00\x00\x24
	//测试8返回的从22开始8位随机数+测试8返回的从8开始4位交易序列号
	//8位充值金额+06+交易时间（yyyyMMddHHmmss）
	//0100+测试7返回的应用序列号+4位城市编码+FF0000000000
	//验证MAC1  的数据
	ioutlen = 0;
	iReportlen = 0;
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0x80;
	stApduSend.INS = 0x70;
	stApduSend.P1 = 0x00;
	stApduSend.P2 = 0x00;
	stApduSend.Lc = 0x24;
	//memcpy(stApduSend.DataIn,"\xCD\xEC\x00\xFF\xFF\xFF\xFF\xFF",8);
	memcpy(stApduSend.DataIn,initbuf + 11,4);//8位随机数
	ioutlen+=4;
	memcpy(stApduSend.DataIn+ioutlen,initbuf + 4,2);//4位交易序列号
	ioutlen+=2;
	lValue = PubBcd2Long(PosCom.stTrans.sAmount,6,NULL); //金额
	PubLong2Char(lValue, 4, stApduSend.DataIn+ioutlen);
	ioutlen+=4;
	memcpy(stApduSend.DataIn+ioutlen,"\x06",1);//交易类型标识06普通消费 09复合消费
	ioutlen+=1;
	memcpy(stApduSend.DataIn+ioutlen,&time,7);//交易日期时间
	ioutlen+=7;
	memcpy(stApduSend.DataIn+ioutlen,"\x01",1); //消费密钥版本号 
	ioutlen+=1;
	//memcpy(stApduSend.DataIn+ioutlen,initbuf + 10,1);//消费密钥算法标识
	memcpy(stApduSend.DataIn+ioutlen,"\x00",1);
	ioutlen+=1;
	PubAsc2Bcd(gstYktEnrichLog.sCardSerialNo,16,stApduSend.DataIn+ioutlen);//卡应用序列号
	ioutlen+=8;
	memcpy(stApduSend.DataIn+ioutlen,"\x75\x00",2);//城市编码
	ioutlen+=2;
	memcpy(stApduSend.DataIn+ioutlen,"\xFF\x00\x00\x00\x00\x00",6);
	ioutlen+=6;
	
	//while(ioutlen%8!=0){
	//	memcpy(stApduSend.DataIn+ioutlen,"\x00",1);
	//	ioutlen+=1;
	//}
	stApduSend.Le = 0x00;
	stApduSend.LeFlag = 1;
	
//NacTxd("8007",4);
//NacTxd(&stApduSend, ioutlen + 5);
	iRet = iccExchangeAPDU(ICC_SAM2,&stApduSend,&stApduResp);
//NacTxd(&stApduResp,15);

	memset(initbuf, 0x00, sizeof(initbuf));
	memcpy(initbuf,&stApduResp, 15);
//NacTxd(&initbuf,15);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "MAC错误");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X",  stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	
	/*memcpy(macSendData_1+macSendLen_1,"\x80",1);
	macSendLen_1+=1;
	while(macSendLen_1%8!=0){
		memcpy(macSendData+macSendLen_1,"\x00",1);
		macSendLen_1+=1;
	}*/

	///////////////////////////消费////////////////////////////////////
	//\x80\x54\x01\x00\x0F
	//上一步返回的从0开始8位交易号+交易时间（一定要跟上步中的时间值一样）
	//+测试9返回的从8开始8位MAC1
	iSendLen = 0;
	iRecvLen = 0;  
	memset(szRecvData, 0x00, sizeof(szRecvData));
	memset(szSendData, 0x00, sizeof(szSendData));
	
	memcpy(szSendData,"\x80\x54\x01\x00\x0F",5);
	iSendLen = 5;
	memcpy(szSendData+iSendLen,&initbuf[4], 4);
	iSendLen += 4;
	memcpy(szSendData+iSendLen,&time,7);
	iSendLen+=(7);
	memcpy(szSendData+iSendLen,&initbuf[8],4);
	iSendLen+=4;

//NacTxd("8008",4);	
//NacTxd(szSendData, iSendLen);	
	iRet = InDataExchange(iSendLen,szSendData,&iRecvLen,szRecvData);
//NacTxd(szRecvData, iRecvLen);
	if ((iRet !=MIF_RET_SUCCESS) || (szRecvData[iRecvLen-2] != 0x90 || szRecvData[iRecvLen-1]!= 0x00))
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "消费指令错");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	else
	{
		memcpy(gstYktEnrichLog.sTac,szRecvData,4);
	}
	
	if(iRet!=0)
	{//检测是否要沖正
		//NacTxd("1231",4);
		//SaveReversalTranData();
		ReverseTranProc();
		return NO_DISP; 

	}
	else
	{
      fileRemove(REVERSAL_FILE);
	  PubBeepOk();
	}
	
	//----------------------
	gstYktEnrichLog.ulNowBalaceAmt=gstYktEnrichLog.ulOldBalaceAmt-gstYktEnrichLog.ulTransAmt;
	//----保存控制参数-------------------------
	gstYktEnrichLog.lEnrichTraceNo=stPosParam.lEnrichTraceNo;
	gstYktEnrichLog.lSaleCumulativeBalance=stPosParam.lSaleCumulativeBalance+gstYktEnrichLog.ulTransAmt;
	gstYktEnrichLog.uMode=1;//0充值，1消费

	//终端保存记录
	ucRet = SaveM1EnrichLog(0);
	if( ucRet!=OK )
	{
		lcdClrLine(2, 7);
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "保存文件操作失败", "FILE ERROR");
		ErrorBeep();
		kbGetKeyMs(2000);
	}

	stPosParam.lEnrichTraceNo++;
	stPosParam.lSaleCumulativeBalance=stPosParam.lSaleCumulativeBalance+gstYktEnrichLog.ulTransAmt;
	SaveAppParam();

	//打印
	PrtM1TranSaleTicket(NORMAL);
	
	return 0;
 
}


INT32 M1_Sale(void)
{
	INT32 iCardInfoLen, iRet = 0;
	UINT8 ucRet;
	UINT8 sCardInfo[30];
	UINT8 sCsn[4];
	UINT8 sSect1KeyA[6];
	//UINT8 sSect1KeyB[6];
	UINT8 sSectorData[50];
	UINT8 sCityCode[2];
	UINT8 sTrace[4];
	UINT8 sMac[4];
	int szTemp[50];
	ulong  reOldBalaceAmt;
//	UINT8 szPsamId[6];
//	UINT8 sSectorOper[16];
	//UINT8 sAllKeyA[16*6];
	//UINT8 sAllKeyB[16*6];
//	UINT8 sCardDataList[64][16];	  //S50卡所有数据信息 64个block，每个block16字节
//	UINT8 sBlockData[16];
//	ulong ulBalaceAmt;
//	UINT8 sTimeStr[7];
//	UINT8 sPubKey[6];
	//UINT8 u8Ret;
 //   int itemp;
	int  iOldTime,iNowTime;
	MIF_TIME_STRUCT stMifTimeInfo;
	unsigned char szCommand[200];
	int i,iCnt;
	//char   data[16];
	uint8_t	buf[1];

//	int   iDatalen=0;
//	char  szBuffer[300];
//	char		  szAmount[10];
	unsigned long ultemp;
	UINT8 blkTmp[8];

	EtcardInParam_t 	stInParam;

	if(stPosParam.lUnSettTraceNum>=100 && stPosParam.lUnSettTraceNum%20==0)
	{
		PubDisplayTitle(TRUE, "提  示");
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "请在空间时");
		lcdDisplay(0, 6, DISP_CFONT, "结算交易数据");
		ErrorBeep();
		iRet = kbGetString(KB_EN_BIGFONT+KB_EN_CHAR+KB_EN_REVDISP, 0, 1, (3000), buf);
		if( iRet==KB_CANCEL || iRet==KEY_ENTER|| iRet==KB_TIMEOUT  )
		{}
	}

	PubDisplayTitle(TRUE, "消  费");
	memset(&gstYktEnrichLog,0,sizeof(YKTENRICH_LOG));
	strcpy((char *)gstYktEnrichLog.sTellNo, (char *)stTransCtrl.szNowTellerNo);//sTellNo
	//iRet =ResetPsam(ICC_SAM1, ICC_ISO);//复位psam卡,里面提示错误
	iRet =ResetPsam2();//卡槽2
	
	if(iRet !=0)
	{
		lcdClrLine(2, 7);
		PubDispString("SAM上电复位失败", 3|DISP_LINE_CENTER);
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
	//--------管理卡ISAM卡认证------------
	/*ucRet =EnrichAuthenticate();
	if( ucRet!=OK )
	{
		return ucRet;
	} */
	/*
	iRet = GetPsamId(gstYktEnrichLog.sPsamId);
	if(iRet !=0)
	{
		return NO_DISP;
	}*/
	/*iRet = GetTellNo(gstYktEnrichLog.sNowTellNo);
	if(iRet !=0)
	{
		return NO_DISP;
	}*/
	//memcpy(gstYktEnrichLog.sNowTellNo,"\x00\x00\x12\x34",4);
	//PubAsc2Bcd(stPosParam.sM1TellNo,4,&gstYktEnrichLog.sNowTellNo[2]);
	//memcpy(gstYktEnrichLog.sCompanyNo,stPosParam.szCompanyNo,4);
	//memcpy(gstYktEnrichLog.pointofsaleNo,stPosParam.pointofsaleNo,3);

	memset(gstYktEnrichLog.sPosSN,0,sizeof(gstYktEnrichLog.sPosSN));
	memcpy(gstYktEnrichLog.sPosSN,stPosParam.sM1TellNo,8);//终端号
	
	memset(gstYktEnrichLog.pointofsaleNo,0,sizeof(gstYktEnrichLog.pointofsaleNo));
	memcpy(gstYktEnrichLog.pointofsaleNo,stPosParam.pointofsaleNo,4);//线路/ 充值点号

	memset(gstYktEnrichLog.sCompanyNo,0,sizeof(gstYktEnrichLog.sCompanyNo));
	memcpy(gstYktEnrichLog.sCompanyNo,stPosParam.szCompanyNo,strlen(stPosParam.szCompanyNo));//公司号
	
//NacTxd(gstYktEnrichLog.sPsamId,10);
//NacTxd(gstYktEnrichLog.sNowTellNo,4);//
	//ucRet = AppChxGetAmount(9, TRAN_AMOUNT,1);
	//if( ucRet!=OK )
	//{
	//	return ucRet;
	//}
//NacTxd("1111",10);
	PubBcd2Long(PosCom.stTrans.sAmount,6,&gstYktEnrichLog.ulTransAmt);//gstYktEnrichLog.ulTransAmt
	gstYktEnrichLog.ulPaidupAmt=gstYktEnrichLog.ulTransAmt;

	memset(szTemp,0,sizeof(szTemp));
	ConvBcdAmount(PosCom.stTrans.sAmount,(uint8_t*)szTemp);
	
	lcdClrLine(2, 7);
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "金额:%s元",szTemp); 
	lcdDisplay(0, 6, DISP_CFONT|DISP_MEDIACY, "请刷公交卡"); 
	iOldTime = sysGetTimerCount();
	mifClose();
	mifOpen();
	while(1)
    {
       iNowTime= sysGetTimerCount();
	   if( (iNowTime-iOldTime) >= (UINT32)(stPosParam.ucOprtLimitTime*1000) )	// 检查定时器
	   {
			//iRet = ERR_COMM_TIMEOUT;
			 return E_TRANS_CANCEL;
	   }
	    if (gstPosCapability.uiUnContactlessCard)
		{
            mifOpen();
			mifGetTimeOut(&stMifTimeInfo);
			stMifTimeInfo.WaitTime_PollTypeB *= 5;
			stMifTimeInfo.WaitTime_PollTypeA *= 5;
 			stMifTimeInfo.WaitTime_RATS *= 20;
			mifSetTimeOut(&stMifTimeInfo);

			memset(sCardInfo, 0x00, sizeof(sCardInfo));
			memcpy(szCommand,"\x00\xFF\xFF\x01\x00",5);
            // 在感应区内查找指定调制类型的卡片，并返回卡片的序列号等相关信息
			iRet = InListPassiveTarget(0,0,szCommand,&iCardInfoLen, sCardInfo);

			if (iRet == MIF_RET_SUCCESS)
			{
				break;
			}
		}
	   if( kbhit()==YES )
	   {
		   iRet = kbGetKey();
		   if( iRet==KEY_CANCEL )
		   {
			   return E_TRANS_CANCEL;
		   }
	   }
   }

	memset(sCsn, 0, sizeof(sCsn));
	memcpy(stTemp.sCardUid,sCardInfo+5, 4);

	if(memcmp(&sCardInfo[3],"\x28",1)==0 || memcmp(&sCardInfo[3],"\x20",1)==0){// CPU卡消费

		iRet = InATR(&iCardInfoLen,sCardInfo);
		if (iRet != 0)
		{
			return E_TRANS_FAIL;
		}
		
		stInParam.ucSwipedFlag=CARD_PASSIVE;
		GetPosTime(stInParam.szDate, stInParam.szTime);	
		PubAsc2Bcd(stInParam.szDate,8,&gstYktEnrichLog.sTranDateTime[0]);
		PubAsc2Bcd(stInParam.szTime,6,&gstYktEnrichLog.sTranDateTime[4]);

		/*
		ucRet = CPU_ReadSaleCard(&stInParam);// 处理CPU卡文件

		if(ucRet !=OK)
		{
		   return NO_DISP;
		}

		*/
		
//NacTxd("2222",4);
		ucRet = CPU_SaleWriteData(&stInParam);
		if(ucRet !=OK)
		{
		   return NO_DISP;
		}
	}else if(memcmp(&sCardInfo[3],"\x08",1)==0){//M1卡消费
	
		ultemp=stTemp.sCardUid[0]+stTemp.sCardUid[1]*256+stTemp.sCardUid[2]*256*256+stTemp.sCardUid[3]*256*256*256;
		//PubChar2Long(gstYktEnrichLog.sCSN,4,&ultemp);
		memset(gstYktEnrichLog.sCSN,0,sizeof(gstYktEnrichLog.sCSN));
		sprintf((INT8 *)gstYktEnrichLog.sCSN, "%010u", ultemp);
		
		memcpy(sSect1KeyA,"\xA0\xA1\xA2\xA3\xA4\xA5",6);
		memcpy(&stTemp.sKeyBList ,sSect1KeyA,6);
		ucRet =M1_ReadSector(0, sSect1KeyA,MIF_KEYA, sSectorData,stTemp.sCardUid);//返回48字节
		if( ucRet!=OK )
		{
		  return E_TRANS_FAIL;
		}
		memcpy(stTemp.sCardDataList,sSectorData,48);
	//NacTxd("8001", 4);
	//NacTxd(sSectorData, 48);
		//----------------
		memcpy(sSect1KeyA, stTemp.sCardUid, 4);
		memcpy(sSect1KeyA+4, stTemp.sCardUid, 2);
		memset(sSectorData, 0, sizeof(sSectorData));
		memcpy(&stTemp.sKeyBList[1*6] ,sSect1KeyA,6);
	//NacTxd("9001", 4);
	//NacTxd(sSect1KeyA,6);
		ucRet =M1_ReadSector(1, sSect1KeyA,MIF_KEYA, sSectorData,stTemp.sCardUid);//返回48字节
		if(ucRet !=0)
		{
			return NO_DISP;
		}
		memcpy(stTemp.sCardDataList[4],sSectorData,48);
	//NacTxd("8002", 4);
	//NacTxd(sSectorData, 48);

		//保存临时变量
		memcpy(sCityCode,&sSectorData[0],2);//
		memcpy(sTrace,&sSectorData[4],4);// liu shui
		memcpy(sMac,&sSectorData[8],4);//ren zhen ma
		PubBcd2Asc(&sSectorData[0],8,gstYktEnrichLog.sCardSerialNo);
		gstYktEnrichLog.uCardtype=sSectorData[13];//卡类型
	//NacTxd(sMac, 8);

		//判断黑名单
		PubBcd2Asc(&sSectorData[4],4,blkTmp);
	//NacTxd(blkTmp, 8);
		iRet = ChkIfBlkCard(blkTmp);
		if(iRet !=0)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 2, DISP_CFONT, "黑名单卡");
			lcdDisplay(0, 4, DISP_CFONT, "请联系管理员!");
			ErrorBeep();
			sysDelayMs(3000);
			return NO_DISP;
		}


	//sSectorOper	
	//	memset(sAllKeyB,0,sizeof(sAllKeyB));
		
		iRet= M1_GetCalKeyA(stTemp.sCardUid, sTrace, sMac, sCityCode, "\x00\x01\x10\x03\x03\x03\x06\x11\x12\x13\x14\x15\x16\x17\x18\x19","1111111110000000", stTemp.sKeyBList);
		if(iRet !=0)
		{
			return NO_DISP;
		}
	//NacTxd("1008", 4);
	//NacTxd(stTemp.sKeyBList, 6*16);

		
		//获取卡内所有数据
		for(i=0;i<=8;i++)
		{
			memset(sSectorData,0,sizeof(sSectorData));//stTemp.sKeyBList
			ucRet =M1_ReadSector(i, &stTemp.sKeyBList[6*i],MIF_KEYA, sSectorData,stTemp.sCardUid);//返回48字节
			//NacTxd(&i, 4);
			if(ucRet !=0)
			{
				return NO_DISP;
			}
			memcpy(stTemp.sCardDataList[i*4],sSectorData,48);
	//NacTxd(stTemp.sCardDataList[i*4],48);
		}
			
		iRet = M1_IsCardInfoLegal();//判断卡内数据的合法性
		if(iRet != 0)
		{
			return iRet;
		}
		memset(stTemp.sCardDataListSub, 0, sizeof(stTemp.sCardDataListSub));
		memcpy(stTemp.sCardDataListSub, stTemp.sCardDataList, 64*16);

		
		
	//读钱包，读BLOCK9,
	//memcpy(gstYktEnrichLog.uOldBalaceAmt,sCardDataList[2*4+1],4);
		gstYktEnrichLog.ulOldBalaceAmt=stTemp.sCardDataList[2*4+1][0]+stTemp.sCardDataList[2*4+1][1]*256
		+stTemp.sCardDataList[2*4+1][2]*256*256+stTemp.sCardDataList[2*4+1][3]*256*256*256;
		//	gstYktEnrichLog.ulMaxBalaceAmt=stTemp.sCardDataList[2*4][4]*256*256*256+stTemp.sCardDataList[2*4][5]*256*256
		//	+stTemp.sCardDataList[2*4][6]*256+stTemp.sCardDataList[2*4][7];
		//gstYktEnrichLog.ulMaxBalaceAmt=stTemp.sCardDataList[2*4][4]+stTemp.sCardDataList[2*4][5]*256
		//+stTemp.sCardDataList[2*4][6]*256*256+stTemp.sCardDataList[2*4][7]*256*256*256;
		// gstYktEnrichLog.ulMaxBalaceAmt=gstYktEnrichLog.ulMaxBalaceAmt*100;
		gstYktEnrichLog.ulMaxBalaceAmt= stPosParam.ulMaxBalaceAmt*100;
	
		if(gstYktEnrichLog.ulOldBalaceAmt<gstYktEnrichLog.ulTransAmt )
		{
			//-------------余额不足----
			lcdClrLine(2, 7);
			lcdDisplay(0, 4, DISP_CFONT, "余额不足");
			ErrorBeep();
			sysDelayMs(3000);
			return NO_DISP;

		}


	//判断，消费最大金额，消费上线
		if(gstYktEnrichLog.ulMaxBalaceAmt<gstYktEnrichLog.ulTransAmt)
		{
			//-------------已经超最大充值金额----
			lcdClrLine(2, 7);
			lcdDisplay(0, 4, DISP_CFONT, "超出消费限额");
			ErrorBeep();
			//lcdDisplay(0, 6, DISP_CFONT, "请重试!");
			sysDelayMs(3000);
			return NO_DISP;

		}
	//NacTxd("1007", 4);
		iRet=M1_SaleProcess();
		if(iRet!=0)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 4, DISP_CFONT, "扣款失败");
			ErrorBeep();
			sysDelayMs(3000);
			return NO_DISP;
		}

		//再次获取卡内所有数据验证是否扣款成功
		for(i=0;i<=8;i++)
		{
			memset(sSectorData,0,sizeof(sSectorData));//stTemp.sKeyBList
			ucRet =M1_ReadSector(i, &stTemp.sKeyBList[6*i],MIF_KEYA, sSectorData,stTemp.sCardUid);//返回48字节
			if(ucRet !=0)
			{
				return NO_DISP;
			}
			memcpy(stTemp.sCardDataList[i*4],sSectorData,48);
	//NacTxd(stTemp.sCardDataList[i*4],48);
		}
		reOldBalaceAmt=stTemp.sCardDataList[2*4+1][0]+stTemp.sCardDataList[2*4+1][1]*256
		+stTemp.sCardDataList[2*4+1][2]*256*256+stTemp.sCardDataList[2*4+1][3]*256*256*256;

		if(reOldBalaceAmt+gstYktEnrichLog.ulTransAmt != gstYktEnrichLog.ulOldBalaceAmt)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 4, DISP_CFONT, "扣款失败");
			ErrorBeep();
			sysDelayMs(3000);
			return NO_DISP;
		}
		
	
		gstYktEnrichLog.ulNowBalaceAmt=gstYktEnrichLog.ulOldBalaceAmt-gstYktEnrichLog.ulTransAmt;
		//----保存控制参数-------------------------
		gstYktEnrichLog.lEnrichTraceNo=stPosParam.lEnrichTraceNo;
		gstYktEnrichLog.lSaleCumulativeBalance=stPosParam.lSaleCumulativeBalance+gstYktEnrichLog.ulTransAmt;
		gstYktEnrichLog.uMode=1;//0充值，1消费

		//终端保存记录
		ucRet = SaveM1EnrichLog(0);
		if( ucRet!=OK )
		{
			lcdClrLine(2, 7);
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "保存文件操作失败", "FILE ERROR");
			ErrorBeep();
			kbGetKeyMs(2000);
		}
		stPosParam.lEnrichTraceNo++;
		stPosParam.lSaleCumulativeBalance=stPosParam.lSaleCumulativeBalance+gstYktEnrichLog.ulTransAmt;
		stPosParam.lUnSettTraceNum++;
		SaveAppParam();

		//打印
		PrtM1TranSaleTicket(NORMAL);
		
	}

//提示充值成功

//-----再读取数据。输出--------------------------------------------

  /*NacTxd("OK11",4);
 int i;
 for(i=0;i<=7;i++)
 	{
     ucRet =M1_ReadSector(i, &stTemp.sKeyBList[6*i],MIF_KEYB, sSectorData,stTemp.sCardUid);//返回48字节
	NacTxd("0007", 4);
    
	if(ucRet !=0)
	{
		return NO_DISP;
	}
	NacTxd(&i,4);
	NacTxd(sSectorData, 48);
 	}*/
//--------------------------------------------------
	M1_mifClose();
	PubBeepOk();
	lcdClrLine(2, 7);
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "请移走卡");
	
	//PubWaitKey(0);
//	showDebugMessage(iCardInfoLen, sCardInfo, "read card succ!");

	return 0;
}


//输入是16为的字符串，把
UINT8 GetBlockVerifybyte(UINT8 *sBlockData )
{
	 int i ;
	 UINT8 data;
	 data=sBlockData[0];
	 for(i=1;i<15;i++)
	   data=data^sBlockData[i];
	 sBlockData[15]=data;
	 return 0;
	 
}

/*UINT8  MakeWalletBlock(ulong  ulOldBalaceAmt,UINT8 *sBlockOut)
{
;
}*/
 UINT8 Check_valid_purse(UINT8 *sBlockdata)
{
int i;
  for(i=0;i<4;i++)
  {
  	if((sBlockdata[i]^sBlockdata[i+4])!=0xFF)
  	{
         return FALSE;
	}
  }
  for(i=4;i<8;i++)
  {
  	if((sBlockdata[i]^sBlockdata[i+4])!=0xFF)
  	{
         return FALSE;
	}
  }
  return TRUE;
}

UINT8 VeryfyWalletData(UINT8 *pdata)
{

   UINT8 u8Ret1,u8Ret2,IsEqualWallet;
 //  NacTxd(pdata,16);
 //  NacTxd(pdata+16,16);
  	u8Ret1=Check_valid_purse(pdata);//检验钱包是否有效
    u8Ret2=Check_valid_purse(pdata+16);//检验钱包是否有�
   if( memcmp(&pdata[0],&pdata[16+0],4)==0)
   {
       IsEqualWallet=TRUE;
   }
   else
   {
	   IsEqualWallet=FALSE;
   }
  // lcdCls();
  // lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY, "u8Ret1=%d",u8Ret1);
  // lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "u8Ret2=%d",u8Ret2);
  //  lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "IsEqualWallet=%d",IsEqualWallet);
	//kbGetKey();
   if(u8Ret1==TRUE && u8Ret2==TRUE &&IsEqualWallet== TRUE )
   {
      return 0; //不需要修改    1
   }
   else if(u8Ret1==TRUE && u8Ret2==TRUE &&IsEqualWallet== FALSE)
   {
       return 1;//以正本为准，更新数据正本到副本 4   
   }
   else if(u8Ret1==TRUE && u8Ret2==FALSE )
   {
    return 2;  //以正本为准，更新数据正本到副本 4    
   }
   else if(u8Ret1==FALSE && u8Ret2==TRUE )
   {
    return 3;//以副本为准，把副本的数据拷贝到正本是里面
   }
   else
   {
   
    return 4;              //正副本均无效，停止交易
   }


}




/*INT32 M1_Enrich_1(void)
{
	INT32     iCardInfoLen, iRet = 0;
	UINT8     sCardInfo[30];
	int       iCnt;
	UINT8     gl_CardUID[5];
	UINT8     uUIDLen;
	UINT8     sPubKey[6];
	UINT8     szBlockData[48+1];
	UINT8     ucRet;
	UINT8     szPasmId[6+1];


	iCnt = 10;
	PubDisplayTitle(TRUE, "充  值");
	ucRet = SocketSet(4, 0);
	if( ucRet!=0 )
	{
	  lcdClrLine(2, 7);
	  PubDispString("没有SAM卡", 3|DISP_LINE_CENTER);
	  PubBeepErr();
	  PubWaitKey(5);
	  return NO_DISP;	  
	}

	// SAM卡上电复位
	ucRet = SamReset(4, NULL);
	if( ucRet==1 )
	{
	  lcdClrLine(2, 7);
	  PubDispString("SAM上电复位失败", 3|DISP_LINE_CENTER);
	  PubBeepErr();
	  PubWaitKey(5);
	  return NO_DISP;	   
	}
	  
//NacTxd("0001",4);
	mifClose();
	mifOpen();
	lcdClrLine(2, 7);
	PubDispString("请刷卡", 3|DISP_LINE_CENTER);
	memset(sCardInfo, 0x00, sizeof(sCardInfo));
	ucRet =InListPassive(&iCardInfoLen, sCardInfo);
	if(ucRet !=0)
	{
		lcdClrLine(2, 7);
		PubDispString("读卡失败!", 3|DISP_LINE_CENTER);
		PubBeepErr();
		mifClose();
		PubWaitKey(5);
		return NO_DISP;
	}

//	NacTxd("0004",4);
	uUIDLen = sCardInfo[4];               // UID Len
	memset(gl_CardUID, 0, sizeof(gl_CardUID));
	memcpy(gl_CardUID, sCardInfo+5, uUIDLen);    // UID:CD 65 78 38 
//	NacTxd(gl_CardUID,4);
//计算1区的密钥
//CalPubKey;

	//
//	NacTxd("111111111111", 10);
	ucRet =GetPsamId();
//	NacTxd(szPasmId,6);
	if( ucRet!=OK )
	 {
		  return E_TRANS_FAIL;
	 }//szPasmId

//	NacTxd("2222222222222", 10);

//CheckData_Sectors1(szBlockData);  //checkout data 







//-----------------------
	M1_mifClose();

	PubBeepOk();
	lcdClrLine(2, 7);
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "请移走卡");
	PubWaitKey(0);
	showDebugMessage(iCardInfoLen, sCardInfo, "read card succ!");

	return 0;
}*/
UINT8 Verifybyte(const char *pdata )//传入16字节，前15字节异或，与第16字节比较是否相等
{
  int i =0;

  char data;
  data=pdata[0];
  for(i=1;i<15;i++)
  	data=data^pdata[i];
  if(data==pdata[15])
  	return TRUE;//FALSE
  else
	return FALSE;	

}

int VeryfyCard_Sectors1(const char *Sectorsdata)
{
UINT8 u8Ret;
//UINT8 sCardNo[100]; 
UINT8 sDateTime[32];
//---每一Block的数据的校验----
u8Ret=Verifybyte(Sectorsdata);
if(u8Ret!=TRUE)
{//段校验错
	lcdClrLine(2, 7);
	lcdDisplay(0, 4, DISP_CFONT, "数据块校验错误!");
	lcdDisplay(0, 6, DISP_CFONT, "数据块4!");
	sysDelayMs(3000);
	return NO_DISP;
}
u8Ret=Verifybyte(Sectorsdata +16);
if(u8Ret!=TRUE)
{//段校验错
	lcdClrLine(2, 7);
	lcdDisplay(0, 4, DISP_CFONT, "数据块校验错误!");
	lcdDisplay(0, 6, DISP_CFONT, "数据块5!");
	sysDelayMs(3000);
	return NO_DISP;
}
u8Ret=Verifybyte(Sectorsdata+32);
if(u8Ret!=TRUE)
{//段校验错
	lcdClrLine(2, 7);
	lcdDisplay(0, 4, DISP_CFONT, "数据块校验错误!");
	lcdDisplay(0, 6, DISP_CFONT, "数据块6!");
	sysDelayMs(3000);
	return NO_DISP;

}
//---卡号有效性的验证-------
/*memset(sCardNo,0,sizeof(sCardNo));
memcpy(sCardNo,Sectorsdata,2);//发卡城市代码
memcpy(&sCardNo[2],Sectorsdata+3,1);//应用代码
memcpy(&sCardNo[3],Sectorsdata+4,4);//发行流水号
memcpy(&sCardNo[7],Sectorsdata+8,4);
u8Ret=VerifyCardNo();*/
//----启用标志----------------------
u8Ret=VerifyStartUse(Sectorsdata[12]);
if(u8Ret!=TRUE)
{
	return NO_DISP;
}
//----日期认证---------------------
//GetDays(uint8_t * szDate)
memset(sDateTime,0,sizeof(sDateTime));
/*sDateTime[0]=0x20;
sysGetTime(&sDateTime[1]);
if(memcmp(sDateTime,&Sectorsdata[16+4],4)>0)
{
	lcdClrLine(2, 7);
	lcdDisplay(0, 4, DISP_CFONT, "有效日期过期");
	sysDelayMs(3000);
	return NO_DISP;
}*/
return 0;



}
UINT8 VerifyStartUse(char flag)
{
    
	if(flag==0x01)
	{
			lcdClrLine(2, 7);
			lcdDisplay(0, 4, DISP_CFONT, "卡未启用");
			ErrorBeep();
			sysDelayMs(3000);
			return FALSE;//TRUE

	}
	else if(flag==0x02)
	{
           return TRUE;
	}
	else if(flag==0x03)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "卡已停用");
		ErrorBeep();
		sysDelayMs(3000);
		return FALSE;

	}
	else if(flag==0x04 )
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "黑名单卡");
		ErrorBeep();
		sysDelayMs(3000);
		return FALSE;

	}
	else
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "卡不可用");
		ErrorBeep();
		sysDelayMs(3000);
		return FALSE;

	}

}

/*UINT8 VerifyCardNo(const char *pCardNo)
{
    ;//----------
}*/
//这个函数校验段2的数据
//Sectorsdata:段数据
//IsUpdateData:是否更新数据，1更新，0更新，
//如果跟新，Sectorsdata返回跟新后的数据
int CheckData_Sectors2(char *Sectorsdata,UINT8 IsUpdateData,UINT8 * Uid,char * cKeyB)  //校验完后，可能会修改数据
{
	UINT8 u8Ret;//,u8Ret1,u8Ret2
	int iRet;
	
	u8Ret=Verifybyte(Sectorsdata);
	if(u8Ret!=TRUE)
	{//段校验错
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "数据块校验错误!");
		lcdDisplay(0, 6, DISP_CFONT, "数据块8!");
		sysDelayMs(3000);
		return NO_DISP;
		//return ERROR;//OK;
	}
	u8Ret=VeryfyWalletData(Sectorsdata+16);
	if(IsUpdateData==1)
	{
		if(u8Ret==0)
		{
		   return 0;  //u8Ret==0
		}
		else if(u8Ret==1 )//以正本为准，更新数据正本到副本 4   
		{
		   iRet = M1_BlocktoBlockValue(10, 9, cKeyB, MIF_KEYB,Uid);
		 // NacTxd("11111",5);
		   //u8Ret= M1_WriteBlock(2*4+2,cKeyB,MIF_KEYB,Sectorsdata+16,Uid);
		   if(iRet==MIF_RET_SUCCESS)
		   {
		     //  memcpy(Sectorsdata+32,Sectorsdata+16,16);
			   return 0;
			
		   }
		   else
		   {
		       return NO_DISP; 
		   }
		}
		else if(u8Ret==2)
		{
			//u8Ret= M1_WriteBlock(2*4+2,cKeyB,MIF_KEYB,Sectorsdata+16,Uid);
		//	NacTxd("22222",5);
			 iRet = M1_BlocktoBlockValue(10, 9, cKeyB, MIF_KEYB,Uid);
			if(iRet==MIF_RET_SUCCESS)
			{
				//memcpy(Sectorsdata+32,Sectorsdata+16,16);
				return 0;

			}
			else
			{
				lcdClrLine(2, 7);
				lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "钱包恢复失败");
				PubWaitKey(3);
				return NO_DISP; 
			}

		}
		else if(u8Ret==3)
		{
		 //  NacTxd("33333",5);
			iRet = M1_BlocktoBlockValue(9, 10, cKeyB, MIF_KEYB,Uid);
			if(iRet==MIF_RET_SUCCESS)
			{
				//memcpy(Sectorsdata+16,Sectorsdata+32,16);
				return 0;
			 
			}
			else
			{
				lcdClrLine(2, 7);
				lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "钱包恢复失败");
				PubWaitKey(3);
				return NO_DISP; 
			}
		}
		else //==4
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 4, DISP_CFONT, "钱包数据无效");
			//lcdDisplay(0, 6, DISP_CFONT, "请重试!");
			sysDelayMs(3000);
			return NO_DISP;

		}
	}
	else
	{
		if(u8Ret==0)
		{
			return 0;
		}
		else
		{
		    lcdClrLine(2, 7);
			lcdDisplay(0, 4, DISP_CFONT, "钱包数据错误");
			sysDelayMs(3000);
			return NO_DISP;
		}

	}
	//钱包校验
	
	//AdjustPurse(Sectorsdata+16);
	//u8Ret1=Check_valid_purse(Sectorsdata+16);//检验钱包是否有效

	//u8Ret2=Check_valid_purse(Sectorsdata+16);//检验钱包是否有效
   //if()
	//备份钱包校验

	
/*电子钱包包括钱包正本、钱包副本和钱包校验码
等信息，钱包正本和钱包副本为Hex码，逆序，被分
别存贮在同一扇区的1块和2块内。卡在每次充值或
消费交易时，对钱包的正本与副本作有效性及是否
相符的比较，分五种情况：
１．正本副本有效且一致，以正本或副本为准；
２．正本有效，副本无效，以正本为准；
３．副本有效，正本无效，以副本为准；
４．正本副本有效但不一致，以正本为准；
5．正本副本均无效，钱包无效，充值或消费交易终止。
以上２／３／４的情况下，有一种数据无效或两者数据
不一致，应将正确的数据拷贝到不正确的数据块。*/
}

	//----------------------------------------------
	/*int ChkContactlessCardPad(void)
{
	int       iRet;
	MIF_TIME_STRUCT stMifTimeInfo;

	iRet=0;
	mifOpen();
	mifGetTimeOut(&stMifTimeInfo);
	stMifTimeInfo.WaitTime_PollTypeB *= 5;
	stMifTimeInfo.WaitTime_PollTypeA *= 5;
	stMifTimeInfo.WaitTime_RATS *= 20;
	mifSetTimeOut(&stMifTimeInfo);
	if (1)//非接触式卡
	{
		while(1)
		{
			iRet = mifCheckOnlyOneCard(0x07);
			if (iRet == MIF_RET_MULTICARD)
			{
				sysBeep();
				lcdClrLine(2,7);
				lcdPrintf("有 多 张 卡");
				sysDelayMs(3000);
				mifResetRF(0);
				return ERR_NO_DISP;
			} 
			else if ((MIF_RET_SUCCESS!=iRet) && (MIF_RET_NOCARD!=iRet) && (MIF_RET_TIMEOUT!=iRet))
			{
				sysBeep();
				lcdClrLine(2,7);
				lcdPrintf("读 卡 错 误:%d", iRet);
				sysDelayMs(3000);
				mifResetRF(0);
				return ERR_NO_DISP;
			}
			else if(iRet == MIF_RET_SUCCESS)
			{
				// 在感应区内查找指定调制类型的卡片，并返回卡片的序列号等相关信息
				iRet = InListPassiveTarget(0, 0, NULL, &glstMsgInfo.stMagInfo.iRetCode, (UINT8*)glstMsgInfo.stMagInfo.szTrack1);
				//glstMsgInfo.stMagInfo.iRetCode = glstMsgInfo.stMagInfo.szTrack1[4];
				//memcpy(glstMsgInfo.stMagInfo.szTrack1,&glstMsgInfo.stMagInfo.szTrack1[5],glstMsgInfo.stMagInfo.iRetCode);
				//glstMsgInfo.stMagInfo.szTrack1[glstMsgInfo.stMagInfo.iRetCode] = 0;
				if (iRet == MIF_RET_SUCCESS)
				{
					//PubBeepOk();
					return MIF_RET_SUCCESS;
				}
			}
			else if(iRet == -5){}
			else
			{
				lcdPrintf("读    卡:%d", iRet);
				sysDelayMs(3000);
				break;
			}
			if(PubChkKeyInput())
			{
				iRet = kbGetKey();
				if(iRet == KEY_CANCEL)
				{
					return 0;
				}
			}

		}
		mifResetRF(0);
	}

	return iRet;
}*/

//sOnlyNo流水号
//	int M1_GetCalKeyA(UINT8* pCsn,UINT8* pCityCode,UINT8* pTrace,UINT8* PMac,UINT8* PSectorOper,int StartSection,UINT8* PAllKey)

UINT8 GetSecPW(UINT8*  pCsn, UINT8*  pTrace, UINT8* sAuthKey, UINT8* pCityCode, UINT8* PSectorOper,UINT8* sSecNo, int iKeyNo, UINT8* SecPW)
{
	int    iRet, j;//iCnt,
	APDU_SEND stApduSend;
	APDU_RESP stApduResp;
	//UINT8 sAuthCode[8];
//NacTxd("9008",8);
//NacTxd(sAuthKey,8);
//NacTxd(&sAuthKey,8);

/*//---------//认证MAC------------------------
    memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS =0x88;
	stApduSend.P1 = 0x00;
	stApduSend.P2 = 0x70;//70 ;//&(UINT8)(iKeyNo-1);
	stApduSend.Lc = 0x08;
	stApduSend.Le = 0x00;
	stApduSend.LeFlag = 0x01;
	memcpy(&stApduSend.DataIn[0],pCsn,4);
	memcpy(&stApduSend.DataIn[4],&pTrace[2],2);
	memcpy(&stApduSend.DataIn[6],&pCityCode[0],2);
	//NacTxd(&stApduSend,20);
	iRet = iccExchangeAPDU(ICC_SAM1,&stApduSend,&stApduResp);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{

		lcdClrLine(2, 7);
		PubDispString("认证MAC错误", 3|DISP_LINE_CENTER);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP;
	}
	memcpy(sAuthCode,stApduResp.DataOut,4);*/

	//---------选择文件------------------------
		memset((char*)&stApduSend,0,sizeof(stApduSend));
		memset((char*)&stApduResp,0,sizeof(stApduResp));
		stApduSend.CLA =0x00;
		stApduSend.INS =0xA4;
		stApduSend.P1 = 0x00;
		stApduSend.P2 = 0x00 ;//&(UINT8)(iKeyNo-1);
		stApduSend.Lc = 0x02;
		stApduSend.Le = 0x00;
		stApduSend.LeFlag = 0x01;
		memcpy(&stApduSend.DataIn[0],"\x10\x01",2);
		//memcpy(&stApduSend,"\x00\xA4\x00\x00\x02\x10\x01",7);

		iRet = iccExchangeAPDU(ICC_SAM1,&stApduSend,&stApduResp);
		
//NacTxd("1111",4);
//NacTxd(&stApduSend,20);

		if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
		{
	
			lcdClrLine(2, 7);
			PubDispString("选择文件错误", 3|DISP_LINE_CENTER);
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP;
		}
		//memcpy(sAuthCode,stApduResp.DataOut,4);

	
	//NacTxd(stApduResp.DataOut,stApduResp.LenOut);
	//if(memcmp(sAuthCode,sAuthKey,4)==0)
	//{
		for (j = 0; j <16; j++)
		{
			if (sSecNo[j]=='1')
			{
			
				memset((char*)&stApduSend,0,sizeof(stApduSend));
				memset((char*)&stApduResp,0,sizeof(stApduResp));
				stApduSend.CLA =0x80;
				stApduSend.INS =0xFC;
				stApduSend.P1 = 0x01;
				stApduSend.P2 = 0x01 ;//&(UINT8)(iKeyNo-1);
				stApduSend.Lc = 0x0D;
				
				stApduSend.Le = 0x00;
				stApduSend.LeFlag = 0x01;
				
				memcpy(&stApduSend.DataIn[0],&pCityCode[0],2);
				memcpy(&stApduSend.DataIn[2],pCsn,4);
				memcpy(&stApduSend.DataIn[6],&pTrace[2],2);
		//NacTxd(&sAuthKey,16);
				memcpy(&stApduSend.DataIn[8],sAuthKey,4);
				//stApduSend.DataIn[6]=sAuthKey[0];
				memcpy(&stApduSend.DataIn[12],PSectorOper+j,1);
				
				iRet = iccExchangeAPDU(ICC_SAM1,&stApduSend,&stApduResp);
		//NacTxd("2222",4);
		//NacTxd(&stApduSend,20);
		//NacTxd(&sAuthKey,20);


				if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
				{

					lcdClrLine(2, 7);
					PubDispString("认证MAC错误", 3|DISP_LINE_CENTER);
					lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
					PubBeepErr();
					PubWaitKey(3);
					return NO_DISP;
				}
				memcpy(&SecPW[j*6],stApduResp.DataOut,6);
					//	NacTxd(stApduResp.DataOut,stApduResp.LenOut);
						//SecPW[j] = sRetStr.Substring(0, 12);
			}
		}


	//}
	/*else
	{
		lcdClrLine(2, 7);
		PubDispString("认证MAC错误", 3|DISP_LINE_CENTER);
	//	lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP;


	}*/
return 0;
//---------------------------------
	
}
	


int M1_GetCalKeyA(UINT8* pCsn,UINT8* pTrace,UINT8* PMac,UINT8* pCityCode,UINT8* PSectorOper,UINT8* sSecNo,UINT8* PAllKey)
{
	int    iRet;//, iCnt
	APDU_SEND stApduSend;
	APDU_RESP stApduResp;
	//int SectionTemp,CountTemp,i;
	int j;

	//----------进入1003目录--"\x00\xA4\x00\x00\x02\x10\x03\x00\x01"-------------------
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS =0xA4;
	stApduSend.P1 = 0x00;
	stApduSend.P2 = 0x00;
	stApduSend.Lc = 0x02;
	stApduSend.Le = 0x00;
	stApduSend.LeFlag = 0x01;
	memcpy(stApduSend.DataIn,"\x10\x01",2);
//NacTxd(&stApduSend,20);
	iRet = iccExchangeAPDU(ICC_SAM2,&stApduSend,&stApduResp);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{
		lcdClrLine(2, 7);
		PubDispString("进入1003目录错误", 3|DISP_LINE_CENTER);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP;
	}
//NacTxd("1000",4);
//NacTxd(&stApduResp.DataOut,30);

	for (j = 2; j <16; j++)
		{
			if (sSecNo[j]=='1')
			{
			
				memset((char*)&stApduSend,0,sizeof(stApduSend));
				memset((char*)&stApduResp,0,sizeof(stApduResp));
				stApduSend.CLA =0x80;
				stApduSend.INS =0xFC;
				stApduSend.P1 = 0x01;
				stApduSend.P2 = 0x01 ;//&(UINT8)(iKeyNo-1);
				stApduSend.Lc = 0x0D;
				
				stApduSend.Le = 0x00;
				stApduSend.LeFlag = 0x01;
				
				memcpy(&stApduSend.DataIn[0],&pCityCode[0],2);
				memcpy(&stApduSend.DataIn[2],pCsn,4);
				memcpy(&stApduSend.DataIn[6],&pTrace[2],2);
				memcpy(&stApduSend.DataIn[8],PMac,4);
				//stApduSend.DataIn[6]=sAuthKey[0];
				memcpy(&stApduSend.DataIn[12],PSectorOper+j,1);
				
				iRet = iccExchangeAPDU(ICC_SAM2,&stApduSend,&stApduResp);
//NacTxd("2222",4);
//NacTxd(&stApduSend,20);

				if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
				{

					lcdClrLine(2, 7);
					PubDispString("认证MAC错误", 3|DISP_LINE_CENTER);
					lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
					PubBeepErr();
					PubWaitKey(3);
					return NO_DISP;
				}
				memcpy(&PAllKey[j*6],stApduResp.DataOut,6);
					//	NacTxd(stApduResp.DataOut,stApduResp.LenOut);
						//SecPW[j] = sRetStr.Substring(0, 12);
			}
		}
	
	return 0;

	//-------------------------


	/*//----------------------------------
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS = 0xB0;
	stApduSend.P1 = 0x96;
	stApduSend.P2 = 0x00;
	stApduSend.Lc = 0;
	stApduSend.Le = 6;
	stApduSend.LeFlag = 1;
	iRet = iccExchangeAPDU(ICC_USERCARD,&stApduSend,&stApduResp);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{

	lcdClrLine(2, 7);
	PubDispString("读psam卡号错!", 3|DISP_LINE_CENTER);
	lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
	PubBeepErr();
	PubWaitKey(3);
	return NO_DISP;
	}
	memcpy(pzPasmId, stApduResp.DataOut, 6);
	NacTxd(stApduResp.DataOut, stApduResp.LenOut);
*/
	//iRet = M1_GetCalKeyA(sCsn,sCityCode, sTrace, sMac, sSectorOper, sAllKey);

	/*	UINT32 Section;
		char   sBuf[2]; 
		int iRet = 0;
		
		memset(gstTransInfo.sKeyAList, 0, 16*6);
		memset(gstTransInfo.sCardDataList, 0, 64*16);
		
		iRet = ChkContactlessCardPad();
		if(iRet != MIF_RET_SUCCESS)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "获取卡失败");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "请重新刷卡");
			return iRet;
	
		}
		memset(gstTransInfo.sCardUid, 0, sizeof(gstTransInfo.sCardUid));
		memcpy((char*)gstTransInfo.sCardUid,(char*)&glstMsgInfo.stMagInfo.szTrack1[5],4);
		//获取0扇区的数据
		Section = 0;
		memcpy(gstTransInfo.sKeyAList[Section], KEYA_SECTOR0, 6);
		iRet = M1_ReadCardSectionData(Section, gstTransInfo.sKeyAList[Section], MIF_TYPE_A);
		if(iRet != MIF_RET_SUCCESS)
		{
			return iRet;
		}
		memcpy(gstTransInfo.sCardDataList[1], S0B1_DATA, 16);
		memcpy(gstTransInfo.stTranLog.ucCardCSN,gstTransInfo.sCardDataList[0],4);
		//Uart_RawTxd(gstTransInfo.sKeyAList[Section], 6, -1);
		
		//获取1扇区的数据
		Section = 1;
		memcpy(gstTransInfo.sKeyAList[Section], gstTransInfo.stTranLog.ucCardCSN, 4);
		memcpy(sBuf, gstTransInfo.stTranLog.ucCardCSN, 2);
		PubXor(sBuf, "\xFF\xFF", 2);		//CSN取反前两个字节加在CSN的后面
		memcpy(&gstTransInfo.sKeyAList[Section][4], sBuf, 2);
		iRet = M1_ReadCardSectionData(Section, gstTransInfo.sKeyAList[Section], MIF_TYPE_A);
		//Uart_RawTxd(gstTransInfo.sKeyAList[Section], 6, -1);
		if(iRet != MIF_RET_SUCCESS)
		{
			return iRet;
	
		}
		//获取2-16扇区的KeyA
		iRet = M1_GetAuthKeyAFormPSAM(M1_PsamSlot, 2);
		if(iRet != 0)
		{
			return iRet;
	
		}*/
	
}

	int  SocketSet(int socket, int rate_idx)
{
	INT32 iRet = ICC_SUCCESS;
	INT32 CardSlot, iCardType;
	
	if( socket==0 )
		CardSlot = ICC_SAM1;
	else if( socket==1 )
		CardSlot = ICC_SAM2;
	else if( socket==2 )
		CardSlot = ICC_SAM3; 
	else if( socket==3 )
		CardSlot = ICC_SAM4;
	else
		CardSlot = ICC_USERCARD;
	
	switch (rate_idx)
	{
	case 0:
		iCardType = ICC_ISO;
		break;
	case 1:
		iCardType = ICC_19200;
		break;
	case 2:
		iCardType = ICC_38400;
		break;
	default:
		iCardType = ICC_ISO;
		break;
	}

	iRet = iccSetPara(CardSlot, ICC_VOL_5V, iCardType); 
	if( iRet )
	{
		return 1;
	}
	
	iRet = iccDetect(CardSlot);
	if( iRet!=ICC_SUCCESS )
	{
		return 2;		
	}
	return 0;
}
	//功能:复位psam卡
	//输入:卡槽号,初始信息
	//失败返回 1 成功返回 对应卡槽标准编号
	int  SamReset(int socket, UINT8 *buf)
	{
		INT32 iRet = 0;
		UINT32 AtrLen = 0;
		INT32 CardSlot;
		UINT8 AtrData[256];

		if( socket==0 )
			CardSlot = ICC_SAM1;
		else if( socket==1 )
			CardSlot = ICC_SAM2;
		else if( socket==2 )
			CardSlot = ICC_SAM3; 
		else if( socket==3 )
			CardSlot = ICC_SAM4;
		else
			CardSlot = ICC_USERCARD;
		iccPowerDown(CardSlot);
		if( buf==NULL )
		{
			memset(AtrData, 0x00, sizeof(AtrData));
			iRet = iccPowerUp(CardSlot, &AtrLen, AtrData);
			sysDelayMs(100);
			//NacTxd(AtrData, AtrLen);
		}
		else
		{
			iRet = iccPowerUp(CardSlot, &AtrLen, buf);
			sysDelayMs(11);
			//NacTxd(buf, AtrLen);
		}
		if( iRet!=ICC_SUCCESS )
		{
			
			iccPowerDown(CardSlot);
			return 1;
		}
	
		return CardSlot;
	}
	//复位psam卡指令
	UINT8 ResetPsam(void)
	{
		int    iRet;
		
		iRet = SocketSet(0, 0);
		if( iRet!=0 )
		{
			//lcdClrLine(2, 7);
			//PubDispString("没有SAM卡", 3|DISP_LINE_CENTER);
			//PubBeepErr();
			//PubWaitKey(5);
			return NO_DISP; 	
		}
	
		// SAM卡上电复位
		iRet = SamReset(0, NULL);
		if( iRet==1 )
		{
			//lcdClrLine(2, 7);
			//PubDispString("SAM上电复位失败", 3|DISP_LINE_CENTER);
			//PubBeepErr();
			//PubWaitKey(5);
			return NO_DISP; 	 
		}
		return 0;
	}

	
	//复位住建部psam卡指令
	UINT8 ResetPsamZJB(void)
	{
		int    iRet;
		UINT32 AtrLen = 0;
		INT32 CardSlot;
		UINT8 AtrData[256];
		
		iRet = SocketSet(1, 2);
		if( iRet!=0 )
		{
			return NO_DISP; 	
		}
		// SAM卡上电复位
		iRet = SamReset(1, NULL);
		if( iRet==1 )
		{
			return NO_DISP; 	 
		}
		return 0;
	}

/*int IcCardRePower(int socket)
{
	int		iRet;
		
	// 相关信息送SAM卡计算密钥
	if(socket == 0)
		iRet = SocketSet(socket, 2);
	else
		iRet = SocketSet(socket, 0);
	if( iRet!=0 )
	{
		lcdClrLine(2, 7);
		PubDispString("没有SAM卡", 3|DISP_LINE_CENTER);
		mifClose();
		PubWaitKey(5);
		return ERR_SAM_POWERUP; 	
	}
	// SAM卡上电复位
	iRet = SamReset(socket, NULL);
	if( iRet==1 )
	{
		lcdClrLine(2, 7);
		PubDispString("SAM上电复位失败", 3|DISP_LINE_CENTER);
		mifClose();
  	       iRet = iccPowerDown(socket+0x11);
		PubWaitKey(5);
		return ERR_SAM_POWERUP;		
	}
	return 0;
}*/
///////////////读卡指令区///////////////////////
//调用该函数前要先对psam卡上电复位
//读取会多卡psam卡终端机具号
//

//寻卡
UINT8 InListPassive(INT32 *iCardInfoLen, UINT8 *sCardInfo)
{
    int         iRet;
//	INT32       CardInfoLen =0;
//	char        CardInfo[256];
    char        szBuffer[50];
//    UINT8       uType;
    
//	int    status = 0;
//	int    lasterror;
 //   int    iCnt;

    memset(szBuffer, 0, sizeof(szBuffer));
 //   memset(CardInfo, 0, sizeof(CardInfo));
 //   uType =0;
	mifClose();
    mifOpen();
    while(1)
    {
 //       memset(CardInfo, 0x00, sizeof(CardInfo));
        iRet = InListPassiveTarget(0,0,NULL,iCardInfoLen, sCardInfo);
        if( iRet==MIF_RET_SUCCESS )
        {
	        break;
        }
         
        if( kbhit()==YES )
        {
            iRet = kbGetKey();
            if( iRet==KEY_CANCEL )
            {
                return NO_DISP;
            }
        }
    }
    return 0;
}
int GetPsamId(UINT8 *pzPasmId)//10字节
{
    int    iRet;//, iCnt
    APDU_SEND stApduSend;
    APDU_RESP stApduResp;

  
//NacTxd("8888",4);

    memset((char*)&stApduSend,0,sizeof(stApduSend));
    memset((char*)&stApduResp,0,sizeof(stApduResp));
    stApduSend.CLA = 0;
    stApduSend.INS = 0xB0;
    stApduSend.P1 = 0x82;
    stApduSend.P2 = 0x00;
    stApduSend.Lc = 0;
    stApduSend.Le = 0x0C;
    stApduSend.LeFlag = 1;
    iRet = iccExchangeAPDU( ISAM_SLOT,&stApduSend,&stApduResp);
    if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
    {

		lcdClrLine(2, 7);
		PubDispString("读psam卡号错!", 3|DISP_LINE_CENTER);
        lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP;
    }
    memcpy(pzPasmId, stApduResp.DataOut, 10);
  //  NacTxd(stApduResp.DataOut, stApduResp.LenOut);
    return 0;
}
int GetTellNo(UINT8 *pzTellNo)//10字节
{
    int    iRet;//, iCnt
    APDU_SEND stApduSend;
    APDU_RESP stApduResp;

//NacTxd("8888",4);

    memset((char*)&stApduSend,0,sizeof(stApduSend));
    memset((char*)&stApduResp,0,sizeof(stApduResp));
    stApduSend.CLA = 0;
    stApduSend.INS = 0xB0;
    stApduSend.P1 = 0x82;
    stApduSend.P2 = 0x00;
    stApduSend.Lc = 0;
    stApduSend.Le = 0x0C;
    stApduSend.LeFlag = 1;
    iRet = iccExchangeAPDU( ICC_USERCARD,&stApduSend,&stApduResp);
    if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
    {

		lcdClrLine(2, 7);
		PubDispString("读操作员号错!", 3|DISP_LINE_CENTER);
        lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP;
    }
   // memcpy(pzTellNo, stApduResp.DataOut+6, 4);
   PubAsc2Bcd(stApduResp.DataOut+2,8,pzTellNo);
  //  NacTxd(stApduResp.DataOut, stApduResp.LenOut);
   // NacTxd(pzTellNo, 4);
    return 0;
}
int M1_IncValue(UINT32 BlockNo, char* cKey, int iKeyType, char* WriteData,UINT8 *Uid)
{
	int       iRet = 0;

	
		iRet = ProcContEventWrite(BlockNo, cKey, iKeyType,WriteData,  M1_INCREMENT, Uid);//卡数据处理
		if(iRet != MIF_RET_SUCCESS)
		{
			return -1;
		}
	
	
	return iRet;
}
int M1_DecValue(UINT32 BlockNo, char* cKey, int iKeyType, char* WriteData,UINT8 *Uid)
{
	int       iRet = 0;

	
		iRet = ProcContEventWrite(BlockNo, cKey, iKeyType,WriteData,  M1_DECREMENT, Uid);//卡数据处理
		if(iRet != MIF_RET_SUCCESS)
		{
			return -1;
		}
	
	
	return iRet;
}

/****************************************************************************
 功    能:      整型数转换成低位在前的字符串表示(按照网络字节格式存放)
****************************************************************************/
void PubLong2CharEx(ulong ulSource, int iAsclen, char *psOAsc)
{
	int   iCnt;
	char  szBuffer[100];
	
	memset(szBuffer,0,sizeof(szBuffer));
	PubLong2Char(ulSource,iAsclen,szBuffer);

	for (iCnt=0; iCnt< iAsclen; iCnt++)
	{
		psOAsc[iCnt] = szBuffer[iAsclen-iCnt-1];
	}
}
int   M1_BlocktoBlockValue(int DesBlock, int SourceBlock, char* cKey, int iKeyType,UINT8 *UID)
{
	int iRet = 0;

	iRet = mifAuthByKey(SourceBlock, iKeyType, cKey, UID);
	if (iRet != MIF_RET_SUCCESS)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "验证%d块数据错误", SourceBlock);
		sysDelayMs(3000);
		return NO_DISP;
	} 
	iRet = mifRestore(SourceBlock);
	if (iRet != MIF_RET_SUCCESS)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "保存副本失败1");
		sysDelayMs(3000);
		return NO_DISP;
	}
	iRet = mifTransfer(DesBlock);
	if (iRet != MIF_RET_SUCCESS)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "保存副本失败2");
		sysDelayMs(3000);
		return NO_DISP;
	}

	return iRet;
}
int GetAllDataFromM1Card(void)
{
	//UINT32 BlockNo;
	//int iRet = 0; 
	//int i;
	UINT8 ucRet;
	UINT8 sSectorData[50];
	//--------------------
	//for(i=0;i<=2;i++)
	//{
		memset(sSectorData,0,sizeof(sSectorData));//stTemp.sKeyBList
		ucRet =M1_ReadSector(2, &stTemp.sKeyBList[6*2],MIF_KEYB, sSectorData,stTemp.sCardUid);//返回48字节
		//NacTxd(&i, 4);
		if(ucRet !=0)
		{
			return NO_DISP;
		}
		memcpy(stTemp.sCardDataList[2*4],sSectorData,48);
	  //  NacTxd(stTemp.sCardDataList[i*4],48);
	//}

	//-----------------------
	
	return MIF_RET_SUCCESS;
}
int M1_IsCardInfoLegal(void)
{
	int iRet = 0;
//	INT32 ulOrgAmt = 0;
//	char szDateTime[6+1], szOldTime[6+1];
//	ulong ulDevDays = 0;
	iRet = VeryfyCard_Sectors1(stTemp.sCardDataList[1*4]);//验证卡片的合法性，标志，
	if(iRet !=0)
	{
		return NO_DISP;
	}
	/*if(stTemp.sCardDataList[6*4][6]==0x04)
	{//---黑名单卡
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "黑名单卡");
		ErrorBeep();
		sysDelayMs(3000);
		return NO_DISP;
	}*/
	iRet=CheckData_Sectors2(stTemp.sCardDataList[2*4],1,stTemp.sCardUid,&stTemp.sKeyBList[2*6]);
	if(iRet !=0)
	{
		return NO_DISP;
	}
	return 0;

}
//

int  M1_SaleProcess(void)//UINT32 uiTransAmt
{
	int		  iRet = 0;
	//UINT8     	  ucLrc = 0;
//	ulong	  ulCount;
	char          szAmount[4];
	///char		  szDateTime[7], szValidityTime[4];
//	char		  sTermID[6], szTermID[12+1];
	//ulong	  iYear;
	UINT8 sTimeStr[7];
	//UINT8 u8Ret;
	//int itemp;
	//UINT8 sBlockData[16];
	//ulong ulBalaceAmt;
//	INT32	  iAfterRchAmt;
		
	sysGetTime(sTimeStr);
    gstYktEnrichLog.sTranDateTime[0]=0x20;
    memcpy(&gstYktEnrichLog.sTranDateTime[1],sTimeStr,6);
	iRet = M1_SWIPERET_1;
	while(1)
	{
		switch(iRet)
		{
			case 0:
				//NacTxd("6000",4);
				return 0;
				break;
			case M1_SWIPERET_1:
				/*NacTxd("6001",4);
				//写卡第一步
				// 更新S6B0，黑名单标志=0x04(公共信息区置黑)
				stTemp.sCardDataList[24][6] = 0x04;
				//gstTransInfo.sCardDataList[24][0] = ucLrc;
				ucLrc = PubCalcLRC((char*)stTemp.sCardDataList[24],15,0);
				stTemp.sCardDataList[24][15] = ucLrc;
				u8Ret= M1_WriteBlock(6*4,&stTemp.sKeyBList[6*6],MIF_KEYB,stTemp.sCardDataList[24],stTemp.sCardUid);//副本公  共信息区
				if(u8Ret!=OK)
				{
				    ErrorBeep();
					iRet = M1_SWIPE_AGAIN;
					break;
				}*/
				iRet = M1_SWIPERET_2;
				break;
			case M1_SWIPERET_2:
				//写卡第二步
//NacTxd("6002",4);
				//PubLong2CharEx(gstYktEnrichLog.ulTransAmt, 4, szAmount);
				PubLong2Char(gstYktEnrichLog.ulTransAmt, 4, szAmount);
//NacTxd(&gstYktEnrichLog.ulTransAmt,10);
//NacTxd(szAmount,10);
				iRet = M1_DecValue(4*2+2, &stTemp.sKeyBList[6*2], MIF_KEYA, (UINT8*)szAmount,stTemp.sCardUid);
				if(iRet!=OK)
				{
					//lcdClrLine(2, 7);
					//lcdDisplay(0, 4, DISP_CFONT, "写钱包副本失败");
					//lcdDisplay(0, 6, DISP_CFONT, "加值副本");
					ErrorBeep();
					iRet = M1_SWIPE_AGAIN;
					break;

				}
				iRet = M1_SWIPERET_3;
				break;
			case M1_SWIPERET_3:
				//写成第三步
				// 写钱包正本
//NacTxd("6003",4);
				iRet = M1_BlocktoBlockValue(9, 10, &stTemp.sKeyBList[6*2], MIF_KEYA,stTemp.sCardUid);//把十传给九
				if (iRet != 0)
				{
					//重刷卡流程
					ErrorBeep();
					iRet = M1_SWIPE_AGAIN;
					break;
				}
				iRet = M1_SWIPERET_4;
				break;
			case M1_SWIPERET_4:
				/*//NacTxd("6004",4);
				//-----------------
				gstYktEnrichLog.ulPOSTraceNo=stTemp.sCardDataList[1*4+1][12]*256+stTemp.sCardDataList[1*4+1][13];
				//memcpy(sCardDataList[1*4],sBlockData,16);
				memcpy(sBlockData,stTemp.sCardDataList[1*4+1],16);
				PubLong2Char(gstYktEnrichLog.ulPOSTraceNo+1,2,&sBlockData[12]);//计数器加一
				GetBlockVerifybyte(sBlockData);
				u8Ret= M1_WriteBlock(1*4+1,&stTemp.sKeyBList[6*1],MIF_KEYA,sBlockData,stTemp.sCardUid);
				if(u8Ret!=OK)
				{
					//lcdClrLine(2, 7);
					//lcdDisplay(0, 4, DISP_CFONT, "写卡失败");
				//	lcdDisplay(0, 6, DISP_CFONT, "更新计数器");
					ErrorBeep();
				//	sysDelayMs(3000);
					iRet = M1_SWIPE_AGAIN;
					break;
				}*/
				iRet = M1_SWIPERET_5;
				break;
			case M1_SWIPERET_5:
				/*//NacTxd("6005",4);
				//写卡第四步
				memset(sBlockData,0,sizeof(sBlockData));
				memcpy(sBlockData,&gstYktEnrichLog.sTranDateTime[1],5);
				PubLong2Char(gstYktEnrichLog.ulOldBalaceAmt,4,&sBlockData[5]);
				//PubLong2Char(gstYktEnrichLog.ulOldBalaceAmt,4,&sBlockData[9]);
				PubLong2Bcd(gstYktEnrichLog.ulTransAmt/100,2,&sBlockData[9]);
				memcpy(&sBlockData[11],gstYktEnrichLog.sNowTellNo,4);
				GetBlockVerifybyte(sBlockData);
				u8Ret= M1_WriteBlock(1*4+2,&stTemp.sKeyBList[6*1],MIF_KEYB,sBlockData,stTemp.sCardUid);
				if(u8Ret!=OK)
				{
					//lcdClrLine(2, 7);
					//lcdDisplay(0, 4, DISP_CFONT, "写卡失败");
					//lcdDisplay(0, 6, DISP_CFONT, "更新block6");
					ErrorBeep();
					//sysDelayMs(3000);
					iRet = M1_SWIPE_AGAIN;
					break;
				}*/
				iRet = M1_SWIPERET_6;
				//DispErrPrompt("第三步完成", 10);
				break;
			case M1_SWIPERET_6:
				/*//NacTxd("6006",4);
				//----------------------------------------
				//更新二扇区数据(2:  0)
				memset(sBlockData,0,sizeof(sBlockData));
				ulBalaceAmt=stTemp.sCardDataList[2*4][0]*256*256*256+stTemp.sCardDataList[2*4][1]*256*256
				+stTemp.sCardDataList[2*4][2]*256+stTemp.sCardDataList[2*4][3];
				ulBalaceAmt=ulBalaceAmt+gstYktEnrichLog.ulTransAmt;
				PubLong2Char(ulBalaceAmt,4,&sBlockData[0]);
			//	PubLong2Char(gstYktEnrichLog.ulMaxBalaceAmt,4,&sBlockData[4]);
			    memcpy(&sBlockData[4],&stTemp.sCardDataList[2*4][4],4);
				//memcpy(&sBlockData[8],&gstYktEnrichLog.sPsamId[2],4);
				PubAsc2Bcd(&gstYktEnrichLog.sPsamId[2],8,&sBlockData[8]);
				//memcpy(&sCardDataList[2*4][12],&sBlockData[12],3);
				memcpy(&sBlockData[12],&stTemp.sCardDataList[2*4][12],3);
				GetBlockVerifybyte(sBlockData);
				u8Ret= M1_WriteBlock(2*4,&stTemp.sKeyBList[6*2],MIF_KEYB,sBlockData,stTemp.sCardUid);
				if(u8Ret!=OK)
				{
					//lcdClrLine(2, 7);
					//lcdDisplay(0, 4, DISP_CFONT, "写卡失败");
					//lcdDisplay(0, 6, DISP_CFONT, "更新二扇区数据");
					ErrorBeep();
					//sysDelayMs(3000);
					iRet = M1_SWIPE_AGAIN;
					break;
				}*/
				iRet = M1_SWIPERET_7;
				//DispErrPrompt("第三步完成", 10);
				break;
			
			case M1_SWIPERET_7:
				/*//写卡第五步
				NacTxd("6007",4);
				memset(sBlockData,0,sizeof(sBlockData));
				sBlockData[0]=stTemp.sCardDataList[6*4][0];//sBlockData[0]=(sCardDataList[6*4][0]-1+1)%9+1;
				itemp=(stTemp.sCardDataList[6*4][1]*256+stTemp.sCardDataList[6*4][2]+1)/256;
				sBlockData[1]=(UINT8)itemp;
				itemp=(stTemp.sCardDataList[6*4][1]*256+stTemp.sCardDataList[6*4][2]+1)%256;
				sBlockData[2]=(UINT8)itemp;
				sBlockData[3]=stTemp.sCardDataList[6*4][3];
				memcpy(&sBlockData[4],&stTemp.sCardDataList[6*4][4],11);
				sBlockData[6]=0x02;
				GetBlockVerifybyte(sBlockData);
				u8Ret= M1_WriteBlock(6*4+1,&stTemp.sKeyBList[6*6],MIF_KEYB,sBlockData,stTemp.sCardUid);//副本公  共信息区
				if(u8Ret!=OK)
				{
				//lcdClrLine(2, 7);
				//lcdDisplay(0, 4, DISP_CFONT, "写卡失败");
				//lcdDisplay(0, 6, DISP_CFONT, "副本公共信息区");
				ErrorBeep();
				//sysDelayMs(3000);
				iRet = M1_SWIPE_AGAIN;
				break;
				}
				memcpy(stTemp.sCardDataList[6*4+1],sBlockData,16);
				*/
			    iRet = M1_SWIPERET_8;
				break;
			case M1_SWIPERET_8:	
				/*NacTxd("6008",4);
				//u8Ret = M1_BlocktoBlockValue(4*6, 4*6+1, &stTemp.sKeyBList[6*6], MIF_KEYB,stTemp.sCardUid);//把十传给九
				u8Ret= M1_WriteBlock(6*4,&stTemp.sKeyBList[6*6],MIF_KEYB,stTemp.sCardDataList[6*4+1],stTemp.sCardUid);//副本公  共信息区
				if (u8Ret != 0)
				{
					//重刷卡流程
					//lcdClrLine(2, 7);
					//lcdDisplay(0, 4, DISP_CFONT, "写卡失败");
					//lcdDisplay(0, 6, DISP_CFONT, "5008");
					ErrorBeep();
					//sysDelayMs(3000);
					iRet = M1_SWIPE_AGAIN;
					break;
				}*/
				iRet = 0;
				break;
			case M1_SWIPE_AGAIN:
				//NacTxd("6009",4);
				iRet = 0;//M1_RchSwipCardAgain();
				break;
			default:
				return NO_DISP;
				
		}
	}
}


int  M1_RchAmtProcess(void)//UINT32 uiTransAmt
{
	int		  iRet = 0;
	//UINT8     	  ucLrc = 0;
//	ulong	  ulCount;
	char          szAmount[10];
	///char		  szDateTime[7], szValidityTime[4];
//	char		  sTermID[6], szTermID[12+1];
	//ulong	  iYear;
	UINT8 sTimeStr[7];
	UINT8 u8Ret;
	//char  szAmount[10];
	//int itemp;
	UINT8 sBlockData[16];
	ulong ulBalaceAmt;
//	INT32	  iAfterRchAmt;

	
	sysGetTime(sTimeStr);
    gstYktEnrichLog.sTranDateTime[0]=0x20;
    memcpy(&gstYktEnrichLog.sTranDateTime[1],sTimeStr,6);
	iRet = M1_SWIPERET_1;
	while(1)
	{
		switch(iRet)
		{
			case 0:
			//	NacTxd("6000",4);
				return 0;
				break;
			case M1_SWIPERET_1:
				/*
				//NacTxd("6001",4);
				//写卡第一步
				// 更新S6B0，黑名单标志=0x04(公共信息区置黑)
				stTemp.sCardDataList[24][6] = 0x04;
				//gstTransInfo.sCardDataList[24][0] = ucLrc;
				ucLrc = PubCalcLRC((char*)stTemp.sCardDataList[24],15,0);
				stTemp.sCardDataList[24][15] = ucLrc;
				u8Ret= M1_WriteBlock(6*4,&stTemp.sKeyBList[6*6],MIF_KEYB,stTemp.sCardDataList[24],stTemp.sCardUid);//副本公  共信息区
				if(u8Ret!=OK)
				{
				    ErrorBeep();
					iRet = M1_SWIPE_AGAIN;
					break;
				}
				*/
				iRet = M1_SWIPERET_2;
				break;
			case M1_SWIPERET_2:
				//写卡第二步
				//NacTxd("6002",4);
				PubLong2CharEx(gstYktEnrichLog.ulTransAmt, 4, szAmount);
				//NacTxd(&gstYktEnrichLog.ulTransAmt,4);
				//NacTxd(szAmount,4);
				iRet = M1_IncValue(4*2+2, &stTemp.sKeyBList[6*2], MIF_KEYB, (UINT8*)szAmount,stTemp.sCardUid);
				if(iRet!=OK)
				{
					//lcdClrLine(2, 7);
					//lcdDisplay(0, 4, DISP_CFONT, "写钱包副本失败");
					//lcdDisplay(0, 6, DISP_CFONT, "加值副本");
					ErrorBeep();
					iRet = M1_SWIPE_AGAIN;
					break;

				}
				iRet = M1_SWIPERET_3;
				break;
			case M1_SWIPERET_3:
				//写成第三步
				// 写钱包正本
				//NacTxd("6003",4);
				iRet = M1_BlocktoBlockValue(9, 10, &stTemp.sKeyBList[6*2], MIF_KEYB,stTemp.sCardUid);//把十传给九
				if (iRet != 0)
				{
					//重刷卡流程
					ErrorBeep();
					iRet = M1_SWIPE_AGAIN;
					break;
				}
				iRet = M1_SWIPERET_4;
				break;
			case M1_SWIPERET_4:
				//NacTxd("6004",4);
				//-----------------
				gstYktEnrichLog.ulPOSTraceNo=stTemp.sCardDataList[1*4+1][12]*256+stTemp.sCardDataList[1*4+1][13];
				//memcpy(sCardDataList[1*4],sBlockData,16);
				memcpy(sBlockData,stTemp.sCardDataList[1*4+1],16);
				PubLong2Char(gstYktEnrichLog.ulPOSTraceNo+1,2,&sBlockData[12]);//计数器加一
				GetBlockVerifybyte(sBlockData);
				u8Ret= M1_WriteBlock(1*4+1,&stTemp.sKeyBList[6*1],MIF_KEYB,sBlockData,stTemp.sCardUid);
				if(u8Ret!=OK)
				{
					//lcdClrLine(2, 7);
					//lcdDisplay(0, 4, DISP_CFONT, "写卡失败");
				//	lcdDisplay(0, 6, DISP_CFONT, "更新计数器");
					ErrorBeep();
				//	sysDelayMs(3000);
					iRet = M1_SWIPE_AGAIN;
					break;
				}
				iRet = M1_SWIPERET_5;
				break;
			case M1_SWIPERET_5:
				//NacTxd("6005",4);
				//写卡第四步
				memset(sBlockData,0,sizeof(sBlockData));
				memcpy(sBlockData,&gstYktEnrichLog.sTranDateTime[1],5);
				PubLong2Char(gstYktEnrichLog.ulOldBalaceAmt,4,&sBlockData[5]);
				//PubLong2Char(gstYktEnrichLog.ulOldBalaceAmt,4,&sBlockData[9]);
				PubLong2Bcd(gstYktEnrichLog.ulTransAmt/100,2,&sBlockData[9]);
				memcpy(&sBlockData[11],gstYktEnrichLog.sNowTellNo,4);
				GetBlockVerifybyte(sBlockData);
				u8Ret= M1_WriteBlock(1*4+2,&stTemp.sKeyBList[6*1],MIF_KEYB,sBlockData,stTemp.sCardUid);
				if(u8Ret!=OK)
				{
					//lcdClrLine(2, 7);
					//lcdDisplay(0, 4, DISP_CFONT, "写卡失败");
					//lcdDisplay(0, 6, DISP_CFONT, "更新block6");
					ErrorBeep();
					//sysDelayMs(3000);
					iRet = M1_SWIPE_AGAIN;
					break;
				}
				iRet = M1_SWIPERET_6;
				//DispErrPrompt("第三步完成", 10);
				break;
			case M1_SWIPERET_6:
				//NacTxd("6006",4);
				//----------------------------------------
				//更新二扇区数据(2:  0)
				memset(sBlockData,0,sizeof(sBlockData));
				ulBalaceAmt=stTemp.sCardDataList[2*4][0]*256*256*256+stTemp.sCardDataList[2*4][1]*256*256
				+stTemp.sCardDataList[2*4][2]*256+stTemp.sCardDataList[2*4][3];
				ulBalaceAmt=ulBalaceAmt+gstYktEnrichLog.ulTransAmt;
				PubLong2Char(ulBalaceAmt,4,&sBlockData[0]);
			//	PubLong2Char(gstYktEnrichLog.ulMaxBalaceAmt,4,&sBlockData[4]);
			    memcpy(&sBlockData[4],&stTemp.sCardDataList[2*4][4],4);
				//memcpy(&sBlockData[8],&gstYktEnrichLog.sPsamId[2],4);
				PubAsc2Bcd(&gstYktEnrichLog.sPsamId[2],8,&sBlockData[8]);
				//memcpy(&sCardDataList[2*4][12],&sBlockData[12],3);
				memcpy(&sBlockData[12],&stTemp.sCardDataList[2*4][12],3);
				GetBlockVerifybyte(sBlockData);
				u8Ret= M1_WriteBlock(2*4,&stTemp.sKeyBList[6*2],MIF_KEYB,sBlockData,stTemp.sCardUid);
				if(u8Ret!=OK)
				{
					//lcdClrLine(2, 7);
					//lcdDisplay(0, 4, DISP_CFONT, "写卡失败");
					//lcdDisplay(0, 6, DISP_CFONT, "更新二扇区数据");
					ErrorBeep();
					//sysDelayMs(3000);
					iRet = M1_SWIPE_AGAIN;
					break;
				}
				iRet = M1_SWIPERET_7;
				//DispErrPrompt("第三步完成", 10);
				break;
			
			case M1_SWIPERET_7:
				/*
				//写卡第五步
				//NacTxd("6007",4);
				memset(sBlockData,0,sizeof(sBlockData));
				sBlockData[0]=stTemp.sCardDataList[6*4][0];//sBlockData[0]=(sCardDataList[6*4][0]-1+1)%9+1;
				itemp=(stTemp.sCardDataList[6*4][1]*256+stTemp.sCardDataList[6*4][2]+1)/256;
				sBlockData[1]=(UINT8)itemp;
				itemp=(stTemp.sCardDataList[6*4][1]*256+stTemp.sCardDataList[6*4][2]+1)%256;
				sBlockData[2]=(UINT8)itemp;
				sBlockData[3]=stTemp.sCardDataList[6*4][3];
				memcpy(&sBlockData[4],&stTemp.sCardDataList[6*4][4],11);
				sBlockData[6]=0x02;
				GetBlockVerifybyte(sBlockData);
				u8Ret= M1_WriteBlock(6*4+1,&stTemp.sKeyBList[6*6],MIF_KEYB,sBlockData,stTemp.sCardUid);//副本公  共信息区
				if(u8Ret!=OK)
				{
				//lcdClrLine(2, 7);
				//lcdDisplay(0, 4, DISP_CFONT, "写卡失败");
				//lcdDisplay(0, 6, DISP_CFONT, "副本公共信息区");
				ErrorBeep();
				//sysDelayMs(3000);
				iRet = M1_SWIPE_AGAIN;
				break;
				}
				memcpy(stTemp.sCardDataList[6*4+1],sBlockData,16);
				*/
			    iRet = M1_SWIPERET_8;
				break;
			case M1_SWIPERET_8:	
				/*
				//NacTxd("6008",4);
				//u8Ret = M1_BlocktoBlockValue(4*6, 4*6+1, &stTemp.sKeyBList[6*6], MIF_KEYB,stTemp.sCardUid);//把十传给九
				u8Ret= M1_WriteBlock(6*4,&stTemp.sKeyBList[6*6],MIF_KEYB,stTemp.sCardDataList[6*4+1],stTemp.sCardUid);//副本公  共信息区
				if (u8Ret != 0)
				{
					//重刷卡流程
					//lcdClrLine(2, 7);
					//lcdDisplay(0, 4, DISP_CFONT, "写卡失败");
					//lcdDisplay(0, 6, DISP_CFONT, "5008");
					ErrorBeep();
					//sysDelayMs(3000);
					iRet = M1_SWIPE_AGAIN;
					break;
				}
				*/
				iRet = 0;
				break;
			case M1_SWIPE_AGAIN:
			//	NacTxd("6009",4);
				iRet = M1_RchSwipCardAgain();
				break;
			default:
				return NO_DISP;
				
		}
	}
}

int M1_RchSwipCardAgain(void)
{
   INT32 CardInfoLen;
   UINT8 CardInfo[50];
	int iRet = 0;

	while(1)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT, "请重新刷卡");

		iRet = ChkContactlessCardPad(&CardInfoLen, CardInfo);
		if(iRet != MIF_RET_SUCCESS)
		{
			return iRet ;
		}
		if(memcmp(stTemp.sCardUid, &CardInfo[5], 4) != 0)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "非上一笔交易卡号");
			PubWaitKey(3);
			continue;
		}
		//获取卡片所有数据
		iRet = GetAllDataFromM1Card();
		if(iRet != 0)
		{
			continue;
		}
		//判断是否是交易完预期数据
		/*if(gstTransInfo.sCardDataList[24][6] == 0x02
			&&memcmp(stTemp.sCardDataList[10], stTemp.sCardDataListSub[10], 12) != 0//步骤二成功
			&&memcmp(stTemp.sCardDataList[9], stTemp.sCardDataListSub[9], 12) != 0//步骤三成功
			//&&memcmp(gstTransInfo.sCardDataList[5], gstTransInfo.sCardDataListSub[5], 16) != 0
			&&memcmp(stTemp.sCardDataList[6], stTemp.sCardDataListSub[6], 16) != 0//步骤四成功
			&&memcmp(stTemp.sCardDataList[24], stTemp.sCardDataList[24], 16) != 0)//步骤五成功
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "重刷卡成功");
			sysDelayMs(3000);
			return 0;
		}*/
		//判断S6B0_6 是否被置黑
		if(stTemp.sCardDataList[24][6] != 0x04)
		{
			if(memcmp(stTemp.sCardDataList[10], stTemp.sCardDataListSub[10], 12) == 0)
			{
				return M1_SWIPERET_1;
			}

			return 0;
		}
		//副本加值是否成功
		if(memcmp(stTemp.sCardDataList[10], stTemp.sCardDataListSub[10], 12) == 0)
		{
			return M1_SWIPERET_2;
		}
			//正本加值是否成功
		if(memcmp(stTemp.sCardDataList[9], stTemp.sCardDataListSub[9], 12) == 0)
		{
			return M1_SWIPERET_3;
		}
	    //计数器是否跟新
		if(memcmp(stTemp.sCardDataList[5], stTemp.sCardDataListSub[5], 16) == 0)
		{
			return M1_SWIPERET_4;
		}
		//卡片加值记录是否成功
		if(memcmp(stTemp.sCardDataList[6], stTemp.sCardDataListSub[6], 16) == 0)
		{
			return M1_SWIPERET_5;
		}
		//公共信息区0块是否与充值预期结果一致
		if(memcmp(stTemp.sCardDataList[8], stTemp.sCardDataListSub[8], 16) == 0)
		{
			return M1_SWIPERET_6;
		}
		if(memcmp(stTemp.sCardDataList[25], stTemp.sCardDataListSub[25], 16) == 0)
		{
			return M1_SWIPERET_7;
		}
		//-会有问题--
		if(memcmp(stTemp.sCardDataList[24], stTemp.sCardDataList[25], 16) != 0)
		{
			return M1_SWIPERET_8;
		}
		return 0;
	}
}
int ChkContactlessCardPad(INT32 *CardInfoLen, UINT8 *CardInfo)
{
	int       iRet;
	MIF_TIME_STRUCT stMifTimeInfo;
	unsigned char szCommand[200];
	memset(szCommand,0,sizeof(szCommand));

	iRet=0;
	mifOpen();
	mifGetTimeOut(&stMifTimeInfo);
	stMifTimeInfo.WaitTime_PollTypeB *= 5;
	stMifTimeInfo.WaitTime_PollTypeA *= 5;
	stMifTimeInfo.WaitTime_RATS *= 20;
	mifSetTimeOut(&stMifTimeInfo);
	memcpy(szCommand,"\x00\xFF\xFF\x01\x00",5);
	if (1)//非接触式卡
	{
		while(1)
		{
			iRet = mifCheckOnlyOneCard(0x07);
			if (iRet == MIF_RET_MULTICARD)
			{
				sysBeep();
				lcdClrLine(2,7);
				lcdPrintf("有 多 张 卡");
				sysDelayMs(3000);
				mifResetRF(0);
				return NO_DISP;
			} 
			else if ((MIF_RET_SUCCESS!=iRet) && (MIF_RET_NOCARD!=iRet) && (MIF_RET_TIMEOUT!=iRet))
			{
				sysBeep();
				lcdClrLine(2,7);
				lcdPrintf("读 卡 错 误:%d", iRet);
				sysDelayMs(3000);
				mifResetRF(0);
				return NO_DISP;
			}
			else if(iRet == MIF_RET_SUCCESS)
			{
				// 在感应区内查找指定调制类型的卡片，并返回卡片的序列号等相关信息
				iRet = InListPassiveTarget(0, 0, szCommand, CardInfoLen,  CardInfo);
				//glstMsgInfo.stMagInfo.iRetCode = glstMsgInfo.stMagInfo.szTrack1[4];
				//memcpy(glstMsgInfo.stMagInfo.szTrack1,&glstMsgInfo.stMagInfo.szTrack1[5],glstMsgInfo.stMagInfo.iRetCode);
				//glstMsgInfo.stMagInfo.szTrack1[glstMsgInfo.stMagInfo.iRetCode] = 0;
				if (iRet == MIF_RET_SUCCESS)
				{
					//PubBeepOk();
					return MIF_RET_SUCCESS;
				}
			}
			else if(iRet == -5){}
			else
			{
				lcdPrintf("读    卡:%d", iRet);
				sysDelayMs(3000);
				break;
			}
			if(PubChkKeyInput())
			{
				iRet = kbGetKey();
				if(iRet == KEY_CANCEL)
				{
					return M1_ERR_USERCANCEL;
				}
			}

		}
		mifResetRF(0);
	}

	return iRet;
}
