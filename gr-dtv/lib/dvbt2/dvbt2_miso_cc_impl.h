/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DTV_DVBT2_MISO_CC_IMPL_H
#define INCLUDED_DTV_DVBT2_MISO_CC_IMPL_H

#include <gnuradio/dtv/dvbt2_miso_cc.h>
#include "dvb/dvb_defines.h"

namespace gr {
  namespace dtv {

    class dvbt2_miso_cc_impl : public dvbt2_miso_cc
    {
     private:
      int miso_items;
      int N_P2;
      int C_P2;
      int N_FC;
      int C_FC;
      int C_DATA;

     public:
      dvbt2_miso_cc_impl(dvbt2_extended_carrier_t carriermode, dvbt2_fftsize_t fftsize, dvbt2_pilotpattern_t pilotpattern, dvb_guardinterval_t guardinterval, int numdatasyms, dvbt2_papr_t paprmode);
      ~dvbt2_miso_cc_impl();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT2_MISO_CC_IMPL_H */

