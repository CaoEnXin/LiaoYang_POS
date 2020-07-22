/****************************************************************************
NAME
    Rs232.h - ���崮���շ�ͨѶ����ģ��

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    pyming     2010.12.29      - created
****************************************************************************/
//���Ӵ��ں�����ԭ����Commlib.c���涨�������뱾�ļ��г�ͻ�Ľ�����

#ifndef _RS232LIB_H
#define _RS232LIB_H

#define  STR_RS232_PARAM    "115200,8,n,1"

// for RS232 communication
#ifndef  STX
#define  STX             0x02
#define  ETX             0x03
#endif

#define  RS232_TX_PURGE    0x0001    // ���ͻ�����
#define  RS232_RX_PURGE    0x0002    // ���ջ�����

#define  ERR_RS232_OK              0   // �ɹ�
#define  ERR_RS232_FAILURE        -1   // ���ڲ���ʧ��
#define  ERR_RS232_RX_DATA        -2   // �������ݴ���
#define  ERR_RS232_RX_TIMEOUT     -3   // �������ݳ�ʱ
#define  ERR_RS232_RX_LENGTH      -4   // �������ݳ��ȳ���
#define  ERR_RS232_RX_CHECKSUM    -5   // У�����ݳ���

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//   �򿪴���
int  Rs232Open(int iportNum, char *pszPortPara);

//   �رմ���
int  Rs232Close(void);

//   ��⴮�ڻ������Ƿ�������,uiFlag ��ʾ���ͻ��ǽ��ջ�����
int  Rs232CheckPurge(unsigned int uiFlag);

//   ��⴮�ڻ������Ƿ�������, uiFlag ��ʾ���ͻ��ǽ��ջ�����
int  Rs232ResetPurge(unsigned int uiFlag);

//   ���ڷ�������  
int  MainPosRS232Txd(unsigned char*psTxdData, unsigned int uiDataLen, unsigned int uiTimeOut);

//   ���ڽ�������
//��ӿ���ĸPOS PC�˵Ľӿڽ����⣬���Ըú�����ͨ��
int  MainPosRS232Rxd(unsigned char *psRxdData, unsigned int uiExpLen, unsigned int uiTimeOut, unsigned int *puiOutLen);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _RS232_H */

