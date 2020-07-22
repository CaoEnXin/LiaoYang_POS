#ifndef __SZCICDLL_H__
#define __SZCICDLL_H__

//#define _IPOS_SYSTEM_

#include <stdio.h>
#include <string.h>
#ifdef _IPOS_SYSTEM_
#include "util.h"
#include "ctrl2lib.h"
#include "define.h"
#include "system.h"
#include "Sam.h"

#define LCP_ETCARDLL      131
#endif

typedef unsigned char  uint8;                   /* defined for unsigned 8-bits integer variable 	�޷���8λ���ͱ���  */
typedef signed   char  int8;                    /* defined for signed 8-bits integer variable		�з���8λ���ͱ���  */
typedef unsigned short uint16;                  /* defined for unsigned 16-bits integer variable 	�޷���16λ���ͱ��� */
typedef signed   short int16;                   /* defined for signed 16-bits integer variable 		�з���16λ���ͱ��� */
typedef unsigned int   uint32;                  /* defined for unsigned 32-bits integer variable 	�޷���32λ���ͱ��� */
typedef signed   int   int32;                   /* defined for signed 32-bits integer variable 		�з���32λ���ͱ��� */
typedef float          fp32;                    /* single precision floating point variable (32bits) �����ȸ�������32λ���ȣ� */
typedef double         fp64;                    /* double precision floating point variable (64bits) ˫���ȸ�������64λ���ȣ� */

typedef unsigned char   BOOLEAN;
//typedef unsigned char   BYTE;
//typedef unsigned short  WORD;
//typedef unsigned int    DWORD;          /* 32 bit data type */
//typedef long long       DDWORD;         /* 64 bit data type */
/*
union BYTEtoWORD
{
	WORD	WData;
	BYTE	BData[2];
};

union BYTEtoDWORD
{
	DWORD	DData;
	BYTE	BData[4];
};
*/
#ifndef BYTE
#define BYTE unsigned char
#endif

#define SUCCESS								0x00			/** �����ɹ�					*/

#define ERR_CODE_SAM_SOCKET					0xD4//0x10			/** ����SAM������ʧ��			*/
#define ERR_CODE_SAM_RESET					0xD5//0x11			/** ��λSAM��ʧ��				*/
#define ERR_CODE_SAM_READ_TERMNUM			0xD6//0x12			/** ��ȡ�ն˻����ʧ��			*/
#define ERR_CODE_SAM_READ_PUBINFO			0xD7//0x13			/** ��ȡ��Ƭ������Ϣ�ļ�ʧ��	*/
#define ERR_CODE_SAM_SELECT_ADF				0xD8//0x14			/** SAMѡ��Ӧ��ʧ��				*/
#define ERR_CODE_SAM_READ_APPPUBINFO		0xD9//0x15			/** SAM��ȡӦ�ù�����Ϣ�ļ�ʧ��	*/
#define ERR_CODE_CALC_MAC1					0xDA//0x16			/** SAM����MAC1ʧ��				*/
#define ERR_CODE_SAM_CHKOUT_MAC2			0xDB//0x17			/** SAM��֤MAC2ʧ��				*/
#define ERR_CODE_USER_CANCEL				0xDC//0x0F			/** �û�ȡ��					*/
#define ERR_CODE_USERCARD_VAL_INVAL         0xDE                /** �Ǵ�ֵ��                    */
#define ERR_CODE_USERCARD_BAL_INVAL         0xDF                /** ��ֵ������10000         */

#define ERR_CODE_MIFARE_ANTICOLL			0xE0			/** mifare anticollʧ��			*/
#define ERR_CODE_MIFARE_SELECT				0xE1			/** mifare selectʧ��			*/

#define ERR_CODE_USECARD_SOCKET				0xE2			/** ����û���Ƭʧ��			*/
#define ERR_CODE_USECARD_RESET				0xE3			/** �û�����λʧ��				*/
#define ERR_CODE_USECARD_READ_BASICINFO		0xE4			/** ��ȡ�û�����������ʧ��		*/
#define ERR_CODE_USECARD_SELECT_ADF			0xE5			/** ѡ���û���ADF1001ʧ��		*/
#define ERR_CODE_USECARD_READ_APPINFO		0xE6			/** ��ȡ�û���Ӧ������ʧ��		*/
#define ERR_CODE_USECARD_INVALID			0xE7			/** ��Ƭ������Ч(���Ѿ�����)	*/
#define ERR_CODE_READ_BALANCE				0xE8			/** ��ȡ�û������ʧ��			*/
#define ERR_CODE_INIT_PURCHASE				0xE9			/** ���ѳ�ʼ��ʧ��				*/
#define ERR_CODE_LACK_OF_BALANCE			0xEA			/** ����					*/
#define ERR_CODE_CHKOUT_MAC1				0xEB			/** �û���У��MAC1ʧ��			*/
#define ERR_CODE_DEBIT_PURCHASE				0xEC			/** �ۿ�ʧ��					*/

#define ERR_CODE_VERIFY_PIN					0xED			/** У��PINʧ��					*/
#define ERR_CODE_INIT_LOAD					0xEE			/** ��ֵ��ʼ������				*/

#define ERR_CODE_ADSAM_SOCKET				0xC0			/** ��������SAM������ʧ��		*/
#define ERR_CODE_ADSAM_RESET				0xC1			/** ������λSAM��ʧ��			*/
#define ERR_CODE_ADSAM_SELECT_ADF_1			0xC3			/** ����SAMѡ��Ӧ��ʧ��		*/
#define ERR_CODE_ADSAM_SELECT_ADF_2			0xC4			/** ����SAMѡ��Ӧ��ʧ��		*/
#define ERR_CODE_ADSAM_READ_PSAMID			0xC5			/** ������ȡ����psamidʧ��	*/
#define ERR_CODE_ADSAM_MATCH				0xC6			/** ������psam����ƥ��		*/
#define ERR_CODE_ADSAM_GET_RANDOM			0xC7			/** ������ȡ�����ʧ��		*/ 
#define ERR_CODE_ADSAM_VERIFY_PIN			0xC8			/** ����У��PINʧ��			*/
#define ERR_CODE_INTERNAL_AUTH				0xC9			/** �����ڲ���֤ʧ��			*/
#define ERR_CODE_EXTERNAL_AUTH				0xCA			/** PSAM�ⲿ��֤ʧ��			*/
#define ERR_CODE_RESPONSE_CHALLENGE			0xCB			/** PSAM������սӦ����Ϣʧ��	*/
#define ERR_CODE_SAM_CALC_LINEMAC			0xCC			/** ������·����MAC��ʧ��		*/
#define ERR_CODE_DATA_LEN					0xCD			/** ���ݳ��ȴ���				*/
#define ERR_CODE_VERIFY_LINE_MAC			0xCE			/** ��֤��·����macʧ��			*/
#define ERR_CODE_RESPONCE_HEADTYPE			0xCF			/** ������Ӧ��Ϣ���ʹ���		*/
#define ERR_CODE_RESPONCE_DATA				0xD0			/** ������Ӧ��Ϣ���ݴ���		*/
#define	ERR_CODE_CREBIT_FOR_LOAD			0xD1			/** ��ֵʧ��					*/
#define ERR_CODE_VERIFY_MAC					0xDD//0xD2		/** ��·����MAC����				*/
#define ERR_CODE_CAL_ENCRY_PIN              0xD3            /** �������pinʧ��             */


#define ERR_CODE_RESP_ERR1					0x04//0x80			/** û�տ� ����ͨ����������� */
#define ERR_CODE_RESP_ERR2					0x14//0x81			/** ��Ч���� */
#define ERR_CODE_RESP_ERR3					0x25//0x82			/** �Ҳ���ԭ���� */
#define ERR_CODE_RESP_ERR4					0x30//0x83			/** ��ʽ���� */
#define ERR_CODE_RESP_ERR5					0x33//0x84			/** ���ڿ� */
#define ERR_CODE_RESP_ERR6					0x40//0x85			/** ����Ĺ��ܲ�֧�� */
#define ERR_CODE_RESP_ERR7					0x41//0x86			/** ��ʧ�� */
#define ERR_CODE_RESP_ERR8					0x43//0x87			/** ���Կ� */
#define ERR_CODE_RESP_ERR9					0x63//0x88			/** �ַ���ȫ */
#define ERR_CODE_RESP_ERR10					0x64//0x89			/** ԭʼ������ */
#define ERR_CODE_RESP_ERR11					0x76//0x8A			/** ��Ч�ʻ�/�����ʽ���� */ // �������޸�
#define ERR_CODE_RESP_ERR12					0x91//0x8B			/** ���Ĳ��ܲ��� δǩ�� */
#define ERR_CODE_RESP_ERR13					0x94//0x8C			/** �ظ����� */
#define ERR_CODE_RESP_ERR14					0xA0//0x8D			/** MAC ����ʧ�� */
#define ERR_CODE_RESP_ERR15					0xD2//0x8E			/** ���ڴ��� */
#define ERR_CODE_RESP_ERR16					0xB1//0x8F			/** ��Ƭ��֤ʧ�� */ ʵ��ΪQ1
#define ERR_CODE_RESP_ERR17					0xF0//0x90			/** �޴�PSAM�� */
#define ERR_CODE_RESP_ERR18					0xF1//0x91			/** PSAM��������� */
#define ERR_CODE_RESP_ERR19					0xF2//0x92			/** �޴��ն˺� */
#define ERR_CODE_RESP_ERR20					0xF3//0x93			/** PSAM��ʹ�ò��� */
#define ERR_CODE_RESP_ERR21					0xF4//0x94			/** psam��������״̬(����) */ // �������޸� ,δǩ��,1���ն�δǩ���ɹ���2���ն�ǩ����Ч�ڹ���
#define ERR_CODE_RESP_ERR22					0xF5//0x95			/** IC������MAC���� */
#define ERR_CODE_RESP_ERR23					0xFA//0x96			/** �Ǳ�ϵͳ�� */
#define ERR_CODE_RESP_ERR24					0xFB//0x97			/** �޿�ע���� */
#define ERR_CODE_RESP_ERR25					0x99//0x98			/** δ֪���� */
#define ERR_CODE_RESP_ERR26					0x68//0x99			/** ǩ����Ч�ڴ��� */
#define ERR_CODE_RESP_ERR27					0x66//0x9A			/** �Ѿ���ǩ����Ч�� */ // �������޸�
// �����ӵĴ������2010/1/16
#define ERR_CODE_RESP_ERR28                 0x65//0x9B            // ���׽�����,��������ʱ������̨Լ������
#define ERR_CODE_RESP_ERR29                 0x71//0x9C            // �ն˽����ܶ��ѳ�
#define ERR_CODE_RESP_ERR30                 0x72//0x9D            // ��Ƭ�����ܶ��ѳ�
#define ERR_CODE_RESP_ERR31                 0x73//0x9E            // ��Ƭ���׽�����
#define ERR_CODE_RESP_ERR32                 0x74//0x9F            // ��Ƭ���״����ѳ�
#define ERR_CODE_RESP_ERR33                 0x75//0xA0            // ��Ƭ�������
#define ERR_CODE_RESP_ERR34                 0x92//0xA1            // ������Ч�ڣ������ն�ǩ����Ч��
#define ERR_CODE_RESP_ERR35                 0x93//0xA2            // ��ʱ�����׳�ʱ

#define ERR_CODE_RESP_ERR36                 0x95//0xA3            // �����ѳ���
#define ERR_CODE_RESP_ERR37                 0x96//0xA4            // �����ѳ���
#define ERR_CODE_RESP_ERR38                 0xF6//0xA5            // PsamΪ��ʧ��
#define ERR_CODE_RESP_ERR39                 0xF7//0xA6            // PsamΪ�޿�ע��
#define ERR_CODE_RESP_ERR40                 0xFC//0xA7            // α��
#define ERR_CODE_RESP_ERR41                 0xA1//0xA8            // �ϴ�������ˮ������ϵͳ��¼������ˮ����
#define ERR_CODE_RESP_ERR42                 0xA2//0xA9            // ���ѽ�����ϵͳ��� 
#define ERR_CODE_RESP_ERR43                 0xA3//0xAA            // �������Ѿ����� 
#define ERR_CODE_RESP_ERR44                 0xA4//0xAB            // 57���������
#define ERR_CODE_RESP_ERR45                 0xA5//0xAC            // ���ܱ�������
#define ERR_CODE_RESP_ERR46                 0xA6//0xAD            // ���ܽ����� 

#define ERR_CODE_RESP_ERR47                 0x69//0xAE            // ��һ�ʲ��ǳ�ֵ
//#define ERR_CODE_RESP_ERR48                 0xAF            // ǩ����Ч�ڴ���
#define ERR_CODE_RESP_ERR49                 0xB0//0xB0            // �޴�PSAM��,ʵ��ΪX0
#define ERR_CODE_RESP_ERR50                 0x67//0xB1            // δǩ��
#define ERR_CODE_RESP_ERR51                 0xA7//0xB2            // ҵ����������

typedef struct dealrecord
{
	BYTE	RedType;				// ��¼���� bcd
	BYTE	DealType[2];			// �������� hex
	BYTE	AppSerilaNum[16];		// �û���Ӧ��ϵ�к�
	BYTE	POSNum[6];				// pos�����
	BYTE	TerminalNum[12];		// �ն˻����
	BYTE	TerminalDealNum[8];		// �ն˽������
	BYTE	Date[8];				// ��������
	BYTE	Time[6];				// ����ʱ��
	BYTE	Pre_Balance[8];			// ����ǰ��� 8
	BYTE	PAmount[8];				// ���׽��(ʵ�ս��) 4
	BYTE	BAmount[8];				// Ӧ�ս�� 4
	BYTE	PsamVer[2];				// PSAM���汾��
	BYTE	OfflineDealSerial[5];	// ��Ƭ�ѻ��������к�
	BYTE	Tac[8];					// ������֤tac
	BYTE	CityCode[4];			// ���д���
	BYTE	TradeCode[2];			// ��ҵ����
	BYTE	BranchCode[4];			// ���Ŵ���
	BYTE	Mac[8];					// MAC��
}dealrecord_t;
typedef dealrecord_t* pdealrecord_t;

typedef struct topup_cancel_record
{
	BYTE	AppSerilaNum[16];		// ��ͨ������16
	BYTE	Date[10];				// ��������
	BYTE	Time[8];				// ����ʱ��
	BYTE	type;					// ��������
	BYTE	Amount[8];				// ���׽��
	BYTE	RAmount[8];				// ���˽��
	BYTE	Pre_Balance[8];			// ����ǰ��Ƭ���
	BYTE	Balance[8];				// ���׺�Ƭ���
	BYTE    PosDSN[6];              // �ն���ˮ�� 2010/1/14 added by zj
	BYTE	Status;					// ״̬
	BYTE	rflag;					// ��¼��־
	BYTE	cflag;					// ���ʱ�־
}topup_cancel_record_t;
typedef topup_cancel_record_t* ptopup_cancel_record_t;

//////////////////////////////////////////////////////
// ��Ƭ������Ϣ�ļ�
typedef struct PsamPublicInfo
{
	BYTE PsamId[10];		// PSAM���к�
	BYTE PsamVer;			// PSAM�汾��
	BYTE EncrKeyType;		// ��Կ������
	BYTE FCIData[2];		// �������Զ���FCI����
}PsamPublicInfo_t;
typedef PsamPublicInfo_t* pPsamPublicInfo_t;

typedef struct AppPublicInfo
{
	BYTE ConsumeKeyidx;			// ������Կ������
	BYTE IssuerMarker[8];		// ��������ʶ��
	BYTE LinkChargeMarker[8];	// �����շ�������
	BYTE StartDate[4];			// Ӧ����������
	BYTE AvailData[4];			// Ӧ����Ч����
}AppPublicInfo_t;
typedef AppPublicInfo_t* pAppPublicInfo_t;

// PSAM���ݽṹ
typedef struct PsamStruct
{
	BYTE				TerminalNum[6];		// �ն˱��
	PsamPublicInfo_t	SamPubInfoStr;		// ��Ƭ������Ϣ
	AppPublicInfo_t		AppPubInfoStr;		// Ӧ�ù�����Ϣ�ļ�
}PsamStruct_t;
typedef PsamStruct_t* pPsamStruct_t;
//////////////////////////////////////////////////////////
// ��Ƭ�ṹ

// ����Ӧ�û��������ļ�
typedef struct CardPublicBasicInfo
{
	BYTE IssueMarker[8];			// ��������ʶ08
	BYTE AppType;					// �����ͱ�ʶ01
	BYTE AppVer;					// ��Ƭ�汾��01
	BYTE AppNetworkNum[2];			// ��Ƭ������(02)
	BYTE AppInsideNum[8];			// ��Ƭ�ڲ����(08)
	//BYTE AppSerilaNum[10];		// Ӧ�����к�
	BYTE AppStartDate[4];			// Ӧ����������
	BYTE AppAvailDate[4];			// Ӧ����Ч����
	BYTE FCIData[2];				// �������Զ���FCI����
}CardPublicBasicInfo_t;
typedef CardPublicBasicInfo_t* pCardPublicBasicInfo_t;

// �ֿ��˻��������ļ�
typedef struct CardholderBasicInfo
{
	BYTE FigureMarker;			// �ֿ�����ݱ�ʶ(01)
	BYTE PersonnelMarker;		// ��ϵͳְ����ʶ(01)
	BYTE Name[20];				// �ֿ�������
	BYTE PapersNum[16];			// �ֿ���֤������
	BYTE PapersType;			// �ֿ���֤������
}CardholderBasicInfo_t;
typedef CardholderBasicInfo_t* pCardholderBasicInfo_t;

// ��Ƭ��Ϣ
typedef struct CardInfo
{
	CardPublicBasicInfo_t	AppInfo;	// ����Ӧ�û��������ļ�
	CardholderBasicInfo_t	HolderInfo;	// �ֿ��˻��������ļ�
}CardInfo_t;
typedef CardInfo_t* pCardInfo_t;

// Init_for_Purchase�ṹ
typedef struct InitforPurchaseStr
{
	BYTE 				Balance[4];				// ���
	BYTE				OfflineDealSerial[2];	// Ǯ���ѻ��������
	BYTE				Overdraft[3];			// ͸֧�޶�
	BYTE 				EncryVer;				// ��Կ�汾��
	BYTE 				CalcMarker;				// �㷨��ʶ
	BYTE 				Random[4];				// α�����
}InitforPurchaseStr_t;
typedef InitforPurchaseStr_t* pInitforPurchaseStr_t;

// Init_for_Load�ṹ
typedef struct InitforLoadStr
{
	BYTE 				Balance[4];					// ���
	BYTE				OnlineDealSerial[2];		// Ǯ�������������
	BYTE 				EncryVer;					// ��Կ�汾��
	BYTE 				CalcMarker;					// �㷨��ʶ
	BYTE 				Random[4];					// α�����
	BYTE				Mac1[4];					// MAC1
}InitforLoadStr_t;
typedef InitforLoadStr_t* pInitforLoadStr_t;

// ��ֵ������Ӧ����
typedef struct AckTopUpStr
{
	BYTE				AppInsideNum[6];			// ��ֵ�����к�
	BYTE				Random[4];					// α�����
	BYTE				CalcMarker;					// �����㷨�汾
	BYTE				EncryVer;					// ������Կ�汾
	BYTE				OnlineDealSerial[2];		// ��ֵ����ǰ�����������к�,ÿ�����׷���ʱ������
//	BYTE                OnlineDealSerialofter[2];		// ��ֵ����ǰ�����������к�,ÿ�����׷���ʱ������
	BYTE				Value[4];					// ���׽��
	BYTE				Type;						// �������ͱ�ʶ 02������Ǯ����ֵ
	BYTE				TerminalNum[6];				// ��ֵ�ն˱���
	BYTE				Date[4];					// ��ֵ���� ������CCYYMMDD�������С�CC����ʾ������
	BYTE				Time[3];					// ��ֵʱ�� ������HHMMSS��
	BYTE				Mac2[4];					// MAC2
}AckTopUpStr_t;
typedef AckTopUpStr_t* pAckTopUpStr_t;

// ����������Ӧ����
typedef struct AckCancelStr
{
	BYTE				AppInsideNum[6];			// ��ֵ�����к�
	BYTE				Random[4];					// α�����
	BYTE				CalcMarker;					// �����㷨�汾
	BYTE				EncryVer;					// ������Կ�汾
	BYTE				TerminalDealNum[4];			// �ն˽������
	BYTE				Value[4];					// ���׽��
	BYTE				Type;						// �������ͱ�ʶ 06������Ǯ����ֵ����
	BYTE				TerminalNum[6];				// ��ֵ�ն˱���
	BYTE				Date[4];					// ��ֵ���� ������CCYYMMDD�������С�CC����ʾ������
	BYTE				Time[3];					// ��ֵʱ�� ������HHMMSS��
	BYTE				Mac1[4];					// MAC1
}AckCancelStr_t;
typedef AckCancelStr_t* pAckCancelStr_t;
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////

// DLL��ڳ��ڲ����ṹ(��ֵ���ѻ�����)

typedef struct EtcardInParam
{
//	BOOLEAN				SrhCard;		// ֻ����Ѱ��
//	BOOLEAN				Beep;			// <�����Ƿ������ ����ڳ�ֵʱ�˲�����λ��ֻ���ڳ�ֵ��ɺ�Ż����>
//	UINT8				SrhNum;			// <Ѱ������:Ĭ��3>
//    DWORD 				ReqTimeOut; 	// <����Req����:Ĭ��200>
	UINT8				Socket;			// <SAM���۱��>
	UINT8            	lValue[4];		// <��ֵʱΪҪ��ֵ�Ľ��>(b)
	UINT8				SysTm[7];		// <ϵͳʱ��7byte>,BCD
	UINT8				Pin[3];			// ����pin,BCD,�ڽ��й�����֤ʱʹ��
	UINT8               ucSwipedFlag;   //��ֵʱ�Ǳ�ʶ��IC ���ǷǽӴ���
	// ��ֵ\ǩ������ʱ��Ҫ�õ��Ĳ���
	UINT8				PosNum[4];		// Pos�����,BCD
	UINT8				PosDSN[3];		// �ն���ˮ��
	UINT8				OperaId[6];		// ����Ա��,���ȿɱ�,�ַ�����"A001"
	UINT8				ClientIp[32];	// �ն�IP,���ȿɱ�,�ַ�����"192.168.1.1"
	//--------
	UINT8 szDate[9];
	UINT8 szTime[7];
	UINT8 skey[16];
	UINT8 TerminalNum[6];			// PSAM���
     UINT32 uoldBalaceAmt;
}EtcardInParam_t;
typedef EtcardInParam_t* pEtcardInParam_t;

typedef struct EtcardOutParam
{
	CardInfo_t					CardInfo;				// ��Ƭ��Ϣ
	UINT8						TerminalNum[6];			// PSAM���
	UINT8						Balance[4];				// <�����4byte>
	UINT8						sCredit[3];				// <͸���޶�byte>
	UINT8                       szPollAmt[13];          // ���˽��
	UINT8                       iPollAmtLen;            //���˽���
	UINT8						OnlineDealSerial[2];	// Ǯ�������������(�ڳ�ֵȷ�ϲ�����ȡ)
	UINT8						OnlineDealSerialafter[2];	// Ǯ�������������(�ڳ�ֵȷ�ϲ�����ȡ)
	UINT8						OfflineDealSerial[2];	// Ǯ���ѻ��������
	pdealrecord_t				pRecordData;			// ���ѽ��׼�¼
	ptopup_cancel_record_t		pTopupCalData;			// ��ֵ��������¼
	UINT8						PinTimes;				// ������״̬��ΪERR_CODE_ADSAM_VERIFY_PINʱ,���ڷ��������Ե�PIN����
	UINT8						PrintTac[4];			// ��ֵȷ�ϣ�����ȷ�ϲ���󣬿��Ի�ȡ��ӡ��tac
}EtcardOutParam_t;
typedef EtcardOutParam_t* pEtcardOutParam_t;
//////////////////////////////////////////////////////////

typedef struct ETC_PriceList
{
    BYTE    Nums[2];                       // ����
    BYTE    Price[4];                      // �۸�
}ETC_PriceList_t;
typedef ETC_PriceList_t* pETC_PriceList_t;

#if 0

typedef struct ETC_OnlineConsume_InParam
{
	BOOLEAN				SrhCard;		// ֻ����Ѱ��
	BOOLEAN				Beep;			// <�����Ƿ������ ����ڳ�ֵʱ�˲�����λ��ֻ���ڳ�ֵ��ɺ�Ż����>
	BYTE				SrhNum;			// <Ѱ������:Ĭ��3>
    DWORD 				ReqTimeOut; 	// <����Req����:Ĭ��200>
	union BYTEtoDWORD	lValue;			// <����ʱΪ���׽������ʱ��0>/<��ֵʱΪҪ��ֵ�Ľ��>
	BYTE				Socket;			// <SAM���۱��>
	BYTE				SysTm[7];		// <ϵͳʱ��7byte>,BCD
	// ��������\ǩ������ʱ��Ҫ�õ��Ĳ���
	BYTE				CardPin[3];		// �û���pin,BCD,������������ʱʹ��
	BYTE                CardPinLen;     // �û���pin����,ע��pin�ĳ���Ϊż����Ϊpin�ַ�����,��pinΪ0x123456,pinlen = 6
	BYTE				PosNum[6];		// Pos�����,�����İ��Ų�ά��
	BYTE				PosDSN[4];		// �ն���ˮ��ָ�ն˲�������Ψһ�Ľ�����ˮ
	BYTE				OperaId[10];	// ����Ա��,N20
	BYTE                GroupId[10];    // �����ţ��ն�ά����������Χ�����ظ��Ĺ�����
	//BYTE				ClientIp[32];	// �ն�IP,���ȿɱ�,�ַ�����"192.168.1.1"

	// ��ʱʹ��Ʊ�۽ӿ�,����ETC_OnlineConsume_Apply_PaidCard�ӿ�ʱʹ��
	//BYTE                CdType;         // �ۿ����� 0x0A, 0x0B, 0x0C, 0x0D
	//BYTE                CdNum[2];       // ������ 
	////BYTE                CdCurrVal[4];   // ��ǰ�ۼۼ۸�

	//BYTE                CdTy_A_Val[4];  // �۸��A,B,C,D,���뵱ǰ���õļ۸��
 //   BYTE                CdTy_B_Val[4];
 //   BYTE                CdTy_C_Val[4];
 //   BYTE                CdTy_D_Val[4];

    // Old
    //ETC_PriceList_t     CdTypeA;
    //ETC_PriceList_t     CdTypeB;
    //ETC_PriceList_t     CdTypeC;
    //ETC_PriceList_t     CdTypeD;

    // New
    BYTE                GoodsName[30];                      // ��Ʒ����,����30byte����0
    BYTE                GoodsNums[2];                       // ����
    BYTE                GoodsPrice[4];                      // �۸�

}ETC_OnlineConsume_InParam_t;
typedef ETC_OnlineConsume_InParam_t* pETC_OnlineConsume_InParam_t;

typedef struct ETC_OnlineConsume_OutParam
{
	CardInfo_t					CardInfo;				// ��Ƭ��Ϣ
	BYTE						TerminalNum[6];			// PSAM���
	BYTE						Balance[4];				// <�����4byte>
	BYTE						OnlineDealSerial[2];	// Ǯ�������������(�ڳ�ֵȷ�ϲ�����ȡ)
	pdealrecord_t				pRecordData;			// ���ѽ��׼�¼
	//ptopup_cancel_record_t		pTopupCalData;			// ��ֵ��������¼
	BYTE						PinTimes;				// ������״̬��ΪERR_CODE_ADSAM_VERIFY_PINʱ,���ڷ��������Ե�PIN����
	BYTE						PrintTac[4];			// ��ֵȷ�ϣ�����ȷ�ϲ���󣬿��Ի�ȡ��ӡ��tac
}ETC_OnlineConsume_OutParam_t;
typedef ETC_OnlineConsume_OutParam_t* pETC_OnlineConsume_OutParam_t;

////////////////////////////////////////////////////////////////////////////////////////////////
// ��������������ݽṹ

// 67���ն˿�����Ϣ
typedef struct ETC_SignIn_TermCtrlInfo
{
    BYTE    SignInDate[4];      // ǩ������ ������CCYYMMDD��
    BYTE    SignInTime[3];      // ǩ��ʱ�� ������HHMMSS��
    BYTE    SignInUseTm[7];     // ǩ����Ч�� ������CCYYMMDDHHMMSS��
    BYTE    UpLimit[4];         // ���׽������
    BYTE    DownLimit[4];       // ���׽������
    BYTE    CardType;           // �������ѵĿ�Ƭ����,��ֵ������ʿ�
}ETC_SignIn_TermCtrlInfo_t;
typedef ETC_SignIn_TermCtrlInfo_t* pETC_SignIn_TermCtrlInfo_t;

// ���׼�¼
typedef struct ETC_RecordInfo
{
    BYTE    Value[4];               // ���׽��
    BYTE    AppCode[3];             // Ӧ�ô���
    BYTE    CoreSerialNum[10];      // ���Ľ�����ˮ
    BYTE    PsamId[6];              // psam����
    BYTE    OnlineDealSerial[2];    // ����������� 2
    BYTE    OfflineDealSerial[2];   // �ѻ�������� 2
    BYTE    Mac2[4];                // MAC2 4
    BYTE    Tac[4];                 // TAC 4
    BYTE    Balance[4];             // ��� 4
    BYTE    TerminalDealNum[4];     // �ն˽������ 4
    BYTE    SysTm[7];               // ����ʱ��
    BYTE    AppInsideNum[8];        // ��ͨ������
}ETC_RecordInfo_t;
typedef ETC_RecordInfo_t* pETC_RecordInfo_t;

// �ս����ݽṹ
typedef struct ETC_SettleInfo
{
    BYTE    SettleDate[4];          // �ս����� ������CCYYMMDD��
    BYTE    SettleTime[7];          // �ս�ʱ�� ������CCYYMMDDHHMMSS��
    BYTE    DealConunt[3];          // �����ܱ��� �ϴ��ն��ս���ն��Ѿ��ɹ����׵��ܱ���
    BYTE    DealVale[5];           // �����ܽ�� �ϴ��ն��ս���ն��Ѿ��ɹ����׵��ܽ��
    //BYTE    // ��չ���� �Զ������ݣ���������Ŀ���ִ��
}ETC_SettleInfo_t;
typedef ETC_SettleInfo_t* pETC_SettleInfo_t;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
/** ��ʼ��Sam */
//extern BYTE Etcard_InitSam(pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam);

/** �Ӵ�ʽ���� ���� */
//extern BYTE Etcard_Main(pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam);

/** �ǽӴ�ʽ���� ���� */
//extern BYTE RfEtcard_Main(pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam);

/* 
	1.�͹����໥��֤
	pInParam - ��ڲ���
	pOutParam - ���ڲ���
*/
//extern BYTE Etcard_AdminCard_Auth(pEtcardInParam_t pInParam, pEtcardOutParam_t pOutParam);

/* 
	�ǽӴ��ӿ�
	1.�͹����໥��֤
	pInParam - ��ڲ���
	pOutParam - ���ڲ���
*/
//extern BYTE RfEtcard_AdminCard_Auth(pEtcardInParam_t pInParam, pEtcardOutParam_t pOutParam);

/*
	2.ǩ����ʼ������
	pInParam - ��ڲ���
	pMessage - ���ص�ǩ����ʼ������
	pMessLen - ���ĳ���
*/
//extern BYTE Etcard_SignIn_Init(pEtcardInParam_t  pInParam, BYTE *pMessage, BYTE *pMessLen);

/*
	3.ǩ����֤����
	pInParam - ��ڲ���
	Responce - ǩ����ʼ����Ӧ����
	ResLen - ��Ӧ���ĳ���
	pMessage - ���ص�ǩ����֤����
	pMessLen - ���ĳ���
*/
//extern BYTE Etcard_SignIn_Auth(pEtcardInParam_t  pInParam, 
//							BYTE *Responce, BYTE ResLen, 
//							BYTE *pMessage, BYTE *pMessLen);

/*
	4.ǩ����֤��Ӧ
	Responce - ǩ����֤��Ӧ����
	ResLen - ��Ӧ���ĳ���
	TopUpLimit - ��ֵ���� 5byte,�� "00900"
	SysTm - ϵͳ���� 14byte �� "20080122083030"
	UseTm - ǩ��ʧЧ�� 14byte �� "20080123092020"
*/
//extern BYTE Etcard_SignIn_Auth_End(BYTE *Responce, BYTE ResLen, 
//									BYTE *TopUpLimit, BYTE *SysTm, BYTE *UseTm);

/*
	5.ǩ��
*/
//extern BYTE Etcard_SignIn_Out(pEtcardInParam_t  pInParam, 
//							BYTE *pMessage, BYTE *pMessLen);

/*
	6.ǩ����Ӧ
*/
//extern BYTE Etcard_SignIn_Out_End(pEtcardInParam_t  pInParam, BYTE *Responce, BYTE ResLen);

/*
	1.��ֵ���˲�ѯ
	pInParam - ��ڲ���
	pOutParam - ���ڲ���
	pMessage - ��ֵ��ѯ����
	pMessLen - ���ĳ���
Ʊ����ز�����OutParam�л�ȡ
*/
extern BYTE RfEtcCard_TopUp_Poll(pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam,
						BYTE *pMessage, BYTE *pMessLen);
extern BYTE EtcCard_TopUp_Poll(pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam,
						BYTE *pMessage, BYTE *pMessLen);

/*
	2.��ֵ����
	pInParam - ��ڲ���
	Responce - ��ֵ���˲�ѯ��Ӧ����
	ResLen - ��Ӧ���ĳ���
	pMessage - ��ֵ���뱨��
	pMessLen - ���ĳ���
*/
extern BYTE EtcCard_Cancel_ReqReady(BYTE PsamSocket, BYTE *pBackMoney, BYTE *pBmLen, BYTE *Responce, BYTE ResLen);
extern BYTE RfEtcCard_Cancel_ReqReady(BYTE PsamSocket, BYTE *pBackMoney, BYTE *pBmLen, BYTE *Responce, BYTE ResLen);


extern BYTE RfEtcCard_TopUp_Request(pEtcardInParam_t  pInParam, BYTE *pBackMoney, BYTE *pBmLen,
					BYTE *Responce, BYTE ResLen, 
					BYTE *pMessage, BYTE *pMessLen);
extern BYTE EtcCard_TopUp_Request(pEtcardInParam_t  pInParam, BYTE *pBackMoney, BYTE *pBmLen,
					BYTE *Responce, BYTE ResLen, 
					BYTE *pMessage, BYTE *pMessLen);


/*
	3.��ֵȷ��
	pInParam - ��ڲ���
	pAckTopUpStruct - ic������
	pOutParam - ���ڲ���
	Responce - ��ֵ������Ӧ����
	ResLen - ��Ӧ���ĳ���
	pMessage - ��ֵȷ�ϱ���
	pMessLen - ���ĳ���
*/
//#if 0
extern BYTE RfEtcCard_TopUp_Responce(pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam,
					BYTE *Responce, BYTE ResLen, 
					BYTE *pMessage, BYTE *pMessLen);
extern BYTE EtcCard_TopUp_Responce(pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam,
					BYTE *Responce, BYTE ResLen, 
					BYTE *pMessage, BYTE *pMessLen);
//#endif
extern BYTE RfEtcCard_TopUp_Responce(pEtcardInParam_t  pInParam, 
					pAckTopUpStr_t pAckTopUpStruct, 
					pEtcardOutParam_t pOutParam,
					BYTE *Responce, BYTE ResLen, 
					BYTE *pMessage, BYTE *pMessLen);

extern BYTE EtcCard_TopUp_Responce(pEtcardInParam_t  pInParam, 
					pAckTopUpStr_t pAckTopUpStruct, 
					pEtcardOutParam_t pOutParam,
					BYTE *Responce, BYTE ResLen, 
					BYTE *pMessage, BYTE *pMessLen);


/*
	4.У���ֵȷ����Ӧ����
	Responce - ��ֵȷ����Ӧ����
	ResLen - ��Ӧ���ĳ���
*/
extern BYTE RfEtcCard_TopUp_End(pEtcardInParam_t  pInParam, BYTE *Responce, BYTE ResLen);
extern BYTE EtcCard_TopUp_End(pEtcardInParam_t  pInParam, BYTE *Responce, BYTE ResLen);

/*
	1.��ֵ������ѯ
*/
extern BYTE RfEtcard_Cancel_Poll(pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam,
						BYTE *pMessage, BYTE *pMessLen);
extern BYTE Etcard_Cancel_Poll(pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam,
						BYTE *pMessage, BYTE *pMessLen);

/*
	2.��ֵ��������
*/
extern BYTE RfEtcard_Cancel_Request(pEtcardInParam_t  pInParam, BYTE *pBackMoney, BYTE *pBmLen,
					BYTE *Responce, BYTE ResLen, 
					BYTE *pMessage, BYTE *pMessLen);
extern BYTE Etcard_Cancel_Request(pEtcardInParam_t  pInParam, BYTE *pBackMoney, BYTE *pBmLen,
					BYTE *Responce, BYTE ResLen, 
					BYTE *pMessage, BYTE *pMessLen);

/*
	3.��ֵ����ȷ��

*/
extern BYTE RfEtcard_Cancel_Responce(pEtcardInParam_t  pInParam, 
					pAckCancelStr_t	pAckCancelStruct,
					pEtcardOutParam_t pOutParam,
					BYTE *Responce, BYTE ResLen, 
					BYTE *pMessage, BYTE *pMessLen);

extern BYTE Etcard_Cancel_Responce(pEtcardInParam_t  pInParam, 
					pAckCancelStr_t	pAckCancelStruct,
					pEtcardOutParam_t pOutParam,
					BYTE *Responce, BYTE ResLen, 
					BYTE *pMessage, BYTE *pMessLen);

/*
	4.��ֵ����ȷ����Ӧ
*/
extern BYTE RfEtcard_Cancel_End(pEtcardInParam_t  pInParam, BYTE *Responce, BYTE ResLen);
extern BYTE Etcard_Cancel_End(pEtcardInParam_t  pInParam, BYTE *Responce, BYTE ResLen);


/*
	��ȡ�汾��,��ʽ�� V0.01_20090410_01
*/
extern void Etcard_GetVersion(BYTE *pVersion);

/*
	��֯��ֵ���������׼�¼
	�ڳ�ֵȷ�ϡ�����ȷ�Ϻ����!
	��pOutParam��pTopupCalData�л�ȡ��¼
	����˵��:
	bTopup - ��ֵ������˵������ֵ:TRUE, ����:FALSE;
	
	Status - ״̬˵��,	'1'��"δд��"-���������������Σ�A.��ֵ�ն˷��ͳ�ֵ������������ʧ�ܣ�B.��ֵ�ն˷��ͳ�ֵ�������������δ�յ���Ӧ���ݣ�C. ��ֵ�ն˷��ͳ�ֵ��������������յ���������Ӧ����ʾ����D.��ֵ�ն˷��ͳ�ֵ��������������յ���������Ӧ����ʾ�ɹ��������ڳ�ֵ�����쳣��ԭ��δ������һ����д��������
						'2'��"д��ʧ��"- ָ����д��������δ�Դ�ֵ���ϵĵ���Ǯ�������гɹ����޸ģ�
						'3'��"д���쳣"- ָ����д�������󣬴�ֵ������Ǯ���Ľ������쳣��
						'4'��"��ȷ��"- ָ����д�������󣬴�ֵ���ϵĵ���Ǯ������Ѿ����ɹ��޸ģ���������·���ϵ�ԭ���·���"��ֵ���"���ݰ�ʱʧ�ܣ�
						'5'��"��ȷ��"- ָ��ֵ���ϵĵ���Ǯ������Ѿ����ɹ��޸��ҷ���"��ֵ���"���ݰ��ɹ�����"��ֵ��¼����"���ݰ��ɹ���

	rflag - ��¼��־��	'Y'������"��ֵ��¼����"���ݰ��ɹ���
						'N'���������

	cflag - ���ʱ�־��	'Y'������(��������˾������ֵ���������)
						'N'��������

*/
extern void Buidup_TopUp_Cancel_Record(BOOLEAN bTopup, BYTE Status, BYTE rflag, BYTE cflag, 
										pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam);


/*
	��ȡ��̨���صĴ�����붨�壬�����޷���������ʱ���鿴�������
	pErrCode - 2byte�������
	ע��:ֻ���ڴ������ʱ���ſ��Ի����Ч�Ĵ������!
*/
extern void GetBackgroundErrCode(BYTE	*pErrCode);


///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// �������Ѻ���

// psam ��ʼ��
extern BYTE ETC_OnlineConsume_InitSam(pETC_OnlineConsume_InParam_t  pInParam, 
                                            pETC_OnlineConsume_OutParam_t pOutParam);


// ǩ������
// "ǩ������"���ն������ķ����ն����ȶ�PSAM�����������PSAM������һ�������RAM1���ն˷���"ǩ������"����
extern BYTE ETC_OnlineConsume_SignIn_Init(pETC_OnlineConsume_InParam_t  pInParam, BYTE *pMessage, BYTE *pMessLen);


// ǩ����֤����
extern BYTE ETC_OnlineConsume_SignIn_Auth(pETC_OnlineConsume_InParam_t  pInParam, BYTE SignInType /* 0x00 - ����ǩ����0x01 - ��ǩ��*/, 
                    							BYTE *Responce, BYTE ResLen, 
                    							BYTE *pMessage, BYTE *pMessLen);

// ǩ����֤��Ӧ
extern BYTE ETC_OnlineConsume_SignIn_Auth_End(BYTE *Responce, BYTE ResLen, pETC_SignIn_TermCtrlInfo_t pSignInTermInfo);

// ǩ������
extern BYTE ETC_OnlineConsume_SignIn_Out(pETC_OnlineConsume_InParam_t  pInParam, 
							BYTE *pMessage, BYTE *pMessLen);

// ���ǩ����Ӧ����
extern BYTE ETC_OnlineConsume_SignIn_Out_End(pETC_OnlineConsume_InParam_t pInParam, BYTE *Responce, BYTE ResLen);


// ��ֵ����������
// �ɲ�ѯ�����ֻ��Ѱ��
extern BYTE    ETC_OnlineConsume_Apply_PaidCard(pETC_OnlineConsume_InParam_t pInParam, 
                                                         pETC_OnlineConsume_OutParam_t pOutParam,
                                                         BYTE *pMessage, BYTE *pMessLen);


// ��ֵ����ˮ�ϴ�
//extern BYTE    ETC_OnlineConsume_UpdateMessage_PaidCard(pETC_OnlineConsume_InParam_t pInParam, 
//                                                                      pETC_OnlineConsume_OutParam_t pOutParam,
//                                                                      BYTE *Responce, BYTE ResLen, 
//                                                                      BYTE *pMessage, BYTE *pMessLen);

extern BYTE    ETC_OnlineConsume_UpdateMessage_PaidCard(pETC_OnlineConsume_InParam_t pInParam, 
                                                                      pETC_OnlineConsume_OutParam_t pOutParam,
                                                                      pETC_RecordInfo_t pRecordInfo,
                                                                      BYTE *Responce, BYTE ResLen, 
                                                                      BYTE *pMessage, BYTE *pMessLen);


// �����ˮ�ϴ���Ӧ����
extern BYTE    ETC_OnlineConsume_End_PaidCard(pETC_OnlineConsume_InParam_t pInParam, /* ��ڲ���ֻʹ�õ���socket */
                                                        BYTE *Responce, 
                                                        BYTE ResLen);


// �ն��ս�����
extern BYTE    ETC_OnlineConsume_Day_Settlement(pETC_OnlineConsume_InParam_t pInParam, 
                                                            BYTE *pMessage, BYTE *pMessLen);

// ��ȡ�ն��ս�����
extern BYTE    ETC_OnlineConsume_Day_Settlement_End(pETC_OnlineConsume_InParam_t pInParam, pETC_SettleInfo_t pInfo,
                                                        BYTE *Responce, 
                                                        BYTE ResLen);
#endif
#endif
