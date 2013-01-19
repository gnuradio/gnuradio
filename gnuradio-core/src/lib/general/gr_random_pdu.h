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

#ifndef INCLUDED_GR_RANDOM_PDU_H
#define INCLUDED_GR_RANDOM_PDU_H

#include <gr_core_api.h>
#include <gr_block.h>
#include <gr_message.h>
#include <gr_msg_queue.h>

#include <boost/random.hpp>
#include <boost/generator_iterator.hpp>

class gr_random_pdu;
typedef boost::shared_ptr<gr_random_pdu> gr_random_pdu_sptr;

GR_CORE_API gr_random_pdu_sptr gr_make_random_pdu (int mintime, int maxtime);

/*!
 * \brief Send message at defined interval
 * \ingroup msg_blk
 */
class GR_CORE_API gr_random_pdu : public gr_block
{
 private:
  friend GR_CORE_API gr_random_pdu_sptr
  gr_make_random_pdu(int mintime, int maxtime);

  void output_random();

  boost::mt19937 rng;
  boost::uniform_int<> urange;
  boost::uniform_int<> brange;
  boost::variate_generator< boost::mt19937, boost::uniform_int<> > rvar; // pdu length
  boost::variate_generator< boost::mt19937, boost::uniform_int<> > bvar; // pdu contents

 public:
  gr_random_pdu (int, int);
  bool start();
  void generate_pdu(pmt::pmt_t msg){ output_random(); }
  void generate_pdu(){ output_random(); }
};

#endif /* INCLUDED_GR_RANDOM_PDU_H */
