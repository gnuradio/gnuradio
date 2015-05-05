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

#ifndef INCLUDED_DTV_DVB_BBSCRAMBLER_BB_IMPL_H
#define INCLUDED_DTV_DVB_BBSCRAMBLER_BB_IMPL_H

#include <gnuradio/dtv/dvb_bbscrambler_bb.h>
#include "dvb_defines.h"

namespace gr {
  namespace dtv {

    class dvb_bbscrambler_bb_impl : public dvb_bbscrambler_bb
    {
     private:
      unsigned int kbch;
      unsigned char bb_randomise[FRAME_SIZE_NORMAL];
      void init_bb_randomiser(void);

     public:
      dvb_bbscrambler_bb_impl(dvb_standard_t standard, dvb_framesize_t framesize, dvb_code_rate_t rate);
      ~dvb_bbscrambler_bb_impl();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVB_BBSCRAMBLER_BB_IMPL_H */

