
#ifndef _POSMACRO_H
#define _POSMACRO_H


//#define _EMV_TEST_VERSION		// PBOC测试需要屏蔽
#define PBOC_VERSION


// 根据银联规范 8110和6110签到上送的设备序列号不同(设置序列号由银联唯一分配)，而且规范版本好也不同
//#define VERSION_NEW6110    

// 软件版本V+银联规定的软件版本号(2byte) + 自定义的版本号(4byte). 每次修改后都需要重新修改版本号,目前我们定义的是"年份+月份"
 
 #define VERSION_STRING "YC150826"
 

#define  TABLES_LEN     85+2
#define  ERR_INFO_NUM  	47
#define  APP_NUM         10     //应用数目
#define  APP_NAME        17     //应用名称

#define  NUM_PER_PAGE	 50     //打印明细笔数


#define  NO_TRANS_SELECT 	999
#define  MAX_ACQUIRER_NUM   30 

enum{
	POS_SALE=1,			//消费
	POS_PREAUTH,		//予授权
	POS_AUTH_CM,		//予授权完成
	POS_PREAUTH_VOID,   //预授权撤销
	POS_AUTH_VOID,		//予授权完成撤销
	POS_SALE_VOID,	    //消费撤销
	POS_REFUND,         //隔日退货	
	POS_PREAUTH_ADD,    //追加预授权
	POS_QUE,            //查余额	
//	POS_STOP_CARD,      //卡止付查询
	POS_REVERSE,        //冲正

	POS_LOGON,		    // POS签到 
	POS_LOGOFF,		    //联机  签退
	POS_SETT,		    //结算
	POS_BATCH_UP,		//联机  披上送开始与传送
	POS_DOWNLOAD_PARAM, //参数下载传递
	POS_UPLOAD_PARAM,   //POS上送终端参数
	RESPONSE_TEST,      //POS回响测试
	OFF_SALE,           //离线结算	
	OFF_ADJUST,         //结算调整	 
	ADJUST_TIP,         //结算调整小费
	POS_OFF_CONFIRM,    //予授权完成离线方式,属于通知类报文
	OFF_SALE_BAT,       //	批上送 离线结算	
	OFF_ADJUST_BAT,     //	批上送 结算调整	
	ADJUST_TIP_BAT,     //  批上送 结算调整	
	POS_REFUND_BAT,	    //	批上送 隔日退货	
	OFF_CONFIRM_BAT,	//	批上送 完成离线	
	QUE_TOTAL,	        //当班交易总计	
	QUE_BATCH_TOTAL,    //当批交易总计	
	QUE_BATCH_MX,       //当批明细	
	QUE_BATCH_ANY,	    //当批某笔		
	PRT_LAST,
	PRT_ANY, 
	PRT_DETAIL,
	PRT_TOTAL,
	PRT_LAST_TOTAL,	
	TELLER_LOGON,	    //操作员签到 
	TELLER_LOGOFF,	    //操作员签退
	TELLER_ADD,		    //增加操作员
	TELLER_DEL,			//删除操作员 
	TELLER_DISP,	    //显示操作员 
	TELLER_CHGPWD,	    //操作员改密 
	SETUP_EXTNUM,
	OPER_QUE_TERM_INFO,  //察看终端信息
//	OPER_CLEAR_TERM,     //操作员清除部分终端信息
//	DOWNLOAD_APPLICATION,	//程序下载
	DOWNLOAD_EMV_CAPK,		// IC卡公钥下载
	DOWNLOAD_EMV_PARAM,		// EMV参数下载
	QUERY_EMV_CAPK,			// 查询IC卡公钥
	QUERY_EMV_PARAM,		// 查询IC卡参数
	END_EMV_CAPK,			// 结束IC卡公钥下载
	END_EMV_PARAM,			// 结束IC卡参数下载
	UPLOAD_SCRIPT_RESULT,	// 上送脚本结果
	ICC_OFFSALE,			// ic卡离线消费
	ICC_BATCHUP,			// IC卡批上送
	DOWNLOAD,
	//电子现金类
    EC_QUICK_SALE,          //电子现金快速支付
	QPBOC_ONLINE_SALE,      //QPBOC联机交易
	EC_NORMAL_SALE,         //电子现金普通支付
	EC_TOPUP_CASH,          //电子现金圈存-现金圈存
	EC_TOPUP_SPEC,          //电子现金圈存-指定账户圈存
	EC_TOPUP_NORAML,        //电子EC_TOPUP_NORAML现金圈存-非指定账户圈存
	EC_REFUND,              //电子现金脱机退货
	EC_VOID_TOPUP,          //电子现金现金圈存撤消
	EC_BALANCE,             //电子现金余额查询
	EC_QUE_TXN,             //电子现金交易记录查询
	//电子钱包类
	ED_SALE,                //电子钱包支付
	ED_TOPUP_CASH,          //电子钱包圈存-现金圈存
	ED_TOPUP_SPEC,          //电子钱包圈存-指定账户圈存
	ED_TOPUP_NORAML,        //电子钱包圈存-非指定账户圈存
	ED_BALANCE,             //电子钱包余额查询
	ED_QUE_TXN,             //电子钱包交易记录查询
	// 分期类
	POS_INSTALLMENT,        //分期消费
	POS_VOID_INSTAL,        //分期消费撤消
	// 积分类
	POS_LOYATL_ISSUER,        //发卡行积分消费
	POS_LOYATL_ALL,           //联盟积分消费
	POS_VOID_LOYATL_ISSUER,   //发卡行积分消费撤消
	POS_VOID_LOYATL_ALL,      //联盟积分消费撤消
	POS_REFUND_LOYATL_ALL,    //联盟积分退货
	POS_QUE_LOYATL_ALL,       //联盟积分查询
	// 手机类
	POS_MOBILE_SALE,          //手机消费
	POS_MOBILE_VOID,          //手机消费撤销
	POS_MOBILE_REFUND,        //手机芯片退货
	POS_MOBILE_PREAUTH,       //手机芯片预授权
	POS_MOBILE_VOID_PRE,      //手手机预授权撤销
	POS_MOBILE_AUTH,          //预授权完成（请求）
	POS_MOBILE_AUTH_OFF,      //预授权完成通知
	POS_MOBILE_VOID_AUTH,     //预授权完成撤消
	POS_MOBILE_QUE,           //手机余额查询
	// 预约类
	POS_RESER_SALE,           //预约消费
	POS_RESER_VOID,           //预约消费撤销
	// 定购类
	POS_MOTO_SALE,			    //定购消费
	POS_MOTO_PREAUTH,		    //定购预授权
	POS_MOTO_AUTH_CM,		    //定购预授权完成请求
	POS_MOTO_PREAUTH_VOID,      //定购预授权撤销
	POS_MOTO_AUTH_VOID,		    //定购预授权完成撤销
	POS_MOTO_SALE_VOID,	        //定购消费撤销
	POS_MOTO_REFUND,            //定购隔日退货
	POS_MOTO_AUTH_OFF,		    //定购预授权完成通知
	// 磁条卡充值
	POS_MAGTOPUP_CASH,           //磁条卡现金充值
	POS_MAGTOPUP_ACCT,           //磁条卡帐户充值
	POS_MAGTOPUP_CM,             //磁条卡现金充值确认
	POS_QUERY_ACCT,              //帐户验证
	// 黑名单下载
	POS_DOWNLOADBLK,            // // 黑名单下载
	END_DOWNLOADBLK,            // 黑名单下载结束
	POS_LYT_LOGON,              // 收银员积分签到
	ICC_FAIL_BATCHUP,           // 失败交易批上送
	SEND_TAC,                   //TAC上送
	M1_ENRICH,//M1充值
	M1_RAEDCARD,//读卡
	M1_MONTHENRICH,//月充
	M1_SELL,//买卡
	M1_QUE_SETT,//1．	查询当天结算
	M1_SETT,//2．	充值点结算，生成结算记录
	M1_SETT_PRT,//3．	结算打印
	M1_DATA_UPLOAD,//4．	数据上传
	M1_SLAEDATE_UPLOAD,//5．	按销售日期补采
	M1_SETTDATE_UPLOAD,//6．	按结算日期补采
	M1_QUERY_NOT_UPLOAD,  // 7.查看未上送文件
	M1_DATA_UPLOAD_COM,//8．	数据上传[串口]
	M1_QUE_BATCH_TOTAL,    //当批交易总计	
	M1_QUE_BATCH_MX,       //当批明细	
	M1_QUE_BATCH_ANY,	    //当批某笔	
	M1_PRT_LAST,
	M1_PRT_ANY, 
	M1_PRT_DETAIL,
	M1_PRT_TOTAL,
};

///适用于2种状态的判断
#define  TRUE    1
#define  FALSE   0

#define  E_TRANS_CANCEL   1    //交易被取消
#define  E_TRANS_FAIL     2    //交易失败   
#define  E_NO_TRANS       3    // 无交易
#define  E_MAKE_PACKET    4    // 打包错    
#define  E_ERR_CONNECT    5    //联接失败 
#define  E_SEND_PACKET    6    //发包错误  
#define  E_RECV_PACKET    7    //收包错误  
#define  E_RESOLVE_PACKET 8    //解包错误  
#define  E_REVERSE_FAIL   9    //冲正失败 
#define  E_NO_OLD_TRANS   10   //无原始交易
#define  E_TRANS_VOIDED   11   //交易已被撤销
#define  E_ERR_SWIPE      12   //刷卡错误
#define  E_MEM_ERR        13   //文件操作失败
#define  E_PINPAD_KEY     14   //密码键盘或者密钥出错
#define  E_TIP_AMOUNT	  15   //小费金额超限
#define  E_TIP_NO_OPEN	  16   //小费没开
#define  E_TRANS_HAVE_ADJUESTED 17 //交易已经被调整
#define  E_FILE_OPEN	  18   //打开文件错
#define  E_FILE_SEEK	  19   //定位文件错
#define  E_FILE_READ	  20   //读文件错 
#define  E_FILE_WRITE	  21   //写文件错
#define  E_CHECK_MAC_VALUE 22  //收包MAC校验错
#define  E_CARD_BLOCKER    23  //卡被锁定
#define  E_APP_BLOCKED     24   // 应用锁定
#define  E_TRAN_DEFUSED    25   // 交易拒绝
#define  NO_DISP          36

#define  E_NEED_FALLBACK	51 //需要FALLBACK
#define  E_NEED_INSERT		52 // 需要插卡交易


#define  POS_LOG_FILE		    "pos.log"
#define  REVERSAL_FILE			"dup_file"
#define  REFUND_TRACK_FILE		"refnd.trk"
#define  TERM_AIDLIST			"termaid.lst"	
#define  TERM_CAPK_FILE         "termcapk.lst"
#define  ICC_SCRIPT_FILE		"iccscript"	
#define  ICC_FAILUR_TXN_FILE    "ICC_FAILUR.LOG"	 //失败的交易日志
#define  POS_M1_LOG_FILE		"posM1Enrich.log"

#define NO_SWIPE_INSERT		0x00	// 没有刷卡/插卡
#define CARD_SWIPED			0x01	// 刷卡
#define CARD_INSERTED		0x02	// 插卡
#define CARD_KEYIN			0x04	// 手输卡号
#define FALLBACK_SWIPED		0x08	// 刷卡(FALLBACK)
#define CARD_PASSIVE        0x20    // 射频卡打卡

#define  PARAM_OPEN		'1'
#define  PARAM_CLOSE	'0'

#define KEY_MODE_DES      '1'
#define KEY_MODE_3DES     '0'

#define  PIN_KEY_ID		2
#define  MAC_KEY_ID		3
#define  TEST_KEY_ID	4
#define  TDK_KEY_ID     5   // 磁道加密密钥

#define  EMV_PIN_KEY_ID  31    // 脱机明文PIN KEY ID

#define  DOUBLE_PIN_KEY_ID  6
#define  DOUBLE_MAC_KEY_ID  8
#define  DOUBLE_TDK_KEY_ID  10   // 磁道加密密钥

#define  HAVE_INPUT_PIN	  '1'
#define  NOT_INPUT_PIN	  '2'

#define  TRACK2_LEN        40
#define  TRACK3_LEN        107

#define  NORMAL            0
#define  REPRINT           1

#define  MAX_PACKET_LEN 2048  

#define  MAX_OPER_NUM 		101  

#define  SYS_TELLER_NO	"099"   ////系统管理员
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

#define MAX_APP_NUM       32         //EMV库应用列表最多可存储的应用数
#define MAX_KEY_NUM       64         //EMV库认证中心公钥表最多可存储的公钥数
#define PART_MATCH        0x01       //应用选择匹配标志(部分匹配)
#define FULL_MATCH        0x00       //应用选择匹配标志(完全匹配)


#define ERR_USERCANCEL  1   //等同 E_TRANS_CANCEL
#define ERR_TRAN_FAIL	2

#define STR_PBOC_AID    "A000000333"

#endif
