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

#ifndef INCLUDED_DTV_DVBT2_CELLINTERLEAVER_CC_IMPL_H
#define INCLUDED_DTV_DVBT2_CELLINTERLEAVER_CC_IMPL_H

#include <gnuradio/dtv/dvbt2_cellinterleaver_cc.h>
#include "dvb/dvb_defines.h"

namespace gr {
  namespace dtv {

    class dvbt2_cellinterleaver_cc_impl : public dvbt2_cellinterleaver_cc
    {
     private:
      int cell_size;
      int pn_degree;
      int ti_blocks;
      int fec_blocks;
      int permutations[32768];
      int FECBlocksPerSmallTIBlock;
      int FECBlocksPerBigTIBlock;
      int numBigTIBlocks;
      int numSmallTIBlocks;
      int interleaved_items;
      gr_complex *time_interleave;
      gr_complex **cols;

     public:
      dvbt2_cellinterleaver_cc_impl(dvb_framesize_t framesize, dvb_constellation_t constellation, int fecblocks, int tiblocks);
      ~dvbt2_cellinterleaver_cc_impl();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT2_CELLINTERLEAVER_CC_IMPL_H */

