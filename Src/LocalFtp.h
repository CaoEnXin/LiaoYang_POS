/****************************************************************************
NAME
    LocalFtp.h - 定义FTP应用模块

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    howard.joe     2011.10.31      - created
****************************************************************************/
#ifndef _LOCALFTP_H
#define _LOCALFTP_H

typedef struct {
	char				szHostip[32]; // 服务器IP地址
	char				szPort[8]; // 服务器端口
	char				szUser[32];  // 登录时用户名，匿名登录时设置成"anonymous"
	char				szPwd[32]; // 登录时密码，匿名登录时，可设置成长度小于31的字符串
	char				szRemotefile[256]; // 服务器的文件路径
	char				szLocalfile[256]; // 本地的文件路径，需要注意的是pos的文件名最大长度为16个字节
} FTP_PARA;

/*
typedef struct {
	UINT32 iRetCode;	//返回码
	char szChnMsg[40 + 1];	// 中文信息
}FTP_ERR_INFO;
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// 需要把连接主机的函数指针传入
INT32 ftpDemo(INT32 (* PreDial)(void)); // 测试ftp的基本功能

/****************************************************
函数功能: 从服务器上获取文件
输入参数: 
	FTP_PARA * FtpPara ----> ftp参数结构体
	INT32 (* PreDial)(void) ----> 函数指针，函数功能为预拨号，需要自己去实现
函数返回:
	0: 成功
	非0: 失败
*****************************************************/
INT32 NewFtpGetFile(FTP_PARA * FtpPara, INT32 (* PreDial)(void));

/*****************************************************
函数功能: 将文件上送到服务器上
输入参数: 
	FTP_PARA * FtpPara ----> ftp参数结构体
	INT32 (* PreDial)(void) ----> 函数指针，函数功能为预拨号，需要自己去实现
函数返回:
	0: 成功
	非0: 失败
******************************************************/
INT32 NewFtpPutFile(FTP_PARA * FtpPara, INT32 (* PreDial)(void));

INT32 NewFtpQuit(void);
 int ftp_get_data(void *arg, void *data, loff_t offset, int count);
 INT32 NewFtpPut(FTP_PARA * FtpPara);
 int ftp_store_data(void *arg, const void *data, int count);
 INT32 NewFtpDownLoad(FTP_PARA * FtpPara, int iFileSize);
 INT32 NewFtpGetSize(FTP_PARA * FtpPara, int * ipCount);
 INT32 NewFtpLogon(FTP_PARA * FtpPara);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // _LOCALFTP_H
//end of file

