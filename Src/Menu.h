/****************************************************************************
NAME
	Menu.h - ���ײ˵���������

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

uint8_t SelectEcFunc(void);         // �����ֽ�˵�
uint8_t SelectEdFunc(void);         // ����Ǯ���˵�
uint8_t SelectInstalFunc(void);         // ���ڲ˵�
uint8_t SelectLoyaltFunc(void);         // ���ֲ˵�
uint8_t SelectMobileFunc(void);         // �ֻ�оƬ�˵�

uint8_t TransEcSale(int iTransType);       // �����ֽ�����
uint8_t TransEcTopup(int iTransType);      //  �����ֽ��ֵ
uint8_t TransEcVoidTopup(int iTransType);  //  �����ֽ��ֵ����
uint8_t TransEcBalance(void);               // �����ֽ����
uint8_t TransEcViewRecord(void);            // �����ֽ��ѯ��¼
uint8_t GetEcElementData(uint8_t *tag, uint8_t *len, uint8_t *val);  // ��ȡ�����ֽ����
uint8_t TransEcRefund(void);               // �����ֽ��˻�

uint8_t TransEdSale(int iTransType);        // ����Ǯ������
uint8_t TransEdTopup(int iTransType);       // ����Ǯ����ֵ
uint8_t TransEdBalance(void);               // ����Ǯ�����
uint8_t TransEdViewRecord(void);            // ����Ǯ����ѯ��¼

uint8_t TransInstallment(void);               // ���ڸ���
uint8_t TransVoidInstal(void);               // ���ڸ����

uint8_t TransLoyalt(int iTransType);               // ��������
uint8_t TransVoidLoyalt(int iTransType);           // ���ֳ���
uint8_t TransQueryLoyalt(void);          // ���ֲ�ѯ
uint8_t TransRefundLoyalt(void);         // �����˻�

uint8_t TransMobileSale(void);               // �ֻ�оƬ����
uint8_t TransVoidMobileSale(void);           // �ֻ�оƬ���ѣ�Ԥ��Ȩ��ɳ���
uint8_t TransMobilePreAuth(void);		     // �ֻ�оƬԤ��Ȩ
uint8_t TransMobileAuth(void);		         // �ֻ�оƬԤ��Ȩ�������,֪ͨ
uint8_t TransMobileVoidPreAuth(void);		 // �ֻ�оƬԤ��Ȩ����
uint8_t TransMobileRefund(void);         // �ֻ�оƬ�˻�
uint8_t TransMobileBalance(void);            // �ֻ�оƬ����ѯ   

uint8_t TransLytLogon(void);                 // ����ǩ��

#endif

// end of file


