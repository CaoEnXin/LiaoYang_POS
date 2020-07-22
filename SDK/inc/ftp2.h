/*
 * ftp.h
 *
 * Header file for ftp function
 *
 * Copyright (C) 2008 Lihua <lihua_338@163.com>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 *
 */
#ifndef __FTP_H__
#define __FTP_H__

#include <stdint.h>
#include <sys/types.h>
#include <time.h>

typedef struct {
	uint16_t	port_cmd;				/* ��/������Ķ˿ں� */
	uint16_t	port_data;				/* ��/�����ݵĶ˿ں� */
	int			sk_cmd;					/* ��/�������socket */
	int			sk_data;				/* ��/�����ݵ�socket */
	int			recv_retry;				/* �������Դ��� */
	char 		user[32];				/* ��½���������û��� */
	char 		password[32];			/* ��½���� */
	char		hostip[32];				/* ������IP��ַ��p1.p2.p3.p4 */
	char		err_info[512];			/* �洢������Ϣ */
	char		recvbuf_cmd[512];		/* ����������� */
	char		recvbuf_data[1 * 1024];	/* ���ݽ��ջ����� */

	struct timeval timeout;				/* ���ճ�ʱ */
} ftp_host_info_t;

typedef struct {
	char		filename[256];
	int 		fd;
	int 		size;
	int 		offset;
	int 		StartOffset;
	time_t		ts;
}store_t;



int ftp_login2(ftp_host_info_t *hftp, const char *hostip, uint16_t port);
int ftp_size2(ftp_host_info_t *hftp, const char *server_path, int *filesize);
int ftp_get2(ftp_host_info_t *hftp, const char *server_path,
				int (*store_data)(void *arg, const void *data, int count), void *argment);
int ftp_put2(ftp_host_info_t *hftp, const char *server_path, ssize_t size,
				int (*get_data)(void *arg, void *data, loff_t offset, int count), void *argment);
int ftp_logout2(ftp_host_info_t *hftp);

int ftp_ls2(ftp_host_info_t *hftp, const char *server_path);

#endif

