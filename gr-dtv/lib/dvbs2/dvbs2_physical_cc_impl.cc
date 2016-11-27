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
#include "dvbs2_physical_cc_impl.h"

namespace gr {
  namespace dtv {

    dvbs2_physical_cc::sptr
    dvbs2_physical_cc::make(dvb_framesize_t framesize, dvb_code_rate_t rate, dvb_constellation_t constellation, dvbs2_pilots_t pilots, int goldcode)
    {
      return gnuradio::get_initial_sptr
        (new dvbs2_physical_cc_impl(framesize, rate, constellation, pilots, goldcode));
    }

    /*
     * The private constructor
     */
    dvbs2_physical_cc_impl::dvbs2_physical_cc_impl(dvb_framesize_t framesize, dvb_code_rate_t rate, dvb_constellation_t constellation, dvbs2_pilots_t pilots, int goldcode)
      : gr::block("dvbs2_physical_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {
      int type, modcod;
      double r0 = 1.0;

      signal_constellation = constellation;
      modcod = 0;
      if (framesize == FECFRAME_NORMAL) {
        frame_size = FRAME_SIZE_NORMAL;
        type = 0;
        if (rate == C2_9_VLSNR) {
          frame_size = (FRAME_SIZE_NORMAL - NORMAL_PUNCTURING) + (EXTRA_PILOT_SYMBOLS_SET1 * 2);
          pilots = PILOTS_ON;    /* force pilots on for VL-SNR */
        }
      }

      else if (framesize == FECFRAME_SHORT) {
        frame_size = FRAME_SIZE_SHORT;
        type = 2;
        if (rate == C1_5_VLSNR_SF2 || rate == C11_45_VLSNR_SF2) {
          frame_size = ((FRAME_SIZE_SHORT - SHORT_PUNCTURING_SET1) * 2) + EXTRA_PILOT_SYMBOLS_SET1;
          pilots = PILOTS_ON;    /* force pilots on for VL-SNR */
        }
        if (rate == C1_5_VLSNR || rate == C4_15_VLSNR || rate == C1_3_VLSNR) {
          frame_size = (FRAME_SIZE_SHORT - SHORT_PUNCTURING_SET2) + EXTRA_PILOT_SYMBOLS_SET2;
          pilots = PILOTS_ON;    /* force pilots on for VL-SNR */
        }
      }
      else  {
        frame_size = FRAME_SIZE_MEDIUM - MEDIUM_PUNCTURING + EXTRA_PILOT_SYMBOLS_SET1;
        type = 0;
        pilots = PILOTS_ON;    /* force pilots on for VL-SNR */
      }

      pilot_mode = pilots;
      if (pilot_mode) {
        type |= 1;
      }
      if (goldcode < 0 || goldcode > 262141) {
        GR_LOG_WARN(d_logger, "Gold Code must be between 0 and 262141 inclusive.");
        GR_LOG_WARN(d_logger, "Gold Code set to 0.");
        goldcode = 0;
      }
      gold_code = goldcode;

      vlsnr_set = VLSNR_OFF;
      switch (rate) {
        case C2_9_VLSNR:
          vlsnr_header = 0;
          vlsnr_set = VLSNR_SET1;
          break;
        case C1_5_MEDIUM:
          vlsnr_header = 1;
          vlsnr_set = VLSNR_SET1;
          break;
        case C11_45_MEDIUM:
          vlsnr_header = 2;
          vlsnr_set = VLSNR_SET1;
          break;
        case C1_3_MEDIUM:
          vlsnr_header = 3;
          vlsnr_set = VLSNR_SET1;
          break;
        case C1_5_VLSNR_SF2:
          vlsnr_header = 4;
          vlsnr_set = VLSNR_SET1;
          break;
        case C11_45_VLSNR_SF2:
          vlsnr_header = 5;
          vlsnr_set = VLSNR_SET1;
          break;
        case C1_5_VLSNR:
          vlsnr_header = 9;
          vlsnr_set = VLSNR_SET2;
          break;
        case C4_15_VLSNR:
          vlsnr_header = 10;
          vlsnr_set = VLSNR_SET2;
          break;
        case C1_3_VLSNR:
          vlsnr_header = 11;
          vlsnr_set = VLSNR_SET2;
          break;
        default:
          vlsnr_header = 12;
          break;
      }

      m_bpsk[0][0] = gr_complex((r0 * cos(M_PI / 4.0)), (r0 * sin(M_PI / 4.0)));
      m_bpsk[0][1] = gr_complex((r0 * cos(5.0 * M_PI / 4.0)), (r0 * sin(5.0 * M_PI / 4.0)));
      m_bpsk[1][0] = gr_complex((r0 * cos(5.0 * M_PI / 4.0)), (r0 * sin(M_PI / 4.0)));
      m_bpsk[1][1] = gr_complex((r0 * cos(M_PI / 4.0)), (r0 * sin(5.0 * M_PI /4.0)));
      m_bpsk[2][0] = gr_complex((r0 * cos(5.0 * M_PI / 4.0)), (r0 * sin(M_PI / 4.0)));
      m_bpsk[2][1] = gr_complex((r0 * cos(M_PI / 4.0)), (r0 * sin(5.0 * M_PI /4.0)));
      m_bpsk[3][0] = gr_complex((r0 * cos(5.0 * M_PI / 4.0)), (r0 * sin(5.0 * M_PI / 4.0)));
      m_bpsk[3][1] = gr_complex((r0 * cos(M_PI / 4.0)), (r0 * sin(M_PI / 4.0)));

      m_zero = gr_complex(0.0, 0.0);

      // Mode and code rate
      if (constellation == MOD_BPSK) {
        slots = (frame_size / 1) / 90;
        pilot_symbols = (slots / 16) * 36;
        if (!(slots % 16)) {
          pilot_symbols -= 36;
        }
        switch (rate) {
          case C1_5_MEDIUM:
          case C11_45_MEDIUM:
          case C1_3_MEDIUM:
            modcod = 128;
            break;
          case C1_5_VLSNR:
          case C4_15_VLSNR:
          case C1_3_VLSNR:
            modcod = 130;
            break;
          default:
            modcod = 0;
            break;
        }
      }

      if (constellation == MOD_BPSK_SF2) {
        slots = (frame_size / 1) / 90;
        pilot_symbols = (slots / 16) * 36;
        if (!(slots % 16)) {
          pilot_symbols -= 36;
        }
        switch (rate) {
          case C1_5_VLSNR_SF2:
          case C11_45_VLSNR_SF2:
            modcod = 128;
            break;
          default:
            modcod = 0;
            break;
        }
      }

      if (constellation == MOD_QPSK) {
        slots = (frame_size / 2) / 90;
        pilot_symbols = (slots / 16) * 36;
        if (!(slots % 16)) {
          pilot_symbols -= 36;
        }
        switch (rate) {
          case C2_9_VLSNR:
            modcod = 128;
            break;
          case C1_4:
            modcod = 1;
            break;
          case C1_3:
            modcod = 2;
            break;
          case C2_5:
            modcod = 3;
            break;
          case C1_2:
            modcod = 4;
            break;
          case C3_5:
            modcod = 5;
            break;
          case C2_3:
            modcod = 6;
            break;
          case C3_4:
            modcod = 7;
            break;
          case C4_5:
            modcod = 8;
            break;
          case C5_6:
            modcod = 9;
            break;
          case C8_9:
            modcod = 10;
            break;
          case C9_10:
            modcod = 11;
            break;
          case C13_45:
            modcod = 132;
            break;
          case C9_20:
            modcod = 134;
            break;
          case C11_20:
            modcod = 136;
            break;
          case C11_45:
            modcod = 216;
            break;
          case C4_15:
            modcod = 218;
            break;
          case C14_45:
            modcod = 220;
            break;
          case C7_15:
            modcod = 222;
            break;
          case C8_15:
            modcod = 224;
            break;
          case C32_45:
            modcod = 226;
            break;
          default:
            modcod = 0;
            break;
        }
      }

      if (constellation == MOD_8PSK) {
        slots = (frame_size / 3) / 90;
        pilot_symbols = (slots / 16) * 36;
        if (!(slots % 16)) {
          pilot_symbols -= 36;
        }
        switch (rate) {
          case C3_5:
            modcod = 12;
            break;
          case C2_3:
            modcod = 13;
            break;
          case C3_4:
            modcod = 14;
            break;
          case C5_6:
            modcod = 15;
            break;
          case C8_9:
            modcod = 16;
            break;
          case C9_10:
            modcod = 17;
            break;
          case C23_36:
            modcod = 142;
            break;
          case C25_36:
            modcod = 144;
            break;
          case C13_18:
            modcod = 146;
            break;
          case C7_15:
            modcod = 228;
            break;
          case C8_15:
            modcod = 230;
            break;
          case C26_45:
            modcod = 232;
            break;
          case C32_45:
            modcod = 234;
            break;
          default:
            modcod = 0;
            break;
        }
      }

      if (constellation == MOD_8APSK) {
        slots = (frame_size / 3) / 90;
        pilot_symbols = (slots / 16) * 36;
        if (!(slots % 16)) {
          pilot_symbols -= 36;
        }
        switch (rate) {
          case C100_180:
            modcod = 138;
            break;
          case C104_180:
            modcod = 140;
            break;
          default:
            modcod = 0;
            break;
        }
      }

      if (constellation == MOD_16APSK) {
        slots = (frame_size / 4) / 90;
        pilot_symbols = (slots / 16) * 36;
        if (!(slots % 16)) {
          pilot_symbols -= 36;
        }
        switch (rate) {
          case C2_3:
            modcod = 18;
            break;
          case C3_4:
            modcod = 19;
            break;
          case C4_5:
            modcod = 20;
            break;
          case C5_6:
            modcod = 21;
            break;
          case C8_9:
            modcod = 22;
            break;
          case C9_10:
            modcod = 23;
            break;
          case C26_45:
            if (frame_size == FRAME_SIZE_NORMAL) {
              modcod = 154;
            }
            else {
              modcod = 240;
            }
            break;
          case C3_5:
            if (frame_size == FRAME_SIZE_NORMAL) {
              modcod = 156;
            }
            else {
              modcod = 242;
            }
            break;
          case C28_45:
            modcod = 160;
            break;
          case C23_36:
            modcod = 162;
            break;
          case C25_36:
            modcod = 166;
            break;
          case C13_18:
            modcod = 168;
            break;
          case C140_180:
            modcod = 170;
            break;
          case C154_180:
            modcod = 172;
            break;
          case C7_15:
            modcod = 236;
            break;
          case C8_15:
            modcod = 238;
            break;
          case C32_45:
            modcod = 244;
            break;
          default:
            modcod = 0;
            break;
        }
      }

      if (constellation == MOD_8_8APSK)
      {
        slots = (frame_size / 4) / 90;
        pilot_symbols = (slots / 16) * 36;
        if (!(slots % 16)) {
          pilot_symbols -= 36;
        }
        switch (rate) {
          case C90_180:
            modcod = 148;
            break;
          case C96_180:
            modcod = 150;
            break;
          case C100_180:
            modcod = 152;
            break;
          case C18_30:
            modcod = 158;
            break;
          case C20_30:
            modcod = 164;
            break;
          default:
            modcod = 0;
            break;
        }
      }

      if (constellation == MOD_32APSK) {
        slots = (frame_size / 5) / 90;
        pilot_symbols = (slots / 16) * 36;
        if (!(slots % 16)) {
          pilot_symbols -= 36;
        }
        switch (rate) {
          case C3_4:
            modcod = 24;
            break;
          case C4_5:
            modcod = 25;
            break;
          case C5_6:
            modcod = 26;
            break;
          case C8_9:
            modcod = 27;
            break;
          case C9_10:
            modcod = 28;
            break;
          default:
            modcod = 0;
            break;
        }
      }

      if (constellation == MOD_4_12_16APSK) {
        slots = (frame_size / 5) / 90;
        pilot_symbols = (slots / 16) * 36;
        if (!(slots % 16)) {
          pilot_symbols -= 36;
        }
        switch (rate) {
          case C2_3:
            if (frame_size == FRAME_SIZE_NORMAL) {
              modcod = 174;
            }
            else {
              modcod = 246;
            }
            break;
          case C32_45:
            modcod = 248;
            break;
          default:
            modcod = 0;
            break;
        }
      }

      if (constellation == MOD_4_8_4_16APSK) {
        slots = (frame_size / 5) / 90;
        pilot_symbols = (slots / 16) * 36;
        if (!(slots % 16)) {
          pilot_symbols -= 36;
        }
        switch (rate) {
          case C128_180:
            modcod = 178;
            break;
          case C132_180:
            modcod = 180;
            break;
          case C140_180:
            modcod = 182;
            break;
          default:
            modcod = 0;
            break;
        }
      }

      if (constellation == MOD_64APSK) {
        slots = (frame_size / 6) / 90;
        pilot_symbols = (slots / 16) * 36;
        if (!(slots % 16)) {
          pilot_symbols -= 36;
        }
        switch (rate) {
          case C128_180:
            modcod = 184;
            break;
          default:
            modcod = 0;
            break;
        }
      }

      if (constellation == MOD_8_16_20_20APSK) {
        slots = (frame_size / 6) / 90;
        pilot_symbols = (slots / 16) * 36;
        if (!(slots % 16)) {
          pilot_symbols -= 36;
        }
        switch (rate) {
          case C7_9:
            modcod = 190;
            break;
          case C4_5:
            modcod = 194;
            break;
          case C5_6:
            modcod = 198;
            break;
          default:
            modcod = 0;
            break;
        }
      }

      if (constellation == MOD_4_12_20_28APSK) {
        slots = (frame_size / 6) / 90;
        pilot_symbols = (slots / 16) * 36;
        if (!(slots % 16)) {
          pilot_symbols -= 36;
        }
        switch (rate) {
          case C132_180:
            modcod = 186;
            break;
          default:
            modcod = 0;
            break;
        }
      }

      if (constellation == MOD_128APSK) {
        slots = 103;
        pilot_symbols = (slots / 16) * 36;
        if (!(slots % 16)) {
          pilot_symbols -= 36;
        }
        switch (rate) {
          case C135_180:
            modcod = 200;
            break;
          case C140_180:
            modcod = 202;
            break;
          default:
            modcod = 0;
            break;
        }
      }

      if (constellation == MOD_256APSK) {
        slots = (frame_size / 8) / 90;
        pilot_symbols = (slots / 16) * 36;
        if (!(slots % 16)) {
          pilot_symbols -= 36;
        }
        switch (rate) {
          case C116_180:
            modcod = 204;
            break;
          case C20_30:
            modcod = 206;
            break;
          case C124_180:
            modcod = 208;
            break;
          case C128_180:
            modcod = 210;
            break;
          case C22_30:
            modcod = 212;
            break;
          case C135_180:
            modcod = 214;
            break;
          default:
            modcod = 0;
            break;
        }
      }

      // Now create the PL header.
      // Add the sync sequence SOF
      for (int i = 0; i < 26; i++) {
        b[i] = ph_sync_seq[i];
      }
      // Add the mode and code
      pl_header_encode(modcod, type, &b[26]);

      // BPSK modulate and create the header
      for (int i = 0; i < 26; i++) {
        m_pl[i] = m_bpsk[i & 1][b[i]];
      }
      if (modcod & 0x80) {
        for (int i = 26; i < 90; i++) {
          m_pl[i] = m_bpsk[(i & 1) + 2][b[i]];
        }
      }
      else {
        for (int i = 26; i < 90; i++) {
          m_pl[i] = m_bpsk[i & 1][b[i]];
        }
      }

      // Create the VL-SNR header.
      // Add leading zeroes
      for (int i = 0; i < 2; i++) {
        b[i] = 0;
      }
      for (int i = 2; i < 898; i++) {
        b[i] = ph_vlsnr_seq[vlsnr_header][i - 2];
      }
      // Add trailing zeroes
      for (int i = 898; i < VLSNR_HEADER_LENGTH; i++) {
        b[i] = 0;
      }
      // BPSK modulate and create the VL-SNR header
      for (int i = 0; i < VLSNR_HEADER_LENGTH; i++) {
        m_vlsnr_header[i] = m_bpsk[i & 1][b[i]];
      }

      build_symbol_scrambler_table();
      if (!pilot_mode) {
        pilot_symbols = 0;
      }
      if (vlsnr_set == VLSNR_OFF) {
        set_output_multiple((((slots * 90) + 90) + pilot_symbols) * 2);
      }
      else {
        set_output_multiple((((slots * 90) + 90) + (pilot_symbols + 36) + VLSNR_HEADER_LENGTH) * 2);
      }
    }

    /*
     * Our virtual destructor.
     */
    dvbs2_physical_cc_impl::~dvbs2_physical_cc_impl()
    {
    }

    void
    dvbs2_physical_cc_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      if (vlsnr_set == VLSNR_OFF) {
        ninput_items_required[0] = (noutput_items / ((((slots * 90) + 90) + pilot_symbols) * 2)) * (slots * 90);
      }
      else if (vlsnr_set == VLSNR_SET1) {
        ninput_items_required[0] = (noutput_items / ((((slots * 90) + 90) + (pilot_symbols + 36) + VLSNR_HEADER_LENGTH) * 2)) * ((slots * 90) - EXTRA_PILOT_SYMBOLS_SET1);
      }
      else {
        ninput_items_required[0] = (noutput_items / ((((slots * 90) + 90) + (pilot_symbols + 36) + VLSNR_HEADER_LENGTH) * 2)) * ((slots * 90) - EXTRA_PILOT_SYMBOLS_SET2);
      }
    }

    void
    dvbs2_physical_cc_impl::b_64_8_code(unsigned char in, int *out)
    {
      unsigned long temp, bit;

      temp = 0;

      if (in & 0x80) {
        temp ^= g[0];
      }
      if (in & 0x40) {
        temp ^= g[1];
      }
      if (in & 0x20) {
        temp ^= g[2];
      }
      if (in & 0x10) {
        temp ^= g[3];
      }
      if (in & 0x08) {
        temp ^= g[4];
      }
      if (in & 0x04) {
        temp ^= g[5];
      }
      if (in & 0x02) {
        temp ^= g[6];
      }

      bit = 0x80000000;
      for (int m = 0; m < 32; m++) {
        out[(m * 2)] = (temp & bit) ? 1 : 0;
        out[(m * 2) + 1] = out[m * 2] ^ (in & 0x01);
        bit >>= 1;
      }
      // Randomise it
      for (int m = 0; m < 64; m++) {
        out[m] = out[m] ^ ph_scram_tab[m];
      }
    }

    void
    dvbs2_physical_cc_impl::pl_header_encode(unsigned char modcod, unsigned char type, int *out)
    {
      unsigned char code;

      if (modcod & 0x80) {
        code = modcod | (type & 0x1);
      }
      else {
        code = (modcod << 2) | type;
      }
      // Add the modcod and type information and scramble it
      b_64_8_code (code, out);
    }

    int
    dvbs2_physical_cc_impl::parity_chk(long a, long b)
    {
      int c = 0;
      a = a & b;
      for (int i = 0; i < 18; i++) {
        if(a & (1L << i)) {
          c++;
        }
      }
      return c & 1;
    }

    void
    dvbs2_physical_cc_impl::build_symbol_scrambler_table(void)
    {
      long x, y;
      int xa, xb, xc, ya, yb, yc;
      int rn, zna, znb;

      // Initialisation
      x = 0x00001;
      y = 0x3FFFF;

      for (int n = 0; n < gold_code; n++) {
        xb = parity_chk(x, 0x0081);

        x >>= 1;
        if (xb) {
          x |= 0x20000;
        }
      }

      for (int i = 0; i < FRAME_SIZE_NORMAL; i++) {
        xa = parity_chk(x, 0x8050);
        xb = parity_chk(x, 0x0081);
        xc = x & 1;

        x >>= 1;
        if (xb) {
          x |= 0x20000;
        }

        ya = parity_chk(y, 0x04A1);
        yb = parity_chk(y, 0xFF60);
        yc = y & 1;

        y >>= 1;
        if (ya) {
          y |= 0x20000;
        }

        zna = xc ^ yc;
        znb = xa ^ yb;
        rn = (znb << 1) + zna;
        m_cscram[i] = rn;
      }
    }

    int
    dvbs2_physical_cc_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      int consumed = 0;
      int produced = 0;
      int slot_count, n;
      int group, symbols;
      gr_complex tempin, tempout;

      if (vlsnr_set == VLSNR_OFF) {
        for (int i = 0; i < noutput_items / 2; i += (((slots * 90) + 90) + pilot_symbols)) {
          n = 0;
          slot_count = 0;
          for (int plh = 0; plh < 90; plh++) {
            out[produced++] = m_pl[plh];
            out[produced++] = m_zero;
          }
          for (int j = 0; j < slots; j++) {
            for (int k = 0; k < 90; k++) {
              tempin = in[consumed++];
              switch (m_cscram[n++]) {
                case 0:
                  tempout = tempin;
                  break;
                case 1:
                  tempout = gr_complex(-tempin.imag(),  tempin.real());
                  break;
                case 2:
                  tempout = -tempin;
                  break;
                case 3:
                  tempout = gr_complex( tempin.imag(), -tempin.real());
                  break;
              }
              out[produced++] = tempout;
              out[produced++] = m_zero;
            }
            slot_count = (slot_count + 1) % 16;
            if ((slot_count == 0) && (j < slots - 1)) {
              if (pilot_mode) {
                // Add pilots if needed
                for (int p = 0; p < 36; p++) {
                  tempin = m_bpsk[0][0];
                  switch (m_cscram[n++]) {
                    case 0:
                      tempout = tempin;
                      break;
                    case 1:
                      tempout = gr_complex(-tempin.imag(),  tempin.real());
                      break;
                    case 2:
                      tempout = -tempin;
                      break;
                    case 3:
                      tempout = gr_complex( tempin.imag(), -tempin.real());
                      break;
                  }
                  out[produced++] = tempout;
                  out[produced++] = m_zero;
                }
              }
            }
          }
        }
      }
      else if (vlsnr_set == VLSNR_SET1) {
        for (int i = 0; i < noutput_items / 2; i += (((slots * 90) + 90) + pilot_symbols + VLSNR_HEADER_LENGTH + 36)) {
          n = 0;
          slot_count = 10;
          group = 0;
          symbols = 0;
          for (int plh = 0; plh < 90; plh++) {
            out[produced++] = m_pl[plh];
            out[produced++] = m_zero;
          }
          for (int vlh = 0; vlh < VLSNR_HEADER_LENGTH; vlh++) {
            out[produced++] = m_vlsnr_header[vlh];
            out[produced++] = m_zero;
          }
          for (int j = 0; j < slots; j++) {
            for (int k = 0; k < 90; k++) {
              tempin = in[consumed++];
              if (signal_constellation == MOD_QPSK) {
                switch (m_cscram[n++]) {
                  case 0:
                    tempout = tempin;
                    break;
                  case 1:
                    tempout = gr_complex(-tempin.imag(),  tempin.real());
                    break;
                  case 2:
                    tempout = -tempin;
                    break;
                  case 3:
                    tempout = gr_complex( tempin.imag(), -tempin.real());
                    break;
                }
              }
              else {
                switch (m_cscram[n++]) {
                  case 0:
                    tempout = tempin;
                    break;
                  case 1:
                    tempout = -tempin;
                    break;
                  case 2:
                    tempout = tempin;
                    break;
                  case 3:
                    tempout = -tempin;
                    break;
                }
              }
              out[produced++] = tempout;
              out[produced++] = m_zero;
              symbols++;
              if (group <= 18 && symbols == 703) {
                for (int p = 0; p < 34; p++) {
                  tempin = m_bpsk[0][0];
                  switch (m_cscram[n++]) {
                    case 0:
                      tempout = tempin;
                      break;
                    case 1:
                      tempout = gr_complex(-tempin.imag(),  tempin.real());
                      break;
                    case 2:
                      tempout = -tempin;
                      break;
                    case 3:
                      tempout = gr_complex( tempin.imag(), -tempin.real());
                      break;
                  }
                  out[produced++] = tempout;
                  out[produced++] = m_zero;
                }
                for (int x = (k + 1 + 34) - 90; x < 90; x++) {
                  tempin = in[consumed++];
                  if (signal_constellation == MOD_QPSK) {
                    switch (m_cscram[n++]) {
                      case 0:
                        tempout = tempin;
                        break;
                      case 1:
                        tempout = gr_complex(-tempin.imag(),  tempin.real());
                        break;
                      case 2:
                        tempout = -tempin;
                        break;
                      case 3:
                        tempout = gr_complex( tempin.imag(), -tempin.real());
                        break;
                    }
                  }
                  else {
                    switch (m_cscram[n++]) {
                      case 0:
                        tempout = tempin;
                        break;
                      case 1:
                        tempout = -tempin;
                        break;
                      case 2:
                        tempout = tempin;
                        break;
                      case 3:
                        tempout = -tempin;
                        break;
                    }
                  }
                  out[produced++] = tempout;
                  out[produced++] = m_zero;
                  symbols++;
                }
                slot_count = (slot_count + 1) % 16;
                j++;
                break;
              }
              else if ((group > 18 && group <= 21) && symbols == 702) {
                for (int p = 0; p < 36; p++) {
                  tempin = m_bpsk[0][0];
                  switch (m_cscram[n++]) {
                    case 0:
                      tempout = tempin;
                      break;
                    case 1:
                      tempout = gr_complex(-tempin.imag(),  tempin.real());
                      break;
                    case 2:
                      tempout = -tempin;
                      break;
                    case 3:
                      tempout = gr_complex( tempin.imag(), -tempin.real());
                      break;
                  }
                  out[produced++] = tempout;
                  out[produced++] = m_zero;
                }
                for (int x = (k + 1 + 36) - 90; x < 90; x++) {
                  tempin = in[consumed++];
                  if (signal_constellation == MOD_QPSK) {
                    switch (m_cscram[n++]) {
                      case 0:
                        tempout = tempin;
                        break;
                      case 1:
                        tempout = gr_complex(-tempin.imag(),  tempin.real());
                        break;
                      case 2:
                        tempout = -tempin;
                        break;
                      case 3:
                        tempout = gr_complex( tempin.imag(), -tempin.real());
                        break;
                    }
                  }
                  else {
                    switch (m_cscram[n++]) {
                      case 0:
                        tempout = tempin;
                        break;
                      case 1:
                        tempout = -tempin;
                        break;
                      case 2:
                        tempout = tempin;
                        break;
                      case 3:
                        tempout = -tempin;
                        break;
                    }
                  }
                  out[produced++] = tempout;
                  out[produced++] = m_zero;
                  symbols++;
                }
                slot_count = (slot_count + 1) % 16;
                j++;
                break;
              }
            }
            slot_count = (slot_count + 1) % 16;
            if ((slot_count == 0) && (j < slots - 1)) {
              if (pilot_mode) {
                // Add pilots if needed
                group++;
                symbols = 0;
                for (int p = 0; p < 36; p++) {
                  tempin = m_bpsk[0][0];
                  switch (m_cscram[n++]) {
                    case 0:
                      tempout = tempin;
                      break;
                    case 1:
                      tempout = gr_complex(-tempin.imag(),  tempin.real());
                      break;
                    case 2:
                      tempout = -tempin;
                      break;
                    case 3:
                      tempout = gr_complex( tempin.imag(), -tempin.real());
                      break;
                  }
                  out[produced++] = tempout;
                  out[produced++] = m_zero;
                }
              }
            }
          }
        }
      }
      else {    /* VL-SNR set 2 */
        for (int i = 0; i < noutput_items / 2; i += (((slots * 90) + 90) + pilot_symbols + VLSNR_HEADER_LENGTH + 36)) {
          n = 0;
          slot_count = 10;
          group = 0;
          symbols = 0;
          for (int plh = 0; plh < 90; plh++) {
            out[produced++] = m_pl[plh];
            out[produced++] = m_zero;
          }
          for (int vlh = 0; vlh < VLSNR_HEADER_LENGTH; vlh++) {
            out[produced++] = m_vlsnr_header[vlh];
            out[produced++] = m_zero;
          }
          for (int j = 0; j < slots; j++) {
            for (int k = 0; k < 90; k++) {
              tempin = in[consumed++];
              switch (m_cscram[n++]) {
                case 0:
                  tempout = tempin;
                  break;
                case 1:
                  tempout = -tempin;
                  break;
                case 2:
                  tempout = tempin;
                  break;
                case 3:
                  tempout = -tempin;
                  break;
              }
              out[produced++] = tempout;
              out[produced++] = m_zero;
              symbols++;
              if (group <= 9 && symbols == 704) {
                for (int p = 0; p < 32; p++) {
                  tempin = m_bpsk[0][0];
                  switch (m_cscram[n++]) {
                    case 0:
                      tempout = tempin;
                      break;
                    case 1:
                      tempout = gr_complex(-tempin.imag(),  tempin.real());
                      break;
                    case 2:
                      tempout = -tempin;
                      break;
                    case 3:
                      tempout = gr_complex( tempin.imag(), -tempin.real());
                      break;
                  }
                  out[produced++] = tempout;
                  out[produced++] = m_zero;
                }
                for (int x = (k + 1 + 32) - 90; x < 90; x++) {
                  tempin = in[consumed++];
                  switch (m_cscram[n++]) {
                    case 0:
                      tempout = tempin;
                      break;
                    case 1:
                      tempout = -tempin;
                      break;
                    case 2:
                      tempout = tempin;
                      break;
                    case 3:
                      tempout = -tempin;
                      break;
                  }
                  out[produced++] = tempout;
                  out[produced++] = m_zero;
                  symbols++;
                }
                slot_count = (slot_count + 1) % 16;
                j++;
                break;
              }
              else if ((group == 10) && symbols == 702) {
                for (int p = 0; p < 36; p++) {
                  tempin = m_bpsk[0][0];
                  switch (m_cscram[n++]) {
                    case 0:
                      tempout = tempin;
                      break;
                    case 1:
                      tempout = gr_complex(-tempin.imag(),  tempin.real());
                      break;
                    case 2:
                      tempout = -tempin;
                      break;
                    case 3:
                      tempout = gr_complex( tempin.imag(), -tempin.real());
                      break;
                  }
                  out[produced++] = tempout;
                  out[produced++] = m_zero;
                }
                for (int x = (k + 1 + 36) - 90; x < 90; x++) {
                  tempin = in[consumed++];
                  switch (m_cscram[n++]) {
                    case 0:
                      tempout = tempin;
                      break;
                    case 1:
                      tempout = -tempin;
                      break;
                    case 2:
                      tempout = tempin;
                      break;
                    case 3:
                      tempout = -tempin;
                      break;
                  }
                  out[produced++] = tempout;
                  out[produced++] = m_zero;
                  symbols++;
                }
                slot_count = (slot_count + 1) % 16;
                j++;
                break;
              }
            }
            slot_count = (slot_count + 1) % 16;
            if ((slot_count == 0) && (j < slots - 1)) {
              if (pilot_mode) {
                // Add pilots if needed
                group++;
                symbols = 0;
                for (int p = 0; p < 36; p++) {
                  tempin = m_bpsk[0][0];
                  switch (m_cscram[n++]) {
                    case 0:
                      tempout = tempin;
                      break;
                    case 1:
                      tempout = gr_complex(-tempin.imag(),  tempin.real());
                      break;
                    case 2:
                      tempout = -tempin;
                      break;
                    case 3:
                      tempout = gr_complex( tempin.imag(), -tempin.real());
                      break;
                  }
                  out[produced++] = tempout;
                  out[produced++] = m_zero;
                }
              }
            }
          }
        }
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (consumed);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    const unsigned long dvbs2_physical_cc_impl::g[7] =
    {
      0x90AC2DDD, 0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF, 0x0000FFFF, 0xFFFFFFFF
    };

    const int dvbs2_physical_cc_impl::ph_scram_tab[64] =
    {
      0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1,
      0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0
    };

    const int dvbs2_physical_cc_impl::ph_sync_seq[26] =
    {
      0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0
    };

    const int dvbs2_physical_cc_impl::ph_vlsnr_seq[16][896] =
    {
      {1,1,1,1,1,0,1,1,1,1,1,1,0,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,0,0,1,0,0,
       1,0,0,1,1,0,0,0,0,1,1,1,0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0,1,0,1,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,0,0,0,1,0,1,1,1,1,0,
       1,1,1,1,0,1,1,0,1,0,1,0,0,0,1,0,1,1,0,0,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,0,1,1,0,0,0,1,0,1,1,1,0,0,1,1,0,1,1,1,
       1,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,1,0,1,1,0,0,1,0,1,0,1,1,0,1,0,0,1,1,0,1,1,1,1,1,0,0,1,1,0,0,1,0,1,1,1,1,0,1,0,
       0,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,0,1,1,0,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,1,1,0,0,1,0,0,1,1,1,1,0,1,0,1,0,
       0,1,0,1,1,1,1,0,0,1,1,1,1,0,0,0,1,0,1,1,1,0,1,0,0,0,0,0,0,0,1,1,1,0,1,0,0,1,1,0,1,1,0,1,0,1,0,1,0,0,0,1,1,0,1,0,
       0,0,1,0,0,1,1,1,1,0,0,1,1,1,0,0,1,1,0,0,0,0,1,0,0,1,1,0,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,0,1,1,0,0,1,1,0,1,0,0,0,0,
       0,0,1,1,0,1,0,0,0,0,1,0,1,0,1,1,0,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,1,0,1,1,1,1,1,1,0,0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,
       1,0,1,0,1,1,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,1,1,0,1,1,1,0,1,0,0,1,1,1,0,1,0,1,0,1,0,0,1,1,0,0,0,1,0,0,1,0,1,1,1,1,
       0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,0,1,1,0,1,1,0,1,1,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0,0,1,1,0,1,1,1,
       0,1,1,1,0,0,1,0,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,1,0,0,1,1,0,0,0,1,1,1,
       0,0,1,1,1,0,1,1,1,1,0,1,0,1,0,1,1,0,1,0,1,1,0,0,1,1,1,0,1,1,1,0,0,0,1,0,0,1,0,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,
       0,1,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,0,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,1,0,1,0,1,1,0,1,0,1,1,0,1,1,0,1,0,
       1,1,1,0,1,0,0,1,1,0,1,0,1,1,1,1,0,0,0,0,0,0,0,1,0,1,1,1,0,0,1,0,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,1,1,0,1,0,0,1,1,1,
       0,0,1,1,1,1,1,1,0,1,0,0,1,0,0,0,0,0,1,1,0,1,0,1,1,0,1,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,
       0,0,1,0,0,0,1,1,1,1,0,0,1,0,0,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,0,0,1,1,1,1,0,0,1,0,1,1,1,0,1,1,0,1,0,1,0,0,0,0,0,1},

      {1,1,1,1,1,0,1,1,1,1,1,1,0,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,0,0,1,0,0,
       0,1,1,0,0,1,1,1,1,0,0,0,1,1,1,1,0,1,1,1,0,0,0,1,1,1,1,1,0,1,0,0,1,1,0,0,0,1,1,0,1,1,0,0,1,0,1,1,1,0,1,0,0,0,0,1,
       1,1,1,1,0,1,1,0,1,0,1,0,0,0,1,0,1,1,0,0,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,0,1,1,0,0,0,1,0,1,1,1,0,0,1,1,0,1,1,1,
       0,1,1,1,1,0,1,1,1,1,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,0,1,0,0,1,0,1,1,0,0,1,0,0,0,0,0,1,1,0,0,1,1,0,1,0,0,0,0,1,0,1,
       0,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,0,1,1,0,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,1,1,0,0,1,0,0,1,1,1,1,0,1,0,1,0,
       1,0,1,0,0,0,0,1,1,0,0,0,0,1,1,1,0,1,0,0,0,1,0,1,1,1,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,1,0,1,
       0,0,1,0,0,1,1,1,1,0,0,1,1,1,0,0,1,1,0,0,0,0,1,0,0,1,1,0,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,0,1,1,0,0,1,1,0,1,0,0,0,0,
       1,1,0,0,1,0,1,1,1,1,0,1,0,1,0,0,1,1,1,1,1,0,1,1,0,1,1,0,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,1,0,
       1,0,1,0,1,1,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,1,1,0,1,1,1,0,1,0,0,1,1,1,0,1,0,1,0,1,0,0,1,1,0,0,0,1,0,0,1,0,1,1,1,1,
       1,1,1,0,1,1,1,1,1,0,0,1,1,1,1,0,0,0,1,1,1,0,0,1,0,0,1,0,0,0,0,0,0,1,1,1,1,1,0,1,1,0,0,1,1,1,0,1,1,1,0,0,1,0,0,0,
       0,1,1,1,0,0,1,0,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,1,0,0,1,1,0,0,0,1,1,1,
       1,1,0,0,0,1,0,0,0,0,1,0,1,0,1,0,0,1,0,1,0,0,1,1,0,0,0,1,0,0,0,1,1,1,0,1,1,0,1,0,0,0,0,1,1,1,0,1,0,0,1,1,0,1,1,0,
       0,1,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,0,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,1,0,1,0,1,1,0,1,0,1,1,0,1,1,0,1,0,
       0,0,0,1,0,1,1,0,0,1,0,1,0,0,0,0,1,1,1,1,1,1,1,0,1,0,0,0,1,1,0,1,0,0,1,1,0,0,0,0,0,1,1,0,0,0,1,0,0,1,0,1,1,0,0,0,
       0,0,1,1,1,1,1,1,0,1,0,0,1,0,0,0,0,0,1,1,0,1,0,1,1,0,1,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,
       1,1,0,1,1,1,0,0,0,0,1,1,0,1,1,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,1,1,0,0,0,0,1,1,0,1,0,0,0,1,0,0,1,0,1,0,1,1,1,1,1,0},

      {1,1,1,1,1,0,1,1,1,1,1,1,0,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,0,0,1,0,0,
       1,0,0,1,1,0,0,0,0,1,1,1,0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0,1,0,1,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,0,0,0,1,0,1,1,1,1,0,
       0,0,0,0,1,0,0,1,0,1,0,1,1,1,0,1,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,0,0,1,1,1,0,1,0,0,0,1,1,0,0,1,0,0,0,
       0,1,1,1,1,0,1,1,1,1,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,0,1,0,0,1,0,1,1,0,0,1,0,0,0,0,0,1,1,0,0,1,1,0,1,0,0,0,0,1,0,1,
       0,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,0,1,1,0,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,1,1,0,0,1,0,0,1,1,1,1,0,1,0,1,0,
       0,1,0,1,1,1,1,0,0,1,1,1,1,0,0,0,1,0,1,1,1,0,1,0,0,0,0,0,0,0,1,1,1,0,1,0,0,1,1,0,1,1,0,1,0,1,0,1,0,0,0,1,1,0,1,0,
       1,1,0,1,1,0,0,0,0,1,1,0,0,0,1,1,0,0,1,1,1,1,0,1,1,0,0,1,1,0,1,0,1,0,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,1,0,1,1,1,1,
       1,1,0,0,1,0,1,1,1,1,0,1,0,1,0,0,1,1,1,1,1,0,1,1,0,1,1,0,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,1,0,
       1,0,1,0,1,1,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,1,1,0,1,1,1,0,1,0,0,1,1,1,0,1,0,1,0,1,0,0,1,1,0,0,0,1,0,0,1,0,1,1,1,1,
       0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,0,1,1,0,1,1,0,1,1,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0,0,1,1,0,1,1,1,
       1,0,0,0,1,1,0,1,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,0,1,1,0,0,1,1,1,0,0,0,
       1,1,0,0,0,1,0,0,0,0,1,0,1,0,1,0,0,1,0,1,0,0,1,1,0,0,0,1,0,0,0,1,1,1,0,1,1,0,1,0,0,0,0,1,1,1,0,1,0,0,1,1,0,1,1,0,
       0,1,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,0,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,1,0,1,0,1,1,0,1,0,1,1,0,1,1,0,1,0,
       1,1,1,0,1,0,0,1,1,0,1,0,1,1,1,1,0,0,0,0,0,0,0,1,0,1,1,1,0,0,1,0,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,1,1,0,1,0,0,1,1,1,
       1,1,0,0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,0,0,1,0,1,0,0,1,0,1,1,0,1,1,1,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,
       1,1,0,1,1,1,0,0,0,0,1,1,0,1,1,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,1,1,0,0,0,0,1,1,0,1,0,0,0,1,0,0,1,0,1,0,1,1,1,1,1,0},

      {1,1,1,1,1,0,1,1,1,1,1,1,0,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,0,0,1,0,0,
       0,1,1,0,0,1,1,1,1,0,0,0,1,1,1,1,0,1,1,1,0,0,0,1,1,1,1,1,0,1,0,0,1,1,0,0,0,1,1,0,1,1,0,0,1,0,1,1,1,0,1,0,0,0,0,1,
       0,0,0,0,1,0,0,1,0,1,0,1,1,1,0,1,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,0,0,1,1,1,0,1,0,0,0,1,1,0,0,1,0,0,0,
       1,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,1,0,1,1,0,0,1,0,1,0,1,1,0,1,0,0,1,1,0,1,1,1,1,1,0,0,1,1,0,0,1,0,1,1,1,1,0,1,0,
       0,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,0,1,1,0,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,1,1,0,0,1,0,0,1,1,1,1,0,1,0,1,0,
       1,0,1,0,0,0,0,1,1,0,0,0,0,1,1,1,0,1,0,0,0,1,0,1,1,1,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,1,0,1,
       1,1,0,1,1,0,0,0,0,1,1,0,0,0,1,1,0,0,1,1,1,1,0,1,1,0,0,1,1,0,1,0,1,0,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,1,0,1,1,1,1,
       0,0,1,1,0,1,0,0,0,0,1,0,1,0,1,1,0,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,1,0,1,1,1,1,1,1,0,0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,
       1,0,1,0,1,1,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,1,1,0,1,1,1,0,1,0,0,1,1,1,0,1,0,1,0,1,0,0,1,1,0,0,0,1,0,0,1,0,1,1,1,1,
       1,1,1,0,1,1,1,1,1,0,0,1,1,1,1,0,0,0,1,1,1,0,0,1,0,0,1,0,0,0,0,0,0,1,1,1,1,1,0,1,1,0,0,1,1,1,0,1,1,1,0,0,1,0,0,0,
       1,0,0,0,1,1,0,1,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,0,1,1,0,0,1,1,1,0,0,0,
       0,0,1,1,1,0,1,1,1,1,0,1,0,1,0,1,1,0,1,0,1,1,0,0,1,1,1,0,1,1,1,0,0,0,1,0,0,1,0,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,
       0,1,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,0,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,1,0,1,0,1,1,0,1,0,1,1,0,1,1,0,1,0,
       0,0,0,1,0,1,1,0,0,1,0,1,0,0,0,0,1,1,1,1,1,1,1,0,1,0,0,0,1,1,0,1,0,0,1,1,0,0,0,0,0,1,1,0,0,0,1,0,0,1,0,1,1,0,0,0,
       1,1,0,0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,0,0,1,0,1,0,0,1,0,1,1,0,1,1,1,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,
       0,0,1,0,0,0,1,1,1,1,0,0,1,0,0,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,0,0,1,1,1,1,0,0,1,0,1,1,1,0,1,1,0,1,0,1,0,0,0,0,0,1},

      {1,1,1,1,1,0,1,1,1,1,1,1,0,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,0,0,1,0,0,
       1,0,0,1,1,0,0,0,0,1,1,1,0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0,1,0,1,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,0,0,0,1,0,1,1,1,1,0,
       1,1,1,1,0,1,1,0,1,0,1,0,0,0,1,0,1,1,0,0,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,0,1,1,0,0,0,1,0,1,1,1,0,0,1,1,0,1,1,1,
       1,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,1,0,1,1,0,0,1,0,1,0,1,1,0,1,0,0,1,1,0,1,1,1,1,1,0,0,1,1,0,0,1,0,1,1,1,1,0,1,0,
       1,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,0,1,0,1,
       1,0,1,0,0,0,0,1,1,0,0,0,0,1,1,1,0,1,0,0,0,1,0,1,1,1,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,1,0,1,
       1,1,0,1,1,0,0,0,0,1,1,0,0,0,1,1,0,0,1,1,1,1,0,1,1,0,0,1,1,0,1,0,1,0,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,1,0,1,1,1,1,
       1,1,0,0,1,0,1,1,1,1,0,1,0,1,0,0,1,1,1,1,1,0,1,1,0,1,1,0,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,1,0,
       1,0,1,0,1,1,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,1,1,0,1,1,1,0,1,0,0,1,1,1,0,1,0,1,0,1,0,0,1,1,0,0,0,1,0,0,1,0,1,1,1,1,
       0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,0,1,1,0,1,1,0,1,1,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0,0,1,1,0,1,1,1,
       0,1,1,1,0,0,1,0,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,1,0,0,1,1,0,0,0,1,1,1,
       0,0,1,1,1,0,1,1,1,1,0,1,0,1,0,1,1,0,1,0,1,1,0,0,1,1,1,0,1,1,1,0,0,0,1,0,0,1,0,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,
       1,0,1,0,0,1,1,0,1,1,1,1,0,1,1,1,1,0,0,0,0,0,1,0,0,1,1,1,1,1,0,1,1,0,0,1,1,1,1,0,1,0,1,0,0,1,0,1,0,0,1,0,0,1,0,1,
       0,0,0,1,0,1,1,0,0,1,0,1,0,0,0,0,1,1,1,1,1,1,1,0,1,0,0,0,1,1,0,1,0,0,1,1,0,0,0,0,0,1,1,0,0,0,1,0,0,1,0,1,1,0,0,0,
       1,1,0,0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,0,0,1,0,1,0,0,1,0,1,1,0,1,1,1,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,
       1,1,0,1,1,1,0,0,0,0,1,1,0,1,1,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,1,1,0,0,0,0,1,1,0,1,0,0,0,1,0,0,1,0,1,0,1,1,1,1,1,0},

      {1,1,1,1,1,0,1,1,1,1,1,1,0,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,0,0,1,0,0,
       0,1,1,0,0,1,1,1,1,0,0,0,1,1,1,1,0,1,1,1,0,0,0,1,1,1,1,1,0,1,0,0,1,1,0,0,0,1,1,0,1,1,0,0,1,0,1,1,1,0,1,0,0,0,0,1,
       0,0,0,0,1,0,0,1,0,1,0,1,1,1,0,1,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,0,0,1,1,1,0,1,0,0,0,1,1,0,0,1,0,0,0,
       1,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,1,0,1,1,0,0,1,0,1,0,1,1,0,1,0,0,1,1,0,1,1,1,1,1,0,0,1,1,0,0,1,0,1,1,1,1,0,1,0,
       1,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,0,1,0,1,
       0,1,0,1,1,1,1,0,0,1,1,1,1,0,0,0,1,0,1,1,1,0,1,0,0,0,0,0,0,0,1,1,1,0,1,0,0,1,1,0,1,1,0,1,0,1,0,1,0,0,0,1,1,0,1,0,
       0,0,1,0,0,1,1,1,1,0,0,1,1,1,0,0,1,1,0,0,0,0,1,0,0,1,1,0,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,0,1,1,0,0,1,1,0,1,0,0,0,0,
       1,1,0,0,1,0,1,1,1,1,0,1,0,1,0,0,1,1,1,1,1,0,1,1,0,1,1,0,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,1,0,
       1,0,1,0,1,1,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,1,1,0,1,1,1,0,1,0,0,1,1,1,0,1,0,1,0,1,0,0,1,1,0,0,0,1,0,0,1,0,1,1,1,1,
       1,1,1,0,1,1,1,1,1,0,0,1,1,1,1,0,0,0,1,1,1,0,0,1,0,0,1,0,0,0,0,0,0,1,1,1,1,1,0,1,1,0,0,1,1,1,0,1,1,1,0,0,1,0,0,0,
       1,0,0,0,1,1,0,1,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,0,1,1,0,0,1,1,1,0,0,0,
       0,0,1,1,1,0,1,1,1,1,0,1,0,1,0,1,1,0,1,0,1,1,0,0,1,1,1,0,1,1,1,0,0,0,1,0,0,1,0,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,
       1,0,1,0,0,1,1,0,1,1,1,1,0,1,1,1,1,0,0,0,0,0,1,0,0,1,1,1,1,1,0,1,1,0,0,1,1,1,1,0,1,0,1,0,0,1,0,1,0,0,1,0,0,1,0,1,
       1,1,1,0,1,0,0,1,1,0,1,0,1,1,1,1,0,0,0,0,0,0,0,1,0,1,1,1,0,0,1,0,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,1,1,0,1,0,0,1,1,1,
       0,0,1,1,1,1,1,1,0,1,0,0,1,0,0,0,0,0,1,1,0,1,0,1,1,0,1,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,
       1,1,0,1,1,1,0,0,0,0,1,1,0,1,1,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,1,1,0,0,0,0,1,1,0,1,0,0,0,1,0,0,1,0,1,0,1,1,1,1,1,0},

      {1,1,1,1,1,0,1,1,1,1,1,1,0,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,0,0,1,0,0,
       1,0,0,1,1,0,0,0,0,1,1,1,0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0,1,0,1,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,0,0,0,1,0,1,1,1,1,0,
       0,0,0,0,1,0,0,1,0,1,0,1,1,1,0,1,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,0,0,1,1,1,0,1,0,0,0,1,1,0,0,1,0,0,0,
       0,1,1,1,1,0,1,1,1,1,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,0,1,0,0,1,0,1,1,0,0,1,0,0,0,0,0,1,1,0,0,1,1,0,1,0,0,0,0,1,0,1,
       0,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,0,1,1,0,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,1,1,0,0,1,0,0,1,1,1,1,0,1,0,1,0,
       0,1,0,1,1,1,1,0,0,1,1,1,1,0,0,0,1,0,1,1,1,0,1,0,0,0,0,0,0,0,1,1,1,0,1,0,0,1,1,0,1,1,0,1,0,1,0,1,0,0,0,1,1,0,1,0,
       1,1,0,1,1,0,0,0,0,1,1,0,0,0,1,1,0,0,1,1,1,1,0,1,1,0,0,1,1,0,1,0,1,0,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,1,0,1,1,1,1,
       1,1,0,0,1,0,1,1,1,1,0,1,0,1,0,0,1,1,1,1,1,0,1,1,0,1,1,0,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,1,0,
       0,1,0,1,0,0,1,0,0,0,1,0,1,1,1,1,1,1,0,0,1,0,0,1,0,0,0,1,0,1,1,0,0,0,1,0,1,0,1,0,1,1,0,0,1,1,1,0,1,1,0,1,0,0,0,0,
       1,1,1,0,1,1,1,1,1,0,0,1,1,1,1,0,0,0,1,1,1,0,0,1,0,0,1,0,0,0,0,0,0,1,1,1,1,1,0,1,1,0,0,1,1,1,0,1,1,1,0,0,1,0,0,0,
       0,1,1,1,0,0,1,0,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,1,0,0,1,1,0,0,0,1,1,1,
       0,0,1,1,1,0,1,1,1,1,0,1,0,1,0,1,1,0,1,0,1,1,0,0,1,1,1,0,1,1,1,0,0,0,1,0,0,1,0,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,
       1,0,1,0,0,1,1,0,1,1,1,1,0,1,1,1,1,0,0,0,0,0,1,0,0,1,1,1,1,1,0,1,1,0,0,1,1,1,1,0,1,0,1,0,0,1,0,1,0,0,1,0,0,1,0,1,
       0,0,0,1,0,1,1,0,0,1,0,1,0,0,0,0,1,1,1,1,1,1,1,0,1,0,0,0,1,1,0,1,0,0,1,1,0,0,0,0,0,1,1,0,0,0,1,0,0,1,0,1,1,0,0,0,
       0,0,1,1,1,1,1,1,0,1,0,0,1,0,0,0,0,0,1,1,0,1,0,1,1,0,1,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,
       0,0,1,0,0,0,1,1,1,1,0,0,1,0,0,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,0,0,1,1,1,1,0,0,1,0,1,1,1,0,1,1,0,1,0,1,0,0,0,0,0,1},

      {1,1,1,1,1,0,1,1,1,1,1,1,0,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,0,0,1,0,0,
       0,1,1,0,0,1,1,1,1,0,0,0,1,1,1,1,0,1,1,1,0,0,0,1,1,1,1,1,0,1,0,0,1,1,0,0,0,1,1,0,1,1,0,0,1,0,1,1,1,0,1,0,0,0,0,1,
       0,0,0,0,1,0,0,1,0,1,0,1,1,1,0,1,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,0,0,1,1,1,0,1,0,0,0,1,1,0,0,1,0,0,0,
       1,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,1,0,1,1,0,0,1,0,1,0,1,1,0,1,0,0,1,1,0,1,1,1,1,1,0,0,1,1,0,0,1,0,1,1,1,1,0,1,0,
       0,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,0,1,1,0,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,1,1,0,0,1,0,0,1,1,1,1,0,1,0,1,0,
       1,0,1,0,0,0,0,1,1,0,0,0,0,1,1,1,0,1,0,0,0,1,0,1,1,1,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,1,0,1,
       1,1,0,1,1,0,0,0,0,1,1,0,0,0,1,1,0,0,1,1,1,1,0,1,1,0,0,1,1,0,1,0,1,0,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,1,0,1,1,1,1,
       0,0,1,1,0,1,0,0,0,0,1,0,1,0,1,1,0,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,1,0,1,1,1,1,1,1,0,0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,
       0,1,0,1,0,0,1,0,0,0,1,0,1,1,1,1,1,1,0,0,1,0,0,1,0,0,0,1,0,1,1,0,0,0,1,0,1,0,1,0,1,1,0,0,1,1,1,0,1,1,0,1,0,0,0,0,
       0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,0,1,1,0,1,1,0,1,1,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0,0,1,1,0,1,1,1,
       0,1,1,1,0,0,1,0,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,1,0,0,1,1,0,0,0,1,1,1,
       1,1,0,0,0,1,0,0,0,0,1,0,1,0,1,0,0,1,0,1,0,0,1,1,0,0,0,1,0,0,0,1,1,1,0,1,1,0,1,0,0,0,0,1,1,1,0,1,0,0,1,1,0,1,1,0,
       1,0,1,0,0,1,1,0,1,1,1,1,0,1,1,1,1,0,0,0,0,0,1,0,0,1,1,1,1,1,0,1,1,0,0,1,1,1,1,0,1,0,1,0,0,1,0,1,0,0,1,0,0,1,0,1,
       1,1,1,0,1,0,0,1,1,0,1,0,1,1,1,1,0,0,0,0,0,0,0,1,0,1,1,1,0,0,1,0,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,1,1,0,1,0,0,1,1,1,
       0,0,1,1,1,1,1,1,0,1,0,0,1,0,0,0,0,0,1,1,0,1,0,1,1,0,1,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,
       1,1,0,1,1,1,0,0,0,0,1,1,0,1,1,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,1,1,0,0,0,0,1,1,0,1,0,0,0,1,0,0,1,0,1,0,1,1,1,1,1,0},

      {1,1,1,1,1,0,1,1,1,1,1,1,0,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,0,0,1,0,0,
       1,0,0,1,1,0,0,0,0,1,1,1,0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0,1,0,1,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,0,0,0,1,0,1,1,1,1,0,
       1,1,1,1,0,1,1,0,1,0,1,0,0,0,1,0,1,1,0,0,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,0,1,1,0,0,0,1,0,1,1,1,0,0,1,1,0,1,1,1,
       1,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,1,0,1,1,0,0,1,0,1,0,1,1,0,1,0,0,1,1,0,1,1,1,1,1,0,0,1,1,0,0,1,0,1,1,1,1,0,1,0,
       1,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,0,1,0,1,
       1,0,1,0,0,0,0,1,1,0,0,0,0,1,1,1,0,1,0,0,0,1,0,1,1,1,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,1,0,1,
       1,1,0,1,1,0,0,0,0,1,1,0,0,0,1,1,0,0,1,1,1,1,0,1,1,0,0,1,1,0,1,0,1,0,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,1,0,1,1,1,1,
       1,1,0,0,1,0,1,1,1,1,0,1,0,1,0,0,1,1,1,1,1,0,1,1,0,1,1,0,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,1,0,
       0,1,0,1,0,0,1,0,0,0,1,0,1,1,1,1,1,1,0,0,1,0,0,1,0,0,0,1,0,1,1,0,0,0,1,0,1,0,1,0,1,1,0,0,1,1,1,0,1,1,0,1,0,0,0,0,
       1,1,1,0,1,1,1,1,1,0,0,1,1,1,1,0,0,0,1,1,1,0,0,1,0,0,1,0,0,0,0,0,0,1,1,1,1,1,0,1,1,0,0,1,1,1,0,1,1,1,0,0,1,0,0,0,
       1,0,0,0,1,1,0,1,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,0,1,1,0,0,1,1,1,0,0,0,
       1,1,0,0,0,1,0,0,0,0,1,0,1,0,1,0,0,1,0,1,0,0,1,1,0,0,0,1,0,0,0,1,1,1,0,1,1,0,1,0,0,0,0,1,1,1,0,1,0,0,1,1,0,1,1,0,
       0,1,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,0,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,1,0,1,0,1,1,0,1,0,1,1,0,1,1,0,1,0,
       1,1,1,0,1,0,0,1,1,0,1,0,1,1,1,1,0,0,0,0,0,0,0,1,0,1,1,1,0,0,1,0,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,1,1,0,1,0,0,1,1,1,
       0,0,1,1,1,1,1,1,0,1,0,0,1,0,0,0,0,0,1,1,0,1,0,1,1,0,1,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,
       0,0,1,0,0,0,1,1,1,1,0,0,1,0,0,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,0,0,1,1,1,1,0,0,1,0,1,1,1,0,1,1,0,1,0,1,0,0,0,0,0,1},

      {1,1,1,1,1,0,1,1,1,1,1,1,0,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,0,0,1,0,0,
       1,0,0,1,1,0,0,0,0,1,1,1,0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0,1,0,1,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,0,0,0,1,0,1,1,1,1,0,
       0,0,0,0,1,0,0,1,0,1,0,1,1,1,0,1,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,0,0,1,1,1,0,1,0,0,0,1,1,0,0,1,0,0,0,
       0,1,1,1,1,0,1,1,1,1,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,0,1,0,0,1,0,1,1,0,0,1,0,0,0,0,0,1,1,0,0,1,1,0,1,0,0,0,0,1,0,1,
       1,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,0,1,0,1,
       1,0,1,0,0,0,0,1,1,0,0,0,0,1,1,1,0,1,0,0,0,1,0,1,1,1,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,1,0,1,
       0,0,1,0,0,1,1,1,1,0,0,1,1,1,0,0,1,1,0,0,0,0,1,0,0,1,1,0,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,0,1,1,0,0,1,1,0,1,0,0,0,0,
       0,0,1,1,0,1,0,0,0,0,1,0,1,0,1,1,0,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,1,0,1,1,1,1,1,1,0,0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,
       1,0,1,0,1,1,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,1,1,0,1,1,1,0,1,0,0,1,1,1,0,1,0,1,0,1,0,0,1,1,0,0,0,1,0,0,1,0,1,1,1,1,
       0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,0,1,1,0,1,1,0,1,1,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0,0,1,1,0,1,1,1,
       1,0,0,0,1,1,0,1,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,0,1,1,0,0,1,1,1,0,0,0,
       1,1,0,0,0,1,0,0,0,0,1,0,1,0,1,0,0,1,0,1,0,0,1,1,0,0,0,1,0,0,0,1,1,1,0,1,1,0,1,0,0,0,0,1,1,1,0,1,0,0,1,1,0,1,1,0,
       1,0,1,0,0,1,1,0,1,1,1,1,0,1,1,1,1,0,0,0,0,0,1,0,0,1,1,1,1,1,0,1,1,0,0,1,1,1,1,0,1,0,1,0,0,1,0,1,0,0,1,0,0,1,0,1,
       0,0,0,1,0,1,1,0,0,1,0,1,0,0,0,0,1,1,1,1,1,1,1,0,1,0,0,0,1,1,0,1,0,0,1,1,0,0,0,0,0,1,1,0,0,0,1,0,0,1,0,1,1,0,0,0,
       0,0,1,1,1,1,1,1,0,1,0,0,1,0,0,0,0,0,1,1,0,1,0,1,1,0,1,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,
       0,0,1,0,0,0,1,1,1,1,0,0,1,0,0,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,0,0,1,1,1,1,0,0,1,0,1,1,1,0,1,1,0,1,0,1,0,0,0,0,0,1},

      {1,1,1,1,1,0,1,1,1,1,1,1,0,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,0,0,1,0,0,
       0,1,1,0,0,1,1,1,1,0,0,0,1,1,1,1,0,1,1,1,0,0,0,1,1,1,1,1,0,1,0,0,1,1,0,0,0,1,1,0,1,1,0,0,1,0,1,1,1,0,1,0,0,0,0,1,
       0,0,0,0,1,0,0,1,0,1,0,1,1,1,0,1,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,0,0,1,1,1,0,1,0,0,0,1,1,0,0,1,0,0,0,
       1,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,1,0,1,1,0,0,1,0,1,0,1,1,0,1,0,0,1,1,0,1,1,1,1,1,0,0,1,1,0,0,1,0,1,1,1,1,0,1,0,
       0,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,0,1,1,0,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,1,1,0,0,1,0,0,1,1,1,1,0,1,0,1,0,
       1,0,1,0,0,0,0,1,1,0,0,0,0,1,1,1,0,1,0,0,0,1,0,1,1,1,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,1,0,1,
       1,1,0,1,1,0,0,0,0,1,1,0,0,0,1,1,0,0,1,1,1,1,0,1,1,0,0,1,1,0,1,0,1,0,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,1,0,1,1,1,1,
       0,0,1,1,0,1,0,0,0,0,1,0,1,0,1,1,0,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,1,0,1,1,1,1,1,1,0,0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,
       1,0,1,0,1,1,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,1,1,0,1,1,1,0,1,0,0,1,1,1,0,1,0,1,0,1,0,0,1,1,0,0,0,1,0,0,1,0,1,1,1,1,
       1,1,1,0,1,1,1,1,1,0,0,1,1,1,1,0,0,0,1,1,1,0,0,1,0,0,1,0,0,0,0,0,0,1,1,1,1,1,0,1,1,0,0,1,1,1,0,1,1,1,0,0,1,0,0,0,
       1,0,0,0,1,1,0,1,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,0,1,1,0,0,1,1,1,0,0,0,
       0,0,1,1,1,0,1,1,1,1,0,1,0,1,0,1,1,0,1,0,1,1,0,0,1,1,1,0,1,1,1,0,0,0,1,0,0,1,0,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,
       0,1,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,0,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,1,0,1,0,1,1,0,1,0,1,1,0,1,1,0,1,0,
       0,0,0,1,0,1,1,0,0,1,0,1,0,0,0,0,1,1,1,1,1,1,1,0,1,0,0,0,1,1,0,1,0,0,1,1,0,0,0,0,0,1,1,0,0,0,1,0,0,1,0,1,1,0,0,0,
       1,1,0,0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,0,0,1,0,1,0,0,1,0,1,1,0,1,1,1,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,
       0,0,1,0,0,0,1,1,1,1,0,0,1,0,0,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,0,0,1,1,1,1,0,0,1,0,1,1,1,0,1,1,0,1,0,1,0,0,0,0,0,1},

      {0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,1,1,1,0,0,0,0,0,1,0,1,1,1,1,1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,1,1,1,0,1,1,
       0,1,1,0,0,1,1,1,1,0,0,0,1,1,1,1,0,1,1,1,0,0,0,1,1,1,1,1,0,1,0,0,1,1,0,0,0,1,1,0,1,1,0,0,1,0,1,1,1,0,1,0,0,0,0,1,
       0,0,0,0,1,0,0,1,0,1,0,1,1,1,0,1,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,0,0,1,1,1,0,1,0,0,0,1,1,0,0,1,0,0,0,
       0,1,1,1,1,0,1,1,1,1,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,0,1,0,0,1,0,1,1,0,0,1,0,0,0,0,0,1,1,0,0,1,1,0,1,0,0,0,0,1,0,1,
       1,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,0,1,0,1,
       1,0,1,0,0,0,0,1,1,0,0,0,0,1,1,1,0,1,0,0,0,1,0,1,1,1,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,1,0,1,
       1,1,0,1,1,0,0,0,0,1,1,0,0,0,1,1,0,0,1,1,1,1,0,1,1,0,0,1,1,0,1,0,1,0,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,1,0,1,1,1,1,
       1,1,0,0,1,0,1,1,1,1,0,1,0,1,0,0,1,1,1,1,1,0,1,1,0,1,1,0,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,1,0,
       1,0,1,0,1,1,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,1,1,0,1,1,1,0,1,0,0,1,1,1,0,1,0,1,0,1,0,0,1,1,0,0,0,1,0,0,1,0,1,1,1,1,
       0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,0,1,1,0,1,1,0,1,1,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0,0,1,1,0,1,1,1,
       0,1,1,1,0,0,1,0,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,1,0,0,1,1,0,0,0,1,1,1,
       0,0,1,1,1,0,1,1,1,1,0,1,0,1,0,1,1,0,1,0,1,1,0,0,1,1,1,0,1,1,1,0,0,0,1,0,0,1,0,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,
       0,1,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,0,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,1,0,1,0,1,1,0,1,0,1,1,0,1,1,0,1,0,
       1,1,1,0,1,0,0,1,1,0,1,0,1,1,1,1,0,0,0,0,0,0,0,1,0,1,1,1,0,0,1,0,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,1,1,0,1,0,0,1,1,1,
       0,0,1,1,1,1,1,1,0,1,0,0,1,0,0,0,0,0,1,1,0,1,0,1,1,0,1,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,
       0,0,1,0,0,0,1,1,1,1,0,0,1,0,0,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,0,0,1,1,1,1,0,0,1,0,1,1,1,0,1,1,0,1,0,1,0,0,0,0,0,1},

      {1,1,1,1,1,0,1,1,1,1,1,1,0,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,0,0,1,0,0,
       0,1,1,0,0,1,1,1,1,0,0,0,1,1,1,1,0,1,1,1,0,0,0,1,1,1,1,1,0,1,0,0,1,1,0,0,0,1,1,0,1,1,0,0,1,0,1,1,1,0,1,0,0,0,0,1,
       1,1,1,1,0,1,1,0,1,0,1,0,0,0,1,0,1,1,0,0,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,0,1,1,0,0,0,1,0,1,1,1,0,0,1,1,0,1,1,1,
       0,1,1,1,1,0,1,1,1,1,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,0,1,0,0,1,0,1,1,0,0,1,0,0,0,0,0,1,1,0,0,1,1,0,1,0,0,0,0,1,0,1,
       0,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,0,1,1,0,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,1,1,0,0,1,0,0,1,1,1,1,0,1,0,1,0,
       1,0,1,0,0,0,0,1,1,0,0,0,0,1,1,1,0,1,0,0,0,1,0,1,1,1,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,1,0,1,
       0,0,1,0,0,1,1,1,1,0,0,1,1,1,0,0,1,1,0,0,0,0,1,0,0,1,1,0,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,0,1,1,0,0,1,1,0,1,0,0,0,0,
       1,1,0,0,1,0,1,1,1,1,0,1,0,1,0,0,1,1,1,1,1,0,1,1,0,1,1,0,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,1,0,
       0,1,0,1,0,0,1,0,0,0,1,0,1,1,1,1,1,1,0,0,1,0,0,1,0,0,0,1,0,1,1,0,0,0,1,0,1,0,1,0,1,1,0,0,1,1,1,0,1,1,0,1,0,0,0,0,
       0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,0,1,1,0,1,1,0,1,1,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0,0,1,1,0,1,1,1,
       1,0,0,0,1,1,0,1,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,0,1,1,0,0,1,1,1,0,0,0,
       0,0,1,1,1,0,1,1,1,1,0,1,0,1,0,1,1,0,1,0,1,1,0,0,1,1,1,0,1,1,1,0,0,0,1,0,0,1,0,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,
       1,0,1,0,0,1,1,0,1,1,1,1,0,1,1,1,1,0,0,0,0,0,1,0,0,1,1,1,1,1,0,1,1,0,0,1,1,1,1,0,1,0,1,0,0,1,0,1,0,0,1,0,0,1,0,1,
       1,1,1,0,1,0,0,1,1,0,1,0,1,1,1,1,0,0,0,0,0,0,0,1,0,1,1,1,0,0,1,0,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,1,1,0,1,0,0,1,1,1,
       1,1,0,0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,0,0,1,0,1,0,0,1,0,1,1,0,1,1,1,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,
       0,0,1,0,0,0,1,1,1,1,0,0,1,0,0,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,0,0,1,1,1,1,0,0,1,0,1,1,1,0,1,1,0,1,0,1,0,0,0,0,0,1},

      {1,1,1,1,1,0,1,1,1,1,1,1,0,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,0,0,1,0,0,
       0,1,1,0,0,1,1,1,1,0,0,0,1,1,1,1,0,1,1,1,0,0,0,1,1,1,1,1,0,1,0,0,1,1,0,0,0,1,1,0,1,1,0,0,1,0,1,1,1,0,1,0,0,0,0,1,
       1,1,1,1,0,1,1,0,1,0,1,0,0,0,1,0,1,1,0,0,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,0,1,1,0,0,0,1,0,1,1,1,0,0,1,1,0,1,1,1,
       0,1,1,1,1,0,1,1,1,1,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,0,1,0,0,1,0,1,1,0,0,1,0,0,0,0,0,1,1,0,0,1,1,0,1,0,0,0,0,1,0,1,
       1,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,0,1,0,1,
       0,1,0,1,1,1,1,0,0,1,1,1,1,0,0,0,1,0,1,1,1,0,1,0,0,0,0,0,0,0,1,1,1,0,1,0,0,1,1,0,1,1,0,1,0,1,0,1,0,0,0,1,1,0,1,0,
       1,1,0,1,1,0,0,0,0,1,1,0,0,0,1,1,0,0,1,1,1,1,0,1,1,0,0,1,1,0,1,0,1,0,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,1,0,1,1,1,1,
       0,0,1,1,0,1,0,0,0,0,1,0,1,0,1,1,0,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,1,0,1,1,1,1,1,1,0,0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,
       0,1,0,1,0,0,1,0,0,0,1,0,1,1,1,1,1,1,0,0,1,0,0,1,0,0,0,1,0,1,1,0,0,0,1,0,1,0,1,0,1,1,0,0,1,1,1,0,1,1,0,1,0,0,0,0,
       0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,0,1,1,0,1,1,0,1,1,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0,0,1,1,0,1,1,1,
       1,0,0,0,1,1,0,1,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,0,1,1,0,0,1,1,1,0,0,0,
       0,0,1,1,1,0,1,1,1,1,0,1,0,1,0,1,1,0,1,0,1,1,0,0,1,1,1,0,1,1,1,0,0,0,1,0,0,1,0,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,
       0,1,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,0,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,1,0,1,0,1,1,0,1,0,1,1,0,1,1,0,1,0,
       0,0,0,1,0,1,1,0,0,1,0,1,0,0,0,0,1,1,1,1,1,1,1,0,1,0,0,0,1,1,0,1,0,0,1,1,0,0,0,0,0,1,1,0,0,0,1,0,0,1,0,1,1,0,0,0,
       0,0,1,1,1,1,1,1,0,1,0,0,1,0,0,0,0,0,1,1,0,1,0,1,1,0,1,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,
       1,1,0,1,1,1,0,0,0,0,1,1,0,1,1,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,1,1,0,0,0,0,1,1,0,1,0,0,0,1,0,0,1,0,1,0,1,1,1,1,1,0},

      {1,1,1,1,1,0,1,1,1,1,1,1,0,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,0,0,1,0,0,
       1,0,0,1,1,0,0,0,0,1,1,1,0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0,1,0,1,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,0,0,0,1,0,1,1,1,1,0,
       0,0,0,0,1,0,0,1,0,1,0,1,1,1,0,1,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,0,0,1,1,1,0,1,0,0,0,1,1,0,0,1,0,0,0,
       0,1,1,1,1,0,1,1,1,1,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,0,1,0,0,1,0,1,1,0,0,1,0,0,0,0,0,1,1,0,0,1,1,0,1,0,0,0,0,1,0,1,
       1,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,0,1,0,1,
       1,0,1,0,0,0,0,1,1,0,0,0,0,1,1,1,0,1,0,0,0,1,0,1,1,1,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,1,0,1,
       0,0,1,0,0,1,1,1,1,0,0,1,1,1,0,0,1,1,0,0,0,0,1,0,0,1,1,0,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,0,1,1,0,0,1,1,0,1,0,0,0,0,
       0,0,1,1,0,1,0,0,0,0,1,0,1,0,1,1,0,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,1,0,1,1,1,1,1,1,0,0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,
       0,1,0,1,0,0,1,0,0,0,1,0,1,1,1,1,1,1,0,0,1,0,0,1,0,0,0,1,0,1,1,0,0,0,1,0,1,0,1,0,1,1,0,0,1,1,1,0,1,1,0,1,0,0,0,0,
       1,1,1,0,1,1,1,1,1,0,0,1,1,1,1,0,0,0,1,1,1,0,0,1,0,0,1,0,0,0,0,0,0,1,1,1,1,1,0,1,1,0,0,1,1,1,0,1,1,1,0,0,1,0,0,0,
       0,1,1,1,0,0,1,0,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,1,0,0,1,1,0,0,0,1,1,1,
       0,0,1,1,1,0,1,1,1,1,0,1,0,1,0,1,1,0,1,0,1,1,0,0,1,1,1,0,1,1,1,0,0,0,1,0,0,1,0,1,1,1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,
       0,1,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,0,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,1,0,1,0,1,1,0,1,0,1,1,0,1,1,0,1,0,
       1,1,1,0,1,0,0,1,1,0,1,0,1,1,1,1,0,0,0,0,0,0,0,1,0,1,1,1,0,0,1,0,1,1,0,0,1,1,1,1,1,0,0,1,1,1,0,1,1,0,1,0,0,1,1,1,
       1,1,0,0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,0,0,1,0,1,0,0,1,0,1,1,0,1,1,1,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,
       1,1,0,1,1,1,0,0,0,0,1,1,0,1,1,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,1,1,0,0,0,0,1,1,0,1,0,0,0,1,0,0,1,0,1,0,1,1,1,1,1,0},

      {1,1,1,1,1,0,1,1,1,1,1,1,0,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,0,0,1,0,0,
       0,1,1,0,0,1,1,1,1,0,0,0,1,1,1,1,0,1,1,1,0,0,0,1,1,1,1,1,0,1,0,0,1,1,0,0,0,1,1,0,1,1,0,0,1,0,1,1,1,0,1,0,0,0,0,1,
       0,0,0,0,1,0,0,1,0,1,0,1,1,1,0,1,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,0,0,1,1,1,0,1,0,0,0,1,1,0,0,1,0,0,0,
       1,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,1,0,1,1,0,0,1,0,1,0,1,1,0,1,0,0,1,1,0,1,1,1,1,1,0,0,1,1,0,0,1,0,1,1,1,1,0,1,0,
       1,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,0,1,0,1,
       0,1,0,1,1,1,1,0,0,1,1,1,1,0,0,0,1,0,1,1,1,0,1,0,0,0,0,0,0,0,1,1,1,0,1,0,0,1,1,0,1,1,0,1,0,1,0,1,0,0,0,1,1,0,1,0,
       0,0,1,0,0,1,1,1,1,0,0,1,1,1,0,0,1,1,0,0,0,0,1,0,0,1,1,0,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,0,1,1,0,0,1,1,0,1,0,0,0,0,
       1,1,0,0,1,0,1,1,1,1,0,1,0,1,0,0,1,1,1,1,1,0,1,1,0,1,1,0,0,1,1,1,0,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,1,0,
       0,1,0,1,0,0,1,0,0,0,1,0,1,1,1,1,1,1,0,0,1,0,0,1,0,0,0,1,0,1,1,0,0,0,1,0,1,0,1,0,1,1,0,0,1,1,1,0,1,1,0,1,0,0,0,0,
       0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,0,1,1,0,1,1,0,1,1,1,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0,0,1,1,0,1,1,1,
       0,1,1,1,0,0,1,0,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,1,0,0,1,1,0,0,0,1,1,1,
       1,1,0,0,0,1,0,0,0,0,1,0,1,0,1,0,0,1,0,1,0,0,1,1,0,0,0,1,0,0,0,1,1,1,0,1,1,0,1,0,0,0,0,1,1,1,0,1,0,0,1,1,0,1,1,0,
       0,1,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,0,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,1,0,1,0,1,1,0,1,0,1,1,0,1,1,0,1,0,
       0,0,0,1,0,1,1,0,0,1,0,1,0,0,0,0,1,1,1,1,1,1,1,0,1,0,0,0,1,1,0,1,0,0,1,1,0,0,0,0,0,1,1,0,0,0,1,0,0,1,0,1,1,0,0,0,
       1,1,0,0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,0,0,1,0,1,0,0,1,0,1,1,0,1,1,1,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,
       0,0,1,0,0,0,1,1,1,1,0,0,1,0,0,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,0,0,1,1,1,1,0,0,1,0,1,1,1,0,1,1,0,1,0,1,0,0,0,0,0,1}
    };

  } /* namespace dtv */
} /* namespace gr */

