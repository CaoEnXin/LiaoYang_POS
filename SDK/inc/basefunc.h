



#ifndef __BASE_FUNC_H__
#define __BASE_FUNC_H__

#ifndef OPENSSL_SYS_UNIX

#include <time.h>
#include <stdio.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SOCKET_FD_BEGIN			1000
#define FD2SOCKET(fd)			(fd - SOCKET_FD_BEGIN)
#define SOCKET2FD(socket)		(socket + SOCKET_FD_BEGIN)


#undef  assert
#define assert(expr)	do {} while (0)

#undef  abort
#define abort()			do {Printf("abort() in file '%s', line %d\n", __FUNCTION__, __LINE__);} while(0);  while(1)

#undef  time
#define time(t)									SYS_TIME(t)
extern time_t SYS_TIME(time_t *t);

#undef  mktime
#define mktime(tm)								SYS_MKTIME(tm)
extern time_t SYS_MKTIME(struct tm *tm);

#undef  gmtime
#define gmtime(timer)							SYS_GMTIME(timer)
extern struct tm *SYS_GMTIME(const time_t *timer);

#undef  localtime
#define localtime(timer)						SYS_LOCALTIME(timer)
extern struct tm *SYS_LOCALTIME(const time_t *timer);

#undef  getenv
#define getenv(name)							SYS_GETENV(name)
char *SYS_GETENV(const char *name);

/******************************************************************************/
#undef  printf
#define printf(format,arg...)					Printf(format, ##arg)
extern 	int Printf (const char *fmt, ...);

#undef  fprintf
#define fprintf(stream, format, arg...)			SYS_FPRINTF(stream, format, ##arg)
extern int SYS_FPRINTF(FILE * __restrict stream, const char * __restrict format, ...);

#undef  fputs
#define fputs									SYS_FPUTS
extern int SYS_FPUTS(const char * __restrict s, register FILE * __restrict stream);

#undef  perror
#define perror(s)								SYS_PERROR(s)
extern void SYS_PERROR(register const char *s);

#undef  strerror
#define strerror(no)								SYS_STRERROR(no)
char *SYS_STRERROR(int no);

int console_init(void);

/******************************************************************************/
#undef  open
#define open(file, flag, arg...)				SYS_OPEN(file, flag, ##arg)
int SYS_OPEN(const char *file, int oflag, ...);

#undef  close
#define close(fd)								SYS_CLOSE(fd)
int SYS_CLOSE(int fd);

#undef  read
#define read(fd, buf, count)					SYS_READ(fd, buf, count)
ssize_t SYS_READ(int fd, void *buf, size_t count);

#undef  write
#define write(fd, buf, count)					SYS_WRITE(fd, buf, count)
ssize_t SYS_WRITE(int fd, const void *buf, size_t count);

#undef  lseek
#define lseek(fd, offset, whence)				SYS_LSEEK(fd, offset, whence)
int SYS_LSEEK(int fd, long offset, int whence);

#undef  ioctl
#define ioctl(fd, request, arg...)				SYS_IOCTL(fd, request, ##arg)
int SYS_IOCTL (int fd, unsigned long int request, ...);

/******************************************************************************/
#undef  fopen
#define fopen(filename, mode)					SYS_FOPEN(filename, mode)
FILE *SYS_FOPEN(const char * __restrict filename, const char * __restrict mode);

#undef  fwrite
#define fwrite(ptr, size, nmemb, stream)		SYS_FWRITE(ptr, size, nmemb, stream)
size_t SYS_FWRITE(const void * __restrict ptr, size_t size,
						 size_t nmemb, register FILE * __restrict stream);

#undef  fread
#define fread(ptr, size, nmemb, stream)			SYS_FREAD(ptr, size, nmemb, stream)
size_t SYS_FREAD(void * __restrict ptr, size_t size, size_t nmemb,
						FILE * __restrict stream);

#undef  ftell
#define ftell(stream)							SYS_FTELL(stream)
long int SYS_FTELL(register FILE *stream);

#undef  feof
#define feof(stream)							SYS_FEOF(stream)
int SYS_FEOF(register FILE *stream);

#undef  ferror
#define ferror(stream)							SYS_FERROR(stream)
int SYS_FERROR(register FILE *stream);

#undef  fileno
#define fileno(stream)							SYS_FILENO(stream)
int SYS_FILENO(register FILE *stream);

#undef  fflush
#define fflush(stream)							SYS_FFLUSH(stream)
int SYS_FFLUSH(register FILE *stream);

#undef  fseek
#define fseek(stream, offset, whence) 			SYS_FSEEK(stream, offset, whence)
int SYS_FSEEK(register FILE *stream, long int offset, int whence);

#undef  fgets
#define fgets(s, n, stream)						SYS_FGETS(s, n, stream)
char *SYS_FGETS(char *__restrict s, int n, register FILE * __restrict stream);

#undef  fclose
#define fclose(stream)							SYS_FCLOSE(stream)
int SYS_FCLOSE(register FILE *stream);

#undef  malloc
#define malloc									_malloc
void *_malloc(unsigned int);

#undef  free
#define free									_free
void _free(void *);

#undef calloc
#define calloc									_calloc
void *_calloc(unsigned int , unsigned int );

#undef  realloc
#define realloc									_realloc
void *_realloc(void *, unsigned int );

void _HeapInit(unsigned long base, unsigned long limit);

/******************************************************************************/
#undef  socket
#define socket(domain, type, protocol)			SYS_SOCKET(domain, type, protocol)
int SYS_SOCKET(int domain, int type, int protocol);

#undef  shutdown
#define shutdown(fd, how)						SYS_SHUTDOWN(fd, how)
int SYS_SHUTDOWN(int fd, int how);

#undef  socketpair
#define socketpair(domain, type, protocol, fds)	SYS_SOCKETPAIR(domain, type, protocol, fds)
int SYS_SOCKETPAIR(int domain, int type, int protocol, int fds[2]);

#undef  bind
#define bind(fd, addr, len)						SYS_BIND(fd, addr, len)
int SYS_BIND(int fd, __CONST_SOCKADDR_ARG addr, socklen_t len);

#undef  getsockname
#define getsockname(fd, addr, len)				SYS_GETSOCKNAME(fd, addr, len)
int SYS_GETSOCKNAME(int fd, __SOCKADDR_ARG addr, socklen_t *__restrict len);

#undef  connect
#define connect(fd, addr, len)					SYS_CONNECT(fd, addr, len)
int SYS_CONNECT(int fd, __CONST_SOCKADDR_ARG addr, socklen_t len);

#undef  getpeername
#define getpeername(fd, addr, len)				SYS_GETPEERNAME(fd, addr, len)
int SYS_GETPEERNAME(int fd, __SOCKADDR_ARG addr,  socklen_t *__restrict len);

#undef  send
#define send(fd, buf, n, flags)					SYS_SEND(fd, buf, n, flags)
ssize_t SYS_SEND(int fd, const void *buf, size_t n, int flags);

#undef  recv
#define recv(fd, buf, n, flags)					SYS_RECV(fd, buf, n, flags)
ssize_t SYS_RECV(int fd, void *buf, size_t n, int flags);

#undef  sendto
#define sendto(fd, buf, n, flags, addr, addr_len)	SYS_SENDTO(fd, buf, n, flags, addr, addr_len)
ssize_t SYS_SENDTO(int fd, const void *buf, size_t n,
                 int flags, __CONST_SOCKADDR_ARG addr, socklen_t addr_len);

#undef  recvfrom
#define recvfrom(fd, buf, n, flags, addr, addr_len)	SYS_RECVFROM(fd, buf, n, flags, addr, addr_len)
ssize_t SYS_RECVFROM(int fd, void *__restrict buf, size_t n,
                         int flags, __SOCKADDR_ARG addr, socklen_t *__restrict addr_len);

#undef  sendmsg
#define sendmsg(fd, message, flags)				SYS_SENDMSG(fd, message, flags)
ssize_t SYS_SENDMSG(int fd, const struct msghdr *message, int flags);

#undef  recvmsg
#define recvmsg(fd, message, flags)				SYS_RECVMSG(fd, message, flags)
ssize_t SYS_RECVMSG(int fd, struct msghdr *message, int flags);

#undef  getsockopt
#define getsockopt(fd, level, optname, optval, optlen)	SYS_GETSOCKOPT(fd, level, optname, optval, optlen)
int SYS_GETSOCKOPT(int fd, int level, int optname, 
					void *__restrict optval, socklen_t *__restrict optlen);

#undef  setsockopt
#define setsockopt(fd, level, optname, optval, optlen)	SYS_SETSOCKOPT(fd, level, optname, optval, optlen)
int SYS_SETSOCKOPT(int fd, int level, int optname, const void *optval, socklen_t optlen);

#undef  listen
#define listen(fd, n)							SYS_LISTEN(fd, n)
int SYS_LISTEN(int fd, int n);

#undef  accept
#define accept(fd, addr, addr_len)				SYS_ACCEPT(fd, addr, addr_len)
int SYS_ACCEPT(int fd, __SOCKADDR_ARG addr, socklen_t *__restrict addr_len);

#undef  getservbyname
#define getservbyname(servname, protoname)		SYS_GETSERVBYNAME(servname, protoname)
struct servent *SYS_GETSERVBYNAME(const char *servname, const char *protoname);

#undef  gethostbyname
#define gethostbyname(hostname)					SYS_GETHOSTBYNAME(hostname)
struct hostent *SYS_GETHOSTBYNAME(const char *hostname);

#undef  gethostbyaddr
#define gethostbyaddr(addr, len, family)		SYS_GETHOSTBYADDR(addr, len, family)
struct hostent *SYS_GETHOSTBYADDR(const char *addr, socklen_t len, int family);

#ifdef  __cplusplus
}
#endif

#endif /* OPENSSL_SYS_UNIX */

#if defined(SSL_DEBUG)
	#define ssl_debug(format, arg...)	printf("\tSSL debug: %s[%d]" format, __FUNCTION__, __LINE__, ##arg)
#else
	#define ssl_debug(format, arg...)
#endif /* SSL_DEBUG */

#endif /* __BASE_FUNC_H__ */


