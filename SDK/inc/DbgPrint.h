#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H

#if defined(_DEBUG) 
void dbg_printf_line(const char * pszFmt, ...);
void dbg_print_tag(const void * pvTag, unsigned long uiTag);
void dbg_print_memory(const char * pszTitle, const void * pvData, unsigned long lSize);
#else
#define dbg_printf_line(pszFmt,...)
#define dbg_print_tag(ptag, uitag)
#define dbg_print_memory(title, pdata, size)
#endif
#endif
