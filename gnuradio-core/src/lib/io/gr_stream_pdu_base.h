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

#ifndef INCLUDED_GR_STREAM_PDU_BASE_H
#define INCLUDED_GR_STREAM_PDU_BASE_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_message.h>
#include <gr_msg_queue.h>

/*!
 * \brief Gather received items into messages and insert into msgq
 * \ingroup sink_blk
 */
class GR_CORE_API gr_stream_pdu_base : public gr_sync_block
{
 public:
  boost::shared_ptr<boost::thread> d_thread;
  bool d_finished;
  bool d_started;
  std::vector<uint8_t> d_rxbuf;
  void run();
  int d_fd;
  gr_stream_pdu_base (int MTU=10000);
  ~gr_stream_pdu_base ();
  void send(pmt::pmt_t msg);
  bool wait_ready();
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
  void start_rxthread(pmt::pmt_t _rxport);
  void stop_rxthread();
 private:
  pmt::pmt_t rxport;
};

typedef boost::shared_ptr<gr_stream_pdu_base> gr_stream_pdu_base_sptr;

#endif /* INCLUDED_GR_TUNTAP_PDU_H */
