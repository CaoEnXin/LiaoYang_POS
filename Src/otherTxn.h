/****************************************************************************
NAME
	otherTxn.h - 其它交易公共函数

REFERENCE

MODIFICATION SHEET:
	MODIFIED   (YYYY.MM.DD)
	pyming		2011.08.02		- add precompile process
****************************************************************************/

#ifndef _OTHER_TXN_H
#define _OTHER_TXN_H

#include "posapi.h"
#include "posappapi.h"
#include "emvapi.h"
#include "emvtranproc.h"


uint8_t SelectBookFunc(void);          // 预约消费菜单
uint8_t SelectMotoFunc(void);         // 订购菜单
uint8_t SelectMagTopupFunc(void);      // 磁条卡充值菜单


uint8_t TransBookSale(void);               // 预约消费
uint8_t TransVoidBookSale(void);           // 预约消费撤消


uint8_t TransMagTopupCash(void);           // 磁条卡现金充值
uint8_t TransMagTopupAcct(void);           // 磁条卡帐户充值
uint8_t TransValidAcct(void);              // 帐户验证
uint8_t TransmagTopupConfirm(void);        // 磁条卡现金充值确认

uint8_t TransMotoSale(void);               // 订购消费
uint8_t TransVoidMotoSale(void);           // 订购消费，预授权完成撤消
uint8_t TransMotoPreAuth(void);		       // 订购预授权
uint8_t TransMotoAuth(void);		       // 订购预授权完成请求,通知
uint8_t TransMotoVoidPreAuth(void);		   // 订购预授权撤消
uint8_t TransMotoRefund(void);             // 订购退货


uint8_t TransPartSale(void);               // 部分扣款

#endif

// end of file


