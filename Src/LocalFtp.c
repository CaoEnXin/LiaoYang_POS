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
static FTP_ERR_INFO FtpErrInfo[] =   // 这个仅仅写在这里供参考ftp返回码的定义，暂不实现在pos上显示中文信息
{
	{110, "重启标记应答"},
	{120, "服务在nnn分钟内准备好"},
	{125, "数据连接已打开，准备传送"},
	{150, "文件状态良好，打开数据连接"},
	{200, "命令成功"},
	{202, "命令未实现"},
	{211, "系统状态或系统帮助响应"},
	{212, "目录状态"},
	{213, "文件状态"},
	{214, "帮助信息，信息仅对人类用户有用"},
	{215, "名字系统类型"},
	{220, "对新用户服务准备好"},
	{221, "服务关闭控制连接，可以退出登录"},
	{225, "数据连接打开，无传输正在进行"},
	{226, "关闭数据连接，请求的文件操作成功"},
	{227, "进入被动模式"},
	{230, "用户登录"},
	{250, "请求的文件操作完成"},
	{257, "创建"PATHNAME""},
	{331, "用户名正确，需要口令"},
	{332, "登录时需要帐户信息"},
	{350, "请求的文件操作需要进一步命令"},
	{421, "不能提供服务，关闭控制连接"},
	{425, "不能打开数据连接"},
	{426, "关闭连接，中止传输"},
	{450, "请求的文件操作未执行"},
	{451, "中止请求的操作：有本地错误"},
	{452, "未执行请求的操作：系统存储空间不足"},
	{500, "格式错误，命令不可识别"},
	{501, "参数语法错误"},
	{502, "命令未实现"},
	{503, "命令顺序错误"},
	{504, "此参数下的命令功能未实现"},
	{530, "未登录"},
	{532, "存储文件需要帐户信息"},
	{550, "未执行请求的操作"},
	{551, "请求操作中止：页类型未知"},
	{552, "请求的文件操作中止，存储分配溢出"},
	{553, "未执行请求的操作：文件名不合法"},
	{0, ""}
};
*/
/********************** external reference declaration *********************/

INT32 ftpDemo(INT32 (* PreDial)(void)) // 该函数仅供测试ftp的基本功能，其中预拨号函数PreDial需要自己实现
{
	int iKey;
	FTP_PARA stFtpPara;

	if (PreDial == NULL)
	{
		lcdCls();
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "缺少回调函数");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "PreDial");
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}

	lcdCls();
	lcdDisplay(0, 0, DISP_CFONT | DISP_REVERSE | DISP_MEDIACY, "FTP功能测试");
	lcdDisplay(0, 2, DISP_CFONT, "1. 重启");
	lcdDisplay(0, 4, DISP_CFONT, "2. FTP下载");
	lcdDisplay(0, 6, DISP_CFONT, "3. FTP上送");
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
				strcpy(stFtpPara.szUser, "anonymous"); // 匿名登录，用户名必须设置为anonymous
				strcpy(stFtpPara.szPwd, "tc111111"); // 密码为任意字符串，但必须小于31个字符
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
								strcpy(stFtpPara.szUser, "anonymous"); // 匿名登录，用户名必须设置为anonymous
				strcpy(stFtpPara.szPwd, "tc111111"); // 密码为任意字符串，但必须小于31个字符
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
		lcdDisplay(0, 3, DISP_CFONT | DISP_MEDIACY, "ftp参数设置出错");
		PubBeepErr();
		PubWaitKey(5);
		return 1; 
	}

	if(fileExist(FtpPara->szLocalfile) < 0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "%s", FtpPara->szLocalfile);
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "文件不存在");
		PubBeepErr();
		PubWaitKey(5);
		return 1; 
	}

	// 拨号
	iRet = PreDial();
	if( iRet!=0 )
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "拨号失败");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "DialErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}

	// 登录ftp
	iRet = NewFtpLogon(FtpPara);
	if (iRet != 0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "FTP登录失败");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "LogErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}

	//上送文件
	iRet = NewFtpPut(FtpPara);
	
	// 登出服务器
	NewFtpQuit();
	
	return iRet;
}

// 实现保存从ftp上下载的文件的方法
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
			lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "文件上送失败");
			lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "UpLoadErr:%d", iRet);
			PubBeepErr();
			PubWaitKey(5);
			return 1;
		}
		else
		{
			lcdClrLine(2, 7);
			lcdDisplay(0, 3, DISP_CFONT | DISP_MEDIACY, "文件上送完成");
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
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "文件打开失败");
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
		lcdDisplay(0, 3, DISP_CFONT | DISP_MEDIACY, "ftp参数设置出错");
		PubBeepErr();
		PubWaitKey(5);
		return 1; 
	}

	// 拨号
	iRet = PreDial();
	if( iRet!=0 )
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "拨号失败");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "DialErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}

	// 登录ftp
	iRet = NewFtpLogon(FtpPara);
	if (iRet != 0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "FTP登录失败");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "LogErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}

	//获取文件大小
	iRet = NewFtpGetSize(FtpPara, &iFileSize);
	if (iRet != 0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "获取文件大小失败");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "SizeErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}

	//下载文件
	iRet = NewFtpDownLoad(FtpPara, iFileSize);
	
	// 登出服务器
	NewFtpQuit();
	
	return iRet;
}

INT32 NewFtpQuit(void)
{
	lcdClrLine(2, 7);
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "FTP登出中...");
	
	ftp_logout(&theFtp);
	
	return 0;
}

// 实现保存从ftp上下载的文件的方法
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
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "文件保存失败");
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

// 返回: 成功 0，失败 1
INT32 NewFtpDownLoad(FTP_PARA * FtpPara, int iFileSize)
{
	int iRet;
	store_t store;
	
	lcdClrLine(2, 7);
	lcdDisplay(0, 2, DISP_CFONT|DISP_MEDIACY, "SIZE: %d", iFileSize);
	lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "下载中...");

	strcpy(store.filename, FtpPara->szLocalfile);
	store.offset = 0;
	store.size = iFileSize;
	store.ts = SYS_TIME(NULL);
	
	iRet = ftp_get(&theFtp, FtpPara->szRemotefile, 0, iFileSize, ftp_store_data, &store);
	if (iRet != 0)
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "文件下载失败");
		lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "LoadErr:%d", iRet);
		PubBeepErr();
		PubWaitKey(5);
		return 1;
	}
	else
	{
		lcdClrLine(2, 7);
		lcdDisplay(0, 3, DISP_CFONT | DISP_MEDIACY, "文件下载完成");
//		PubBeepOk();
		PubWaitKey(2);
		return 0;
	}
}

INT32 NewFtpGetSize(FTP_PARA * FtpPara, int * ipFileSize)
{
	int iRet;
	
	lcdClrLine(2, 7);
	lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "获取文件大小...");

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
	lcdDisplay(0, 2, DISP_CFONT | DISP_MEDIACY, "FTP登录中...");
	lcdDisplay(0, 4, DISP_CFONT | DISP_MEDIACY, "请稍后");
	
	if ((iRet = ftp_login(&theFtp, FtpPara->szHostip, iPort)) != 0)
	{
		return iRet;
	}
	
	return 0;
}

//end of file

