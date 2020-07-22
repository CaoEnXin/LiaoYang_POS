
#ifndef _NEWPOS_FUNCLIST_H
#define _NEWPOS_FUNCLIST_H

#include "posapi.h"
#include "posappapi.h"
#include "emvapi.h"
#include "emvtranproc.h"
#include "showDebug/showDebug.h"
#include "m1_tranproc.h"

void  InitPos(void);
void  DispResult(uint8_t rej);
void  InitCommData(void);
void  AppFirstRun(void);
void  CheckLimitTotal(void);
void  InitAllTransData(void);
void  SetParaFunc(void);
void  DispMainScreen(void);
void  OnlineDownOrUp(void);
void  CommHangUp(uint8_t bReleaseAll);
void  MakeReversalPack(void);
void  SaveRspPackData(void);
void  IncreaseTraceNo(void);
void  UpdateParameters(uint8_t *psPosParam, uint16_t iMaxLen);
void  MagTest(int ShowFlag);
void  PabxSetup(void);
void  PosDeviceDetect(void);
void  AfterSettleTran(void);
int KeyboardTest(void);
int LcdTest(int flag);
uint8_t CheckSupport(int iTransNo);
int CheckParamPass(void);
int TermCheck(void);
uint8_t DispTermInfo(void);
uint8_t ProcEvent(void);
uint8_t ProcSelectTran(void);
uint8_t OnlineCommProc(void);
uint8_t SendPacket(uint8_t *sSendData, uint16_t PacketLen);
uint8_t RecvPacket(uint8_t *psPacket, uint16_t *usPacketLen, uint8_t psWaitTime);
uint8_t SendRecvPacket(void);
int ConnectHost(void);
uint8_t SaveReversalTranData(void);
uint8_t ReadReversalTranData(void);
uint8_t ReverseTranProc(void);
uint8_t UpdateReversalFile(uint8_t Err); 
int MakeReqPacket(void);
int ProcRspPacket(void);
uint8_t TranNeedReversal(int iTranType);
uint8_t TranNeedIncTraceNo(int iTranType);
uint8_t HaveMacData(int iTranType);
uint8_t TrickleFeedOffLineTxn(uint8_t ucAllowBreak);
uint8_t TrickleFeedICOffLineTxn(uint8_t ucAllowBreak);

uint8_t PosOnlineLogon(uint8_t flag);
uint8_t PosOperLogon(uint8_t flag);

uint8_t PosOnlineLogoff(uint8_t flag);
uint8_t PosSettleTran(void);
uint8_t PosBatchUp(uint8_t flag);
uint8_t SelectCardUnit(uint8_t *szCardUnit);
uint8_t BatchSendOffTrans1(int *sendLen, uint8_t flag);
uint8_t BatchSendOffTrans2_Refund(int *sendLen, uint8_t flag);
uint8_t BatchSendOffTrans2_CM(int *sendLen, uint8_t flag);

uint8_t SendOffLineTrans(uint8_t flag, int maxSendTimes); 
uint8_t PosBatchSendTran(void);
uint8_t GetOldTraceNo(void);
uint8_t GetSysRefNo(void);
uint8_t GetAuthNo(void);
uint8_t GetOrignTxnDate(void);
uint8_t GetTxnTableIndex(int txn);
uint8_t GetAuthType(void);
uint8_t NeedPIN(int iTxnNo);
uint8_t ICNeedPIN(int iTxnNo);
uint8_t PreAuthAddInput(void);
uint8_t GetUpdateInfo(void);
uint8_t CheckAdjustAmt(uint8_t *sAmount, uint8_t *sOldAmount, uint8_t ucPercent);

uint8_t OnlineSale(void);
uint8_t VoidSaleAuth(void);
uint8_t PreAuth(void);
uint8_t VoidPreAuth(void);
uint8_t RefundTran(void);
uint8_t PreAuthAdd(void);
uint8_t OnlineAuthConfirm(void);
uint8_t OfflineAuthConfirm(void);
uint8_t QueryBalance(void);
uint8_t OfflineSale(void);
uint8_t OfflineAdjust(void);
uint8_t TestResponse(void); 
uint8_t AfterCommProc(void);
void  SetCommReqField(uint8_t *pszMsgCode, uint8_t *pszProcCode);
uint8_t DownLoadCAPK(uint8_t ucNeedBak);
uint8_t DownLoadEmvParam(uint8_t ucNeedBak);
uint8_t QueryEmvParam(void);
uint8_t UnpackTlvData(SaveTLVData pfSaveData);
void  SaveOneAid(uint16_t uiTag, uint8_t *psData, int iDataLen);
void  SaveAppParamItem(uint16_t uiTag, uint8_t *psData, int iDataLen);
uint8_t SaveEmvAppParam(TermAidList *pstTermAidList);
uint8_t FinishDownLoad(int iTranNo);
uint8_t QueryEmvCapk(void);
void  SaveCapkInfoItem(uint16_t uiTag, uint8_t *psData, int iDataLen);
uint8_t UpdateCoreCapkInfo(void);
int SearchCapkInfo(uint8_t *psRid, uint8_t ucKeyID);
void  SaveEmvCapkItem(uint16_t uiTag, uint8_t *psData, int iDataLen);
uint8_t SaveNewEmvCapk(struct issuer_ca_public_key *pstNewCapkList);
uint8_t SendICCOffLineTrans(uint8_t flag, int maxSendTimes);
uint8_t BatchMagOnLineTrans(int *piTotalNum, uint8_t ucSendFalg);
uint8_t BatchSendICCOnLineTrans(int *piTotalNum, uint8_t ucSendFlag);
#ifdef PBOC_VERSION
uint8_t BatchICCOffLineTrans(int *piTotalNum, uint8_t ucSendFlag);
#endif
void  UpdateEntryModeByPIN(void);

uint8_t TransBatchUpLoadFailureTxn(int *piTotalNum, uint8_t ucSendFlag);
uint8_t TransBatchUpLoadIccFailureTxn(int *piTotalNum, uint8_t ucSendFlag);
uint8_t TransBatchUpLoadIccAACTxn(int *piTotalNum, uint8_t ucSendFlag);

uint8_t SaveLogFile(void);
uint8_t CheckData(int lPosStan, NEWPOS_LOG_STRC *pstOutLog);
uint8_t UpdateLogFile(void);
uint8_t ComputeTransTotal(int flag);
uint8_t queryDetail(int flag);
uint8_t reprintTrans(uint8_t prt_bz);
uint8_t PrintDetail(uint8_t bPrintAll,uint8_t ucTitleFlag);
uint8_t queryTrans(void);
uint8_t QueryTransTotal(void);
void  update_limit_total(void);
void  DispOneTrans(NEWPOS_LOG_STRC *pstLog);
void  GetTransStatusInfo(NEWPOS_LOG_STRC *pstLog,char *pszStatus);

int  emvSaveAIDList(int iNo, const struct terminal_aid_info *info);
int  emvLoadAIDList(int iNo, struct terminal_aid_info *info);
int  emvSaveCAPKList(int iNo, const struct issuer_ca_public_key *info);
int  emvLoadCAPKList(int iNo, struct issuer_ca_public_key *info);

uint8_t WriteEnrichFtpFile(void* pdata, int size, char* filename);

uint8_t SaveCtrlParam(void);
uint8_t ReadCtrlParam(void);
uint8_t ReadOperFile(void);
uint8_t WriteOperFile(void);
void  DispFileErrInfo(void);
uint8_t SaveLimitTotal(void);
uint8_t ReadLimitTotal(void);
uint8_t SaveLastTotal(void);
uint8_t ReadLastTotal(void);
uint8_t read_data(void* pdata, int size, char* filename);
uint8_t write_data(void* pdata, int size, char* filename);
uint8_t SaveRefundDataFile(void);
uint8_t CheckRefundDataFile(int lTrace);
uint8_t ReadTermAidList(void);
uint8_t WriteScriptFile(void);
uint8_t ReadScriptFile(void);
uint8_t LoadTranLog(int iRecNo, NEWPOS_LOG_STRC *pstTranLog);
uint8_t UpdateTranLog(int iRecNo, NEWPOS_LOG_STRC *pstTranLog);

uint8_t TellerLogon(uint8_t flag);
void  TellerAdd(void);
void  TellerDel(void);
void  TellerShow(void);
void  TellerChgPwd(void);
void  TellerManageMenu(void);

void  TellerRunLock(void);
uint8_t CheckSupPwd(void);
uint8_t CheckTellerPwd(void);
void  SupTellerFunc(void);
void  SupChgPwd(void);

uint8_t EnterPIN(uint8_t ucFlag);
uint8_t DispBalance(void);
uint8_t CalMAC(uint8_t * inbuf, int len, uint8_t * outbuf);
uint8_t ExtractPAN(uint8_t* cardno, uint8_t* pan);
uint8_t LogonDivKey(uint8_t *pKeyData);

uint8_t PrintData(void);
uint8_t PrtTranDetailHead(void);
uint8_t PrtOneTranTxn(NEWPOS_LOG_STRC *pstLog);
uint8_t PrintAllErrOffTrans(void);
uint8_t PrintOneErrOffTrans(NEWPOS_LOG_STRC *stLog);
uint8_t PrtTransTotal(uint8_t flag, int batchNo);
uint8_t PrtNowTotal(void);
void  DispPrnErrInfo(uint8_t err);
void  PrtTranTicket(int flag);
void  MakeFormatCardNo(const char *card, char *fmtCard);
int   GetBankChnName(uint8_t * id, uint8_t *bankName);
void  GetTransPrtName(int iTransType, char *pszTransName);
void  PrintAmount(void);
void  PrintAddtionalInfo(void);
uint8_t GetEntryMode(void);
void  PrnEmvParam(void);
void SetupTranFunc(void);

void  SetupPosPara(void);          // 设置POS系统参数
void  SetupTerminalPara(void);     // 设置终端参数

void  SetTpduOutTime(void);
void  SetTestStatus(void);
void  SetMasterKeys(void);
void  SetTranResendTime(void);
void  SetTransFlow(void);
void  SetTransMask(void);
void AutoLogoutSetup(void);
void TipperSetup(void);
void ChangePwdFunc(void);

void  SetTranMaxNumber(void);
void  TestPrint(void);
uint8_t MaskTrans(int iTransId, int iMaskBit, char *pFuncName, uint8_t ucInKey);
void  SetPrintTicketNum(void);
void SetErrPrnTicketNum(void);
void  SetMaxOfflineNums(void);

void  PrintParam(uint8_t prtFlag);
uint8_t CheckParamOpen(char * item,char condition);
void  DeviceTest(void);
void  ChangeSysPwd(void);
void  OtherFunc(void);
void ChangeSafePwd(void);
void ClearFile(void);

void  SetCurCommType(void);
void  init_dial_statics(void);
uint8_t calculate_dial_statics(void);

uint8_t DetectCardEvent(uint8_t mode);
uint8_t DispCardNo(uint8_t *pszCardNo);
uint8_t GetCardNoFromTrack(uint8_t *szCardNo, uint8_t *track2, uint8_t *track3);
uint8_t GetTrackData(uint8_t *pszTrack2, uint8_t *pszTrack3, uint8_t *pszCardNo);
uint8_t PosGetCard(uint8_t ucMode);
uint8_t ManualEntryExpDate(uint8_t *pszExpDate);
uint8_t ManualEntryCardNo(uint8_t *pszCardNo);
uint8_t IsEmvCard(uint8_t *pszTrack2Data);
void  DispRemoveICC(void);
void  WaitRemoveICC(void);
void  DispICCProcessing(void);
void  MaskPan(uint8_t *pszInPan, uint8_t *pszOutPan);

uint32_t    SelectSetTwo(uint8_t *buf);

void ShowGetCardScreen(uint8_t ucMode);
void PrnBigFontChnData(char *pBuf);
void PrnBigFontEngData(char *pBuf);
void PrnNormalFontData(char *pBuf);
void PrnSmallFontData(char *pBuf);

int  WaitEvent(void);
void  GetPosTime(uint8_t *pszDate, uint8_t *pszTime);
void  dispRetCode(void);
uint8_t SelectTrans(void);
uint8_t SelectPrintFunc(void);
uint8_t M1SelectPrintFunc(void);

uint8_t LogonFunc(void);
void  ConvErrCode(uint8_t errCode, char *szErrInfo);
uint8_t AppGetAmount(int length, uint8_t flag);
uint8_t AppEnrichGetAmount(int length, uint8_t flag);//单位元

void  GetTransName(int tCode, uint8_t *szName);
uint8_t SelectAuthFunc(void);
uint8_t write_stat_time(void);
uint8_t AmountIsZero(uint8_t *sBcdAmt, uint8_t ucAmtLen);
uint8_t SelectPosManFunc(void);
uint8_t AppConfirmAmount(void);

uint8_t SwipeCardProc(uint8_t bCheckICC);
uint8_t InsertCardProc(uint8_t bShowDefTranName);
int GetEmvTrackData(void);
void  RemoveTailChars(uint8_t *pszString, uint8_t ucRemove);
uint8_t MatchTrack2AndPan(uint8_t *pszTrack2, uint8_t *pszPan);
void  ModifyTermCapForApp(void);
uint8_t PosDownLoadParam(void);
uint8_t PosUpLoadParam(void);
int MakeTermStatusData(uint8_t *pStatus);
void  DispTransName(void);
void  SetEmvTranType(void);
void  DispMulLanguageString(int Col, int Line, uint32_t Mode, char *pstChBuf, char *pstEnBuf);
void  SetDispLanguage(void);
uint8_t ContackCardProc(uint8_t bShowDefTranName); // 非接触式卡处理


void    ViewEmvResult(void);
void    PrnCommLog(void);
void    SndCommLog(void);
void    vTwo2One(uint8_t *psIn, uint16_t nLength, uint8_t *psOut);
void    vOne2Two0(uint8_t *psIn, uint16_t nLength, uint8_t *psOut);
void    vOne2Two(uint8_t *psIn, uint16_t nLength, uint8_t *psOut);
void    vLong2Bcd(uint32_t ulSource, uint16_t uiTCnt, uint8_t *psTarget);
uint16_t  uiPortRecvBuf(uint8_t ucPort, uint8_t *psBuff, uint16_t nLength, uint32_t nTimeOut);
void    vNacSend(uint8_t ucPort, uint8_t *psBuff, uint16_t nLength);
uint16_t  uiNacRecv(uint8_t ucPort, uint8_t *psBuff, uint32_t *puiLen, uint32_t nTimeOut);

//void DispMulLanguageString(int Col, int Line, uint32_t Mode, uint8_t *pstChBuf, uint8_t *pstEnBuf);

int  AdjustCommParam(void);
void   AdjustAppParam(void);
void   LoadDefaultAppParam(void);
int    SaveAppParam(void);
int    LoadAppParam(void);
void   ClearAllTxnStatus(void);  //所有交易状态为无效
void   SetTxnCtrlStatus(POS_COM *pstTxnLog);  // 设置交易状态

void   UpdataTxnCtrlStatus(uint32_t uiTxnIndex,uint32_t uiStatus);  // 更新交易状态
int    GetOfflineTxnNums(void);   // 获取脱机交易的笔数
int    GetMinSendTimesTxn(uint32_t uiTxnType);  // 获取记录中上送次数最少的交易,

// 电子钱包，存折卡操作函数
uint8_t PosGetEdCard(uint8_t ucMode);
uint8_t InsertEdCardProc(uint8_t bShowDefTranName);
int    VerifyUserPwd(void);
int    GetEdEpBalance(void);
int    CheckPSamCard(void);
int    SelectSamAPP(int ifd);

void  DispContackLessProcessing(void);

void  EncrypteFieldTrac23(uint8_t *pszTrack, uint8_t *pszEncrypData);
void  SetCommField60(void);
uint8_t DisplayLoyaltBalance(void);

uint8_t TransDownloadBlkCard(void);
int     SaveBlkCard(ST_BLACKLIST_TOTAL *pstBlkCard);
int     LoadBlkCard(ST_BLACKLIST_TOTAL *pstBlkCard);
uint8_t SaveM1EnrichLastTotal(void);
INT32 M1_ReadCard(void);
uint8_t M1_Que_Sett(void);
uint8_t M1_Sett(void);
uint8_t M1_Sett_Prt(void);
uint8_t M1_Data_UpLoad(void);
uint8_t M1_Data_UpLoad_TCP(void);

uint8_t M1_SaleDate_Upload(void);
uint8_t M1_SettDate_Upload(void);
uint8_t M1_Que_NotUploadFile(void);
INT32 ReadYtkEnrichLog(int *iRecNo);
void PrtM1TranEnrichTicket(INT32 flag);
INT32 SaveM1EnrichLog(int iOffSet);//如果是制定的iOffSet，iCurIndex不更形
uint8_t PrtM1EnrichTotal(uint8_t flag);
uint8_t ReadM1EnrichLastTotal(void);
uint8_t M1_queryDetail(void);
uint8_t M1_queryTrans(void)  ;
uint8_t  M1_QueryTransTotal(void);
void DispDateTime(void);








#endif

// end of file
