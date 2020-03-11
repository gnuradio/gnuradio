/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_FIELD_SYNC_MUX_IMPL_H
#define INCLUDED_DTV_ATSC_FIELD_SYNC_MUX_IMPL_H

#include "atsc_pnXXX_impl.h"
#include "atsc_types.h"
#include <gnuradio/dtv/atsc_field_sync_mux.h>

namespace gr {
namespace dtv {

class atsc_field_sync_mux_impl : public atsc_field_sync_mux
{
private:
    static constexpr int N_SAVED_SYMBOLS = 12;
    bool d_already_output_field_sync;
    unsigned char d_saved_symbols[N_SAVED_SYMBOLS];

    void init_field_sync_common(unsigned char* p,
                                int mask,
                                const unsigned char saved_symbols[N_SAVED_SYMBOLS]);
    inline void init_field_sync_1(atsc_data_segment* s,
                                  const unsigned char saved_symbols[N_SAVED_SYMBOLS]);
    inline void init_field_sync_2(atsc_data_segment* s,
                                  const unsigned char saved_symbols[N_SAVED_SYMBOLS]);
    void save_last_symbols(unsigned char saved_symbols[N_SAVED_SYMBOLS],
                           const atsc_data_segment& seg);
    inline bool last_regular_seg_p(const plinfo& pli);

public:
    atsc_field_sync_mux_impl();
    ~atsc_field_sync_mux_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_ATSC_FIELD_SYNC_MUX_IMPL_H */
