/* -*- c++ -*- */
/* 
 * Copyright 2015,2016 Free Software Foundation, Inc.
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

#include <gnuradio/io_signature.h>
#include "dvbs2_interleaver_bb_impl.h"

namespace gr {
  namespace dtv {

    dvbs2_interleaver_bb::sptr
    dvbs2_interleaver_bb::make(dvb_framesize_t framesize, dvb_code_rate_t rate, dvb_constellation_t constellation)
    {
      return gnuradio::get_initial_sptr
        (new dvbs2_interleaver_bb_impl(framesize, rate, constellation));
    }

    /*
     * The private constructor
     */
    dvbs2_interleaver_bb_impl::dvbs2_interleaver_bb_impl(dvb_framesize_t framesize, dvb_code_rate_t rate, dvb_constellation_t constellation)
      : gr::block("dvbs2_interleaver_bb",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char)))
    {
      int rows;
      signal_constellation = constellation;
      code_rate = rate;
      if (framesize == FECFRAME_NORMAL) {
        frame_size = FRAME_SIZE_NORMAL;
        if (rate == C2_9_VLSNR) {
          frame_size = FRAME_SIZE_NORMAL - NORMAL_PUNCTURING;
        }
      }
      else if (framesize == FECFRAME_SHORT) {
        frame_size = FRAME_SIZE_SHORT;
        if (rate == C1_5_VLSNR_SF2 || rate == C11_45_VLSNR_SF2) {
          frame_size = FRAME_SIZE_SHORT - SHORT_PUNCTURING_SET1;
        }
        if (rate == C1_5_VLSNR || rate == C4_15_VLSNR || rate == C1_3_VLSNR) {
          frame_size = FRAME_SIZE_SHORT - SHORT_PUNCTURING_SET2;
        }
      }
      else {
        frame_size = FRAME_SIZE_MEDIUM - MEDIUM_PUNCTURING;
      }
      switch (constellation) {
        case MOD_BPSK:
          mod = 1;
          rows = frame_size / mod;
          set_output_multiple(rows);
          packed_items = rows;
          break;
        case MOD_BPSK_SF2:
          mod = 1;
          rows = frame_size / mod;
          set_output_multiple(rows * 2);
          packed_items = rows * 2;
          break;
        case MOD_QPSK:
          mod = 2;
          rows = frame_size / mod;
          set_output_multiple(rows);
          packed_items = rows;
          break;
        case MOD_8PSK:
          mod = 3;
          rows = frame_size / mod;
          /* 210 */
          if (rate == C3_5) {
            rowaddr0 = rows * 2;
            rowaddr1 = rows;
            rowaddr2 = 0;
          }
          /* 102 */
          else if (rate == C25_36 || rate == C13_18 || rate == C7_15 || rate == C8_15 || rate == C26_45) {
            rowaddr0 = rows;
            rowaddr1 = 0;
            rowaddr2 = rows * 2;
          }
          /* 012 */
          else {
            rowaddr0 = 0;
            rowaddr1 = rows;
            rowaddr2 = rows * 2;
          }
          set_output_multiple(rows);
          packed_items = rows;
          break;
        case MOD_8APSK:
          mod = 3;
          rows = frame_size / mod;
          /* 012 */
          rowaddr0 = 0;
          rowaddr1 = rows;
          rowaddr2 = rows * 2;
          set_output_multiple(rows);
          packed_items = rows;
          break;
        case MOD_16APSK:
          mod = 4;
          rows = frame_size / mod;
          if (rate == C26_45) {
            /* 3201 */
            if (frame_size == FRAME_SIZE_NORMAL) {
              rowaddr0 = rows * 3;
              rowaddr1 = rows * 2;
              rowaddr2 = 0;
              rowaddr3 = rows;
            }
            /* 2130 */
            else {
              rowaddr0 = rows * 2;
              rowaddr1 = rows;
              rowaddr2 = rows * 3;
              rowaddr3 = 0;
            }
          }
          else if (rate == C3_5) {
            /* 3210 */
            if (frame_size == FRAME_SIZE_NORMAL) {
              rowaddr0 = rows * 3;
              rowaddr1 = rows * 2;
              rowaddr2 = rows;
              rowaddr3 = 0;
            }
            /* 3201 */
            else {
              rowaddr0 = rows * 3;
              rowaddr1 = rows * 2;
              rowaddr2 = 0;
              rowaddr3 = rows;
            }
          }
          /* 3012 */
          else if (rate == C28_45) {
            rowaddr0 = rows * 3;
            rowaddr1 = 0;
            rowaddr2 = rows;
            rowaddr3 = rows * 2;
          }
          /* 3021 */
          else if (rate == C23_36 || rate == C13_18) {
            rowaddr0 = rows * 3;
            rowaddr1 = 0;
            rowaddr2 = rows * 2;
            rowaddr3 = rows;
          }
          /* 2310 */
          else if (rate == C25_36) {
            rowaddr0 = rows * 2;
            rowaddr1 = rows * 3;
            rowaddr2 = rows;
            rowaddr3 = 0;
          }
          /* 2103 */
          else if (rate == C7_15 || rate == C8_15) {
            rowaddr0 = rows * 2;
            rowaddr1 = rows;
            rowaddr2 = 0;
            rowaddr3 = rows * 3;
          }
          /* 3210 */
          else if (rate == C140_180) {
            rowaddr0 = rows * 3;
            rowaddr1 = rows * 2;
            rowaddr2 = rows;
            rowaddr3 = 0;
          }
          /* 0321 */
          else if (rate == C154_180) {
            rowaddr0 = 0;
            rowaddr1 = rows * 3;
            rowaddr2 = rows * 2;
            rowaddr3 = rows;
          }
          /* 0123 */
          else {
            rowaddr0 = 0;
            rowaddr1 = rows;
            rowaddr2 = rows * 2;
            rowaddr3 = rows * 3;
          }
          set_output_multiple(rows);
          packed_items = rows;
          break;
        case MOD_8_8APSK:
          mod = 4;
          rows = frame_size / mod;
          /* 3210 */
          if (rate == C90_180) {
            rowaddr0 = rows * 3;
            rowaddr1 = rows * 2;
            rowaddr2 = rows;
            rowaddr3 = 0;
          }
          /* 2310 */
          else if (rate == C96_180) {
            rowaddr0 = rows * 2;
            rowaddr1 = rows * 3;
            rowaddr2 = rows;
            rowaddr3 = 0;
          }
          /* 2301 */
          else if (rate == C100_180) {
            rowaddr0 = rows * 2;
            rowaddr1 = rows * 3;
            rowaddr2 = 0;
            rowaddr3 = rows;
          }
          /* 0123 */
          else {
            rowaddr0 = 0;
            rowaddr1 = rows;
            rowaddr2 = rows * 2;
            rowaddr3 = rows * 3;
          }
          set_output_multiple(rows);
          packed_items = rows;
          break;
        case MOD_32APSK:
          mod = 5;
          rows = frame_size / mod;
          /* 01234 */
          rowaddr0 = 0;
          rowaddr1 = rows;
          rowaddr2 = rows * 2;
          rowaddr3 = rows * 3;
          rowaddr4 = rows * 4;
          set_output_multiple(rows);
          packed_items = rows;
          break;
        case MOD_4_12_16APSK:
          mod = 5;
          rows = frame_size / mod;
          /* 21430 */
          if (frame_size == FRAME_SIZE_NORMAL) {
            rowaddr0 = rows * 2;
            rowaddr1 = rows;
            rowaddr2 = rows * 4;
            rowaddr3 = rows * 3;
            rowaddr4 = 0;
          }
          else {
            /* 41230 */
            if (rate == C2_3) {
              rowaddr0 = rows * 4;
              rowaddr1 = rows;
              rowaddr2 = rows * 2;
              rowaddr3 = rows * 3;
              rowaddr4 = 0;
            }
            /* 10423 */
            else if (rate == C32_45) {
              rowaddr0 = rows;
              rowaddr1 = 0;
              rowaddr2 = rows * 4;
              rowaddr3 = rows * 2;
              rowaddr4 = rows * 3;
            }
          }
          set_output_multiple(rows);
          packed_items = rows;
          break;
        case MOD_4_8_4_16APSK:
          mod = 5;
          rows = frame_size / mod;
          /* 40213 */
          if (rate == C140_180) {
            rowaddr0 = rows * 4;
            rowaddr1 = 0;
            rowaddr2 = rows * 2;
            rowaddr3 = rows;
            rowaddr4 = rows * 3;
          }
          /* 40312 */
          else {
            rowaddr0 = rows * 4;
            rowaddr1 = 0;
            rowaddr2 = rows * 3;
            rowaddr3 = rows;
            rowaddr4 = rows * 2;
          }
          set_output_multiple(rows);
          packed_items = rows;
          break;
        case MOD_64APSK:
          mod = 6;
          rows = frame_size / mod;
          /* 305214 */
          rowaddr0 = rows * 3;
          rowaddr1 = 0;
          rowaddr2 = rows * 5;
          rowaddr3 = rows * 2;
          rowaddr4 = rows;
          rowaddr5 = rows * 4;
          set_output_multiple(rows);
          packed_items = rows;
          break;
        case MOD_8_16_20_20APSK:
          mod = 6;
          rows = frame_size / mod;
          /* 201543 */
          if (rate == C7_9) {
            rowaddr0 = rows * 2;
            rowaddr1 = 0;
            rowaddr2 = rows;
            rowaddr3 = rows * 5;
            rowaddr4 = rows * 4;
            rowaddr5 = rows * 3;
          }
          /* 124053 */
          else if (rate == C4_5) {
            rowaddr0 = rows;
            rowaddr1 = rows * 2;
            rowaddr2 = rows * 4;
            rowaddr3 = 0;
            rowaddr4 = rows * 5;
            rowaddr5 = rows * 3;
          }
          /* 421053 */
          else if (rate == C5_6) {
            rowaddr0 = rows * 4;
            rowaddr1 = rows * 2;
            rowaddr2 = rows;
            rowaddr3 = 0;
            rowaddr4 = rows * 5;
            rowaddr5 = rows * 3;
          }
          set_output_multiple(rows);
          packed_items = rows;
          break;
        case MOD_4_12_20_28APSK:
          mod = 6;
          rows = frame_size / mod;
          /* 520143 */
          rowaddr0 = rows * 5;
          rowaddr1 = rows * 2;
          rowaddr2 = 0;
          rowaddr3 = rows;
          rowaddr4 = rows * 4;
          rowaddr5 = rows * 3;
          set_output_multiple(rows);
          packed_items = rows;
          break;
        case MOD_128APSK:
          mod = 7;
          rows = (frame_size + 6) / mod;
          /* 4250316 */
          if (rate == C135_180) {
            rowaddr0 = rows * 4;
            rowaddr1 = rows * 2;
            rowaddr2 = rows * 5;
            rowaddr3 = 0;
            rowaddr4 = rows * 3;
            rowaddr5 = rows;
            rowaddr6 = rows * 6;
          }
          /* 4130256 */
          else if (rate == C140_180) {
            rowaddr0 = rows * 4;
            rowaddr1 = rows;
            rowaddr2 = rows * 3;
            rowaddr3 = 0;
            rowaddr4 = rows * 2;
            rowaddr5 = rows * 5;
            rowaddr6 = rows * 6;
          }
          set_output_multiple(rows + 12);
          packed_items = rows + 12;
          break;
        case MOD_256APSK:
          mod = 8;
          rows = frame_size / mod;
          /* 40372156 */
          if (rate == C116_180) {
            rowaddr0 = rows * 4;
            rowaddr1 = 0;
            rowaddr2 = rows * 3;
            rowaddr3 = rows * 7;
            rowaddr4 = rows * 2;
            rowaddr5 = rows;
            rowaddr6 = rows * 5;
            rowaddr7 = rows * 6;
          }
          /* 46320571 */
          else if (rate == C124_180) {
            rowaddr0 = rows * 4;
            rowaddr1 = rows * 6;
            rowaddr2 = rows * 3;
            rowaddr3 = rows * 2;
            rowaddr4 = 0;
            rowaddr5 = rows * 5;
            rowaddr6 = rows * 7;
            rowaddr7 = rows;
          }
          /* 75642301 */
          else if (rate == C128_180) {
            rowaddr0 = rows * 7;
            rowaddr1 = rows * 5;
            rowaddr2 = rows * 6;
            rowaddr3 = rows * 4;
            rowaddr4 = rows * 2;
            rowaddr5 = rows * 3;
            rowaddr6 = 0;
            rowaddr7 = rows;
          }
          /* 50743612 */
          else if (rate == C135_180) {
            rowaddr0 = rows * 5;
            rowaddr1 = 0;
            rowaddr2 = rows * 7;
            rowaddr3 = rows * 4;
            rowaddr4 = rows * 3;
            rowaddr5 = rows * 6;
            rowaddr6 = rows;
            rowaddr7 = rows * 2;
          }
          /* 01234567 */
          else {
            rowaddr0 = 0;
            rowaddr1 = rows;
            rowaddr2 = rows * 2;
            rowaddr3 = rows * 3;
            rowaddr4 = rows * 4;
            rowaddr5 = rows * 5;
            rowaddr6 = rows * 6;
            rowaddr7 = rows * 7;
          }
          set_output_multiple(rows);
          packed_items = rows;
          break;
        default:
          mod = 2;
          rows = frame_size / mod;
          set_output_multiple(rows);
          packed_items = rows;
          break;
      }
    }

    /*
     * Our virtual destructor.
     */
    dvbs2_interleaver_bb_impl::~dvbs2_interleaver_bb_impl()
    {
    }

    void
    dvbs2_interleaver_bb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      if (signal_constellation == MOD_128APSK) {
        ninput_items_required[0] = ((noutput_items / 9270) * 9258) * mod;
      }
      else if (signal_constellation == MOD_BPSK_SF2) {
        ninput_items_required[0] = (noutput_items * mod) / 2;
      }
      else {
        ninput_items_required[0] = noutput_items * mod;
      }
    }

    int
    dvbs2_interleaver_bb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];
      int consumed = 0;
      int produced = 0;
      int rows;

      switch (signal_constellation) {
        case MOD_BPSK:
          for (int i = 0; i < noutput_items; i += packed_items) {
            rows = frame_size;
            for (int j = 0; j < rows; j++) {
              out[produced++] = in[consumed++];
            }
          }
          break;
        case MOD_BPSK_SF2:
          for (int i = 0; i < noutput_items; i += packed_items) {
            rows = frame_size;
            for (int j = 0; j < rows; j++) {
              out[produced++] = in[consumed];
              out[produced++] = in[consumed++];
            }
          }
          break;
        case MOD_QPSK:
          for (int i = 0; i < noutput_items; i += packed_items) {
            rows = frame_size / 2;
            for (int j = 0; j < rows; j++) {
              out[produced] = in[consumed++] << 1;
              out[produced++] |= in[consumed++];
            }
          }
          break;
        case MOD_8PSK:
        case MOD_8APSK:
          for (int i = 0; i < noutput_items; i += packed_items) {
            rows = frame_size / 3;
            const unsigned char  *c1, *c2, *c3;
            c1 = &in[consumed + rowaddr0];
            c2 = &in[consumed + rowaddr1];
            c3 = &in[consumed + rowaddr2];
            for (int j = 0; j < rows; j++) {
              out[produced++] = (c1[j]<<2) | (c2[j]<<1) | (c3[j]);
              consumed += 3;
            }
          }
          break;
        case MOD_16APSK:
        case MOD_8_8APSK:
          for (int i = 0; i < noutput_items; i += packed_items) {
            rows = frame_size / 4;
            const unsigned char *c1, *c2, *c3, *c4;
            c1 = &in[consumed + rowaddr0];
            c2 = &in[consumed + rowaddr1];
            c3 = &in[consumed + rowaddr2];
            c4 = &in[consumed + rowaddr3];
            for (int j = 0; j < rows; j++) {
              out[produced++] = (c1[j]<<3) | (c2[j]<<2) | (c3[j]<<1) | (c4[j]);
              consumed += 4;
            }
          }
          break;
        case MOD_32APSK:
        case MOD_4_12_16APSK:
        case MOD_4_8_4_16APSK:
          for (int i = 0; i < noutput_items; i += packed_items) {
            rows = frame_size / 5;
            const unsigned char *c1, *c2, *c3, *c4, *c5;
            c1 = &in[consumed + rowaddr0];
            c2 = &in[consumed + rowaddr1];
            c3 = &in[consumed + rowaddr2];
            c4 = &in[consumed + rowaddr3];
            c5 = &in[consumed + rowaddr4];
            for (int j = 0; j < rows; j++) {
              out[produced++] = (c1[j]<<4) | (c2[j]<<3) | (c3[j]<<2) | (c4[j]<<1) | c5[j];
              consumed += 5;
            }
          }
          break;
        case MOD_64APSK:
        case MOD_8_16_20_20APSK:
        case MOD_4_12_20_28APSK:
          for (int i = 0; i < noutput_items; i += packed_items) {
            rows = frame_size / 6;
            const unsigned char *c1, *c2, *c3, *c4, *c5, *c6;
            c1 = &in[consumed + rowaddr0];
            c2 = &in[consumed + rowaddr1];
            c3 = &in[consumed + rowaddr2];
            c4 = &in[consumed + rowaddr3];
            c5 = &in[consumed + rowaddr4];
            c6 = &in[consumed + rowaddr5];
            for (int j = 0; j < rows; j++) {
              out[produced++] = (c1[j]<<5) | (c2[j]<<4) | (c3[j]<<3) | (c4[j]<<2) | (c5[j]<<1) | c6[j];
              consumed += 6;
            }
          }
          break;
        case MOD_128APSK:
          for (int i = 0; i < noutput_items; i += packed_items) {
            rows = (frame_size + 6) / 7;
            const unsigned char *c1, *c2, *c3, *c4, *c5, *c6, *c7;
            c1 = &in[consumed + rowaddr0];
            c2 = &in[consumed + rowaddr1];
            c3 = &in[consumed + rowaddr2];
            c4 = &in[consumed + rowaddr3];
            c5 = &in[consumed + rowaddr4];
            c6 = &in[consumed + rowaddr5];
            c7 = &in[consumed + rowaddr6];
            for (int j = 0; j < rows; j++) {
              out[produced++] = (c1[j]<<6) | (c2[j]<<5) | (c3[j]<<4) | (c4[j]<<3) | (c5[j]<<2) | (c6[j]<<1) | c7[j];
              consumed += 7;
            }
            for (int j = 0; j < 12; j++) {
              out[produced++] = 0x7f;
            }
          }
          break;
        case MOD_256APSK:
          for (int i = 0; i < noutput_items; i += packed_items) {
            rows = frame_size / 8;
            const unsigned char *c1, *c2, *c3, *c4, *c5, *c6, *c7, *c8;
            c1 = &in[consumed + rowaddr0];
            c2 = &in[consumed + rowaddr1];
            c3 = &in[consumed + rowaddr2];
            c4 = &in[consumed + rowaddr3];
            c5 = &in[consumed + rowaddr4];
            c6 = &in[consumed + rowaddr5];
            c7 = &in[consumed + rowaddr6];
            c8 = &in[consumed + rowaddr7];
            for (int j = 0; j < rows; j++) {
              out[produced++] = (c1[j]<<7) | (c2[j]<<6) | (c3[j]<<5) | (c4[j]<<4) | (c5[j]<<3) | (c6[j]<<2) | (c7[j]<<1) | c8[j];
              consumed += 8;
            }
          }
          break;
        default:
          for (int i = 0; i < noutput_items; i += packed_items) {
            rows = frame_size / 2;
            for (int j = 0; j < rows; j++) {
              out[produced] = in[consumed++] << 1;
              out[produced++] |= in[consumed++];
            }
          }
          break;
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (consumed);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */

