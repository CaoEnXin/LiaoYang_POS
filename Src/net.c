/* 
 * net.c
 *
 * source code file for net function
 *
 * Copyright (C) 2008 Lihua <lihua_338@163.com> 
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 *
 */

#include "net.h"
#include "posapi.h"

/* Create a new socket of type TYPE in domain DOMAIN, using
   protocol PROTOCOL.  If PROTOCOL is zero, one is chosen automatically.
   Returns a file descriptor for the new socket, or -1 for errors.  */
int Socket(int domain, int type, int protocol)
{
	int retval;
	
	retval =  NetSocket(domain, type, protocol);

	return retval;
}

/* Open a connection on socket FD to peer at ADDR (which LEN bytes long).
   For connectionless socket types, just set the default address to send to
   and the only address from which to accept transmissions.
   Return 0 on success, -1 for errors.
 
   This function is a cancellation point and therefore not marked with
   THROW.  */
int Connect(int sock, __CONST_SOCKADDR_ARG addr, socklen_t len)
{
	int retval;
	
	retval = NetConnect(sock, addr, len);

	return retval;
}

/* Send N bytes of BUF to socket FD.  Returns the number sent or -1.
 
   This function is a cancellation point and therefore not marked with
   THROW.  */
ssize_t Send(int sock, const void *buf, size_t n, int flags)
{
	int retval;
	
	retval = NetSend(sock, buf, n, flags);
	
	return retval;
}
 
/* Read N bytes into BUF from socket FD.
   Returns the number read or -1 for errors.
 
   This function is a cancellation point and therefore not marked with
   THROW.  */
ssize_t Recv(int sock, void *buf, size_t n, int flags)
{
	int retval;
	
	retval = NetRecv(sock, buf, n, flags);

	return retval;
}

/* Put the current value for socket FD's option OPTNAME at protocol level LEVEL
   into OPTVAL (which is *OPTLEN bytes long), and set *OPTLEN to the value's
   actual length.  Returns 0 on success, -1 for errors.  */
int Getsockopt(int sock, int level, int optname, 
					void *__restrict optval, socklen_t *__restrict optlen)
{
	int retval;
	
	retval = NetGetsockopt(sock, level, optname, optval, optlen);

	return retval;
}
 
/* Set socket FD's option OPTNAME at protocol level LEVEL
   to *OPTVAL (which is OPTLEN bytes long).
   Returns 0 on success, -1 for errors.  */
int Setsockopt(int sock, int level, int optname, const void *optval, socklen_t optlen)
{
	int retval;
	
	retval = NetSetsockopt(sock, level, optname, optval, optlen);
	
	return retval;
}

/* Shut down all or part of the connection open on socket FD.
   HOW determines what to shut down:
     SHUT_RD   = No more receptions;
     SHUT_WR   = No more transmissions;
     SHUT_RDWR = No more receptions or transmissions.
   Returns 0 on success, -1 for errors.  */
int Shutdown(int sock)
{
	int retval;

	retval = NetClose(sock);
	
	return retval;
}


