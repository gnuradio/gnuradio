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

#ifndef INCLUDED_BLOCKS_TUNTAP_PDU_IMPL_H
#define INCLUDED_BLOCKS_TUNTAP_PDU_IMPL_H

#include <gnuradio/blocks/tuntap_pdu.h>
#include "stream_pdu_base.h"

#if (defined(linux) || defined(__linux) || defined(__linux__))
#include <linux/if_tun.h>
#endif

namespace gr {
  namespace blocks {

    class tuntap_pdu_impl : public tuntap_pdu, public stream_pdu_base
    {
#if (defined(linux) || defined(__linux) || defined(__linux__))
    private:
      std::string d_dev;
      bool d_istunflag;
      int tun_alloc(char *dev, int flags);
      int set_mtu(const char *dev, int MTU);

    public:
      tuntap_pdu_impl(std::string dev, int MTU, bool istunflag);
#endif
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_TUNTAP_PDU_IMPL_H */
