/******************************************************************************
 * font.h
 *
 * Copyright (c) 2008 by Li.Hua, Newpos Technology Co.,Ltd.
 *
 * DESCRIPTION: - 字库操作方法定义
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

/*
 * CP932,    日文Shift-JIS， 如：日语
 * CP936,    简体中文GBK，   如：中文(中华人民共和国), 中文(香港特别行政区), 中文(新加坡)
 * CP949,    韩文，          如：朝鲜语
 * CP950,    繁体中文Big5，  如；中文(台湾), 中文(澳门特别行政区)
 * CP874,    泰文，          如：泰语
 * CP1250,   中欧，          如：捷克语,匈牙利语,波兰语,罗马尼亚语,克罗地亚语,斯洛伐克语,阿尔巴尼亚语,斯洛文尼亚语,塞尔维亚语(拉丁文)
 * CP1251,   西里尔文，      如：保加利亚语,俄语,乌克兰语,比利时语,马其顿语(FYROM),哈萨克语,吉尔吉斯语,鞑靼语,蒙古语,阿塞拜疆语,乌兹别克语,塞尔维亚语
 * CP1252,   西欧(拉丁文I)， 如：加泰隆语,丹麦语,德语,英语,西班牙语,芬兰语,法语,冰岛语,意大利语,荷兰语,挪威语,葡萄牙语,印度尼西亚语,巴士克语,南非语,法罗语,马来语,斯瓦希里语,加里西亚语,瑞典语
 * CP1253,   希腊文，        如：希腊语
 * CP1254,   土耳其文，      如：土耳其语,阿塞拜疆语,乌兹别克语
 * CP1255,   希伯来文，      如：希伯来语
 * CP1256,   阿拉伯文，      如：乌都语,波斯语,阿拉伯语(伊拉克,埃及,利比亚,阿尔及利亚,摩洛哥,突尼斯,阿曼,也门,叙利亚,约旦,黎巴嫩,科威特,阿联酋,巴林,卡塔尔)
 * CP1257,   波罗的海文，    如：爱沙尼亚语,拉脱维亚语,立陶宛语，
 * CP1258,   越南，          如：越南语
 *
 * ISO8859-1  字符集，也就是 Latin-1，是西欧常用字符，包括德法两国的字母。
 * ISO8859-2  字符集，也称为 Latin-2，收集了东欧字符。
 * ISO8859-3  字符集，也称为 Latin-3，收集了南欧字符。
 * ISO8859-4  字符集，也称为 Latin-4，收集了北欧字符。
 * ISO8859-5  字符集，也称为 Cyrillic，收集了斯拉夫语系字符。
 * ISO8859-6  字符集，也称为 Arabic，收集了阿拉伯语系字符。
 * ISO8859-7  字符集，也称为 Greek，收集了希腊字符。
 * ISO8859-8  字符集，也称为 Hebrew，收集了西伯莱 (犹太人) 字符。
 * ISO8859-9  字符集，也称为 Latin-5 或 Turkish，收集了土耳其字符。
 * ISO8859-10 字符集，也称为 Latin-6 或 Nordic，收集了北欧 (主要指斯堪地那维亚半岛) 的字符。
 * ISO8859-11 字符集，也称为 Thai，它是从泰国的 TIS620 标准字符集演化而来。
 * ISO8859-12 字符集，目前尚未定义。
 * ISO8859-13 字符集，也称为 Latin-7，主要函盖波罗的海 (Baltic) 诸国的文字符号，也补充一些在 Latin-6 中遗漏的拉托维亚 (Latvian) 字符。
 * ISO8859-14 字符集，也称为 Latin-8，它将 Latin-1 中的某些符号换成塞尔特语 (Celtic) 的字符。塞尔特族是指英伦外围的威尔斯人 (Welsh) 和盖尔人 (Gaelic)。
 * ISO8859-15 字符集，也称为 Latin-9，或者被匿称为 Latin-0，它将 Latin-1 中较少用到的符号删除，换成当初遗漏的法文和芬兰字母；还有，把英镑和日元之间的金钱符号，换成了欧盟货币符号。
 * ISO8859-16 字符集，正式编号为ISO/IEC 8859-16:2001，又称Latin-10或“东南欧语言”，是国际标准化组织于2000年制定的ISO/IEC 88598位字符集。
 */




#undef CFG_CHINESE_SUPPORT_USER_REGIN	/* GB18030汉字用户自定义区 */
#undef CFG_FONT_DEBUG					/* 调试开关 */
//#define CFG_FONT_DEBUG

#define SET_PRIMARY_FONT                0   //  设置首选字库
#define SET_SECONDARY_FONT              1   //  设置备选字库(针对首选字库为汉字，通过这个备选字库选择ascii部分的字体)

/* 字体定义 */
#define FONT_COURIER_NEW                "CourierNew"
#define FONT_COURIER_NEW_B              "CourierNewB"
#define FONT_SONG                       "宋体"
#define FONT_IMITATE_SONG               "仿宋"
#define FONT_SYSTEM                     "System"
#define FONT_ARIAL                      "Arial"
#define FONT_TAHOMA                     "Tahoma"
#define FONT_TAHOMAB                    "TahomaB"
#define FONT_VERDANA                    "Verdana"
#define FONT_DOTUM                      "Dotum"
#define FONT_KAPUTA                     "Kaputa"
#define FONT_LATHA                      "Latha"
#define FONT_TSC_KOMATHI                "TSC_Komathi"
#define FONT_WUJEYA                     "Wujeya"
#define FONT_SYS_TCVN3                  "SYS_TCVN3"
#define FONT_TERMINAL                   "Terminal"

/* 字符集定义 */
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
#define FONT_CHARSET_CP1252				"CP1252"
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
//类型分类定义

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
	struct list_head	m_font_node;		/* 将本类型链接起来 */
	struct list_head	m_lib_header;		/* 字库链表头 */
	char				m_font[32]; 		/* 字体 */
}font_class_t;

typedef struct {
	font_lib_class_t	*m_libclass;		/* 取点阵首选字体 */
	char				m_libcharset[32];	/* 操作用字符集 */
	unsigned char		unused[12];
} font_set_item_t;

typedef struct font_setting {
	font_set_item_t	m_preferred;		/* 首选字库 */
	font_set_item_t	m_secondary;		/* 备选字库 */
}font_setting_t;


/******************************************************************************
 * Function:	font_scan_format_h2v
 *
 * DESCRIPTION:	将字库中横排扫描格式换成竖排扫描，要求横排为字节对齐的，
 *				例如横排为6点阵，那么也需要用一个字节来表示这6个点，横排
 *				为高位在左，竖排为低位在上
 *				源数据大小 :  (width + 7) / 8 * height
 *				目的数据大小: (height + 7) / 8 * width
 *
 * Input:		src : 横排源数据
 *				width :  宽度
 *				height : 高度
 *
 * Output:		OutBuf : 转换后的数据
 *
 * Returns:		无
 *
 *
 * Modification history
 * ----------------------------------------------------------------------------
 *	Li.Hua	2007-10-18 14:54:09 ,  Written，验证没问题
 ******************************************************************************/
void font_scan_format_h2v(const void *src, void *OutBuf, int32_t width, int32_t height);

/******************************************************************************
 * Function:	font_scan_format_v2h
 *
 * DESCRIPTION:	将字库中竖排扫描格式换成横排扫描，要求竖排为字节对齐的，
 *				例如竖排为6点阵，那么也需要用一个字节来表示这6个点，横排
 *				为高位在左，竖排为低位在上
 *				源数据大小 :  (height + 7) / 8 * width
 *				目的数据大小: (width + 7) / 8 * height
 *
 * Input:		src : 横排源数据
 *				width :  宽度
 *				height : 高度
 *
 * Output:		OutBuf : 转换后的数据
 *
 * Returns:		无
 *
 *
 * Modification history
 * ----------------------------------------------------------------------------
 *	Li.Hua	2007-10-18 14:54:09 ,  Written，验证没问题
 ******************************************************************************/
void font_scan_format_v2h(const void *src, void *OutBuf, int32_t width, int32_t height);

/******************************************************************************
 * Function:	font_module_init
 *
 * DESCRIPTION:	字库模块初始化,该函数不应该在应用层调用
 *
 * Input:
 *
 * Output:
 *
 * Returns:		0 - 初始化成功
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
 * DESCRIPTION:	载入字库数据
 *
 * Input:		fontlib - 字库文件头信息
 *				data	- 字库数据
 *
 * Output:
 *
 * Returns:		0	 - 载入字库成功
 *				其它 - 载入失败
 *
 * Modification history
 * ----------------------------------------------------------------------------
 *	Li.Hua	2008-06-26 10:53:20 ,  Written
 ******************************************************************************/
int font_load_lib(const font_lib_t *fontlib, const void *data);

/******************************************************************************
 * Function:	font_list_lib
 *
 * DESCRIPTION:	查询安装的字库
 *
 * Input:		fontname - 字体名,例如 "System", "宋体", NULL
 *				charset  - 字符集,例如 "ASCII", "GB18030", NULL
 *				maxitem  - 查询最大个数
 *
 * Output:		fontlib  - 查询到的字库信息
 *
 * Returns:		查询到的字库个数
 *
 * Modification history
 * ----------------------------------------------------------------------------
 *	Li.Hua	2008-06-26 11:22:48 ,  Written
 ******************************************************************************/
int font_list_lib(const char *fontname, const char *charset, font_lib_t fontlib[], int maxitem);


/******************************************************************************
 * Function:	font_set_trans_func
 *
 * DESCRIPTION:	设置字符集的转换操作函数,例如设置将简体转繁体的转换函数,设置后
 *				对所有该字符集操作有效
 *
 * Input:		charset   - 字符集,例如 "ASCII", "GB18030", NULL
 *				transfunc - 转换函数
 *
 * Output:
 *
 * Returns:		0
 *
 * Notes:		如果charset为NULL,则会对所有字符集操作都进行转换,否则只对指定的
 *				进行转换. transfunc为NULL意味清除先前的设置函数.
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

//  return the bitmap width of string
int font_get_str_width(const char *pFontName, const char *pFontCharacter, int Style, int Size, const char *str);
//  return the bitmap width of wchar_t string
int font_get_wstr_width(const char *pFontName, const char *pFontCharacter, int Style, int Size, const wchar_t *str);



#ifdef __cplusplus
}
#endif


#endif


