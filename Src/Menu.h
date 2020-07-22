/****************************************************************************
NAME
	Menu.h - 交易菜单公共函数

REFERENCE

MODIFICATION SHEET:
	MODIFIED   (YYYY.MM.DD)
	pyming		2011.08.01		- add precompile process
****************************************************************************/

#ifndef _TXN_MENU_H
#define _TXN_MENU_H

#include "posapi.h"
#include "posappapi.h"
#include "emvapi.h"
#include "emvtranproc.h"

uint8_t SelectEcFunc(void);         // 电子现金菜单
uint8_t SelectEdFunc(void);         // 电子钱包菜单
uint8_t SelectInstalFunc(void);         // 分期菜单
uint8_t SelectLoyaltFunc(void);         // 积分菜单
uint8_t SelectMobileFunc(void);         // 手机芯片菜单

uint8_t TransEcSale(int iTransType);       // 电子现金消费
uint8_t TransEcTopup(int iTransType);      //  电子现金充值
uint8_t TransEcVoidTopup(int iTransType);  //  电子现金充值撤消
uint8_t TransEcBalance(void);               // 电子现金余额
uint8_t TransEcViewRecord(void);            // 电子现金查询记录
uint8_t GetEcElementData(uint8_t *tag, uint8_t *len, uint8_t *val);  // 获取电子现金余额
uint8_t TransEcRefund(void);               // 电子现金退货

uint8_t TransEdSale(int iTransType);        // 电子钱包消费
uint8_t TransEdTopup(int iTransType);       // 电子钱包充值
uint8_t TransEdBalance(void);               // 电子钱包余额
uint8_t TransEdViewRecord(void);            // 电子钱包查询记录

uint8_t TransInstallment(void);               // 分期付款
uint8_t TransVoidInstal(void);               // 分期付款撤消

uint8_t TransLoyalt(int iTransType);               // 积分消费
uint8_t TransVoidLoyalt(int iTransType);           // 积分撤消
uint8_t TransQueryLoyalt(void);          // 积分查询
uint8_t TransRefundLoyalt(void);         // 积分退货

uint8_t TransMobileSale(void);               // 手机芯片消费
uint8_t TransVoidMobileSale(void);           // 手机芯片消费，预授权完成撤消
uint8_t TransMobilePreAuth(void);		     // 手机芯片预授权
uint8_t TransMobileAuth(void);		         // 手机芯片预授权完成请求,通知
uint8_t TransMobileVoidPreAuth(void);		 // 手机芯片预授权撤消
uint8_t TransMobileRefund(void);         // 手机芯片退货
uint8_t TransMobileBalance(void);            // 手机芯片余额查询   

uint8_t TransLytLogon(void);                 // 积分签到

#endif

// end of file


