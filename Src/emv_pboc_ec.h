#ifndef EMV_PBOC_EC_H_
#define EMV_PBOC_EC_H_

void emv_pboc_ec_set(int cSupEC, int ECTTL);
uint32_t emv_pboc_ec_get_ttl(void);
int emv_pboc_check_ec(void);


#endif


