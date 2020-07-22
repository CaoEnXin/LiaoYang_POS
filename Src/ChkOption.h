
/****************************************************************************
NAME
    checkopt.h - 定义选项检查函数

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    pyming     2011.08.12      - created
****************************************************************************/

#ifndef _CHECKOPT_H
#define _CHECKOPT_H

#include "posapi.h"
#include "posappapi.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// 判断是否保存交易记录
uint8_t ChkIfSaveLog(void);

// 撤消等交易更新原来交易日志信息
uint8_t ChkIfUpdateLog(void);

// 判断是否统计到Debit总数里面
uint8_t ChkIfDebitTxn(int iTransType);

// 判断是否统计到Credit总数里面
uint8_t ChkIfCreditTxn(int iTransType);

// 判断是否统计到Sale总数里面
uint8_t ChkIfSaleTxn(int iTransType);

// 判断是否打印交易明细
uint8_t ChkIfPrtDetill(int iTransType);

// 判断该卡是否是黑名单卡
uint8_t ChkIfBlkCard(char *pszCardNo);

// 判断应答码是不是A类代码-- 和00一样算是成功
uint8_t ChkAcceptTxnCode(char *pszRespcode);

// 判断是不是纯电子现金卡
uint8_t ChkOnlyEcCard(uint8_t *sAppAid);

// 判断是否存在失败的交易记录
uint8_t ChkExTxnLog(void);

// 判断是否不需要设置60.6 -- 部分消费标志
uint8_t ChkSetPartSaleFlag(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _CHECKOPT_H

// end of file
