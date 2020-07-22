//****************************************************************************
//
// MCB.H - Memory Control Block data structures and declarations.
//
// Copyright (c) 2000 Start Computer Group.
//
//****************************************************************************

#define FIRST_FIT       0
#define BEST_FIT        1
#define LAST_FIT        2
#define WORST_FIT       3

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern void * _malloc(unsigned int size);
extern void * _mallocalign(unsigned int, unsigned char align);
extern void * _realloc(void * para, unsigned int size);
extern void *_calloc(unsigned int nmemb, unsigned int size);
extern void _free(void * para);
extern void _HeapInit(unsigned long base, unsigned long size);
extern unsigned long _MemLargest(void);
extern unsigned long _MemCheck(void);
extern unsigned long _FreeProcessMem(char owner);

#undef  malloc
#define malloc		_malloc

#undef  realloc
#define realloc		_realloc

#undef  calloc
#define calloc		_calloc

#undef  free
#define free		_free

#undef  heap_init
#define heap_init	_HeapInit

#ifdef __cplusplus
}
#endif /* __cplusplus */
