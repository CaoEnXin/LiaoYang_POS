#ifndef EMV_PBOC_EC_H_
#define EMV_PBOC_EC_H_

void emv_pboc_ec_set(INT32 cSupEC, INT32 ECTTL);
UINT32 emv_pboc_ec_get_ttl(void);
INT32 emv_pboc_check_ec(void);

#endif

