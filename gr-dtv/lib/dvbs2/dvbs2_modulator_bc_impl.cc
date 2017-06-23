/* -*- c++ -*- */
/* 
 * Copyright 2015-2017 Free Software Foundation, Inc.
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
#include "dvbs2_modulator_bc_impl.h"

namespace gr {
  namespace dtv {

    dvbs2_modulator_bc::sptr
    dvbs2_modulator_bc::make(dvb_framesize_t framesize, dvb_code_rate_t rate, dvb_constellation_t constellation, dvbs2_interpolation_t interpolation)
    {
      return gnuradio::get_initial_sptr
        (new dvbs2_modulator_bc_impl(framesize, rate, constellation, interpolation));
    }

    /*
     * The private constructor
     */
    dvbs2_modulator_bc_impl::dvbs2_modulator_bc_impl(dvb_framesize_t framesize, dvb_code_rate_t rate, dvb_constellation_t constellation, dvbs2_interpolation_t interpolation)
      : gr::block("dvbs2_modulator_bc",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {
      double r1, r2, r3, r4, r5, r6, r7, r8;
      double m = 1.0;
      r1 = m;
      switch (constellation) {
        case MOD_BPSK:
        case MOD_BPSK_SF2:
          m_bpsk[0][0] = gr_complex((r1 * cos(M_PI / 4.0)), (r1 * sin(M_PI / 4.0)));
          m_bpsk[0][1] = gr_complex((r1 * cos(5.0 * M_PI / 4.0)), (r1 * sin(5.0 * M_PI / 4.0)));
          m_bpsk[1][0] = gr_complex((r1 * cos(5.0 * M_PI / 4.0)), (r1 * sin(M_PI / 4.0)));
          m_bpsk[1][1] = gr_complex((r1 * cos(M_PI / 4.0)), (r1 * sin(5.0 * M_PI /4.0)));
          break;
        case MOD_QPSK:
          m_qpsk[0] = gr_complex((r1 * cos(M_PI / 4.0)), (r1 * sin(M_PI / 4.0)));
          m_qpsk[1] = gr_complex((r1 * cos(7 * M_PI / 4.0)), (r1 * sin(7 * M_PI / 4.0)));
          m_qpsk[2] = gr_complex((r1 * cos(3 * M_PI / 4.0)), (r1 * sin(3 * M_PI / 4.0)));
          m_qpsk[3] = gr_complex((r1 * cos(5 * M_PI / 4.0)), (r1 * sin(5 * M_PI / 4.0)));
          break;
        case MOD_8PSK:
          m_8psk[0] = gr_complex((r1 * cos(M_PI / 4.0)), (r1 * sin(M_PI / 4.0)));
          m_8psk[1] = gr_complex((r1 * cos(0.0)), (r1 * sin(0.0)));
          m_8psk[2] = gr_complex((r1 * cos(4 * M_PI / 4.0)), (r1 * sin(4 * M_PI / 4.0)));
          m_8psk[3] = gr_complex((r1 * cos(5 * M_PI / 4.0)), (r1 * sin(5 * M_PI / 4.0)));
          m_8psk[4] = gr_complex((r1 * cos(2 * M_PI / 4.0)), (r1 * sin(2 * M_PI / 4.0)));
          m_8psk[5] = gr_complex((r1 * cos(7 * M_PI / 4.0)), (r1 * sin(7 * M_PI / 4.0)));
          m_8psk[6] = gr_complex((r1 * cos(3 * M_PI / 4.0)), (r1 * sin(3 * M_PI / 4.0)));
          m_8psk[7] = gr_complex((r1 * cos(6 * M_PI / 4.0)), (r1 * sin(6 * M_PI / 4.0)));
          break;
        case MOD_8APSK:
          r3 = m;
          switch(rate) {
            case C100_180:
              r1 = r3 / 6.8;
              r2 = r1 * 5.32;
              break;
            case C104_180:
              r1 = r3 / 8.0;
              r2 = r1 * 6.39;
              break;
            default:
              r1 = 0;
              r2 = 0;
              break;
          }
          m_8psk[0] = gr_complex((r1 * cos(M_PI)), (r1 * sin(M_PI)));
          m_8psk[1] = gr_complex((r2 * cos(M_PI * 1.352)), (r2 * sin(M_PI * 1.352)));
          m_8psk[2] = gr_complex((r2 * cos(M_PI * -1.352)), (r2 * sin(M_PI * -1.352)));
          m_8psk[3] = gr_complex((r3 * cos(M_PI)), (r3 * sin(M_PI)));
          m_8psk[4] = gr_complex((r1 * cos(0.0)), (r1 * sin(0.0)));
          m_8psk[5] = gr_complex((r2 * cos(M_PI * -0.352)), (r2 * sin(M_PI * -0.352)));
          m_8psk[6] = gr_complex((r2 * cos(M_PI * 0.352)), (r2 * sin(M_PI * 0.352)));
          m_8psk[7] = gr_complex((r3 * cos(0.0)), (r3 * sin(0.0)));
          break;
        case MOD_16APSK:
          r2 = m;
          if (framesize == FECFRAME_NORMAL) {
            switch(rate) {
              case C2_3:
                r1 = r2 / 3.15;
                break;
              case C3_4:
                r1 = r2 / 2.85;
                break;
              case C4_5:
                r1 = r2 / 2.75;
                break;
              case C5_6:
                r1 = r2 / 2.70;
                break;
              case C8_9:
                r1 = r2 / 2.60;
                break;
              case C9_10:
                r1 = r2 / 2.57;
                break;
              case C26_45:
              case C3_5:
                r1 = r2 / 3.70;
                break;
              case C28_45:
                r1 = r2 / 3.50;
                break;
              case C23_36:
              case C25_36:
                r1 = r2 / 3.10;
                break;
              case C13_18:
                r1 = r2 / 2.85;
                break;
              case C140_180:
                r1 = r2 / 3.60;
                break;
              case C154_180:
                r1 = r2 / 3.20;
                break;
              default:
                r1 = 0;
                break;
            }
          }
          else {
            switch(rate) {
              case C2_3:
                r1 = r2 / 3.15;
                break;
              case C3_4:
                r1 = r2 / 2.85;
                break;
              case C4_5:
                r1 = r2 / 2.75;
                break;
              case C5_6:
                r1 = r2 / 2.70;
                break;
              case C8_9:
                r1 = r2 / 2.60;
                break;
              case C7_15:
                r1 = r2 / 3.32;
                break;
              case C8_15:
                r1 = r2 / 3.50;
                break;
              case C26_45:
              case C3_5:
                r1 = r2 / 3.70;
                break;
              case C32_45:
                r1 = r2 / 2.85;
                break;
              default:
                r1 = 0;
                break;
            }
          }
          m_16apsk[0] = gr_complex((r2 * cos(M_PI / 4.0)), (r2 * sin(M_PI / 4.0)));
          m_16apsk[1] = gr_complex((r2 * cos(-M_PI / 4.0)), (r2 * sin(-M_PI / 4.0)));
          m_16apsk[2] = gr_complex((r2 * cos(3 * M_PI / 4.0)), (r2 * sin(3 * M_PI / 4.0)));
          m_16apsk[3] = gr_complex((r2 * cos(-3 * M_PI / 4.0)), (r2 * sin(-3 * M_PI / 4.0)));
          m_16apsk[4] = gr_complex((r2 * cos(M_PI / 12.0)), (r2 * sin(M_PI / 12.0)));
          m_16apsk[5] = gr_complex((r2 * cos(-M_PI / 12.0)), (r2 * sin(-M_PI / 12.0)));
          m_16apsk[6] = gr_complex((r2 * cos(11 * M_PI / 12.0)), (r2 * sin(11 * M_PI / 12.0)));
          m_16apsk[7] = gr_complex((r2 * cos(-11 * M_PI / 12.0)), (r2 * sin(-11 * M_PI / 12.0)));
          m_16apsk[8] = gr_complex((r2 * cos(5 * M_PI / 12.0)), (r2 * sin(5 * M_PI / 12.0)));
          m_16apsk[9] = gr_complex((r2 * cos(-5 * M_PI / 12.0)), (r2 * sin(-5 * M_PI / 12.0)));
          m_16apsk[10] = gr_complex((r2 * cos(7 * M_PI / 12.0)), (r2 * sin(7 * M_PI / 12.0)));
          m_16apsk[11] = gr_complex((r2 * cos(-7 * M_PI / 12.0)), (r2 * sin(-7 * M_PI / 12.0)));
          m_16apsk[12] = gr_complex((r1 * cos(M_PI / 4.0)), (r1 * sin(M_PI / 4.0)));
          m_16apsk[13] = gr_complex((r1 * cos(-M_PI / 4.0)), (r1 * sin(-M_PI / 4.0)));
          m_16apsk[14] = gr_complex((r1 * cos(3 * M_PI / 4.0)), (r1 * sin(3 * M_PI / 4.0)));
          m_16apsk[15] = gr_complex((r1 * cos(-3 * M_PI / 4.0)), (r1 * sin(-3 * M_PI / 4.0)));
          break;
        case MOD_8_8APSK:
          if (rate == C18_30) {
            m_16apsk[0] = gr_complex(0.4718, 0.2606);
            m_16apsk[1] = gr_complex(0.2606, 0.4718);
            m_16apsk[2] = gr_complex(-0.4718, 0.2606);
            m_16apsk[3] = gr_complex(-0.2606, 0.4718);
            m_16apsk[4] = gr_complex(0.4718, -0.2606);
            m_16apsk[5] = gr_complex(0.2606, -0.4718);
            m_16apsk[6] = gr_complex(-0.4718, -0.2606);
            m_16apsk[7] = gr_complex(-0.2606, -0.4718);
            m_16apsk[8] = gr_complex(1.2088, 0.4984);
            m_16apsk[9] = gr_complex(0.4984, 1.2088);
            m_16apsk[10] = gr_complex(-1.2088, 0.4984);
            m_16apsk[11] = gr_complex(-0.4984, 1.2088);
            m_16apsk[12] = gr_complex(1.2088, -0.4984);
            m_16apsk[13] = gr_complex(0.4984, -1.2088);
            m_16apsk[14] = gr_complex(-1.2088, -0.4984);
            m_16apsk[15] = gr_complex(-0.4984, -1.2088);
          }
          else if (rate == C20_30) {
            m_16apsk[0] = gr_complex(0.5061, 0.2474);
            m_16apsk[1] = gr_complex(0.2474, 0.5061);
            m_16apsk[2] = gr_complex(-0.5061, 0.2474);
            m_16apsk[3] = gr_complex(-0.2474, 0.5061);
            m_16apsk[4] = gr_complex(0.5061, -0.2474);
            m_16apsk[5] = gr_complex(0.2474, -0.5061);
            m_16apsk[6] = gr_complex(-0.5061, -0.2474);
            m_16apsk[7] = gr_complex(-0.2474, -0.5061);
            m_16apsk[8] = gr_complex(1.2007, 0.4909);
            m_16apsk[9] = gr_complex(0.4909, 1.2007);
            m_16apsk[10] = gr_complex(-1.2007, 0.4909);
            m_16apsk[11] = gr_complex(-0.4909, 1.2007);
            m_16apsk[12] = gr_complex(1.2007, -0.4909);
            m_16apsk[13] = gr_complex(0.4909, -1.2007);
            m_16apsk[14] = gr_complex(-1.2007, -0.4909);
            m_16apsk[15] = gr_complex(-0.4909, -1.2007);
          }
          else {
            r2 = m;
            switch(rate) {
              case C90_180:
              case C96_180:
              case C100_180:
                r1 = r2 / 2.19;
                break;
              default:
                r1 = 0;
                break;
            }
            m_16apsk[0] = gr_complex((r1 * cos(M_PI / 8.0)), (r1 * sin(M_PI / 8.0)));
            m_16apsk[1] = gr_complex((r1 * cos(3 * M_PI / 8.0)), (r1 * sin(3 * M_PI / 8.0)));
            m_16apsk[2] = gr_complex((r1 * cos(7 * M_PI / 8.0)), (r1 * sin(7 * M_PI / 8.0)));
            m_16apsk[3] = gr_complex((r1 * cos(5 * M_PI / 8.0)), (r1 * sin(5 * M_PI / 8.0)));
            m_16apsk[4] = gr_complex((r1 * cos(15 * M_PI / 8.0)), (r1 * sin(15 * M_PI / 8.0)));
            m_16apsk[5] = gr_complex((r1 * cos(13 * M_PI / 8.0)), (r1 * sin(13 * M_PI / 8.0)));
            m_16apsk[6] = gr_complex((r1 * cos(9 * M_PI / 8.0)), (r1 * sin(9 * M_PI / 8.0)));
            m_16apsk[7] = gr_complex((r1 * cos(11 * M_PI / 8.0)), (r1 * sin(11 * M_PI / 8.0)));
            m_16apsk[8] = gr_complex((r2 * cos(M_PI / 8.0)), (r2 * sin(M_PI / 8.0)));
            m_16apsk[9] = gr_complex((r2 * cos(3 * M_PI / 8.0)), (r2 * sin(3 * M_PI / 8.0)));
            m_16apsk[10] = gr_complex((r2 * cos(7 * M_PI / 8.0)), (r2 * sin(7 * M_PI / 8.0)));
            m_16apsk[11] = gr_complex((r2 * cos(5 * M_PI / 8.0)), (r2 * sin(5 * M_PI / 8.0)));
            m_16apsk[12] = gr_complex((r2 * cos(15 * M_PI / 8.0)), (r2 * sin(15 * M_PI / 8.0)));
            m_16apsk[13] = gr_complex((r2 * cos(13 * M_PI / 8.0)), (r2 * sin(13 * M_PI / 8.0)));
            m_16apsk[14] = gr_complex((r2 * cos(9 * M_PI / 8.0)), (r2 * sin(9 * M_PI / 8.0)));
            m_16apsk[15] = gr_complex((r2 * cos(11 * M_PI / 8.0)), (r2 * sin(11 * M_PI / 8.0)));
          }
          break;
        case MOD_32APSK:
          r3 = m;
          switch(rate) {
            case C3_4:
              r1 = r3 / 5.27;
              r2 = r1 * 2.84;
              break;
            case C4_5:
              r1 = r3 / 4.87;
              r2 = r1 * 2.72;
              break;
            case C5_6:
              r1 = r3 / 4.64;
              r2 = r1 * 2.64;
              break;
            case C8_9:
              r1 = r3 / 4.33;
              r2 = r1 * 2.54;
              break;
            case C9_10:
              r1 = r3 / 4.30;
              r2 = r1 * 2.53;
              break;
            default:
              r1 = 0;
              r2 = 0;
              break;
          }
          m_32apsk[0] = gr_complex((r2 * cos(M_PI / 4.0)), (r2 * sin(M_PI / 4.0)));
          m_32apsk[1] = gr_complex((r2 * cos(5 * M_PI / 12.0)), (r2 * sin(5 * M_PI / 12.0)));
          m_32apsk[2] = gr_complex((r2 * cos(-M_PI / 4.0)), (r2 * sin(-M_PI / 4.0)));
          m_32apsk[3] = gr_complex((r2 * cos(-5 * M_PI / 12.0)), (r2 * sin(-5 * M_PI / 12.0)));
          m_32apsk[4] = gr_complex((r2 * cos(3 * M_PI / 4.0)), (r2 * sin(3 * M_PI / 4.0)));
          m_32apsk[5] = gr_complex((r2 * cos(7 * M_PI / 12.0)), (r2 * sin(7 * M_PI / 12.0)));
          m_32apsk[6] = gr_complex((r2 * cos(-3 * M_PI / 4.0)), (r2 * sin(-3 * M_PI / 4.0)));
          m_32apsk[7] = gr_complex((r2 * cos(-7 * M_PI / 12.0)), (r2 * sin(-7 * M_PI / 12.0)));
          m_32apsk[8] = gr_complex((r3 * cos(M_PI / 8.0)), (r3 * sin(M_PI / 8.0)));
          m_32apsk[9] = gr_complex((r3 * cos(3 * M_PI / 8.0)), (r3 * sin(3 * M_PI / 8.0)));
          m_32apsk[10] = gr_complex((r3 * cos(-M_PI / 4.0)), (r3 * sin(-M_PI / 4.0)));
          m_32apsk[11] = gr_complex((r3 * cos(-M_PI / 2.0)), (r3 * sin(-M_PI / 2.0)));
          m_32apsk[12] = gr_complex((r3 * cos(3 * M_PI / 4.0)), (r3 * sin(3 * M_PI / 4.0)));
          m_32apsk[13] = gr_complex((r3 * cos(M_PI / 2.0)), (r3 * sin(M_PI / 2.0)));
          m_32apsk[14] = gr_complex((r3 * cos(-7 * M_PI / 8.0)), (r3 * sin(-7 * M_PI / 8.0)));
          m_32apsk[15] = gr_complex((r3 * cos(-5 * M_PI / 8.0)), (r3 * sin(-5 * M_PI / 8.0)));
          m_32apsk[16] = gr_complex((r2 * cos(M_PI / 12.0)), (r2 * sin(M_PI / 12.0)));
          m_32apsk[17] = gr_complex((r1 * cos(M_PI / 4.0)), (r1 * sin(M_PI / 4.0)));
          m_32apsk[18] = gr_complex((r2 * cos(-M_PI / 12.0)), (r2 * sin(-M_PI / 12.0)));
          m_32apsk[19] = gr_complex((r1 * cos(-M_PI / 4.0)), (r1 * sin(-M_PI / 4.0)));
          m_32apsk[20] = gr_complex((r2 * cos(11 * M_PI / 12.0)), (r2 * sin(11 * M_PI / 12.0)));
          m_32apsk[21] = gr_complex((r1 * cos(3 * M_PI / 4.0)), (r1 * sin(3 * M_PI / 4.0)));
          m_32apsk[22] = gr_complex((r2 * cos(-11 * M_PI / 12.0)), (r2 * sin(-11 * M_PI / 12.0)));
          m_32apsk[23] = gr_complex((r1 * cos(-3 * M_PI / 4.0)), (r1 * sin(-3 * M_PI / 4.0)));
          m_32apsk[24] = gr_complex((r3 * cos(0.0)), (r3 * sin(0.0)));
          m_32apsk[25] = gr_complex((r3 * cos(M_PI / 4.0)), (r3 * sin(M_PI / 4.0)));
          m_32apsk[26] = gr_complex((r3 * cos(-M_PI / 8.0)), (r3 * sin(-M_PI / 8.0)));
          m_32apsk[27] = gr_complex((r3 * cos(-3 * M_PI / 8.0)), (r3 * sin(-3 * M_PI / 8.0)));
          m_32apsk[28] = gr_complex((r3 * cos(7 * M_PI / 8.0)), (r3 * sin(7 * M_PI / 8.0)));
          m_32apsk[29] = gr_complex((r3 * cos(5 * M_PI / 8.0)), (r3 * sin(5 * M_PI / 8.0)));
          m_32apsk[30] = gr_complex((r3 * cos(M_PI)), (r3 * sin(M_PI)));
          m_32apsk[31] = gr_complex((r3 * cos(-3 * M_PI / 4.0)), (r3 * sin(-3 * M_PI / 4.0)));
          break;
        case MOD_4_12_16APSK:
          r3 = m;
          if (framesize == FECFRAME_NORMAL) {
            switch(rate) {
              case C2_3:
                r1 = r3 / 5.55;
                r2 = r1 * 2.85;
                break;
              default:
                r1 = 0;
                r2 = 0;
                break;
            }
          }
          else {
            switch(rate) {
              case C2_3:
                r1 = r3 / 5.54;
                r2 = r1 * 2.84;
                break;
              case C32_45:
                r1 = r3 / 5.26;
                r2 = r1 * 2.84;
                break;
              default:
                r1 = 0;
                r2 = 0;
                break;
            }
          }
          m_32apsk[0] = gr_complex((r3 * cos(11 * M_PI / 16.0)), (r3 * sin(11 * M_PI / 16.0)));
          m_32apsk[1] = gr_complex((r3 * cos(9 * M_PI / 16.0)), (r3 * sin(9 * M_PI / 16.0)));
          m_32apsk[2] = gr_complex((r3 * cos(5 * M_PI / 16.0)), (r3 * sin(5 * M_PI / 16.0)));
          m_32apsk[3] = gr_complex((r3 * cos(7 * M_PI / 16.0)), (r3 * sin(7 * M_PI / 16.0)));
          m_32apsk[4] = gr_complex((r2 * cos(3 * M_PI / 4.0)), (r2 * sin(3 * M_PI / 4.0)));
          m_32apsk[5] = gr_complex((r2 * cos(7 * M_PI / 12.0)), (r2 * sin(7 * M_PI / 12.0)));
          m_32apsk[6] = gr_complex((r2 * cos(M_PI / 4.0)), (r2 * sin(M_PI / 4.0)));
          m_32apsk[7] = gr_complex((r2 * cos(5 * M_PI / 12.0)), (r2 * sin(5 * M_PI / 12.0)));
          m_32apsk[8] = gr_complex((r3 * cos(13 * M_PI / 16.0)), (r3 * sin(13 * M_PI / 16.0)));
          m_32apsk[9] = gr_complex((r3 * cos(15 * M_PI / 16.0)), (r3 * sin(15 * M_PI / 16.0)));
          m_32apsk[10] = gr_complex((r3 * cos(3 * M_PI / 16.0)), (r3 * sin(3 * M_PI / 16.0)));
          m_32apsk[11] = gr_complex((r3 * cos(M_PI / 16.0)), (r3 * sin(M_PI / 16.0)));
          m_32apsk[12] = gr_complex((r2 * cos(11 * M_PI / 12.0)), (r2 * sin(11 * M_PI / 12.0)));
          m_32apsk[13] = gr_complex((r1 * cos(3 * M_PI / 4.0)), (r1 * sin(3 * M_PI / 4.0)));
          m_32apsk[14] = gr_complex((r2 * cos(M_PI / 12.0)), (r2 * sin(M_PI / 12.0)));
          m_32apsk[15] = gr_complex((r1 * cos(M_PI / 4.0)), (r1 * sin(M_PI / 4.0)));
          m_32apsk[16] = gr_complex((r3 * cos(21 * M_PI / 16.0)), (r3 * sin(21 * M_PI / 16.0)));
          m_32apsk[17] = gr_complex((r3 * cos(23 * M_PI / 16.0)), (r3 * sin(23 * M_PI / 16.0)));
          m_32apsk[18] = gr_complex((r3 * cos(27 * M_PI / 16.0)), (r3 * sin(27 * M_PI / 16.0)));
          m_32apsk[19] = gr_complex((r3 * cos(25 * M_PI / 16.0)), (r3 * sin(25 * M_PI / 16.0)));
          m_32apsk[20] = gr_complex((r2 * cos(5 * M_PI / 4.0)), (r2 * sin(5 * M_PI / 4.0)));
          m_32apsk[21] = gr_complex((r2 * cos(17 * M_PI / 12.0)), (r2 * sin(17 * M_PI / 12.0)));
          m_32apsk[22] = gr_complex((r2 * cos(7 * M_PI / 4.0)), (r2 * sin(7 * M_PI / 4.0)));
          m_32apsk[23] = gr_complex((r2 * cos(19 * M_PI / 12.0)), (r2 * sin(19 * M_PI / 12.0)));
          m_32apsk[24] = gr_complex((r3 * cos(19 * M_PI / 16.0)), (r3 * sin(19 * M_PI / 16.0)));
          m_32apsk[25] = gr_complex((r3 * cos(17 * M_PI / 16.0)), (r3 * sin(17 * M_PI / 16.0)));
          m_32apsk[26] = gr_complex((r3 * cos(29 * M_PI / 16.0)), (r3 * sin(29 * M_PI / 16.0)));
          m_32apsk[27] = gr_complex((r3 * cos(31 * M_PI / 16.0)), (r3 * sin(31 * M_PI / 16.0)));
          m_32apsk[28] = gr_complex((r2 * cos(13 * M_PI / 12.0)), (r2 * sin(13 * M_PI / 12.0)));
          m_32apsk[29] = gr_complex((r1 * cos(5 * M_PI / 4.0)), (r1 * sin(5 * M_PI / 4.0)));
          m_32apsk[30] = gr_complex((r2 * cos(23 * M_PI / 12.0)), (r2 * sin(23 * M_PI / 12.0)));
          m_32apsk[31] = gr_complex((r1 * cos(7 * M_PI / 4.0)), (r1 * sin(7 * M_PI / 4.0)));
          break;
        case MOD_4_8_4_16APSK:
          r4 = m;
          switch(rate) {
            case C128_180:
              r1 = r4 / 5.6;
              r3 = r1 * 2.99;
              r2 = r1 * 2.6;
              break;
            case C132_180:
              r1 = r4 / 5.6;
              r3 = r1 * 2.86;
              r2 = r1 * 2.6;
              break;
            case C140_180:
              r1 = r4 / 5.6;
              r3 = r1 * 3.08;
              r2 = r1 * 2.6;
              break;
            default:
              r1 = 0;
              r2 = 0;
              r3 = 0;
              break;
          }
          m_32apsk[0] = gr_complex((r1 * cos(M_PI / 4.0)), (r1 * sin(M_PI / 4.0)));
          m_32apsk[1] = gr_complex((r4 * cos(7 * M_PI / 16.0)), (r4 * sin(7 * M_PI / 16.0)));
          m_32apsk[2] = gr_complex((r1 * cos(7 * M_PI / 4.0)), (r1 * sin(7 * M_PI / 4.0)));
          m_32apsk[3] = gr_complex((r4 * cos(25 * M_PI / 16.0)), (r4 * sin(25 * M_PI / 16.0)));
          m_32apsk[4] = gr_complex((r1 * cos(3 * M_PI / 4.0)), (r1 * sin(3 * M_PI / 4.0)));
          m_32apsk[5] = gr_complex((r4 * cos(9 * M_PI / 16.0)), (r4 * sin(9 * M_PI / 16.0)));
          m_32apsk[6] = gr_complex((r1 * cos(5 * M_PI / 4.0)), (r1 * sin(5 * M_PI / 4.0)));
          m_32apsk[7] = gr_complex((r4 * cos(23 * M_PI / 16.0)), (r4 * sin(23 * M_PI / 16.0)));
          m_32apsk[8] = gr_complex((r2 * cos(M_PI / 12.0)), (r2 * sin(M_PI / 12.0)));
          m_32apsk[9] = gr_complex((r4 * cos(M_PI / 16.0)), (r4 * sin(M_PI / 16.0)));
          m_32apsk[10] = gr_complex((r2 * cos(23 * M_PI / 12.0)), (r2 * sin(23 * M_PI / 12.0)));
          m_32apsk[11] = gr_complex((r4 * cos(31 * M_PI / 16.0)), (r4 * sin(31 * M_PI / 16.0)));
          m_32apsk[12] = gr_complex((r2 * cos(11 * M_PI / 12.0)), (r2 * sin(11 * M_PI / 12.0)));
          m_32apsk[13] = gr_complex((r4 * cos(15 * M_PI / 16.0)), (r4 * sin(15 * M_PI / 16.0)));
          m_32apsk[14] = gr_complex((r2 * cos(13 * M_PI / 12.0)), (r2 * sin(13 * M_PI / 12.0)));
          m_32apsk[15] = gr_complex((r4 * cos(17 * M_PI / 16.0)), (r4 * sin(17 * M_PI / 16.0)));
          m_32apsk[16] = gr_complex((r2 * cos(5 * M_PI / 12.0)), (r2 * sin(5 * M_PI / 12.0)));
          m_32apsk[17] = gr_complex((r4 * cos(5 * M_PI / 16.0)), (r4 * sin(5 * M_PI / 16.0)));
          m_32apsk[18] = gr_complex((r2 * cos(19 * M_PI / 12.0)), (r2 * sin(19 * M_PI / 12.0)));
          m_32apsk[19] = gr_complex((r4 * cos(27 * M_PI / 16.0)), (r4 * sin(27 * M_PI / 16.0)));
          m_32apsk[20] = gr_complex((r2 * cos(7 * M_PI / 12.0)), (r2 * sin(7 * M_PI / 12.0)));
          m_32apsk[21] = gr_complex((r4 * cos(11 * M_PI / 16.0)), (r4 * sin(11 * M_PI / 16.0)));
          m_32apsk[22] = gr_complex((r2 * cos(17 * M_PI / 12.0)), (r2 * sin(17 * M_PI / 12.0)));
          m_32apsk[23] = gr_complex((r4 * cos(21 * M_PI / 16.0)), (r4 * sin(21 * M_PI / 16.0)));
          m_32apsk[24] = gr_complex((r3 * cos(M_PI / 4.0)), (r3 * sin(M_PI / 4.0)));
          m_32apsk[25] = gr_complex((r4 * cos(3 * M_PI / 16.0)), (r4 * sin(3 * M_PI / 16.0)));
          m_32apsk[26] = gr_complex((r3 * cos(7 * M_PI / 4.0)), (r3 * sin(7 * M_PI / 4.0)));
          m_32apsk[27] = gr_complex((r4 * cos(29 * M_PI / 16.0)), (r4 * sin(29 * M_PI / 16.0)));
          m_32apsk[28] = gr_complex((r3 * cos(3 * M_PI / 4.0)), (r3 * sin(3 * M_PI / 4.0)));
          m_32apsk[29] = gr_complex((r4 * cos(13 * M_PI / 16.0)), (r4 * sin(13 * M_PI / 16.0)));
          m_32apsk[30] = gr_complex((r3 * cos(5 * M_PI / 4.0)), (r3 * sin(5 * M_PI / 4.0)));
          m_32apsk[31] = gr_complex((r4 * cos(19 * M_PI / 16.0)), (r4 * sin(19 * M_PI / 16.0)));
          break;
        case MOD_64APSK:
          r4 = m;
          switch(rate) {
            case C128_180:
              r1 = r4 / 3.95;
              r3 = r1 * 2.72;
              r2 = r1 * 1.88;
              break;
            default:
              r1 = 0;
              r2 = 0;
              r3 = 0;
              break;
          }
          m_64apsk[0] = gr_complex((r1 * cos(M_PI / 16.0)), (r1 * sin(M_PI / 16.0)));
          m_64apsk[1] = gr_complex((r1 * cos(3 * M_PI / 16.0)), (r1 * sin(3 * M_PI / 16.0)));
          m_64apsk[2] = gr_complex((r1 * cos(7 * M_PI / 16.0)), (r1 * sin(7 * M_PI / 16.0)));
          m_64apsk[3] = gr_complex((r1 * cos(5 * M_PI / 16.0)), (r1 * sin(5 * M_PI / 16.0)));
          m_64apsk[4] = gr_complex((r1 * cos(31 * M_PI / 16.0)), (r1 * sin(31 * M_PI / 16.0)));
          m_64apsk[5] = gr_complex((r1 * cos(29 * M_PI / 16.0)), (r1 * sin(29 * M_PI / 16.0)));
          m_64apsk[6] = gr_complex((r1 * cos(25 * M_PI / 16.0)), (r1 * sin(25 * M_PI / 16.0)));
          m_64apsk[7] = gr_complex((r1 * cos(27 * M_PI / 16.0)), (r1 * sin(27 * M_PI / 16.0)));
          m_64apsk[8] = gr_complex((r1 * cos(15 * M_PI / 16.0)), (r1 * sin(15 * M_PI / 16.0)));
          m_64apsk[9] = gr_complex((r1 * cos(13 * M_PI / 16.0)), (r1 * sin(13 * M_PI / 16.0)));
          m_64apsk[10] = gr_complex((r1 * cos(9 * M_PI / 16.0)), (r1 * sin(9 * M_PI / 16.0)));
          m_64apsk[11] = gr_complex((r1 * cos(11 * M_PI / 16.0)), (r1 * sin(11 * M_PI / 16.0)));
          m_64apsk[12] = gr_complex((r1 * cos(17 * M_PI / 16.0)), (r1 * sin(17 * M_PI / 16.0)));
          m_64apsk[13] = gr_complex((r1 * cos(19 * M_PI / 16.0)), (r1 * sin(19 * M_PI / 16.0)));
          m_64apsk[14] = gr_complex((r1 * cos(23 * M_PI / 16.0)), (r1 * sin(23 * M_PI / 16.0)));
          m_64apsk[15] = gr_complex((r1 * cos(21 * M_PI / 16.0)), (r1 * sin(21 * M_PI / 16.0)));
          m_64apsk[16] = gr_complex((r2 * cos(M_PI / 16.0)), (r2 * sin(M_PI / 16.0)));
          m_64apsk[17] = gr_complex((r2 * cos(3 * M_PI / 16.0)), (r2 * sin(3 * M_PI / 16.0)));
          m_64apsk[18] = gr_complex((r2 * cos(7 * M_PI / 16.0)), (r2 * sin(7 * M_PI / 16.0)));
          m_64apsk[19] = gr_complex((r2 * cos(5 * M_PI / 16.0)), (r2 * sin(5 * M_PI / 16.0)));
          m_64apsk[20] = gr_complex((r2 * cos(31 * M_PI / 16.0)), (r2 * sin(31* M_PI / 16.0)));
          m_64apsk[21] = gr_complex((r2 * cos(29 * M_PI / 16.0)), (r2 * sin(29 * M_PI / 16.0)));
          m_64apsk[22] = gr_complex((r2 * cos(25 * M_PI / 16.0)), (r2 * sin(25 * M_PI / 16.0)));
          m_64apsk[23] = gr_complex((r2 * cos(27 * M_PI / 16.0)), (r2 * sin(27 * M_PI / 16.0)));
          m_64apsk[24] = gr_complex((r2 * cos(15 * M_PI / 16.0)), (r2 * sin(15 * M_PI / 16.0)));
          m_64apsk[25] = gr_complex((r2 * cos(13 * M_PI / 16.0)), (r2 * sin(13 * M_PI / 16.0)));
          m_64apsk[26] = gr_complex((r2 * cos(9 * M_PI / 16.0)), (r2 * sin(9 * M_PI / 16.0)));
          m_64apsk[27] = gr_complex((r2 * cos(11 * M_PI / 16.0)), (r2 * sin(11 * M_PI / 16.0)));
          m_64apsk[28] = gr_complex((r2 * cos(17 * M_PI / 16.0)), (r2 * sin(17 * M_PI / 16.0)));
          m_64apsk[29] = gr_complex((r2 * cos(19 * M_PI / 16.0)), (r2 * sin(19 * M_PI / 16.0)));
          m_64apsk[30] = gr_complex((r2 * cos(23 * M_PI / 16.0)), (r2 * sin(23 * M_PI / 16.0)));
          m_64apsk[31] = gr_complex((r2 * cos(21 * M_PI / 16.0)), (r2 * sin(21 * M_PI / 16.0)));
          m_64apsk[32] = gr_complex((r4 * cos(M_PI / 16.0)), (r4 * sin(M_PI / 16.0)));
          m_64apsk[33] = gr_complex((r4 * cos(3 * M_PI / 16.0)), (r4 * sin(3 * M_PI / 16.0)));
          m_64apsk[34] = gr_complex((r4 * cos(7 * M_PI / 16.0)), (r4 * sin(7 * M_PI / 16.0)));
          m_64apsk[35] = gr_complex((r4 * cos(5 * M_PI / 16.0)), (r4 * sin(5 * M_PI / 16.0)));
          m_64apsk[36] = gr_complex((r4 * cos(31 * M_PI / 16.0)), (r4 * sin(31 * M_PI / 16.0)));
          m_64apsk[37] = gr_complex((r4 * cos(29 * M_PI / 16.0)), (r4 * sin(29 * M_PI / 16.0)));
          m_64apsk[38] = gr_complex((r4 * cos(25 * M_PI / 16.0)), (r4 * sin(25 * M_PI / 16.0)));
          m_64apsk[39] = gr_complex((r4 * cos(27 * M_PI / 16.0)), (r4 * sin(27 * M_PI / 16.0)));
          m_64apsk[40] = gr_complex((r4 * cos(15 * M_PI / 16.0)), (r4 * sin(15 * M_PI / 16.0)));
          m_64apsk[41] = gr_complex((r4 * cos(13 * M_PI / 16.0)), (r4 * sin(13 * M_PI / 16.0)));
          m_64apsk[42] = gr_complex((r4 * cos(9 * M_PI / 16.0)), (r4 * sin(9 * M_PI / 16.0)));
          m_64apsk[43] = gr_complex((r4 * cos(11 * M_PI / 16.0)), (r4 * sin(11 * M_PI / 16.0)));
          m_64apsk[44] = gr_complex((r4 * cos(17 * M_PI / 16.0)), (r4 * sin(17 * M_PI / 16.0)));
          m_64apsk[45] = gr_complex((r4 * cos(19 * M_PI / 16.0)), (r4 * sin(19 * M_PI / 16.0)));
          m_64apsk[46] = gr_complex((r4 * cos(23 * M_PI / 16.0)), (r4 * sin(23 * M_PI / 16.0)));
          m_64apsk[47] = gr_complex((r4 * cos(21 * M_PI / 16.0)), (r4 * sin(21 * M_PI / 16.0)));
          m_64apsk[48] = gr_complex((r3 * cos(M_PI / 16.0)), (r3 * sin(M_PI / 16.0)));
          m_64apsk[49] = gr_complex((r3 * cos(3 * M_PI / 16.0)), (r3 * sin(3 * M_PI / 16.0)));
          m_64apsk[50] = gr_complex((r3 * cos(7 * M_PI / 16.0)), (r3 * sin(7 * M_PI / 16.0)));
          m_64apsk[51] = gr_complex((r3 * cos(5 * M_PI / 16.0)), (r3 * sin(5 * M_PI / 16.0)));
          m_64apsk[52] = gr_complex((r3 * cos(31 * M_PI / 16.0)), (r3 * sin(31 * M_PI / 16.0)));
          m_64apsk[53] = gr_complex((r3 * cos(29 * M_PI / 16.0)), (r3 * sin(29 * M_PI / 16.0)));
          m_64apsk[54] = gr_complex((r3 * cos(25 * M_PI / 16.0)), (r3 * sin(25 * M_PI / 16.0)));
          m_64apsk[55] = gr_complex((r3 * cos(27 * M_PI / 16.0)), (r3 * sin(27 * M_PI / 16.0)));
          m_64apsk[56] = gr_complex((r3 * cos(15 * M_PI / 16.0)), (r3 * sin(15 * M_PI / 16.0)));
          m_64apsk[57] = gr_complex((r3 * cos(13 * M_PI / 16.0)), (r3 * sin(13 * M_PI / 16.0)));
          m_64apsk[58] = gr_complex((r3 * cos(9 * M_PI / 16.0)), (r3 * sin(9 * M_PI / 16.0)));
          m_64apsk[59] = gr_complex((r3 * cos(11 * M_PI / 16.0)), (r3 * sin(11 * M_PI / 16.0)));
          m_64apsk[60] = gr_complex((r3 * cos(17 * M_PI / 16.0)), (r3 * sin(17 * M_PI / 16.0)));
          m_64apsk[61] = gr_complex((r3 * cos(19 * M_PI / 16.0)), (r3 * sin(19 * M_PI / 16.0)));
          m_64apsk[62] = gr_complex((r3 * cos(23 * M_PI / 16.0)), (r3 * sin(23 * M_PI / 16.0)));
          m_64apsk[63] = gr_complex((r3 * cos(21 * M_PI / 16.0)), (r3 * sin(21 * M_PI / 16.0)));
          break;
        case MOD_8_16_20_20APSK:
          r4 = m;
          switch(rate) {
            case C7_9:
            case C4_5:
              r1 = r4 / 5.2;
              r3 = r1 * 3.6;
              r2 = r1 * 2.2;
              break;
            case C5_6:
              r1 = r4 / 5.0;
              r3 = r1 * 3.5;
              r2 = r1 * 2.2;
              break;
            default:
              r1 = 0;
              r2 = 0;
              r3 = 0;
              break;
          }
          m_64apsk[0] = gr_complex((r2 * cos(25 * M_PI / 16.0)), (r2 * sin(25 * M_PI / 16.0)));
          m_64apsk[1] = gr_complex((r4 * cos(7 * M_PI / 4.0)), (r4 * sin(7 * M_PI / 4.0)));
          m_64apsk[2] = gr_complex((r2 * cos(27 * M_PI / 16.0)), (r2 * sin(27 * M_PI / 16.0)));
          m_64apsk[3] = gr_complex((r3 * cos(7 * M_PI / 4.0)), (r3 * sin(7 * M_PI / 4.0)));
          m_64apsk[4] = gr_complex((r4 * cos(31 * M_PI / 20.0)), (r4 * sin(31 * M_PI / 20.0)));
          m_64apsk[5] = gr_complex((r4 * cos(33 * M_PI / 20.0)), (r4 * sin(33 * M_PI / 20.0)));
          m_64apsk[6] = gr_complex((r3 * cos(31 * M_PI / 20.0)), (r3 * sin(31 * M_PI / 20.0)));
          m_64apsk[7] = gr_complex((r3 * cos(33 * M_PI / 20.0)), (r3 * sin(33 * M_PI / 20.0)));
          m_64apsk[8] = gr_complex((r2 * cos(23 * M_PI / 16.0)), (r2 * sin(23 * M_PI / 16.0)));
          m_64apsk[9] = gr_complex((r4 * cos(5 * M_PI / 4.0)), (r4 * sin(5 * M_PI / 4.0)));
          m_64apsk[10] = gr_complex((r2 * cos(21 * M_PI / 16.0)), (r2 * sin(21 * M_PI / 16.0)));
          m_64apsk[11] = gr_complex((r3 * cos(5 * M_PI / 4.0)), (r3 * sin(5 * M_PI / 4.0)));
          m_64apsk[12] = gr_complex((r4 * cos(29 * M_PI / 20.0)), (r4 * sin(29 * M_PI / 20.0)));
          m_64apsk[13] = gr_complex((r4 * cos(27 * M_PI / 20.0)), (r4 * sin(27 * M_PI / 20.0)));
          m_64apsk[14] = gr_complex((r3 * cos(29 * M_PI / 20.0)), (r3 * sin(29 * M_PI / 20.0)));
          m_64apsk[15] = gr_complex((r3 * cos(27 * M_PI / 20.0)), (r3 * sin(27 * M_PI / 20.0)));
          m_64apsk[16] = gr_complex((r1 * cos(13 * M_PI / 8.0)), (r1 * sin(13 * M_PI / 8.0)));
          m_64apsk[17] = gr_complex((r4 * cos(37 * M_PI / 20.0)), (r4 * sin(37 * M_PI / 20.0)));
          m_64apsk[18] = gr_complex((r2 * cos(29 * M_PI / 16.0)), (r2 * sin(29 * M_PI / 16.0)));
          m_64apsk[19] = gr_complex((r3 * cos(37 * M_PI / 20.0)), (r3 * sin(37 * M_PI / 20.0)));
          m_64apsk[20] = gr_complex((r1 * cos(15 * M_PI / 8.0)), (r1 * sin(15 * M_PI / 8.0)));
          m_64apsk[21] = gr_complex((r4 * cos(39 * M_PI / 20.0)), (r4 * sin(39 * M_PI / 20.0)));
          m_64apsk[22] = gr_complex((r2 * cos(31 * M_PI / 16.0)), (r2 * sin(31 * M_PI / 16.0)));
          m_64apsk[23] = gr_complex((r3 * cos(39 * M_PI / 20.0)), (r3 * sin(39 * M_PI / 20.0)));
          m_64apsk[24] = gr_complex((r1 * cos(11 * M_PI / 8.0)), (r1 * sin(11 * M_PI / 8.0)));
          m_64apsk[25] = gr_complex((r4 * cos(23 * M_PI / 20.0)), (r4 * sin(23 * M_PI / 20.0)));
          m_64apsk[26] = gr_complex((r2 * cos(19 * M_PI / 16.0)), (r2 * sin(19 * M_PI / 16.0)));
          m_64apsk[27] = gr_complex((r3 * cos(23 * M_PI / 20.0)), (r3 * sin(23 * M_PI / 20.0)));
          m_64apsk[28] = gr_complex((r1 * cos(9 * M_PI / 8.0)), (r1 * sin(9 * M_PI / 8.0)));
          m_64apsk[29] = gr_complex((r4 * cos(21 * M_PI / 20.0)), (r4 * sin(21 * M_PI / 20.0)));
          m_64apsk[30] = gr_complex((r2 * cos(17 * M_PI / 16.0)), (r2 * sin(17 * M_PI / 16.0)));
          m_64apsk[31] = gr_complex((r3 * cos(21 * M_PI / 20.0)), (r3 * sin(21 * M_PI / 20.0)));
          m_64apsk[32] = gr_complex((r2 * cos(7 * M_PI / 16.0)), (r2 * sin(7 * M_PI / 16.0)));
          m_64apsk[33] = gr_complex((r4 * cos(M_PI / 4.0)), (r4 * sin(M_PI / 4.0)));
          m_64apsk[34] = gr_complex((r2 * cos(5 * M_PI / 16.0)), (r2 * sin(5 * M_PI / 16.0)));
          m_64apsk[35] = gr_complex((r3 * cos(M_PI / 4.0)), (r3 * sin(M_PI / 4.0)));
          m_64apsk[36] = gr_complex((r4 * cos(9 * M_PI / 20.0)), (r4 * sin(9 * M_PI / 20.0)));
          m_64apsk[37] = gr_complex((r4 * cos(7 * M_PI / 20.0)), (r4 * sin(7 * M_PI / 20.0)));
          m_64apsk[38] = gr_complex((r3 * cos(9 * M_PI / 20.0)), (r3 * sin(9 * M_PI / 20.0)));
          m_64apsk[39] = gr_complex((r3 * cos(7 * M_PI / 20.0)), (r3 * sin(7 * M_PI / 20.0)));
          m_64apsk[40] = gr_complex((r2 * cos(9 * M_PI / 16.0)), (r2 * sin(9 * M_PI / 16.0)));
          m_64apsk[41] = gr_complex((r4 * cos(3 * M_PI / 4.0)), (r4 * sin(3 * M_PI / 4.0)));
          m_64apsk[42] = gr_complex((r2 * cos(11 * M_PI / 16.0)), (r2 * sin(11 * M_PI / 16.0)));
          m_64apsk[43] = gr_complex((r3 * cos(3 * M_PI / 4.0)), (r3 * sin(3 * M_PI / 4.0)));
          m_64apsk[44] = gr_complex((r4 * cos(11 * M_PI / 20.0)), (r4 * sin(11 * M_PI / 20.0)));
          m_64apsk[45] = gr_complex((r4 * cos(13 * M_PI / 20.0)), (r4 * sin(13 * M_PI / 20.0)));
          m_64apsk[46] = gr_complex((r3 * cos(11 * M_PI / 20.0)), (r3 * sin(11 * M_PI / 20.0)));
          m_64apsk[47] = gr_complex((r3 * cos(13 * M_PI / 20.0)), (r3 * sin(13 * M_PI / 20.0)));
          m_64apsk[48] = gr_complex((r1 * cos(3 * M_PI / 8.0)), (r1 * sin(3 * M_PI / 8.0)));
          m_64apsk[49] = gr_complex((r4 * cos(3 * M_PI / 20.0)), (r4 * sin(3 * M_PI / 20.0)));
          m_64apsk[50] = gr_complex((r2 * cos(3 * M_PI / 16.0)), (r2 * sin(3 * M_PI / 16.0)));
          m_64apsk[51] = gr_complex((r3 * cos(3 * M_PI / 20.0)), (r3 * sin(3 * M_PI / 20.0)));
          m_64apsk[52] = gr_complex((r1 * cos(M_PI / 8.0)), (r1 * sin(M_PI / 8.0)));
          m_64apsk[53] = gr_complex((r4 * cos(M_PI / 20.0)), (r4 * sin(M_PI / 20.0)));
          m_64apsk[54] = gr_complex((r2 * cos(M_PI / 16.0)), (r2 * sin(M_PI / 16.0)));
          m_64apsk[55] = gr_complex((r3 * cos(M_PI / 20.0)), (r3 * sin(M_PI / 20.0)));
          m_64apsk[56] = gr_complex((r1 * cos(5 * M_PI / 8.0)), (r1 * sin(5 * M_PI / 8.0)));
          m_64apsk[57] = gr_complex((r4 * cos(17 * M_PI / 20.0)), (r4 * sin(17 * M_PI / 20.0)));
          m_64apsk[58] = gr_complex((r2 * cos(13 * M_PI / 16.0)), (r2 * sin(13 * M_PI / 16.0)));
          m_64apsk[59] = gr_complex((r3 * cos(17 * M_PI / 20.0)), (r3 * sin(17 * M_PI / 20.0)));
          m_64apsk[60] = gr_complex((r1 * cos(7 * M_PI / 8.0)), (r1 * sin(7 * M_PI / 8.0)));
          m_64apsk[61] = gr_complex((r4 * cos(19 * M_PI / 20.0)), (r4 * sin(19 * M_PI / 20.0)));
          m_64apsk[62] = gr_complex((r2 * cos(15 * M_PI / 16.0)), (r2 * sin(15 * M_PI / 16.0)));
          m_64apsk[63] = gr_complex((r3 * cos(19 * M_PI / 20.0)), (r3 * sin(19 * M_PI / 20.0)));
          break;
        case MOD_4_12_20_28APSK:
          r4 = m;
          switch(rate) {
            case C132_180:
              r1 = r4 / 7.0;
              r3 = r1 * 4.3;
              r2 = r1 * 2.4;
              break;
            default:
              r1 = 0;
              r2 = 0;
              r3 = 0;
              break;
          }
          m_64apsk[0] = gr_complex((r4 * cos(M_PI / 4.0)), (r4 * sin(M_PI / 4.0)));
          m_64apsk[1] = gr_complex((r4 * cos(7 * M_PI / 4.0)), (r4 * sin(7 * M_PI / 4.0)));
          m_64apsk[2] = gr_complex((r4 * cos(3 * M_PI / 4.0)), (r4 * sin(3 * M_PI / 4.0)));
          m_64apsk[3] = gr_complex((r4 * cos(5 * M_PI / 4.0)), (r4 * sin(5 * M_PI / 4.0)));
          m_64apsk[4] = gr_complex((r4 * cos(13 * M_PI / 28.0)), (r4 * sin(13 * M_PI / 28.0)));
          m_64apsk[5] = gr_complex((r4 * cos(43 * M_PI / 28.0)), (r4 * sin(43 * M_PI / 28.0)));
          m_64apsk[6] = gr_complex((r4 * cos(15 * M_PI / 28.0)), (r4 * sin(15 * M_PI / 28.0)));
          m_64apsk[7] = gr_complex((r4 * cos(41 * M_PI / 28.0)), (r4 * sin(41 * M_PI / 28.0)));
          m_64apsk[8] = gr_complex((r4 * cos(M_PI / 28.0)), (r4 * sin(M_PI / 28.0)));
          m_64apsk[9] = gr_complex((r4 * cos(55 * M_PI / 28.0)), (r4 * sin(55 * M_PI / 28.0)));
          m_64apsk[10] = gr_complex((r4 * cos(27 * M_PI / 28.0)), (r4 * sin(27 * M_PI / 28.0)));
          m_64apsk[11] = gr_complex((r4 * cos(29 * M_PI / 28.0)), (r4 * sin(29 * M_PI / 28.0)));
          m_64apsk[12] = gr_complex((r1 * cos(M_PI / 4.0)), (r1 * sin(M_PI / 4.0)));
          m_64apsk[13] = gr_complex((r1 * cos(7 * M_PI / 4.0)), (r1 * sin(7 * M_PI / 4.0)));
          m_64apsk[14] = gr_complex((r1 * cos(3 * M_PI / 4.0)), (r1 * sin(3 * M_PI / 4.0)));
          m_64apsk[15] = gr_complex((r1 * cos(5 * M_PI / 4.0)), (r1 * sin(5 * M_PI / 4.0)));
          m_64apsk[16] = gr_complex((r4 * cos(9 * M_PI / 28.0)), (r4 * sin(9 * M_PI / 28.0)));
          m_64apsk[17] = gr_complex((r4 * cos(47 * M_PI / 28.0)), (r4 * sin(47 * M_PI / 28.0)));
          m_64apsk[18] = gr_complex((r4 * cos(19 * M_PI / 28.0)), (r4 * sin(19 * M_PI / 28.0)));
          m_64apsk[19] = gr_complex((r4 * cos(37 * M_PI / 28.0)), (r4 * sin(37 * M_PI / 28.0)));
          m_64apsk[20] = gr_complex((r4 * cos(11 * M_PI / 28.0)), (r4 * sin(11 * M_PI / 28.0)));
          m_64apsk[21] = gr_complex((r4 * cos(45 * M_PI / 28.0)), (r4 * sin(45 * M_PI / 28.0)));
          m_64apsk[22] = gr_complex((r4 * cos(17 * M_PI / 28.0)), (r4 * sin(17 * M_PI / 28.0)));
          m_64apsk[23] = gr_complex((r4 * cos(39 * M_PI / 28.0)), (r4 * sin(39 * M_PI / 28.0)));
          m_64apsk[24] = gr_complex((r3 * cos(M_PI / 20.0)), (r3 * sin(M_PI / 20.0)));
          m_64apsk[25] = gr_complex((r3 * cos(39 * M_PI / 20.0)), (r3 * sin(39 * M_PI / 20.0)));
          m_64apsk[26] = gr_complex((r3 * cos(19 * M_PI / 20.0)), (r3 * sin(19 * M_PI / 20.0)));
          m_64apsk[27] = gr_complex((r3 * cos(21 * M_PI / 20.0)), (r3 * sin(21 * M_PI / 20.0)));
          m_64apsk[28] = gr_complex((r2 * cos(M_PI / 12.0)), (r2 * sin(M_PI / 12.0)));
          m_64apsk[29] = gr_complex((r2 * cos(23 * M_PI / 12.0)), (r2 * sin(23 * M_PI / 12.0)));
          m_64apsk[30] = gr_complex((r2 * cos(11 * M_PI / 12.0)), (r2 * sin(11 * M_PI / 12.0)));
          m_64apsk[31] = gr_complex((r2 * cos(13 * M_PI / 12.0)), (r2 * sin(13 * M_PI / 12.0)));
          m_64apsk[32] = gr_complex((r4 * cos(5 * M_PI / 28.0)), (r4 * sin(5 * M_PI / 28.0)));
          m_64apsk[33] = gr_complex((r4 * cos(51 * M_PI / 28.0)), (r4 * sin(51 * M_PI / 28.0)));
          m_64apsk[34] = gr_complex((r4 * cos(23 * M_PI / 28.0)), (r4 * sin(23 * M_PI / 28.0)));
          m_64apsk[35] = gr_complex((r4 * cos(33 * M_PI / 28.0)), (r4 * sin(33 * M_PI / 28.0)));
          m_64apsk[36] = gr_complex((r3 * cos(9 * M_PI / 20.0)), (r3 * sin(9 * M_PI / 20.0)));
          m_64apsk[37] = gr_complex((r3 * cos(31 * M_PI / 20.0)), (r3 * sin(31 * M_PI / 20.0)));
          m_64apsk[38] = gr_complex((r3 * cos(11 * M_PI / 20.0)), (r3 * sin(11 * M_PI / 20.0)));
          m_64apsk[39] = gr_complex((r3 * cos(29 * M_PI / 20.0)), (r3 * sin(29 * M_PI / 20.0)));
          m_64apsk[40] = gr_complex((r4 * cos(3 * M_PI / 28.0)), (r4 * sin(3 * M_PI / 28.0)));
          m_64apsk[41] = gr_complex((r4 * cos(53 * M_PI / 28.0)), (r4 * sin(53 * M_PI / 28.0)));
          m_64apsk[42] = gr_complex((r4 * cos(25 * M_PI / 28.0)), (r4 * sin(25 * M_PI / 28.0)));
          m_64apsk[43] = gr_complex((r4 * cos(31 * M_PI / 28.0)), (r4 * sin(31 * M_PI / 28.0)));
          m_64apsk[44] = gr_complex((r2 * cos(5 * M_PI / 12.0)), (r2 * sin(5 * M_PI / 12.0)));
          m_64apsk[45] = gr_complex((r2 * cos(19 * M_PI / 12.0)), (r2 * sin(19 * M_PI / 12.0)));
          m_64apsk[46] = gr_complex((r2 * cos(7 * M_PI / 12.0)), (r2 * sin(7 * M_PI / 12.0)));
          m_64apsk[47] = gr_complex((r2 * cos(17 * M_PI / 12.0)), (r2 * sin(17 * M_PI / 12.0)));
          m_64apsk[48] = gr_complex((r3 * cos(M_PI / 4.0)), (r3 * sin(M_PI / 4.0)));
          m_64apsk[49] = gr_complex((r3 * cos(7 * M_PI / 4.0)), (r3 * sin(7 * M_PI / 4.0)));
          m_64apsk[50] = gr_complex((r3 * cos(3 * M_PI / 4.0)), (r3 * sin(3 * M_PI / 4.0)));
          m_64apsk[51] = gr_complex((r3 * cos(5 * M_PI / 4.0)), (r3 * sin(5 * M_PI / 4.0)));
          m_64apsk[52] = gr_complex((r3 * cos(7 * M_PI / 20.0)), (r3 * sin(7 * M_PI / 20.0)));
          m_64apsk[53] = gr_complex((r3 * cos(33 * M_PI / 20.0)), (r3 * sin(33 * M_PI / 20.0)));
          m_64apsk[54] = gr_complex((r3 * cos(13 * M_PI / 20.0)), (r3 * sin(13 * M_PI / 20.0)));
          m_64apsk[55] = gr_complex((r3 * cos(27 * M_PI / 20.0)), (r3 * sin(27 * M_PI / 20.0)));
          m_64apsk[56] = gr_complex((r3 * cos(3 * M_PI / 20.0)), (r3 * sin(3 * M_PI / 20.0)));
          m_64apsk[57] = gr_complex((r3 * cos(37 * M_PI / 20.0)), (r3 * sin(37 * M_PI / 20.0)));
          m_64apsk[58] = gr_complex((r3 * cos(17 * M_PI / 20.0)), (r3 * sin(17 * M_PI / 20.0)));
          m_64apsk[59] = gr_complex((r3 * cos(23 * M_PI / 20.0)), (r3 * sin(23 * M_PI / 20.0)));
          m_64apsk[60] = gr_complex((r2 * cos(M_PI / 4.0)), (r2 * sin(M_PI / 4.0)));
          m_64apsk[61] = gr_complex((r2 * cos(7 * M_PI / 4.0)), (r2 * sin(7 * M_PI / 4.0)));
          m_64apsk[62] = gr_complex((r2 * cos(3 * M_PI / 4.0)), (r2 * sin(3 * M_PI / 4.0)));
          m_64apsk[63] = gr_complex((r2 * cos(5 * M_PI / 4.0)), (r2 * sin(5 * M_PI / 4.0)));
          break;
        case MOD_128APSK:
          r6 = m;
          switch(rate) {
            case C135_180:
              r1 = r6 / 3.819;
              r5 = r1 * 2.75;
              r4 = r1 * 2.681;
              r3 = r1 * 2.118;
              r2 = r1 * 1.715;
              break;
            case C140_180:
              r1 = r6 / 3.733;
              r5 = r1 * 2.75;
              r4 = r1 * 2.681;
              r3 = r1 * 2.118;
              r2 = r1 * 1.715;
              break;
            default:
              r1 = 0;
              r2 = 0;
              r3 = 0;
              r4 = 0;
              r5 = 0;
              break;
          }
          m_128apsk[0] = gr_complex((r1 * cos(83 * M_PI / 1260.0)), (r1 * sin(83 * M_PI / 1260.0)));
          m_128apsk[1] = gr_complex((r6 * cos(11 * M_PI / 105.0)), (r6 * sin(11 * M_PI / 105.0)));
          m_128apsk[2] = gr_complex((r6 * cos(37 * M_PI / 1680.0)), (r6 * sin(37 * M_PI / 1680.0)));
          m_128apsk[3] = gr_complex((r6 * cos(11 * M_PI / 168.0)), (r6 * sin(11 * M_PI / 168.0)));
          m_128apsk[4] = gr_complex((r2 * cos(121 * M_PI / 2520.0)), (r2 * sin(121 * M_PI / 2520.0)));
          m_128apsk[5] = gr_complex((r3 * cos(23 * M_PI / 280.0)), (r3 * sin(23 * M_PI / 280.0)));
          m_128apsk[6] = gr_complex((r5 * cos(19 * M_PI / 720.0)), (r5 * sin(19 * M_PI / 720.0)));
          m_128apsk[7] = gr_complex((r4 * cos(61 * M_PI / 720.0)), (r4 * sin(61 * M_PI / 720.0)));
          m_128apsk[8] = gr_complex((r1 * cos(103 * M_PI / 560.0)), (r1 * sin(103 * M_PI / 560.0)));
          m_128apsk[9] = gr_complex((r6 * cos(61 * M_PI / 420.0)), (r6 * sin(61 * M_PI / 420.0)));
          m_128apsk[10] = gr_complex((r6 * cos(383 * M_PI / 1680.0)), (r6 * sin(383 * M_PI / 1680.0)));
          m_128apsk[11] = gr_complex((r6 * cos(929 * M_PI / 5040.0)), (r6 * sin(929 * M_PI / 5040.0)));
          m_128apsk[12] = gr_complex((r2 * cos(113 * M_PI / 560.0)), (r2 * sin(113 * M_PI / 560.0)));
          m_128apsk[13] = gr_complex((r3 * cos(169 * M_PI / 1008.0)), (r3 * sin(169 * M_PI / 1008.0)));
          m_128apsk[14] = gr_complex((r5 * cos(563 * M_PI / 2520.0)), (r5 * sin(563 * M_PI / 2520.0)));
          m_128apsk[15] = gr_complex((r4 * cos(139 * M_PI / 840.0)), (r4 * sin(139 * M_PI / 840.0)));
          m_128apsk[16] = gr_complex((r1 * cos(243 * M_PI / 560.0)), (r1 * sin(243 * M_PI / 560.0)));
          m_128apsk[17] = gr_complex((r6 * cos(1993 * M_PI / 5040.0)), (r6 * sin(1993 * M_PI / 5040.0)));
          m_128apsk[18] = gr_complex((r6 * cos(43 * M_PI / 90.0)), (r6 * sin(43 * M_PI / 90.0)));
          m_128apsk[19] = gr_complex((r6 * cos(73 * M_PI / 168.0)), (r6 * sin(73 * M_PI / 168.0)));
          m_128apsk[20] = gr_complex((r2 * cos(1139 * M_PI / 2520.0)), (r2 * sin(1139 * M_PI / 2520.0)));
          m_128apsk[21] = gr_complex((r3 * cos(117 * M_PI / 280.0)), (r3 * sin(117 * M_PI / 280.0)));
          m_128apsk[22] = gr_complex((r5 * cos(341 * M_PI / 720.0)), (r5 * sin(341 * M_PI / 720.0)));
          m_128apsk[23] = gr_complex((r4 * cos(349 * M_PI / 840.0)), (r4 * sin(349 * M_PI / 840.0)));
          m_128apsk[24] = gr_complex((r1 * cos(177 * M_PI / 560.0)), (r1 * sin(177 * M_PI / 560.0)));
          m_128apsk[25] = gr_complex((r6 * cos(1789 * M_PI / 5040.0)), (r6 * sin(1789 * M_PI / 5040.0)));
          m_128apsk[26] = gr_complex((r6 * cos(49 * M_PI / 180.0)), (r6 * sin(49 * M_PI / 180.0)));
          m_128apsk[27] = gr_complex((r6 * cos(53 * M_PI / 168.0)), (r6 * sin(53 * M_PI / 168.0)));
          m_128apsk[28] = gr_complex((r2 * cos(167 * M_PI / 560.0)), (r2 * sin(167 * M_PI / 560.0)));
          m_128apsk[29] = gr_complex((r3 * cos(239 * M_PI / 720.0)), (r3 * sin(239 * M_PI / 720.0)));
          m_128apsk[30] = gr_complex((r5 * cos(199 * M_PI / 720.0)), (r5 * sin(199 * M_PI / 720.0)));
          m_128apsk[31] = gr_complex((r4 * cos(281 * M_PI / 840.0)), (r4 * sin(281 * M_PI / 840.0)));
          m_128apsk[32] = gr_complex((r1 * cos(1177 * M_PI / 1260.0)), (r1 * sin(1177 * M_PI / 1260.0)));
          m_128apsk[33] = gr_complex((r6 * cos(94 * M_PI / 105.0)), (r6 * sin(94 * M_PI / 105.0)));
          m_128apsk[34] = gr_complex((r6 * cos(1643 * M_PI / 1680.0)), (r6 * sin(1643 * M_PI / 1680.0)));
          m_128apsk[35] = gr_complex((r6 * cos(157 * M_PI / 168.0)), (r6 * sin(157 * M_PI / 168.0)));
          m_128apsk[36] = gr_complex((r2 * cos(2399 * M_PI / 2520.0)), (r2 * sin(2399 * M_PI / 2520.0)));
          m_128apsk[37] = gr_complex((r3 * cos(257 * M_PI / 280.0)), (r3 * sin(257 * M_PI / 280.0)));
          m_128apsk[38] = gr_complex((r5 * cos(701 * M_PI / 720.0)), (r5 * sin(701 * M_PI / 720.0)));
          m_128apsk[39] = gr_complex((r4 * cos(659 * M_PI / 720.0)), (r4 * sin(659 * M_PI / 720.0)));
          m_128apsk[40] = gr_complex((r1 * cos(457 * M_PI / 560.0)), (r1 * sin(457 * M_PI / 560.0)));
          m_128apsk[41] = gr_complex((r6 * cos(359 * M_PI / 420.0)), (r6 * sin(359 * M_PI / 420.0)));
          m_128apsk[42] = gr_complex((r6 * cos(1297 * M_PI / 1680.0)), (r6 * sin(1297 * M_PI / 1680.0)));
          m_128apsk[43] = gr_complex((r6 * cos(4111 * M_PI / 5040.0)), (r6 * sin(4111 * M_PI / 5040.0)));
          m_128apsk[44] = gr_complex((r2 * cos(447 * M_PI / 560.0)), (r2 * sin(447 * M_PI / 560.0)));
          m_128apsk[45] = gr_complex((r3 * cos(839 * M_PI / 1008.0)), (r3 * sin(839 * M_PI / 1008.0)));
          m_128apsk[46] = gr_complex((r5 * cos(1957 * M_PI / 2520.0)), (r5 * sin(1957 * M_PI / 2520.0)));
          m_128apsk[47] = gr_complex((r4 * cos(701 * M_PI / 840.0)), (r4 * sin(701 * M_PI / 840.0)));
          m_128apsk[48] = gr_complex((r1 * cos(317 * M_PI / 560.0)), (r1 * sin(317 * M_PI / 560.0)));
          m_128apsk[49] = gr_complex((r6 * cos(3047 * M_PI / 5040.0)), (r6 * sin(3047 * M_PI / 5040.0)));
          m_128apsk[50] = gr_complex((r6 * cos(47 * M_PI / 90.0)), (r6 * sin(47 * M_PI / 90.0)));
          m_128apsk[51] = gr_complex((r6 * cos(95 * M_PI / 168.0)), (r6 * sin(95 * M_PI / 168.0)));
          m_128apsk[52] = gr_complex((r2 * cos(1381 * M_PI / 2520.0)), (r2 * sin(1381 * M_PI / 2520.0)));
          m_128apsk[53] = gr_complex((r3 * cos(163 * M_PI / 280.0)), (r3 * sin(163 * M_PI / 280.0)));
          m_128apsk[54] = gr_complex((r5 * cos(379 * M_PI / 720.0)), (r5 * sin(379 * M_PI / 720.0)));
          m_128apsk[55] = gr_complex((r4 * cos(491 * M_PI / 840.0)), (r4 * sin(491 * M_PI / 840.0)));
          m_128apsk[56] = gr_complex((r1 * cos(383 * M_PI / 560.0)), (r1 * sin(383 * M_PI / 560.0)));
          m_128apsk[57] = gr_complex((r6 * cos(3251 * M_PI / 5040.0)), (r6 * sin(3251 * M_PI / 5040.0)));
          m_128apsk[58] = gr_complex((r6 * cos(131 * M_PI / 180.0)), (r6 * sin(131 * M_PI / 180.0)));
          m_128apsk[59] = gr_complex((r6 * cos(115 * M_PI / 168.0)), (r6 * sin(115 * M_PI / 168.0)));
          m_128apsk[60] = gr_complex((r2 * cos(393 * M_PI / 560.0)), (r2 * sin(393 * M_PI / 560.0)));
          m_128apsk[61] = gr_complex((r3 * cos(481 * M_PI / 720.0)), (r3 * sin(481 * M_PI / 720.0)));
          m_128apsk[62] = gr_complex((r5 * cos(521 * M_PI / 720.0)), (r5 * sin(521 * M_PI / 720.0)));
          m_128apsk[63] = gr_complex((r4 * cos(559 * M_PI / 840.0)), (r4 * sin(559 * M_PI / 840.0)));
          m_128apsk[64] = gr_complex((r1 * cos(2437 * M_PI / 1260.0)), (r1 * sin(2437 * M_PI / 1260.0)));
          m_128apsk[65] = gr_complex((r6 * cos(199 * M_PI / 105.0)), (r6 * sin(199 * M_PI / 105.0)));
          m_128apsk[66] = gr_complex((r6 * cos(3323 * M_PI / 1680.0)), (r6 * sin(3323 * M_PI / 1680.0)));
          m_128apsk[67] = gr_complex((r6 * cos(325 * M_PI / 168.0)), (r6 * sin(325 * M_PI / 168.0)));
          m_128apsk[68] = gr_complex((r2 * cos(4919 * M_PI / 2520.0)), (r2 * sin(4919 * M_PI / 2520.0)));
          m_128apsk[69] = gr_complex((r3 * cos(537 * M_PI / 280.0)), (r3 * sin(537 * M_PI / 280.0)));
          m_128apsk[70] = gr_complex((r5 * cos(1421 * M_PI / 720.0)), (r5 * sin(1421 * M_PI / 720.0)));
          m_128apsk[71] = gr_complex((r4 * cos(1379 * M_PI / 720.0)), (r4 * sin(1379 * M_PI / 720.0)));
          m_128apsk[72] = gr_complex((r1 * cos(1017 * M_PI / 560.0)), (r1 * sin(1017 * M_PI / 560.0)));
          m_128apsk[73] = gr_complex((r6 * cos(779 * M_PI / 420.0)), (r6 * sin(779 * M_PI / 420.0)));
          m_128apsk[74] = gr_complex((r6 * cos(2977 * M_PI / 1680.0)), (r6 * sin(2977 * M_PI / 1680.0)));
          m_128apsk[75] = gr_complex((r6 * cos(9151 * M_PI / 5040.0)), (r6 * sin(9151 * M_PI / 5040.0)));
          m_128apsk[76] = gr_complex((r2 * cos(1007 * M_PI / 560.0)), (r2 * sin(1007 * M_PI / 560.0)));
          m_128apsk[77] = gr_complex((r3 * cos(1847 * M_PI / 1008.0)), (r3 * sin(1847 * M_PI / 1008.0)));
          m_128apsk[78] = gr_complex((r5 * cos(4477 * M_PI / 2520.0)), (r5 * sin(4477 * M_PI / 2520.0)));
          m_128apsk[79] = gr_complex((r4 * cos(1541 * M_PI / 840.0)), (r4 * sin(1541 * M_PI / 840.0)));
          m_128apsk[80] = gr_complex((r1 * cos(877 * M_PI / 560.0)), (r1 * sin(877 * M_PI / 560.0)));
          m_128apsk[81] = gr_complex((r6 * cos(8087 * M_PI / 5040.0)), (r6 * sin(8087 * M_PI / 5040.0)));
          m_128apsk[82] = gr_complex((r6 * cos(137 * M_PI / 90.0)), (r6 * sin(137 * M_PI / 90.0)));
          m_128apsk[83] = gr_complex((r6 * cos(263 * M_PI / 168.0)), (r6 * sin(263 * M_PI / 168.0)));
          m_128apsk[84] = gr_complex((r2 * cos(3901 * M_PI / 2520.0)), (r2 * sin(3901 * M_PI / 2520.0)));
          m_128apsk[85] = gr_complex((r3 * cos(443 * M_PI / 280.0)), (r3 * sin(443 * M_PI / 280.0)));
          m_128apsk[86] = gr_complex((r5 * cos(1099 * M_PI / 720.0)), (r5 * sin(1099 * M_PI / 720.0)));
          m_128apsk[87] = gr_complex((r4 * cos(1331 * M_PI / 840.0)), (r4 * sin(1331 * M_PI / 840.0)));
          m_128apsk[88] = gr_complex((r1 * cos(943 * M_PI / 560.0)), (r1 * sin(943 * M_PI / 560.0)));
          m_128apsk[89] = gr_complex((r6 * cos(8291 * M_PI / 5040.0)), (r6 * sin(8291 * M_PI / 5040.0)));
          m_128apsk[90] = gr_complex((r6 * cos(311 * M_PI / 180.0)), (r6 * sin(311 * M_PI / 180.0)));
          m_128apsk[91] = gr_complex((r6 * cos(283 * M_PI / 168.0)), (r6 * sin(283 * M_PI / 168.0)));
          m_128apsk[92] = gr_complex((r2 * cos(953 * M_PI / 560.0)), (r2 * sin(953 * M_PI / 560.0)));
          m_128apsk[93] = gr_complex((r3 * cos(1201 * M_PI / 720.0)), (r3 * sin(1201 * M_PI / 720.0)));
          m_128apsk[94] = gr_complex((r5 * cos(1241 * M_PI / 720.0)), (r5 * sin(1241 * M_PI / 720.0)));
          m_128apsk[95] = gr_complex((r4 * cos(1399 * M_PI / 840.0)), (r4 * sin(1399 * M_PI / 840.0)));
          m_128apsk[96] = gr_complex((r1 * cos(1343 * M_PI / 1260.0)), (r1 * sin(1343 * M_PI / 1260.0)));
          m_128apsk[97] = gr_complex((r6 * cos(116 * M_PI / 105.0)), (r6 * sin(116 * M_PI / 105.0)));
          m_128apsk[98] = gr_complex((r6 * cos(1717 * M_PI / 1680.0)), (r6 * sin(1717 * M_PI / 1680.0)));
          m_128apsk[99] = gr_complex((r6 * cos(179 * M_PI / 168.0)), (r6 * sin(179 * M_PI / 168.0)));
          m_128apsk[100] = gr_complex((r2 * cos(2641 * M_PI / 2520.0)), (r2 * sin(2641 * M_PI / 2520.0)));
          m_128apsk[101] = gr_complex((r3 * cos(303 * M_PI / 280.0)), (r3 * sin(303 * M_PI / 280.0)));
          m_128apsk[102] = gr_complex((r5 * cos(739 * M_PI / 720.0)), (r5 * sin(739 * M_PI / 720.0)));
          m_128apsk[103] = gr_complex((r4 * cos(781 * M_PI / 720.0)), (r4 * sin(781 * M_PI / 720.0)));
          m_128apsk[104] = gr_complex((r1 * cos(663 * M_PI / 560.0)), (r1 * sin(663 * M_PI / 560.0)));
          m_128apsk[105] = gr_complex((r6 * cos(481 * M_PI / 420.0)), (r6 * sin(481 * M_PI / 420.0)));
          m_128apsk[106] = gr_complex((r6 * cos(2063 * M_PI / 1680.0)), (r6 * sin(2063 * M_PI / 1680.0)));
          m_128apsk[107] = gr_complex((r6 * cos(5969 * M_PI / 5040.0)), (r6 * sin(5969 * M_PI / 5040.0)));
          m_128apsk[108] = gr_complex((r2 * cos(673 * M_PI / 560.0)), (r2 * sin(673 * M_PI / 560.0)));
          m_128apsk[109] = gr_complex((r3 * cos(1177 * M_PI / 1008.0)), (r3 * sin(1177 * M_PI / 1008.0)));
          m_128apsk[110] = gr_complex((r5 * cos(3083 * M_PI / 2520.0)), (r5 * sin(3083 * M_PI / 2520.0)));
          m_128apsk[111] = gr_complex((r4 * cos(979 * M_PI / 840.0)), (r4 * sin(979 * M_PI / 840.0)));
          m_128apsk[112] = gr_complex((r1 * cos(803 * M_PI / 560.0)), (r1 * sin(803 * M_PI / 560.0)));
          m_128apsk[113] = gr_complex((r6 * cos(7033 * M_PI / 5040.0)), (r6 * sin(7033 * M_PI / 5040.0)));
          m_128apsk[114] = gr_complex((r6 * cos(133 * M_PI / 90.0)), (r6 * sin(133 * M_PI / 90.0)));
          m_128apsk[115] = gr_complex((r6 * cos(241 * M_PI / 168.0)), (r6 * sin(241 * M_PI / 168.0)));
          m_128apsk[116] = gr_complex((r2 * cos(3659 * M_PI / 2520.0)), (r2 * sin(3659 * M_PI / 2520.0)));
          m_128apsk[117] = gr_complex((r3 * cos(397 * M_PI / 280.0)), (r3 * sin(397 * M_PI / 280.0)));
          m_128apsk[118] = gr_complex((r5 * cos(1061 * M_PI / 720.0)), (r5 * sin(1061 * M_PI / 720.0)));
          m_128apsk[119] = gr_complex((r4 * cos(1189 * M_PI / 840.0)), (r4 * sin(1189 * M_PI / 840.0)));
          m_128apsk[120] = gr_complex((r1 * cos(737 * M_PI / 560.0)), (r1 * sin(737 * M_PI / 560.0)));
          m_128apsk[121] = gr_complex((r6 * cos(6829 * M_PI / 5040.0)), (r6 * sin(6829 * M_PI / 5040.0)));
          m_128apsk[122] = gr_complex((r6 * cos(229 * M_PI / 180.0)), (r6 * sin(229 * M_PI / 180.0)));
          m_128apsk[123] = gr_complex((r6 * cos(221 * M_PI / 168.0)), (r6 * sin(221 * M_PI / 168.0)));
          m_128apsk[124] = gr_complex((r2 * cos(727 * M_PI / 560.0)), (r2 * sin(727 * M_PI / 560.0)));
          m_128apsk[125] = gr_complex((r3 * cos(959 * M_PI / 720.0)), (r3 * sin(959 * M_PI / 720.0)));
          m_128apsk[126] = gr_complex((r5 * cos(919 * M_PI / 720.0)), (r5 * sin(919 * M_PI / 720.0)));
          m_128apsk[127] = gr_complex((r4 * cos(1121 * M_PI / 840.0)), (r4 * sin(1121 * M_PI / 840.0)));
          break;
        case MOD_256APSK:
          if (rate == C20_30) {
            m_256apsk[0] = gr_complex(1.6350, 0.1593);
            m_256apsk[1] = gr_complex(1.5776, 0.4735);
            m_256apsk[2] = gr_complex(0.9430, 0.1100);
            m_256apsk[3] = gr_complex(0.9069, 0.2829);
            m_256apsk[4] = gr_complex(0.3237, 0.0849);
            m_256apsk[5] = gr_complex(0.3228, 0.0867);
            m_256apsk[6] = gr_complex(0.7502, 0.1138);
            m_256apsk[7] = gr_complex(0.7325, 0.2088);
            m_256apsk[8] = gr_complex(0.1658, 1.6747);
            m_256apsk[9] = gr_complex(0.4907, 1.6084);
            m_256apsk[10] = gr_complex(0.1088, 0.9530);
            m_256apsk[11] = gr_complex(0.2464, 0.9270);
            m_256apsk[12] = gr_complex(0.0872, 0.1390);
            m_256apsk[13] = gr_complex(0.0871, 0.1392);
            m_256apsk[14] = gr_complex(0.1091, 0.7656);
            m_256apsk[15] = gr_complex(0.1699, 0.7537);
            m_256apsk[16] = gr_complex(-1.6350, 0.1593);
            m_256apsk[17] = gr_complex(-1.5776, 0.4735);
            m_256apsk[18] = gr_complex(-0.9430, 0.1100);
            m_256apsk[19] = gr_complex(-0.9069, 0.2829);
            m_256apsk[20] = gr_complex(-0.3237, 0.0849);
            m_256apsk[21] = gr_complex(-0.3228, 0.0867);
            m_256apsk[22] = gr_complex(-0.7502, 0.1138);
            m_256apsk[23] = gr_complex(-0.7325, 0.2088);
            m_256apsk[24] = gr_complex(-0.1658, 1.6747);
            m_256apsk[25] = gr_complex(-0.4907, 1.6084);
            m_256apsk[26] = gr_complex(-0.1088, 0.9530);
            m_256apsk[27] = gr_complex(-0.2464, 0.9270);
            m_256apsk[28] = gr_complex(-0.0872, 0.1390);
            m_256apsk[29] = gr_complex(-0.0871, 0.1392);
            m_256apsk[30] = gr_complex(-0.1091, 0.7656);
            m_256apsk[31] = gr_complex(-0.1699, 0.7537);
            m_256apsk[32] = gr_complex(1.3225, 0.1320);
            m_256apsk[33] = gr_complex(1.2742, 0.3922);
            m_256apsk[34] = gr_complex(1.0854, 0.1139);
            m_256apsk[35] = gr_complex(1.0441, 0.3296);
            m_256apsk[36] = gr_complex(0.4582, 0.1123);
            m_256apsk[37] = gr_complex(0.4545, 0.1251);
            m_256apsk[38] = gr_complex(0.6473, 0.1138);
            m_256apsk[39] = gr_complex(0.6339, 0.1702);
            m_256apsk[40] = gr_complex(0.1322, 1.3631);
            m_256apsk[41] = gr_complex(0.3929, 1.3102);
            m_256apsk[42] = gr_complex(0.1124, 1.1327);
            m_256apsk[43] = gr_complex(0.3160, 1.0913);
            m_256apsk[44] = gr_complex(0.0928, 0.3970);
            m_256apsk[45] = gr_complex(0.0937, 0.3973);
            m_256apsk[46] = gr_complex(0.1054, 0.5979);
            m_256apsk[47] = gr_complex(0.1230, 0.5949);
            m_256apsk[48] = gr_complex(-1.3225, 0.1320);
            m_256apsk[49] = gr_complex(-1.2742, 0.3922);
            m_256apsk[50] = gr_complex(-1.0854, 0.1139);
            m_256apsk[51] = gr_complex(-1.0441, 0.3296);
            m_256apsk[52] = gr_complex(-0.4582, 0.1123);
            m_256apsk[53] = gr_complex(-0.4545, 0.1251);
            m_256apsk[54] = gr_complex(-0.6473, 0.1138);
            m_256apsk[55] = gr_complex(-0.6339, 0.1702);
            m_256apsk[56] = gr_complex(-0.1322, 1.3631);
            m_256apsk[57] = gr_complex(-0.3929, 1.3102);
            m_256apsk[58] = gr_complex(-0.1124, 1.1327);
            m_256apsk[59] = gr_complex(-0.3160, 1.0913);
            m_256apsk[60] = gr_complex(-0.0928, 0.3970);
            m_256apsk[61] = gr_complex(-0.0937, 0.3973);
            m_256apsk[62] = gr_complex(-0.1054, 0.5979);
            m_256apsk[63] = gr_complex(-0.1230, 0.5949);
            m_256apsk[64] = gr_complex(1.6350, -0.1593);
            m_256apsk[65] = gr_complex(1.5776, -0.4735);
            m_256apsk[66] = gr_complex(0.9430, -0.1100);
            m_256apsk[67] = gr_complex(0.9069, -0.2829);
            m_256apsk[68] = gr_complex(0.3237, -0.0849);
            m_256apsk[69] = gr_complex(0.3228, -0.0867);
            m_256apsk[70] = gr_complex(0.7502, -0.1138);
            m_256apsk[71] = gr_complex(0.7325, -0.2088);
            m_256apsk[72] = gr_complex(0.1658, -1.6747);
            m_256apsk[73] = gr_complex(0.4907, -1.6084);
            m_256apsk[74] = gr_complex(0.1088, -0.9530);
            m_256apsk[75] = gr_complex(0.2464, -0.9270);
            m_256apsk[76] = gr_complex(0.0872, -0.1390);
            m_256apsk[77] = gr_complex(0.0871, -0.1392);
            m_256apsk[78] = gr_complex(0.1091, -0.7656);
            m_256apsk[79] = gr_complex(0.1699, -0.7537);
            m_256apsk[80] = gr_complex(-1.6350, -0.1593);
            m_256apsk[81] = gr_complex(-1.5776, -0.4735);
            m_256apsk[82] = gr_complex(-0.9430, -0.1100);
            m_256apsk[83] = gr_complex(-0.9069, -0.2829);
            m_256apsk[84] = gr_complex(-0.3237, -0.0849);
            m_256apsk[85] = gr_complex(-0.3228, -0.0867);
            m_256apsk[86] = gr_complex(-0.7502, -0.1138);
            m_256apsk[87] = gr_complex(-0.7325, -0.2088);
            m_256apsk[88] = gr_complex(-0.1658, -1.6747);
            m_256apsk[89] = gr_complex(-0.4907, -1.6084);
            m_256apsk[90] = gr_complex(-0.1088, -0.9530);
            m_256apsk[91] = gr_complex(-0.2464, -0.9270);
            m_256apsk[92] = gr_complex(-0.0872, -0.1390);
            m_256apsk[93] = gr_complex(-0.0871, -0.1392);
            m_256apsk[94] = gr_complex(-0.1091, -0.7656);
            m_256apsk[95] = gr_complex(-0.1699, -0.7537);
            m_256apsk[96] = gr_complex(1.3225, -0.1320);
            m_256apsk[97] = gr_complex(1.2742, -0.3922);
            m_256apsk[98] = gr_complex(1.0854, -0.1139);
            m_256apsk[99] = gr_complex(1.0441, -0.3296);
            m_256apsk[100] = gr_complex(0.4582, -0.1123);
            m_256apsk[101] = gr_complex(0.4545, -0.1251);
            m_256apsk[102] = gr_complex(0.6473, -0.1138);
            m_256apsk[103] = gr_complex(0.6339, -0.1702);
            m_256apsk[104] = gr_complex(0.1322, -1.3631);
            m_256apsk[105] = gr_complex(0.3929, -1.3102);
            m_256apsk[106] = gr_complex(0.1124, -1.1327);
            m_256apsk[107] = gr_complex(0.3160, -1.0913);
            m_256apsk[108] = gr_complex(0.0928, -0.3970);
            m_256apsk[109] = gr_complex(0.0937, -0.3973);
            m_256apsk[110] = gr_complex(0.1054, -0.5979);
            m_256apsk[111] = gr_complex(0.1230, -0.5949);
            m_256apsk[112] = gr_complex(-1.3225, -0.1320);
            m_256apsk[113] = gr_complex(-1.2742, -0.3922);
            m_256apsk[114] = gr_complex(-1.0854, -0.1139);
            m_256apsk[115] = gr_complex(-1.0441, -0.3296);
            m_256apsk[116] = gr_complex(-0.4582, -0.1123);
            m_256apsk[117] = gr_complex(-0.4545, -0.1251);
            m_256apsk[118] = gr_complex(-0.6473, -0.1138);
            m_256apsk[119] = gr_complex(-0.6339, -0.1702);
            m_256apsk[120] = gr_complex(-0.1322, -1.3631);
            m_256apsk[121] = gr_complex(-0.3929, -1.3102);
            m_256apsk[122] = gr_complex(-0.1124, -1.1327);
            m_256apsk[123] = gr_complex(-0.3160, -1.0913);
            m_256apsk[124] = gr_complex(-0.0928, -0.3970);
            m_256apsk[125] = gr_complex(-0.0937, -0.3973);
            m_256apsk[126] = gr_complex(-0.1054, -0.5979);
            m_256apsk[127] = gr_complex(-0.1230, -0.5949);
            m_256apsk[128] = gr_complex(1.2901, 1.0495);
            m_256apsk[129] = gr_complex(1.4625, 0.7740);
            m_256apsk[130] = gr_complex(0.7273, 0.6160);
            m_256apsk[131] = gr_complex(0.8177, 0.4841);
            m_256apsk[132] = gr_complex(0.2844, 0.1296);
            m_256apsk[133] = gr_complex(0.2853, 0.1309);
            m_256apsk[134] = gr_complex(0.5902, 0.4857);
            m_256apsk[135] = gr_complex(0.6355, 0.4185);
            m_256apsk[136] = gr_complex(1.0646, 1.2876);
            m_256apsk[137] = gr_complex(0.7949, 1.4772);
            m_256apsk[138] = gr_complex(0.5707, 0.7662);
            m_256apsk[139] = gr_complex(0.4490, 0.8461);
            m_256apsk[140] = gr_complex(0.1053, 0.1494);
            m_256apsk[141] = gr_complex(0.1052, 0.1495);
            m_256apsk[142] = gr_complex(0.4294, 0.6363);
            m_256apsk[143] = gr_complex(0.3744, 0.6744);
            m_256apsk[144] = gr_complex(-1.2901, 1.0495);
            m_256apsk[145] = gr_complex(-1.4625, 0.7740);
            m_256apsk[146] = gr_complex(-0.7273, 0.6160);
            m_256apsk[147] = gr_complex(-0.8177, 0.4841);
            m_256apsk[148] = gr_complex(-0.2844, 0.1296);
            m_256apsk[149] = gr_complex(-0.2853, 0.1309);
            m_256apsk[150] = gr_complex(-0.5902, 0.4857);
            m_256apsk[151] = gr_complex(-0.6355, 0.4185);
            m_256apsk[152] = gr_complex(-1.0646, 1.2876);
            m_256apsk[153] = gr_complex(-0.7949, 1.4772);
            m_256apsk[154] = gr_complex(-0.5707, 0.7662);
            m_256apsk[155] = gr_complex(-0.4490, 0.8461);
            m_256apsk[156] = gr_complex(-0.1053, 0.1494);
            m_256apsk[157] = gr_complex(-0.1052, 0.1495);
            m_256apsk[158] = gr_complex(-0.4294, 0.6363);
            m_256apsk[159] = gr_complex(-0.3744, 0.6744);
            m_256apsk[160] = gr_complex(1.0382, 0.8623);
            m_256apsk[161] = gr_complex(1.1794, 0.6376);
            m_256apsk[162] = gr_complex(0.8504, 0.7217);
            m_256apsk[163] = gr_complex(0.9638, 0.5407);
            m_256apsk[164] = gr_complex(0.3734, 0.2560);
            m_256apsk[165] = gr_complex(0.3799, 0.2517);
            m_256apsk[166] = gr_complex(0.4968, 0.3947);
            m_256apsk[167] = gr_complex(0.5231, 0.3644);
            m_256apsk[168] = gr_complex(0.8555, 1.0542);
            m_256apsk[169] = gr_complex(0.6363, 1.2064);
            m_256apsk[170] = gr_complex(0.6961, 0.8850);
            m_256apsk[171] = gr_complex(0.5229, 1.0037);
            m_256apsk[172] = gr_complex(0.1938, 0.3621);
            m_256apsk[173] = gr_complex(0.1909, 0.3627);
            m_256apsk[174] = gr_complex(0.3224, 0.5236);
            m_256apsk[175] = gr_complex(0.3016, 0.5347);
            m_256apsk[176] = gr_complex(-1.0382, 0.8623);
            m_256apsk[177] = gr_complex(-1.1794, 0.6376);
            m_256apsk[178] = gr_complex(-0.8504, 0.7217);
            m_256apsk[179] = gr_complex(-0.9638, 0.5407);
            m_256apsk[180] = gr_complex(-0.3734, 0.2560);
            m_256apsk[181] = gr_complex(-0.3799, 0.2517);
            m_256apsk[182] = gr_complex(-0.4968, 0.3947);
            m_256apsk[183] = gr_complex(-0.5231, 0.3644);
            m_256apsk[184] = gr_complex(-0.8555, 1.0542);
            m_256apsk[185] = gr_complex(-0.6363, 1.2064);
            m_256apsk[186] = gr_complex(-0.6961, 0.8850);
            m_256apsk[187] = gr_complex(-0.5229, 1.0037);
            m_256apsk[188] = gr_complex(-0.1938, 0.3621);
            m_256apsk[189] = gr_complex(-0.1909, 0.3627);
            m_256apsk[190] = gr_complex(-0.3224, 0.5236);
            m_256apsk[191] = gr_complex(-0.3016, 0.5347);
            m_256apsk[192] = gr_complex(1.2901, -1.0495);
            m_256apsk[193] = gr_complex(1.4625, -0.7740);
            m_256apsk[194] = gr_complex(0.7273, -0.6160);
            m_256apsk[195] = gr_complex(0.8177, -0.4841);
            m_256apsk[196] = gr_complex(0.2844, -0.1296);
            m_256apsk[197] = gr_complex(0.2853, -0.1309);
            m_256apsk[198] = gr_complex(0.5902, -0.4857);
            m_256apsk[199] = gr_complex(0.6355, -0.4185);
            m_256apsk[200] = gr_complex(1.0646, -1.2876);
            m_256apsk[201] = gr_complex(0.7949, -1.4772);
            m_256apsk[202] = gr_complex(0.5707, -0.7662);
            m_256apsk[203] = gr_complex(0.4490, -0.8461);
            m_256apsk[204] = gr_complex(0.1053, -0.1494);
            m_256apsk[205] = gr_complex(0.1052, -0.1495);
            m_256apsk[206] = gr_complex(0.4294, -0.6363);
            m_256apsk[207] = gr_complex(0.3744, -0.6744);
            m_256apsk[208] = gr_complex(-1.2901, -1.0495);
            m_256apsk[209] = gr_complex(-1.4625, -0.7740);
            m_256apsk[210] = gr_complex(-0.7273, -0.6160);
            m_256apsk[211] = gr_complex(-0.8177, -0.4841);
            m_256apsk[212] = gr_complex(-0.2844, -0.1296);
            m_256apsk[213] = gr_complex(-0.2853, -0.1309);
            m_256apsk[214] = gr_complex(-0.5902, -0.4857);
            m_256apsk[215] = gr_complex(-0.6355, -0.4185);
            m_256apsk[216] = gr_complex(-1.0646, -1.2876);
            m_256apsk[217] = gr_complex(-0.7949, -1.4772);
            m_256apsk[218] = gr_complex(-0.5707, -0.7662);
            m_256apsk[219] = gr_complex(-0.4490, -0.8461);
            m_256apsk[220] = gr_complex(-0.1053, -0.1494);
            m_256apsk[221] = gr_complex(-0.1052, -0.1495);
            m_256apsk[222] = gr_complex(-0.4294, -0.6363);
            m_256apsk[223] = gr_complex(-0.3744, -0.6744);
            m_256apsk[224] = gr_complex(1.0382, -0.8623);
            m_256apsk[225] = gr_complex(1.1794, -0.6376);
            m_256apsk[226] = gr_complex(0.8504, -0.7217);
            m_256apsk[227] = gr_complex(0.9638, -0.5407);
            m_256apsk[228] = gr_complex(0.3734, -0.2560);
            m_256apsk[229] = gr_complex(0.3799, -0.2517);
            m_256apsk[230] = gr_complex(0.4968, -0.3947);
            m_256apsk[231] = gr_complex(0.5231, -0.3644);
            m_256apsk[232] = gr_complex(0.8555, -1.0542);
            m_256apsk[233] = gr_complex(0.6363, -1.2064);
            m_256apsk[234] = gr_complex(0.6961, -0.8850);
            m_256apsk[235] = gr_complex(0.5229, -1.0037);
            m_256apsk[236] = gr_complex(0.1938, -0.3621);
            m_256apsk[237] = gr_complex(0.1909, -0.3627);
            m_256apsk[238] = gr_complex(0.3224, -0.5236);
            m_256apsk[239] = gr_complex(0.3016, -0.5347);
            m_256apsk[240] = gr_complex(-1.0382, -0.8623);
            m_256apsk[241] = gr_complex(-1.1794, -0.6376);
            m_256apsk[242] = gr_complex(-0.8504, -0.7217);
            m_256apsk[243] = gr_complex(-0.9638, -0.5407);
            m_256apsk[244] = gr_complex(-0.3734, -0.2560);
            m_256apsk[245] = gr_complex(-0.3799, -0.2517);
            m_256apsk[246] = gr_complex(-0.4968, -0.3947);
            m_256apsk[247] = gr_complex(-0.5231, -0.3644);
            m_256apsk[248] = gr_complex(-0.8555, -1.0542);
            m_256apsk[249] = gr_complex(-0.6363, -1.2064);
            m_256apsk[250] = gr_complex(-0.6961, -0.8850);
            m_256apsk[251] = gr_complex(-0.5229, -1.0037);
            m_256apsk[252] = gr_complex(-0.1938, -0.3621);
            m_256apsk[253] = gr_complex(-0.1909, -0.3627);
            m_256apsk[254] = gr_complex(-0.3224, -0.5236);
            m_256apsk[255] = gr_complex(-0.3016, -0.5347);
          }
          else if (rate == C22_30) {
            m_256apsk[0] = gr_complex(1.5977, 0.1526);
            m_256apsk[1] = gr_complex(1.3187, 0.1269);
            m_256apsk[2] = gr_complex(-1.5977, 0.1526);
            m_256apsk[3] = gr_complex(-1.3187, 0.1269);
            m_256apsk[4] = gr_complex(0.2574, 0.0733);
            m_256apsk[5] = gr_complex(0.4496, 0.0807);
            m_256apsk[6] = gr_complex(-0.2574, 0.0733);
            m_256apsk[7] = gr_complex(-0.4496, 0.0807);
            m_256apsk[8] = gr_complex(1.5977, -0.1526);
            m_256apsk[9] = gr_complex(1.3187, -0.1269);
            m_256apsk[10] = gr_complex(-1.5977, -0.1526);
            m_256apsk[11] = gr_complex(-1.3187, -0.1269);
            m_256apsk[12] = gr_complex(0.2574, -0.0733);
            m_256apsk[13] = gr_complex(0.4496, -0.0807);
            m_256apsk[14] = gr_complex(-0.2574, -0.0733);
            m_256apsk[15] = gr_complex(-0.4496, -0.0807);
            m_256apsk[16] = gr_complex(0.9269, 0.0943);
            m_256apsk[17] = gr_complex(1.1024, 0.1086);
            m_256apsk[18] = gr_complex(-0.9269, 0.0943);
            m_256apsk[19] = gr_complex(-1.1024, 0.1086);
            m_256apsk[20] = gr_complex(0.7663, 0.0867);
            m_256apsk[21] = gr_complex(0.6115, 0.0871);
            m_256apsk[22] = gr_complex(-0.7663, 0.0867);
            m_256apsk[23] = gr_complex(-0.6115, 0.0871);
            m_256apsk[24] = gr_complex(0.9269, -0.0943);
            m_256apsk[25] = gr_complex(1.1024, -0.1086);
            m_256apsk[26] = gr_complex(-0.9269, -0.0943);
            m_256apsk[27] = gr_complex(-1.1024, -0.1086);
            m_256apsk[28] = gr_complex(0.7663, -0.0867);
            m_256apsk[29] = gr_complex(0.6115, -0.0871);
            m_256apsk[30] = gr_complex(-0.7663, -0.0867);
            m_256apsk[31] = gr_complex(-0.6115, -0.0871);
            m_256apsk[32] = gr_complex(1.2701, 1.0139);
            m_256apsk[33] = gr_complex(1.0525, 0.8406);
            m_256apsk[34] = gr_complex(-1.2701, 1.0139);
            m_256apsk[35] = gr_complex(-1.0525, 0.8406);
            m_256apsk[36] = gr_complex(0.2487, 0.1978);
            m_256apsk[37] = gr_complex(0.3523, 0.2915);
            m_256apsk[38] = gr_complex(-0.2487, 0.1978);
            m_256apsk[39] = gr_complex(-0.3523, 0.2915);
            m_256apsk[40] = gr_complex(1.2701, -1.0139);
            m_256apsk[41] = gr_complex(1.0525, -0.8406);
            m_256apsk[42] = gr_complex(-1.2701, -1.0139);
            m_256apsk[43] = gr_complex(-1.0525, -0.8406);
            m_256apsk[44] = gr_complex(0.2487, -0.1978);
            m_256apsk[45] = gr_complex(0.3523, -0.2915);
            m_256apsk[46] = gr_complex(-0.2487, -0.1978);
            m_256apsk[47] = gr_complex(-0.3523, -0.2915);
            m_256apsk[48] = gr_complex(0.7359, 0.6043);
            m_256apsk[49] = gr_complex(0.8807, 0.7105);
            m_256apsk[50] = gr_complex(-0.7359, 0.6043);
            m_256apsk[51] = gr_complex(-0.8807, 0.7105);
            m_256apsk[52] = gr_complex(0.6017, 0.5019);
            m_256apsk[53] = gr_complex(0.4747, 0.3996);
            m_256apsk[54] = gr_complex(-0.6017, 0.5019);
            m_256apsk[55] = gr_complex(-0.4747, 0.3996);
            m_256apsk[56] = gr_complex(0.7359, -0.6043);
            m_256apsk[57] = gr_complex(0.8807, -0.7105);
            m_256apsk[58] = gr_complex(-0.7359, -0.6043);
            m_256apsk[59] = gr_complex(-0.8807, -0.7105);
            m_256apsk[60] = gr_complex(0.6017, -0.5019);
            m_256apsk[61] = gr_complex(0.4747, -0.3996);
            m_256apsk[62] = gr_complex(-0.6017, -0.5019);
            m_256apsk[63] = gr_complex(-0.4747, -0.3996);
            m_256apsk[64] = gr_complex(1.5441, 0.4545);
            m_256apsk[65] = gr_complex(1.2750, 0.3775);
            m_256apsk[66] = gr_complex(-1.5441, 0.4545);
            m_256apsk[67] = gr_complex(-1.2750, 0.3775);
            m_256apsk[68] = gr_complex(0.2586, 0.0752);
            m_256apsk[69] = gr_complex(0.4435, 0.1065);
            m_256apsk[70] = gr_complex(-0.2586, 0.0752);
            m_256apsk[71] = gr_complex(-0.4435, 0.1065);
            m_256apsk[72] = gr_complex(1.5441, -0.4545);
            m_256apsk[73] = gr_complex(1.2750, -0.3775);
            m_256apsk[74] = gr_complex(-1.5441, -0.4545);
            m_256apsk[75] = gr_complex(-1.2750, -0.3775);
            m_256apsk[76] = gr_complex(0.2586, -0.0752);
            m_256apsk[77] = gr_complex(0.4435, -0.1065);
            m_256apsk[78] = gr_complex(-0.2586, -0.0752);
            m_256apsk[79] = gr_complex(-0.4435, -0.1065);
            m_256apsk[80] = gr_complex(0.8925, 0.2771);
            m_256apsk[81] = gr_complex(1.0649, 0.3219);
            m_256apsk[82] = gr_complex(-0.8925, 0.2771);
            m_256apsk[83] = gr_complex(-1.0649, 0.3219);
            m_256apsk[84] = gr_complex(0.7362, 0.2279);
            m_256apsk[85] = gr_complex(0.5936, 0.1699);
            m_256apsk[86] = gr_complex(-0.7362, 0.2279);
            m_256apsk[87] = gr_complex(-0.5936, 0.1699);
            m_256apsk[88] = gr_complex(0.8925, -0.2771);
            m_256apsk[89] = gr_complex(1.0649, -0.3219);
            m_256apsk[90] = gr_complex(-0.8925, -0.2771);
            m_256apsk[91] = gr_complex(-1.0649, -0.3219);
            m_256apsk[92] = gr_complex(0.7362, -0.2279);
            m_256apsk[93] = gr_complex(0.5936, -0.1699);
            m_256apsk[94] = gr_complex(-0.7362, -0.2279);
            m_256apsk[95] = gr_complex(-0.5936, -0.1699);
            m_256apsk[96] = gr_complex(1.4352, 0.7452);
            m_256apsk[97] = gr_complex(1.1866, 0.6182);
            m_256apsk[98] = gr_complex(-1.4352, 0.7452);
            m_256apsk[99] = gr_complex(-1.1866, 0.6182);
            m_256apsk[100] = gr_complex(0.2523, 0.1944);
            m_256apsk[101] = gr_complex(0.3695, 0.2695);
            m_256apsk[102] = gr_complex(-0.2523, 0.1944);
            m_256apsk[103] = gr_complex(-0.3695, 0.2695);
            m_256apsk[104] = gr_complex(1.4352, -0.7452);
            m_256apsk[105] = gr_complex(1.1866, -0.6182);
            m_256apsk[106] = gr_complex(-1.4352, -0.7452);
            m_256apsk[107] = gr_complex(-1.1866, -0.6182);
            m_256apsk[108] = gr_complex(0.2523, -0.1944);
            m_256apsk[109] = gr_complex(0.3695, -0.2695);
            m_256apsk[110] = gr_complex(-0.2523, -0.1944);
            m_256apsk[111] = gr_complex(-0.3695, -0.2695);
            m_256apsk[112] = gr_complex(0.8273, 0.4493);
            m_256apsk[113] = gr_complex(0.9911, 0.5243);
            m_256apsk[114] = gr_complex(-0.8273, 0.4493);
            m_256apsk[115] = gr_complex(-0.9911, 0.5243);
            m_256apsk[116] = gr_complex(0.6708, 0.3859);
            m_256apsk[117] = gr_complex(0.5197, 0.3331);
            m_256apsk[118] = gr_complex(-0.6708, 0.3859);
            m_256apsk[119] = gr_complex(-0.5197, 0.3331);
            m_256apsk[120] = gr_complex(0.8273, -0.4493);
            m_256apsk[121] = gr_complex(0.9911, -0.5243);
            m_256apsk[122] = gr_complex(-0.8273, -0.4493);
            m_256apsk[123] = gr_complex(-0.9911, -0.5243);
            m_256apsk[124] = gr_complex(0.6708, -0.3859);
            m_256apsk[125] = gr_complex(0.5197, -0.3331);
            m_256apsk[126] = gr_complex(-0.6708, -0.3859);
            m_256apsk[127] = gr_complex(-0.5197, -0.3331);
            m_256apsk[128] = gr_complex(0.1646, 1.6329);
            m_256apsk[129] = gr_complex(0.1379, 1.3595);
            m_256apsk[130] = gr_complex(-0.1646, 1.6329);
            m_256apsk[131] = gr_complex(-0.1379, 1.3595);
            m_256apsk[132] = gr_complex(0.0736, 0.0898);
            m_256apsk[133] = gr_complex(0.0742, 0.5054);
            m_256apsk[134] = gr_complex(-0.0736, 0.0898);
            m_256apsk[135] = gr_complex(-0.0742, 0.5054);
            m_256apsk[136] = gr_complex(0.1646, -1.6329);
            m_256apsk[137] = gr_complex(0.1379, -1.3595);
            m_256apsk[138] = gr_complex(-0.1646, -1.6329);
            m_256apsk[139] = gr_complex(-0.1379, -1.3595);
            m_256apsk[140] = gr_complex(0.0736, -0.0898);
            m_256apsk[141] = gr_complex(0.0742, -0.5054);
            m_256apsk[142] = gr_complex(-0.0736, -0.0898);
            m_256apsk[143] = gr_complex(-0.0742, -0.5054);
            m_256apsk[144] = gr_complex(0.0992, 0.9847);
            m_256apsk[145] = gr_complex(0.1170, 1.1517);
            m_256apsk[146] = gr_complex(-0.0992, 0.9847);
            m_256apsk[147] = gr_complex(-0.1170, 1.1517);
            m_256apsk[148] = gr_complex(0.0894, 0.8287);
            m_256apsk[149] = gr_complex(0.0889, 0.6739);
            m_256apsk[150] = gr_complex(-0.0894, 0.8287);
            m_256apsk[151] = gr_complex(-0.0889, 0.6739);
            m_256apsk[152] = gr_complex(0.0992, -0.9847);
            m_256apsk[153] = gr_complex(0.1170, -1.1517);
            m_256apsk[154] = gr_complex(-0.0992, -0.9847);
            m_256apsk[155] = gr_complex(-0.1170, -1.1517);
            m_256apsk[156] = gr_complex(0.0894, -0.8287);
            m_256apsk[157] = gr_complex(0.0889, -0.6739);
            m_256apsk[158] = gr_complex(-0.0894, -0.8287);
            m_256apsk[159] = gr_complex(-0.0889, -0.6739);
            m_256apsk[160] = gr_complex(1.0516, 1.2481);
            m_256apsk[161] = gr_complex(0.8742, 1.0355);
            m_256apsk[162] = gr_complex(-1.0516, 1.2481);
            m_256apsk[163] = gr_complex(-0.8742, 1.0355);
            m_256apsk[164] = gr_complex(0.0970, 0.2450);
            m_256apsk[165] = gr_complex(0.1959, 0.4045);
            m_256apsk[166] = gr_complex(-0.0970, 0.2450);
            m_256apsk[167] = gr_complex(-0.1959, 0.4045);
            m_256apsk[168] = gr_complex(1.0516, -1.2481);
            m_256apsk[169] = gr_complex(0.8742, -1.0355);
            m_256apsk[170] = gr_complex(-1.0516, -1.2481);
            m_256apsk[171] = gr_complex(-0.8742, -1.0355);
            m_256apsk[172] = gr_complex(0.0970, -0.2450);
            m_256apsk[173] = gr_complex(0.1959, -0.4045);
            m_256apsk[174] = gr_complex(-0.0970, -0.2450);
            m_256apsk[175] = gr_complex(-0.1959, -0.4045);
            m_256apsk[176] = gr_complex(0.6150, 0.7441);
            m_256apsk[177] = gr_complex(0.7345, 0.8743);
            m_256apsk[178] = gr_complex(-0.6150, 0.7441);
            m_256apsk[179] = gr_complex(-0.7345, 0.8743);
            m_256apsk[180] = gr_complex(0.4932, 0.6301);
            m_256apsk[181] = gr_complex(0.3620, 0.5258);
            m_256apsk[182] = gr_complex(-0.4932, 0.6301);
            m_256apsk[183] = gr_complex(-0.3620, 0.5258);
            m_256apsk[184] = gr_complex(0.6150, -0.7441);
            m_256apsk[185] = gr_complex(0.7345, -0.8743);
            m_256apsk[186] = gr_complex(-0.6150, -0.7441);
            m_256apsk[187] = gr_complex(-0.7345, -0.8743);
            m_256apsk[188] = gr_complex(0.4932, -0.6301);
            m_256apsk[189] = gr_complex(0.3620, -0.5258);
            m_256apsk[190] = gr_complex(-0.4932, -0.6301);
            m_256apsk[191] = gr_complex(-0.3620, -0.5258);
            m_256apsk[192] = gr_complex(0.4866, 1.5660);
            m_256apsk[193] = gr_complex(0.4068, 1.3027);
            m_256apsk[194] = gr_complex(-0.4866, 1.5660);
            m_256apsk[195] = gr_complex(-0.4068, 1.3027);
            m_256apsk[196] = gr_complex(0.0732, 0.0899);
            m_256apsk[197] = gr_complex(0.0877, 0.4997);
            m_256apsk[198] = gr_complex(-0.0732, 0.0899);
            m_256apsk[199] = gr_complex(-0.0877, 0.4997);
            m_256apsk[200] = gr_complex(0.4866, -1.5660);
            m_256apsk[201] = gr_complex(0.4068, -1.3027);
            m_256apsk[202] = gr_complex(-0.4866, -1.5660);
            m_256apsk[203] = gr_complex(-0.4068, -1.3027);
            m_256apsk[204] = gr_complex(0.0732, -0.0899);
            m_256apsk[205] = gr_complex(0.0877, -0.4997);
            m_256apsk[206] = gr_complex(-0.0732, -0.0899);
            m_256apsk[207] = gr_complex(-0.0877, -0.4997);
            m_256apsk[208] = gr_complex(0.2927, 0.9409);
            m_256apsk[209] = gr_complex(0.3446, 1.1023);
            m_256apsk[210] = gr_complex(-0.2927, 0.9409);
            m_256apsk[211] = gr_complex(-0.3446, 1.1023);
            m_256apsk[212] = gr_complex(0.2350, 0.7945);
            m_256apsk[213] = gr_complex(0.1670, 0.6529);
            m_256apsk[214] = gr_complex(-0.2350, 0.7945);
            m_256apsk[215] = gr_complex(-0.1670, 0.6529);
            m_256apsk[216] = gr_complex(0.2927, -0.9409);
            m_256apsk[217] = gr_complex(0.3446, -1.1023);
            m_256apsk[218] = gr_complex(-0.2927, -0.9409);
            m_256apsk[219] = gr_complex(-0.3446, -1.1023);
            m_256apsk[220] = gr_complex(0.2350, -0.7945);
            m_256apsk[221] = gr_complex(0.1670, -0.6529);
            m_256apsk[222] = gr_complex(-0.2350, -0.7945);
            m_256apsk[223] = gr_complex(-0.1670, -0.6529);
            m_256apsk[224] = gr_complex(0.7867, 1.4356);
            m_256apsk[225] = gr_complex(0.6561, 1.1927);
            m_256apsk[226] = gr_complex(-0.7867, 1.4356);
            m_256apsk[227] = gr_complex(-0.6561, 1.1927);
            m_256apsk[228] = gr_complex(0.0947, 0.2451);
            m_256apsk[229] = gr_complex(0.1865, 0.4121);
            m_256apsk[230] = gr_complex(-0.0947, 0.2451);
            m_256apsk[231] = gr_complex(-0.1865, 0.4121);
            m_256apsk[232] = gr_complex(0.7867, -1.4356);
            m_256apsk[233] = gr_complex(0.6561, -1.1927);
            m_256apsk[234] = gr_complex(-0.7867, -1.4356);
            m_256apsk[235] = gr_complex(-0.6561, -1.1927);
            m_256apsk[236] = gr_complex(0.0947, -0.2451);
            m_256apsk[237] = gr_complex(0.1865, -0.4121);
            m_256apsk[238] = gr_complex(-0.0947, -0.2451);
            m_256apsk[239] = gr_complex(-0.1865, -0.4121);
            m_256apsk[240] = gr_complex(0.4677, 0.8579);
            m_256apsk[241] = gr_complex(0.5537, 1.0081);
            m_256apsk[242] = gr_complex(-0.4677, 0.8579);
            m_256apsk[243] = gr_complex(-0.5537, 1.0081);
            m_256apsk[244] = gr_complex(0.3893, 0.7143);
            m_256apsk[245] = gr_complex(0.3110, 0.5686);
            m_256apsk[246] = gr_complex(-0.3893, 0.7143);
            m_256apsk[247] = gr_complex(-0.3110, 0.5686);
            m_256apsk[248] = gr_complex(0.4677, -0.8579);
            m_256apsk[249] = gr_complex(0.5537, -1.0081);
            m_256apsk[250] = gr_complex(-0.4677, -0.8579);
            m_256apsk[251] = gr_complex(-0.5537, -1.0081);
            m_256apsk[252] = gr_complex(0.3893, -0.7143);
            m_256apsk[253] = gr_complex(0.3110, -0.5686);
            m_256apsk[254] = gr_complex(-0.3893, -0.7143);
            m_256apsk[255] = gr_complex(-0.3110, -0.5686);
          }
          else {
            r8 = m;
            switch(rate) {
              case C116_180:
              case C124_180:
                r1 = r8 / 6.536;
                r7 = r1 * 5.078;
                r6 = r1 * 4.235;
                r5 = r1 * 3.569;
                r4 = r1 * 2.98;
                r3 = r1 * 2.405;
                r2 = r1 * 1.791;
                break;
              case C128_180:
                r1 = r8 / 5.4;
                r7 = r1 * 4.6;
                r6 = r1 * 4.045;
                r5 = r1 * 3.579;
                r4 = r1 * 2.986;
                r3 = r1 * 2.409;
                r2 = r1 * 1.794;
                break;
              case C135_180:
                r1 = r8 / 5.2;
                r7 = r1 * 4.5;
                r6 = r1 * 4.045;
                r5 = r1 * 3.579;
                r4 = r1 * 2.986;
                r3 = r1 * 2.409;
                r2 = r1 * 1.794;
                break;
              default:
                r1 = 0;
                r2 = 0;
                r3 = 0;
                r4 = 0;
                r5 = 0;
                r6 = 0;
                r7 = 0;
                break;
            }
            m_256apsk[0] = gr_complex((r1 * cos(M_PI / 32.0)), (r1 * sin(M_PI / 32.0)));
            m_256apsk[1] = gr_complex((r1 * cos(3 * M_PI / 32.0)), (r1 * sin(3 * M_PI / 32.0)));
            m_256apsk[2] = gr_complex((r1 * cos(7 * M_PI / 32.0)), (r1 * sin(7 * M_PI / 32.0)));
            m_256apsk[3] = gr_complex((r1 * cos(5 * M_PI / 32.0)), (r1 * sin(5 * M_PI / 32.0)));
            m_256apsk[4] = gr_complex((r1 * cos(15 * M_PI / 32.0)), (r1 * sin(15 * M_PI / 32.0)));
            m_256apsk[5] = gr_complex((r1 * cos(13 * M_PI / 32.0)), (r1 * sin(13 * M_PI / 32.0)));
            m_256apsk[6] = gr_complex((r1 * cos(9 * M_PI / 32.0)), (r1 * sin(9 * M_PI / 32.0)));
            m_256apsk[7] = gr_complex((r1 * cos(11 * M_PI / 32.0)), (r1 * sin(11 * M_PI / 32.0)));
            m_256apsk[8] = gr_complex((r1 * cos(31 * M_PI / 32.0)), (r1 * sin(31 * M_PI / 32.0)));
            m_256apsk[9] = gr_complex((r1 * cos(29 * M_PI / 32.0)), (r1 * sin(29 * M_PI / 32.0)));
            m_256apsk[10] = gr_complex((r1 * cos(25 * M_PI / 32.0)), (r1 * sin(25 * M_PI / 32.0)));
            m_256apsk[11] = gr_complex((r1 * cos(27 * M_PI / 32.0)), (r1 * sin(27 * M_PI / 32.0)));
            m_256apsk[12] = gr_complex((r1 * cos(17 * M_PI / 32.0)), (r1 * sin(17 * M_PI / 32.0)));
            m_256apsk[13] = gr_complex((r1 * cos(19 * M_PI / 32.0)), (r1 * sin(19 * M_PI / 32.0)));
            m_256apsk[14] = gr_complex((r1 * cos(23 * M_PI / 32.0)), (r1 * sin(23 * M_PI / 32.0)));
            m_256apsk[15] = gr_complex((r1 * cos(21 * M_PI / 32.0)), (r1 * sin(21 * M_PI / 32.0)));
            m_256apsk[16] = gr_complex((r1 * cos(-1 * M_PI / 32.0)), (r1 * sin(-1 * M_PI / 32.0)));
            m_256apsk[17] = gr_complex((r1 * cos(-3 * M_PI / 32.0)), (r1 * sin(-3 * M_PI / 32.0)));
            m_256apsk[18] = gr_complex((r1 * cos(-7 * M_PI / 32.0)), (r1 * sin(-7 * M_PI / 32.0)));
            m_256apsk[19] = gr_complex((r1 * cos(-5 * M_PI / 32.0)), (r1 * sin(-5 * M_PI / 32.0)));
            m_256apsk[20] = gr_complex((r1 * cos(-15 * M_PI / 32.0)), (r1 * sin(-15 * M_PI / 32.0)));
            m_256apsk[21] = gr_complex((r1 * cos(-13 * M_PI / 32.0)), (r1 * sin(-13 * M_PI / 32.0)));
            m_256apsk[22] = gr_complex((r1 * cos(-9 * M_PI / 32.0)), (r1 * sin(-9 * M_PI / 32.0)));
            m_256apsk[23] = gr_complex((r1 * cos(-11 * M_PI / 32.0)), (r1 * sin(-11 * M_PI / 32.0)));
            m_256apsk[24] = gr_complex((r1 * cos(33 * M_PI / 32.0)), (r1 * sin(33 * M_PI / 32.0)));
            m_256apsk[25] = gr_complex((r1 * cos(35 * M_PI / 32.0)), (r1 * sin(35 * M_PI / 32.0)));
            m_256apsk[26] = gr_complex((r1 * cos(39 * M_PI / 32.0)), (r1 * sin(39 * M_PI / 32.0)));
            m_256apsk[27] = gr_complex((r1 * cos(37 * M_PI / 32.0)), (r1 * sin(37 * M_PI / 32.0)));
            m_256apsk[28] = gr_complex((r1 * cos(47 * M_PI / 32.0)), (r1 * sin(47 * M_PI / 32.0)));
            m_256apsk[29] = gr_complex((r1 * cos(45 * M_PI / 32.0)), (r1 * sin(45 * M_PI / 32.0)));
            m_256apsk[30] = gr_complex((r1 * cos(41 * M_PI / 32.0)), (r1 * sin(41 * M_PI / 32.0)));
            m_256apsk[31] = gr_complex((r1 * cos(43 * M_PI / 32.0)), (r1 * sin(43 * M_PI / 32.0)));
            m_256apsk[32] = gr_complex((r2 * cos(M_PI / 32.0)), (r2 * sin(M_PI / 32.0)));
            m_256apsk[33] = gr_complex((r2 * cos(3 * M_PI / 32.0)), (r2 * sin(3 * M_PI / 32.0)));
            m_256apsk[34] = gr_complex((r2 * cos(7 * M_PI / 32.0)), (r2 * sin(7 * M_PI / 32.0)));
            m_256apsk[35] = gr_complex((r2 * cos(5 * M_PI / 32.0)), (r2 * sin(5 * M_PI / 32.0)));
            m_256apsk[36] = gr_complex((r2 * cos(15 * M_PI / 32.0)), (r2 * sin(15 * M_PI / 32.0)));
            m_256apsk[37] = gr_complex((r2 * cos(13 * M_PI / 32.0)), (r2 * sin(13 * M_PI / 32.0)));
            m_256apsk[38] = gr_complex((r2 * cos(9 * M_PI / 32.0)), (r2 * sin(9 * M_PI / 32.0)));
            m_256apsk[39] = gr_complex((r2 * cos(11 * M_PI / 32.0)), (r2 * sin(11 * M_PI / 32.0)));
            m_256apsk[40] = gr_complex((r2 * cos(31 * M_PI / 32.0)), (r2 * sin(31 * M_PI / 32.0)));
            m_256apsk[41] = gr_complex((r2 * cos(29 * M_PI / 32.0)), (r2 * sin(29 * M_PI / 32.0)));
            m_256apsk[42] = gr_complex((r2 * cos(25 * M_PI / 32.0)), (r2 * sin(25 * M_PI / 32.0)));
            m_256apsk[43] = gr_complex((r2 * cos(27 * M_PI / 32.0)), (r2 * sin(27 * M_PI / 32.0)));
            m_256apsk[44] = gr_complex((r2 * cos(17 * M_PI / 32.0)), (r2 * sin(17 * M_PI / 32.0)));
            m_256apsk[45] = gr_complex((r2 * cos(19 * M_PI / 32.0)), (r2 * sin(19 * M_PI / 32.0)));
            m_256apsk[46] = gr_complex((r2 * cos(23 * M_PI / 32.0)), (r2 * sin(23 * M_PI / 32.0)));
            m_256apsk[47] = gr_complex((r2 * cos(21 * M_PI / 32.0)), (r2 * sin(21 * M_PI / 32.0)));
            m_256apsk[48] = gr_complex((r2 * cos(-1 * M_PI / 32.0)), (r2 * sin(-1 * M_PI / 32.0)));
            m_256apsk[49] = gr_complex((r2 * cos(-3 * M_PI / 32.0)), (r2 * sin(-3 * M_PI / 32.0)));
            m_256apsk[50] = gr_complex((r2 * cos(-7 * M_PI / 32.0)), (r2 * sin(-7 * M_PI / 32.0)));
            m_256apsk[51] = gr_complex((r2 * cos(-5 * M_PI / 32.0)), (r2 * sin(-5 * M_PI / 32.0)));
            m_256apsk[52] = gr_complex((r2 * cos(-15 * M_PI / 32.0)), (r2 * sin(-15 * M_PI / 32.0)));
            m_256apsk[53] = gr_complex((r2 * cos(-13 * M_PI / 32.0)), (r2 * sin(-13 * M_PI / 32.0)));
            m_256apsk[54] = gr_complex((r2 * cos(-9 * M_PI / 32.0)), (r2 * sin(-9 * M_PI / 32.0)));
            m_256apsk[55] = gr_complex((r2 * cos(-11 * M_PI / 32.0)), (r2 * sin(-11 * M_PI / 32.0)));
            m_256apsk[56] = gr_complex((r2 * cos(33 * M_PI / 32.0)), (r2 * sin(33 * M_PI / 32.0)));
            m_256apsk[57] = gr_complex((r2 * cos(35 * M_PI / 32.0)), (r2 * sin(35 * M_PI / 32.0)));
            m_256apsk[58] = gr_complex((r2 * cos(39 * M_PI / 32.0)), (r2 * sin(39 * M_PI / 32.0)));
            m_256apsk[59] = gr_complex((r2 * cos(37 * M_PI / 32.0)), (r2 * sin(37 * M_PI / 32.0)));
            m_256apsk[60] = gr_complex((r2 * cos(47 * M_PI / 32.0)), (r2 * sin(47 * M_PI / 32.0)));
            m_256apsk[61] = gr_complex((r2 * cos(45 * M_PI / 32.0)), (r2 * sin(45 * M_PI / 32.0)));
            m_256apsk[62] = gr_complex((r2 * cos(41 * M_PI / 32.0)), (r2 * sin(41 * M_PI / 32.0)));
            m_256apsk[63] = gr_complex((r2 * cos(43 * M_PI / 32.0)), (r2 * sin(43 * M_PI / 32.0)));
            m_256apsk[64] = gr_complex((r4 * cos(M_PI / 32.0)), (r4 * sin(M_PI / 32.0)));
            m_256apsk[65] = gr_complex((r4 * cos(3 * M_PI / 32.0)), (r4 * sin(3 * M_PI / 32.0)));
            m_256apsk[66] = gr_complex((r4 * cos(7 * M_PI / 32.0)), (r4 * sin(7 * M_PI / 32.0)));
            m_256apsk[67] = gr_complex((r4 * cos(5 * M_PI / 32.0)), (r4 * sin(5 * M_PI / 32.0)));
            m_256apsk[68] = gr_complex((r4 * cos(15 * M_PI / 32.0)), (r4 * sin(15 * M_PI / 32.0)));
            m_256apsk[69] = gr_complex((r4 * cos(13 * M_PI / 32.0)), (r4 * sin(13 * M_PI / 32.0)));
            m_256apsk[70] = gr_complex((r4 * cos(9 * M_PI / 32.0)), (r4 * sin(9 * M_PI / 32.0)));
            m_256apsk[71] = gr_complex((r4 * cos(11 * M_PI / 32.0)), (r4 * sin(11 * M_PI / 32.0)));
            m_256apsk[72] = gr_complex((r4 * cos(31 * M_PI / 32.0)), (r4 * sin(31 * M_PI / 32.0)));
            m_256apsk[73] = gr_complex((r4 * cos(29 * M_PI / 32.0)), (r4 * sin(29 * M_PI / 32.0)));
            m_256apsk[74] = gr_complex((r4 * cos(25 * M_PI / 32.0)), (r4 * sin(25 * M_PI / 32.0)));
            m_256apsk[75] = gr_complex((r4 * cos(27 * M_PI / 32.0)), (r4 * sin(27 * M_PI / 32.0)));
            m_256apsk[76] = gr_complex((r4 * cos(17 * M_PI / 32.0)), (r4 * sin(17 * M_PI / 32.0)));
            m_256apsk[77] = gr_complex((r4 * cos(19 * M_PI / 32.0)), (r4 * sin(19 * M_PI / 32.0)));
            m_256apsk[78] = gr_complex((r4 * cos(23 * M_PI / 32.0)), (r4 * sin(23 * M_PI / 32.0)));
            m_256apsk[79] = gr_complex((r4 * cos(21 * M_PI / 32.0)), (r4 * sin(21 * M_PI / 32.0)));
            m_256apsk[80] = gr_complex((r4 * cos(-1 * M_PI / 32.0)), (r4 * sin(-1 * M_PI / 32.0)));
            m_256apsk[81] = gr_complex((r4 * cos(-3 * M_PI / 32.0)), (r4 * sin(-3 * M_PI / 32.0)));
            m_256apsk[82] = gr_complex((r4 * cos(-7 * M_PI / 32.0)), (r4 * sin(-7 * M_PI / 32.0)));
            m_256apsk[83] = gr_complex((r4 * cos(-5 * M_PI / 32.0)), (r4 * sin(-5 * M_PI / 32.0)));
            m_256apsk[84] = gr_complex((r4 * cos(-15 * M_PI / 32.0)), (r4 * sin(-15 * M_PI / 32.0)));
            m_256apsk[85] = gr_complex((r4 * cos(-13 * M_PI / 32.0)), (r4 * sin(-13 * M_PI / 32.0)));
            m_256apsk[86] = gr_complex((r4 * cos(-9 * M_PI / 32.0)), (r4 * sin(-9 * M_PI / 32.0)));
            m_256apsk[87] = gr_complex((r4 * cos(-11 * M_PI / 32.0)), (r4 * sin(-11 * M_PI / 32.0)));
            m_256apsk[88] = gr_complex((r4 * cos(33 * M_PI / 32.0)), (r4 * sin(33 * M_PI / 32.0)));
            m_256apsk[89] = gr_complex((r4 * cos(35 * M_PI / 32.0)), (r4 * sin(35 * M_PI / 32.0)));
            m_256apsk[90] = gr_complex((r4 * cos(39 * M_PI / 32.0)), (r4 * sin(39 * M_PI / 32.0)));
            m_256apsk[91] = gr_complex((r4 * cos(37 * M_PI / 32.0)), (r4 * sin(37 * M_PI / 32.0)));
            m_256apsk[92] = gr_complex((r4 * cos(47 * M_PI / 32.0)), (r4 * sin(47 * M_PI / 32.0)));
            m_256apsk[93] = gr_complex((r4 * cos(45 * M_PI / 32.0)), (r4 * sin(45 * M_PI / 32.0)));
            m_256apsk[94] = gr_complex((r4 * cos(41 * M_PI / 32.0)), (r4 * sin(41 * M_PI / 32.0)));
            m_256apsk[95] = gr_complex((r4 * cos(43 * M_PI / 32.0)), (r4 * sin(43 * M_PI / 32.0)));
            m_256apsk[96] = gr_complex((r3 * cos(M_PI / 32.0)), (r3 * sin(M_PI / 32.0)));
            m_256apsk[97] = gr_complex((r3 * cos(3 * M_PI / 32.0)), (r3 * sin(3 * M_PI / 32.0)));
            m_256apsk[98] = gr_complex((r3 * cos(7 * M_PI / 32.0)), (r3 * sin(7 * M_PI / 32.0)));
            m_256apsk[99] = gr_complex((r3 * cos(5 * M_PI / 32.0)), (r3 * sin(5 * M_PI / 32.0)));
            m_256apsk[100] = gr_complex((r3 * cos(15 * M_PI / 32.0)), (r3 * sin(15 * M_PI / 32.0)));
            m_256apsk[101] = gr_complex((r3 * cos(13 * M_PI / 32.0)), (r3 * sin(13 * M_PI / 32.0)));
            m_256apsk[102] = gr_complex((r3 * cos(9 * M_PI / 32.0)), (r3 * sin(9 * M_PI / 32.0)));
            m_256apsk[103] = gr_complex((r3 * cos(11 * M_PI / 32.0)), (r3 * sin(11 * M_PI / 32.0)));
            m_256apsk[104] = gr_complex((r3 * cos(31 * M_PI / 32.0)), (r3 * sin(31 * M_PI / 32.0)));
            m_256apsk[105] = gr_complex((r3 * cos(29 * M_PI / 32.0)), (r3 * sin(29 * M_PI / 32.0)));
            m_256apsk[106] = gr_complex((r3 * cos(25 * M_PI / 32.0)), (r3 * sin(25 * M_PI / 32.0)));
            m_256apsk[107] = gr_complex((r3 * cos(27 * M_PI / 32.0)), (r3 * sin(27 * M_PI / 32.0)));
            m_256apsk[108] = gr_complex((r3 * cos(17 * M_PI / 32.0)), (r3 * sin(17 * M_PI / 32.0)));
            m_256apsk[109] = gr_complex((r3 * cos(19 * M_PI / 32.0)), (r3 * sin(19 * M_PI / 32.0)));
            m_256apsk[110] = gr_complex((r3 * cos(23 * M_PI / 32.0)), (r3 * sin(23 * M_PI / 32.0)));
            m_256apsk[111] = gr_complex((r3 * cos(21 * M_PI / 32.0)), (r3 * sin(21 * M_PI / 32.0)));
            m_256apsk[112] = gr_complex((r3 * cos(-1 * M_PI / 32.0)), (r3 * sin(-1 * M_PI / 32.0)));
            m_256apsk[113] = gr_complex((r3 * cos(-3 * M_PI / 32.0)), (r3 * sin(-3 * M_PI / 32.0)));
            m_256apsk[114] = gr_complex((r3 * cos(-7 * M_PI / 32.0)), (r3 * sin(-7 * M_PI / 32.0)));
            m_256apsk[115] = gr_complex((r3 * cos(-5 * M_PI / 32.0)), (r3 * sin(-5 * M_PI / 32.0)));
            m_256apsk[116] = gr_complex((r3 * cos(-15 * M_PI / 32.0)), (r3 * sin(-15 * M_PI / 32.0)));
            m_256apsk[117] = gr_complex((r3 * cos(-13 * M_PI / 32.0)), (r3 * sin(-13 * M_PI / 32.0)));
            m_256apsk[118] = gr_complex((r3 * cos(-9 * M_PI / 32.0)), (r3 * sin(-9 * M_PI / 32.0)));
            m_256apsk[119] = gr_complex((r3 * cos(-11 * M_PI / 32.0)), (r3 * sin(-11 * M_PI / 32.0)));
            m_256apsk[120] = gr_complex((r3 * cos(33 * M_PI / 32.0)), (r3 * sin(33 * M_PI / 32.0)));
            m_256apsk[121] = gr_complex((r3 * cos(35 * M_PI / 32.0)), (r3 * sin(35 * M_PI / 32.0)));
            m_256apsk[122] = gr_complex((r3 * cos(39 * M_PI / 32.0)), (r3 * sin(39 * M_PI / 32.0)));
            m_256apsk[123] = gr_complex((r3 * cos(37 * M_PI / 32.0)), (r3 * sin(37 * M_PI / 32.0)));
            m_256apsk[124] = gr_complex((r3 * cos(47 * M_PI / 32.0)), (r3 * sin(47 * M_PI / 32.0)));
            m_256apsk[125] = gr_complex((r3 * cos(45 * M_PI / 32.0)), (r3 * sin(45 * M_PI / 32.0)));
            m_256apsk[126] = gr_complex((r3 * cos(41 * M_PI / 32.0)), (r3 * sin(41 * M_PI / 32.0)));
            m_256apsk[127] = gr_complex((r3 * cos(43 * M_PI / 32.0)), (r3 * sin(43 * M_PI / 32.0)));
            m_256apsk[128] = gr_complex((r8 * cos(M_PI / 32.0)), (r8 * sin(M_PI / 32.0)));
            m_256apsk[129] = gr_complex((r8 * cos(3 * M_PI / 32.0)), (r8 * sin(3 * M_PI / 32.0)));
            m_256apsk[130] = gr_complex((r8 * cos(7 * M_PI / 32.0)), (r8 * sin(7 * M_PI / 32.0)));
            m_256apsk[131] = gr_complex((r8 * cos(5 * M_PI / 32.0)), (r8 * sin(5 * M_PI / 32.0)));
            m_256apsk[132] = gr_complex((r8 * cos(15 * M_PI / 32.0)), (r8 * sin(15 * M_PI / 32.0)));
            m_256apsk[133] = gr_complex((r8 * cos(13 * M_PI / 32.0)), (r8 * sin(13 * M_PI / 32.0)));
            m_256apsk[134] = gr_complex((r8 * cos(9 * M_PI / 32.0)), (r8 * sin(9 * M_PI / 32.0)));
            m_256apsk[135] = gr_complex((r8 * cos(11 * M_PI / 32.0)), (r8 * sin(11 * M_PI / 32.0)));
            m_256apsk[136] = gr_complex((r8 * cos(31 * M_PI / 32.0)), (r8 * sin(31 * M_PI / 32.0)));
            m_256apsk[137] = gr_complex((r8 * cos(29 * M_PI / 32.0)), (r8 * sin(29 * M_PI / 32.0)));
            m_256apsk[138] = gr_complex((r8 * cos(25 * M_PI / 32.0)), (r8 * sin(25 * M_PI / 32.0)));
            m_256apsk[139] = gr_complex((r8 * cos(27 * M_PI / 32.0)), (r8 * sin(27 * M_PI / 32.0)));
            m_256apsk[140] = gr_complex((r8 * cos(17 * M_PI / 32.0)), (r8 * sin(17 * M_PI / 32.0)));
            m_256apsk[141] = gr_complex((r8 * cos(19 * M_PI / 32.0)), (r8 * sin(19 * M_PI / 32.0)));
            m_256apsk[142] = gr_complex((r8 * cos(23 * M_PI / 32.0)), (r8 * sin(23 * M_PI / 32.0)));
            m_256apsk[143] = gr_complex((r8 * cos(21 * M_PI / 32.0)), (r8 * sin(21 * M_PI / 32.0)));
            m_256apsk[144] = gr_complex((r8 * cos(-1 * M_PI / 32.0)), (r8 * sin(-1 * M_PI / 32.0)));
            m_256apsk[145] = gr_complex((r8 * cos(-3 * M_PI / 32.0)), (r8 * sin(-3 * M_PI / 32.0)));
            m_256apsk[146] = gr_complex((r8 * cos(-7 * M_PI / 32.0)), (r8 * sin(-7 * M_PI / 32.0)));
            m_256apsk[147] = gr_complex((r8 * cos(-5 * M_PI / 32.0)), (r8 * sin(-5 * M_PI / 32.0)));
            m_256apsk[148] = gr_complex((r8 * cos(-15 * M_PI / 32.0)), (r8 * sin(-15 * M_PI / 32.0)));
            m_256apsk[149] = gr_complex((r8 * cos(-13 * M_PI / 32.0)), (r8 * sin(-13 * M_PI / 32.0)));
            m_256apsk[150] = gr_complex((r8 * cos(-9 * M_PI / 32.0)), (r8 * sin(-9 * M_PI / 32.0)));
            m_256apsk[151] = gr_complex((r8 * cos(-11 * M_PI / 32.0)), (r8 * sin(-11 * M_PI / 32.0)));
            m_256apsk[152] = gr_complex((r8 * cos(33 * M_PI / 32.0)), (r8 * sin(33 * M_PI / 32.0)));
            m_256apsk[153] = gr_complex((r8 * cos(35 * M_PI / 32.0)), (r8 * sin(35 * M_PI / 32.0)));
            m_256apsk[154] = gr_complex((r8 * cos(39 * M_PI / 32.0)), (r8 * sin(39 * M_PI / 32.0)));
            m_256apsk[155] = gr_complex((r8 * cos(37 * M_PI / 32.0)), (r8 * sin(37 * M_PI / 32.0)));
            m_256apsk[156] = gr_complex((r8 * cos(47 * M_PI / 32.0)), (r8 * sin(47 * M_PI / 32.0)));
            m_256apsk[157] = gr_complex((r8 * cos(45 * M_PI / 32.0)), (r8 * sin(45 * M_PI / 32.0)));
            m_256apsk[158] = gr_complex((r8 * cos(41 * M_PI / 32.0)), (r8 * sin(41 * M_PI / 32.0)));
            m_256apsk[159] = gr_complex((r8 * cos(43 * M_PI / 32.0)), (r8 * sin(43 * M_PI / 32.0)));
            m_256apsk[160] = gr_complex((r7 * cos(M_PI / 32.0)), (r7 * sin(M_PI / 32.0)));
            m_256apsk[161] = gr_complex((r7 * cos(3 * M_PI / 32.0)), (r7 * sin(3 * M_PI / 32.0)));
            m_256apsk[162] = gr_complex((r7 * cos(7 * M_PI / 32.0)), (r7 * sin(7 * M_PI / 32.0)));
            m_256apsk[163] = gr_complex((r7 * cos(5 * M_PI / 32.0)), (r7 * sin(5 * M_PI / 32.0)));
            m_256apsk[164] = gr_complex((r7 * cos(15 * M_PI / 32.0)), (r7 * sin(15 * M_PI / 32.0)));
            m_256apsk[165] = gr_complex((r7 * cos(13 * M_PI / 32.0)), (r7 * sin(13 * M_PI / 32.0)));
            m_256apsk[166] = gr_complex((r7 * cos(9 * M_PI / 32.0)), (r7 * sin(9 * M_PI / 32.0)));
            m_256apsk[167] = gr_complex((r7 * cos(11 * M_PI / 32.0)), (r7 * sin(11 * M_PI / 32.0)));
            m_256apsk[168] = gr_complex((r7 * cos(31 * M_PI / 32.0)), (r7 * sin(31 * M_PI / 32.0)));
            m_256apsk[169] = gr_complex((r7 * cos(29 * M_PI / 32.0)), (r7 * sin(29 * M_PI / 32.0)));
            m_256apsk[170] = gr_complex((r7 * cos(25 * M_PI / 32.0)), (r7 * sin(25 * M_PI / 32.0)));
            m_256apsk[171] = gr_complex((r7 * cos(27 * M_PI / 32.0)), (r7 * sin(27 * M_PI / 32.0)));
            m_256apsk[172] = gr_complex((r7 * cos(17 * M_PI / 32.0)), (r7 * sin(17 * M_PI / 32.0)));
            m_256apsk[173] = gr_complex((r7 * cos(19 * M_PI / 32.0)), (r7 * sin(19 * M_PI / 32.0)));
            m_256apsk[174] = gr_complex((r7 * cos(23 * M_PI / 32.0)), (r7 * sin(23 * M_PI / 32.0)));
            m_256apsk[175] = gr_complex((r7 * cos(21 * M_PI / 32.0)), (r7 * sin(21 * M_PI / 32.0)));
            m_256apsk[176] = gr_complex((r7 * cos(-1 * M_PI / 32.0)), (r7 * sin(-1 * M_PI / 32.0)));
            m_256apsk[177] = gr_complex((r7 * cos(-3 * M_PI / 32.0)), (r7 * sin(-3 * M_PI / 32.0)));
            m_256apsk[178] = gr_complex((r7 * cos(-7 * M_PI / 32.0)), (r7 * sin(-7 * M_PI / 32.0)));
            m_256apsk[179] = gr_complex((r7 * cos(-5 * M_PI / 32.0)), (r7 * sin(-5 * M_PI / 32.0)));
            m_256apsk[180] = gr_complex((r7 * cos(-15 * M_PI / 32.0)), (r7 * sin(-15 * M_PI / 32.0)));
            m_256apsk[181] = gr_complex((r7 * cos(-13 * M_PI / 32.0)), (r7 * sin(-13 * M_PI / 32.0)));
            m_256apsk[182] = gr_complex((r7 * cos(-9 * M_PI / 32.0)), (r7 * sin(-9 * M_PI / 32.0)));
            m_256apsk[183] = gr_complex((r7 * cos(-11 * M_PI / 32.0)), (r7 * sin(-11 * M_PI / 32.0)));
            m_256apsk[184] = gr_complex((r7 * cos(33 * M_PI / 32.0)), (r7 * sin(33 * M_PI / 32.0)));
            m_256apsk[185] = gr_complex((r7 * cos(35 * M_PI / 32.0)), (r7 * sin(35 * M_PI / 32.0)));
            m_256apsk[186] = gr_complex((r7 * cos(39 * M_PI / 32.0)), (r7 * sin(39 * M_PI / 32.0)));
            m_256apsk[187] = gr_complex((r7 * cos(37 * M_PI / 32.0)), (r7 * sin(37 * M_PI / 32.0)));
            m_256apsk[188] = gr_complex((r7 * cos(47 * M_PI / 32.0)), (r7 * sin(47 * M_PI / 32.0)));
            m_256apsk[189] = gr_complex((r7 * cos(45 * M_PI / 32.0)), (r7 * sin(45 * M_PI / 32.0)));
            m_256apsk[190] = gr_complex((r7 * cos(41 * M_PI / 32.0)), (r7 * sin(41 * M_PI / 32.0)));
            m_256apsk[191] = gr_complex((r7 * cos(43 * M_PI / 32.0)), (r7 * sin(43 * M_PI / 32.0)));
            m_256apsk[192] = gr_complex((r5 * cos(M_PI / 32.0)), (r5 * sin(M_PI / 32.0)));
            m_256apsk[193] = gr_complex((r5 * cos(3 * M_PI / 32.0)), (r5 * sin(3 * M_PI / 32.0)));
            m_256apsk[194] = gr_complex((r5 * cos(7 * M_PI / 32.0)), (r5 * sin(7 * M_PI / 32.0)));
            m_256apsk[195] = gr_complex((r5 * cos(5 * M_PI / 32.0)), (r5 * sin(5 * M_PI / 32.0)));
            m_256apsk[196] = gr_complex((r5 * cos(15 * M_PI / 32.0)), (r5 * sin(15 * M_PI / 32.0)));
            m_256apsk[197] = gr_complex((r5 * cos(13 * M_PI / 32.0)), (r5 * sin(13 * M_PI / 32.0)));
            m_256apsk[198] = gr_complex((r5 * cos(9 * M_PI / 32.0)), (r5 * sin(9 * M_PI / 32.0)));
            m_256apsk[199] = gr_complex((r5 * cos(11 * M_PI / 32.0)), (r5 * sin(11 * M_PI / 32.0)));
            m_256apsk[200] = gr_complex((r5 * cos(31 * M_PI / 32.0)), (r5 * sin(31 * M_PI / 32.0)));
            m_256apsk[201] = gr_complex((r5 * cos(29 * M_PI / 32.0)), (r5 * sin(29 * M_PI / 32.0)));
            m_256apsk[202] = gr_complex((r5 * cos(25 * M_PI / 32.0)), (r5 * sin(25 * M_PI / 32.0)));
            m_256apsk[203] = gr_complex((r5 * cos(27 * M_PI / 32.0)), (r5 * sin(27 * M_PI / 32.0)));
            m_256apsk[204] = gr_complex((r5 * cos(17 * M_PI / 32.0)), (r5 * sin(17 * M_PI / 32.0)));
            m_256apsk[205] = gr_complex((r5 * cos(19 * M_PI / 32.0)), (r5 * sin(19 * M_PI / 32.0)));
            m_256apsk[206] = gr_complex((r5 * cos(23 * M_PI / 32.0)), (r5 * sin(23 * M_PI / 32.0)));
            m_256apsk[207] = gr_complex((r5 * cos(21 * M_PI / 32.0)), (r5 * sin(21 * M_PI / 32.0)));
            m_256apsk[208] = gr_complex((r5 * cos(-1 * M_PI / 32.0)), (r5 * sin(-1 * M_PI / 32.0)));
            m_256apsk[209] = gr_complex((r5 * cos(-3 * M_PI / 32.0)), (r5 * sin(-3 * M_PI / 32.0)));
            m_256apsk[210] = gr_complex((r5 * cos(-7 * M_PI / 32.0)), (r5 * sin(-7 * M_PI / 32.0)));
            m_256apsk[211] = gr_complex((r5 * cos(-5 * M_PI / 32.0)), (r5 * sin(-5 * M_PI / 32.0)));
            m_256apsk[212] = gr_complex((r5 * cos(-15 * M_PI / 32.0)), (r5 * sin(-15 * M_PI / 32.0)));
            m_256apsk[213] = gr_complex((r5 * cos(-13 * M_PI / 32.0)), (r5 * sin(-13 * M_PI / 32.0)));
            m_256apsk[214] = gr_complex((r5 * cos(-9 * M_PI / 32.0)), (r5 * sin(-9 * M_PI / 32.0)));
            m_256apsk[215] = gr_complex((r5 * cos(-11 * M_PI / 32.0)), (r5 * sin(-11 * M_PI / 32.0)));
            m_256apsk[216] = gr_complex((r5 * cos(33 * M_PI / 32.0)), (r5 * sin(33 * M_PI / 32.0)));
            m_256apsk[217] = gr_complex((r5 * cos(35 * M_PI / 32.0)), (r5 * sin(35 * M_PI / 32.0)));
            m_256apsk[218] = gr_complex((r5 * cos(39 * M_PI / 32.0)), (r5 * sin(39 * M_PI / 32.0)));
            m_256apsk[219] = gr_complex((r5 * cos(37 * M_PI / 32.0)), (r5 * sin(37 * M_PI / 32.0)));
            m_256apsk[220] = gr_complex((r5 * cos(47 * M_PI / 32.0)), (r5 * sin(47 * M_PI / 32.0)));
            m_256apsk[221] = gr_complex((r5 * cos(45 * M_PI / 32.0)), (r5 * sin(45 * M_PI / 32.0)));
            m_256apsk[222] = gr_complex((r5 * cos(41 * M_PI / 32.0)), (r5 * sin(41 * M_PI / 32.0)));
            m_256apsk[223] = gr_complex((r5 * cos(43 * M_PI / 32.0)), (r5 * sin(43 * M_PI / 32.0)));
            m_256apsk[224] = gr_complex((r6 * cos(M_PI / 32.0)), (r6 * sin(M_PI / 32.0)));
            m_256apsk[225] = gr_complex((r6 * cos(3 * M_PI / 32.0)), (r6 * sin(3 * M_PI / 32.0)));
            m_256apsk[226] = gr_complex((r6 * cos(7 * M_PI / 32.0)), (r6 * sin(7 * M_PI / 32.0)));
            m_256apsk[227] = gr_complex((r6 * cos(5 * M_PI / 32.0)), (r6 * sin(5 * M_PI / 32.0)));
            m_256apsk[228] = gr_complex((r6 * cos(15 * M_PI / 32.0)), (r6 * sin(15 * M_PI / 32.0)));
            m_256apsk[229] = gr_complex((r6 * cos(13 * M_PI / 32.0)), (r6 * sin(13 * M_PI / 32.0)));
            m_256apsk[230] = gr_complex((r6 * cos(9 * M_PI / 32.0)), (r6 * sin(9 * M_PI / 32.0)));
            m_256apsk[231] = gr_complex((r6 * cos(11 * M_PI / 32.0)), (r6 * sin(11 * M_PI / 32.0)));
            m_256apsk[232] = gr_complex((r6 * cos(31 * M_PI / 32.0)), (r6 * sin(31 * M_PI / 32.0)));
            m_256apsk[233] = gr_complex((r6 * cos(29 * M_PI / 32.0)), (r6 * sin(29 * M_PI / 32.0)));
            m_256apsk[234] = gr_complex((r6 * cos(25 * M_PI / 32.0)), (r6 * sin(25 * M_PI / 32.0)));
            m_256apsk[235] = gr_complex((r6 * cos(27 * M_PI / 32.0)), (r6 * sin(27 * M_PI / 32.0)));
            m_256apsk[236] = gr_complex((r6 * cos(17 * M_PI / 32.0)), (r6 * sin(17 * M_PI / 32.0)));
            m_256apsk[237] = gr_complex((r6 * cos(19 * M_PI / 32.0)), (r6 * sin(19 * M_PI / 32.0)));
            m_256apsk[238] = gr_complex((r6 * cos(23 * M_PI / 32.0)), (r6 * sin(23 * M_PI / 32.0)));
            m_256apsk[239] = gr_complex((r6 * cos(21 * M_PI / 32.0)), (r6 * sin(21 * M_PI / 32.0)));
            m_256apsk[240] = gr_complex((r6 * cos(-1 * M_PI / 32.0)), (r6 * sin(-1 * M_PI / 32.0)));
            m_256apsk[241] = gr_complex((r6 * cos(-3 * M_PI / 32.0)), (r6 * sin(-3 * M_PI / 32.0)));
            m_256apsk[242] = gr_complex((r6 * cos(-7 * M_PI / 32.0)), (r6 * sin(-7 * M_PI / 32.0)));
            m_256apsk[243] = gr_complex((r6 * cos(-5 * M_PI / 32.0)), (r6 * sin(-5 * M_PI / 32.0)));
            m_256apsk[244] = gr_complex((r6 * cos(-15 * M_PI / 32.0)), (r6 * sin(-15 * M_PI / 32.0)));
            m_256apsk[245] = gr_complex((r6 * cos(-13 * M_PI / 32.0)), (r6 * sin(-13 * M_PI / 32.0)));
            m_256apsk[246] = gr_complex((r6 * cos(-9 * M_PI / 32.0)), (r6 * sin(-9 * M_PI / 32.0)));
            m_256apsk[247] = gr_complex((r6 * cos(-11 * M_PI / 32.0)), (r6 * sin(-11 * M_PI / 32.0)));
            m_256apsk[248] = gr_complex((r6 * cos(33 * M_PI / 32.0)), (r6 * sin(33 * M_PI / 32.0)));
            m_256apsk[249] = gr_complex((r6 * cos(35 * M_PI / 32.0)), (r6 * sin(35 * M_PI / 32.0)));
            m_256apsk[250] = gr_complex((r6 * cos(39 * M_PI / 32.0)), (r6 * sin(39 * M_PI / 32.0)));
            m_256apsk[251] = gr_complex((r6 * cos(37 * M_PI / 32.0)), (r6 * sin(37 * M_PI / 32.0)));
            m_256apsk[252] = gr_complex((r6 * cos(47 * M_PI / 32.0)), (r6 * sin(47 * M_PI / 32.0)));
            m_256apsk[253] = gr_complex((r6 * cos(45 * M_PI / 32.0)), (r6 * sin(45 * M_PI / 32.0)));
            m_256apsk[254] = gr_complex((r6 * cos(41 * M_PI / 32.0)), (r6 * sin(41 * M_PI / 32.0)));
            m_256apsk[255] = gr_complex((r6 * cos(43 * M_PI / 32.0)), (r6 * sin(43 * M_PI / 32.0)));
          }
          break;
        case MOD_64QAM:
          m_64apsk[0] = gr_complex(  1.0,  1.0);
          m_64apsk[1] = gr_complex(  1.0, -1.0);
          m_64apsk[2] = gr_complex(  1.0, -3.0);
          m_64apsk[3] = gr_complex( -3.0, -1.0);
          m_64apsk[4] = gr_complex( -3.0,  1.0);
          m_64apsk[5] = gr_complex(  1.0,  3.0);
          m_64apsk[6] = gr_complex( -3.0, -3.0);
          m_64apsk[7] = gr_complex( -3.0,  3.0);
          m_64apsk[8] = gr_complex( -1.0,  1.0);
          m_64apsk[9] = gr_complex( -1.0, -1.0);
          m_64apsk[10] = gr_complex( 3.0,  1.0);
          m_64apsk[11] = gr_complex(-1.0,  3.0);
          m_64apsk[12] = gr_complex(-1.0, -3.0);
          m_64apsk[13] = gr_complex( 3.0, -1.0);
          m_64apsk[14] = gr_complex( 3.0, -3.0);
          m_64apsk[15] = gr_complex( 3.0,  3.0);
          m_64apsk[16] = gr_complex( 5.0,  1.0);
          m_64apsk[17] = gr_complex( 1.0, -5.0);
          m_64apsk[18] = gr_complex( 1.0, -7.0);
          m_64apsk[19] = gr_complex(-7.0, -1.0);
          m_64apsk[20] = gr_complex(-3.0,  5.0);
          m_64apsk[21] = gr_complex( 5.0,  3.0);
          m_64apsk[22] = gr_complex(-7.0, -3.0);
          m_64apsk[23] = gr_complex(-3.0,  7.0);
          m_64apsk[24] = gr_complex(-1.0,  5.0);
          m_64apsk[25] = gr_complex(-5.0, -1.0);
          m_64apsk[26] = gr_complex( 7.0,  1.0);
          m_64apsk[27] = gr_complex(-1.0,  7.0);
          m_64apsk[28] = gr_complex(-5.0, -3.0);
          m_64apsk[29] = gr_complex( 3.0, -5.0);
          m_64apsk[30] = gr_complex( 3.0, -7.0);
          m_64apsk[31] = gr_complex( 7.0,  3.0);
          m_64apsk[32] = gr_complex( 1.0,  5.0);
          m_64apsk[33] = gr_complex( 5.0, -1.0);
          m_64apsk[34] = gr_complex( 5.0, -3.0);
          m_64apsk[35] = gr_complex(-3.0, -5.0);
          m_64apsk[36] = gr_complex(-7.0,  1.0);
          m_64apsk[37] = gr_complex( 1.0,  7.0);
          m_64apsk[38] = gr_complex(-3.0, -7.0);
          m_64apsk[39] = gr_complex(-7.0,  3.0);
          m_64apsk[40] = gr_complex(-5.0,  1.0);
          m_64apsk[41] = gr_complex(-1.0, -5.0);
          m_64apsk[42] = gr_complex( 3.0,  5.0);
          m_64apsk[43] = gr_complex(-5.0,  3.0);
          m_64apsk[44] = gr_complex(-1.0, -7.0);
          m_64apsk[45] = gr_complex( 7.0, -1.0);
          m_64apsk[46] = gr_complex( 7.0, -3.0);
          m_64apsk[47] = gr_complex( 3.0,  7.0);
          m_64apsk[48] = gr_complex( 5.0,  5.0);
          m_64apsk[49] = gr_complex( 5.0, -5.0);
          m_64apsk[50] = gr_complex( 5.0, -7.0);
          m_64apsk[51] = gr_complex(-7.0, -5.0);
          m_64apsk[52] = gr_complex(-7.0,  5.0);
          m_64apsk[53] = gr_complex( 5.0,  7.0);
          m_64apsk[54] = gr_complex(-7.0, -7.0);
          m_64apsk[55] = gr_complex(-7.0,  7.0);
          m_64apsk[56] = gr_complex(-5.0,  5.0);
          m_64apsk[57] = gr_complex(-5.0, -5.0);
          m_64apsk[58] = gr_complex( 7.0,  5.0);
          m_64apsk[59] = gr_complex(-5.0,  7.0);
          m_64apsk[60] = gr_complex(-5.0, -7.0);
          m_64apsk[61] = gr_complex( 7.0, -5.0);
          m_64apsk[62] = gr_complex( 7.0, -7.0);
          m_64apsk[63] = gr_complex( 7.0,  7.0);
          break;
        case MOD_256QAM:
          m_256apsk[0] = gr_complex(    1.0,   1.0);
          m_256apsk[1] = gr_complex(    1.0,  -1.0);
          m_256apsk[2] = gr_complex(   -3.0,   1.0);
          m_256apsk[3] = gr_complex(    1.0,   3.0);
          m_256apsk[4] = gr_complex(    1.0,   5.0);
          m_256apsk[5] = gr_complex(    5.0,  -1.0);
          m_256apsk[6] = gr_complex(   -7.0,   1.0);
          m_256apsk[7] = gr_complex(    1.0,   7.0);
          m_256apsk[8] = gr_complex(    1.0,   9.0);
          m_256apsk[9] = gr_complex(    9.0,  -1.0);
          m_256apsk[10] = gr_complex( -11.0,   1.0);
          m_256apsk[11] = gr_complex(   1.0,  11.0);
          m_256apsk[12] = gr_complex(   1.0,  13.0);
          m_256apsk[13] = gr_complex(  13.0,  -1.0);
          m_256apsk[14] = gr_complex( -15.0,   1.0);
          m_256apsk[15] = gr_complex(   1.0,  15.0);
          m_256apsk[16] = gr_complex(  -1.0,   1.0);
          m_256apsk[17] = gr_complex(  -1.0,  -1.0);
          m_256apsk[18] = gr_complex(  -1.0,  -3.0);
          m_256apsk[19] = gr_complex(   3.0,  -1.0);
          m_256apsk[20] = gr_complex(  -5.0,   1.0);
          m_256apsk[21] = gr_complex(  -1.0,  -5.0);
          m_256apsk[22] = gr_complex(  -1.0,  -7.0);
          m_256apsk[23] = gr_complex(   7.0,  -1.0);
          m_256apsk[24] = gr_complex(  -9.0,   1.0);
          m_256apsk[25] = gr_complex(  -1.0,  -9.0);
          m_256apsk[26] = gr_complex(  -1.0, -11.0);
          m_256apsk[27] = gr_complex(  11.0,  -1.0);
          m_256apsk[28] = gr_complex( -13.0,   1.0);
          m_256apsk[29] = gr_complex(  -1.0, -13.0);
          m_256apsk[30] = gr_complex(  -1.0, -15.0);
          m_256apsk[31] = gr_complex(  15.0,  -1.0);
          m_256apsk[32] = gr_complex(   1.0,  -3.0);
          m_256apsk[33] = gr_complex(  -3.0,  -1.0);
          m_256apsk[34] = gr_complex(  -3.0,  -3.0);
          m_256apsk[35] = gr_complex(  -3.0,   3.0);
          m_256apsk[36] = gr_complex(   5.0,  -3.0);
          m_256apsk[37] = gr_complex(  -3.0,  -5.0);
          m_256apsk[38] = gr_complex(  -3.0,  -7.0);
          m_256apsk[39] = gr_complex(  -7.0,   3.0);
          m_256apsk[40] = gr_complex(   9.0,  -3.0);
          m_256apsk[41] = gr_complex(  -3.0,  -9.0);
          m_256apsk[42] = gr_complex(  -3.0, -11.0);
          m_256apsk[43] = gr_complex( -11.0,   3.0);
          m_256apsk[44] = gr_complex(  13.0,  -3.0);
          m_256apsk[45] = gr_complex(  -3.0, -13.0);
          m_256apsk[46] = gr_complex(  -3.0, -15.0);
          m_256apsk[47] = gr_complex( -15.0,   3.0);
          m_256apsk[48] = gr_complex(   3.0,   1.0);
          m_256apsk[49] = gr_complex(  -1.0,   3.0);
          m_256apsk[50] = gr_complex(   3.0,  -3.0);
          m_256apsk[51] = gr_complex(   3.0,   3.0);
          m_256apsk[52] = gr_complex(   3.0,   5.0);
          m_256apsk[53] = gr_complex(  -5.0,   3.0);
          m_256apsk[54] = gr_complex(   7.0,  -3.0);
          m_256apsk[55] = gr_complex(   3.0,   7.0);
          m_256apsk[56] = gr_complex(   3.0,   9.0);
          m_256apsk[57] = gr_complex(  -9.0,   3.0);
          m_256apsk[58] = gr_complex(  11.0,  -3.0);
          m_256apsk[59] = gr_complex(   3.0,  11.0);
          m_256apsk[60] = gr_complex(   3.0,  13.0);
          m_256apsk[61] = gr_complex( -13.0,   3.0);
          m_256apsk[62] = gr_complex(  15.0,  -3.0);
          m_256apsk[63] = gr_complex(   3.0,  15.0);
          m_256apsk[64] = gr_complex(   5.0,   1.0);
          m_256apsk[65] = gr_complex(   1.0,  -5.0);
          m_256apsk[66] = gr_complex(  -3.0,   5.0);
          m_256apsk[67] = gr_complex(   5.0,   3.0);
          m_256apsk[68] = gr_complex(   5.0,   5.0);
          m_256apsk[69] = gr_complex(   5.0,  -5.0);
          m_256apsk[70] = gr_complex(  -7.0,   5.0);
          m_256apsk[71] = gr_complex(   5.0,   7.0);
          m_256apsk[72] = gr_complex(   5.0,   9.0);
          m_256apsk[73] = gr_complex(   9.0,  -5.0);
          m_256apsk[74] = gr_complex( -11.0,   5.0);
          m_256apsk[75] = gr_complex(   5.0,  11.0);
          m_256apsk[76] = gr_complex(   5.0,  13.0);
          m_256apsk[77] = gr_complex(  13.0,  -5.0);
          m_256apsk[78] = gr_complex( -15.0,   5.0);
          m_256apsk[79] = gr_complex(   5.0,  15.0);
          m_256apsk[80] = gr_complex(  -1.0,   5.0);
          m_256apsk[81] = gr_complex(  -5.0,  -1.0);
          m_256apsk[82] = gr_complex(  -5.0,  -3.0);
          m_256apsk[83] = gr_complex(   3.0,  -5.0);
          m_256apsk[84] = gr_complex(  -5.0,   5.0);
          m_256apsk[85] = gr_complex(  -5.0,  -5.0);
          m_256apsk[86] = gr_complex(  -5.0,  -7.0);
          m_256apsk[87] = gr_complex(   7.0,  -5.0);
          m_256apsk[88] = gr_complex(  -9.0,   5.0);
          m_256apsk[89] = gr_complex(  -5.0,  -9.0);
          m_256apsk[90] = gr_complex(  -5.0, -11.0);
          m_256apsk[91] = gr_complex(  11.0,  -5.0);
          m_256apsk[92] = gr_complex( -13.0,   5.0);
          m_256apsk[93] = gr_complex(  -5.0, -13.0);
          m_256apsk[94] = gr_complex(  -5.0, -15.0);
          m_256apsk[95] = gr_complex(  15.0,  -5.0);
          m_256apsk[96] = gr_complex(   1.0,  -7.0);
          m_256apsk[97] = gr_complex(  -7.0,  -1.0);
          m_256apsk[98] = gr_complex(  -7.0,  -3.0);
          m_256apsk[99] = gr_complex(  -3.0,   7.0);
          m_256apsk[100] = gr_complex(  5.0,  -7.0);
          m_256apsk[101] = gr_complex( -7.0,  -5.0);
          m_256apsk[102] = gr_complex( -7.0,  -7.0);
          m_256apsk[103] = gr_complex( -7.0,   7.0);
          m_256apsk[104] = gr_complex(  9.0,  -7.0);
          m_256apsk[105] = gr_complex( -7.0,  -9.0);
          m_256apsk[106] = gr_complex( -7.0, -11.0);
          m_256apsk[107] = gr_complex(-11.0,   7.0);
          m_256apsk[108] = gr_complex( 13.0,  -7.0);
          m_256apsk[109] = gr_complex( -7.0, -13.0);
          m_256apsk[110] = gr_complex( -7.0, -15.0);
          m_256apsk[111] = gr_complex(-15.0,   7.0);
          m_256apsk[112] = gr_complex(  7.0,   1.0);
          m_256apsk[113] = gr_complex( -1.0,   7.0);
          m_256apsk[114] = gr_complex(  3.0,  -7.0);
          m_256apsk[115] = gr_complex(  7.0,   3.0);
          m_256apsk[116] = gr_complex(  7.0,   5.0);
          m_256apsk[117] = gr_complex( -5.0,   7.0);
          m_256apsk[118] = gr_complex(  7.0,  -7.0);
          m_256apsk[119] = gr_complex(  7.0,   7.0);
          m_256apsk[120] = gr_complex(  7.0,   9.0);
          m_256apsk[121] = gr_complex( -9.0,   7.0);
          m_256apsk[122] = gr_complex( 11.0,  -7.0);
          m_256apsk[123] = gr_complex(  7.0,  11.0);
          m_256apsk[124] = gr_complex(  7.0,  13.0);
          m_256apsk[125] = gr_complex(-13.0,   7.0);
          m_256apsk[126] = gr_complex( 15.0,  -7.0);
          m_256apsk[127] = gr_complex(  7.0,  15.0);
          m_256apsk[128] = gr_complex(  9.0,   1.0);
          m_256apsk[129] = gr_complex(  1.0,  -9.0);
          m_256apsk[130] = gr_complex( -3.0,   9.0);
          m_256apsk[131] = gr_complex(  9.0,   3.0);
          m_256apsk[132] = gr_complex(  9.0,   5.0);
          m_256apsk[133] = gr_complex(  5.0,  -9.0);
          m_256apsk[134] = gr_complex( -7.0,   9.0);
          m_256apsk[135] = gr_complex(  9.0,   7.0);
          m_256apsk[136] = gr_complex(  9.0,   9.0);
          m_256apsk[137] = gr_complex(  9.0,  -9.0);
          m_256apsk[138] = gr_complex(-11.0,   9.0);
          m_256apsk[139] = gr_complex(  9.0,  11.0);
          m_256apsk[140] = gr_complex(  9.0,  13.0);
          m_256apsk[141] = gr_complex( 13.0,  -9.0);
          m_256apsk[142] = gr_complex(-15.0,   9.0);
          m_256apsk[143] = gr_complex(  9.0,  15.0);
          m_256apsk[144] = gr_complex( -1.0,   9.0);
          m_256apsk[145] = gr_complex( -9.0,  -1.0);
          m_256apsk[146] = gr_complex( -9.0,  -3.0);
          m_256apsk[147] = gr_complex(  3.0,  -9.0);
          m_256apsk[148] = gr_complex( -5.0,   9.0);
          m_256apsk[149] = gr_complex( -9.0,  -5.0);
          m_256apsk[150] = gr_complex( -9.0,  -7.0);
          m_256apsk[151] = gr_complex(  7.0,  -9.0);
          m_256apsk[152] = gr_complex( -9.0,   9.0);
          m_256apsk[153] = gr_complex( -9.0,  -9.0);
          m_256apsk[154] = gr_complex( -9.0, -11.0);
          m_256apsk[155] = gr_complex( 11.0,  -9.0);
          m_256apsk[156] = gr_complex(-13.0,   9.0);
          m_256apsk[157] = gr_complex( -9.0, -13.0);
          m_256apsk[158] = gr_complex( -9.0, -15.0);
          m_256apsk[159] = gr_complex( 15.0,  -9.0);
          m_256apsk[160] = gr_complex(  1.0, -11.0);
          m_256apsk[161] = gr_complex(-11.0,  -1.0);
          m_256apsk[162] = gr_complex(-11.0,  -3.0);
          m_256apsk[163] = gr_complex( -3.0,  11.0);
          m_256apsk[164] = gr_complex(  5.0, -11.0);
          m_256apsk[165] = gr_complex(-11.0,  -5.0);
          m_256apsk[166] = gr_complex(-11.0,  -7.0);
          m_256apsk[167] = gr_complex( -7.0,  11.0);
          m_256apsk[168] = gr_complex(  9.0, -11.0);
          m_256apsk[169] = gr_complex(-11.0,  -9.0);
          m_256apsk[170] = gr_complex(-11.0, -11.0);
          m_256apsk[171] = gr_complex(-11.0,  11.0);
          m_256apsk[172] = gr_complex( 13.0, -11.0);
          m_256apsk[173] = gr_complex(-11.0, -13.0);
          m_256apsk[174] = gr_complex(-11.0, -15.0);
          m_256apsk[175] = gr_complex(-15.0,  11.0);
          m_256apsk[176] = gr_complex( 11.0,   1.0);
          m_256apsk[177] = gr_complex( -1.0,  11.0);
          m_256apsk[178] = gr_complex(  3.0, -11.0);
          m_256apsk[179] = gr_complex( 11.0,   3.0);
          m_256apsk[180] = gr_complex( 11.0,   5.0);
          m_256apsk[181] = gr_complex( -5.0,  11.0);
          m_256apsk[182] = gr_complex(  7.0, -11.0);
          m_256apsk[183] = gr_complex( 11.0,   7.0);
          m_256apsk[184] = gr_complex( 11.0,   9.0);
          m_256apsk[185] = gr_complex( -9.0,  11.0);
          m_256apsk[186] = gr_complex( 11.0, -11.0);
          m_256apsk[187] = gr_complex( 11.0,  11.0);
          m_256apsk[188] = gr_complex( 11.0,  13.0);
          m_256apsk[189] = gr_complex(-13.0,  11.0);
          m_256apsk[190] = gr_complex( 15.0, -11.0);
          m_256apsk[191] = gr_complex( 11.0,  15.0);
          m_256apsk[192] = gr_complex( 13.0,   1.0);
          m_256apsk[193] = gr_complex(  1.0, -13.0);
          m_256apsk[194] = gr_complex( -3.0,  13.0);
          m_256apsk[195] = gr_complex( 13.0,   3.0);
          m_256apsk[196] = gr_complex( 13.0,   5.0);
          m_256apsk[197] = gr_complex(  5.0, -13.0);
          m_256apsk[198] = gr_complex( -7.0,  13.0);
          m_256apsk[199] = gr_complex( 13.0,   7.0);
          m_256apsk[200] = gr_complex( 13.0,   9.0);
          m_256apsk[201] = gr_complex(  9.0, -13.0);
          m_256apsk[202] = gr_complex(-11.0,  13.0);
          m_256apsk[203] = gr_complex( 13.0,  11.0);
          m_256apsk[204] = gr_complex( 13.0,  13.0);
          m_256apsk[205] = gr_complex( 13.0, -13.0);
          m_256apsk[206] = gr_complex(-15.0,  13.0);
          m_256apsk[207] = gr_complex( 13.0,  15.0);
          m_256apsk[208] = gr_complex( -1.0,  13.0);
          m_256apsk[209] = gr_complex(-13.0,  -1.0);
          m_256apsk[210] = gr_complex(-13.0,  -3.0);
          m_256apsk[211] = gr_complex(  3.0, -13.0);
          m_256apsk[212] = gr_complex( -5.0,  13.0);
          m_256apsk[213] = gr_complex(-13.0,  -5.0);
          m_256apsk[214] = gr_complex(-13.0,  -7.0);
          m_256apsk[215] = gr_complex(  7.0, -13.0);
          m_256apsk[216] = gr_complex( -9.0,  13.0);
          m_256apsk[217] = gr_complex(-13.0,  -9.0);
          m_256apsk[218] = gr_complex(-13.0, -11.0);
          m_256apsk[219] = gr_complex( 11.0, -13.0);
          m_256apsk[220] = gr_complex(-13.0,  13.0);
          m_256apsk[221] = gr_complex(-13.0, -13.0);
          m_256apsk[222] = gr_complex(-13.0, -15.0);
          m_256apsk[223] = gr_complex( 15.0, -13.0);
          m_256apsk[224] = gr_complex(  1.0, -15.0);
          m_256apsk[225] = gr_complex(-15.0,  -1.0);
          m_256apsk[226] = gr_complex(-15.0,  -3.0);
          m_256apsk[227] = gr_complex( -3.0,  15.0);
          m_256apsk[228] = gr_complex(  5.0, -15.0);
          m_256apsk[229] = gr_complex(-15.0,  -5.0);
          m_256apsk[230] = gr_complex(-15.0,  -7.0);
          m_256apsk[231] = gr_complex( -7.0,  15.0);
          m_256apsk[232] = gr_complex(  9.0, -15.0);
          m_256apsk[233] = gr_complex(-15.0,  -9.0);
          m_256apsk[234] = gr_complex(-15.0, -11.0);
          m_256apsk[235] = gr_complex(-11.0,  15.0);
          m_256apsk[236] = gr_complex( 13.0, -15.0);
          m_256apsk[237] = gr_complex(-15.0, -13.0);
          m_256apsk[238] = gr_complex(-15.0, -15.0);
          m_256apsk[239] = gr_complex(-15.0,  15.0);
          m_256apsk[240] = gr_complex( 15.0,   1.0);
          m_256apsk[241] = gr_complex( -1.0,  15.0);
          m_256apsk[242] = gr_complex(  3.0, -15.0);
          m_256apsk[243] = gr_complex( 15.0,   3.0);
          m_256apsk[244] = gr_complex( 15.0,   5.0);
          m_256apsk[245] = gr_complex( -5.0,  15.0);
          m_256apsk[246] = gr_complex(  7.0, -15.0);
          m_256apsk[247] = gr_complex( 15.0,   7.0);
          m_256apsk[248] = gr_complex( 15.0,   9.0);
          m_256apsk[249] = gr_complex( -9.0,  15.0);
          m_256apsk[250] = gr_complex( 11.0, -15.0);
          m_256apsk[251] = gr_complex( 15.0,  11.0);
          m_256apsk[252] = gr_complex( 15.0,  13.0);
          m_256apsk[253] = gr_complex(-13.0,  15.0);
          m_256apsk[254] = gr_complex( 15.0, -15.0);
          m_256apsk[255] = gr_complex( 15.0,  15.0);
          break;
        case MOD_8VSB:
          m_8psk[0] = gr_complex(-7.0 + 1.25, 0.0);
          m_8psk[1] = gr_complex(-5.0 + 1.25, 0.0);
          m_8psk[2] = gr_complex(-3.0 + 1.25, 0.0);
          m_8psk[3] = gr_complex(-1.0 + 1.25, 0.0);
          m_8psk[4] = gr_complex( 1.0 + 1.25, 0.0);
          m_8psk[5] = gr_complex( 3.0 + 1.25, 0.0);
          m_8psk[6] = gr_complex( 5.0 + 1.25, 0.0);
          m_8psk[7] = gr_complex( 7.0 + 1.25, 0.0);
          break;
        default:
          m_qpsk[0] = gr_complex((r1 * cos(M_PI / 4.0)), (r1 * sin(M_PI / 4.0)));
          m_qpsk[1] = gr_complex((r1 * cos(7 * M_PI / 4.0)), (r1 * sin(7 * M_PI / 4.0)));
          m_qpsk[2] = gr_complex((r1 * cos(3 * M_PI / 4.0)), (r1 * sin(3 * M_PI / 4.0)));
          m_qpsk[3] = gr_complex((r1 * cos(5 * M_PI / 4.0)), (r1 * sin(5 * M_PI / 4.0)));
          break;
      }
      signal_constellation = constellation;
      signal_interpolation = interpolation;
      set_output_multiple(2);
    }

    /*
     * Our virtual destructor.
     */
    dvbs2_modulator_bc_impl::~dvbs2_modulator_bc_impl()
    {
    }

    void
    dvbs2_modulator_bc_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      if (signal_interpolation == INTERPOLATION_OFF) {
        ninput_items_required[0] = noutput_items;
      }
      else {
        ninput_items_required[0] = noutput_items / 2;
      }
    }

    int
    dvbs2_modulator_bc_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      int index;
      gr_complex zero;

      zero = gr_complex(0.0, 0.0);

      if (signal_interpolation == INTERPOLATION_OFF) {
        switch (signal_constellation) {
          case MOD_BPSK:
          case MOD_BPSK_SF2:
            for (int i = 0; i < noutput_items; i++) {
              index = *in++;
              *out++ = m_bpsk[i & 1][index & 0x1];
            }
            break;
          case MOD_QPSK:
            for (int i = 0; i < noutput_items; i++) {
              index = *in++;
              *out++ = m_qpsk[index & 0x3];
            }
            break;
          case MOD_8PSK:
          case MOD_8VSB:
          case MOD_8APSK:
            for (int i = 0; i < noutput_items; i++) {
              index = *in++;
              *out++ = m_8psk[index & 0x7];
            }
            break;
          case MOD_16APSK:
          case MOD_8_8APSK:
            for (int i = 0; i < noutput_items; i++) {
              index = *in++;
              *out++ = m_16apsk[index & 0xf];
            }
            break;
          case MOD_32APSK:
          case MOD_4_12_16APSK:
          case MOD_4_8_4_16APSK:
            for (int i = 0; i < noutput_items; i++) {
              index = *in++;
              *out++ = m_32apsk[index & 0x1f];
            }
            break;
          case MOD_64APSK:
          case MOD_64QAM:
          case MOD_8_16_20_20APSK:
          case MOD_4_12_20_28APSK:
            for (int i = 0; i < noutput_items; i++) {
              index = *in++;
              *out++ = m_64apsk[index & 0x3f];
            }
            break;
          case MOD_128APSK:
            for (int i = 0; i < noutput_items; i++) {
              index = *in++;
              *out++ = m_128apsk[index & 0x7f];
            }
            break;
          case MOD_256APSK:
          case MOD_256QAM:
            for (int i = 0; i < noutput_items; i++) {
              index = *in++;
              *out++ = m_256apsk[index & 0xff];
            }
            break;
          default:
            for (int i = 0; i < noutput_items; i++) {
              index = *in++;
              *out++ = m_qpsk[index & 0x3];
            }
            break;
        }
      }
      else {
        switch (signal_constellation) {
          case MOD_BPSK:
          case MOD_BPSK_SF2:
            for (int i = 0; i < noutput_items; i++) {
              index = *in++;
              *out++ = m_bpsk[i & 1][index & 0x1];
              *out++ = zero;
            }
            break;
          case MOD_QPSK:
            for (int i = 0; i < noutput_items / 2; i++) {
              index = *in++;
              *out++ = m_qpsk[index & 0x3];
              *out++ = zero;
            }
            break;
          case MOD_8PSK:
          case MOD_8APSK:
            for (int i = 0; i < noutput_items / 2; i++) {
              index = *in++;
              *out++ = m_8psk[index & 0x7];
              *out++ = zero;
            }
            break;
          case MOD_16APSK:
          case MOD_8_8APSK:
            for (int i = 0; i < noutput_items / 2; i++) {
              index = *in++;
              *out++ = m_16apsk[index & 0xf];
              *out++ = zero;
            }
            break;
          case MOD_32APSK:
          case MOD_4_12_16APSK:
          case MOD_4_8_4_16APSK:
            for (int i = 0; i < noutput_items / 2; i++) {
              index = *in++;
              *out++ = m_32apsk[index & 0x1f];
              *out++ = zero;
            }
            break;
          case MOD_64APSK:
          case MOD_64QAM:
          case MOD_8_16_20_20APSK:
          case MOD_4_12_20_28APSK:
            for (int i = 0; i < noutput_items / 2; i++) {
              index = *in++;
              *out++ = m_64apsk[index & 0x3f];
              *out++ = zero;
            }
            break;
          case MOD_128APSK:
            for (int i = 0; i < noutput_items / 2; i++) {
              index = *in++;
              *out++ = m_128apsk[index & 0x7f];
              *out++ = zero;
            }
            break;
          case MOD_256APSK:
          case MOD_256QAM:
            for (int i = 0; i < noutput_items / 2; i++) {
              index = *in++;
              *out++ = m_256apsk[index & 0xff];
              *out++ = zero;
            }
            break;
          default:
            for (int i = 0; i < noutput_items / 2; i++) {
              index = *in++;
              *out++ = m_qpsk[index & 0x3];
              *out++ = zero;
            }
            break;
        }
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      if (signal_interpolation == INTERPOLATION_OFF) {
        consume_each (noutput_items);
      }
      else {
        consume_each (noutput_items / 2);
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */

