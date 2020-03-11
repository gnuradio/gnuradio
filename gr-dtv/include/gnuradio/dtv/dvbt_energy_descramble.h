/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_ENERGY_DESCRAMBLE_H
#define INCLUDED_DTV_DVBT_ENERGY_DESCRAMBLE_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>

namespace gr {
namespace dtv {

/*!
 * \brief Energy descramble
 * \ingroup dtv
 *
 * ETSI EN 300 744 - Clause 4.3.1. \n
 * Input - Randomized MPEG-2 transport packets. \n
 * Output - MPEG-2 transport packets (including sync - 0x47). \n
 * We assume the first byte is a NSYNC. \n
 * First sync in a row of 8 packets is reversed - 0xB8. \n
 * Block size is 188 bytes
 */
class DTV_API dvbt_energy_descramble : virtual public block
{
public:
    typedef boost::shared_ptr<dvbt_energy_descramble> sptr;

    /*!
     * \brief Create DVB-T Energy descramble.
     *
     * \param nblocks number of blocks.
     */
    static sptr make(int nblocks);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_ENERGY_DESCRAMBLE_H */
