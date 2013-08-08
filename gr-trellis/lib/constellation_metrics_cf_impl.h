/* -*- c++ -*- */
/*
 * Copyright 2004,2010-2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_TRELLIS_CONSTELLATION_METRICS_CF_IMPL_H
#define INCLUDED_TRELLIS_CONSTELLATION_METRICS_CF_IMPL_H

#include <gnuradio/trellis/api.h>
#include <gnuradio/trellis/constellation_metrics_cf.h>

namespace gr {
  namespace trellis {

    class constellation_metrics_cf_impl : public constellation_metrics_cf
    {
    private:
      digital::constellation_sptr d_constellation;
      digital::trellis_metric_type_t d_TYPE;
      unsigned int d_O;
      unsigned int d_D;

    public:
      constellation_metrics_cf_impl(digital::constellation_sptr constellation,
				    digital::trellis_metric_type_t TYPE);
      ~constellation_metrics_cf_impl();

      void forecast(int noutput_items,
		    gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_TRELLIS_CONSTELLATION_METRICS_CF_IMPL_H */
