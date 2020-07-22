#include "Glbvar.h"
#include "posapi.h"
#include "ftp.h"
#include "PosLib.h"
#include "NewTime.h"
#include "LocalFtp.h"

/********************** Internal macros declaration ************************/
/********************** Internal structure declaration *********************/
int ftp_debug(const char *format, ...)
{
;//NacTxd(format,strlen(format));
return 0;
}
typedef  struct {
	char		filename[256];
	int 		fd;
	int 		size;
	int 		offset;
	time_t		ts;
}store_t;

/********************** Internal functions declaration *********************/
//static INT32 NewFtpQuit(void);
//static int ftp_get_data(void *arg, void *data, loff_t offset, int count);
//static INT32 NewFtpPut(FTP_PARA * FtpPara);
//static int ftp_store_data(void *arg, const void *data, int count);
//static INT32 NewFtpDownLoad(FTP_PARA * FtpPara, int iFileSize);
//static INT32 NewFtpGetSize(FTP_PARA * FtpPara, int * ipCount);
//static INT32 NewFtpLogon(FTP_PARA * FtpPara);
/********************** Internal variables declaration *********************/
static ftp_host_info_t theFtp;
/*
static FTP_ERR_INFO FtpErrInfo[] =   // �������д�����﹩�ο�ftp������Ķ��壬�ݲ�ʵ����pos����ʾ������Ϣ
{
	{110, "�������Ӧ��"},
	{120, "������nnn������׼����"},
	{125, "���������Ѵ򿪣�׼������"},
	{150, "�ļ�״̬���ã�����������"},
	{200, "����ɹ�"},
	{202, "����δʵ��"},
	{211, "ϵͳ״̬��ϵͳ������Ӧ"},
	{212, "Ŀ¼״̬"},
	{213, "�ļ�״̬"},
	{214, "������Ϣ����Ϣ���������û�����"},
	{215, "����ϵͳ����"},
	{220, "�����û�����׼����"},
	{221, "����رտ������ӣ������˳���¼"},
	{225, "�������Ӵ򿪣��޴������ڽ���"},
	{226, "�ر��������ӣ�������ļ������ɹ�"},
	{227, "���뱻��ģʽ"},
	{230, "�û���¼"},
	{250, "������ļ��������"},
	{257, "����"PATHNAME""},
	{331, "�û�����ȷ����Ҫ����"},
	{332, "��¼ʱ��Ҫ�ʻ���Ϣ"},
	{350, "������ļ�������Ҫ��һ������"},
	{421, "�����ṩ���񣬹رտ�������"},
	{425, "���ܴ���������"},
	{426, "�ر����ӣ���ֹ����"},
	{450, "������ļ�����δִ��"},
	{451, "��ֹ����Ĳ������б��ش���"},
	{452, "δִ������Ĳ�����ϵͳ�洢�ռ䲻��"},
	{500, "��ʽ���������ʶ��"},
	{501, "�����﷨����"},
	{502, "����δʵ��"},
	{503, "����˳�����"},
	{504, "�˲����µ������δʵ��"},
	{530, "δ��¼"},
	{532, "�洢�ļ���Ҫ�ʻ���Ϣ"},
	{550, "δִ������Ĳ���"},
	{551, "���������ֹ��ҳ����δ֪"},
	{552, "������ļ�������ֹ���洢�������"},
	{553, "δִ������Ĳ������ļ������Ϸ�"},
	{0, ""}
};
*/
/********************** external reference declaration *********************/

INT32 ftpDemo(INT32 (* PreDial)(void)) // �ú�����������ftp�Ļ������ܣ�����Ԥ���ź���PreDial��Ҫ�Լ�ʵ��
{
	int iKey;
	FTP_PARA stFtpPara;

	if (PreDial == NULL)
	{
		lcdCls();
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "ȱ�ٻص�����");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "PreDial");
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}

	lcdCls();
	lcdDisplay(0, 0, DISP_CFONT | DISP_REVERSE | DISP_MEDIACY, "FTP���ܲ���");
	lcdDisplay(0, 2, DISP_CFONT, "1. ����");
	lcdDisplay(0, 4, DISP_CFONT, "2. FTP����");
	lcdDisplay(0, 6, DISP_CFONT, "3. FTP����");
	kbFlush();
	while(1)
	{
		iKey = kbGetKey();
		if (iKey == KEY1)
		{
			sysReset();
			return 1;
		}
		if (iKey == KEY2)
		{
			{
				strcpy(stFtpPara.szUser, "anonymous"); // ������¼���û�����������Ϊanonymous
				strcpy(stFtpPara.szPwd, "tc111111"); // ����Ϊ�����ַ�����������С��31���ַ�
				strcpy(stFtpPara.szHostip, "123.119.184.6");
				strcpy(stFtpPara.szPort, "21");
				strcpy(stFtpPara.szRemotefile, "readme.txt");
				strcpy(stFtpPara.szLocalfile, "demo.flg");
			}
			return (NewFtpGetFile(&stFtpPara, PreDial));
		}
		if (iKey == KEY3)
		{
			{
								strcpy(stFtpPara.szUser, "anonymous"); // ������¼���û�����������Ϊanonymous
				strcpy(stFtpPara.szPwd, "tc111111"); // ����Ϊ�����ַ�����������С��31���ַ�
				strcpy(stFtpPara.szHostip, "123.119.184.6");
				strcpy(stFtpPara.szPort, "21");
				strcpy(stFtpPara.szRemotefile, "readme1.txt");
				strcpy(stFtpPara.szLocalfile, "demo.flg");
			}
			return(NewFtpPutFile(&stFtpPara, PreDial));
		}
	}
}

INT32 NewFtpPutFile(FTP_PARA * FtpPara, INT32 (* PreDial)(void))
{
	INT32 iRet;
	
	if (FtpPara == NULL || PreDial == NULL ||
		strlen(FtpPara->szHostip) < 7 || strlen(FtpPara->szPort) < 0 ||
		strlen(FtpPara->szUser) == 0 || strlen(FtpPara->szPwd) == 0 ||
		strlen(FtpPara->szRemotefile) == 0 || strlen(FtpPara->szLocalfile) == 0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT | DISP_MEDIACY, "ftp�������ó���");
		PubBeepErr();
		PubWaitKey(5);
		return 1; 
	}

	if(fileExist(FtpPara->szLocalfile) < 0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "%s", FtpPara->szLocalfile);
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "�ļ�������");
		PubBeepErr();
		PubWaitKey(5);
		return 1; 
	}

	// ����
	iRet = PreDial();
	if( iRet!=0 )
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "����ʧ��");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "DialErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}

	// ��¼ftp
	iRet = NewFtpLogon(FtpPara);
	if (iRet != 0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "FTP��¼ʧ��");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "LogErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}

	//�����ļ�
	iRet = NewFtpPut(FtpPara);
	
	// �ǳ�������
	NewFtpQuit();
	
	return iRet;
}

// ʵ�ֱ����ftp�����ص��ļ��ķ���
int ftp_get_data(void *arg, void *data, loff_t offset, int count)
{
	int iRet;
	store_t *store = (store_t *)arg;
	time_t t3;
	struct tm t;
	int fd = store->fd;
	time_t t2 = SYS_TIME(NULL);
	
	if (kbhit() == YES) {
		if (kbGetKey() == KEY_CANCEL) {
			return KEY_CANCEL;
		}
	}

	t3 = t2 - store->ts;
	t = *SYS_GMTIME(&t3);
	fileSeek(fd, offset, SEEK_SET);
	if ((iRet = fileRead(fd, data, count)) != count) 
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "O=%ld N=%d", offset, count);
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "ReadErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return iRet;
	}

	store->offset = offset + count;

	lcdClrLine(4, 7);
	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "%d%% %d/%d", store->offset * 100 / store->size, store->offset, store->size);
	lcdDisplay(0, 6, DISP_CFONT|DISP_MEDIACY, "%02d:%02d:%02d %4dBps", t.tm_hour, t.tm_min, t.tm_sec, (int)(store->offset / t3));
	return 0;
}

INT32 NewFtpPut(FTP_PARA * FtpPara)
{
	int iRet;
	store_t store;
	
	strcpy(store.filename, FtpPara->szLocalfile);
	store.offset = 0;
	store.ts = SYS_TIME(NULL);
	store.fd = fileOpen((uint8_t *)store.filename, O_RDWR);	
	if (store.fd >= 0) 
	{
		store.size = fileSeek(store.fd, 0, SEEK_END);
		fileSeek(store.fd, 0, SEEK_SET);
		iRet = ftp_put(&theFtp, FtpPara->szRemotefile, (ssize_t)store.size, ftp_get_data, &store);
		fileClose(store.fd);
		if (iRet != 0)
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "�ļ�����ʧ��");
			lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "UpLoadErr:%d", iRet);
			PubBeepErr();
			PubWaitKey(5);
			return 1;
		}
		else
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT | DISP_MEDIACY, "�ļ��������");
//			PubBeepOk();
			PubWaitKey(2);
			return 0;
		}
		//NacTxd(uint8_t * psTxdData,uint32_t uiDataLen)
	}
	else 
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "%s", FtpPara->szLocalfile);
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "�ļ���ʧ��");
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}
}

INT32 NewFtpGetFile(FTP_PARA * FtpPara, INT32 (* PreDial)(void))
{
	INT32 iRet, iFileSize = 0;
	
	if (FtpPara == NULL || PreDial == NULL ||
		strlen(FtpPara->szHostip) < 7 || strlen(FtpPara->szPort) < 0 ||
		strlen(FtpPara->szUser) == 0 || strlen(FtpPara->szPwd) == 0 ||
		strlen(FtpPara->szRemotefile) == 0 || strlen(FtpPara->szLocalfile) == 0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT | DISP_MEDIACY, "ftp�������ó���");
		PubBeepErr();
		PubWaitKey(5);
		return 1; 
	}

	// ����
	iRet = PreDial();
	if( iRet!=0 )
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "����ʧ��");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "DialErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}

	// ��¼ftp
	iRet = NewFtpLogon(FtpPara);
	if (iRet != 0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "FTP��¼ʧ��");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "LogErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}

	//��ȡ�ļ���С
	iRet = NewFtpGetSize(FtpPara, &iFileSize);
	if (iRet != 0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "��ȡ�ļ���Сʧ��");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "SizeErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}

	//�����ļ�
	iRet = NewFtpDownLoad(FtpPara, iFileSize);
	
	// �ǳ�������
	NewFtpQuit();
	
	return iRet;
}

INT32 NewFtpQuit(void)
{
	lcdClrLine(2, 7);
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "FTP�ǳ���...");
	
	ftp_logout(&theFtp);
	
	return 0;
}

// ʵ�ֱ����ftp�����ص��ļ��ķ���
int ftp_store_data(void *arg, const void *data, int count)
{
	int iRet;
	store_t *store = (store_t *)arg;
	time_t t2 = SYS_TIME(NULL);
	time_t t3;
	struct tm t;	

	if (kbhit() == YES) {
		if (kbGetKey() == KEY_CANCEL)
			return KEY_CANCEL;
	}

	iRet = PubFileWrite((UINT8 *)(store->filename), (long)(store->offset), (void *)data, count);
	if (iRet != 0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "�ļ�����ʧ��");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "SaveErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return iRet;
	}
	
	store->offset += count;
	t3 = t2-store->ts;
	t = *SYS_GMTIME(&t3);

	lcdClrLine(4, 7);
	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "%d%% %d/%d", store->offset * 100 / store->size, store->offset, store->size);
	lcdDisplay(0, 6, DISP_CFONT|DISP_MEDIACY, "%02d:%02d:%02d %4dBps", t.tm_hour, t.tm_min, t.tm_sec, (int)(store->offset / t3));

	return 0;
}

// ����: �ɹ� 0��ʧ�� 1
INT32 NewFtpDownLoad(FTP_PARA * FtpPara, int iFileSize)
{
	int iRet;
	store_t store;
	
	lcdClrLine(2, 7);
	lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "SIZE: %d", iFileSize);
	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "������...");

	strcpy(store.filename, FtpPara->szLocalfile);
	store.offset = 0;
	store.size = iFileSize;
	store.ts = SYS_TIME(NULL);
	
	iRet = ftp_get(&theFtp, FtpPara->szRemotefile, 0, iFileSize, ftp_store_data, &store);
	if (iRet != 0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "�ļ�����ʧ��");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "LoadErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}
	else
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT | DISP_MEDIACY, "�ļ��������");
//		PubBeepOk();
		PubWaitKey(2);
		return 0;
	}
}

INT32 NewFtpGetSize(FTP_PARA * FtpPara, int * ipFileSize)
{
	int iRet;
	
	lcdClrLine(2, 7);
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "��ȡ�ļ���С...");

	iRet = ftp_size(&theFtp, FtpPara->szRemotefile, ipFileSize);

	return iRet;
}

INT32 NewFtpLogon(FTP_PARA * FtpPara)
{
	int iRet, iPort = atol(FtpPara->szPort);
	memset(&theFtp, 0, sizeof(ftp_host_info_t));

	strcpy(theFtp.user, FtpPara->szUser);
	strcpy(theFtp.password, FtpPara->szPwd);

	lcdClrLine(2, 7);
	lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "FTP��¼��...");
	lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "���Ժ�");
	
	if ((iRet = ftp_login(&theFtp, FtpPara->szHostip, iPort)) != 0)
	{
		return iRet;
	}
	
	return 0;
}

//end of file

