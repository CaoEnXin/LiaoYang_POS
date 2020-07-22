#ifndef _LIBARABIC_H_
#define _LIBARABIC_H_


int arSetLcdFont(const char *pFontName, const char *pFontCharacter, int Style, int Size, int FontPriority);
int arSetPrnFont(const char *pFontName, const char *pFontCharacter, int Style, int Size, int FontPriority);
int arSetIndianNum(int mode);
int arPrnPrintf(int mode, char *fmt, ...);
int arLcdPrintf(int lineno, int mode, char *fmt, ...);



#endif
