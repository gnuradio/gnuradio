/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CORRECTIQ_CORRECTIQ_MAN_IMPL_H
#define INCLUDED_CORRECTIQ_CORRECTIQ_MAN_IMPL_H

#include <gnuradio/blocks/correctiq_man.h>
#include <volk/volk_alloc.hh>

namespace gr {
namespace blocks {

class correctiq_man_impl : public correctiq_man
{
private:
    gr_complex d_k;

    volk::vector<gr_complex> d_volk_const_buffer;

    void set_const_buffer(int new_size);
    void fill_const_buffer();

public:
    correctiq_man_impl(float real, float imag);
    ~correctiq_man_impl();

    float get_real() const override;
    float get_imag() const override;

    virtual void set_real(float new_value);
    virtual void set_imag(float new_value);

    void handle_real(pmt::pmt_t msg);
    void handle_imag(pmt::pmt_t msg);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_CORRECTIQ_CORRECTIQ_MAN_IMPL_H */
