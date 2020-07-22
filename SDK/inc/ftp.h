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


#ifdef __cplusplus
extern "C"{
#endif



typedef struct {
	uint16_t	port_cmd;				/* 收/发命令的端口号 */
	uint16_t	port_data;				/* 收/发数据的端口号 */
	int			sk_cmd;					/* 收/发命令的socket */
	int			sk_data;				/* 收/发数据的socket */
	int			recv_retry;				/* 接收重试次数 */
	char 		user[32];				/* 登陆服务器的用户名 */
	char 		password[32];			/* 登陆密码 */
	char		hostip[32];				/* 服务器IP地址，p1.p2.p3.p4 */
	char		err_info[512];			/* 存储错误信息 */
	char		recvbuf_cmd[512];		/* 接收命令缓冲区 */
	char		recvbuf_data[1 * 1024];	/* 数据接收缓冲区 */

	struct timeval timeout;				/* 接收超时 */
} ftp_host_info_t;


int ftp_login(ftp_host_info_t *hftp, const char *hostip, uint16_t port);
int ftp_cmd(ftp_host_info_t *hftp, const char *cmd, ...);
int ftp_size(ftp_host_info_t *hftp, const char *server_path, int *filesize);
int ftp_get(ftp_host_info_t *hftp, const char *server_path, loff_t offset, ssize_t size,
				int (*store_data)(void *arg, const void *data, int count), void *argment);
int ftp_put(ftp_host_info_t *hftp, const char *server_path, ssize_t size,
				int (*get_data)(void *arg, void *data, loff_t offset, int count), void *argment);
int ftp_logout(ftp_host_info_t *hftp);

#ifdef __cplusplus
}
#endif

#endif

