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

#ifndef INCLUDED_DIGITAL_OFDM_SYNC_SC_CFB_IMPL_H
#define INCLUDED_DIGITAL_OFDM_SYNC_SC_CFB_IMPL_H

#include <gnuradio/digital/ofdm_sync_sc_cfb.h>
#include <gnuradio/blocks/plateau_detector_fb.h>

namespace gr {
  namespace digital {

    class ofdm_sync_sc_cfb_impl : public ofdm_sync_sc_cfb
    {
     public:
      ofdm_sync_sc_cfb_impl(int fft_len, int cp_len, bool use_even_carriers, float threshold);
      ~ofdm_sync_sc_cfb_impl();

      virtual void set_threshold(float threshold);
      virtual float threshold() const;

     private:
      gr::blocks::plateau_detector_fb::sptr d_plateau_detector;

    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_OFDM_SYNC_SC_CFB_IMPL_H */

