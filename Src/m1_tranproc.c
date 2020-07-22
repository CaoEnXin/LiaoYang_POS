/*****************************************************************************************************
* m1_tranproc.c
*
*auth: howard.joe
*
* ����: ���ļ�������ʵ��M1���Ĵ������ڱ����̵�ʹ�õģ����豣�ֶ����ԣ����Ҫ��ֲ����Ĺ����ϣ��迼���¹��̵�
*���������޸ġ����⣬Ҫ�����ܱ�֤���������̲���ҪM1������ʱ�����Է���ز�ж��
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
	memcpy(stApduSend.DataIn,"\x10\x01",8);//��������
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
		lcdDisplay(0, 4, DISP_CFONT, "�������1");
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
		memcpy(stApduSend.DataIn,"\x20\x00\x07\x01\x20\x00\x11\x01",8);//��������
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
			lcdDisplay(0, 4, DISP_CFONT, "�������2");
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
			PubDispString("ȡ�������", 3|DISP_LINE_CENTER);
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		}
		memcpy(sRandom,stApduResp.DataOut,8);
		//NacTxd(stApduResp.DataOut, 50);
		return 0;

}


//����:inbufҪ���ܵ���������
//����:lenҪ���ܵ����ݳ��ȣ�����֮ǰ�����Ȳ���0x00
//���:inbuf���ܺ����������
int TDEACBCEnry(uint8_t * inbuf, int len,int* poutlen,uint8_t * key)
{
	uint8_t  buf[17];//, tmpbuf[17];
	int 	 l,  ret;
	int      i,k;

	
	memset(buf, 0, 8);//��ʼ������
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
//����:inbufҪ���ܵ���������
//����:lenҪ���ܵ����ݳ���
//���:inbuf���ܺ����������
int TDEACBCDecry(uint8_t * inbuf, int len,uint8_t * key)
{
	uint8_t  buf[17],inbuftemp[500];// tmpbuf[17],
	int 	 l,  ret;
	int      i,k;

	memcpy(inbuftemp,inbuf,len+8);
	memset(buf, 0, 8);//��ʼ������
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
		PubDispString("ISAM�ڲ���֤��", 3|DISP_LINE_CENTER);
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
		PubDispString("ISAM�ڲ���֤��", 3|DISP_LINE_CENTER);
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
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��������"); 
//	lcdDisplay(0, 6, DISP_CFONT|DISP_MEDIACY, "��ֵ������ο�"); 
	iOldTime = sysGetTimerCount();
	while(1)
	{
		iNowTime= sysGetTimerCount();
		if( (iNowTime-iOldTime) >= (UINT32)(stPosParam.ucOprtLimitTime*1000) )	 // ��鶨ʱ��
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
				PubDispString("�û����ϵ�ʧ��", 3|DISP_LINE_CENTER);
				PubDispString("��Ч����������", 5|DISP_LINE_CENTER);
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
	/////////�Թ�������/////////
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS = 0x20;
	stApduSend.P1 = 0;
	stApduSend.P2 = 0x41;
	stApduSend.Lc = 0x08;
	memcpy(stApduSend.DataIn,"\x00\x00\x00\x00\x00\x00\x00\x00",8);//��������
	stApduSend.Le = 0;
	stApduSend.LeFlag = 0;
//	NacTxd(&stApduSend, 20);
	iRet = iccExchangeAPDU(ICC_USERCARD,&stApduSend,&stApduResp);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{

	    lcdClrLine(2,7);
		lcdDisplay(0, 2, DISP_CFONT, "%02X %02X", stApduResp.SWA, stApduResp.SWB);
		lcdDisplay(0, 4, DISP_CFONT, "�������1");
		kbGetKey();
		return -2;
	}
	//
	//////////�Թ�����֤��������Կ/////////////
	memset((char*)&stApduSend,0,sizeof(stApduSend));
	memset((char*)&stApduResp,0,sizeof(stApduResp));
	stApduSend.CLA = 0;
	stApduSend.INS = 0x20;
	stApduSend.P1 = 0;
	stApduSend.P2 = 0x42;
	stApduSend.Lc = 0x08;
	memcpy(stApduSend.DataIn,"\x20\x00\x07\x01\x20\x00\x11\x01",8);//��������
	stApduSend.Le = 0;
	stApduSend.LeFlag = 0;
//	NacTxd("2001",4);
//	NacTxd(&stApduSend, 20);
	iRet = iccExchangeAPDU(ICC_USERCARD,&stApduSend,&stApduResp);
	if ((iRet!=ICC_SUCCESS)||(!(stApduResp.SWA == 0x90 && stApduResp.SWB == 0x00 )))
	{
	
		lcdClrLine(2,7);
		lcdDisplay(0, 2, DISP_CFONT, "%02X %02X", stApduResp.SWA, stApduResp.SWB);
		lcdDisplay(0, 4, DISP_CFONT, "�������2");
		kbGetKey();
		return -2;
	}
//	NacTxd("2002",4);

	/////////С��ȡ�����//////
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
		PubDispString("ȡ�������", 3|DISP_LINE_CENTER);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	memcpy(sRandom,stApduResp.DataOut,8);
//	NacTxd(stApduResp.DataOut, 50);

	///////////////���ڲ���֤/////////////
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
		PubDispString("���ڲ���֤��", 3|DISP_LINE_CENTER);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	memcpy(sRandom,stApduResp.DataOut,8);
//	NacTxd(stApduResp.DataOut, 50);
//	NacTxd("2010", 4);

	///////////////С���ⲿ��֤/////////////
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
		PubDispString("С���ⲿ��֤��", 3|DISP_LINE_CENTER);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	///////////////�Թ���ȡ�����///////////////
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
		PubDispString("ȡ�������", 3|DISP_LINE_CENTER);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	memcpy(sRandom,stApduResp.DataOut,8);
//	NacTxd(stApduResp.DataOut, 50);
//	NacTxd("2012", 4);

	///////////////ISAM�ڲ���֤/////////////
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
		PubDispString("ISAM�ڲ���֤��", 3|DISP_LINE_CENTER);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	memcpy(sRandom,stApduResp.DataOut,8);
//	NacTxd(stApduResp.DataOut, 50);

	///////////////���ⲿ��֤/////////////
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
		PubDispString("���ⲿ��֤��", 3|DISP_LINE_CENTER);
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
	
	/////////////////ѡ�񹫹�Ǯ��:00A4040009A00000000386980701/////
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
	    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��Ӧ��ѡ���");
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
		    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "�����ѿ��ļ���");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		// return ERR_CODE_USECARD_SELECT_ADF;
	    }
		//gstYktEnrichLog.uCardtype=szRecvData[30];
		//memcpy(gstYktEnrichLog.sCardSerialNo,&szRecvData[12],8);//��Ӧ�����к� 
		PubBcd2Asc(&szRecvData[12],8,gstYktEnrichLog.sCardSerialNo);//��Ӧ�����к� 

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
		    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "�����ѿ��ļ���");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		// return ERR_CODE_USECARD_SELECT_ADF;
	    }
		gstYktEnrichLog.uCardtype=szRecvData[0];
 

/*			
//�ж����ñ�־
       
	   	if(szRecvData[37]==0x01)
	   	{
			; 
		}
		else if(szRecvData[37]==0x02)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT, "��Ƭ״̬:");
			lcdDisplay(0, 5, DISP_CFONT, "ͣ��");
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP;

		}
		else if(szRecvData[37]==0x00)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT, "��Ƭ״̬:");
			lcdDisplay(0, 5, DISP_CFONT, "δ����");
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP;

		}
		else
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT, "��Ƭ״̬:");
			lcdDisplay(0, 5, DISP_CFONT, "δ֪");
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP;
		}

		
		PubAsc2Bcd(pInParam->szDate,8,szBuffer);
		if (memcmp(&szRecvData[33],szBuffer,4)<0 )
	   	{
		   lcdClrLine(2, 7);
		   lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��������ѹ���");
		   lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "�����");
		   PubBeepErr();
		   PubWaitKey(3);
		   return NO_DISP; 
	    }
		 	
		if (memcmp(&szRecvData[24],szBuffer,4)<0 )
	   	{
		   lcdClrLine(2, 7);
		   lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "Ӧ����Ч���ѹ�");
		  // lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "�����");
		   PubBeepErr();
		   PubWaitKey(3);
		   return NO_DISP; 
	    }
	   	PubBcd2Asc(&szRecvData[12],8,gstYktEnrichLog.sCardSerialNo);
	    //PubBcd2Asc(&szRecvData[12],8,PosCom.stTrans.sCardSerialNo);
	//   memcpy(PosCom.stTrans.SCardSerialNo,&szRecvData[12],8);//UINT8 SCardSerialNo[8];//��Ӧ�����к� 
	 //  gstYktEnrichLog.uCardtype=szRecvData[8];
*/

		//----ѡ��1001  Ŀ¼
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
	    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "Ŀ¼ѡ���");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
		 //return ERR_CODE_USECARD_SELECT_ADF;
	    }*/
		 
		//-------���������-------------------------
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
			lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "����������ڴ�");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		// return ERR_CODE_USECARD_SELECT_ADF;
	    }
		//---��֤���---------
         PubAsc2Bcd(pInParam->szDate,8,szBuffer);
	   if (memcmp(szRecvData,szBuffer,4)<0 )
	   	{
		   lcdClrLine(2, 7);
		   lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��������ѹ���");
		   lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "�����");
		   PubBeepErr();
		   PubWaitKey(3);
		   return NO_DISP; 


	    }*/
		//----------------
//		NacTxd("0017", 4);
//		NacTxd(szRecvData,iRecvLen);//
		//memcpy(pOutParam->CardInfo.AppInfo.AppInsideNum, &szRecvData[12], 8);//����Ʊ������

		//----������֤-----------
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
			lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "����У���");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		  // return ERR_CODE_VERIFY_PIN;
		}*/
//NacTxd("0018", 4);
//NacTxd(szRecvData,iRecvLen);
		//---����ѯ
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
			lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "����ѯ��");
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

    //��λס����psam��ָ��
	UINT8 ResetPsam2(void)
	{
		int    iRet;
		
		iRet = SocketSet(1, 0);
		if( iRet!=0 )
		{
			return NO_DISP; 	
		}
	
		// SAM���ϵ縴λ
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
	/////////////////ѡ�񹫹�Ǯ��:\x00\xA4\x04\x00\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01/////
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
	    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��Ӧ��ѡ���");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}

	////////////////�û�����0015����Ӧ�������ļ���ȡӦ�����к�:\x00\xB0\x96\x00\x1E���ӵ�24λ��ʼ����ȡ16λ�ַ���
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
		    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "�����ѿ��ļ���");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		// return ERR_CODE_USECARD_SELECT_ADF;
	    }
		//gstYktEnrichLog.uCardtype=szRecvData[30];
		//memcpy(gstYktEnrichLog.sCardSerialNo,&szRecvData[12],8);//��Ӧ�����к� 
		PubBcd2Asc(&szRecvData[12],8,gstYktEnrichLog.sCardSerialNo);//��Ӧ�����к� 

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
		    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "�����ѿ��ļ���");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		// return ERR_CODE_USECARD_SELECT_ADF;
	    }
		gstYktEnrichLog.uCardtype=szRecvData[0];
 

/*			
//�ж����ñ�־
       
	   	if(szRecvData[37]==0x01)
	   	{
			; 
		}
		else if(szRecvData[37]==0x02)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT, "��Ƭ״̬:");
			lcdDisplay(0, 5, DISP_CFONT, "ͣ��");
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP;

		}
		else if(szRecvData[37]==0x00)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT, "��Ƭ״̬:");
			lcdDisplay(0, 5, DISP_CFONT, "δ����");
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP;

		}
		else
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT, "��Ƭ״̬:");
			lcdDisplay(0, 5, DISP_CFONT, "δ֪");
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP;
		}

		
		PubAsc2Bcd(pInParam->szDate,8,szBuffer);
		if (memcmp(&szRecvData[33],szBuffer,4)<0 )
	   	{
		   lcdClrLine(2, 7);
		   lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��������ѹ���");
		   lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "�����");
		   PubBeepErr();
		   PubWaitKey(3);
		   return NO_DISP; 
	    }
		 	
		if (memcmp(&szRecvData[24],szBuffer,4)<0 )
	   	{
		   lcdClrLine(2, 7);
		   lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "Ӧ����Ч���ѹ�");
		  // lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "�����");
		   PubBeepErr();
		   PubWaitKey(3);
		   return NO_DISP; 
	    }
	   	PubBcd2Asc(&szRecvData[12],8,gstYktEnrichLog.sCardSerialNo);
	    //PubBcd2Asc(&szRecvData[12],8,PosCom.stTrans.sCardSerialNo);
	//   memcpy(PosCom.stTrans.SCardSerialNo,&szRecvData[12],8);//UINT8 SCardSerialNo[8];//��Ӧ�����к� 
	 //  gstYktEnrichLog.uCardtype=szRecvData[8];
*/

		//----ѡ��1001  Ŀ¼
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
	    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "Ŀ¼ѡ���");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
		 //return ERR_CODE_USECARD_SELECT_ADF;
	    }*/
		 
		//-------���������-------------------------
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
			lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "����������ڴ�");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		// return ERR_CODE_USECARD_SELECT_ADF;
	    }
		//---��֤���---------
         PubAsc2Bcd(pInParam->szDate,8,szBuffer);
	   if (memcmp(szRecvData,szBuffer,4)<0 )
	   	{
		   lcdClrLine(2, 7);
		   lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��������ѹ���");
		   lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "�����");
		   PubBeepErr();
		   PubWaitKey(3);
		   return NO_DISP; 


	    }*/
		//----------------
//		NacTxd("0017", 4);
//		NacTxd(szRecvData,iRecvLen);//
		//memcpy(pOutParam->CardInfo.AppInfo.AppInsideNum, &szRecvData[12], 8);//����Ʊ������

		//----������֤-----------
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
			lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "����У���");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		  // return ERR_CODE_VERIFY_PIN;
		}*/
//		NacTxd("0018", 4);
//		NacTxd(szRecvData,iRecvLen);
		//---����ѯ
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
			lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "����ѯ��");
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

	
	PubDisplayTitle(TRUE, "��  ��");
	//iRet =ResetPsam(ICC_SAM1, ICC_ISO);//��λpsam��,������ʾ����
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
		PubDispString("SAM�ϵ縴λʧ��", 3|DISP_LINE_CENTER);
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
	   if( (iNowTime-iOldTime) >= (UINT32)(stPosParam.ucOprtLimitTime*1000) )	// ��鶨ʱ��
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
            // �ڸ�Ӧ���ڲ���ָ���������͵Ŀ�Ƭ�������ؿ�Ƭ�����кŵ������Ϣ
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
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��ˢ��");
		iNowTime= sysGetTimerCount();
		if( (iNowTime-iOldTime) >= (UINT32)(stPosParam.ucOprtLimitTime*1000) )	// ��鶨ʱ��
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

	if(memcmp(&sCardInfo[3],"\x28",1)==0 || memcmp(&sCardInfo[3],"\x20",1)==0){// CPU����ֵ
		iRet = InATR(&iCardInfoLen,sCardInfo);
		if (iRet != 0)
		{
			return E_TRANS_FAIL;
		}
		stInParam.ucSwipedFlag=CARD_PASSIVE;
		GetPosTime(stInParam.szDate, stInParam.szTime);	
		PubAsc2Bcd(stInParam.szDate,8,&gstYktEnrichLog.sTranDateTime[0]);
		PubAsc2Bcd(stInParam.szTime,6,&gstYktEnrichLog.sTranDateTime[4]);
	
		ucRet = CPU_ReadSaleCard(&stInParam);// ����CPU���ļ�

		if(ucRet !=OK)
		{
		   return NO_DISP;
		}
		
	}else if(memcmp(&sCardInfo[3],"\x08",1)==0){//M1����ֵ
		memcpy(sSect1KeyA,"\xA0\xA1\xA2\xA3\xA4\xA5",6);
		memcpy(&stTemp.sKeyBList ,sSect1KeyA,6);
		ucRet =M1_ReadSector(0, sSect1KeyA,MIF_KEYA, sSectorData,stTemp.sCardUid);//����48�ֽ�
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
		ucRet =M1_ReadSector(1, sSect1KeyA,MIF_KEYA, sSectorData,stTemp.sCardUid);//����48�ֽ�
		if(ucRet !=0)
		{
			return NO_DISP;
		}
		memcpy(stTemp.sCardDataList[4],sSectorData,48);
	//NacTxd("9001", 4);
	//NacTxd(sSectorData, 48);
	
		//������ʱ����
		memcpy(sCityCode,&sSectorData[0],2);//
		memcpy(sTrace,&sSectorData[4],4);// liu shui
		memcpy(sMac,&sSectorData[8],4);//ren zhen ma
		PubBcd2Asc(&sSectorData[0],8,gstYktEnrichLog.sCardSerialNo);
		gstYktEnrichLog.uCardtype=sSectorData[13];//������
	//NacTxd(sMac, 8);
	
	//sSectorOper	
	//	memset(sAllKeyB,0,sizeof(sAllKeyB));
		if(keyType==0)//����1�ϵ�С��
		{
			iRet= GetSecPW(stTemp.sCardUid, sTrace, sMac, sCityCode, "\x00\x01\x10\x03\x03\x03\x06\x11\x12\x13\x14\x15\x16\x17\x18\x19","1111111110000000", 1, stTemp.sKeyBList);
			if(iRet !=0)
			{
				return NO_DISP;
			}
		//NacTxd("0008", 4);
		//NacTxd(stTemp.sKeyBList, 6*16);
			iRet = GetAllDataFromM1Card();//��ȡ������������
			if(iRet != 0)
			{
				return iRet;
			}
		//NacTxd("0009", 4);
			iRet = M1_IsCardInfoLegal();//�жϿ������ݵĺϷ���
			if(iRet != 0)
			{
				return iRet;
			}
		}
		
		else{  //����2�ϵ�С��
			iRet= M1_GetCalKeyA(stTemp.sCardUid, sTrace, sMac, sCityCode, "\x00\x01\x10\x03\x03\x03\x06\x11\x12\x13\x14\x15\x16\x17\x18\x19","1111111110000000", stTemp.sKeyBList);
			if(iRet !=0)
			{
				return NO_DISP;
			}
		//NacTxd("1008", 4);
		//NacTxd(stTemp.sKeyBList, 6*16);
			
			//��ȡ������������
			for(i=0;i<=8;i++)
			{
				memset(sSectorData,0,sizeof(sSectorData));//stTemp.sKeyBList
				ucRet =M1_ReadSector(i, &stTemp.sKeyBList[6*i],MIF_KEYA, sSectorData,stTemp.sCardUid);//����48�ֽ�
				//NacTxd(&i, 4);
				if(ucRet !=0)
				{
					return NO_DISP;
				}
				memcpy(stTemp.sCardDataList[i*4],sSectorData,48);
		//NacTxd(stTemp.sCardDataList[i*4],48);
			}
				
			iRet = M1_IsCardInfoLegal();//�жϿ������ݵĺϷ���
			if(iRet != 0)
			{
				return iRet;
			}
			memset(stTemp.sCardDataListSub, 0, sizeof(stTemp.sCardDataListSub));
			memcpy(stTemp.sCardDataListSub, stTemp.sCardDataList, 64*16);
		}
		
		//---- �����stTemp.sCardDataList[4][13]
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
	lcdDisplay(0, 2, DISP_CFONT, "���:%s", szAmount);
	LongToAmount(szAmount, gstYktEnrichLog.ulMaxBalaceAmt);
	lcdDisplay(0, 6, DISP_CFONT, "�޶�:%s", szAmount);
	
    if(gstYktEnrichLog.uCardtype==0x00)
	{
		lcdDisplay(0, 4, DISP_CFONT, "������:��ͨ��");
	}
	 else if(gstYktEnrichLog.uCardtype==1)
	{
		lcdDisplay(0, 4, DISP_CFONT, "������:���˿� A");
	}
	 else if(gstYktEnrichLog.uCardtype==2)
	{
		lcdDisplay(0, 4, DISP_CFONT, "������:���˿� B");
	}
	 else if(gstYktEnrichLog.uCardtype==3)
	{
		lcdDisplay(0, 4, DISP_CFONT, "������:ѧ���� A");
	}
	 else if(gstYktEnrichLog.uCardtype==4)
	{
		lcdDisplay(0, 4, DISP_CFONT, "������:ѧ���� B");
	}
	else if(gstYktEnrichLog.uCardtype==5)
	{
		lcdDisplay(0, 4, DISP_CFONT, "������:Ԥ���� A");
	}
	else if(gstYktEnrichLog.uCardtype==6)
	{
		lcdDisplay(0, 4, DISP_CFONT, "������:Ԥ���� B");
	}
	else if(gstYktEnrichLog.uCardtype==7)
	{
		lcdDisplay(0, 4, DISP_CFONT, "������:CPU��");
	}
	else if(gstYktEnrichLog.uCardtype==8)
	{
		lcdDisplay(0, 4, DISP_CFONT, "������:Ա����");
	}
	else if(gstYktEnrichLog.uCardtype==9)
	{
		lcdDisplay(0, 4, DISP_CFONT, "������:���Ŀ�");
	}
	else if(gstYktEnrichLog.uCardtype==10)
	{
		lcdDisplay(0, 4, DISP_CFONT, "������:�Ŵ���");
	}
	else if(gstYktEnrichLog.uCardtype==11)
	{
		lcdDisplay(0, 4, DISP_CFONT, "������:���ſ�");
	}
	else if(gstYktEnrichLog.uCardtype==12)
	{
		lcdDisplay(0, 4, DISP_CFONT, "������:������");
	}
	else
	{
		lcdDisplay(0, 4, DISP_CFONT, "������:δ֪����");

	}
    
   kbGetKeyMs(10*1000);
	return 0;
}
//-----��ѯ�������---
uint8_t M1_Que_Sett(void)
{
	//----
	INT32 iTotalNum,lCurIndex;
	INT32  iTranNum,iTotalTranAmt,iTotalSaleAmt;
    INT32	 iCnt,i;//, iRet
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ���׽���    ", "  QUERY TRANS   ");
	kbFlush();
	// PubDisplayTitle(TRUE,"������ϸ��ѯ");
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
	   PubDisplayTitle(TRUE,"�������ײ�ѯ");
	   PubDispString("�޽���", 3|DISP_LINE_CENTER);
	   PubWaitKey(10);
	   return 0;
	}
	for(i=0;i<iTotalNum;i++)
	{
	  ReadYtkEnrichLog(&iCnt);
	  if(gstYktEnrichLog.uiStatus==0x01)//������
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
	   PubDisplayTitle(TRUE,"�������ײ�ѯ");
	   PubDispString("�޽���", 3|DISP_LINE_CENTER);
	   PubWaitKey(10);
	   return 0;
	}
	
	lcdClrLine(2,7);
	lcdDisplay(0, 2, DISP_CFONT, "���ױ���: %d", iTranNum);
	lcdDisplay(0, 4, DISP_CFONT, "��ֵ���: %d.%02d", iTotalTranAmt/100,iTotalTranAmt%100);
	lcdDisplay(0, 6, DISP_CFONT, "���ѽ��: %d.%02d", iTotalSaleAmt/100,iTotalSaleAmt%100);
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
	memcpy(&Pfield[iCnt],PLog->sCardSerialNo,16); //����
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
	sprintf(szBuffer, "%d",PLog->uCardtype); //������
	istrlen=strlen(szBuffer);
	memcpy(&Pfield[iCnt],szBuffer,istrlen);
	iCnt+=istrlen;
	Pfield[iCnt]=',';
	iCnt+=1;

	memset(szBuffer,0,sizeof(szBuffer));
	sprintf(szBuffer, "%d",PLog->uMode); //��ֵ������
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

	memset(szBuffer,0,sizeof(szBuffer));//��ֵ���
	sprintf(szBuffer, "%ld",PLog->ulTransAmt);
	istrlen=strlen(szBuffer);
	memcpy(&Pfield[iCnt],szBuffer,istrlen);
	iCnt+=istrlen;
	Pfield[iCnt]=',';
	iCnt+=1;
	
	//NacTxd("0009",4);
	memset(szBuffer,0,sizeof(szBuffer));//��ֵ����
	sprintf(szBuffer, "%ld",PLog->ulNowBalaceAmt);
	istrlen=strlen(szBuffer);
	memcpy(&Pfield[iCnt],szBuffer,istrlen);
	iCnt+=istrlen;
	Pfield[iCnt]=',';
	iCnt+=1;

	//NacTxd("0008",4);
	memset(szBuffer,0,sizeof(szBuffer));//��ֵǰ���
	sprintf(szBuffer, "%ld",PLog->ulOldBalaceAmt);
	istrlen=strlen(szBuffer);
	memcpy(&Pfield[iCnt],szBuffer,istrlen);
	iCnt+=istrlen;
	Pfield[iCnt]=',';
	iCnt+=1;

 	//30 30 31 00 38 31 34 37 39 35 30 39 31 32 33 34 35 30 39 00 00 00 00 00 00 
	//istrlen=strlen(PLog->sTellNo);//����Ա��
	//memcpy(&Pfield[iCnt], PLog->sTellNo,istrlen);
	memcpy(&Pfield[iCnt], PLog->sTellNo+1,2);
	iCnt+=2;
	Pfield[iCnt]=',';
	iCnt+=1;

	//NacTxd("1007",4);
	//memcpy(&Pfield[iCnt], PLog->sPsamId,10);//�ն˺�
	memcpy(&Pfield[iCnt], PLog->sPosSN,8);
	iCnt+=8;
	Pfield[iCnt]=',';
	iCnt+=1;

	//NacTxd("1008",4);
	//PubBcd2Asc(&PLog->sNowTellNo[2],2,&Pfield[iCnt]);//��·/ ��ֵ���
	memcpy(&Pfield[iCnt], PLog->pointofsaleNo,4);
	iCnt+=4;
	Pfield[iCnt]=',';
	iCnt+=1;
	//NacTxd("0007",4);
	
	i = strlen(PLog->sCompanyNo);
	memcpy(&Pfield[iCnt], PLog->sCompanyNo,i);//��˾��/ ���ź�
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
	
	//--��ֵPOS��ˮ��--------------
	memset(szBuffer,0,sizeof(szBuffer));
	sprintf(szBuffer, "%d",PLog->lEnrichTraceNo);
	istrlen=strlen(szBuffer);
	memcpy(&Pfield[iCnt],szBuffer,istrlen);
	iCnt+=istrlen;
	Pfield[iCnt]=',';
	iCnt+=1;
	//-----POS��ֵ�ۻ����-----------------
	memset(szBuffer,0,sizeof(szBuffer));//POS��ֵ�ۻ����
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
//-----���ҿɴ�ȡ�Ľڵ㡣

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

//----��ֵ����㣬���ɽ����¼-
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ���׽���    ", "  QUERY TRANS   ");
	kbFlush();
	// PubDisplayTitle(TRUE,"������ϸ��ѯ");
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
	   PubDisplayTitle(TRUE,"�������ײ�ѯ");
	   PubDispString("�޽���", 3|DISP_LINE_CENTER);
	   PubWaitKey(10);
	   return NO_DISP;
	}

	//-----���ҿɴ�ȡ�Ľڵ�
	iRet=FindEnrichStoreFilePosition();
	if(iRet<0)
	{
	 
	   lcdClrLine(2,7);
	   lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "���������ϴ�");
	   lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "�ٽ���");
	   PubWaitKey(10);
	   return NO_DISP;

	}
	 
	for(i=0;i<iTotalNum;i++)
	{
		ReadYtkEnrichLog(&iCnt);
	    if(gstYktEnrichLog.uiStatus==0x01)//������
		{
			iCnt--;
			if (iCnt == 0)
			{
				iCnt=iTotalNum;
	 		}
			 // break;   //ע�⣬���п���������
			 // iCnt=iCnt%iTotalNum;
		 }
		 else
		 {
			
			//---���----
			memset(szEnrichdata,0,sizeof(szEnrichdata));
			PackFTP(&gstYktEnrichLog,szEnrichdata,&iEnrichDatalen,sztime);
			uRet= WriteEnrichFtpFile(szEnrichdata,iEnrichDatalen, sztime);//����sztime�Ǵ洢��POS������ļ�����
			if(uRet==OK)
			{
				//������־
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
					DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "�����ļ�����ʧ��", "FILE ERROR");
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
    	PubDisplayTitle(TRUE,"�������׽���");
   		PubDispString("�޽���", 3|DISP_LINE_CENTER);
   		PubWaitKey(10);
    	return NO_DISP;
	}

	SaveM1EnrichLastTotal();
	PrtM1EnrichTotal(1);
	
	lcdCls();
    PubDisplayTitle(TRUE,"���׽���");
	lcdDisplay(0, 2, DISP_CFONT, "���ױ���: %d", iTranNum);
	lcdDisplay(0, 4, DISP_CFONT, "��ֵ���: %d.%02d", iTotalTranAmt/100,iTotalTranAmt%100);
	lcdDisplay(0, 6, DISP_CFONT, "���ѽ��: %d.%02d", iTotalSalAmt/100,iTotalSalAmt%100);
	kbGetKeyMs(10*1000);
	kbFlush();

	M1_Data_UpLoad_TCP();
	
	return 0;

}
//--�����ӡ----
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
	    PubDispString("������", 3|DISP_LINE_CENTER);
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
//---�����ϴ�---------------
uint8_t M1_Data_UpLoad_TCP(void)
{
	FTP_PARA stFtpPara;
	int iRet,fd;
	int iposition;
	uint8_t Ret;
	int itotalFile;
	int ioffSet=0;//ƫ����
	int iFileSize=0;//�ļ���С
	int iCnt;   //
	int iMsgSN=1; //���ݱ����к�
	int iReadLen;
	uint8_t szReadbuf[2048];
	uint8_t utemp;
	int  iuploadCnt=0;

	memset(&stFtpPara, 0, sizeof(FTP_PARA));
	itotalFile=GetFtpFiletotal();

	iRet=GetFtpFilePosition();
	if(iRet<0)
	{//û���ϴ�������
		lcdClrLine(0,7);
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "�������ϴ�");
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "���Ƚ���");
		PubWaitKey(10);
		return NO_DISP;
	}
	iposition=iRet;
	strcpy(stFtpPara.szLocalfile,stTransCtrl.stEnrichFile[iposition].szLocalfile);
	sprintf(stFtpPara.szRemotefile, "%s",  stTransCtrl.stEnrichFile[iposition].szRemotefile);

	iFileSize=fileSize(stTransCtrl.stEnrichFile[iposition].szLocalfile);
	if(iFileSize<0)
	{
		lcdDisplay(0, 4, DISP_MEDIACY|DISP_CFONT, "�ϴ�֮ǰ����");
	    DispFileErrInfo();
		return (E_MEM_ERR);
	}
	
	iuploadCnt=0;

	while (ioffSet < iFileSize)
	{
	    if(iuploadCnt>3)
	    {
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "�������");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "������");
			FailBeep();
			kbGetKey();
			return NO_DISP;
		}
		iReadLen = MIN(iFileSize - ioffSet, 800);
		fd = fileOpen(stTransCtrl.stEnrichFile[iposition].szLocalfile, O_RDWR);
		if( fd<0 ) 
		{
			lcdDisplay(0, 4, DISP_MEDIACY|DISP_CFONT, "���ļ�����");
			DispFileErrInfo();
			return (E_MEM_ERR);
		}
		iRet = fileSeek(fd, (int)ioffSet, SEEK_SET);
		if( iRet<0 ) 
		{
			lcdDisplay(0, 4, DISP_MEDIACY|DISP_CFONT, "�鿴�ļ�����");
			DispFileErrInfo();
			fileClose(fd);
			return (E_MEM_ERR);
		} 
		memset(szReadbuf,0,sizeof(szReadbuf));
		iRet =fileRead(fd,szReadbuf,iReadLen);
		if( iRet<0 ) 
		{
			lcdDisplay(0, 4, DISP_MEDIACY|DISP_CFONT, "���ļ�����");
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
		memcpy(&sSendPacketBuff[iCnt],stPosParam.sM1TellNo,4);//�ն˱��
		iCnt+=4;
		memset(&sSendPacketBuff[iCnt],' ',30);
		memcpy(&sSendPacketBuff[iCnt],stTransCtrl.stEnrichFile[iposition].szRemotefile,strlen(stTransCtrl.stEnrichFile[iposition].szRemotefile));//�ն˱��
	    iCnt+=30;	
		sprintf(&sSendPacketBuff[iCnt], "%016d", iFileSize);
		iCnt+=16;
		if(ioffSet+iReadLen<iFileSize)//����
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
		sprintf(&sSendPacketBuff[iCnt], "%016d", ioffSet);//ƫ����
		iCnt+=16;	
		sprintf(&sSendPacketBuff[iCnt], "%04d", iReadLen);//����
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
			//�ϴ�ʧ�� �˳�
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "�������");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "������");
			FailBeep();
			kbGetKey();
			return NO_DISP;
		}
		if(sRecvPacketBuff[42]=='2')//�ش�
		{
             iuploadCnt++;
		     continue;
		}
		else if(sRecvPacketBuff[42]=='1')
		{
			if(ioffSet+iReadLen<iFileSize)//����
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
				{//û���ϴ�������
					lcdClrLine(0,7);
					lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "�����ļ�");
					lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "�ϴ��ɹ�");
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
					lcdDisplay(0, 4, DISP_MEDIACY|DISP_CFONT, "�����д���");
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

//ͨ�������ϴ�����
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
		DisplayMainPosOperFailed("���ڴ�ʧ��", 5);
		return NO_DISP;
	}
	
	iRet = Rs232ResetPurge(RS232_TX_PURGE|RS232_RX_PURGE);
	if(iRet != OK)
	{
		DisplayMainPosOperFailed("���ڻ�������", 5);
		return NO_DISP;
	}

	sprintf(buf, "%04d", usSendPacketLen);
	memmove(&sSendPacketBuff[4], sSendPacketBuff, usSendPacketLen);
	memcpy(sSendPacketBuff,buf,4);
	usSendPacketLen += 4;

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

	lcdClrLine(0,7);
	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "���ڴ�������");

	memset(sRecvPacketBuff,0,sizeof(sRecvPacketBuff));
	usRecvPacketLen = 0;

	/*iRet = MainPosRS232Rxd_(sRecvPacketBuff, MAX_PACKET_LEN, 60, (unsigned int*)&usRecvPacketLen);
	if (iRet != ERR_RS232_OK)
	{
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "�������ݴ� %d", iRet);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "��������");
		PubWaitKey(5);
		return NO_DISP;
	}
	*/

	PubChar2Long(sRecvPacketBuff, 2, &ulTemp);
	memmove(sRecvPacketBuff, &sRecvPacketBuff[2], (uint)ulTemp);
	usRecvPacketLen = (uint32_t)ulTemp;

	return OK;	
}


//---�����ϴ�---------------
uint8_t M1_Data_UpLoad_COM(void)
{
	FTP_PARA stFtpPara;
	 int iRet,fd;
	 int iposition;
	 uint8_t Ret;
	 int itotalFile;
	 //int iCurCntFile=0;
	 int ioffSet=0;//ƫ����
	 int iFileSize=0;//�ļ���С
	 int iCnt;   //
	 int iMsgSN=1; //���ݱ����к�
	 int iReadLen;
	 uint8_t szReadbuf[2048];
	 uint8_t utemp;
	 int  iuploadCnt=0;
	 INT8  serialNo[10];

	memset(&stFtpPara, 0, sizeof(FTP_PARA));
	
	itotalFile=GetFtpFiletotal();
	iRet=GetFtpFilePosition();
	if(iRet<0)
	{//û���ϴ�������
		lcdClrLine(0,7);
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "�������ϴ�");
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "���Ƚ���");
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
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "�������");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "������");
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
		memcpy(&sSendPacketBuff[iCnt],serialNo,8);//�ն˱��
		iCnt+=8;

		memset(&sSendPacketBuff[iCnt],' ',30);
		memcpy(&sSendPacketBuff[iCnt],stTransCtrl.stEnrichFile[iposition].szRemotefile,strlen(stTransCtrl.stEnrichFile[iposition].szRemotefile));
	    iCnt+=30;	
		sprintf(&sSendPacketBuff[iCnt], "%016d", iFileSize);
		iCnt+=16;
		if(ioffSet+iReadLen<iFileSize)//����
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
		sprintf(&sSendPacketBuff[iCnt], "%016d", ioffSet);//ƫ����
		iCnt+=16;	
		sprintf(&sSendPacketBuff[iCnt], "%04d", iReadLen);//����
		iCnt+=4;
		memcpy(&sSendPacketBuff[iCnt],szReadbuf,iReadLen);
		iCnt+=iReadLen;
		utemp=PubCalcLRC(sSendPacketBuff,iCnt,0);
		sprintf(&sSendPacketBuff[iCnt],"%03d",utemp);
		iCnt+=3;
		usSendPacketLen=iCnt;
		
		//���͡���������
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
               //�ϴ�ʧ�� �˳�
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "�������");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "������");
			FailBeep();
			kbGetKey();
			return NO_DISP;
		}
		
		if(sRecvPacketBuff[42]=='2')//�ش�
		{
             iuploadCnt++;
		     continue;
		}
		else if(sRecvPacketBuff[42]=='1')
		{
		  if(ioffSet+iReadLen<iFileSize)//����
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
				{//û���ϴ�������
					lcdClrLine(0,7);
					lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "�����ļ�");
					lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "�ϴ��ɹ�");
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    �����ϴ�    ", "  QUERY TRANS   ");
	//ͨ��FTP�����ļ�
	memset(&stFtpPara, 0, sizeof(FTP_PARA));
	
	strcpy(stFtpPara.szHostip, stPosParam.szFtpServIP);
	strcpy(stFtpPara.szPort, stPosParam.szFtpServPort);
	strcpy(stFtpPara.szUser, stPosParam.szFtpUserName);
	strcpy(stFtpPara.szPwd, stPosParam.szFtpPwd);
	itotalFile=GetFtpFiletotal();

	iRet=GetFtpFilePosition();
	if(iRet<0)
	{//û���ϴ�������
		lcdClrLine(0,7);
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "�������ϴ�");
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "���Ƚ���");
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
		lcdDisplay(0, 3, DISP_CFONT | DISP_MEDIACY, "ftp�������ó���");
		PubBeepErr();
		PubWaitKey(5);
		return 1; 
	}
	// ����
	iRet = PreDial();
	if( iRet!=0 )
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "����ʧ��");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "DialErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}

	// ��¼ftp
	iRet = NewFtpLogon(&stFtpPara);
	if (iRet != 0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "FTP��¼ʧ��");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "LogErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}
    while(1)
    {
		//�����ļ�
		iRet=GetFtpFilePosition();
		if(iRet<0)
		{//û���ϴ������
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
		  lcdDisplay(0, 3, DISP_CFONT | DISP_MEDIACY, "ftp�������ó���");
		  PubBeepErr();
		  PubWaitKey(5);
		  Ret=NO_DISP;
		 	break;

		}
		if(fileExist(stFtpPara.szLocalfile) < 0)
		{
				lcdClrLine(2, 7);
				lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "%s", stFtpPara.szLocalfile);
				lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "�ļ�������");
				PubBeepErr();
				PubWaitKey(5);
				 Ret=NO_DISP;
				break;
		}
		//itotalFile;
		iCurCntFile++;
		lcdClrLine(2,7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "�ļ������ϴ���");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "�ܼ��ļ�:%d",itotalFile);
		lcdDisplay(0, 6, DISP_CFONT | DISP_MEDIACY, "��ǰ�ļ�:%d",iCurCntFile);

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
	
	// �ǳ�������
	NewFtpQuit();
	return Ret;
}

//--���������ڲ���----
uint8_t M1_SaleDate_Upload(void)
{
	int iRet;
	UINT8  buf[50];
	UINT8  szDate[4+1];//yyyymmdd ����ʱ�� BCD
	INT32  iTotalNum,lCurIndex;
	INT32  iTranNum, enrichNum, saleNum;
	INT32  iTotalTranAmt,iTotalSalAmt;
	INT32  iCnt,i;
	UINT8  szFileName[30];
	UINT8  sztime[20];//����ʱ�� ASC
	UINT8  szEnrichdata[500];
	int    iEnrichDatalen;
	UINT8  uRet,Ret;
	FTP_PARA stFtpPara;
	int iposition=0;
	int ioffSet=0;//ƫ����
	int iFileSize=0;//�ļ���С
	int iMsgSN=1; //���ݱ����к�
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
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    �����ϴ�    ", "  QUERY TRANS   ");	
	DispMulLanguageString(0, 2, DISP_CFONT, "��������������", "YYYY");
	lcdDisplay(0, 4, DISP_CFONT, "YYYYMMDD");
	memset(buf, 0, sizeof(buf));
	lcdGoto(0, 6);
	iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 8, 8, (stPosParam.ucOprtLimitTime*1000), (char *)buf);
   if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
   {
       lcdClrLine(2,7);
	   lcdDisplay(0, 4, DISP_CFONT, "�������ڲ���ȡ��");
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
		   DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "���ڸ�ʽ����", "DATE FORMAT ERR");
		   kbGetKeyMs(2000);
		   return NO_DISP;
	   }
	   PubAsc2Bcd(buf,8,szDate);//4                    //4 λbcd
   }
   else
   {
   		FailBeep();
		lcdClrLine(2,7);
		lcdDisplay(0, 4, DISP_CFONT, "�������ڲ���ȡ��");
		kbGetKeyMs(3*1000);
		return NO_DISP;

   }
   //----------------------------------------
	memset(sztime,0,sizeof(sztime));
	memset(szFileName,0,sizeof(szFileName));
	GetEnrichFileName(szFileName);
	memcpy(sztime,szFileName+5,14);
	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ���׽���    ", "  QUERY TRANS   ");
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
	   lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "���������ϴ�");
	   lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "�ٽ���");
	   PubWaitKey(10);
	   return NO_DISP;
	}
	iRet=FindEnrichStoreFilePosition();
	if(iRet<0)
	{
	   lcdCls();
	   PubDisplayTitle(TRUE,"�������ײ�ѯ");
	   PubDispString("�޽���", 3|DISP_LINE_CENTER);
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
			//---���----
			memset(szEnrichdata,0,sizeof(szEnrichdata));
			PackFTP(&gstYktEnrichLog,szEnrichdata,&iEnrichDatalen,sztime);
			uRet= WriteEnrichFtpFile(szEnrichdata,iEnrichDatalen, sztime);//����sztime�Ǵ洢��POS������ļ�����
			if(uRet==OK)
			{
				//������־
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
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "û�н��������");
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
    	PubDisplayTitle(TRUE,"�������׽���");
   		PubDispString("�޽���", 3|DISP_LINE_CENTER);
   		PubWaitKey(10);
    	return NO_DISP;
	}

	SaveM1EnrichLastTotal();
	PrtM1EnrichTotal(1);
	lcdClrLine(2,7);
	lcdDisplay(0, 2, DISP_CFONT, "���ױ���: %d", iTranNum);
	lcdDisplay(0, 4, DISP_CFONT, "��ֵ���: %d.%02d", iTotalTranAmt/100,iTotalTranAmt%100);
	lcdDisplay(0, 6, DISP_CFONT, "���ѽ��: %d.%02d", iTotalSalAmt/100,iTotalSalAmt%100);
	kbGetKeyMs(3*1000);
	kbFlush();

	lcdCls();
	DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    �����ϴ�    ", "  QUERY TRANS   ");
	//ͨ��FTP�����ļ�
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
			  lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "�������");
			  lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "������");
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
		memcpy(&sSendPacketBuff[iCnt],stPosParam.sM1TellNo,4);//�ն˱��
		iCnt+=4;
		memset(&sSendPacketBuff[iCnt],' ',30);
		memcpy(&sSendPacketBuff[iCnt],stTransCtrl.stEnrichFile[iposition].szRemotefile,strlen(stTransCtrl.stEnrichFile[iposition].szRemotefile));//�ն˱��
	    iCnt+=30;	
		sprintf(&sSendPacketBuff[iCnt], "%016d", iFileSize);
		iCnt+=16;
		if(ioffSet+iReadLen<iFileSize)//����
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
		sprintf(&sSendPacketBuff[iCnt], "%016d", ioffSet);//ƫ����
		iCnt+=16;	
		sprintf(&sSendPacketBuff[iCnt], "%04d", iReadLen);//����
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
			//�ϴ�ʧ�� �˳�
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "�������");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "������");
			FailBeep();
			kbGetKey();
			return NO_DISP;
		}
		if(sRecvPacketBuff[42]=='2')//�ش�
		{
             iuploadCnt++;
		     continue;
		}
		else if(sRecvPacketBuff[42]=='1')
		{
		  	if(ioffSet+iReadLen<iFileSize)//����
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
				lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "�����ļ�");
				lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "�ϴ��ɹ�");
				OkBeep();
				PubWaitKey(3);
				return NO_DISP;
			}
		}
	}
	return 0;
}

//--���������ڲ���-----
uint8_t M1_SettDate_Upload(void)
{
	int iRet;
	UINT8  buf[50];
	UINT8  szDate[4+1];//yyyymmdd ����ʱ�� BCD
	INT32  iTotalNum,lCurIndex;
	INT32  iTranNum, enrichNum, saleNum;
	INT32  iTotalTranAmt,iTotalSalAmt;
	INT32  iCnt,i;
	UINT8  szFileName[30];
	UINT8  sztime[20];//����ʱ�� ASC
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
	int ioffSet=0;//ƫ����
	int iFileSize=0;//�ļ���С
	//int iCnt; //
	int iMsgSN=1; //���ݱ����к�
	int iReadLen;
	uint8_t szReadbuf[2048];
	uint8_t utemp;
	int  iuploadCnt=0;
	//FTP_PARA stFtpPara;
		 int fd;
	
			lcdCls();
			DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    �����ϴ�    ", "  QUERY TRANS   ");	
			// DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "�ն˲�������	 ", "	SETUP DATE	 ");
			DispMulLanguageString(0, 2, DISP_CFONT, "�������������", "YYYY");
			lcdDisplay(0, 4, DISP_CFONT, "YYYYMMDD");
			memset(buf, 0, sizeof(buf));
			lcdGoto(0, 6);
			iRet = kbGetString(KB_EN_NUM+KB_EN_BIGFONT+KB_EN_REVDISP, 8, 8, 
			  (stPosParam.ucOprtLimitTime*1000), (char *)buf);
		   if( iRet==KB_CANCEL || iRet==KB_TIMEOUT )
		   {
			   lcdClrLine(2,7);
			   lcdDisplay(0, 4, DISP_CFONT, "�������ڲ���ȡ��");
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
				   DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "���ڸ�ʽ����", "DATE FORMAT ERR");
				   kbGetKeyMs(2000);
				   return NO_DISP;
			   }
			   PubAsc2Bcd(buf,8,szDate);//4 				   //4 λbcd
			  
		   }
		   else
		   {
				FailBeep();
				lcdClrLine(2,7);
				lcdDisplay(0, 4, DISP_CFONT, "�������ڲ���ȡ��");
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
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    ���׽���    ", "  QUERY TRANS   ");
		kbFlush();
		// PubDisplayTitle(TRUE,"������ϸ��ѯ");
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
		   lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "���������ϴ�");
		   lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "�ٽ���");
		   PubWaitKey(10);
		   return NO_DISP;
		}
		 iRet=FindEnrichStoreFilePosition();
		 if(iRet<0)
		 {
		   lcdCls();
		   PubDisplayTitle(TRUE,"�������ײ�ѯ");
		   PubDispString("�޽���", 3|DISP_LINE_CENTER);
		   PubWaitKey(10);
		   return NO_DISP;
	
		 }
		for(i=0;i<iTotalNum;i++)
		{
			ReadYtkEnrichLog(&iCnt);
			if(memcmp(gstYktEnrichLog.sLogDateTime,szDate,4)!=0)
		   // if(gstYktEnrichLog.uiStatus==0x01)//������
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
				//---���----
				memset(szEnrichdata,0,sizeof(szEnrichdata));
				PackFTP(&gstYktEnrichLog,szEnrichdata,&iEnrichDatalen,sztime);
				uRet= WriteEnrichFtpFile(szEnrichdata,iEnrichDatalen, sztime);//����sztime�Ǵ洢��POS������ļ�����
				if(uRet==OK)
				{
				  //������־
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
				lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "û�н��������");
				//	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "�ٽ���");
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
			lcdDisplay(0, 2, DISP_CFONT, "���ױ���: %d", iTranNum);
			lcdDisplay(0, 4, DISP_CFONT, "��ֵ���: %d.%02d", iTotalTranAmt/100,iTotalTranAmt%100);
			lcdDisplay(0, 6, DISP_CFONT, "���ѽ��: %d.%02d", iTotalSalAmt/100,iTotalSalAmt%100);
			kbGetKeyMs(3*1000);
			kbFlush();
	
			lcdCls();
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "    �����ϴ�    ", "  QUERY TRANS   ");
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
				  lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "�������");
				  lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "������");
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
			memcpy(&sSendPacketBuff[iCnt],stPosParam.sM1TellNo,4);//�ն˱��
			iCnt+=4;
			memset(&sSendPacketBuff[iCnt],' ',30);
			memcpy(&sSendPacketBuff[iCnt],stTransCtrl.stEnrichFile[iposition].szRemotefile,strlen(stTransCtrl.stEnrichFile[iposition].szRemotefile));//�ն˱��
			iCnt+=30;	
			sprintf(&sSendPacketBuff[iCnt], "%016d", iFileSize);
			iCnt+=16;
			if(ioffSet+iReadLen<iFileSize)//����
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
			sprintf(&sSendPacketBuff[iCnt], "%016d", ioffSet);//ƫ����
			iCnt+=16;	
			sprintf(&sSendPacketBuff[iCnt], "%04d", iReadLen);//����
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
				   //�ϴ�ʧ�� �˳�
				//   NacTxd("3333",4);
				   lcdClrLine(2,7);
				  lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "�������");
				  lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "������");
				   FailBeep();
				  kbGetKey();
				  return NO_DISP;
				  // return NO_DISP;
		
			}
			if(sRecvPacketBuff[42]=='2')//�ش�
			{
			// NacTxd("4444",4);
				 iuploadCnt++;
				 continue;
			}
			else if(sRecvPacketBuff[42]=='1')
			{
			  if(ioffSet+iReadLen<iFileSize)//����
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
		
		
				  
				 lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "�����ļ�");
				  lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "�ϴ��ɹ�");
				  OkBeep();
				  PubWaitKey(3);
				  return NO_DISP;
			  }
		
			}
			//ioffSet+=iReadLen;
			//iMsgSN++;
		}
		
		

		//ͨ��FTP�����ļ�
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
			   prnPrintf("�ļ���:%s\n",stTransCtrl.stEnrichFile[i].szRemotefile);
			   prnPrintf("��ֵ����:%d\n",stTransCtrl.stEnrichFile[i].iRmbM1EnrichNum);
			   prnPrintf("��ֵ���:%d.%02d\n",stTransCtrl.stEnrichFile[i].iRmbM1EnrichAmt/100,stTransCtrl.stEnrichFile[i].iRmbM1EnrichAmt%100);
			   prnPrintf("���ѱ���:%d\n",stTransCtrl.stEnrichFile[i].iRmbM1SaleNum);
			   prnPrintf("���ѽ��:%d.%02d\n",stTransCtrl.stEnrichFile[i].iRmbM1SaleAmt/100,stTransCtrl.stEnrichFile[i].iRmbM1SaleAmt%100);
              if(stTransCtrl.stEnrichFile[i].u8Settlemode==1)
				{
					prnPrintf("���㷽ʽ:��ͨ����\n");

				}
				else if(stTransCtrl.stEnrichFile[i].u8Settlemode==2)
				{
					prnPrintf("���㷽ʽ:���������ڲ���\n");
					prnPrintf("�������ڲ���:%s\n",stTransCtrl.stEnrichFile[i].szDate);
				}
				else if(stTransCtrl.stEnrichFile[i].u8Settlemode==3)
				{
					prnPrintf("���㷽ʽ:���������ڲ���\n");
					prnPrintf("�������ڲ���:%s\n",stTransCtrl.stEnrichFile[i].szDate);

				}
				prnPrintf("\n");
		   }
			   
	  }
	  if(j==0)
	  {
       lcdCls();
	   lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "û����Ҫ�ϴ���");
	   lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "�����ļ�");
	   kbGetKeyMs(3*1000);
	   return 0;

	  }
	  prnPrintf("\n\n\n\n");
	  
		   PrintData();
		  return 0;


}


//---���³�ֵ���---------------
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
		  if(gstYktEnrichLog.uiStatus==0x01)//������
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
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "���ͬ������");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "������");
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

		sprintf(&sSendPacketBuff[iCnt],"00",2);//����
		iCnt+=2;

		//PubAsc2Bcd(stPosParam.sM1TellNo,8,&sSendPacketBuff[iCnt]);//�ն˱��
		sprintf(&sSendPacketBuff[iCnt],stPosParam.sM1TellNo,8);//�ն˱��
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
            //�ϴ�ʧ�� �˳�
//NacTxd("3333",4);
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "���ͬ������");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "����....");
			FailBeep();
			kbGetKey();
			continue;
		}

		//memset(szAmount, 0, 16);
		memset(buf, 0, sizeof(buf));
		memcpy(buf,&sRecvPacketBuff[18],10);
//NacTxd(buf,10);		
		lTmpInt = atol((char *)buf);
		
		//prnInit();//��ӡ��ʼ��
		//prnPrintf("%10d",lTmpInt);//��
		//stPosParam.maxAgentAmt=lTmpInt;
		//prnPrintf("%10d",lTmpInt);//��
		//prnStart();//��ӡ��ʼ

		SaveAppParam();
		
		return OK;

	}

}


//---���³�ֵ���---------------
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
		  if(gstYktEnrichLog.uiStatus==0x01)//������
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
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "��¼ʧ��");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "������");
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

		memcpy(&sSendPacketBuff[iCnt],"00",2);//����
		iCnt+=2;

		sprintf(&sSendPacketBuff[iCnt],stPosParam.sM1TellNo,8);//�ն˱��
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
            //�ϴ�ʧ�� �˳�
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "��¼��֤ʧ��");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "����1....");
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
		{//���ܴ���
		    lcdClrLine(2, 7);
		    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "���ܴ���");
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		}
		
		PubBcd2Asc(szSendMsgData,ioutlen + ioutlen,&key[0]);
//NacTxd("1234",4);

		if(memcmp(&sRecvPacketBuff[18],key,ioutlen + ioutlen)!=0)
		{
            //�ϴ�ʧ�� �˳�
			lcdClrLine(2,7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "��¼��֤ʧ��");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "����2....");
			FailBeep();
			kbGetKey();
			continue;
		}


		return OK;

	}

}

//ͨ��1����ɢ���Ȧ����Կ��
//��Ȧ����Կ��ͨ��Ȧ���ʼ���õ��Ĳ������ݽ��м�������8�ֽڵĹ�����ԿSK��
//Ȼ�����ù�����ԿSK  ��Ȧ�����ݰ�MAC�������ɳ�MAC2,
//�ٰ�MAC2ƴ���Ͻ�������ʱ�䣬��Ϊָ�����Ȧ��
UINT8 GetMAC(UINT8 initbuf[], UINT8 mac_data[],int mlen, UINT8 mac_data_1[], int mlen_1)
{
	uint8_t key[16];//����Կ
	uint8_t l_data[8];
	uint8_t r_data[8];
	uint8_t data[16];//Ȧ����Կ
	uint8 sk_data[8];//������Կ
	int i, l, k,sk, ret;
	int iSendLen, iRecvLen;
	UINT8 szSendData[300], szRecvData[300];
	UINT8 mac_init[8];
	APDU_SEND stApduSend;
	APDU_RESP stApduResp;

	uint8 result[8];
	uint8 temp[8];

	/////һ����ɢ����Ȧ����Կ
	//memcpy(l_data,gstYktEnrichLog.sCardSerialNo,8);
	memcpy(l_data,"\x00\x00\x00\x00",4);
	memcpy(l_data+4,stTemp.sCardUid,4);
//NacTxd(l_data,20);
	for(i=0;i<8;i++){
		r_data[i]=~l_data[i];
	}
	memcpy(key,gstYktEnrichLog.sIsamKey,16);//����Կ
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
	/////��Ȧ����Կ��ͨ��Ȧ���ʼ���õ��Ĳ������ݽ��м�������8�ֽڵĹ�����ԿSK��
	memset(sk_data, 0x00, sizeof(sk_data));
	memcpy(sk_data,initbuf+8,4);//α�����
	memcpy(sk_data+4,initbuf+4,2);//�������
	memcpy(sk_data+6,"\x80\x00",2);
//NacTxd("1006",4);
//NacTxd(sk_data,10);
	ret=TDEA(sk_data,sk_data,data,16,TDEA_ENCRYPT);
	if( ret!=PED_RET_OK )
			return(1);
//NacTxd(sk_data,20);

	/////�ù�����Կ��Ȧ���ʼ���õ��Ĳ�����������MAC2
	/*//ȡ��������õ�8    �ֽڵĳ�ʼֵ
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
		PubDispString("ȡ�������", 3|DISP_LINE_CENTER);
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
//NacTxd(initbuf,30);//Ȧ���ʼ�����
//NacTxd(mac_data_1,32);
//NacTxd(mac_data,32);

	//��֤MAC1
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

	//����MAC2
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
    //---------��ʼ��ָ����ر���������-------
	uint8_t szSendMsgData[500];
	uint8_t szRevMsgData[500];
	int iReportlen,ioutlen;
	int iRet,iCnt;
	uint8_t uRet,ucRet;
    uint8_t sMsgBodyKey[16];

	int 	   iSendLen, iRecvLen;
	UINT8 szSendData[300], szRecvData[300];
	int macSendLen, macSendLen_1,macRecvLen;
	UINT8 macSendData[300], macRecvData[300];//����MAC2  ���õ�����
	UINT8 macSendData_1[300],mac1[4];//��֤MAC1  ��������
	UINT8 initbuf[300];//Ȧ���ʼ�����ؽ��
	int lValue;
	APDU_SEND stApduSend;
	APDU_RESP stApduResp;
	UINT8 result[16];
	uint8_t time[11];

	////////////////////������֤//////////////////
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
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "����У���");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	  // return ERR_CODE_VERIFY_PIN;
	}
//NacTxd("1000", 4);
//NacTxd(szRecvData,iRecvLen);

	////////////////////Ȧ���ʼ��//////////////////
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
	    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "Ȧ���ʼ����");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		//lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "����ʧ��");
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

	////////////////////Ȧ��//////////////////
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

	//��֤����Կ
	ucRet =EnrichAuthenticate();
	if( ucRet!=OK )
	{
		return NO_DISP; 
	}
//NacTxd("1005",4);
//NacTxd(gstYktEnrichLog.sIsamKey,16);

	//��֤MAC1  ������
	macSendLen_1=0;
	memset(macSendData_1,0x00, sizeof(macSendData_1));
	memcpy(macSendData_1,initbuf,4);//�ɽ��
	macSendLen_1+=4;
	lValue = PubBcd2Long(PosCom.stTrans.sAmount,6,NULL); //���
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
	
	
	//��ȡMAC2
	macSendLen=0;
	memset(macSendData,0x00, sizeof(szSendData));
	lValue = PubBcd2Long(PosCom.stTrans.sAmount,6,NULL); //���
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
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "MAC����ȷ");
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
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "Ȧ��ָ���");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		/*	
		iRet=Get_tran_Prove(gstYktEnrichLog.ulPOSTraceNo+1,gstYktEnrichLog.sTac);
			if(iRet!=OK)//�_��
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
	{//����Ƿ�Ҫ�_��
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
	//----������Ʋ���-------------------------
	gstYktEnrichLog.lEnrichTraceNo=stPosParam.lEnrichTraceNo;
	gstYktEnrichLog.lEnrichCumulativeBalance=stPosParam.lEnrichCumulativeBalance+gstYktEnrichLog.ulTransAmt;

	//�ն˱����¼
	ucRet = SaveM1EnrichLog(0);
	if( ucRet!=OK )
	{
		lcdClrLine(2, 7);
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "�����ļ�����ʧ��", "FILE ERROR");
		ErrorBeep();
		kbGetKeyMs(2000);
	}
	stPosParam.lEnrichTraceNo++;
	stPosParam.lEnrichCumulativeBalance=stPosParam.lEnrichCumulativeBalance+gstYktEnrichLog.ulTransAmt;
	SaveAppParam();

	//��ӡ
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
	{//���ܴ���
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "���ܴ���");
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
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "δ�ҵ���������");
		//lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "����Χ");
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	else if(memcmp(&sRecvPacketBuff[7],"\x40\x12",2)==0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��������");
	//	lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "��������");
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 

	}
	else
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��������");
		//	lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "��������");
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
		PubDisplayTitle(TRUE, "��  ʾ");
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "���ڿ���ʱ");
		lcdDisplay(0, 6, DISP_CFONT, "���㽻������");
		ErrorBeep();
		iRet = kbGetString(KB_EN_BIGFONT+KB_EN_CHAR+KB_EN_REVDISP, 0, 1, (3000), buf);
		if( iRet==KB_CANCEL || iRet==KEY_ENTER|| iRet==KB_TIMEOUT  )
		{}
	}

	PubDisplayTitle(TRUE, "��  ��");
	memset(&gstYktEnrichLog,0,sizeof(YKTENRICH_LOG));
	strcpy((char *)gstYktEnrichLog.sTellNo, (char *)stTransCtrl.szNowTellerNo);//sTellNo

	iRet =ResetPsam();

	memset(gstYktEnrichLog.sPosSN,0,sizeof(gstYktEnrichLog.sPosSN));
	memcpy(gstYktEnrichLog.sPosSN,stPosParam.sM1TellNo,8);//�ն˺�
	
	memset(gstYktEnrichLog.pointofsaleNo,0,sizeof(gstYktEnrichLog.pointofsaleNo));
	memcpy(gstYktEnrichLog.pointofsaleNo,stPosParam.pointofsaleNo,4);//��·/ ��ֵ���

	memset(gstYktEnrichLog.sCompanyNo,0,sizeof(gstYktEnrichLog.sCompanyNo));
	memcpy(gstYktEnrichLog.sCompanyNo,stPosParam.szCompanyNo,strlen(stPosParam.szCompanyNo));//��˾��

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
	//	//-------------�Ѿ�������ֵ�޶�----
	//	lcdClrLine(2, 7);
	//	lcdDisplay(0, 4, DISP_CFONT, "������ֵ���޶�");
	//	ErrorBeep();
	//	sysDelayMs(3000);
	//	return NO_DISP;
	//}

	memset(szTemp,0,sizeof(szTemp));
	ConvBcdAmount(PosCom.stTrans.sAmount,(uint8_t*)szTemp);
	
	lcdClrLine(2, 7);
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "���:%sԪ",szTemp); 
	lcdDisplay(0, 6, DISP_CFONT|DISP_MEDIACY, "��ˢ������"); 
	iOldTime = sysGetTimerCount();
	mifClose();
	mifOpen();
	while(1)
    {
       iNowTime= sysGetTimerCount();
	   if( (iNowTime-iOldTime) >= (UINT32)(stPosParam.ucOprtLimitTime*1000) )	// ��鶨ʱ��
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
            // �ڸ�Ӧ���ڲ���ָ���������͵Ŀ�Ƭ�������ؿ�Ƭ�����кŵ������Ϣ
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

	if(memcmp(&sCardInfo[3],"\x28",1)==0 || memcmp(&sCardInfo[3],"\x20",1)==0)// CPU����ֵ
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
	
		ucRet = CPU_ReadEnrichData(&stInParam);// ����CPU���ļ�
		if(ucRet !=OK)
		{
		   return NO_DISP;
		}

		//gstYktEnrichLog.ulMaxBalaceAmt= stPosParam.ulMaxBalaceAmt*100;
		//if(gstYktEnrichLog.ulMaxBalaceAmt<gstYktEnrichLog.ulOldBalaceAmt+gstYktEnrichLog.ulTransAmt)
		//{
		//	//-------------�Ѿ�������ֵ���----
		//	lcdClrLine(2, 7);
		//	lcdDisplay(0, 4, DISP_CFONT, "������ֵ�޶�");
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
	else if(memcmp(&sCardInfo[3],"\x08",1)==0)//M1����ֵ
	{
		ultemp=stTemp.sCardUid[0]+stTemp.sCardUid[1]*256+stTemp.sCardUid[2]*256*256+stTemp.sCardUid[3]*256*256*256;
		memset(gstYktEnrichLog.sCSN,0,sizeof(gstYktEnrichLog.sCSN));
		sprintf((INT8 *)gstYktEnrichLog.sCSN, "%010u", ultemp);
		
		memcpy(sSect1KeyA,"\xA0\xA1\xA2\xA3\xA4\xA5",6);
		memcpy(&stTemp.sKeyBList ,sSect1KeyA,6);
		ucRet =M1_ReadSector(0, sSect1KeyA,MIF_KEYA, sSectorData,stTemp.sCardUid);//����48�ֽ�
		if( ucRet!=OK )
		{
		  return E_TRANS_FAIL;
		}
		memcpy(stTemp.sCardDataList,sSectorData,48);

		memcpy(sSect1KeyA, stTemp.sCardUid, 4);
		memcpy(sSect1KeyA+4, stTemp.sCardUid, 2);
		memset(sSectorData, 0, sizeof(sSectorData));
		memcpy(&stTemp.sKeyBList[1*6] ,sSect1KeyA,6);

		ucRet =M1_ReadSector(1, sSect1KeyA,MIF_KEYA, sSectorData,stTemp.sCardUid);//����48�ֽ�
		if(ucRet !=0)
		{
			return NO_DISP;
		}
		memcpy(stTemp.sCardDataList[4],sSectorData,48);

		//������ʱ����
		memcpy(sCityCode,&sSectorData[0],2);//
		memcpy(sTrace,&sSectorData[4],4);// liu shui
		memcpy(sMac,&sSectorData[8],4);//ren zhen ma
		PubBcd2Asc(&sSectorData[0],8,gstYktEnrichLog.sCardSerialNo);
		gstYktEnrichLog.uCardtype=sSectorData[13];//������

		//�жϺ�����
		PubBcd2Asc(&sSectorData[4],4,blkTmp);

		iRet = ChkIfBlkCard(blkTmp);
		if(iRet !=0)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 2, DISP_CFONT, "��������");
			lcdDisplay(0, 4, DISP_CFONT, "����ϵ����Ա!");
			ErrorBeep();
			sysDelayMs(3000);
			return NO_DISP;
		}
		//sSectorOper	
		//	memset(sAllKeyB,0,sizeof(sAllKeyB));
		//���������Կ
		iRet= GetSecPW(stTemp.sCardUid, sTrace, sMac, sCityCode, "\x00\x01\x10\x03\x03\x03\x06\x11\x12\x13\x14\x15\x16\x17\x18\x19","1111111110000000", 1, stTemp.sKeyBList);
		if(iRet !=0)
		{
			return NO_DISP;
		}
//NacTxd("1008", 4);
//NacTxd(stTemp.sKeyBList, 6*16);
		iRet = GetAllDataFromM1Card();//��ȡ������������
		if(iRet != 0)
		{
			return iRet;
		}
		iRet = M1_IsCardInfoLegal();//�жϿ������ݵĺϷ���
		if(iRet != 0)
		{
			return iRet;
		}
		memset(stTemp.sCardDataListSub, 0, sizeof(stTemp.sCardDataListSub));
		memcpy(stTemp.sCardDataListSub, stTemp.sCardDataList, 64*16);
		
		//��Ǯ������BLOCK9,
		//memcpy(gstYktEnrichLog.uOldBalaceAmt,sCardDataList[2*4+1],4);
		gstYktEnrichLog.ulOldBalaceAmt=stTemp.sCardDataList[2*4+1][0]+stTemp.sCardDataList[2*4+1][1]*256
			+stTemp.sCardDataList[2*4+1][2]*256*256+stTemp.sCardDataList[2*4+1][3]*256*256*256;
		gstYktEnrichLog.ulMaxBalaceAmt= stPosParam.ulMaxBalaceAmt*100;

		////�жϣ���ֵ������ֵ����
		//if(gstYktEnrichLog.ulMaxBalaceAmt<gstYktEnrichLog.ulOldBalaceAmt+gstYktEnrichLog.ulTransAmt)
		//{
		//	//-------------�Ѿ�������ֵ���----
		//	lcdClrLine(2, 7);
		//	lcdDisplay(0, 4, DISP_CFONT, "������ֵ�޶�");
		//	ErrorBeep();
		//	sysDelayMs(3000);
		//	return NO_DISP;
		//}
		
		iRet=M1_RchAmtProcess();//��ֵ
		if(iRet!=0)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 4, DISP_CFONT, "��ֵʧ��");
			ErrorBeep();
			sysDelayMs(3000);
			return NO_DISP;
		}

		//�ٴλ�ȡ��������������֤�Ƿ��ֵ�ɹ�
		memset(sSectorData,0,sizeof(sSectorData));//stTemp.sKeyBList
		ucRet =M1_ReadSector(2, &stTemp.sKeyBList[6*2],MIF_KEYB, sSectorData,stTemp.sCardUid);//����48�ֽ�
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
			lcdDisplay(0, 4, DISP_CFONT, "��ֵʧ��");
			ErrorBeep();
			sysDelayMs(3000);
			return NO_DISP;
		}
	}

	gstYktEnrichLog.ulNowBalaceAmt=gstYktEnrichLog.ulTransAmt+gstYktEnrichLog.ulOldBalaceAmt;
	//----������Ʋ���-------------------------
	gstYktEnrichLog.lEnrichTraceNo=stPosParam.lEnrichTraceNo;
	gstYktEnrichLog.lEnrichCumulativeBalance=stPosParam.lEnrichCumulativeBalance+gstYktEnrichLog.ulTransAmt;
	gstYktEnrichLog.uMode=0;

	//�ն˱����¼
	ucRet = SaveM1EnrichLog(0);
	if( ucRet!=OK )
	{
		lcdClrLine(2, 7);
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "�����ļ�����ʧ��", "FILE ERROR");
		ErrorBeep();
		kbGetKeyMs(2000);
	}
	stPosParam.lEnrichTraceNo++;
	stPosParam.lEnrichCumulativeBalance=stPosParam.lEnrichCumulativeBalance+gstYktEnrichLog.ulTransAmt;
	//stPosParam.maxAgentAmt=stPosParam.maxAgentAmt-gstYktEnrichLog.ulTransAmt;
	stPosParam.lUnSettTraceNum++;

	SaveAppParam();

	//��ӡ
	PrtM1TranEnrichTicket(NORMAL);

//��ʾ��ֵ�ɹ�

//-----�ٶ�ȡ���ݡ����--------------------------------------------

  /*NacTxd("OK11",4);
 int i;
 for(i=0;i<=7;i++)
 	{
     ucRet =M1_ReadSector(i, &stTemp.sKeyBList[6*i],MIF_KEYB, sSectorData,stTemp.sCardUid);//����48�ֽ�
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
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "�����߿�");
	
	//PubWaitKey(0);
//	showDebugMessage(iCardInfoLen, sCardInfo, "read card succ!");

	return 0;
}


UINT8 CPU_SaleWriteData(pEtcardInParam_t  pInParam)
{
    //---------��ʼ��ָ����ر���������-------
	//uint8_t szSendMsgData[500];
	//uint8_t szRevMsgData[500];
	int iReportlen,ioutlen;
	int iRet;//,iCnt;
	uint8_t ucRet;//uRet,
    //uint8_t sMsgBodyKey[16];

	int 	   iSendLen, iRecvLen;
	UINT8 szSendData[300], szRecvData[300];
	//int  macSendLen_1,macRecvLen;//macSendLen,
	//UINT8 macSendData[300], macRecvData[300];//����MAC2  ���õ�����
	//UINT8 macSendData_1[300],mac1[4];//��֤MAC1  ��������
	UINT8 initbuf[300];//Ȧ���ʼ�����ؽ��
	int lValue;
	APDU_SEND stApduSend;
	APDU_RESP stApduResp;
	UINT8 terminal[6];
	UINT8 keyindex[1];
	uint8_t time[11];
	UINT8 sBlance[10];
	UINT8 city[4];

	/////////////////ѡ�񹫹�Ǯ��:\x00\xA4\x04\x00\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01
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
			lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��Ӧ��ѡ���");
			lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
			PubBeepErr();
			PubWaitKey(3);
			return NO_DISP; 
		}
	}

	/////////////////////---����ѯ///////////////////////////
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
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "����ѯ��");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}

	gstYktEnrichLog.ulMaxBalaceAmt= stPosParam.ulMaxBalaceAmt*100;

	if(gstYktEnrichLog.ulOldBalaceAmt<gstYktEnrichLog.ulTransAmt )
	{
		//-------------����----
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "����");
		ErrorBeep();
		sysDelayMs(3000);
		return NO_DISP;

	}

	//�жϣ�����������������
	if(gstYktEnrichLog.ulMaxBalaceAmt<gstYktEnrichLog.ulTransAmt)
	{
		//-------------�Ѿ���������ѽ��----
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "���������޶�");
		ErrorBeep();
		//lcdDisplay(0, 6, DISP_CFONT, "������!");
		sysDelayMs(3000);
		return NO_DISP;

	}

	/////////////////////���û���0015��ȡӦ�����к�: \x00\xB0\x95\x00\x1E/////////////////////
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
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��ȡӦ�����кŴ�");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	PubBcd2Asc(&szRecvData[12],8,gstYktEnrichLog.sCardSerialNo);//��Ӧ�����к� 
	//gstYktEnrichLog.uCardtype=szRecvData[15];
	memcpy(city,&szRecvData[2],2);
//NacTxd("111111111",10);
//NacTxd(city,10);
	/*
	if(memcmp(&city,"\x31\x31",2)!=0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "��ؿ����ۿ�ʧ��");
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
	    lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "�����ѿ��ļ���");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	// return ERR_CODE_USECARD_SELECT_ADF;
    }
	gstYktEnrichLog.uCardtype=szRecvData[0];

	////////////////////PSAM��ѡ���ļ�: \x00\xA4\x00\x00\x02\x3F\x00///////////////
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
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "MAC����");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X",  stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}

	////////////////////PSAM����ȡ�ն˱��: 00 B0 96 00 06//////////////////
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
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��ȡ�ն˱�Ŵ�");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X",  stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	memcpy(terminal, &stApduResp.DataOut, 6);

/*
	////////////////////PSAM����Կ���� 00 B0 97 00 01 //////////////////
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
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��ȡ��Կ������");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X",  stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	memcpy(keyindex, &stApduResp.DataOut, 1);
	*/

	/*ס������Կʱ�õ�
	////////////////////�û�����ȡ��ȫʶ����:\x80\xCA\x00\x00\x09//////////////////
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
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��ȡ��ȫʶ�����");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	  // return ERR_CODE_VERIFY_PIN;
	}
	*/
	
	////////////////////PSAM��ѡ���ļ�:\x00\xA4\x00\x00\x02\x10\x01///////////////
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
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "MAC����");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X",  stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	
	/*
	////////////PSAM����֤�û���ȫʶ����:80 CA 00 00 09+��ȫ��///////////////
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
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��֤����");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X",  stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP; 
	}
	*/
	////////////////////��ͨ���ѳ�ʼ��//////////////////
	//\x80\x50\x01\x02\x0B\x01+00000001�����ѽ��8λ��+������2���ص��ն˱�ţ�+0F
	PubAsc2Bcd(stPosParam.szPosId,12,pInParam->TerminalNum);
	iSendLen = 0;
	iRecvLen = 0;  
	//memset(szSendData, 0x00, sizeof(szSendData));
	//memset(szRecvData, 0x00, sizeof(szRecvData));
	memcpy(szSendData,"\x80\x50\x01\x02\x0B",5); //p1=01����ͨ����//p1=03�Ǹ������ѣ�PSAM����ʼ���н������ͱ�ʶȡֵҪ���һ��
	iSendLen = 5;
	memcpy(szSendData+iSendLen, "\x01", 1);//��Կ����PSAM��00B0970001ָ��
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
	   /* lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "���ѳ�ʼ����");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", szRecvData[iRecvLen-2], szRecvData[iRecvLen-1]);*/
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "����ʧ��");
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

	///////////////////////PSAM�����ѳ�ʼ��(����MAC1)///////////////////////////
	//\x80\x70\x00\x00\x24
	//����8���صĴ�22��ʼ8λ�����+����8���صĴ�8��ʼ4λ�������к�
	//8λ��ֵ���+06+����ʱ�䣨yyyyMMddHHmmss��
	//0100+����7���ص�Ӧ�����к�+4λ���б���+FF0000000000
	//��֤MAC1  ������
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
	memcpy(stApduSend.DataIn,initbuf + 11,4);//8λ�����
	ioutlen+=4;
	memcpy(stApduSend.DataIn+ioutlen,initbuf + 4,2);//4λ�������к�
	ioutlen+=2;
	lValue = PubBcd2Long(PosCom.stTrans.sAmount,6,NULL); //���
	PubLong2Char(lValue, 4, stApduSend.DataIn+ioutlen);
	ioutlen+=4;
	memcpy(stApduSend.DataIn+ioutlen,"\x06",1);//�������ͱ�ʶ06��ͨ���� 09��������
	ioutlen+=1;
	memcpy(stApduSend.DataIn+ioutlen,&time,7);//��������ʱ��
	ioutlen+=7;
	memcpy(stApduSend.DataIn+ioutlen,"\x01",1); //������Կ�汾�� 
	ioutlen+=1;
	//memcpy(stApduSend.DataIn+ioutlen,initbuf + 10,1);//������Կ�㷨��ʶ
	memcpy(stApduSend.DataIn+ioutlen,"\x00",1);
	ioutlen+=1;
	PubAsc2Bcd(gstYktEnrichLog.sCardSerialNo,16,stApduSend.DataIn+ioutlen);//��Ӧ�����к�
	ioutlen+=8;
	memcpy(stApduSend.DataIn+ioutlen,"\x75\x00",2);//���б���
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
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "MAC����");
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

	///////////////////////////����////////////////////////////////////
	//\x80\x54\x01\x00\x0F
	//��һ�����صĴ�0��ʼ8λ���׺�+����ʱ�䣨һ��Ҫ���ϲ��е�ʱ��ֵһ����
	//+����9���صĴ�8��ʼ8λMAC1
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
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "����ָ���");
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
	{//����Ƿ�Ҫ�_��
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
	//----������Ʋ���-------------------------
	gstYktEnrichLog.lEnrichTraceNo=stPosParam.lEnrichTraceNo;
	gstYktEnrichLog.lSaleCumulativeBalance=stPosParam.lSaleCumulativeBalance+gstYktEnrichLog.ulTransAmt;
	gstYktEnrichLog.uMode=1;//0��ֵ��1����

	//�ն˱����¼
	ucRet = SaveM1EnrichLog(0);
	if( ucRet!=OK )
	{
		lcdClrLine(2, 7);
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "�����ļ�����ʧ��", "FILE ERROR");
		ErrorBeep();
		kbGetKeyMs(2000);
	}

	stPosParam.lEnrichTraceNo++;
	stPosParam.lSaleCumulativeBalance=stPosParam.lSaleCumulativeBalance+gstYktEnrichLog.ulTransAmt;
	SaveAppParam();

	//��ӡ
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
//	UINT8 sCardDataList[64][16];	  //S50������������Ϣ 64��block��ÿ��block16�ֽ�
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
		PubDisplayTitle(TRUE, "��  ʾ");
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "���ڿռ�ʱ");
		lcdDisplay(0, 6, DISP_CFONT, "���㽻������");
		ErrorBeep();
		iRet = kbGetString(KB_EN_BIGFONT+KB_EN_CHAR+KB_EN_REVDISP, 0, 1, (3000), buf);
		if( iRet==KB_CANCEL || iRet==KEY_ENTER|| iRet==KB_TIMEOUT  )
		{}
	}

	PubDisplayTitle(TRUE, "��  ��");
	memset(&gstYktEnrichLog,0,sizeof(YKTENRICH_LOG));
	strcpy((char *)gstYktEnrichLog.sTellNo, (char *)stTransCtrl.szNowTellerNo);//sTellNo
	//iRet =ResetPsam(ICC_SAM1, ICC_ISO);//��λpsam��,������ʾ����
	iRet =ResetPsam2();//����2
	
	if(iRet !=0)
	{
		lcdClrLine(2, 7);
		PubDispString("SAM�ϵ縴λʧ��", 3|DISP_LINE_CENTER);
		PubBeepErr();
		PubWaitKey(5);
		return NO_DISP;
	}
	//--------����ISAM����֤------------
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
	memcpy(gstYktEnrichLog.sPosSN,stPosParam.sM1TellNo,8);//�ն˺�
	
	memset(gstYktEnrichLog.pointofsaleNo,0,sizeof(gstYktEnrichLog.pointofsaleNo));
	memcpy(gstYktEnrichLog.pointofsaleNo,stPosParam.pointofsaleNo,4);//��·/ ��ֵ���

	memset(gstYktEnrichLog.sCompanyNo,0,sizeof(gstYktEnrichLog.sCompanyNo));
	memcpy(gstYktEnrichLog.sCompanyNo,stPosParam.szCompanyNo,strlen(stPosParam.szCompanyNo));//��˾��
	
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
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "���:%sԪ",szTemp); 
	lcdDisplay(0, 6, DISP_CFONT|DISP_MEDIACY, "��ˢ������"); 
	iOldTime = sysGetTimerCount();
	mifClose();
	mifOpen();
	while(1)
    {
       iNowTime= sysGetTimerCount();
	   if( (iNowTime-iOldTime) >= (UINT32)(stPosParam.ucOprtLimitTime*1000) )	// ��鶨ʱ��
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
            // �ڸ�Ӧ���ڲ���ָ���������͵Ŀ�Ƭ�������ؿ�Ƭ�����кŵ������Ϣ
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

	if(memcmp(&sCardInfo[3],"\x28",1)==0 || memcmp(&sCardInfo[3],"\x20",1)==0){// CPU������

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
		ucRet = CPU_ReadSaleCard(&stInParam);// ����CPU���ļ�

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
	}else if(memcmp(&sCardInfo[3],"\x08",1)==0){//M1������
	
		ultemp=stTemp.sCardUid[0]+stTemp.sCardUid[1]*256+stTemp.sCardUid[2]*256*256+stTemp.sCardUid[3]*256*256*256;
		//PubChar2Long(gstYktEnrichLog.sCSN,4,&ultemp);
		memset(gstYktEnrichLog.sCSN,0,sizeof(gstYktEnrichLog.sCSN));
		sprintf((INT8 *)gstYktEnrichLog.sCSN, "%010u", ultemp);
		
		memcpy(sSect1KeyA,"\xA0\xA1\xA2\xA3\xA4\xA5",6);
		memcpy(&stTemp.sKeyBList ,sSect1KeyA,6);
		ucRet =M1_ReadSector(0, sSect1KeyA,MIF_KEYA, sSectorData,stTemp.sCardUid);//����48�ֽ�
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
		ucRet =M1_ReadSector(1, sSect1KeyA,MIF_KEYA, sSectorData,stTemp.sCardUid);//����48�ֽ�
		if(ucRet !=0)
		{
			return NO_DISP;
		}
		memcpy(stTemp.sCardDataList[4],sSectorData,48);
	//NacTxd("8002", 4);
	//NacTxd(sSectorData, 48);

		//������ʱ����
		memcpy(sCityCode,&sSectorData[0],2);//
		memcpy(sTrace,&sSectorData[4],4);// liu shui
		memcpy(sMac,&sSectorData[8],4);//ren zhen ma
		PubBcd2Asc(&sSectorData[0],8,gstYktEnrichLog.sCardSerialNo);
		gstYktEnrichLog.uCardtype=sSectorData[13];//������
	//NacTxd(sMac, 8);

		//�жϺ�����
		PubBcd2Asc(&sSectorData[4],4,blkTmp);
	//NacTxd(blkTmp, 8);
		iRet = ChkIfBlkCard(blkTmp);
		if(iRet !=0)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 2, DISP_CFONT, "��������");
			lcdDisplay(0, 4, DISP_CFONT, "����ϵ����Ա!");
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

		
		//��ȡ������������
		for(i=0;i<=8;i++)
		{
			memset(sSectorData,0,sizeof(sSectorData));//stTemp.sKeyBList
			ucRet =M1_ReadSector(i, &stTemp.sKeyBList[6*i],MIF_KEYA, sSectorData,stTemp.sCardUid);//����48�ֽ�
			//NacTxd(&i, 4);
			if(ucRet !=0)
			{
				return NO_DISP;
			}
			memcpy(stTemp.sCardDataList[i*4],sSectorData,48);
	//NacTxd(stTemp.sCardDataList[i*4],48);
		}
			
		iRet = M1_IsCardInfoLegal();//�жϿ������ݵĺϷ���
		if(iRet != 0)
		{
			return iRet;
		}
		memset(stTemp.sCardDataListSub, 0, sizeof(stTemp.sCardDataListSub));
		memcpy(stTemp.sCardDataListSub, stTemp.sCardDataList, 64*16);

		
		
	//��Ǯ������BLOCK9,
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
			//-------------����----
			lcdClrLine(2, 7);
			lcdDisplay(0, 4, DISP_CFONT, "����");
			ErrorBeep();
			sysDelayMs(3000);
			return NO_DISP;

		}


	//�жϣ�����������������
		if(gstYktEnrichLog.ulMaxBalaceAmt<gstYktEnrichLog.ulTransAmt)
		{
			//-------------�Ѿ�������ֵ���----
			lcdClrLine(2, 7);
			lcdDisplay(0, 4, DISP_CFONT, "���������޶�");
			ErrorBeep();
			//lcdDisplay(0, 6, DISP_CFONT, "������!");
			sysDelayMs(3000);
			return NO_DISP;

		}
	//NacTxd("1007", 4);
		iRet=M1_SaleProcess();
		if(iRet!=0)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 4, DISP_CFONT, "�ۿ�ʧ��");
			ErrorBeep();
			sysDelayMs(3000);
			return NO_DISP;
		}

		//�ٴλ�ȡ��������������֤�Ƿ�ۿ�ɹ�
		for(i=0;i<=8;i++)
		{
			memset(sSectorData,0,sizeof(sSectorData));//stTemp.sKeyBList
			ucRet =M1_ReadSector(i, &stTemp.sKeyBList[6*i],MIF_KEYA, sSectorData,stTemp.sCardUid);//����48�ֽ�
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
			lcdDisplay(0, 4, DISP_CFONT, "�ۿ�ʧ��");
			ErrorBeep();
			sysDelayMs(3000);
			return NO_DISP;
		}
		
	
		gstYktEnrichLog.ulNowBalaceAmt=gstYktEnrichLog.ulOldBalaceAmt-gstYktEnrichLog.ulTransAmt;
		//----������Ʋ���-------------------------
		gstYktEnrichLog.lEnrichTraceNo=stPosParam.lEnrichTraceNo;
		gstYktEnrichLog.lSaleCumulativeBalance=stPosParam.lSaleCumulativeBalance+gstYktEnrichLog.ulTransAmt;
		gstYktEnrichLog.uMode=1;//0��ֵ��1����

		//�ն˱����¼
		ucRet = SaveM1EnrichLog(0);
		if( ucRet!=OK )
		{
			lcdClrLine(2, 7);
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "�����ļ�����ʧ��", "FILE ERROR");
			ErrorBeep();
			kbGetKeyMs(2000);
		}
		stPosParam.lEnrichTraceNo++;
		stPosParam.lSaleCumulativeBalance=stPosParam.lSaleCumulativeBalance+gstYktEnrichLog.ulTransAmt;
		stPosParam.lUnSettTraceNum++;
		SaveAppParam();

		//��ӡ
		PrtM1TranSaleTicket(NORMAL);
		
	}

//��ʾ��ֵ�ɹ�

//-----�ٶ�ȡ���ݡ����--------------------------------------------

  /*NacTxd("OK11",4);
 int i;
 for(i=0;i<=7;i++)
 	{
     ucRet =M1_ReadSector(i, &stTemp.sKeyBList[6*i],MIF_KEYB, sSectorData,stTemp.sCardUid);//����48�ֽ�
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
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "�����߿�");
	
	//PubWaitKey(0);
//	showDebugMessage(iCardInfoLen, sCardInfo, "read card succ!");

	return 0;
}


//������16Ϊ���ַ�������
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
  	u8Ret1=Check_valid_purse(pdata);//����Ǯ���Ƿ���Ч
    u8Ret2=Check_valid_purse(pdata+16);//����Ǯ���Ƿ����
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
      return 0; //����Ҫ�޸�    1
   }
   else if(u8Ret1==TRUE && u8Ret2==TRUE &&IsEqualWallet== FALSE)
   {
       return 1;//������Ϊ׼�������������������� 4   
   }
   else if(u8Ret1==TRUE && u8Ret2==FALSE )
   {
    return 2;  //������Ϊ׼�������������������� 4    
   }
   else if(u8Ret1==FALSE && u8Ret2==TRUE )
   {
    return 3;//�Ը���Ϊ׼���Ѹ��������ݿ���������������
   }
   else
   {
   
    return 4;              //����������Ч��ֹͣ����
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
	PubDisplayTitle(TRUE, "��  ֵ");
	ucRet = SocketSet(4, 0);
	if( ucRet!=0 )
	{
	  lcdClrLine(2, 7);
	  PubDispString("û��SAM��", 3|DISP_LINE_CENTER);
	  PubBeepErr();
	  PubWaitKey(5);
	  return NO_DISP;	  
	}

	// SAM���ϵ縴λ
	ucRet = SamReset(4, NULL);
	if( ucRet==1 )
	{
	  lcdClrLine(2, 7);
	  PubDispString("SAM�ϵ縴λʧ��", 3|DISP_LINE_CENTER);
	  PubBeepErr();
	  PubWaitKey(5);
	  return NO_DISP;	   
	}
	  
//NacTxd("0001",4);
	mifClose();
	mifOpen();
	lcdClrLine(2, 7);
	PubDispString("��ˢ��", 3|DISP_LINE_CENTER);
	memset(sCardInfo, 0x00, sizeof(sCardInfo));
	ucRet =InListPassive(&iCardInfoLen, sCardInfo);
	if(ucRet !=0)
	{
		lcdClrLine(2, 7);
		PubDispString("����ʧ��!", 3|DISP_LINE_CENTER);
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
//����1������Կ
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
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "�����߿�");
	PubWaitKey(0);
	showDebugMessage(iCardInfoLen, sCardInfo, "read card succ!");

	return 0;
}*/
UINT8 Verifybyte(const char *pdata )//����16�ֽڣ�ǰ15�ֽ�������16�ֽڱȽ��Ƿ����
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
//---ÿһBlock�����ݵ�У��----
u8Ret=Verifybyte(Sectorsdata);
if(u8Ret!=TRUE)
{//��У���
	lcdClrLine(2, 7);
	lcdDisplay(0, 4, DISP_CFONT, "���ݿ�У�����!");
	lcdDisplay(0, 6, DISP_CFONT, "���ݿ�4!");
	sysDelayMs(3000);
	return NO_DISP;
}
u8Ret=Verifybyte(Sectorsdata +16);
if(u8Ret!=TRUE)
{//��У���
	lcdClrLine(2, 7);
	lcdDisplay(0, 4, DISP_CFONT, "���ݿ�У�����!");
	lcdDisplay(0, 6, DISP_CFONT, "���ݿ�5!");
	sysDelayMs(3000);
	return NO_DISP;
}
u8Ret=Verifybyte(Sectorsdata+32);
if(u8Ret!=TRUE)
{//��У���
	lcdClrLine(2, 7);
	lcdDisplay(0, 4, DISP_CFONT, "���ݿ�У�����!");
	lcdDisplay(0, 6, DISP_CFONT, "���ݿ�6!");
	sysDelayMs(3000);
	return NO_DISP;

}
//---������Ч�Ե���֤-------
/*memset(sCardNo,0,sizeof(sCardNo));
memcpy(sCardNo,Sectorsdata,2);//�������д���
memcpy(&sCardNo[2],Sectorsdata+3,1);//Ӧ�ô���
memcpy(&sCardNo[3],Sectorsdata+4,4);//������ˮ��
memcpy(&sCardNo[7],Sectorsdata+8,4);
u8Ret=VerifyCardNo();*/
//----���ñ�־----------------------
u8Ret=VerifyStartUse(Sectorsdata[12]);
if(u8Ret!=TRUE)
{
	return NO_DISP;
}
//----������֤---------------------
//GetDays(uint8_t * szDate)
memset(sDateTime,0,sizeof(sDateTime));
/*sDateTime[0]=0x20;
sysGetTime(&sDateTime[1]);
if(memcmp(sDateTime,&Sectorsdata[16+4],4)>0)
{
	lcdClrLine(2, 7);
	lcdDisplay(0, 4, DISP_CFONT, "��Ч���ڹ���");
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
			lcdDisplay(0, 4, DISP_CFONT, "��δ����");
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
		lcdDisplay(0, 4, DISP_CFONT, "����ͣ��");
		ErrorBeep();
		sysDelayMs(3000);
		return FALSE;

	}
	else if(flag==0x04 )
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "��������");
		ErrorBeep();
		sysDelayMs(3000);
		return FALSE;

	}
	else
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "��������");
		ErrorBeep();
		sysDelayMs(3000);
		return FALSE;

	}

}

/*UINT8 VerifyCardNo(const char *pCardNo)
{
    ;//----------
}*/
//�������У���2������
//Sectorsdata:������
//IsUpdateData:�Ƿ�������ݣ�1���£�0���£�
//������£�Sectorsdata���ظ��º������
int CheckData_Sectors2(char *Sectorsdata,UINT8 IsUpdateData,UINT8 * Uid,char * cKeyB)  //У����󣬿��ܻ��޸�����
{
	UINT8 u8Ret;//,u8Ret1,u8Ret2
	int iRet;
	
	u8Ret=Verifybyte(Sectorsdata);
	if(u8Ret!=TRUE)
	{//��У���
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "���ݿ�У�����!");
		lcdDisplay(0, 6, DISP_CFONT, "���ݿ�8!");
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
		else if(u8Ret==1 )//������Ϊ׼�������������������� 4   
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
				lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "Ǯ���ָ�ʧ��");
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
				lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "Ǯ���ָ�ʧ��");
				PubWaitKey(3);
				return NO_DISP; 
			}
		}
		else //==4
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 4, DISP_CFONT, "Ǯ��������Ч");
			//lcdDisplay(0, 6, DISP_CFONT, "������!");
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
			lcdDisplay(0, 4, DISP_CFONT, "Ǯ�����ݴ���");
			sysDelayMs(3000);
			return NO_DISP;
		}

	}
	//Ǯ��У��
	
	//AdjustPurse(Sectorsdata+16);
	//u8Ret1=Check_valid_purse(Sectorsdata+16);//����Ǯ���Ƿ���Ч

	//u8Ret2=Check_valid_purse(Sectorsdata+16);//����Ǯ���Ƿ���Ч
   //if()
	//����Ǯ��У��

	
/*����Ǯ������Ǯ��������Ǯ��������Ǯ��У����
����Ϣ��Ǯ��������Ǯ������ΪHex�룬���򣬱���
�������ͬһ������1���2���ڡ�����ÿ�γ�ֵ��
���ѽ���ʱ����Ǯ���������븱������Ч�Լ��Ƿ�
����ıȽϣ������������
��������������Ч��һ�£��������򸱱�Ϊ׼��
����������Ч��������Ч��������Ϊ׼��
����������Ч��������Ч���Ը���Ϊ׼��
��������������Ч����һ�£�������Ϊ׼��
5��������������Ч��Ǯ����Ч����ֵ�����ѽ�����ֹ��
���ϣ���������������£���һ��������Ч����������
��һ�£�Ӧ����ȷ�����ݿ���������ȷ�����ݿ顣*/
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
	if (1)//�ǽӴ�ʽ��
	{
		while(1)
		{
			iRet = mifCheckOnlyOneCard(0x07);
			if (iRet == MIF_RET_MULTICARD)
			{
				sysBeep();
				lcdClrLine(2,7);
				lcdPrintf("�� �� �� ��");
				sysDelayMs(3000);
				mifResetRF(0);
				return ERR_NO_DISP;
			} 
			else if ((MIF_RET_SUCCESS!=iRet) && (MIF_RET_NOCARD!=iRet) && (MIF_RET_TIMEOUT!=iRet))
			{
				sysBeep();
				lcdClrLine(2,7);
				lcdPrintf("�� �� �� ��:%d", iRet);
				sysDelayMs(3000);
				mifResetRF(0);
				return ERR_NO_DISP;
			}
			else if(iRet == MIF_RET_SUCCESS)
			{
				// �ڸ�Ӧ���ڲ���ָ���������͵Ŀ�Ƭ�������ؿ�Ƭ�����кŵ������Ϣ
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
				lcdPrintf("��    ��:%d", iRet);
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

//sOnlyNo��ˮ��
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

/*//---------//��֤MAC------------------------
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
		PubDispString("��֤MAC����", 3|DISP_LINE_CENTER);
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP;
	}
	memcpy(sAuthCode,stApduResp.DataOut,4);*/

	//---------ѡ���ļ�------------------------
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
			PubDispString("ѡ���ļ�����", 3|DISP_LINE_CENTER);
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
					PubDispString("��֤MAC����", 3|DISP_LINE_CENTER);
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
		PubDispString("��֤MAC����", 3|DISP_LINE_CENTER);
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

	//----------����1003Ŀ¼--"\x00\xA4\x00\x00\x02\x10\x03\x00\x01"-------------------
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
		PubDispString("����1003Ŀ¼����", 3|DISP_LINE_CENTER);
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
					PubDispString("��֤MAC����", 3|DISP_LINE_CENTER);
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
	PubDispString("��psam���Ŵ�!", 3|DISP_LINE_CENTER);
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
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "��ȡ��ʧ��");
			lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "������ˢ��");
			return iRet;
	
		}
		memset(gstTransInfo.sCardUid, 0, sizeof(gstTransInfo.sCardUid));
		memcpy((char*)gstTransInfo.sCardUid,(char*)&glstMsgInfo.stMagInfo.szTrack1[5],4);
		//��ȡ0����������
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
		
		//��ȡ1����������
		Section = 1;
		memcpy(gstTransInfo.sKeyAList[Section], gstTransInfo.stTranLog.ucCardCSN, 4);
		memcpy(sBuf, gstTransInfo.stTranLog.ucCardCSN, 2);
		PubXor(sBuf, "\xFF\xFF", 2);		//CSNȡ��ǰ�����ֽڼ���CSN�ĺ���
		memcpy(&gstTransInfo.sKeyAList[Section][4], sBuf, 2);
		iRet = M1_ReadCardSectionData(Section, gstTransInfo.sKeyAList[Section], MIF_TYPE_A);
		//Uart_RawTxd(gstTransInfo.sKeyAList[Section], 6, -1);
		if(iRet != MIF_RET_SUCCESS)
		{
			return iRet;
	
		}
		//��ȡ2-16������KeyA
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
	//����:��λpsam��
	//����:���ۺ�,��ʼ��Ϣ
	//ʧ�ܷ��� 1 �ɹ����� ��Ӧ���۱�׼���
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
	//��λpsam��ָ��
	UINT8 ResetPsam(void)
	{
		int    iRet;
		
		iRet = SocketSet(0, 0);
		if( iRet!=0 )
		{
			//lcdClrLine(2, 7);
			//PubDispString("û��SAM��", 3|DISP_LINE_CENTER);
			//PubBeepErr();
			//PubWaitKey(5);
			return NO_DISP; 	
		}
	
		// SAM���ϵ縴λ
		iRet = SamReset(0, NULL);
		if( iRet==1 )
		{
			//lcdClrLine(2, 7);
			//PubDispString("SAM�ϵ縴λʧ��", 3|DISP_LINE_CENTER);
			//PubBeepErr();
			//PubWaitKey(5);
			return NO_DISP; 	 
		}
		return 0;
	}

	
	//��λס����psam��ָ��
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
		// SAM���ϵ縴λ
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
		
	// �����Ϣ��SAM��������Կ
	if(socket == 0)
		iRet = SocketSet(socket, 2);
	else
		iRet = SocketSet(socket, 0);
	if( iRet!=0 )
	{
		lcdClrLine(2, 7);
		PubDispString("û��SAM��", 3|DISP_LINE_CENTER);
		mifClose();
		PubWaitKey(5);
		return ERR_SAM_POWERUP; 	
	}
	// SAM���ϵ縴λ
	iRet = SamReset(socket, NULL);
	if( iRet==1 )
	{
		lcdClrLine(2, 7);
		PubDispString("SAM�ϵ縴λʧ��", 3|DISP_LINE_CENTER);
		mifClose();
  	       iRet = iccPowerDown(socket+0x11);
		PubWaitKey(5);
		return ERR_SAM_POWERUP;		
	}
	return 0;
}*/
///////////////����ָ����///////////////////////
//���øú���ǰҪ�ȶ�psam���ϵ縴λ
//��ȡ��࿨psam���ն˻��ߺ�
//

//Ѱ��
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
int GetPsamId(UINT8 *pzPasmId)//10�ֽ�
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
		PubDispString("��psam���Ŵ�!", 3|DISP_LINE_CENTER);
        lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "sw1sw2:%02X%02X", stApduResp.SWA, stApduResp.SWB);
		PubBeepErr();
		PubWaitKey(3);
		return NO_DISP;
    }
    memcpy(pzPasmId, stApduResp.DataOut, 10);
  //  NacTxd(stApduResp.DataOut, stApduResp.LenOut);
    return 0;
}
int GetTellNo(UINT8 *pzTellNo)//10�ֽ�
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
		PubDispString("������Ա�Ŵ�!", 3|DISP_LINE_CENTER);
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

	
		iRet = ProcContEventWrite(BlockNo, cKey, iKeyType,WriteData,  M1_INCREMENT, Uid);//�����ݴ���
		if(iRet != MIF_RET_SUCCESS)
		{
			return -1;
		}
	
	
	return iRet;
}
int M1_DecValue(UINT32 BlockNo, char* cKey, int iKeyType, char* WriteData,UINT8 *Uid)
{
	int       iRet = 0;

	
		iRet = ProcContEventWrite(BlockNo, cKey, iKeyType,WriteData,  M1_DECREMENT, Uid);//�����ݴ���
		if(iRet != MIF_RET_SUCCESS)
		{
			return -1;
		}
	
	
	return iRet;
}

/****************************************************************************
 ��    ��:      ������ת���ɵ�λ��ǰ���ַ�����ʾ(���������ֽڸ�ʽ���)
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
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "��֤%d�����ݴ���", SourceBlock);
		sysDelayMs(3000);
		return NO_DISP;
	} 
	iRet = mifRestore(SourceBlock);
	if (iRet != MIF_RET_SUCCESS)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "���渱��ʧ��1");
		sysDelayMs(3000);
		return NO_DISP;
	}
	iRet = mifTransfer(DesBlock);
	if (iRet != MIF_RET_SUCCESS)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "���渱��ʧ��2");
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
		ucRet =M1_ReadSector(2, &stTemp.sKeyBList[6*2],MIF_KEYB, sSectorData,stTemp.sCardUid);//����48�ֽ�
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
	iRet = VeryfyCard_Sectors1(stTemp.sCardDataList[1*4]);//��֤��Ƭ�ĺϷ��ԣ���־��
	if(iRet !=0)
	{
		return NO_DISP;
	}
	/*if(stTemp.sCardDataList[6*4][6]==0x04)
	{//---��������
		lcdClrLine(2, 7);
		lcdDisplay(0, 4, DISP_CFONT, "��������");
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
				//д����һ��
				// ����S6B0����������־=0x04(������Ϣ���ú�)
				stTemp.sCardDataList[24][6] = 0x04;
				//gstTransInfo.sCardDataList[24][0] = ucLrc;
				ucLrc = PubCalcLRC((char*)stTemp.sCardDataList[24],15,0);
				stTemp.sCardDataList[24][15] = ucLrc;
				u8Ret= M1_WriteBlock(6*4,&stTemp.sKeyBList[6*6],MIF_KEYB,stTemp.sCardDataList[24],stTemp.sCardUid);//������  ����Ϣ��
				if(u8Ret!=OK)
				{
				    ErrorBeep();
					iRet = M1_SWIPE_AGAIN;
					break;
				}*/
				iRet = M1_SWIPERET_2;
				break;
			case M1_SWIPERET_2:
				//д���ڶ���
//NacTxd("6002",4);
				//PubLong2CharEx(gstYktEnrichLog.ulTransAmt, 4, szAmount);
				PubLong2Char(gstYktEnrichLog.ulTransAmt, 4, szAmount);
//NacTxd(&gstYktEnrichLog.ulTransAmt,10);
//NacTxd(szAmount,10);
				iRet = M1_DecValue(4*2+2, &stTemp.sKeyBList[6*2], MIF_KEYA, (UINT8*)szAmount,stTemp.sCardUid);
				if(iRet!=OK)
				{
					//lcdClrLine(2, 7);
					//lcdDisplay(0, 4, DISP_CFONT, "дǮ������ʧ��");
					//lcdDisplay(0, 6, DISP_CFONT, "��ֵ����");
					ErrorBeep();
					iRet = M1_SWIPE_AGAIN;
					break;

				}
				iRet = M1_SWIPERET_3;
				break;
			case M1_SWIPERET_3:
				//д�ɵ�����
				// дǮ������
//NacTxd("6003",4);
				iRet = M1_BlocktoBlockValue(9, 10, &stTemp.sKeyBList[6*2], MIF_KEYA,stTemp.sCardUid);//��ʮ������
				if (iRet != 0)
				{
					//��ˢ������
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
				PubLong2Char(gstYktEnrichLog.ulPOSTraceNo+1,2,&sBlockData[12]);//��������һ
				GetBlockVerifybyte(sBlockData);
				u8Ret= M1_WriteBlock(1*4+1,&stTemp.sKeyBList[6*1],MIF_KEYA,sBlockData,stTemp.sCardUid);
				if(u8Ret!=OK)
				{
					//lcdClrLine(2, 7);
					//lcdDisplay(0, 4, DISP_CFONT, "д��ʧ��");
				//	lcdDisplay(0, 6, DISP_CFONT, "���¼�����");
					ErrorBeep();
				//	sysDelayMs(3000);
					iRet = M1_SWIPE_AGAIN;
					break;
				}*/
				iRet = M1_SWIPERET_5;
				break;
			case M1_SWIPERET_5:
				/*//NacTxd("6005",4);
				//д�����Ĳ�
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
					//lcdDisplay(0, 4, DISP_CFONT, "д��ʧ��");
					//lcdDisplay(0, 6, DISP_CFONT, "����block6");
					ErrorBeep();
					//sysDelayMs(3000);
					iRet = M1_SWIPE_AGAIN;
					break;
				}*/
				iRet = M1_SWIPERET_6;
				//DispErrPrompt("���������", 10);
				break;
			case M1_SWIPERET_6:
				/*//NacTxd("6006",4);
				//----------------------------------------
				//���¶���������(2:  0)
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
					//lcdDisplay(0, 4, DISP_CFONT, "д��ʧ��");
					//lcdDisplay(0, 6, DISP_CFONT, "���¶���������");
					ErrorBeep();
					//sysDelayMs(3000);
					iRet = M1_SWIPE_AGAIN;
					break;
				}*/
				iRet = M1_SWIPERET_7;
				//DispErrPrompt("���������", 10);
				break;
			
			case M1_SWIPERET_7:
				/*//д�����岽
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
				u8Ret= M1_WriteBlock(6*4+1,&stTemp.sKeyBList[6*6],MIF_KEYB,sBlockData,stTemp.sCardUid);//������  ����Ϣ��
				if(u8Ret!=OK)
				{
				//lcdClrLine(2, 7);
				//lcdDisplay(0, 4, DISP_CFONT, "д��ʧ��");
				//lcdDisplay(0, 6, DISP_CFONT, "����������Ϣ��");
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
				//u8Ret = M1_BlocktoBlockValue(4*6, 4*6+1, &stTemp.sKeyBList[6*6], MIF_KEYB,stTemp.sCardUid);//��ʮ������
				u8Ret= M1_WriteBlock(6*4,&stTemp.sKeyBList[6*6],MIF_KEYB,stTemp.sCardDataList[6*4+1],stTemp.sCardUid);//������  ����Ϣ��
				if (u8Ret != 0)
				{
					//��ˢ������
					//lcdClrLine(2, 7);
					//lcdDisplay(0, 4, DISP_CFONT, "д��ʧ��");
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
				//д����һ��
				// ����S6B0����������־=0x04(������Ϣ���ú�)
				stTemp.sCardDataList[24][6] = 0x04;
				//gstTransInfo.sCardDataList[24][0] = ucLrc;
				ucLrc = PubCalcLRC((char*)stTemp.sCardDataList[24],15,0);
				stTemp.sCardDataList[24][15] = ucLrc;
				u8Ret= M1_WriteBlock(6*4,&stTemp.sKeyBList[6*6],MIF_KEYB,stTemp.sCardDataList[24],stTemp.sCardUid);//������  ����Ϣ��
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
				//д���ڶ���
				//NacTxd("6002",4);
				PubLong2CharEx(gstYktEnrichLog.ulTransAmt, 4, szAmount);
				//NacTxd(&gstYktEnrichLog.ulTransAmt,4);
				//NacTxd(szAmount,4);
				iRet = M1_IncValue(4*2+2, &stTemp.sKeyBList[6*2], MIF_KEYB, (UINT8*)szAmount,stTemp.sCardUid);
				if(iRet!=OK)
				{
					//lcdClrLine(2, 7);
					//lcdDisplay(0, 4, DISP_CFONT, "дǮ������ʧ��");
					//lcdDisplay(0, 6, DISP_CFONT, "��ֵ����");
					ErrorBeep();
					iRet = M1_SWIPE_AGAIN;
					break;

				}
				iRet = M1_SWIPERET_3;
				break;
			case M1_SWIPERET_3:
				//д�ɵ�����
				// дǮ������
				//NacTxd("6003",4);
				iRet = M1_BlocktoBlockValue(9, 10, &stTemp.sKeyBList[6*2], MIF_KEYB,stTemp.sCardUid);//��ʮ������
				if (iRet != 0)
				{
					//��ˢ������
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
				PubLong2Char(gstYktEnrichLog.ulPOSTraceNo+1,2,&sBlockData[12]);//��������һ
				GetBlockVerifybyte(sBlockData);
				u8Ret= M1_WriteBlock(1*4+1,&stTemp.sKeyBList[6*1],MIF_KEYB,sBlockData,stTemp.sCardUid);
				if(u8Ret!=OK)
				{
					//lcdClrLine(2, 7);
					//lcdDisplay(0, 4, DISP_CFONT, "д��ʧ��");
				//	lcdDisplay(0, 6, DISP_CFONT, "���¼�����");
					ErrorBeep();
				//	sysDelayMs(3000);
					iRet = M1_SWIPE_AGAIN;
					break;
				}
				iRet = M1_SWIPERET_5;
				break;
			case M1_SWIPERET_5:
				//NacTxd("6005",4);
				//д�����Ĳ�
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
					//lcdDisplay(0, 4, DISP_CFONT, "д��ʧ��");
					//lcdDisplay(0, 6, DISP_CFONT, "����block6");
					ErrorBeep();
					//sysDelayMs(3000);
					iRet = M1_SWIPE_AGAIN;
					break;
				}
				iRet = M1_SWIPERET_6;
				//DispErrPrompt("���������", 10);
				break;
			case M1_SWIPERET_6:
				//NacTxd("6006",4);
				//----------------------------------------
				//���¶���������(2:  0)
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
					//lcdDisplay(0, 4, DISP_CFONT, "д��ʧ��");
					//lcdDisplay(0, 6, DISP_CFONT, "���¶���������");
					ErrorBeep();
					//sysDelayMs(3000);
					iRet = M1_SWIPE_AGAIN;
					break;
				}
				iRet = M1_SWIPERET_7;
				//DispErrPrompt("���������", 10);
				break;
			
			case M1_SWIPERET_7:
				/*
				//д�����岽
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
				u8Ret= M1_WriteBlock(6*4+1,&stTemp.sKeyBList[6*6],MIF_KEYB,sBlockData,stTemp.sCardUid);//������  ����Ϣ��
				if(u8Ret!=OK)
				{
				//lcdClrLine(2, 7);
				//lcdDisplay(0, 4, DISP_CFONT, "д��ʧ��");
				//lcdDisplay(0, 6, DISP_CFONT, "����������Ϣ��");
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
				//u8Ret = M1_BlocktoBlockValue(4*6, 4*6+1, &stTemp.sKeyBList[6*6], MIF_KEYB,stTemp.sCardUid);//��ʮ������
				u8Ret= M1_WriteBlock(6*4,&stTemp.sKeyBList[6*6],MIF_KEYB,stTemp.sCardDataList[6*4+1],stTemp.sCardUid);//������  ����Ϣ��
				if (u8Ret != 0)
				{
					//��ˢ������
					//lcdClrLine(2, 7);
					//lcdDisplay(0, 4, DISP_CFONT, "д��ʧ��");
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
		lcdDisplay(0, 2, DISP_CFONT, "������ˢ��");

		iRet = ChkContactlessCardPad(&CardInfoLen, CardInfo);
		if(iRet != MIF_RET_SUCCESS)
		{
			return iRet ;
		}
		if(memcmp(stTemp.sCardUid, &CardInfo[5], 4) != 0)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "����һ�ʽ��׿���");
			PubWaitKey(3);
			continue;
		}
		//��ȡ��Ƭ��������
		iRet = GetAllDataFromM1Card();
		if(iRet != 0)
		{
			continue;
		}
		//�ж��Ƿ��ǽ�����Ԥ������
		/*if(gstTransInfo.sCardDataList[24][6] == 0x02
			&&memcmp(stTemp.sCardDataList[10], stTemp.sCardDataListSub[10], 12) != 0//������ɹ�
			&&memcmp(stTemp.sCardDataList[9], stTemp.sCardDataListSub[9], 12) != 0//�������ɹ�
			//&&memcmp(gstTransInfo.sCardDataList[5], gstTransInfo.sCardDataListSub[5], 16) != 0
			&&memcmp(stTemp.sCardDataList[6], stTemp.sCardDataListSub[6], 16) != 0//�����ĳɹ�
			&&memcmp(stTemp.sCardDataList[24], stTemp.sCardDataList[24], 16) != 0)//������ɹ�
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "��ˢ���ɹ�");
			sysDelayMs(3000);
			return 0;
		}*/
		//�ж�S6B0_6 �Ƿ��ú�
		if(stTemp.sCardDataList[24][6] != 0x04)
		{
			if(memcmp(stTemp.sCardDataList[10], stTemp.sCardDataListSub[10], 12) == 0)
			{
				return M1_SWIPERET_1;
			}

			return 0;
		}
		//������ֵ�Ƿ�ɹ�
		if(memcmp(stTemp.sCardDataList[10], stTemp.sCardDataListSub[10], 12) == 0)
		{
			return M1_SWIPERET_2;
		}
			//������ֵ�Ƿ�ɹ�
		if(memcmp(stTemp.sCardDataList[9], stTemp.sCardDataListSub[9], 12) == 0)
		{
			return M1_SWIPERET_3;
		}
	    //�������Ƿ����
		if(memcmp(stTemp.sCardDataList[5], stTemp.sCardDataListSub[5], 16) == 0)
		{
			return M1_SWIPERET_4;
		}
		//��Ƭ��ֵ��¼�Ƿ�ɹ�
		if(memcmp(stTemp.sCardDataList[6], stTemp.sCardDataListSub[6], 16) == 0)
		{
			return M1_SWIPERET_5;
		}
		//������Ϣ��0���Ƿ����ֵԤ�ڽ��һ��
		if(memcmp(stTemp.sCardDataList[8], stTemp.sCardDataListSub[8], 16) == 0)
		{
			return M1_SWIPERET_6;
		}
		if(memcmp(stTemp.sCardDataList[25], stTemp.sCardDataListSub[25], 16) == 0)
		{
			return M1_SWIPERET_7;
		}
		//-��������--
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
	if (1)//�ǽӴ�ʽ��
	{
		while(1)
		{
			iRet = mifCheckOnlyOneCard(0x07);
			if (iRet == MIF_RET_MULTICARD)
			{
				sysBeep();
				lcdClrLine(2,7);
				lcdPrintf("�� �� �� ��");
				sysDelayMs(3000);
				mifResetRF(0);
				return NO_DISP;
			} 
			else if ((MIF_RET_SUCCESS!=iRet) && (MIF_RET_NOCARD!=iRet) && (MIF_RET_TIMEOUT!=iRet))
			{
				sysBeep();
				lcdClrLine(2,7);
				lcdPrintf("�� �� �� ��:%d", iRet);
				sysDelayMs(3000);
				mifResetRF(0);
				return NO_DISP;
			}
			else if(iRet == MIF_RET_SUCCESS)
			{
				// �ڸ�Ӧ���ڲ���ָ���������͵Ŀ�Ƭ�������ؿ�Ƭ�����кŵ������Ϣ
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
				lcdPrintf("��    ��:%d", iRet);
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
