/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_ENERGY_DISPERSAL_H
#define INCLUDED_DTV_DVBT_ENERGY_DISPERSAL_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>

namespace gr {
namespace dtv {

/*!
 * \brief Energy dispersal.
 * \ingroup dtv
 *
 * ETSI EN 300 744 - Clause 4.3.1 \n
 * Input - MPEG-2 transport packets (including sync - 0x47). \n
 * Output - Randomized MPEG-2 transport packets. \n
 * If first byte is not a SYNC then look for it. \n
 * First sync in a row of 8 packets is reversed - 0xB8. \n
 * Block size is 188 bytes.
 */
class DTV_API dvbt_energy_dispersal : virtual public gr::block
{
public:
    typedef boost::shared_ptr<dvbt_energy_dispersal> sptr;

    /*!
     * \brief Create DVB-T energy dispersal.
     *
     * \param nsize number of blocks.
     */
    static sptr make(int nsize);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_ENERGY_DISPERSAL_H */
