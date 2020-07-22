
#include "Rs232lib.h"
#include "Glbvar.h"

/********************** Internal macros declaration ************************/
/********************** Internal structure declaration *********************/
/********************** Internal functions declaration *********************/
/********************** Internal variables declaration *********************/
static int          sg_PortFd =  -1 ;
/********************** external reference declaration *********************/
/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

//   �򿪴���
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

//   �رմ���
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

//   ��⴮�ڻ������Ƿ�������,uiFlag ��ʾ���ͻ��ǽ��ջ�����
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

//   ��⴮�ڻ������Ƿ�������, uiFlag ��ʾ���ͻ��ǽ��ջ�����
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


//   ���ڷ�������  
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


//   ���ڽ�������
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
		// ����Ƿ�ʱ
		iNewTimeMs= sysGetTimerCount();
		if( (iNewTimeMs-iOldTimeMs) >= (unsigned int)(uiTimeOut *1000) )	// ��鶨ʱ��
		{
			if( uiReadCnt>0 )	// �Ѿ���ȡ������
			{
				break;
			}			
			return ERR_RS232_RX_TIMEOUT;
		}
		// �����ջ������Ƿ�������		
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
		// ����ͷ
		iRet = portRecv(sg_PortFd, &psRxdData[uiReadCnt], 1000);  // �ӳ���1s��ԭ����100ms
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

		// ����2���ֽڵ������� �� 2 �ֽ����ݳ���
		for (; uiReadCnt < 5; uiReadCnt++)
		{
			iRet = portRecv(sg_PortFd, &psRxdData[uiReadCnt], 100);
			if( iRet!=0 )
			{
				return ERR_RS232_RX_DATA;
			}
		}

		// ���ݳ���
		uiLength = PubChar2Long(&psRxdData[uiReadCnt-2],2,NULL);
		if (uiLength > uiExpLen)
		{
			return ERR_RS232_RX_LENGTH; // ���ݳ���
		}

		// ����N�����ݳ���+ 2�ֽ� CRCУ������
		uiLength += 5+2;
		for (; uiReadCnt < uiLength; uiReadCnt++)
		{
			iRet = portRecv(sg_PortFd, &psRxdData[uiReadCnt], 100);
			if( iRet!=0 )
			{
				return ERR_RS232_RX_DATA;
			}
		}

		// ����CRC���: �������뿪ʼ��CRC����֮ǰ����������
		//ĸPOS��У��CRC���������Ժ��ɽ��պ�������У��
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
//   ���ڽ�������, ������ũ����Ĳ��Ի��ϣ����ڳ�����0x0dת���ַ��������յ������ݶ���һ���ֽ�0x00��
//   �ʴ˰汾�����е����ݶ�������bcd-->ascii��ת��
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
		// ����Ƿ�ʱ
		iNewTimeMs= sysGetTimerCount();
		if( (iNewTimeMs-iOldTimeMs) >= (UINT32)(uiTimeOut *1000) )	// ��鶨ʱ��
		{
			if( uiReadCnt>0 )	// �Ѿ���ȡ������
			{
				break;
			}			
			return ERR_RS232_RX_TIMEOUT;
		}
		// �����ջ������Ƿ�������		
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
		// ����ͷ
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

		if( memcmp(sContent,"02",2) != 0) // STX ����> 0x30 0x32
		{
			continue;
		}

		// ����2���ֽڵ������� �� 2 �ֽ����ݳ���
		for (; uiReadCnt < 10; uiReadCnt++)
		{
			iRet = portRecv(sg_PortFd, &sContent[uiReadCnt], 100);
			if( iRet!=0 )
			{
				return ERR_RS232_RX_DATA;
			}
		}

		// ���ݳ���
		PubAsc2Bcd(&sContent[6],4,szTemp);
		uiLength = PubChar2Long(szTemp,2,NULL);
		uiLength = uiLength*2;  // bc--asciiת������Ҫ������
		if (uiLength > uiExpLen)
		{
			return ERR_RS232_RX_LENGTH; // ���ݳ���
		}

		// ����N�����ݳ��� �� 2�ֽ� CRCУ������
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

		// ����CRC���: �������뿪ʼ��CRC����֮ǰ����������
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
