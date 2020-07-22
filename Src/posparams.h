
#ifndef NEWPOS_PARAMS_H
#define NEWPOS_PARAMS_H


#define  STR_PARAMS(A)  (uint8_t*)A,	sizeof(A)
#define  CHAR_PARAMS(A) (uint8_t*)&A,	sizeof(char)
#define  INT_PARAMS(A)	(uint8_t*)&A,	sizeof(int)
#define  LONG_PARAMS(A)	(uint8_t*)&A,	sizeof(int)

#define STRING_ITEM	0x00	//保证最后一位为0x00;
#define ASSCII_ITEM 0x01	
#define HEX_ITEM	0x02	
#define BIN_ITEM	0x03


typedef struct
{
	uint8_t name[40];
	void *ini_ptr;
	uint8_t len;
	uint8_t type;
}PARAMS;

extern int GetParams(char *filename, PARAMS *myParams);

int  GetParamfileName(void);

#endif
