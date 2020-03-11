/* -*- c++ -*- */
/*
 * Copyright 2015,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef BURST_SHAPER_IMPL_H
#define BURST_SHAPER_IMPL_H

#include <gnuradio/digital/burst_shaper.h>

namespace gr {
namespace digital {

template <class T>
class burst_shaper_impl : public burst_shaper<T>
{
protected:
    enum state_t {
        STATE_WAIT,
        STATE_PREPAD,
        STATE_RAMPUP,
        STATE_COPY,
        STATE_RAMPDOWN,
        STATE_POSTPAD
    };

private:
    const std::vector<T> d_up_ramp;
    const std::vector<T> d_down_ramp;
    const int d_nprepad;
    const int d_npostpad;
    const bool d_insert_phasing;
    const pmt::pmt_t d_length_tag_key;
    std::vector<T> d_up_phasing;
    std::vector<T> d_down_phasing;
    int d_ncopy;
    int d_limit;
    int d_index;
    uint64_t d_length_tag_offset;
    bool d_finished;
    state_t d_state;

    void write_padding(T*& dst, int& nwritten, int nspace);
    void copy_items(T*& dst, const T*& src, int& nwritten, int& nread, int nspace);
    void apply_ramp(T*& dst, const T*& src, int& nwritten, int& nread, int nspace);
    void add_length_tag(int offset);
    void propagate_tags(int in_offset, int out_offset, int count, bool skip = true);
    void enter_wait();
    void enter_prepad();
    void enter_rampup();
    void enter_copy();
    void enter_rampdown();
    void enter_postpad();

public:
    burst_shaper_impl(const std::vector<T>& taps,
                      int pre_padding,
                      int post_padding,
                      bool insert_phasing,
                      const std::string& length_tag_name);
    ~burst_shaper_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
    int pre_padding() const { return d_nprepad; }
    int post_padding() const { return d_npostpad; }
    int prefix_length() const;
    int suffix_length() const;
};

} // namespace digital
} // namespace gr

#endif /* BURST_SHAPER_IMPL_H */
