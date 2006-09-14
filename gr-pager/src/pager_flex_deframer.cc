/*
 * Copyright 2004,2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pager_flex_deframer.h>
#include <pageri_flex_modes.h>
#include <gr_io_signature.h>
#include <gr_count_bits.h>

pager_flex_deframer_sptr pager_make_flex_deframer(int rate)
{
    return pager_flex_deframer_sptr(new pager_flex_deframer(rate));
}

// FLEX deframer block takes input from symbol stream with alphabet [0, 1, 2, 3]
// at specified channel rate and and outputs deinterleaved 32-bit FLEX code words 
// at 50, 100, or 200 code words per second (based on detected mode in sync
// word).

pager_flex_deframer::pager_flex_deframer(int rate) :
    gr_block ("flex_deframer",
              gr_make_io_signature (1, 1, sizeof(unsigned char)),
              gr_make_io_signature (1, 1, sizeof(gr_int32))),
    d_sync(rate/1600) // Maximum samples per baud
{
    d_rate = rate;
    set_output_multiple(1);
    enter_idle();
}

void pager_flex_deframer::forecast(int noutput_items, gr_vector_int &inputs_required)
{
    // samples per bit X 32 bits * number of outputs needed
    int items = noutput_items*sizeof(gr_int32)*8*d_spb;
    for (unsigned int i = 0; i < inputs_required.size(); i++)
        inputs_required[i] = items;
}

int pager_flex_deframer::index_avg(int start, int end)
{
    // modulo average
    if (start < end)
        return (end + start)/2;
    else
        return ((end + start)/2 + d_spb/2) % d_spb;
}

bool pager_flex_deframer::test_sync(unsigned char sym)
{
    // 64-bit FLEX sync code:
    // AAAA:BBBBBBBB:CCCC
    //
    // Where BBBBBBBB is always 0xA6C6AAAA
    // and AAAA^CCCC is 0xFFFF
    // 
    // Specific values of AAAA determine what bps and encoding the
    // packet is beyond the frame information word
    //
    // First we match on the marker field with a hamming distance < 4
    // Then we match on the outer code with a hamming distance < 4

    d_sync[d_index] = (d_sync[d_index] << 1) | (sym < 2);
    gr_int64 val = d_sync[d_index];
    gr_int32 marker = ((val & 0x0000FFFFFFFF0000ULL)) >> 16;

    if (gr_count_bits32(marker^FLEX_SYNC_MARKER) < 4) {
        gr_int32 code = ((val & 0xFFFF000000000000ULL) >> 32) |
                         (val & 0x000000000000FFFFULL);

        for (int i = 0; i < num_flex_modes; i++) {
            if (gr_count_bits32(code^flex_modes[i].sync) < 4) {
                d_mode = i;
                return true;
            }
        }

        // Marker received but doesn't match known codes
        // All codes have high word inverted to low word
        unsigned short high = (code & 0xFFFF0000) >> 16;
        unsigned short low = code & 0x0000FFFF;
        unsigned short syn = high^low;
        if (syn == 0xFFFF)
            fprintf(stderr, "Unknown sync code detected: %08X\n", code);
    }

    return false;
}

void pager_flex_deframer::enter_idle()
{
    d_state = ST_IDLE;
    d_index = 0;
    d_start = 0;
    d_center = 0;
    d_end = 0;
    d_count = 0;
    d_mode = 0;
    d_baudrate = 1600;
    d_levels = 2;
    d_spb = d_rate/d_baudrate;
    d_hibit = false;
    d_cdi = 0;
    d_cdw = 0;
    d_blk = 0;
}

void pager_flex_deframer::enter_syncing()
{
    d_start = d_index;
    d_state = ST_SYNCING;
}

void pager_flex_deframer::enter_sync1()
{
    d_state = ST_SYNC1;
    d_end = d_index;
    d_center = index_avg(d_start, d_end);
    d_count = 0;
    fprintf(stderr, "SYNC1=%08X\n", flex_modes[d_mode].sync);
}

void pager_flex_deframer::enter_sync2()
{
    d_state = ST_SYNC2;
    d_count = 0;
    d_baudrate = flex_modes[d_mode].baud;
    d_levels = flex_modes[d_mode].levels;
    d_spb = d_rate/d_baudrate;

    if (d_baudrate == 3200) {
        // Oversampling buffer just got halved
        d_center = d_center/2;
        d_index = d_index/2-d_spb/2; // We're here at the center of a 1600 baud bit
        d_count = -1;                // So this hack gets us in the right place for 3200
    }
}

void pager_flex_deframer::enter_data()
{
    d_state = ST_DATA;
    d_count = 0;
}

void pager_flex_deframer::enter_output()
{
    d_state = ST_OUTPUT;
    d_count = flex_modes[d_mode].phases*88; // Now d_count will count codewords, not bits
    d_output_phase = 0;    // Always phase A
    d_output_index = 0;
}

void pager_flex_deframer::accumulate_frames(unsigned char sym)
{
    // Bits are encoded with 2-bit gray code 
    // 
    // SYM      Bit1 Bit2
    // ---      ---- ----
    //  0         1    1
    //  1         1    0
    //  2         0    0
    //  3         0    1

    // Codewords are interleaved in blocks of 8
    //
    // ABCDEFGH
    // ABCDEFGH
    // ABCDEFGH
    // ABCDEFGH
    // ...
    // etc., for all 32 bits. So each successive incoming bit is multiplexed
    // into a different d_frames[][], indexed by d_cdw

    d_cdw = d_blk*8+d_cdi;
    assert(d_cdw < 88);

    if (d_baudrate == 1600) {
        d_frames[0][d_cdw] <<= 1;
        d_frames[0][d_cdw]  |= (sym < 2);

        if (d_levels == 4) {
            d_frames[1][d_cdw] <<= 1;
            d_frames[1][d_cdw]  |= (sym == 0 || sym == 3);
        }
    }
    else { 
        if (!d_hibit) {
            d_frames[0][d_cdw] <<= 1;
            d_frames[0][d_cdw]  |= (sym < 2);

            if (d_levels == 4) {
                d_frames[1][d_cdw] <<= 1;
                d_frames[1][d_cdw]  |= (sym == 0 || sym == 3);
            }
            d_hibit = true;
        }
        else {
            d_frames[2][d_cdw] <<= 1;
            d_frames[2][d_cdw]  |= (sym < 2);

            if (d_levels == 4) {
                d_frames[3][d_cdw] <<= 1;
                d_frames[3][d_cdw]  |= (sym == 0 || sym == 3);
            }
            d_hibit = false;
        }
    }

    d_cdi = ++d_cdi % 8;
    if (++d_count % (d_baudrate*4/25) == 0)
        d_blk++;    
}

int pager_flex_deframer::general_work(int noutput_items,
    gr_vector_int &ninput_items,
    gr_vector_const_void_star &input_items,
	gr_vector_void_star &output_items)
{
    const unsigned char *in = (const unsigned char *)input_items[0];
    gr_int32 *out = (gr_int32 *)output_items[0];

    int i = 0, j = 0;
    int ninputs = ninput_items[0];

    while (i < ninputs && j < noutput_items) {
        unsigned char sym = 0;
        if (d_state != ST_OUTPUT) {
            sym = *in++; i++;
            d_index = ++d_index % d_spb;
        }
    
        switch (d_state) {
            case ST_IDLE:
                if (test_sync(sym))
                    enter_syncing();
                break;
    
            case ST_SYNCING:
                if (!test_sync(sym)) {
                    enter_sync1();
                    // Output sync code
                    *out++ = flex_modes[d_mode].sync; j++;
                }
                break;
    
            case ST_SYNC1:
                if (d_index == d_center) {
                    d_sync[d_index] = (d_sync[d_index] << 1) | (sym < 2);
                    if (++d_count == 48) { // Skip 16 bits of dotting
                        // Output frame information word
                        *out++ = (gr_int32)(d_sync[d_index] & 0x00000000FFFFFFFFULL); j++;
                        enter_sync2();
                    }
                }
                break;
    
            case ST_SYNC2:
                if (d_index == d_center) {
                    // Skip 25 ms = 40 bits @ 1600 bps, 80 @ 3200 bps
                    if (++d_count == d_baudrate/40)
                        enter_data();
                }
                break;
    
            case ST_DATA:
                if (d_index == d_center) {
                    accumulate_frames(sym);
                    if (d_count == d_baudrate*1760/1000)
                        enter_output();
                }
                break;

            case ST_OUTPUT:
                output_codeword(out++); j++;
                if (--d_count == 0)
                    enter_idle();
                break;

            default:
                assert(0); // memory corruption of d_state if ever gets here
                break;

        }
    }

    consume_each(i);
    return j;
}

void pager_flex_deframer::output_codeword(gr_int32 *out)
{
    *out = d_frames[d_output_phase][d_output_index++];
   
    if (d_output_index == 88) {
        d_output_index = 0;
        d_output_phase++;
        if (d_output_phase == 1 && !flex_modes[d_mode].phase_b)
            d_output_phase++;
        if (d_output_phase == 2 && !flex_modes[d_mode].phase_c)
            d_output_phase++;
        if (d_output_phase == 3 && !flex_modes[d_mode].phase_d)
            d_output_phase++;
    }
}
