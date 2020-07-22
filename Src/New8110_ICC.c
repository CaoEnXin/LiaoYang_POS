#include "New8110_ICC.h"
#include <posapi.h>

extern int    imif_fd;

int New8110_icc_Init(void)
{
	int Ret = iccSetPara(ICC_USERCARD,ICC_VOL_5V,ICC_EMV);
	if(Ret){
		return -1;
	}
	return Ret;
}
int New8110_icc_powerup(int * plen, uint8_t * atr)
{
	int Ret = 0;
//	char szTemp[255];
	Ret = iccPowerUp(ICC_USERCARD, (uint32_t*)plen, atr);
	//sprintf(szTemp, "iccPowerUp() return %d", Ret);
	//printmsg(szTemp);
	if(Ret != 0){
		return -1;
	}else{			
		return 0;
	}
}
int New8110_icc_poweroff(void)
{
	iccPowerDown(ICC_USERCARD);
	return 0;
}
int New8110_icc_exchange_apdu(int fd, uint32_t sendlen, const void *senddata, uint32_t *recvlen, void *recvdata)
{
	APDU_SEND apdu_s;
	APDU_RESP apdu_r;
	INT32 Ret=0;	
	if (sendlen <4)
		return -1;
	memset(&apdu_s, 0, sizeof(apdu_s));
	memcpy(&apdu_s.CLA, senddata, 4);
	if (sendlen == 5) //Has Le, no lc and data CLA INS P1 P2 LE
	{
		memcpy(&apdu_s.Le, senddata +4, 1);
		apdu_s.LeFlag = 1;	
	}
	else if (sendlen >5) // CLA INS P1 P2 LC Data (LE)
	{
		memcpy(&apdu_s.Lc, senddata + 4, 1);
		if (apdu_s.Lc)
		{
			memcpy(apdu_s.DataIn, senddata +5, apdu_s.Lc);
		}

		if (apdu_s.Lc + 5 < sendlen) //There maybe has le
		{
			memcpy(&apdu_s.Le, senddata + 5 + apdu_s.Lc, 1);
			apdu_s.LeFlag = 1;		
		}
	}
	
	Ret = iccExchangeAPDU(0,&apdu_s,&apdu_r);
//	dbg_printf_line("iccExchangeApdu() return %d", Ret);
	if(Ret){
		Ret = -1;
	}	
	else
	{
		*recvlen  = apdu_r.LenOut;
		memcpy(recvdata, apdu_r.DataOut, apdu_r.LenOut);
		
		memcpy(recvdata + *recvlen, &apdu_r.SWA, 1);
		*recvlen = *recvlen + 1;
		memcpy(recvdata + *recvlen, &apdu_r.SWB, 1);
		*recvlen = *recvlen + 1;
	}
	return Ret;
}


int contactness_open(void)
{
	return mif_open("/dev/nfc");
}

int contactness_close(int icc_fd)
{
	int  iRet;
	
	if (icc_fd < 0)
	{
		return 0;
	}

//	emv_contactless_deactive_picc(icc_fd); // 先deactive,然后再关闭
	iRet = mif_close(icc_fd);
	return iRet;
}

int contactness_poll(int icc_fd)
{
/*	int   iRet, iCardInfoLen, iOldTime, iNowTime;
    UINT8  szCardInfo[256],szCommand[10];
	
	kbFlush();
	
	memset(szCardInfo, 0x00, sizeof(szCardInfo));
	memcpy(szCommand,"\x00\xFF\xFF\x01\x00",5);
	iOldTime = sysGetTimerCount();
	while(1)
	{
		// 检测感应区内是否存在多张卡片
		iRet = mifCheckOnlyOneCard(0x07);
		if (iRet == MIF_RET_MULTICARD)
		{
			return MIF_RET_MULTICARD;
		}
        else if ((MIF_RET_SUCCESS!=iRet) && (MIF_RET_NOCARD!=iRet) && (MIF_RET_TIMEOUT!=iRet))
		{
			return iRet;
		}
		
        // 在感应区内查找指定调制类型的卡片，并返回卡片的序列号等相关信息，106kbps typeA
		iRet = InListPassiveTarget(0,0,szCommand,&iCardInfoLen, szCardInfo);
		if( iRet==MIF_RET_SUCCESS )
		{
			// 激活卡片内已处于ACTIVE态的T=CL卡片，并使之进入ISO14443-4协议态
			iRet = InATR(&iCardInfoLen,szCardInfo);
			if (iRet != 0)
			{
				continue;
			}
			break;
		}
		
        // 在感应区内查找指定调制类型的卡片，并返回卡片的序列号等相关信息，106kbps typeB
		iRet = InListPassiveTarget(3,1,szCommand,&iCardInfoLen, szCardInfo);
		if( iRet==MIF_RET_SUCCESS )
		{
			iRet = InATR(&iCardInfoLen,szCardInfo);
			if (iRet != 0)
			{
				continue;
			}
			break;
		}
		
        // 在感应区内查找指定调制类型的卡片，并返回卡片的序列号等相关信息，212kbps
		iRet = InListPassiveTarget(1,5,szCommand,&iCardInfoLen, szCardInfo);
		if( iRet==MIF_RET_SUCCESS )
		{
			break;
		}
		
		if( kbhit()==YES )
		{
			iRet = kbGetKey();
			if( iRet==KEY_CANCEL )
			{
				return MIF_RET_NOCARD;
			}
		}
		
		iNowTime = sysGetTimerCount();
		if ((iNowTime-iOldTime) >= 1000)
		{
			return MIF_RET_TIMEOUT;
		}
	}
	
	return 0; */
	return emv_contactless_active_picc(icc_fd); 
//	return ioctl(fd, PN512_IOC_T_REMOVE);
}

int contactness_poweredon(int icc_fd)
{
/*	int   iRet, iCardInfoLen, iOldTime;
    UINT8  szCardInfo[256],szCommand[10];
		
	memset(szCardInfo, 0x00, sizeof(szCardInfo));
	memcpy(szCommand,"\x00\xFF\xFF\x01\x00",5);
	iOldTime = sysGetTimerCount();
	while(1)
	{
		// 检测感应区内是否存在多张卡片
		iRet = mifCheckOnlyOneCard(0x07);
		if (iRet == MIF_RET_MULTICARD)
		{
			return MIF_RET_MULTICARD;
		}
        else if ((MIF_RET_SUCCESS!=iRet) && (MIF_RET_NOCARD!=iRet) && (MIF_RET_TIMEOUT!=iRet))
		{
			return 1;
		}
		
        // 在感应区内查找指定调制类型的卡片，并返回卡片的序列号等相关信息，106kbps typeA
		iRet = InListPassiveTarget(0,0,szCommand,&iCardInfoLen, szCardInfo);
		if( iRet==MIF_RET_SUCCESS )
		{
			// 激活卡片内已处于ACTIVE态的T=CL卡片，并使之进入ISO14443-4协议态
			iRet = InATR(&iCardInfoLen,szCardInfo);
			if (iRet != 0)
			{
				continue;
			}
			break;
		}
		
        // 在感应区内查找指定调制类型的卡片，并返回卡片的序列号等相关信息，106kbps typeB
		iRet = InListPassiveTarget(3,1,szCommand,&iCardInfoLen, szCardInfo);
		if( iRet==MIF_RET_SUCCESS )
		{
			iRet = InATR(&iCardInfoLen,szCardInfo);
			if (iRet != 0)
			{
				continue;
			}
			break;
		}
		
        // 在感应区内查找指定调制类型的卡片，并返回卡片的序列号等相关信息，212kbps
		iRet = InListPassiveTarget(1,5,szCommand,&iCardInfoLen, szCardInfo);
		if( iRet==MIF_RET_SUCCESS )
		{
			break;
		}
		
		return 1;
	}
	
	return 0; */

/*	int iret = 0;
	int status;
	int error;
	static int multicard_times = 0;
	status = NFC_OP_POLL;
	
//	iret = ioctl(icc_fd, PN512_IOC_G_OPERATE_STATE, &status);
	
	emv_contactless_obtain_status(icc_fd, &status);
	
	if (iret == 0)
	{
		if (status != NFC_OP_EXCHANGE_APDU)
		{
			emv_contactless_get_lasterror(icc_fd, &error);
			if (EMULTIHOP == error)
			{
				multicard_times ++;
				if (multicard_times > 3)
				{
					iret = -1;				
				}
				else
				{
					iret = 1;
				}
			}
			else
			{
				iret = 1;
				multicard_times = 0;
			}
						
		}
		else
		{
			multicard_times = 0;
		}
	}
	return iret;*/
	int status = 0;
	int lasterror;
	
	emv_contactless_get_lasterror(icc_fd, &lasterror);
	
	if (NFC_ERR_ENODATA == lasterror)
		return MIF_RET_NOCARD;
	else if (NFC_ERR_EMULTIHOP == lasterror)
		return MIF_RET_MULTICARD;
	else if (NFC_ERR_NONE == lasterror)
	{
		emv_contactless_obtain_status(icc_fd, &status);	
		if (NFC_OP_EXCHANGE_APDU == status)
		{
			return MIF_RET_SUCCESS;
		}
		else
		{
			return MIF_RET_NOCARD;
		}
	}
	return MIF_RET_NOCARD;
}

int contactness_poweroff(int fd)
{
	return emv_contactless_deactive_picc(fd); 
}

int contactness_exchangeapdu(int icc_fd, uint32_t sendlen, const void *senddata, uint32_t *recvlen, void *recvdata)
{
//	return InDataExchange(sendlen, (UINT8*)senddata, recvlen, (UINT8*)recvdata);
	return emv_contactless_exchange_apdu(icc_fd, sendlen, senddata, recvlen, recvdata);
}




