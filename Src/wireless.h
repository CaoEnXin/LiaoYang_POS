/****************************************************************************
实现CDMA/GPRS 模块的基本操作库
****************************************************************************/
#ifndef __NEWPOS_WIRELESS_H
#define __NEWPOS_WIRELESS_H

#include "posapi.h"

#define WIRELESS_TIMER_ID 4

#define WIRELESS_TIME_OUT 1	

#define  TRANS_COMMTYPE        0x01
#define  DOWNLAOD_COMMTYPE     0x02

#ifdef __cplusplus
extern "C" {
#endif 

uint8_t  SetWirelessIPPort(void);
uint8_t  SetWirelessParam(void);
int  PreDial(void);
int    CheckPreDial(int iPreDialRet);
int    SetCurCommParam(int iMode);
int    SelectAndSetWIFIAP(void);
int    SetWIFIParam(int iMode);
int    SetWiFiLoaclIPCfg(void);
int    SetTcpIpParam(void);
int    SetRS232Param(void);
int    SetAcqTransTelNo(int iMode);
int    SetModemParam(void);
int SetIpAddr(uint8_t x, uint8_t y, char *ipAddr);

void   NacTxd(uint8_t *psTxdData, uint32_t uiDataLen);

#ifdef __cplusplus
}
#endif 

#endif 


