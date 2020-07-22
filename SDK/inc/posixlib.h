// BASE API
#ifndef POSIXLIB_H_
#define POSIXLIB_H_

#include <sys/stat.h>
#include <linux/utime.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <termios.h>
#include <sys/poll.h>
#include <sys/select.h>


#ifdef __cplusplus
extern "C"{
#endif

struct hostinfo {
	char	h_name[256];
	int		h_addrtype;
	int 	h_ipnum;
	int		h_length;
	char	h_addr_list[0];
};

#define BC_PARMS_NULL			0   /* The parameter type is pointer, pass a NULL pointer */
#define BC_PARMS_POINTER		1   /* The parameter type is pointer, pass a buffer */
#define BC_PARMS_VALUE			2   /* The parameter type is value */



int PosixOpen(const char *pathname, int oflag, .../*mode_t mode*/);      //  �򿪻򴴽�һ���ļ�
int PosixClose(int filedes);     //  �ر�
ssize_t PosixRead(int filedes, void *buf, size_t nbytes);    //  �Ӵ򿪵��ļ���ȡ����
ssize_t PosixWrite(int filedes, const void *buf, size_t nbytes);     //  ��򿪵��ļ�д������
off_t PosixLseek(int filedes, off_t offset, int whence);     //  ��ʾ��Ϊһ���򿪵��ļ�������ƫ����
//int PosixIoctl(int filedes, int request, ...);
int PosixIoctlA(int filedes, int request);
int PosixIoctlB(int filedes, int request, int arg, int argtype, int arglen);
int PosixTruncate(const char *pathname, off_t length);       //  �ض��ļ�
int PosixFtruncate(int filedes, off_t length);      //  �ض��ļ�
int PosixFsync(int filedes);     //  �ȴ�ϵͳ���ļ�����д�����
int PosixDup(int filedes);       //  ����һ���ִ���ļ�������
int PosixDup2(int filedes, int filedes2);    //  ����һ���ִ���ļ�������
int PosixPipe(int filedes[2]);       //  �����ܵ�
/* �ļ���Ŀ¼ */
int PosixStat(const char *pathname, struct stat *buf);       //  ������������ļ��йص���Ϣ�ṹ
int PosixLstat(const char *pathname, struct stat *buf);      //  ��stat���ƣ����ǵ��������ļ���һ����������ʱ�����ظ÷������ӵ��й���Ϣ(���ڽ������Ŀ¼��νṹ)
int PosixFstat(int filedes, struct stat *buf);               //  ��ȡ����������filedes�ϴ��ļ����й���Ϣ
int PosixAccess(const char *pathname, int mode);             //  ����Ȩ�޲���
int PosixChmod(const char *pathname, mode_t mode);       //  �ı������ļ��ķ���Ȩ��
int PosixFchmod(int filedes, mode_t mode);               //  �ı������ļ��ķ���Ȩ��
int PosixChown(const char *pathname, uid_t owner, gid_t group);  //  �����ļ����û�ID����ID
int PosixFchown(int filedes, uid_t owner, gid_t group);      //  �����ļ����û�ID����ID
int PosixLchown(const char *pathname, uid_t owner, gid_t group);     //  �����ļ����û�ID����ID
int PosixFcntl(int filedes, int cmd, int arg);      //  �ı��Ѵ��ļ�������
int PosixUtime(const char *pathname, const struct utimbuf *times);   //  ����һ���ļ��ķ��ʺ��޸�ʱ��
int PosixMknod(const char *pathname, mode_t mode, dev_t dev);    //  �����豸�ļ��Լ������ܵ���Ŀ¼��
int PosixLink(const char *existingpath, const char *newpath);    //  ����һ��ָ�������ļ�������
int PosixSymlink(const char *actualpath, const char *sympath);   //  ����һ����������
int PosixUnlink(const char *pathname);   //  ɾ��һ�����е�Ŀ¼��
int PosixRename(const char *oldpath, const char *newpath);       //  �������ļ�
int PosixMkdir(const char *pathname, mode_t mode);       //  ����Ŀ¼
int PosixRmdir(const char *pathname);    // ɾ��һ����Ŀ¼
DIR *PosixOpendir(const char *pathname); //  ��Ŀ¼����
struct dirent *PosixReaddir(DIR *dir);   //  ���ζ�ȡĿ¼�µ�������Ŀ(���ص��Ǹ�Ŀ¼�µ���һ���ļ����ļ���)
int PosixRewinddir(DIR *dir);    //  ��������DIR�ṹ��������һ�ε���readdir()��������Ȼ����Ŀ¼�еĵ�һ���ļ�
int PosixClosedir(DIR *dir);     //�ر�Ŀ¼����
int PosixChdir(const char *pathname);    //  �ı�һ�����̵ĵ�ǰĿ¼
int PosixFchdir(int fd);     //  �ı�һ�����̵ĵ�ǰĿ¼
//char *PosixGetcwd(char *buf, size_t size);   //  ��ȡ��һ����Ŀ¼(..����)

int PosixSocket(int family, int type, int protocol);
int PosixConnect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);
int PosixBind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen);
int PosixListen(int sockfd, int backlog);
int PosixAccept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);
//int PosixGetsockname(int sockfd, struct sockaddr *addr, socklen_t *alenp);  //  ʹ��������������ְ󶨵�һ���׽��ֵĵ�ַ
//int PosixGetpeername(int sockfd, struct sockaddr *addr, socklen_t *alenp);   // ����׽����Ѿ��ͶԷ����ӣ����ô˺������ҵ��Է��ĵ�ַ
int PosixSetsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
int PosixGetsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
int PosixRecvfrom(int sockfd, void *buff, int nbytes, int flags, struct sockaddr *from, socklen_t *addrlen);
int PosixSendto(int sockfd, const void *buff, int nbytes, int flags, const struct sockaddr *to, socklen_t addrlen);
//struct hostent *PosixGethostbyname (const char *name);
//struct servent *PosixGetservbyname (const char *name, const char *proto);
//struct servent *PosixGetservbyport (int port, const char *proto);
int PosixRecv(int sockfd, void *buff, int nbytes, int flags);
int PosixSend(int sockfd, const void *buff, int nbytes, int flags);
//int PosixClose(int sockfd);
struct hostinfo *PosixGethostbyname2(const char *hostname, int maxipnum);

int SockAddrset(struct sockaddr *addr, const uint8_t *ipstr, const in_port_t port);
int SockAddrget(const struct sockaddr *addr, uint8_t *ipstr, in_port_t *port);
int PosixSystem(const char *cmdstring);

// Added at 2009:02:18 14:22:00
/* Return the output baud rate stored in *TERMIOS_P.  */
speed_t PosixCfgetospeed(const struct termios *termios_p);

/* Return the input baud rate stored in *TERMIOS_P.  */
speed_t PosixCfgetispeed(const struct termios *termios_p);

/* Set the output baud rate stored in *TERMIOS_P to SPEED.  */
int PosixCfsetospeed(struct termios *termios_p, speed_t speed);

/* Set the input baud rate stored in *TERMIOS_P to SPEED.  */
int PosixCfsetispeed(struct termios *termios_p, speed_t speed);

int PosixCfsetspeed(struct termios *termios_p, speed_t speed);

/* Put the state of FD into *TERMIOS_P.  */
int PosixTcgetattr (int fd, struct termios *termios_p);

/* Set the state of FD to *TERMIOS_P.
   Values for OPTIONAL_ACTIONS (TCSA*) are in <bits/termios.h>.  */
int PosixTcsetattr (int fd, int optional_actions,
                      const struct termios *termios_p);

/* Wait for pending output to be written on FD.
   This function is a cancellation point and therefore not marked with
   __THROW.  */
int PosixTcdrain (int fd);
/* Flush pending data on FD.
   Values for QUEUE_SELECTOR (TC{I,O,IO}FLUSH) are in <bits/termios.h>.  */
int PosixTcflush (int fd, int queue_selector);

/* Suspend or restart transmission on FD.
   Values for ACTION (TC[IO]{OFF,ON}) are in <bits/termios.h>.  */
int PosixTcflow (int fd, int action);

void PosixExit(int exitCode);
int PosixGeterrno(void);
int PosixShutdown(int sockfd, int howto);
int PosixPoll(struct pollfd *fds, int numfds, int timeout);
int PosixSelect(int numfds, fd_set *readfds, fd_set *writefds, 
	              fd_set *exceptfds, struct timeval *timeout);



#ifdef __cplusplus
}
#endif


#endif

