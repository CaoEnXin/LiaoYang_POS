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
   M1���ĳ�ֵ
*/
typedef struct NEWPOS_YKTENRICH_LOG_STRUCT { 
//������Ϣ
	UINT32 uiIndex;//�����¼��λ��
	UINT32 uiStatus;//�Ƿ���㡣û�н���0������Ϊ1��
	//UINT8  uVersion;
	UINT32 uMode;
	ulong  sIsamKey;
	ulong  sPosSN;
	ulong  ulPosTermId;//�ն˺�
	ulong  ulPosUnitId;//�̻���
	ulong  ulNowTellNo;//hex��ǰ����Ա��
	ulong  ulTermSerialNo;//psam����ˮ��
	ulong  lSaleCumulativeBalance;
	//---
	int lEnrichTraceNo;  //��ֵ��ˮ��
	ulong lEnrichCumulativeBalance;//��ֵ�ۻ����

	//----
	UINT8  uSaleStatus;//д��״̬��0x00 д���ɹ���0x01δд��(д��ʧ��)��0x08д���쳣��0x02����������
	//UINT8  uCardType;//0x01 cpu 0x02.m1
	UINT8  uSaleMode;//0�Ƕ�ֵ��1��ֵ��10��ݼ�
//������Ϣ
	UINT8 sCardSerialNo[16+1];//1.	���ţ�16λ)  ��Ӧ�����к� 
	ulong ulPOSTraceNo;//2.	��ֵ����,Ǯ��������
	//UINT8 s
	UINT8 mac2[8];
	UINT8 sCSN[10+1];//3.	��оƬ�ţ�10λ����
	UINT8 uCardtype;  // ������
	UINT8 sTranDateTime[7];//bcd����ʱ��20120729115400   ��ֵʱ��
	UINT8 sCompanyNo[20];//6.	��˾���
	UINT8 sPsamId[10+1];//acs�ն˻��ߺ� ISAM  ����
	UINT8 sNowTellNo[4];//BCD8.	ϵͳ���õĲ���Ա���ţ�4λ��
	UINT8 sTellNo[4];//pos��¼ʱ�Ĳ���Ա
	ulong ulTransAmt;//hex  9.	��ֵ����λ�֣���
	ulong ulPaidupAmt;//10.	ʵ�ս���λ�֣���
	ulong ulNowBalaceAmt;//hex11.	��ֵ������λ�֣���
	UINT8 sLogDateTime[7];//bcd����ʱ��20120729115400   ��־�ϴ�
	//UINT8 uOldBalaceAmt[4];//
	UINT8   pointofsaleNo[6+1]; //���۵��
//---------------
	ulong  ulOldBalaceAmt;//he
	ulong  ulMaxBalaceAmt;//hex
//-----------------------
	UINT8  uClipCard;//�ӿ���
	//ulong  ulPOSTraceNo; //hex
	ulong  ulTranCardNo;//hex
	ulong  ulSubsCardNo;//hex��������
	ulong  ulOfflineTracedNo;//hex

	ulong  ulTranSubsAmt;//���۽��hex
	ulong  ulTranTipsAmt;//����hex
	
	//UINT8  sCardSerialNo[8];//bcd�����к�
	UINT32 iCardNo;//�ͻ���
	UINT8  sExpDate[3];//��Ч��
	
	UINT8  sTac[4];//hex
	
	//int OnlineDealSerial;
	//------------
	UINT8  sCPUCSN[4];
	//UINT8 sOnlineTraceNo[2];//����Ǯ�������������к�
//	UINT8 SCardSerialNo[8];//��Ӧ�����к� 
	uint8_t szLSDepartmentNo[2*2+1] ;      //���ű��
	uint8_t szLSM1TellNo[4*2+1];//��ɽ����Ա
	uint8_t szLSPosId[12+1];  //�ն˱��

}YKTENRICH_LOG;

#define YKTENRICH_LOG_SIZE  sizeof(struct NEWPOS_YKTENRICH_LOG_STRUCT)

INT32 M1_Enrich(void);
UINT8 GetBlockVerifybyte(UINT8 *sBlockData );
UINT8  MakeWalletBlock(ulong  ulOldBalaceAmt,UINT8 *sBlockOut);
UINT8 VerifyStartUse(char flag);
int CheckData_Sectors2(char *Sectorsdata,UINT8 IsUpdateData,UINT8 * Uid,char * cKeyB);  //У����󣬿��ܻ��޸�����
UINT8 ResetPsam(void);
UINT8 GetSecPW(UINT8*  pCsn, UINT8*  pTrace, UINT8* sAuthKey, UINT8* pCityCode, UINT8* PSectorOper,UINT8* sSecNo, int iKeyNo, UINT8* SecPW);
int GetPsamId(UINT8 *pzPasmId);//10�ֽ�
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

