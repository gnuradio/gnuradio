/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_tuntap_pdu.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <string.h>
#include <iostream>
#include <gr_pdu.h>
#include <boost/format.hpp>

#if (defined(linux) || defined(__linux) || defined(__linux__))

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <linux/if.h>


// public constructor that returns a shared_ptr

gr_tuntap_pdu_sptr
gr_make_tuntap_pdu (std::string dev, int MTU)
{
  return gnuradio::get_initial_sptr(new gr_tuntap_pdu(dev, MTU));
}

gr_tuntap_pdu::gr_tuntap_pdu (std::string dev, int MTU)
    : gr_stream_pdu_base(MTU)
{

    // make the tuntap
    char dev_cstr[1024];
    memset(dev_cstr, 0x00, 1024);
    strncpy(dev_cstr, dev.c_str(), std::min(sizeof(dev_cstr), dev.size()));
    d_fd = tun_alloc(dev_cstr);
    if(d_fd <= 0){
        throw std::runtime_error("TunTap make: tun_alloc failed (are you running as root?)");
        }

    std::cout << boost::format(
        "Allocated virtual ethernet interface: %s\n"
        "You must now use ifconfig to set its IP address. E.g.,\n"
        "  $ sudo ifconfig %s 192.168.200.1\n"
        "Be sure to use a different address in the same subnet for each machine.\n"
        ) % dev % dev << std::endl;

    // set up output message port
    message_port_register_out(pmt::mp("pdus"));
    start_rxthread(pmt::mp("pdus"));
    
    // set up input message port
    message_port_register_in(pmt::mp("pdus"));
    set_msg_handler(pmt::mp("pdus"), boost::bind(&gr_tuntap_pdu::send, this, _1));
}


int gr_tuntap_pdu::tun_alloc(char *dev, int flags) {
  struct ifreq ifr;
  int fd, err;
  const char *clonedev = "/dev/net/tun";

  /* Arguments taken by the function:
   *
   * char *dev: the name of an interface (or '\0'). MUST have enough
   *   space to hold the interface name if '\0' is passed
   * int flags: interface flags (eg, IFF_TUN etc.)
   */

   /* open the clone device */
   if( (fd = open(clonedev, O_RDWR)) < 0 ) {
     return fd;
   }

   /* preparation of the struct ifr, of type "struct ifreq" */
   memset(&ifr, 0, sizeof(ifr));

   ifr.ifr_flags = flags;   /* IFF_TUN or IFF_TAP, plus maybe IFF_NO_PI */

   if (*dev) {
     /* if a device name was specified, put it in the structure; otherwise,
      * the kernel will try to allocate the "next" device of the
      * specified type */
     strncpy(ifr.ifr_name, dev, IFNAMSIZ);
   }

   /* try to create the device */
   if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ) {
     close(fd);
     return err;
   }

  /* if the operation was successful, write back the name of the
   * interface to the variable "dev", so the caller can know
   * it. Note that the caller MUST reserve space in *dev (see calling
   * code below) */
  strcpy(dev, ifr.ifr_name);

  /* this is the special file descriptor that the caller will use to talk
   * with the virtual interface */
  return fd;
}

#else //if not linux

gr_block_sptr
gr_make_tuntap_pdu(std::string dev, int MTU)
{
    gr_block_sptr rv;
    throw std::runtime_error("tuntap only implemented on linux");
    return rv;
}

#endif
