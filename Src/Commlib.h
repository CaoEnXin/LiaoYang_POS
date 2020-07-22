
/****************************************************************************
NAME
	commlib.h - ʵ�ֹ���ͨѶ����

DESCRIPTION
	1. ʵ��POSӦ�ÿ����г���ͨѶ��ʽ�����ӵĽ������Ͽ��������շ��ȹ���.
	2. ��ģ���ڲ�ռ�ö�ʱ��4#(����ģ�������,�˳�ģ����ͷ�)

REFERENCE

MODIFICATION SHEET:
	MODIFIED   (YYYY.MM.DD)
	pyming     2008.09.05      - created
****************************************************************************/

#ifndef _COMMLIB_H
#define _COMMLIB_H

#include "posapi.h"
#include "modem_iface.h"
#include "modem.h"

#define MASK_COMM_TYPE			0xF000		// ��ȡ�������
#define MASK_ERR_CODE			0x0FFF		// ��ȡ�������

#define ERR_COMM_RS232_BASE		0x1000		// RS232�����
#define ERR_COMM_MODEM_BASE		0x2000		// Modem�����
#define ERR_COMM_TCPIP_BASE		0x4000		// TCPIP�����
#define ERR_COMM_WIRELESS_BASE	0x8000		// GPRS/CDMA�����
#define ERR_COMM_WIFI_BASE      0x3000		// RS232�����

// �������ʹ���
#define ERR_COMM_ALL_BASE		0x0000		// �������ʹ���
#define ERR_COMM_INV_PARAM		0x0001		// ��������
#define ERR_COMM_INV_TYPE		0x0002		// ��ЧͨѶ����
#define ERR_COMM_CANCEL			0x0003		// �û�ȡ��
#define ERR_COMM_TIMEOUT		0x0004		// ͨѶ��ʱ
#define ERR_COMM_COMERR			0x0005
#define	ERR_COMM_TOOBIG			0x0006
#define ERR_COMM_STATUS_TIMEOUT 0x0007      // ��ȡ״̬��ʱ
#define ERR_COMM_MODEM_INIT     0x0008      // ��ʼ��ʧ��
#define ERR_COMM_MODEM_NOPARAM  0x0009      // ���ò���ʧ��

// ���ŷ�ʽ
#define PREDIAL_MODE		0		// Ԥ����/Ԥ����
#define ACTDIAL_MODE			1		// ʵ�ʲ���/����

// #define _WIRELESS_PRE_DIAL		// open for pre-connect of wireless module

// �������ݷ�ʽ
#define CM_RAW				0		// ԭ���ݷ��ͷ�ʽ(�������ֹ�ַ�,For RS232)
#define CM_SYNC				1		// ͬ��
#define CM_ASYNC			2		// �첽

// for RS232 communication
#define STX             0x02
#define ETX             0x03
#define ENQ             0x05
#define ACK             0x06
#define NAK             0x15

// PSTN���ò���
typedef struct _tagST_PSTN_PARA
{
	uint8_t		        ucSendMode;		     // ���ݷ��ͷ�ʽ
	uint8_t             szTxnTelNo[100+1];   // ���ź���
	ModemDialParms_t    stDialPara;          // modem���Ų���
}ST_PSTN_PARA; 

// RS232���ò���
typedef struct _tagRS232_INFO
{
	uint8_t	ucPortNo;			// Port #, COM1, COM2 ....
	uint8_t	ucSendMode;			// ���ݷ��ͷ�ʽ
	uint8_t	szAttr[20+1];		// ���ڴ򿪲���, "9600,8,n,1", ....
}ST_RS232_INFO;

// TCP/IP���ò���
typedef struct _tagST_TCPIP_INFO
{
	uint8_t	szNetMask[15+1];
	uint8_t	szGatewayIP[15+1];
	uint8_t	szLocalIP[15+1];
	uint8_t	szRemoteIP[15+1];
	uint8_t	szRemotePort[5+1];
	uint8_t	szDNS[32+1];
}ST_TCPIP_INFO;


// GPRS/CDMA configurations
typedef struct _tagST_WIRELESS_INFO
{
	uint8_t		szAPN[64+1];  ///���ź��� CDMA: #777; GPRS: cmnet
	uint8_t		szUID[64+1];
	uint8_t		szPwd[16+1];
	uint8_t		szSimPin[16+1];     // SIM card PIN
	uint8_t		szDNS[32+1];
	uint8_t		szRemoteIP[20+1];
	uint8_t		szRemotePort[5+1];
}ST_WIRELESS_INFO;

// WIFI  AP ���ò���
typedef struct _tagST_WIFI_AP_INFO
{
	int         iChannel;                  //  Wireless LAN communication channel(0 - 13)����ͨѶ�ŵ�
	int         iSecurityType;             //  Sets the Wireless LAN security type. ���� ����ģʽ
	int         iWEPKeyIdx;                //  WEP key index (1 - 4) -- WEP��Կ��
    char        szListWEPKey[4][32];       //  4 sets of WEP Key  --- ��Կ����Կ
	char        szWPAPSK[64];              //  Personal Shared Key Pass-Phrase,
	char        szSSID[64];                //  SSID
}ST_WIFI_AP_INFO;

// ͨѶģ�������Ϣ
typedef struct _tagCOMM_ERR_MSG
{
	uint8_t		szChnMsg[16+1];		// ���Ĵ�����Ϣ
	uint8_t		szEngMsg[16+1];		// Ӣ�Ĵ�����Ϣ
}COMM_ERR_MSG;


#define CT_RS232	0x01			// RS232ͨѶ
#define CT_MODEM	0x02			// ModemͨѶ
#define CT_TCPIP	0x04			// TCP/IPͨѶ
#define	CT_CDMA		0x08			// CDMAͨѶ
#define CT_GPRS		0x10			// GPRSͨѶ
#define CT_WIFI     0x20            // WIFIͨѶ
#define CT_ALLCOMM  0x3F            // ���е�ͨѶ����

// ͨѶģ��������Ϣ
typedef struct _tagST_COMM_CONFIG
{
	uint8_t			  ucCommType;		    // ͨѶ����(RS232/modem/tcp...)    
	ST_RS232_INFO	  stRS232Para;	        // RS232 ���ò���
	ST_TCPIP_INFO	  stTcpIpPara;	        // TCP ���ò���
	ST_WIRELESS_INFO  stWirlessPara;        // GRPS/CDMA����ͨѶ���ò���
	ST_PSTN_PARA	  stPSTNPara;		    // modem ���ò���
	ST_TCPIP_INFO	  stWIFIIpPara;	        // WIFI ͨѶģʽ���ò���
}ST_COMM_CONFIG;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/****************************************************************************
 Function:		ͨѶģ���ʼ��(ֻ����������ģ��)
 Param In:
	pstComCfg	������Ϣ
 Param Out:		none
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommInitModule(ST_COMM_CONFIG *pstCfg);

/****************************************************************************
 Function:		����ͨѶģ�����
 Param In:
	pstComCfg	������Ϣ
 Param Out:		none
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommSetCfgParam(ST_COMM_CONFIG *pstCfg);

/****************************************************************************
 Function:		ͨѶģ�鲦��(MODEM���Ż���TCP����TCP���ӵ�)
 Param In:
	ucDialMode	���ű�־,PREDIAL_MODE:���ź����̷���, ACTDIAL_MODE: ��·�����󷵻�
 Param Out:		none
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommDial(uint8_t ucDialMode);

/****************************************************************************
 Function:		ͨѶģ�鷢������
 Param In:
	psTxdData	�����͵�����
	uiDataLen	���������ݵ�ʵ�ʳ���(���ֽ�Ϊ��λ)
	ulTimeOut	���ͳ�ʱ,��100�����(RFU)
 Param Out:		none
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommTxd(uint8_t *psTxdData, uint32_t uiDataLen, uint32_t uiTimeOut);

/****************************************************************************
 Function:		ͨѶģ���������
 Param In:
	uiExpLen	�������յ�������ݳ���(���ֽ�Ϊ��λ)
	ulTimeOut	�ȴ��������ݵĳ�ʱʱ��,��100�����
 Param Out:
	psRxdData	���յ�������
	puiOutLen	���յ���ʵ�����ݳ���
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommRxd(uint8_t *psRxdData, uint32_t uiExpLen, uint32_t uiTimeOut, uint32_t *puiOutLen);

/****************************************************************************
 Function:		ͨѶģ��Ͽ���·(MODEM�һ�����TCP�Ͽ�TCP���ӵ�)
 Param In:
	bReleaseAll	�Ƿ���ȫ�Ͽ���·������, TRUE: �Ͽ�, FALSE: �Ͽ�Ӧ��������·
				(RFU for ����ģ��)
 Param Out:		none
 Return Code:
	0			OK
	other		fail
****************************************************************************/
int CommOnHook(uint8_t bReleaseAll);

/****************************************************************************
 Function:			��ȡͨѶ������Ϣ
 Param In:
	iErrCode		����ͨѶ�������صĴ������
	pstCommErrMsg	������Ϣ(������Ӣ��)
 Param Out:			none
 Return Code:		none
****************************************************************************/
void CommGetErrMsg(int iErrCode, COMM_ERR_MSG *pstCommErrMsg);

//  ��ȡ��ǰ����ʹ�õ��׽���
int  GetCurSocket(void);


/****************************************************************************
 Function:			��ʼ��WIFIģ�飬����AP����ȡ����IP��ַ
 Param In:
 Param Out:			none
 Return Code:		0---��ʼ���ɹ�;  ����--��ʧ��
****************************************************************************/
int   InitWiFiModule(uint8_t bInitModule);

/****************************************************************************
 Function:			��ȡWIFIģ���Ƿ�����AP����ȡ����IP��ַ���ú���������
                    InitWiFiModule�������ƣ�ֻ�Ǹú���ֻ�ǻ�ȡ״̬���ڲ��ɹ�ʱ
					������ʾ�û��޸�WIFI�����ò�����
 Param In:
 Param Out:			none
 Return Code:		0---��ʼ���ɹ�;  ����--��ʧ��
****************************************************************************/
int   GetWiFiStatus(void);


void DispCommErrMsg(int iErrCode);

int     CommResetModule(ST_COMM_CONFIG *pstCfg);
int   WirelessClose(uint8_t bRelease);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _CommLIB_H

// end of file
