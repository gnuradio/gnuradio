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


#ifndef VITERBI_IMPL_H
#define VITERBI_IMPL_H

#include <gnuradio/trellis/viterbi.h>

namespace gr {
  namespace trellis {

template<class T>
    class viterbi_impl : public  viterbi<T>
    {
    private:
      fsm d_FSM;
      int d_K;
      int d_S0;
      int d_SK;
      //std::vector<int> d_trace;

    public:
      viterbi_impl(const fsm &FSM, int K,
		  int S0, int SK);
      ~viterbi_impl();

      fsm FSM() const  { return d_FSM; }
      int K()  const { return d_K; }
      int S0()  const { return d_S0; }
      int SK()  const { return d_SK; }

      void set_FSM(const fsm &FSM);
      void set_K(int K);
      void set_S0(int S0);
      void set_SK(int SK);
      //std::vector<int> trace () const { return d_trace; }

      void forecast(int noutput_items,
		    gr_vector_int &ninput_items_required);
      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } /* namespace trellis */
} /* namespace gr */

#endif /* VITERBI_IMPL_H */
