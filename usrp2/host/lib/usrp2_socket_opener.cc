/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*!
 * setuid root program that opens a socket using (PF_PACKET, SOCK_RAW,
 * htons(0xBEEF)), and sends the resulting file descriptor by way of
 * of the file descriptor specified as the first command line argument.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#elif defined(HAVE_NETINET_IN_H)
#include <netinet/in.h>
#endif


ssize_t
write_fd(int fd, const void *ptr, size_t nbytes, int sendfd)
{
  struct msghdr msg;
  struct iovec iov[1];

#ifdef  HAVE_STRUCT_MSGHDR_MSG_CONTROL
  union {
    struct cmsghdr cm;
    char    control[CMSG_SPACE(sizeof(int))];
  } control_un;
  struct cmsghdr *cmptr;

  msg.msg_control = control_un.control;
  msg.msg_controllen = sizeof(control_un.control);

  cmptr = CMSG_FIRSTHDR(&msg);
  cmptr->cmsg_len = CMSG_LEN(sizeof(int));
  cmptr->cmsg_level = SOL_SOCKET;
  cmptr->cmsg_type = SCM_RIGHTS;
  *((int *) CMSG_DATA(cmptr)) = sendfd;
#else
  msg.msg_accrights = (char *) &sendfd;
  msg.msg_accrightslen = sizeof(int);
#endif

  msg.msg_name = NULL;
  msg.msg_namelen = 0;

  iov[0].iov_base = const_cast<void *>(ptr);
  iov[0].iov_len = nbytes;
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;

  return sendmsg(fd, &msg, 0);
}

bool
reset_eids()
{
  if (setgid(getgid()) < 0){
    perror("setguid");
    return false;
  }

  if (setuid(getuid()) < 0){
    perror("setuid");
    return false;
  }

  return true;
}


static void
usage()
{
  fprintf(stderr, "usage: usrp2_socket_opener file-descriptor\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  if (argc != 2)
    usage();

  char *endptr;
  int unix_domain_fd = strtol(argv[1], &endptr, 0);
  if (*endptr != 0)
    usage();

  // FIXME get client credentials from unix_domain_fd using SCM_CREDENTIALS

  // open the raw socket
  int socket_fd = socket(PF_PACKET, SOCK_RAW, htons(0xBEEF));
  if (socket_fd == -1){
    perror("socket(PF_PACKET, SOCK_RAW, htons(0xBEEF))");
    // printf("errno = %d\n", errno);
    if (errno == EACCES || errno == ESPIPE){
      fprintf(stderr, "usrp2_socket_opener must be setuid root to open the socket using SOCK_RAW.\n");
      fprintf(stderr, "Running as root, please execute:  \n");
      fprintf(stderr, "  # chown root:usrp usrp2_socket_opener\n");
      fprintf(stderr, "  # chmod 04750 usrp2_socket_opener\n");
    }
    exit(2);
  }

  // drop privs
  if (!reset_eids()){
    fprintf(stderr, "Can't drop root permissions\n");
    exit(3);
  }

  if (write_fd(unix_domain_fd, "", 1, socket_fd) != 1){
    perror("write_fd");
    exit(4);
  }

  return 0;
}
