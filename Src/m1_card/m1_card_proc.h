/**************************************************************************************************
* M1_card_proc.h
*
* auth:howard.joe
*
**************************************************************************************************/

#ifndef M1_CARD_PROC_H
#define M1_CARD_PROC_H


typedef struct _tagM1InternalMember {
	UINT8 ucMifOpenFlag;  // 1 代表已经初始化模块  0 代表未初始化模块
}M1_INTERNAL_MEMBER;


typedef struct _tagM1DispErrList {
	int iErrCode;       // 错误码
	char szInfo1[17];  // 提示信息 一行不超过16字节
}M1_DISP_ERRLIST;

#define M1_ERR_OTHER         -1 // 其他错误
#define M1_ERR_MULTICARD      1 // 多张卡
#define M1_ERR_RETRY          2 // 寻卡失败，请重试
#define M1_ERR_USERCANCEL     3 // 用户主动取消
#define  M1_READ_CARD       0x01
#define  M1_WRITE_CARD      0x02
#define  M1_DECREMENT       0x03
#define  M1_INCREMENT      0x04


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
不重复打开，所以通过一个内部的静态变量来控制
注意: 只受理M1卡的时候，才使用此函数。如果受理多种非接卡，那么请使用过自定义的函数来控制模块的初始化
*/
void M1_mifOpen(void);


/*
不重复关闭，所以通过一个内部的静态变量来控制
注意: 只受理M1卡的时候，才使用此函数。如果受理多种非接卡，那么请使用过自定义的函数来控制模块的关闭
*/
void M1_mifClose(void);


/*
关闭载波对感应区内所有卡片进行复位，然后等待dur_time毫秒后再重新打开载波信号
当dur_time为0时，则一直关闭载波，直到下一次重新调用mifOpen()
*/
void M1_mifResetRF(UINT32 dur_time);


/*
询卡
注意: 只受理M1卡的时候，才使用此函数。如果受理多种非接卡，那么请使用过自定义的函数来完成询卡操作
*/
INT32 M1_WaitCard(INT32 *CardInfoLen, UINT8 *CardInfo);


/*
清除2-7行
显示M1卡处理函数返回的错误码
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
