/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc.
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
#include "dvbt2_modulator_bc_impl.h"

namespace gr {
  namespace dtv {

    dvbt2_modulator_bc::sptr
    dvbt2_modulator_bc::make(dvb_framesize_t framesize, dvb_constellation_t constellation, dvbt2_rotation_t rotation)
    {
      return gnuradio::get_initial_sptr
        (new dvbt2_modulator_bc_impl(framesize, constellation, rotation));
    }

    /*
     * The private constructor
     */
    dvbt2_modulator_bc_impl::dvbt2_modulator_bc_impl(dvb_framesize_t framesize, dvb_constellation_t constellation, dvbt2_rotation_t rotation)
      : gr::block("dvbt2_modulator_bc",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {
      double normalization;
      double rotation_angle;
      gr_complex temp;
      cyclic_delay = FALSE;
      if (framesize == FECFRAME_NORMAL) {
        switch (constellation) {
          case MOD_QPSK:
            cell_size = 32400;
            break;
          case MOD_16QAM:
            cell_size = 16200;
            break;
          case MOD_64QAM:
            cell_size = 10800;
            break;
          case MOD_256QAM:
            cell_size = 8100;
            break;
          default:
            cell_size = 0;
            break;
        }
      }
      else {
        switch (constellation) {
          case MOD_QPSK:
            cell_size = 8100;
            break;
          case MOD_16QAM:
            cell_size = 4050;
            break;
          case MOD_64QAM:
            cell_size = 2700;
            break;
          case MOD_256QAM:
            cell_size = 2025;
            break;
          default:
            cell_size = 0;
            break;
        }
      }
      switch (constellation) {
        case MOD_QPSK:
          normalization = std::sqrt(2.0);
          m_qpsk[0] = gr_complex( 1.0 / normalization,  1.0 / normalization);
          m_qpsk[1] = gr_complex( 1.0 / normalization, -1.0 / normalization);
          m_qpsk[2] = gr_complex(-1.0 / normalization,  1.0 / normalization);
          m_qpsk[3] = gr_complex(-1.0 / normalization, -1.0 / normalization);
          if (rotation == ROTATION_ON) {
            cyclic_delay = TRUE;
            rotation_angle = (2.0 * M_PI * 29.0) / 360.0;
            temp = std::exp(gr_complexd(0.0, rotation_angle));
            for (int i = 0; i < 4; i++) {
              m_qpsk[i] *= temp;
            }
          }
          break;
        case MOD_16QAM:
          normalization = std::sqrt(10.0);
          m_16qam[0] = gr_complex( 3.0 / normalization,  3.0 / normalization);
          m_16qam[1] = gr_complex( 3.0 / normalization,  1.0 / normalization);
          m_16qam[2] = gr_complex( 1.0 / normalization,  3.0 / normalization);
          m_16qam[3] = gr_complex( 1.0 / normalization,  1.0 / normalization);
          m_16qam[4] = gr_complex( 3.0 / normalization, -3.0 / normalization);
          m_16qam[5] = gr_complex( 3.0 / normalization, -1.0 / normalization);
          m_16qam[6] = gr_complex( 1.0 / normalization, -3.0 / normalization);
          m_16qam[7] = gr_complex( 1.0 / normalization, -1.0 / normalization);
          m_16qam[8] = gr_complex(-3.0 / normalization,  3.0 / normalization);
          m_16qam[9] = gr_complex(-3.0 / normalization,  1.0 / normalization);
          m_16qam[10] = gr_complex(-1.0 / normalization,  3.0 / normalization);
          m_16qam[11] = gr_complex(-1.0 / normalization,  1.0 / normalization);
          m_16qam[12] = gr_complex(-3.0 / normalization, -3.0 / normalization);
          m_16qam[13] = gr_complex(-3.0 / normalization, -1.0 / normalization);
          m_16qam[14] = gr_complex(-1.0 / normalization, -3.0 / normalization);
          m_16qam[15] = gr_complex(-1.0 / normalization, -1.0 / normalization);
          if (rotation == ROTATION_ON) {
            cyclic_delay = TRUE;
            rotation_angle = (2.0 * M_PI * 16.8) / 360.0;
            temp = std::exp(gr_complexd(0.0, rotation_angle));
            for (int i = 0; i < 16; i++) {
              m_16qam[i] *= temp;
            }
          }
          break;
        case MOD_64QAM:
          normalization = std::sqrt(42.0);
          m_64qam[0] = gr_complex(  7.0 / normalization,   7.0 / normalization);
          m_64qam[1] = gr_complex(  7.0 / normalization,   5.0 / normalization);
          m_64qam[2] = gr_complex(  5.0 / normalization,   7.0 / normalization);
          m_64qam[3] = gr_complex(  5.0 / normalization,   5.0 / normalization);
          m_64qam[4] = gr_complex(  7.0 / normalization,   1.0 / normalization);
          m_64qam[5] = gr_complex(  7.0 / normalization,   3.0 / normalization);
          m_64qam[6] = gr_complex(  5.0 / normalization,   1.0 / normalization);
          m_64qam[7] = gr_complex(  5.0 / normalization,   3.0 / normalization);
          m_64qam[8] = gr_complex(  1.0 / normalization,   7.0 / normalization);
          m_64qam[9] = gr_complex(  1.0 / normalization,   5.0 / normalization);
          m_64qam[10] = gr_complex( 3.0 / normalization,  7.0 / normalization);
          m_64qam[11] = gr_complex( 3.0 / normalization,  5.0 / normalization);
          m_64qam[12] = gr_complex( 1.0 / normalization,  1.0 / normalization);
          m_64qam[13] = gr_complex( 1.0 / normalization,  3.0 / normalization);
          m_64qam[14] = gr_complex( 3.0 / normalization,  1.0 / normalization);
          m_64qam[15] = gr_complex( 3.0 / normalization,  3.0 / normalization);
          m_64qam[16] = gr_complex( 7.0 / normalization, -7.0 / normalization);
          m_64qam[17] = gr_complex( 7.0 / normalization, -5.0 / normalization);
          m_64qam[18] = gr_complex( 5.0 / normalization, -7.0 / normalization);
          m_64qam[19] = gr_complex( 5.0 / normalization, -5.0 / normalization);
          m_64qam[20] = gr_complex( 7.0 / normalization, -1.0 / normalization);
          m_64qam[21] = gr_complex( 7.0 / normalization, -3.0 / normalization);
          m_64qam[22] = gr_complex( 5.0 / normalization, -1.0 / normalization);
          m_64qam[23] = gr_complex( 5.0 / normalization, -3.0 / normalization);
          m_64qam[24] = gr_complex( 1.0 / normalization, -7.0 / normalization);
          m_64qam[25] = gr_complex( 1.0 / normalization, -5.0 / normalization);
          m_64qam[26] = gr_complex( 3.0 / normalization, -7.0 / normalization);
          m_64qam[27] = gr_complex( 3.0 / normalization, -5.0 / normalization);
          m_64qam[28] = gr_complex( 1.0 / normalization, -1.0 / normalization);
          m_64qam[29] = gr_complex( 1.0 / normalization, -3.0 / normalization);
          m_64qam[30] = gr_complex( 3.0 / normalization, -1.0 / normalization);
          m_64qam[31] = gr_complex( 3.0 / normalization, -3.0 / normalization);
          m_64qam[32] = gr_complex(-7.0 / normalization,  7.0 / normalization);
          m_64qam[33] = gr_complex(-7.0 / normalization,  5.0 / normalization);
          m_64qam[34] = gr_complex(-5.0 / normalization,  7.0 / normalization);
          m_64qam[35] = gr_complex(-5.0 / normalization,  5.0 / normalization);
          m_64qam[36] = gr_complex(-7.0 / normalization,  1.0 / normalization);
          m_64qam[37] = gr_complex(-7.0 / normalization,  3.0 / normalization);
          m_64qam[38] = gr_complex(-5.0 / normalization,  1.0 / normalization);
          m_64qam[39] = gr_complex(-5.0 / normalization,  3.0 / normalization);
          m_64qam[40] = gr_complex(-1.0 / normalization,  7.0 / normalization);
          m_64qam[41] = gr_complex(-1.0 / normalization,  5.0 / normalization);
          m_64qam[42] = gr_complex(-3.0 / normalization,  7.0 / normalization);
          m_64qam[43] = gr_complex(-3.0 / normalization,  5.0 / normalization);
          m_64qam[44] = gr_complex(-1.0 / normalization,  1.0 / normalization);
          m_64qam[45] = gr_complex(-1.0 / normalization,  3.0 / normalization);
          m_64qam[46] = gr_complex(-3.0 / normalization,  1.0 / normalization);
          m_64qam[47] = gr_complex(-3.0 / normalization,  3.0 / normalization);
          m_64qam[48] = gr_complex(-7.0 / normalization, -7.0 / normalization);
          m_64qam[49] = gr_complex(-7.0 / normalization, -5.0 / normalization);
          m_64qam[50] = gr_complex(-5.0 / normalization, -7.0 / normalization);
          m_64qam[51] = gr_complex(-5.0 / normalization, -5.0 / normalization);
          m_64qam[52] = gr_complex(-7.0 / normalization, -1.0 / normalization);
          m_64qam[53] = gr_complex(-7.0 / normalization, -3.0 / normalization);
          m_64qam[54] = gr_complex(-5.0 / normalization, -1.0 / normalization);
          m_64qam[55] = gr_complex(-5.0 / normalization, -3.0 / normalization);
          m_64qam[56] = gr_complex(-1.0 / normalization, -7.0 / normalization);
          m_64qam[57] = gr_complex(-1.0 / normalization, -5.0 / normalization);
          m_64qam[58] = gr_complex(-3.0 / normalization, -7.0 / normalization);
          m_64qam[59] = gr_complex(-3.0 / normalization, -5.0 / normalization);
          m_64qam[60] = gr_complex(-1.0 / normalization, -1.0 / normalization);
          m_64qam[61] = gr_complex(-1.0 / normalization, -3.0 / normalization);
          m_64qam[62] = gr_complex(-3.0 / normalization, -1.0 / normalization);
          m_64qam[63] = gr_complex(-3.0 / normalization, -3.0 / normalization);
          if (rotation == ROTATION_ON) {
            cyclic_delay = TRUE;
            rotation_angle = (2.0 * M_PI * 8.6) / 360.0;
            temp = std::exp(gr_complexd(0.0, rotation_angle));
            for (int i = 0; i < 64; i++) {
              m_64qam[i] *= temp;
            }
          }
          break;
        case MOD_256QAM:
          normalization = std::sqrt(170.0);
          m_256qam[0] = gr_complex( 15 / normalization,  15 / normalization);
          m_256qam[1] = gr_complex( 15 / normalization,  13 / normalization);
          m_256qam[2] = gr_complex( 13 / normalization,  15 / normalization);
          m_256qam[3] = gr_complex( 13 / normalization,  13 / normalization);
          m_256qam[4] = gr_complex( 15 / normalization,   9 / normalization);
          m_256qam[5] = gr_complex( 15 / normalization,  11 / normalization);
          m_256qam[6] = gr_complex( 13 / normalization,   9 / normalization);
          m_256qam[7] = gr_complex( 13 / normalization,  11 / normalization);
          m_256qam[8] = gr_complex(  9 / normalization,  15 / normalization);
          m_256qam[9] = gr_complex(  9 / normalization,  13 / normalization);
          m_256qam[10] = gr_complex( 11 / normalization,  15 / normalization);
          m_256qam[11] = gr_complex( 11 / normalization,  13 / normalization);
          m_256qam[12] = gr_complex(  9 / normalization,   9 / normalization);
          m_256qam[13] = gr_complex(  9 / normalization,  11 / normalization);
          m_256qam[14] = gr_complex( 11 / normalization,   9 / normalization);
          m_256qam[15] = gr_complex( 11 / normalization,  11 / normalization);
          m_256qam[16] = gr_complex( 15 / normalization,   1 / normalization);
          m_256qam[17] = gr_complex( 15 / normalization,   3 / normalization);
          m_256qam[18] = gr_complex( 13 / normalization,   1 / normalization);
          m_256qam[19] = gr_complex( 13 / normalization,   3 / normalization);
          m_256qam[20] = gr_complex( 15 / normalization,   7 / normalization);
          m_256qam[21] = gr_complex( 15 / normalization,   5 / normalization);
          m_256qam[22] = gr_complex( 13 / normalization,   7 / normalization);
          m_256qam[23] = gr_complex( 13 / normalization,   5 / normalization);
          m_256qam[24] = gr_complex(  9 / normalization,   1 / normalization);
          m_256qam[25] = gr_complex(  9 / normalization,   3 / normalization);
          m_256qam[26] = gr_complex( 11 / normalization,   1 / normalization);
          m_256qam[27] = gr_complex( 11 / normalization,   3 / normalization);
          m_256qam[28] = gr_complex(  9 / normalization,   7 / normalization);
          m_256qam[29] = gr_complex(  9 / normalization,   5 / normalization);
          m_256qam[30] = gr_complex( 11 / normalization,   7 / normalization);
          m_256qam[31] = gr_complex( 11 / normalization,   5 / normalization);
          m_256qam[32] = gr_complex(  1 / normalization,  15 / normalization);
          m_256qam[33] = gr_complex(  1 / normalization,  13 / normalization);
          m_256qam[34] = gr_complex(  3 / normalization,  15 / normalization);
          m_256qam[35] = gr_complex(  3 / normalization,  13 / normalization);
          m_256qam[36] = gr_complex(  1 / normalization,   9 / normalization);
          m_256qam[37] = gr_complex(  1 / normalization,  11 / normalization);
          m_256qam[38] = gr_complex(  3 / normalization,   9 / normalization);
          m_256qam[39] = gr_complex(  3 / normalization,  11 / normalization);
          m_256qam[40] = gr_complex(  7 / normalization,  15 / normalization);
          m_256qam[41] = gr_complex(  7 / normalization,  13 / normalization);
          m_256qam[42] = gr_complex(  5 / normalization,  15 / normalization);
          m_256qam[43] = gr_complex(  5 / normalization,  13 / normalization);
          m_256qam[44] = gr_complex(  7 / normalization,   9 / normalization);
          m_256qam[45] = gr_complex(  7 / normalization,  11 / normalization);
          m_256qam[46] = gr_complex(  5 / normalization,   9 / normalization);
          m_256qam[47] = gr_complex(  5 / normalization,  11 / normalization);
          m_256qam[48] = gr_complex(  1 / normalization,   1 / normalization);
          m_256qam[49] = gr_complex(  1 / normalization,   3 / normalization);
          m_256qam[50] = gr_complex(  3 / normalization,   1 / normalization);
          m_256qam[51] = gr_complex(  3 / normalization,   3 / normalization);
          m_256qam[52] = gr_complex(  1 / normalization,   7 / normalization);
          m_256qam[53] = gr_complex(  1 / normalization,   5 / normalization);
          m_256qam[54] = gr_complex(  3 / normalization,   7 / normalization);
          m_256qam[55] = gr_complex(  3 / normalization,   5 / normalization);
          m_256qam[56] = gr_complex(  7 / normalization,   1 / normalization);
          m_256qam[57] = gr_complex(  7 / normalization,   3 / normalization);
          m_256qam[58] = gr_complex(  5 / normalization,   1 / normalization);
          m_256qam[59] = gr_complex(  5 / normalization,   3 / normalization);
          m_256qam[60] = gr_complex(  7 / normalization,   7 / normalization);
          m_256qam[61] = gr_complex(  7 / normalization,   5 / normalization);
          m_256qam[62] = gr_complex(  5 / normalization,   7 / normalization);
          m_256qam[63] = gr_complex(  5 / normalization,   5 / normalization);
          m_256qam[64] = gr_complex( 15 / normalization, -15 / normalization);
          m_256qam[65] = gr_complex( 15 / normalization, -13 / normalization);
          m_256qam[66] = gr_complex( 13 / normalization, -15 / normalization);
          m_256qam[67] = gr_complex( 13 / normalization, -13 / normalization);
          m_256qam[68] = gr_complex( 15 / normalization,  -9 / normalization);
          m_256qam[69] = gr_complex( 15 / normalization, -11 / normalization);
          m_256qam[70] = gr_complex( 13 / normalization,  -9 / normalization);
          m_256qam[71] = gr_complex( 13 / normalization, -11 / normalization);
          m_256qam[72] = gr_complex(  9 / normalization, -15 / normalization);
          m_256qam[73] = gr_complex(  9 / normalization, -13 / normalization);
          m_256qam[74] = gr_complex( 11 / normalization, -15 / normalization);
          m_256qam[75] = gr_complex( 11 / normalization, -13 / normalization);
          m_256qam[76] = gr_complex(  9 / normalization,  -9 / normalization);
          m_256qam[77] = gr_complex(  9 / normalization, -11 / normalization);
          m_256qam[78] = gr_complex( 11 / normalization,  -9 / normalization);
          m_256qam[79] = gr_complex( 11 / normalization, -11 / normalization);
          m_256qam[80] = gr_complex( 15 / normalization,  -1 / normalization);
          m_256qam[81] = gr_complex( 15 / normalization,  -3 / normalization);
          m_256qam[82] = gr_complex( 13 / normalization,  -1 / normalization);
          m_256qam[83] = gr_complex( 13 / normalization,  -3 / normalization);
          m_256qam[84] = gr_complex( 15 / normalization,  -7 / normalization);
          m_256qam[85] = gr_complex( 15 / normalization,  -5 / normalization);
          m_256qam[86] = gr_complex( 13 / normalization,  -7 / normalization);
          m_256qam[87] = gr_complex( 13 / normalization,  -5 / normalization);
          m_256qam[88] = gr_complex(  9 / normalization,  -1 / normalization);
          m_256qam[89] = gr_complex(  9 / normalization,  -3 / normalization);
          m_256qam[90] = gr_complex( 11 / normalization,  -1 / normalization);
          m_256qam[91] = gr_complex( 11 / normalization,  -3 / normalization);
          m_256qam[92] = gr_complex(  9 / normalization,  -7 / normalization);
          m_256qam[93] = gr_complex(  9 / normalization,  -5 / normalization);
          m_256qam[94] = gr_complex( 11 / normalization,  -7 / normalization);
          m_256qam[95] = gr_complex( 11 / normalization,  -5 / normalization);
          m_256qam[96] = gr_complex(  1 / normalization, -15 / normalization);
          m_256qam[97] = gr_complex(  1 / normalization, -13 / normalization);
          m_256qam[98] = gr_complex(  3 / normalization, -15 / normalization);
          m_256qam[99] = gr_complex(  3 / normalization, -13 / normalization);
          m_256qam[100] = gr_complex(  1 / normalization,  -9 / normalization);
          m_256qam[101] = gr_complex(  1 / normalization, -11 / normalization);
          m_256qam[102] = gr_complex(  3 / normalization,  -9 / normalization);
          m_256qam[103] = gr_complex(  3 / normalization, -11 / normalization);
          m_256qam[104] = gr_complex(  7 / normalization, -15 / normalization);
          m_256qam[105] = gr_complex(  7 / normalization, -13 / normalization);
          m_256qam[106] = gr_complex(  5 / normalization, -15 / normalization);
          m_256qam[107] = gr_complex(  5 / normalization, -13 / normalization);
          m_256qam[108] = gr_complex(  7 / normalization,  -9 / normalization);
          m_256qam[109] = gr_complex(  7 / normalization, -11 / normalization);
          m_256qam[110] = gr_complex(  5 / normalization,  -9 / normalization);
          m_256qam[111] = gr_complex(  5 / normalization, -11 / normalization);
          m_256qam[112] = gr_complex(  1 / normalization,  -1 / normalization);
          m_256qam[113] = gr_complex(  1 / normalization,  -3 / normalization);
          m_256qam[114] = gr_complex(  3 / normalization,  -1 / normalization);
          m_256qam[115] = gr_complex(  3 / normalization,  -3 / normalization);
          m_256qam[116] = gr_complex(  1 / normalization,  -7 / normalization);
          m_256qam[117] = gr_complex(  1 / normalization,  -5 / normalization);
          m_256qam[118] = gr_complex(  3 / normalization,  -7 / normalization);
          m_256qam[119] = gr_complex(  3 / normalization,  -5 / normalization);
          m_256qam[120] = gr_complex(  7 / normalization,  -1 / normalization);
          m_256qam[121] = gr_complex(  7 / normalization,  -3 / normalization);
          m_256qam[122] = gr_complex(  5 / normalization,  -1 / normalization);
          m_256qam[123] = gr_complex(  5 / normalization,  -3 / normalization);
          m_256qam[124] = gr_complex(  7 / normalization,  -7 / normalization);
          m_256qam[125] = gr_complex(  7 / normalization,  -5 / normalization);
          m_256qam[126] = gr_complex(  5 / normalization,  -7 / normalization);
          m_256qam[127] = gr_complex(  5 / normalization,  -5 / normalization);
          m_256qam[128] = gr_complex(-15 / normalization,  15 / normalization);
          m_256qam[129] = gr_complex(-15 / normalization,  13 / normalization);
          m_256qam[130] = gr_complex(-13 / normalization,  15 / normalization);
          m_256qam[131] = gr_complex(-13 / normalization,  13 / normalization);
          m_256qam[132] = gr_complex(-15 / normalization,   9 / normalization);
          m_256qam[133] = gr_complex(-15 / normalization,  11 / normalization);
          m_256qam[134] = gr_complex(-13 / normalization,   9 / normalization);
          m_256qam[135] = gr_complex(-13 / normalization,  11 / normalization);
          m_256qam[136] = gr_complex( -9 / normalization,  15 / normalization);
          m_256qam[137] = gr_complex( -9 / normalization,  13 / normalization);
          m_256qam[138] = gr_complex(-11 / normalization,  15 / normalization);
          m_256qam[139] = gr_complex(-11 / normalization,  13 / normalization);
          m_256qam[140] = gr_complex( -9 / normalization,   9 / normalization);
          m_256qam[141] = gr_complex( -9 / normalization,  11 / normalization);
          m_256qam[142] = gr_complex(-11 / normalization,   9 / normalization);
          m_256qam[143] = gr_complex(-11 / normalization,  11 / normalization);
          m_256qam[144] = gr_complex(-15 / normalization,   1 / normalization);
          m_256qam[145] = gr_complex(-15 / normalization,   3 / normalization);
          m_256qam[146] = gr_complex(-13 / normalization,   1 / normalization);
          m_256qam[147] = gr_complex(-13 / normalization,   3 / normalization);
          m_256qam[148] = gr_complex(-15 / normalization,   7 / normalization);
          m_256qam[149] = gr_complex(-15 / normalization,   5 / normalization);
          m_256qam[150] = gr_complex(-13 / normalization,   7 / normalization);
          m_256qam[151] = gr_complex(-13 / normalization,   5 / normalization);
          m_256qam[152] = gr_complex( -9 / normalization,   1 / normalization);
          m_256qam[153] = gr_complex( -9 / normalization,   3 / normalization);
          m_256qam[154] = gr_complex(-11 / normalization,   1 / normalization);
          m_256qam[155] = gr_complex(-11 / normalization,   3 / normalization);
          m_256qam[156] = gr_complex( -9 / normalization,   7 / normalization);
          m_256qam[157] = gr_complex( -9 / normalization,   5 / normalization);
          m_256qam[158] = gr_complex(-11 / normalization,   7 / normalization);
          m_256qam[159] = gr_complex(-11 / normalization,   5 / normalization);
          m_256qam[160] = gr_complex( -1 / normalization,  15 / normalization);
          m_256qam[161] = gr_complex( -1 / normalization,  13 / normalization);
          m_256qam[162] = gr_complex( -3 / normalization,  15 / normalization);
          m_256qam[163] = gr_complex( -3 / normalization,  13 / normalization);
          m_256qam[164] = gr_complex( -1 / normalization,   9 / normalization);
          m_256qam[165] = gr_complex( -1 / normalization,  11 / normalization);
          m_256qam[166] = gr_complex( -3 / normalization,   9 / normalization);
          m_256qam[167] = gr_complex( -3 / normalization,  11 / normalization);
          m_256qam[168] = gr_complex( -7 / normalization,  15 / normalization);
          m_256qam[169] = gr_complex( -7 / normalization,  13 / normalization);
          m_256qam[170] = gr_complex( -5 / normalization,  15 / normalization);
          m_256qam[171] = gr_complex( -5 / normalization,  13 / normalization);
          m_256qam[172] = gr_complex( -7 / normalization,   9 / normalization);
          m_256qam[173] = gr_complex( -7 / normalization,  11 / normalization);
          m_256qam[174] = gr_complex( -5 / normalization,   9 / normalization);
          m_256qam[175] = gr_complex( -5 / normalization,  11 / normalization);
          m_256qam[176] = gr_complex( -1 / normalization,   1 / normalization);
          m_256qam[177] = gr_complex( -1 / normalization,   3 / normalization);
          m_256qam[178] = gr_complex( -3 / normalization,   1 / normalization);
          m_256qam[179] = gr_complex( -3 / normalization,   3 / normalization);
          m_256qam[180] = gr_complex( -1 / normalization,   7 / normalization);
          m_256qam[181] = gr_complex( -1 / normalization,   5 / normalization);
          m_256qam[182] = gr_complex( -3 / normalization,   7 / normalization);
          m_256qam[183] = gr_complex( -3 / normalization,   5 / normalization);
          m_256qam[184] = gr_complex( -7 / normalization,   1 / normalization);
          m_256qam[185] = gr_complex( -7 / normalization,   3 / normalization);
          m_256qam[186] = gr_complex( -5 / normalization,   1 / normalization);
          m_256qam[187] = gr_complex( -5 / normalization,   3 / normalization);
          m_256qam[188] = gr_complex( -7 / normalization,   7 / normalization);
          m_256qam[189] = gr_complex( -7 / normalization,   5 / normalization);
          m_256qam[190] = gr_complex( -5 / normalization,   7 / normalization);
          m_256qam[191] = gr_complex( -5 / normalization,   5 / normalization);
          m_256qam[192] = gr_complex(-15 / normalization, -15 / normalization);
          m_256qam[193] = gr_complex(-15 / normalization, -13 / normalization);
          m_256qam[194] = gr_complex(-13 / normalization, -15 / normalization);
          m_256qam[195] = gr_complex(-13 / normalization, -13 / normalization);
          m_256qam[196] = gr_complex(-15 / normalization,  -9 / normalization);
          m_256qam[197] = gr_complex(-15 / normalization, -11 / normalization);
          m_256qam[198] = gr_complex(-13 / normalization,  -9 / normalization);
          m_256qam[199] = gr_complex(-13 / normalization, -11 / normalization);
          m_256qam[200] = gr_complex( -9 / normalization, -15 / normalization);
          m_256qam[201] = gr_complex( -9 / normalization, -13 / normalization);
          m_256qam[202] = gr_complex(-11 / normalization, -15 / normalization);
          m_256qam[203] = gr_complex(-11 / normalization, -13 / normalization);
          m_256qam[204] = gr_complex( -9 / normalization,  -9 / normalization);
          m_256qam[205] = gr_complex( -9 / normalization, -11 / normalization);
          m_256qam[206] = gr_complex(-11 / normalization,  -9 / normalization);
          m_256qam[207] = gr_complex(-11 / normalization, -11 / normalization);
          m_256qam[208] = gr_complex(-15 / normalization,  -1 / normalization);
          m_256qam[209] = gr_complex(-15 / normalization,  -3 / normalization);
          m_256qam[210] = gr_complex(-13 / normalization,  -1 / normalization);
          m_256qam[211] = gr_complex(-13 / normalization,  -3 / normalization);
          m_256qam[212] = gr_complex(-15 / normalization,  -7 / normalization);
          m_256qam[213] = gr_complex(-15 / normalization,  -5 / normalization);
          m_256qam[214] = gr_complex(-13 / normalization,  -7 / normalization);
          m_256qam[215] = gr_complex(-13 / normalization,  -5 / normalization);
          m_256qam[216] = gr_complex( -9 / normalization,  -1 / normalization);
          m_256qam[217] = gr_complex( -9 / normalization,  -3 / normalization);
          m_256qam[218] = gr_complex(-11 / normalization,  -1 / normalization);
          m_256qam[219] = gr_complex(-11 / normalization,  -3 / normalization);
          m_256qam[220] = gr_complex( -9 / normalization,  -7 / normalization);
          m_256qam[221] = gr_complex( -9 / normalization,  -5 / normalization);
          m_256qam[222] = gr_complex(-11 / normalization,  -7 / normalization);
          m_256qam[223] = gr_complex(-11 / normalization,  -5 / normalization);
          m_256qam[224] = gr_complex( -1 / normalization, -15 / normalization);
          m_256qam[225] = gr_complex( -1 / normalization, -13 / normalization);
          m_256qam[226] = gr_complex( -3 / normalization, -15 / normalization);
          m_256qam[227] = gr_complex( -3 / normalization, -13 / normalization);
          m_256qam[228] = gr_complex( -1 / normalization,  -9 / normalization);
          m_256qam[229] = gr_complex( -1 / normalization, -11 / normalization);
          m_256qam[230] = gr_complex( -3 / normalization,  -9 / normalization);
          m_256qam[231] = gr_complex( -3 / normalization, -11 / normalization);
          m_256qam[232] = gr_complex( -7 / normalization, -15 / normalization);
          m_256qam[233] = gr_complex( -7 / normalization, -13 / normalization);
          m_256qam[234] = gr_complex( -5 / normalization, -15 / normalization);
          m_256qam[235] = gr_complex( -5 / normalization, -13 / normalization);
          m_256qam[236] = gr_complex( -7 / normalization,  -9 / normalization);
          m_256qam[237] = gr_complex( -7 / normalization, -11 / normalization);
          m_256qam[238] = gr_complex( -5 / normalization,  -9 / normalization);
          m_256qam[239] = gr_complex( -5 / normalization, -11 / normalization);
          m_256qam[240] = gr_complex( -1 / normalization,  -1 / normalization);
          m_256qam[241] = gr_complex( -1 / normalization,  -3 / normalization);
          m_256qam[242] = gr_complex( -3 / normalization,  -1 / normalization);
          m_256qam[243] = gr_complex( -3 / normalization,  -3 / normalization);
          m_256qam[244] = gr_complex( -1 / normalization,  -7 / normalization);
          m_256qam[245] = gr_complex( -1 / normalization,  -5 / normalization);
          m_256qam[246] = gr_complex( -3 / normalization,  -7 / normalization);
          m_256qam[247] = gr_complex( -3 / normalization,  -5 / normalization);
          m_256qam[248] = gr_complex( -7 / normalization,  -1 / normalization);
          m_256qam[249] = gr_complex( -7 / normalization,  -3 / normalization);
          m_256qam[250] = gr_complex( -5 / normalization,  -1 / normalization);
          m_256qam[251] = gr_complex( -5 / normalization,  -3 / normalization);
          m_256qam[252] = gr_complex( -7 / normalization,  -7 / normalization);
          m_256qam[253] = gr_complex( -7 / normalization,  -5 / normalization);
          m_256qam[254] = gr_complex( -5 / normalization,  -7 / normalization);
          m_256qam[255] = gr_complex( -5 / normalization,  -5 / normalization);
          if (rotation == ROTATION_ON) {
            cyclic_delay = TRUE;
            rotation_angle = (2.0 * M_PI * 3.576334375) / 360.0;
            temp = std::exp(gr_complexd(0.0, rotation_angle));
            for (int i = 0; i < 256; i++) {
              m_256qam[i] *= temp;
            }
          }
          break;
        default:
          normalization = std::sqrt(2.0);
          m_qpsk[0] = gr_complex( 1.0 / normalization,  1.0 / normalization);
          m_qpsk[1] = gr_complex( 1.0 / normalization, -1.0 / normalization);
          m_qpsk[2] = gr_complex(-1.0 / normalization,  1.0 / normalization);
          m_qpsk[3] = gr_complex(-1.0 / normalization, -1.0 / normalization);
          if (rotation == ROTATION_ON) {
            cyclic_delay = TRUE;
            rotation_angle = (2.0 * M_PI * 29.0) / 360.0;
            temp = std::exp(gr_complexd(0.0, rotation_angle));
            for (int i = 0; i < 4; i++) {
              m_qpsk[i] *= temp;
            }
          }
          break;
      }
      signal_constellation = constellation;
      set_output_multiple(cell_size);
    }

    /*
     * Our virtual destructor.
     */
    dvbt2_modulator_bc_impl::~dvbt2_modulator_bc_impl()
    {
    }

    void
    dvbt2_modulator_bc_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        ninput_items_required[0] = noutput_items;
    }

    int
    dvbt2_modulator_bc_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      const unsigned char *in_delay;
      int index, index_delay;

      switch (signal_constellation) {
        case MOD_QPSK:
          for (int i = 0; i < noutput_items; i += cell_size) {
            if (cyclic_delay == FALSE) {
              for (int j = 0; j < cell_size; j++) {
                index = *in++;
                *out++ = m_qpsk[index & 0x3];
              }
            }
            else {
              in_delay = in;
              for (int j = 0; j < cell_size; j++) {
                index = *in++;
                index_delay = in_delay[(j + cell_size - 1) % cell_size];
                *out++ = gr_complex(m_qpsk[index & 0x3].real(),
                                    m_qpsk[index_delay & 0x3].imag());
              }
            }
          }
          break;
        case MOD_16QAM:
          for (int i = 0; i < noutput_items; i += cell_size) {
            if (cyclic_delay == FALSE) {
              for (int j = 0; j < cell_size; j++) {
                index = *in++;
                *out++ = m_16qam[index & 0xf];
              }
            }
            else {
              in_delay = in;
              for (int j = 0; j < cell_size; j++) {
                index = *in++;
                index_delay = in_delay[(j + cell_size - 1) % cell_size];
                *out++ = gr_complex(m_16qam[index & 0xf].real(),
                                    m_16qam[index_delay & 0xf].imag());
              }
            }
          }
          break;
        case MOD_64QAM:
          for (int i = 0; i < noutput_items; i += cell_size) {
            if (cyclic_delay == FALSE) {
              for (int j = 0; j < noutput_items; j++) {
                index = *in++;
                *out++ = m_64qam[index & 0x3f];
              }
            }
            else {
              in_delay = in;
              for (int j = 0; j < cell_size; j++) {
                index = *in++;
                index_delay = in_delay[(j + cell_size - 1) % cell_size];
                *out++ = gr_complex(m_64qam[index & 0x3f].real(),
                                    m_64qam[index_delay & 0x3f].imag());
              }
            }
          }
          break;
        case MOD_256QAM:
          for (int i = 0; i < noutput_items; i += cell_size) {
            if (cyclic_delay == FALSE) {
              for (int j = 0; j < cell_size; j++) {
                index = *in++;
                *out++ = m_256qam[index & 0xff];
              }
            }
            else {
              in_delay = in;
              for (int j = 0; j < cell_size; j++) {
                index = *in++;
                index_delay = in_delay[(j + cell_size - 1) % cell_size];
                *out++ = gr_complex(m_256qam[index & 0xff].real(),
                                    m_256qam[index_delay & 0xff].imag());
              }
            }
          }
          break;
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */

