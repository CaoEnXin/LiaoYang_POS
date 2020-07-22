
#ifndef _GLBVAR_H
#define _GLBVAR_H

#include "posapi.h"
#include "terstruc.h"
#include "posmacro.h"
#include "emvapi.h"
#include "MathLib.h"
#include "PosLib.h"
#include "Display.h"
#include "Commlib.h"
#include "wireless.h"
#include "tmslib.h"
#include "ChkOption.h"
#include "Rs232lib.h"
#include "New8110_ICC.h"


extern struct NEWPOS_COM_STRUCT PosCom;
extern struct NEWPOS_COM_STRUCT BackPosCom;
extern struct _total_strc stTotal; 
extern struct _total_m1Enrich_strc stM1EnrichTotal;

extern struct _limit_total_strc stLimitTotal; 
extern struct _oper_structure operStrc[MAX_OPER_NUM];
extern struct _TRANS_CTRL_PARAM stTransCtrl; 
extern struct _NEWPOS_PARAM_STRC stPosParam;    
extern struct _EPP_CONFIGURE_ *glbEpp;
extern struct NEWPOS_YKTENRICH_LOG_STRUCT  gstYktEnrichLog;

extern int    icc_fd;
extern int    iPsam_fd; 
extern int    imif_fd;

extern uint16_t usSendPacketLen;  
extern uint16_t usRecvPacketLen;
extern uint8_t  sSendPacketBuff[MAX_PACKET_LEN];	
extern uint8_t  sRecvPacketBuff[MAX_PACKET_LEN];	

extern struct temp_global_var_strc stTemp;

extern struct emv_core_init tEMVCoreInit;
extern struct terminal_mck_configure tConfig;
extern struct emv_callback_func tEMVCallBackFunc;

//extern struct terminal_aid_info gTAidInfo;

extern DIAL_STAT dial_stat;	/*����ͳ��*/
extern ACQUIRERIDNAME sAcqList[MAX_ACQUIRER_NUM];
extern struct errinfostru sRetErrInfo[ERR_INFO_NUM];
extern NEWPOS_8583PACKET_STRC TrTable[TABLES_LEN];


extern TermAidList glTermAidList[MAX_APP_NUM];
extern int       glTermAidNum;

//extern int       iAllAppNum;

//extern APPINFO	otherAppInfo[APP_NUM];     //���10��Ӧ�ã������ˣ��Ͳ�ȡ
//extern APPMENU  AppMenu;				   //���ڲ˵���ʾ

extern ST_POS_CONFIG_INFO    gstPosCapability;      // �ն����õ�����
extern ST_COMM_CONFIG		 gstCurCommCfg;		 // ��ǰͨѶ���ò���
extern  char                 gszParamFileName[50];      //  �����ļ���

extern  unsigned char Bmp_Show_in_PIN[];
extern  unsigned char Bmp_Show_Balance[];
extern  unsigned char Bmp_Show_CurrName[];
extern  unsigned char Bmp_Show_AmtName[];
extern  unsigned char Bmp_Show_LytBalance[];


void  lcdFlip(void);

#endif
