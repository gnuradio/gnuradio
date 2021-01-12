/* -*- c++ -*- */
/*
 * Copyright 2014,2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "atsc_fs_checker_impl.h"
#include "atsc_pnXXX_impl.h"
#include "atsc_syminfo_impl.h"
#include "atsc_types.h"
#include "gnuradio/dtv/atsc_consts.h"
#include <gnuradio/io_signature.h>
#include <string>

#define ATSC_SEGMENTS_PER_DATA_FIELD 313

static const int PN511_ERROR_LIMIT = 20; // max number of bits wrong
static const int PN63_ERROR_LIMIT = 5;

namespace gr {
namespace dtv {

atsc_fs_checker::sptr atsc_fs_checker::make()
{
    return gnuradio::make_block_sptr<atsc_fs_checker_impl>();
}

atsc_fs_checker_impl::atsc_fs_checker_impl()
    : gr::block("dtv_atsc_fs_checker",
                io_signature::make(1, 1, sizeof(atsc_soft_data_segment)),
                io_signature::make(1, 1, sizeof(atsc_soft_data_segment)))
{
    gr::configure_default_loggers(d_logger, d_debug_logger, "dtv_atsc_fs_checker");
    reset();
}

void atsc_fs_checker_impl::reset()
{
    d_index = 0;
    memset(d_sample_sr, 0, sizeof(d_sample_sr));
    memset(d_tag_sr, 0, sizeof(d_tag_sr));
    memset(d_bit_sr, 0, sizeof(d_bit_sr));
    d_field_num = 0;
    d_segment_num = 0;
}

atsc_fs_checker_impl::~atsc_fs_checker_impl() {}

int atsc_fs_checker_impl::general_work(int noutput_items,
                                       gr_vector_int& ninput_items,
                                       gr_vector_const_void_star& input_items,
                                       gr_vector_void_star& output_items)
{
    const atsc_soft_data_segment* in = (const atsc_soft_data_segment*)input_items[0];
    atsc_soft_data_segment* out = (atsc_soft_data_segment*)output_items[0];

    int output_produced = 0;

    for (int i = 0; i < noutput_items; i++) {
        // check for a hit on the PN 511 pattern
        int errors = 0;

        for (int j = 0; j < LENGTH_511 && errors < PN511_ERROR_LIMIT; j++)
            errors += (in[i].data[j + OFFSET_511] >= 0) ^ atsc_pn511[j];

        GR_LOG_DEBUG(d_debug_logger,
                     std::string("second PN63 error count = ") + std::to_string(errors));

        if (errors < PN511_ERROR_LIMIT) { // 511 pattern is good.
            // determine if this is field 1 or field 2
            errors = 0;
            for (int j = 0; j < LENGTH_2ND_63; j++)
                errors += (in[i].data[j + OFFSET_2ND_63] >= 0) ^ atsc_pn63[j];

            // we should have either field 1 (== PN63) or field 2 (== ~PN63)
            if (errors <= PN63_ERROR_LIMIT) {
                GR_LOG_DEBUG(d_debug_logger, "Found FIELD_SYNC_1")
                d_field_num = 1;    // We are in field number 1 now
                d_segment_num = -1; // This is the first segment
            } else if (errors >= (LENGTH_2ND_63 - PN63_ERROR_LIMIT)) {
                GR_LOG_DEBUG(d_debug_logger, "Found FIELD_SYNC_2")
                d_field_num = 2;    // We are in field number 2 now
                d_segment_num = -1; // This is the first segment
            } else {
                // should be extremely rare.
                GR_LOG_WARN(d_logger,
                            std::string("PN63 error count = ") + std::to_string(errors));
            }
        }

        if (d_field_num == 1 || d_field_num == 2) { // If we have sync
            // So we copy out current packet data to an output packet and fill its plinfo
            for (int j = 0; j < ATSC_DATA_SEGMENT_LENGTH; j++)
                out[output_produced].data[j] = in[i].data[j];
            out[output_produced].pli.set_regular_seg((d_field_num == 2), d_segment_num);
            d_segment_num++;
            if (d_segment_num > (ATSC_SEGMENTS_PER_DATA_FIELD - 1)) {
                d_field_num = 0;
                d_segment_num = 0;
            } else {
                output_produced++;
            }
        }
    }

    consume_each(noutput_items);
    return output_produced;
}

} /* namespace dtv */
} /* namespace gr */
