
#ifndef _ST8583_H
#define _ST8583_H

#include "posapi.h"
#include "pack8583.h"

#define LEN_MSG_CODE			4
#define LEN_BITMAP				8
#define LEN_PAN					19
#define LEN_PROC_CODE			6
#define LEN_TRAN_AMT			12
#define LEN_POS_STAN			6
#define LEN_LOCAL_TIME			6
#define LEN_LOCAL_DATE			4
#define LEN_EXP_DATE			4
#define LEN_SETTLE_DATE			4
#define LEN_ENTRY_MODE			4	// 为了处理左对齐方式
#define LEN_PAN_SEQ_NO			3
#define LEN_SVR_CODE			2
#define LEN_CAPTURE_CODE		2
#define LEN_ACQUIRER_ID			11
#define LEN_TRACK2				37
#define LEN_TRACK3				104
#define LEN_RRN					12
#define LEN_AUTH_CODE			6
#define LEN_RSP_CODE			2
#define LEN_TERM_ID				8
#define LEN_MERCHANT_ID			15
#define LEN_ADD_RSP_DATA		25
#define LEN_FIELD48				322
#define LEN_CURRENCY_CODE		3
#define LEN_PIN_DATA			8
#define LEN_SECURITY_INFO		16
#define LEN_EXT_AMOUNT			20
#define LEN_ICC_DATA			255
#define LEN_EP_DATA				100
#define LEN_FIELD60				30
#define LEN_ORIGINAL_MSG		29
#define LEN_FIELD62				512
#define LEN_FIELD63				63
#define LEN_MAC_DATA			8

// 说明：
// 1. 对Binary类型的域，结构成员变量的前面2个字节为数据长度, 格式为高有效为在前的网络字节顺序
// 2. 对非Binary类型域，直接使用C的字符串操作/格式化函数进行赋值即可;
//    多余的2个字节是为了存储'\0'字符(为方便处理冗余了一个字符)
typedef struct _tagSTISO8583{
	uint8_t	szMsgCode[LEN_MSG_CODE+2];
	uint8_t	sBitMap[2*LEN_BITMAP];	// 不要加2
	uint8_t	szPan[LEN_PAN+2];
	uint8_t	szProcCode[LEN_PROC_CODE+2];
	uint8_t	szTranAmt[LEN_TRAN_AMT+2];
	uint8_t	szSTAN[LEN_POS_STAN+2];
	uint8_t	szLocalTime[LEN_LOCAL_TIME+2];
	uint8_t	szLocalDate[LEN_LOCAL_DATE+2];
	uint8_t	szExpDate[LEN_EXP_DATE+2];
	uint8_t	szSettleDate[LEN_SETTLE_DATE+2];
	uint8_t	szEntryMode[LEN_ENTRY_MODE+2];
	uint8_t	szPanSeqNo[LEN_PAN_SEQ_NO+2];
	uint8_t	szSvrCode[LEN_SVR_CODE+2];
	uint8_t	szCaptureCode[LEN_CAPTURE_CODE+2];
	uint8_t	szAcquirerID[LEN_ACQUIRER_ID+2];
	uint8_t	szTrack2[LEN_TRACK2+2];
	uint8_t	szTrack3[LEN_TRACK3+2];
	uint8_t	szRRN[LEN_RRN+2];
	uint8_t	szAuthCode[LEN_AUTH_CODE+2];
	uint8_t	szRspCode[LEN_RSP_CODE+2];
	uint8_t	szTermID[LEN_TERM_ID+2];
	uint8_t	szMerchantID[LEN_MERCHANT_ID+2];
	uint8_t	szAddRspData[LEN_ADD_RSP_DATA+2];
	uint8_t	szField48[LEN_FIELD48+2];
	uint8_t	szCurrencyCode[LEN_CURRENCY_CODE+2];
	uint8_t	sPINData[LEN_PIN_DATA+2];
	uint8_t	szSecurityInfo[LEN_SECURITY_INFO+2];
	uint8_t	szExtAmount[LEN_EXT_AMOUNT+2];
	uint8_t	sICCData[LEN_ICC_DATA+2];
	uint8_t	szEPData[LEN_EP_DATA+2];
	uint8_t	szField60[LEN_FIELD60+2];
	uint8_t	szOrginalMsg[LEN_ORIGINAL_MSG+2];
	uint8_t	sField62[LEN_FIELD62+2];
	uint8_t	szField63[LEN_FIELD63+2];
	uint8_t	sMacData[LEN_MAC_DATA+2];
}STISO8583;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

extern FIELD_ATTR	glMsg0[];
extern FIELD_ATTR	glData0[];
extern STISO8583	glSendPack, glRecvPack;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _ST8583_H */

// end of file
