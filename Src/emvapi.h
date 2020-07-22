/*
 * emvapi.h
 *
 *  Created on: Oct 20, 2010
 *      Author: neal
 */

#ifndef EMVAPI_H_
#define EMVAPI_H_

enum emv_error_code{
	EMV_ERRNO_INVAL = 2048,
	EMV_ERRNO_NOMEM,
	EMV_ERRNO_NODATA,
	EMV_ERRNO_DATE,
	EMV_ERRNO_CHECKSUM,
	EMV_ERRNO_EXPRIED = 2048+5,
	EMV_ERRNO_TLV,
	EMV_ERRNO_UNKNOWN_TAG,
	EMV_ERRNO_EXISTED,
	EMV_ERRNO_LEN,
	EMV_ERRNO_PIN = 2048+10,
	EMV_ERRNO_KEY,
	EMV_ERRNO_SW,
	EMV_ERRNO_DATA,
	EMV_ERRNO_CARD_BLOCKED,
	EMV_ERRNO_APP_BLOCKED = 2048+15,
	EMV_ERRNO_NO_APP,
	EMV_ERRNO_CANCEL,
	EMV_ERRNO_NO_ACCEPTED,
	EMV_ERRNO_PIN_BLOCKED,
	EMV_ERRNO_BYPASS_PIN = 2048+20,
	EMV_ERRNO_DECLINE,
	EMV_ERRNO_FORCE_ACCEPTED,
	EMV_ERRNO_ONLINE_ERROR,
	EMV_ERRNO_ONLINE_TIMEOUT,
	EMV_ERRNO_SERVICE_NOT_ALLOWED,
};

#define EMV_RET_SUCCESS  0
#define ONLINE_APPROVE   0

struct terminal_aid_info
{
	uint8_t aid_len;                
	uint8_t aid[16];	         
	uint8_t support_partial_aid_select;             
	uint8_t application_priority;            
	uint32_t target_percentage;           
	uint32_t maximum_target_percentage;        
	uint32_t terminal_floor_limit;          
	uint32_t threshold_value;          
	uint8_t terminal_action_code_denial[5];        
	uint8_t terminal_action_code_online[5];        
	uint8_t terminal_action_code_default[5];       
	uint8_t acquirer_identifier[6];        
	uint8_t len_of_default_ddol;           
	uint8_t default_ddol[254];           
	uint8_t len_of_default_tdol;           
	uint8_t default_tdol[254];           
	uint8_t application_version[2];          
	uint8_t len_of_terminal_risk_management_data;      
	uint8_t terminal_risk_management_data[8]; 
	uint32_t cl_ReaderMaxTransAmount;
	uint32_t cl_Floor_Limit;
	uint32_t cl_CVM_Amount;
};
void emv_delete_all_terminal_aids(void);
int emv_add_one_aid_info(const struct terminal_aid_info *info);
int emv_get_one_aid_info(int index, struct terminal_aid_info *info);
int emv_del_one_aid_info(uint8_t aid_len, const void *aid);
int emv_get_aid_info_num(void);
int emv_check_aid_in_terminal(uint8_t aid_len, const void *aid);

struct issuer_ca_public_key
{
	uint8_t RID[5];  		       
	uint8_t Index;   		       
	uint8_t len_of_modulus;       	       
	uint8_t modulus[248];             
	uint8_t len_of_exponent;               
	uint8_t exponent[3];           	  
	uint8_t expiration_date[3];           
	uint8_t checksum[20];            
};
void emv_delete_all_ca_public_keys(void);
int emv_add_one_ca_public_key(const struct issuer_ca_public_key *key);
int emv_get_one_ca_public_key(const void *RID, uint8_t index, struct issuer_ca_public_key *key);
int emv_get_one_ca_public_key_by_no(int no, struct issuer_ca_public_key *key);
int emv_delete_one_ca_public_key(const void *RID, uint8_t index);
int emv_get_ca_public_key_num(void);

struct terminal_mck_configure
{
	uint8_t terminal_type;  
	uint8_t terminal_capabilities[3];
	uint8_t additional_terminal_capabilities[5]; 

	uint8_t support_pse_selection_method;
	uint8_t support_cardholder_confirmation;
	uint8_t support_certificate_revocation;
	uint8_t support_default_ddol;

	uint8_t support_bypass_pin_entry;
	uint8_t support_get_pin_try_counter;

	uint8_t support_floor_limit_checking;
	uint8_t support_random_transaction_selection;
	uint8_t support_velocity_checking;
	uint8_t support_transaction_log;
	uint8_t support_exception_file;

	uint8_t support_terminal_action_codes;
	uint8_t support_forced_online_capability;
	uint8_t support_advices;
	uint8_t support_issuer_initiated_voice_referrals;
	uint8_t support_batch_data_capture;
	uint8_t support_online_data_capture;
	uint8_t support_default_tdol;	

	uint8_t support_multi_language;
	uint8_t support_forced_acceptance_capability;
	uint8_t support_card_initiated_voice_referrals;
	uint8_t support_trm_based_on_aip;

	uint8_t pos_entry_mode;
	uint8_t checksum[20];
}; // 1-> Support 0 -> No Support
int emv_set_mck_configure(struct terminal_mck_configure *config);
int emv_get_mck_configure(struct terminal_mck_configure *config);

// Kernel Type
#define KERNEL_TYPE_EMV         0			
#define KERNEL_TYPE_PBOC        1			
#define KERNEL_TYPE_VISA        2			
#define KERNEL_TYPE_MASTERCARD  3			
#define KERNEL_TYPE_JCB         4			
#define KERNEL_TYPE_AMEX        5			
void emv_set_kernel_type(int mode);
int emv_get_kernel_type(void);

void emv_pboc_ec_enable(int mode);
void emv_pboc_set_ec_ttl(uint32_t value);
uint32_t emv_pboc_get_ec_ttl(void);
int emv_pboc_check_ec(void);

void emv_init_data_element(void);
int emv_get_data_element(const void *tag, uint8_t taglen, uint8_t *len, void *val);
int emv_set_data_element(const void *tag, uint8_t taglen, uint8_t len, const void *val);
int emv_del_data_element(const void *tag, uint8_t taglen);
int	emv_check_data_element(const void *tag, uint8_t taglen);

struct emv_callback_func{
	int (*emv_candidate_apps_selection)(void);   
	void(*emv_multi_language_selection)(void);   
	int (*emv_get_amount)(uint32_t *, uint32_t *);  
	int (*emv_get_pin)(void *);				
	int (*emv_return_pin_verify_result)(uint8_t);		
	int (*emv_check_online_pin)(void);
	int (*emv_check_certificate)(void); 
	int (*emv_online_transaction_process)(uint8_t *,uint8_t *, int *, uint8_t *, int *,
	       				  				uint8_t *, int *,int *);
	int (*emv_issuer_referral_process)(void);
	int (*emv_advice_process)(int);

	int (*emv_check_revocation_certificate)(uint8_t, const void *, const void *);
	int (*emv_check_exception_file)(uint8_t, const void *, uint8_t);
	int (*emv_get_transaction_log_amount)(uint8_t, const void *, uint8_t);
};
int emv_init_callback_func(const struct emv_callback_func *ptFunc);
void emv_get_callback_func(struct emv_callback_func *ptFunc);


#define EMV_CASH          0x80       
#define EMV_GOODS	      0x40       
#define EMV_SERVICE       0x20       
#define EMV_CASHBACK      0x10      
#define EMV_INQUIRY       0x08       
#define EMV_TRANSFER      0x04      
#define EMV_PAYMENT       0x02       
#define EMV_ADMIN         0x01  


struct emv_core_init{
    uint8_t terminal_id[8];
    uint8_t merchant_id[15];
    uint8_t merchant_cate_code[2];
    uint8_t merchant_name_loc_len;
    uint8_t merchant_name_loc[256];
    uint8_t transaction_type;
    uint8_t terminal_country_code[2];
    uint8_t transacion_currency_code[2];
    uint8_t refer_currency_code[2];
    uint8_t transacion_currency_exponent;
    uint8_t refer_currency_exponent;
    uint32_t refer_currency_coefficient;
};

int emv_set_core_init_parameter(const struct emv_core_init *ptInit);
int emv_get_core_init_parameter(struct emv_core_init *ptInit);

struct candapp{
    uint8_t tCandAppName[33];
    uint8_t cFlgAPID;
    uint8_t cAPID;
};
int emv_get_all_candapps(uint8_t *num, struct candapp *applist);

int emv_application_select(int fd, uint32_t transno);

int emv_read_app_data(int fd);
int emv_offline_data_authentication(int fd);
void emv_process_restriction(void);
int emv_cardholder_verify(int fd);
int emv_terminal_risk_manage(int fd);
int emv_terminal_action_analysis(int fd,int *need_online);
int emv_online_transaction(int fd);

int emv_get_script_result(void *result, int *len);
int emv_check_signature(void);
int emv_check_force_accept(void);
int emv_check_online_flag(void);
uint16_t emv_get_last_iccard_sw(void);

int emv_application_select_no_gpo(int fd);
int emv_read_log_record(int fd,uint8_t record_no);
int emv_get_log_item(const void *tag, uint8_t taglen, void *value, uint8_t *valuelen);



#endif /* EMVAPI_H_ */

