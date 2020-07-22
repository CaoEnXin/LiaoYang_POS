
#include "posapi.h"
#include "posmacro.h"
#include "terstruc.h"
#include "glbvar.h"
#include "tranfunclist.h"
#include "posappapi.h"

extern uint8_t g_InputPinLogoBmp[190];


uint8_t EnterPIN(uint8_t ucFlag)
{
	uint8_t	 ucRet, szTmpBuff[25], sPan[17], szAmount[15];
	uint8_t	 ucKeyId, ucMode;
	int      iRet;
	uint32_t uiAmt=0;
	UINT8    szBuf[32];

	uiAmt=0;
	if (PosCom.ucSwipedFlag == CARD_PASSIVE)
	{
		if (!(PosCom.stTrans.iTransNo>=POS_MOBILE_SALE && PosCom.stTrans.iTransNo<=POS_MOBILE_QUE)
			&& PosCom.stTrans.iTransNo != QPBOC_ONLINE_SALE)
		{
			return OK;
		}
	}

//printf("ucFlag=%02X\n", ucFlag);
	if( (ucFlag & 0x80)==0x00 )
	{
		if( !NeedPIN(stTemp.iTransNo) )
		{
			return OK;
		}
#ifndef PBOC_VERSION
		if( PosCom.ucSwipedFlag==CARD_INSERTED )
		{
			return OK;
		}
#endif
	}

//printf("11111\n");
	if( (ucFlag & 0x80)!=0x00 )
	{
		if( !ICNeedPIN(stTemp.iTransNo) )
		{
			return OK;
		}
	}
//printf("11112\n");
	
//20080716 Pboc测试不要清除bit 8
//	ucFlag &= 0x7F;	// clear bit 8
		
	lcdClrLine(2, 7);
	lcdFlip();	
	memset(szAmount,0,sizeof(szAmount));
	ConvBcdAmount(PosCom.stTrans.sAmount, szAmount);
	do{
		iRet = PedGetSensitiveTimer(TYPE_PIN_INTERVAL);
		if( iRet>0 )
		{
			if( stPosParam.iDispLanguage==1 )
				sprintf(szBuf, "请稍后...");
			else
				sprintf(szBuf, "WAITING...");
			
			lcdDisplay(0, 2, DISP_CFONT|DISP_CLRLINE, szBuf);
		}
		else
		{
			lcdClrLine(2, 7);
			//20080716 增加Pboc的判断
			if( ucFlag==0 || ucFlag==0x80 )
			{
				if( stTemp.iTransNo!=POS_QUE && stTemp.iTransNo != POS_QUE_LOYATL_ALL &&
					stTemp.iTransNo != POS_MOBILE_QUE)
				{
					if( stPosParam.iDispLanguage==1 )
						sprintf((char *)szTmpBuff, "%s元", szAmount);
					else
						sprintf((char *)szTmpBuff, "%s", szAmount);
					lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, szTmpBuff);
				}
			}
			else
			{
				DispMulLanguageString(0, 2, DISP_CFONT|DISP_MEDIACY, "(55)密码错请重试", "PWD ERR,REINPUT");
			}
			
			if (stTemp.iTransNo == EC_TOPUP_NORAML )
			{
				DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "请输入转出卡密码", "INPUT PWD:");
			}
			else
			{
				DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "请持卡人输入密码", "INPUT PWD:");
			}
			break;
		}		
	}while(1);

	if (PosCom.stTrans.iTransNo == POS_RESER_SALE || PosCom.stTrans.iTransNo == POS_RESER_VOID ||
		(PosCom.stTrans.iTransNo>=POS_MOBILE_SALE && PosCom.stTrans.iTransNo <= POS_MOBILE_QUE))
	{
		strcpy((char*)szTmpBuff,"0000000000000000000");
		ucRet = ExtractPAN(szTmpBuff, sPan);
	}
	else
	{
		if (PosCom.stTrans.iTransNo == EC_TOPUP_NORAML)
		{
			ucRet = ExtractPAN(PosCom.stTrans.szTranferCardNo, sPan);
		}
		else
		{
			ucRet = ExtractPAN(PosCom.stTrans.szCardNo, sPan);
		}
	}
	if( ucRet!=OK )
	{
		return ucRet;
	}

	if( stPosParam.ucKeyMode==KEY_MODE_DES )
	{
		ucKeyId = PIN_KEY_ID;
		ucMode  = 0x01;
	}
	else
	{
		ucKeyId = DOUBLE_PIN_KEY_ID;
		ucMode  = 0x31;
	}

	lcdGoto(32, 6);
	iRet = PedGetPin(ucKeyId, "0, 4, 5, 6, 7, 8, 9, 10, 11, 12", sPan, FORMAT_0, PosCom.sPIN);
	switch( iRet ){
	case PED_RET_OK:
		PosCom.ucPinEntry = TRUE;
		return OK;
	case PED_RET_NOPIN:
		if( (ucFlag&0x80)!=0x00 )
		{
			return PED_RET_NOPIN;
		}
		else
		{
			return OK;
		}
	case PED_RET_CANCEL:
	case PED_RET_TIMEOUT:
		return E_TRANS_CANCEL;
	case PED_RET_NOKEY:
		return PED_RET_NOKEY;
	default:
		return E_PINPAD_KEY;
	}
}

uint8_t DispBalance(void)
{
	UINT8	sBalance[6], szBuff[20]; 
	UINT8   szBuf[17];
	INT32   iLen;
	
	// 增加了金额正负的标志之后，再行补充实现
	AscToBcd(sBalance, &glRecvPack.szExtAmount[8], 12);
	lcdClrLine(2, 7);
	DispMulLanguageString(0, 2, DISP_MEDIACY|DISP_CFONT, "    查询成功    ", " QUERY SUCCESS  ");
	
	memset(szBuff, 0, sizeof(szBuff));
	szBuff[0] = (glRecvPack.szExtAmount[7]!='C') ? '-' : ' ';
	ConvBcdAmount(sBalance, &szBuff[1]);
	DispMulLanguageString(0, 4, DISP_CFONT, "可用余额:", "BALANCE:");
	memset(szBuf, 0x20, 16);
	iLen = strlen(szBuff);
	memcpy(&szBuf[16-iLen], szBuff, iLen);
	szBuf[16] = 0;
	lcdDisplay(0, 6, DISP_CFONT, szBuf);
	PubBeepErr();
	
	kbGetKeyMs(10000);		// 等待4秒钟
	
	return (OK);
}

uint8_t CalMAC(uint8_t * inbuf, int len, uint8_t * outbuf)
{
	uint8_t buf[17], tmpbuf[17];
	int     i, l, k, ret;
	uint8_t KeyId;

	if( stPosParam.ucKeyMode==KEY_MODE_DES )
		KeyId = MAC_KEY_ID;
	else
		KeyId = DOUBLE_MAC_KEY_ID;

	memset(buf, 0, 8);
	memset(inbuf+len, 0, 8);

	if( len%8 ) 
		l = len/8 + 1;		
	else 
		l = len/8;
	
	for(i=0; i<l; i++) 
	{
		for(k=0; k<8; k++) 
		{
			buf[k] = buf[k] ^ inbuf[i*8+k];
		}
		buf[8] = '\0';
	}
	
	BcdToAsc(tmpbuf, buf, 16);
	tmpbuf[16] = 0;
	
	memcpy(buf, tmpbuf, 8);
	buf[8] = 0;
	
	ret = PedGetMac(KeyId, buf, 8, MAC_MODE_1, buf);
	if( ret!=PED_RET_OK )
			return(1);
	
	for(k=0; k<8; k++) 
	{
		buf[k] = buf[k] ^ tmpbuf[8+k];
	}

	ret = PedGetMac(KeyId, buf, 8, MAC_MODE_1, buf);
	if( ret!=PED_RET_OK )
		return(1);
	
	BcdToAsc(tmpbuf, buf, 16);
	
	memcpy(outbuf, tmpbuf, 8);
	outbuf[8] = '\0';
	return (OK);	
}


uint8_t ExtractPAN(uint8_t* cardno, uint8_t* pan)
{
	int16_t	len;
	
	len = strlen((char*)cardno);
	if( len<13 || len>19 ) 
		return E_ERR_SWIPE;
	
	memset(pan, '0', 16);
	
	memcpy(pan+4, cardno+len-13, 12);
	pan[16] = 0;
	
	return OK;
}

uint8_t LogonDivKey(uint8_t *pKeyData)
{
	if( stPosParam.ucKeyMode==KEY_MODE_DES )
	{
		uint8_t sTempBuff[17], sTemp[17];
		int     ret, ucKeyId;
		
		PedDelKey(TYPE_MACKEY, TEST_KEY_ID);
		PedDelKey(TYPE_PINKEY, PIN_KEY_ID);
		PedDelKey(TYPE_MACKEY, MAC_KEY_ID);
		PedDelKey(TYPE_PINKEY, DOUBLE_PIN_KEY_ID);
		PedDelKey(TYPE_MACKEY, DOUBLE_MAC_KEY_ID);
		PedDelKey(TYPE_MACKEY, DOUBLE_TDK_KEY_ID);

		memcpy(sTempBuff, pKeyData, 8);
		memcpy(sTempBuff+8, pKeyData, 8);
		ucKeyId = stPosParam.ucKeyIndex + 1;
		ret = PedWriteMacKey(PARITY_NONE, ucKeyId, TEST_KEY_ID, 16, sTempBuff);
		if( ret!=PED_RET_OK )
		{
			return (E_PINPAD_KEY);
		}


		memset(sTemp, 0x00, 8);
		ret = PedGetMac(TEST_KEY_ID, sTemp, 8, MAC_MODE_1, sTemp);
		if( ret!=PED_RET_OK ) 
		{
			return(0xff);
		}
		if( memcmp(sTemp, pKeyData+8, 4)!=0 )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "解包密钥验证不符", "KEY VERIFY ERR");
			ErrorBeep();
			kbGetKeyMs(3000);

			return (0xff);
		}

		memcpy(sTempBuff, pKeyData+12, 8);
		memcpy(sTempBuff+8, pKeyData+12, 8);
		ret = PedWriteMacKey(PARITY_NONE, ucKeyId, TEST_KEY_ID, 16, sTempBuff);
		if( ret!=PED_RET_OK )
		{
			return (E_PINPAD_KEY);
		}

		memset(sTemp, 0, 8);
		ret = PedGetMac(TEST_KEY_ID, sTemp, 8, MAC_MODE_1, sTemp);
		if( ret!=PED_RET_OK ) 
		{
			return(0xff);
		}

		if( memcmp(sTemp, pKeyData+20, 4)!=0 )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "解包密钥验证不符", "KEY VERIFY ERR");
			ErrorBeep();
			kbGetKeyMs(3000);
			return (0xff);
		}
		
		memcpy(sTempBuff, pKeyData, 8);
		memcpy(sTempBuff+8, pKeyData, 8);
		ret = PedWritePinKey(PARITY_NONE, ucKeyId, PIN_KEY_ID, 16, sTempBuff);
		if( ret!=PED_RET_OK ) 
		{
			return (E_PINPAD_KEY);
		}
		
		memcpy(sTempBuff, pKeyData+12, 8);
		memcpy(sTempBuff+8, pKeyData+12, 8);
		ret = PedWriteMacKey(PARITY_NONE, ucKeyId, MAC_KEY_ID, 16, sTempBuff);
		if( ret!=PED_RET_OK ) 
		{
			return (E_PINPAD_KEY);
		}
	}
	else
	{
				
		uint8_t sTempBuff[32+1], sTemp[32+1];
		int  ret, ucKeyId;

		PedDelKey(TYPE_MACKEY, TEST_KEY_ID);
		PedDelKey(TYPE_PINKEY, PIN_KEY_ID);
		PedDelKey(TYPE_MACKEY, MAC_KEY_ID);
		PedDelKey(TYPE_PINKEY, DOUBLE_PIN_KEY_ID);
		PedDelKey(TYPE_MACKEY, DOUBLE_MAC_KEY_ID);
		PedDelKey(TYPE_MACKEY, DOUBLE_TDK_KEY_ID);
				
		memcpy(sTempBuff, pKeyData, 16);
		ucKeyId = stPosParam.ucKeyIndex + 1;
		ret = PedWriteMacKey(PARITY_NONE, ucKeyId, TEST_KEY_ID, 16, sTempBuff);
		if( ret!=PED_RET_OK )
		{
			return (E_PINPAD_KEY);
		}
		
		memset(sTemp, 0, 8);
		ret = PedGetMac(TEST_KEY_ID, sTemp, 8, MAC_MODE_1, sTemp);
		if( ret!=PED_RET_OK ) 
			return(0xff);

		if (memcmp(sTemp, pKeyData+16, 4)!=0 )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "解包PKEY验证不符", "PKEY VERIFY ERR");
			ErrorBeep();
			kbGetKeyMs(3000);
			return (0xff);
		}
//20080730
		memcpy(sTempBuff, pKeyData+20, 8);
		memcpy(sTempBuff+8, pKeyData+20, 8);
		ret = PedWriteMacKey(PARITY_NONE, ucKeyId, TEST_KEY_ID, 16, sTempBuff);
		if( ret!=PED_RET_OK )
			return (E_PINPAD_KEY);

		memset(sTemp, 0, 8);
		ret = PedGetMac(TEST_KEY_ID, sTemp, 8, MAC_MODE_1, sTemp);
		if( ret!=PED_RET_OK ) 
			return(0xff);

		if( memcmp(sTemp, pKeyData+36, 4)!=0 )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "解包MKEY验证不符", "MKEY VERIFY ERR");
			ErrorBeep();
			kbGetKeyMs(3000);

			return (0xff);
		}

		// 验证TDK
		memcpy(sTempBuff, pKeyData+40, 16);
		ucKeyId = stPosParam.ucKeyIndex + 1;
		ret = PedWriteMacKey(PARITY_NONE, ucKeyId, TEST_KEY_ID, 16, sTempBuff);
		if( ret!=PED_RET_OK )
			return (E_PINPAD_KEY);
		
		memset(sTemp, 0, 8);
		ret = PedGetMac(TEST_KEY_ID, sTemp, 8, MAC_MODE_1, sTemp);
		if( ret!=PED_RET_OK ) 
			return(0xff);
		
		if (memcmp(sTemp, pKeyData+56, 4)!=0 )
		{
			lcdClrLine(2, 7);
			lcdFlip();	
			DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "解包TKEY验证不符", "PKEY VERIFY ERR");
			ErrorBeep();
			kbGetKeyMs(3000);
			return (0xff);
		}

		memcpy(sTempBuff, pKeyData, 16);
		ret = PedWritePinKey(PARITY_NONE, ucKeyId, DOUBLE_PIN_KEY_ID, 16, sTempBuff);
		if( ret!=PED_RET_OK ) 
				return (E_PINPAD_KEY);

//20080730
		memcpy(sTempBuff, pKeyData+20, 8);
		memcpy(sTempBuff+8, pKeyData+20, 8);
		ret = PedWriteMacKey(PARITY_NONE, ucKeyId, DOUBLE_MAC_KEY_ID, 16, sTempBuff);
		if( ret!=PED_RET_OK ) 
			return (E_PINPAD_KEY);
		
		memcpy(sTempBuff, pKeyData+40, 16);			
		ret = PedWriteMacKey(PARITY_NONE, ucKeyId, DOUBLE_TDK_KEY_ID, 16, sTempBuff);
		if( ret!=PED_RET_OK ) 
			return (E_PINPAD_KEY);
	}

	return OK;
}


uint8_t DisplayLoyaltBalance(void)
{
	uint8_t	sBalance[6], szBuff[20]; 
	uint8_t szBuf[17];
	int     iLen;
	uint32_t uiBalance;
	
	// 增加了金额正负的标志之后，再行补充实现
	AscToBcd(sBalance, &glRecvPack.szExtAmount[8], 12);
	memset(szBuff, 0, sizeof(szBuff));
	szBuff[0] = (glRecvPack.szExtAmount[7]!='C') ? '-' : ' ';
	uiBalance = PubBcd2Long(sBalance,6,NULL);
//	ConvBcdAmount(sBalance, &szBuff[1]);
	sprintf((char*)&szBuff[1],"%d",uiBalance);
	
	lcdClrLine(2, 7);
	lcdFlip();	
	DispMulLanguageString(0, 2, DISP_MEDIACY|DISP_CFONT, "    查询成功    ", " QUERY SUCCESS  ");
	DispMulLanguageString(0, 4, DISP_CFONT, "可用积分:", "BALANCE:");
	memset(szBuf, 0x20, 16);
	iLen = strlen((char *)szBuff);
	memcpy(&szBuf[16-iLen-1], szBuff, iLen);
	szBuf[15] = 0;
	lcdDisplay(0, 6, DISP_CFONT, (char *)szBuf);
	PubBeepErr();
	
	kbGetKeyMs(10000);		// 等待4秒钟	

	return (OK);
}

// end of file 
