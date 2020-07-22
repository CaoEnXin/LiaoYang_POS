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
 * DESCRIPTION:	��ftp��������������ú����÷�������vfprintf
 *
 * Input:			hftp			�� ftp��½�ľ��
 *				cmd			�� ��������
 *
 * Output:
 *
 * Returns:		����������ֵ�������FTPЭ��淶
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

	/* �������� */
	if (size > 0) {
		ftp_debug("\r\n\r\n[--->]%s", buff);
		size += sprintf(&buff[size], "\r\n");
		retval = Send(hftp->sk_cmd, buff, size, 0);
		if (retval != size) {
			snprintf(hftp->err_info, sizeof(hftp->err_info)-1, "Error in send command: %d\r\n", retval);
			return retval;
		}
	}

	/* ���շ�����Ӧ�� */
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

		/* ������ջ�������? */
		if (offset >= sizeof(hftp->recvbuf_cmd)-1) {
			if (isdigit(ptr[0]) && isdigit(ptr[1]) && isdigit(ptr[2]) && ptr[3] == ' ')
				offset = 4;	/* ������Ӧ����� ��[����][����][����] �ո� ��Ϣ�� ������Ϣ̫����������Ϣ */
			else
				offset = 0;	/* ������Ӧ������Ϣ������ȫ������ */
		}

		/* ÿ��ֻ���ղ�����һ���ֽ� */
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

		/* ������յ����� */
		hftp->recvbuf_cmd[offset++] = data;
		if (data == '\n') {	/* ������һ�� */
			if (offset >= 2) {
				hftp->recvbuf_cmd[offset-2] = '\0';
			}

			hftp->recvbuf_cmd[offset] = '\0';
			ftp_debug("\r\n[<---]%s", ptr);
			if (isdigit(ptr[0]) && isdigit(ptr[1]) && isdigit(ptr[2]) && ptr[3] == ' ')
				break;	/* ������Ӧ����ȷ */
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

	/* ָ������ģʽ���������� */
	if ((retval = ftp_cmd(hftp, "PASV")) != 227) {
		return -1;
	}

	/* �ӷ�����������Ϣ�н�������ݶ˿ں� */
	hftp->port_data = decode_data_port(hftp->recvbuf_cmd);

	/* �������ݶ˿� */
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
 * DESCRIPTION:	��½��ftp������
 *
 * Input:			hostip		�� Զ�̷�������ַ������ "121.35.140.49"
 *				port			�� Զ�̷������˿ںţ�ftp����Ĭ�϶˿ں���21
 *
 * Input:			hftp			�� ftp��½�ľ��
 *
 * Returns:		0	��	��½�ɹ���hftp���ں�������
 *				����	��	����ʧ��
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

	/* �����ͻ��˺ͷ�����������ͨ�� */
	hftp->sk_cmd = ftp_connect(hostip, port);
	if (hftp->sk_cmd < 0)
		return hftp->sk_cmd;

	// set the timeout
	Setsockopt(hftp->sk_cmd, SOL_SOCKET, SO_RCVTIMEO, &hftp->timeout, sizeof(hftp->timeout));

	/* �ȴ���ӭ��Ϣ */
	if ((retval = ftp_cmd(hftp, "")) != 220)	/* �����û��ķ���׼���� */
		goto err;

	/* ���͵�½�û��� */
	retval = ftp_cmd(hftp, "USER %s", hftp->user);
check_login:
	switch (retval) {
		case 230:	/* �û��Ѿ���½ */
			break;
		case 331:	/* ��½��Ҫ���� */
			retval = ftp_cmd(hftp, "PASS %s", hftp->password); /* �û���½ */
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
 * DESCRIPTION:	�˳�ftp��½
 *
 * Input:			hftp			�� ftp��½�ľ��
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
 * DESCRIPTION:	��ȡԶ�̷��������ļ�����
 *
 * Input:			hftp			�� ftp��½�ľ��
 *				server_path	�� Զ���ļ�·�����ļ���
 *				offset		�� Զ���ļ����ش���
 *				size			�� Ҫ�������ݵĳ���
 *				store_data	�� �����������ݵĻص�����
 *					arg		�� ʹ�ú����argment����
 *					data		�� Ҫ�������ݵĻ�����
 *					count	�� Ҫ�������ݵĳ���
 *				argment		�� �ص�������Ҫ�ĵ�һ������
 *
 * Output:
 *
 * Returns:		0	��	�����ɹ�
 *             	����	��	����ʧ�ܣ������ǻص��������ش���
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

	/* ���sizeС��0����Ϊ��֪���ļ���� */
	if (size < 0)
		size = 0x7FFFFFFF;

restart:
	if (retry++ > hftp->recv_retry) {	/* ���Դ���̫�� */
		retval = -EAGAIN;
		ftp_debug("\nToo many retry times\n");
		ftp_close_sock(&hftp->sk_data);
		return retval;
	}

	/* ָ���������� */
	if ((retval = ftp_cmd(hftp, "TYPE I")) != 200)
		return retval;

	/* ����������· */
	if ((retval = ftp_data_connect(hftp)) < 0)
		return retval;

	/* ���ô���� */
	if ((retval = ftp_cmd(hftp, "REST %d", offset)) != 350)
		goto exit_entry;

	cursize = offset;

	/* �ӷ�������ȡ�ļ� */
	retval = ftp_cmd(hftp, "RETR %s", server_path);
	if (retval  != 150 && retval != 125)
		goto exit_entry;

	retval = 0;
	store_data(argment, hftp->recvbuf_data, 0);
	while (cursize < size) {
		minsize = MIN(size - cursize, sizeof(hftp->recvbuf_data));

		/* ��ȡ���� */
		readsize = Recv(hftp->sk_data, hftp->recvbuf_data, minsize, 0);
		if (readsize < 0) {	/* ����ʧ�� */
			snprintf(hftp->err_info, sizeof(hftp->err_info)-1,
					"%d: receive data failed, error=%d.", readsize, readsize);
			ftp_debug(hftp->err_info);
			ftp_close_sock(&hftp->sk_data);
			ftp_cmd(hftp, "");
			goto restart;
		}

		/* ������յ����� */
		if ((retval = store_data(argment, hftp->recvbuf_data, readsize)) != 0) {
			break;
		}

		cursize += readsize;
		offset  += readsize;
	}

exit_entry:
	ftp_close_sock(&hftp->sk_data);

	/* �ȴ�������·�˿ں����Ϣ */
	ftp_cmd(hftp, "");

	return retval;
}

/******************************************************************************
 * Function:		ftp_put
 *
 * DESCRIPTION:	�ϴ��ļ�
 *
 * Input:			hftp			�� ftp��½�ľ��
 *				server_path	�� Զ���ļ�·�����ļ���
 *				size			�� �ϴ��ļ��Ĵ�С
 *				get_data		�� ��ȡ�����ļ����ݵĻص�����
 *					arg		�� ʹ�ú����argment����
 *					data		�� �����ȡ���ݵĻ�����
 *					offset	�� Ҫ��ȡ��������ļ�ͷ��ƫ��
 *					count	�� Ҫ��ȡ���ݵĳ���
 *					����:	0 	�� ��ȡ��ȷ
 *							����	�� �����ϴ���ֹ
 *				argment		�� �ص�������Ҫ�ĵ�һ������
 *
 * Output:
 *
 * Returns:		0	��	�����ɹ�
 *             	����	��	����ʧ�ܣ������ǻص��������ش���
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

#if 0	/* �������ǲ������ */
	/* �ڷ������Ϸ����ļ��ռ� */
	retval = ftp_cmd(hftp, "ALLO %d", size);
	if (retval != 200 && retval != 202)
		return retval;
#endif

	/* ָ���������� */
	if ((retval = ftp_cmd(hftp, "TYPE I")) != 200) {
		return retval;
	}

	/* ����������· */
	if ((retval = ftp_data_connect(hftp)) < 0)
		return retval;

	/* ����ʹ��RESET����ָ������� */
	/* ���ô���� */
	if ((retval = ftp_cmd(hftp, "REST %d", offset)) != 350)
		goto exit_entry;

	/* ֪ͨ������׼���������ݲ����浽�ļ� */
	retval = ftp_cmd(hftp, "STOR %s", server_path);
	if ((150!=retval) && (125!=retval))
		return retval;

	retval = 0;
	while (offset < size) {
		count = MIN(size - offset, sizeof(hftp->recvbuf_data));

		/* ��ȡ�����ļ����� */
		if (get_data(argment, hftp->recvbuf_data, offset, count) != 0) {
			break;
		}

		/* �����ļ����ݸ������� */
		sendcnt = Send(hftp->sk_data, hftp->recvbuf_data, count, 0);
		if (sendcnt < 0) {
			retval = sendcnt;
			ftp_debug("\n%s: Send data failed, %d\n", __FUNCTION__, sendcnt);
			break;
		}

		offset += sendcnt;
	}

exit_entry:
	/* �ر�������· */
	ftp_close_sock(&hftp->sk_data);

	ftp_cmd(hftp, "");

	return retval;
}

/******************************************************************************
 * Function:		ftp_size
 *
 * DESCRIPTION:	��ȡԶ���ļ���С
 *
 * Input:			hftp			�� ftp��½�ľ��
 *				server_path	�� Զ���ļ�·�����ļ���
 *
 * Output:		filesize		�� Զ���ļ���С
 *
 * Returns:		0	��	�����ɹ����ļ���С���
 *             	����	��	����ʧ�ܣ������Ƿ�������֧�ָ�����
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

	/* ָ���������� */
	if ((retval = ftp_cmd(hftp, "TYPE I")) != 200) {
		return retval;
	}

	/* ��ȡ�ļ���С */
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

	/* ָ���������� */
	if ((retval = ftp_cmd(hftp, "TYPE A")) != 200)
		return retval;

	/* ����������· */
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

