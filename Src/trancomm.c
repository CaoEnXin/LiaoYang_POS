
#include "posapi.h"
#include "posmacro.h"
#include "glbvar.h"
#include "terstruc.h"
#include "tranfunclist.h"
#include "st8583.h"
#include "posappapi.h"
#include "emvtranproc.h"
#include "wireless.h"
#include "qpbocProc.h"

uint8_t OnlineCommProc(void)
{
	uint8_t	ucRet;
//	int16_t	PinEntryTimes;

	//EMV卡交易处理
	if( PosCom.ucSwipedFlag==CARD_INSERTED && 
		(stTemp.iTransNo==POS_SALE || stTemp.iTransNo==POS_PREAUTH || stTemp.iTransNo==POS_QUE) )
	{
		return ProcEmvTran();
	}

	if (PosCom.ucSwipedFlag == CARD_PASSIVE)
	{
		if (!(PosCom.stTrans.iTransNo>=POS_MOBILE_SALE && PosCom.stTrans.iTransNo<=POS_MOBILE_QUE))
		{
			return ProcQpbocTran();
		}
	}

	if( stTransCtrl.bEmvCapkLoaded==FALSE && stTemp.iTransNo != DOWNLOAD_EMV_CAPK 
		&& stTemp.iTransNo != QUERY_EMV_CAPK && stTemp.iTransNo != END_EMV_CAPK)
	{
		ucRet = DownLoadCAPK(TRUE);
		if(ucRet != 0)
		{
			lcdClrLine(2,7);
			lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"请先下载CAPK参数");
			PubBeepErr();
			PubWaitKey(5);
			return NO_DISP;
		}
	}

	if( stTransCtrl.bEmvAppLoaded==FALSE && stTemp.iTransNo != DOWNLOAD_EMV_PARAM 
		&& stTemp.iTransNo != QUERY_EMV_PARAM && stTemp.iTransNo != END_EMV_PARAM)
	{	
		ucRet = DownLoadEmvParam(TRUE);
		if(ucRet != 0)
		{
			lcdClrLine(2,7);
			lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"请先下载EMV参数");
			PubBeepErr();
			PubWaitKey(5);
			return NO_DISP;
		}
	}

	SendScriptResult();
	ReverseTranProc();

//	PinEntryTimes = 0;
	while(1)
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT, "已连接银联中心", "CONNECT BANK");
		ucRet = SendRecvPacket();
		if( ucRet!=OK )
		{
			UpdateReversalFile(ucRet); // 更新记录的原冲正文件
		
			return ucRet;
		}
		fileRemove(REVERSAL_FILE);   // 通讯成功
	
//20080731 密码错就提示退出
		break;

	}
	
	return AfterCommProc(); 
}

/************************************************************************
冲正的处理
************************************************************************/
uint8_t ReverseTranProc(void)
{
	uint8_t ucTryCnt, ucRet=0;

	if( fileExist(REVERSAL_FILE)<0 )
	{
		return OK;
	}
	
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_CFONT, "自动冲正...", "AUTO REVERSAL..");
	
	PosCom.stTrans.iTransNo = stTemp.iTransNo;

	memcpy(&PosCom.stOrgPack, &glSendPack, sizeof(glSendPack));	// 保存请求报文
	memcpy(&BackPosCom, &PosCom, COM_SIZE);
	for(ucTryCnt=0; ucTryCnt<stPosParam.ucResendTimes+1; ucTryCnt++) 
	{

		ucRet = ReadReversalTranData();	// fileRead reversal data
		if( ucRet!=OK )
		{
			break;
		}
		
		stTemp.iTransNo = POS_REVERSE;

		//MakeReversalPack();
		//reversalOnline
		//MakeReversalPack_CPU();

		ucRet = SendRecvPacket();
		if( ucRet!=OK )
		{
			//ReadReversalTranData();
			//strcpy((char *)PosCom.szRespCode, "98");
		//	SaveReversalTranData();
			continue;
		}
	//	NacTxd("1230",4);
	//    NacTxd(sRecvPacketBuff,usRecvPacketLen);
	   if(memcmp(&sRecvPacketBuff[7],"\x40\x03",2)==0)
	   {
            ucRet = OK;
			break;
	   }
	else if(memcmp(&sRecvPacketBuff[7],"\x41\x03",2)==0)
	{
	        ucRet = OK;
			break;
	}
	/*else if(memcmp(&sRecvPacketBuff[7],"\x40\x13",2)==0)
	{

	   continue;
	}
	else if(memcmp(&sRecvPacketBuff[7],"\x41\x13",2)==0)
	{
		continue;

	}

	else
	{
		
		continue;
		
	}*/
	

		ReadReversalTranData();

		//当返回冲正不成功时,变为其他原因
		strcpy((char *)PosCom.szRespCode, "06"); 
		SaveReversalTranData();
	}

	fileRemove(REVERSAL_FILE);
//20080730 将打印放在赋值之前
	if( ucTryCnt>=stPosParam.ucResendTimes+1 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "   冲正不成功   ", "REVERSAL FAIL");
		DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "  请与客服联系  ", "CONNECT BANK");
		ErrorBeep();
		kbGetKeyMs(3000);
		//PrtReverseFailTicket(0);
        /*if( stPosParam.ucErrorPrt==PARAM_OPEN )
		{
			PrtTranTicket(2);
		}*/
	}

	memcpy(&PosCom, &BackPosCom, COM_SIZE);
	memcpy(&glSendPack, &PosCom.stOrgPack, sizeof(glSendPack));	// 恢复请求报文
	stTemp.iTransNo = PosCom.stTrans.iTransNo;
	PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;
	if( ucTryCnt>=stPosParam.ucResendTimes+1 )
	{
		return E_TRANS_FAIL;
	}

	lcdClrLine(2, 3);
	lcdFlip();	
	
	return ucRet;
}


/****************************************************************************
 函数功能:  终端与后台之间一次发收的过程
****************************************************************************/
uint8_t SendRecvPacket(void)
{
//	int16_t	iResult;
	uint8_t	ucRet;
	unsigned long  ulTemp;
	uint8_t buf[10],MAC[8];
	uint8_t  sRecbuf[1024];
    int ioutlen ;
	int iRet;
	uint8_t sMsgBodyKey[16],sMacKey[8];
	/*iResult = MakeReqPacket();
	if( iResult<0 )
	{
		return E_MAKE_PACKET;
	}*/

	if( ConnectHost()!=OK )
	{
		return E_ERR_CONNECT;
	}

	if( stTemp.iTransNo==POS_REVERSE )
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_CLRLINE|DISP_REVERSE, "    自动冲正    ", "  AUTO REVERSAL ");
	}
	else
	{
		DispTransName();		
	}

	lcdClrLine(2,7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT|DISP_CLRLINE, "已连接公交服务器");
	DispMulLanguageString(0, 4, DISP_CFONT|DISP_CLRLINE, "处理中...       ", "DEALING WITH    ");
	DispMulLanguageString(0, 6, DISP_CFONT|DISP_CLRLINE, "发送数据包      ", "SEND DATA       ");

	if( gstCurCommCfg.ucCommType==CT_TCPIP ||
		gstCurCommCfg.ucCommType==CT_GPRS  ||
		gstCurCommCfg.ucCommType==CT_CDMA  ||
		gstCurCommCfg.ucCommType==CT_WIFI )
//		|| gstCurCommCfg.ucCommType==CT_MODEM )		
	{
	    // memcpy(&sSendPacketBuff,buf,4);
	 	
	//	sprintf(buf, "%04d", usSendPacketLen);
	    usSendPacketLen += 4;//把最后4字节的mac的
		memmove(&sSendPacketBuff[2], sSendPacketBuff, usSendPacketLen);
		// memcpy(sSendPacketBuff,buf,4);
		PubLong2Char((ulong)usSendPacketLen, 2, (char *)sSendPacketBuff);
		usSendPacketLen += 2;
	}
	//CalCpuMAC(sSendPacketBuff,usSendPacketLen-4,"",MAC);
	
	PubAsc2Bcd(stPosParam.szMacKey,16,sMacKey);
	GetMAC(sSendPacketBuff,usSendPacketLen-4,sMacKey,MAC);
	memcpy(sSendPacketBuff+usSendPacketLen-4,MAC,4);
//	NacTxd("11111",5);
 //   NacTxd(sSendPacketBuff,usSendPacketLen);
/*	if( stTemp.iTransNo==POS_REVERSE )
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_CLRLINE|DISP_REVERSE, "    自动冲正    ", "  AUTO REVERSAL ");
	}
	else
	{
		DispTransName();
	}*/
//PubXor(char * psSource1,char * psSource2,int iLength)

//NacTxd(sSendPacketBuff,150);
	ucRet = SendPacket(sSendPacketBuff, usSendPacketLen);
	if( ucRet!=OK )
	{
		return E_SEND_PACKET;
	}
   if(stTemp.iTransNo==M1_ENRICH)
    {
       SaveReversalTranData();
	}
//	PosCom.bOnlineTxn = TRUE; //  设置已经进行过联机交易
	
/*#ifdef PBOC_VERSION
	if( (PosCom.ucSwipedFlag==CARD_INSERTED) && (stTemp.iTransNo==POS_QUE) )
	{
		memcpy(&PosCom.stOrgPack, &glSendPack, sizeof(glSendPack));
	}
#endif*/

/*	if( TranNeedReversal(stTemp.iTransNo) )	// 判断本交易是否需要冲正
	{
		strcpy((char *)PosCom.szRespCode, "06");
		memcpy(&PosCom.stOrgPack, &glSendPack, sizeof(glSendPack));

		SaveReversalTranData();
		if( PosCom.ucSwipedFlag==CARD_INSERTED )
		{
			UpdateEmvRevFile();
		}
		if( stTemp.iTransNo == QPBOC_ONLINE_SALE )
		{
			UpdateEmvRevFile();
		}
	}*/
/*	if( TranNeedIncTraceNo(stTemp.iTransNo) )	// 判断本交易是否需要增加流水号
	{
		IncreaseTraceNo();
	}*/
	//stPosParam.lNowTraceNo++;
    SaveAppParam();
	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT|DISP_CLRLINE, "已连接公交服务器");
	DispMulLanguageString(0, 4, DISP_CFONT|DISP_CLRLINE, "处理中...       ", "DEALING WITH    ");
	DispMulLanguageString(0, 6, DISP_CFONT|DISP_CLRLINE, "接收返回        ", "RECV DATA       ");
	ucRet = RecvPacket(sRecvPacketBuff, &usRecvPacketLen, stPosParam.ucCommWaitTime);
	if( ucRet!=OK )
	{
		return E_RECV_PACKET;
	}
	//NacTxd(sRecvPacketBuff,usRecvPacketLen);
	if( gstCurCommCfg.ucCommType==CT_TCPIP ||
		gstCurCommCfg.ucCommType==CT_GPRS  ||
		gstCurCommCfg.ucCommType==CT_CDMA  ||
		gstCurCommCfg.ucCommType==CT_WIFI )
//		|| gstCurCommCfg.ucCommType==CT_MODEM )		
	{
		PubChar2Long(sRecvPacketBuff, 2, &ulTemp);
		if( ulTemp+2!=(ulong)usRecvPacketLen )
		{
			Display2StringInRect((uint8_t *)"接收数据错误", (uint8_t *)"RECV DATA ERR");
			PubBeepErr();
			PubWaitKey(2);
		
			return E_RECV_PACKET;
		}
	//	memmove(sRecvPacketBuff, &sRecvPacketBuff[2], (uint)ulTemp);
		usRecvPacketLen = (uint32_t)ulTemp+2;//两位头长度
	}
//---mac 计算--------------
   memcpy(sRecbuf,sRecvPacketBuff,usRecvPacketLen);

  PubAsc2Bcd(stPosParam.szMacKey,16,sMacKey);
//  NacTxd(sMacKey,8);
  iRet=GetMAC(sRecbuf,usRecvPacketLen-4,sMacKey,MAC);
  if(iRet!=0)
  {

     return  E_CHECK_MAC_VALUE;
			//计算mac错
  }//memcmp(&glRecvPack.sMacData[2], sMacData, LEN_MAC_DATA)!=0
  if(memcmp(MAC,&sRecvPacketBuff[usRecvPacketLen-4],4)!=0)
  {//mac校验错，需要沖正
  
    return  E_CHECK_MAC_VALUE;

  }
  //----解包成明文
     PubChar2Long(&sRecvPacketBuff[5],2,&ioutlen);
	 // PubLong2Char
	  	PubAsc2Bcd(stPosParam.szMsgBodyKey,32,sMsgBodyKey);
	 TDEACBCDecry(&sRecvPacketBuff[7],ioutlen,sMsgBodyKey);
  //   NacTxd("00000",5);
	// NacTxd(sRecvPacketBuff,usRecvPacketLen);

//----------------
	/*iResult = ProcRspPacket();
	if( iResult<0 ) 
	{
		return ((iResult==-2) ? E_CHECK_MAC_VALUE : E_RESOLVE_PACKET);
	}*/

    if(stTemp.iTransNo==M1_ENRICH)
    {
       	fileRemove(REVERSAL_FILE);
	}
	return OK;	
}

uint8_t SendPacket(uint8_t *sSendData, uint16_t PacketLen)
{
	int   iRet;

	iRet = CommTxd(sSendData,PacketLen,stPosParam.ucCommWaitTime);
	if (iRet != 0)
	{
		DispCommErrMsg(iRet);
		return NO_DISP;
	}

	return iRet;
}

uint8_t RecvPacket(uint8_t *psPacket, uint16_t *usPacketLen, uint8_t psWaitTime)
{
	int    iRet;
	uint32_t  ulRecvLen;

	iRet = CommRxd(psPacket, MAX_PACKET_LEN, psWaitTime, &ulRecvLen);
	if (iRet != 0)
	{
		DispCommErrMsg(iRet);
		return NO_DISP;
	}

	*usPacketLen = (uint16_t)ulRecvLen;
	
	return iRet;
}

int ConnectHost(void)
{
	int		iRet;
	uint8_t	ucKey;
	
	iRet = AdjustCommParam();
	if( iRet!=0 )
	{
		DispCommErrMsg(iRet);
		return NO_DISP;
	}
	
	if( stTemp.iTransNo==POS_REVERSE )
	{
		DispMulLanguageString(0, 0, DISP_CFONT|DISP_CLRLINE|DISP_REVERSE, "    自动冲正    ", "  AUTO REVERSAL ");
	}
	else
	{
		DispTransName();		
	}

	while( 1 )
	{
		lcdClrLine(2, 7);		
		lcdFlip();	
		DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, " 拨 号 中...", " DAILING...");
		iRet = CommDial(ACTDIAL_MODE);
		if( iRet==0 )
		{
			break;
		}
		if (iRet == ERR_USERCANCEL)
		{
			DispResult(iRet);
			return NO_DISP;
		}
		if( (iRet & MASK_COMM_TYPE)!=ERR_COMM_ALL_BASE )
		{
			DispCommErrMsg(iRet);
			return NO_DISP;
		}
		
		sysBeef(1, 200);
		DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY,"拨号失败,重试?  ", "DAIL FAIL,RETRY?");
		lcdDisplay(114, 7, DISP_ASCII, "%02X", (uint8_t)(iRet & MASK_ERR_CODE));
		ucKey = kbGetKeyMs(30000);
		if( ucKey==KEY_CANCEL || ucKey==KEY_INVALID )
		{
			return ERR_USERCANCEL;
		}
	}
	
	return 0;
}

void CommHangUp(uint8_t bReleaseAll)
{
	CommOnHook(bReleaseAll);
}

//保存冲正记录需要的全局变量到文件中
uint8_t SaveReversalTranData(void)
{
	int16_t fd;
	int16_t len;

	fd = fileOpen(REVERSAL_FILE, O_CREAT|O_RDWR);
	if( fd<0 )
	{
		return (E_MEM_ERR);
	}

	len = fileWrite(fd, (uint8_t *)&PosCom, COM_SIZE);
	if( len!=COM_SIZE )
	{
		fileClose(fd);
		return (E_MEM_ERR);
	}
	fileClose(fd);

	return (OK);
}

//从文件中读取保存的冲正记录到全局变量
uint8_t ReadReversalTranData(void)
{
	int16_t		fd;

	fd = fileOpen(REVERSAL_FILE, O_RDWR);
	if( fd<0 )
	{
		return E_MEM_ERR;
	}

	if( fileRead(fd, (uint8_t *)&PosCom, COM_SIZE)!=COM_SIZE )
	{
		fileClose(fd);
		return E_MEM_ERR;
	}

	fileClose(fd);

	return OK;
}

//冲正时候要送38域。
uint8_t UpdateReversalFile(uint8_t Err) ///更新记录的原冲正文件
{
	uint8_t	szBuff[10];
	uint8_t szBuff1[10];

	memset(szBuff, 0, sizeof(szBuff));
	if( Err==E_RECV_PACKET || Err==E_CHECK_MAC_VALUE || Err==E_RESOLVE_PACKET )
	{
		if( fileExist(REVERSAL_FILE)>=0 ) 
		{
			if( strlen((char *)PosCom.stTrans.szAuthNo)>0 )
			{
				sprintf((char *)szBuff, "%s", PosCom.stTrans.szAuthNo);
			}
			if( strlen((char *)PosCom.stTrans.szDate)>0 )
			{
				sprintf((char *)szBuff1, "%.8s", PosCom.stTrans.szDate);
			}

			ReadReversalTranData();

			if( stTemp.iTransNo==POS_SALE || stTemp.iTransNo==POS_PREAUTH )
			{
				if( strlen((char *)szBuff)>0 )
				{
					sprintf((char *)PosCom.stTrans.szAuthNo, "%s", szBuff);
				}
			}
			
			if( (stTemp.iTransNo==POS_PREAUTH_VOID || stTemp.iTransNo==POS_AUTH_CM 
				|| stTemp.iTransNo==POS_AUTH_VOID) )
			{
				if( Err==E_CHECK_MAC_VALUE )
				{
					if( strlen((char *)szBuff1)>0 )
					{
						sprintf((char *)PosCom.stTrans.szDate, "%s", szBuff1);
					}
				}
				else
				{
					strcpy((char *)PosCom.stTrans.szDate, "00000000");
				}
			}

			if( Err==E_RECV_PACKET )
			{
				strcpy((char *)PosCom.szRespCode, "98"); 
			}
			else if( Err==E_CHECK_MAC_VALUE )
			{
				strcpy((char *)PosCom.szRespCode, "A0");  
			}

			SaveReversalTranData();
		}
	}
	
	return Err;	
}

int MakeReqPacket(void)
{
	int16_t	iRet;
	uint8_t	 sMacData[LEN_MAC_DATA];
	uint8_t	 szBuffer[10];
	uint16_t uiLen;

	// 初始化发送/接收缓冲区
	memset(sSendPacketBuff, 0, sizeof(sSendPacketBuff));
	memset(sRecvPacketBuff, 0, sizeof(sRecvPacketBuff));
	usRecvPacketLen = 0;

//20080715 串口测试不需要TPDU	
	memcpy(sSendPacketBuff, stPosParam.sTpdu, 5);	// TPDU
	if( stPosParam.ucTestFlag==PARAM_OPEN )			// pos包控制信息
	{
		memcpy(&sSendPacketBuff[5], "\x60\x31\x10\x31\x00\x00", 6);
	}
	else
	{
// 		if (stTemp.iTransNo == POS_DOWNLOADBLK)
// 			memcpy(&sSendPacketBuff[5], "\x60\x31\x70\x31\x00\x00", 6);
// 		else
//			memcpy(&sSendPacketBuff[5], "\x60\x31\x00\x31\x00\x00", 6);
        memcpy(&sSendPacketBuff[5], "\x60\x31\x00", 3);
        memset(szBuffer, 0, sizeof(szBuffer));
        sprintf((char *)szBuffer, VERSION_STRING);
        PubAsc2Bcd((char *)&szBuffer[1], 6, (char *)&sSendPacketBuff[8]);
	}


	iRet = iPack8583(glMsg0, glData0, &glSendPack, &sSendPacketBuff[5+6], &uiLen);
	if( iRet<0 )
	{
		return iRet;
	}

	usSendPacketLen = (uint16_t)(uiLen+5+6);

	if( HaveMacData(stTemp.iTransNo) )
	{
		if( CalMAC(&sSendPacketBuff[5+6], (int16_t)(uiLen-LEN_MAC_DATA), sMacData)!=OK )
		{
			return -1;
		}
		memcpy(&sSendPacketBuff[usSendPacketLen-8], sMacData, LEN_MAC_DATA);
	}

	return 0;
}

// 处理接收包，并检查公共数据要素
int ProcRspPacket(void)
{
	int16_t	iRet;
	int	lTemp1, lTemp2;
	uint8_t	sMacData[LEN_MAC_DATA];

	// 检查接收到的数据长度
//20080715串口测试不需要TPDU
	if( usRecvPacketLen<5+6+2+8 )
	{
		return -1;
	}

	BcdToAsc0(stTemp.sPacketHead, &sRecvPacketBuff[5], 6);
	memset(&glRecvPack, 0, sizeof(STISO8583));
	iRet = iUnPack8583(glMsg0, glData0, &sRecvPacketBuff[5+6], usRecvPacketLen-5-6, &glRecvPack);
	if( iRet<0 )
	{
		return -1;
	}

	SaveRspPackData();

	lTemp1 = atol((char *)glSendPack.szMsgCode);
	lTemp2 = atol((char *)glRecvPack.szMsgCode);
	if( lTemp2!=(lTemp1+10) )	// 检查消息码
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " 解包消息码不符 ", "MSGID ERROR");
		ErrorBeep();
		kbGetKeyMs(2000);		
		return -1;
	}

	if( glSendPack.szProcCode[0]!=0 &&
		memcmp(glSendPack.szProcCode, glRecvPack.szProcCode, LEN_PROC_CODE)!=0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " 解包处理码不符 ", "PROCODE ERROR");
		ErrorBeep();
		kbGetKey();		
		return -1;
	}

	if( glSendPack.szSTAN[0]!=0 &&
		memcmp(glSendPack.szSTAN, glRecvPack.szSTAN, LEN_POS_STAN)!=0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " 解包流水号不符 ", "TRACE ERROR");
		ErrorBeep();
		kbGetKey();		
		return -1;
	}

	if( memcmp(glSendPack.szTermID, glRecvPack.szTermID, LEN_TERM_ID)!=0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " 解包终端号不符 ", "TERMINAL ERROR");
		ErrorBeep();
		kbGetKey();		
		return -1;
	}
	if( (memcmp(glRecvPack.szRspCode, "00", 2)==0)  
		&& (memcmp(glSendPack.szMerchantID, glRecvPack.szMerchantID, LEN_MERCHANT_ID)!=0) )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, " 解包商户号不符 ", "MERCHANT ERROR");
		ErrorBeep();
		kbGetKey();		
		return -1;
	}

//20080716 Pboc脚本上送不要判断交易金额
	if( (stTemp.iTransNo!=UPLOAD_SCRIPT_RESULT && stTemp.iTransNo!=ICC_BATCHUP && stTemp.iTransNo!=ICC_FAIL_BATCHUP) && (glSendPack.szTranAmt[0]!=0) )
	{
		if ( !((stTemp.iTransNo == POS_SALE || stTemp.iTransNo == POS_LOYATL_ALL)
			&& (memcmp(glRecvPack.szRspCode, "10", 2)==0 )))
		{
			if( memcmp(glSendPack.szTranAmt, glRecvPack.szTranAmt, LEN_TRAN_AMT)!=0 )
			{
				lcdClrLine(2, 7);
				lcdFlip();	
				DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "解包交易金额不符", "AMOUNT ERROR");
				ErrorBeep();
				kbGetKey();		
				return -1;
			}
		}
	}

	// 只校验成功交易的MAC
	if( HaveMacData(stTemp.iTransNo)==FALSE ||
		((memcmp(glRecvPack.szRspCode, "00", LEN_RSP_CODE)!=0)) ) 
	{
		return 0;
	}
	if( CalMAC(&sRecvPacketBuff[5+6], usRecvPacketLen-5-6-8, sMacData)!=OK )
	{
		return -1;
	}
	
	if( memcmp(&glRecvPack.sMacData[2], sMacData, LEN_MAC_DATA)!=0 )
	{
		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "  解包校验不符  ");
		ErrorBeep();
		kbGetKey();
		return -2;  
	} 

	return 0;
}

uint8_t TranNeedReversal(int iTranType)
{
	if( iTranType==POS_SALE        || iTranType==POS_SALE_VOID    ||
		iTranType==POS_PREAUTH     || iTranType==POS_PREAUTH_VOID ||
		iTranType==POS_AUTH_CM     || iTranType==POS_AUTH_VOID    ||
		iTranType==POS_PREAUTH_ADD || iTranType == QPBOC_ONLINE_SALE ||
//		(iTranType>=EC_QUICK_SALE && iTranType<=EC_VOID_TOPUP) ||   // 电子现金消费和QPBOC不需要冲正,脱机的
        (iTranType>=EC_TOPUP_CASH && iTranType<=EC_VOID_TOPUP) ||     
		(iTranType>=ED_TOPUP_CASH && iTranType<=ED_TOPUP_NORAML) ||
		iTranType==POS_INSTALLMENT || iTranType==POS_VOID_INSTAL ||
		(iTranType>=POS_LOYATL_ISSUER && iTranType<=POS_REFUND_LOYATL_ALL)||
		iTranType==POS_MOBILE_SALE || iTranType==POS_MOBILE_VOID ||
		(iTranType>=POS_MOBILE_REFUND && iTranType<=POS_MOBILE_VOID_AUTH) ||
		iTranType==POS_RESER_SALE || iTranType==POS_RESER_VOID||
		(iTranType>=POS_MOTO_SALE && iTranType<=POS_MOTO_AUTH_OFF) )
//		iTranType==POS_MAGTOPUP_CASH || iTranType==POS_MAGTOPUP_ACCT )
	{
		return TRUE;
	}

	return FALSE;
}

uint8_t TranNeedIncTraceNo(int iTranType)
{
	if( iTranType==POS_SALE        || iTranType==POS_SALE_VOID    ||
		iTranType==POS_PREAUTH     || iTranType==POS_PREAUTH_VOID ||
		iTranType==POS_AUTH_CM     || iTranType==POS_AUTH_VOID    ||
		iTranType==POS_PREAUTH_ADD || iTranType == ICC_FAIL_BATCHUP ||
		iTranType==POS_REFUND      || iTranType==POS_QUE          ||
		iTranType==POS_LOGON       || iTranType==POS_LOGOFF       ||
		iTranType==POS_SETT        || iTranType==POS_BATCH_UP     ||
		iTranType==POS_OFF_CONFIRM || iTranType==UPLOAD_SCRIPT_RESULT ||
		iTranType==ICC_OFFSALE 	   || iTranType == QPBOC_ONLINE_SALE ||
		(iTranType>=EC_TOPUP_CASH && iTranType<=POS_LYT_LOGON) )
	{
		return TRUE;
	}
	
	return FALSE;
}

uint8_t HaveMacData(int iTranType)
{
	if( iTranType==POS_SALE        || iTranType==POS_SALE_VOID    ||
		iTranType==POS_PREAUTH     || iTranType==POS_PREAUTH_VOID ||
		iTranType==POS_AUTH_CM     || iTranType==POS_AUTH_VOID    ||
		iTranType==POS_PREAUTH_ADD || 
		iTranType==POS_REFUND      || iTranType==POS_QUE          ||
		iTranType==POS_REVERSE     || iTranType==OFF_SALE         ||
		iTranType==OFF_ADJUST      || iTranType==ADJUST_TIP       ||
		iTranType==POS_OFF_CONFIRM || iTranType==POS_REVERSE      ||
		iTranType==ICC_OFFSALE     || iTranType==UPLOAD_SCRIPT_RESULT ||
		(iTranType>=EC_QUICK_SALE && iTranType<=POS_QUERY_ACCT))
	{
		return TRUE;
	}

	return FALSE;
}

// 处理离线交易
uint8_t TrickleFeedOffLineTxn(uint8_t ucAllowBreak)
{
	uint8_t	ucRet;

/*	if( stPosParam.ucSendOffFlag==PARAM_CLOSE || stTemp.iTransNo==POS_SETT ) 
	{
		return OK;
	}*/

	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  离线交易上送  ", "  SEND OFFLINE  ");
	lcdClrLine(2, 7);
	DispMulLanguageString(0, 2, DISP_CFONT, "已连接银联中心  ", "CONNECT BANK");

	PosCom.stTrans.iTransNo = stTemp.iTransNo;
	memcpy(&PosCom.stOrgPack, &glSendPack, sizeof(glSendPack));	// 保存请求报文
	memcpy(&BackPosCom, &PosCom, COM_SIZE);
//20080730 上送完所有离线交易
	ucRet = SendOffLineTrans(ucAllowBreak, stPosParam.ucResendTimes);  // 交易之间上送一笔离线交易
	memcpy(&PosCom, &BackPosCom, COM_SIZE);
	memcpy(&glSendPack, &PosCom.stOrgPack, sizeof(glSendPack));	// 恢复请求报文
	stTemp.iTransNo = PosCom.stTrans.iTransNo;

	// 修改流水号
	PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;
	sprintf((char *)glSendPack.szSTAN, "%0*d", LEN_POS_STAN,  PosCom.stTrans.lTraceNo);

//	lcdCls();
//	lcdFlip();	

	return ucRet;
}

// 处理IC卡脱机交易
uint8_t TrickleFeedICOffLineTxn(uint8_t ucAllowBreak)
{
	uint8_t	ucRet;

/*	if( stPosParam.ucSendOffFlag==PARAM_CLOSE || stTemp.iTransNo==POS_SETT ) 
	{
		return OK;
	}*/

	DispMulLanguageString(0, 0, DISP_CFONT|DISP_REVERSE, "  脱机交易上送  ", "  SEND OFFLINE  ");
	lcdClrLine(2, 7);
	DispMulLanguageString(0, 2, DISP_CFONT, "已连接银联中心  ", "CONNECT BANK");

	PosCom.stTrans.iTransNo = stTemp.iTransNo;
	memcpy(&PosCom.stOrgPack, &glSendPack, sizeof(glSendPack));	// 保存请求报文
	memcpy(&BackPosCom, &PosCom, COM_SIZE);
//20080730 上送完所有脱机交易
	ucRet = SendICCOffLineTrans(ucAllowBreak, 0);
	memcpy(&PosCom, &BackPosCom, COM_SIZE);
	memcpy(&glSendPack, &PosCom.stOrgPack, sizeof(glSendPack));	// 恢复请求报文
	stTemp.iTransNo = PosCom.stTrans.iTransNo;
	// 修改流水号
	PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;
	sprintf((char *)glSendPack.szSTAN, "%0*d", LEN_POS_STAN,  PosCom.stTrans.lTraceNo);

//	lcdCls();
//	lcdFlip();	

	return ucRet;
}


void MakeReversalPack(void)
{
	memcpy(&glSendPack, &PosCom.stOrgPack, sizeof(glSendPack));

	sprintf((char *)glSendPack.szMsgCode, "0400");
	sprintf((char *)glSendPack.szAuthCode, "%s", PosCom.stTrans.szAuthNo);
	sprintf((char *)glSendPack.szRspCode,  "%s", PosCom.szRespCode);
	glSendPack.szTrack2[0] = 0;	// 规范要求不需要送track2/3
	glSendPack.szTrack3[0] = 0;
	memcpy(glSendPack.sPINData, "\x00\x00", 2);	// 删除安全相关的信息
	glSendPack.szCaptureCode[0]  = 0;
	glSendPack.szSecurityInfo[0] = 0;
	glSendPack.szRRN[0] = 0; 
	if( PosCom.stTrans.iTransNo==POS_SALE_VOID )
	{
		sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d", PosCom.stTrans.lBatchNumber,
				PosCom.stTrans.lTraceNo);
	}	
	if( PosCom.stTrans.iTransNo==POS_PREAUTH_VOID )
	{
		if( memcmp(&PosCom.stTrans.szDate[4], "0000", 4)!=0 )
		{
			sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%.4s", PosCom.stTrans.lBatchNumber,
					PosCom.stTrans.lTraceNo, &PosCom.stTrans.szDate[4]);
		}
		else
		{
			sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d0000", PosCom.stTrans.lBatchNumber,
					PosCom.stTrans.lTraceNo);
		}
	}
	if( PosCom.stTrans.iTransNo==POS_AUTH_CM )
	{
		if( memcmp(&PosCom.stTrans.szDate[4], "0000", 4)!=0 )
		{
			sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%.4s", PosCom.stTrans.lBatchNumber,
					PosCom.stTrans.lTraceNo, &PosCom.stTrans.szDate[4]);
		}
		else
		{
			sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d0000", PosCom.stTrans.lBatchNumber,
					PosCom.stTrans.lTraceNo);
		}
	}
	if( PosCom.stTrans.iTransNo==POS_AUTH_VOID )
	{
		if( memcmp(&PosCom.stTrans.szDate[4], "0000", 4)!=0 )
		{
			sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d%.4s", PosCom.stTrans.lBatchNumber,
					PosCom.stTrans.lTraceNo, &PosCom.stTrans.szDate[4]);
		}
		else
		{
			sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d0000", PosCom.stTrans.lBatchNumber,
					PosCom.stTrans.lTraceNo);
		}
	}
	if(PosCom.stTrans.iTransNo == POS_MOTO_PREAUTH_VOID)
	{
		sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d0000", PosCom.stTrans.lBatchNumber,
			PosCom.stTrans.lTraceNo);
	}
	if( PosCom.stTrans.iTransNo==POS_PREAUTH_ADD )
	{
		glSendPack.szOrginalMsg[0] = 0;
	}

	if( PosCom.stTrans.iTransNo==EC_VOID_TOPUP )
	{
		sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d", PosCom.stTrans.lBatchNumber,
			PosCom.stTrans.lTraceNo);
	}

	if( PosCom.stTrans.iTransNo==POS_VOID_INSTAL || PosCom.stTrans.iTransNo==POS_VOID_LOYATL_ISSUER 
		|| PosCom.stTrans.iTransNo==POS_VOID_LOYATL_ALL || PosCom.stTrans.iTransNo == POS_MOTO_SALE_VOID)
	{
		sprintf((char *)glSendPack.szOrginalMsg, "%06d%06d", PosCom.stTrans.lBatchNumber,
			PosCom.stTrans.lTraceNo);
	}

	if ((PosCom.stTrans.iTransNo>=POS_MOTO_PREAUTH && PosCom.stTrans.iTransNo <= POS_MOTO_AUTH_VOID)
		|| PosCom.stTrans.iTransNo==POS_MOTO_AUTH_OFF || PosCom.stTrans.iTransNo==POS_INSTALLMENT ||
		PosCom.stTrans.iTransNo== POS_VOID_INSTAL || PosCom.stTrans.iTransNo== POS_LOYATL_ALL ||
		PosCom.stTrans.iTransNo==POS_LOYATL_ISSUER)
	{
		memset(glSendPack.sField62,0,sizeof(glSendPack.sField62));		
	}

	if( PosCom.stTrans.iTransNo==POS_SALE || PosCom.stTrans.iTransNo==POS_QUE || PosCom.stTrans.iTransNo==POS_PREAUTH ||
		PosCom.stTrans.iTransNo== EC_TOPUP_CASH || PosCom.stTrans.iTransNo== EC_TOPUP_NORAML || PosCom.stTrans.iTransNo== EC_TOPUP_SPEC ||
		PosCom.stTrans.iTransNo== EC_VOID_TOPUP || PosCom.stTrans.iTransNo== QPBOC_ONLINE_SALE)
	{
		ShortToByte(glSendPack.sICCData, (uint16_t)PosCom.iReversalDE55Len);
		memcpy(&glSendPack.sICCData[2], PosCom.sReversalDE55, PosCom.iReversalDE55Len);
	}

	if( HaveMacData(stTemp.iTransNo) )
	{
		ShortToByte(glSendPack.sMacData, LEN_MAC_DATA);
	}
}

void SaveRspPackData(void)
{
	int16_t		iLen;
	char        szBuffer[20],szTemp[20];

	if (PosCom.stTrans.iTransNo != EC_TOPUP_NORAML)
	{
		sprintf((char *)PosCom.stTrans.szCardNo,       "%.19s", glRecvPack.szPan);
	}
	sprintf((char *)PosCom.szRespCode,             "%.2s",  glRecvPack.szRspCode);
	sprintf((char *)PosCom.stTrans.szTime,         "%.6s",  glRecvPack.szLocalTime);
	sprintf((char *)&PosCom.stTrans.szDate[4],     "%.4s",  glRecvPack.szLocalDate);
	sprintf((char *)PosCom.stTrans.szExpDate,      "%.4s",  glRecvPack.szExpDate);
	sprintf((char *)PosCom.stTrans.szSysReferNo,   "%.12s", glRecvPack.szRRN);
	if( glRecvPack.szAuthCode[0]!=0 )
	{	// 授权完成离线不返回authcode
		sprintf((char *)PosCom.stTrans.szAuthNo,   "%.6s",  glRecvPack.szAuthCode);
	}
	sprintf((char *)PosCom.stTrans.szIssuerBankId, "%.8s",  glRecvPack.szAddRspData);
	sprintf((char *)PosCom.stTrans.szRecvBankId,   "%.8s",  &glRecvPack.szAddRspData[11]);
	sprintf((char *)PosCom.stTrans.szCenterId,     "%.8s",  glRecvPack.szAcquirerID);
	
	if( stTemp.iTransNo==POS_SETT )
	{
		stTemp.ucRmbSettRsp = glRecvPack.szField48[30];
		stTemp.ucFrnSettRsp = glRecvPack.szField48[61];
	}
	
	if( stTemp.iTransNo!=POS_SETT && stTemp.iTransNo!=POS_LOGON )
	{
		sprintf((char *)PosCom.stTrans.szCardUnit, "%.3s", glRecvPack.szField63);
		iLen = strlen((char *)glRecvPack.szField63);
		if( iLen>=23 )
		{
			memcpy(PosCom.stTrans.szIssuerResp, &glRecvPack.szField63[3], 20);
			PosCom.stTrans.szIssuerResp[20] = 0;
		}
		if( iLen>=43 )
		{
			memcpy(PosCom.stTrans.szCenterResp, &glRecvPack.szField63[23], 20);
			PosCom.stTrans.szCenterResp[20] = 0;
		}
		if( iLen>=63 )
		{
			memcpy(PosCom.stTrans.szRecvBankResp, &glRecvPack.szField63[43], 20);
			PosCom.stTrans.szRecvBankResp[20] = 0;
		}
	}

	if( stTemp.iTransNo== POS_INSTALLMENT )
	{
		PubAsc2Bcd((char *)&glRecvPack.sField62[2],12,(char *)PosCom.stTrans.sTipAmount);    // 首期还款金额
		memcpy((char *)PosCom.stTrans.szInstalCurrCode,(char *)&glRecvPack.sField62[2+12],3);         // 还款货币种
		if (PosCom.stTrans.ucInstalPayMode == 0)
		{	
			PubAsc2Bcd((char *)&glRecvPack.sField62[2+15],12,(char *)PosCom.stTrans.sPreAddAmount);  // 持卡人分期付款手续费
		}
		else
		{
			memset(szTemp,0,sizeof(szTemp));
			memcpy(szTemp,(char *)&glRecvPack.sField62[2+15+12+12+1],12);
			PubTrimTailChars(szTemp,' ');
			PubTrimHeadChars(szTemp,' ');
			if (strlen(szTemp)==0)
			{
				PosCom.stTrans.ucInstalAmtFlag |= TS_INST_SPAC;
				memcpy((char *)PosCom.stTrans.sPreAddAmount,"\x00\x00\x00\x00\x00\x00",6);
			}
			else
			{
				PubAsc2Bcd((char *)&glRecvPack.sField62[2+15+12+12+1],12,(char *)PosCom.stTrans.sPreAddAmount);  // 持卡人分期付款首期手续费
			}

			memset(szTemp,0,sizeof(szTemp));
			memcpy(szTemp,(char *)&glRecvPack.sField62[2+15+12+12+1+12],12);
			PubTrimTailChars(szTemp,' ');
			PubTrimHeadChars(szTemp,' ');
			if (strlen(szTemp)==0)
			{
				PosCom.stTrans.ucInstalAmtFlag |= TS_FEE_SPAC;
				memcpy((char *)PosCom.stTrans.sLoyaltAmount,"\x00\x00\x00\x00\x00\x00",6);
			}
			else
			{
				PubAsc2Bcd((char *)&glRecvPack.sField62[2+15+12+12+1+12],12,(char *)PosCom.stTrans.sLoyaltAmount);  // 持卡人分期付款分期手续费
			}
			memset(PosCom.stTrans.szInstalIssurResp,0,sizeof(PosCom.stTrans.szInstalIssurResp));
			memcpy(PosCom.stTrans.szInstalIssurResp,(char *)&glRecvPack.sField62[2+15+12+12+1+12+12],13);
			memset(szBuffer,0,sizeof(szBuffer));
			PubBcd2Asc0(PosCom.stTrans.sLoyaltAmount,6,szBuffer);
		}
	}

	if( stTemp.iTransNo==POS_LOYATL_ALL || stTemp.iTransNo==POS_LOYATL_ISSUER )
	{
		PubAsc2Bcd((char *)&glRecvPack.sField62[2],10,(char *)PosCom.stTrans.sTipAmount);        // 兑换积分分数
		PubAsc2Bcd((char *)&glRecvPack.sField62[2+10],12,(char *)PosCom.stTrans.sPreAddAmount);  // 自付金金额
//		PubAsc2Bcd((char *)&glRecvPack.sField62[2+10+12],10,(char *)PosCom.stTrans.sLoyaltAmount);  // 积分余额
		// 积分余额取54域
		PosCom.stTrans.ucInstalAmtFlag = glRecvPack.szExtAmount[7];
		AscToBcd((char *)PosCom.stTrans.sLoyaltAmount, &glRecvPack.szExtAmount[8], 12);
	}
}

void IncreaseTraceNo(void)
{
	uint8_t	szBuff[LEN_POS_STAN+1];

	stPosParam.lNowTraceNo++;
	if( stPosParam.lNowTraceNo<=0 || stPosParam.lNowTraceNo>999999L )
	{
		stPosParam.lNowTraceNo = 1L;
	}
	sprintf((char*)szBuff, "%0*d", LEN_POS_STAN, stPosParam.lNowTraceNo);
	SaveAppParam();
}


// 调整通讯参数(只是设置Modem电话号码)
int AdjustCommParam(void)
{
	char  szTelNo[100];
	
	if (PosCom.stTrans.iTransNo == DOWNLOAD)
	{
		memcpy(&gstCurCommCfg,&stPosParam.stDownParamCommCfg,sizeof(ST_COMM_CONFIG));
	}
	else
	{
		memcpy(&gstCurCommCfg,&stPosParam.stTxnCommCfg,sizeof(ST_COMM_CONFIG));
	}
	if (gstCurCommCfg.ucCommType == CT_MODEM)// 调整外线号码
	{
		memset(szTelNo,0,sizeof(szTelNo));
		if (stPosParam.szPhoneNum1[0]!=0)
		{
			if (stTransCtrl.szExtNum[0]==0)
			{
				sprintf(szTelNo,"%s",stPosParam.szPhoneNum1);
			}
			else
			{
				sprintf(szTelNo,"%s,%s",stTransCtrl.szExtNum,stPosParam.szPhoneNum1);
			}
		}
		else
		{
			if (stPosParam.szPhoneNum2[0]!=0)
			{
				if (stTransCtrl.szExtNum[0]==0)
				{
					sprintf(szTelNo,"%s",stPosParam.szPhoneNum2);
				}
				else
				{
					sprintf(szTelNo,"%s,%s",stTransCtrl.szExtNum,stPosParam.szPhoneNum2);
				}
			}
			else
			{
				if (stTransCtrl.szExtNum[0]==0)
				{
					sprintf(szTelNo,"%s",stPosParam.szPhoneNum3);
				}
				else
				{
					sprintf(szTelNo,"%s,%s",stTransCtrl.szExtNum,stPosParam.szPhoneNum3);
				}
			}
		}

		if (PosCom.stTrans.iTransNo == DOWNLOAD)
		{
			sprintf(szTelNo,"%s",stPosParam.szDownloadTel);
		}
		
		strcpy((char *)gstCurCommCfg.stPSTNPara.szTxnTelNo,szTelNo);
	}
	
	return CommSetCfgParam(&gstCurCommCfg);
}


