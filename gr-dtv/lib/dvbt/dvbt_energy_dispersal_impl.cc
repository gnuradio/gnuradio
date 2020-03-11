/* -*- c++ -*- */
/*
 * Copyright 2015,2016 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dvbt_energy_dispersal_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace dtv {

const int dvbt_energy_dispersal_impl::d_npacks = 8;
const int dvbt_energy_dispersal_impl::d_psize = 188;
const int dvbt_energy_dispersal_impl::d_SYNC = 0x47;
const int dvbt_energy_dispersal_impl::d_NSYNC = 0xB8;

void dvbt_energy_dispersal_impl::init_prbs() { d_reg = 0xa9; }

int dvbt_energy_dispersal_impl::clock_prbs(int clocks)
{
    int res = 0;
    int feedback = 0;

    for (int i = 0; i < clocks; i++) {
        feedback = ((d_reg >> (14 - 1)) ^ (d_reg >> (15 - 1))) & 0x1;
        d_reg = ((d_reg << 1) | feedback) & 0x7fff;
        res = (res << 1) | feedback;
    }
    return res;
}

dvbt_energy_dispersal::sptr dvbt_energy_dispersal::make(int nblocks)
{
    return gnuradio::get_initial_sptr(new dvbt_energy_dispersal_impl(nblocks));
}

/*
 * The private constructor
 */
dvbt_energy_dispersal_impl::dvbt_energy_dispersal_impl(int nblocks)
    : gr::block("dvbt_energy_dispersal",
                gr::io_signature::make(1, 1, sizeof(unsigned char)),
                gr::io_signature::make(
                    1, 1, sizeof(unsigned char) * nblocks * d_npacks * d_psize)),
      d_nblocks(nblocks),
      d_reg(0xa9)
{
    set_relative_rate(1, (uint64_t)(d_nblocks * d_npacks * d_psize));
}

/*
 * Our virtual destructor.
 */
dvbt_energy_dispersal_impl::~dvbt_energy_dispersal_impl() {}

void dvbt_energy_dispersal_impl::forecast(int noutput_items,
                                          gr_vector_int& ninput_items_required)
{
    // Add one block size for SYNC search
    ninput_items_required[0] = d_npacks * (d_psize + 1) * d_nblocks * noutput_items;
}

int dvbt_energy_dispersal_impl::general_work(int noutput_items,
                                             gr_vector_int& ninput_items,
                                             gr_vector_const_void_star& input_items,
                                             gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    int index = 0;
    int count = 0;
    int ret = 0;
    int is_sync = 0;

    // Search for SYNC byte
    while (is_sync == 0 && index < d_psize) {
        if (in[index] == d_SYNC) {
            is_sync = 1;
        } else {
            index++;
        }
    }

    // If we found a SYNC byte
    if (is_sync) {
        for (int i = 0; i < (d_nblocks * noutput_items); i++) {
            init_prbs();

            int sync = d_NSYNC;

            for (int j = 0; j < d_npacks; j++) {
                if (in[index + count] != d_SYNC) {
                    GR_LOG_WARN(d_logger, "Malformed MPEG-TS!");
                }

                out[count++] = sync;

                for (int k = 1; k < d_psize; k++) {
                    out[count] = in[index + count] ^ clock_prbs(d_npacks);
                    count++;
                }

                sync = d_SYNC;
                clock_prbs(d_npacks);
            }
        }
        consume_each(index + d_npacks * d_psize * d_nblocks * noutput_items);
        ret = noutput_items;
    } else {
        consume_each(index);
        ret = 0;
    }

    // Tell runtime system how many output items we produced.
    return ret;
}
} /* namespace dtv */
} /* namespace gr */
