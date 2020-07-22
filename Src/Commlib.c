
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include "Commlib.h"
#include "glbvar.h"
#include "posmacro.h"
#include "tranfunclist.h"

#define PosixGeterrno()  errno

/********************** Internal macros declaration ************************/
#define   NET_ERR_RETRY         1000
#define   MODEM_PATHNAME        "/dev/ttyS1"
#define   MODEM_PATHNAME1       "/dev/modem0"
#define   LEN_WORKBUF			(1024*4)
#define   INVALID_SOCKET        0xFFFF
/********************** Internal structure declaration *********************/
typedef struct _tagERR_INFO
{
	int		iErrCode;
	uint8_t	szChnMsg[16+1];
	uint8_t	szEngMsg[16+1];
}ERR_INFO;

/********************** Internal functions declaration *********************/
static int RS232Dial(uint8_t ucDialMode);
static int RS232Txd(uint8_t *psTxdData, uint32_t uiDataLen, uint32_t uiTimeOut);
static int RS232RawTxd(uint8_t *psTxdData, uint32_t uiDataLen, uint32_t uiTimeOut);
static int RS232NacTxd(uint8_t *psTxdData, uint32_t uiDataLen, uint32_t uiTimeOut);
static int RS232Rxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen);
static int RS232RawRxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen);
static int RS232NacRxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen);
static int RS232OnHook(uint8_t bReleaseAll);

static int  TcpDial(uint8_t ucDialMode);
static int  TcpTxd(uint8_t *psTxdData, uint32_t uiDataLen, uint32_t uiTimeOut);
static int  TcpRxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen);
static int  TcpOnHook(uint8_t bReleaseAll);
static int  PSTNDial(uint8_t ucDialMode);
static int  PSTNTxd(uint8_t *psTxdData, uint32_t uiDataLen, uint32_t uiTimeOut);
static int  PSTNRxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen);
static int  PSTNSyncRxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen);
static int  PSTNAsyncRxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen);
static int  PSTNOnHook(uint8_t bReleaseAll);
static int  PSTNGetStatus(int *piStatus);
static int  PSTNInitModem(ModemDialParms_t *pstModemParam);
static INT32  PSTNOpenModem(void);

static int   InitWirelessModule(ST_WIRELESS_INFO *pstParam);
static int   WirelessDial(uint8_t ucDialFlag);
static int   WirelessRxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen);
static int   WirelessTxd(uint8_t *psTxdData, uint32_t uiDataLen, uint32_t uiTimeOut);
//static int   WirelessClose(uint8_t bRelease);
static void    DispWirelessSignal(void);
static int     WirelessReset(ST_WIRELESS_INFO *pstParam);

static int   SetSockAddr(ST_COMM_CONFIG *pstCfg);

static int   WiFiDial(uint8_t ucDialMode);

static void  GetAllErrMsg(int iErrCode, ERR_INFO *pstInfo, COMM_ERR_MSG *pstCommErrMsg);
static uint8_t CalcLRC(uint8_t *psData, uint32_t uiLength, uint8_t ucInit);
/********************** Internal variables declaration *********************/
static int          sg_Wiresocket = -1 ;   
static int          sg_Tcpsocket =  -1 ;
static int          sg_Wire_PreDial_Ret;
static int          sg_iModemfd = -1;
static UINT32       sg_uiWirePPPLogoutTime = 0; // �洢ǰһ��ppp�ɹ��Ҷϵ�ʱ�䣬�Ӷ���֤��3��ļ���ſ�ʼ�µ�ppp���Ŷ���
static UINT8        sg_ucWirePPPFlag = 0; // ��ʾppp���ӵ�״̬ 0��ʾ�ѹҶ� 1��ʾδ�Ҷ�
static struct sockaddr     sg_stWiresockaddr;
static struct sockaddr     sg_stTcpsockaddr;

static ST_COMM_CONFIG	sg_stCurCfg = {0xFF};
static uint8_t		sg_sWorkBuf[LEN_WORKBUF+50];
static ERR_INFO		sg_stCommErrMsg[] =
{
	{ERR_COMM_INV_PARAM, "��������",     "INVALID PARAM"},
	{ERR_COMM_INV_TYPE,  "��ЧͨѶ����", "INV COMM TYPE"},
	{ERR_COMM_CANCEL,    "�û�ȡ��",     "USER CANCEL"},
	{ERR_COMM_TIMEOUT,   "ͨѶ��ʱ",     "TIMEOUT"},
	{ERR_COMM_COMERR,    "ͨѶ����",     "COMM ERROR"},
	{ERR_COMM_TOOBIG,    "���ݰ�̫��",   "DATA TOO BIG"},
	{0, "", ""},
};

static ERR_INFO		sg_stRS232ErrMsg[] =
{
	{PORT_RET_NOTEMPTY,    "���ͻ�������", "OVERFLOW"},
	{PORT_RET_PORTERR,     "��Ч�˿ں�",   "INVALID PORT"},
	{PORT_RET_NOTOPEN,     "�˿�δ��",   "PORT CLOSED"},
	{PORT_RET_BUSY,        "�޿��ö˿�",   "NO PORT AVAIL"},
	{PORT_RET_MODEM_INUSE, "�˿ڱ�ռ��",   "PORT OCCUPY"},
	{PORT_RET_PARAERR,     "��ЧͨѶ����", "INVALID PARAM"},
	{PORT_RET_TIMEOUT,     "���ڳ�ʱ",     "TIMEOUT"},
	{0, "", ""},
};

static ERR_INFO		sg_stPPPErrMsg[] =
{
	{NET_ERR_RETRY,        "ͨѶ��ʱ",        "TIMEOUT"},
	{NET_ERR_RSP,          "��Ӧ��,���ģ��", "CHECK MODULE"},
	{NET_ERR_NOSIM,        "SIM��������",     "NO SIM CARD"},
	{NET_ERR_PIN,          "��Ҫ����PIN��",   "NEED SIM PIN"},
	{NET_ERR_PUK,          "��Ҫ����PUK��",   "NEED SIM PUK"},
	{NET_ERR_PWD,          "PIN�����",       "SIM PIN ERROR"},
	{NET_ERR_SIMDESTROY,   "SIM������",       "NO SIM/NEED PIN"},
	{NET_ERR_CSQWEAK,      "�ź�̫��",        "SIGNAL TOO WEAK"},
	{NET_ERR_LINKCLOSED,   "�������ز�",      "NO CARRIER"},
	{NET_ERR_LINKOPENING,  "���ӳɹ�",        "LINK OK"},
	{NET_ERR_ATTACHING,    "���ڲ�������",    "SEEKING NETWORK"},
	{NET_ERR_DETTACHED,    "�ѶϿ���������",  "DETTACH NETWORK"},
	{NET_ERR_EMERGENCY,    "���SIM��",       "PLS CHECK SIM"},
	{NET_ERR_RING,         "�ȴ�������",      "ACCEPTED ERR"},
	{NET_ERR_BUSY,         "����ͨѶ��",      "COMMINICATE ERR"},
	{NET_ERR_DIALING,      "ģ�����ڲ���",    "LINKING"},
	{NET_ERR_UNKNOWN,      "δ֪������",      "UNKNOW ERR"},
	{NET_ERR_ABNORMAL,     "δ֪������",      "UNKNOW ERR"},
	{0, "", ""},
};

//#define	EINTR		 4	/* Interrupted system call */
//#define	EIO		 5	/* I/O error */
static ERR_INFO	sg_stModemErrMsg[] =
{
	{-MODEM_ERRNO_ERROR,             "�豸�쳣",       "MODEM ERROR"},
	{-MODEM_ERRNO_BUSY,              "�� �� æ",       "MODEM BUSY"},
	{-MODEM_ERRNO_NO_DIALTONE,       "�޲�����",       "NO DIAL TONE"},
	{-MODEM_ERRNO_NO_CARRIER,        "��·�ز���ʧ",   "LINE BREAK"},
	{-MODEM_ERRNO_NO_LINE,           "��Ӻõ绰��",   "LINE READY ?"},
	{-MODEM_ERRNO_NO_ANSWER,         "������Ӧ��",     "NO ACK"},
	{-MODEM_ERRNO_OFF_HOOK,          "���ȹһ�",       "PLS ONHOOK"},
	{-MODEM_ERRNO_LINE_IN_USE,       "������·æ",     "LINE BUSY"},
	{-MODEM_ERRNO_UN_OBTAINABLE,     "��·���ɵ���",   "UNOBTAINABLE"},
	{-MODEM_ERRNO_LINE_REVERSAL,     "��·��ת",       "LINE REVERSAL"},
	{-MODEM_ERRNO_EXTENSION_IN_USE,  "���õ绰ռ��",   "PHONE OCCUPIED"},
	{ERR_COMM_STATUS_TIMEOUT,        "����������ʱ",   "TIME OUT"},
	{ERR_COMM_MODEM_INIT,            "��ʼ���豸ʧ��", "MODEM INIT ERROR"},
	{NET_ERR_RETRY,                  "���Դ�������",   "RETRY OVERRUN"},
	{4,                              "��Ӻõ绰��",   "LINE READY ?"},
	{5,                              "��Ӻõ绰��",   "LINE READY ?"},
	{16,                             "�� �� æ",       "MODEM BUSY"},
	{22,                             "��������",       "INVALID ARGUMENT"},
	{ETIMEDOUT,                      "����������ʱ",   "TIME OUT"},
	{ETIME,                          "����������ʱ",   "TIME OUT"},
	{0, "", ""},
};

/********************** external reference declaration *********************/
/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

int  GetCurSocket(void)
{
	if (sg_stCurCfg.ucCommType == CT_CDMA || sg_stCurCfg.ucCommType == CT_GPRS)
	{
		return sg_Wiresocket;
	}

	if (sg_stCurCfg.ucCommType == CT_TCPIP || sg_stCurCfg.ucCommType == CT_WIFI)
	{
		return sg_Tcpsocket;
	}

	return -1;
}
 
int   SetSockAddr(ST_COMM_CONFIG *pstCfg)
{
	int iRet,iPort;

	if (pstCfg->ucCommType == CT_CDMA || pstCfg->ucCommType == CT_GPRS)
	{
		iPort = atoi((char *)pstCfg->stWirlessPara.szRemotePort);
		iRet = SockAddrset(&sg_stWiresockaddr,pstCfg->stWirlessPara.szRemoteIP,iPort);
		return iRet;		
	} 

	if (pstCfg->ucCommType==CT_TCPIP)
	{
		char temp[sizeof(pstCfg->stTcpIpPara.szRemoteIP)], *p, *t;
		int flag = 0;

		p = (char *)pstCfg->stTcpIpPara.szRemoteIP;
		t = temp;
		while (*p) {
			if (flag) {
				*t++ = *p;
				if (*p == '.')
					flag = 0;
				p++;
			} else {
				if (*p != '0')
					flag = 1;
				else
					p++;
			}
		}
		*t = '\0';

		iPort = atoi((char *)pstCfg->stTcpIpPara.szRemotePort);
		iRet = SockAddrset(&sg_stTcpsockaddr,temp,iPort);	
		return iRet;		
	}

	if (pstCfg->ucCommType == CT_WIFI)
	{
		iPort = atoi((char *)pstCfg->stWIFIIpPara.szRemotePort);
		iRet = SockAddrset(&sg_stTcpsockaddr,pstCfg->stWIFIIpPara.szRemoteIP,iPort);	
		return iRet;
	}

	return 0;
}


// ͨѶģ���ʼ��
int CommInitModule(ST_COMM_CONFIG *pstCfg)
{
	int    iRet;

	CommSetCfgParam(pstCfg);

	switch (pstCfg->ucCommType)
	{
	case CT_WIFI:
		iRet = InitWiFiModule(TRUE);
		break;

	case CT_CDMA:
	case CT_GPRS:
		iRet = InitWirelessModule(&pstCfg->stWirlessPara);
		break;

	case CT_MODEM:
		iRet = PSTNInitModem(&pstCfg->stPSTNPara.stDialPara);
		break;

	default:
		iRet = 0;
		break;
	}

	return iRet;
}

// ����ͨѶģ�����
int CommSetCfgParam(ST_COMM_CONFIG *pstCfg)
{
	int		iLen;

	if( pstCfg==NULL )
	{
		return ERR_COMM_INV_PARAM;
	}

	switch( pstCfg->ucCommType )
	{
	case CT_RS232:
		iLen = strlen((char *)pstCfg->stRS232Para.szAttr);
		if( iLen<10 || iLen>20 )	// �򵥼�����
		{
			return ERR_COMM_INV_PARAM;
		}
		break;

	case CT_MODEM:
		if (!(gstPosCapability.uiCommType & CT_MODEM))
		{
			return ERR_COMM_INV_PARAM;
		}
		if(  pstCfg->stPSTNPara.szTxnTelNo[0]==0  )
		{
			return ERR_COMM_INV_PARAM;
		}
		if( pstCfg->stPSTNPara.ucSendMode!=MODEM_COMM_ASYNC &&
			pstCfg->stPSTNPara.ucSendMode!=MODEM_COMM_SYNC )
		{
			return ERR_COMM_INV_PARAM;
		} 
		break;

	case CT_TCPIP:
		if (!(gstPosCapability.uiCommType & CT_TCPIP))
		{
			return ERR_COMM_INV_PARAM;
		}

		if( pstCfg->stTcpIpPara.szRemoteIP[0]==0   ||
			pstCfg->stTcpIpPara.szRemotePort[0]==0 )
		{
			return ERR_COMM_INV_PARAM;
		}
	    break;

	case CT_CDMA:
	case CT_GPRS:
		if (!(gstPosCapability.uiCommType & CT_CDMA) && !(gstPosCapability.uiCommType & CT_GPRS))
		{
			return ERR_COMM_INV_PARAM;
		}

		if( pstCfg->stWirlessPara.szAPN[0]==0     ||
			pstCfg->stWirlessPara.szRemoteIP[0]==0 )
		{
			return ERR_COMM_INV_PARAM;
		}
		break;

	case CT_WIFI:
		if (!(gstPosCapability.uiCommType & CT_WIFI))
		{
			return ERR_COMM_INV_PARAM;
		}

		if( pstCfg->stWIFIIpPara.szRemoteIP[0]==0   ||
			pstCfg->stWIFIIpPara.szRemotePort[0]==0 )
		{
			return ERR_COMM_INV_PARAM;
		}
		break;

	default:
		return ERR_COMM_INV_TYPE;
	}

	memcpy(&sg_stCurCfg, pstCfg, sizeof(ST_COMM_CONFIG));

	return 0;
}


// ͨѶģ�鲦��(MODEM���Ż���TCP����TCP���ӵ�)
int CommDial(uint8_t ucDialMode)
{
	int		iRet;

	switch( sg_stCurCfg.ucCommType )
	{
	case CT_RS232:
		iRet = RS232Dial(ucDialMode);
		break;
		
	case CT_MODEM:
		iRet = PSTNDial(ucDialMode);
		break;
		
	case CT_TCPIP:
		iRet = TcpDial(ucDialMode);
		break;
		
	case CT_CDMA:
	case CT_GPRS:
		iRet = WirelessDial(ucDialMode);
		break;
		
	case CT_WIFI:
		iRet = WiFiDial(ucDialMode);
		break;
		
	default:
		iRet = ERR_COMM_INV_TYPE;
	}

	return iRet;
}

// ͨѶģ�鷢������
int CommTxd(uint8_t *psTxdData, uint32_t uiDataLen, uint32_t uiTimeOut)
{
	int		iRet;

	if( psTxdData==NULL )
	{
		return ERR_COMM_INV_PARAM;
	}
	switch( sg_stCurCfg.ucCommType )
	{
	case CT_RS232:
		iRet = RS232Txd(psTxdData, uiDataLen, uiTimeOut);
		break;

	case CT_MODEM:
		iRet = PSTNTxd(psTxdData, uiDataLen, uiTimeOut);
		break;

	case CT_TCPIP:
	case CT_WIFI:
		iRet = TcpTxd(psTxdData, uiDataLen, uiTimeOut);
		break;

	case CT_CDMA:
	case CT_GPRS:
		NacTxd("1111",4);
		iRet = WirelessTxd(psTxdData, uiDataLen, uiTimeOut);
		break;

	default:
		iRet = ERR_COMM_INV_TYPE;
	}

	return iRet;
}

// ͨѶģ���������
int CommRxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen)
{
	int		iRet;

	if( psRxdData==NULL )
	{
		return ERR_COMM_INV_PARAM;
	}

	switch( sg_stCurCfg.ucCommType )
	{
	case CT_RS232:
		if (uiTimeOut<=5)
		{
			uiTimeOut = 5;
		}
		iRet = RS232Rxd(psRxdData, uiExpLen, uiTimeOut, puiOutLen);
		RS232OnHook(FALSE);
		break;

	case CT_MODEM:
		if (uiTimeOut<=15)
		{
			uiTimeOut = 15;
		}
		iRet = PSTNRxd(psRxdData, uiExpLen, uiTimeOut, puiOutLen);
// MODEM�յ����غ����ϹҶ� 20110331
//		PSTNOnHook(FALSE);
		break;

	case CT_TCPIP:
	case CT_WIFI:
		if (uiTimeOut<=10)
		{
			uiTimeOut = 10;
		}
		iRet = TcpRxd(psRxdData, uiExpLen, uiTimeOut, puiOutLen);
		TcpOnHook(FALSE);
		break;

	case CT_CDMA:
	case CT_GPRS:
		if (uiTimeOut<=15)
		{
			uiTimeOut = 15;
		}
		iRet = WirelessRxd(psRxdData, uiExpLen, uiTimeOut, puiOutLen);
		WirelessClose(FALSE);  // ���ܺ���Ҫ�Ͽ�TCP���ӣ��´�����ʱ��������
		break;

	default:
		iRet = ERR_COMM_INV_TYPE;
	}

	return iRet;
}

// ͨѶģ��Ͽ���·(MODEM�һ�����TCP�Ͽ�TCP���ӵ�)
int CommOnHook(uint8_t bReleaseAll)
{
	int		iRet;
//	int Col, Line;
//	uint8_t Buff[2048]; // ��ǰ��ĻΪ128*64�����鿪�ٵĻ�������СΪ2K��

//	lcdStore(&Col, &Line, Buff);
	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY," �� �� ��");
	lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY," PROCESSING....");
	DrawRect(0, 17, 127, 63);

	switch( sg_stCurCfg.ucCommType )
	{
	case CT_RS232:
		iRet = RS232OnHook(bReleaseAll);
		break;

	case CT_MODEM:
		iRet = PSTNOnHook(bReleaseAll);
		break;

	case CT_TCPIP:
	case CT_WIFI:
		iRet = TcpOnHook(bReleaseAll);
		break;

	case CT_CDMA:
	case CT_GPRS:
		iRet = WirelessClose(bReleaseAll);
		break;

	default:
		iRet = ERR_COMM_INV_TYPE;
	}

//	lcdRestore(Col, Line, Buff);
	return iRet;
}
 
// ��ȡͨѶ������Ϣ
void CommGetErrMsg(int iErrCode, COMM_ERR_MSG *pstCommErrMsg)
{
	int  iReErrCode;

	if( pstCommErrMsg==NULL )
	{
		return;
	}

	iReErrCode = iErrCode & MASK_ERR_CODE;

	switch( iErrCode & MASK_COMM_TYPE )
	{
	case ERR_COMM_ALL_BASE:
		sprintf((char *)pstCommErrMsg->szChnMsg, "ͨѶ����:%04X", iReErrCode);
		sprintf((char *)pstCommErrMsg->szEngMsg, "COMM ERR:%04X", iReErrCode);
		GetAllErrMsg(iErrCode, sg_stCommErrMsg, pstCommErrMsg);
		break;

	case ERR_COMM_RS232_BASE:
		sprintf((char *)pstCommErrMsg->szChnMsg, "���ڴ���:%04X", iReErrCode);
		sprintf((char *)pstCommErrMsg->szEngMsg, "PORT ERR:%04X", iReErrCode);
		GetAllErrMsg(iErrCode, sg_stRS232ErrMsg, pstCommErrMsg);
		break;

	case ERR_COMM_MODEM_BASE:
		sprintf((char *)pstCommErrMsg->szChnMsg, "���Ŵ���:%04X", iReErrCode);
		sprintf((char *)pstCommErrMsg->szEngMsg, "MODEM ERR:%04X", iReErrCode);
		GetAllErrMsg(iErrCode, sg_stModemErrMsg, pstCommErrMsg);
	    break;

	case ERR_COMM_TCPIP_BASE:
		sprintf((char *)pstCommErrMsg->szChnMsg, "TCPIP����:%04X", iReErrCode);
		sprintf((char *)pstCommErrMsg->szEngMsg, "TCPIP ERR:%04X", iReErrCode);
		GetAllErrMsg(iErrCode, sg_stPPPErrMsg, pstCommErrMsg);
		break;
	case ERR_COMM_WIRELESS_BASE:
		sprintf((char *)pstCommErrMsg->szChnMsg, "���ߴ���:%04X", iReErrCode);
		sprintf((char *)pstCommErrMsg->szEngMsg, "WIRE ERR:%04X", iReErrCode);
		GetAllErrMsg(iErrCode, sg_stPPPErrMsg, pstCommErrMsg);
		break;
	default:
		sprintf((char *)pstCommErrMsg->szChnMsg, "ͨѶ����");
		sprintf((char *)pstCommErrMsg->szEngMsg, "ERR CODE:%04X", iReErrCode);
		break;
	}
}

void GetAllErrMsg(int iErrCode, ERR_INFO *pstInfo, COMM_ERR_MSG *pstCommErrMsg)
{
	int		iCnt;

	for(iCnt=0; pstInfo[iCnt].iErrCode!=0; iCnt++)
	{
		if( pstInfo[iCnt].iErrCode==iErrCode ||
			pstInfo[iCnt].iErrCode==(iErrCode & MASK_ERR_CODE) )
		{
			sprintf((char *)pstCommErrMsg->szChnMsg, "%.16s", pstInfo[iCnt].szChnMsg);
			sprintf((char *)pstCommErrMsg->szEngMsg, "%.16s", pstInfo[iCnt].szEngMsg);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// RS232 ͨѶģ��
//////////////////////////////////////////////////////////////////////////
// ���ڴ�
int RS232Dial(uint8_t ucDialMode)
{
	int 	iRet;

	if( ucDialMode==PREDIAL_MODE )
	{
		return 0;
	}

	iRet = portOpen(sg_stCurCfg.stRS232Para.ucPortNo,(char *)sg_stCurCfg.stRS232Para.szAttr);
	if( iRet != PORT_RET_OK )
	{
		return (ERR_COMM_RS232_BASE | iRet);
	}

	return PORT_RET_OK;
}

// ���ڷ�������
int RS232Txd(uint8_t *psTxdData, uint32_t uiDataLen, uint32_t uiTimeOut)
{
	int		iRet;

	if (sg_stCurCfg.stRS232Para.ucSendMode == CM_RAW)
	{	
		iRet = RS232RawTxd(psTxdData, uiDataLen, uiTimeOut);
	}
	else
	{
		iRet = RS232NacTxd(psTxdData, uiDataLen, uiTimeOut);
	}

	return iRet;
}

// ����ֱ�ӷ���
int RS232RawTxd(uint8_t *psTxdData, uint32_t uiDataLen, uint32_t uiTimeOut)
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
			iRet =  ERR_COMM_RS232_BASE | iRet;
			break;
		}

		iRet = portSends(sg_stCurCfg.stRS232Para.ucPortNo, psTxdData,uiDataLen);
		if( iRet == PORT_RET_OK )
		{
			break;
		}
		sysDelayMs(50);
	}
 
	return iRet;
}

// STX+Len1+Len2+strings+ETX+CKS, CKS = Len1 -- ETX (^)
int RS232NacTxd(uint8_t *psTxdData, uint32_t uiDataLen, uint32_t uiTimeOut)
{
	int		iRet;

	if( uiDataLen>LEN_WORKBUF )
	{
		return ERR_COMM_TOOBIG;
	}

	sg_sWorkBuf[0] = STX;
	sg_sWorkBuf[1] = (uiDataLen/1000)<<4    | (uiDataLen/100)%10;	// convert to BCD
	sg_sWorkBuf[2] = ((uiDataLen/10)%10)<<4 | uiDataLen%10;
	memcpy(&sg_sWorkBuf[3], psTxdData, uiDataLen);
	sg_sWorkBuf[3+uiDataLen]   = ETX;
	sg_sWorkBuf[3+uiDataLen+1] = CalcLRC(psTxdData, uiDataLen, (uint8_t)(sg_sWorkBuf[1] ^ sg_sWorkBuf[2] ^ ETX));

	iRet = RS232RawTxd(sg_sWorkBuf, (uint32_t)(uiDataLen+5), uiTimeOut);	// data
	if( iRet!=0 )
	{
		return iRet;
	}

	return 0;
}

// ���ڽ���
int RS232Rxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen)
{
	int		iRet;

	if (sg_stCurCfg.stRS232Para.ucSendMode == CM_RAW)
	{
		iRet = RS232RawRxd(psRxdData, uiExpLen, uiTimeOut, puiOutLen);
	}
	else
	{
		iRet = RS232NacRxd(psRxdData, uiExpLen, uiTimeOut, puiOutLen);
	}

	return iRet;
}

// ����ֱ�ӽ���
int RS232RawRxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen)
{
	int   iRet,iFlag;
	uint32_t	uiReadCnt, uiTemp;
	uint32_t  iOldTimeMs,iNewTimeMs;

	uiReadCnt = iFlag = 0;
	uiTemp = 5000;
	iOldTimeMs = sysGetTimerCount();
	while( uiReadCnt<uiExpLen )
	{
		if (iFlag==0)
		{
			iNewTimeMs= sysGetTimerCount();
			if( (iNewTimeMs-iOldTimeMs) >= (uint32_t)(uiTimeOut *1000) )	// ��鶨ʱ��
			{
				if( uiReadCnt>0 )	// �Ѿ���ȡ������
				{
					break;
				}

				return ERR_COMM_TIMEOUT;
			}

//			lcdDisplay(87,4,DISP_CFONT|DISP_CLRLINE,"(%d)",(iNewTimeMs-iOldTimeMs)/1000);
			lcdDisplay(0,4,DISP_CFONT|DISP_CLRLINE,"������...(%d)",(iNewTimeMs-iOldTimeMs)/1000);
			iRet = portCheckRecvBuf(sg_stCurCfg.stRS232Para.ucPortNo);
			if (iRet != PORT_RET_NOTEMPTY)
			{
				if (iRet != PORT_RET_OK )
				{
					return (ERR_COMM_RS232_BASE | iRet);
				}
				
				continue;
			}
			else
			{
				iFlag = 1;
			}
		}

		iRet = portRecv(sg_stCurCfg.stRS232Para.ucPortNo, psRxdData, uiTemp);
		if( iRet==0x00 )
		{	// ���ճɹ�,����
			uiTemp = 80;
			psRxdData++;
			uiReadCnt++;
		}
		else if( iRet==0xFF )
		{
			if( uiReadCnt>0 )
			{
				break;
			}

			return ERR_COMM_TIMEOUT;
		}
		else
		{	// �ǳ�ʱ����,�˳�
			return (ERR_COMM_RS232_BASE | iRet);
		}
	}   // end of while( uiReadCnt<uiExpLen

	if( puiOutLen!=NULL )
	{
		*puiOutLen = uiReadCnt;
	}

	return 0;
}

// STX+Len1+Len2+strings+ETX+CKS, CKS = Len1 -- ETX (^)
int RS232NacRxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen)
{
	int   iRet ,iFlag;
	uint32_t	uiReadCnt, uiLength;
	uint32_t  iOldTimeMs,iNewTimeMs,uiTemp;
	
	if( uiExpLen>LEN_WORKBUF )
	{
		return ERR_COMM_TOOBIG;
	}
	
	iFlag = 0;
	uiTemp = 5000;
	uiReadCnt = uiLength = 0;
	memset(sg_sWorkBuf, 0, sizeof(sg_sWorkBuf));
	iOldTimeMs = sysGetTimerCount();
	while( 1 )
	{
		if (iFlag==0)
		{
			iNewTimeMs= sysGetTimerCount();
			if( (iNewTimeMs-iOldTimeMs) >= (uint32_t)(uiTimeOut *1000) )	// ��鶨ʱ��
			{
				if( uiReadCnt>0 )	// �Ѿ���ȡ������
				{
					break;
				}
				
				return ERR_COMM_TIMEOUT;
			}
			
//			lcdDisplay(87,4,DISP_CFONT,"(%d)",(iNewTimeMs-iOldTimeMs)/1000);
			lcdDisplay(0,4,DISP_CFONT|DISP_CLRLINE,"������...(%d)",(iNewTimeMs-iOldTimeMs)/1000);
			iRet = portCheckRecvBuf(sg_stCurCfg.stRS232Para.ucPortNo);
			if (iRet != PORT_RET_NOTEMPTY)
			{
				if (iRet != PORT_RET_OK )
				{
					return (ERR_COMM_RS232_BASE | iRet);
				}
				
				continue;
			}
			else
			{
				iFlag = 1;
			}
		}
		
		iRet = portRecv(sg_stCurCfg.stRS232Para.ucPortNo, &sg_sWorkBuf[uiReadCnt], uiTemp);
		if( iRet!=0 )
		{
			if( iRet==0xFF )
			{
				continue;
			}
			return ERR_COMM_RS232_BASE|iRet;
		}
		uiTemp = 100;
		if( sg_sWorkBuf[0]!=STX )
		{
			continue;
		}
		
		uiReadCnt++;
		if( uiReadCnt==3 )
		{
			uiLength =  ((sg_sWorkBuf[1]>>4) & 0x0F) * 1000 + (sg_sWorkBuf[1] & 0x0F) * 100 +
				((sg_sWorkBuf[2]>>4) & 0x0F) * 10   + (sg_sWorkBuf[2] & 0x0F);
		}
		if( uiReadCnt==uiLength+5 )
		{	// read data ok, verify it ...
			if( sg_sWorkBuf[uiReadCnt-2]==ETX &&
				CalcLRC(&sg_sWorkBuf[1], (uint32_t)(uiReadCnt-1), 0)==0 )
			{
				break;
			}
			return ERR_COMM_COMERR;
		}
	}
	
	memcpy(psRxdData, &sg_sWorkBuf[3], uiLength);
	if( puiOutLen!=NULL )
	{
		*puiOutLen = uiLength;
	}
	
	return 0;
}


// ���ڹر�
int RS232OnHook(uint8_t bReleaseAll)
{
	int	iRet;

	iRet = portClose(sg_stCurCfg.stRS232Para.ucPortNo);
	if( iRet ==0 )
	{
		return 0;
	}

	return (ERR_COMM_RS232_BASE | iRet);
}

//////////////////////////////////////////////////////////////////////////
// TCP ͨѶģ��
//////////////////////////////////////////////////////////////////////////
// ����TCP����
int TcpDial(uint8_t ucDialMode)
{
	int iRet,iFlag,iRetyTimes;
	uint32_t   uiOldTime;

	if( ucDialMode==PREDIAL_MODE )
	{
		return 0;
	}

    //  ʵ������
	iFlag = 1;
	iRetyTimes = 1;
	uiOldTime = sysGetTimerCount();	
	while (iFlag)
	{
		if ( sysGetTimerCount() >= (uiOldTime+60000) )
		{ // �������ӳ�ʱ60s���򷵻�
			TcpOnHook(FALSE);
			return (ERR_COMM_TCPIP_BASE|NET_ERR_RETRY);
		}
		if( PubChkKeyInput() )
		{
			if (kbGetKey() == KEY_CANCEL)
			{
				PSTNOnHook(FALSE);
				return ERR_USERCANCEL;
			}
		}
		
		if (iRetyTimes > 3 )
		{// ���β������Ӳ��ɹ���ر��׽��֣��Ͽ�PPP����
			TcpOnHook(FALSE);
			return (ERR_COMM_TCPIP_BASE|NET_ERR_RETRY);
		}
		
		if (sg_Tcpsocket < 0)
		{
			sg_Tcpsocket = NetSocket(AF_INET,SOCK_STREAM,0);
			if (sg_Tcpsocket < 0)
			{
				TcpOnHook(FALSE);
				iRet = sg_Tcpsocket;
				if (iRet<0)
				{
					iRet = -iRet;
				}
				return (ERR_COMM_TCPIP_BASE|iRet);
			}

			iRet = SetSockAddr(&sg_stCurCfg);
			if (iRet != NET_OK)
			{
				if (iRet<0)
				{
					iRet = -iRet;
				}
				TcpOnHook(TRUE);
				return ERR_COMM_TCPIP_BASE|iRet;
			}

			lcdClrLine(2,7);
			lcdFlip();	
			lcdDisplay(0,3,DISP_CFONT,"   �� �� �� ... ");
			lcdDisplay(0,5,DISP_CFONT,"TCPIP CONNECT(%d)",iRetyTimes++);
			DrawRect(0, 17, 127, 63);

			iRet = NetConnect(sg_Tcpsocket,&sg_stTcpsockaddr,sizeof(struct sockaddr));
//lcdClrLine(2, 7);
//lcdFlip();
//lcdDisplay(0, 2, DISP_CFONT, "iRet=%d", iRet);
//kbGetKey();		
			if (iRet != NET_OK)
			{
				CommOnHook(FALSE);  // �Ҷ�TCPIP��������
				iRet = ERR_COMM_TCPIP_BASE|iRet;
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			return 0;
		}
	}

	return iRet;
}

// ��������
int TcpTxd(uint8_t *psTxdData, uint32_t uiDataLen, uint32_t uiTimeOut)
{
	int  iRet,iRelSendlen;
	struct timeval stTimeVal;
	
	stTimeVal.tv_sec = 30;
	stTimeVal.tv_usec = 0;
	NetSetsockopt(sg_Tcpsocket,SOL_SOCKET,SO_SNDTIMEO,&stTimeVal,sizeof(stTimeVal));
	
	for (iRelSendlen=0; iRelSendlen<uiDataLen;)
	{
		iRet = NetSend(sg_Tcpsocket,psTxdData+iRelSendlen,(uiDataLen-iRelSendlen),0);
		if (iRet < 0)
		{
			TcpOnHook(TRUE);
			iRet = -iRet;
			return ERR_COMM_TCPIP_BASE|iRet;
		}
		iRelSendlen += iRet;
	}
	
	return 0;
}

// ��������
int TcpRxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen)
{
	int  iRet,iRetryCnt;
	int  iOldTime,iNowTime;
	uint32_t  ulRealRecvLen = 0;
	unsigned long  ulTemp;
	struct timeval stTimeVal;
	socklen_t      Socklen;
	
	stTimeVal.tv_sec = 1;
	stTimeVal.tv_usec = 0;
	Socklen = sizeof(stTimeVal);
	NetSetsockopt(sg_Tcpsocket,SOL_SOCKET,SO_RCVTIMEO,&stTimeVal,Socklen);

	iRetryCnt = 0;
	iOldTime = sysGetTimerCount();
	while( 1 )
	{
		iNowTime= sysGetTimerCount();
		if( (iNowTime-iOldTime) >= (uint32_t)(uiTimeOut *1000) )	// ��鶨ʱ��
		{
			iRet = ERR_COMM_TIMEOUT;
			TcpOnHook(TRUE);
			break;
		}

//		lcdDisplay(87,4,DISP_CFONT|DISP_CLRLINE,"(%d)",(iNowTime-iOldTime)/1000);		
		lcdDisplay(0,4,DISP_CFONT|DISP_CLRLINE,"������...(%d)",(iNowTime-iOldTime)/1000);
		iRet = NetRecv(sg_Tcpsocket,psRxdData+ulRealRecvLen,uiExpLen,0);
		if (iRet > 0)
		{
			PubChar2Long(psRxdData, 2, &ulTemp);
			ulRealRecvLen += iRet;
			if (ulRealRecvLen < ulTemp+2)
			{
				continue;
			}

			*puiOutLen = ulRealRecvLen;
			iRet = 0;
			break;
		}
		else
		{
			iRetryCnt++;
			if (iRetryCnt>3)
			{
				if (iRet<0)
				{
					iRet = -iRet;
				}
				iRet = ERR_COMM_TCPIP_BASE|iRet;
			}
			
			sysDelayMs(20);
		}
	}

	return iRet;
}

// �Ͽ�TCP����
int TcpOnHook(uint8_t bReleaseAll)
{
	int  iRet;

	iRet = NetClose(sg_Tcpsocket);
	sg_Tcpsocket = -1;

	if (bReleaseAll)
	{
		iRet = PPPLogout(OPT_DEVETH);
		if (iRet<0)
		{
			iRet = -iRet;
		}
	}

	return iRet;
}


//////////////////////////////////////////////////////////////////////////
// MODEM ͨѶģ��
//////////////////////////////////////////////////////////////////////////
#define MODEM_COMM_ERR(a)		(ERR_COMM_MODEM_BASE|(a))
// modem ����
int PSTNDial(uint8_t ucDialMode)
{
	int      iRet, iStatus, iDialTimes, iLastErr;
	uint32_t   uiOldTime;

	if (ucDialMode == PREDIAL_MODE)
	{
		iDialTimes = 0;
		iStatus = MODEM_STATE_NOT_INIT;

		uiOldTime = sysGetTimerCount();
		while (1)
		{
			if ( sysGetTimerCount() >= (uiOldTime+10000) )
			{ // �������ӳ�ʱ60s���򷵻�
				PSTNOnHook(TRUE);
				return MODEM_COMM_ERR(NET_ERR_RETRY);
			}

			if (iDialTimes>3)  // ���β��ɹ����˳�ʧ��
			{
				modem_get_last_errno(sg_iModemfd,&iLastErr);
				iLastErr = ABS(iLastErr);
				if (iLastErr == 0)
				{
					iLastErr = -MODEM_ERRNO_NO_CARRIER;
				}
				return MODEM_COMM_ERR(iLastErr);
			}
			
			iStatus = MODEM_STATE_NOT_INIT;
			iRet = PSTNGetStatus(&iStatus);
			if (iRet !=0 ) 
			{
				if (sg_iModemfd >= 0 )  // ���modem�����Ч�����ȴ�
				{
					PSTNOnHook(TRUE);
					sg_iModemfd = PSTNOpenModem();
					if (sg_iModemfd < 0)
					{
						return MODEM_COMM_ERR(ERR_COMM_MODEM_INIT); 
					}
					continue;
				}
				else
				{
					iRet = ABS(iRet);
					return MODEM_COMM_ERR(iRet);
				}
			}

			switch (iStatus)
			{
			case MODEM_STATE_NOT_INIT:
				PSTNOnHook(TRUE);
				sg_iModemfd = PSTNOpenModem();
				if (sg_iModemfd < 0)
				{
					return MODEM_COMM_ERR(ERR_COMM_MODEM_INIT); 
				}
				break;

			case MODEM_STATE_NO_SET_MODE:
				iRet = modem_set_dial_parms(sg_iModemfd,&sg_stCurCfg.stPSTNPara.stDialPara);
				if (iRet !=0 )
				{
					return MODEM_COMM_ERR(ERR_COMM_MODEM_NOPARAM);
				}
				break;

			case MODEM_STATE_SYNC_MODE:  // ��״̬��ʾ��������,��Ҫ�ȴ����óɹ�
			case MODEM_STATE_ASYN_MODE:  // ��״̬��ʾ��������,��Ҫ�ȴ����óɹ�
				break;

			case MODEM_STATE_DISCONNECT:
				iDialTimes++;
				iRet = modem_get_last_errno(sg_iModemfd,&iLastErr);
				if (iLastErr != 0)
				{
					PSTNOnHook(FALSE);
				}

				iRet = modem_dialing(sg_iModemfd,(char *)sg_stCurCfg.stPSTNPara.szTxnTelNo);
				iRet = ABS(iRet);
				if (iRet != 0)
				{
					dial_stat.fails++;
					write_data(&dial_stat, sizeof(DIAL_STAT), "dial.sta");
					return MODEM_COMM_ERR(iRet);
				}
				else
				{
					dial_stat.dials++;
					write_data(&dial_stat, sizeof(DIAL_STAT), "dial.sta");

					return 0;
				}
				break;

			case MODEM_STATE_WAITTING:  // ������״̬���ȹҶϵ�ǰ�������ӣ������²���
				PSTNOnHook(FALSE);
				break;

			default:
				return 0;
				break;
			}
		}

		return MODEM_COMM_ERR(ERR_COMM_MODEM_INIT);  
	}

	iDialTimes = 0;
	uiOldTime = sysGetTimerCount();
	while (1)
	{
		if ( sysGetTimerCount() >= (uiOldTime+60000) )
		{ // �������ӳ�ʱ60s���򷵻�
			PSTNOnHook(TRUE);
			return MODEM_COMM_ERR(NET_ERR_RETRY);
		}

		if( PubChkKeyInput() )
		{
			if (kbGetKey() == KEY_CANCEL)
			{
				PSTNOnHook(FALSE);
				return ERR_USERCANCEL;
			}
		}

		if (iDialTimes >= 3)  // ���β��ɹ����˳�ʧ��
		{
			modem_get_last_errno(sg_iModemfd,&iLastErr);
			iLastErr = ABS(iLastErr);
			return MODEM_COMM_ERR(iLastErr);
		}

		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 3, DISP_CFONT, "    �� �� ��   ");
		lcdDisplay(0, 5, DISP_CFONT|DISP_CLRLINE, "  DIALING...(%d)", iDialTimes);
		DrawRect(0, 17, 127, 63);

		iStatus = MODEM_STATE_NOT_INIT;
		iRet = PSTNGetStatus(&iStatus);
		if (iRet !=0 ) 
		{
			if (sg_iModemfd < 0)
			{
				PSTNOnHook(TRUE);
				sg_iModemfd = PSTNOpenModem();
				if (sg_iModemfd < 0)
				{
					return MODEM_COMM_ERR(ERR_COMM_MODEM_INIT); 
				}
				continue;
			}
			else
			{
				iRet = ABS(iRet);
				return MODEM_COMM_ERR(iRet);
			}
		}
		
		switch (iStatus)
		{
		case MODEM_STATE_NOT_INIT:
			PSTNOnHook(TRUE);
			sg_iModemfd = PSTNOpenModem();
			if (sg_iModemfd < 0)
			{
				return MODEM_COMM_ERR(ERR_COMM_MODEM_INIT); 
			}
			break;

		case MODEM_STATE_NO_SET_MODE:
			iRet = modem_set_dial_parms(sg_iModemfd,&sg_stCurCfg.stPSTNPara.stDialPara);
			if (iRet !=0 )
			{
				return MODEM_COMM_ERR(ERR_COMM_MODEM_NOPARAM);
			}
			break;

		case MODEM_STATE_SYNC_MODE:  // ��״̬��ʾ��������,��Ҫ�ȴ����óɹ�
		case MODEM_STATE_ASYN_MODE:  // ��״̬��ʾ��������,��Ҫ�ȴ����óɹ�
		case MODEM_STATE_DAILING:
		case MODEM_STATE_CONNECT_SDLC:
			sysDelayMs(100); // ��ʱ���ڲ鿴
			break;
			
		case MODEM_STATE_DISCONNECT:
			iDialTimes++;
			iRet = modem_dialing(sg_iModemfd, (char *)sg_stCurCfg.stPSTNPara.szTxnTelNo);
			if (iRet != 0)
			{
				dial_stat.fails++;
				write_data(&dial_stat, sizeof(DIAL_STAT), "dial.sta");
				iRet = ABS(iRet);
				return MODEM_COMM_ERR(iRet);
			}
			else
			{
				dial_stat.dials++;
				write_data(&dial_stat, sizeof(DIAL_STAT), "dial.sta");
			}
			break;
			
		case MODEM_STATE_WAITTING:  // ������״̬���ȹҶϵ�ǰ�������ӣ������²���
			PSTNOnHook(FALSE);
			break;
	
		case MODEM_STATE_CONNECT:
			return 0;

		default:
			iStatus = ABS(iStatus);
			return MODEM_COMM_ERR(iStatus);
		}
	}
	
	return MODEM_COMM_ERR(MODEM_ERRNO_ERROR);
}

// Modem ����
int PSTNTxd(uint8_t *psTxdData, uint32_t uiDataLen, uint32_t uiTimeOut)
{
	int  iRet,iRelSendlen;
		
	for (iRelSendlen=0; iRelSendlen<uiDataLen;)
	{
		iRet = modem_write_timeout(sg_iModemfd,psTxdData+iRelSendlen,(uiDataLen-iRelSendlen),uiTimeOut);
		if (iRet < 0)
		{
			PSTNOnHook(FALSE);   // ����ʧ�ܺ�Ͽ����ӣ��´���������
			iRet = ABS(iRet);
			return MODEM_COMM_ERR(iRet);
		}
		iRelSendlen += iRet;
	}

	return 0;
}

// Modem����
int PSTNRxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen)
{
	int		iRet;
	
	switch( sg_stCurCfg.stPSTNPara.ucSendMode )
	{
	case MODEM_COMM_ASYNC:
		iRet = PSTNAsyncRxd(psRxdData, uiExpLen, uiTimeOut, puiOutLen);
		break;
		
	case MODEM_COMM_SYNC:
		iRet = PSTNSyncRxd(psRxdData, uiExpLen, uiTimeOut, puiOutLen);
		break;
		
	default:
		iRet = ERR_COMM_INV_PARAM;
	}
	
	return iRet;
}


// Modemͬ������
int PSTNSyncRxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen)
{
	int  iRet,  iRetryCnt;
	int  iOldTime,iNowTime;
	uint32_t  ulRealRecvLen = 0;
	int  iRecvFlag=0;

	iRet = MODEM_ERRNO_ERROR;
	iRetryCnt = 0;
	iOldTime = sysGetTimerCount();
	while( 1 )
	{
		iNowTime= sysGetTimerCount();
		if( (iNowTime-iOldTime) >= (uint32_t)(uiTimeOut *1000) )	// ��鶨ʱ��
		{
			iRet = ERR_COMM_TIMEOUT;
			break;
		}
		
//		lcdDisplay(87,4,DISP_CFONT|DISP_CLRLINE,"(%d)",(iNowTime-iOldTime)/1000);
		lcdDisplay(0,4,DISP_CFONT|DISP_CLRLINE,"������...(%d)",(iNowTime-iOldTime)/1000);
		iRet = modem_read_timeout(sg_iModemfd,psRxdData+ulRealRecvLen,uiExpLen,1000);
		if (iRet > 0)
		{
			ulRealRecvLen += iRet;
			iRecvFlag = 1;			
			*puiOutLen = ulRealRecvLen;
		}
		else if( iRet==0 )
		{
			if( iRecvFlag==0 )
			{
				iRetryCnt++;
				if (iRetryCnt>3)
				{
					iRet = ABS(iRet);
					iRet = MODEM_COMM_ERR(iRet);
				}
			}
			if( iRecvFlag==1 )
			{
				iRet = 0;
				break;
			}

		}
		else
		{
			if (iRet < 0)
			{
				iRet = PosixGeterrno();
				iRet = ABS(iRet);
				iRet = MODEM_COMM_ERR(iRet);
				return iRet;
			}
			
			sysDelayMs(100);
		}
	}
	
	if(iRet != 0)
	{
		iRet = ABS(iRet);
		iRet = MODEM_COMM_ERR(iRet);
	}
	return iRet;
}

// Modem�첽����
int PSTNAsyncRxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen)
{
	int  iRet,  iRetryCnt;
	int  iOldTime,iNowTime;
	uint32_t  ulRealRecvLen = 0;
	unsigned long  ulTemp = 0;
	
	iRet = MODEM_ERRNO_ERROR;
	iRetryCnt = 0;
	iOldTime = sysGetTimerCount();
	while( 1 )
	{
		iNowTime= sysGetTimerCount();
		if( (iNowTime-iOldTime) >= (uint32_t)(uiTimeOut *1000) )	// ��鶨ʱ��
		{
			iRet = ERR_COMM_TIMEOUT;
			break;
		}
		
//		lcdDisplay(87,4,DISP_CFONT|DISP_CLRLINE,"(%d)",(iNowTime-iOldTime)/1000);
		lcdDisplay(0,4,DISP_CFONT|DISP_CLRLINE,"������...(%d)",(iNowTime-iOldTime)/1000);
		iRet = modem_read_timeout(sg_iModemfd,psRxdData+ulRealRecvLen,uiExpLen,850);
		if (iRet > 0)
		{			
			PubBcd2Long(psRxdData, 2, &ulTemp);
			ulRealRecvLen += iRet;
			if (ulRealRecvLen < ulTemp+2)
			{
				continue;
			}
			
			*puiOutLen = ulRealRecvLen;
			iRet = 0;
			break;
		}
		else
		{			
			if (iRet < 0)
			{
				iRet = PosixGeterrno();
				iRet = ABS(iRet);
				iRet = MODEM_COMM_ERR(iRet);
				return iRet;
			}
			
			iRetryCnt++;
			if (iRetryCnt>3)
			{
				iRet = ABS(iRet);
				iRet = MODEM_COMM_ERR(iRet);
			}
			
			sysDelayMs(100);
		}
	}
	
	if(iRet != 0)
	{
		iRet = ABS(iRet);
		iRet = MODEM_COMM_ERR(iRet);
	}
	return iRet;
}

// Modem �һ�
int PSTNOnHook(uint8_t bReleaseAll)
{
	int  iRet,iStatus;

	modem_wait_until_sent(sg_iModemfd);

	// �ų�������ιҶϵ�����
	iRet = modem_get_status(sg_iModemfd, &iStatus);
	if ( ! (iStatus == (0x80000000 | MODEM_STATE_CONNECT)) )
		iRet = modem_hangup(sg_iModemfd);

	if (bReleaseAll)
	{
		while(1)
		{
			iRet = modem_get_status(sg_iModemfd, &iStatus);
			if (iRet != 0)
				break;
			
			if (iStatus & 0x80000000) // Ŀǰ���λΪ1��ʾ������û��ɣ���Ҫ�ٴμ��
			{
				sysDelayMs(100);
				continue;
			}

			break;
		}
		iRet = modem_close(sg_iModemfd);
		sg_iModemfd = -1;
	}
	
	if(iRet != 0)
	{
		iRet = ABS(iRet);
		iRet = MODEM_COMM_ERR(iRet);
	}
	
	return iRet;
}


int  PSTNGetStatus(int *piStatus)
{
	int  iRet, iStatus;
	int  uiOldTime , iTimeOut;

	iTimeOut = sg_stCurCfg.stPSTNPara.stDialPara.dial_timeo;
	if (iTimeOut >= 30 || iTimeOut<=0)
	{
		iTimeOut = 30;
	}
	uiOldTime = sysGetTimerCount();
	while (1) 
	{
		if ( sysGetTimerCount() >= (uiOldTime+iTimeOut*1000) )
		{ // ��ʱ10s���򷵻�
			modem_get_last_errno(sg_iModemfd,&iStatus);
			iStatus = ABS(iStatus);	
			if (iStatus != 0)
			{
				return MODEM_COMM_ERR(iStatus);
			}
			else
			{
				return MODEM_COMM_ERR(ERR_COMM_STATUS_TIMEOUT);
			}			
		}

		iRet = modem_get_status(sg_iModemfd, &iStatus);
		if (iRet != 0)
			break;
		
		if (iStatus & 0x80000000) // Ŀǰ���λΪ1��ʾ������û��ɣ���Ҫ�ٴμ��
		{
			continue;
		}
		
		*piStatus = iStatus & 0x7FFFFFFF;
		break;
	}
	
	if(iRet != 0)
	{
		iRet = ABS(iRet);
		iRet = MODEM_COMM_ERR(iRet);
	}

	return iRet;
}

int  PSTNInitModem(ModemDialParms_t *pstModemParam)
{
	int  iRet, iStatus;
	ModemDialParms_t  stOldModemParam;
	
	iStatus = MODEM_STATE_NOT_INIT;
	iRet = PSTNGetStatus(&iStatus);
	if (iRet !=0 ) 
	{
		if (sg_iModemfd < 0 )  // ���modem�����Ч�����ȴ�
		{
			PSTNOnHook(TRUE);
			sg_iModemfd = PSTNOpenModem(); 
			if (sg_iModemfd < 0)
			{
				return MODEM_COMM_ERR(ERR_COMM_MODEM_INIT); 
			}
		}
		else
		{
			iRet = ABS(iRet);
			return MODEM_COMM_ERR(iRet);
		}
	}

	iStatus = MODEM_STATE_NOT_INIT;
	iRet = PSTNGetStatus(&iStatus);
	if (iRet != 0 || iStatus == MODEM_STATE_NOT_INIT)
	{
		return MODEM_COMM_ERR(MODEM_STATE_NOT_INIT);
	}

	iRet = modem_get_dial_parms(sg_iModemfd, &stOldModemParam);
	if (iRet != 0 || memcmp(&stOldModemParam,pstModemParam,sizeof(ModemDialParms_t)) !=0 )
	{
		iRet = modem_set_dial_parms(sg_iModemfd,pstModemParam);
	}
	else
	{
		iRet = 0;
	}
	
	if(iRet != 0)
	{
		iRet = ABS(iRet);
		iRet = MODEM_COMM_ERR(iRet);
	}

	return iRet;  
}


INT32  PSTNOpenModem(void)
{
	int   iFd;

	iFd = modem_open(MODEM_PATHNAME, O_RDWR); 
	if (iFd < 0)
	{
		iFd = modem_open(MODEM_PATHNAME1, O_RDWR); 
	}

	return iFd;
}
//////////////////////////////////////////////////////////////////////////
// GPRS/CDMA ͨѶģ��
//////////////////////////////////////////////////////////////////////////

#define WIRELESS_ERR(a)		(ERR_COMM_WIRELESS_BASE|(a))
int InitWirelessModule(ST_WIRELESS_INFO *pstParam)
{
	int   iRet,iCnt;

	sg_Wiresocket = -1;	
	iRet = WnetInit(20000);
	if( iRet!= NET_OK )
	{		
		if (iRet<0)
		{
			iRet = -iRet;
		}
		return WIRELESS_ERR(iRet);
	}

	iCnt = 2;  // 2�μ��SIM��������ʱ2S�����ֹSIM���ϵ�ʱ�䲻��������
	while(iCnt--)
	{
		// check PIN of SIM card
		iRet = WnetCheckSim();
		if( iRet == -NET_ERR_PIN )
		{
			iRet =  WnetInputSimPin((char *)pstParam->szPwd);
			if( iRet != NET_OK )
			{
				if (iRet<0)
				{
					iRet = -iRet;
				}
				return WIRELESS_ERR(iRet);
			}

			DispWirelessSignal();
			return 0;
		}
		else
		{	
			if (iRet != NET_OK)
			{
				if (iRet<0)
				{
					iRet = -iRet;
				}
				iRet = WIRELESS_ERR(iRet);
				sysDelayMs(1000);
				continue;
			}
			else
			{
				DispWirelessSignal();
				return 0;
			}
		}
	}
	
	DispWirelessSignal();
	return iRet;
}

#if 1
int WirelessDial(uint8_t ucDialFlag)
{
	int iRet,iFlag,iRetyTimes,iDialTimes;
	uint32_t   uiOldTime;

	kbFlush();
	// Ԥ����
	if( ucDialFlag== PREDIAL_MODE )  
	{
		sg_Wire_PreDial_Ret = 0;
		iRet = PPPCheck(OPT_DEVWNET);
		if (iRet < 0)
		{
			if (iRet == -ETIMEDOUT)
			{   // ��ʱ�˳����ȸ�λģ��
				WirelessReset(&sg_stCurCfg.stWirlessPara);
			}
			while((UINT32)(sysGetTimerCount() - sg_uiWirePPPLogoutTime) < 3000); // �ҶϺ�����3�����ϲ����ٲ���
			iRet = PPPLogin(OPT_DEVWNET,(char *)sg_stCurCfg.stWirlessPara.szAPN,
				            (char *)sg_stCurCfg.stWirlessPara.szUID,
							(char *)sg_stCurCfg.stWirlessPara.szPwd,0,0);
			sg_ucWirePPPFlag = 1;	// ÿ��������PPP���ŵĶ������������ɹ���񣬶�������һ�ιҶ�ppp�Ķ���			
			if (iRet<0)
			{
				iRet = -iRet;
			}
			sg_Wire_PreDial_Ret = iRet;
		}

		return WIRELESS_ERR(iRet);
	}

	iDialTimes = 1;
	iFlag = 1;
	iRetyTimes = 1;
	uiOldTime = sysGetTimerCount();
	while (iFlag)
	{
		if( PubChkKeyInput() )
		{
			if (kbGetKey() == KEY_CANCEL)
			{
				return ERR_USERCANCEL;
			}
		}

		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 3, DISP_CFONT, "    �� �� ��   ");
		lcdDisplay(0, 5, DISP_CFONT|DISP_CLRLINE, "  DIALING...(%d)", iDialTimes++); 
		DrawRect(0, 17, 127, 63);

		if( sg_Wire_PreDial_Ret==NET_ERR_NOSIM ||
			sg_Wire_PreDial_Ret==NET_ERR_SIMDESTROY)
		{
			WirelessClose(TRUE);
			return WIRELESS_ERR(sg_Wire_PreDial_Ret);
		}

		iRet = PPPCheck(OPT_DEVWNET);
		if (iRet < 0)
		{
			/****
			��δ�����λ�����ԭ���ǣ������ʱ���������ppp���ӽ����ĳ�ʱʱ�䣬��ô��ֻ�е�pppcheck���ط�0ʱ������ȥ�ж��Ƿ�ʱ�����򣬽�
			���ܳ�����������������һ��pppcheck�Ѿ��ɹ��ˣ�������һ��tcp���ӣ�����ʧ���ˣ�������ѭ����ȴ��Ϊ�����ʱ�ж϶��˳��˺�����
			*****/
			if ( sysGetTimerCount() >= (uiOldTime+60000) )
			{ // �������ӳ�ʱ60s���򷵻�
				WirelessClose(TRUE);
				return WIRELESS_ERR(NET_ERR_RETRY);
			}
			
			/****
			��δ����ƽ�����ԭ���ǣ����ڳ�ʱ�ж��Ѿ��ƽ����ˣ�����Ϊ�˷��ӳ�ʱ�жϵ����ң��������δ���Ҳ�ƽ�����
			****/
			if (iRet == -NET_ERR_LINKOPENING)
			{
				sysDelayMs(400);
				iDialTimes--;  // ��鲦�Ž������Ҫ�Ӳ��Ŵ���
				continue;
			}
		
			CommOnHook(FALSE);// �ȹҶ�����
			if (iRet == -ETIMEDOUT)
			{   // ��ʱ�˳����ȸ�λģ��
				WirelessReset(&sg_stCurCfg.stWirlessPara);
			}
			while((UINT32)(sysGetTimerCount() - sg_uiWirePPPLogoutTime) < 3000); // �ҶϺ�����3�����ϲ����ٲ���
			iRet = PPPLogin(OPT_DEVWNET,(char *)sg_stCurCfg.stWirlessPara.szAPN,
				            (char *)sg_stCurCfg.stWirlessPara.szUID,
							(char *)sg_stCurCfg.stWirlessPara.szPwd,0,20000);
			sg_ucWirePPPFlag = 1; // ÿ��������PPP���ŵĶ������������ɹ���񣬶�������һ�ιҶ�ppp�Ķ���		
			if (iRet<0)
			{
				iRet = -iRet;
			}
			sg_Wire_PreDial_Ret = iRet;
			sysDelayMs(100);
			continue;
		}

		// ����Ŀǰʹ�õ��׽��������0,1,2 . �������sg_Wiresocketֵ�쳣���Ƚ��丳��ֵ(�Ҷ�)
		if(sg_Wiresocket<0 || sg_Wiresocket>3)  
			CommOnHook(FALSE);
			
		if (sg_Wiresocket == -1)
		{
			//  ʵ������
			sg_Wiresocket = NetSocket(AF_INET,SOCK_STREAM,0);
			if (sg_Wiresocket < 0)
			{
				iRet = sg_Wiresocket;
				if (iRet<0)
				{
					iRet = -iRet;
				}

				WirelessClose(FALSE);
				return WIRELESS_ERR(iRet);
			}

			iRet = SetSockAddr(&sg_stCurCfg);
			if (iRet != NET_OK)
			{
				if (iRet<0)
				{
					iRet = -iRet;
				}
				WirelessClose(FALSE);
				return WIRELESS_ERR(iRet);
			}

			lcdClrLine(2,7);
			lcdFlip();	
			lcdDisplay(0,3,DISP_CFONT,"   �� �� �� ... ");
			lcdDisplay(0,5,DISP_CFONT,"TCPIP CONNECT(%d)",iRetyTimes++); 
			DrawRect(0, 17, 127, 63);

			iRet = NetConnect(sg_Wiresocket,&sg_stWiresockaddr,sizeof(struct sockaddr));
			/***
			��δ�����ô�޸ĵ�ԭ���ǣ�����tcp���ӳ��Դ����Ƿ��޵��ж��Ѿ����ں��棬���ԣ������������޸ġ�
			��tcp����ʧ�ܣ���Ȼ��ִ�к���Ĵ����жϴ��롣
			***/						
			if (NET_OK == iRet)
			{
				return iRet;
			}

			/***
			������δ����ԭ����: ������-1043ʱ��˵���û���ȡ����ȡ��TCP���ӣ���ôӦ���˳�ͨѶ��
			***/
			if (-NET_ERR_USER_CANCEL == iRet)
			{
				return ERR_USERCANCEL;
			}	
		}
		else
		{ 
			// Ŀǰ�����ܼ����׽����Ƿ񻹴������ӣ������У�ֻ��Ӧ���Լ��ϸ�????????
			return 0;
		}
		
		/***
		��δ����Ƶ������ԭ���ǣ���Ȼ���һ�γ���tcp���Ӷ����ɹ��ˣ���������Ҫѭ������ͷȥ����ֱ���˳�������
		���ң���δ�����ǰ���һ��Σ���ǣ�������һ�γ���tcp����ʧ�ܣ�ѭ����ͷ����pppcheck��飬���pppcheck���ط�0����ô������ȥ��ppplogin����ʱ����ֱ��
		ppp���ӽ��������ˣ�ȴ����δ����˳��˺��������������ù�������ʱ�䡣
		***/
		if (iRetyTimes > 3 )
		{// ���β������Ӳ��ɹ���ر��׽��֣��Ͽ�PPP����
			WirelessClose(TRUE);
			return WIRELESS_ERR(NET_ERR_RETRY);
		}
		else /***��δ�����ǰ��tcp����ʧ�ܺ�Ĵ����Ƶ���������***/
		{
			CommOnHook(FALSE);  // �Ҷ�TCPIP��������
			iRet = WIRELESS_ERR(iRet);
			continue;
		}
	}
	
	return iRet;
}
#else
int WirelessDial(uint8_t ucDialFlag)
{
	int iRet,iFlag,iRetyTimes,iDialTimes;
	uint32_t   uiOldTime;

	kbFlush();
	// Ԥ����
	if( ucDialFlag== PREDIAL_MODE )  
	{
		sg_Wire_PreDial_Ret = 0;
		iRet = PPPCheck(OPT_DEVWNET);
		if (iRet < 0)
		{
			if (iRet == -ETIMEDOUT)
			{   // ��ʱ�˳����ȸ�λģ��
				WirelessReset(&sg_stCurCfg.stWirlessPara);
			}
			iRet = PPPLogin(OPT_DEVWNET,(char *)sg_stCurCfg.stWirlessPara.szAPN,
				            (char *)sg_stCurCfg.stWirlessPara.szUID,
							(char *)sg_stCurCfg.stWirlessPara.szPwd,0,0);
			if (iRet<0)
			{
				iRet = -iRet;
			}
			sg_Wire_PreDial_Ret = iRet;
		}

		return WIRELESS_ERR(iRet);
	}

	iDialTimes = 1;
	iFlag = 1;
	iRetyTimes = 1;
	uiOldTime = sysGetTimerCount();
	while (iFlag)
	{
		if ( sysGetTimerCount() >= (uiOldTime+60000) )
		{ // �������ӳ�ʱ60s���򷵻�
			WirelessClose(TRUE);
			return WIRELESS_ERR(NET_ERR_RETRY);
		}

		if( PubChkKeyInput() )
		{
			if (kbGetKey() == KEY_CANCEL)
			{
				return ERR_USERCANCEL;
			}
		}

		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 3, DISP_CFONT, "    �� �� ��   ");
		lcdDisplay(0, 5, DISP_CFONT|DISP_CLRLINE, "  DIALING...(%d)", iDialTimes++); 
		DrawRect(0, 17, 127, 63);

		if( sg_Wire_PreDial_Ret==NET_ERR_NOSIM ||
			sg_Wire_PreDial_Ret==NET_ERR_SIMDESTROY)
		{
			WirelessClose(TRUE);
			return WIRELESS_ERR(sg_Wire_PreDial_Ret);
		}

		iRet = PPPCheck(OPT_DEVWNET);
		if (iRet == -NET_ERR_LINKOPENING)
		{
			sysDelayMs(400);
			iDialTimes--;  // ��鲦�Ž������Ҫ�Ӳ��Ŵ���
			continue;
		}
		if (iRet < 0)
		{
			CommOnHook(FALSE);// �ȹҶ�����
			if (iRet == -ETIMEDOUT)
			{   // ��ʱ�˳����ȸ�λģ��
				WirelessReset(&sg_stCurCfg.stWirlessPara);
			}
			iRet = PPPLogin(OPT_DEVWNET,(char *)sg_stCurCfg.stWirlessPara.szAPN,
				            (char *)sg_stCurCfg.stWirlessPara.szUID,
							(char *)sg_stCurCfg.stWirlessPara.szPwd,0,20000);
			if (iRet<0)
			{
				iRet = -iRet;
			}
			sg_Wire_PreDial_Ret = iRet;
			sysDelayMs(100);
			continue;
		}

		if (iRetyTimes > 3 )
		{// ���β������Ӳ��ɹ���ر��׽��֣��Ͽ�PPP����
			WirelessClose(TRUE);
			return WIRELESS_ERR(NET_ERR_RETRY);
		}

		// ����Ŀǰʹ�õ��׽��������0,1,2 . �������sg_Wiresocketֵ�쳣���Ƚ��丳��ֵ(�Ҷ�)
		if(sg_Wiresocket<0 || sg_Wiresocket>3)  
			CommOnHook(FALSE);
			
		if (sg_Wiresocket == -1)
		{
			//  ʵ������
			sg_Wiresocket = NetSocket(AF_INET,SOCK_STREAM,0);
			if (sg_Wiresocket < 0)
			{
				iRet = sg_Wiresocket;
				if (iRet<0)
				{
					iRet = -iRet;
				}

				WirelessClose(FALSE);
				return WIRELESS_ERR(iRet);
			}

			iRet = SetSockAddr(&sg_stCurCfg);
			if (iRet != NET_OK)
			{
				if (iRet<0)
				{
					iRet = -iRet;
				}
				WirelessClose(FALSE);
				return WIRELESS_ERR(iRet);
			}

			lcdClrLine(2,7);
			lcdFlip();	
			lcdDisplay(0,3,DISP_CFONT,"   �� �� �� ... ");
			lcdDisplay(0,5,DISP_CFONT,"TCPIP CONNECT(%d)",iRetyTimes++); 
			DrawRect(0, 17, 127, 63);

			iRet = NetConnect(sg_Wiresocket,&sg_stWiresockaddr,sizeof(struct sockaddr));
			if (iRet != NET_OK)
			{
				CommOnHook(FALSE);  // �Ҷ�TCPIP��������
				iRet = WIRELESS_ERR(iRet);
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{ 
			// Ŀǰ�����ܼ����׽����Ƿ񻹴������ӣ������У�ֻ��Ӧ���Լ��ϸ�????????
			return 0;
		}
	}
	
	return iRet;
}
#endif

int WirelessReset(ST_WIRELESS_INFO *pstParam)
{
    int   iRet;
	
	WirelessClose(TRUE);
	
    iRet = WnetReset();
	if( iRet!= NET_OK )
	{
		if (iRet<0)
		{
			iRet = -iRet;
		}
		return WIRELESS_ERR(iRet);
	}
	sysDelayMs(5000); // ��λ����ʱ5s
	
	return InitWirelessModule(pstParam);
}


//����ͨѶ����
int WirelessRxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen)
{
	int  iRet,  iRetryCnt;
	int  iOldTime,iNowTime;
	uint32_t  ulRealRecvLen = 0;
	unsigned long  ulTemp = 0;
	struct timeval stTimeVal;
	socklen_t      Socklen;

	stTimeVal.tv_sec = 1;
	stTimeVal.tv_usec = 0;
	Socklen = sizeof(stTimeVal);
	NetSetsockopt(sg_Wiresocket,SOL_SOCKET,SO_RCVTIMEO,&stTimeVal,Socklen);
	
	iRetryCnt = 0;
	iOldTime = sysGetTimerCount();
	while( 1 )
	{
		iNowTime= sysGetTimerCount();
		if( (iNowTime-iOldTime) >= (uint32_t)(uiTimeOut *1000) )	// ��鶨ʱ��
		{
			iRet = ERR_COMM_TIMEOUT;
			break;
		}
		
//		lcdDisplay(87,4,DISP_CFONT|DISP_CLRLINE,"(%d)",(iNowTime-iOldTime)/1000);
		lcdDisplay(0,4,DISP_CFONT|DISP_CLRLINE,"������...(%d)",(iNowTime-iOldTime)/1000);
		lcdFlip();
		iRet = NetRecv(sg_Wiresocket,psRxdData+ulRealRecvLen,uiExpLen-ulRealRecvLen,0);
		if (iRet > 0)
		{
			PubChar2Long(psRxdData, 2, &ulTemp);
			ulRealRecvLen += iRet;			
			if (ulRealRecvLen < ulTemp+2)
			{
				continue;
			}
	
			*puiOutLen = ulRealRecvLen;
			iRet = 0;
			break;
		}
		else
		{
			iRetryCnt++;
			if (iRetryCnt>3)
			{
				if (iRet<0)
				{
					iRet = -iRet;
				}
				iRet = WIRELESS_ERR(iRet);
			}

			sysDelayMs(20);
		}
	}

	return iRet;
}

// ����ͨѶ����
int WirelessTxd(uint8_t *psTxdData, uint32_t uiDataLen, uint32_t uiTimeOut)
{
	int  iRet,iRelSendlen;
	struct timeval stTimeVal;
	
	stTimeVal.tv_sec = 30;
	stTimeVal.tv_usec = 0;
	NetSetsockopt(sg_Wiresocket,SOL_SOCKET,SO_SNDTIMEO,&stTimeVal,sizeof(stTimeVal));

	for (iRelSendlen=0; iRelSendlen<uiDataLen;)
	{
		iRet = NetSend(sg_Wiresocket,psTxdData+iRelSendlen,(uiDataLen-iRelSendlen),0);
		if (iRet < 0)
		{
			WirelessClose(TRUE);   // ����ʧ�ܺ�Ͽ����ӣ��´���������
			return WIRELESS_ERR(iRet);
		}
		iRelSendlen += iRet;
	}

	return 0;
}

// ����ģ��һ�
int WirelessClose(uint8_t bRelease)
{
	int  iRet = 0;

	if(sg_Wiresocket != -1)
	{
		iRet = NetClose(sg_Wiresocket);
		if (-NET_ERR_USER_CANCEL == iRet)
		{
			return ERR_USERCANCEL;
		}
		sg_Wiresocket = -1;
	}

	if (bRelease && 0 != sg_ucWirePPPFlag)
	{
		iRet = PPPLogout(OPT_DEVWNET);
		if (NET_OK != iRet && -NET_ERR_LINKCLOSED != iRet && 
			-NET_ERR_USER_CANCEL != iRet)
		{
			iRet = WnetReset();
			if( iRet!= NET_OK )
			{
				if (iRet<0)
				{
					iRet = -iRet;
				}
				return WIRELESS_ERR(iRet);
			}
			sysDelayMs(5000); // ��λ����ʱ5s
			
			iRet = InitWirelessModule(&stPosParam.stTxnCommCfg.stWirlessPara); //���Ų��������׺�tms����һ�µ�
			if (0 == iRet)
			{
				sg_ucWirePPPFlag = 0;
			}
			return iRet;
		}

		if (NET_OK == iRet)
		{
			sg_uiWirePPPLogoutTime = sysGetTimerCount();
		}

		if (NET_OK == iRet || -NET_ERR_LINKCLOSED == iRet)
		{
			sg_ucWirePPPFlag = 0;
		}
		
	}
	
	return WIRELESS_ERR(iRet);
}

// ��ʾ�ź�ǿ��
void DispWirelessSignal(void)
{
	int  iRet, iSignal;
	
	if( sg_stCurCfg.ucCommType!=CT_GPRS && sg_stCurCfg.ucCommType!=CT_CDMA )
	{
		return;
	}
	
	iRet = WnetSignal(&iSignal);
	if( (iRet != 0 ) || (iSignal == 99) )
	{
		lcdSetIcon(ICON_SIGNAL, CLOSEICON);
		return;
	}
	
    if((iSignal >= 1) && (iSignal <= 7))
    {
        iSignal = 1;
    }
    else if((iSignal >= 8) && (iSignal <= 13))
    {
        iSignal = 3;
    }
    else if((iSignal >= 14) && (iSignal <= 19))
    {
        iSignal = 4;
    }
    else if((iSignal >= 20) && (iSignal <= 25))
    {
        iSignal = 5;
    }
    else if((iSignal >= 26) && (iSignal <= 31))
    {
        iSignal = 6;
    }

	lcdSetIcon(ICON_SIGNAL, iSignal);
}


//////////////////////////////////////////////////////////////////////////
// WIFI ͨѶģ��
//////////////////////////////////////////////////////////////////////////
#define WIFI_COMM_ERR(a)		(ERR_COMM_WIFI_BASE|(a))
int   InitWiFiModule(uint8_t bInitModule)
{
	int       iRet, iRetry, iTemp;
    char      szlocalIP[32];
    WiFiStatus_t    stWiFiStatus;

	if (!(gstPosCapability.uiCommType & CT_WIFI))
	{
		return ERR_COMM_INV_TYPE;
	}

	if (bInitModule)
	{
		iRet = WnetInit(20000);
		if( iRet!= NET_OK )
		{		
			if (iRet<0)
			{
				iRet = -iRet;
			}
			return WIFI_COMM_ERR(iRet);
		}
	}

	memset(szlocalIP, 0, sizeof(szlocalIP));
	iRetry = 0;
	while(1)
	{
        // �ȼ������״̬
		while(1)
		{
			iRetry++;
			lcdClrLine(2, 7);
			lcdFlip();	
			lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"���Ժ�...");
			lcdDisplay(0,5,DISP_CFONT|DISP_MEDIACY|DISP_CLRLINE,"WIFI������..(%d)",iRetry);
			DrawRect(0, 17, 127, 63);
			iRet = WifiGetCurConnect(&stWiFiStatus);
			iTemp = iRet;
			if (iRet != NET_OK || stWiFiStatus.Status != WIFI_STATUS_CONNECTED)
			{
				if (iTemp== NET_OK)
				{
					iTemp = WIFI_COMM_ERR(EWIFI_ONLINELOST);
				}
				lcdClrLine(2,7);
				lcdFlip();	
				PubDispString("��ȡ��ǰ����ʧ��",2|DISP_LINE_CENTER);
				PubDispString("�����ܻ�˵���",4|DISP_LINE_CENTER);
				PubDispString("����WIFI���ò���",6|DISP_LINE_CENTER);
				iRet = PubWaitKey(6);
				if (iRet == KEY_FN || iRet == KEY_MENU)
				{
					iRet = SelectAndSetWIFIAP();
					if (iRetry>=3)
					{
						return  WIFI_COMM_ERR(iTemp);
					}
					sysDelayMs(1000);// ��ʱ1s
					continue;
				}
				else
				{
					return  WIFI_COMM_ERR(iTemp);
				}
			}
			break;
		}

		// ����Ƿ��ȡ����IP��ַ
		iRetry = 0;
		while(1)
		{
			iRetry++;
			lcdClrLine(2,7);
			lcdFlip();	
			lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"���Ժ�...");
			lcdDisplay(0,5,DISP_CFONT|DISP_MEDIACY|DISP_CLRLINE,"��ȡ����IP..(%d)",iRetry);
			DrawRect(0, 17, 127, 63);
			memset(szlocalIP, 0, sizeof(szlocalIP));
			iRet = WifiGetLocalIP(szlocalIP);
			if(iRet != NET_OK)
			{
				iTemp = iRet;
				lcdClrLine(2,7);
				lcdFlip();	
				PubDispString("��ȡ��ǰIPʧ��",2|DISP_LINE_CENTER);
				PubDispString("�����ܻ�˵���",4|DISP_LINE_CENTER);
				PubDispString("����WIFI���ò���",6|DISP_LINE_CENTER);
				iRet = PubWaitKey(6);
				if (iRet == KEY_FN || iRet == KEY_MENU)
				{
					iRet = SelectAndSetWIFIAP();
					if (iRetry>=3)
					{
						return  WIFI_COMM_ERR(iTemp);
					}
					continue;
				}
				else
				{
					return  WIFI_COMM_ERR(iTemp);
				}
			}

			if (strcmp(szlocalIP, "0.0.0.0") == 0)
			{
				if (iRetry>=3)
				{
					return  WIFI_COMM_ERR(EWIFI_SOCKIPREG);
				}
				sysDelayMs(1000);
				continue;
			}
			else
			{
				break;
			}
		}

		if (strcmp(szlocalIP, "0.0.0.0") != 0)
		{
			strcpy((char *)stPosParam.stTxnCommCfg.stWIFIIpPara.szLocalIP,szlocalIP);
			break;
		}
	}

	return 0;
}

int   GetWiFiStatus(void)
{
	int       iRet, iRetry, iTemp;
    char      szlocalIP[32];
    WiFiStatus_t    stWiFiStatus;

	if (!(gstPosCapability.uiCommType & CT_WIFI))
	{
		return ERR_COMM_INV_TYPE;
	}

	memset(szlocalIP, 0, sizeof(szlocalIP));
	iRetry = 0;
	while(1)
	{
        // �ȼ������״̬
		while(1)
		{
			iRetry++;
			lcdClrLine(2,7);
			lcdFlip();	
			lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"���Ժ�...");
			lcdDisplay(0,5,DISP_CFONT|DISP_MEDIACY|DISP_CLRLINE,"WIFI������..(%d)",iRetry);
			DrawRect(0, 17, 127, 63);
			iRet = WifiGetCurConnect(&stWiFiStatus);
			iTemp = iRet;
			if (iRet != NET_OK || stWiFiStatus.Status != WIFI_STATUS_CONNECTED)
			{
				if (iTemp== NET_OK)
				{
					iTemp = WIFI_COMM_ERR(EWIFI_ONLINELOST);
				}

				if (iRetry>=3)
				{
					return  WIFI_COMM_ERR(iTemp);
				}
				sysDelayMs(2000);// ��ʱ1s
				continue;	
			}

			break;
		}

		// ����Ƿ��ȡ����IP��ַ
		iRetry = 0;
		while(1)
		{
			iRetry++;
			lcdClrLine(2,7);
			lcdFlip();	
			lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"���Ժ�...");
			lcdDisplay(0,5,DISP_CFONT|DISP_MEDIACY|DISP_CLRLINE,"��ȡ����IP..(%d)",iRetry);
			DrawRect(0, 17, 127, 63);
			memset(szlocalIP, 0, sizeof(szlocalIP));
			iRet = WifiGetLocalIP(szlocalIP);
			if(iRet != NET_OK)
			{
				if (iRetry>=3)
				{
					return  WIFI_COMM_ERR(iRet);
				}
				continue;
			}

			if (strcmp(szlocalIP, "0.0.0.0") == 0)
			{
				if (iRetry>=3)
				{
					return  WIFI_COMM_ERR(EWIFI_SOCKIPREG);
				}
				sysDelayMs(2000);
				continue;
			}
			else
			{
				break;
			}
		}

		if (strcmp(szlocalIP, "0.0.0.0") != 0)
		{
			break;
		}
	}

	return 0;
}

int   WiFiDial(uint8_t ucDialMode)
{
	int iRet,iFlag,iRetyTimes;
	uint32_t   uiOldTime;
	
	if( ucDialMode==PREDIAL_MODE )
	{
		iRet = InitWiFiModule(FALSE);
		return iRet;
	}
	
    //  ʵ������
	iFlag = 1;
	iRetyTimes = 1;
	uiOldTime = sysGetTimerCount();	
	while (iFlag)
	{
		if ( sysGetTimerCount() >= (uiOldTime+60000) )
		{ // �������ӳ�ʱ60s���򷵻�
			TcpOnHook(FALSE);
			return (ERR_COMM_TCPIP_BASE|NET_ERR_RETRY);
		}
				
		if( PubChkKeyInput() )
		{
			if (kbGetKey() == KEY_CANCEL)
			{
				PSTNOnHook(FALSE);
				return ERR_USERCANCEL;
			}
		}

		if (iRetyTimes > 3 )
		{// ���β������Ӳ��ɹ���ر��׽��֣��Ͽ�PPP����
			TcpOnHook(FALSE);
			return (ERR_COMM_TCPIP_BASE|NET_ERR_RETRY);
		}

		// ����Ƿ�������WIFI,���ʱ�����û�����ӳɹ��Ͳ���ʾ�û�����WIFI���ö���ֱ�ӷ���
		iRet = GetWiFiStatus();
		if (iRet != 0)
		{
			return iRet;
		}

		if (sg_Tcpsocket < 0)
		{
			sg_Tcpsocket = NetSocket(AF_INET,SOCK_STREAM,0);
			if (sg_Tcpsocket < 0)
			{
				TcpOnHook(FALSE);
				iRet = sg_Tcpsocket;
				if (iRet<0)
				{
					iRet = -iRet;
				}
				return (ERR_COMM_TCPIP_BASE|iRet);
			}
			iRet = SetSockAddr(&sg_stCurCfg);
			if (iRet != NET_OK)
			{
				if (iRet<0)
				{
					iRet = -iRet;
				}
				TcpOnHook(TRUE);
				return ERR_COMM_TCPIP_BASE|iRet;
			}
			
			lcdClrLine(2,7);
			lcdFlip();	
    		lcdDisplay(0,3,DISP_CFONT,"   �� �� �� ... ");
		    lcdDisplay(0,5,DISP_CFONT,"TCPIP CONNECT(%d)",iRetyTimes++);
			DrawRect(0, 17, 127, 63);
			
			iRet = NetConnect(sg_Tcpsocket,&sg_stTcpsockaddr,sizeof(struct sockaddr));
			if (iRet != NET_OK)
			{
				CommOnHook(FALSE);  // �Ҷ�TCPIP��������
				iRet = ERR_COMM_TCPIP_BASE|iRet;
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			return 0;
		}
	}
	
	return iRet;
}

// ����LRC
uint8_t CalcLRC(uint8_t *psData, uint32_t uiLength, uint8_t ucInit)
{
	while( uiLength>0 )
	{
		ucInit ^= *psData++;
		uiLength--;
	}

	return ucInit;
}


void DispCommErrMsg(int iErrCode)
{
	COMM_ERR_MSG	stCommErrMsg;
	
	memset((char*)&stCommErrMsg,0,sizeof(stCommErrMsg));
	CommGetErrMsg(iErrCode, &stCommErrMsg);
	lcdClrLine(2,7);
	lcdFlip();	
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, (char *)stCommErrMsg.szChnMsg);
	lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, (char *)stCommErrMsg.szEngMsg);
	DrawRect(0, 17, 127, 63);
	sysBeef(1, 200);
	kbGetKeyMs(5000);
}

// end of file
