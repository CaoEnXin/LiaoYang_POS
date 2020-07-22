//  POS API
#ifndef  _POSAPI_H_
#define  _POSAPI_H_

//=============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <asm/socket.h>
#include <errno.h>
#include <wchar.h>

#include <arpa/inet.h>
#include <net/if.h>

#include "modem.h"
#include "barcode.h"
#include "pn512_drv.h"

#ifdef __cplusplus
extern "C"{
#endif

//=============================================================================

#ifndef     INT8
typedef     char                    INT8;
#endif
#ifndef     INT16
typedef     short                   INT16;
#endif
#ifndef     INT32
typedef     int                     INT32;
#endif
#ifndef     INT64
typedef     long long               INT64;
#endif

#ifndef     UINT8
typedef     unsigned char           UINT8;
#endif
#ifndef     UINT16
typedef     unsigned short          UINT16;
#endif
#ifndef     UINT32
typedef     unsigned int            UINT32;
#endif
#ifndef     UINT64
typedef     unsigned long long      UINT64;
#endif


#define     YES                     0x01
#define     NO                      0x00

#define     OK                      0x00
#define     ERROR                   0x01

#define     ON                      0x01
#define     OFF                     0x00

//#ifndef     MIN
//#define     MIN(X,Y)        ((X)>(Y) ? (Y) : (X))
//#endif
//
//#ifndef     MAX
//#define     MAX(X,Y)        ((X)>(Y) ? (X) : (Y))
//#endif

#ifndef     MIN
#define MIN(x, y) ({ \
	typeof(x) _x = (x);	\
	typeof(y) _y = (y);	\
	(void) (&_x == &_y);		\
	_x < _y ? _x : _y; })

#endif

#ifndef     MAX
#define MAX(x, y) ({ \
	typeof(x) _x = (x);	\
	typeof(y) _y = (y);	\
	(void) (&_x == &_y);		\
	_x > _y ? _x : _y; })

#endif



#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef ALIGN
#define ALIGN(x,a) (((x)+(a)-1)&~((a)-1))
#endif

//=======================SYSTEM BASIC API======================================
#define     EVENT_TIMEOUT           0x01
#define     EVENT_KEYPRESS          0x02
#define     EVENT_MAGSWIPED         0x04
#define     EVENT_ICCIN             0x08
#define     EVENT_PORTRECV          0x10
#define     EVENT_WNETRECV          0x20

int ReadDllVer(void);

INT32  sysCheckFirstRun(void);
void   sysReadSN(UINT8 *SerialNo);
void   sysReadVer(UINT8 *VerInfo);
INT32  sysReadConfig(UINT8 *ConfigInfo);
void   sysGetLastError(INT32 *ModInfo, INT32 *ErrorNum);
void   sysBeep(void);
void   sysBeef(UINT32 Freq, UINT32 BeepTime);
INT32  sysSetTime(UINT8 *TimeStr);
void   sysGetTime(UINT8 *TimeStr);
UINT32 sysGetTimerCount(void);
void   sysDelayMs(UINT32 CountMs);
void   sysTimerSet(UINT32 TimerNo, UINT32 CountMs);
UINT32 sysTimerCheck(UINT32 TimerNo);
void   sysPowerOff(void);
int32_t sysSetPowerKey(int32_t DelayMs);
int32_t sysGetPowerKeyStatus(void);

void   sysReset(void);
INT32  sysGetPowerStatus(void);
INT32  sysGetVolt(void);
void   sysAutoPowerSave(INT32 mode);
UINT32 sysPowerSave(UINT32 Event, UINT32 TimeOutMs);
INT32  sysEncode2bmp(const void *image, UINT32 width, UINT32 height, void *bmpfilebuffer);
int32_t accGetMotion(int32_t *Xaxis, int32_t *Yaxis, int32_t *Zaxis);
int32_t sysClipboardRead(int32_t ReadOffset, int32_t ReadByteLen, void *ReadBuf);
int32_t sysClipboardWrite(int32_t WriteOffset, int32_t WriteByteLen, const void *WriteBuf);
void sysSetPowerOffLogo(int x, int y, uint32_t showms, const void *logo);
int32_t sysGetCPU_UniqueSN(UINT8 *pbyUniqueSnOut);


//======================== STANDARD ALGORITHM =================================
#define TDEA_NONE               0x02            // 不做加解密操作(给ped的api使用)
#define TDEA_ENCRYPT            0x01            // TDES加密
#define TDEA_DECRYPT            0x00            // TDES解密
/* RSA public and private key. */
#define MIN_RSA_MODULUS_BITS    508
#define MAX_RSA_MODULUS_BITS    2048
#define MAX_RSA_MODULUS_LEN     ((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS      ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN       ((MAX_RSA_PRIME_BITS + 7) / 8)

#define US_PUK         1
#define UA_PUK         2


typedef struct
{
    UINT32 bits;                           /* length in bits of modulus */
    UINT8  modulus[MAX_RSA_MODULUS_LEN];                    /* modulus */
    UINT8  exponent[4];           /* public exponent */
    UINT16 usCRC;                /*CRC 校验码*/
} R_RSA_PUBLIC_KEY;

typedef struct
{
    UINT32 bits;                           /* length in bits of modulus */
    UINT8  modulus[MAX_RSA_MODULUS_LEN];                    /* modulus */
    UINT8  publicExponent[4];     /* public exponent */
    UINT8  exponent[MAX_RSA_MODULUS_LEN];          /* private exponent */
    UINT8  prime[2][MAX_RSA_PRIME_LEN];               /* prime factors */
    UINT8  primeExponent[2][MAX_RSA_PRIME_LEN];   /* exponents for CRT */
    UINT8  coefficient[MAX_RSA_PRIME_LEN];          /* CRT coefficient */
    UINT16 usCRC;                             /*CRC 校验码*/
} R_RSA_PRIVATE_KEY;

UINT16 CalcCRC16(UINT8 *Buf, UINT32 Len, UINT16 PreCrcValue);
INT32  TDEA(UINT8 *DataIn, UINT8 *DataOut, UINT8 *DesKey, UINT32 KeyLen, UINT32 Mode);
INT32  RSAPublicDecrypt(UINT8 *output,              /* output block */
                         UINT32 *outputLen,                /* length of output block */
                         UINT8 *input,                     /* input block */
                         UINT32 inputLen,                  /* length of input block */
                         R_RSA_PUBLIC_KEY *publicKey);     /* RSA public key */
INT32  RSAPrivateEncrypt(UINT8 *output,            /* output block */
                          UINT32 *outputLen,               /* length of output block */
                          UINT8 *input,                    /* input block */
                          UINT32 inputLen,                 /* length of input block */
                          R_RSA_PRIVATE_KEY *privateKey);  /* RSA private key */

typedef struct {
    UINT32 digest[5];		/* message digest */
    UINT32 count_lo, count_hi;	/* 64-bit bit count */
    UINT8  data[64];	/* SHA data buffer */
    INT32  local;			/* unprocessed amount in data */
} SHA_INFO;

void sha_init(SHA_INFO *sha_info);
void sha_update(SHA_INFO *sha_info, UINT8 *buffer, INT32 count);
void sha_final(UINT8 *digest, SHA_INFO *sha_info);
void Hash(UINT8 *DataIn, UINT32 DataInLen, UINT8 *DataOut);

typedef struct {
	UINT32 hash[4];
	UINT32 block[16];
	UINT64 byte_count;
}md5_ctx;

void md5_init(md5_ctx *tfm);
void md5_update(md5_ctx *tfm, const UINT8 *data, unsigned int len);
void md5_final(md5_ctx *tfm, UINT8 *out);
void md5(UINT8* DataIn,UINT32 DataInLen,UINT8* DataOut);

// AES algorithm
#define AES_ENCRYPT	1
#define AES_DECRYPT	0

#define AES_MAXNR 14
#define AES_BLOCK_SIZE 16

typedef struct{
		unsigned int rd_key[4 *(AES_MAXNR + 1)];
		int rounds;
}AES_KEY;

int aes_set_encrypt_key(const unsigned char *userKey,
								const int bits,
								AES_KEY *key);
int aes_set_decrypt_key(const unsigned char *userKey,
								const int bits,
								AES_KEY *key);
void aes_encrypt(const unsigned char *in,
						unsigned char *out,
						const AES_KEY *key);
void aes_decrypt(const unsigned char *in,
						unsigned char *out,
						const AES_KEY *key);

void aes_ecb_encrypt(const unsigned char *in,
							unsigned char *out,
							const AES_KEY *key,
							const int enc);

void aes_cbc_encrypt(const unsigned char *in,
							unsigned char *out,
							const unsigned long length,
							const AES_KEY *key,
							unsigned char *ivec,
							const int enc);

void aes_cfb128_encrypt(const unsigned char *in,
								unsigned char *out,
								const unsigned long length,
								const AES_KEY *key,
								unsigned char *ivec,
								int *num,
								const int enc);

void aes_cfb1_encrypt(const unsigned char *in,
							unsigned char *out,
							const unsigned long length,
							const AES_KEY *key,
							unsigned char *ivec,
							int *num,
							const int enc);

void aes_cfb8_encrypt(const unsigned char *in,
							unsigned char *out,
							const unsigned long length,
							const AES_KEY *key,
							unsigned char *ivec,
							int *num,
							const int enc);

void aes_cfbr_encrypt_block(const unsigned char *in,
									unsigned char *out,
									const int nbits,
									const AES_KEY *key,
									unsigned char *ivec,
									const int enc);

void aes_ofb128_encrypt(const unsigned char *in,
								unsigned char *out,
								const unsigned long length,
								const AES_KEY *key,
								unsigned char *ivec,
								int *num);

void aes_ctr128_encrypt(const unsigned char *in,
							unsigned char *out,
							const unsigned long length,
							const AES_KEY *key,
							unsigned char ivec[AES_BLOCK_SIZE],
							unsigned char ecount_buf[AES_BLOCK_SIZE],
							unsigned int *num);

/* For IGE, see also http://www.links.org/files/openssl-ige.pdf */
/* NB: the IV is _two_ blocks long */
void aes_ige_encrypt(const unsigned char *in,
							unsigned char *out,
							const unsigned long length,
							const AES_KEY *key,
							unsigned char *ivec,
							const int enc);
/* NB: the IV is _four_ blocks long */
void aes_bi_ige_encrypt(const unsigned char *in,
								unsigned char *out,
								const unsigned long length,
								const AES_KEY *key,
								const AES_KEY *key2,
								const unsigned char *ivec,
								const int enc);


//=========================== KEYBOARD ========================================
//  定义按键值
#define     KEY0                    0x30        //  '0'
#define     KEY1                    0x31        //  '1'
#define     KEY2                    0x32        //  '2'
#define     KEY3                    0x33        //  '3'
#define     KEY4                    0x34        //  '4'
#define     KEY5                    0x35        //  '5'
#define     KEY6                    0x36        //  '6'
#define     KEY7                    0x37        //  '7'
#define     KEY8                    0x38        //  '8'
#define     KEY9                    0x39        //  '9'
#define     KEY_BACKSPACE           0x08        //  退格
#define     KEY_CLEAR               0x2E        //  全部清除
#define     KEY_ALPHA               0x07        //  字母
#define     KEY_UP                  0x26        //  向上
#define     KEY_DOWN                0x28        //  向下
#define     KEY_FN                  0x15        //  功能
#define     KEY_MENU                0x14        //  菜单
#define     KEY_ENTER               0x0D        //  确认
#define     KEY_CANCEL              0x1B        //  取消
#define     KEY_PRNUP               0x19        //  打印向上走纸
#define     KEY_INVALID             0xFF        //  无效按键
#define     KEY_TIMEOUT             0x00        //  超时无按键

//  串输入控制和返回值
#define     KB_MAXLEN               256         //  串输入最大允许输入的长度
#define     KB_EN_REVDISP           0x01        //  1（0） 正（反）显示
#define     KB_EN_FLOAT             0x02        //  1（0） 有（否）小数点
#define     KB_EN_SHIFTLEFT         0x04        //  1（0） 左（右）对齐输入
#define     KB_EN_CRYPTDISP         0x08        //  1（0） 是（否）密码方式
#define     KB_EN_CHAR              0x10        //  1（0） 能（否）输字符
#define     KB_EN_NUM               0x20        //  1（0） 能（否）输数字
#define     KB_EN_BIGFONT           0x40        //  1（0） 大（小）字体
#define     KB_EN_PRESETSTR         0x80        //  1（0） 是（否）允许预设的字符串
#define     KB_EN_NEWLINE           0x100       //  1（0） 是（否）允许左对齐输入换行显示
//  预设几组常用的输入控制模式
#define     KB_BIG_ALPHA            0x1F5       //  大字体字符串输入
#define     KB_BIG_NUM              0x1E5       //  大字体数字串输入
#define     KB_BIG_PWD              0x6D        //  大字体密码输入
#define     KB_SMALL_ALPHA          0x1B5       //  小字体字符串输入
#define     KB_SMALL_NUM            0x1A5       //  小字体数字串输入
#define     KB_SMALL_PWD            0x2D        //  小字体密码输入

#define     KB_CANCEL               -1          //  串输入被用户按取消键退出
#define     KB_TIMEOUT              -2          //  串输入超时退出
#define     KB_ERROR                -3          //  串输入参数非法
//  背光模式
#define     LIGHT_OFF_TEMP          0           //  背光临时关闭(对于定时模式用)
#define     LIGHT_TIMER_MODE        1           //  背光定时模式
#define     LIGHT_ON_MODE           2           //  背光常开模式
#define     LIGHT_OFF_MODE          3           //  背光常关模式

typedef struct    //
{
	INT32 iAmount;  // 要显示的金额，如果为0或者正数，则合法；如果为负数，则不显示
	INT32 iStartX;
	INT32 iStartY;
	INT32 iDsplayMode;
}DISPLAY_AMOUNT;

//  API
INT32  kbhit(void);
INT32  kbGetKey(void);
INT32  kbGetKeyMs(INT32 TimeOutMs);
void   kbFlush(void);
void   kbSound(INT32 Mode,INT32 DelayTimeMs);
INT32  kbGetString(UINT32 Mode, INT32 MinLen, INT32 MaxLen, INT32 TimeOutMs, UINT8 *StrBuf);
INT32  kbGetHZ(INT32 MinLen, INT32 MaxLen, INT32 TimeOutMs, UINT8 *StrBuf);
void   kbLight(INT32 Mode);
void   kbSetLightTime(UINT32 LightTime);
void   kbSetSlipFW(INT32 Mode);


//=========================== LCD CONTROL =====================================
#define     SCR_WIDTH           128
#define     SCR_HEIGHT          64
//  lcdDisplay mode
#define     DISP_ASCII          0x00    //  display small font(6*8)
#define     DISP_CFONT          0x01    //  display the pointed font (default 8*16)
#define     DISP_CLRLINE        0x20    //  clear the line before display
#define     DISP_MEDIACY        0x40    //  display in the middle of line
#define     DISP_REVERSE        0x80    //  display inverse character
#define		DISP_INVLINE		0x10	//  display inverse whole line

#define     DISP_LEFT           0x100   //  display in the LEFT of line
#define     DISP_RIGHT          0x200   //  display in the RIGHT of line


//  ICON
#define     ICON_PHONE          1       //  phone
#define     ICON_SIGNAL         2       //  wireless signal
#define     ICON_PRINTER        3       //  printer
#define     ICON_ICCARD         4       //  ICC
#define     ICON_LOCK           5       //  lock
#define     ICON_BATTERY        6       //  battery
#define     ICON_EXTPOWER       7       //  external power
#define     ICON_UP             8       //  up
#define     ICON_DOWN           9       //  down
#define     ICON_ETH            10      //  ethernet
#define     ICON_MAIL           11      //  mail


#define     CLOSEICON           0       //  close icon
#define     OPENICON            1       //  open icon[for printer,ICC, lock, bttery, extpower, up, down, eth and mail]

void lcdCls(void);
void lcdClrLine(INT32 BeginLine, INT32 EndLine);
void lcdLight(INT32 Mode);
void lcdGoto(INT32 Col, INT32 Line);
void lcdSetInverse(INT32 Mode);
void lcdPrintf(UINT8 *FmtBuf, ...);
void lcdWprintf(wchar_t *FmtBuf, ...);
void lcdDisplay(INT32 Col, INT32 Line, UINT32 Mode, UINT8 *FmtBuf, ...);
void lcdWdisplay(INT32 Col, INT32 Line, UINT32 Mode, wchar_t *FmtBuf, ...);
void lcdStipple(INT32 X, INT32 Y, INT32 Color);
void lcdDrLogo(INT32 X, INT32 Y, UINT8 *pucBitMap);
void lcdSetIcon(INT32 IconNo, INT32 Mode);
INT32 lcdStore(INT32 *pCol, INT32 *pLine, UINT8* pucBitMapBuf);
void lcdRestore(INT32 Col, INT32 Line, UINT8* pucBitMapBuf);
INT32 lcdSetFont(INT8 *pFontName, INT8 *pFontCharacter, INT32 Style, INT32 Size, INT32 FontPriority);
void lcdGetCurFont(INT8 *pFontName, INT8 *pFontCharacter, INT32 *pStyle, INT32 *pSize, INT32 FontPriority);
void lcdDrawFrame(INT32 StartX, INT32 StartY, INT32 EndX, INT32 EndY, INT32 Mode);

//======================== MAGCARD READER CONTROL =============================
void   magOpen(void);
void   magClose(void);
void   magReset(void);
INT32  magSwiped(void);
UINT32 magRead(UINT8 *Track1, UINT8 *Track2, UINT8 *Track3);

//============================ ICC MODULE ======================================
// 卡座号
#define     ICC_USERCARD          0x00        // 半埋卡座
#define     ICC_SAM1              0x11        // SAM卡座1
#define     ICC_SAM2              0x12        // SAM卡座2
#define     ICC_SAM3              0x13        // SAM卡座1
#define     ICC_SAM4              0x14        // SAM卡座2

// ICC卡通信参数
#define     ICC_VOL_5V            0x05        //  5V卡
#define     ICC_VOL_3V            0x03        //  3V卡
#define     ICC_VOL_1_8V          0x01        //  1.8V卡

#define     ICC_ISO               0x00        // 符合ISO7816规范,上电初始化通信速度为372CLK/Bit
#define     ICC_EMV               0x01        // 符合EMV v4.1规范,上电初始化通信速度为372CLK/Bit
#define     ICC_19200             0x02        // 上电初始化通信速度为186CLK/Bit
#define     ICC_38400             0x03        // 上电初始化通信速度为93CLK/Bit

// ICC数据通信结构
typedef struct
{
    UINT8       CLA;
    UINT8       INS;
    UINT8       P1;
    UINT8       P2;
    UINT8       Lc;
    UINT8       DataIn[300];
    UINT8       LeFlag;
    UINT8       Le;
}APDU_SEND;

typedef struct
{
    UINT32      LenOut;
    UINT8       DataOut[300];
    UINT8       SWA;
    UINT8       SWB;
}APDU_RESP;

//  返回值定义
#define     ICC_SUCCESS          0x00      // 卡操作成功
#define     ICC_TIMEOUT          0x01      // 卡超时响应
#define     ICC_COMMERR          0x02      // 卡通信错误(奇偶错或LRC错)
#define     ICC_RESPERR          0x03      // 卡片返回信息错误
#define     ICC_CARDOUT          0x04      // 卡不在位
#define     ICC_NOPOWER          0x05      // 卡未上电
#define     ICC_ERRORSLOT        0x06      // 卡座错误
#define     ICC_ERRPARA          0x07      // 参数错误

// API
INT32 iccSetPara(INT32 CardSlot, UINT32 CardVol, UINT32 CardType);
INT32 iccDetect(INT32 CardSlot);
INT32 iccPowerUp(INT32 CardSlot, UINT32 *pAtrLen, UINT8 *pAtrData);
INT32 iccExchangeAPDU(INT32 CardSlot, APDU_SEND *pSendApdu, APDU_RESP *pRespApdu);
INT32 iccPowerDown(INT32 CardSlot);
INT32 iccWarmReset(INT32 CardSlot, UINT32 *pAtrLen, UINT8 *pAtrData);

//======================== PRINTER ============================================
#define     PRN_WIDTH           384         //  打印宽度像素

#define EM_BMP_ROTATE0          0x00
#define EM_BMP_ROTATE90         0x01
#define EM_BMP_ROTATE180        0x02
#define EM_BMP_ROTATE270        0x03

//  返回值
#define     PRN_OK              0x00
#define     PRN_BUSY            0x01
#define     PRN_PAPEROUT        0x02
#define     PRN_WRONG_PACKAGE   0x03
#define     PRN_FAULT           0x04
#define     PRN_TOOHEAT         0x08
#define     PRN_UNFINISHED      0xF0
#define     PRN_NOFONTLIB       0xFC
#define     PRN_OUTOFMEMORY     0xFE

INT32 prnInit(void);
INT32  prnSetFont(INT8 *pFontName, INT8 *pFontCharacter, INT32 Style, INT32 Size, INT32 FontPriority);
void  prnGetCurFont(INT8 *pFontName, INT8 *pFontCharacter, INT32 *pStyle, INT32 *pSize, INT32 FontPriority);
void  prnSetFontZoom(INT32 WidthZoom, INT32 HeightZoom);
void  prnSetSpace(INT32 Width, INT32 Height);
void  prnLeftIndent(INT32 Indent);
void   prnSetGray(INT32 Level);
INT32  prnGetCurLine(void);
void   prnStep(INT32 Line);
INT32  prnTemperature(void);
INT32  prnPrintf(UINT8 *FmtBuf, ...);
INT32 prnWprintf(wchar_t *FmtBuf, ...);
INT32  prnLogo(INT32 Rotate, UINT8 *pBitMapBuf);
INT32  prnStart(void);
INT32  prnStatus(void);
void prnEnBlackMark(int32_t Mode, int32_t BlackMarkHeight);
int32_t prnBlackMarkStep(int32_t MaxStep, int32_t BlackMarkNum);


//======================== FILE SYSTEM ========================================
//  返回值定义
#define     FILE_EXIST          1
#define     FILE_NOEXIST        2
#define     MEM_OVERFLOW        3
#define     TOO_MANY_FILES      4
#define     INVALID_HANDLE      5
#define     INVALID_MODE        6
#define     FILE_NOT_OPENED     8
#define     FILE_OPENED         9
#define     END_OVERFLOW        10
#define     TOP_OVERFLOW        11
#define     NO_PERMISSION       12
#define     FS_CORRUPT          13
//  文件打开模式，可使用"或"组合
#ifndef O_RDWR
#define     O_RDWR              02
#endif
#ifndef O_CREAT
#define     O_CREAT             0100
#endif
//  文件定位起点
#ifndef SEEK_SET
#define     SEEK_SET            0
#endif
#ifndef SEEK_CUR
#define     SEEK_CUR            1
#endif
#ifndef SEEK_END
#define     SEEK_END            2
#endif
//  文件类型
#define     FILE_TYPE_BOOTLOADER    0x80    //  BOOT LOADER软件
#define     FILE_TYPE_BOOT          0x81    //  BOOT软件
#define     FILE_TYPE_MONITOR       0x82    //  MONITOR软件
#define     FILE_TYPE_CONFIG        0x90    //  系统配置文件
#define     FILE_TYPE_FONT          0xA0    //  字库文件
#define     FILE_TYPE_TMS           0xA1    //  tms临时文件
#define     FILE_TYPE_PED           0xB0    //  PED文件

#define     FILE_TYPE_APP           0x01    //  0x01    //  应用程序
#define     FILE_TYPE_DSRM          0x02    //  0x07    //  应用程序显示注册文件
#define     FILE_TYPE_DATA          0x03    //  0x04    //  应用程序创建的数据文件
#define     FILE_TYPE_PARA          0x04    //  0x06    //  参数文件
#define     FILE_TYPE_ASPUBKEY      0x11    //  收单行签名公钥文件
#define     FILE_TYPE_AAPUBKEY      0x12    //  收单行KISSA授权公钥文件

typedef struct
{
    UINT8       fid;
    UINT8       attr;
    UINT8       type;
    UINT8       name[17];
    UINT32      length;
} FILE_INFO;

INT32  fileOpen(UINT8 *FileName, UINT32 Mode);
INT32  fileSeek(INT32 FileID, INT32 Offset, INT32 Whence);
INT32  fileRead(INT32 FileID, UINT8 *DataBuf, INT32 Len);
INT32  fileWrite(INT32 FileID, UINT8 *DataBuf, INT32 Len);
INT32  fileTruncate(INT32 FileID, INT32 Len);
INT32  fileClose(INT32 FileID);
INT32  fileRemove(UINT8 *FileName);
INT32  fileSize(UINT8 *FileName);
INT32  fileFreeSpace(void);
INT32  fileExist(UINT8 *FileName);
INT32  fileGetInfo(FILE_INFO *pFileInfo);
INT32  fileRename(UINT8 *OldFileName, UINT8 *NewFileName);

INT32  fileOpenExt(UINT8 *FileName, UINT32 Mode, UINT32 BelongApp, UINT32 FileType);
INT32  fileRemoveExt(UINT8 *FileName, UINT32 BelongApp, UINT32 FileType);
INT32  fileSizeExt(UINT8 *FileName, UINT32 BelongApp, UINT32 FileType);
INT32  fileExistExt(UINT8 *FileName, UINT32 BelongApp, UINT32 FileType);
INT32  fileRenameExt(UINT8 *OldFileName, UINT32 OldBelongApp, UINT32 OldFileType, UINT8 *NewFileName, UINT32 NewBelongApp, UINT32 NewFileType);

//======================= APP MANAGE ==========================================
typedef struct
{
    UINT8       AppName[32];
    UINT8       AppVer[16];
    UINT8       AppProvider[32];
    UINT8       BuildTime[14];
    UINT8       Descript[64];
    INT32       AppNo;
}APPINFO;

#define     LD_OK                   0x00        //  成功
#define     LDERR_GENERIC           0x01        //  失败
#define     LDERR_BAUDERR           0x03        //  波特率不支持
#define     LDERR_INVALIDTIME       0x04        //  非法时间
#define     LDERR_CLOCKHWERR        0x05        //  时钟硬件错误
#define     LDERR_SIGERR            0x06        //  验证签名失败
#define     LDERR_TOOMANYAPP        0x07        //  应用太多，不能下载更多应用
#define     LDERR_TOOMANYFILES      0x08        //  文件太多，不能下载更多文件
#define     LDERR_NOAPP             0x09        //  指定应用不存在
#define     LDERR_UNKNOWNAPP        0x0A        //  不可识别的应用类型
#define     LDERR_SIGTYPEERR        0x0B        //  签名的数据类型和下载请求类型不一致
#define     LDERR_SIGAPPERR         0x0C        //  签名的数据所属的应用名和下载请求应用名不一致
#define     LDERR_WRITEFILEFAIL     0x10        //  文件写入失败
#define     LDERR_NOSPACE           0x11        //  没有足够的空间
#define     LDERR_RESENDLASTPACK    0x20        //  校验错误，要求重新传输最后一个包
#define     LDERR_UNSUPPORTEDCMD    0xFF        //  不支持该命令


INT32  appReadInfo(UINT8 AppNo, APPINFO *CurAppInfo);
INT32  appReadState(UINT8 AppNo);
INT32  appRun(UINT8 AppNo);
int32_t appDelete(uint8_t AppNo, int32_t DelSubFile);

int32_t fileToKernel(uint8_t *srcFileName, uint32_t srcBelongApp, uint32_t srcFileType);
int32_t fileToPubKey(uint8_t *srcFileName, uint32_t srcBelongApp, uint32_t srcFileType, uint32_t PubKeyType);
int32_t fileToFont(uint8_t *srcFileName, uint32_t srcBelongApp, uint32_t srcFileType);
int32_t fileToApp(uint8_t *srcFileName, uint32_t srcBelongApp, uint32_t srcFileType, uint8_t *RetAppNo);
int32_t fileToDSRM(uint8_t *srcFileName, uint32_t srcBelongApp, uint32_t srcFileType, uint8_t dstAppNo);
int32_t fileToAppData(uint8_t *srcFileName, uint32_t srcBelongApp, uint32_t srcFileType,
                            uint8_t *dstFileName, uint32_t dstBelongApp, uint32_t dstFileType);
void GetKernelFileHash(uint32_t *FileLen, uint8_t *HashValue);
int32_t GetPubKeyFileHash(uint32_t PubKeyType, uint32_t *FileLen, uint8_t *HashValue);

//======================== APP PARA FILE ======================================
UINT8  paraReadData(UINT8 *ParaName, UINT8 *ParaValue);
UINT8  paraWriteData(UINT8 *ParaName, UINT8 *ParaValue);

//========================== PED  =============================================
//  Definition
#define     TYPE_ALL_KEY            0x00        //  all keys
#define     TYPE_MASTERKEY          0x01        //  MASTER KEY
#define     TYPE_PINKEY             0x02        //  PIN KEY
#define     TYPE_MACKEY             0x03        //  MAC KEY
#define     TYPE_DUKPTKEY           0x04        //  DUKPT KEY
#define     TYPE_SIEK               0x05        //  The key to encrypt the internal sensitive infomation(internal use)
#define     TYPE_TSSAK              0x06        //  The key for authenticate application
#define     TYPE_TEK                0x07        //  The temporary random key to transfer plain keys from KLD to PED
#define     TYPE_OVERFLOW           0xFE        //  The key expired(DUKPT)
#define     TYPE_INVALID            0xFF        //  Invalid key

#define     PARITY_NONE             0x00        //  No parity
#define     PARITY_ODD              0x10        //  Odd parity
#define     PARITY_EVEN             0x20        //  Even parity

#define     TDEA_MODE_ECB           0x0000      //  TDEA MODE ECB
#define     TDEA_MODE_CBC           0x0100      //  TDEA MODE CBC
#define     TDEA_MODE_CFB           0x0200      //  TDEA MODE CFB
#define     TDEA_MODE_OFB           0x0300      //  TDEA MODE OFB

#define     FORMAT_0                0x00        //  PIN BLOCK FORMAT 0
#define     FORMAT_1                0x01        //  PIN BLOCK FORMAT 1
#define     FORMAT_2                0x02        //  PIN BLOCK FORMAT 2
#define     FORMAT_3                0x03        //  PIN BLOCK FORMAT 3
#define     FORMAT_4                0x04        //  PIN BLOCK FORMAT 4
#define     FORMAT_EPS              0x0A        //  PIN BLOCK FORMAT EPS

#define     MAC_MODE_1              0x00        //  MAC method 1, TDES-TDES...TDES
#define     MAC_MODE_2              0x01        //  MAC method 2, XOR...XOR...TDES
#define     MAC_MODE_EMV            0x02        //  MAC for EMV EMV, DES-DES...TDES
#define     MAC_MODE_CUP            0x03        //  MAC for CUP, XOR-XOR...TDES(left)-XOR-TDES...
#define     MAC_MODE_DUKPT          0x04        //  MAC for DUKPT,Expand, XOR-XOR...TDES(left)-XOR-TDES...

#define     DUKPT_OLD_FUTUREKEY     0x00        //  Use the MacKey which is generated by the last FutureKey. It is recommand to use this if you generate PinBlock.(Recommend)
#define     DUKPT_NEW_FUTUREKEY     0x01        //  Use new FutureKey to generate new MacKey. It is recommended to use this if you don't generate PinBlock

#define     TYPE_PIN_INTERVAL   1           //  TYPE: The interval time between 2 times PIN input
#define     TYPE_PIN_DULATION   2           //  TYPE: The timeout of waiting PIN input
#define     TYPE_KI_INTERVAL    3           //  TYPE: The interval time between 2 times key loading
#define     TYPE_KI_DULATION    4           //  TYPE: The timeout of waiting key loading
#define     TYPE_TSS_INTERVAL   5           //  TYPE: The timeout of transaction sensitive service

typedef struct
{
    UINT8   Model[16];          //  The terminal type
    UINT8   HwVer[16];          //  The hardware version
    UINT8   SwVer[16];          //  The Software version
    INT32   Status;             //  PED status,PED_RET_OK/PED_RET_LOCKED
    INT32   MaxBps;             //  The highest speed which is support by PED
    INT32   MaxAppNum;          //  The maximum application number which is support by PED
    INT32   MasterKeyNum;       //  The maximum number of Master Key
    INT32   PINKeyNum;          //  The maximum number of PIN key
    INT32   MACKeyNum;          //  The maximum number of MAC Key
    UINT8   Res[184];           //  reserve
}tPedConfig;   //  PED infomation(256 bytes)

typedef struct
{
     UINT32     modlen;           // The module length of PIN encryption public key,
     UINT8      mod[256];         // The module data of PIN encryption public key,
     UINT32     explen;           // The exponet length of PIN encryption public key,
     UINT8      exp[4];           // The exponet data of PIN encryption public key,
     UINT8      iccrandomlen;     // The random data length from ICC
     UINT8      iccrandom[8];     // The random data from ICC
     UINT32     termrandomlen;    // The fill data length from terminal
     UINT8      termrandom[256];  // The fill data from terminal
} RSA_PINKEY;

//  return value
#define     PED_RET_OK              0x00        //  PED OK
#define     PED_RET_LOCKED          0x01        //  PED Locked
#define     PED_RET_ERROR           0x02        //  The others error
#define     PED_RET_COMMERR         0x03        //  Communicate with PED failed
#define     PED_RET_NEEDAUTH        0x04        //  Need auth before use sensitive service or expired
#define     PED_RET_AUTHERR         0x05        //  PED auth failed
#define     PED_RET_APPTSSAKERR     0x06        //  No TSSAK for this APP in APP
#define     PED_RET_PEDTSSAKERR     0x07        //  No TSSAK for this APP in PED
#define     PED_RET_KEYINDEXERR     0x08        //  The index of key incorrect
#define     PED_RET_NOKEY           0x09        //  No designated key in PED
#define     PED_RET_KEYFULL         0x0A        //  Key space is full
#define     PED_RET_OTHERAPPKEY     0x0B        //  The designated key is not belong to this APP
#define     PED_RET_KEYLENERR       0x0C        //  The key length error
#define     PED_RET_NOPIN           0x0D        //  Card holder press ENTER directly when enter PIN(no PIN)
#define     PED_RET_CANCEL          0x0E        //  Card holder press CANCEL to quit enter PIN
#define     PED_RET_TIMEOUT         0x0F        //  Timeout
#define     PED_RET_NEEDWAIT        0x10        //  Two frequent between 2 sensitive API
#define     PED_RET_KEYOVERFLOW     0x11        //  DUKPT KEY overflow
#define     PED_RET_NOCARD          0x12        //  No ICC
#define     PED_RET_ICCNOTPWRUP     0x13        //  ICC no power up
#define     PED_RET_PARITYERR       0x14        //  The parity incorrect
#define     PED_RET_UNSUPPORTED     0xFF        //  can not support

// 提供外部使用的API函数接口
int32_t PedSetTdesIV(int32_t IVlen, const uint8_t *IVvalue);
INT32  PedGetConfig(tPedConfig *Config);
INT32  PedGetKeyInfo(INT32 KeyType, UINT16 *UsedKeyNum, UINT16 *RemainKeyNum, UINT16 *KeyIdxList);
INT32  PedGetRandom(UINT8 *RandBuf);
INT32  PedRebuild(void);
INT32 PedWriteMasterKey(UINT32 Mode, UINT16  srcKeyID, UINT16 destKeyID, UINT16 KeyLen, UINT8 *KeyBuf);
INT32  PedWritePinKey(UINT32 Mode, UINT16  MasterKeyID, UINT16 PinKeyID, UINT16 PinKeyLen, UINT8 *PinKey);
INT32  PedWriteMacKey(UINT32 Mode, UINT16  MasterKeyID, UINT16 MacKeyID, UINT16 MacKeyLen, UINT8 *MacKey);
INT32  PedGetPin(UINT16 PinKeyID, UINT8 *ExpectPinLen, UINT8 *CardNo, INT32 Mode, UINT8 *PinBlock);
INT32  PedGetMac(UINT16 MacKeyID, UINT8 *DataIn, UINT16 InLen, INT32 Mode, UINT8 *MacOut);
INT32  PedGetPinDukpt(UINT8 *ExpectPinLen, UINT8 *CardNo, INT32 Mode, UINT8 *PinBlock, UINT8 *OutKsn);
INT32  PedGetMacDukpt(UINT8 *DataIn, UINT16 InLen, INT32 Mode, INT32 DukptMode, UINT8 *MacOut, UINT8 *OutKsn);
INT32  PedGetOffLinePlainPin(INT32 CardSlot, UINT8 *ExpectPinLen, APDU_SEND *pSendApdu, APDU_RESP *pRespApdu);
INT32  PedGetOffLineCipherPin(INT32 CardSlot, UINT8 *ExpectPinLen, RSA_PINKEY *RsaPinKey, APDU_SEND *pSendApdu, APDU_RESP *pRespApdu);
INT32  PedTDEA(UINT32 Mode, INT32 KeyType, UINT16  KeyID, UINT32 DataLen, UINT8 *InBuf, UINT8 *OutBuf);
INT32  PedDelKey(INT32 KeyType, UINT16  KeyID);
int32_t PedGetSensitiveTimer(int32_t SensitiveType);
int32_t DukptLoadInitKey(UINT16 KeyLen, const UINT8 *KeyData, const UINT8 *Ksn);
int32_t DukptLoadInitKeyExt(UINT16 KeyLen, const UINT8 *KeyData, const UINT8 *Ksn);
//======================= UART PORT ===========================================

//  定义逻辑串口号
#define     PORT_COM1				0x00        //  下载口
#define     PORT_MODEM				0x01        //  modem模块
#define     PORT_WNET				0x02        //  无线模块和以太网
#define     PORT_PINPAD             0x03        //  PINPAD口
#define     PORT_GPS                0x04        //  GPS
#define     PORT_INVALID            0x05        //  无效串口号

//  定义返回值
#define     PORT_RET_OK             0x00        //  成功
#define     PORT_RET_NOTEMPTY       0x01        //  发送缓冲还有数据
#define     PORT_RET_PORTERR        0x02        //  逻辑串口号非法
#define     PORT_RET_NOTOPEN        0x03        //  串口未打开
#define     PORT_RET_BUSY           0x05        //  没有物理串口资源分配
#define     PORT_RET_MODEM_INUSE    0xf0        //  MODEM通道正被系统占用（仅针对PortNum=PORT_MODEM）
#define     PORT_RET_PARAERR        0xfe        //  无效的通讯参数
#define     PORT_RET_TIMEOUT        0xff        //  超时

INT32  portOpen(INT32 PortNum, UINT8 *PortPara);
INT32  portClose(INT32 PortNum);
INT32  portSends(INT32 PortNum, UINT8 *SendBuf, UINT32 SendLen);
INT32  portPrintf(INT32 PortNum, UINT8 *FmtBuf, ...);
INT32  portRecv(INT32 PortNum, UINT8 *RecvBuf, UINT32 TimeOutMs);
INT32  portReset(INT32 PortNum);
INT32  portCheckSendBuf(INT32 PortNum);
INT32  portCheckRecvBuf(INT32 PortNum);

//======================= SLECARD卡模块========================================
// 定义卡片类型
#define     EM_SYNCARD_SLE4418             0x00             // 4418卡
#define     EM_SYNCARD_SLE4428             0x01             // 4428卡
#define     EM_SYNCARD_SLE4432             0x02             // 4432卡
#define     EM_SYNCARD_SLE4442             0x03             // 4442卡

// 函数返回值定义
#define     EM_SLECARD_SUCCESS             0x00            // 操作成功
#define     EM_SLECARD_ERROR               0x01            // 操作失败
#define     EM_SLECARD_CARDOUT             0x02            // 卡不在位
#define     EM_SLECARD_NOPOWER             0x03            // 卡未上电
#define     EM_SLECARD_NOVERIFY			   0x04			   // 卡片密码未校验
#define     EM_SLECARD_CHGABLE			   0x05			   // 该地址数据可修改
#define     EM_SLECARD_CHGDISABLE		   0x06			   // 该地址数据不可修改

// API函数定义
INT32 sleCardPowerUp(UINT8 *pucCardType, UINT8 *pucATRLen, UINT8 *pucATRData);
void  sleCardPowerDown(void);
INT32 sleCardVerify (UINT8 *ucPassWord , INT32 PassWordLen);
INT32 sleCardReadSCErr(UINT8* pucSCErrCount);
INT32 sleCardChangeSC(UINT8* pucNewSecCode, INT32 Len);
INT32 sleCardLock(void);
INT32 sleCardRead(UINT32 uiAddr, UINT32 uiLen, void* pvRecData );
INT32 sleCardReadStatus(UINT32 uiAddr, UINT8* pucStatus);
INT32 sleCardCheckData(UINT32 uiAddr, UINT8 ucData);
INT32 sleCardWrite (UINT32 uiAddr,UINT32 uiLen,void *pvDataIn);


// AT24CXX卡模块驱动
// 卡类型定义
#define AT24C01			1			// 128byte
#define AT24C02			2			// 256byte
#define AT24C04			4			// 512byte
#define AT24C08			8			// 1024byte
#define AT24C16			16			// 2048byte
#define AT24C32			32			// 4096byte
#define AT24C64			64			// 8192byte
#define AT24C128		128			// 16384byte
#define AT24C256		256			// 32768byte
#define AT24C512		512			// 65536byte
#define AT24C1024		1024	    // 131072byte

INT32 at24cxxOpen(UINT32 CardType);
INT32 at24cxxRead(INT32 StartAddr, INT32 Len, UINT8 *pucDataOut);
INT32 at24cxxWrite(INT32 StartAddr, INT32 Len, UINT8 *pucDataIn);
void at24cxxClose(void);

//=============================================================================
//  socket API 函数定义，含ppp

//typedef uint8_t         sa_family_t;
//typedef uint32_t        socklen_t;
//typedef uint32_t        in_addr_t;
//typedef uint16_t        in_port_t;
//
///* Supported address families. */
//#define AF_UNSPEC       0
//#define AF_UNIX         1           /* Unix domain sockets          */
//#define AF_LOCAL        1           /* POSIX name for AF_UNIX       */
//#define AF_INET         2           /* Internet IP Protocol         */
//#define AF_AX25         3           /* Amateur Radio AX.25          */
//#define AF_IPX          4           /* Novell IPX                   */
//#define AF_APPLETALK    5           /* AppleTalk DDP                */
//#define AF_NETROM       6           /* Amateur Radio NET/ROM        */
//#define AF_BRIDGE       7           /* Multiprotocol bridge         */
//#define AF_ATMPVC       8           /* ATM PVCs                     */
//#define AF_X25          9           /* Reserved for X.25 project    */
//#define AF_INET6        10          /* IP version 6                 */
//#define AF_ROSE         11          /* Amateur Radio X.25 PLP       */
//#define AF_DECnet       12          /* Reserved for DECnet project  */
//#define AF_NETBEUI      13          /* Reserved for 802.2LLC project*/
//#define AF_SECURITY     14          /* Security callback pseudo AF  */
//#define AF_KEY          15          /* PF_KEY key management API    */
//#define AF_NETLINK      16
//#define AF_ROUTE        AF_NETLINK  /* Alias to emulate 4.4BSD      */
//#define AF_PACKET       17          /* Packet family                */
//#define AF_ASH          18          /* Ash                          */
//#define AF_ECONET       19          /* Acorn Econet                 */
//#define AF_ATMSVC       20          /* ATM SVCs                     */
//#define AF_SNA          22          /* Linux SNA Project (nutters!) */
//#define AF_IRDA         23          /* IRDA sockets                 */
//#define AF_PPPOX        24          /* PPPoX sockets                */
//#define AF_WANPIPE      25          /* Wanpipe API Sockets          */
//#define AF_LLC          26          /* Linux LLC                    */
//#define AF_BLUETOOTH    31          /* Bluetooth sockets            */
//#define AF_MAX          32          /* For now..                    */

/* Socket types. */
//#define SOCK_STREAM     1           /* stream (connection) socket(TCP)   */
//#define SOCK_DGRAM      2           /* datagram (conn.less) socket(UDP)  */
//#define SOCK_RAW        3           /* raw socket                   */
//#define SOCK_RDM        4           /* reliably-delivered message   */
//#define SOCK_SEQPACKET  5           /* sequential packet socket     */
//#define SOCK_PACKET     10          /* linux specific way of        */
//                                    /* getting packets at the dev   */
//                                    /* level.  For writing rarp and */
//                                    /* other similar things on the  */
//                                    /* user level.                  */
//
#define	SOCK_MAX        (SOCK_PACKET + 1)


/* getsockopt/setsockopt level types. */
#define SOL_SOCKET      1
#define SOL_DEVICE      101

/* getsockopt/setsockopt option types. */
#define SO_RCVTIMEO     20
#define SO_SNDTIMEO     21

#define SO_BINDDEV      1001

/* getsockopt/setsockopt optval types. */
#define OPT_DEVWNET     1
#define OPT_DEVETH      2
#define OPT_DEVWIFI     3
#define OPT_DEVMODEM    4

#define __SOCK_SIZE__	16		/* sizeof(struct sockaddr)	*/

//struct in_addr
//{
//	in_addr_t       s_addr;
//};
//
//struct sockaddr_in
//{
//	uint8_t         sin_len;
//	sa_family_t     sin_family;
//	in_port_t       sin_port;
//	struct in_addr  sin_addr;
//	int8_t          sin_zero[__SOCK_SIZE__ - sizeof(uint8_t)- sizeof(sa_family_t) -
//			sizeof(in_port_t) - sizeof(struct in_addr)];
//};
//
//struct sockaddr
//{
//    uint8_t         sa_len;         //  本结构的长度(16)
//    sa_family_t     sa_family;	    /* address family, AF_xxx	一般为AF_INET*/
//    char            sa_data[14];	/* 14 bytes of protocol address	[0][1]为大端模式的端口号,[2][3][4][5]为从左到右的ip地址,其他为0*/
//};
//
//
///*  超时时间参数   */
//struct timeval
//{
//    int32_t         tv_sec;		/* seconds */
//    int32_t         tv_usec;	/* microseconds */
//};

//#define	ENOMEM              12      /* Out of memory 内存不够 */
//#define	EINVAL		        22      /* Invalid argument */
//#define	ENODATA             61      /* No data available */
//#define	ENONET              64      /* Machine is not on the network */
//#define	ENOLINK             67      /* Link has been severed */
//#define	ECOMM               70      /* Communication error on send */
//#define	EPROTO              71      /* Protocol error */
//#define	EOVERFLOW           75      /* Value too large for defined data type */
//#define	ENOTSOCK            88      /* Socket operation on non-socket */
//#define	EPROTOTYPE          91      /* Protocol wrong type for socket */
//#define	ENOPROTOOPT         92      /* Protocol not available */
//#define	EPROTONOSUPPORT     93      /* Protocol not supported */
//#define	ESOCKTNOSUPPORT     94      /* Socket type not supported */
//#define	EOPNOTSUPP          95      /* Operation not supported on transport endpoint */
//#define	EPFNOSUPPORT        96      /* Protocol family not supported */
//#define	ENETDOWN            100     /* Network is down 连接已关闭*/
//#define	ENETUNREACH         101     /* Network is unreachable 请求资源不存在，请重试*/
//#define	ENETRESET           102     /* Network dropped connection because of reset 连接被对方Reset*/
//#define	ECONNRESET          104     /* Connection reset by peer */
//#define	EISCONN             106     /* Transport endpoint is already connected 连接已建立*/
//#define	ENOTCONN            107     /* Transport endpoint is not connected 连接未成功建立*/
//#define	ESHUTDOWN           108     /* Cannot send after transport endpoint shutdown */
//#define	ETIMEDOUT           110     /* Connection timed out 超时*/
//#define	ECONNREFUSED        111     /* Connection refused 连接被对方abort*/
//#define	EHOSTDOWN           112     /* Host is down */
//#define	EHOSTUNREACH        113     /* No route to host 错误路由(route)*/

//  GPRS/CDMA macros
#define NET_OK              0       //  无错误，正常
#define NET_ERR_RSP         1024    //  "ERROR" is return from the module
#define NET_ERR_NOSIM       1025    //  SIM/UIM card is not inserted or not detected
#define NET_ERR_PIN         1026    //  SIM PIN is required
#define NET_ERR_PUK         1027    //  SIM PUK is required
#define NET_ERR_PWD         1028    //  SIM PIN is incorrect
#define NET_ERR_SIMDESTROY  1029    //  SIM card destroy
#define NET_ERR_CSQWEAK     1030    //  the signal is too weak, please check the antenna
#define NET_ERR_LINKCLOSED  1031    //  the module is offline
#define NET_ERR_LINKOPENING 1032    //  the module is connecting to the network
#define NET_ERR_DETTACHED   1033    //  Dettached
#define NET_ERR_ATTACHING   1034    //  the module is looking for the base station.
#define NET_ERR_EMERGENCY   1035    //  SIM/UIM is in emergency status
#define NET_ERR_RING        1036    //  detect ringing
#define NET_ERR_BUSY        1037    //  detect call in progress
#define NET_ERR_DIALING     1038    //  dialing
#define NET_ERR_UNKNOWN     1039    //  unknown module type
#define NET_ERR_ABNORMAL    1040    //  abnormal error
#define NET_ERR_NOMODULE    1041    //  no module in the unit
#define NET_ERR_NEEDRST     1042    //  need reset modem and init again
#define NET_ERR_USER_CANCEL 1043    //  User press key cancel
#define NET_ERR_NO_SUPPORT  1044    //  not support command
#define NET_ERR_SIM_BUSY    1045    //  SIM busy
#define NET_ERR_REG_FORBID  1046    //  Net work register forbid
#define NET_ERR_SRV_REJECT  1047    //  Server reject

//  Wi-Fi return code
#define EWIFI_BUSY          2001    //  RETURN I/BUSY
#define EWIFI_DONE          2002    //  RETURN I/DONE
#define EWIFI_ONLINE        2003    //  RETURN I/ONLINE
#define EWIFI_OFFLINE       2004    //  RETURN I/OFFLINE
#define EWIFI_RCV           2005    //  RETURN I/RCV
#define EWIFI_PART          2006    //  RETURN I/PART
#define EWIFI_EOP           2007    //  RETURN I/EOP
#define EWIFI_EOM           2008    //  RETURN I/EOM
#define EWIFI_MBE           2009    //  RETURN I/MBE
#define EWIFI_UPDATE        2010    //  RETURN I/UPDATE
#define EWIFI_ILLDELIMITER  2041    //  Illegal delimiter
#define EWIFI_ILLVAL        2042    //  Illegal value
#define EWIFI_EXPCR         2043    //  CR expected
#define EWIFI_EXPNUM        2044    //  Number expected
#define EWIFI_EXPCRORCOMMA  2045    //  CR or ‘,’ expected
#define EWIFI_EXPDNS        2046    //  DNS expected
#define EWIFI_EXPTILDE      2047    //  ‘:’ or ‘~’ expected
#define EWIFI_EXPSTR        2048    //  String expected
#define EWIFI_EXPEQL        2049    //  ‘:’ or ‘=’ expected
#define EWIFI_EXPTXT        2050    //  Text expected
#define EWIFI_SYNTAX        2051    //  Syntax error
#define EWIFI_EXPCOMMA      2052    //  ‘,’ expected
#define EWIFI_ILLCMD        2053    //  Illegal command code
#define EWIFI_SETPARA       2054    //  Error when setting parameter
#define EWIFI_GETPARA       2055    //  Error when getting parameter value
#define EWIFI_USERABORT     2056    //  User abort
#define EWIFI_BUILDPPP      2057    //  Error when trying to establish PPP
#define EWIFI_BUILDSMTP     2058    //  Error when trying to establish SMTP
#define EWIFI_BUILDPOP3     2059    //  Error when trying to establish POP3
#define EWIFI_MAXMINE       2060    //  Single session body for MIME exceeds the maximum allowed
#define EWIFI_INTMEMFAIL    2061    //  Internal memory failure
#define EWIFI_USERABORTSYS  2062    //  User aborted the system
#define EWIFI_HFCCTSH       2063    //  ~CTSH needs to be LOW to change to hardware flow control.
#define EWIFI_USERABORTDDD  2064    //  User aborted last command using ‘---’
#define EWIFI_RES2065       2065    //  RESERVED
#define EWIFI_RES2066       2066    //  RESERVED
#define EWIFI_CMDIGNORE     2067    //  Command ignored as irrelevant
#define EWIFI_SNEXIST       2068    //  iChip serial number already exists
#define EWIFI_HOSTTO        2069    //  Timeout on host communication
#define EWIFI_MODEMRESPOND  2070    //  Modem failed to respond
#define EWIFI_NODIALTONE    2071    //  No dial tone response
#define EWIFI_NOCARRIER     2072    //  No carrier modem response
#define EWIFI_DIALFAIL      2073    //  Dial failed
#define EWIFI_CONNLOST      2074    //  Modem connection with ISP lost -or- LAN connection lost -or- WLAN connection lost
#define EWIFI_ACCESSISP     2075    //  Access denied to ISP server
#define EWIFI_LOCPOP3       2076    //  Unable to locate POP3 server
#define EWIFI_POP3TO        2077    //  POP3 server timed out
#define EWIFI_ACCESSPOP3    2078    //  Access denied to POP3 server
#define EWIFI_POP3FAIL      2079    //  POP3 failed
#define EWIFI_NOSUITMSG     2080    //  No suitable message in mailbox
#define EWIFI_LOCSMTP       2081    //  Unable to locate SMTP server
#define EWIFI_SMTPTO        2082    //  SMTP server timed out
#define EWIFI_SMTPFAIL      2083    //  SMTP failed
#define EWIFI_RES2084       2084    //  RESERVED
#define EWIFI_RES2085       2085    //  RESERVED
#define EWIFI_WRINTPARA     2086    //  Writing to internal non-volatile parameters database failed
#define EWIFI_WEBIPREG      2087    //  Web server IP registration failed
#define EWIFI_SOCKIPREG     2088    //  Socket IP registration failed
#define EWIFI_EMAILIPREG    2089    //  E-mail IP registration failed
#define EWIFI_IPREGFAIL     2090    //  IP registration failed for all methods specified
#define EWIFI_RES2091       2091    //  RESERVED
#define EWIFI_RES2092       2092    //  RESERVED
#define EWIFI_RES2093       2093    //  RESERVED
#define EWIFI_ONLINELOST    2094    //  In Always Online mode, connection was lost and re-established
#define EWIFI_REMOTELOST    2096    //  A remote host, which had taken over iChip through the LATI port, was disconnected
#define EWIFI_RES2098       2098    //  RESERVED
#define EWIFI_RES2099       2099    //  RESERVED
#define EWIFI_DEFAULTRES    2100    //  Error restoring default parameters
#define EWIFI_NOISPDEF      2101    //  No ISP access numbers defined
#define EWIFI_NOUSRNDEF     2102    //  No USRN defined
#define EWIFI_NOPWDENTER    2103    //  No PWD entered
#define EWIFI_NODNSDEF      2104    //  No DNS defined
#define EWIFI_NOPOP3DEF     2105    //  POP3 server not defined
#define EWIFI_NOMBXDEF      2106    //  MBX (mailbox) not defined
#define EWIFI_NOMPWDDEF     2107    //  MPWD (mailbox password) not defined
#define EWIFI_NOTOADEF      2108    //  TOA (addressee) not defined
#define EWIFI_NOREADEF      2109    //  REA (return e-mail address) not defined
#define EWIFI_NOSMTPDEF     2110    //  SMTP server not defined
#define EWIFI_SDATAOVERFLOW 2111    //  Serial data overflow
#define EWIFI_ILLCMDMDM     2112    //  Illegal command when modem online
#define EWIFI_FWUPDT        2113    //  E-mail firmware update attempted but not completed. The original firmware remained intact.
#define EWIFI_EMAILPARAUPDT 2114    //  E-mail parameters update rejected
#define EWIFI_SNETPARA      2115    //  SerialNET could not be started due to missing parameters
#define EWIFI_PARSECA       2116    //  Error parsing a new trusted CA certificate
#define EWIFI_RES2117       2117    //  RESERVED
#define EWIFI_USRVPARA      2118    //  Protocol specified in the USRV parameter does not exist or is unknown
#define EWIFI_WLPPSHORT     2119    //  WPA passphrase too short - has to be 8-63 chars
#define EWIFI_RES2120       2120    //  RESERVED
#define EWIFI_RES2121       2121    //  RESERVED
#define EWIFI_SNETHIF0      2122    //  SerialNET error: Host Interface undefined (HIF=0)
#define EWIFI_SNETBAUD      2123    //  SerialNET mode error: Host baud rate cannot be determined
#define EWIFI_SNETHIFPARA   2124    //  SerialNET over TELNET error: HIF parameter must be set to 1 or 2
#define EWIFI_NOSOCK        2200    //  Socket does not exist
#define EWIFI_SOCKEMPTY     2201    //  Socket empty on receive
#define EWIFI_SOCKUNUSE     2202    //  Socket not in use
#define EWIFI_SOCKDOWN      2203    //  Socket down
#define EWIFI_NOAVAILDSOCK  2204    //  No available sockets
#define EWIFI_PPPOPEN       2206    //  PPP open failed for socket
#define EWIFI_CREATSOCK     2207    //  Error creating socket
#define EWIFI_SOCKSEND      2208    //  Socket send error
#define EWIFI_SOCKRECV      2209    //  Socket receive error
#define EWIFI_PPPDOWN       2210    //  PPP down for socket
#define EWIFI_SOCKFLUSH     2212    //  Socket flush error
#define EWIFI_NOCARRIERSOCK 2215    //  No carrier error on socket operation
#define EWIFI_GENERALEXCEPT 2216    //  General exception
#define EWIFI_OUTOFMEM      2217    //  Out of memory
#define EWIFI_LOCPORTINUSE  2218    //  An STCP (Open Socket) command specified a local port number that is already in use
#define EWIFI_LOADCA        2219    //  SSL initialization/internal CA certificate loading error
#define EWIFI_NEGSSL3       2220    //  SSL3 negotiation error
#define EWIFI_ILLSSLSOCK    2221    //  Illegal SSL socket handle. Must be an open and active TCP socket.
#define EWIFI_NOTRUSTCA     2222    //  Trusted CA certificate does not exist
#define EWIFI_RES2223       2223    //  RESERVED
#define EWIFI_SSLDECODE     2224    //  Decoding error on incoming SSL data
#define EWIFI_NOADDSSLSOCK  2225    //  No additional SSL sockets available
#define EWIFI_SSLPACKSIZE   2226    //  Maximum SSL packet size (2K) exceeded
#define EWIFI_SSNDSIZE      2227    //  AT+iSSND command failed because size of stream sent exceeded 2048 bytes
#define EWIFI_SSNDCHKSUM    2228    //  AT+iSSND command failed because checksum calculated does not match checksum sent by host
#define EWIFI_HTTPUNKNOWN   2300    //  HTTP server unknown
#define EWIFI_HTTPTO        2301    //  HTTP server timeout
#define EWIFI_HTTPFAIL      2302    //  HTTP failure
#define EWIFI_NOURL         2303    //  No URL specified
#define EWIFI_ILLHTTPNAME   2304    //  Illegal HTTP host name
#define EWIFI_ILLHTTPPORT   2305    //  Illegal HTTP port number
#define EWIFI_ILLURL        2306    //  Illegal URL address
#define EWIFI_URLTOOLONG    2307    //  URL address too long
#define EWIFI_WWWFAIL       2308    //  The AT+iWWW command failed because iChip does not contain a home page
#define EWIFI_MACEXIST      2400    //  MAC address exists
#define EWIFI_NOIP          2401    //  No IP address
#define EWIFI_WLANPWR       2402    //  Wireless LAN power set failed
#define EWIFI_WLANRADIO     2403    //  Wireless LAN radio control failed
#define EWIFI_WLENRESET     2404    //  Wireless LAN reset failed
#define EWIFI_WLANHWSETUP   2405    //  Wireless LAN hardware setup failed
#define EWIFI_WIFIBUSY      2406    //  Command failed because WiFi module is currently busy
#define EWIFI_ILLWIFICHNL   2407    //  Illegal WiFi channel
#define EWIFI_ILLSNR        2408    //  Illegal SNR threshold
#define EWIFI_RES2500       2500    //  RESERVED
#define EWIFI_COMMPLATFORM  2501    //  Communications platform already active
#define EWIFI_RES2502       2502    //  RESERVED
#define EWIFI_RES2503       2503    //  RESERVED
#define EWIFI_RES2504       2504    //  RESERVED
#define EWIFI_ALLFTPINUSE   2505    //  Cannot open additional FTP session – all FTP handles in use
#define EWIFI_NOTANFTP      2506    //  Not an FTP session handle
#define EWIFI_FTPSVRFOUND   2507    //  FTP server not found
#define EWIFI_CONNFTPTO     2508    //  Timeout when connecting to FTP server
#define EWIFI_FTPLOGIN      2509    //  Failed to login to FTP server (bad username or password or account)
#define EWIFI_FTPCMD        2510    //  FTP command could not be completed
#define EWIFI_FTPDATASOCK   2511    //  FTP data socket could not be opened
#define EWIFI_FTPSEND       2512    //  Failed to send data on FTP data socket
#define EWIFI_FTPDOWN       2513    //  FTP shutdown by remote server
#define EWIFI_RES2514       2514    //  RESERVED
#define EWIFI_TELNETSVR     2550    //  Telnet server not found
#define EWIFI_CONNTELNETTO  2551    //  Timeout when connecting to Telnet server
#define EWIFI_TELNETCMD     2552    //  Telnet command could not be completed
#define EWIFI_TELNETDOWN    2553    //  Telnet session shutdown by remote server
#define EWIFI_TELNETACTIVE  2554    //  A Telnet session is not currently active
#define EWIFI_TELNETOPENED  2555    //  A Telnet session is already open
#define EWIFI_TELNETBINMODE 2556    //  Telnet server refused to switch to BINARY mode
#define EWIFI_TELNETASCMODE 2557    //  Telnet server refused to switch to ASCII mode
#define EWIFI_RES2558       2558    //  RESERVED
#define EWIFI_RES2559       2559    //  RESERVED
#define EWIFI_RETRIEVERSP   2560    //  Client could not retrieve a ring response e-mail
#define EWIFI_SNETSOCKCLOSE 2561    //  Remote peer closed the SerialNET socket
#define EWIFI_PINGDEST      2570    //  PING destination not found
#define EWIFI_PINGNOREPLY   2571    //  No reply to PING request

typedef struct
{
    int             dhcp;       //  YES: enable dhcp, NO: disable dhcp
    struct in_addr  localip;    //  local ip
    struct in_addr  subnet;     //  net subnet addr;
    struct in_addr  gateway;    //  gate way addr;
    struct in_addr  dns1;       //  DNS1;
    struct in_addr  dns2;       //  DNS2;
}tcpipcfg_t;


int NetGetTCPIPCfg(int devtype, tcpipcfg_t *cfgparms);
int NetSetTCPIPCfg(int devtype, const tcpipcfg_t *cfgparms);

int WnetSelectModule(int slotNo);

/* ppp API */
int PPPLogin(int devtype, const uint8_t *APN, const uint8_t *name, const uint8_t *passwd,const uint32_t auth , int timeout);
int PPPLogout(int devtype);
int PPPCheck(int devtype);

/*  Socket API   */
int NetSocket(int family, int type, int protocol);
int NetBind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen);
int NetConnect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);
int NetConnectURL(int sockfd, const char *servurl, in_port_t servport);
int NetListen(int sockfd, int backlog);
int NetAccept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);
int NetSend(int sockfd, const void *buff, int nbytes, int flags);
int NetRecv(int sockfd, void *buff, int nbytes, int flags);
int NetSendto(int sockfd, const void *buff, int nbytes, int flags, const struct sockaddr *to, socklen_t addrlen);
int NetRecvfrom(int sockfd, void *buff, int nbytes, int flags, struct sockaddr *from, socklen_t *addrlen);
int NetClose(int sockfd);
int NetSetsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
int NetGetsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);

int SockAddrset(struct sockaddr *addr, const uint8_t *ipstr, const in_port_t port);
int SockAddrget(const struct sockaddr *addr, uint8_t *ipstr, in_port_t *port);
int NetGetPrompt(int IsChPrompt, int RetCode, void *Prompt);


//=============================================================================
//  wnet(gprs/cdma) API

#define  NETTYPE_GPRS           0x10
#define  NETTYPE_CDMA           0x20
#define  NETTYPE_WIFI           0x30

#define  NET_MOD_ME3000         0x11
#define  NET_MOD_GTM900         0x12
#define  NET_MOD_Q24            0x13
#define  NET_MOD_EM310          0x14
#define  NET_MOD_MC52I          0x15
#define  NET_MOD_MC55I          0x16
#define  NET_MOD_G200           0x17
#define  NET_MOD_G600           0x18
#define  NET_MOD_MG323          0x19

#define  NET_MOD_MG815          0x21
#define  NET_MOD_CM320          0x22
#define  NET_MOD_EM200          0x23
#define  NET_MOD_MC8331A        0x24

#define  NET_MOD_CONNECTONE     0x31

#define STK_MAX_ITEM        50          //  max item in one menu
#define STK_MAX_NAMELEN     255         //  max word len in an item

typedef struct{
	int     cur_cmdtype;            //universal cmd type
	int     numofitem;	            //for 36,37 as no of item, 33,34,35 as command detail
	char    title[STK_MAX_NAMELEN]; //for 36,37 as title, 33,34,35 as text
	char    content[STK_MAX_ITEM][STK_MAX_NAMELEN]; //for 36,37
	int     content_idx[STK_MAX_ITEM];  //for 36,37
	int     inputMin;               //for 34,35
	int     inputMax;	            //for 34,35
}tStkMenu;

typedef struct{
    int     index;          // the index in the sms memory
    int     status;         // 0:"REC UNREAD", 1:"REC READ", 2:"STO UNSENT", 3:"STO SENT", 4:"ALL"
    char    daoa[64];       // the telephone number of sms, like "10086000"
    char    scts[64];       // the time of sms, like "10/05/11,18:25:00+32"
    int     length;         // the length of sms content
    char    content[512];   // the content of sms
}tSms;


int32_t WnetInit(int32_t TimeoutMs);
int32_t WnetGetInfo(int32_t *NetType, int32_t *ModType, uint8_t *ModVer);
int32_t WnetReset(void);
int32_t WnetSignal(int32_t *Signal);
int32_t WnetCheckSim(void);
int32_t WnetInputSimPin(int8_t *Pin);
int32_t WnetGetIMEI(int8_t *IMEI);
int32_t WnetDnsResolve(const int8_t *name, int8_t *result, int32_t maxlen);
int Wnet_SetDnsServer(const int8_t *MDNS, const int8_t *SDNS);
int Wnet_GetDnsServer(int8_t *MDNS, int8_t *SDNS);

int32_t WPhoneStatus(void);
int32_t WPhoneStatusEx(int8_t * pPhoneNumber, uint32_t uiBuffSize);
int32_t WPhoneAnswer(void);
int32_t WPhoneHangUp(void);
int32_t WPhoneCall(uint8_t *PhoneNum);
int32_t WPhoneSendDTMF(uint8_t DTMFNum, uint32_t DurationMs);

int Stk_Init(int timeout_ms, int *ret_cmd_type);
int Stk_GetCurCmd(int *ret_cmd_type);
int Stk_ListMenu(int cmd_type, tStkMenu *ret_menu);
int Stk_Handle(int cmd_type, int *status, int *item_id, char *input_string, int *ret_cmd_type);

int Ussd_Cmd(int n, const void *sendstr, int dcs, int *m, void *recvstr, int *recvdcs);

//  get current sms service number
int Sms_GetCSCA(void *sca);

//  set sms service number
//  sca: sms service number
//  tosca: format, default:0, can be set to 145
int Sms_SetCSCA(const void *sca, int tosca);

//  list sms in the memory
//  listtype: 0:"REC UNREAD", 1:"REC READ", 2:"STO UNSENT", 3:"STO SENT", 4:"ALL"
//  maxno: the max number that smsdata can be stored into
//  recvnum: the number of sms which are successful read out
//  smsdata: the struct array to store sms
int Sms_Cmgl(int listtype, size_t maxno, size_t *recvnum, tSms smsdata[]);

//  read sms by index
//  index: the index in sms memory
int Sms_Cmgr(int index, tSms *smsdata);

//  delete sms from memory
//  index: the index in sms memory
int Sms_Cmgd(int index);

//  send sms
//  de: the target telephone number
//  content: the sms content to be sent
int Sms_Cmgs(const void *de, const void *content);

//int Sms_CmgsUcs2(const void *de, const void *content, int iLen);

//=============================================================================

#define WIFI_SECURITY_NONE          0       //  No security;
#define WIFI_SECURITY_WEP64         1       //  WEP-64;
#define WIFI_SECURITY_WEP128        2       //  WEP-128;
#define WIFI_SECURITY_WPATKIP       3       //  WPA-TKIP;
#define WIFI_SECURITY_WPA2AES       4       //  WPA2-AES;
#define WIFI_SECURITY_WPATKIP_E     5       //  WPA-TKIP Enterprise PEAP-MSCHAPv2;
#define WIFI_SECURITY_WPA2AES_E     6       //  WPA2-AES Enterprise PEAP-MSCHAPv2;

//  要启用省电模式，.IchipPowerSave=1, .WLANPowerSave=5是最优化的设置
typedef struct
{
    int     IchipPowerSave;     //  Wi-Fi module (iChip) power save (0..255, 0:Disable Power Save mode, 1..255:seconds without any activity)
    int     Channel;            //  Wireless LAN communication channel(0 - 13)
    int     WLANPowerSave;      //  Wireless LAN Power Save. (0(default): WiFi chipset Power Save mode is disabled. 1-5:The number of beacon periods during chipset remains in Power Save mode.)
    int     RoamingMode;        //  0(default):disable roaming mode, 1:enable roaming mode
    int     PeriodicScanInt;    //  Periodic WiFi Scan Interval(1~3600, default=5)
    int     RoamingLowSNR;      //  Sets a low SNR threshold for iChip in Roaming mode.(0 - 255dB, default:10dB)
    int     RoamingHighSNR;     //  Sets a high SNR threshold for iChip in Roaming mode.(0 - 255dB, default:30dB)
}WiFiConfig_t;      //  Wi-Fi通用参数设置

typedef struct
{
    char    SSID[64];           //  SSID
    int     WEPMode;            //  WEP mode: 0-disable; 1-enable 64-bit; 2-enable 128-bit
    int     WEPKeyIdx;          //  WEP key index (1 - 4)
    char    WEPKey[4][32];      //  4 sets of WEP Key
    char    WPAPSK[64];         //  Personal Shared Key Pass-Phrase, ("":WPA security is disabled, else enable for WPA-PSK encryption key)
}WiFiDefAP_t;

typedef struct
{
    char    SSID[64];           //  SSID
    int     SecurityType;       //  Sets the Wireless LAN security type.(0:No security; 1:WEP-64; 2:WEP-128; 3:WPA-TKIP; 4:WPA2-AES)
    char    WPAPSK[64];         //  Personal Shared Key Pass-Phrase.
    char    WEPKey[32];         //  WEP Key
}WiFiAPx_t;       //  can be configured up to 10 sets(0~9), the default set is No.0.


#define WIFI_STATUS_NOTPRESENT          0       //  Wlan adapter not present
#define WIFI_STATUS_DISABLED            1       //  Wlan adapter disabled
#define WIFI_STATUS_SEARCHING           2       //  Searching for initial connection
#define WIFI_STATUS_CONNECTED           4       //  Connected
#define WIFI_STATUS_OUTOFRANGE          5       //  Out of range

typedef struct
{
    int     Status;             /*  port status:    0:Wlan adapter not present;
                                                    1:Wlan adapter disabled;
                                                    2:Searching for initial connection;
                                                    4:Connected;
                                                    5:Out of range      */
    int     XferRate;           //  Transfer rate in the range 1..54
    int     SigLevel;           //  signal level [%], in the range 0..100
    int     LnkQual;            //  Link quality [%], in the range 0..100
    char    SSID[64];           //  SSID
    int     SecurityType;       //  The Wireless LAN security type.(0:No security; 1:WEP-64; 2:WEP-128; 3:WPA-TKIP; 4:WPA2-AES)
    int     Channel;            //  Current communication channel
}WiFiStatus_t;  // Wi-Fi连接状态

typedef struct
{
    char    SSID[64];           //  SSID
    int     SecurityType;       //  The Wireless LAN security type.(0:No security; 1:WEP-64; 2:WEP-128; 3:WPA-TKIP; 4:WPA2-AES)
    int     SigStrength;        //  Signal strength, 0-low, 1-good, 2-excellent
}WiFiAPInfo_t;      //  available ap info in surrounding area

//  Wi-Fi  API
int WifiGetCurConnect(WiFiStatus_t *CurStatus);
int WifiScanAP(int maxlistnum, WiFiAPInfo_t APlist[]);
int WifiGetLocalIP(char *ipstr);
int WifiSetParms(const WiFiConfig_t *parms);
int WifiSetDefAP(const WiFiDefAP_t *defAPParms);
int WifiSetAP(int index, const WiFiAPx_t *APxParms);
int WifiSoftReset(int mode);
int WifiPing(char *ipstr, int timeoutms);
int WifiGetMAC(void *macaddr);

//=============================================================================
//  NFC  API
// 卡类型定义
#define MIF_M1_S50      		0x00	// S50存储结构的M1卡
#define MIF_M1_S70      		0x01	// S70存储结构的M1卡
#define MIF_ULTRALIGHT  		0x02	// mifare ultralight
#define MIF_DESFIRE     		0x03	// mifare desfire
#define MIF_PRO         		0x04	// 支持ISO14443-4协议的TypeA卡
#define MIF_PRO_S50     		0x05
#define MIF_PRO_S70     		0x06
#define MIF_TYPEB       		0x07
#define MIF_TYPEB_TCL   		0x08	// 支持ISO14443-4协议的TypeB卡
#define MIF_FELICA      		0x09	// SonyFeliCa
#define MIF_UNKNOWEN_TYPEA 		0x0A	// 未知类型的TypeA卡

// S50/S70扇区认证密钥类型
#define MIF_KEYA        		0x00
#define MIF_KEYB        		0x01

// 定义当前的载波类型
#define MIF_TYPE_A      		0x00	// TypeA
#define MIF_TYPE_B      		0x01	// TypeB
#define MIF_TYPE_C1      		0x02	// FeliCa, 212kbits/s
#define MIF_TYPE_C2      		0x03	// FeliCa, 424kbits/s

// 指示灯标示
#define MIF_RED_LED             0x00	// 红色灯
#define MIF_BLUE1_LED           0x01	// 蓝色一号灯
#define MIF_BLUE2_LED           0x02	// 蓝色二号灯
#define MIF_GREEN_LED           0x03	// 绿色灯

// 定义返回值
#define MIF_RET_SUCCESS         0		// 操作正确
#define MIF_RET_TRANSERR        -1		// 通信失败
#define MIF_RET_TIMEOUT         -2		// 超时错误
#define MIF_RET_PROTERR         -3		// 数据错
#define MIF_RET_MULTICARD       -4		// 多卡存在
#define MIF_RET_NOCARD          -5		// 无卡
#define MIF_RET_ERRPARA         -6		// 参数错误
#define MIF_RET_NOACTIVED       -7		// 卡片未激活
#define MIF_RET_NOAUTH          -8		// 扇区未验证
#define MIF_RET_OVERFLOW        -9		// 内部缓冲区溢出
#define MIF_RET_AUTHERR         -10		// 认证失败
#define MIF_RET_VALUEERR        -11		// 值错误
#define MIF_RET_NOSUPPORTTCL    -12		// 该Tag不支持T=CL协议
#define MIF_RET_NOSUPPORTCMD    -13		// 该命令Tag不支持

//FOR Mifare Plus Card Err Code
#define MIF_RET_PLCMDFLOWERR    -16     // 太多读写操作导致命令溢出
#define MIF_RET_PLCRC1ERR       -17     // 错误的CRC在第一包
#define MIF_RET_PLCRC2ERR       -18     // 错误的CRC在第二包
#define MIF_RET_PLCONDITIONERR  -19     // 不满足访问条件
#define MIF_RET_PLMACERR        -20     // PCD发送的MAC错误
#define MIF_RET_PLBNOERR        -21     // PCD发送的块号错误
#define MIF_RET_PLNOBNOERR      -22     // PICC无此块号
#define MIF_RET_PLLENERR        -23     // PCD发送数据长度错误
#define MIF_RET_PLLOPERATEERR   -24     // PCD对PICC值相关操作错误

#define MIF_RET_PLPICCMACERR    -25     // PICC返回的MAC错误
#define MIF_RET_PLNOFIRAUTHERR  -26     // 未进行first auth


typedef struct
{
	UINT32 WaitTime_PollTypeA;		// TypeA寻卡期间单条指令等待时间(默认100us)
	UINT32 WaitTime_PollTypeB;		// TypeB寻卡期间单条指令等待时间(默认500etu)
	UINT32 WaitTime_Mifare;			// Mifare One卡操作期间指令等待时间(默认10240etu)
	UINT32 WaitTime_RATS;			// 激活Pro卡指令等待时间(默认500etu)
	UINT32 WaitTime_CardResponse;	// T=CL卡操作等待卡响应超时时间(默认573440etu)
	UINT32 WaitTime_CompleteCmd;	// 等待终端完成本次数据发送接收总时间(默认6000ms)
	UINT32 WaitTime_DelayBefore;	// 发送下一条指令前的等待时间(默认500us)
	UINT32 WaitTime_IdleRF;			// 复位载波的等待时间(默认6ms)
}MIF_TIME_STRUCT;

//for mifare plus auth struct
typedef struct
{
	UINT32 BlockNo;//块号
	UINT8 AESKeyType;//可为A/B块密钥，master key,configth key,L2 switch key,L3 switch key,SL1 Additional AES auth key
	UINT8 AESKey[16];
	UINT8 M1KeyType;
	UINT8 M1Key[6];
	UINT8 UID[4];//目前支持4字节UID卡
}MIF_PLUSAUTH_STRUCT;


// API函数定义
void mifOpen(void);							// 打开射频卡模块
void mifClose(void);							// 关闭射频卡模块
void mifSelectMifType(UINT32 Type);			// 选择调制载波类型
void mifResetRF(UINT32 dur_time);			// 复位载波

INT32 mifReqA(UINT8 *ATQA);					// 唤醒所有未被锁定的TypeA卡片
INT32 mifWupA(UINT8 *ATQA);					// 唤醒所有TypeA卡片(包括被锁定和未被锁定的)
INT32 mifAnticollion(UINT8 SEL, UINT8 *UID);	// TypeA 卡防冲突
INT32 mifSelect(UINT8 SEL, UINT8 *UID, UINT8 *SAK);		// TypeA卡激活
INT32 mifHaltA(void);							// TypeA卡片休眠或锁定
INT32 mifRATS(UINT32 *Len, UINT8 *Data);		// 激活T=CL卡片
INT32 mifPPPS(UINT8 BRit, UINT8 BRti);		// PPS协议

INT32 mifAuthByKey(UINT32 BlockNo, UINT32 KeyType, UINT8 *Key, UINT8 *UID);	// S50或者S70扇区/块密码验证
INT32 mifAuthByE2(UINT32 BlockNo, UINT32 KeyType, UINT8 *UID);	// S50或者S70扇区/块密码验证
INT32 mifReadBlock(UINT32 BlockNo, UINT8 *BlockData);	// 读取S50/S70的一个数据块信息(16字节)
INT32 mifWriteBlock(UINT32 BlockNo, UINT8 *BlockData);	// 修改S50/S70的一个数据块信息(16字节)
INT32 mifSetValue(UINT32 BlockNo, UINT32 Value);			// 将S50/S70的一个数据块设置成数值块，并赋予初始值
INT32 mifIncrement(UINT32 BlockNo, UINT32 Value);		// 增值操作
INT32 mifDecrement(UINT32 BlockNo, UINT32 Value);		// 减值操作
INT32 mifRestore(UINT32 BlockNo);							// 恢复操作
INT32 mifTransfer(UINT32 BlockNo);						// 转存操作
INT32 mifWriteKey(UINT32 KeyType, UINT32 Sector, UINT8 *Key);	// 写入密钥文件
INT32 mifWriteE2(UINT32 StartAddr, UINT32 Len, UINT8 *Data);	// 在密钥文件中保存一些额外信息
INT32 mifReadE2(UINT32 StartAddr, UINT32 Len, UINT8 *Data);	// 读取密钥文件的额外信息

INT32 mifReadULData(UINT32 PageAddr, UINT8 *Data);		// 读取ultra-light卡的一个页信息
INT32 mifWriteULData(UINT32 PageAddr, UINT8 *Data);		// 修改ultra-light卡的一个页信息(4字节)
INT32 mifWriteULData16(UINT32 PageAddr, UINT8 *Data);	// 修改ultra-light卡的一个页信息(16字节)

INT32 mifReqB(UINT32 AFI, UINT32 SlotNum,UINT32 *LenOfATQB, UINT8 *ATQB);	// 唤醒所有未被锁定的TypeB卡
INT32 mifWupB(UINT32 AFI, UINT32 SlotNum,UINT32 *LenOfATQB, UINT8 *ATQB);// 唤醒所有TypeB卡片(包括被锁定和未被锁定的)
INT32 mifSlotMarker(UINT32 Slot, UINT32 *LenOfATQB, UINT8 *ATQB);	// Slot-Marker指令
INT32 mifAttriB(UINT8 *UID, UINT8 BRit, UINT8 BRti);	// AttriB指令
INT32 mifHaltB(UINT8 *UID);			// 休眠或锁定TypeB卡

INT32 mifProcess(UINT32 SendLen, UINT8 *SendData, UINT32 *RecvLen,
					UINT8 *RecvData, UINT32 TimeOut);
INT32 mifExchangeAPDU(UINT32 SendLen, UINT8 *SendData, UINT32 *RecvLen, UINT8 *RecvData);

INT32 mifFeliCaExchange(UINT32 SendLen, UINT8 *SendData,
		                       UINT32 *RecvLen, UINT8 *RecvData,
		                       UINT32 TimeOutValue);

INT32 InListPassiveTarget(UINT8 Mode, INT32 InitLen, UINT8 *InitInfo, INT32 *CardInfoLen, UINT8 *CardInfo);
INT32 InATR(UINT32 *RespLen, UINT8 *ATRInfo);
INT32 InPSL(UINT8 BRit, UINT8 BRti);
INT32 InDataExchange(INT32 SendLen, UINT8 *SendData, UINT32 *RecvLen, UINT8 *RecvData);
void InRelease(void);
void mifLEDControl(UINT8 LEDNo, UINT8 Mode);

void mifSetTimeOut(MIF_TIME_STRUCT *ptTimeOutStruct);
void mifGetTimeOut(MIF_TIME_STRUCT *ptTimeOutStruct);

INT32 mifCheckOnlyOneCard(UINT8 Mode);

enum {
	NFC_OP_NULL			= 0x00,

	NFC_OP_POLL,			/* 轮询卡片 */
	NFC_OP_ANTICOLL, 		/* 防冲突处理 */
	NFC_OP_REMOVE,			/* 移除卡片 */
	NFC_OP_EXCHANGE_APDU,	/* 数据交换 */
};

int mif_open(const char *filename);
int mif_close(int fd);
int mif_ioctl(int fd, unsigned int cmd, unsigned long arg);
int emv_contactless_active_picc(int fd);
int emv_contactless_deactive_picc(int fd);
int emv_contactless_obtain_status(int fd, int *status);
int emv_contactless_get_lasterror(int fd, int *error);
int emv_contactless_exchange_apdu(int fd, uint32_t sendlen, const void *senddata,
									uint32_t *recvlen, void *recvdata);


//for mifare plus Api
INT32 mifPLAuthInAct(MIF_PLUSAUTH_STRUCT *AuthPara);//auth in active mode,flow auth
INT32 mifPLAuthInPro(const UINT32 AuthType,MIF_PLUSAUTH_STRUCT *AuthPara);//auth in protocol mode,firse auth by aes
//for L0 fun
INT32 mifPLWritePerso(const UINT16 BNo,UINT8 *Data);//write Card config key,master key,(L2 swtich key,L3 swtich key for L1) or other block key or block values
INT32 mifPLCommitPerso(void);//switch up card to L1 or L3
//for L1 fun
//the same with M1 card interface

//for L2 fun
//operate card is the same with M1 card interface
//addtional fun as follow
//以下两个接口仅限于数据块操作
INT32 mifPLMultiWriteBlock(const UINT32 StartBlockNo,const INT32 BlockCount,const UINT8 *BlockData);
INT32 mifPLMultiReadBlock(const UINT32 StartBlockNo, const INT32 BlockCount,UINT8 *BlockData);

//for ISO14443-4 fun
//operate card is differ with M1 card interface
INT32 mifPLWriteBlock(const UINT32 CommType,const UINT16 BNo,const INT32 BlockCount, const UINT8 *BlockData);
INT32 mifPLReadBlock(const UINT32 CommType,const UINT16 BNo,const INT32 BlockCount,UINT8 *BlockData);

//以下值操作仅限于L3安全级别数据块操作
INT32 mifPLSetValue(const UINT32 CommType,const UINT32 BNo,const INT32 BlockCount, const UINT32 Values);
INT32 mifPLIncreTrans(const UINT32 CommType,const UINT32 SrcBNo,const UINT32 DesBNo, const UINT32 Values);
INT32 mifPLDecreTrans(const UINT32 CommType,const UINT32 SrcBNo,const UINT32 DesBNo, const UINT32 Values);
INT32 mifPLIncrement(const UINT32 CommType,const UINT32 BNo ,const UINT32 Values);
INT32 mifPLDecrement(const UINT32 CommType,const UINT32 BNo, const UINT32 Values);
INT32 mifPLRestore(const UINT32 CommType,const UINT32 BNo);
INT32 mifPLTransfer(const UINT32 CommType,const UINT32 BNo);


INT32 mifExchangeData(UINT8 CrcMode,UINT32 SendLen, UINT8 *SendData, UINT32 *RecvLen, UINT8 *RecvData);



//=============================================================================


//=============================================================================

//=============================================================================
#ifdef __cplusplus
}
#endif

#endif
