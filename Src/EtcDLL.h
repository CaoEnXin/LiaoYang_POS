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

typedef unsigned char  uint8;                   /* defined for unsigned 8-bits integer variable 	无符号8位整型变量  */
typedef signed   char  int8;                    /* defined for signed 8-bits integer variable		有符号8位整型变量  */
typedef unsigned short uint16;                  /* defined for unsigned 16-bits integer variable 	无符号16位整型变量 */
typedef signed   short int16;                   /* defined for signed 16-bits integer variable 		有符号16位整型变量 */
typedef unsigned int   uint32;                  /* defined for unsigned 32-bits integer variable 	无符号32位整型变量 */
typedef signed   int   int32;                   /* defined for signed 32-bits integer variable 		有符号32位整型变量 */
typedef float          fp32;                    /* single precision floating point variable (32bits) 单精度浮点数（32位长度） */
typedef double         fp64;                    /* double precision floating point variable (64bits) 双精度浮点数（64位长度） */

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

#define SUCCESS								0x00			/** 操作成功					*/

#define ERR_CODE_SAM_SOCKET					0xD4//0x10			/** 设置SAM波特率失败			*/
#define ERR_CODE_SAM_RESET					0xD5//0x11			/** 复位SAM卡失败				*/
#define ERR_CODE_SAM_READ_TERMNUM			0xD6//0x12			/** 读取终端机编号失败			*/
#define ERR_CODE_SAM_READ_PUBINFO			0xD7//0x13			/** 读取卡片公共信息文件失败	*/
#define ERR_CODE_SAM_SELECT_ADF				0xD8//0x14			/** SAM选择应用失败				*/
#define ERR_CODE_SAM_READ_APPPUBINFO		0xD9//0x15			/** SAM读取应用公共信息文件失败	*/
#define ERR_CODE_CALC_MAC1					0xDA//0x16			/** SAM计算MAC1失败				*/
#define ERR_CODE_SAM_CHKOUT_MAC2			0xDB//0x17			/** SAM验证MAC2失败				*/
#define ERR_CODE_USER_CANCEL				0xDC//0x0F			/** 用户取消					*/
#define ERR_CODE_USERCARD_VAL_INVAL         0xDE                /** 非储值卡                    */
#define ERR_CODE_USERCARD_BAL_INVAL         0xDF                /** 储值卡余额超过10000         */

#define ERR_CODE_MIFARE_ANTICOLL			0xE0			/** mifare anticoll失败			*/
#define ERR_CODE_MIFARE_SELECT				0xE1			/** mifare select失败			*/

#define ERR_CODE_USECARD_SOCKET				0xE2			/** 检测用户卡片失败			*/
#define ERR_CODE_USECARD_RESET				0xE3			/** 用户卡复位失败				*/
#define ERR_CODE_USECARD_READ_BASICINFO		0xE4			/** 读取用户卡基本数据失败		*/
#define ERR_CODE_USECARD_SELECT_ADF			0xE5			/** 选择用户卡ADF1001失败		*/
#define ERR_CODE_USECARD_READ_APPINFO		0xE6			/** 读取用户卡应用数据失败		*/
#define ERR_CODE_USECARD_INVALID			0xE7			/** 卡片数据无效(如已经过期)	*/
#define ERR_CODE_READ_BALANCE				0xE8			/** 读取用户卡余额失败			*/
#define ERR_CODE_INIT_PURCHASE				0xE9			/** 消费初始化失败				*/
#define ERR_CODE_LACK_OF_BALANCE			0xEA			/** 余额不足					*/
#define ERR_CODE_CHKOUT_MAC1				0xEB			/** 用户卡校验MAC1失败			*/
#define ERR_CODE_DEBIT_PURCHASE				0xEC			/** 扣款失败					*/

#define ERR_CODE_VERIFY_PIN					0xED			/** 校验PIN失败					*/
#define ERR_CODE_INIT_LOAD					0xEE			/** 充值初始化错误				*/

#define ERR_CODE_ADSAM_SOCKET				0xC0			/** 管理卡设置SAM波特率失败		*/
#define ERR_CODE_ADSAM_RESET				0xC1			/** 管理卡复位SAM卡失败			*/
#define ERR_CODE_ADSAM_SELECT_ADF_1			0xC3			/** 管理卡SAM选择应用失败		*/
#define ERR_CODE_ADSAM_SELECT_ADF_2			0xC4			/** 管理卡SAM选择应用失败		*/
#define ERR_CODE_ADSAM_READ_PSAMID			0xC5			/** 管理卡读取保存psamid失败	*/
#define ERR_CODE_ADSAM_MATCH				0xC6			/** 管理卡与psam卡不匹配		*/
#define ERR_CODE_ADSAM_GET_RANDOM			0xC7			/** 管理卡获取随机数失败		*/ 
#define ERR_CODE_ADSAM_VERIFY_PIN			0xC8			/** 管理卡校验PIN失败			*/
#define ERR_CODE_INTERNAL_AUTH				0xC9			/** 管理卡内部认证失败			*/
#define ERR_CODE_EXTERNAL_AUTH				0xCA			/** PSAM外部认证失败			*/
#define ERR_CODE_RESPONSE_CHALLENGE			0xCB			/** PSAM生成挑战应答信息失败	*/
#define ERR_CODE_SAM_CALC_LINEMAC			0xCC			/** 生成线路保护MAC码失败		*/
#define ERR_CODE_DATA_LEN					0xCD			/** 数据长度错误				*/
#define ERR_CODE_VERIFY_LINE_MAC			0xCE			/** 验证线路保护mac失败			*/
#define ERR_CODE_RESPONCE_HEADTYPE			0xCF			/** 报文响应信息类型错误		*/
#define ERR_CODE_RESPONCE_DATA				0xD0			/** 报文响应信息数据错误		*/
#define	ERR_CODE_CREBIT_FOR_LOAD			0xD1			/** 充值失败					*/
#define ERR_CODE_VERIFY_MAC					0xDD//0xD2		/** 线路保护MAC错误				*/
#define ERR_CODE_CAL_ENCRY_PIN              0xD3            /** 计算加密pin失败             */


#define ERR_CODE_RESP_ERR1					0x04//0x80			/** 没收卡 该粤通卡已入黑名单 */
#define ERR_CODE_RESP_ERR2					0x14//0x81			/** 无效卡号 */
#define ERR_CODE_RESP_ERR3					0x25//0x82			/** 找不到原交易 */
#define ERR_CODE_RESP_ERR4					0x30//0x83			/** 格式错误 */
#define ERR_CODE_RESP_ERR5					0x33//0x84			/** 过期卡 */
#define ERR_CODE_RESP_ERR6					0x40//0x85			/** 请求的功能不支持 */
#define ERR_CODE_RESP_ERR7					0x41//0x86			/** 挂失卡 */
#define ERR_CODE_RESP_ERR8					0x43//0x87			/** 被窃卡 */
#define ERR_CODE_RESP_ERR9					0x63//0x88			/** 侵犯安全 */
#define ERR_CODE_RESP_ERR10					0x64//0x89			/** 原始金额错误 */
#define ERR_CODE_RESP_ERR11					0x76//0x8A			/** 无效帐户/密码格式错误 */ // 解释有修改
#define ERR_CODE_RESP_ERR12					0x91//0x8B			/** 中心不能操作 未签到 */
#define ERR_CODE_RESP_ERR13					0x94//0x8C			/** 重复交易 */
#define ERR_CODE_RESP_ERR14					0xA0//0x8D			/** MAC 鉴别失败 */
#define ERR_CODE_RESP_ERR15					0xD2//0x8E			/** 日期错误 */
#define ERR_CODE_RESP_ERR16					0xB1//0x8F			/** 卡片认证失败 */ 实际为Q1
#define ERR_CODE_RESP_ERR17					0xF0//0x90			/** 无此PSAM卡 */
#define ERR_CODE_RESP_ERR18					0xF1//0x91			/** PSAM卡入黑名单 */
#define ERR_CODE_RESP_ERR19					0xF2//0x92			/** 无此终端号 */
#define ERR_CODE_RESP_ERR20					0xF3//0x93			/** PSAM卡使用不当 */
#define ERR_CODE_RESP_ERR21					0xF4//0x94			/** psam卡非正常状态(废弃) */ // 解释有修改 ,未签到,1、终端未签到成功，2、终端签到有效期过期
#define ERR_CODE_RESP_ERR22					0xF5//0x95			/** IC卡数据MAC错误 */
#define ERR_CODE_RESP_ERR23					0xFA//0x96			/** 非本系统卡 */
#define ERR_CODE_RESP_ERR24					0xFB//0x97			/** 无卡注销卡 */
#define ERR_CODE_RESP_ERR25					0x99//0x98			/** 未知错误 */
#define ERR_CODE_RESP_ERR26					0x68//0x99			/** 签到有效期错误 */
#define ERR_CODE_RESP_ERR27					0x66//0x9A			/** 已经在签到有效期 */ // 解释有修改
// 新增加的错误代码2010/1/16
#define ERR_CODE_RESP_ERR28                 0x65//0x9B            // 交易金额错误,消费申请时金额与后台约定不符
#define ERR_CODE_RESP_ERR29                 0x71//0x9C            // 终端交易总额已超
#define ERR_CODE_RESP_ERR30                 0x72//0x9D            // 卡片交易总额已超
#define ERR_CODE_RESP_ERR31                 0x73//0x9E            // 卡片交易金额过低
#define ERR_CODE_RESP_ERR32                 0x74//0x9F            // 卡片交易次数已超
#define ERR_CODE_RESP_ERR33                 0x75//0xA0            // 卡片密码错误
#define ERR_CODE_RESP_ERR34                 0x92//0xA1            // 超出有效期，超出终端签到有效期
#define ERR_CODE_RESP_ERR35                 0x93//0xA2            // 超时，交易超时

#define ERR_CODE_RESP_ERR36                 0x95//0xA3            // 交易已冲正
#define ERR_CODE_RESP_ERR37                 0x96//0xA4            // 交易已撤销
#define ERR_CODE_RESP_ERR38                 0xF6//0xA5            // Psam为挂失卡
#define ERR_CODE_RESP_ERR39                 0xF7//0xA6            // Psam为无卡注销
#define ERR_CODE_RESP_ERR40                 0xFC//0xA7            // 伪卡
#define ERR_CODE_RESP_ERR41                 0xA1//0xA8            // 上传中心流水与中心系统记录中心流水不符
#define ERR_CODE_RESP_ERR42                 0xA2//0xA9            // 消费金额大于系统金额 
#define ERR_CODE_RESP_ERR43                 0xA3//0xAA            // 请求交易已经处理 
#define ERR_CODE_RESP_ERR44                 0xA4//0xAB            // 57域解析出错
#define ERR_CODE_RESP_ERR45                 0xA5//0xAC            // 汇总笔数错误
#define ERR_CODE_RESP_ERR46                 0xA6//0xAD            // 汇总金额错误 

#define ERR_CODE_RESP_ERR47                 0x69//0xAE            // 上一笔不是充值
//#define ERR_CODE_RESP_ERR48                 0xAF            // 签到有效期错误
#define ERR_CODE_RESP_ERR49                 0xB0//0xB0            // 无此PSAM卡,实际为X0
#define ERR_CODE_RESP_ERR50                 0x67//0xB1            // 未签到
#define ERR_CODE_RESP_ERR51                 0xA7//0xB2            // 业务类型限制

typedef struct dealrecord
{
	BYTE	RedType;				// 记录类型 bcd
	BYTE	DealType[2];			// 交易类型 hex
	BYTE	AppSerilaNum[16];		// 用户卡应用系列号
	BYTE	POSNum[6];				// pos机编号
	BYTE	TerminalNum[12];		// 终端机编号
	BYTE	TerminalDealNum[8];		// 终端交易序号
	BYTE	Date[8];				// 交易日期
	BYTE	Time[6];				// 交易时间
	BYTE	Pre_Balance[8];			// 交易前余额 8
	BYTE	PAmount[8];				// 交易金额(实收金额) 4
	BYTE	BAmount[8];				// 应收金额 4
	BYTE	PsamVer[2];				// PSAM卡版本号
	BYTE	OfflineDealSerial[5];	// 卡片脱机交易序列号
	BYTE	Tac[8];					// 交易认证tac
	BYTE	CityCode[4];			// 城市代码
	BYTE	TradeCode[2];			// 行业代码
	BYTE	BranchCode[4];			// 部门代码
	BYTE	Mac[8];					// MAC码
}dealrecord_t;
typedef dealrecord_t* pdealrecord_t;

typedef struct topup_cancel_record
{
	BYTE	AppSerilaNum[16];		// 粤通卡卡号16
	BYTE	Date[10];				// 交易日期
	BYTE	Time[8];				// 交易时间
	BYTE	type;					// 交易类型
	BYTE	Amount[8];				// 交易金额
	BYTE	RAmount[8];				// 回退金额
	BYTE	Pre_Balance[8];			// 交易前卡片余额
	BYTE	Balance[8];				// 交易后卡片余额
	BYTE    PosDSN[6];              // 终端流水号 2010/1/14 added by zj
	BYTE	Status;					// 状态
	BYTE	rflag;					// 补录标志
	BYTE	cflag;					// 记帐标志
}topup_cancel_record_t;
typedef topup_cancel_record_t* ptopup_cancel_record_t;

//////////////////////////////////////////////////////
// 卡片公共信息文件
typedef struct PsamPublicInfo
{
	BYTE PsamId[10];		// PSAM序列号
	BYTE PsamVer;			// PSAM版本号
	BYTE EncrKeyType;		// 密钥卡类型
	BYTE FCIData[2];		// 发卡方自定义FCI数据
}PsamPublicInfo_t;
typedef PsamPublicInfo_t* pPsamPublicInfo_t;

typedef struct AppPublicInfo
{
	BYTE ConsumeKeyidx;			// 消费密钥索引号
	BYTE IssuerMarker[8];		// 发卡方标识串
	BYTE LinkChargeMarker[8];	// 联网收费区域编号
	BYTE StartDate[4];			// 应用启用日期
	BYTE AvailData[4];			// 应用有效日期
}AppPublicInfo_t;
typedef AppPublicInfo_t* pAppPublicInfo_t;

// PSAM数据结构
typedef struct PsamStruct
{
	BYTE				TerminalNum[6];		// 终端编号
	PsamPublicInfo_t	SamPubInfoStr;		// 卡片公共信息
	AppPublicInfo_t		AppPubInfoStr;		// 应用公共信息文件
}PsamStruct_t;
typedef PsamStruct_t* pPsamStruct_t;
//////////////////////////////////////////////////////////
// 卡片结构

// 公共应用基本数据文件
typedef struct CardPublicBasicInfo
{
	BYTE IssueMarker[8];			// 发卡方标识08
	BYTE AppType;					// 卡类型标识01
	BYTE AppVer;					// 卡片版本号01
	BYTE AppNetworkNum[2];			// 卡片网络编号(02)
	BYTE AppInsideNum[8];			// 卡片内部编号(08)
	//BYTE AppSerilaNum[10];		// 应用序列号
	BYTE AppStartDate[4];			// 应用启用日期
	BYTE AppAvailDate[4];			// 应用有效日期
	BYTE FCIData[2];				// 发卡方自定义FCI数据
}CardPublicBasicInfo_t;
typedef CardPublicBasicInfo_t* pCardPublicBasicInfo_t;

// 持卡人基本数据文件
typedef struct CardholderBasicInfo
{
	BYTE FigureMarker;			// 持卡人身份标识(01)
	BYTE PersonnelMarker;		// 本系统职工标识(01)
	BYTE Name[20];				// 持卡人姓名
	BYTE PapersNum[16];			// 持卡人证件号码
	BYTE PapersType;			// 持卡人证件类型
}CardholderBasicInfo_t;
typedef CardholderBasicInfo_t* pCardholderBasicInfo_t;

// 卡片信息
typedef struct CardInfo
{
	CardPublicBasicInfo_t	AppInfo;	// 公共应用基本数据文件
	CardholderBasicInfo_t	HolderInfo;	// 持卡人基本数据文件
}CardInfo_t;
typedef CardInfo_t* pCardInfo_t;

// Init_for_Purchase结构
typedef struct InitforPurchaseStr
{
	BYTE 				Balance[4];				// 余额
	BYTE				OfflineDealSerial[2];	// 钱包脱机交易序号
	BYTE				Overdraft[3];			// 透支限额
	BYTE 				EncryVer;				// 密钥版本号
	BYTE 				CalcMarker;				// 算法标识
	BYTE 				Random[4];				// 伪随机数
}InitforPurchaseStr_t;
typedef InitforPurchaseStr_t* pInitforPurchaseStr_t;

// Init_for_Load结构
typedef struct InitforLoadStr
{
	BYTE 				Balance[4];					// 余额
	BYTE				OnlineDealSerial[2];		// 钱包联机交易序号
	BYTE 				EncryVer;					// 密钥版本号
	BYTE 				CalcMarker;					// 算法标识
	BYTE 				Random[4];					// 伪随机数
	BYTE				Mac1[4];					// MAC1
}InitforLoadStr_t;
typedef InitforLoadStr_t* pInitforLoadStr_t;

// 充值申请响应报文
typedef struct AckTopUpStr
{
	BYTE				AppInsideNum[6];			// 储值卡序列号
	BYTE				Random[4];					// 伪随机数
	BYTE				CalcMarker;					// 交易算法版本
	BYTE				EncryVer;					// 交易密钥版本
	BYTE				OnlineDealSerial[2];		// 储值卡当前联机交易序列号,每当交易发生时就增加
//	BYTE                OnlineDealSerialofter[2];		// 储值卡当前联机交易序列号,每当交易发生时就增加
	BYTE				Value[4];					// 交易金额
	BYTE				Type;						// 交易类型标识 02：电子钱包充值
	BYTE				TerminalNum[6];				// 充值终端编码
	BYTE				Date[4];					// 充值日期 主机（CCYYMMDD），其中“CC”表示：世纪
	BYTE				Time[3];					// 充值时间 主机（HHMMSS）
	BYTE				Mac2[4];					// MAC2
}AckTopUpStr_t;
typedef AckTopUpStr_t* pAckTopUpStr_t;

// 撤销申请响应报文
typedef struct AckCancelStr
{
	BYTE				AppInsideNum[6];			// 储值卡序列号
	BYTE				Random[4];					// 伪随机数
	BYTE				CalcMarker;					// 交易算法版本
	BYTE				EncryVer;					// 交易密钥版本
	BYTE				TerminalDealNum[4];			// 终端交易序号
	BYTE				Value[4];					// 交易金额
	BYTE				Type;						// 交易类型标识 06：电子钱包充值撤销
	BYTE				TerminalNum[6];				// 充值终端编码
	BYTE				Date[4];					// 充值日期 主机（CCYYMMDD），其中“CC”表示：世纪
	BYTE				Time[3];					// 充值时间 主机（HHMMSS）
	BYTE				Mac1[4];					// MAC1
}AckCancelStr_t;
typedef AckCancelStr_t* pAckCancelStr_t;
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////

// DLL入口出口参数结构(充值、脱机消费)

typedef struct EtcardInParam
{
//	BOOLEAN				SrhCard;		// 只进行寻卡
//	BOOLEAN				Beep;			// <读卡是否轰鸣， 如果在充值时此参数置位，只有在充值完成后才会轰鸣>
//	UINT8				SrhNum;			// <寻卡次数:默认3>
//    DWORD 				ReqTimeOut; 	// <单次Req次数:默认200>
	UINT8				Socket;			// <SAM卡槽编号>
	UINT8            	lValue[4];		// <充值时为要充值的金额>(b)
	UINT8				SysTm[7];		// <系统时间7byte>,BCD
	UINT8				Pin[3];			// 管理卡pin,BCD,在进行管理卡认证时使用
	UINT8               ucSwipedFlag;   //充值时是标识是IC 还是非接触卡
	// 充值\签到操作时需要用到的参数
	UINT8				PosNum[4];		// Pos机编号,BCD
	UINT8				PosDSN[3];		// 终端流水号
	UINT8				OperaId[6];		// 操作员号,长度可变,字符串如"A001"
	UINT8				ClientIp[32];	// 终端IP,长度可变,字符串如"192.168.1.1"
	//--------
	UINT8 szDate[9];
	UINT8 szTime[7];
	UINT8 skey[16];
	UINT8 TerminalNum[6];			// PSAM编号
     UINT32 uoldBalaceAmt;
}EtcardInParam_t;
typedef EtcardInParam_t* pEtcardInParam_t;

typedef struct EtcardOutParam
{
	CardInfo_t					CardInfo;				// 卡片信息
	UINT8						TerminalNum[6];			// PSAM编号
	UINT8						Balance[4];				// <卡余额4byte>
	UINT8						sCredit[3];				// <透资限额byte>
	UINT8                       szPollAmt[13];          // 回退金额
	UINT8                       iPollAmtLen;            //回退金额长度
	UINT8						OnlineDealSerial[2];	// 钱包联机交易序号(在充值确认步骤后获取)
	UINT8						OnlineDealSerialafter[2];	// 钱包联机交易序号(在充值确认步骤后获取)
	UINT8						OfflineDealSerial[2];	// 钱包脱机交易序号
	pdealrecord_t				pRecordData;			// 消费交易记录
	ptopup_cancel_record_t		pTopupCalData;			// 充值、冲正记录
	UINT8						PinTimes;				// 当返回状态码为ERR_CODE_ADSAM_VERIFY_PIN时,用于返回能重试的PIN次数
	UINT8						PrintTac[4];			// 充值确认，撤销确认步骤后，可以获取打印用tac
}EtcardOutParam_t;
typedef EtcardOutParam_t* pEtcardOutParam_t;
//////////////////////////////////////////////////////////

typedef struct ETC_PriceList
{
    BYTE    Nums[2];                       // 数量
    BYTE    Price[4];                      // 价格
}ETC_PriceList_t;
typedef ETC_PriceList_t* pETC_PriceList_t;

#if 0

typedef struct ETC_OnlineConsume_InParam
{
	BOOLEAN				SrhCard;		// 只进行寻卡
	BOOLEAN				Beep;			// <读卡是否轰鸣， 如果在充值时此参数置位，只有在充值完成后才会轰鸣>
	BYTE				SrhNum;			// <寻卡次数:默认3>
    DWORD 				ReqTimeOut; 	// <单次Req次数:默认200>
	union BYTEtoDWORD	lValue;			// <消费时为交易金额，读余额时置0>/<充值时为要充值的金额>
	BYTE				Socket;			// <SAM卡槽编号>
	BYTE				SysTm[7];		// <系统时间7byte>,BCD
	// 联机消费\签到操作时需要用到的参数
	BYTE				CardPin[3];		// 用户卡pin,BCD,在做消费申请时使用
	BYTE                CardPinLen;     // 用户卡pin长度,注意pin的长度为偶数，为pin字符长度,如pin为0x123456,pinlen = 6
	BYTE				PosNum[6];		// Pos机编号,由中心安排并维护
	BYTE				PosDSN[4];		// 终端流水号指终端产生自身唯一的交易流水
	BYTE				OperaId[10];	// 操作员号,N20
	BYTE                GroupId[10];    // 工班编号，终端维护的在自身范围不可重复的工班编号
	//BYTE				ClientIp[32];	// 终端IP,长度可变,字符串如"192.168.1.1"

	// 临时使用票价接口,调用ETC_OnlineConsume_Apply_PaidCard接口时使用
	//BYTE                CdType;         // 售卡类型 0x0A, 0x0B, 0x0C, 0x0D
	//BYTE                CdNum[2];       // 卡数量 
	////BYTE                CdCurrVal[4];   // 当前售价价格

	//BYTE                CdTy_A_Val[4];  // 价格表，A,B,C,D,填入当前设置的价格表
 //   BYTE                CdTy_B_Val[4];
 //   BYTE                CdTy_C_Val[4];
 //   BYTE                CdTy_D_Val[4];

    // Old
    //ETC_PriceList_t     CdTypeA;
    //ETC_PriceList_t     CdTypeB;
    //ETC_PriceList_t     CdTypeC;
    //ETC_PriceList_t     CdTypeD;

    // New
    BYTE                GoodsName[30];                      // 商品名称,不够30byte的左补0
    BYTE                GoodsNums[2];                       // 数量
    BYTE                GoodsPrice[4];                      // 价格

}ETC_OnlineConsume_InParam_t;
typedef ETC_OnlineConsume_InParam_t* pETC_OnlineConsume_InParam_t;

typedef struct ETC_OnlineConsume_OutParam
{
	CardInfo_t					CardInfo;				// 卡片信息
	BYTE						TerminalNum[6];			// PSAM编号
	BYTE						Balance[4];				// <卡余额4byte>
	BYTE						OnlineDealSerial[2];	// 钱包联机交易序号(在充值确认步骤后获取)
	pdealrecord_t				pRecordData;			// 消费交易记录
	//ptopup_cancel_record_t		pTopupCalData;			// 充值、冲正记录
	BYTE						PinTimes;				// 当返回状态码为ERR_CODE_ADSAM_VERIFY_PIN时,用于返回能重试的PIN次数
	BYTE						PrintTac[4];			// 充值确认，撤销确认步骤后，可以获取打印用tac
}ETC_OnlineConsume_OutParam_t;
typedef ETC_OnlineConsume_OutParam_t* pETC_OnlineConsume_OutParam_t;

////////////////////////////////////////////////////////////////////////////////////////////////
// 联机消费相关数据结构

// 67域终端控制信息
typedef struct ETC_SignIn_TermCtrlInfo
{
    BYTE    SignInDate[4];      // 签到日期 主机（CCYYMMDD）
    BYTE    SignInTime[3];      // 签到时间 主机（HHMMSS）
    BYTE    SignInUseTm[7];     // 签到有效期 主机（CCYYMMDDHHMMSS）
    BYTE    UpLimit[4];         // 交易金额上限
    BYTE    DownLimit[4];       // 交易金额下限
    BYTE    CardType;           // 允许消费的卡片种类,储值卡或记帐卡
}ETC_SignIn_TermCtrlInfo_t;
typedef ETC_SignIn_TermCtrlInfo_t* pETC_SignIn_TermCtrlInfo_t;

// 交易记录
typedef struct ETC_RecordInfo
{
    BYTE    Value[4];               // 交易金额
    BYTE    AppCode[3];             // 应用代码
    BYTE    CoreSerialNum[10];      // 中心交易流水
    BYTE    PsamId[6];              // psam卡号
    BYTE    OnlineDealSerial[2];    // 联机交易序号 2
    BYTE    OfflineDealSerial[2];   // 脱机交易序号 2
    BYTE    Mac2[4];                // MAC2 4
    BYTE    Tac[4];                 // TAC 4
    BYTE    Balance[4];             // 余额 4
    BYTE    TerminalDealNum[4];     // 终端交易序号 4
    BYTE    SysTm[7];               // 交易时间
    BYTE    AppInsideNum[8];        // 粤通卡卡号
}ETC_RecordInfo_t;
typedef ETC_RecordInfo_t* pETC_RecordInfo_t;

// 日结数据结构
typedef struct ETC_SettleInfo
{
    BYTE    SettleDate[4];          // 日结日期 主机（CCYYMMDD）
    BYTE    SettleTime[7];          // 日结时间 主机（CCYYMMDDHHMMSS）
    BYTE    DealConunt[3];          // 交易总笔数 上次终端日结后，终端已经成功交易的总笔数
    BYTE    DealVale[5];           // 交易总金额 上次终端日结后，终端已经成功交易的总金额
    //BYTE    // 扩展数据 自定义数据，可以视项目情况执行
}ETC_SettleInfo_t;
typedef ETC_SettleInfo_t* pETC_SettleInfo_t;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
/** 初始化Sam */
//extern BYTE Etcard_InitSam(pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam);

/** 接触式界面 消费 */
//extern BYTE Etcard_Main(pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam);

/** 非接触式界面 消费 */
//extern BYTE RfEtcard_Main(pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam);

/* 
	1.和管理卡相互认证
	pInParam - 入口参数
	pOutParam - 出口参数
*/
//extern BYTE Etcard_AdminCard_Auth(pEtcardInParam_t pInParam, pEtcardOutParam_t pOutParam);

/* 
	非接触接口
	1.和管理卡相互认证
	pInParam - 入口参数
	pOutParam - 出口参数
*/
//extern BYTE RfEtcard_AdminCard_Auth(pEtcardInParam_t pInParam, pEtcardOutParam_t pOutParam);

/*
	2.签到初始化请求
	pInParam - 入口参数
	pMessage - 返回的签到初始化报文
	pMessLen - 报文长度
*/
//extern BYTE Etcard_SignIn_Init(pEtcardInParam_t  pInParam, BYTE *pMessage, BYTE *pMessLen);

/*
	3.签到认证请求
	pInParam - 入口参数
	Responce - 签到初始化响应报文
	ResLen - 响应报文长度
	pMessage - 返回的签到认证报文
	pMessLen - 报文长度
*/
//extern BYTE Etcard_SignIn_Auth(pEtcardInParam_t  pInParam, 
//							BYTE *Responce, BYTE ResLen, 
//							BYTE *pMessage, BYTE *pMessLen);

/*
	4.签到认证响应
	Responce - 签到认证响应报文
	ResLen - 响应报文长度
	TopUpLimit - 充值上限 5byte,如 "00900"
	SysTm - 系统日期 14byte 如 "20080122083030"
	UseTm - 签到失效期 14byte 如 "20080123092020"
*/
//extern BYTE Etcard_SignIn_Auth_End(BYTE *Responce, BYTE ResLen, 
//									BYTE *TopUpLimit, BYTE *SysTm, BYTE *UseTm);

/*
	5.签退
*/
//extern BYTE Etcard_SignIn_Out(pEtcardInParam_t  pInParam, 
//							BYTE *pMessage, BYTE *pMessLen);

/*
	6.签退响应
*/
//extern BYTE Etcard_SignIn_Out_End(pEtcardInParam_t  pInParam, BYTE *Responce, BYTE ResLen);

/*
	1.充值回退查询
	pInParam - 入口参数
	pOutParam - 出口参数
	pMessage - 充值查询报文
	pMessLen - 报文长度
票卡相关参数在OutParam中获取
*/
extern BYTE RfEtcCard_TopUp_Poll(pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam,
						BYTE *pMessage, BYTE *pMessLen);
extern BYTE EtcCard_TopUp_Poll(pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam,
						BYTE *pMessage, BYTE *pMessLen);

/*
	2.充值申请
	pInParam - 入口参数
	Responce - 充值回退查询响应报文
	ResLen - 响应报文长度
	pMessage - 充值申请报文
	pMessLen - 报文长度
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
	3.充值确认
	pInParam - 入口参数
	pAckTopUpStruct - ic卡数据
	pOutParam - 出口参数
	Responce - 充值申请响应报文
	ResLen - 响应报文长度
	pMessage - 充值确认报文
	pMessLen - 报文长度
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
	4.校验充值确认响应报文
	Responce - 充值确认响应报文
	ResLen - 响应报文长度
*/
extern BYTE RfEtcCard_TopUp_End(pEtcardInParam_t  pInParam, BYTE *Responce, BYTE ResLen);
extern BYTE EtcCard_TopUp_End(pEtcardInParam_t  pInParam, BYTE *Responce, BYTE ResLen);

/*
	1.充值撤销查询
*/
extern BYTE RfEtcard_Cancel_Poll(pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam,
						BYTE *pMessage, BYTE *pMessLen);
extern BYTE Etcard_Cancel_Poll(pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam,
						BYTE *pMessage, BYTE *pMessLen);

/*
	2.充值撤销申请
*/
extern BYTE RfEtcard_Cancel_Request(pEtcardInParam_t  pInParam, BYTE *pBackMoney, BYTE *pBmLen,
					BYTE *Responce, BYTE ResLen, 
					BYTE *pMessage, BYTE *pMessLen);
extern BYTE Etcard_Cancel_Request(pEtcardInParam_t  pInParam, BYTE *pBackMoney, BYTE *pBmLen,
					BYTE *Responce, BYTE ResLen, 
					BYTE *pMessage, BYTE *pMessLen);

/*
	3.充值撤销确认

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
	4.充值撤销确认响应
*/
extern BYTE RfEtcard_Cancel_End(pEtcardInParam_t  pInParam, BYTE *Responce, BYTE ResLen);
extern BYTE Etcard_Cancel_End(pEtcardInParam_t  pInParam, BYTE *Responce, BYTE ResLen);


/*
	获取版本号,格式如 V0.01_20090410_01
*/
extern void Etcard_GetVersion(BYTE *pVersion);

/*
	组织充值、冲正交易记录
	在充值确认、冲正确认后调用!
	从pOutParam的pTopupCalData中获取记录
	参数说明:
	bTopup - 充值、冲正说明，充值:TRUE, 冲正:FALSE;
	
	Status - 状态说明,	'1'："未写卡"-包括以下四种情形：A.充值终端发送充值申请或冲正申请失败；B.充值终端发送充值申请或冲正申请后未收到响应数据；C. 充值终端发送充值申请或冲正申请后收到的申请响应码提示错误；D.充值终端发送充值申请或冲正申请后收到的申请响应码提示成功，但由于充值程序异常等原因未进行下一步的写卡操作；
						'2'："写卡失败"- 指经过写卡操作后，未对储值卡上的电子钱包金额进行成功的修改；
						'3'："写卡异常"- 指经过写卡操作后，储值卡电子钱包的金额出现异常；
						'4'："待确认"- 指经过写卡操作后，储值卡上的电子钱包金额已经被成功修改，但由于线路故障等原因导致发送"充值结果"数据包时失败；
						'5'："已确认"- 指储值卡上的电子钱包金额已经被成功修改且发送"充值结果"数据包成功或发送"充值补录半条"数据包成功；

	rflag - 补录标志，	'Y'：发送"充值补录半条"数据包成功；
						'N'：其他情况

	cflag - 记帐标志，	'Y'：记帐(银行向我司划拨充值款项的依据)
						'N'：不记帐

*/
extern void Buidup_TopUp_Cancel_Record(BOOLEAN bTopup, BYTE Status, BYTE rflag, BYTE cflag, 
										pEtcardInParam_t  pInParam, pEtcardOutParam_t pOutParam);


/*
	获取后台返回的错误代码定义，用于无法解析错误时，查看具体代码
	pErrCode - 2byte错误代码
	注意:只有在错误出现时，才可以获得有效的错误代码!
*/
extern void GetBackgroundErrCode(BYTE	*pErrCode);


///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// 联机消费函数

// psam 初始化
extern BYTE ETC_OnlineConsume_InitSam(pETC_OnlineConsume_InParam_t  pInParam, 
                                            pETC_OnlineConsume_OutParam_t pOutParam);


// 签到申请
// "签到申请"由终端向中心发起。终端首先对PSAM卡发出命令，由PSAM卡产生一个随机数RAM1。终端发送"签到申请"报文
extern BYTE ETC_OnlineConsume_SignIn_Init(pETC_OnlineConsume_InParam_t  pInParam, BYTE *pMessage, BYTE *pMessLen);


// 签到认证请求
extern BYTE ETC_OnlineConsume_SignIn_Auth(pETC_OnlineConsume_InParam_t  pInParam, BYTE SignInType /* 0x00 - 初次签到，0x01 - 重签到*/, 
                    							BYTE *Responce, BYTE ResLen, 
                    							BYTE *pMessage, BYTE *pMessLen);

// 签到认证响应
extern BYTE ETC_OnlineConsume_SignIn_Auth_End(BYTE *Responce, BYTE ResLen, pETC_SignIn_TermCtrlInfo_t pSignInTermInfo);

// 签退申请
extern BYTE ETC_OnlineConsume_SignIn_Out(pETC_OnlineConsume_InParam_t  pInParam, 
							BYTE *pMessage, BYTE *pMessLen);

// 检查签退响应报文
extern BYTE ETC_OnlineConsume_SignIn_Out_End(pETC_OnlineConsume_InParam_t pInParam, BYTE *Responce, BYTE ResLen);


// 储值卡消费申请
// 可查询余额与只做寻卡
extern BYTE    ETC_OnlineConsume_Apply_PaidCard(pETC_OnlineConsume_InParam_t pInParam, 
                                                         pETC_OnlineConsume_OutParam_t pOutParam,
                                                         BYTE *pMessage, BYTE *pMessLen);


// 储值卡流水上传
//extern BYTE    ETC_OnlineConsume_UpdateMessage_PaidCard(pETC_OnlineConsume_InParam_t pInParam, 
//                                                                      pETC_OnlineConsume_OutParam_t pOutParam,
//                                                                      BYTE *Responce, BYTE ResLen, 
//                                                                      BYTE *pMessage, BYTE *pMessLen);

extern BYTE    ETC_OnlineConsume_UpdateMessage_PaidCard(pETC_OnlineConsume_InParam_t pInParam, 
                                                                      pETC_OnlineConsume_OutParam_t pOutParam,
                                                                      pETC_RecordInfo_t pRecordInfo,
                                                                      BYTE *Responce, BYTE ResLen, 
                                                                      BYTE *pMessage, BYTE *pMessLen);


// 检查流水上传响应报文
extern BYTE    ETC_OnlineConsume_End_PaidCard(pETC_OnlineConsume_InParam_t pInParam, /* 入口参数只使用到了socket */
                                                        BYTE *Responce, 
                                                        BYTE ResLen);


// 终端日结申请
extern BYTE    ETC_OnlineConsume_Day_Settlement(pETC_OnlineConsume_InParam_t pInParam, 
                                                            BYTE *pMessage, BYTE *pMessLen);

// 获取终端日结数据
extern BYTE    ETC_OnlineConsume_Day_Settlement_End(pETC_OnlineConsume_InParam_t pInParam, pETC_SettleInfo_t pInfo,
                                                        BYTE *Responce, 
                                                        BYTE ResLen);
#endif
#endif
