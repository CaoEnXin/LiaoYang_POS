/*****************************************************************************************************
* m1_tranproc.h
*
*auth: howard.joe
*****************************************************************************************************/

#ifndef M1_TRANPROC_H
#define M1_TRANPROC_H
#define ISAM_SLOT 0x11
#define  M1_SWIPERET_1			0x01
#define  M1_SWIPERET_2			0x02
#define  M1_SWIPERET_3			0x03
#define  M1_SWIPERET_4			0x04
#define  M1_SWIPERET_5			0x05
#define  M1_SWIPERET_6			0x06
#define  M1_SWIPERET_7	        0x07
#define  M1_SWIPERET_8	        0x08


#define  M1_SWIPE_AGAIN		0x80


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
   M1卡的充值
*/
typedef struct NEWPOS_YKTENRICH_LOG_STRUCT { 
//公共信息
	UINT32 uiIndex;//保存记录的位置
	UINT32 uiStatus;//是否结算。没有结算0，结算为1；
	//UINT8  uVersion;
	UINT32 uMode;
	ulong  sIsamKey;
	ulong  sPosSN;
	ulong  ulPosTermId;//终端号
	ulong  ulPosUnitId;//商户号
	ulong  ulNowTellNo;//hex当前操作员号
	ulong  ulTermSerialNo;//psam卡流水号
	ulong  lSaleCumulativeBalance;
	//---
	int lEnrichTraceNo;  //充值流水号
	ulong lEnrichCumulativeBalance;//充值累积金额

	//----
	UINT8  uSaleStatus;//写卡状态，0x00 写卡成功，0x01未写卡(写卡失败)，0x08写卡异常、0x02黑名单锁卡
	//UINT8  uCardType;//0x01 cpu 0x02.m1
	UINT8  uSaleMode;//0非定值，1定值，10快捷键
//交易信息
	UINT8 sCardSerialNo[16+1];//1.	卡号（16位)  卡应用序列号 
	ulong ulPOSTraceNo;//2.	充值次数,钱包计数器
	//UINT8 s
	UINT8 mac2[8];
	UINT8 sCSN[10+1];//3.	卡芯片号（10位）。
	UINT8 uCardtype;  // 卡类型
	UINT8 sTranDateTime[7];//bcd交易时间20120729115400   充值时间
	UINT8 sCompanyNo[20];//6.	公司编号
	UINT8 sPsamId[10+1];//acs终端机具号 ISAM  卡号
	UINT8 sNowTellNo[4];//BCD8.	系统设置的操作员卡号（4位）
	UINT8 sTellNo[4];//pos登录时的操作员
	ulong ulTransAmt;//hex  9.	充值金额（单位分），
	ulong ulPaidupAmt;//10.	实收金额（单位分），
	ulong ulNowBalaceAmt;//hex11.	充值后余额（单位分），
	UINT8 sLogDateTime[7];//bcd交易时间20120729115400   日志上传
	//UINT8 uOldBalaceAmt[4];//
	UINT8   pointofsaleNo[6+1]; //销售点号
//---------------
	ulong  ulOldBalaceAmt;//he
	ulong  ulMaxBalaceAmt;//hex
//-----------------------
	UINT8  uClipCard;//子卡类
	//ulong  ulPOSTraceNo; //hex
	ulong  ulTranCardNo;//hex
	ulong  ulSubsCardNo;//hex补助卡号
	ulong  ulOfflineTracedNo;//hex

	ulong  ulTranSubsAmt;//补扣金额hex
	ulong  ulTranTipsAmt;//打火费hex
	
	//UINT8  sCardSerialNo[8];//bcd卡序列号
	UINT32 iCardNo;//客户号
	UINT8  sExpDate[3];//有效期
	
	UINT8  sTac[4];//hex
	
	//int OnlineDealSerial;
	//------------
	UINT8  sCPUCSN[4];
	//UINT8 sOnlineTraceNo[2];//电子钱包联机交易序列号
//	UINT8 SCardSerialNo[8];//卡应用序列号 
	uint8_t szLSDepartmentNo[2*2+1] ;      //部门编号
	uint8_t szLSM1TellNo[4*2+1];//乐山操作员
	uint8_t szLSPosId[12+1];  //终端编号

}YKTENRICH_LOG;

#define YKTENRICH_LOG_SIZE  sizeof(struct NEWPOS_YKTENRICH_LOG_STRUCT)

INT32 M1_Enrich(void);
UINT8 GetBlockVerifybyte(UINT8 *sBlockData );
UINT8  MakeWalletBlock(ulong  ulOldBalaceAmt,UINT8 *sBlockOut);
UINT8 VerifyStartUse(char flag);
int CheckData_Sectors2(char *Sectorsdata,UINT8 IsUpdateData,UINT8 * Uid,char * cKeyB);  //校验完后，可能会修改数据
UINT8 ResetPsam(void);
UINT8 GetSecPW(UINT8*  pCsn, UINT8*  pTrace, UINT8* sAuthKey, UINT8* pCityCode, UINT8* PSectorOper,UINT8* sSecNo, int iKeyNo, UINT8* SecPW);
int GetPsamId(UINT8 *pzPasmId);//10字节
int ChkContactlessCardPad(INT32 *CardInfoLen, UINT8 *CardInfo);
int GetAllDataFromM1Card(void);
int M1_IsCardInfoLegal(void);
int  M1_RchAmtProcess(void);//UINT32 uiTransAmt
int   M1_BlocktoBlockValue(int DesBlock, int SourceBlock, char* cKey, int iKeyType,UINT8 *UID);
int M1_RchSwipCardAgain(void);
//int GetMAC(uint8_t * inbuf, int len,uint8_t * key, uint8_t * outbuf);
UINT8 EnrichAuthenticate(void);
uint8_t SendTAC(void);
uint8_t CPUEnrichReverse(void);





#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // M1_TRANPROC_H

