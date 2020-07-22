
/****************************************************************************
NAME
	Poslib.h - ʵ�ֹ�������

REFERENCE

MODIFICATION SHEET:
	MODIFIED   (YYYY.MM.DD)
	pyming		2008.03.14		- add precompile process
****************************************************************************/

#ifndef _POSLIB_H
#define _POSLIB_H

#include "posapi.h"
// #define _POS_DEBUG

#ifndef TRUE
	#define TRUE	1
	#define FALSE	0
#endif

// macros for vDes()
#define ONE_DECRYPT     0           // ��DES����
#define ONE_ENCRYPT     1           // ��DES����
#define TRI_DECRYPT     2           // ��DES����(16 bytes key)
#define TRI_ENCRYPT     3           // ��DES����(16 bytes key)
#define TRI_DECRYPT3    4           // ��DES����(24 bytes key)
#define TRI_ENCRYPT3    5           // ��DES����(24 bytes key)

// macros for vCalcMac()
#define MAC_ANSIX99     0           // ANSI9.9 standard MAC algorithm
#define MAC_FAST        1           // HyperCom fast MAC algorithm

#define ERR_OPENFILE     -1          //���ļ�����
#define ERR_READFILE     -2          //���ļ�����
#define ERR_SEEKFILE     -3          //��λ�ļ�����
#define ERR_WRITEFILE    -4          //д�ļ�����


// macros for PubGetString()
#define NUM_IN			0x01	// ��������
#define ALPHA_IN		0x02	// ��ĸ���ֻ������
#define PASS_IN			0x04	// ��������
#define CARRY_IN		0x08	// �д�������
#define ECHO_IN			0x10	// ���Դ�������,���������ԭ����
#define HEX_IN          0x20    // ʮ����������      
#define AUTO_FONT		0x80	// RFU


// macros for screen functions
#define NUM_MAXZHCHARS      16          // ÿ������ַ���Ŀ(������)
#define NUM_MAXCOLS         128         // ÿ���������
#define NUM_MAXCOLNO        (NUM_MAXCOLS-1)     // ����к�
#define NUM_MAXLINES        8           // �������
#define NUM_MAXLINENO       (NUM_MAXLINES-1)    // ����к�
#define NUM_MAXZHLINES      4           // �����������
#define NUM_MAXCHARS		21

#define DISP_LINE_LEFT		0x80
#define DISP_LINE_RIGHT		0x40
#define DISP_LINE_CENTER	0x20
#define DISP_SCR_CENTER		0x10

// menu display mode
#define MENU_NOSPEC		0x00	// �Զ���ʾ
#define MENU_USESNO		0x01	// ʹ�ò˵��Լ��ı��(������MENU_AUTOSNO)
#define MENU_AUTOSNO	0x02	// �Զ����
#define MENU_CENTER		0x10	// �м������ʾ
#define MENU_PROMPT		0x20	// ǿ����ʾ��Ϣ�ڵײ�
#define MENU_ASCII		0x40	// ǿ����ʾС����
#define MENU_CFONT		0x80	// ǿ����ʾ������(������MENU_ASCII)

// types for PubGetMenu()
typedef void (*MenuFunc)(void);
typedef struct _tagMenuItem{
	uint8_t		bVisible;						// TRUE: show it, FALSE: hide
    char        szMenuName[NUM_MAXCHARS+1];		// menu name  ENGLISH
    MenuFunc    pfMenuFunc;						// callback function(if set)
}MenuItem;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/****************************************************************************
Function:      ���ϵͳ�������Ƿ��������
****************************************************************************/
int  PubCheckPosPower(void);

/****************************************************************************
 Function:      ��6Ƶ�ʷ���60ms��Ȼ��ֹͣ80ms. ����3��
****************************************************************************/
void PubBeepOk(void);		//

/****************************************************************************
 Function:       ��1Ƶ�ʷ���200ms��Ȼ��ֹͣ200ms. 
****************************************************************************/
void PubBeepErr(void);		//

/****************************************************************************
 Function:      ��6Ƶ�ʷ���800ms
****************************************************************************/
void PubLongBeep(void);	//

/****************************************************************************
 Function:      ����û����а��°�ť�Ĳ���.
 Return Code:
    TRUE        �а�������.
    FALSE       �ް�������.
****************************************************************************/
int PubChkKeyInput(void);

/****************************************************************************
 Function:      �����Ƶ�ʱ���ڣ��ȴ��û�����.
 Param In:
    ulWaitTimeMs   >0:  �ȴ��������ֵ��ʱ�䣬����Ϊ��λ.
                  <��0:  һֱ�ȴ�,ֱ���а�������
 Return Code:
                �û�����İ���ֵ�����߳�ʱ�˳�.
ע�⣺����PCIԭ����δע����棬��������ְ�����������Чֵ--0xFF
****************************************************************************/
int PubWaitKey(ulong ulWaitTime);


/****************************************************************************
 Function:      ���趨��֮���ڵȴ��û����� "ȷ��" ���� "ȡ��".
 Param In:
    nWaitTime   >0: �ȴ��������ֵ��ʱ�䣬����Ϊ��λ.
                =0:  һֱ�ȴ�,ֱ���а�������
 Param Out:
    none
 Return Code:
    TRUE        ����"ȷ��".
    FALSE       ����"ȡ��",���߳�ʱ�˳�.
****************************************************************************/
uint8_t PubYesNo(ulong ulWaitTime);


/****************************************************************************
 Function:      Ӧ�ó�����ִ���ʱ����ʾ����ǿ�ƹػ�.
 Param In:
    pszfile     �����������ڵ��ļ���.
    uiLine      �����������ڵ���.
****************************************************************************/
void PubHalt(uint8_t *pszfile, uint32_t uiLine);


//��ʾ
/****************************************************************************
 Function:      Show title at line zero & Clear screen first, if needed.
 Param In:
    bClrScreen  TRUE:  Clear screen before show title.
                FALSE: Keep original screen.
    pszTitle    Pointer of title to be displayed. string should end with '\0'.
 Param Out:
    none
 Return Code:   none
****************************************************************************/
void PubDisplayTitle(int IfClrScr, char *pszTitle);	//

/****************************************************************************
 Function:      Show message(CFONT) at ucLine, center-aligned. If the text
                is out of right-border, extra characters will be truncated.
 Param In:
    ucLine      Line number to show message. If ucLine & DISP_REVERSE is true, the
                background of the text is reversed.
    pszMsg      Pointer of string to be displayed. string should end with '\0'.
 Param Out:
    none
 Return Code:   none
****************************************************************************/
void PubDisplayMsg(uint8_t ucLine, uint8_t *pszMsg);			//

/****************************************************************************
 Function:      Show two message(CFONT) at ucLine and ucLine+2, center-aligned.
                If each text is out of right-border, extra characters will be
                truncated.
 Param In:
    ucLine      Line number to show first message. If ucLine & DISP_REVERSE is true,
                the background of the text is reversed. Since 2 lines of CFONT will
                be displayed, ucLine should be 0 to 4.
    pszMsg1     Pointer of 1st string to be displayed. string should end with '\0'.
    pszMsg2     Pointer of 2nd string to be displayed. string should end with '\0'.
 Param Out:
    none
 Return Code:   none
****************************************************************************/
void PubShowTwoMsg(uint8_t ucLine, uint8_t *pszMsg1, uint8_t *pszMsg2);		//


/****************************************************************************
 Function:      Display a string(CFONT) at specified line and backgroud.
 Param In:
    pszStr      Pointer of string to be displayed. string should end with '\0'.
	            get rid of bRever
    ucPosition  the value : (dispMode & 0x0F) indicates the line where to display,
                while the value : (dispMode & 0x0F) indicates the align rule.
                Line number should be 0 to 6, and align value should be DISP_SCR_CENTER,
                DISP_LINE_CENTER, DISP_LINE_RIGH and DISP_LINE_LEFT
 Param Out:
    none
 Return Code:   none
 Example:
                PubDispString("This is a sample", TRUE, DISP_LINE_LEFT+3);
                  will display the string in CFONT with black background, left align
                on the 4th line (use 0x00 to 0x07 to indicate line 1 to 8)
****************************************************************************/
void PubDispString(void *pszStr, uint8_t ucPosition);


/****************************************************************************
 Function:      Convert a numeric string(no dot) to the format of amount.
 Param In:
    pszPrefix   Prefix to the amount output, such as "HKD" in "HKD*****.**",
                   allow 4 character at most. if NULL, no prefix will be attached.
	pszIn		Pointer to input amount string (without dot), e.g "314277".
	ucDeciPos   Decimal position of the numeric input string. should be less than 4.
	               e.g. if ucDeciPos==2, "14277" may output "142.77"
				   if ucDeciPos==0, "14277" may output "14,277" or "14277"
    ucMisc      Misc parameter,
                   GA_SEPARATOR : use ',' to separater large amount: "12,345.67"
				   GA_NEGATIVE	: display negative sign: "SGD-123,45.67"
 Param Out:
	pszOut      Pointer to output amount string, the result may be like
	               "SGD$-3,142.77". (depending on call parameter)
 Return Code:
    TRUE        Keyboard is pressed.
    FALSE       No key is pressed.
****************************************************************************/
uint8_t PubConvAmount(uint8_t *pszPrefix, uint8_t *pszIn, uint8_t ucDeciPos, uint8_t *pszOut, uint8_t ucMisc);


/****************************************************************************
 Function:      Display menu & call menu process function, the max # of menu
                item is 99. Max characters of menu item name is 19 bytes.
 Param In:
    pszPrompt   Title/prompt of menu group(optional), NULL or zero length
                means none. string should end with '\0'.
    pstMenu     Array of menu structure, null string of menu name means end
                of menu items.
    ucMode      Mode for show menu:MENU_NOSPEC,MENU_CENTERR,MENU_AUTOSNO,
                MENU_USESNO,MENU_PROMPT,MENU_CFONT,MENU_ASCII
    nTimeOut    >0: Seconds before user select menu.
                =0:  Infinite.
 Param Out:
    none
 Return Code:
    >=0         Index of menu user selected. In the most time, user only need
                to set the callback function member and let PubGetMenu() call
                it automatically. But in some other unusual time user need set
                callback function member to NULL, and use this returned index
                to call the process function. This is useful for dynamically
                generated menus or applications which need pass parameters to
                the process function. For example, application manager can
                read information of applications and build menu, show it, then
                run application by the returned index of the menu.
    -1          NULL menu pointer.
    -2          Invalid menu content.
    -3          No menu content.
    -4          User cancel or timeout.
****************************************************************************/
int PubGetMenu(uint8_t *pszPrompt, MenuItem *pstMenu, uint8_t ucMode, uint32_t nTimeOut);


int PubGetMenuEx(uint8_t *pszPrompt, MenuItem *pstMenu, uint8_t ucFirstNo, uint8_t ucMode, uint32_t nTimeOut);

/****************************************************************************
 Function:      Read boolean parameter value.
 Param In:
    ucLine      Line number where to display prompt message.
    pszPrompt   Prompt message.
 Param Out:
    pucResult   Pointer of boolean parameter value.
 Return Code:
    0           OK.
    0xFF        User cancelled.
****************************************************************************/
uint8_t PubGetBoolParam(uint8_t ucLine, uint8_t *pszPrompt, uint8_t *pucResult);


/****************************************************************************
 Function:      Input amount value.
 Notice:        This function will use whole line6-7 and the last 4 character
                  of line4-5 to display the amount string and prefix, so do not
				  place prompt information at these places.
 Param In:
    pszPrefix   Pointer to the prefix string. e.g "HKD", "SGD$". max allow
                  4 bytes. if NULL of empty content, prefix won't be displayed.
    ucDeciPos   decimal position of the amount, should be same with tims/Protims para.
                  allow 0 to 5.
    ucMinLen    minimal amount length(digits).
    ucMaxLen    Maximal amount length(digits).
    ucTimeout   waiting timeout(seconds).
    ucMisc      Misc parameter,
                   GA_SEPARATOR : use ',' to separater large amount: "HKD$12,345.67"
				   GA_NEGATIVE	: display negative sign: "SGD-123,45.67"
 Param Out:
	pszData     Pointer to output string (end with '\0')
    pszData     Pointer of parameter string value.
 Return Code:
    0           OK.
    0xFE        Illegal parameter.
    0xFF        User cancelled.
****************************************************************************/
#define GA_SEPARATOR	0x80
#define GA_NEGATIVE		0x40
uint8_t PubGetAmount(uint8_t *pszPrefix, uint8_t ucDeciPos,
				  uint8_t ucMinLen, uint8_t ucMaxLen, uint8_t *pszData,
				  uint8_t ucTimeout, uint8_t ucMisc);

// get string of number/password/alpha
uint8_t PubGetString(uint8_t ucMode, uint8_t ucMinlen, uint8_t ucMaxlen, uint8_t *pszOut, uint8_t ucTimeOut);


//�ļ�
/****************************************************************************
 Function:     �ӵ�ǰ�ļ�ƫ��λ�ô�,��������
 Param In:
    fd          �ļ����--�������ļ�
	psData      ���ݻ�����
    iNeedByte   ϣ�����������ݳ���.
 Return Code:
    ʵ�ʶ��������ݳ���.
****************************************************************************/
int PubFReadN(int fd, void *psData, int iNeedBytes);

/****************************************************************************
 Function:      ��ָ�����ļ����ж�������.
 Param In:
    pszFileName �ļ���.
    lOffset     ƫ��λ��.
    iLen        ��ȡ���ļ����ݳ���.
 Param Out:
    pszData     ���ݻ�����.
 Return Code:
    0:          OK.
    -1:         ���ļ�����.
    -3:         ��λ�ļ�����.
    -2:         ���ļ�����.
****************************************************************************/
int PubFileRead(uint8_t *pszFileName, long lOffset, void *psData, int iLen);		//

/****************************************************************************
 Function:      ��չȨ�޵��ļ������������ú���������PubFileRead.
                ���Ǹú������Զ�ȡ����Ӧ�õ��ļ����ݣ��������ں�����
****************************************************************************/
int PubFileReadEX(uint32_t AppID,uint32_t FileType,uint8_t *pszFileName, long lOffset, void *psData, int iLen);		//

/****************************************************************************
 Function:      �����ݱ��浽�ļ���ָ��ƫ��λ��
 Param In:
    fd          д����ļ����
	psData      ���������
    iNeedBytes  �������ݵĳ���
 Return Code:
    ����д������ݳ���.
****************************************************************************/
int PubFWriteN(int fd, void *psData, int iNeedBytes);

/****************************************************************************
 Function:      �����ļ�.
 Param In:
    pszFileName �ļ���.
    lOffset     ���ݱ����ƫ��λ��.
    pszData     ���ݻ�����.
    iLen        д������ݳ���.
 Return Code:
	0:          OK.
   -1:         ���ļ�����.
   -3:         ��λ�ļ�����.
   -4:         д�ļ�����.
****************************************************************************/
int PubFileWrite(uint8_t *pszFileName, long lOffset, void *psData, int iLen);	//

/****************************************************************************
 Function:      ��չȨ�޵��ļ����溯�����ú���������PubFileWrite.
 ���Ǹú������Ա�������Ӧ�õ��ļ����ݣ��������ں�����
****************************************************************************/
int PubFileWriteEX(uint32_t AppID,uint32_t FileType,uint8_t *pszFileName, long lOffset, void *psData, int iLen);	//


//����
/****************************************************************************
Function:      �๦��DES����/��������
Param In:
    ucMode      DES����/����ģʽ:
                ONE_ENCRYPT --> ��DES����
                ONE_DECRYPT --> ��DES����
                TRI_ENCRYPT --> ��DES����
                TRI_DECRYPT --> ��DES����
    psData:     ������/���ܵ�����(8�ֽ�)
    psKey:      DES�������Կ(�������Ȼ�����������,����ucModeȷ��)
 Param Out:
    psResult    DES����Ľ��(8�ֽ�)
 Return Code:   none
****************************************************************************/
void PubDes(uint8_t ucMode, uint8_t *psData, uint8_t *psKey, uint8_t *psResult);	//

/****************************************************************************
 Function:      ����MAC���㺯��
 Param In:
    ucMode      ����MAC����ģʽ:
                MAC_FAST    --> ����MAC����(HyperCom���ٷ�ʽ)
                MAC_ANSIX99 --> ANSI x9.9 ��ʽ
    psKey:      MAC�������Կ(8�ֽ�)
    psMsg:      ����MAC������
    uiLength:   ����MAC�����ݵĳ���
 Param Out:
    psMAC       ���ĵ�MAC(8�ֽ�)
 Return Code:   none
****************************************************************************/
void PubCalcMac(uint8_t ucMode, uint8_t *psKey, uint8_t *psMsg, uint32_t uiLen, uint8_t *psMAC);	//


/****************************************************************************
 Function:      ����ַ����Ƿ��������ַ���
 Param In:     
       psString:   �������ַ���
	   uiLen:      �ַ�������
 Param Out:
       ��
 Return Code:   TRUE: �������ַ���; FALSE: �������ַ���
****************************************************************************/
int PubCheckCHNString(char *psString, int iLen);	//


#ifdef _POS_DEBUG

/****************************************************************************
 Function:      Show HEX message on screen.
 Param In:
    pszTitle    Title of the message.
    psMsg       Pointer of message to be displayed.
    uiMsgLen    Bytes of the message.
    nTimeOut    Seconds between user interaction.
 Param Out:
    none
 Return Code:   none
****************************************************************************/
void DispHexMsg(uint8_t *pszTitle, uint8_t *psMsg, uint32_t uiMsgLen, short nTimeOut);

// print a line as hexadecimal format
int DispHexLine(uint8_t *pszBuff, uint32_t uiOffset, uint8_t *psMsg, uint32_t uiMsgLen);

// For Debug use, disp file name and line
void DispAssert(uint8_t *pszFileName, ulong ulLineNo);

    // debug macro for boolean expression
	#define _POS_DEBUG_WAIT		15
    #define PubASSERT(expr)                                           \
                if( !(expr) ){                                      \
                    DispAssert((uint8_t *)__FILE__, (ulong)(__LINE__));       \
                }

    // print string debug information
    #define PubTRACE0(sz)                                             \
            {                                                       \
                lcdCls();                                           \
				lcdFlip();	
                lcdDisplay(0, 0, DISP_CFONT, "%s", sz);                    \
                PubWaitKey(_POS_DEBUG_WAIT);							\
            }

    #define PubTRACE1(sz, p1)                                         \
            {                                                       \
                lcdCls();                                           \
				lcdFlip();	
                lcdDisplay(0, 0, DISP_CFONT, sz, p1);                      \
                PubWaitKey(_POS_DEBUG_WAIT);							\
            }

    #define PubTRACE2(sz, p1, p2)                                     \
            {                                                       \
                lcdCls();                                           \
				lcdFlip();	
                lcdDisplay(0, 0, DISP_CFONT, sz, p1, p2);                  \
                PubWaitKey(_POS_DEBUG_WAIT);							\
            }

    #define PubTRACE3(sz, p1, p2, p3)                                 \
            {                                                       \
                lcdCls();                                           \
				lcdFlip();	
                lcdDisplay(0, 0, DISP_CFONT, sz, p1, p2, p3);              \
                PubWaitKey(_POS_DEBUG_WAIT);							\
            }

    #define PubTRACEHEX(t, s, l)    DispHexMsg((t), (s), (l), _POS_DEBUG_WAIT)

#else /* _POS_DEBUG */

    #define PubASSERT(expr)
    #define PubTRACE0(sz)
    #define PubTRACE1(sz, p1)
    #define PubTRACE2(sz, p1, p2)
    #define PubTRACE3(sz, p1, p2, p3)
    #define PubTRACEHEX(t, s, l)

#endif /* _POS_DEBUG */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _UTIL_H */
