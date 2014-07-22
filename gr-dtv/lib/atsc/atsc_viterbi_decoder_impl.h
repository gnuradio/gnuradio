/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DTV_ATSC_VITERBI_DECODER_IMPL_H
#define INCLUDED_DTV_ATSC_VITERBI_DECODER_IMPL_H

#include <gnuradio/dtv/atsc_viterbi_decoder.h>
#include <gnuradio/dtv/atsc_consts.h>
#include "atsc_syminfo_impl.h"
#include "atsc_interleaver_fifo.h"

#define	USE_SIMPLE_SLICER  0
#define NCODERS 12

#if USE_SIMPLE_SLICER
#include "atsc_fake_single_viterbi.h"
#else
#include "atsc_single_viterbi.h"
#endif

namespace gr {
  namespace dtv {

#if USE_SIMPLE_SLICER
    typedef atsc_fake_single_viterbi single_viterbi_t;
#else
    typedef atsc_single_viterbi      single_viterbi_t;
#endif

    class atsc_viterbi_decoder_impl : public atsc_viterbi_decoder
    {
    private:
      int last_start;
      typedef interleaver_fifo<unsigned char> fifo_t;

      static const int SEGMENT_SIZE = ATSC_MPEG_RS_ENCODED_LENGTH;	// 207
      static const int OUTPUT_SIZE = (SEGMENT_SIZE * 12);
      static const int INPUT_SIZE = (ATSC_DATA_SEGMENT_LENGTH * 12);

      single_viterbi_t viterbi[NCODERS];
      fifo_t *fifo[NCODERS];

    public:
      atsc_viterbi_decoder_impl();
      ~atsc_viterbi_decoder_impl();

      void reset();

      virtual int work(int noutput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_VITERBI_DECODER_IMPL_H */
