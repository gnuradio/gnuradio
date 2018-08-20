/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
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

#ifndef ENCODER_H
#define ENCODER_H

#include <gnuradio/trellis/api.h>
#include <gnuradio/trellis/fsm.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
  namespace trellis {

    /*!
     * \brief Convolutional encoder.
     * \ingroup trellis_coding_blk
     */
    template <class IN_T, class OUT_T>
    class TRELLIS_API encoder : virtual public sync_block
    {
    public:
      typedef boost::shared_ptr< encoder<IN_T,OUT_T> > sptr;

      static sptr make(const fsm &FSM, int ST);

      static sptr make(const fsm &FSM, int ST, int K);

      virtual fsm FSM() const = 0;
      virtual int ST() const = 0;
      virtual int K() const = 0;
      virtual void set_FSM(const fsm &FSM) =0;
      virtual void set_ST(int ST) =0;
      virtual void set_K(int K) =0;
    };
    typedef encoder<std::uint8_t, std::uint8_t> encoder_bb;
    typedef encoder<std::uint8_t, std::int16_t> encoder_bs;
    typedef encoder<std::uint8_t, std::int32_t> encoder_bi;
    typedef encoder<std::int16_t, std::int16_t> encoder_ss;
    typedef encoder<std::int16_t, std::int32_t> encoder_si;
    typedef encoder<std::int32_t, std::int32_t> encoder_ii;

  } /* namespace trellis */
} /* namespace gr */

#endif /* ENCODER_H */
