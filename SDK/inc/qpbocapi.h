#ifndef qpboc_h_
#define qpboc_h_
#include "posapi.h"

#include "emvapi.h"

#define EMV_QPBOC_OK						    0
#define EMV_QPBOC_TRANSACTION_DECLIEN           1
#define EMV_QPBOC_TRANSACTION_TERMINATED        2
#define EMV_QPBOC_APP_BLOCKED                   3
#define EMV_QPBOC_CANCELED                 	    4
#define EMV_QPBOC_CONNECT_TRANSACTION_NEEDED 	5
#define EMV_QPBOC_NEED_PBOC_TRANSACTION         6
#define EMV_QPBOC_NEED_RETRY                    7

#define EMV_QPBOC_ICC_ERROR			            8
#define EMV_QPBOC_INVALID_DATA                  9
#define EMV_QPBOC_DATA_MISSING                  10
#define EMV_QPBOC_DATA_DUPLICATE                11
#define EMV_QPBOC_MEMORY_OVERFLOW			    12
#define EMV_QPBOC_MEMORY_NO_ENOUGH			    13
#define EMV_QPBOC_PROGRAMING_ERROR              14
#define EMV_QPBOC_COMMUNICATION_ERROR           15 
#define EMV_QPBOC_CONDITIONS_NOT_SATISFIED      16
#define EMV_QPBOC_CARD_EXPIRED                  17
#define EMV_QPBOC_CARD_IN_BLACK_LIST            18



struct qpboc_parameters
{
	uint8_t 	m_TransactionProperty[4];	
	uint8_t     m_StatusCheckSupported;
};

typedef enum
{
	EMV_MSD,
		EMV_QPBOC,
		EMV_PBOC
}EMV_APP_TYPE;

typedef EMV_APP_TYPE * LP_EMV_APP_TYPE;

int emv_qpboc_set_parameter(const struct qpboc_parameters * parameters);
int emv_qpboc_get_parameter(struct qpboc_parameters * parameters);
int emv_qpboc_pre_transaction(uint32_t uiTarnsCounter, uint8_t TransType, uint32_t Amount/*, uint32_t AmountOther*/);
int emv_qpboc_read_data(int fd);
int emv_qpboc_complete(void);
int emv_qpboc_signature_needed(int * pSignature);
int emv_qpboc_get_app_type(LP_EMV_APP_TYPE pAppType);
int emv_qpboc_select_no_gpo(int fd);
int emv_qpboc_need_save_failed_log(void);

#endif
