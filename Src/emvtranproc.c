#include <openssl/des.h>
#include <openssl/sha.h>
#include "posapi.h"
#include "posmacro.h"
#include "glbvar.h"
#include "st8583.h"
#include "tranfunclist.h"
#include "emvtranproc.h"
#include "emvapi.h"
#include "Menu.h"
#include "qpbocProc.h"

static uint8_t sAuthData[16];			// 发卡方认证数据
static uint8_t sIssuerScript[300];	// 发卡方脚本
static uint8_t sIssuerScriptRet[128];

static int   iAuthDataLen, iScriptLen;
static EmvScriptInfo stScriptInfo;
static uint8_t   sScriptBak[300];
static int   iCurScript, bHaveScript, iScriptBakLen, iScriptNums, sIssuerScriptRetlen;
int gl_FirstRunSelApp = 0x01;

const uint8_t g_InputPinLogoBmp[190] = {
    0x42, 0x4D, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x28, 0x00,
    0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x73, 0xF5, 0x33, 0xC0, 0x0F, 0xFF, 0xDF, 0xF7, 0x7B,
    0xF5, 0xBB, 0xDD, 0xEF, 0x7F, 0xAF, 0xF3, 0x03, 0xD5, 0xAB, 0xDD, 0xEF, 0x6F, 0xF7, 0xF5, 0x7B,
    0x84, 0x2B, 0xDD, 0xEF, 0x0F, 0xF7, 0xF7, 0x03, 0xF1, 0xAB, 0xFD, 0xFF, 0x68, 0x17, 0xF7, 0x7B,
    0xF4, 0x2B, 0x88, 0x1F, 0x6F, 0xF7, 0xF7, 0x03, 0xF5, 0xAB, 0xF3, 0xD6, 0x6C, 0x07, 0xF7, 0xFF,
    0x80, 0x3B, 0xD5, 0xD7, 0x6D, 0xDF, 0xC6, 0x00, 0xB7, 0xFF, 0xD6, 0xEF, 0x0D, 0xDF, 0xFF, 0xEF,
    0xD5, 0x09, 0x75, 0x6F, 0xBD, 0xDF, 0xFF, 0x03, 0xDE, 0xF7, 0xBB, 0xB7, 0xBD, 0xDF, 0xF7, 0xEF,
    0x81, 0x6F, 0x80, 0x07, 0xBF, 0xDF, 0xF6, 0x01, 0xEF, 0x9F, 0xFE, 0xFE, 0x04, 0x0F, 0xEF, 0xEF,
    0xEF, 0xBF, 0xFD, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

// 余额查询/消费/预授权,55域EMV标签
static Def55EmvTag stOnlineTxnTagList[] =
{
	{0x9F26, DE55_MUST_SET},
	{0x9F27, DE55_MUST_SET},
	{0x9F10, DE55_MUST_SET},
	{0x9F37, DE55_MUST_SET},
	{0x9F36, DE55_MUST_SET},
	{0x95,   DE55_MUST_SET},
	{0x9A,   DE55_MUST_SET},
	{0x9C,   DE55_MUST_SET},
	{0x9F02, DE55_MUST_SET},
	{0x5F2A, DE55_MUST_SET},
	{0x82,   DE55_MUST_SET},
	{0x9F1A, DE55_MUST_SET},
	{0x9F03, DE55_MUST_SET},
	{0x9F33, DE55_MUST_SET},
	{0x9F34, DE55_OPT_SET},
	{0x9F35, DE55_OPT_SET},
	{0x9F1E, DE55_COND_SET},
	{0x84,   DE55_OPT_SET},
	{0x9F09, DE55_OPT_SET},
	{0x9F41, DE55_OPT_SET},
	{0x9F63, DE55_COND_SET},
	{0x9F74, DE55_COND_SET},  // 电子现金类的
	{NULL_TAG_1},
};

// 冲正,55域EMV标签
static Def55EmvTag stReversalTagList[] =
{
	{0x95,   DE55_COND_SET},
	{0x9F1E, DE55_COND_SET},
	{0x9F10, DE55_COND_SET},
	{0x9F36, DE55_MUST_SET},
	{0xDF31, DE55_COND_SET},
	{NULL_TAG_1},
};

// 脚本处理结果上送,55域EMV标签
static Def55EmvTag stScriptResultTagList[] =
{
	{0x9F33, DE55_COND_SET},
	{0x95,   DE55_MUST_SET},
	{0x9F37, DE55_COND_SET},
	{0x9F1E, DE55_COND_SET},
	{0x9F10, DE55_MUST_SET},
	{0x9F26, DE55_MUST_SET},
	{0x9F36, DE55_MUST_SET},
	{0x82,   DE55_MUST_SET},
	{0xDF31, DE55_MUST_SET},
	{0x9F1A, DE55_MUST_SET},
	{0x9A,   DE55_MUST_SET},
	{NULL_TAG_1},
};

// 终端支持的语言类表en EN cn CN
uint8_t gl_TmSupLang[8] = {0x65, 0x6E, 0x45, 0x4E, 0x7A, 0x68, 0x5A, 0x48};
int gl_SupEn=1;  // IC卡和终端都支持英文
int gl_SupCN=0;  // IC卡和终端都支持中文

ST_EMVPRINT_INFO gstIccPrintData;   //用于打印的IC卡相关数据

int emv_candidate_apps_selection(void)
{
    int  Ret = 0;
    uint8_t CandListNum=0;
    int  j=0, Cursor=0;
    struct candapp CandApp[32];
    struct terminal_mck_configure tMckConfig;

	if( stPosParam.iDispLanguage==1 )
		gl_SupCN = 1;
	else
		gl_SupCN = 0;
    emv_get_mck_configure(&tMckConfig);  // 获取MCK配置
    emv_get_all_candapps(&CandListNum, CandApp); // 获取所有的候选应用信息
    if( CandListNum==0 )
    {
		errno = EMV_ERRNO_NO_APP;
		return -1;
    }

    if( !gl_FirstRunSelApp )
    {
        lcdClrLine(2, 7);
		lcdFlip();	
		if( gl_SupCN==0 )
			lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "TRY AGAIN?");
		else
    		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "重试?");
        Ret = kbGetKey();
        if( Ret!=KEY_ENTER )
        {
			errno = EMV_ERRNO_CANCEL;
			return -1;
        }
    }

    if( CandListNum==1 )
    {
        if( gl_FirstRunSelApp )
        {
            if( CandApp[0].cFlgAPID )
            {
                if( ((CandApp[0].cAPID & 0x80)==0x80) && tMckConfig.support_cardholder_confirmation )  // 需要用户确认的
                {
                    lcdCls();
					lcdFlip();	
					if( gl_SupCN==0 )
					{
						lcdDisplay(0, 2, DISP_MEDIACY|DISP_CLRLINE|DISP_CFONT, "%s", CandApp[0].tCandAppName);
						lcdDisplay(0, 4, DISP_MEDIACY|DISP_CLRLINE|DISP_CFONT, "[ENTER]-YES");
						lcdDisplay(0, 6, DISP_MEDIACY|DISP_CLRLINE|DISP_CFONT, "[CANCEL]-NO");
					}
					else
					{
						lcdDisplay(0, 2, DISP_MEDIACY|DISP_CLRLINE|DISP_CFONT, "%s", CandApp[0].tCandAppName);
						lcdDisplay(0, 4, DISP_MEDIACY|DISP_CLRLINE|DISP_CFONT, "[确认]-YES");
						lcdDisplay(0, 6, DISP_MEDIACY|DISP_CLRLINE|DISP_CFONT, "[取消]-NO");
					}

                    Ret = kbGetKey();
                    if( Ret!=KEY_ENTER )
                    {
						errno = EMV_ERRNO_CANCEL;
						return -1;
                    }
                }
                else if( ((CandApp[0].cAPID & 0x80)==0x80) && !tMckConfig.support_cardholder_confirmation )
                {
					errno = EMV_ERRNO_NO_APP;
                    return -1;
                }
            }
            gl_FirstRunSelApp = 0;
        }
        else
        {
            lcdCls();
			lcdFlip();	
			if( gl_SupCN==0 )
			{
				lcdDisplay(0, 2, DISP_MEDIACY|DISP_CLRLINE|DISP_CFONT, "%s", CandApp[0].tCandAppName);
				lcdDisplay(0, 4, DISP_MEDIACY|DISP_CLRLINE|DISP_CFONT, "[ENTER]-YES");
				lcdDisplay(0, 6, DISP_MEDIACY|DISP_CLRLINE|DISP_CFONT, "[CANCEL]-NO");
			}
			else
			{
				lcdDisplay(0, 2, DISP_MEDIACY|DISP_CLRLINE|DISP_CFONT, "%s", CandApp[0].tCandAppName);
				lcdDisplay(0, 4, DISP_MEDIACY|DISP_CLRLINE|DISP_CFONT, "[确认]-YES");
				lcdDisplay(0, 6, DISP_MEDIACY|DISP_CLRLINE|DISP_CFONT, "[取消]-NO");
			}
            Ret = kbGetKey();
			if( Ret!=KEY_ENTER )
			{
				errno = EMV_ERRNO_CANCEL;
				return -1;
            }
        }
        return 0;
    }

    if( !tMckConfig.support_cardholder_confirmation )
    {
        for(j=0; j<CandListNum; j++)
        {
            if( CandApp[j].cFlgAPID && (CandApp[j].cAPID & 0x80) )
            {
                continue;
            }
            else
            {
                gl_FirstRunSelApp = 0;
                return j;
            }
        }
    }

    lcdCls();
	lcdFlip();	
    kbFlush();
    if( CandListNum>1 )
    {
        lcdSetIcon(ICON_DOWN, ON);
    }
    else
    {
        lcdSetIcon(ICON_DOWN, OFF);
    }
    lcdSetIcon(ICON_UP, OFF);
    j = 0;

	
    while(1)
    {
        lcdCls();
		lcdFlip();	
		if( gl_SupCN==0 )
			lcdDisplay(0, 0, DISP_CFONT|DISP_REVERSE, "    APP LIST    ");
		else
			lcdDisplay(0, 0, DISP_CFONT|DISP_REVERSE, "    应用列表    ");
        if( j<CandListNum )
        {
            if( (Cursor%3)==0 )
            {
                lcdDisplay(0, 2, DISP_REVERSE|DISP_CLRLINE|DISP_CFONT, "%s", CandApp[j].tCandAppName);
            }
            else
            {
                lcdDisplay(0, 2, DISP_CLRLINE|DISP_CFONT, "%s", CandApp[j].tCandAppName);
            }
        }

        if( (j+1)<CandListNum )
        {
            if( (Cursor%3)==1 )
            {
                lcdDisplay(0, 4, DISP_REVERSE|DISP_CLRLINE|DISP_CFONT, "%s", CandApp[j+1].tCandAppName);
            }
            else
            {
                lcdDisplay(0, 4, DISP_CLRLINE|DISP_CFONT, "%s", CandApp[j+1].tCandAppName);
            }
        }

        if( (j+2)<CandListNum )
        {
            if( (Cursor%3)==2 )
            {
                lcdDisplay(0, 6, DISP_REVERSE|DISP_CLRLINE|DISP_CFONT, "%s", CandApp[j+2].tCandAppName);
            }
            else
            {
                lcdDisplay(0, 6, DISP_CLRLINE|DISP_CFONT, "%s", CandApp[j+2].tCandAppName);
            }
        }

        Ret = kbGetKey();

        switch(Ret)
        {
        case KEY_DOWN:
            if( Cursor < (CandListNum-1) )
            {
                Cursor++;
                lcdSetIcon(ICON_UP, ON);
                if( Cursor==(CandListNum-1) )
                {
                    lcdSetIcon(ICON_DOWN, OFF);  // 到菜单尾，就不再显示下翻图标
                }
            }

            j = (Cursor/3)*3;
            break;
        case KEY_UP:
            if( Cursor!=0 )
            {
                Cursor--;
                lcdSetIcon(ICON_DOWN, ON);
            }

            if( Cursor==0 )
            {
                lcdSetIcon(ICON_UP, OFF);// 到菜单顶，就不再显示上翻图标
            }

            j = (Cursor/3)*3;
            break;
        case KEY_ENTER:
            gl_FirstRunSelApp = 0;
			errno = 0;
            return Cursor;
        case KEY_CANCEL:
			errno = EMV_ERRNO_CANCEL;
            return -1;
        default:
            break;
        }
    }
}


void emv_multi_language_selection(void)
{
    int i=0;
	int Key=0;
	int j=0;
	uint8_t Len;
	uint8_t pCardLang[256];
	int iRet, iErrNo;

	iRet = emv_get_data_element("\x5F\x2D", 2, &Len, pCardLang);
	iErrNo = iRet?errno:0;
	if( iErrNo!=EMV_RET_SUCCESS )
	{
		errno = iRet;
		return;
	}
	if( Len%2 )
	{
		return;
	}
	for(i = 0; i < Len; i++)
	{
		if( ((pCardLang[i]>='a') && (pCardLang[i]<='z'))
			|| ((pCardLang[i]>='A') && (pCardLang[i]<='Z')) )
		{
		}
		else
		{
			return; 
		}
	}

	gl_SupEn = 0;
	gl_SupCN = 0;

	for(j=0; j<Len; j+=2)
	{
		for(i=0; i<sizeof(gl_TmSupLang); i+=2)
		{
			if( !memcmp(pCardLang+j, gl_TmSupLang+i, 2) )
			{
				if( (i==0) || (i==2) )
				{
					gl_SupEn = 1;  // IC卡和终端都支持英文
					return;
				}

				if( (i==4) || (i==6) )
				{
					gl_SupCN = 1;  // IC卡和终端都支持中文
					return;
				}
			}
		}
	}

	if( (gl_SupEn==0) && (gl_SupCN==0) )
	{
		lcdCls();
		lcdFlip();	
		lcdDisplay(0, 0, DISP_CFONT|DISP_MEDIACY|DISP_REVERSE, "  SEL LANGUAGE  ");
		lcdDisplay(0, 3, DISP_CFONT|DISP_MEDIACY, "1 - ENGLISH");
		lcdDisplay(0, 5, DISP_CFONT|DISP_MEDIACY, "2 - CHINESE");

		Key = kbGetKey();
		if( Key==KEY1 )
		{
			gl_SupEn = 1;  // 只支持英文
		}
		else if( Key==KEY2 )
		{
			gl_SupCN = 1;  // 只支持中文
		}
		return;
	}
	
}

int emvCheckCRL(uint8_t index,const void *rid,const void *cert_sn)
{
	errno = EMV_ERRNO_NODATA;
	return -1;
//	errno = EMV_RET_SUCCESS;
//	return 0;
}

int emvCheckExceptPAN(uint8_t PANLen, const void *PAN, uint8_t PANSn)
{	
	char  szCardNo[21];

	if (PosCom.stTrans.iTransNo != EC_QUICK_SALE)
	{
		errno = EMV_ERRNO_NODATA;
		return -1;
	}

	memset(szCardNo,0,sizeof(szCardNo));
	BcdToAsc0((uint8_t*)szCardNo, (uint8_t*)PAN, PANLen*2);
	RemoveTailChars((uint8_t*)szCardNo, 'F');		// erase padded 'F' chars

	if (ChkIfBlkCard((char*)szCardNo))
	{
		return 0;
	}

	errno = EMV_ERRNO_NODATA;
	return -1;
}

int emvGetTransAmount(int PANLen, uint8_t *PAN, uint8_t PANSn, uint32_t *Amount)
{
	int ucRet;    
	int iRecNo=0;
	NEWPOS_LOG_STRC	stLog;
	uint8_t szCardTemp[21];
	int iAmtTmp=0;

	if((PAN == NULL) || (Amount == NULL) || (PANLen > 10))
	{
		errno = EMV_ERRNO_INVAL;
		return -1;
	}

	*Amount = 0;
	if( stTransCtrl.iTransNum==0 )
	{
		errno = EMV_ERRNO_NODATA;
		return -1;
	}
	
	memset(szCardTemp, 0, sizeof(szCardTemp));
	BcdToAsc0(szCardTemp, PAN, PANLen*2);
	for(iRecNo=stTransCtrl.iTransNum-1; iRecNo>=0; iRecNo--)
	{
		memset(&stLog, 0, sizeof(stLog));
		ucRet = LoadTranLog(iRecNo, &stLog);
		if( ucRet!=OK )
		{
			lcdCls();
			lcdFlip();	
			errno = EMV_ERRNO_INVAL;
			return ucRet;
		}
		
		if( PANLen==10 )
		{
			if( memcmp(szCardTemp, stLog.szCardNo, 19)==0 )      
			{
				iAmtTmp = BcdToLong(stLog.sAmount, 6);

				*Amount += iAmtTmp;
			}
		}
		else
		{
			if( memcmp(szCardTemp, stLog.szCardNo, PANLen*2)==0 )      
			{
				iAmtTmp = BcdToLong(stLog.sAmount, 6);

				*Amount += iAmtTmp;
			}
		}
	}

	if(*Amount != 0)
		return EMV_RET_SUCCESS;
	
	errno = EMV_ERRNO_NODATA;
	
	return -1;
}

int emvGetTransLogAmount(uint8_t PanLen, const void *PAN, uint8_t PanSn)
{
	int Ret;
	uint32_t Amount=0;

	Ret = emvGetTransAmount(PanLen,(uint8_t *)PAN,PanSn,&Amount);
	if( Ret==EMV_RET_SUCCESS )
	{
		return Amount;
	}
	else
	{
		return 0;
	}
}

//交易金额输入函数
int emvInputTransAmount(uint32_t *pTransAmount)
{
	
	*pTransAmount = (uint32_t)BcdToLong(PosCom.stTrans.sAmount, 6);
	emv_set_data_element("\x9F\x03", 2, 6, "\x00\x00\x00\x00\x00\x00");

	return EMV_RET_SUCCESS;
}

int emvInputCashBackAmount(uint32_t *pCashbackAmount)
{
	
	if( pCashbackAmount!=NULL )
	{
		*pCashbackAmount = 0;
	}
	else
	{	// 设置CASHBACK金额
		emv_set_data_element("\x9F\x03", 2, 6, "\x00\x00\x00\x00\x00\x00");
		emv_set_data_element("\x9F\x04", 2, 4, "\x00\x00\x00\x00");
	}
	
	return EMV_RET_SUCCESS;
}

void LongToStr(uint32_t ldat, uint8_t *str)
{
    str[0] = (uint8_t)(ldat >> 24);
    str[1] = (uint8_t)(ldat >> 16);
    str[2] = (uint8_t)(ldat >> 8);
    str[3] = (uint8_t)(ldat);
}

void  auxLongToBCD(uint8_t* pStr, int  cLen, uint32_t lLng)
{
	int   i;
	uint8_t   cBR;

	memset(pStr, 0x00, cLen);
	for(i=(cLen-1); i>=0; i--)
	{
		cBR  = (uint8_t)(lLng%10);
		lLng = lLng/10;
		pStr[i] = (uint8_t)(lLng%10);
		pStr[i] = pStr[i]*0x10 + cBR;
		lLng = lLng/10;
	}
	return;
}

int emvGetOnlinePIN(void)
{
	int	iResult;

	// 联机PIN处理
	iResult = EnterPIN(0x80);
	if( iResult==OK )
	{
		return 0;
	}
	else if( iResult==PED_RET_NOPIN )
	{
		errno = EMV_ERRNO_BYPASS_PIN;
		return -1;
	}
	else if( iResult==E_TRANS_CANCEL )
	{
		errno = EMV_ERRNO_CANCEL;
		return -1;
	}
	else
	{
		errno = EMV_ERRNO_DATA;
		return -1;
	}
}

int emv_check_online_pin(void)
{
    int Ret = EMV_RET_SUCCESS;
	int iErrNo;

	if (PosCom.stTrans.iTransNo== EC_TOPUP_CASH || PosCom.stTrans.iTransNo== EC_TOPUP_SPEC
		|| PosCom.stTrans.iTransNo== EC_TOPUP_NORAML)
	{
		errno = EMV_ERRNO_BYPASS_PIN;
		 return -1;
	}

    Ret = emvGetOnlinePIN();
	iErrNo = Ret?errno:0;
    switch(iErrNo)
    {
    case PED_RET_APPTSSAKERR:
		errno = EMV_ERRNO_DATE;
        break;
    case PED_RET_NOPIN:
		errno = EMV_ERRNO_BYPASS_PIN;
        break;
    case EMV_ERRNO_CANCEL:
		errno = EMV_ERRNO_CANCEL;
        break;
    case PED_RET_OK:
		return 0;
    default:
//		errno = EMV_ERRNO_DATE;
		break;
    }

    return -1;
}

uint32_t asc2ulong(uint8_t *asc)
{
    int i, blen;
    uint32_t dwTmp;

    blen = strlen((char *)asc);
    dwTmp = 0;

    for(i=0; i<blen; i++)
    {
        if((asc[i] < '0') || (asc[i] > '9'))
            return -1;
        dwTmp = dwTmp*10 + (asc[i] - 0x30);
    }
    return dwTmp;
}

int emv_get_amount(uint32_t *pTransAmount, uint32_t *pOtherAmount)
{
	int iRet=0;
	int iAmount;
	uint8_t ucRet;

	if ((stTemp.iTransNo == EC_TOPUP_NORAML && PosCom.ucEdOrEpFlag)) 
	{
		if( pTransAmount )
		{
			*pTransAmount = 0;
		}
		if( pOtherAmount )
		{
			*pOtherAmount = 0;
		}
		return 0;
	}

	if (stTemp.iTransNo == POS_MAGTOPUP_ACCT)
	{
		if( pTransAmount )
		{
			*pTransAmount = 0;
		}
		if( pOtherAmount )
		{
			*pOtherAmount = 0;
		}
		return 0;
	}

REINPUT:
	if( stTemp.iTransNo!=POS_QUE && stTemp.iTransNo!=POS_SALE_VOID &&
		stTemp.iTransNo!=POS_AUTH_VOID && stTemp.iTransNo!= POS_LYT_LOGON
		&& stTemp.iTransNo != EC_NORMAL_SALE && stTemp.iTransNo != EC_REFUND)
	{
		iAmount = BcdToLong(PosCom.stTrans.sAmount, 6);
		if (iAmount != 0)
		{
			*pTransAmount = 0;
			*pTransAmount = iAmount;
			return 0;
		}

		iRet = AppGetAmount(9, IC_AMOUNT);
		if( iRet!=OK )
		{
			errno = E_TRANS_CANCEL;
			return -1;
		}
	}

	if (stTemp.iTransNo == POS_OFF_CONFIRM || stTemp.iTransNo == POS_REFUND)
	{
		ucRet = AppConfirmAmount();
		if( ucRet==KEY_CANCEL )
		{
			memset(PosCom.stTrans.sAmount,0,sizeof(PosCom.stTrans.sAmount));
			goto REINPUT;
		}	
	}

	if( pTransAmount )
	{
		*pTransAmount = 0;
		*pTransAmount = BcdToLong(PosCom.stTrans.sAmount, 6);
	}
	if( pOtherAmount )
	{
		*pOtherAmount = 0;
	}

	return 0;
}


int cEMVUnknowTLVData(uint16_t iTag, uint8_t *psDat, int iDataLen)
{
	switch( iTag )
	{
	case 0x9F53:	 
		*psDat = 0x52;
		break;

	default:
		return -1;
	}

	return EMV_RET_SUCCESS;
}

int emvCertCheck(void)
{
	return EMV_RET_SUCCESS;
}


int emv_issuer_referral_process(void)
{
	uint8_t len;
	uint8_t pan[100], buff[200];
	int rc;

	memset(pan, 0x00, sizeof(pan));
	memset(buff, 0x00, sizeof(buff));
	len = 0;
	emv_get_data_element("\x5A", 1, &len, buff);
	BcdToAsc(pan, buff, len*2);

	lcdCls();
	lcdFlip();	
	if(gl_SupCN == 0) // if no support chinese
	{
		lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"CALL YOUR BANK");
	}
	else // if support chinese
	{
		lcdDisplay(0,0,DISP_CFONT|DISP_MEDIACY,"请电告银行");
	}
	lcdDisplay(0,3,0,"APP PAN:");
	lcdDisplay(0,5,0,"%s", pan);
	if(gl_SupCN == 0) // if no support chinese
	{
		lcdDisplay(0,7,0,"Enter to Continue");
	}
	else // if support chinese
	{
		lcdDisplay(0,7,0,"按确认键继续");
	}
	kbGetKey();
	lcdCls();
	lcdFlip();	
	if(gl_SupCN == 0) // if no support chinese
	{
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"ACCEPT TRANS?");
	}
	else // if support chinese
	{
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"批准交易?");
	}

	while(1)
	{
		rc = kbGetKey();
		if(rc == KEY_ENTER)
		{
			return 0;
		}

		if(rc == KEY_CANCEL)
		{
			errno = EMV_ERRNO_DECLINE;
			return -1;  // Decline the transaction
		}
	}
}

// 银联检测Advice通过冲正实现
int emv_advice_process(int Flg)
{
	struct terminal_mck_configure tMckConfig;

	emv_get_mck_configure(&tMckConfig);  // Get current emv kernel MCK configuration

	if(!tMckConfig.support_advices) // the MCK do not support advice
	{
		return 0;
	}


	return 0;
}

//联机处理
int  emv_online_transaction_process(uint8_t *psRspCode,  uint8_t *psAuthCode, int *piAuthCodeLen,
					uint8_t *psAuthData, int *piAuthDataLen,
					uint8_t *psScript,   int *piScriptLen, int *online_result)
{
	uint8_t	ucRet, bOnlineTxnBak;
	int	iLength, PinEntryTimes, iResult;
	uint32_t	ulICCDataLen;
	uint8_t	*psICCData, *psTemp;

	*psRspCode      = 0;
	*piAuthCodeLen  = 0;
	*piAuthDataLen  = 0;
	*piScriptLen    = 0;
	PosCom.bIsFirstGAC = FALSE;

	emv_del_data_element("\x89", 1);
	emv_del_data_element("\x8A", 1);
	emv_del_data_element("\x91", 1);

	// 由于电子现金的QPBOC联机交易到回调函数里面才知道是联机交易，所有再磁设置一下报文
	if (PosCom.stTrans.iTransNo == EC_QUICK_SALE)
	{
		PosCom.stTrans.iTransNo = QPBOC_ONLINE_SALE;
		stTemp.iTransNo = QPBOC_ONLINE_SALE;

		ucRet = EnterPIN(0);
		if(ucRet)
		{
			errno = EMV_ERRNO_DECLINE;
			return -1;
		}

		SetCommReqField((uint8_t *)"0200",(uint8_t *)"000000");
	}

	// 由于普通电子现金的联机交易到回调函数里面才知道是联机交易,而上送的报文也应该按照消费来处理
	if (PosCom.stTrans.iTransNo == EC_NORMAL_SALE)
	{
		PosCom.stTrans.iTransNo = POS_SALE;
		stTemp.iTransNo = POS_SALE;
		
		SetCommReqField((uint8_t *)"0200",(uint8_t *)"000000");
	}
	
	ucRet = SetOnlineTranDE55(stOnlineTxnTagList, &glSendPack.sICCData[2], &iLength);
	if( ucRet!=OK )
	{
		errno = EMV_ERRNO_ONLINE_ERROR;
		return -1;
	}

	ShortToByte(glSendPack.sICCData, (uint16_t)iLength);
	memcpy(PosCom.stTrans.sIccData, &glSendPack.sICCData[2], iLength);	// save for batch upload
	PosCom.stTrans.iIccDataLen = (uint8_t)iLength;

	if( stTemp.iTransNo==POS_QUE || stTemp.iTransNo==POS_SALE )
	{	
		ShortToByte(glSendPack.sField62, (uint16_t)strlen((char *)PosCom.szCertData));
		sprintf((char *)&glSendPack.sField62[2], "%s", PosCom.szCertData);
	}

	UpdateEntryModeByPIN();	
	if( stTemp.iTransNo == EC_TOPUP_NORAML && PosCom.stTrans.ucEcTopOldSwipeFlag == CARD_SWIPED) //电子现金圈存根据原转出卡的标识
	{
		glSendPack.szEntryMode[1] = '2';
	}

	SendScriptResult();

	ReverseTranProc();

	PinEntryTimes = 0;
	while(1)
	{
		bOnlineTxnBak = PosCom.bOnlineTxn;
		ucRet = SendRecvPacket();
	    PosCom.bOnlineTxn = bOnlineTxnBak;
		if( ucRet!=OK )
		{
			UpdateReversalFile(ucRet); // 更新记录的原冲正文件
			errno = EMV_ERRNO_ONLINE_ERROR;
			PosCom.bOnlineTxn = bOnlineTxnBak;
			return -1;
		}

		break;
	}

	memcpy(psRspCode,  glRecvPack.szRspCode,  LEN_RSP_CODE);
	if( memcmp(glRecvPack.szRspCode, "00", LEN_RSP_CODE)!=0 )
	{
		fileRemove(REVERSAL_FILE);
	}
	PosCom.bOnlineTxn = TRUE;

	iAuthDataLen = iScriptLen = 0;
	iScriptNums = 0;

	Char2Long(glRecvPack.sICCData, 2, &ulICCDataLen);
	psICCData = &glRecvPack.sICCData[2];


	for(psTemp=psICCData; psTemp<psICCData+ulICCDataLen; )
	{
		iResult = GetTlvValue(&psTemp, psICCData+ulICCDataLen-psTemp, SaveRspICCData, TRUE);
		if( iResult<0 )
		{	
			*online_result = 1;
			errno = EMV_ERRNO_DECLINE;
			return -1;
		}
	}
	memcpy(psAuthData, sAuthData, iAuthDataLen);
	*piAuthDataLen = iAuthDataLen;
	AdjustIssuerScript();	
	memcpy(psScript, sIssuerScript, iScriptLen);
	
	*piScriptLen  = iScriptLen;

	// 电子现金圈存检查
	if (stTemp.iTransNo == EC_TOPUP_CASH || stTemp.iTransNo == EC_TOPUP_SPEC 
		|| stTemp.iTransNo == EC_TOPUP_NORAML)
	{
		if (iScriptNums > 1)
		{
			ucRet = E_RECV_PACKET;
			UpdateReversalFile(ucRet); // 更新记录的原冲正文件
			errno = EMV_ERRNO_ONLINE_ERROR;
			return -1;
		}
	}

	if( memcmp(glRecvPack.szRspCode, "05", LEN_RSP_CODE)==0 ||
		memcmp(glRecvPack.szRspCode, "51", LEN_RSP_CODE)==0 )
	{
		*online_result = 1;
	}
	else if( memcmp(glRecvPack.szRspCode, "01", LEN_RSP_CODE)==0 )
	{
		*online_result = 2;		
	}
	else if( memcmp(glRecvPack.szRspCode, "00", LEN_RSP_CODE)!=0 )
	{
		*online_result = 3;
		errno = EMV_ERRNO_DECLINE;
		return -1;
	}
	else 
	{
		*online_result = 0;
	}
	memcpy(psAuthCode, glRecvPack.szAuthCode, LEN_AUTH_CODE);
	*piAuthCodeLen = strlen((char *)glRecvPack.szAuthCode);

	return EMV_RET_SUCCESS;
}

//如果不需要提示密码验证成功，则直接返回就可以了
void emvDispPINOK(void)
{
	lcdClrLine(2, 7);
	lcdFlip();	
	if( gl_SupCN==0 )
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "PIN OK!");
	else
		lcdDisplay(0, 4, DISP_CFONT|DISP_MEDIACY, "密码正确");
	kbGetKeyMs(1000);
}

//持卡人认证例程
int emv_check_certificate(void)
{
//	int	    iRet, iErrNo;
	int     ucKey;
	uint8_t	ucCertType, szCertNo[50], iLength;

	if( emv_get_data_element("\x9F\x62", 2, &iLength, &ucCertType) )
	{
		errno = EMV_ERRNO_DATA;
		return -1;
	}

	memset(szCertNo, 0, sizeof(szCertNo));
	if( emv_get_data_element("\x9F\x61", 2, &iLength, szCertNo) )
	{
		errno = EMV_ERRNO_DATA;
		return -1;
	}
	if( iLength>40 )
	{
		errno = EMV_ERRNO_DATA;
		return -1;
	}

	lcdCls();
	lcdFlip();	
	lcdDisplay(0, 0, DISP_CFONT, "请核对持卡人证件");
	switch( ucCertType )
	{
	case 0x00:
		lcdDisplay(0, 2, DISP_CFONT, "身份证:");
		break;

	case 0x01:
		lcdDisplay(0, 2, DISP_CFONT, "军官证:");
		break;

	case 0x02:
		lcdDisplay(0, 2, DISP_CFONT, "护照:");
		break;

	case 0x03:
		lcdDisplay(0, 2, DISP_CFONT, "入境证:");
		break;

	case 0x04:
		lcdDisplay(0, 2, DISP_CFONT, "临时身份证:");
		break;

	case 0x05:
		lcdDisplay(0, 2, DISP_CFONT, "其它证件:");
		break;

	default:
		lcdDisplay(0, 2, DISP_CFONT, "其它证件:");
		break;
	}
	lcdDisplay(0, 4, DISP_ASCII, "%.40s", szCertNo);
	lcdDisplay(0, 6, DISP_CFONT, "0-取消  1-确认");
	

	while(1)
	{
		ucKey = kbGetKey();
		if( (ucKey!=KEY1) && (ucKey!=KEY0) )
		{
			continue;
		}
		break;
	}
	if( ucKey==KEY0 )
	{
		lcdCls();
		lcdFlip();	
		errno = EMV_ERRNO_DATA;
		return -1;

	}
	if( ucKey==KEY1 )
	{
		lcdCls();
		lcdFlip();	
		sprintf((char *)&PosCom.szCertData, "%02X%-20.20s", ucCertType, szCertNo);
		return 0;
	}
	return 0;
}

int emv_get_pin(void *CardPin)
{
	int retval;
	struct terminal_mck_configure tMCKConfig;
	
/*	if (PosCom.stTrans.iTransNo== EC_TOPUP_CASH || PosCom.stTrans.iTransNo== EC_TOPUP_SPEC
		|| PosCom.stTrans.iTransNo== EC_TOPUP_NORAML)
	{
		errno = EMV_ERRNO_BYPASS_PIN;
		return -1;
		errno = EMV_ERRNO_DATA;
		return -1;
	}*/

	emv_get_mck_configure(&tMCKConfig);

	lcdCls();
	if(gl_SupCN == 0) 
	{
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"ENTER PIN:");
	}
	else 
	{
		lcdDisplay(0,2,DISP_CFONT|DISP_MEDIACY,"请输入脱机密码:");
	}
	
	lcdGoto(20,5);
	while(PedGetSensitiveTimer(TYPE_PIN_INTERVAL));
	
	if(tMCKConfig.support_bypass_pin_entry)
		retval = kbGetString(KB_BIG_PWD,0,12,0,CardPin);
	else
		retval = kbGetString(KB_BIG_PWD,4,12,0,CardPin);
	
	if(retval == KB_CANCEL)
	{
		errno = EMV_ERRNO_CANCEL;
		return -1;
	}
	else if(retval== 0)
	{
		errno = EMV_ERRNO_BYPASS_PIN;
		return -1;			
	}
	return retval;
}

int emv_return_pin_verify_result(uint8_t TryCounter)
{
	int Ret;

	if( TryCounter==0 )
	{
		lcdCls();
		lcdFlip();	
		if(gl_SupCN == 0)  // Both terminal and card do not support chinese
		{
			lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"PIN OK");
		}
		else // Both terminal and card support chinese
		{
			lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"密码正确");
		}
		return 0;
	}

	lcdCls();
	lcdFlip();	
	if(gl_SupCN == 0) // both terminal and card do not support chinese
	{
		lcdDisplay(0,1,DISP_CFONT|DISP_MEDIACY,"PIN ERROR");
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"Remain %d Times",TryCounter);
		lcdDisplay(0,5,DISP_CFONT|DISP_MEDIACY,"TRY AGAIN?");
	}
	else  // both terminal and card do support chinese
	{
		lcdDisplay(0,1,DISP_CFONT|DISP_MEDIACY,"密码错误");
		lcdDisplay(0,3,DISP_CFONT|DISP_MEDIACY,"剩下%d次机会",TryCounter);
		lcdDisplay(0,5,DISP_CFONT|DISP_MEDIACY,"重新输入?");
	}
	while(1)
	{
		Ret = kbGetKey();		
		if(Ret == KEY_ENTER)
		{    
			Ret = EMV_RET_SUCCESS;
			break;
		}
		if(Ret == KEY_CANCEL)
		{
			errno = EMV_ERRNO_CANCEL;
			return -1;
		}
	}
	
	return Ret;
}

uint8_t ProcEmvTran(void)
{
	int	iRet, iErrNo, need_online;
	uint16_t  iSw;
	uint8_t   ucNeedonline,uclen;
	uint8_t   ucScripRet = FALSE;
	uint8_t   uLen;
	uint8_t   szBuffer[10];

	emv_get_mck_configure(&tConfig);
 
	ucNeedonline = 0;
	if (stTemp.iTransNo != POS_SALE && stTemp.iTransNo != EC_QUICK_SALE
		&& stTemp.iTransNo != EC_NORMAL_SALE)
	{
		ucNeedonline = 1;
	}

	memset(sIssuerScriptRet,0,sizeof(sIssuerScriptRet));
	sIssuerScriptRetlen = 0;

	tConfig.support_forced_online_capability = ucNeedonline;
//	tConfig.support_forced_online_capability = (stTemp.iTransNo!=POS_SALE ? 1 : 0);
//	Hash(&tConfig.terminal_type, sizeof(tConfig)-20, tConfig.checksum);
	SHA1((uint8_t*)&tConfig, sizeof(struct terminal_mck_configure)-20, tConfig.checksum);
	emv_set_mck_configure(&tConfig);
	
	// 处理EMV交易
	sprintf((char *)PosCom.szRespCode, "00");
	PosCom.stTrans.iTransNo = stTemp.iTransNo;

	emv_process_restriction();		
	iRet = emv_cardholder_verify(icc_fd);	
	iErrNo = iRet?errno:0;	
	if( iRet<0 )
	{
		goto ERRCODE;
	}

	iRet = emv_terminal_risk_manage(icc_fd);	
	iErrNo = iRet?errno:0;	
	if( iRet<0 )
	{
		goto ERRCODE;
	}
// 20110401加上交易限额的判断，如果超过或等于交易限额，强行加上置位
//	{
//		int         iCnt;
//		struct terminal_aid_info stEmvApp;
//		uint8_t		szBuff[40];
//		uint32_t	tmp=0;
//		uint8_t     uLen;
//
//		for(iCnt=0; iCnt<MAX_APP_NUM; iCnt++)
//		{
//			memset(&stEmvApp, 0, sizeof(stEmvApp));
//			iRet = emv_get_one_aid_info(iCnt, &stEmvApp);
//			if( iRet<0 )
//			{
//				continue;
//			}
//			memset(szBuff, 0, sizeof(szBuff));
//			BcdToAsc0(szBuff, stEmvApp.aid, stEmvApp.aid_len*2);
//			RemoveTailChars(szBuff, 'F');
//			tmp = BcdToLong(PosCom.stTrans.sAmount, 6);
//			if( memcmp(PosCom.stTrans.szAID, szBuff, strlen((char *)szBuff))==0 )
//			{
//				if( tmp>=stEmvApp.terminal_floor_limit )
//				{
//					emv_get_data_element("\x95", 1, &uLen, PosCom.stTrans.sTVR);
//					PosCom.stTrans.sTVR[3] |= 0x80;
//					emv_set_data_element("\x95", 1, 5, PosCom.stTrans.sTVR);
//				}
//			}
//		}
//	}

	iRet = emv_terminal_action_analysis(icc_fd, &need_online);
	if(!iRet && need_online){
		iRet = emv_online_transaction(icc_fd);
	}
	iErrNo = iRet?errno:0;	
	// 在此之后不要随意修改iRet 和 iError值
	
	memset(szBuffer,0,sizeof(szBuffer));
	szBuffer[0] = 0xFF;
	emv_get_data_element("\x9F\x27", 2, &uLen, szBuffer);
	szBuffer[0] &= 0xC0;
	// 脱机拒绝的AAC交易要求保存在失败记录里面
	if (!PosCom.bOnlineTxn && szBuffer[0] == 0)
	{
		GetAndSetQpbocElement();
		SaveIccFailureTranData();
		strcpy((char*)PosCom.stTrans.szEntryMode,"052");
		if (PosCom.ucPinEntry)
		{
			strcpy((char*)PosCom.stTrans.szEntryMode,"051");
		}
		SaveIccFailureFile();
	}

	// 在此之前不要随意修改iRet 和 iError值
ERRCODE:
	if( iErrNo==EMV_ERRNO_ONLINE_TIMEOUT || iErrNo==EMV_ERRNO_CANCEL )
	{
		return E_TRANS_CANCEL;
	}
	if( iErrNo!=EMV_RET_SUCCESS )
	{
		UpdateEmvRevFile();	
		iSw = emv_get_last_iccard_sw();
		if( PosCom.bIsFirstGAC && (iSw==0x6985) )
		{	
			return E_NEED_FALLBACK;
		}
//		if( PosCom.bOnlineTxn && memcmp(PosCom.szRespCode, "00", 2)!=0 )
		if( PosCom.bOnlineTxn && memcmp(glRecvPack.szRspCode, "00", 2)!=0 )
		{	
			return AfterCommProc();
		}
		if( iErrNo==EMV_ERRNO_DECLINE )
		{
			return E_TRAN_DEFUSED;
		}
		return E_TRANS_FAIL;
	}

	// 电子现金圈存看脚本执行结果
	emv_get_data_element("\x95", 1, &uclen, PosCom.stTrans.sTVR);
	ucScripRet = TRUE;
	if (PosCom.stTrans.sTVR[4] & 0x10)
	{
		ucScripRet = FALSE;
	} 

	// 现金圈存判断是否执行脚本成功，不成功要退出并冲正
	if (PosCom.stTrans.iTransNo== EC_TOPUP_CASH || PosCom.stTrans.iTransNo== EC_TOPUP_SPEC
		|| PosCom.stTrans.iTransNo== EC_TOPUP_NORAML)
	{
		if (!ucScripRet)
		{
			SaveEmvTranData();  // 保存脚本
			UpdateEcRevFile();

			return E_TRANS_FAIL;
		}
	}

	if( PosCom.bOnlineTxn)
	{
		fileRemove(REVERSAL_FILE);
	}

	SaveEmvTranData();

	return AfterCommProc();
}

uint8_t SetOnlineTranDE55(Def55EmvTag *pstList, uint8_t *psTLVData, int *piDataLen)
{
	int	i, iRet, iTemp, iErrNo;
	uint8_t	*psTemp, sBuff[128], iLength;
	uint8_t   sTemp[3];

	*piDataLen = 0;
	psTemp     = psTLVData;

	for(i=0; pstList[i].uiTagNum!=NULL_TAG_1; i++)
	{
		if (pstList[i].uiTagNum == 0x9F74)
		{
			if (PosCom.stTrans.iTransNo == EC_TOPUP_CASH || PosCom.stTrans.iTransNo == EC_TOPUP_SPEC 
				|| PosCom.stTrans.iTransNo == EC_TOPUP_NORAML)
			{
				continue;
			}
		}

		memset(sBuff, 0, sizeof(sBuff));
		memset(sTemp, 0, sizeof(sTemp));
		if( pstList[i].uiTagNum>0xFF )
		{
			sTemp[0] = (uint8_t)(pstList[i].uiTagNum>>8);
			sTemp[1] = (uint8_t)(pstList[i].uiTagNum);
			iTemp = 2;
		}
		else
		{
			sTemp[0] = (uint8_t)(pstList[i].uiTagNum);
			iTemp = 1;
		}

//		if((sTemp[0] == 0xDF) && (sTemp[1] == 0x31))
		if(pstList[i].uiTagNum == 0xDF31) // 由于DF31只在冲正的里面有，而emv_get_script_result 返回的ilength为0,所以暂时这样赋值
		{
			memset(sBuff, 0, sizeof(sBuff));
			iRet = 0;
//			iRet = emv_get_script_result(sBuff, &iLength);	
			memcpy(sBuff,sIssuerScriptRet,sIssuerScriptRetlen);
			iLength = sIssuerScriptRetlen;
		}
		else
		{
			iRet = emv_get_data_element(sTemp, iTemp, &iLength, sBuff);
		}

		iErrNo = iRet?errno:0;
		if( iErrNo==EMV_RET_SUCCESS )
		{
			MakeTlvString(pstList[i].uiTagNum, sBuff, iLength, &psTemp);
		}
		else if( pstList[i].ucTagOpt==DE55_MUST_SET )
		{
			return E_TRANS_FAIL;
		}
	}
	*piDataLen = (psTemp-psTLVData);

	return OK;
}

int GetTlvValue(uint8_t **ppsTLVString, int iMaxLen, SaveTLVData pfSaveData, uint8_t bExpandAll)
{
	int		iRet;
	uint8_t		*psTag, *psSubTag;
	uint16_t		uiTag, uiLenBytes;
	uint32_t		lTemp;

	for(psTag=*ppsTLVString; psTag<*ppsTLVString+iMaxLen; psTag++)
	{
		if( (*psTag!=NULL_TAG_1) && (*psTag!=NULL_TAG_2) )
		{
			break;
		}
	}
	if( psTag>=*ppsTLVString+iMaxLen )
	{
		*ppsTLVString = psTag;
		return 0;	
	}

	uiTag = *psTag++;
	if( (uiTag & TAGMASK_FIRSTBYTE)==TAGMASK_FIRSTBYTE )
	{	
		uiTag = (uiTag<<8) + *psTag++;
	}
	if( psTag>=*ppsTLVString+iMaxLen )
	{
		return -1;
	}

	if( (*psTag & LENMASK_NEXTBYTE)==LENMASK_NEXTBYTE )
	{
		uiLenBytes = *psTag & LENMASK_LENBYTES;
		Char2Long(psTag+1, uiLenBytes, &lTemp);
	}
	else
	{
		uiLenBytes = 0;
		lTemp      = *psTag & LENMASK_LENBYTES;
	}
	psTag += uiLenBytes+1;
	if( psTag+lTemp>*ppsTLVString+iMaxLen )
	{
		return -2;
	}
	*ppsTLVString = psTag+lTemp;	

	(*pfSaveData)(uiTag, psTag, (int)lTemp);
	if( !IsConstructedTag(uiTag) || !bExpandAll )
	{
		return 0;
	}

	for(psSubTag=psTag; psSubTag<psTag+lTemp; )
	{
		iRet = GetTlvValue(&psSubTag, psTag+lTemp-psSubTag, pfSaveData, TRUE);
		if( iRet<0 )
		{
			return iRet;
		}
	}

	return 0;
}

int IsConstructedTag(uint16_t uiTag)
{
	int		i;

	for(i=0; (uiTag&0xFF00) && i<2; i++)
	{
		uiTag >>= 8;
	}

	return ((uiTag & TAGMASK_CONSTRUCTED)==TAGMASK_CONSTRUCTED);
}

void Char2Long(uint8_t *psSource, uint16_t uiSCnt, uint32_t *pulTarget)
{
	uint16_t    i;
	
	*pulTarget = 0L;
	for(i=0; i<uiSCnt; i++)
	{
		*pulTarget |= ((uint32_t)psSource[i] << 8*(uiSCnt-i-1));
	}
}

void SaveRspICCData(uint16_t uiTag, uint8_t *psData, int iDataLen)
{
	switch( uiTag )
	{
	case 0x91:
		memcpy(sAuthData, psData, MIN(iDataLen, 16));
		iAuthDataLen = MIN(iDataLen, 16);
		break;
	
	case 0x71:
	case 0x72:
		sIssuerScript[iScriptLen++] = (uint8_t)uiTag;
		if( iDataLen>127 )
		{
			sIssuerScript[iScriptLen++] = 0x81;
		}
		sIssuerScript[iScriptLen++] = (uint8_t)iDataLen;
		memcpy(&sIssuerScript[iScriptLen], psData, iDataLen);
		iScriptLen += iDataLen;
		break;
	
	case 0x9F36:
		break;
		
	default:
		break;
	}
}

uint8_t SetIssuerName(void)
{
	int	iRet, iErrNo;
	uint8_t	sAID[16], iLength;
	
	// Application Identifier (AID) - ICC
	memset(sAID, 0, sizeof(sAID));
	iRet = emv_get_data_element("\x4F", 1, &iLength, sAID);
	iErrNo = iRet?errno:0;
	if( iErrNo!=EMV_RET_SUCCESS )
	{
		return E_TRANS_FAIL;
	}
	if( memcmp(sAID, "\xD1\x56", 2)==0 )
	{
		sprintf((char *)PosCom.stTrans.szCardUnit, "CUP");
	}
	else if( memcmp(PosCom.stTrans.szCardNo, "4", 1)==0 )
	{
		sprintf((char *)PosCom.stTrans.szCardUnit, "VIS");
	}
	else if( memcmp(PosCom.stTrans.szCardNo, "5", 1)==0 )
	{
		sprintf((char *)PosCom.stTrans.szCardUnit, "MCC");
	}
	else if( memcmp(PosCom.stTrans.szCardNo, "34", 2)==0 ||
		memcmp(PosCom.stTrans.szCardNo, "37", 2)==0 )
	{
		sprintf((char *)PosCom.stTrans.szCardUnit, "AEX");
	}
	else if( memcmp(PosCom.stTrans.szCardNo, "30", 2)==0 ||
		memcmp(PosCom.stTrans.szCardNo, "36", 2)==0 ||
		memcmp(PosCom.stTrans.szCardNo, "38", 2)==0 ||
		memcmp(PosCom.stTrans.szCardNo, "39", 2)==0 )
	{
		sprintf((char *)PosCom.stTrans.szCardUnit, "DCC");
	}
	else if( memcmp(PosCom.stTrans.szCardNo, "35", 2)==0 )
	{
		sprintf((char *)PosCom.stTrans.szCardUnit, "JCB");
	}

	return OK;
}

// called only if emvProcessTrans() failed
uint8_t UpdateEmvRevFile(void)
{
	uint8_t	ucRet;
	int	iLength;
	uint8_t	sReversalDE55[128];
	uint8_t   szBuff[10];

	// 保存脚本处理结果
	SaveScriptResult();
	
	if( fileExist(REVERSAL_FILE)<0 )
	{
		return OK;
	}

	memset(sReversalDE55, 0, sizeof(sReversalDE55));
	ucRet = SetOnlineTranDE55(stReversalTagList, sReversalDE55, &iLength);
	if( ucRet!=OK )
	{
		return E_TRANS_FAIL;
	}

	memset(szBuff, 0, sizeof(szBuff));
	if( strlen((char *)PosCom.stTrans.szAuthNo)>0 )
	{
		sprintf((char *)szBuff, "%.6s", PosCom.stTrans.szAuthNo);
	}
	
	ReadReversalTranData();

	memcpy(PosCom.sReversalDE55, sReversalDE55, iLength);
	PosCom.iReversalDE55Len = iLength;
	if( PosCom.szRespCode[0]==0 || memcmp(PosCom.szRespCode, "00", 2)==0 )
	{
		sprintf((char *)PosCom.szRespCode, "06"); 
	}

	if( strlen((char *)szBuff)>0 )
	{
		sprintf((char *)PosCom.stTrans.szAuthNo, "%.6s", szBuff);
	}

	SaveReversalTranData();

	return OK;
}


uint8_t UpdateEcRevFile(void)
{
	uint8_t	ucRet;
	int	iLength;
	uint8_t	sReversalDE55[128];

	if( fileExist(REVERSAL_FILE)<0 )
	{
		return OK;
	}
	
	memset(sReversalDE55, 0, sizeof(sReversalDE55));
	ucRet = SetOnlineTranDE55(stReversalTagList, sReversalDE55, &iLength);
	if( ucRet!=OK )
	{
		return E_TRANS_FAIL;
	}
		
	ReadReversalTranData();
	
	memcpy(PosCom.sReversalDE55, sReversalDE55, iLength);
	PosCom.iReversalDE55Len = iLength;
	
	SaveReversalTranData();
	
	return OK;
}

// 只处理基本数据元素Tag,不包括结构/模板类的Tag
void MakeTlvString(uint16_t uiTagNum, uint8_t *psData, int iLength, uint8_t **ppsOutData)
{
	uint8_t	*psTemp;

	if( iLength<=0 )
	{
		return;
	}

	psTemp = *ppsOutData;
	if( uiTagNum & 0xFF00 )
	{
		*psTemp++ = (uint8_t)(uiTagNum >> 8);
	}
	*psTemp++ = (uint8_t)uiTagNum;
	
	if( iLength<=127 )	// 目前数据长度均小余127字节,但仍按标准进行处理
	{
		*psTemp++ = (uint8_t)iLength;
	}
	else
	{	
		*psTemp++ = 0x81;
		*psTemp++ = (uint8_t)iLength;
	}
	
	memcpy(psTemp, psData, iLength);
	psTemp += iLength;
	
	*ppsOutData = psTemp;
}

void SaveEmvTranData(void)
{
	int		iLength, iRet;
	uint8_t uLen;
	char    szBuffer[15];

	if( !PosCom.bOnlineTxn && PosCom.stTrans.iTransNo==POS_SALE )
	{	
		PosCom.stTrans.iTransNo = ICC_OFFSALE;
		stTemp.iTransNo = ICC_OFFSALE;
		iLength = 0;
		SetOnlineTranDE55(stOnlineTxnTagList, PosCom.stTrans.sIccData, &iLength);
		PosCom.stTrans.iIccDataLen = (uint8_t)iLength;
		IncreaseTraceNo();

		if (memcmp(PosCom.stTrans.szAID,STR_PBOC_AID,10) == 0)
		{
			strcpy((char *)PosCom.stTrans.szCardUnit, "CUP");
		}
	}
	if (!PosCom.bOnlineTxn) // 电子现金消费也要保存55域数据
	{
		if (PosCom.stTrans.iTransNo==EC_NORMAL_SALE || PosCom.stTrans.iTransNo==EC_QUICK_SALE)
		{
			stTemp.iTransNo = PosCom.stTrans.iTransNo;
			iLength = 0;
			SetOnlineTranDE55(stOnlineTxnTagList, PosCom.stTrans.sIccData, &iLength);
			PosCom.stTrans.iIccDataLen = (uint8_t)iLength;
			IncreaseTraceNo();
		}

		if (memcmp(PosCom.stTrans.szAID,STR_PBOC_AID,10) == 0)
		{
			strcpy((char *)PosCom.stTrans.szCardUnit, "CUP");
		}
	}

	emv_get_data_element("\x9F\x26", 2, &uLen, PosCom.stTrans.sAppCrypto);
	emv_get_data_element("\x8A", 1, &uLen, PosCom.stTrans.sAuthRspCode);
	emv_get_data_element("\x95", 1, &uLen, PosCom.stTrans.sTVR);
	emv_get_data_element("\x9B", 1, &uLen, PosCom.stTrans.sTSI);
	emv_get_data_element("\x9F\x36", 2, &uLen, PosCom.stTrans.sATC);

	if(PosCom.stTrans.iTransNo==EC_NORMAL_SALE)
	{
		memset(szBuffer,0,sizeof(szBuffer));
		iRet = emv_get_data_element("\x9F\x79", 2, &uLen, szBuffer);
		iLength = PubBcd2Long((uint8_t*)szBuffer,6,NULL);
		PubLong2Char(iLength,4,(char*)PosCom.stTrans.sAfterBalanceAmt);
	}

	if (PosCom.stTrans.iTransNo==EC_TOPUP_CASH || PosCom.stTrans.iTransNo==EC_TOPUP_NORAML ||
		PosCom.stTrans.iTransNo==EC_TOPUP_SPEC )
	{
		uLen = 0;
		iRet = GetEcElementData((uint8_t*)"\x9F\x79", &uLen, (uint8_t*)szBuffer);	// 电子现金余额
		if (iRet == 0)
		{
			iRet = PubBcd2Long((uint8_t*)szBuffer,uLen,NULL);
			PubLong2Char(iRet,4,(char*)PosCom.stTrans.sAfterBalanceAmt);
		}
	}

	SaveScriptResult();
}

uint8_t SaveScriptResult(void)
{
	int	iRet, iCnt, iTemp, iLen, iErrNo;
	uint8_t	*psTemp, sBuff[128];
	uint8_t   sTemp[3], iLength;

	psTemp = PosCom.sScriptDE55;
	for(iCnt=0; stScriptResultTagList[iCnt].uiTagNum!=NULL_TAG_1; iCnt++)
	{
		memset(sBuff, 0, sizeof(sBuff));
		memset(sTemp, 0, sizeof(sTemp));
		if( stScriptResultTagList[iCnt].uiTagNum>0xFF )
		{
			sTemp[0] = (uint8_t)(stScriptResultTagList[iCnt].uiTagNum>>8);
			sTemp[1] = (uint8_t)(stScriptResultTagList[iCnt].uiTagNum);
			iTemp = 2;
		}
		else
		{
			sTemp[0] = (uint8_t)(stScriptResultTagList[iCnt].uiTagNum);
			iTemp = 1;
		}

		if((sTemp[0] == 0xDF) && (sTemp[1] == 0x31))
		{
			sysDelayMs(100);
			// 保存脚本处理结果
			memset(sBuff, 0, sizeof(sBuff));
			iRet = emv_get_script_result(sBuff, &iLen);			
			if( iRet!=EMV_RET_SUCCESS )
			{
				return OK;
			}

			memcpy(sIssuerScriptRet,sBuff,iLen);
			sIssuerScriptRetlen = iLen;
			MakeTlvString(stScriptResultTagList[iCnt].uiTagNum, sBuff, iLen, &psTemp);			
		}
		else
		{		
			iRet = emv_get_data_element(sTemp, iTemp, &iLength, sBuff);
			iErrNo = iRet?errno:0;
			if( iErrNo==EMV_RET_SUCCESS )
			{
				MakeTlvString(stScriptResultTagList[iCnt].uiTagNum, sBuff, iLength, &psTemp);
			}
			else if( stScriptResultTagList[iCnt].ucTagOpt==DE55_MUST_SET )
			{	// 如果必须存在的TAG不存在,交易失败
				return E_TRANS_FAIL;
			}
		}
	}
	PosCom.iScriptDE55Len = (psTemp-PosCom.sScriptDE55);

	return WriteScriptFile();

}

uint8_t SendScriptResult(void)
{
	uint8_t	ucRet;
	uint8_t   ucIndex;

	if( fileExist(ICC_SCRIPT_FILE)<0 )
	{
		return OK;
	}
	
	lcdClrLine(2, 7);
	lcdFlip();	
	lcdDisplay(0, 2, DISP_CFONT, "  脚本通知上送  ");
	DispMulLanguageString(0, 4, DISP_CFONT|DISP_MEDIACY, "正在上送脚本结果", "Upload Scrip... ");
	DispMulLanguageString(0, 6, DISP_CFONT|DISP_MEDIACY, "请稍候  ", "PLS WAIT....");
	sysDelayMs(500); // 延迟等
	
	PosCom.stTrans.iTransNo = stTemp.iTransNo;
	memcpy(&PosCom.stOrgPack, &glSendPack, sizeof(glSendPack));	// 保存请求报文
	memcpy(&BackPosCom, &PosCom, COM_SIZE);

	ucRet = ReadScriptFile();	// read PosCom structure 
	if( ucRet!=OK )
	{
		memcpy( &PosCom, &BackPosCom, COM_SIZE);
		memcpy(&glSendPack, &PosCom.stOrgPack, sizeof(glSendPack));	// 保存请求报文
		return ucRet;
	}
		
	stTemp.iTransNo = UPLOAD_SCRIPT_RESULT;
	memset(&glSendPack, 0, sizeof(STISO8583));
	sprintf((char *)glSendPack.szMsgCode,      "0620");
	sprintf((char *)glSendPack.szPan, "%.19s", PosCom.stOrgPack.szPan);

	sprintf((char *)glSendPack.szProcCode,     "%s",    PosCom.stOrgPack.szProcCode);
	sprintf((char *)glSendPack.szTranAmt,      "%s",    PosCom.stOrgPack.szTranAmt);
	sprintf((char *)glSendPack.szSTAN,         "%0*d", LEN_POS_STAN, stPosParam.lNowTraceNo);
	sprintf((char *)glSendPack.szEntryMode,    "%s",    PosCom.stOrgPack.szEntryMode);
	sprintf((char *)glSendPack.szPanSeqNo,     "%s",    PosCom.stOrgPack.szPanSeqNo);
	sprintf((char *)glSendPack.szAcquirerID,   "%.*s",  LEN_ACQUIRER_ID, PosCom.stTrans.szCenterId);
	sprintf((char *)glSendPack.szRRN,          "%.12s", PosCom.stTrans.szSysReferNo);
	sprintf((char *)glSendPack.szAuthCode,     "%.6s",  PosCom.stTrans.szAuthNo);
	sprintf((char *)glSendPack.szTermID,       "%s",    PosCom.stOrgPack.szTermID);
	sprintf((char *)glSendPack.szMerchantID,   "%s",    PosCom.stOrgPack.szMerchantID);
	sprintf((char *)glSendPack.szCurrencyCode, "%s",    PosCom.stOrgPack.szCurrencyCode);
	ShortToByte(glSendPack.sICCData, (uint16_t)PosCom.iScriptDE55Len);
	memcpy(&glSendPack.sICCData[2], PosCom.sScriptDE55, PosCom.iScriptDE55Len);
	ucIndex = GetTxnTableIndex(PosCom.stTrans.iTransNo);
	sprintf((char *)glSendPack.szField60, "00%06d%s%.2s", 
	stPosParam.lNowBatchNum, "951", &PosCom.stOrgPack.szField60[11]);
	sprintf((char *)glSendPack.szOrginalMsg, "%.6s%.6s%.4s", &PosCom.stOrgPack.szField60[2],
		PosCom.stOrgPack.szSTAN, &PosCom.stTrans.szDate[4]);
	if( HaveMacData(stTemp.iTransNo) )
	{
		ShortToByte(glSendPack.sMacData, LEN_MAC_DATA);
	}

	ucRet = SendRecvPacket();
	if( ucRet!=E_MAKE_PACKET && ucRet!=E_ERR_CONNECT && ucRet!=E_SEND_PACKET )
	{	// 只要发送成功需要删除脚本处理结果文件 
		fileRemove(ICC_SCRIPT_FILE);
	}
	
	memcpy(&PosCom, &BackPosCom, COM_SIZE);
	memcpy(&glSendPack, &PosCom.stOrgPack, sizeof(glSendPack));	// 恢复请求报文
	stTemp.iTransNo = PosCom.stTrans.iTransNo;
	PosCom.stTrans.lTraceNo = stPosParam.lNowTraceNo;

	// set bit 11
	if( stTemp.iTransNo!=RESPONSE_TEST      && stTemp.iTransNo!=POS_DOWNLOAD_PARAM &&
		stTemp.iTransNo!=POS_UPLOAD_PARAM   && stTemp.iTransNo!=QUERY_EMV_CAPK     &&
		stTemp.iTransNo!=QUERY_EMV_PARAM    && stTemp.iTransNo!=DOWNLOAD_EMV_CAPK  &&
		stTemp.iTransNo!=DOWNLOAD_EMV_PARAM && stTemp.iTransNo!=END_EMV_CAPK       &&
		stTemp.iTransNo!=END_EMV_PARAM && stTemp.iTransNo!=END_DOWNLOADBLK)    
	{
		sprintf((char *)glSendPack.szSTAN, "%0*d", LEN_POS_STAN,  PosCom.stTrans.lTraceNo);
	}


	lcdClrLine(2, 3);
	lcdFlip();	
	CommOnHook(FALSE);

	return ucRet;
}

void AdjustIssuerScript(void)
{
	int	iRet;
	uint8_t	*psTemp;

	memset(sScriptBak, 0, sizeof(sScriptBak));
	memset(&stScriptInfo, 0, sizeof(stScriptInfo));
	iCurScript = iScriptBakLen = 0;
	bHaveScript  = FALSE;
	for(psTemp=sIssuerScript; psTemp<sIssuerScript+iScriptLen; )
	{
		iRet = GetTlvValue(&psTemp, sIssuerScript+iScriptLen-psTemp, SaveScriptData, TRUE);
		if( iRet<0 )
		{
			return;
		}
	}
	if( bHaveScript && iCurScript>0 )
	{
		PackScriptData();
	}

	memcpy(sIssuerScript, sScriptBak, iScriptBakLen);
	iScriptLen = iScriptBakLen;
}

void  SaveScriptData(uint16_t uiTag, uint8_t *psData, int iDataLen)
{
	switch( uiTag )
	{
	case 0x71:
	case 0x72:
		if( bHaveScript && iCurScript>0 )
		{
			PackScriptData();
		}
		stScriptInfo.uiTag = uiTag;
		bHaveScript = TRUE;
		break;
		
	case 0x9F18:
		stScriptInfo.iIDLen = MIN(4, iDataLen);
		memcpy(stScriptInfo.sScriptID, psData, MIN(4, iDataLen));
		break;

	case 0x86:
		stScriptInfo.iCmdLen[iCurScript] = iDataLen;
		memcpy(stScriptInfo.sScriptCmd[iCurScript], psData, iDataLen);
		iCurScript++;
		break;

	default:
		break;
	}

	iScriptNums = iCurScript;
}

void PackTLVHead(uint16_t uiTag, uint16_t uiTLVDataLen, uint8_t *psOutData, int *piOutLen)
{
	int		iTagLen, iLen;
	
	PackTLVTag(uiTag, psOutData, &iTagLen);
	PackTLVLength(uiTLVDataLen, psOutData+iTagLen, &iLen);
	*piOutLen = iTagLen + iLen;
}

void PackTLVTag(uint16_t uiTag, uint8_t *psOutData, int *piOutLen)
{
	uint8_t	*psTemp;
	
	psTemp = psOutData;
	if( uiTag & 0xFF00 )
	{
		*psTemp++ = uiTag>>8;
	}
	*psTemp++ = uiTag;
	
	*piOutLen = (psTemp-psOutData);
}

void PackTLVLength(uint16_t uiTLVDataLen, uint8_t *psOutData, int *piOutLen)
{
	uint8_t	*psTemp;
	
	psTemp = psOutData;
	if( uiTLVDataLen<=127 )
	{
		*psTemp++ = (uint8_t)(uiTLVDataLen & 0xFF);
	}
	else
	{
		*psTemp++ = LENMASK_NEXTBYTE|0x01;
		*psTemp++ = (uint8_t)(uiTLVDataLen & 0xFF);
	}
	
	*piOutLen = (psTemp-psOutData);
}

int CalcTLVTotalLen(uint16_t uiTag, uint16_t uiDataLen)
{
	int	iLen, iTempLen;
	uint8_t	sTemp[4];
	
	iLen = 1;
	if( uiTag & 0xFF00 )
	{
		iLen++;
	}
	PackTLVLength(uiDataLen, sTemp, &iTempLen);
	
	return (iLen+iTempLen+uiDataLen);
}

void PackScriptData(void)
{
	int		iCnt, iTotalLen, iTempLen;

	iTotalLen = 0;
	if( stScriptInfo.iIDLen>0 )
	{
		iTotalLen += CalcTLVTotalLen(0x9F18, 4);
	}
	for(iCnt=0; iCnt<iCurScript; iCnt++)
	{
		iTotalLen += CalcTLVTotalLen(0x86, stScriptInfo.iCmdLen[iCnt]);
	}
	PackTLVHead(stScriptInfo.uiTag, iTotalLen, &sScriptBak[iScriptBakLen], &iTempLen);
	iScriptBakLen += iTempLen;
	
	if( stScriptInfo.iIDLen>0 )
	{
		PackTLVHead(0x9F18, 4, &sScriptBak[iScriptBakLen], &iTempLen);
		iScriptBakLen += iTempLen;
		memcpy(&sScriptBak[iScriptBakLen], stScriptInfo.sScriptID, 4);
		iScriptBakLen += 4;
	}
	for(iCnt=0; iCnt<iCurScript; iCnt++)
	{
		PackTLVHead(0x86, stScriptInfo.iCmdLen[iCnt], &sScriptBak[iScriptBakLen], &iTempLen);
		iScriptBakLen += iTempLen;
		memcpy(&sScriptBak[iScriptBakLen], stScriptInfo.sScriptCmd[iCnt], stScriptInfo.iCmdLen[iCnt]);
		iScriptBakLen += stScriptInfo.iCmdLen[iCnt];
	}

	memset(&stScriptInfo, 0, sizeof(stScriptInfo));
	iCurScript = 0;
}


void  SavePrintICCData(uint16_t uiTag, uint8_t *psData, int iDataLen)
{
	switch( uiTag )
	{
	case 0x82:
		memset(gstIccPrintData.sAip,0,sizeof(gstIccPrintData.sAip));
		memcpy(gstIccPrintData.sAip, psData, MIN(iDataLen, 2));	
		break;
		
	case 0x9F10:
		memset(gstIccPrintData.sIssureAppData,0,sizeof(gstIccPrintData.sIssureAppData));
		memcpy(gstIccPrintData.sIssureAppData, psData, MIN(iDataLen, 32));
		gstIccPrintData.ucIssureAppDatalen = iDataLen;
		if (gstIccPrintData.ucIssureAppDatalen > 32)
		{
			gstIccPrintData.ucIssureAppDatalen = 32;
		}
		break;
		
	case 0x9F33:
		memset(gstIccPrintData.sTerminalCapab,0,sizeof(gstIccPrintData.sTerminalCapab));
		memcpy(gstIccPrintData.sTerminalCapab, psData, MIN(iDataLen, 3));
		break;
		
	case 0x9F37:
		memset(gstIccPrintData.sTerminalRand,0,sizeof(gstIccPrintData.sTerminalRand));
		memcpy(gstIccPrintData.sTerminalRand, psData, MIN(iDataLen, 4));
		break;
		
	case 0x9F74:
		memset(gstIccPrintData.sEcIssureAuthCode,0,sizeof(gstIccPrintData.sEcIssureAuthCode));
		memcpy(gstIccPrintData.sEcIssureAuthCode, psData, MIN(iDataLen, 6));	
		break;

	case 0x9F26:
		memset(gstIccPrintData.sARQC,0,sizeof(gstIccPrintData.sARQC));
		memcpy(gstIccPrintData.sARQC, psData, MIN(iDataLen, 8));
		break;

	case 0x9F36:
		memset(gstIccPrintData.sAppAtc,0,sizeof(gstIccPrintData.sAppAtc));
		memcpy(gstIccPrintData.sAppAtc, psData, MIN(iDataLen, 2));
		break;
		
	case 0x95:
		memset(gstIccPrintData.sTVR,0,sizeof(gstIccPrintData.sTVR));
		memcpy(gstIccPrintData.sTVR, psData, MIN(iDataLen, 5));
		gstIccPrintData.ucTVRoK = TRUE;
		break;
		
	case 0x9F1E:
		memset(gstIccPrintData.sTIDSn,0,sizeof(gstIccPrintData.sTIDSn));
		memcpy(gstIccPrintData.sTIDSn, psData, MIN(iDataLen, 8));
		gstIccPrintData.ucTIDSnoK = TRUE;
		break;

	case 0xDF31:
		memset(gstIccPrintData.sScritptRet,0,sizeof(gstIccPrintData.sScritptRet));
		memcpy(gstIccPrintData.sScritptRet, psData, MIN(iDataLen, 21));
		gstIccPrintData.ucScritptRetlen = iDataLen;
		if (gstIccPrintData.ucScritptRetlen > 21)
		{
			gstIccPrintData.ucScritptRetlen = 21;
		}	
		break;
		
	default:
		break;
	}
}


// end of file

