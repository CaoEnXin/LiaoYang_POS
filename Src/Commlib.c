
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
static UINT32       sg_uiWirePPPLogoutTime = 0; // 存储前一次ppp成功挂断的时间，从而保证有3秒的间隔才开始新的ppp拨号动作
static UINT8        sg_ucWirePPPFlag = 0; // 表示ppp链接的状态 0表示已挂断 1表示未挂断
static struct sockaddr     sg_stWiresockaddr;
static struct sockaddr     sg_stTcpsockaddr;

static ST_COMM_CONFIG	sg_stCurCfg = {0xFF};
static uint8_t		sg_sWorkBuf[LEN_WORKBUF+50];
static ERR_INFO		sg_stCommErrMsg[] =
{
	{ERR_COMM_INV_PARAM, "参数错误",     "INVALID PARAM"},
	{ERR_COMM_INV_TYPE,  "无效通讯类型", "INV COMM TYPE"},
	{ERR_COMM_CANCEL,    "用户取消",     "USER CANCEL"},
	{ERR_COMM_TIMEOUT,   "通讯超时",     "TIMEOUT"},
	{ERR_COMM_COMERR,    "通讯错误",     "COMM ERROR"},
	{ERR_COMM_TOOBIG,    "数据包太大",   "DATA TOO BIG"},
	{0, "", ""},
};

static ERR_INFO		sg_stRS232ErrMsg[] =
{
	{PORT_RET_NOTEMPTY,    "发送缓冲区满", "OVERFLOW"},
	{PORT_RET_PORTERR,     "无效端口号",   "INVALID PORT"},
	{PORT_RET_NOTOPEN,     "端口未打开",   "PORT CLOSED"},
	{PORT_RET_BUSY,        "无可用端口",   "NO PORT AVAIL"},
	{PORT_RET_MODEM_INUSE, "端口被占用",   "PORT OCCUPY"},
	{PORT_RET_PARAERR,     "无效通讯参数", "INVALID PARAM"},
	{PORT_RET_TIMEOUT,     "串口超时",     "TIMEOUT"},
	{0, "", ""},
};

static ERR_INFO		sg_stPPPErrMsg[] =
{
	{NET_ERR_RETRY,        "通讯超时",        "TIMEOUT"},
	{NET_ERR_RSP,          "无应答,检查模块", "CHECK MODULE"},
	{NET_ERR_NOSIM,        "SIM卡不存在",     "NO SIM CARD"},
	{NET_ERR_PIN,          "需要输入PIN码",   "NEED SIM PIN"},
	{NET_ERR_PUK,          "需要输入PUK码",   "NEED SIM PUK"},
	{NET_ERR_PWD,          "PIN码错误",       "SIM PIN ERROR"},
	{NET_ERR_SIMDESTROY,   "SIM卡错误",       "NO SIM/NEED PIN"},
	{NET_ERR_CSQWEAK,      "信号太弱",        "SIGNAL TOO WEAK"},
	{NET_ERR_LINKCLOSED,   "网络无载波",      "NO CARRIER"},
	{NET_ERR_LINKOPENING,  "连接成功",        "LINK OK"},
	{NET_ERR_ATTACHING,    "正在查找网络",    "SEEKING NETWORK"},
	{NET_ERR_DETTACHED,    "已断开无线网络",  "DETTACH NETWORK"},
	{NET_ERR_EMERGENCY,    "检查SIM卡",       "PLS CHECK SIM"},
	{NET_ERR_RING,         "等待接入中",      "ACCEPTED ERR"},
	{NET_ERR_BUSY,         "正在通讯中",      "COMMINICATE ERR"},
	{NET_ERR_DIALING,      "模块正在拨号",    "LINKING"},
	{NET_ERR_UNKNOWN,      "未知错误码",      "UNKNOW ERR"},
	{NET_ERR_ABNORMAL,     "未知错误码",      "UNKNOW ERR"},
	{0, "", ""},
};

//#define	EINTR		 4	/* Interrupted system call */
//#define	EIO		 5	/* I/O error */
static ERR_INFO	sg_stModemErrMsg[] =
{
	{-MODEM_ERRNO_ERROR,             "设备异常",       "MODEM ERROR"},
	{-MODEM_ERRNO_BUSY,              "设 备 忙",       "MODEM BUSY"},
	{-MODEM_ERRNO_NO_DIALTONE,       "无拨号音",       "NO DIAL TONE"},
	{-MODEM_ERRNO_NO_CARRIER,        "线路载波丢失",   "LINE BREAK"},
	{-MODEM_ERRNO_NO_LINE,           "请接好电话线",   "LINE READY ?"},
	{-MODEM_ERRNO_NO_ANSWER,         "拨号无应答",     "NO ACK"},
	{-MODEM_ERRNO_OFF_HOOK,          "请先挂机",       "PLS ONHOOK"},
	{-MODEM_ERRNO_LINE_IN_USE,       "被叫线路忙",     "LINE BUSY"},
	{-MODEM_ERRNO_UN_OBTAINABLE,     "线路不可到达",   "UNOBTAINABLE"},
	{-MODEM_ERRNO_LINE_REVERSAL,     "线路反转",       "LINE REVERSAL"},
	{-MODEM_ERRNO_EXTENSION_IN_USE,  "旁置电话占用",   "PHONE OCCUPIED"},
	{ERR_COMM_STATUS_TIMEOUT,        "连接主机超时",   "TIME OUT"},
	{ERR_COMM_MODEM_INIT,            "初始化设备失败", "MODEM INIT ERROR"},
	{NET_ERR_RETRY,                  "重试次数超限",   "RETRY OVERRUN"},
	{4,                              "请接好电话线",   "LINE READY ?"},
	{5,                              "请接好电话线",   "LINE READY ?"},
	{16,                             "设 备 忙",       "MODEM BUSY"},
	{22,                             "参数错误",       "INVALID ARGUMENT"},
	{ETIMEDOUT,                      "连接主机超时",   "TIME OUT"},
	{ETIME,                          "连接主机超时",   "TIME OUT"},
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


// 通讯模块初始化
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

// 设置通讯模块参数
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
		if( iLen<10 || iLen>20 )	// 简单检查参数
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


// 通讯模块拨号(MODEM拨号或者TCP建立TCP连接等)
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

// 通讯模块发送数据
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

// 通讯模块接收数据
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
// MODEM收到返回后马上挂断 20110331
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
		WirelessClose(FALSE);  // 接受后需要断开TCP连接，下次联机时重新连接
		break;

	default:
		iRet = ERR_COMM_INV_TYPE;
	}

	return iRet;
}

// 通讯模块断开链路(MODEM挂机或者TCP断开TCP连接等)
int CommOnHook(uint8_t bReleaseAll)
{
	int		iRet;
//	int Col, Line;
//	uint8_t Buff[2048]; // 当前屏幕为128*64，建议开辟的缓冲区大小为2K；

//	lcdStore(&Col, &Line, Buff);
	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY," 处 理 中");
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
 
// 获取通讯错误信息
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
		sprintf((char *)pstCommErrMsg->szChnMsg, "通讯错误:%04X", iReErrCode);
		sprintf((char *)pstCommErrMsg->szEngMsg, "COMM ERR:%04X", iReErrCode);
		GetAllErrMsg(iErrCode, sg_stCommErrMsg, pstCommErrMsg);
		break;

	case ERR_COMM_RS232_BASE:
		sprintf((char *)pstCommErrMsg->szChnMsg, "串口错误:%04X", iReErrCode);
		sprintf((char *)pstCommErrMsg->szEngMsg, "PORT ERR:%04X", iReErrCode);
		GetAllErrMsg(iErrCode, sg_stRS232ErrMsg, pstCommErrMsg);
		break;

	case ERR_COMM_MODEM_BASE:
		sprintf((char *)pstCommErrMsg->szChnMsg, "拨号错误:%04X", iReErrCode);
		sprintf((char *)pstCommErrMsg->szEngMsg, "MODEM ERR:%04X", iReErrCode);
		GetAllErrMsg(iErrCode, sg_stModemErrMsg, pstCommErrMsg);
	    break;

	case ERR_COMM_TCPIP_BASE:
		sprintf((char *)pstCommErrMsg->szChnMsg, "TCPIP错误:%04X", iReErrCode);
		sprintf((char *)pstCommErrMsg->szEngMsg, "TCPIP ERR:%04X", iReErrCode);
		GetAllErrMsg(iErrCode, sg_stPPPErrMsg, pstCommErrMsg);
		break;
	case ERR_COMM_WIRELESS_BASE:
		sprintf((char *)pstCommErrMsg->szChnMsg, "无线错误:%04X", iReErrCode);
		sprintf((char *)pstCommErrMsg->szEngMsg, "WIRE ERR:%04X", iReErrCode);
		GetAllErrMsg(iErrCode, sg_stPPPErrMsg, pstCommErrMsg);
		break;
	default:
		sprintf((char *)pstCommErrMsg->szChnMsg, "通讯错误");
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
// RS232 通讯模块
//////////////////////////////////////////////////////////////////////////
// 串口打开
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

// 串口发送数据
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

// 串口直接发送
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

// 串口接收
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

// 串口直接接收
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
			if( (iNewTimeMs-iOldTimeMs) >= (uint32_t)(uiTimeOut *1000) )	// 检查定时器
			{
				if( uiReadCnt>0 )	// 已经读取到数据
				{
					break;
				}

				return ERR_COMM_TIMEOUT;
			}

//			lcdDisplay(87,4,DISP_CFONT|DISP_CLRLINE,"(%d)",(iNewTimeMs-iOldTimeMs)/1000);
			lcdDisplay(0,4,DISP_CFONT|DISP_CLRLINE,"处理中...(%d)",(iNewTimeMs-iOldTimeMs)/1000);
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
		{	// 接收成功,继续
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
		{	// 非超时错误,退出
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
			if( (iNewTimeMs-iOldTimeMs) >= (uint32_t)(uiTimeOut *1000) )	// 检查定时器
			{
				if( uiReadCnt>0 )	// 已经读取到数据
				{
					break;
				}
				
				return ERR_COMM_TIMEOUT;
			}
			
//			lcdDisplay(87,4,DISP_CFONT,"(%d)",(iNewTimeMs-iOldTimeMs)/1000);
			lcdDisplay(0,4,DISP_CFONT|DISP_CLRLINE,"处理中...(%d)",(iNewTimeMs-iOldTimeMs)/1000);
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


// 串口关闭
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
// TCP 通讯模块
//////////////////////////////////////////////////////////////////////////
// 建立TCP连接
int TcpDial(uint8_t ucDialMode)
{
	int iRet,iFlag,iRetyTimes;
	uint32_t   uiOldTime;

	if( ucDialMode==PREDIAL_MODE )
	{
		return 0;
	}

    //  实际连接
	iFlag = 1;
	iRetyTimes = 1;
	uiOldTime = sysGetTimerCount();	
	while (iFlag)
	{
		if ( sysGetTimerCount() >= (uiOldTime+60000) )
		{ // 拨号连接超时60s，则返回
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
		{// 三次拨号连接不成功则关闭套节字，断开PPP连接
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
			lcdDisplay(0,3,DISP_CFONT,"   连 接 中 ... ");
			lcdDisplay(0,5,DISP_CFONT,"TCPIP CONNECT(%d)",iRetyTimes++);
			DrawRect(0, 17, 127, 63);

			iRet = NetConnect(sg_Tcpsocket,&sg_stTcpsockaddr,sizeof(struct sockaddr));
//lcdClrLine(2, 7);
//lcdFlip();
//lcdDisplay(0, 2, DISP_CFONT, "iRet=%d", iRet);
//kbGetKey();		
			if (iRet != NET_OK)
			{
				CommOnHook(FALSE);  // 挂断TCPIP重新连接
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

// 发送数据
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

// 接收数据
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
		if( (iNowTime-iOldTime) >= (uint32_t)(uiTimeOut *1000) )	// 检查定时器
		{
			iRet = ERR_COMM_TIMEOUT;
			TcpOnHook(TRUE);
			break;
		}

//		lcdDisplay(87,4,DISP_CFONT|DISP_CLRLINE,"(%d)",(iNowTime-iOldTime)/1000);		
		lcdDisplay(0,4,DISP_CFONT|DISP_CLRLINE,"处理中...(%d)",(iNowTime-iOldTime)/1000);
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

// 断开TCP连接
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
// MODEM 通讯模块
//////////////////////////////////////////////////////////////////////////
#define MODEM_COMM_ERR(a)		(ERR_COMM_MODEM_BASE|(a))
// modem 拨号
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
			{ // 拨号连接超时60s，则返回
				PSTNOnHook(TRUE);
				return MODEM_COMM_ERR(NET_ERR_RETRY);
			}

			if (iDialTimes>3)  // 三次不成功则退出失败
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
				if (sg_iModemfd >= 0 )  // 如果modem句柄无效，则先打开
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

			case MODEM_STATE_SYNC_MODE:  // 该状态表示正在设置,需要等待设置成功
			case MODEM_STATE_ASYN_MODE:  // 该状态表示正在设置,需要等待设置成功
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

			case MODEM_STATE_WAITTING:  // 被拨号状态，先挂断当前被拨连接，再重新拨号
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
		{ // 拨号连接超时60s，则返回
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

		if (iDialTimes >= 3)  // 三次不成功则退出失败
		{
			modem_get_last_errno(sg_iModemfd,&iLastErr);
			iLastErr = ABS(iLastErr);
			return MODEM_COMM_ERR(iLastErr);
		}

		lcdClrLine(2, 7);
		lcdFlip();	
		lcdDisplay(0, 3, DISP_CFONT, "    拨 号 中   ");
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

		case MODEM_STATE_SYNC_MODE:  // 该状态表示正在设置,需要等待设置成功
		case MODEM_STATE_ASYN_MODE:  // 该状态表示正在设置,需要等待设置成功
		case MODEM_STATE_DAILING:
		case MODEM_STATE_CONNECT_SDLC:
			sysDelayMs(100); // 延时后在查看
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
			
		case MODEM_STATE_WAITTING:  // 被拨号状态，先挂断当前被拨连接，再重新拨号
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

// Modem 发送
int PSTNTxd(uint8_t *psTxdData, uint32_t uiDataLen, uint32_t uiTimeOut)
{
	int  iRet,iRelSendlen;
		
	for (iRelSendlen=0; iRelSendlen<uiDataLen;)
	{
		iRet = modem_write_timeout(sg_iModemfd,psTxdData+iRelSendlen,(uiDataLen-iRelSendlen),uiTimeOut);
		if (iRet < 0)
		{
			PSTNOnHook(FALSE);   // 发送失败后断开连接，下次重新连接
			iRet = ABS(iRet);
			return MODEM_COMM_ERR(iRet);
		}
		iRelSendlen += iRet;
	}

	return 0;
}

// Modem接收
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


// Modem同步接收
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
		if( (iNowTime-iOldTime) >= (uint32_t)(uiTimeOut *1000) )	// 检查定时器
		{
			iRet = ERR_COMM_TIMEOUT;
			break;
		}
		
//		lcdDisplay(87,4,DISP_CFONT|DISP_CLRLINE,"(%d)",(iNowTime-iOldTime)/1000);
		lcdDisplay(0,4,DISP_CFONT|DISP_CLRLINE,"处理中...(%d)",(iNowTime-iOldTime)/1000);
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

// Modem异步接收
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
		if( (iNowTime-iOldTime) >= (uint32_t)(uiTimeOut *1000) )	// 检查定时器
		{
			iRet = ERR_COMM_TIMEOUT;
			break;
		}
		
//		lcdDisplay(87,4,DISP_CFONT|DISP_CLRLINE,"(%d)",(iNowTime-iOldTime)/1000);
		lcdDisplay(0,4,DISP_CFONT|DISP_CLRLINE,"处理中...(%d)",(iNowTime-iOldTime)/1000);
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

// Modem 挂机
int PSTNOnHook(uint8_t bReleaseAll)
{
	int  iRet,iStatus;

	modem_wait_until_sent(sg_iModemfd);

	// 排除连续多次挂断的请求
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
			
			if (iStatus & 0x80000000) // 目前最高位为1表示操作还没完成，需要再次检测
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
		{ // 超时10s，则返回
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
		
		if (iStatus & 0x80000000) // 目前最高位为1表示操作还没完成，需要再次检测
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
		if (sg_iModemfd < 0 )  // 如果modem句柄无效，则先打开
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
// GPRS/CDMA 通讯模块
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

	iCnt = 2;  // 2次检查SIM卡，中延时2S，间防止SIM卡上电时间不够的问题
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
	// 预拨号
	if( ucDialFlag== PREDIAL_MODE )  
	{
		sg_Wire_PreDial_Ret = 0;
		iRet = PPPCheck(OPT_DEVWNET);
		if (iRet < 0)
		{
			if (iRet == -ETIMEDOUT)
			{   // 超时退出则先复位模块
				WirelessReset(&sg_stCurCfg.stWirlessPara);
			}
			while((UINT32)(sysGetTimerCount() - sg_uiWirePPPLogoutTime) < 3000); // 挂断后必须隔3秒以上才能再拨号
			iRet = PPPLogin(OPT_DEVWNET,(char *)sg_stCurCfg.stWirlessPara.szAPN,
				            (char *)sg_stCurCfg.stWirlessPara.szUID,
							(char *)sg_stCurCfg.stWirlessPara.szPwd,0,0);
			sg_ucWirePPPFlag = 1;	// 每次做过了PPP拨号的动作，不管最后成功与否，都可以做一次挂断ppp的动作			
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
		lcdDisplay(0, 3, DISP_CFONT, "    拨 号 中   ");
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
			这段代码移位到这的原因是：这个超时本意是针对ppp链接建立的超时时间，那么，只有当pppcheck返回非0时，才能去判断是否超时。否则，将
			可能出现这样的情况，最后一次pppcheck已经成功了，尝试了一次tcp连接，但是失败了，但是在循环后，却因为这个超时判断而退出了函数。
			*****/
			if ( sysGetTimerCount() >= (uiOldTime+60000) )
			{ // 拨号连接超时60s，则返回
				WirelessClose(TRUE);
				return WIRELESS_ERR(NET_ERR_RETRY);
			}
			
			/****
			这段代码移进来的原因是：由于超时判断已经移进来了，所以为了发挥超时判断的左右，必须把这段代码也移进来。
			****/
			if (iRet == -NET_ERR_LINKOPENING)
			{
				sysDelayMs(400);
				iDialTimes--;  // 检查拨号结果不需要加拨号次数
				continue;
			}
		
			CommOnHook(FALSE);// 先挂断链接
			if (iRet == -ETIMEDOUT)
			{   // 超时退出则先复位模块
				WirelessReset(&sg_stCurCfg.stWirlessPara);
			}
			while((UINT32)(sysGetTimerCount() - sg_uiWirePPPLogoutTime) < 3000); // 挂断后必须隔3秒以上才能再拨号
			iRet = PPPLogin(OPT_DEVWNET,(char *)sg_stCurCfg.stWirlessPara.szAPN,
				            (char *)sg_stCurCfg.stWirlessPara.szUID,
							(char *)sg_stCurCfg.stWirlessPara.szPwd,0,20000);
			sg_ucWirePPPFlag = 1; // 每次做过了PPP拨号的动作，不管最后成功与否，都可以做一次挂断ppp的动作		
			if (iRet<0)
			{
				iRet = -iRet;
			}
			sg_Wire_PreDial_Ret = iRet;
			sysDelayMs(100);
			continue;
		}

		// 由于目前使用的套节字最多是0,1,2 . 所以如果sg_Wiresocket值异常，先将其赋初值(挂断)
		if(sg_Wiresocket<0 || sg_Wiresocket>3)  
			CommOnHook(FALSE);
			
		if (sg_Wiresocket == -1)
		{
			//  实际连接
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
			lcdDisplay(0,3,DISP_CFONT,"   连 接 中 ... ");
			lcdDisplay(0,5,DISP_CFONT,"TCPIP CONNECT(%d)",iRetyTimes++); 
			DrawRect(0, 17, 127, 63);

			iRet = NetConnect(sg_Wiresocket,&sg_stWiresockaddr,sizeof(struct sockaddr));
			/***
			这段代码这么修改的原因是：由于tcp连接尝试次数是否超限的判断已经放在后面，所以，这里必须如此修改。
			当tcp连接失败，自然地执行后面的次数判断代码。
			***/						
			if (NET_OK == iRet)
			{
				return iRet;
			}

			/***
			增加这段代码的原因是: 当返回-1043时，说明用户按取消键取消TCP连接，那么应该退出通讯。
			***/
			if (-NET_ERR_USER_CANCEL == iRet)
			{
				return ERR_USERCANCEL;
			}	
		}
		else
		{ 
			// 目前还不能检测该套节字是否还处在连接，可用中，只有应用自己严格处????????
			return 0;
		}
		
		/***
		这段代码移到后面的原因是：既然最后一次尝试tcp连接都不成功了，根本不需要循环到开头去，可直接退出函数。
		而且，这段代码在前面的一个危害是：如果最后一次尝试tcp连接失败，循环回头后，做pppcheck检查，如果pppcheck返回非0，那么将会再去做ppplogin，耗时耗力直到
		ppp链接建立起来了，却在这段代码退出了函数，白做了无用功，还耗时间。
		***/
		if (iRetyTimes > 3 )
		{// 三次拨号连接不成功则关闭套节字，断开PPP连接
			WirelessClose(TRUE);
			return WIRELESS_ERR(NET_ERR_RETRY);
		}
		else /***这段代码是前面tcp连接失败后的处理，移到这里来了***/
		{
			CommOnHook(FALSE);  // 挂断TCPIP重新连接
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
	// 预拨号
	if( ucDialFlag== PREDIAL_MODE )  
	{
		sg_Wire_PreDial_Ret = 0;
		iRet = PPPCheck(OPT_DEVWNET);
		if (iRet < 0)
		{
			if (iRet == -ETIMEDOUT)
			{   // 超时退出则先复位模块
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
		{ // 拨号连接超时60s，则返回
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
		lcdDisplay(0, 3, DISP_CFONT, "    拨 号 中   ");
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
			iDialTimes--;  // 检查拨号结果不需要加拨号次数
			continue;
		}
		if (iRet < 0)
		{
			CommOnHook(FALSE);// 先挂断链接
			if (iRet == -ETIMEDOUT)
			{   // 超时退出则先复位模块
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
		{// 三次拨号连接不成功则关闭套节字，断开PPP连接
			WirelessClose(TRUE);
			return WIRELESS_ERR(NET_ERR_RETRY);
		}

		// 由于目前使用的套节字最多是0,1,2 . 所以如果sg_Wiresocket值异常，先将其赋初值(挂断)
		if(sg_Wiresocket<0 || sg_Wiresocket>3)  
			CommOnHook(FALSE);
			
		if (sg_Wiresocket == -1)
		{
			//  实际连接
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
			lcdDisplay(0,3,DISP_CFONT,"   连 接 中 ... ");
			lcdDisplay(0,5,DISP_CFONT,"TCPIP CONNECT(%d)",iRetyTimes++); 
			DrawRect(0, 17, 127, 63);

			iRet = NetConnect(sg_Wiresocket,&sg_stWiresockaddr,sizeof(struct sockaddr));
			if (iRet != NET_OK)
			{
				CommOnHook(FALSE);  // 挂断TCPIP重新连接
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
			// 目前还不能检测该套节字是否还处在连接，可用中，只有应用自己严格处????????
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
	sysDelayMs(5000); // 复位后，延时5s
	
	return InitWirelessModule(pstParam);
}


//无线通讯接收
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
		if( (iNowTime-iOldTime) >= (uint32_t)(uiTimeOut *1000) )	// 检查定时器
		{
			iRet = ERR_COMM_TIMEOUT;
			break;
		}
		
//		lcdDisplay(87,4,DISP_CFONT|DISP_CLRLINE,"(%d)",(iNowTime-iOldTime)/1000);
		lcdDisplay(0,4,DISP_CFONT|DISP_CLRLINE,"处理中...(%d)",(iNowTime-iOldTime)/1000);
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

// 无线通讯发送
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
			WirelessClose(TRUE);   // 发送失败后断开连接，下次重新连接
			return WIRELESS_ERR(iRet);
		}
		iRelSendlen += iRet;
	}

	return 0;
}

// 无线模块挂机
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
			sysDelayMs(5000); // 复位后，延时5s
			
			iRet = InitWirelessModule(&stPosParam.stTxnCommCfg.stWirlessPara); //拨号参数，交易和tms的是一致的
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

// 显示信号强度
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
// WIFI 通讯模块
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
        // 先检测连接状态
		while(1)
		{
			iRetry++;
			lcdClrLine(2, 7);
			lcdFlip();	
			lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"请稍候...");
			lcdDisplay(0,5,DISP_CFONT|DISP_MEDIACY|DISP_CLRLINE,"WIFI连接中..(%d)",iRetry);
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
				PubDispString("获取当前连接失败",2|DISP_LINE_CENTER);
				PubDispString("按功能或菜单键",4|DISP_LINE_CENTER);
				PubDispString("设置WIFI配置参数",6|DISP_LINE_CENTER);
				iRet = PubWaitKey(6);
				if (iRet == KEY_FN || iRet == KEY_MENU)
				{
					iRet = SelectAndSetWIFIAP();
					if (iRetry>=3)
					{
						return  WIFI_COMM_ERR(iTemp);
					}
					sysDelayMs(1000);// 延时1s
					continue;
				}
				else
				{
					return  WIFI_COMM_ERR(iTemp);
				}
			}
			break;
		}

		// 检测是否获取本地IP地址
		iRetry = 0;
		while(1)
		{
			iRetry++;
			lcdClrLine(2,7);
			lcdFlip();	
			lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"请稍候...");
			lcdDisplay(0,5,DISP_CFONT|DISP_MEDIACY|DISP_CLRLINE,"获取本地IP..(%d)",iRetry);
			DrawRect(0, 17, 127, 63);
			memset(szlocalIP, 0, sizeof(szlocalIP));
			iRet = WifiGetLocalIP(szlocalIP);
			if(iRet != NET_OK)
			{
				iTemp = iRet;
				lcdClrLine(2,7);
				lcdFlip();	
				PubDispString("获取当前IP失败",2|DISP_LINE_CENTER);
				PubDispString("按功能或菜单键",4|DISP_LINE_CENTER);
				PubDispString("设置WIFI配置参数",6|DISP_LINE_CENTER);
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
        // 先检测连接状态
		while(1)
		{
			iRetry++;
			lcdClrLine(2,7);
			lcdFlip();	
			lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"请稍候...");
			lcdDisplay(0,5,DISP_CFONT|DISP_MEDIACY|DISP_CLRLINE,"WIFI连接中..(%d)",iRetry);
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
				sysDelayMs(2000);// 延时1s
				continue;	
			}

			break;
		}

		// 检测是否获取本地IP地址
		iRetry = 0;
		while(1)
		{
			iRetry++;
			lcdClrLine(2,7);
			lcdFlip();	
			lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"请稍候...");
			lcdDisplay(0,5,DISP_CFONT|DISP_MEDIACY|DISP_CLRLINE,"获取本地IP..(%d)",iRetry);
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
	
    //  实际连接
	iFlag = 1;
	iRetyTimes = 1;
	uiOldTime = sysGetTimerCount();	
	while (iFlag)
	{
		if ( sysGetTimerCount() >= (uiOldTime+60000) )
		{ // 拨号连接超时60s，则返回
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
		{// 三次拨号连接不成功则关闭套节字，断开PPP连接
			TcpOnHook(FALSE);
			return (ERR_COMM_TCPIP_BASE|NET_ERR_RETRY);
		}

		// 检测是否连接上WIFI,这个时候如果没有连接成功就不提示用户进行WIFI设置而是直接返回
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
    		lcdDisplay(0,3,DISP_CFONT,"   连 接 中 ... ");
		    lcdDisplay(0,5,DISP_CFONT,"TCPIP CONNECT(%d)",iRetyTimes++);
			DrawRect(0, 17, 127, 63);
			
			iRet = NetConnect(sg_Tcpsocket,&sg_stTcpsockaddr,sizeof(struct sockaddr));
			if (iRet != NET_OK)
			{
				CommOnHook(FALSE);  // 挂断TCPIP重新连接
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

// 计算LRC
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
