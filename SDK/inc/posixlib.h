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



int PosixOpen(const char *pathname, int oflag, .../*mode_t mode*/);      //  打开或创建一个文件
int PosixClose(int filedes);     //  关闭
ssize_t PosixRead(int filedes, void *buf, size_t nbytes);    //  从打开的文件读取数据
ssize_t PosixWrite(int filedes, const void *buf, size_t nbytes);     //  向打开的文件写入数据
off_t PosixLseek(int filedes, off_t offset, int whence);     //  显示地为一个打开的文件设置其偏移量
//int PosixIoctl(int filedes, int request, ...);
int PosixIoctlA(int filedes, int request);
int PosixIoctlB(int filedes, int request, int arg, int argtype, int arglen);
int PosixTruncate(const char *pathname, off_t length);       //  截短文件
int PosixFtruncate(int filedes, off_t length);      //  截短文件
int PosixFsync(int filedes);     //  等待系统把文件缓存写入磁盘
int PosixDup(int filedes);       //  复制一个现存的文件描述符
int PosixDup2(int filedes, int filedes2);    //  复制一个现存的文件描述符
int PosixPipe(int filedes[2]);       //  创建管道
/* 文件和目录 */
int PosixStat(const char *pathname, struct stat *buf);       //  返回与此命名文件有关的信息结构
int PosixLstat(const char *pathname, struct stat *buf);      //  与stat类似，但是当命名的文件是一个符号链接时，返回该符号链接的有关信息(用于降序遍历目录层次结构)
int PosixFstat(int filedes, struct stat *buf);               //  获取已在描述符filedes上打开文件的有关信息
int PosixAccess(const char *pathname, int mode);             //  访问权限测试
int PosixChmod(const char *pathname, mode_t mode);       //  改变现有文件的访问权限
int PosixFchmod(int filedes, mode_t mode);               //  改变现有文件的访问权限
int PosixChown(const char *pathname, uid_t owner, gid_t group);  //  更改文件的用户ID和组ID
int PosixFchown(int filedes, uid_t owner, gid_t group);      //  更改文件的用户ID和组ID
int PosixLchown(const char *pathname, uid_t owner, gid_t group);     //  更改文件的用户ID和组ID
int PosixFcntl(int filedes, int cmd, int arg);      //  改变已打开文件的性质
int PosixUtime(const char *pathname, const struct utimbuf *times);   //  更改一个文件的访问和修改时间
int PosixMknod(const char *pathname, mode_t mode, dev_t dev);    //  创建设备文件以及命名管道的目录项
int PosixLink(const char *existingpath, const char *newpath);    //  创建一个指向现有文件的链接
int PosixSymlink(const char *actualpath, const char *sympath);   //  创建一个符号链接
int PosixUnlink(const char *pathname);   //  删除一个现有的目录项
int PosixRename(const char *oldpath, const char *newpath);       //  重命名文件
int PosixMkdir(const char *pathname, mode_t mode);       //  创建目录
int PosixRmdir(const char *pathname);    // 删除一个空目录
DIR *PosixOpendir(const char *pathname); //  打开目录操作
struct dirent *PosixReaddir(DIR *dir);   //  依次读取目录下的所有条目(返回的是该目录下的下一个文件的文件名)
int PosixRewinddir(DIR *dir);    //  重新设置DIR结构，这样下一次调用readdir()函数将仍然返回目录中的第一个文件
int PosixClosedir(DIR *dir);     //关闭目录操作
int PosixChdir(const char *pathname);    //  改变一个进程的当前目录
int PosixFchdir(int fd);     //  改变一个进程的当前目录
//char *PosixGetcwd(char *buf, size_t size);   //  读取上一级的目录(..操作)

int PosixSocket(int family, int type, int protocol);
int PosixConnect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);
int PosixBind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen);
int PosixListen(int sockfd, int backlog);
int PosixAccept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);
//int PosixGetsockname(int sockfd, struct sockaddr *addr, socklen_t *alenp);  //  使用这个函数来发现绑定到一个套接字的地址
//int PosixGetpeername(int sockfd, struct sockaddr *addr, socklen_t *alenp);   // 如果套接字已经和对方连接，调用此函数来找到对方的地址
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

