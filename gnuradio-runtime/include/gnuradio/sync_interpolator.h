/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RUNTIME_SYNC_INTERPOLATOR_H
#define INCLUDED_GR_RUNTIME_SYNC_INTERPOLATOR_H

#include <gnuradio/api.h>
#include <gnuradio/sync_block.h>

namespace gr {

/*!
 * \brief synchronous 1:N input to output with history
 * \ingroup base_blk
 *
 * Override work to provide the signal processing implementation.
 */
class GR_RUNTIME_API sync_interpolator : public sync_block
{
private:
    unsigned d_interpolation;

protected:
    sync_interpolator(void) {} // allows pure virtual interface sub-classes
    sync_interpolator(const std::string& name,
                      gr::io_signature::sptr input_signature,
                      gr::io_signature::sptr output_signature,
                      unsigned interpolation);

public:
    unsigned interpolation() const { return d_interpolation; }
    void set_interpolation(unsigned interpolation)
    {
        d_interpolation = interpolation;
        set_relative_rate((uint64_t)interpolation, 1);
        set_output_multiple(interpolation);
    }

    // gr::sync_interpolator overrides these to assist work
    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;
    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    // derived classes should override work

    int fixed_rate_ninput_to_noutput(int ninput) override;
    int fixed_rate_noutput_to_ninput(int noutput) override;
};

} /* namespace gr */

#endif /* INCLUDED_GR_RUNTIME_SYNC_INTERPOLATOR_H */
