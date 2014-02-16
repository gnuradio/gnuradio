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

#ifndef INCLUDED_BLOCKS_RANDOM_PDU_IMPL_H
#define INCLUDED_BLOCKS_RANDOM_PDU_IMPL_H

#include <gnuradio/blocks/random_pdu.h>
#include <boost/random.hpp>
#include <boost/generator_iterator.hpp>

namespace gr {
  namespace blocks {

    class random_pdu_impl : public random_pdu
    {
    private:
      boost::mt19937 d_rng;
      boost::uniform_int<> d_urange;
      boost::uniform_int<> d_brange;
      boost::variate_generator< boost::mt19937, boost::uniform_int<> > d_rvar; // pdu length
      boost::variate_generator< boost::mt19937, boost::uniform_int<> > d_bvar; // pdu contents
      char d_mask;
      int d_length_modulo;

    public:
      random_pdu_impl(int min_items, int max_items, char byte_mask, int length_modulo);

      bool start();
      void output_random();
      void generate_pdu(pmt::pmt_t msg) { output_random(); }
      void generate_pdu() { output_random(); }
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_RANDOM_PDU_IMPL_H */
