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

#ifndef INCLUDED_DTV_ATSC_FIELD_SYNC_MUX_IMPL_H
#define INCLUDED_DTV_ATSC_FIELD_SYNC_MUX_IMPL_H

#include <gnuradio/dtv/atsc_field_sync_mux.h>
#include "atsc_pnXXX_impl.h"
#include "atsc_types.h"

namespace gr {
  namespace dtv {

    class atsc_field_sync_mux_impl : public atsc_field_sync_mux
    {
    private:
      static const int N_SAVED_SYMBOLS = 12;
      bool d_already_output_field_sync;
      unsigned char d_saved_symbols[N_SAVED_SYMBOLS];

      void init_field_sync_common(unsigned char *p, int mask, const unsigned char saved_symbols[N_SAVED_SYMBOLS]);
      inline void init_field_sync_1(atsc_data_segment *s, const unsigned char saved_symbols[N_SAVED_SYMBOLS]);
      inline void init_field_sync_2(atsc_data_segment *s, const unsigned char saved_symbols[N_SAVED_SYMBOLS]);
      void save_last_symbols(unsigned char saved_symbols[N_SAVED_SYMBOLS], const atsc_data_segment &seg);
      inline bool last_regular_seg_p(const plinfo &pli);

    public:
      atsc_field_sync_mux_impl();
      ~atsc_field_sync_mux_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_ATSC_FIELD_SYNC_MUX_IMPL_H */
