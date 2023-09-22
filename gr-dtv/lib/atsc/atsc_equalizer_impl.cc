/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "atsc_equalizer_impl.h"
#include "atsc_pnXXX_impl.h"
#include "atsc_types.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <fstream>

namespace gr {
namespace dtv {

atsc_equalizer::sptr atsc_equalizer::make()
{
    return gnuradio::make_block_sptr<atsc_equalizer_impl>();
}

static float bin_map(int bit) { return bit ? +5 : -5; }

static void init_field_sync_common(float* p, int mask)
{
    int i = 0;

    p[i++] = bin_map(1); // data segment sync pulse
    p[i++] = bin_map(0);
    p[i++] = bin_map(0);
    p[i++] = bin_map(1);

    for (int j = 0; j < 511; j++) // PN511
        p[i++] = bin_map(atsc_pn511[j]);

    for (int j = 0; j < 63; j++) // PN63
        p[i++] = bin_map(atsc_pn63[j]);

    for (int j = 0; j < 63; j++) // PN63, toggled on field 2
        p[i++] = bin_map(atsc_pn63[j] ^ mask);

    for (int j = 0; j < 63; j++) // PN63
        p[i++] = bin_map(atsc_pn63[j]);
}

atsc_equalizer_impl::atsc_equalizer_impl()
    : gr::block("dtv_atsc_equalizer",
                io_signature::make2(
                    2, 2, ATSC_DATA_SEGMENT_LENGTH * sizeof(float), sizeof(plinfo)),
                io_signature::make2(
                    2, 2, ATSC_DATA_SEGMENT_LENGTH * sizeof(float), sizeof(plinfo)))
{
    init_field_sync_common(training_sequence1, 0);
    init_field_sync_common(training_sequence2, 1);

    d_taps.resize(NTAPS, 0.0f);

    const int alignment_multiple = volk_get_alignment() / sizeof(float);
    set_alignment(std::max(1, alignment_multiple));
}

atsc_equalizer_impl::~atsc_equalizer_impl() {}

std::vector<float> atsc_equalizer_impl::taps() const { return d_taps; }

std::vector<float> atsc_equalizer_impl::data() const
{
    std::vector<float> ret(&data_mem2[0], &data_mem2[ATSC_DATA_SEGMENT_LENGTH - 1]);
    return ret;
}

void atsc_equalizer_impl::filterN(const float* input_samples,
                                  float* output_samples,
                                  int nsamples)
{
    for (int j = 0; j < nsamples; j++) {
        output_samples[j] = 0;
        volk_32f_x2_dot_prod_32f(
            &output_samples[j], &input_samples[j], &d_taps[0], NTAPS);
    }
}

void atsc_equalizer_impl::adaptN(const float* input_samples,
                                 const float* training_pattern,
                                 float* output_samples,
                                 int nsamples)
{
    static const double BETA = 0.00005; // FIXME figure out what this ought to be
                                        // FIXME add gear-shifting

    for (int j = 0; j < nsamples; j++) {
        output_samples[j] = 0;
        volk_32f_x2_dot_prod_32f(
            &output_samples[j], &input_samples[j], &d_taps[0], NTAPS);

        float e = output_samples[j] - training_pattern[j];

        // update taps...
        float tmp_taps[NTAPS];
        volk_32f_s32f_multiply_32f(tmp_taps, &input_samples[j], BETA * e, NTAPS);
        volk_32f_x2_subtract_32f(&d_taps[0], &d_taps[0], tmp_taps, NTAPS);
    }
}

int atsc_equalizer_impl::general_work(int noutput_items,
                                      gr_vector_int& ninput_items,
                                      gr_vector_const_void_star& input_items,
                                      gr_vector_void_star& output_items)
{
    auto in = static_cast<const float*>(input_items[0]);
    auto out = static_cast<float*>(output_items[0]);
    auto in_pl = static_cast<const plinfo*>(input_items[1]);
    auto out_pl = static_cast<plinfo*>(output_items[1]);

    int output_produced = 0;
    int i = 0;

    std::vector<tag_t> tags;

    plinfo pli_in;
    if (d_buff_not_filled) {
        memset(&data_mem[0], 0, NPRETAPS * sizeof(float));
        memcpy(&data_mem[NPRETAPS],
               in + i * ATSC_DATA_SEGMENT_LENGTH,
               ATSC_DATA_SEGMENT_LENGTH * sizeof(float));

        d_flags = in_pl[i].flags();
        d_segno = in_pl[i].segno();

        d_buff_not_filled = false;
        i++;
    }

    for (; i < noutput_items; i++) {

        memcpy(&data_mem[ATSC_DATA_SEGMENT_LENGTH + NPRETAPS],
               in + i * ATSC_DATA_SEGMENT_LENGTH,
               (NTAPS - NPRETAPS) * sizeof(float));

        if (d_segno == -1) {
            if (d_flags & 0x0010) {
                adaptN(data_mem, training_sequence2, data_mem2, KNOWN_FIELD_SYNC_LENGTH);
            } else if (!(d_flags & 0x0010)) {
                adaptN(data_mem, training_sequence1, data_mem2, KNOWN_FIELD_SYNC_LENGTH);
            }
        } else {
            filterN(data_mem, data_mem2, ATSC_DATA_SEGMENT_LENGTH);

            memcpy(&out[output_produced * ATSC_DATA_SEGMENT_LENGTH],
                   data_mem2,
                   ATSC_DATA_SEGMENT_LENGTH * sizeof(float));

            plinfo pli_out(d_flags, d_segno);
            out_pl[output_produced++] = pli_out;
        }

        memcpy(data_mem, &data_mem[ATSC_DATA_SEGMENT_LENGTH], NPRETAPS * sizeof(float));
        memcpy(&data_mem[NPRETAPS],
               in + i * ATSC_DATA_SEGMENT_LENGTH,
               ATSC_DATA_SEGMENT_LENGTH * sizeof(float));

        d_flags = in_pl[i].flags();
        d_segno = in_pl[i].segno();
    }

    consume_each(noutput_items);
    return output_produced;
}

void atsc_equalizer_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<atsc_equalizer, std::vector<float>>(
            alias(),
            "taps",
            &atsc_equalizer::taps,
            pmt::make_f32vector(1, -10),
            pmt::make_f32vector(1, 10),
            pmt::make_f32vector(1, 0),
            "",
            "Equalizer Taps",
            RPC_PRIVLVL_MIN,
            DISPTIME)));

    add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<atsc_equalizer, std::vector<float>>(
            alias(),
            "data",
            &atsc_equalizer::data,
            pmt::make_f32vector(1, -10),
            pmt::make_f32vector(1, 10),
            pmt::make_f32vector(1, 0),
            "",
            "Post-equalizer Data",
            RPC_PRIVLVL_MIN,
            DISPTIME)));
#endif /* GR_CTRLPORT */
}

} /* namespace dtv */
} /* namespace gr */
