/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tuntap_pdu_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/pdu.h>
#include <boost/format.hpp>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#if (defined(linux) || defined(__linux) || defined(__linux__))
#include <arpa/inet.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#endif

namespace gr {
namespace network {

tuntap_pdu::sptr tuntap_pdu::make(std::string dev, int MTU, bool istunflag)
{
#if (defined(linux) || defined(__linux) || defined(__linux__))
    return gnuradio::make_block_sptr<tuntap_pdu_impl>(dev, MTU, istunflag);
#else
    throw std::runtime_error("tuntap_pdu not implemented on this platform");
#endif
}

#if (defined(linux) || defined(__linux) || defined(__linux__))
tuntap_pdu_impl::tuntap_pdu_impl(std::string dev, int MTU, bool istunflag)
    : block("tuntap_pdu", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0)),
      stream_pdu_base(istunflag ? MTU : MTU + 14),
      d_dev(dev),
      d_istunflag(istunflag)
{
    // make the tuntap
    char dev_cstr[IFNAMSIZ];
    memset(dev_cstr, 0x00, IFNAMSIZ);
    strncpy(dev_cstr, dev.c_str(), IFNAMSIZ);
    dev_cstr[IFNAMSIZ - 1] = '\0';


    bool istun = d_istunflag;
    if (istun) {
        d_fd = tun_alloc(dev_cstr, (IFF_TUN | IFF_NO_PI));
    } else {
        d_fd = tun_alloc(dev_cstr, (IFF_TAP | IFF_NO_PI));
    }

    if (d_fd <= 0)
        throw std::runtime_error(
            "gr::tuntap_pdu::make: tun_alloc failed (are you running as root?)");

    int err = set_mtu(dev_cstr, MTU);
    if (err < 0) {
        std::ostringstream msg;
        msg << boost::format("failed to set MTU to %d. You should use ifconfig to set "
                             "the MTU. E.g., `$ sudo ifconfig %s mtu %d`") %
                   MTU % dev % MTU;
        GR_LOG_ERROR(d_logger, msg.str());
    }


    std::cout << boost::format("Allocated virtual ethernet interface: %s\n"
                               "You must now use ifconfig to set its IP address. E.g.,\n"
                               "  $ sudo ifconfig %s 192.168.200.1\n"
                               "Be sure to use a different address in the same subnet "
                               "for each machine.\n") %
                     dev % dev
              << std::endl;

    // set up output message port
    message_port_register_out(msgport_names::pdus());
    start_rxthread(this, msgport_names::pdus());

    // set up input message port
    message_port_register_in(msgport_names::pdus());
    set_msg_handler(msgport_names::pdus(), [this](pmt::pmt_t msg) { this->send(msg); });
}

int tuntap_pdu_impl::tun_alloc(char* dev, int flags)
{
    struct ifreq ifr;
    int fd, err;
    const char* clonedev = "/dev/net/tun";

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

    ifr.ifr_flags = flags; /* IFF_TUN or IFF_TAP, plus maybe IFF_NO_PI */

    /* if a device name was specified, put it in the structure; otherwise,
     * the kernel will try to allocate the "next" device of the
     * specified type
     */
    if (*dev)
        strncpy(ifr.ifr_name, dev, IFNAMSIZ - 1);

    /* try to create the device */
    if ((err = ioctl(fd, TUNSETIFF, (void*)&ifr)) < 0) {
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

int tuntap_pdu_impl::set_mtu(const char* dev, int MTU)
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
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    ifr.ifr_addr.sa_family = AF_INET; /* address family */
    ifr.ifr_mtu = MTU;

    /* try to set MTU */
    if ((err = ioctl(sfd, SIOCSIFMTU, (void*)&ifr)) < 0) {
        close(sfd);
        return err;
    }

    close(sfd);
    return MTU;
}
#endif

} /* namespace network */
} /* namespace gr */
