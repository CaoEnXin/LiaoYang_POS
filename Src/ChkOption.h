
/****************************************************************************
NAME
    checkopt.h - ����ѡ���麯��

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

// �ж��Ƿ񱣴潻�׼�¼
uint8_t ChkIfSaveLog(void);

// �����Ƚ��׸���ԭ��������־��Ϣ
uint8_t ChkIfUpdateLog(void);

// �ж��Ƿ�ͳ�Ƶ�Debit��������
uint8_t ChkIfDebitTxn(int iTransType);

// �ж��Ƿ�ͳ�Ƶ�Credit��������
uint8_t ChkIfCreditTxn(int iTransType);

// �ж��Ƿ�ͳ�Ƶ�Sale��������
uint8_t ChkIfSaleTxn(int iTransType);

// �ж��Ƿ��ӡ������ϸ
uint8_t ChkIfPrtDetill(int iTransType);

// �жϸÿ��Ƿ��Ǻ�������
uint8_t ChkIfBlkCard(char *pszCardNo);

// �ж�Ӧ�����ǲ���A�����-- ��00һ�����ǳɹ�
uint8_t ChkAcceptTxnCode(char *pszRespcode);

// �ж��ǲ��Ǵ������ֽ�
uint8_t ChkOnlyEcCard(uint8_t *sAppAid);

// �ж��Ƿ����ʧ�ܵĽ��׼�¼
uint8_t ChkExTxnLog(void);

// �ж��Ƿ���Ҫ����60.6 -- �������ѱ�־
uint8_t ChkSetPartSaleFlag(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _CHECKOPT_H

// end of file
