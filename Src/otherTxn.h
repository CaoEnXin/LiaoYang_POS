/****************************************************************************
NAME
	otherTxn.h - �������׹�������

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


uint8_t SelectBookFunc(void);          // ԤԼ���Ѳ˵�
uint8_t SelectMotoFunc(void);         // �����˵�
uint8_t SelectMagTopupFunc(void);      // ��������ֵ�˵�


uint8_t TransBookSale(void);               // ԤԼ����
uint8_t TransVoidBookSale(void);           // ԤԼ���ѳ���


uint8_t TransMagTopupCash(void);           // �������ֽ��ֵ
uint8_t TransMagTopupAcct(void);           // �������ʻ���ֵ
uint8_t TransValidAcct(void);              // �ʻ���֤
uint8_t TransmagTopupConfirm(void);        // �������ֽ��ֵȷ��

uint8_t TransMotoSale(void);               // ��������
uint8_t TransVoidMotoSale(void);           // �������ѣ�Ԥ��Ȩ��ɳ���
uint8_t TransMotoPreAuth(void);		       // ����Ԥ��Ȩ
uint8_t TransMotoAuth(void);		       // ����Ԥ��Ȩ�������,֪ͨ
uint8_t TransMotoVoidPreAuth(void);		   // ����Ԥ��Ȩ����
uint8_t TransMotoRefund(void);             // �����˻�


uint8_t TransPartSale(void);               // ���ֿۿ�

#endif

// end of file


