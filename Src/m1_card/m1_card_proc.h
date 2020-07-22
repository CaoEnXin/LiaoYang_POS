/**************************************************************************************************
* M1_card_proc.h
*
* auth:howard.joe
*
**************************************************************************************************/

#ifndef M1_CARD_PROC_H
#define M1_CARD_PROC_H


typedef struct _tagM1InternalMember {
	UINT8 ucMifOpenFlag;  // 1 �����Ѿ���ʼ��ģ��  0 ����δ��ʼ��ģ��
}M1_INTERNAL_MEMBER;


typedef struct _tagM1DispErrList {
	int iErrCode;       // ������
	char szInfo1[17];  // ��ʾ��Ϣ һ�в�����16�ֽ�
}M1_DISP_ERRLIST;

#define M1_ERR_OTHER         -1 // ��������
#define M1_ERR_MULTICARD      1 // ���ſ�
#define M1_ERR_RETRY          2 // Ѱ��ʧ�ܣ�������
#define M1_ERR_USERCANCEL     3 // �û�����ȡ��
#define  M1_READ_CARD       0x01
#define  M1_WRITE_CARD      0x02
#define  M1_DECREMENT       0x03
#define  M1_INCREMENT      0x04


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
���ظ��򿪣�����ͨ��һ���ڲ��ľ�̬����������
ע��: ֻ����M1����ʱ�򣬲�ʹ�ô˺��������������ַǽӿ�����ô��ʹ�ù��Զ���ĺ���������ģ��ĳ�ʼ��
*/
void M1_mifOpen(void);


/*
���ظ��رգ�����ͨ��һ���ڲ��ľ�̬����������
ע��: ֻ����M1����ʱ�򣬲�ʹ�ô˺��������������ַǽӿ�����ô��ʹ�ù��Զ���ĺ���������ģ��Ĺر�
*/
void M1_mifClose(void);


/*
�ر��ز��Ը�Ӧ�������п�Ƭ���и�λ��Ȼ��ȴ�dur_time����������´��ز��ź�
��dur_timeΪ0ʱ����һֱ�ر��ز���ֱ����һ�����µ���mifOpen()
*/
void M1_mifResetRF(UINT32 dur_time);


/*
ѯ��
ע��: ֻ����M1����ʱ�򣬲�ʹ�ô˺��������������ַǽӿ�����ô��ʹ�ù��Զ���ĺ��������ѯ������
*/
INT32 M1_WaitCard(INT32 *CardInfoLen, UINT8 *CardInfo);


/*
���2-7��
��ʾM1�����������صĴ�����
*/
void M1_DispErrInfo(int iErrCode);
ulong PubChar2LongEx(char *psString, int iStrLen, ulong *pulOut);
UINT8  M1_ReadSector(UINT32 SectorNo, char* cKey, int iKeyType,char* pzOutData, UINT8 *Uid);
UINT8  ProcContEventWrite(UINT32 BlockNo, char* cKey, int iKeyType, char* pzTranferData, int iMode, UINT8 *Uid);
UINT8  M1_WriteBlock(UINT32 BlockNo, char* cKey, int iKeyType,char* pzInData, UINT8 *Uid);//
UINT8  M1_Decrement(UINT32 BlockNo, char* cKey, int iKeyType,char* pzInData, UINT8 *Uid);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // M1_CARD_PROC_H
