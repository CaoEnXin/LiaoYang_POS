
#ifndef _POSMACRO_H
#define _POSMACRO_H


//#define _EMV_TEST_VERSION		// PBOC������Ҫ����
#define PBOC_VERSION


// ���������淶 8110��6110ǩ�����͵��豸���кŲ�ͬ(�������к�������Ψһ����)�����ҹ淶�汾��Ҳ��ͬ
//#define VERSION_NEW6110    

// ����汾V+�����涨������汾��(2byte) + �Զ���İ汾��(4byte). ÿ���޸ĺ���Ҫ�����޸İ汾��,Ŀǰ���Ƕ������"���+�·�"
 
 #define VERSION_STRING "YC150826"
 

#define  TABLES_LEN     85+2
#define  ERR_INFO_NUM  	47
#define  APP_NUM         10     //Ӧ����Ŀ
#define  APP_NAME        17     //Ӧ������

#define  NUM_PER_PAGE	 50     //��ӡ��ϸ����


#define  NO_TRANS_SELECT 	999
#define  MAX_ACQUIRER_NUM   30 

enum{
	POS_SALE=1,			//����
	POS_PREAUTH,		//����Ȩ
	POS_AUTH_CM,		//����Ȩ���
	POS_PREAUTH_VOID,   //Ԥ��Ȩ����
	POS_AUTH_VOID,		//����Ȩ��ɳ���
	POS_SALE_VOID,	    //���ѳ���
	POS_REFUND,         //�����˻�	
	POS_PREAUTH_ADD,    //׷��Ԥ��Ȩ
	POS_QUE,            //�����	
//	POS_STOP_CARD,      //��ֹ����ѯ
	POS_REVERSE,        //����

	POS_LOGON,		    // POSǩ�� 
	POS_LOGOFF,		    //����  ǩ��
	POS_SETT,		    //����
	POS_BATCH_UP,		//����  �����Ϳ�ʼ�봫��
	POS_DOWNLOAD_PARAM, //�������ش���
	POS_UPLOAD_PARAM,   //POS�����ն˲���
	RESPONSE_TEST,      //POS�������
	OFF_SALE,           //���߽���	
	OFF_ADJUST,         //�������	 
	ADJUST_TIP,         //�������С��
	POS_OFF_CONFIRM,    //����Ȩ������߷�ʽ,����֪ͨ�౨��
	OFF_SALE_BAT,       //	������ ���߽���	
	OFF_ADJUST_BAT,     //	������ �������	
	ADJUST_TIP_BAT,     //  ������ �������	
	POS_REFUND_BAT,	    //	������ �����˻�	
	OFF_CONFIRM_BAT,	//	������ �������	
	QUE_TOTAL,	        //���ཻ���ܼ�	
	QUE_BATCH_TOTAL,    //���������ܼ�	
	QUE_BATCH_MX,       //������ϸ	
	QUE_BATCH_ANY,	    //����ĳ��		
	PRT_LAST,
	PRT_ANY, 
	PRT_DETAIL,
	PRT_TOTAL,
	PRT_LAST_TOTAL,	
	TELLER_LOGON,	    //����Աǩ�� 
	TELLER_LOGOFF,	    //����Աǩ��
	TELLER_ADD,		    //���Ӳ���Ա
	TELLER_DEL,			//ɾ������Ա 
	TELLER_DISP,	    //��ʾ����Ա 
	TELLER_CHGPWD,	    //����Ա���� 
	SETUP_EXTNUM,
	OPER_QUE_TERM_INFO,  //�쿴�ն���Ϣ
//	OPER_CLEAR_TERM,     //����Ա��������ն���Ϣ
//	DOWNLOAD_APPLICATION,	//��������
	DOWNLOAD_EMV_CAPK,		// IC����Կ����
	DOWNLOAD_EMV_PARAM,		// EMV��������
	QUERY_EMV_CAPK,			// ��ѯIC����Կ
	QUERY_EMV_PARAM,		// ��ѯIC������
	END_EMV_CAPK,			// ����IC����Կ����
	END_EMV_PARAM,			// ����IC����������
	UPLOAD_SCRIPT_RESULT,	// ���ͽű����
	ICC_OFFSALE,			// ic����������
	ICC_BATCHUP,			// IC��������
	DOWNLOAD,
	//�����ֽ���
    EC_QUICK_SALE,          //�����ֽ����֧��
	QPBOC_ONLINE_SALE,      //QPBOC��������
	EC_NORMAL_SALE,         //�����ֽ���֧ͨ��
	EC_TOPUP_CASH,          //�����ֽ�Ȧ��-�ֽ�Ȧ��
	EC_TOPUP_SPEC,          //�����ֽ�Ȧ��-ָ���˻�Ȧ��
	EC_TOPUP_NORAML,        //����EC_TOPUP_NORAML�ֽ�Ȧ��-��ָ���˻�Ȧ��
	EC_REFUND,              //�����ֽ��ѻ��˻�
	EC_VOID_TOPUP,          //�����ֽ��ֽ�Ȧ�泷��
	EC_BALANCE,             //�����ֽ�����ѯ
	EC_QUE_TXN,             //�����ֽ��׼�¼��ѯ
	//����Ǯ����
	ED_SALE,                //����Ǯ��֧��
	ED_TOPUP_CASH,          //����Ǯ��Ȧ��-�ֽ�Ȧ��
	ED_TOPUP_SPEC,          //����Ǯ��Ȧ��-ָ���˻�Ȧ��
	ED_TOPUP_NORAML,        //����Ǯ��Ȧ��-��ָ���˻�Ȧ��
	ED_BALANCE,             //����Ǯ������ѯ
	ED_QUE_TXN,             //����Ǯ�����׼�¼��ѯ
	// ������
	POS_INSTALLMENT,        //��������
	POS_VOID_INSTAL,        //�������ѳ���
	// ������
	POS_LOYATL_ISSUER,        //�����л�������
	POS_LOYATL_ALL,           //���˻�������
	POS_VOID_LOYATL_ISSUER,   //�����л������ѳ���
	POS_VOID_LOYATL_ALL,      //���˻������ѳ���
	POS_REFUND_LOYATL_ALL,    //���˻����˻�
	POS_QUE_LOYATL_ALL,       //���˻��ֲ�ѯ
	// �ֻ���
	POS_MOBILE_SALE,          //�ֻ�����
	POS_MOBILE_VOID,          //�ֻ����ѳ���
	POS_MOBILE_REFUND,        //�ֻ�оƬ�˻�
	POS_MOBILE_PREAUTH,       //�ֻ�оƬԤ��Ȩ
	POS_MOBILE_VOID_PRE,      //���ֻ�Ԥ��Ȩ����
	POS_MOBILE_AUTH,          //Ԥ��Ȩ��ɣ�����
	POS_MOBILE_AUTH_OFF,      //Ԥ��Ȩ���֪ͨ
	POS_MOBILE_VOID_AUTH,     //Ԥ��Ȩ��ɳ���
	POS_MOBILE_QUE,           //�ֻ�����ѯ
	// ԤԼ��
	POS_RESER_SALE,           //ԤԼ����
	POS_RESER_VOID,           //ԤԼ���ѳ���
	// ������
	POS_MOTO_SALE,			    //��������
	POS_MOTO_PREAUTH,		    //����Ԥ��Ȩ
	POS_MOTO_AUTH_CM,		    //����Ԥ��Ȩ�������
	POS_MOTO_PREAUTH_VOID,      //����Ԥ��Ȩ����
	POS_MOTO_AUTH_VOID,		    //����Ԥ��Ȩ��ɳ���
	POS_MOTO_SALE_VOID,	        //�������ѳ���
	POS_MOTO_REFUND,            //���������˻�
	POS_MOTO_AUTH_OFF,		    //����Ԥ��Ȩ���֪ͨ
	// ��������ֵ
	POS_MAGTOPUP_CASH,           //�������ֽ��ֵ
	POS_MAGTOPUP_ACCT,           //�������ʻ���ֵ
	POS_MAGTOPUP_CM,             //�������ֽ��ֵȷ��
	POS_QUERY_ACCT,              //�ʻ���֤
	// ����������
	POS_DOWNLOADBLK,            // // ����������
	END_DOWNLOADBLK,            // ���������ؽ���
	POS_LYT_LOGON,              // ����Ա����ǩ��
	ICC_FAIL_BATCHUP,           // ʧ�ܽ���������
	SEND_TAC,                   //TAC����
	M1_ENRICH,//M1��ֵ
	M1_RAEDCARD,//����
	M1_MONTHENRICH,//�³�
	M1_SELL,//��
	M1_QUE_SETT,//1��	��ѯ�������
	M1_SETT,//2��	��ֵ����㣬���ɽ����¼
	M1_SETT_PRT,//3��	�����ӡ
	M1_DATA_UPLOAD,//4��	�����ϴ�
	M1_SLAEDATE_UPLOAD,//5��	���������ڲ���
	M1_SETTDATE_UPLOAD,//6��	���������ڲ���
	M1_QUERY_NOT_UPLOAD,  // 7.�鿴δ�����ļ�
	M1_DATA_UPLOAD_COM,//8��	�����ϴ�[����]
	M1_QUE_BATCH_TOTAL,    //���������ܼ�	
	M1_QUE_BATCH_MX,       //������ϸ	
	M1_QUE_BATCH_ANY,	    //����ĳ��	
	M1_PRT_LAST,
	M1_PRT_ANY, 
	M1_PRT_DETAIL,
	M1_PRT_TOTAL,
};

///������2��״̬���ж�
#define  TRUE    1
#define  FALSE   0

#define  E_TRANS_CANCEL   1    //���ױ�ȡ��
#define  E_TRANS_FAIL     2    //����ʧ��   
#define  E_NO_TRANS       3    // �޽���
#define  E_MAKE_PACKET    4    // �����    
#define  E_ERR_CONNECT    5    //����ʧ�� 
#define  E_SEND_PACKET    6    //��������  
#define  E_RECV_PACKET    7    //�հ�����  
#define  E_RESOLVE_PACKET 8    //�������  
#define  E_REVERSE_FAIL   9    //����ʧ�� 
#define  E_NO_OLD_TRANS   10   //��ԭʼ����
#define  E_TRANS_VOIDED   11   //�����ѱ�����
#define  E_ERR_SWIPE      12   //ˢ������
#define  E_MEM_ERR        13   //�ļ�����ʧ��
#define  E_PINPAD_KEY     14   //������̻�����Կ����
#define  E_TIP_AMOUNT	  15   //С�ѽ���
#define  E_TIP_NO_OPEN	  16   //С��û��
#define  E_TRANS_HAVE_ADJUESTED 17 //�����Ѿ�������
#define  E_FILE_OPEN	  18   //���ļ���
#define  E_FILE_SEEK	  19   //��λ�ļ���
#define  E_FILE_READ	  20   //���ļ��� 
#define  E_FILE_WRITE	  21   //д�ļ���
#define  E_CHECK_MAC_VALUE 22  //�հ�MACУ���
#define  E_CARD_BLOCKER    23  //��������
#define  E_APP_BLOCKED     24   // Ӧ������
#define  E_TRAN_DEFUSED    25   // ���׾ܾ�
#define  NO_DISP          36

#define  E_NEED_FALLBACK	51 //��ҪFALLBACK
#define  E_NEED_INSERT		52 // ��Ҫ�忨����


#define  POS_LOG_FILE		    "pos.log"
#define  REVERSAL_FILE			"dup_file"
#define  REFUND_TRACK_FILE		"refnd.trk"
#define  TERM_AIDLIST			"termaid.lst"	
#define  TERM_CAPK_FILE         "termcapk.lst"
#define  ICC_SCRIPT_FILE		"iccscript"	
#define  ICC_FAILUR_TXN_FILE    "ICC_FAILUR.LOG"	 //ʧ�ܵĽ�����־
#define  POS_M1_LOG_FILE		"posM1Enrich.log"

#define NO_SWIPE_INSERT		0x00	// û��ˢ��/�忨
#define CARD_SWIPED			0x01	// ˢ��
#define CARD_INSERTED		0x02	// �忨
#define CARD_KEYIN			0x04	// ���俨��
#define FALLBACK_SWIPED		0x08	// ˢ��(FALLBACK)
#define CARD_PASSIVE        0x20    // ��Ƶ����

#define  PARAM_OPEN		'1'
#define  PARAM_CLOSE	'0'

#define KEY_MODE_DES      '1'
#define KEY_MODE_3DES     '0'

#define  PIN_KEY_ID		2
#define  MAC_KEY_ID		3
#define  TEST_KEY_ID	4
#define  TDK_KEY_ID     5   // �ŵ�������Կ

#define  EMV_PIN_KEY_ID  31    // �ѻ�����PIN KEY ID

#define  DOUBLE_PIN_KEY_ID  6
#define  DOUBLE_MAC_KEY_ID  8
#define  DOUBLE_TDK_KEY_ID  10   // �ŵ�������Կ

#define  HAVE_INPUT_PIN	  '1'
#define  NOT_INPUT_PIN	  '2'

#define  TRACK2_LEN        40
#define  TRACK3_LEN        107

#define  NORMAL            0
#define  REPRINT           1

#define  MAX_PACKET_LEN 2048  

#define  MAX_OPER_NUM 		101  

#define  SYS_TELLER_NO	"099"   ////ϵͳ����Ա
#define  SUP_TELLER_NO	"000"
#define  SUP_TELLER_PWD "123456"

#define  TELLER_NOBODY	"\x01\x01\x01"	
#define  TELLER_LOCK	"\x02\x01\x01"	

#define  LOGGEDOFF		0	
#define  LOGGEDON		1		

#define	 RMBLOG				1
#define  FRNLOG				2
#define  ALLLOG				3
#define  EMVLOG				4

#define  WORK_STATUS         '0'
#define  TEST_STATUS         '1'
#define  BATCH_STATUS		 '2'


#define  INITSIGNAL   1   

#define ICC_USER	0

#define TRAN_AMOUNT		0
#define TIP_AMOUNT		1
#define ADJUST_AMOUNT	2
#define IC_AMOUNT       3

#define MAX_APP_NUM       32         //EMV��Ӧ���б����ɴ洢��Ӧ����
#define MAX_KEY_NUM       64         //EMV����֤���Ĺ�Կ�����ɴ洢�Ĺ�Կ��
#define PART_MATCH        0x01       //Ӧ��ѡ��ƥ���־(����ƥ��)
#define FULL_MATCH        0x00       //Ӧ��ѡ��ƥ���־(��ȫƥ��)


#define ERR_USERCANCEL  1   //��ͬ E_TRANS_CANCEL
#define ERR_TRAN_FAIL	2

#define STR_PBOC_AID    "A000000333"

#endif
