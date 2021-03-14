/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_NETWORK_TUNTAP_PDU_IMPL_H
#define INCLUDED_NETWORK_TUNTAP_PDU_IMPL_H

#include "stream_pdu_base.h"
#include <gnuradio/network/tuntap_pdu.h>

#if (defined(linux) || defined(__linux) || defined(__linux__))
#include <linux/if_tun.h>
#endif

namespace gr {
namespace network {

class tuntap_pdu_impl : public tuntap_pdu, public stream_pdu_base
{
#if (defined(linux) || defined(__linux) || defined(__linux__))
private:
    const std::string d_dev;
    const bool d_istunflag;
    int tun_alloc(char* dev, int flags);
    int set_mtu(const char* dev, int MTU);

public:
    tuntap_pdu_impl(std::string dev, int MTU, bool istunflag);
#endif
};

} /* namespace network */
} /* namespace gr */

#endif /* INCLUDED_NETWORK_TUNTAP_PDU_IMPL_H */
