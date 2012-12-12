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

#ifndef INCLUDED_GR_TUNTAP_PDU_H
#define INCLUDED_GR_TUNTAP_PDU_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_message.h>
#include <gr_msg_queue.h>
#include <gr_stream_pdu_base.h>

#if (defined(linux) || defined(__linux) || defined(__linux__))

#include <linux/if_tun.h>

class gr_tuntap_pdu;
typedef boost::shared_ptr<gr_tuntap_pdu> gr_tuntap_pdu_sptr;

GR_CORE_API gr_tuntap_pdu_sptr gr_make_tuntap_pdu (std::string dev, int MTU=10000);

/*!
 * \brief Gather received items into messages and insert into msgq
 * \ingroup sink_blk
 */
class GR_CORE_API gr_tuntap_pdu : public gr_stream_pdu_base
{
 private:
  friend GR_CORE_API gr_tuntap_pdu_sptr
  gr_make_tuntap_pdu(std::string dev, int MTU);
  int tun_alloc(char* dev, int flags = IFF_TAP | IFF_NO_PI);
  std::string d_dev;
 protected:
  gr_tuntap_pdu (std::string dev, int MTU=10000);

 public:
  ~gr_tuntap_pdu () {}

};

#else // if not linux

class gr_tuntap_pdu
{
private:
  gr_tuntap_pdu() {};
public:
  ~gr_tuntap_pdu() {};
};

GR_CORE_API gr_block_sptr gr_make_tuntap_pdu (std::string dev, int MTU=0);

#endif

#endif /* INCLUDED_GR_TUNTAP_PDU_H */
