/* -*- c++ -*- */
/*
 * Copyright 2015,2016 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVB_LDPC_BB_IMPL_H
#define INCLUDED_DTV_DVB_LDPC_BB_IMPL_H

#include "dvb_defines.h"

#include <gnuradio/dtv/dvb_ldpc_bb.h>

namespace gr {
namespace dtv {

class dvb_ldpc_bb_impl : public dvb_ldpc_bb
{
private:
    unsigned int frame_size;
    unsigned int frame_size_real;
    unsigned int frame_size_type;
    unsigned int signal_constellation;
    unsigned int nbch;
    unsigned int code_rate;
    unsigned int q_val;
    unsigned int dvb_standard;
    int Xs;
    int P;
    int Xp;
    unsigned char puncturing_buffer[FRAME_SIZE_NORMAL];
    unsigned char shortening_buffer[FRAME_SIZE_NORMAL];
    int ldpc_lut_index[FRAME_SIZE_NORMAL];
    void ldpc_lookup_generate(void);

    std::vector<uint16_t*> ldpc_lut; // Pointers into ldpc_lut_data.
    std::vector<uint16_t> ldpc_lut_data;

    template <typename entry_t, size_t rows, size_t cols>
    void ldpc_bf(entry_t (&table)[rows][cols])
    {
        size_t max_lut_arraysize = 0;
        const unsigned int pbits = (frame_size_real + Xp) - nbch;
        const unsigned int q = q_val;
        for (auto& row : table) { /* count the entries in the table */
            max_lut_arraysize += row[0];
        }

        max_lut_arraysize *= 360;   /* 360 bits per table entry */
        max_lut_arraysize /= pbits; /* spread over all parity bits */

        for (auto& ldpc_lut_index_entry : ldpc_lut_index) {
            ldpc_lut_index_entry = 1;
        }

        uint16_t max_index = 0;
        for (unsigned int row = 0; row < rows; row++) {
            for (unsigned int n = 0; n < 360; n++) {
                for (unsigned int col = 1; col <= table[row][0]; col++) {
                    unsigned int current_pbit = (table[row][col] + (n * q)) % pbits;
                    ldpc_lut_index[current_pbit]++;
                    if (ldpc_lut_index[current_pbit] > max_index) {
                        max_index = ldpc_lut_index[current_pbit];
                    }
                }
            }
        }
        max_lut_arraysize +=
            1 + (max_index -
                 max_lut_arraysize); /* 1 for the size at the start of the array */

        /* Allocate a 2D Array with pbits * max_lut_arraysize
         * while preserving two-subscript access
         * see
         * https://stackoverflow.com/questions/29375797/copy-2d-array-using-memcpy/29375830#29375830
         */
        ldpc_lut.resize(pbits);
        ldpc_lut_data.resize(pbits * max_lut_arraysize);
        ldpc_lut_data[0] = 1;
        ldpc_lut[0] = ldpc_lut_data.data();
        for (unsigned int i = 1; i < pbits; i++) {
            ldpc_lut[i] = ldpc_lut[i - 1] + max_lut_arraysize;
            ldpc_lut[i][0] = 1;
        }
        uint16_t im = 0;
        for (unsigned int row = 0; row < rows; row++) {
            for (unsigned int n = 0; n < 360; n++) {
                for (unsigned int col = 1; col <= table[row][0]; col++) {
                    unsigned int current_pbit = (table[row][col] + (n * q)) % pbits;
                    ldpc_lut[current_pbit][ldpc_lut[current_pbit][0]] = im;
                    ldpc_lut[current_pbit][0]++;
                }
                im++;
            }
        }
    }

    const static uint16_t ldpc_tab_1_4N[45][13];
    const static uint16_t ldpc_tab_1_3N[60][13];
    const static uint16_t ldpc_tab_2_5N[72][13];
    const static uint16_t ldpc_tab_1_2N[90][9];
    const static uint16_t ldpc_tab_3_5N[108][13];
    const static uint16_t ldpc_tab_2_3N_DVBT2[120][14];
    const static uint16_t ldpc_tab_2_3N_DVBS2[120][14];
    const static uint16_t ldpc_tab_3_4N[135][13];
    const static uint16_t ldpc_tab_4_5N[144][12];
    const static uint16_t ldpc_tab_5_6N[150][14];
    const static uint16_t ldpc_tab_8_9N[160][5];
    const static uint16_t ldpc_tab_9_10N[162][5];

    const static uint16_t ldpc_tab_1_4S[9][13];
    const static uint16_t ldpc_tab_1_3S[15][13];
    const static uint16_t ldpc_tab_2_5S[18][13];
    const static uint16_t ldpc_tab_1_2S[20][9];
    const static uint16_t ldpc_tab_3_5S_DVBT2[27][13];
    const static uint16_t ldpc_tab_3_5S_DVBS2[27][13];
    const static uint16_t ldpc_tab_2_3S[30][14];
    const static uint16_t ldpc_tab_3_4S[33][13];
    const static uint16_t ldpc_tab_4_5S[35][4];
    const static uint16_t ldpc_tab_5_6S[37][14];
    const static uint16_t ldpc_tab_8_9S[40][5];

    const static uint16_t ldpc_tab_2_9N[40][12];
    const static uint16_t ldpc_tab_13_45N[52][13];
    const static uint16_t ldpc_tab_9_20N[81][13];
    const static uint16_t ldpc_tab_11_20N[99][14];
    const static uint16_t ldpc_tab_26_45N[104][14];
    const static uint16_t ldpc_tab_28_45N[112][12];
    const static uint16_t ldpc_tab_23_36N[115][12];
    const static uint16_t ldpc_tab_25_36N[125][12];
    const static uint16_t ldpc_tab_13_18N[130][11];
    const static uint16_t ldpc_tab_7_9N[140][13];
    const static uint16_t ldpc_tab_90_180N[90][19];
    const static uint16_t ldpc_tab_96_180N[96][21];
    const static uint16_t ldpc_tab_100_180N[100][17];
    const static uint16_t ldpc_tab_104_180N[104][19];
    const static uint16_t ldpc_tab_116_180N[116][19];
    const static uint16_t ldpc_tab_124_180N[124][17];
    const static uint16_t ldpc_tab_128_180N[128][16];
    const static uint16_t ldpc_tab_132_180N[132][16];
    const static uint16_t ldpc_tab_135_180N[135][15];
    const static uint16_t ldpc_tab_140_180N[140][16];
    const static uint16_t ldpc_tab_154_180N[154][14];
    const static uint16_t ldpc_tab_18_30N[108][20];
    const static uint16_t ldpc_tab_20_30N[120][17];
    const static uint16_t ldpc_tab_22_30N[132][16];

    const static uint16_t ldpc_tab_11_45S[11][11];
    const static uint16_t ldpc_tab_4_15S[12][22];
    const static uint16_t ldpc_tab_14_45S[14][13];
    const static uint16_t ldpc_tab_7_15S[21][25];
    const static uint16_t ldpc_tab_8_15S[24][22];
    const static uint16_t ldpc_tab_26_45S[26][14];
    const static uint16_t ldpc_tab_32_45S[32][13];

    const static uint16_t ldpc_tab_1_5M[18][14];
    const static uint16_t ldpc_tab_11_45M[22][11];
    const static uint16_t ldpc_tab_1_3M[30][13];

public:
    dvb_ldpc_bb_impl(dvb_standard_t standard,
                     dvb_framesize_t framesize,
                     dvb_code_rate_t rate,
                     dvb_constellation_t constellation);
    ~dvb_ldpc_bb_impl() override;

    // Disallow copy/move because of the raw pointers.
    dvb_ldpc_bb_impl(const dvb_ldpc_bb_impl&) = delete;
    dvb_ldpc_bb_impl(dvb_ldpc_bb_impl&&) = delete;
    dvb_ldpc_bb_impl& operator=(const dvb_ldpc_bb_impl&) = delete;
    dvb_ldpc_bb_impl& operator=(dvb_ldpc_bb_impl&&) = delete;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVB_LDPC_BB_IMPL_H */
