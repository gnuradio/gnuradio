/* -*- c++ -*- */
/*
 * Copyright 2005,2007,2008 Free Software Foundation, Inc.
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

#include "eth_buffer.h"
#include "ethernet.h"
#include "pktfilter.h"
#include <open_usrp2_socket.h>

#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
//#include <features.h>
#include <errno.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <netinet/in.h>

#include <linux/types.h>
#include <netpacket/packet.h>
#include <linux/filter.h>	// packet filter

namespace usrp2 {

  static int
  open_packet_socket (std::string ifname, int protocol)
  {
#if 0    
    if (protocol == 0)
      protocol = htons(ETH_P_ALL);
    
    int fd = socket (PF_PACKET, SOCK_RAW, protocol);
#else
    int fd = usrp2::open_usrp2_socket();
#endif

    if (fd == -1){
      fprintf (stderr, "%s: socket: %s\n", ifname.c_str(), strerror (errno));
      return -1;
    }
    
    // get interface index
    struct ifreq ifr;
    memset (&ifr, 0, sizeof(ifr));
    strncpy (ifr.ifr_name, ifname.c_str(), sizeof (ifr.ifr_name));
    int res = ioctl (fd, SIOCGIFINDEX, &ifr);
    if (res != 0){
      ::close (fd);
      fprintf (stderr, "%s: SIOCGIFINDEX: %s\n", ifname.c_str(), strerror(errno));
      return -1;
    }
    int ifindex = ifr.ifr_ifindex;
    
    // bind to the specified interface
    sockaddr_ll sa;
    memset (&sa, 0, sizeof (sa));
    sa.sll_family = AF_PACKET;
    sa.sll_protocol = protocol;
    sa.sll_ifindex = ifindex;
    res = bind (fd, (struct sockaddr *)&sa, sizeof (sa));
    if (res != 0){
      ::close (fd);
      fprintf (stderr, "%s: bind: %s\n", ifname.c_str(), strerror(errno));
      return -1;
    }
    return fd;
  }
  
  static void
  extract_mac_addr (unsigned char *mac, const unsigned char *hwaddr)
  {
    int	i;
    for (i = 0; i < 6; i++)
      mac[i] = 0xff;
    
    i = 0;
    for (int j = 0; j < 14; j++){
      if (hwaddr[j] != 0xff){
	mac[i++] = hwaddr[j];
	if (i == 6)
	  return;
      }
    }
  }
  
  static bool
  get_mac_addr (std::string ifname, int fd, unsigned char *mac)
  {
    struct ifreq ifr;
    memset (&ifr, 0, sizeof(ifr));
    strncpy (ifr.ifr_name, ifname.c_str(), sizeof (ifr.ifr_name));
    int res = ioctl (fd, SIOCGIFHWADDR, &ifr);
    if (res != 0){
      fprintf (stderr, "%s: SIOCGIFHWADDR: %s\n", ifname.c_str(), strerror(errno));
      return false;
    }
    else {
      if (0){
	for (unsigned i = 0; i < sizeof (ifr.ifr_hwaddr.sa_data); i++)
	  fprintf (stderr, "%02x", ifr.ifr_hwaddr.sa_data[i]);
	fprintf (stderr, "\n");
      }
    }
    extract_mac_addr (mac, (unsigned char *)ifr.ifr_hwaddr.sa_data);
    return true;
  }
  
  ethernet::ethernet ()
  {
    d_fd = -1;
    memset (d_mac, 0, sizeof (d_mac));
  }
  
  ethernet::~ethernet ()
  {
    close ();
  }
  
  bool
  ethernet::open (std::string ifname, int protocol)
  {
    if (d_fd != -1){
      fprintf (stderr, "ethernet: already open\n");
      return false;
    }
    if ((d_fd = open_packet_socket (ifname, protocol)) == -1){
      return false;
    }
    get_mac_addr (ifname, d_fd, d_mac);
    return true;
  }
  
  bool
  ethernet::close ()
  {
    if (d_fd >= 0){
      ::close (d_fd);
      d_fd = -1;
    }
    return true;
  }
  
  int
  ethernet::read_packet (void *buf, int buflen)
  {
    int len = recvfrom (d_fd, buf, buflen, 0, (sockaddr *) 0, 0);
    return len;
  }
  
  int
  ethernet::read_packet_dont_block (void *buf, int buflen)
  {
    int len = recvfrom (d_fd, buf, buflen, MSG_DONTWAIT, 0, 0);
    if (len == -1 && errno == EAGAIN)
      return 0;
    
    return len;
  }
  
  int
  ethernet::write_packet (const void *buf, int buflen)
  {
    int retval = send (d_fd, buf, buflen, 0);
    if (retval < 0){
      if (errno == EINTR)
	return write_packet (buf, buflen);
      
      perror ("ethernet:write_packet: send");
      return -1;
    }
    return retval;
  }

  int
  ethernet::write_packetv(const eth_iovec *iov, size_t iovlen)
  {
    struct msghdr mh;
    memset(&mh, 0, sizeof(mh));
    mh.msg_iov = const_cast<eth_iovec*>(iov);
    mh.msg_iovlen = iovlen;

    int retval = sendmsg(d_fd, &mh, 0);
    if (retval < 0){
      if (errno == EINTR)
	return write_packetv(iov, iovlen);
      
      perror("ethernet:write_packetv: send");
      return -1;
    }
    return retval;
  }
  
  bool
  ethernet::attach_pktfilter (pktfilter *pf)
  {
    struct sock_fprog filter;
    filter.len = pf->d_len;
    filter.filter = pf->d_inst;
    
    int r = setsockopt (d_fd, SOL_SOCKET, SO_ATTACH_FILTER, &filter, sizeof (filter));
    if (r < 0){
      perror ("ethernet:attach:  SO_ATTACH_FILTER");
      return false;
    }
    return true;
  }
  
} // namespace usrp2
