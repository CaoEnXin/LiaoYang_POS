#ifndef EMVAPI_H_
#define EMVAPI_H_

// Return Value Definition
#define EMV_RET_SUCCESS          		0        // Transaction Approval
#define EMV_RET_ERRPARA         		-1       // Input Parameter Invalid
#define EMV_RET_DATAERR         		-2       // IC Card Return Invalid data
#define EMV_RET_ICCERR          		-3       // can not get valid response from iccard
#define EMV_RET_CARDBLOCKED     		-4       // IC Card be blocked
#define EMV_RET_NODATA          		-5       // data missing
#define EMV_RET_ONLINEERR      		 	-6       // can not go to online, or host return error response
#define EMV_RET_DECLINE         		-7       // Transaction decline
#define EMV_RET_OVERFLOW        		-8       // memory overflow
#define EMV_RET_NOAPP           		-9       // no valid application in ic card
#define EMV_RET_CANCEL          		-10      // operator terminate the transaction
#define EMV_RET_APPBLOCKED      		-11      // application be blocked
#define EMV_RET_PINBLOCKED      		-12      // pin blocked
#define EMV_RET_RESPERR         		-13      // IC Card return invalid status word
#define EMV_RET_CAPKEXPIRED     		-14      // ca public key expired
#define EMV_RET_NOACCEPTED      		-15      // application not accepted
#define EMV_RET_TIMEOUT   				-16      // timeout during wait response from host
#define EMV_RET_BYPASSPIN       		-17      // pin bypass
#define EMV_RET_FORCEACCEPT				-18      // operator force accept the transaction
#define EMV_RET_ICCPOWERUPERR			-19		 // reset ic card error

// aid list structure
typedef struct
{
	UINT8   cLen;                // application aid length
	UINT8   tAID[16];	         // application aid
	UINT8   SelFlag;             // does it support partial aid selection
	UINT8   Priority;            // Priority
	UINT32  TargetPer;           // Target Percentage to be Used for Random Selection
	UINT32  MaxTargetPer;        // Maximum Target Percentage to be Used for Biased Random Selection
    UINT32  FloorLimit;          // Terminal floor limit
	UINT32  Threshold;           // Threshold Value for Biased Random Selection
 	UINT8   TACDenial[5];        // Terminal action code -  Denial 
	UINT8   TACOnline[5];        // Terminal action code - Online
	UINT8   TACDefault[5];       // Terminal action code - Default 
    UINT8   AcquierId[6];        // Acpuirer Identifier
    INT32   LenOfdDOL;           // the length of default DDOL
	UINT8   dDOL[256];           // Default DDOL
	INT32   LenOftDOL;           // The length of default TDOL
	UINT8   tDOL[256];           // Default TDOL
	UINT8   Version[2];          // Terminal application version
	INT32   RiskManDataLen;      // the length of terminal risk management data
	UINT8   RiskManData[8];      // terminal risk management data	
}AIDLIST;

void emvDelAllAIDs(void);
INT32 emvAddAIDList(INT32 AIDNum, AIDLIST *pAIDList);
INT32 emvGetAIDList(INT32 StartIndex, INT32 AidNum, AIDLIST *pAIDList);
INT32 emvDelAIDList(AIDLIST tAIDList);
INT32 emvGetAIDListNum(void);
INT32 emvLoadAIDList(void);
INT32 emvSaveAIDList(void);

// ca public key structure
typedef  struct
{
	UINT8   cRid[5];  		       // RID
	UINT8   cIndex;   		       // ca public key index
	UINT8   cModLen;       	       // the module len of public key
	UINT8   cMod[248];             // module
	UINT8   cExpLen;               // the length of public key exponent
	UINT8   cExp[3];           	   // exponent
	UINT8   cExpDate[3];           // expire date
	UINT8   cHash[20];             // check sum
}CAPKLIST;

void emvDelCAPKLists (void);
INT32 emvAddCAPK(CAPKLIST tCAPKList);
INT32 emvGetCAPK(UINT8 *pRID, UINT8 Index, CAPKLIST *pCAPKList);
INT32 emvDelCAPK(UINT8 *pRID, UINT8 Index);
INT32 emvCheckCAPK(UINT8 *pRID, UINT8 Index);
INT32 emvChgCAExpDate(UINT8 *pRID, UINT8 Index, UINT8 *ExpDate);
INT32 emvGetCapkListNum(void);
INT32 emvGetCAPKByIndex(INT32 Index, CAPKLIST *pCAPKList);
INT32 emvLoadCAPKList(void);
INT32 emvSaveCAPKList(void);

// MCK Configuration struction
typedef struct
{
	UINT8   cTmType;             // Terminal type
	UINT8   tTmCap[3];           // terminal capabilities
	UINT8   tTmCapAd[5];         // additional terminal capabilities

	UINT8   cSupPSESelect;        // does terminal support PSE selection method
	UINT8   cSupAppConfirm;       // does terminal support cardholder confirmation
	UINT8   cSupIssCertCheck;     // does terminal support multi language
	UINT8   cSupDDOLFlg;          // does terminal contain a default DDOL

	UINT8   cSupBypassPIN;        // does terminal support bypass pin entry
	UINT8   cSupGetPINCnt;        // does terminal support get data for pin try counter

	UINT8   cSupFloorLimitCheck;  // does terminal support floor limit checking
	UINT8   cSupRmdTransCheck;    // does terminal support random transaction selection
	UINT8   cSupVelocityCheck;    // does terminal support velocity checking
	UINT8   cSupTransLog;         // does terminal support transaction log
	UINT8   cSupExceptCheck;      // does terminal support exception file

	UINT8   cSupTmActionCode;     // does terminal support terminal action codes
	UINT8   cSupForceOnline;      // does terminal support forced online capability
	UINT8   cSupAdvice;           // does terminal support advices
	UINT8   cSupIssVoiceRef;      // does terminal support issuer initiated voice referrals
	UINT8   cSupBatchCollect;     // does terminal support batch data capture
	UINT8   cSupOnlineCollect;    // does terminal support online data capture
	UINT8   cSupTDOLFlg;          // does terminal support a default TDOL	

	UINT8   cSupMutliLang;        // does terminal support multi language
	UINT8   cSupForceAccept;      // does terminal support forced acceptance capability
	UINT8   cSupCardVoiceRef;     // does terminal support card initiated voice referrals
	UINT8   cSupTRMOnAIP;         // does terminal performance of terminal risk manage based on AIP

	UINT8   cPOSEntryMode;        // POS Entry Code
	UINT8   tHashVal[20];         // check sum
}EMVMCKCONFIG; // 1-> Support 0 -> No Support

INT32 emvSetMCKConfig(EMVMCKCONFIG MCKConfig);
INT32 emvGetMCKConfig(EMVMCKCONFIG *pMCKConfig);

// EMV Kernel Type
#define KERNEL_TYPE_EMV			0			// for standard EMV transaction
#define KERNEL_TYPE_PBOC		1			// for PBOC transaction
#define KERNEL_TYPE_VISA		2			// for VISA wave transaction
#define KERNEL_TYPE_MASTERCARD	3			// for Mastercard Paypass transaction
#define KERNEL_TYPE_JCB			4			// for JCB transaction
#define KERNEL_TYPE_AMEX		5			// for America Express transaction
void emvSetKernelType(UINT8 Mode);
UINT8 emvGetkernelType(void);

// Data element operation
void emvInitDataElements(void);
INT32 emvSetDataElement(UINT8 *pTag, INT32 cLenTag, INT32 pLen, UINT8 *pVal);
INT32 emvGetDataElement(UINT8 *pTag, INT32 cLenTag, INT32 *pLen, UINT8 *pVal);
INT32 emvCheckDataElement(UINT8 *pTag, INT32 cLenTag);
INT32 emvDelDataElement(UINT8  *pTag,  INT32 cLenTag);

// Transaction type
#define EMV_CASH          0x80       // cash
#define EMV_GOODS	      0x40       // goods
#define EMV_SERVICE       0x20       // service
#define EMV_CASHBACK      0x10       // cashback
#define EMV_INQUIRY       0x08       // inquiry
#define EMV_TRANSFER      0x04       // transfer
#define EMV_PAYMENT       0x02       // payment
#define EMV_ADMIN         0x01       // administration

typedef struct
{
    UINT8  TermId[8];          // Terminal identifier
    UINT8  MerchantID[15];     // Merchant identifier
    UINT8  MerchCatCode[2];    // Merchant category code
    INT32  MCHLOCLen;          // the length of Merchant Name and Location
    UINT8  MCHNameLoc[256];    // Merchant Name and Location
    UINT8  TransType;          // Transaction type
    UINT8  TermCntrCode[2];    // Terminal country code
    UINT8  TransCurCode[2];    // Transaction currency code
    UINT8  ReferCurCode[2];    // Transaction Reference Currency Code
    UINT8  TransCurExp;        // Transation Currency Exponent
    UINT8  ReferCurExp;        // Transation Reference Currency Exponent
    UINT32 ReferCurrCon;       // Transaction Reference Currency Conversion
}EMVCOREINIT;

typedef struct
{
    UINT8   tCandAppName[33];
    UINT8   cFlgAPID;
    UINT8   cAPID;
}EMV_CANDAPP_LIST;

typedef struct
{
    INT32 (*emvSelCandApp)(void);   
    void  (*emvSelectLang)(void);   
    INT32 (*emvGetTransactionAmount)(UINT32 *, UINT32 *);  
    INT32 (*emvGetCardholderPin)(UINT8 *);				
    INT32 (*emvShowPinVerifyResult)(const UINT8);		
    INT32 (*emvCheckOnlinePIN)(void);
    INT32 (*emvCheckCertificate)(void); 
    INT32 (*emvOnlineProc)(UINT8 *,UINT8 *, INT32 *, UINT8 *, INT32 *,
                           UINT8 *, INT32 *);
    INT32 (*emvReferProc)(void);
    INT32  (*emvAdviceProc)(INT32);

	INT32 (*emvCheckCRL)(UINT8, UINT8 *, UINT8 *);
	INT32 (*emvCheckExceptionFile)(UINT8, UINT8 *, UINT8);
	INT32 (*emvGetTransLogAmount)(UINT8, UINT8 *, UINT8);
}EMV_CALLBACK_FUNC;

INT32 emvGetLibVer (void);
INT32 emvInitCallBackFunc(EMV_CALLBACK_FUNC *ptEMVCallBackFunc);
INT32 emvSetCoreInitPara(EMVCOREINIT *ptEMVCoreInit);
INT32 emvGetCoreInitPara(EMVCOREINIT *ptEMVCoreInit);
INT32 emvAppSelect(INT32 CardSlot, UINT32 TransNo);
INT32 emvReadAppData(void);
INT32 emvOfflineDataAuth(void);
INT32 emvProcTrans(void);
UINT16 emvGetICCSW(void);
INT32 emvGetScriptResult(UINT8 *Result, INT32 *RetLen);
INT32 emvCheckPrnReceipt(void);
INT32 emvCheckForceAccept(void);
INT32 emvCheckOnlineFlg(void);
void emvGetAllCandAppInfo(UINT32 *CandAppNum, EMV_CANDAPP_LIST *pCandApp);

INT32 emvAppSelection_NoGPO(INT32 CardSlot);
INT32 emvReadLogRecord(INT32 RecordNo);
INT32 emvGetLogItem(UINT8 *Tag, UINT8 TagLen, UINT8 *Value, INT32 *ValueLen);

// Callback function return value
#define ONLINE_APPROVE    0x00       //host approve the transaction
#define ONLINE_REFER      0x02       //host request issuer refer
#define ONLINE_DENIAL     0x03       //host decline the transaction

#endif

