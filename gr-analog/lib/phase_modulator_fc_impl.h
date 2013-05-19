/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ANALOG_PHASE_MODULATOR_FC_IMPL_H
#define INCLUDED_ANALOG_PHASE_MODULATOR_FC_IMPL_H

#include <gnuradio/analog/phase_modulator_fc.h>

namespace gr {
  namespace analog {

    class phase_modulator_fc_impl : public phase_modulator_fc
    {
    private:
      double d_sensitivity;
      double d_phase;

    public:
      phase_modulator_fc_impl(double sensitivity);
      ~phase_modulator_fc_impl();

      double sensitivity() const { return d_sensitivity; }
      double phase() const { return d_phase; }

      void set_sensitivity(double s) { d_sensitivity = s; }
      void set_phase(double p) { d_phase = p; }

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_PHASE_MODULATOR_FC_IMPL_H */
