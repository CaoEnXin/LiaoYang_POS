
#ifndef _NEWPOS_APP_API_H
#define _NEWPOS_APP_API_H


void BcdToAsc(uint8_t * sAscBuf, uint8_t * sBcdBuf, int iAscLen);
void BcdToAsc0(uint8_t * sAscBuf, uint8_t * sBcdBuf, int iAscLen);
void AscToBcd(uint8_t  * sBcdBuf, uint8_t  * sAscBuf, int iAscLen);

int BcdToLong(uint8_t  *sBcdBuf, int iBcdLen);

int   LongToAmount(uint8_t *szAmount, int lAmount);
uint8_t LongToDec(uint8_t * szAmount);

int CheckDate(char *sDate);

int CheckTime(char *sTime);

int CheckYYMM(char *sYYMM);

uint16_t ByteToShort(uint8_t * buf);

void ShortToByte(uint8_t * buf, uint16_t shortvar);

uint8_t BcdAdd(uint8_t *augend, uint8_t *addend, uint16_t len);

uint8_t ConvBcdAmount(uint8_t *bcdAmt, uint8_t *amount_ptr);

int DispMenu(const char * menu[], int iLines, int iTitle, int iStartKey, int iEndKey, int iTimeOut);

uint8_t OptionsSelect(char * szTitle,	int iCondition, char* szTruePrmpt, char* szFalsePrmpt,		
					char* option1, char* option2, uint8_t ucTimeOut);

/****************************************************************************
 功能描述:      蜂鸣器响声，并短暂延时
****************************************************************************/
void ErrorBeep(void);

  
/************************************************************
功    能： 取参数
*************************************************************/
//uint8_t GetPosPara(char *szName, uint8_t *szValue, uint8_t ucMinLen, uint8_t ucMaxlen);

/************************************************************
功    能： 仅接收'1''2',确认或者取消按键
*************************************************************/
uint8_t OneTwoSelect(const char *szItem, const char *choice1, const char *choice2);


void FailBeep(void);
void OkBeep(void);

// 字节高四位值
uint8_t DataHigh4Bit(uint8_t inchar);

// 字节低4位值
uint8_t DataLow4Bit(uint8_t inchar);


uint32_t AscToLong(uint8_t *sAsc, uint8_t ucLen);

#endif
