/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018 Free Software Foundation, Inc.
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

#include "sccc_decoder_blk_impl.h"
#include <gnuradio/io_signature.h>
#include <iostream>
#include <gnuradio/trellis/core_algorithms.h>

namespace gr {
  namespace trellis {

    template <class T>
    typename sccc_decoder_blk<T>::sptr
    sccc_decoder_blk<T>::make(const fsm &FSMo, int STo0, int SToK,
		      const fsm &FSMi, int STi0, int STiK,
		      const interleaver &INTERLEAVER,
		      int blocklength,
		      int repetitions,
		      siso_type_t SISO_TYPE)
    {
      return gnuradio::get_initial_sptr
	(new sccc_decoder_blk_impl<T>(FSMo, STo0, SToK, FSMi, STi0, STiK,
			 INTERLEAVER, blocklength, repetitions,
			 SISO_TYPE));
    }

    template <class T>
    sccc_decoder_blk_impl<T>::sccc_decoder_blk_impl(const fsm &FSMo, int STo0, int SToK,
			     const fsm &FSMi, int STi0, int STiK,
			     const interleaver &INTERLEAVER,
			     int blocklength,
			     int repetitions,
			     siso_type_t SISO_TYPE)
    : block("sccc_decoder_blk",
	       io_signature::make(1, 1, sizeof(float)),
	       io_signature::make(1, 1, sizeof(T))),
      d_FSMo(FSMo), d_STo0(STo0), d_SToK(SToK),
      d_FSMi(FSMi), d_STi0(STi0), d_STiK(STiK),
      d_INTERLEAVER(INTERLEAVER),
      d_blocklength(blocklength),
      d_repetitions(repetitions),
      d_SISO_TYPE(SISO_TYPE)
    {
      this->set_relative_rate(1, (uint64_t) d_FSMi.O());
      this->set_output_multiple(d_blocklength);
    }

    template <class T>
    sccc_decoder_blk_impl<T>::~sccc_decoder_blk_impl()
    {
    }

    template <class T>
    void
    sccc_decoder_blk_impl<T>::forecast(int noutput_items,
			  gr_vector_int &ninput_items_required)
    {
      int input_required =  d_FSMi.O() * noutput_items ;
      ninput_items_required[0] = input_required;
    }

    //===========================================================

    template <class T>
    int
    sccc_decoder_blk_impl<T>::general_work(int noutput_items,
			      gr_vector_int &ninput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
    {
      int nblocks = noutput_items / d_blocklength;
      float (*p2min)(float, float) = NULL;

      if(d_SISO_TYPE == TRELLIS_MIN_SUM)
	p2min = &min;
      else if(d_SISO_TYPE == TRELLIS_SUM_PRODUCT)
	p2min = &min_star;

      const float *in = (const float*)input_items[0];
      T *out = (T*)output_items[0];

      for(int n = 0; n < nblocks; n++) {
	sccc_decoder(d_FSMo, d_STo0, d_SToK,
		     d_FSMi, d_STi0, d_STiK,
		     d_INTERLEAVER, d_blocklength, d_repetitions,
		     p2min,
		     &(in[n*d_blocklength*d_FSMi.O()]),
		     &(out[n*d_blocklength]));
      }

      this->consume_each(d_FSMi.O() * noutput_items );
      return noutput_items;
    }

template class sccc_decoder_blk<std::uint8_t>;
template class sccc_decoder_blk<std::int16_t>;
template class sccc_decoder_blk<std::int32_t>;
  } /* namespace trellis */
} /* namespace gr */
