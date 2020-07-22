#ifndef NEW8110_ICC_H
#define NEW8110_ICC_H

#include "posapi.h"

int New8110_icc_Init(void);
int New8110_icc_powerup(int * plen, uint8_t * atr);
int New8110_icc_poweroff(void);
int New8110_icc_exchange_apdu(int fd, uint32_t sendlen, const void *senddata, uint32_t *recvlen, void *recvdata);


int contactness_open(void);
int contactness_close(int icc_fd);

int contactness_poll(int icc_fd);
int contactness_poweredon(int icc_fd);
int contactness_poweroff(int fd);
int contactness_exchangeapdu(int icc_fd, uint32_t sendlen, const void *senddata, uint32_t *recvlen, void *recvdata);

#endif
