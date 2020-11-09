/* -*- c++ -*- */
/*
 * Copyright 2004, 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RUNTIME_SYNC_DECIMATOR_H
#define INCLUDED_GR_RUNTIME_SYNC_DECIMATOR_H

#include <gnuradio/api.h>
#include <gnuradio/sync_block.h>

namespace gr {

/*!
 * \brief synchronous N:1 input to output with history
 * \ingroup base_blk
 *
 * Override work to provide the signal processing implementation.
 */
class GR_RUNTIME_API sync_decimator : public sync_block
{
private:
    unsigned d_decimation;

protected:
    sync_decimator(void) {} // allows pure virtual interface sub-classes
    sync_decimator(const std::string& name,
                   gr::io_signature::sptr input_signature,
                   gr::io_signature::sptr output_signature,
                   unsigned decimation);

public:
    unsigned decimation() const { return d_decimation; }
    void set_decimation(unsigned decimation)
    {
        d_decimation = decimation;
        set_relative_rate(1, (uint64_t)decimation);
    }

    // gr::sync_decimator overrides these to assist work
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

#endif /* INCLUDED_GR_RUNTIME_SYNC_DECIMATOR_H */
