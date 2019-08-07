/* -*- c++ -*- */
/*
 * Copyright 2015,2016,2018,2019 Free Software Foundation, Inc.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dvb_bch_bb_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace dtv {

dvb_bch_bb::sptr
dvb_bch_bb::make(dvb_standard_t standard, dvb_framesize_t framesize, dvb_code_rate_t rate)
{
    return gnuradio::get_initial_sptr(new dvb_bch_bb_impl(standard, framesize, rate));
}

/*
 * The private constructor
 */
dvb_bch_bb_impl::dvb_bch_bb_impl(dvb_standard_t standard,
                                 dvb_framesize_t framesize,
                                 dvb_code_rate_t rate)
    : gr::block("dvb_bch_bb",
                gr::io_signature::make(1, 1, sizeof(unsigned char)),
                gr::io_signature::make(1, 1, sizeof(unsigned char)))
{
    if (framesize == FECFRAME_NORMAL) {
        switch (rate) {
        case C1_4:
            kbch = 16008;
            nbch = 16200;
            bch_code = BCH_CODE_N12;
            break;
        case C1_3:
            kbch = 21408;
            nbch = 21600;
            bch_code = BCH_CODE_N12;
            break;
        case C2_5:
            kbch = 25728;
            nbch = 25920;
            bch_code = BCH_CODE_N12;
            break;
        case C1_2:
            kbch = 32208;
            nbch = 32400;
            bch_code = BCH_CODE_N12;
            break;
        case C3_5:
            kbch = 38688;
            nbch = 38880;
            bch_code = BCH_CODE_N12;
            break;
        case C2_3:
            kbch = 43040;
            nbch = 43200;
            bch_code = BCH_CODE_N10;
            break;
        case C3_4:
            kbch = 48408;
            nbch = 48600;
            bch_code = BCH_CODE_N12;
            break;
        case C4_5:
            kbch = 51648;
            nbch = 51840;
            bch_code = BCH_CODE_N12;
            break;
        case C5_6:
            kbch = 53840;
            nbch = 54000;
            bch_code = BCH_CODE_N10;
            break;
        case C8_9:
            kbch = 57472;
            nbch = 57600;
            bch_code = BCH_CODE_N8;
            break;
        case C9_10:
            kbch = 58192;
            nbch = 58320;
            bch_code = BCH_CODE_N8;
            break;
        case C2_9_VLSNR:
            kbch = 14208;
            nbch = 14400;
            bch_code = BCH_CODE_N12;
            break;
        case C13_45:
            kbch = 18528;
            nbch = 18720;
            bch_code = BCH_CODE_N12;
            break;
        case C9_20:
            kbch = 28968;
            nbch = 29160;
            bch_code = BCH_CODE_N12;
            break;
        case C90_180:
            kbch = 32208;
            nbch = 32400;
            bch_code = BCH_CODE_N12;
            break;
        case C96_180:
            kbch = 34368;
            nbch = 34560;
            bch_code = BCH_CODE_N12;
            break;
        case C11_20:
            kbch = 35448;
            nbch = 35640;
            bch_code = BCH_CODE_N12;
            break;
        case C100_180:
            kbch = 35808;
            nbch = 36000;
            bch_code = BCH_CODE_N12;
            break;
        case C104_180:
            kbch = 37248;
            nbch = 37440;
            bch_code = BCH_CODE_N12;
            break;
        case C26_45:
            kbch = 37248;
            nbch = 37440;
            bch_code = BCH_CODE_N12;
            break;
        case C18_30:
            kbch = 38688;
            nbch = 38880;
            bch_code = BCH_CODE_N12;
            break;
        case C28_45:
            kbch = 40128;
            nbch = 40320;
            bch_code = BCH_CODE_N12;
            break;
        case C23_36:
            kbch = 41208;
            nbch = 41400;
            bch_code = BCH_CODE_N12;
            break;
        case C116_180:
            kbch = 41568;
            nbch = 41760;
            bch_code = BCH_CODE_N12;
            break;
        case C20_30:
            kbch = 43008;
            nbch = 43200;
            bch_code = BCH_CODE_N12;
            break;
        case C124_180:
            kbch = 44448;
            nbch = 44640;
            bch_code = BCH_CODE_N12;
            break;
        case C25_36:
            kbch = 44808;
            nbch = 45000;
            bch_code = BCH_CODE_N12;
            break;
        case C128_180:
            kbch = 45888;
            nbch = 46080;
            bch_code = BCH_CODE_N12;
            break;
        case C13_18:
            kbch = 46608;
            nbch = 46800;
            bch_code = BCH_CODE_N12;
            break;
        case C132_180:
            kbch = 47328;
            nbch = 47520;
            bch_code = BCH_CODE_N12;
            break;
        case C22_30:
            kbch = 47328;
            nbch = 47520;
            bch_code = BCH_CODE_N12;
            break;
        case C135_180:
            kbch = 48408;
            nbch = 48600;
            bch_code = BCH_CODE_N12;
            break;
        case C140_180:
            kbch = 50208;
            nbch = 50400;
            bch_code = BCH_CODE_N12;
            break;
        case C7_9:
            kbch = 50208;
            nbch = 50400;
            bch_code = BCH_CODE_N12;
            break;
        case C154_180:
            kbch = 55248;
            nbch = 55440;
            bch_code = BCH_CODE_N12;
            break;
        default:
            kbch = 0;
            nbch = 0;
            bch_code = 0;
            break;
        }
    } else if (framesize == FECFRAME_SHORT) {
        switch (rate) {
        case C1_4:
            kbch = 3072;
            nbch = 3240;
            bch_code = BCH_CODE_S12;
            break;
        case C1_3:
            kbch = 5232;
            nbch = 5400;
            bch_code = BCH_CODE_S12;
            break;
        case C2_5:
            kbch = 6312;
            nbch = 6480;
            bch_code = BCH_CODE_S12;
            break;
        case C1_2:
            kbch = 7032;
            nbch = 7200;
            bch_code = BCH_CODE_S12;
            break;
        case C3_5:
            kbch = 9552;
            nbch = 9720;
            bch_code = BCH_CODE_S12;
            break;
        case C2_3:
            kbch = 10632;
            nbch = 10800;
            bch_code = BCH_CODE_S12;
            break;
        case C3_4:
            kbch = 11712;
            nbch = 11880;
            bch_code = BCH_CODE_S12;
            break;
        case C4_5:
            kbch = 12432;
            nbch = 12600;
            bch_code = BCH_CODE_S12;
            break;
        case C5_6:
            kbch = 13152;
            nbch = 13320;
            bch_code = BCH_CODE_S12;
            break;
        case C8_9:
            kbch = 14232;
            nbch = 14400;
            bch_code = BCH_CODE_S12;
            break;
        case C11_45:
            kbch = 3792;
            nbch = 3960;
            bch_code = BCH_CODE_S12;
            break;
        case C4_15:
            kbch = 4152;
            nbch = 4320;
            bch_code = BCH_CODE_S12;
            break;
        case C14_45:
            kbch = 4872;
            nbch = 5040;
            bch_code = BCH_CODE_S12;
            break;
        case C7_15:
            kbch = 7392;
            nbch = 7560;
            bch_code = BCH_CODE_S12;
            break;
        case C8_15:
            kbch = 8472;
            nbch = 8640;
            bch_code = BCH_CODE_S12;
            break;
        case C26_45:
            kbch = 9192;
            nbch = 9360;
            bch_code = BCH_CODE_S12;
            break;
        case C32_45:
            kbch = 11352;
            nbch = 11520;
            bch_code = BCH_CODE_S12;
            break;
        case C1_5_VLSNR_SF2:
            kbch = 2512;
            nbch = 2680;
            bch_code = BCH_CODE_S12;
            break;
        case C11_45_VLSNR_SF2:
            kbch = 3792;
            nbch = 3960;
            bch_code = BCH_CODE_S12;
            break;
        case C1_5_VLSNR:
            kbch = 3072;
            nbch = 3240;
            bch_code = BCH_CODE_S12;
            break;
        case C4_15_VLSNR:
            kbch = 4152;
            nbch = 4320;
            bch_code = BCH_CODE_S12;
            break;
        case C1_3_VLSNR:
            kbch = 5232;
            nbch = 5400;
            bch_code = BCH_CODE_S12;
            break;
        default:
            kbch = 0;
            nbch = 0;
            bch_code = 0;
            break;
        }
    } else {
        switch (rate) {
        case C1_5_MEDIUM:
            kbch = 5660;
            nbch = 5840;
            bch_code = BCH_CODE_M12;
            break;
        case C11_45_MEDIUM:
            kbch = 7740;
            nbch = 7920;
            bch_code = BCH_CODE_M12;
            break;
        case C1_3_MEDIUM:
            kbch = 10620;
            nbch = 10800;
            bch_code = BCH_CODE_M12;
            break;
        default:
            kbch = 0;
            nbch = 0;
            bch_code = 0;
            break;
        }
    }

    switch (bch_code) {
    case BCH_CODE_N12:
        num_parity_bits = 192;
        break;
    case BCH_CODE_N10:
        num_parity_bits = 160;
        break;
    case BCH_CODE_N8:
        num_parity_bits = 128;
        break;
    case BCH_CODE_M12:
        num_parity_bits = 180;
        break;
    case BCH_CODE_S12:
        num_parity_bits = 168;
        break;
    }

    bch_poly_build_tables();
    frame_size = framesize;
    set_output_multiple(nbch);
}

/*
 * Our virtual destructor.
 */
dvb_bch_bb_impl::~dvb_bch_bb_impl() {}

void dvb_bch_bb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = (noutput_items / nbch) * kbch;
}

/*
 * Polynomial calculation routines
 * multiply polynomials
 */
int dvb_bch_bb_impl::poly_mult(
    const int* ina, int lena, const int* inb, int lenb, int* out)
{
    memset(out, 0, sizeof(int) * (lena + lenb));

    for (int i = 0; i < lena; i++) {
        for (int j = 0; j < lenb; j++) {
            if (ina[i] * inb[j] > 0) {
                out[i + j]++; // count number of terms for this pwr of x
            }
        }
    }
    int max = 0;
    for (int i = 0; i < lena + lenb; i++) {
        out[i] = out[i] & 1; // If even ignore the term
        if (out[i]) {
            max = i;
        }
    }
    // return the size of array to house the result.
    return max + 1;
}

// precalculate the crc from:
// http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html - cf. CRC-32 Lookup
void dvb_bch_bb_impl::calculate_crc_table(void)
{
    for (int divident = 0; divident < 256;
         divident++) { /* iterate over all possible input byte values 0 - 255 */
        std::bitset<MAX_BCH_PARITY_BITS> curByte(divident);
        curByte <<= num_parity_bits - 8;

        for (unsigned char bit = 0; bit < 8; bit++) {
            if ((curByte[num_parity_bits - 1]) != 0) {
                curByte <<= 1;
                curByte ^= polynome;
            } else {
                curByte <<= 1;
            }
        }
        crc_table[divident] = curByte;
    }
}

void dvb_bch_bb_impl::calculate_medium_crc_table(void)
{
    for (int divident = 0; divident < 16;
         divident++) { /* iterate over all possible input byte values 0 - 15 */
        std::bitset<MAX_BCH_PARITY_BITS> curByte(divident);
        curByte <<= num_parity_bits - 4;

        for (unsigned char bit = 0; bit < 4; bit++) {
            if ((curByte[num_parity_bits - 1]) != 0) {
                curByte <<= 1;
                curByte ^= polynome;
            } else {
                curByte <<= 1;
            }
        }
        crc_medium_table[divident] = curByte;
    }
}

void dvb_bch_bb_impl::bch_poly_build_tables(void)
{
    // Normal polynomials
    const int polyn01[] = { 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
    const int polyn02[] = { 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 };
    const int polyn03[] = { 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1 };
    const int polyn04[] = { 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1 };
    const int polyn05[] = { 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1 };
    const int polyn06[] = { 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1 };
    const int polyn07[] = { 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1 };
    const int polyn08[] = { 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1 };
    const int polyn09[] = { 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1 };
    const int polyn10[] = { 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1 };
    const int polyn11[] = { 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1 };
    const int polyn12[] = { 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1 };

    // Medium polynomials
    const int polym01[] = { 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
    const int polym02[] = { 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1 };
    const int polym03[] = { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1 };
    const int polym04[] = { 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1 };
    const int polym05[] = { 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1 };
    const int polym06[] = { 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1 };
    const int polym07[] = { 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1 };
    const int polym08[] = { 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1 };
    const int polym09[] = { 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1 };
    const int polym10[] = { 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1 };
    const int polym11[] = { 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1 };
    const int polym12[] = { 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1 };

    // Short polynomials
    const int polys01[] = { 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
    const int polys02[] = { 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1 };
    const int polys03[] = { 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1 };
    const int polys04[] = { 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1 };
    const int polys05[] = { 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1 };
    const int polys06[] = { 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1 };
    const int polys07[] = { 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1 };
    const int polys08[] = { 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1 };
    const int polys09[] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1 };
    const int polys10[] = { 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1 };
    const int polys11[] = { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1 };
    const int polys12[] = { 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1 };

    int len;
    int polyout[2][200];

#define COPY_BCH_POLYNOME                                \
    for (unsigned int i = 0; i < num_parity_bits; i++) { \
        polynome[i] = polyout[0][i];                     \
    }

    switch (bch_code) {
    case BCH_CODE_N12:
    case BCH_CODE_N10:
    case BCH_CODE_N8:

        len = poly_mult(polyn01, 17, polyn02, 17, polyout[0]);
        len = poly_mult(polyn03, 17, polyout[0], len, polyout[1]);
        len = poly_mult(polyn04, 17, polyout[1], len, polyout[0]);
        len = poly_mult(polyn05, 17, polyout[0], len, polyout[1]);
        len = poly_mult(polyn06, 17, polyout[1], len, polyout[0]);
        len = poly_mult(polyn07, 17, polyout[0], len, polyout[1]);
        len = poly_mult(polyn08, 17, polyout[1], len, polyout[0]);
        if (bch_code == BCH_CODE_N8) {
            COPY_BCH_POLYNOME
        }

        len = poly_mult(polyn09, 17, polyout[0], len, polyout[1]);
        len = poly_mult(polyn10, 17, polyout[1], len, polyout[0]);
        if (bch_code == BCH_CODE_N10) {
            COPY_BCH_POLYNOME
        }

        len = poly_mult(polyn11, 17, polyout[0], len, polyout[1]);
        len = poly_mult(polyn12, 17, polyout[1], len, polyout[0]);
        if (bch_code == BCH_CODE_N12) {
            COPY_BCH_POLYNOME
        }
        break;

    case BCH_CODE_S12:
        len = poly_mult(polys01, 15, polys02, 15, polyout[0]);
        len = poly_mult(polys03, 15, polyout[0], len, polyout[1]);
        len = poly_mult(polys04, 15, polyout[1], len, polyout[0]);
        len = poly_mult(polys05, 15, polyout[0], len, polyout[1]);
        len = poly_mult(polys06, 15, polyout[1], len, polyout[0]);
        len = poly_mult(polys07, 15, polyout[0], len, polyout[1]);
        len = poly_mult(polys08, 15, polyout[1], len, polyout[0]);
        len = poly_mult(polys09, 15, polyout[0], len, polyout[1]);
        len = poly_mult(polys10, 15, polyout[1], len, polyout[0]);
        len = poly_mult(polys11, 15, polyout[0], len, polyout[1]);
        len = poly_mult(polys12, 15, polyout[1], len, polyout[0]);

        COPY_BCH_POLYNOME
        break;

    case BCH_CODE_M12:
        len = poly_mult(polym01, 16, polym02, 16, polyout[0]);
        len = poly_mult(polym03, 16, polyout[0], len, polyout[1]);
        len = poly_mult(polym04, 16, polyout[1], len, polyout[0]);
        len = poly_mult(polym05, 16, polyout[0], len, polyout[1]);
        len = poly_mult(polym06, 16, polyout[1], len, polyout[0]);
        len = poly_mult(polym07, 16, polyout[0], len, polyout[1]);
        len = poly_mult(polym08, 16, polyout[1], len, polyout[0]);
        len = poly_mult(polym09, 16, polyout[0], len, polyout[1]);
        len = poly_mult(polym10, 16, polyout[1], len, polyout[0]);
        len = poly_mult(polym11, 16, polyout[0], len, polyout[1]);
        len = poly_mult(polym12, 16, polyout[1], len, polyout[0]);

        COPY_BCH_POLYNOME
        break;
    }
    calculate_crc_table();
    calculate_medium_crc_table();
}

int dvb_bch_bb_impl::general_work(int noutput_items,
                                  gr_vector_int& ninput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];
    unsigned char b, temp, msb;

    // We can use a 192 bits long bitset, all higher bits not used by the bch will just be
    // ignored
    std::bitset<MAX_BCH_PARITY_BITS> parity_bits;
    int consumed = 0;

    if (frame_size != FECFRAME_MEDIUM) {
        for (int i = 0; i < noutput_items; i += nbch) {
            for (int j = 0; j < (int)kbch / 8; j++) {
                b = 0;

                // calculate the crc using the lookup table, cf.
                // http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html
                for (int e = 0; e < 8; e++) {
                    temp = *in++;
                    *out++ = temp;
                    consumed++;

                    b |= temp << (7 - e);
                }

                msb = 0;
                for (int n = 1; n <= 8; n++) {
                    temp = parity_bits[num_parity_bits - n];
                    msb |= temp << (8 - n);
                }
                /* XOR-in next input byte into MSB of crc and get this MSB, that's our new
                 * intermediate divident */
                unsigned char pos = (msb ^ b);
                /* Shift out the MSB used for division per lookuptable and XOR with the
                 * remainder */
                parity_bits = (parity_bits << 8) ^ crc_table[pos];
            }

            // Now add the parity bits to the output
            for (unsigned int n = 0; n < num_parity_bits; n++) {
                *out++ = (char)parity_bits[num_parity_bits - 1];
                parity_bits <<= 1;
            }
        }
    } else {
        for (int i = 0; i < noutput_items; i += nbch) {
            for (int j = 0; j < (int)kbch / 4; j++) {
                b = 0;

                // calculate the crc using the lookup table, cf.
                // http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html
                for (int e = 0; e < 4; e++) {
                    temp = *in++;
                    *out++ = temp;
                    consumed++;

                    b |= temp << (3 - e);
                }

                msb = 0;
                for (int n = 1; n <= 4; n++) {
                    temp = parity_bits[num_parity_bits - n];
                    msb |= temp << (4 - n);
                }
                /* XOR-in next input byte into MSB of crc and get this MSB, that's our new
                 * intermediate divident */
                unsigned char pos = (msb ^ b);
                /* Shift out the MSB used for division per lookuptable and XOR with the
                 * remainder */
                parity_bits = (parity_bits << 4) ^ crc_medium_table[pos];
            }

            // Now add the parity bits to the output
            for (unsigned int n = 0; n < num_parity_bits; n++) {
                *out++ = (char)parity_bits[num_parity_bits - 1];
                parity_bits <<= 1;
            }
        }
    }

    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each(consumed);

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace dtv */
} /* namespace gr */
