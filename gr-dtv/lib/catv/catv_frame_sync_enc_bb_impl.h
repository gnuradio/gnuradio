/* -*- c++ -*- */
/*
 * Copyright 2016,2017 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_CATV_FRAME_SYNC_ENC_BB_IMPL_H
#define INCLUDED_DTV_CATV_FRAME_SYNC_ENC_BB_IMPL_H

#include <gnuradio/dtv/catv_frame_sync_enc_bb.h>

namespace gr {
namespace dtv {

class catv_frame_sync_enc_bb_impl : public catv_frame_sync_enc_bb
{
private:
    int control_word;
    int signal_constellation;

public:
    catv_frame_sync_enc_bb_impl(catv_constellation_t constellation, int ctrlword);
    ~catv_frame_sync_enc_bb_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_CATV_FRAME_SYNC_ENC_BB_IMPL_H */
