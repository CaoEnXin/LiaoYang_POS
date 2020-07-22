/******************************************************************************
 * font.h
 *
 * Copyright (c) 2008 by Li.Hua, Newpos Technology Co.,Ltd.
 *
 * DESCRIPTION: - �ֿ������������
 *
 * Modification history
 * ----------------------------------------------------------------------------
 * Date 		Version  Author 	  History
 * ----------------------------------------------------------------------------
 * 2008-06-26	1.0.0.0  Li.Hua 	  written
 ******************************************************************************/


#ifndef __FONT_LIB_H__
#define __FONT_LIB_H__

#ifdef __KERNEL__
  #include <linux/types.h>
  #include <linux/list.h>
  #include <linux/string.h>
#else
  #include "posapi.h"
  #include "list.h"
  #include <string.h>
  #include <stdint.h>
  #include <sys/types.h>
#endif


#ifdef __cplusplus
extern "C"{
#endif



#undef CFG_CHINESE_SUPPORT_USER_REGIN	/* GB18030�����û��Զ����� */
#undef CFG_FONT_DEBUG					/* ���Կ��� */
//#define CFG_FONT_DEBUG

#define SET_PRIMARY_FONT                0   //  ������ѡ�ֿ�
#define SET_SECONDARY_FONT              1   //  ���ñ�ѡ�ֿ�(�����ѡ�ֿ�Ϊ���֣�ͨ�������ѡ�ֿ�ѡ��ascii���ֵ�����)

/* ���嶨�� */
#define FONT_COURIER_NEW				"CourierNew"
#define FONT_SONG						"����"
#define FONT_IMITATE_SONG				"����"
#define FONT_SYSTEM						"System"
#define FONT_ARIAL						"Arial"
#define FONT_TAHOMA						"Tahoma"
#define FONT_VERDANA					"Verdana"
#define FONT_DOTUM						"Dotum"

/* �ַ������� */
#define FONT_CHARSET_ASCII				"ASCII"
#define FONT_CHARSET_GB2312				"GB2312"
#define FONT_CHARSET_GB18030			"GB18030"
#define FONT_CHARSET_VIETNAM			"VIETNAM"
#define FONT_CHARSET_ARABIC				"ARABIC"
#define FONT_CHARSET_UNICODE			"UNICODE"
#define FONT_CHARSET_CP437				"CP437"
#define FONT_CHARSET_CP737				"CP737"
#define FONT_CHARSET_CP775				"CP775"
#define FONT_CHARSET_CP850				"CP850"
#define FONT_CHARSET_CP852				"CP852"
#define FONT_CHARSET_CP855				"CP855"
#define FONT_CHARSET_CP857				"CP857"
#define FONT_CHARSET_CP860				"CP860"
#define FONT_CHARSET_CP861				"CP861"
#define FONT_CHARSET_CP862				"CP862"
#define FONT_CHARSET_CP863				"CP863"
#define FONT_CHARSET_CP864				"CP864"
#define FONT_CHARSET_CP865				"CP865"
#define FONT_CHARSET_CP866				"CP866"
#define FONT_CHARSET_CP869				"CP869"
#define FONT_CHARSET_CP874				"CP874"
#define FONT_CHARSET_CP932				"CP932"
#define FONT_CHARSET_CP936				"CP936"
#define FONT_CHARSET_CP949				"CP949"
#define FONT_CHARSET_CP950				"CP950"
#define FONT_CHARSET_CP1250				"CP1250"
#define FONT_CHARSET_CP1251				"CP1251"
#define FONT_CHARSET_CP1255				"CP1255"
#define FONT_CHARSET_ISO8859_1			"ISO8859-1"
#define FONT_CHARSET_ISO8859_2			"ISO8859-2"
#define FONT_CHARSET_ISO8859_3			"ISO8859-3"
#define FONT_CHARSET_ISO8859_4			"ISO8859-4"
#define FONT_CHARSET_ISO8859_5			"ISO8859-5"
#define FONT_CHARSET_ISO8859_6			"ISO8859-6"
#define FONT_CHARSET_ISO8859_7			"ISO8859-7"
#define FONT_CHARSET_ISO8859_8			"ISO8859-8"
#define FONT_CHARSET_ISO8859_9			"ISO8859-9"
#define FONT_CHARSET_ISO8859_10			"ISO8859-10"
#define FONT_CHARSET_ISO8859_11			"ISO8859-11"
#define FONT_CHARSET_ISO8859_12			"ISO8859-12"
#define FONT_CHARSET_ISO8859_13			"ISO8859-13"
#define FONT_CHARSET_ISO8859_14			"ISO8859-14"
#define FONT_CHARSET_ISO8859_15			"ISO8859-15"


//////////////////////////////////////////////////////////////////////////
//���ͷ��ඨ��

typedef struct {
	uint32_t	m_code;
	int16_t		m_type;
	int16_t		m_originSuperX;
	int16_t		m_originSuperY;
	int16_t		m_orginX;
	int16_t		m_orginY;
	int16_t		m_blackX;
	int16_t		m_blackY;
	int16_t		m_ascsent;
	int16_t		m_descent;
	int16_t		m_incX;
	uint32_t	m_offset;
} font_pixmap_t;

typedef struct {
	char		FontLogo[16];
	char		FontName[32];
	int			avgCharWidth;
	int			CharHeight;
	int			fontSize;
	int			fontHeadSize;
	int			fontIndexOffset;
	int			fontIndexSizeof;
	int			fontIndexNum;
	int			fontDataOffset;
	int			fontDataSize;
	int			m_ascsent;
	int			m_descent;
} pixmap_table_t;

typedef struct {
	const char*	m_name;
	int			m_nrchars;
	int			m_bytes_per_char;
	int			m_bytes_maxlen_char;

	int			(*len_first_char)(const char *libcharset, const unsigned char* mstr);
	uint32_t	(*char_offset)(const char *libcharset, const unsigned char* mchar);

	int 		(*is_this_charset)(const char *libcharset, const unsigned char* charset);
} charset_ops_t;


typedef struct font_lib {
	char		m_flags[7];
	uint8_t		m_endian;
	char		m_version[8];
	char		m_font[32];
	char		m_charset[32];
	uint16_t	m_width;
	uint16_t	m_height;
	uint32_t	m_language;
	uint32_t	m_size;
	uint32_t	m_hsize;
	uint32_t	m_offset;
	uint16_t	m_defwidth;
	uint16_t	unused;
	uint8_t		m_info[24+128];
} font_lib_t;


typedef struct font_lib_class {
	char				m_charset[32];
	struct list_head	m_lib_node;
	font_lib_t			m_lib;
	pixmap_table_t		m_pixmaptable;
	const void *		m_libdata;

	int32_t				m_support;
	void *				m_private;
	int32_t				m_style;
	charset_ops_t *		m_charsetops;

	int		(*probe)(struct font_lib_class *thisclass);
	int		(*getpixmap)(struct font_lib_class *thisclass, uint32_t no, font_pixmap_t *pixmap);
	int		(*read)(struct font_lib_class *thisclass, loff_t offset, void *buffer, size_t size);
	int		(*getdot)(struct font_lib_class *thisclass, const char *charset, const char *text,
				  font_pixmap_t *pixmap, void *map);
	int	(*transcode)(const char *src, const char **desp);
} font_lib_class_t;


typedef struct font_class {
	struct list_head	m_font_node;		/* ���������������� */
	struct list_head	m_lib_header;		/* �ֿ�����ͷ */
	char				m_font[32]; 		/* ���� */
}font_class_t;

typedef struct {
	font_lib_class_t	*m_libclass;		/* ȡ������ѡ���� */
	char				m_libcharset[32];	/* �������ַ��� */
	unsigned char		unused[12];
} font_set_item_t;

typedef struct font_setting {
	font_set_item_t	m_preferred;		/* ��ѡ�ֿ� */
	font_set_item_t	m_secondary;		/* ��ѡ�ֿ� */
}font_setting_t;


/******************************************************************************
 * Function:	font_scan_format_h2v
 *
 * DESCRIPTION:	���ֿ��к���ɨ���ʽ��������ɨ�裬Ҫ�����Ϊ�ֽڶ���ģ�
 *				�������Ϊ6������ôҲ��Ҫ��һ���ֽ�����ʾ��6���㣬����
 *				Ϊ��λ��������Ϊ��λ����
 *				Դ���ݴ�С :  (width + 7) / 8 * height
 *				Ŀ�����ݴ�С: (height + 7) / 8 * width
 *
 * Input:		src : ����Դ����
 *				width :  ���
 *				height : �߶�
 *
 * Output:		OutBuf : ת���������
 *
 * Returns:		��
 *
 *
 * Modification history
 * ----------------------------------------------------------------------------
 *	Li.Hua	2007-10-18 14:54:09 ,  Written����֤û����
 ******************************************************************************/
void font_scan_format_h2v(const void *src, void *OutBuf, int32_t width, int32_t height);

/******************************************************************************
 * Function:	font_scan_format_v2h
 *
 * DESCRIPTION:	���ֿ�������ɨ���ʽ���ɺ���ɨ�裬Ҫ������Ϊ�ֽڶ���ģ�
 *				��������Ϊ6������ôҲ��Ҫ��һ���ֽ�����ʾ��6���㣬����
 *				Ϊ��λ��������Ϊ��λ����
 *				Դ���ݴ�С :  (height + 7) / 8 * width
 *				Ŀ�����ݴ�С: (width + 7) / 8 * height
 *
 * Input:		src : ����Դ����
 *				width :  ���
 *				height : �߶�
 *
 * Output:		OutBuf : ת���������
 *
 * Returns:		��
 *
 *
 * Modification history
 * ----------------------------------------------------------------------------
 *	Li.Hua	2007-10-18 14:54:09 ,  Written����֤û����
 ******************************************************************************/
void font_scan_format_v2h(const void *src, void *OutBuf, int32_t width, int32_t height);

/******************************************************************************
 * Function:	font_module_init
 *
 * DESCRIPTION:	�ֿ�ģ���ʼ��,�ú�����Ӧ����Ӧ�ò����
 *
 * Input:
 *
 * Output:
 *
 * Returns:		0 - ��ʼ���ɹ�
 *
 *
 * Modification history
 * ----------------------------------------------------------------------------
 *	Li.Hua	2008-06-26 10:52:53 ,  Written
 ******************************************************************************/
int font_module_init(void);

/******************************************************************************
 * Function:	font_load_lib
 *
 * DESCRIPTION:	�����ֿ�����
 *
 * Input:		fontlib - �ֿ��ļ�ͷ��Ϣ
 *				data	- �ֿ�����
 *
 * Output:
 *
 * Returns:		0	 - �����ֿ�ɹ�
 *				���� - ����ʧ��
 *
 * Modification history
 * ----------------------------------------------------------------------------
 *	Li.Hua	2008-06-26 10:53:20 ,  Written
 ******************************************************************************/
int font_load_lib(const font_lib_t *fontlib, const void *data);

/******************************************************************************
 * Function:	font_list_lib
 *
 * DESCRIPTION:	��ѯ��װ���ֿ�
 *
 * Input:		fontname - ������,���� "System", "����", NULL
 *				charset  - �ַ���,���� "ASCII", "GB18030", NULL
 *				maxitem  - ��ѯ������
 *
 * Output:		fontlib  - ��ѯ�����ֿ���Ϣ
 *
 * Returns:		��ѯ�����ֿ����
 *
 * Modification history
 * ----------------------------------------------------------------------------
 *	Li.Hua	2008-06-26 11:22:48 ,  Written
 ******************************************************************************/
int font_list_lib(const char *fontname, const char *charset, font_lib_t fontlib[], int maxitem);


/******************************************************************************
 * Function:	font_set_trans_func
 *
 * DESCRIPTION:	�����ַ�����ת����������,�������ý�����ת�����ת������,���ú�
 *				�����и��ַ���������Ч
 *
 * Input:		charset   - �ַ���,���� "ASCII", "GB18030", NULL
 *				transfunc - ת������
 *
 * Output:
 *
 * Returns:		0
 *
 * Notes:		���charsetΪNULL,���������ַ�������������ת��,����ֻ��ָ����
 *				����ת��. transfuncΪNULL��ζ�����ǰ�����ú���.
 *
 * Modification history
 * ----------------------------------------------------------------------------
 *	Li.Hua	2008-06-26 11:41:22 ,  Written
 ******************************************************************************/
int font_set_trans_func(const char *charset, int (*transfunc)(const char *, const char **));

int font_big2gb18030(const char *src, const char **des);
int font_gb2big5 (const char *src, const char **des);
int font_simplified2tradition(const char *src, const char **des);
int font_tradition2simplified(const char *src, const char **des);

int uni2char(const wchar_t uni, unsigned char *out, int boundlen);
int char2uni(const unsigned char *rawstring, int boundlen, wchar_t *uni);


#ifdef __cplusplus
}
#endif


#endif


