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
                io_signature::make(1, 1, ATSC_DATA_SEGMENT_LENGTH * sizeof(float)),
                io_signature::make2(
                    2, 2, ATSC_DATA_SEGMENT_LENGTH * sizeof(float), sizeof(plinfo)))
{
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
    auto in = static_cast<const float*>(input_items[0]);
    auto out = static_cast<float*>(output_items[0]);
    auto out_pl = static_cast<plinfo*>(output_items[1]);

    int output_produced = 0;

    for (int i = 0; i < noutput_items; i++) {
        // check for a hit on the PN 511 pattern
        int errors = 0;

        for (int j = 0; j < LENGTH_511 && errors < PN511_ERROR_LIMIT; j++)
            errors +=
                (in[i * ATSC_DATA_SEGMENT_LENGTH + j + OFFSET_511] >= 0) ^ atsc_pn511[j];

        GR_LOG_DEBUG(d_debug_logger,
                     std::string("second PN63 error count = ") + std::to_string(errors));

        if (errors < PN511_ERROR_LIMIT) { // 511 pattern is good.
            // determine if this is field 1 or field 2
            errors = 0;
            for (int j = 0; j < LENGTH_2ND_63; j++)
                errors += (in[i * ATSC_DATA_SEGMENT_LENGTH + j + OFFSET_2ND_63] >= 0) ^
                          atsc_pn63[j];

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
            memcpy(&out[output_produced * ATSC_DATA_SEGMENT_LENGTH],
                   &in[i * ATSC_DATA_SEGMENT_LENGTH],
                   ATSC_DATA_SEGMENT_LENGTH * sizeof(float));

            plinfo pli_out;
            pli_out.set_regular_seg((d_field_num == 2), d_segment_num);

            d_segment_num++;
            if (d_segment_num > (ATSC_SEGMENTS_PER_DATA_FIELD - 1)) {
                d_field_num = 0;
                d_segment_num = 0;
            } else {
                out_pl[output_produced++] = pli_out;
            }
        }
    }

    consume_each(noutput_items);
    return output_produced;
}

} /* namespace dtv */
} /* namespace gr */
