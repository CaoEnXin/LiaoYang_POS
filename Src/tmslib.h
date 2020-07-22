#ifndef     _TMSLIB_H_
#define     _TMSLIB_H_

#include "posapi.h"

#ifdef __cplusplus
extern "C"{
#endif

#define     TMS_PROTO_NEWTMS   0        //  tms协议类型为newtms


#define     TMS_PROMPT_EN       0       //  提示英文界面
#define     TMS_PROMPT_CHN      1       //  提示中文界面

#define     TMS_COMM_GPRS       1       //  使用socket通讯(gprs, cdma, lan...)
#define     TMS_COMM_CDMA       2       //  使用socket通讯(gprs, cdma, lan...)
#define     TMS_COMM_PPP        3       //  使用PPP通信
#define     TMS_COMM_MDMASY     4       //  使用MODEM ASYNC通信
#define     TMS_COMM_UART       5       //  使用串口通信

typedef struct
{
    int32_t     protocol;       //  tms的协议类型
    int32_t     prompt;         //  显示控制
    int32_t     enbreak;        //  控制下载中途是否可以按取消键中断下载, YES/NO
    int32_t     commtype;       //  通讯的类型
    int32_t     commport;       //  通讯句柄(串口号, sockfd)
    uint8_t     termID[16];     //  终端号
    int32_t     specifiedtmk;   //  用户指定密钥, YES(用户指定)/NO(内部默认)
    int32_t     tmklen;         //  与服务器认证使用的密钥长度
    uint8_t     tmk[24];        //  与服务器认证的密钥
}tmsctrl_t;


//  返回值
//  来自终端的返回值:
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

//  来自服务器的返回值:
#define TMS_ERROR_UNKOWN_TID			9001		//未知TID
#define TMS_ERROR_UNKOWN_SN				9002		//未知SN号
#define TMS_ERROR_UNKOWN_MANUFACTURES	9003		//未知厂商
#define TMS_ERROR_UNSPT_PACK_LEN		9004		//不支持的数据包长度
#define TMS_ERROR_UNSPT_COMPRESS_VER	9005		//不支持的压缩算法
#define TMS_ERROR_UNSPT_ENCRYPT_VER		9006		//不支持的加密算法
#define TMS_ERROR_UNSPT_VERIFY_VER		9007		//不支持的校验算法
#define TMS_ERROR_UNSPT_COM_MODEL		9008		//不支持的通讯方式
#define TMS_ERROR_UNSPT_AUTH_VER		9009		//不支持的身份验证方式
#define TMS_ERROR_AUTH_ERROR			9010		//身份验证错误
#define TMS_ERROR_TASK_ERROR			9011		//取任务列表出错
#define TMS_ERROR_UNKOWN_TASK_INDEX		9012		//错误的任务编号
#define TMS_ERROR_UNKOWN_FILE_ADDR		9013		//错误的文件偏移地址
#define TMS_ERROR_UNKOWN				9014		//位置错误
#define TMS_ERROR_TIMEOUT				9015		//超时了
#define TMS_ERROR_VERIFY_ERROR			9016		//校验错
#define TMS_TASKLIST_NULL               9017		//任务列表为空
#define TMS_TASK_FORRBIDEN              9018		//终端被禁止下载
#define TMS_ERROR_FILE_NOTEXIST         9019		//找不到任务文件
#define TMS_ERROR_SERVER_MEMORY_ERROR   9020		//服务器申请内存失败
#define TMS_ERROR_FILE_READ_ERROR       9021		//读文件错误
#define TMS_TRANDATA_MULTI_PACK         9022		//多包传送的返回值，不是错误码
#define TMS_ERROR_APP_FILE_ERROR        9023		//读应用程序文件错误
#define TMS_ERROR_PATH_ERROR            9024        //找不到任务指定下载文件
#define TMS_ERROR_FILE_CHANGED_ERROR    9025        //下载文件被更新


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
