
#include "ChkOption.h"
#include "glbvar.h"
#include "posmacro.h"
#include "tranfunclist.h"

/********************** Internal macros declaration ************************/
/********************** Internal structure declaration *********************/
/********************** Internal functions declaration *********************/
/********************** Internal variables declaration *********************/
/********************** external reference declaration *********************/
/******************>>>>>>>>>>>>>Implementations<<<<<<<<<<<<*****************/

uint8_t ChkIfSaveLog(void)
{
	if( stTemp.iTransNo==POS_SALE        || stTemp.iTransNo==POS_SALE_VOID    ||
		stTemp.iTransNo==POS_PREAUTH     || stTemp.iTransNo==POS_PREAUTH_VOID ||
		stTemp.iTransNo==POS_AUTH_CM     || stTemp.iTransNo==POS_AUTH_VOID    ||
		stTemp.iTransNo==POS_PREAUTH_ADD || stTemp.iTransNo==POS_REFUND       || 
		stTemp.iTransNo==POS_OFF_CONFIRM ||
		stTemp.iTransNo==ICC_OFFSALE     || stTemp.iTransNo==OFF_SALE         ||
		stTemp.iTransNo==ADJUST_TIP      ||
		(stTemp.iTransNo>=EC_QUICK_SALE && stTemp.iTransNo<=EC_VOID_TOPUP) ||       
		(stTemp.iTransNo>=ED_SALE && stTemp.iTransNo<=ED_TOPUP_NORAML) ||
		stTemp.iTransNo==POS_INSTALLMENT || stTemp.iTransNo==POS_VOID_INSTAL ||
		(stTemp.iTransNo>=POS_LOYATL_ISSUER && stTemp.iTransNo<=POS_REFUND_LOYATL_ALL)||
		stTemp.iTransNo==POS_MOBILE_SALE || stTemp.iTransNo==POS_MOBILE_VOID ||
		(stTemp.iTransNo>=POS_MOBILE_REFUND && stTemp.iTransNo<=POS_MOBILE_VOID_AUTH) ||
		stTemp.iTransNo==POS_RESER_SALE || stTemp.iTransNo==POS_RESER_VOID||
		(stTemp.iTransNo>=POS_MOTO_SALE && stTemp.iTransNo<=POS_MOTO_AUTH_OFF) ||
		stTemp.iTransNo==POS_MAGTOPUP_CASH || stTemp.iTransNo==POS_MAGTOPUP_ACCT ||
		(stTemp.iTransNo==OFF_ADJUST && stTemp.bSendId==TRUE) )
	{
		return TRUE;
	}

	return FALSE;
}


uint8_t ChkIfUpdateLog(void)
{
	if( stTemp.iTransNo==POS_SALE_VOID || stTemp.iTransNo==POS_AUTH_VOID ||
		stTemp.iTransNo==OFF_ADJUST    || stTemp.iTransNo==ADJUST_TIP ||
		stTemp.iTransNo== EC_VOID_TOPUP || stTemp.iTransNo==POS_VOID_INSTAL ||
		stTemp.iTransNo==POS_VOID_LOYATL_ALL ||
		stTemp.iTransNo== POS_VOID_LOYATL_ISSUER ||
		stTemp.iTransNo== POS_MOBILE_VOID || stTemp.iTransNo==POS_MOBILE_VOID_AUTH ||
		stTemp.iTransNo==POS_RESER_VOID || stTemp.iTransNo== POS_MOTO_SALE_VOID ||
		stTemp.iTransNo==POS_MOTO_AUTH_VOID)
	{
		return TRUE;
	}

	return FALSE;
}


// 判断是否统计到Debit总数里面
uint8_t ChkIfDebitTxn(int iTransType)
{
	if( iTransType==POS_AUTH_CM || iTransType==POS_OFF_CONFIRM
		|| iTransType==OFF_SALE || iTransType==OFF_ADJUST
		|| iTransType==ADJUST_TIP || iTransType == EC_NORMAL_SALE ||
		iTransType == EC_QUICK_SALE || iTransType == ED_SALE ||
		iTransType == ED_TOPUP_NORAML || iTransType == QPBOC_ONLINE_SALE)
/*		iTransType == POS_RESER_SALE || iTransType == POS_MOBILE_SALE ||
		iTransType == POS_MOBILE_AUTH || iTransType == POS_MOBILE_AUTH_OFF ||
		iTransType == POS_MOTO_SALE || iTransType == POS_MOTO_AUTH_OFF || 
		iTransType == POS_MOTO_AUTH_CM )*/
	{
		return TRUE;
	}

	return FALSE;
}

// 判断是否统计到Credit总数里面
uint8_t ChkIfCreditTxn(int iTransType)
{
	if( iTransType==POS_SALE_VOID || iTransType==POS_REFUND || 
		iTransType==POS_AUTH_VOID || iTransType== EC_TOPUP_CASH ||
		iTransType== ED_TOPUP_CASH || iTransType== POS_MAGTOPUP_CASH)
/*		iTransType== ED_TOPUP_CASH || iTransType== POS_MOTO_SALE_VOID ||
		iTransType== POS_MOTO_REFUND || iTransType== POS_MOTO_AUTH_VOID ||
		iTransType== POS_MOBILE_VOID || iTransType== POS_MOBILE_REFUND ||
		iTransType== POS_MOBILE_VOID_AUTH || iTransType== POS_RESER_VOID ||
		iTransType==POS_MAGTOPUP_CASH)*/
	{
		return TRUE;
	}

	return FALSE;
}

// 判断是否统计到Sale总数里面
uint8_t ChkIfSaleTxn(int iTransType)
{
	if( iTransType ==POS_SALE || iTransType==ICC_OFFSALE || 
		iTransType ==ADJUST_TIP || iTransType == EC_NORMAL_SALE ||
		iTransType == EC_QUICK_SALE || iTransType == ED_SALE ||
		iTransType == POS_RESER_SALE || iTransType == POS_MOBILE_SALE ||
		iTransType == POS_MOTO_SALE || iTransType == QPBOC_ONLINE_SALE)
	{
		return TRUE;
	}
	
	return FALSE;
}


// 判断是否打印交易明细
uint8_t ChkIfPrtDetill(int iTransType)
{
	if( iTransType ==POS_SALE    || iTransType==ICC_OFFSALE     ||
		iTransType ==POS_AUTH_CM || iTransType==POS_OFF_CONFIRM ||
		iTransType ==POS_REFUND  || iTransType==OFF_SALE        ||
		iTransType ==OFF_ADJUST  || iTransType==ADJUST_TIP ||
		iTransType==EC_QUICK_SALE || iTransType ==EC_NORMAL_SALE ||
		iTransType==QPBOC_ONLINE_SALE || iTransType==EC_TOPUP_CASH ||
		iTransType==ED_SALE || iTransType==ED_TOPUP_SPEC ||
		iTransType== ED_TOPUP_CASH || iTransType== POS_MAGTOPUP_CASH )
	{
		return TRUE;
	}

	return FALSE;
}


// 判断该卡是否是黑名单卡
uint8_t ChkIfBlkCard(char *pszCardNo)
{
	int  iRet, iCnt, iCardlen;
	ST_BLACKLIST_TOTAL stBlkCard;
	char  szBuffer[30];

	memset((char*)&stBlkCard,0,sizeof(stBlkCard));
	iRet = LoadBlkCard(&stBlkCard);
	if (iRet != 0)
	{
		return FALSE;
	}

	memset(szBuffer,0,sizeof(szBuffer));
	PubAsc2Bcd(pszCardNo,strlen(pszCardNo),szBuffer);

	for (iCnt=0; iCnt<stBlkCard.ulBlackListNum; iCnt++)
	{
		iCardlen = stBlkCard.szBlackList[iCnt][0];
		if ((iCardlen%2))
		{
			szBuffer[(iCardlen/2)] |= 0xF0;
		}

		if (iCardlen != 0 && memcmp(szBuffer,&stBlkCard.szBlackList[iCnt][1],(iCardlen+1)/2) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}


uint8_t ChkAcceptTxnCode(char *pszRespcode)
{
	if (memcmp(pszRespcode,"00",2) == 0 ||
		memcmp(pszRespcode,"10",2) == 0 ||
		memcmp(pszRespcode,"11",2) == 0 ||
		memcmp(pszRespcode,"A2",2) == 0 ||
		memcmp(pszRespcode,"A4",2) == 0 ||
		memcmp(pszRespcode,"A5",2) == 0 ||
		memcmp(pszRespcode,"A6",2) == 0)
	{
		return TRUE;
	}

	return FALSE;
}


// 判断是不是纯电子现金卡
uint8_t ChkOnlyEcCard(uint8_t *sAppAid)
{
	if (memcmp(sAppAid,"A000000333010106",16) == 0)
	{
		return TRUE;
	}

	return FALSE;
}


// 判断是否存在失败的交易记录
uint8_t ChkExTxnLog(void)
{
	int  iFilesize;

	iFilesize = fileSize(ICC_FAILUR_TXN_FILE);
	if (iFilesize <=0 )
	{
		return 0;
	}

	iFilesize = iFilesize/COM_SIZE;

	return iFilesize;
}


// 判断是否不需要设置60.6 -- 部分消费标志
uint8_t ChkSetPartSaleFlag(void)
{
	if (stTemp.iTransNo>= POS_PREAUTH && stTemp.iTransNo <= POS_AUTH_VOID)
	{
		return TRUE;
	}

	if (stTemp.iTransNo ==POS_OFF_CONFIRM)
	{
		return TRUE;
	}

	if (stTemp.iTransNo == POS_QUE || stTemp.iTransNo == OFF_SALE || stTemp.iTransNo == OFF_ADJUST || stTemp.iTransNo ==ADJUST_TIP)
	{
		return TRUE;
	}

	if (stTemp.iTransNo>= ED_TOPUP_CASH && stTemp.iTransNo <= ED_TOPUP_NORAML)
	{
		return TRUE;
	}

	return FALSE;
}

// end of file
