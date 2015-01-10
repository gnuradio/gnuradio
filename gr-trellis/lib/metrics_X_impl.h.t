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

// @WARNING@

#ifndef @GUARD_NAME@
#define @GUARD_NAME@

#include <gnuradio/trellis/@BASE_NAME@.h>

namespace gr {
  namespace trellis {

    class @IMPL_NAME@ : public @BASE_NAME@
    {
    private:
      int d_O;
      int d_D;
      digital::trellis_metric_type_t d_TYPE;
      std::vector<@I_TYPE@> d_TABLE;

    public:
      @IMPL_NAME@(int O, int D,  const std::vector<@I_TYPE@> &TABLE,
		  digital::trellis_metric_type_t TYPE);
      ~@IMPL_NAME@();

      int O() const { return d_O; }
      int D() const { return d_D; }
      digital::trellis_metric_type_t TYPE() const { return d_TYPE; }
      std::vector<@I_TYPE@> TABLE() const { return d_TABLE; }

      void set_O(int O);
      void set_D(int D);
      void set_TYPE(digital::trellis_metric_type_t type);
      void set_TABLE(const std::vector<@I_TYPE@> &table);

      void forecast(int noutput_items,
		    gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } /* namespace trellis */
} /* namespace gr */

#endif /* @GUARD_NAME@ */
