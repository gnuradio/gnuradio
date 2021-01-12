/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_DESCRAMBLER_BB_H
#define INCLUDED_GR_DESCRAMBLER_BB_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace digital {

/*!
 * \brief Descramber an input stream using an LFSR.
 * \ingroup coding_blk
 *
 * \details
 * Descramble an input stream using an LFSR. This block works on
 * the LSB only of the input data stream, i.e., on an "unpacked
 * binary" stream, and produces the same format on its output.
 */
class DIGITAL_API descrambler_bb : virtual public sync_block
{
public:
    // gr::digital::descrambler_bb::sptr
    typedef std::shared_ptr<descrambler_bb> sptr;

    /*!
     * \brief Make a descrambler block.
     *
     * \param mask     Polynomial mask for LFSR
     * \param seed     Initial shift register contents
     * \param len      Shift register length
     */
    static sptr make(uint64_t mask, uint64_t seed, uint8_t len);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_DESCRAMBLER_BB_H */
