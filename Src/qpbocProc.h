/****************************************************************************
NAME
	qpbocProc.h - QPboc���״���������

REFERENCE

MODIFICATION SHEET:
	MODIFIED   (YYYY.MM.DD)
	pyming		2011.08.01		- add precompile process
****************************************************************************/

#ifndef _QPBOC_PROC_H
#define _QPBOC_PROC_H

#include "posapi.h"
#include "posappapi.h"
#include "emvapi.h"
#include "emvtranproc.h"
#include "qpbocapi.h"


void   DisplayQpbocError(int iErrorCode);
void   DisplayQpbocBalance(void);

// int    LoadQpbocParam(void);
// void   LoadDefaultQpbocParam(void);
// int    SaveQpbocParam(void);

uint8_t ProcQpbocTran(void);

uint8_t TransEcQuickSale(void);       // �����ֽ����֧�� ��qpboc��������

uint8_t CheckQpbocMaxAmount(uint32_t uiTxnAmount);
uint32_t GetMaxQpbocCfgAmount(void);

uint8_t  TransQpbocPreProc(void);  // QPBOC��Ӧ�ò���Ԥ����
uint8_t  GetAndSetQpbocElement(void); 
void     SaveIccFailureTranData(void);

//extern struct qpboc_parameters  gstQpbocParamters;

uint8_t SaveIccFailureFile(void);
uint8_t ReadIccFailureFile(uint32_t uiIndex);
uint8_t UpDateIccFailureFile(uint32_t uiIndex);

uint8_t ViewIccFailureLog(void);

int  TransBatchUploadQpbocFailureTxnLog(int *piTotalNum, uint8_t ucSendFlag);

#endif

// end of file
