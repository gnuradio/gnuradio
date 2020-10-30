/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RUNTIME_SYNC_BLOCK_H
#define INCLUDED_GR_RUNTIME_SYNC_BLOCK_H

#include <gnuradio/api.h>
#include <gnuradio/block.h>

namespace gr {

/*!
 * \brief synchronous 1:1 input to output with history
 * \ingroup base_blk
 *
 * Override work to provide the signal processing implementation.
 */
class GR_RUNTIME_API sync_block : public block
{
protected:
    sync_block(void) {} // allows pure virtual interface sub-classes
    sync_block(const std::string& name,
               gr::io_signature::sptr input_signature,
               gr::io_signature::sptr output_signature);

public:
    /*!
     * \brief just like gr::block::general_work, only this arranges to
     * call consume_each for you
     *
     * The user must override work to define the signal processing code
     */
    virtual int work(int noutput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) = 0;

    // gr::sync_block overrides these to assist work
    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;
    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    int fixed_rate_ninput_to_noutput(int ninput) override;
    int fixed_rate_noutput_to_ninput(int noutput) override;
};

} /* namespace gr */

#endif /* INCLUDED_GR_RUNTIME_SYNC_BLOCK_H */
