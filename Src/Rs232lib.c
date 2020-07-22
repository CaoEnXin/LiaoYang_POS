
#include "Rs232lib.h"
#include "Glbvar.h"

/********************** Internal macros declaration ************************/
/********************** Internal structure declaration *********************/
/********************** Internal functions declaration *********************/
/********************** Internal variables declaration *********************/
static int          sg_PortFd =  -1 ;
/********************** external reference declaration *********************/
/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

//   打开串口
int  Rs232Open(int iportNum, char *pszPortPara)
{
	int  iRet;

	iRet = portOpen(iportNum,pszPortPara);
	if (iRet == PORT_RET_OK)
	{
		sg_PortFd = iportNum;	
	}
	else
	{
		iRet = ERR_RS232_FAILURE;
	}

	return iRet;
}

//   关闭串口
int  Rs232Close(void)
{
	int  iRet;
	
	iRet = portClose(sg_PortFd);
	if (iRet != PORT_RET_OK)
	{
		iRet = ERR_RS232_FAILURE;
	}
	
	return iRet;
}

//   检测串口缓冲区是否有数据,uiFlag 表示发送还是接收缓冲区
int  Rs232CheckPurge(unsigned int uiFlag)
{
	int  iRet = 0;
	
	if (uiFlag & RS232_TX_PURGE)
	{
		iRet = portCheckSendBuf(sg_PortFd);
	}

	if (uiFlag & RS232_RX_PURGE)
	{
		iRet = portCheckRecvBuf(sg_PortFd);
	}
	
	if (iRet != PORT_RET_NOTEMPTY)
	{
		iRet = ERR_RS232_FAILURE;
	}
	else
	{
		iRet = ERR_RS232_OK;
	}

	return iRet;
}

//   检测串口缓冲区是否有数据, uiFlag 表示发送还是接收缓冲区
int  Rs232ResetPurge(unsigned int uiFlag)
{
	int  iRet;
	
	iRet = portReset(sg_PortFd);
	if (iRet != PORT_RET_OK)
	{
		iRet = ERR_RS232_FAILURE;
	}
	
	return iRet;
}


//   串口发送数据  
int  MainPosRS232Txd(unsigned char*psTxdData, unsigned int uiDataLen, unsigned int uiTimeOut)
{
	int 	iRet;
	int     iOldTime,iNowTime;
	
	if (uiTimeOut<=0)
	{
		uiTimeOut = 2;
	}
	iOldTime = sysGetTimerCount();
	iRet = 0;
	while (1)
	{
		iNowTime = sysGetTimerCount();
		if ((iNowTime-iOldTime) >= uiTimeOut*1000)
		{
			break;
		}
		
		iRet = portSends(sg_PortFd, psTxdData,uiDataLen);
		if( iRet == PORT_RET_OK )
		{
			break;
		}
		else
		{
			iRet = ERR_RS232_FAILURE;
		}
		sysDelayMs(50);
	}
	
	return iRet;
}


//   串口接收数据
int  MainPosRS232Rxd(unsigned char *psRxdData, unsigned int uiExpLen, unsigned int uiTimeOut, unsigned int *puiOutLen)
{
	int             iRet;
	unsigned int 	uiReadCnt, uiLength;
	unsigned int    iOldTimeMs,iNewTimeMs;
	unsigned short  usCRCRet;
	
	uiReadCnt = uiLength = 0;
	iOldTimeMs = sysGetTimerCount();
	while( 1 )
	{
		// 检查是否超时
		iNewTimeMs= sysGetTimerCount();
		if( (iNewTimeMs-iOldTimeMs) >= (unsigned int)(uiTimeOut *1000) )	// 检查定时器
		{
			if( uiReadCnt>0 )	// 已经读取到数据
			{
				break;
			}			
			return ERR_RS232_RX_TIMEOUT;
		}
		// 检查接收缓冲区是否有数据		
		iRet = portCheckRecvBuf(sg_PortFd);
		if (iRet != PORT_RET_NOTEMPTY)
		{
			if (iRet != PORT_RET_OK )
			{				
				return ERR_RS232_RX_DATA;
			}			
			continue;
		}
		
		uiReadCnt = 0;
		// 报文头
		iRet = portRecv(sg_PortFd, &psRxdData[uiReadCnt], 1000);  // 延长到1s，原来是100ms
		if( iRet!=0 )
		{
			if( iRet==0xFF )
			{
				continue;
			}
			return ERR_RS232_RX_DATA;
		}
		if( psRxdData[0]!=STX )
		{
			continue;
		}
		uiReadCnt++;

		// 接收2个字节的命令码 和 2 字节数据长度
		for (; uiReadCnt < 5; uiReadCnt++)
		{
			iRet = portRecv(sg_PortFd, &psRxdData[uiReadCnt], 100);
			if( iRet!=0 )
			{
				return ERR_RS232_RX_DATA;
			}
		}

		// 数据长度
		uiLength = PubChar2Long(&psRxdData[uiReadCnt-2],2,NULL);
		if (uiLength > uiExpLen)
		{
			return ERR_RS232_RX_LENGTH; // 数据超长
		}

		// 接收N个数据长度+ 2字节 CRC校验数据
		uiLength += 5+2;
		for (; uiReadCnt < uiLength; uiReadCnt++)
		{
			iRet = portRecv(sg_PortFd, &psRxdData[uiReadCnt], 100);
			if( iRet!=0 )
			{
				return ERR_RS232_RX_DATA;
			}
		}

		// 计算CRC结果: 从命令码开始到CRC数据之前的所有数据
		//母POS不校验CRC，接收了以后，由接收函数负责校对
		usCRCRet = PubGetCRC((char *)(psRxdData+1),uiReadCnt-3);//,0x0000);
		uiLength = PubChar2Long(psRxdData+uiReadCnt-2,2,NULL);
		if (usCRCRet != uiLength)
		{
			return ERR_RS232_RX_CHECKSUM;
		}
		
		break;
	}
	
	if( puiOutLen!=NULL )
	{
		*puiOutLen = uiReadCnt;
	}
	
	return ERR_RS232_OK;
}  

/*
//   串口接收数据, 在内蒙农信社的测试机上，由于出现了0x0d转义字符，导致收到的数据多了一个字节0x00，
//   故此版本的所有的数据都进行了bcd-->ascii的转换
int  RS232Rxd(unsigned char *psRxdData, unsigned int uiExpLen, unsigned int uiTimeOut, unsigned int *puiOutLen)
{
	int             iRet;
	unsigned int 	uiReadCnt, uiLength;
	unsigned int    iOldTimeMs,iNewTimeMs;
	unsigned short  usCRCRet;
	unsigned char	sContent[LEN_MAX_COMM_DATA+10], szTemp[4];
	
	uiReadCnt = uiLength = 0;
	iOldTimeMs = sysGetTimerCount();
	memset(sContent,0,sizeof(sContent));
	while( 1 )
	{
		// 检查是否超时
		iNewTimeMs= sysGetTimerCount();
		if( (iNewTimeMs-iOldTimeMs) >= (UINT32)(uiTimeOut *1000) )	// 检查定时器
		{
			if( uiReadCnt>0 )	// 已经读取到数据
			{
				break;
			}			
			return ERR_RS232_RX_TIMEOUT;
		}
		// 检查接收缓冲区是否有数据		
		iRet = portCheckRecvBuf(sg_PortFd);
		if (iRet != PORT_RET_NOTEMPTY)
		{
			if (iRet != PORT_RET_OK )
			{
				return ERR_RS232_RX_DATA;
			}			
			continue;
		}
		
		uiReadCnt = 0;
		// 报文头
		for (; uiReadCnt < 2; uiReadCnt++)
		{
			iRet = portRecv(sg_PortFd, &sContent[uiReadCnt], 1000);
			if( iRet!=0 )
			{
				if( iRet==0xFF )
				{
					continue;
				}
				return ERR_RS232_RX_DATA;
			}
		}

		if( memcmp(sContent,"02",2) != 0) // STX －－> 0x30 0x32
		{
			continue;
		}

		// 接收2个字节的命令码 和 2 字节数据长度
		for (; uiReadCnt < 10; uiReadCnt++)
		{
			iRet = portRecv(sg_PortFd, &sContent[uiReadCnt], 100);
			if( iRet!=0 )
			{
				return ERR_RS232_RX_DATA;
			}
		}

		// 数据长度
		PubAsc2Bcd(&sContent[6],4,szTemp);
		uiLength = PubChar2Long(szTemp,2,NULL);
		uiLength = uiLength*2;  // bc--ascii转换，需要扩大倍数
		if (uiLength > uiExpLen)
		{
			return ERR_RS232_RX_LENGTH; // 数据超长
		}

		// 接收N个数据长度 和 2字节 CRC校验数据
		uiLength += (5+2)*2;
		for (; uiReadCnt < uiLength; uiReadCnt++)
		{
			iRet = portRecv(sg_PortFd, &sContent[uiReadCnt], 100);
			if( iRet!=0 )
			{
				return ERR_RS232_RX_DATA;
			}
		}

		PubAsc2Bcd(sContent,uiReadCnt,psRxdData);
		uiReadCnt = uiReadCnt/2;

		// 计算CRC结果: 从命令码开始到CRC数据之前的所有数据
		usCRCRet = PubCalcCRC(psRxdData+1,uiReadCnt-3,0x0000);
		uiLength = PubChar2Long(psRxdData+uiReadCnt-2,2,NULL);
		if (usCRCRet != uiLength)
		{
			return ERR_RS232_RX_CHECKSUM;
		}
		
		break;
	}
	
	if( puiOutLen!=NULL )
	{
		*puiOutLen = uiReadCnt;
	}
	
	return 0;
}
*/

// end of file
