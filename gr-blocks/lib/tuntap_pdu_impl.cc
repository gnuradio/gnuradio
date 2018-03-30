/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#include "tuntap_pdu_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/blocks/pdu.h>
#include <boost/format.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if (defined(linux) || defined(__linux) || defined(__linux__))
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/if.h>
#endif

namespace gr {
  namespace blocks {

    tuntap_pdu::sptr
    tuntap_pdu::make(std::string dev, int MTU, bool istunflag)
    {
#if (defined(linux) || defined(__linux) || defined(__linux__))
      return gnuradio::get_initial_sptr(new tuntap_pdu_impl(dev, MTU, istunflag));
#else
      throw std::runtime_error("tuntap_pdu not implemented on this platform");
#endif
    }

#if (defined(linux) || defined(__linux) || defined(__linux__))
    tuntap_pdu_impl::tuntap_pdu_impl(std::string dev, int MTU, bool istunflag)
      :	block("tuntap_pdu",
		 io_signature::make (0, 0, 0),
		 io_signature::make (0, 0, 0)),
	stream_pdu_base(istunflag ? MTU : MTU + 14),
	d_dev(dev),
	d_istunflag(istunflag)
    {
      // make the tuntap
      char dev_cstr[1024];
      memset(dev_cstr, 0x00, 1024);
      strncpy(dev_cstr, dev.c_str(), std::min(sizeof(dev_cstr), dev.size()));

      bool istun = d_istunflag;
      if(istun){
	d_fd = tun_alloc(dev_cstr, (IFF_TUN | IFF_NO_PI));
      } else {
	d_fd = tun_alloc(dev_cstr, (IFF_TAP | IFF_NO_PI));
      }

      if (d_fd <= 0)
        throw std::runtime_error("gr::tuntap_pdu::make: tun_alloc failed (are you running as root?)");

      int err = set_mtu(dev_cstr, MTU);
      if(err < 0)
        std::cerr << boost::format(
          "gr::tuntap_pdu: failed to set MTU to %d.\n"
          "You should use ifconfig to set the MTU. E.g.,\n"
          "  $ sudo ifconfig %s mtu %d\n"
          ) % MTU % dev % MTU << std::endl;

      std::cout << boost::format(
	"Allocated virtual ethernet interface: %s\n"
        "You must now use ifconfig to set its IP address. E.g.,\n"
        "  $ sudo ifconfig %s 192.168.200.1\n"
        "Be sure to use a different address in the same subnet for each machine.\n"
        ) % dev % dev << std::endl;

      // set up output message port
      message_port_register_out(pdu::pdu_port_id());
      start_rxthread(this, pdu::pdu_port_id());

      // set up input message port
      message_port_register_in(pdu::pdu_port_id());
      set_msg_handler(pdu::pdu_port_id(), boost::bind(&tuntap_pdu_impl::send, this, _1));
    }

    int
    tuntap_pdu_impl::tun_alloc(char *dev, int flags)
    {
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
      if ((fd = open(clonedev, O_RDWR)) < 0)
        return fd;

      /* preparation of the struct ifr, of type "struct ifreq" */
      memset(&ifr, 0, sizeof(ifr));

      ifr.ifr_flags = flags;   /* IFF_TUN or IFF_TAP, plus maybe IFF_NO_PI */

      /* if a device name was specified, put it in the structure; otherwise,
       * the kernel will try to allocate the "next" device of the
       * specified type
       */
      if (*dev)
        strncpy(ifr.ifr_name, dev, IFNAMSIZ - 1);

      /* try to create the device */
      if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
        close(fd);
        return err;
      }

      /* if the operation was successful, write back the name of the
       * interface to the variable "dev", so the caller can know
       * it. Note that the caller MUST reserve space in *dev (see calling
       * code below)
       */
      strcpy(dev, ifr.ifr_name);

      /* this is the special file descriptor that the caller will use to talk
       * with the virtual interface
       */
      return fd;
    }

    int
    tuntap_pdu_impl::set_mtu(const char *dev, int MTU)
    {
      struct ifreq ifr;
      int sfd, err;

      /* MTU must be set by passing a socket fd to ioctl;
       * create an arbitrary socket for this purpose
       */
      if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
          return sfd;

      /* preparation of the struct ifr, of type "struct ifreq" */
      memset(&ifr, 0, sizeof(ifr));
      strncpy(ifr.ifr_name, dev, IFNAMSIZ);
      ifr.ifr_addr.sa_family = AF_INET; /* address family */
      ifr.ifr_mtu = MTU;

      /* try to set MTU */
      if ((err = ioctl(sfd, SIOCSIFMTU, (void *) &ifr)) < 0)
        return err;

      return MTU;
    }
#endif

  } /* namespace blocks */
}/* namespace gr */
