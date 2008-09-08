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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <open_usrp2_socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <string>

static const char *helper = "usrp2_socket_opener";

static ssize_t
read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
  struct msghdr msg;
  struct iovec iov[1];
  ssize_t n;

#ifdef HAVE_STRUCT_MSGHDR_MSG_CONTROL
  union {
    struct cmsghdr cm;
    char     control[CMSG_SPACE(sizeof (int))];
  } control_un;
  struct cmsghdr  *cmptr;

  msg.msg_control  = control_un.control;
  msg.msg_controllen = sizeof(control_un.control);
#else
  int     newfd;

  msg.msg_accrights = (char *) &newfd;
  msg.msg_accrightslen = sizeof(int);
#endif

  msg.msg_name = NULL;
  msg.msg_namelen = 0;

  iov[0].iov_base = ptr;
  iov[0].iov_len = nbytes;
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;

  if ((n = recvmsg(fd, &msg, 0)) <= 0)
    return n;

#ifdef  HAVE_STRUCT_MSGHDR_MSG_CONTROL
  if ((cmptr = CMSG_FIRSTHDR(&msg)) != NULL
      && cmptr->cmsg_len == CMSG_LEN(sizeof(int))){
    if (cmptr->cmsg_level != SOL_SOCKET){
      fprintf(stderr, "read_fd: control level != SOL_SOCKET\n");
      return -1;
    }
    if (cmptr->cmsg_type != SCM_RIGHTS){
      fprintf(stderr, "read_fd: control type != SCM_RIGHTS\n");
      return -1;
    }
    *recvfd = *((int *) CMSG_DATA(cmptr));
  } else
    *recvfd = -1;           /* descriptor was not passed */
#else
  if (msg.msg_accrightslen == sizeof(int))
    *recvfd = newfd;
  else
    *recvfd = -1;       /* descriptor was not passed */
#endif

  return n;
}

int
usrp2::open_usrp2_socket()
{
  int     fd = -1, sockfd[2], status;
  pid_t   childpid;
  char    c, argsockfd[10];

  if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd) != 0){
    perror("socketpair");
    return -1;
  }

  if ((childpid = fork()) == 0) { /* child process */
    close(sockfd[0]);
    snprintf(argsockfd, sizeof(argsockfd), "%d", sockfd[1]);
    execlp(helper, helper, argsockfd, (char *) NULL);
    std::string msg("execlp: couldn't exec " + std::string(helper));
    perror(msg.c_str());
    close(sockfd[0]);
    close(sockfd[1]);
    return -1;
  }

  /* parent process - wait for the child to terminate */
  close(sockfd[1]);           /* close the end we don't use */

  waitpid(childpid, &status, 0);
  if (!WIFEXITED(status)){
    fprintf(stderr, "child did not terminate\n");
    return -1;
  }
  if ((status = WEXITSTATUS(status)) == 0)
    read_fd(sockfd[0], &c, 1, &fd);
  else {
    errno = status;         /* bogus: set errno value from child's status */
    fd = -1;
  }

  close(sockfd[0]);
  return (fd);
}
