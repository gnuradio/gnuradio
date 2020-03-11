/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_DEINTERLEAVER_IMPL_H
#define INCLUDED_DTV_ATSC_DEINTERLEAVER_IMPL_H

#include "atsc_types.h"
#include "interleaver_fifo.h"
#include <gnuradio/dtv/atsc_deinterleaver.h>

namespace gr {
namespace dtv {

class atsc_deinterleaver_impl : public atsc_deinterleaver
{
private:
    //! transform a single symbol
    unsigned char transform(unsigned char input)
    {
        unsigned char retval = m_fifo[m_commutator]->stuff(input);
        m_commutator++;
        if (m_commutator >= 52)
            m_commutator = 0;
        return retval;
    }

    /*!
     * Note: The use of the alignment_fifo keeps the encoder and decoder
     * aligned if both are synced to a field boundary.  There may be other
     * ways to implement this function.  This is a best guess as to how
     * this should behave, as we have no test vectors for either the
     * interleaver or deinterleaver.
     */
    interleaver_fifo<unsigned char> alignment_fifo;

    int m_commutator;
    std::vector<interleaver_fifo<unsigned char>*> m_fifo;

public:
    atsc_deinterleaver_impl();
    ~atsc_deinterleaver_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    //! reset interleaver (flushes contents and resets commutator)
    void reset();

    //! sync interleaver (resets commutator, but doesn't flush fifos)
    void sync() { m_commutator = 0; }
};

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_DEINTERLEAVER_IMPL_H */
