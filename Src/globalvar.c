
#include "posapi.h"
#include "posmacro.h"
#include "emvapi.h"
#include "glbvar.h"
#include "M1_tranproc.h"


struct NEWPOS_COM_STRUCT PosCom;
struct NEWPOS_COM_STRUCT BackPosCom;
struct _total_strc stTotal;  
struct _total_m1Enrich_strc stM1EnrichTotal;

struct _limit_total_strc stLimitTotal; 
struct _oper_structure operStrc[MAX_OPER_NUM];
struct _TRANS_CTRL_PARAM stTransCtrl;
struct _NEWPOS_PARAM_STRC stPosParam;    
struct temp_global_var_strc stTemp;
struct NEWPOS_YKTENRICH_LOG_STRUCT  gstYktEnrichLog;

struct _EPP_CONFIGURE_ *glbEpp;
int    icc_fd = ICC_USERCARD;
int    iPsam_fd; 
int    imif_fd = -1;    

uint16_t  usSendPacketLen;  
uint16_t  usRecvPacketLen;
uint8_t	sSendPacketBuff[MAX_PACKET_LEN];	
uint8_t	sRecvPacketBuff[MAX_PACKET_LEN];	


struct emv_core_init  tEMVCoreInit;
struct terminal_mck_configure tConfig;
struct emv_callback_func tEMVCallBackFunc;

// struct terminal_aid_info gTAidInfo;

TermAidList glTermAidList[MAX_APP_NUM];
int		glTermAidNum;
DIAL_STAT   dial_stat;	


ACQUIRERIDNAME sAcqList[MAX_ACQUIRER_NUM]=
{
	{"01020000", "��������"},
	{"01030000", "ũҵ����"},
	{"01040000", "�й�����"},
	{"01050000", "��������"},
	{"01000000", "���������"},
	{"03010000", "��ͨ����"},
	{"03020000", "��������"},
	{"03030000", "�������"},
	{"03040000", "��������"},
	{"03050000", "��������"},
	{"03060000", "�㷢����"},
	{"03070000", "�����"},
	{"03080000", "��������"},
	{"03090000", "��ҵ����"},
	{"03100000", "�ַ�����"},
	{"04478210", "��������"},
	{"00010000", "��������"},
	{"04105840", "����ƽ������"},
	{"05105840", "����ƽ������"},
	{"14045840", "����ũ������"},
	{"        ", "        "}
};

struct errinfostru sRetErrInfo[ERR_INFO_NUM] = {
	{"01",	"[01]  ��ֿ���  ", "�뷢��������ϵ"},
	{"03",	"[03]            ", "��Ч�̻�"},
	{"04",	"[04]            ", "�˿���û��"},
	{"05",	"[05]            ", "�ֿ�����֤ʧ��"},
	{"10",	"[10]  ���׳ɹ�  ", "��Ϊ���ֳж�"},
	{"11",	"[11]            ", "�ɹ���VIP�ͻ�"},
	{"12",	"[12]            ", "��Ч����"},
	{"13",	"[13]            ", "��Ч���"},
	{"14",	"[14]            ", "��Ч����"},
	{"15",	"[15]            ", "�˿��޶�Ӧ������"},
	{"21",	"[21]    �ÿ�    ", "δ��ʼ����˯�߿�"},
	{"22",	"[22] ��������,��", "����������������"},
	{"25",	"[25]û��ԭʼ����", ",����ϵ������"},
	{"30",	"[30]            ", "������"},
//	{"31",	"[31]           ", "�˿���������"},
	{"34",	"[34]            ", "���׿�,�̿�"},
	{"38",	"[38]����������","����, ���뷢������ϵ"},
	{"40",	"[40]   ������   ", "��֧�ֵĽ�������"},
	{"41",	"[41]    ��ʧ��  ",  "��û��(POS)"},
	{"43",	"[43]            ", "���Կ�����û��"},
	{"51",	"[51]            ", "��������"},
	{"54",	"[54]            ", "�ÿ��ѹ���"},
	{"55",	"[55]    �����  ", "          "},
	{"57",	"[57]            ", "�������˿�����"},
	{"58",	"[58]������������", "�ÿ��ڱ��ն˽��д˽���"},
	{"59",	"[59]            ", "��ƬУ���"},
	{"61",	"[61]            ", "���׽��� "},
	{"62",	"[62]            ", "�����ƵĿ�"},
	{"64",	"[64]  ���׽��  ", "��ԭ���ײ�ƥ��"},
	{"65",	"[65]            ", "�������Ѵ�������"},
//	{"66",	"[66]  ����ʧ��", "��ϵ�յ��л�����"},
	{"68",	"[68]            ", "���׳�ʱ��������"},
	{"75",	"[75]            ", "��������������"},
	{"90",	"[90]  ϵͳ����  ", "���Ժ�����"},
	{"91",	"[91]������״̬��", "����,���Ժ�����"},
	{"92",	"[92]  ��������·", "�쳣,���Ժ�����"},
	{"94",	"[94]  �ܾ�, �ظ�", "����,���Ժ�����"},
	{"96",	"[96]  �ܾ�,���� ", "�����쳣�����Ժ�����"},
	{"97",	"[97]            ", "�ն�δ�Ǽ�"},
	{"98",	"[98]            ", "��������ʱ"},
	{"99",	"[99]  PIN��ʽ�� ", "������ǩ��"},
	{"A0",  "[A0]  MACУ��� ", "������ǩ��"},
	{"A1",  "[A1]            ", "ת�˻��Ҳ�һ��"},
	{"A2",  "[A2]  ���׳ɹ�  ", "���򷢿���ȷ��"},
	{"A3",  "[A3]            ", "�˻�����ȷ"},
	{"A4",  "[A4]  ���׳ɹ�  ", "���򷢿���ȷ��"},
	{"A5",  "[A5]  ���׳ɹ�  ", "���򷢿���ȷ��"},
	{"A6",  "[A6]  ���׳ɹ�  ", "���򷢿���ȷ��"},
	{"A7",	"[A7]  �ܾ�,���� ", "�����쳣�����Ժ�����"}
};


NEWPOS_8583PACKET_STRC TrTable[TABLES_LEN] = {
	{POS_SALE,         "00", "22", "000"},
	{POS_PREAUTH,      "06", "10", "000"},
	{POS_AUTH_CM,      "06", "20", "000"},
	{POS_PREAUTH_VOID, "06", "11", "000"},
	{POS_AUTH_VOID,    "06", "21", "000"},
	{POS_SALE_VOID,    "00", "23", "000"}, 
	{POS_REFUND,       "00", "25", "000"}, 
	{POS_PREAUTH_ADD,  "60", "10", "000"},
	{POS_QUE,          "00", "01", "000"},
	{POS_REVERSE,      "00", "00", "000"}, 
	{POS_LOGON,        "00", "00", "001"}, 
	{POS_LOGOFF,       "00", "00", "002"}, 
	{POS_SETT,         "00", "00", "201"}, 
	{POS_BATCH_UP,     "00", "00", "201"}, 
	{POS_DOWNLOAD_PARAM,   "00", "00", "360"}, 
	{POS_UPLOAD_PARAM, "00", "00", "362"}, 
	{RESPONSE_TEST,    "00", "00", "301"}, 
	{OFF_SALE ,        "00", "30", "000"}, 
	{OFF_ADJUST  ,     "00", "32", "000"}, 
	{ADJUST_TIP,       "00", "34", "000"}, 
	{OFF_SALE_BAT,     "00", "30", "000"}, 
	{OFF_ADJUST_BAT,   "00", "32", "000"}, 
	{ADJUST_TIP_BAT,   "00", "34", "000"}, 
	{POS_REFUND_BAT,   "00", "25", "000"},
	{POS_OFF_CONFIRM,  "06", "24", "000"},
	{OFF_CONFIRM_BAT,  "06", "24", "000"},
	{DOWNLOAD_EMV_CAPK,  "00", "00", "370"},
	{DOWNLOAD_EMV_PARAM, "00", "00", "380"},
	{QUERY_EMV_CAPK,     "00", "00", "372"},
	{QUERY_EMV_PARAM,    "00", "00", "382"},
	{END_EMV_CAPK,    "00", "00", "371"},
	{END_EMV_PARAM,   "00", "00", "381"},
	{ICC_OFFSALE,     "00", "36", "000"},
	{ICC_BATCHUP,     "00", "00", "203"},

	//�����ֽ���
	{EC_QUICK_SALE,   "00", "36", "000"},         //�����ֽ����֧��
	{QPBOC_ONLINE_SALE, "00", "22", "000"},      //QPBOC��������
	{EC_NORMAL_SALE,  "00", "36", "000"},       //�����ֽ���֧ͨ��
	{EC_TOPUP_CASH,   "91", "46", "000"},       //�����ֽ�Ȧ��-�ֽ�Ȧ��
	{EC_TOPUP_SPEC,   "91", "45", "000"},        //�����ֽ�Ȧ��-ָ���˻�Ȧ��
	{EC_TOPUP_NORAML, "91", "47", "000"},       //�����ֽ�Ȧ��-��ָ���˻�Ȧ��
	{EC_VOID_TOPUP,   "91", "51", "000"},       //�����ֽ��ֽ�Ȧ�泷��
	{EC_BALANCE,      "00", "22", "000"},        //�����ֽ�����ѯ
	{EC_REFUND,       "00", "27", "000"},        //�����ֽ�����ѯ
	{EC_QUE_TXN,      "00", "22", "000"},        //�����ֽ��׼�¼��ѯ
	//����Ǯ����
	{ED_SALE,         "91", "22", "000"},         //����Ǯ��֧��
	{ED_TOPUP_CASH,   "91", "41", "000"},         //����Ǯ��Ȧ��-�ֽ�Ȧ��
	{ED_TOPUP_SPEC,   "91", "40", "000"},         //����Ǯ��Ȧ��-ָ���˻�Ȧ��
	{ED_TOPUP_NORAML, "91", "42", "000"},         //����Ǯ��Ȧ��-��ָ���˻�Ȧ��
	{ED_BALANCE,      "00", "22", "000"},         //����Ǯ������ѯ
	{ED_QUE_TXN,      "00", "22", "000"},         //����Ǯ�����׼�¼��ѯ
	// ������
	{POS_INSTALLMENT,  "64", "22", "000"},      //��������
	{POS_VOID_INSTAL,  "64", "23", "000"},      //�������ѳ���
	// ������
	{POS_LOYATL_ISSUER,      "65", "22", "000"},      //�����л�������
	{POS_LOYATL_ALL,         "65", "22", "000"},        //���˻�������
	{POS_VOID_LOYATL_ISSUER, "65", "23", "000"},   //�����л������ѳ���
	{POS_VOID_LOYATL_ALL,    "65", "23", "000"},     //���˻������ѳ���
	{POS_QUE_LOYATL_ALL,     "65", "03", "000"},     //���˻��ֲ�ѯ
	{POS_REFUND_LOYATL_ALL,  "00", "25", "000"},  //���˻����˻�
	// �ֻ���
	{POS_MOBILE_SALE,       "00", "22", "000"},        //�ֻ�����
	{POS_MOBILE_VOID,       "00", "23", "000"},        //�ֻ����ѳ���
	{POS_MOBILE_REFUND,     "00", "25", "000"},       //�ֻ�оƬ�˻�
	{POS_MOBILE_PREAUTH,    "06", "10", "000"},     //�ֻ�оƬԤ��Ȩ
	{POS_MOBILE_VOID_PRE,   "06", "11", "000"},     //���ֻ�Ԥ��Ȩ����
	{POS_MOBILE_AUTH,       "06", "20", "000"},     //Ԥ��Ȩ��ɣ�����
	{POS_MOBILE_AUTH_OFF,   "06", "24", "000"},     //Ԥ��Ȩ���֪ͨ
	{POS_MOBILE_VOID_AUTH,  "06", "21", "000"},    //Ԥ��Ȩ��ɳ���
	{POS_MOBILE_QUE,        "00", "01", "000"},          //�ֻ�����ѯ
	// ԤԼ��
	{POS_RESER_SALE,        "67", "54", "000"},          //ԤԼ����
	{POS_RESER_VOID,        "67", "53", "000"},          //ԤԼ���ѳ���
	// ������
	{POS_MOTO_SALE,	         "08", "22", "000"},		    //��������
	{POS_MOTO_PREAUTH,       "18", "10", "000"},		    //����Ԥ��Ȩ
	{POS_MOTO_AUTH_CM,       "18", "20", "000"},		    //����Ԥ��Ȩ�������
	{POS_MOTO_PREAUTH_VOID,  "18", "11", "000"},            //����Ԥ��Ȩ����
	{POS_MOTO_AUTH_VOID,     "18", "21", "000"},		    //����Ԥ��Ȩ��ɳ���
	{POS_MOTO_SALE_VOID,     "08", "23", "000"},	        //�������ѳ���
	{POS_MOTO_REFUND,        "08", "25", "000"},            //���������˻�
	{POS_MOTO_AUTH_OFF,      "18", "24", "000"},		    //����Ԥ��Ȩ���֪ͨ
	// ��������ֵ
	{POS_MAGTOPUP_CASH,      "00", "48", "000"},           //�������ֽ��ֵ
	{POS_MAGTOPUP_ACCT,      "66", "49", "000"},           //�������ʻ���ֵ
	{POS_QUERY_ACCT,         "00", "01", "000"},           //�������ʻ���ѯ
	{POS_MAGTOPUP_CM,        "00", "48", "000"},           //�������ֽ��ֵȷ��
	{POS_DOWNLOADBLK,        "00", "00", "390"},           //����������
	{END_DOWNLOADBLK,        "00", "00", "391"},           //���������ؽ���
	{POS_LYT_LOGON,          "00", "00", "401"},           //����ǩ��
	{ICC_FAIL_BATCHUP,       "00", "00", "204"},           //ʧ�ܽ���������
};


ST_POS_CONFIG_INFO    gstPosCapability;     // �ն����õ�����
ST_COMM_CONFIG		  gstCurCommCfg;		 // ��ǰͨѶ���ò���
char                  gszParamFileName[50];      //  �����ļ���

unsigned char Bmp_Show_in_PIN[] = 
{
    0x42, 0x4D, 0x3E, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x28, 0x00, 
	0x00, 0x00, 0x7A, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xF8, 0x07, 
	0xFF, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFB, 0x77, 
	0xBF, 0x5C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFB, 0x77, 
	0x8F, 0xEC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0x7F, 
	0xA8, 0x2F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xE2, 0x0F, 
	0xAF, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFC, 0xEF, 
	0xAC, 0x0C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xF5, 0x7B, 
	0x2D, 0xDC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xF9, 0xB7, 
	0x8D, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFD, 0x5F, 
	0xBD, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xEF, 0xFB, 
	0xDD, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xE0, 0x03, 
	0x07, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0x7F, 
	0xF8, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 
};


unsigned char Bmp_Show_Balance[] = 
{
    0x42, 0x4D, 0x3E, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x28, 0x00, 
	0x00, 0x00, 0x7A, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0x9E, 
	0xFB, 0x3F, 0x9F, 0xF7, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xDF, 
	0x7B, 0xB9, 0xDE, 0xF0, 0xDB, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xDF, 
	0xBB, 0xBE, 0xDD, 0xF6, 0xE7, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xEE, 0xDF, 
	0xBB, 0xBF, 0x5B, 0xE6, 0xAD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xE0, 0xDF, 
	0x80, 0x3F, 0xDF, 0xF0, 0x2D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xEE, 0xDF, 
	0xBB, 0xBC, 0x01, 0xEA, 0xAD, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xEE, 0xDF, 
	0xBB, 0xBF, 0xDE, 0x75, 0xAD, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xEE, 0xDF, 
	0x80, 0x39, 0x05, 0xF8, 0xBD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xE0, 0xDF, 
	0xBB, 0xBE, 0xFB, 0xEB, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xDF, 
	0xBB, 0xBF, 0x77, 0xE0, 0x6F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x80, 0x07, 
	0xBB, 0xBF, 0xAF, 0xFD, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xEF, 
	0x80, 0x3F, 0xDF, 0xFB, 0x81, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 
};

unsigned char Bmp_Show_CurrName[] = 
{
    0x42, 0x4D, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x28, 0x00, 
	0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 
	0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x9F, 0x87, 0x00, 0x00, 0xEF, 0x77, 
	0x00, 0x00, 0xF7, 0x77, 0x00, 0x00, 0xFB, 0x7F, 0x00, 0x00, 0xFB, 0x7F, 0x00, 0x00, 0xFB, 0x7F, 
	0x00, 0x00, 0xFB, 0x7F, 0x00, 0x00, 0xC0, 0x07, 0x00, 0x00, 0xFF, 0xEF, 0x00, 0x00, 0xFF, 0xFF, 
	0x00, 0x00, 0xE0, 0x0F, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 
};

unsigned char Bmp_Show_AmtName[] = 
{
    0x42, 0x4D, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x28, 0x00, 
	0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 
	0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x80, 0x0E, 
	0xE7, 0xBF, 0x80, 0x00, 0x00, 0x00, 0xF5, 0x7E, 0x1B, 0x79, 0x80, 0x00, 0x00, 0x00, 0xED, 0xBE, 
	0xDC, 0xF9, 0x80, 0x00, 0x00, 0x00, 0xFD, 0xFC, 0xD5, 0xBF, 0x80, 0x00, 0x00, 0x00, 0xC0, 0x1E, 
	0x05, 0xBF, 0x80, 0x00, 0x00, 0x00, 0xFD, 0xFD, 0x55, 0xB9, 0x80, 0x00, 0x00, 0x00, 0xE0, 0x7E, 
	0xB5, 0xB9, 0x80, 0x00, 0x00, 0x00, 0x9F, 0xCF, 0x17, 0xBF, 0x80, 0x00, 0x00, 0x00, 0xEF, 0xBD, 
	0x60, 0x3F, 0x80, 0x00, 0x00, 0x00, 0xF7, 0x7C, 0x0D, 0xFF, 0x80, 0x00, 0x00, 0x00, 0xFA, 0xFF, 
	0xBD, 0xFF, 0x80, 0x00, 0x00, 0x00, 0xFD, 0xFF, 0x70, 0x3F, 0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 
	0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 
};


unsigned char Bmp_Show_LytBalance[] = 
{
    0x42, 0x4D, 0x3E, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x28, 0x00, 
	0x00, 0x00, 0x7A, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0x9E, 
	0xFB, 0x3E, 0xBF, 0x77, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xDF, 
	0x7B, 0xBE, 0xDF, 0x7B, 0xAF, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xDF, 
	0xBB, 0xBA, 0xEE, 0xFD, 0xEF, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xEE, 0xDF, 
	0xBB, 0xBA, 0xF5, 0xFE, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xE0, 0xDF, 
	0x80, 0x3C, 0xBF, 0xFE, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xEE, 0xDF, 
	0xBB, 0xBC, 0x60, 0xEE, 0xEF, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xEE, 0xDF, 
	0xBB, 0xBE, 0xEE, 0xF4, 0x09, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xEE, 0xDF, 
	0x80, 0x38, 0x2E, 0xFB, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xE0, 0xDF, 
	0xBB, 0xBE, 0xEE, 0xFD, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xDF, 
	0xBB, 0xBE, 0xEE, 0xFD, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x80, 0x07, 
	0xBB, 0xB8, 0xE0, 0xFE, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xEF, 
	0x80, 0x3F, 0x3F, 0xFE, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 
};


void  lcdFlip(void)
{
	return;
}

// end file