#ifndef     _TMSLIB_H_
#define     _TMSLIB_H_

#include "posapi.h"

#ifdef __cplusplus
extern "C"{
#endif

#define     TMS_PROTO_NEWTMS   0        //  tmsЭ������Ϊnewtms


#define     TMS_PROMPT_EN       0       //  ��ʾӢ�Ľ���
#define     TMS_PROMPT_CHN      1       //  ��ʾ���Ľ���

#define     TMS_COMM_GPRS       1       //  ʹ��socketͨѶ(gprs, cdma, lan...)
#define     TMS_COMM_CDMA       2       //  ʹ��socketͨѶ(gprs, cdma, lan...)
#define     TMS_COMM_PPP        3       //  ʹ��PPPͨ��
#define     TMS_COMM_MDMASY     4       //  ʹ��MODEM ASYNCͨ��
#define     TMS_COMM_UART       5       //  ʹ�ô���ͨ��

typedef struct
{
    int32_t     protocol;       //  tms��Э������
    int32_t     prompt;         //  ��ʾ����
    int32_t     enbreak;        //  ����������;�Ƿ���԰�ȡ�����ж�����, YES/NO
    int32_t     commtype;       //  ͨѶ������
    int32_t     commport;       //  ͨѶ���(���ں�, sockfd)
    uint8_t     termID[16];     //  �ն˺�
    int32_t     specifiedtmk;   //  �û�ָ����Կ, YES(�û�ָ��)/NO(�ڲ�Ĭ��)
    int32_t     tmklen;         //  ���������֤ʹ�õ���Կ����
    uint8_t     tmk[24];        //  ���������֤����Կ
}tmsctrl_t;


//  ����ֵ
//  �����ն˵ķ���ֵ:
#define E_CANCEL            1100    //  user cancel
#define E_DATAERR           1101    //  data error
#define E_HANDSHAKE         1102    //  handshake error
#define E_AUTH              1103    //  auth key error
#define E_CHECKSUM          1104    //  check sum error
#define E_NOSPACE           1105    //  no space to save
#define E_TASKNUM           1106    //  too many task num
#define E_NOTASK            1107    //  no task to update
#define E_UNCOMPLETE        1108    //  download not complete
#define E_SIGERR            1109    //  signature error
#define E_WRITEFAIL         1110    //  write file error
#define E_NOFILE            1111    //  can not find file
#define E_TOOMANYAPP        1112    //  too many app
#define E_UNKNOWNAPP        1113    //  unknown app format
#define E_SIGTYPEERR        1114    //  signature type error
#define E_NOAPP             1115    //  no app
#define E_SIGAPPERR         1116    //  app's signature error
#define E_TASKTYPE          1117    //  unsupport task type.

//  ���Է������ķ���ֵ:
#define TMS_ERROR_UNKOWN_TID			9001		//δ֪TID
#define TMS_ERROR_UNKOWN_SN				9002		//δ֪SN��
#define TMS_ERROR_UNKOWN_MANUFACTURES	9003		//δ֪����
#define TMS_ERROR_UNSPT_PACK_LEN		9004		//��֧�ֵ����ݰ�����
#define TMS_ERROR_UNSPT_COMPRESS_VER	9005		//��֧�ֵ�ѹ���㷨
#define TMS_ERROR_UNSPT_ENCRYPT_VER		9006		//��֧�ֵļ����㷨
#define TMS_ERROR_UNSPT_VERIFY_VER		9007		//��֧�ֵ�У���㷨
#define TMS_ERROR_UNSPT_COM_MODEL		9008		//��֧�ֵ�ͨѶ��ʽ
#define TMS_ERROR_UNSPT_AUTH_VER		9009		//��֧�ֵ������֤��ʽ
#define TMS_ERROR_AUTH_ERROR			9010		//�����֤����
#define TMS_ERROR_TASK_ERROR			9011		//ȡ�����б����
#define TMS_ERROR_UNKOWN_TASK_INDEX		9012		//�����������
#define TMS_ERROR_UNKOWN_FILE_ADDR		9013		//������ļ�ƫ�Ƶ�ַ
#define TMS_ERROR_UNKOWN				9014		//λ�ô���
#define TMS_ERROR_TIMEOUT				9015		//��ʱ��
#define TMS_ERROR_VERIFY_ERROR			9016		//У���
#define TMS_TASKLIST_NULL               9017		//�����б�Ϊ��
#define TMS_TASK_FORRBIDEN              9018		//�ն˱���ֹ����
#define TMS_ERROR_FILE_NOTEXIST         9019		//�Ҳ��������ļ�
#define TMS_ERROR_SERVER_MEMORY_ERROR   9020		//�����������ڴ�ʧ��
#define TMS_ERROR_FILE_READ_ERROR       9021		//���ļ�����
#define TMS_TRANDATA_MULTI_PACK         9022		//������͵ķ���ֵ�����Ǵ�����
#define TMS_ERROR_APP_FILE_ERROR        9023		//��Ӧ�ó����ļ�����
#define TMS_ERROR_PATH_ERROR            9024        //�Ҳ�������ָ�������ļ�
#define TMS_ERROR_FILE_CHANGED_ERROR    9025        //�����ļ�������


int32_t tmsSetDispLang(int32_t mode);
void tmsShowRet(int32_t Col, int32_t Line, int32_t Mode, int32_t Retcode);

int32_t tmsDownload(tmsctrl_t *ctrlpara);
int32_t tmsUpdate(int32_t prompt, int32_t *UpdateNum, int32_t *RemainNum);
int32_t tmsRemoveTask(int32_t prompt);
int32_t tmsGetAppNo(uint8_t *AppName, uint8_t *retAppNo);

#ifdef __cplusplus
}
#endif


#endif
