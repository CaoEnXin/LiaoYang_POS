/****************************************************************************
NAME
    Rs232.h - 定义串口收发通讯处理模块

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    pyming     2010.12.29      - created
****************************************************************************/
//增加串口函数，原先在Commlib.c里面定义的如果与本文件有冲突的将屏蔽

#ifndef _RS232LIB_H
#define _RS232LIB_H

#define  STR_RS232_PARAM    "115200,8,n,1"

// for RS232 communication
#ifndef  STX
#define  STX             0x02
#define  ETX             0x03
#endif

#define  RS232_TX_PURGE    0x0001    // 发送缓冲区
#define  RS232_RX_PURGE    0x0002    // 接收缓冲区

#define  ERR_RS232_OK              0   // 成功
#define  ERR_RS232_FAILURE        -1   // 串口操作失败
#define  ERR_RS232_RX_DATA        -2   // 接收数据错误
#define  ERR_RS232_RX_TIMEOUT     -3   // 接收数据超时
#define  ERR_RS232_RX_LENGTH      -4   // 接收数据长度超限
#define  ERR_RS232_RX_CHECKSUM    -5   // 校验数据出错

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//   打开串口
int  Rs232Open(int iportNum, char *pszPortPara);

//   关闭串口
int  Rs232Close(void);

//   检测串口缓冲区是否有数据,uiFlag 表示发送还是接收缓冲区
int  Rs232CheckPurge(unsigned int uiFlag);

//   检测串口缓冲区是否有数据, uiFlag 表示发送还是接收缓冲区
int  Rs232ResetPurge(unsigned int uiFlag);

//   串口发送数据  
int  MainPosRS232Txd(unsigned char*psTxdData, unsigned int uiDataLen, unsigned int uiTimeOut);

//   串口接收数据
//因接口与母POS PC端的接口较特殊，所以该函数不通用
int  MainPosRS232Rxd(unsigned char *psRxdData, unsigned int uiExpLen, unsigned int uiTimeOut, unsigned int *puiOutLen);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _RS232_H */

