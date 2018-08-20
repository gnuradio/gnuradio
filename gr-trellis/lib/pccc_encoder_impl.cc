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

#include "pccc_encoder_impl.h"
#include <gnuradio/io_signature.h>
#include <iostream>

namespace gr {
  namespace trellis {

    template <class IN_T, class OUT_T>
    typename pccc_encoder<IN_T,OUT_T>::sptr
    pccc_encoder<IN_T,OUT_T>::make(const fsm &FSM1, int ST1,
		      const fsm &FSM2, int ST2,
		      const interleaver &INTERLEAVER,
		      int blocklength)
    {
      return gnuradio::get_initial_sptr
	(new pccc_encoder_impl<IN_T,OUT_T>(FSM1, ST1, FSM2, ST2, INTERLEAVER, blocklength));
    }

    template <class IN_T, class OUT_T>
    pccc_encoder_impl<IN_T,OUT_T>::pccc_encoder_impl(const fsm &FSM1, int ST1,
			     const fsm &FSM2, int ST2,
			     const interleaver &INTERLEAVER,
			     int blocklength)
    : sync_block("pccc_encoder<IN_T,OUT_T>",
		    io_signature::make(1, 1, sizeof(IN_T)),
		    io_signature::make(1, 1, sizeof(OUT_T))),
      d_FSM1(FSM1), d_ST1(ST1),
      d_FSM2(FSM2), d_ST2(ST2),
      d_INTERLEAVER(INTERLEAVER),
      d_blocklength(blocklength)
    {
      assert(d_FSM1.I() == d_FSM2.I());
      this->set_output_multiple(d_blocklength);
      d_buffer.resize(d_blocklength);
    }

    template <class IN_T, class OUT_T>
    pccc_encoder_impl<IN_T,OUT_T>::~pccc_encoder_impl()
    {
    }

    template <class IN_T, class OUT_T>
    int
    pccc_encoder_impl<IN_T,OUT_T>::work(int noutput_items,
		 gr_vector_const_void_star &input_items,
		 gr_vector_void_star &output_items)
    {
      for(int b = 0 ; b<noutput_items/d_blocklength; b++) {
	const IN_T *in = (const IN_T*)input_items[0]+b*d_blocklength;
	OUT_T *out = (OUT_T*)output_items[0]+b*d_blocklength;

	int ST1_tmp = d_ST1;
	int ST2_tmp = d_ST2;
	for(int i = 0; i < d_blocklength; i++) {
	  int k = d_INTERLEAVER.INTER()[i];
	  int o1 = d_FSM1.OS()[ST1_tmp*d_FSM1.I()+in[i]];
	  ST1_tmp = (int) d_FSM1.NS()[ST1_tmp*d_FSM1.I()+in[i]];
	  int o2 = d_FSM2.OS()[ST2_tmp*d_FSM2.I()+in[k]];
	  ST2_tmp = (int) d_FSM2.NS()[ST2_tmp*d_FSM2.I()+in[k]];
	  out[i] = (OUT_T) (o1*d_FSM2.O() + o2);
	}
      }
      return noutput_items;
    }
    template class pccc_encoder<std::uint8_t, std::uint8_t>;
    template class pccc_encoder<std::uint8_t, std::int16_t>;
    template class pccc_encoder<std::uint8_t, std::int32_t>;
    template class pccc_encoder<std::int16_t, std::int16_t>;
    template class pccc_encoder<std::int16_t, std::int32_t>;
    template class pccc_encoder<std::int32_t, std::int32_t>;

  } /* namespace trellis */
} /* namespace gr */
