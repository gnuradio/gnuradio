/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_TRELLIS_SISO_F_IMPL_H
#define INCLUDED_TRELLIS_SISO_F_IMPL_H

#include <gnuradio/trellis/api.h>
#include <gnuradio/trellis/fsm.h>
#include <gnuradio/trellis/siso_type.h>
#include <gnuradio/trellis/core_algorithms.h>
#include <gnuradio/trellis/siso_f.h>

namespace gr {
  namespace trellis {

    class siso_f_impl : public siso_f
    {
    private:
      fsm d_FSM;
      int d_K;
      int d_S0;
      int d_SK;
      bool d_POSTI;
      bool d_POSTO;
      siso_type_t d_SISO_TYPE;
      void recalculate();
      //std::vector<float> d_alpha;
      //std::vector<float> d_beta;

    public:
      siso_f_impl(const fsm &FSM, int K,
		  int S0, int SK,
		  bool POSTI, bool POSTO,
		  siso_type_t d_SISO_TYPE);
      ~siso_f_impl();

      fsm FSM() const { return d_FSM; }
      int K() const { return d_K; }
      int S0() const { return d_S0; }
      int SK() const { return d_SK; }
      bool POSTI() const { return d_POSTI; }
      bool POSTO() const { return d_POSTO; }
      siso_type_t SISO_TYPE() const { return d_SISO_TYPE; }

      void set_FSM(const fsm &FSM);
      void set_K(int K);
      void set_S0(int S0);
      void set_SK(int SK);
      void set_POSTI(bool POSTI);
      void set_POSTO(bool POSTO);
      void set_SISO_TYPE(trellis::siso_type_t type);

      void forecast(int noutput_items,
		    gr_vector_int &ninput_items_required);

      int general_work (int noutput_items,
			gr_vector_int &ninput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
    };

  } /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_TRELLIS_SISO_F_IMPL_H */
