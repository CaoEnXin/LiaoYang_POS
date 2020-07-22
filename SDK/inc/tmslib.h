#ifndef     _TMSLIB_H_
#define     _TMSLIB_H_

#include "posapi.h"
#include "font.h"

#ifdef __cplusplus
extern "C"{
#endif

#define     TMS_PROTO_NEWTMS   0        //  tms protocol: newtms


#define     TMS_PROMPT_EN       0       //  prompt in English
#define     TMS_PROMPT_CHN      1       //  prompt in Chinese

#define     TMS_COMM_GPRS       1       //  uses socket comm(gprs, cdma, lan...)
#define     TMS_COMM_CDMA       2       //  uses socket comm(gprs, cdma, lan...)
#define     TMS_COMM_PPP        3       //  uses PPP comm
#define     TMS_COMM_MDMASY     4       //  uses MODEM ASYNC
#define     TMS_COMM_UART       5       //  uses RS232 comm

typedef struct
{
    int32_t     protocol;       //  tms protocol
    int32_t     prompt;         //  prompt control on LCD
    int32_t     enbreak;        //  whether CANCEL key is enabled to terminate downloading, YES/NO
    int32_t     commtype;       //  communication type
    int32_t     commport;       //  the handle of communication(port No., sockfd)
    uint8_t     termID[16];     //  terminal ID
    int32_t     specifiedtmk;   //  whether the tmk is given by application, YES/NO
    int32_t     tmklen;         //  the tmk length
    uint8_t     tmk[24];        //  the tmk content
}tmsctrl_t;


//  return code
//  return code from terminal:
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

//  return code from TMS server:
#define TMS_ERROR_UNKOWN_TID			9001		//  unknown TID
#define TMS_ERROR_UNKOWN_SN				9002		//  unknown SN
#define TMS_ERROR_UNKOWN_MANUFACTURES	9003		//  unknown vender name
#define TMS_ERROR_UNSPT_PACK_LEN		9004		//  unsupport data packet length
#define TMS_ERROR_UNSPT_COMPRESS_VER	9005		//  unsupport compress type
#define TMS_ERROR_UNSPT_ENCRYPT_VER		9006		//  unsupport encryption type
#define TMS_ERROR_UNSPT_VERIFY_VER		9007		//  unsupport checksum type
#define TMS_ERROR_UNSPT_COM_MODEL		9008		//  unsupport comm type
#define TMS_ERROR_UNSPT_AUTH_VER		9009		//  unsupport authentication type
#define TMS_ERROR_AUTH_ERROR			9010		//  authenticate failed
#define TMS_ERROR_TASK_ERROR			9011		//  get task error
#define TMS_ERROR_UNKOWN_TASK_INDEX		9012		//  task index error
#define TMS_ERROR_UNKOWN_FILE_ADDR		9013		//  unknown file addr
#define TMS_ERROR_UNKOWN				9014		//  unknown error
#define TMS_ERROR_TIMEOUT				9015		//  timeout
#define TMS_ERROR_VERIFY_ERROR			9016		//  checksum error
#define TMS_TASKLIST_NULL               9017		//  task is empty
#define TMS_TASK_FORRBIDEN              9018		//  the terminal is prevented to download
#define TMS_ERROR_FILE_NOTEXIST         9019		//  can not find the file to download
#define TMS_ERROR_SERVER_MEMORY_ERROR   9020		//  server memory error
#define TMS_ERROR_FILE_READ_ERROR       9021		//  reading file error
#define TMS_TRANDATA_MULTI_PACK         9022		//  return code of multi-packet(not an error code)
#define TMS_ERROR_APP_FILE_ERROR        9023		//  reading app file error
#define TMS_ERROR_PATH_ERROR            9024        //  can not find the path of downloading file
#define TMS_ERROR_FILE_CHANGED_ERROR    9025        //  the downloading file is updated


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
