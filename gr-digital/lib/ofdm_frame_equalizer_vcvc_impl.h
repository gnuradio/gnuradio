/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_OFDM_FRAME_EQUALIZER_VCVC_IMPL_H
#define INCLUDED_DIGITAL_OFDM_FRAME_EQUALIZER_VCVC_IMPL_H

#include <gnuradio/digital/ofdm_frame_equalizer_vcvc.h>

namespace gr {
namespace digital {

class ofdm_frame_equalizer_vcvc_impl : public ofdm_frame_equalizer_vcvc
{
private:
    const int d_fft_len;
    const int d_cp_len;
    ofdm_equalizer_base::sptr d_eq;
    bool d_propagate_channel_state;
    const int d_fixed_frame_len;
    std::vector<gr_complex> d_channel_state;

protected:
    void parse_length_tags(const std::vector<std::vector<tag_t>>& tags,
                           gr_vector_int& n_input_items_reqd);

public:
    ofdm_frame_equalizer_vcvc_impl(ofdm_equalizer_base::sptr equalizer,
                                   int cp_len,
                                   const std::string& len_tag_key,
                                   bool propagate_channel_state,
                                   int fixed_frame_len);
    ~ofdm_frame_equalizer_vcvc_impl();

    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_OFDM_FRAME_EQUALIZER_VCVC_IMPL_H */
