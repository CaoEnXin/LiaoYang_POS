/****************************************************************************
NAME
    LocalFtp.h - ����FTPӦ��ģ��

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    howard.joe     2011.10.31      - created
****************************************************************************/
#ifndef _LOCALFTP_H
#define _LOCALFTP_H

typedef struct {
	char				szHostip[32]; // ������IP��ַ
	char				szPort[8]; // �������˿�
	char				szUser[32];  // ��¼ʱ�û�����������¼ʱ���ó�"anonymous"
	char				szPwd[32]; // ��¼ʱ���룬������¼ʱ�������óɳ���С��31���ַ���
	char				szRemotefile[256]; // ���������ļ�·��
	char				szLocalfile[256]; // ���ص��ļ�·������Ҫע�����pos���ļ�����󳤶�Ϊ16���ֽ�
} FTP_PARA;

/*
typedef struct {
	UINT32 iRetCode;	//������
	char szChnMsg[40 + 1];	// ������Ϣ
}FTP_ERR_INFO;
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// ��Ҫ�����������ĺ���ָ�봫��
INT32 ftpDemo(INT32 (* PreDial)(void)); // ����ftp�Ļ�������

/****************************************************
��������: �ӷ������ϻ�ȡ�ļ�
�������: 
	FTP_PARA * FtpPara ----> ftp�����ṹ��
	INT32 (* PreDial)(void) ----> ����ָ�룬��������ΪԤ���ţ���Ҫ�Լ�ȥʵ��
��������:
	0: �ɹ�
	��0: ʧ��
*****************************************************/
INT32 NewFtpGetFile(FTP_PARA * FtpPara, INT32 (* PreDial)(void));

/*****************************************************
��������: ���ļ����͵���������
�������: 
	FTP_PARA * FtpPara ----> ftp�����ṹ��
	INT32 (* PreDial)(void) ----> ����ָ�룬��������ΪԤ���ţ���Ҫ�Լ�ȥʵ��
��������:
	0: �ɹ�
	��0: ʧ��
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

