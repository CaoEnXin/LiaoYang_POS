/***************************************************************************
* Copyright (c) 2007-2010, NEW POS Technology.
* All rights reserved.
*
* �ļ����ƣ�Epp.h
* ����������   ������NEW8110�ϵ�N20 EPPӦ��API�ӿڣ���Ҫʵ��������̵Ļ����ӽ��ܹ���
               // ������ϸ˵���˳����ļ���ɵ���Ҫ���ܣ�������ģ������Ľ�
*              // �ڣ����ֵ��ȡֵ��Χ�����弰������Ŀ��ơ�˳�򡢶���������
*              // �ȹ�ϵ
* ��������:    // �����������ݵ�˵��
*
* �� �ߣ�
* �汾��1.0
* ������ڣ�200x��xx��xx��
*
* �汾�޸���ʷ��Ϣ
*
* ���� ���ƿ���
* �汾��1.0
* �޸����� ��1������
* ������ڣ�2010��05��17��
****************************************************************************/

#ifndef _PINPAD_H_
#define _PINPAD_H_

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif

#define   TEST_VERSION


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define EPP_PORT PORT_COM1

#define KEY_TYPE_MASTER      0x01
#define KEY_TYPE_MAC         0x02
#define KEY_TYPE_PIN         0x03
#define KEY_TYPE_DUKPT_MAC   0x04
#define KEY_TYPE_DUKPT_PIN   0x05
#define KEY_TYPE_FIXED_MAC   0x10
#define KEY_TYPE_FIXED_PIN   0x11

#define EPPINFO_PRODUCT_NAME    0x00
#define EPPINFO_HW_VER          0x01
#define EPPINFO_HW_CFG          0x02
#define EPPINFO_EPP_SN          0x03
#define EPPINFO_CPU_ID          0x04
#define EPPINFO_BOOT_VER        0x10
#define EPPINFO_KERNEL_VER      0x11
#define EPPINFO_PROTOCOL_VER    0x20


enum EPP_ERROR_CODE
{
    EPP_SUCCESS = 0,
    EPP_RECV_PACKET_ERROR=2001,
    EPP_PORT_OPEN_ERROR,
    EPP_SEND_PACKET_ERROR,  // 03
    EPP_PACKET_LEN_ERROR,
    EPP_RECV_TIMEOUT,  // 05
    EPP_PACKET_LEN_TOO_LONG,
    EPP_CRC_CHECK_ERROR,  // 07
    EPP_OPEN_FILE_ERROR,
    EPP_SEEK_FILE_ERROR,  // 09
    EPP_WRITE_FILE_ERROR,
    EPP_READ_FILE_ERROR,  // 11
    EPP_CONFIGURE_INVALID,
    EPP_CONFIGURE_MAC_ERROR,  // 13
    EPP_NO_PIN,
    EPP_SEND_CMD_ERROR,  // 15
    EPP_RECV_CMD_ERROR,
    EPP_RECV_RET_ERROR,  // 17
    EPP_RECV_LEN_ERROR,
    EPP_MAC_CHECK_ERROR,  // 19
    EPP_AUTHEN_FAILED,
    EPP_INPUT_PARAM_ERROR,  // 21
    EPP_USER_PRESS_CANCEL,
    EPP_INPUT_CMD_ERROR,  // 23
    EPP_INPUT_KEY_INDEX_ERROR,
    EPP_INPUT_MAC_LEN_ERROR, // 25
    EPP_INPUT_MODE_ERROR,
    EPP_KEY_TYPE_ERROR,  // 27
    EPP_KEY_INDEX_ERROR,
    EPP_MASTER_KEY_INDEX_ERROR,  // 29
    EPP_BMP_ERROR,
    EPP_PIN_CFG_LANGE_TYPE_ERROR,  // 31
    EPP_PIN_CFG_SYMBOL_TYPE_ERROR,
    EPP_INPUT_DATA_LEN_ERROR, // 33
};


enum N20_RETURN_CODE{
    RET_ERROR_BASE = 2300,
    RET_PASSWORD_ERROR,  // 01
    RET_BOOT_LEN_ERROR,
    RET_BOOT_HASH_ERROR, // 03
    RET_BOOT_MAC_ERROR,
    RET_NOT_UBSK,  // 05
    RET_CHECK_UBSK_FAILED,
    RET_CHECK_KSK_FAILED,  // 07
    RET_NOT_LCK,
    RET_NOT_AUTH,  // 09
    RET_NOT_LOGON,
    RET_RECV_FAILED,  // 11
    RET_AUTH_CHECK_FAILED,
    RET_MAC_CHECK_FAILED,  // 13
    RET_CRC_CHECK_ERROR,
    RET_KVC_CHECK_ERROR,  // 15
    RET_NOT_APP_FREE_SPACE,
    RET_APP_NOT_EXIST,  // 17
    RET_KEY_TYPE_ERROR,
    RET_KEY_TYPE_NOT_SUPPORT,  // 19
    RET_MAC_LEN_ERROR,
    RET_MAIN_KEY_INDEX_ERROR,  // 21
    RET_MAIN_KEY_NOT_EXIST,
    RET_KEY_INDEX_ERROR,  // 23
    RET_KEY_LEN_ERROR,
    RET_KEY_BE_USE,  // 25
    RET_MAIN_KEY_BE_USE,  // 26
    RET_MAC_ALG_MODE_ERROR,
    RET_KEY_GET_ERROR,
    RET_KEY_OVER_FLOW,  // 29
    RET_KEY_SAME_ERROR,
    RET_KEY_NOT_EXIST,  // 31
    RET_KEY_MAC_CHECK_FAILED,
    RET_KEY_KSN_LEN_ERROR,  // 33
    RET_KEY_BDK_LEN_ERROR,
    RET_USER_PRESS_CANCEL_EXIT,  // 35
    RET_USER_INPUT_TIME_OUT,
    RET_KSN_LEN_RERROR,  // 37
    RET_APP_NUM_OVER_FLOW,
    RET_RW_LENGTH_ERROR,  // 39
    RET_PIN_KEY_TIME_LOCK,
    RET_MAC_KEY_TIME_LOCK,  // 41
    RET_SET_PIN_TIME_OUT_ERROR,
    RET_WRITE_EEPROM_FAILED,  // 43
    RET_RECV_LEN_ERROR,
    RET_PARAM_ERROR  // 45
};




// ��Pinpad�ڣ���N8110�ϵĴ��ں�ΪPORT_COM1
int EppOpen(INT32 PortNum);

// �ر�Pinpad��
int EppClose(void);


/****************************************************************************
  ������     :  int EppInit(const BYTE *pbyLck, const BYTE *pbyMtek, const BYTE *pbyAppName,
                      const BYTE *pbyAkdak, const BYTE *pbyAkuak, const BYTE *pbyMdtek,
                      const BYTE *pbyMutek)
  ����       :  �޸ĵ�ǰ��Կ���ã������ʹ��Ĭ�����ã�ÿ���豸������Ҫ��ʼ������
  �������   :  1��const BYTE *pbyLck ��16�ֽ���Կ
                2��const BYTE *pbyMtek ��24�ֽ���Կ
                3��const BYTE *pbyAppName ���16�ֽڵ�Ӧ����
                4��const BYTE *pbyAkdak ��16�ֽ���Կ
                5��const BYTE *pbyAkuak ��16�ֽ���Կ
                6��const BYTE *pbyMdtek ��24�ֽ���Կ
                7��const BYTE *pbyMutek ��24�ֽ���Կ
  �������   :  ��
  ����ֵ     :  EPP_SUCCESS
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppInit(const BYTE *pbyLck, const BYTE *pbyMtek, const BYTE *pbyAppName, const BYTE *pbyAkdak,
    const BYTE *pbyAkuak, const BYTE *pbyMdtek, const BYTE *pbyMutek);


/****************************************************************************
  ������     :  int EppDownloadLckMtek(void)
  ����       :  ����ǰ�����е�LCK��MTEK���ص�N20��
  �������   :  ��
  �������   :  ��
  ����ֵ     :  EPP_SUCCESS / EPP_CRC_CHECK_ERROR / EPP_SEND_CMD_ERROR
                / EPP_PACKET_LEN_ERROR / EPP_RECV_TIMEOUT
                / EPP_RECV_PACKET_ERROR / EPP_USER_PRESS_CANCEL
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppDownloadLckMtek(void);

/****************************************************************************
  ������     :  int EppDownloadAik(void)
  ����       :  ����ǰ������AIK���ص�N20�ϡ�AIK����AppName��AKDAK/AKUAK/MDTEK/MUTEK��4����Կ��
  �������   :  ��
  �������   :  ��
  ����ֵ     :  EPP_SUCCESS / EPP_CRC_CHECK_ERROR / EPP_SEND_CMD_ERROR
                / EPP_PACKET_LEN_ERROR / EPP_RECV_TIMEOUT / EPP_RECV_LEN_ERROR
                / EPP_RECV_PACKET_ERROR / EPP_USER_PRESS_CANCEL / EPP_AUTHEN_FAILED
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppDownloadAik(void);

/****************************************************************************
  ������     :  int EppDownloadAppKey(BYTE byKeyType, BYTE byMode, BYTE byKeyIndex,
                     const BYTE *pbyKeyDataIn, BYTE byKeyLen, BYTE byMasterKeyIndex)
  ����       :  Ӧ����Կ����
  �������   :  1��BYTE byKeyType ����Կ����
                            0x01������Կ��
                            0x02��MAC��Կ��
                            0x03��PIN��Կ��
                            0x10��Fixed MAC��Կ��
                            0x11��Fixed PIN��Կ��
                2��BYTE byKeyIndex ����Կ����[1��100]
                3��BYTE byMode ��
                         Bit0~3  TDEA_DECRYPT    0x00    ���ý��ܷ����õ�����Ŀ��Key
                                 TDEA_NONE       0x02    ֱ��д������Ŀ��Key
                         Bit4~7  PARITY_NONE     0x00    ��(���ܺ��)Key������У��
                                 PARITY_ODD      0x10    ��(���ܺ��)Key���Ľ�����У��
                                 PARITY_EVEN     0x20    ��(���ܺ��)Key���Ľ���żУ��
                4��const BYTE *pbyKeyDataIn ����Կ����
                5��BYTE byKeyLen ����Կ���ȣ���ȡֵ8/16/24
                6��BYTE byMasterKeyIndex ������Կ����[1��100]��ֻ����Կ������MAC��Կ����
                        PIN��Կ��ʱ�򣬸ò�������Ч
  �������   :  ��
  ����ֵ     :  EPP_SUCCESS / EPP_CRC_CHECK_ERROR / EPP_SEND_CMD_ERROR / EPP_KEY_TYPE_ERROR
                / EPP_PACKET_LEN_ERROR / EPP_RECV_TIMEOUT / EPP_RECV_LEN_ERROR
                / EPP_RECV_PACKET_ERROR / EPP_USER_PRESS_CANCEL
                / EPP_MASTER_KEY_INDEX_ERROR / EPP_KEY_INDEX_ERROR / EPP_INPUT_DATA_LEN_ERROR
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppDownloadAppKey(BYTE byKeyType, BYTE byMode, BYTE byKeyIndex,
    const BYTE *pbyKeyDataIn, BYTE byKeyLen, BYTE byMasterKeyIndex);



/****************************************************************************
  ������     :  int EppDownloadDukptKey(BYTE byKeyIndex,  BYTE *pbyBDK, BYTE *pbyKSN,
                    BYTE byKSNLen)
  ����       :  Ӧ����Կ����
  �������   :  1��BYTE byKeyIndex ����Կ������ȡֵ��Χ[1,32]
                2��BYTE *pbyBDK ��16�ֽ�BDK���룬δ����ɢ��BDK
                3��BYTE *pbyKSN ��KSN����
                4��BYTE byKSNLen ��KSN����[1,10]
  �������   :  ��
  ����ֵ     :  EPP_SUCCESS/EPP_OPEN_FILE_ERROR/EPP_SEEK_FILE_ERROR/EPP_READ_FILE_ERROR/
                EPP_CONFIGURE_INVALID/EPP_CONFIGURE_MAC_ERROR/EPP_SEND_CMD_ERROR/
                EPP_RECV_CMD_ERROR/EPP_RECV_RET_ERROR/EPP_RECV_LEN_ERROR/EPP_AUTHEN_FAILED
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-06-08  V1.0         ����
****************************************************************************/
//int EppDownloadDukptKey(BYTE byKeyIndex,  BYTE *pbyBDK, BYTE *pbyKSN, BYTE byKSNLen);



/****************************************************************************
  ������     :  int EppGetRand(BYTE *pbyRandOut)
  ����       :  ��ȡ�����
  �������   :  ��
  �������   :  1��BYTE *pbyRandOut ���������8�ֽ�
  ����ֵ     :
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppGetRand(BYTE *pbyRandOut);

/****************************************************************************
  ������     :  int EppGetMac(BYTE byKeyType, BYTE byKeyIndex, const BYTE *pbyMacDataIn,
                        WORD wMacLen, BYTE byMode, BYTE *pbyMacOut)
  ����       :  MAC����API
  �������   :  1��BYTE byKeyType ����Կ���͡�KEY_TYPE_MAC/KEY_TYPE_FIXED_MAC/KEY_TYPE_DUKPT
                2��BYTE byKeyIndex ��MAC��Կ�����������MAC����Fixed MAC��ȡֵ[1,100]��
                                     �����DUKPT MAC���ȡֵ[1,32]
                3��const BYTE *pbyMacDataIn �����������MAC����
                4��WORD wMacLen ��MAC���ݳ��ȣ�������8������������СֵΪ8�����ֵΪ2024
                5��BYTE byMode �������㷨ѡ��
                                     0x00	�㷨1
                                     0x01	�㷨2
                                     0x02	EMV2000�㷨
                                     0x03	�й������㷨
  �������   :  1��BYTE *pbyMacOut ��8�ֽ�MAC�������������������DUKPT_MAC_KEY����ǰ8���ֽ�
                   ��MAC����������10���ֽ��ǵ�ǰKSN��
  ����ֵ     :  EPP_SUCCESS/EPP_SEND_CMD_ERROR/EPP_RECV_CMD_ERROR/EPP_RECV_RET_ERROR/
                EPP_RECV_LEN_ERROR/EPP_AUTHEN_FAILED/
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppGetMac(BYTE byKeyType, BYTE byKeyIndex, const BYTE *pbyMacDataIn,
                        WORD wMacLen, BYTE byMode, BYTE *pbyMacOut);

#ifdef   TEST_VERSION
int EppGetMacExt(BYTE byKeyType, BYTE byKeyIndex, const BYTE *pbyMacDataIn,
                        WORD wMacLen, BYTE byMode, BYTE *pbyMacOut, DWORD *pdwOut);
#endif


/****************************************************************************
  ������     :  int EppGetPin(BYTE byKeyType, BYTE byKeyIndex, BYTE byDisplayMode,
            BYTE byMode, const BYTE *pbyCardInfo, const BYTE *pbyLenInfo, BYTE *pbyPinBlockOut)
  ����       :  MAC����API
  �������   :  1��BYTE byKeyType ����Կ���͡�KEY_TYPE_PIN/KEY_TYPE_FIXED_PIN/KEY_TYPE_DUKPT
                2��BYTE byKeyIndex ����Կ�����������PIN����Fixed PIN��ȡֵ[1,100]��
                                     �����DUKPT PIN���ȡֵ[1,32]
                3��BYTE byDisplayMode ��PIN��ʾģʽ��0��ʾ�����ң�1��ʾ���ҵ���2��ʾ���С�
                4��BYTE byMode �������㷨ѡ��
                                        0x00��ʾѡ��Format 0ģʽ���㷨��
                                        0x01��ʾѡ��Format EMVģʽ���㷨��
                                        0x0a��ʾѡ��Format EPSģʽ���㷨��
                5��const BYTE *pbyCardInfo ��16������Ϣ�����峤�ȱ��벻С��16�ֽ�
                6��const BYTE *pbyLenInfo �����������PIN��Կ����ö��ֵ�ַ��������ȿ�ȡֵ0,4~12����
                            �����ַ�'0'��'4'��������'9'��'a'��'b'��'c'��ʾ��������������
                            "0456a\0\0\0\0\0\0\0\0"�����ʾ֧�ֳ���Ϊ0��4��5��6��10��PIN���롣
                            ���峤�Ȳ�С��13�ֽڡ�
  �������   :  1��BYTE *pbyPinBlockOut �����峤����СΪ19�ֽڣ�ǰ8�ֽ���PIN���������ھŸ�
                   �ֽڱ�ʾ�û��Ƿ�����PIN�������0���ʾû����PIN��1��ʾ������PIN��
                   �����DUKPT���������9~18�ֽ��ǵ�ǰKSN
  ����ֵ     :  EPP_SUCCESS/EPP_SEND_CMD_ERROR/EPP_RECV_CMD_ERROR/EPP_RECV_RET_ERROR/
                EPP_RECV_LEN_ERROR/EPP_AUTHEN_FAILED/0x0d:No PIN
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppGetPin(BYTE byKeyType, BYTE byKeyIndex, BYTE byDisplayMode,
            BYTE byMode, const BYTE *pbyCardInfo, const BYTE *pbyLenInfo, BYTE *pbyPinBlockOut);


/****************************************************************************
  ������     :  int EppGetTdea(WORD wMode, BYTE byKeyType, BYTE  byKeyIndex, WORD wDataLen,
                   const BYTE *pbyDataIn, BYTE *pbyDataOut)
  ����       :  TDEA����API
  �������   :  1��WORD wMode  ���㷨ģʽ
                                TDEA_ENCRYPT  ��������
                                TDEA_DECRYPT  ��������
                                TDEA_MODE_ECB---ʹ��ECBģʽ
                                TDEA_MODE_CBC---ʹ��CBCģʽ
                                ��ο�8110��SDK�궨��
                2��BYTE byKeyType ����Կ���͡�
                                0x01������Կ��
                                0x02��MAC��Կ��
                                0x03��PIN��Կ��
                                0x04��DUKPT MAC��Կ���ݲ�֧��
                                0x05��DUKPT PIN��Կ���ݲ�֧��
                                0x10��Fixed MAC��Կ��
                                0x11��Fixed PIN��Կ��
                3��BYTE byKeyIndex ��MAC��Կ�����������MAC����Fixed MAC��ȡֵ[1,100]��
                                     �����DUKPT MAC���ȡֵ[1,32]
                4��WORD wDataLen ��MAC���ݳ��ȣ�������8������������СֵΪ8�����ֵΪ2024
                5��const BYTE *pbyDataIn ���������������
  �������   :  1��BYTE *pbyDataOut ��������
  ����ֵ     :  EPP_SUCCESS/EPP_SEND_CMD_ERROR/EPP_RECV_CMD_ERROR/EPP_RECV_RET_ERROR/
                EPP_RECV_LEN_ERROR/EPP_AUTHEN_FAILED/
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppGetTdea(WORD wMode, BYTE byKeyType, BYTE  byKeyIndex, WORD wDataLen,
                   const BYTE *pbyDataIn, BYTE *pbyDataOut);


/****************************************************************************
  ������     :  int EppSetPinInputTimeout(WORD wTimeout100Ms)
  ����       :  ����PIN���볬ʱʱ�䳤��
  �������   :  1��WORD wTimeout100Ms : ��ʱʱ�䣬��λ100���룬���ֵ1200��120�룩��
                   ���ȡֵ0�����ʾʹ��Ĭ��ֵ120�롣
  �������   :  ��
  ����ֵ     :
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppSetPinInputTimeout(WORD wTimeout100Ms);

/****************************************************************************
  ������     :  int EppSetTdesIv(const BYTE *pbyIvData)
  ����       :  ����Tdes��ʼ������������ֻ��CBC�㷨��Ч
  �������   :  1��const BYTE *pbyIvData : ��ʼ������8�ֽڡ�
  �������   :  ��
  ����ֵ     :
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppSetTdesIv(const BYTE *pbyIvData);

/****************************************************************************
  ������     :  int EppClearAKey(BYTE byKeyType, BYTE byKeyIndex)
  ����       :  ���ĳ����Կ
  �������   :  1��BYTE byKeyType ����Կ����
                                0x01������Կ��
                                0x02��MAC��Կ��
                                0x03��PIN��Կ��
                                0x10��Fixed MAC��Կ��
                                0x11��Fixed PIN��Կ��
                2��BYTE byKeyIndex ����Կ����
  �������   :  ��
  ����ֵ     :
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppClearAKey(BYTE byKeyType, BYTE byKeyIndex);

/****************************************************************************
  ������     :  int EppClearOneAppKey(void)
  ����       :  �����ǰӦ�õ�������Կ
  �������   :  ��
  �������   :  ��
  ����ֵ     :
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppClearOneAppKey(void);

/****************************************************************************
  ������     :  int EppFormatPed(void)
  ����       :  �����������ڵ�����PED�������ݣ�SIEK����
  �������   :  ��
  �������   :  ��
  ����ֵ     :
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppFormatPed(void);

/****************************************************************************
  ������     :  int EppSetIdleLogo(BYTE *pbyBmpIdleLogoIn)
  ����       :  ����IDLE LOGO
  �������   :  1��BYTE *pbyBmpIdleLogoIn ���ֱ���Ϊ122*32�ĵ�ɫBMP�ļ�����������
  �������   :  ��
  ����ֵ     :
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppSetIdleLogo(const BYTE *pbyBmpIdleLogoIn);


/****************************************************************************
  ������     :  int EppRestoreDefaultIdleLogo(void)
  ����       :  �������IDLE LOGO���ָ�Ĭ��IDLE LOGO
  �������   :  ��
  �������   :  ��
  ����ֵ     :
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppRestoreDefaultIdleLogo(void);


/****************************************************************************
  ������     :  int EppDisplayLogo(BYTE *pbyBmpIdleLogoIn)
  ����       :  ��Һ��������ʾͼ����Ϣ
  �������   :  1��BYTE *pbyBmpIdleLogoIn ��
  �������   :  ��
  ����ֵ     :
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppDisplayLogo(const BYTE *pbyBmpIdleLogoIn);

int EppClearScreen(void);

int EppDisplayString(int X, int Y, int iMode, const void *str, int iStrLen);

// �������Խӿڣ�Ӧ�ó���ʹ�ò���
int EppEnterFactoryTest(BYTE bySpeed);

/****************************************************************************
  ������     :  int EppGetSystemInfo(BYTE byType, void *pvInfoOut)
  ����       :  ��ȡEppϵͳ��Ϣ
  �������   :  1��BYTE byType ��
                          ��ѯ���
                          0x00��Ʒ���ƣ�
                          0x01Ӳ���汾��
                          0x02Ӳ�����ã�
                          0x03 �������кţ�
                          0x04 CPU SN��
                          0x10 boot�汾��
                          0x11 Kernel�汾��
                          0x20 ��ѯЭ��汾��
  �������   :  1��void *pvInfoOut ��16�ֽڵ������Ϣ
  ����ֵ     :
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppGetSystemInfo(BYTE byType, void *pvInfoOut);

/****************************************************************************
  ������     :  int EppBeep(DWORD dwFrequency, DWORD dwTimeMs)
  ����       :  ��Epp����������
  �������   :  1��DWORD dwFrequency ��
                        Ƶ�ʿ��Ʋ�����
                        (1)���ȡֵ��0��6�����ӦƵ����1680, 1834, 2020, 2127, 2380, 2700, 2900��
                        (2)���ȡֵ���ڵ���7���򰴸�ֵ������Ƶ�ʣ������ֵ��1500����Ƶ����1500��
                        ˵�����ò�������N20�����壬N20��֧�̶ֹ���2500hz��Ƶ�ʡ�
                  2��DWORD dwTimeMs ������ʱ�䳤�ȣ���λ����
  �������   :  ��
  ����ֵ     :
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppBeep(DWORD dwFrequency, DWORD dwTimeMs);

/****************************************************************************
  ������     :  int EppLight(DWORD dwMode, DWORD dwTimeMs)
  ����       :  Epp�������
  �������   :  1��DWORD dwMode �����⣨ָʾ�ƣ�����ģʽ
                            1  ���⣨ָʾ�ƣ���ʱ����ָ����ʱ�䳤�����ţ�ģʽ
                            2  ���⣨ָʾ�ƣ�������һֱ�򿪣�ģʽ
                            3  ���⣨ָʾ�ƣ����أ�һֱ�رգ�ģʽ
                2��DWORD dwTimeMs ��
                              �������ʱ�䳤�ȣ���λΪ���롣
                              �ò���ֻ����ģʽ1��ʱ�����Ч��
  �������   :  ��
  ����ֵ     :
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2010-12-11  V1.0         ����
****************************************************************************/
int EppLight(DWORD dwMode, DWORD dwTimeMs);

int EppKbGetString(int iMode, int iMinLen, int iMaxlen, int iTimeOutMs, void *strBuff);
















typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;


u32 str2long(const void *buf);  // �ַ���ת������
void long2str(u32 val, void *buf); // ������ת�ַ���
u16 str2short(const void *buf);
void short2str(u16 val, void *buf);

//  N30����
void EppReset(void);




/*
//============================ ICC MODULE ======================================
// ������
#define     ICC_USERCARD          0x00        // ������
#define     ICC_SAM1              0x11        // SAM����1
#define     ICC_SAM2              0x12        // SAM����2
#define     ICC_SAM3              0x13        // SAM����3

// ICC��ͨ�Ų���
#define     ICC_VOL_5V            0x05        //  5V��
#define     ICC_VOL_3V            0x03        //  3V��
#define     ICC_VOL_1_8V          0x01        //  1.8V��

#define     ICC_ISO               0x00        // ����ISO7816�淶,�ϵ��ʼ��ͨ���ٶ�Ϊ372CLK/Bit
#define     ICC_EMV               0x01        // ����EMV v4.1�淶,�ϵ��ʼ��ͨ���ٶ�Ϊ372CLK/Bit
#define     ICC_19200             0x02        // �ϵ��ʼ��ͨ���ٶ�Ϊ186CLK/Bit
#define     ICC_38400             0x03        // �ϵ��ʼ��ͨ���ٶ�Ϊ93CLK/Bit

// ICC����ͨ�Žṹ
typedef struct
{
    UINT8       CLA;
    UINT8       INS;
    UINT8       P1;
    UINT8       P2;
    UINT8       Lc;
    UINT8       DataIn[300];
    UINT8       LeFlag;
    UINT8       Le;
}APDU_SEND;

typedef struct
{
    UINT32      LenOut;
    UINT8       DataOut[300];
    UINT8       SWA;
    UINT8       SWB;
}APDU_RESP;

//  ����ֵ����
#define     ICC_SUCCESS          0x00      // �������ɹ�
#define     ICC_TIMEOUT          0x01      // ����ʱ��Ӧ
#define     ICC_COMMERR          0x02      // ��ͨ�Ŵ���(��ż���LRC��)
#define     ICC_RESPERR          0x03      // ��Ƭ������Ϣ����
#define     ICC_CARDOUT          0x04      // ������λ
#define     ICC_NOPOWER          0x05      // ��δ�ϵ�
#define     ICC_ERRORSLOT        0x06      // ��������
#define     ICC_ERRPARA          0x07      // ��������
*/
int EppIccSetPara(int CardSlot, u32 CardVol, u32 CardType);
int EppIccDetect(int CardSlot);
int EppIccPowerUp(int CardSlot, u32 *pAtrLen, u8 *pAtrData);
int EppIccExchangeAPDU(int CardSlot, const APDU_SEND *pSendApdu, APDU_RESP *pRespApdu);
int EppIccPowerDown(int CardSlot);



/*
// �����Ͷ���
#define MIF_M1_S50          0x00    // S50�洢�ṹ��M1��
#define MIF_M1_S70          0x01    // S70�洢�ṹ��M1��
#define MIF_ULTRALIGHT      0x02    // mifare ultralight
#define MIF_DESFIRE         0x03    // mifare desfire
#define MIF_PRO             0x04    // ֧��ISO14443-4Э���TypeA��
#define MIF_PRO_S50         0x05
#define MIF_PRO_S70         0x06
#define MIF_TYPEB           0x07
#define MIF_TYPEB_TCL       0x08    // ֧��ISO14443-4Э���TypeB��
#define MIF_FELICA          0x09    // SonyFeliCa
#define MIF_UNKNOWEN_TYPEA  0x0A    // δ֪���͵�TypeA��

// ָʾ�Ʊ�ʾ
#define MIF_RED_LED             0x00    // ��ɫ��
#define MIF_BLUE1_LED           0x01    // ��ɫһ�ŵ�
#define MIF_BLUE2_LED           0x02    // ��ɫ���ŵ�
#define MIF_GREEN_LED           0x03    // ��ɫ��

// ���巵��ֵ
#define MIF_RET_SUCCESS         0       // ������ȷ
#define MIF_RET_TRANSERR        -1      // ͨ��ʧ��
#define MIF_RET_TIMEOUT         -2      // ��ʱ����
#define MIF_RET_PROTERR         -3      // ���ݴ�
#define MIF_RET_MULTICARD       -4      // �࿨����
#define MIF_RET_NOCARD          -5      // �޿�
#define MIF_RET_ERRPARA         -6      // ��������
#define MIF_RET_NOACTIVED       -7      // ��Ƭδ����
#define MIF_RET_NOAUTH          -8      // ����δ��֤
#define MIF_RET_OVERFLOW        -9      // �ڲ����������
#define MIF_RET_AUTHERR         -10     // ��֤ʧ��
#define MIF_RET_VALUEERR        -11     // ֵ����
#define MIF_RET_NOSUPPORTTCL    -12     // ��Tag��֧��T=CLЭ��
#define MIF_RET_NOSUPPORTCMD    -13     // ������Tag��֧��

typedef struct
{
	u32 WaitTime_PollTypeA;		// TypeAѰ���ڼ䵥��ָ��ȴ�ʱ��
	u32 WaitTime_PollTypeB;		// TypeBѰ���ڼ䵥��ָ��ȴ�ʱ��
	u32 WaitTime_Mifare;			// Mifare One�������ڼ�ָ��ȴ�ʱ��
	u32 WaitTime_RATS;			// ����Pro��ָ��ȴ�ʱ��
	u32 WaitTime_CardResponse;	// T=CL�������ȴ�����Ӧ��ʱʱ��
	u32 WaitTime_CompleteCmd;	// �ȴ��ն���ɱ������ݷ��ͽ�����ʱ��
	u32 WaitTime_DelayBefore;	// ������һ��ָ��ǰ�ĵȴ�ʱ��
	u32 WaitTime_IdleRF;			// ��λ�ز��ĵȴ�ʱ��
}MIF_TIME_STRUCT;
*/

void EppMifOpen(void);        // ����Ƶ��ģ��
void EppMifClose(void);   // �ر���Ƶ��ģ��
void EppMifSelectType(u32 Type);
void EppMifResetRF(u32 dur_time);

int EppMifReqA(u8 *ATQA);
int EppMifWupA(u8 *pucATQA);
int EppMifAnticollion(u8 ucSEL, u8 *pucUID);
int EppMifSelect(u8 ucSEL, const u8 *pucUID, u8 *pucSAK);
int EppMifHaltA(void);
int EppMifRATS(u32 *puiATSLen, u8 *pucATS);
int EppMifPPPS(u8 BRit, u8 BRti);

int EppMifAuthByKey(u32 BlockNo, u32 KeyType, const u8 *Key, const u8 *UID);
int EppMifAuthByE2(u32 BlockNo, u32 KeyType, const u8 *UID);
int EppMifReadBlock(u32 BlockNo, u8 *BlockData);
int EppMifWriteBlock(u32 BlockNo, const u8 *BlockData);
int EppMifSetValue(u32 BlockNo, u32 Value);
int EppMifIncrement(u32 BlockNo, u32 Value);
int EppMifDecrement(u32 BlockNo, u32 Value);
int EppMifRestore(u32 BlockNo);
int EppMifTransfer(u32 BlockNo);
int EppMifWriteKey(u32 KeyType, u32 Sector, const u8 *Key);
int EppMifWriteE2(u32 StartAddr, u32 Len, const u8 *Data);
int EppMifReadE2(u32 StartAddr, u32 Len, u8 *Data);

int EppMifReadULData(u32 PageAddr, u8 *Data);
int EppMifWriteULData(u32 PageAddr, const u8 *Data);
int EppMifWriteULData16(u32 PageAddr, const u8 *Data);

int EppMifReqB(u32 AFI, u32 SlotNum, u32 *LenOfATQB, u8 *ATQB);
int EppMifWupB(u32 AFI, u32 SlotNum, u32 *LenOfATQB, u8 *ATQB);
int EppMifSlotMarker(u32 Slot, u32 *LenOfATQB, u8 *ATQB);
int EppMifAttriB(const u8 *UID, u8 BRit, u8 BRti);
int EppMifHaltB(const u8 *UID);

int EppMifProcess(u32 uiSendLen, const u8 *pucSendData, u32 *puiReclen, u8 *pucRecData, u32 TimeOut);
int EppMifExchangeAPDU(u32 SendLen, const u8 *SendData, u32 *RecvLen, u8 *RecvData);
int EppMifFeliCaExchange(u32 SendLen, const u8 *pucSendData, u32 *pulRecvLen, u8 *pRecvData, u32 TimeOutValue);

int EppInListPassiveTarget(u8 Mode, int InitLen, const u8 *InitInfo, int *CardInfoLen, u8 *CardInfo);
int EppInATR(u32 *RespLen, u8 *ATRInfo);
int EppInPSL(u8 BRit, u8 BRti);
int EppInDataExchange(int SendLen, const u8 *SendData, u32 *RecvLen, u8 *RecvData);
void EppInRelease(void);

void EppMifLEDControl(u8 LEDNo, u8 Mode);
void EppMifSetTimeOut(const MIF_TIME_STRUCT *ptTimeOutStruct);
void EppMifGetTimeOut(MIF_TIME_STRUCT *ptTimeOutStruct);
int EppMifCheckOnlyOneCard(u8 Mode);





#ifdef __cplusplus
}
#endif /* __cplusplus */









#endif













