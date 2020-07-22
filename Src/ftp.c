/*
 * ftp.c
 *
 * Header file for net function
 *
 * Copyright (C) 2008 Lihua <lihua_338@163.com>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 *
 */

#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>

#include "posapi.h"
#include "net.h"
#include "ftp.h"



#define ftp_debug(format, arg...)	 // portPrintf(PORT_COM1, format, ##arg)

const static struct timeval DefaultFtpRecvTO = {
	.tv_sec		= 1,	/* seconds */
	.tv_usec	= 0,	/* microseconds */
};

static int decode_data_port(char *buf)
{
	char *buf_ptr;
	unsigned short port_num;

	/* Response is "NNN garbageN1,N2,N3,N4,P1,P2[)garbage]
	 * Server's IP is N1.N2.N3.N4 (we ignore it)
	 * Server's port for data connection is P1*256+P2
	 */
	buf_ptr = strrchr(buf, ')');
	if (buf_ptr)
		*buf_ptr = '\0';

	buf_ptr = strrchr(buf, ',');
	*buf_ptr = '\0';
	port_num = strtol(buf_ptr + 1, NULL, 10);

	buf_ptr = strrchr(buf, ',');
	*buf_ptr = '\0';
	port_num += strtol(buf_ptr + 1, NULL, 10) * 256;

	return port_num;
}

static int ftp_connect(const char *hostip, uint16_t port)
{
	struct sockaddr_in sa;
	int sock, retval;

	ftp_debug("\nSocket(...)...");
	if ((sock = Socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		ftp_debug("FAILD:%d\n", sock);
		return sock;
	}
	ftp_debug("sock=%d\n", sock);

	memset (&sa, 0x00, sizeof(sa));
	sa.sin_family	   = AF_INET;
	sa.sin_addr.s_addr = inet_addr(hostip);	/* Server IP */
	sa.sin_port 	   = htons(port);		/* Server Port number */

	ftp_debug("Connect to %s:%d......", hostip, port);
	if ((retval = Connect(sock, (struct sockaddr*) &sa, sizeof(sa))) < 0) {
		ftp_debug("ERROR:%d\n", retval);
		Shutdown(sock);
		return retval;
	}
	ftp_debug("Connected\n");

	return sock;
}

/******************************************************************************
 * Function:		ftp_cmd
 *
 * DESCRIPTION:	向ftp服务器发送命令，该函数用法类似于vfprintf
 *
 * Input:			hftp			－ ftp登陆的句柄
 *				cmd			－ 发送命令
 *
 * Output:
 *
 * Returns:		服务器返回值，请参阅FTP协议规范
 *
 *
 * Modification history
 * ----------------------------------------------------------------------------
 *	Li.Hua	2008-04-29 10:37:57 ,  Written
 ******************************************************************************/
int ftp_cmd(ftp_host_info_t *hftp, const char *cmd, ...)
{
	int retval;
	char buff[512];
	int recvlen;
	int size, offset;
	va_list ap;
	char *ptr;
	int retry;

	va_start(ap, cmd);
	size = vsnprintf(buff, sizeof(buff)-3, cmd, ap);
	va_end(ap);

	/* 发送命令 */
	if (size > 0) {
		ftp_debug("\r\n\r\n[--->]%s", buff);
		size += sprintf(&buff[size], "\r\n");
		retval = Send(hftp->sk_cmd, buff, size, 0);
		if (retval != size) {
			snprintf(hftp->err_info, sizeof(hftp->err_info)-1, "Error in send command: %d\r\n", retval);
			return retval;
		}
	}

	/* 接收服务器应答 */
	offset = 0;
	ptr = hftp->recvbuf_cmd;
	retry = 0;
	while (1) {
		char data;

		if (retry >= 30) {
			snprintf(hftp->err_info, sizeof(hftp->err_info)-1, "recive from server return %d\r\n", recvlen);
			ftp_debug("\n%s", hftp->err_info);
			return 550;
		}

		/* 命令接收缓冲区满? */
		if (offset >= sizeof(hftp->recvbuf_cmd)-1) {
			if (isdigit(ptr[0]) && isdigit(ptr[1]) && isdigit(ptr[2]) && ptr[3] == ' ')
				offset = 4;	/* 服务器应答符合 “[数字][数字][数字] 空格 信息” 但是信息太长，忽略信息 */
			else
				offset = 0;	/* 服务器应答都是信息，可以全部忽略 */
		}

		/* 每次只接收并处理一个字节 */
		recvlen = Recv(hftp->sk_cmd, &data, sizeof(data), 0);
		if (recvlen < 0) {
			snprintf(hftp->err_info, sizeof(hftp->err_info)-1, "recive from server return %d\r\n", recvlen);
			ftp_debug("\n%s", hftp->err_info);
			return recvlen;
		}

		if (recvlen == 0) {
			retry++;
			continue;
		}

		retry = 0;

		/* 缓存接收的数据 */
		hftp->recvbuf_cmd[offset++] = data;
		if (data == '\n') {	/* 接收满一行 */
			if (offset >= 2) {
				hftp->recvbuf_cmd[offset-2] = '\0';
			}

			hftp->recvbuf_cmd[offset] = '\0';
			ftp_debug("\r\n[<---]%s", ptr);
			if (isdigit(ptr[0]) && isdigit(ptr[1]) && isdigit(ptr[2]) && ptr[3] == ' ')
				break;	/* 服务器应答正确 */
			else
				offset = 0;
		}
	};

	ptr[3] = '\0';
	retval = strtol(ptr, NULL, 10);
	ptr[3] = ' ';

	snprintf(hftp->err_info, sizeof(hftp->err_info)-1, "%d: %s", retval, ptr);

	return retval;
}

static int ftp_data_connect(ftp_host_info_t *hftp)
{
	int retval;

	/* 指定传输模式：被动连接 */
	if ((retval = ftp_cmd(hftp, "PASV")) != 227) {
		return -1;
	}

	/* 从服务器返回信息中解码出数据端口号 */
	hftp->port_data = decode_data_port(hftp->recvbuf_cmd);

	/* 连接数据端口 */
	hftp->sk_data = ftp_connect(hftp->hostip, hftp->port_data);
	if (hftp->sk_data < 0) {
		snprintf(hftp->err_info, sizeof(hftp->err_info)-1,
				"%d: Can not connect to %s:%d",
				hftp->sk_data, hftp->hostip, hftp->port_data);
		return hftp->sk_data;
	}

	// set the timeout
	Setsockopt(hftp->sk_data, SOL_SOCKET, SO_RCVTIMEO, &hftp->timeout, sizeof(hftp->timeout));

	return hftp->sk_data;
}

static int ftp_close_sock(int *socket)
{
	if (*socket >= 0) {
		Shutdown(*socket);
		*socket = -1;
	}

	return 0;
}

static int ftp_close(ftp_host_info_t *hftp)
{
	ftp_close_sock(&hftp->sk_cmd);
	ftp_close_sock(&hftp->sk_data);

	return 0;
}
/******************************************************************************
 * Function:		ftp_login
 *
 * DESCRIPTION:	登陆到ftp服务器
 *
 * Input:			hostip		－ 远程服务器地址，例如 "121.35.140.49"
 *				port			－ 远程服务器端口号，ftp服务默认端口号是21
 *
 * Input:			hftp			－ ftp登陆的句柄
 *
 * Returns:		0	：	登陆成功，hftp用于后续操作
 *				其它	：	操作失败
 *
 *
 * Modification history
 * ----------------------------------------------------------------------------
 *	Li.Hua	2008-04-29 10:33:58 ,  Written
 ******************************************************************************/
int ftp_login(ftp_host_info_t *hftp, const char *hostip, uint16_t port)
{
	int retval;

	if (!hostip || !hftp)
		return -EINVAL;

	hftp->sk_cmd = hftp->sk_data = -1;
	strncpy(hftp->hostip, hostip, sizeof(hftp->hostip)-1);
	hftp->port_cmd 	= port;
	hftp->port_data = port-1;
	hftp->recv_retry= 10;
	hftp->timeout   = DefaultFtpRecvTO;
	hftp->err_info[0] = '\0';

	/* 建立客户端和服务器的命令通道 */
	hftp->sk_cmd = ftp_connect(hostip, port);
	if (hftp->sk_cmd < 0)
		return hftp->sk_cmd;

	// set the timeout
	Setsockopt(hftp->sk_cmd, SOL_SOCKET, SO_RCVTIMEO, &hftp->timeout, sizeof(hftp->timeout));

	/* 等待欢迎信息 */
	if ((retval = ftp_cmd(hftp, "")) != 220)	/* 对新用户的服务准备好 */
		goto err;

	/* 发送登陆用户名 */
	retval = ftp_cmd(hftp, "USER %s", hftp->user);
check_login:
	switch (retval) {
		case 230:	/* 用户已经登陆 */
			break;
		case 331:	/* 登陆需要密码 */
			retval = ftp_cmd(hftp, "PASS %s", hftp->password); /* 用户登陆 */
			goto check_login;
		default:
			goto err;
	}

	return 0;

err:
	ftp_debug("FTP login error: %s", hftp->err_info);
	ftp_close_sock(&hftp->sk_cmd);
	return retval;
}

/******************************************************************************
 * Function:		ftp_logout
 *
 * DESCRIPTION:	退出ftp登陆
 *
 * Input:			hftp			－ ftp登陆的句柄
 *
 * Output:
 *
 * Returns:		0
 *
 *
 * Modification history
 * ----------------------------------------------------------------------------
 *	Li.Hua	2008-04-29 10:32:51 ,  Written
 ******************************************************************************/
int ftp_logout(ftp_host_info_t *hftp)
{
	if (hftp->sk_cmd >= 0)
		 Send(hftp->sk_cmd, "QUIT\r\n", 6, 0);

	ftp_close(hftp);

	return 0;
}

/******************************************************************************
 * Function:		ftp_get
 *
 * DESCRIPTION:	获取远程服务器的文件数据
 *
 * Input:			hftp			－ ftp登陆的句柄
 *				server_path	－ 远程文件路径和文件名
 *				offset		－ 远程文件的重传点
 *				size			－ 要下载数据的长度
 *				store_data	－ 保存下载数据的回调函数
 *					arg		－ 使用后面的argment参数
 *					data		－ 要保存数据的缓冲区
 *					count	－ 要保存数据的长度
 *				argment		－ 回调函数需要的第一个参数
 *
 * Output:
 *
 * Returns:		0	：	操作成功
 *             	其它	：	操作失败，可能是回调函数返回错误
 *
 * Modification history
 * ----------------------------------------------------------------------------
 *	Li.Hua	2008-04-29 10:26:24 ,  Written
 ******************************************************************************/
int ftp_get(ftp_host_info_t *hftp, const char *server_path, loff_t offset, ssize_t size,
				int (*store_data)(void *arg, const void *data, int count), void *argment)
{
	int retval, cursize, minsize, readsize;
	int retry = 0;

	if (!hftp || !server_path || !store_data)
		return -EINVAL;

	/* 如果size小于0则认为不知道文件多大 */
	if (size < 0)
		size = 0x7FFFFFFF;

restart:
	if (retry++ > hftp->recv_retry) {	/* 重试次数太多 */
		retval = -EAGAIN;
		ftp_debug("\nToo many retry times\n");
		ftp_close_sock(&hftp->sk_data);
		return retval;
	}

	/* 指定数据类型 */
	if ((retval = ftp_cmd(hftp, "TYPE I")) != 200)
		return retval;

	/* 连接数据链路 */
	if ((retval = ftp_data_connect(hftp)) < 0)
		return retval;

	/* 设置传输点 */
	if ((retval = ftp_cmd(hftp, "REST %d", offset)) != 350)
		goto exit_entry;

	cursize = offset;

	/* 从服务器获取文件 */
	retval = ftp_cmd(hftp, "RETR %s", server_path);
	if (retval  != 150 && retval != 125)
		goto exit_entry;

	retval = 0;
	store_data(argment, hftp->recvbuf_data, 0);
	while (cursize < size) {
		minsize = MIN(size - cursize, sizeof(hftp->recvbuf_data));

		/* 读取数据 */
		readsize = Recv(hftp->sk_data, hftp->recvbuf_data, minsize, 0);
		if (readsize < 0) {	/* 接收失败 */
			snprintf(hftp->err_info, sizeof(hftp->err_info)-1,
					"%d: receive data failed, error=%d.", readsize, readsize);
			ftp_debug(hftp->err_info);
			ftp_close_sock(&hftp->sk_data);
			ftp_cmd(hftp, "");
			goto restart;
		}

		/* 保存接收的数据 */
		if ((retval = store_data(argment, hftp->recvbuf_data, readsize)) != 0) {
			break;
		}

		cursize += readsize;
		offset  += readsize;
	}

exit_entry:
	ftp_close_sock(&hftp->sk_data);

	/* 等待数据链路端口后的消息 */
	ftp_cmd(hftp, "");

	return retval;
}

/******************************************************************************
 * Function:		ftp_put
 *
 * DESCRIPTION:	上传文件
 *
 * Input:			hftp			－ ftp登陆的句柄
 *				server_path	－ 远程文件路径和文件名
 *				size			－ 上传文件的大小
 *				get_data		－ 读取本地文件数据的回调函数
 *					arg		－ 使用后面的argment参数
 *					data		－ 保存读取数据的缓冲区
 *					offset	－ 要读取数据相对文件头的偏移
 *					count	－ 要读取数据的长度
 *					返回:	0 	－ 读取正确
 *							其它	－ 错误，上传终止
 *				argment		－ 回调函数需要的第一个参数
 *
 * Output:
 *
 * Returns:		0	：	操作成功
 *             	其它	：	操作失败，可能是回调函数返回错误
 *
 * Modification history
 * ----------------------------------------------------------------------------
 *	Li.Hua	2008-04-29 10:17:30 ,  Written
 ******************************************************************************/
int ftp_put(ftp_host_info_t *hftp, const char *server_path, loff_t offset, ssize_t size,
				int (*get_data)(void *arg, void *data, loff_t offset, int count), void *argment)
{
	int retval;
	int sendcnt, count;

	if (!hftp || !server_path || !get_data)
		return -EINVAL;

#if 0	/* 该命令是不必须的 */
	/* 在服务器上分配文件空间 */
	retval = ftp_cmd(hftp, "ALLO %d", size);
	if (retval != 200 && retval != 202)
		return retval;
#endif

	/* 指定数据类型 */
	if ((retval = ftp_cmd(hftp, "TYPE I")) != 200) {
		return retval;
	}

	/* 连接数据链路 */
	if ((retval = ftp_data_connect(hftp)) < 0)
		return retval;

	/* 可以使用RESET命令指定传输点 */
	/* 设置传输点 */
	if ((retval = ftp_cmd(hftp, "REST %d", offset)) != 350)
		goto exit_entry;

	/* 通知服务器准备接收数据并保存到文件 */
	retval = ftp_cmd(hftp, "STOR %s", server_path);
	if ((150!=retval) && (125!=retval))
		return retval;

	retval = 0;
	while (offset < size) {
		count = MIN(size - offset, sizeof(hftp->recvbuf_data));

		/* 读取本地文件数据 */
		if (get_data(argment, hftp->recvbuf_data, offset, count) != 0) {
			break;
		}

		/* 发送文件数据给服务器 */
		sendcnt = Send(hftp->sk_data, hftp->recvbuf_data, count, 0);
		if (sendcnt < 0) {
			retval = sendcnt;
			ftp_debug("\n%s: Send data failed, %d\n", __FUNCTION__, sendcnt);
			break;
		}

		offset += sendcnt;
	}

exit_entry:
	/* 关闭数据链路 */
	ftp_close_sock(&hftp->sk_data);

	ftp_cmd(hftp, "");

	return retval;
}

/******************************************************************************
 * Function:		ftp_size
 *
 * DESCRIPTION:	获取远程文件大小
 *
 * Input:			hftp			－ ftp登陆的句柄
 *				server_path	－ 远程文件路径和文件名
 *
 * Output:		filesize		－ 远程文件大小
 *
 * Returns:		0	：	操作成功，文件大小获得
 *             	其它	：	操作失败，可能是服务器不支持该命令
 *
 * Modification history
 * ----------------------------------------------------------------------------
 *	Li.Hua	2008-04-29 10:13:40 ,  Written
 ******************************************************************************/
int ftp_size(ftp_host_info_t *hftp, const char *server_path, int *filesize)
{
	int retval;

	if (!hftp || !server_path || !filesize)
		return -EINVAL;

	*filesize = 0;

	/* 指定数据类型 */
	if ((retval = ftp_cmd(hftp, "TYPE I")) != 200) {
		return retval;
	}

	/* 获取文件大小 */
	if ((retval = ftp_cmd(hftp, "SIZE %s", server_path)) == 213) {
		*filesize = strtol(&hftp->recvbuf_cmd[4], NULL, 10);
		retval = 0;
	}

	return retval;
}

int ftp_ls(ftp_host_info_t *hftp, const char *server_path)
{
	int retval;
	int retry;

	/* 指定数据类型 */
	if ((retval = ftp_cmd(hftp, "TYPE A")) != 200)
		return retval;

	/* 连接数据链路 */
	if ((retval = ftp_data_connect(hftp)) < 0)
		return retval;

	if ((retval = ftp_cmd(hftp, "LIST %s", server_path)) != 150)
		goto exit_entry;

	ftp_debug("\r\n");
	retry = 0;
	while (1) {
		int readsize;
		char data;

		readsize = Recv(hftp->sk_data,&data, sizeof(data), 0);
		if (readsize < 0)
			break;

		if (readsize == 0) {
			retry++;
			if (retry > hftp->recv_retry)
				break;

			continue;
		}

		retry = 0;

		ftp_debug("%c", data);
	}

exit_entry:
	ftp_close_sock(&hftp->sk_data);

	retval = ftp_cmd(hftp, "");
	if (retval != 226 && retval != 426) {
		return retval;
	}

	return 0;
}

