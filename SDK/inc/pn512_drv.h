/******************************************************************************
 * pn512_drv.h
 * 
 * Copyright (c) 2011 by Li.Hua <lihua_338@163.com>.
 * 
 * DESCRIPTION: - 
 * 
 * Modification history
 * ----------------------------------------------------------------------------
 * Date         Version  Author       History
 * ----------------------------------------------------------------------------
 * 2011-06-01   1.0.0    Li.Hua       written
 ******************************************************************************/

#ifndef _PN512_DRV_H_
#define _PN512_DRV_H_

enum {
	NFC_ERR_NONE				= 0,
	NFC_ERR_TIMEOUT				= ETIME,		/* 超时无返回 */
	NFC_ERR_EIO					= EIO,			/* 读卡芯片错误 */
	NFC_ERR_EPROTO				= EPROTO,		/* 卡片返回数据不符合ISO14443规范 */
	NFC_ERR_ECOMM				= ECOMM,		/* 卡片返回数据奇偶错或者CRC校验错 */
	NFC_ERR_EBADE				= EBADE,		/* 卡片返回数据长度错误 */
	NFC_ERR_EMULTIHOP			= EMULTIHOP,	/* 感应区内多卡存在 */
	NFC_ERR_ENODATA 			= ENODATA,		/* 感应区内无T=CL卡 */
};
#define ERESTARTSYS	512

struct apdu_exchange {
	uint32_t	cmd_len;
	uint32_t	rsp_len;
	uint8_t		cmd_data[256+6];	/* CLA INS P1 P2 Lc Data Le */
	uint8_t		rsp_data[256];
};

#define copy_from_user(to,from,n)	(memcpy(to, from, n), 0)
#define copy_to_user(to,from,n)		(memcpy(to, from, n), 0)
#define clear_user(addr,n)			(memset(addr, 0, n), 0)


#ifndef _IOWR
# define _IOWR(x,y,z)	(((x)<<8)|y)
#endif
#ifndef _IOW
# define _IOW(x,y,z)	(((x)<<8)|y)
#endif
#ifndef _IOR
# define _IOR(x,y,z)	(((x)<<8)|y)
#endif
#ifndef _IO
# define _IO(x,y)		(((x)<<8)|y)
#endif
	
/* 使用"c"作为幻数*/
#define PN512_IOC_MAGIC  'r'
	
/*
  * S means "Set" through a ptr,
  * T means "Tell" directly with the argument value
  * G means "Get" reply by setting through a pointer
  * Q means "Query" response is on the return value
  * X means "eXchange" switch G and S atomically
  * H means "sHift" switch T and Q atomically
*/
	
#define PN512_IOC_X_READ_REG		_IOWR(PN512_IOC_MAGIC,	1, int)
#define PN512_IOC_S_WRITE_REG		_IOW(PN512_IOC_MAGIC,	2, int)
#define PN512_IOC_S_CMD_BUFF		_IOW(PN512_IOC_MAGIC,	3, int)
#define PN512_IOC_G_RSP_BUFF		_IOR(PN512_IOC_MAGIC,	4, int)
#define PN512_IOC_S_STATUS			_IOW(PN512_IOC_MAGIC, 	5, int)
#define PN512_IOC_G_STATUS			_IOR(PN512_IOC_MAGIC, 	6, int)

#define PN512_IOC_T_POLL_PICC		_IOW(PN512_IOC_MAGIC, 	7, int *)
#define PN512_IOC_G_OPERATE_STATE	_IOR(PN512_IOC_MAGIC,	8, int *)
#define PN512_IOC_X_APDU			_IOWR(PN512_IOC_MAGIC,	9, struct apdu_exchange *)
#define PN512_IOC_T_REMOVE			_IO(PN512_IOC_MAGIC,	10)
#define PN512_IOC_G_LAST_ERROR		_IOR(PN512_IOC_MAGIC, 	11, int *)

#endif
	

