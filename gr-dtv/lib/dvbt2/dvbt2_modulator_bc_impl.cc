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
      gr_complex m_temp[1];
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
          normalization = sqrt(2);
          m_qpsk[0].real() =  1.0 / normalization;
          m_qpsk[0].imag() =  1.0 / normalization;
          m_qpsk[1].real() =  1.0 / normalization;
          m_qpsk[1].imag() = -1.0 / normalization;
          m_qpsk[2].real() = -1.0 / normalization;
          m_qpsk[2].imag() =  1.0 / normalization;
          m_qpsk[3].real() = -1.0 / normalization;
          m_qpsk[3].imag() = -1.0 / normalization;
          if (rotation == ROTATION_ON) {
            cyclic_delay = TRUE;
            rotation_angle = (2.0 * M_PI * 29.0) / 360;
            for (int i = 0; i < 4; i++) {
              m_temp[0].real() = m_qpsk[i].real();
              m_qpsk[i].real() = (m_qpsk[i].real() * cos(rotation_angle)) - (m_qpsk[i].imag() * sin(rotation_angle));
              m_qpsk[i].imag() = (m_temp[0].real() * sin(rotation_angle)) + (m_qpsk[i].imag() * cos(rotation_angle));
            }
          }
          break;
        case MOD_16QAM:
          normalization = sqrt(10);
          m_16qam[0].real()  =  3.0 / normalization;
          m_16qam[0].imag()  =  3.0 / normalization;
          m_16qam[1].real()  =  3.0 / normalization;
          m_16qam[1].imag()  =  1.0 / normalization;
          m_16qam[2].real()  =  1.0 / normalization;
          m_16qam[2].imag()  =  3.0 / normalization;
          m_16qam[3].real()  =  1.0 / normalization;
          m_16qam[3].imag()  =  1.0 / normalization;
          m_16qam[4].real()  =  3.0 / normalization;
          m_16qam[4].imag()  = -3.0 / normalization;
          m_16qam[5].real()  =  3.0 / normalization;
          m_16qam[5].imag()  = -1.0 / normalization;
          m_16qam[6].real()  =  1.0 / normalization;
          m_16qam[6].imag()  = -3.0 / normalization;
          m_16qam[7].real()  =  1.0 / normalization;
          m_16qam[7].imag()  = -1.0 / normalization;
          m_16qam[8].real()  = -3.0 / normalization;
          m_16qam[8].imag()  =  3.0 / normalization;
          m_16qam[9].real()  = -3.0 / normalization;
          m_16qam[9].imag()  =  1.0 / normalization;
          m_16qam[10].real() = -1.0 / normalization;
          m_16qam[10].imag() =  3.0 / normalization;
          m_16qam[11].real() = -1.0 / normalization;
          m_16qam[11].imag() =  1.0 / normalization;
          m_16qam[12].real() = -3.0 / normalization;
          m_16qam[12].imag() = -3.0 / normalization;
          m_16qam[13].real() = -3.0 / normalization;
          m_16qam[13].imag() = -1.0 / normalization;
          m_16qam[14].real() = -1.0 / normalization;
          m_16qam[14].imag() = -3.0 / normalization;
          m_16qam[15].real() = -1.0 / normalization;
          m_16qam[15].imag() = -1.0 / normalization;
          if (rotation == ROTATION_ON) {
            cyclic_delay = TRUE;
            rotation_angle = (2.0 * M_PI * 16.8) / 360;
            for (int i = 0; i < 16; i++) {
              m_temp[0].real() = m_16qam[i].real();
              m_16qam[i].real() = (m_16qam[i].real() * cos(rotation_angle)) - (m_16qam[i].imag() * sin(rotation_angle));
              m_16qam[i].imag() = (m_temp[0].real() * sin(rotation_angle)) + (m_16qam[i].imag() * cos(rotation_angle));
            }
          }
          break;
        case MOD_64QAM:
          normalization = sqrt(42);
          m_64qam[0].real() =   7.0 / normalization;
          m_64qam[0].imag() =   7.0 / normalization;
          m_64qam[1].real() =   7.0 / normalization;
          m_64qam[1].imag() =   5.0 / normalization;
          m_64qam[2].real() =   5.0 / normalization;
          m_64qam[2].imag() =   7.0 / normalization;
          m_64qam[3].real() =   5.0 / normalization;
          m_64qam[3].imag() =   5.0 / normalization;
          m_64qam[4].real() =   7.0 / normalization;
          m_64qam[4].imag() =   1.0 / normalization;
          m_64qam[5].real() =   7.0 / normalization;
          m_64qam[5].imag() =   3.0 / normalization;
          m_64qam[6].real() =   5.0 / normalization;
          m_64qam[6].imag() =   1.0 / normalization;
          m_64qam[7].real() =   5.0 / normalization;
          m_64qam[7].imag() =   3.0 / normalization;
          m_64qam[8].real() =   1.0 / normalization;
          m_64qam[8].imag() =   7.0 / normalization;
          m_64qam[9].real() =   1.0 / normalization;
          m_64qam[9].imag() =   5.0 / normalization;
          m_64qam[10].real() =  3.0 / normalization;
          m_64qam[10].imag() =  7.0 / normalization;
          m_64qam[11].real() =  3.0 / normalization;
          m_64qam[11].imag() =  5.0 / normalization;
          m_64qam[12].real() =  1.0 / normalization;
          m_64qam[12].imag() =  1.0 / normalization;
          m_64qam[13].real() =  1.0 / normalization;
          m_64qam[13].imag() =  3.0 / normalization;
          m_64qam[14].real() =  3.0 / normalization;
          m_64qam[14].imag() =  1.0 / normalization;
          m_64qam[15].real() =  3.0 / normalization;
          m_64qam[15].imag() =  3.0 / normalization;
          m_64qam[16].real() =  7.0 / normalization;
          m_64qam[16].imag() = -7.0 / normalization;
          m_64qam[17].real() =  7.0 / normalization;
          m_64qam[17].imag() = -5.0 / normalization;
          m_64qam[18].real() =  5.0 / normalization;
          m_64qam[18].imag() = -7.0 / normalization;
          m_64qam[19].real() =  5.0 / normalization;
          m_64qam[19].imag() = -5.0 / normalization;
          m_64qam[20].real() =  7.0 / normalization;
          m_64qam[20].imag() = -1.0 / normalization;
          m_64qam[21].real() =  7.0 / normalization;
          m_64qam[21].imag() = -3.0 / normalization;
          m_64qam[22].real() =  5.0 / normalization;
          m_64qam[22].imag() = -1.0 / normalization;
          m_64qam[23].real() =  5.0 / normalization;
          m_64qam[23].imag() = -3.0 / normalization;
          m_64qam[24].real() =  1.0 / normalization;
          m_64qam[24].imag() = -7.0 / normalization;
          m_64qam[25].real() =  1.0 / normalization;
          m_64qam[25].imag() = -5.0 / normalization;
          m_64qam[26].real() =  3.0 / normalization;
          m_64qam[26].imag() = -7.0 / normalization;
          m_64qam[27].real() =  3.0 / normalization;
          m_64qam[27].imag() = -5.0 / normalization;
          m_64qam[28].real() =  1.0 / normalization;
          m_64qam[28].imag() = -1.0 / normalization;
          m_64qam[29].real() =  1.0 / normalization;
          m_64qam[29].imag() = -3.0 / normalization;
          m_64qam[30].real() =  3.0 / normalization;
          m_64qam[30].imag() = -1.0 / normalization;
          m_64qam[31].real() =  3.0 / normalization;
          m_64qam[31].imag() = -3.0 / normalization;
          m_64qam[32].real() = -7.0 / normalization;
          m_64qam[32].imag() =  7.0 / normalization;
          m_64qam[33].real() = -7.0 / normalization;
          m_64qam[33].imag() =  5.0 / normalization;
          m_64qam[34].real() = -5.0 / normalization;
          m_64qam[34].imag() =  7.0 / normalization;
          m_64qam[35].real() = -5.0 / normalization;
          m_64qam[35].imag() =  5.0 / normalization;
          m_64qam[36].real() = -7.0 / normalization;
          m_64qam[36].imag() =  1.0 / normalization;
          m_64qam[37].real() = -7.0 / normalization;
          m_64qam[37].imag() =  3.0 / normalization;
          m_64qam[38].real() = -5.0 / normalization;
          m_64qam[38].imag() =  1.0 / normalization;
          m_64qam[39].real() = -5.0 / normalization;
          m_64qam[39].imag() =  3.0 / normalization;
          m_64qam[40].real() = -1.0 / normalization;
          m_64qam[40].imag() =  7.0 / normalization;
          m_64qam[41].real() = -1.0 / normalization;
          m_64qam[41].imag() =  5.0 / normalization;
          m_64qam[42].real() = -3.0 / normalization;
          m_64qam[42].imag() =  7.0 / normalization;
          m_64qam[43].real() = -3.0 / normalization;
          m_64qam[43].imag() =  5.0 / normalization;
          m_64qam[44].real() = -1.0 / normalization;
          m_64qam[44].imag() =  1.0 / normalization;
          m_64qam[45].real() = -1.0 / normalization;
          m_64qam[45].imag() =  3.0 / normalization;
          m_64qam[46].real() = -3.0 / normalization;
          m_64qam[46].imag() =  1.0 / normalization;
          m_64qam[47].real() = -3.0 / normalization;
          m_64qam[47].imag() =  3.0 / normalization;
          m_64qam[48].real() = -7.0 / normalization;
          m_64qam[48].imag() = -7.0 / normalization;
          m_64qam[49].real() = -7.0 / normalization;
          m_64qam[49].imag() = -5.0 / normalization;
          m_64qam[50].real() = -5.0 / normalization;
          m_64qam[50].imag() = -7.0 / normalization;
          m_64qam[51].real() = -5.0 / normalization;
          m_64qam[51].imag() = -5.0 / normalization;
          m_64qam[52].real() = -7.0 / normalization;
          m_64qam[52].imag() = -1.0 / normalization;
          m_64qam[53].real() = -7.0 / normalization;
          m_64qam[53].imag() = -3.0 / normalization;
          m_64qam[54].real() = -5.0 / normalization;
          m_64qam[54].imag() = -1.0 / normalization;
          m_64qam[55].real() = -5.0 / normalization;
          m_64qam[55].imag() = -3.0 / normalization;
          m_64qam[56].real() = -1.0 / normalization;
          m_64qam[56].imag() = -7.0 / normalization;
          m_64qam[57].real() = -1.0 / normalization;
          m_64qam[57].imag() = -5.0 / normalization;
          m_64qam[58].real() = -3.0 / normalization;
          m_64qam[58].imag() = -7.0 / normalization;
          m_64qam[59].real() = -3.0 / normalization;
          m_64qam[59].imag() = -5.0 / normalization;
          m_64qam[60].real() = -1.0 / normalization;
          m_64qam[60].imag() = -1.0 / normalization;
          m_64qam[61].real() = -1.0 / normalization;
          m_64qam[61].imag() = -3.0 / normalization;
          m_64qam[62].real() = -3.0 / normalization;
          m_64qam[62].imag() = -1.0 / normalization;
          m_64qam[63].real() = -3.0 / normalization;
          m_64qam[63].imag() = -3.0 / normalization;
          if (rotation == ROTATION_ON) {
            cyclic_delay = TRUE;
            rotation_angle = (2.0 * M_PI * 8.6) / 360;
            for (int i = 0; i < 64; i++) {
              m_temp[0].real() = m_64qam[i].real();
              m_64qam[i].real() = (m_64qam[i].real() * cos(rotation_angle)) - (m_64qam[i].imag() * sin(rotation_angle));
              m_64qam[i].imag() = (m_temp[0].real() * sin(rotation_angle)) + (m_64qam[i].imag() * cos(rotation_angle));
            }
          }
          break;
        case MOD_256QAM:
          normalization = sqrt(170);
          m_256qam[0].real()   =  15 / normalization;
          m_256qam[0].imag()   =  15 / normalization;
          m_256qam[1].real()   =  15 / normalization;
          m_256qam[1].imag()   =  13 / normalization;
          m_256qam[2].real()   =  13 / normalization;
          m_256qam[2].imag()   =  15 / normalization;
          m_256qam[3].real()   =  13 / normalization;
          m_256qam[3].imag()   =  13 / normalization;
          m_256qam[4].real()   =  15 / normalization;
          m_256qam[4].imag()   =   9 / normalization;
          m_256qam[5].real()   =  15 / normalization;
          m_256qam[5].imag()   =  11 / normalization;
          m_256qam[6].real()   =  13 / normalization;
          m_256qam[6].imag()   =   9 / normalization;
          m_256qam[7].real()   =  13 / normalization;
          m_256qam[7].imag()   =  11 / normalization;
          m_256qam[8].real()   =   9 / normalization;
          m_256qam[8].imag()   =  15 / normalization;
          m_256qam[9].real()   =   9 / normalization;
          m_256qam[9].imag()   =  13 / normalization;
          m_256qam[10].real()  =  11 / normalization;
          m_256qam[10].imag()  =  15 / normalization;
          m_256qam[11].real()  =  11 / normalization;
          m_256qam[11].imag()  =  13 / normalization;
          m_256qam[12].real()  =   9 / normalization;
          m_256qam[12].imag()  =   9 / normalization;
          m_256qam[13].real()  =   9 / normalization;
          m_256qam[13].imag()  =  11 / normalization;
          m_256qam[14].real()  =  11 / normalization;
          m_256qam[14].imag()  =   9 / normalization;
          m_256qam[15].real()  =  11 / normalization;
          m_256qam[15].imag()  =  11 / normalization;
          m_256qam[16].real()  =  15 / normalization;
          m_256qam[16].imag()  =   1 / normalization;
          m_256qam[17].real()  =  15 / normalization;
          m_256qam[17].imag()  =   3 / normalization;
          m_256qam[18].real()  =  13 / normalization;
          m_256qam[18].imag()  =   1 / normalization;
          m_256qam[19].real()  =  13 / normalization;
          m_256qam[19].imag()  =   3 / normalization;
          m_256qam[20].real()  =  15 / normalization;
          m_256qam[20].imag()  =   7 / normalization;
          m_256qam[21].real()  =  15 / normalization;
          m_256qam[21].imag()  =   5 / normalization;
          m_256qam[22].real()  =  13 / normalization;
          m_256qam[22].imag()  =   7 / normalization;
          m_256qam[23].real()  =  13 / normalization;
          m_256qam[23].imag()  =   5 / normalization;
          m_256qam[24].real()  =   9 / normalization;
          m_256qam[24].imag()  =   1 / normalization;
          m_256qam[25].real()  =   9 / normalization;
          m_256qam[25].imag()  =   3 / normalization;
          m_256qam[26].real()  =  11 / normalization;
          m_256qam[26].imag()  =   1 / normalization;
          m_256qam[27].real()  =  11 / normalization;
          m_256qam[27].imag()  =   3 / normalization;
          m_256qam[28].real()  =   9 / normalization;
          m_256qam[28].imag()  =   7 / normalization;
          m_256qam[29].real()  =   9 / normalization;
          m_256qam[29].imag()  =   5 / normalization;
          m_256qam[30].real()  =  11 / normalization;
          m_256qam[30].imag()  =   7 / normalization;
          m_256qam[31].real()  =  11 / normalization;
          m_256qam[31].imag()  =   5 / normalization;
          m_256qam[32].real()  =   1 / normalization;
          m_256qam[32].imag()  =  15 / normalization;
          m_256qam[33].real()  =   1 / normalization;
          m_256qam[33].imag()  =  13 / normalization;
          m_256qam[34].real()  =   3 / normalization;
          m_256qam[34].imag()  =  15 / normalization;
          m_256qam[35].real()  =   3 / normalization;
          m_256qam[35].imag()  =  13 / normalization;
          m_256qam[36].real()  =   1 / normalization;
          m_256qam[36].imag()  =   9 / normalization;
          m_256qam[37].real()  =   1 / normalization;
          m_256qam[37].imag()  =  11 / normalization;
          m_256qam[38].real()  =   3 / normalization;
          m_256qam[38].imag()  =   9 / normalization;
          m_256qam[39].real()  =   3 / normalization;
          m_256qam[39].imag()  =  11 / normalization;
          m_256qam[40].real()  =   7 / normalization;
          m_256qam[40].imag()  =  15 / normalization;
          m_256qam[41].real()  =   7 / normalization;
          m_256qam[41].imag()  =  13 / normalization;
          m_256qam[42].real()  =   5 / normalization;
          m_256qam[42].imag()  =  15 / normalization;
          m_256qam[43].real()  =   5 / normalization;
          m_256qam[43].imag()  =  13 / normalization;
          m_256qam[44].real()  =   7 / normalization;
          m_256qam[44].imag()  =   9 / normalization;
          m_256qam[45].real()  =   7 / normalization;
          m_256qam[45].imag()  =  11 / normalization;
          m_256qam[46].real()  =   5 / normalization;
          m_256qam[46].imag()  =   9 / normalization;
          m_256qam[47].real()  =   5 / normalization;
          m_256qam[47].imag()  =  11 / normalization;
          m_256qam[48].real()  =   1 / normalization;
          m_256qam[48].imag()  =   1 / normalization;
          m_256qam[49].real()  =   1 / normalization;
          m_256qam[49].imag()  =   3 / normalization;
          m_256qam[50].real()  =   3 / normalization;
          m_256qam[50].imag()  =   1 / normalization;
          m_256qam[51].real()  =   3 / normalization;
          m_256qam[51].imag()  =   3 / normalization;
          m_256qam[52].real()  =   1 / normalization;
          m_256qam[52].imag()  =   7 / normalization;
          m_256qam[53].real()  =   1 / normalization;
          m_256qam[53].imag()  =   5 / normalization;
          m_256qam[54].real()  =   3 / normalization;
          m_256qam[54].imag()  =   7 / normalization;
          m_256qam[55].real()  =   3 / normalization;
          m_256qam[55].imag()  =   5 / normalization;
          m_256qam[56].real()  =   7 / normalization;
          m_256qam[56].imag()  =   1 / normalization;
          m_256qam[57].real()  =   7 / normalization;
          m_256qam[57].imag()  =   3 / normalization;
          m_256qam[58].real()  =   5 / normalization;
          m_256qam[58].imag()  =   1 / normalization;
          m_256qam[59].real()  =   5 / normalization;
          m_256qam[59].imag()  =   3 / normalization;
          m_256qam[60].real()  =   7 / normalization;
          m_256qam[60].imag()  =   7 / normalization;
          m_256qam[61].real()  =   7 / normalization;
          m_256qam[61].imag()  =   5 / normalization;
          m_256qam[62].real()  =   5 / normalization;
          m_256qam[62].imag()  =   7 / normalization;
          m_256qam[63].real()  =   5 / normalization;
          m_256qam[63].imag()  =   5 / normalization;
          m_256qam[64].real()  =  15 / normalization;
          m_256qam[64].imag()  = -15 / normalization;
          m_256qam[65].real()  =  15 / normalization;
          m_256qam[65].imag()  = -13 / normalization;
          m_256qam[66].real()  =  13 / normalization;
          m_256qam[66].imag()  = -15 / normalization;
          m_256qam[67].real()  =  13 / normalization;
          m_256qam[67].imag()  = -13 / normalization;
          m_256qam[68].real()  =  15 / normalization;
          m_256qam[68].imag()  =  -9 / normalization;
          m_256qam[69].real()  =  15 / normalization;
          m_256qam[69].imag()  = -11 / normalization;
          m_256qam[70].real()  =  13 / normalization;
          m_256qam[70].imag()  =  -9 / normalization;
          m_256qam[71].real()  =  13 / normalization;
          m_256qam[71].imag()  = -11 / normalization;
          m_256qam[72].real()  =   9 / normalization;
          m_256qam[72].imag()  = -15 / normalization;
          m_256qam[73].real()  =   9 / normalization;
          m_256qam[73].imag()  = -13 / normalization;
          m_256qam[74].real()  =  11 / normalization;
          m_256qam[74].imag()  = -15 / normalization;
          m_256qam[75].real()  =  11 / normalization;
          m_256qam[75].imag()  = -13 / normalization;
          m_256qam[76].real()  =   9 / normalization;
          m_256qam[76].imag()  =  -9 / normalization;
          m_256qam[77].real()  =   9 / normalization;
          m_256qam[77].imag()  = -11 / normalization;
          m_256qam[78].real()  =  11 / normalization;
          m_256qam[78].imag()  =  -9 / normalization;
          m_256qam[79].real()  =  11 / normalization;
          m_256qam[79].imag()  = -11 / normalization;
          m_256qam[80].real()  =  15 / normalization;
          m_256qam[80].imag()  =  -1 / normalization;
          m_256qam[81].real()  =  15 / normalization;
          m_256qam[81].imag()  =  -3 / normalization;
          m_256qam[82].real()  =  13 / normalization;
          m_256qam[82].imag()  =  -1 / normalization;
          m_256qam[83].real()  =  13 / normalization;
          m_256qam[83].imag()  =  -3 / normalization;
          m_256qam[84].real()  =  15 / normalization;
          m_256qam[84].imag()  =  -7 / normalization;
          m_256qam[85].real()  =  15 / normalization;
          m_256qam[85].imag()  =  -5 / normalization;
          m_256qam[86].real()  =  13 / normalization;
          m_256qam[86].imag()  =  -7 / normalization;
          m_256qam[87].real()  =  13 / normalization;
          m_256qam[87].imag()  =  -5 / normalization;
          m_256qam[88].real()  =   9 / normalization;
          m_256qam[88].imag()  =  -1 / normalization;
          m_256qam[89].real()  =   9 / normalization;
          m_256qam[89].imag()  =  -3 / normalization;
          m_256qam[90].real()  =  11 / normalization;
          m_256qam[90].imag()  =  -1 / normalization;
          m_256qam[91].real()  =  11 / normalization;
          m_256qam[91].imag()  =  -3 / normalization;
          m_256qam[92].real()  =   9 / normalization;
          m_256qam[92].imag()  =  -7 / normalization;
          m_256qam[93].real()  =   9 / normalization;
          m_256qam[93].imag()  =  -5 / normalization;
          m_256qam[94].real()  =  11 / normalization;
          m_256qam[94].imag()  =  -7 / normalization;
          m_256qam[95].real()  =  11 / normalization;
          m_256qam[95].imag()  =  -5 / normalization;
          m_256qam[96].real()  =   1 / normalization;
          m_256qam[96].imag()  = -15 / normalization;
          m_256qam[97].real()  =   1 / normalization;
          m_256qam[97].imag()  = -13 / normalization;
          m_256qam[98].real()  =   3 / normalization;
          m_256qam[98].imag()  = -15 / normalization;
          m_256qam[99].real()  =   3 / normalization;
          m_256qam[99].imag()  = -13 / normalization;
          m_256qam[100].real() =   1 / normalization;
          m_256qam[100].imag() =  -9 / normalization;
          m_256qam[101].real() =   1 / normalization;
          m_256qam[101].imag() = -11 / normalization;
          m_256qam[102].real() =   3 / normalization;
          m_256qam[102].imag() =  -9 / normalization;
          m_256qam[103].real() =   3 / normalization;
          m_256qam[103].imag() = -11 / normalization;
          m_256qam[104].real() =   7 / normalization;
          m_256qam[104].imag() = -15 / normalization;
          m_256qam[105].real() =   7 / normalization;
          m_256qam[105].imag() = -13 / normalization;
          m_256qam[106].real() =   5 / normalization;
          m_256qam[106].imag() = -15 / normalization;
          m_256qam[107].real() =   5 / normalization;
          m_256qam[107].imag() = -13 / normalization;
          m_256qam[108].real() =   7 / normalization;
          m_256qam[108].imag() =  -9 / normalization;
          m_256qam[109].real() =   7 / normalization;
          m_256qam[109].imag() = -11 / normalization;
          m_256qam[110].real() =   5 / normalization;
          m_256qam[110].imag() =  -9 / normalization;
          m_256qam[111].real() =   5 / normalization;
          m_256qam[111].imag() = -11 / normalization;
          m_256qam[112].real() =   1 / normalization;
          m_256qam[112].imag() =  -1 / normalization;
          m_256qam[113].real() =   1 / normalization;
          m_256qam[113].imag() =  -3 / normalization;
          m_256qam[114].real() =   3 / normalization;
          m_256qam[114].imag() =  -1 / normalization;
          m_256qam[115].real() =   3 / normalization;
          m_256qam[115].imag() =  -3 / normalization;
          m_256qam[116].real() =   1 / normalization;
          m_256qam[116].imag() =  -7 / normalization;
          m_256qam[117].real() =   1 / normalization;
          m_256qam[117].imag() =  -5 / normalization;
          m_256qam[118].real() =   3 / normalization;
          m_256qam[118].imag() =  -7 / normalization;
          m_256qam[119].real() =   3 / normalization;
          m_256qam[119].imag() =  -5 / normalization;
          m_256qam[120].real() =   7 / normalization;
          m_256qam[120].imag() =  -1 / normalization;
          m_256qam[121].real() =   7 / normalization;
          m_256qam[121].imag() =  -3 / normalization;
          m_256qam[122].real() =   5 / normalization;
          m_256qam[122].imag() =  -1 / normalization;
          m_256qam[123].real() =   5 / normalization;
          m_256qam[123].imag() =  -3 / normalization;
          m_256qam[124].real() =   7 / normalization;
          m_256qam[124].imag() =  -7 / normalization;
          m_256qam[125].real() =   7 / normalization;
          m_256qam[125].imag() =  -5 / normalization;
          m_256qam[126].real() =   5 / normalization;
          m_256qam[126].imag() =  -7 / normalization;
          m_256qam[127].real() =   5 / normalization;
          m_256qam[127].imag() =  -5 / normalization;
          m_256qam[128].real() = -15 / normalization;
          m_256qam[128].imag() =  15 / normalization;
          m_256qam[129].real() = -15 / normalization;
          m_256qam[129].imag() =  13 / normalization;
          m_256qam[130].real() = -13 / normalization;
          m_256qam[130].imag() =  15 / normalization;
          m_256qam[131].real() = -13 / normalization;
          m_256qam[131].imag() =  13 / normalization;
          m_256qam[132].real() = -15 / normalization;
          m_256qam[132].imag() =   9 / normalization;
          m_256qam[133].real() = -15 / normalization;
          m_256qam[133].imag() =  11 / normalization;
          m_256qam[134].real() = -13 / normalization;
          m_256qam[134].imag() =   9 / normalization;
          m_256qam[135].real() = -13 / normalization;
          m_256qam[135].imag() =  11 / normalization;
          m_256qam[136].real() =  -9 / normalization;
          m_256qam[136].imag() =  15 / normalization;
          m_256qam[137].real() =  -9 / normalization;
          m_256qam[137].imag() =  13 / normalization;
          m_256qam[138].real() = -11 / normalization;
          m_256qam[138].imag() =  15 / normalization;
          m_256qam[139].real() = -11 / normalization;
          m_256qam[139].imag() =  13 / normalization;
          m_256qam[140].real() =  -9 / normalization;
          m_256qam[140].imag() =   9 / normalization;
          m_256qam[141].real() =  -9 / normalization;
          m_256qam[141].imag() =  11 / normalization;
          m_256qam[142].real() = -11 / normalization;
          m_256qam[142].imag() =   9 / normalization;
          m_256qam[143].real() = -11 / normalization;
          m_256qam[143].imag() =  11 / normalization;
          m_256qam[144].real() = -15 / normalization;
          m_256qam[144].imag() =   1 / normalization;
          m_256qam[145].real() = -15 / normalization;
          m_256qam[145].imag() =   3 / normalization;
          m_256qam[146].real() = -13 / normalization;
          m_256qam[146].imag() =   1 / normalization;
          m_256qam[147].real() = -13 / normalization;
          m_256qam[147].imag() =   3 / normalization;
          m_256qam[148].real() = -15 / normalization;
          m_256qam[148].imag() =   7 / normalization;
          m_256qam[149].real() = -15 / normalization;
          m_256qam[149].imag() =   5 / normalization;
          m_256qam[150].real() = -13 / normalization;
          m_256qam[150].imag() =   7 / normalization;
          m_256qam[151].real() = -13 / normalization;
          m_256qam[151].imag() =   5 / normalization;
          m_256qam[152].real() =  -9 / normalization;
          m_256qam[152].imag() =   1 / normalization;
          m_256qam[153].real() =  -9 / normalization;
          m_256qam[153].imag() =   3 / normalization;
          m_256qam[154].real() = -11 / normalization;
          m_256qam[154].imag() =   1 / normalization;
          m_256qam[155].real() = -11 / normalization;
          m_256qam[155].imag() =   3 / normalization;
          m_256qam[156].real() =  -9 / normalization;
          m_256qam[156].imag() =   7 / normalization;
          m_256qam[157].real() =  -9 / normalization;
          m_256qam[157].imag() =   5 / normalization;
          m_256qam[158].real() = -11 / normalization;
          m_256qam[158].imag() =   7 / normalization;
          m_256qam[159].real() = -11 / normalization;
          m_256qam[159].imag() =   5 / normalization;
          m_256qam[160].real() =  -1 / normalization;
          m_256qam[160].imag() =  15 / normalization;
          m_256qam[161].real() =  -1 / normalization;
          m_256qam[161].imag() =  13 / normalization;
          m_256qam[162].real() =  -3 / normalization;
          m_256qam[162].imag() =  15 / normalization;
          m_256qam[163].real() =  -3 / normalization;
          m_256qam[163].imag() =  13 / normalization;
          m_256qam[164].real() =  -1 / normalization;
          m_256qam[164].imag() =   9 / normalization;
          m_256qam[165].real() =  -1 / normalization;
          m_256qam[165].imag() =  11 / normalization;
          m_256qam[166].real() =  -3 / normalization;
          m_256qam[166].imag() =   9 / normalization;
          m_256qam[167].real() =  -3 / normalization;
          m_256qam[167].imag() =  11 / normalization;
          m_256qam[168].real() =  -7 / normalization;
          m_256qam[168].imag() =  15 / normalization;
          m_256qam[169].real() =  -7 / normalization;
          m_256qam[169].imag() =  13 / normalization;
          m_256qam[170].real() =  -5 / normalization;
          m_256qam[170].imag() =  15 / normalization;
          m_256qam[171].real() =  -5 / normalization;
          m_256qam[171].imag() =  13 / normalization;
          m_256qam[172].real() =  -7 / normalization;
          m_256qam[172].imag() =   9 / normalization;
          m_256qam[173].real() =  -7 / normalization;
          m_256qam[173].imag() =  11 / normalization;
          m_256qam[174].real() =  -5 / normalization;
          m_256qam[174].imag() =   9 / normalization;
          m_256qam[175].real() =  -5 / normalization;
          m_256qam[175].imag() =  11 / normalization;
          m_256qam[176].real() =  -1 / normalization;
          m_256qam[176].imag() =   1 / normalization;
          m_256qam[177].real() =  -1 / normalization;
          m_256qam[177].imag() =   3 / normalization;
          m_256qam[178].real() =  -3 / normalization;
          m_256qam[178].imag() =   1 / normalization;
          m_256qam[179].real() =  -3 / normalization;
          m_256qam[179].imag() =   3 / normalization;
          m_256qam[180].real() =  -1 / normalization;
          m_256qam[180].imag() =   7 / normalization;
          m_256qam[181].real() =  -1 / normalization;
          m_256qam[181].imag() =   5 / normalization;
          m_256qam[182].real() =  -3 / normalization;
          m_256qam[182].imag() =   7 / normalization;
          m_256qam[183].real() =  -3 / normalization;
          m_256qam[183].imag() =   5 / normalization;
          m_256qam[184].real() =  -7 / normalization;
          m_256qam[184].imag() =   1 / normalization;
          m_256qam[185].real() =  -7 / normalization;
          m_256qam[185].imag() =   3 / normalization;
          m_256qam[186].real() =  -5 / normalization;
          m_256qam[186].imag() =   1 / normalization;
          m_256qam[187].real() =  -5 / normalization;
          m_256qam[187].imag() =   3 / normalization;
          m_256qam[188].real() =  -7 / normalization;
          m_256qam[188].imag() =   7 / normalization;
          m_256qam[189].real() =  -7 / normalization;
          m_256qam[189].imag() =   5 / normalization;
          m_256qam[190].real() =  -5 / normalization;
          m_256qam[190].imag() =   7 / normalization;
          m_256qam[191].real() =  -5 / normalization;
          m_256qam[191].imag() =   5 / normalization;
          m_256qam[192].real() = -15 / normalization;
          m_256qam[192].imag() = -15 / normalization;
          m_256qam[193].real() = -15 / normalization;
          m_256qam[193].imag() = -13 / normalization;
          m_256qam[194].real() = -13 / normalization;
          m_256qam[194].imag() = -15 / normalization;
          m_256qam[195].real() = -13 / normalization;
          m_256qam[195].imag() = -13 / normalization;
          m_256qam[196].real() = -15 / normalization;
          m_256qam[196].imag() =  -9 / normalization;
          m_256qam[197].real() = -15 / normalization;
          m_256qam[197].imag() = -11 / normalization;
          m_256qam[198].real() = -13 / normalization;
          m_256qam[198].imag() =  -9 / normalization;
          m_256qam[199].real() = -13 / normalization;
          m_256qam[199].imag() = -11 / normalization;
          m_256qam[200].real() =  -9 / normalization;
          m_256qam[200].imag() = -15 / normalization;
          m_256qam[201].real() =  -9 / normalization;
          m_256qam[201].imag() = -13 / normalization;
          m_256qam[202].real() = -11 / normalization;
          m_256qam[202].imag() = -15 / normalization;
          m_256qam[203].real() = -11 / normalization;
          m_256qam[203].imag() = -13 / normalization;
          m_256qam[204].real() =  -9 / normalization;
          m_256qam[204].imag() =  -9 / normalization;
          m_256qam[205].real() =  -9 / normalization;
          m_256qam[205].imag() = -11 / normalization;
          m_256qam[206].real() = -11 / normalization;
          m_256qam[206].imag() =  -9 / normalization;
          m_256qam[207].real() = -11 / normalization;
          m_256qam[207].imag() = -11 / normalization;
          m_256qam[208].real() = -15 / normalization;
          m_256qam[208].imag() =  -1 / normalization;
          m_256qam[209].real() = -15 / normalization;
          m_256qam[209].imag() =  -3 / normalization;
          m_256qam[210].real() = -13 / normalization;
          m_256qam[210].imag() =  -1 / normalization;
          m_256qam[211].real() = -13 / normalization;
          m_256qam[211].imag() =  -3 / normalization;
          m_256qam[212].real() = -15 / normalization;
          m_256qam[212].imag() =  -7 / normalization;
          m_256qam[213].real() = -15 / normalization;
          m_256qam[213].imag() =  -5 / normalization;
          m_256qam[214].real() = -13 / normalization;
          m_256qam[214].imag() =  -7 / normalization;
          m_256qam[215].real() = -13 / normalization;
          m_256qam[215].imag() =  -5 / normalization;
          m_256qam[216].real() =  -9 / normalization;
          m_256qam[216].imag() =  -1 / normalization;
          m_256qam[217].real() =  -9 / normalization;
          m_256qam[217].imag() =  -3 / normalization;
          m_256qam[218].real() = -11 / normalization;
          m_256qam[218].imag() =  -1 / normalization;
          m_256qam[219].real() = -11 / normalization;
          m_256qam[219].imag() =  -3 / normalization;
          m_256qam[220].real() =  -9 / normalization;
          m_256qam[220].imag() =  -7 / normalization;
          m_256qam[221].real() =  -9 / normalization;
          m_256qam[221].imag() =  -5 / normalization;
          m_256qam[222].real() = -11 / normalization;
          m_256qam[222].imag() =  -7 / normalization;
          m_256qam[223].real() = -11 / normalization;
          m_256qam[223].imag() =  -5 / normalization;
          m_256qam[224].real() =  -1 / normalization;
          m_256qam[224].imag() = -15 / normalization;
          m_256qam[225].real() =  -1 / normalization;
          m_256qam[225].imag() = -13 / normalization;
          m_256qam[226].real() =  -3 / normalization;
          m_256qam[226].imag() = -15 / normalization;
          m_256qam[227].real() =  -3 / normalization;
          m_256qam[227].imag() = -13 / normalization;
          m_256qam[228].real() =  -1 / normalization;
          m_256qam[228].imag() =  -9 / normalization;
          m_256qam[229].real() =  -1 / normalization;
          m_256qam[229].imag() = -11 / normalization;
          m_256qam[230].real() =  -3 / normalization;
          m_256qam[230].imag() =  -9 / normalization;
          m_256qam[231].real() =  -3 / normalization;
          m_256qam[231].imag() = -11 / normalization;
          m_256qam[232].real() =  -7 / normalization;
          m_256qam[232].imag() = -15 / normalization;
          m_256qam[233].real() =  -7 / normalization;
          m_256qam[233].imag() = -13 / normalization;
          m_256qam[234].real() =  -5 / normalization;
          m_256qam[234].imag() = -15 / normalization;
          m_256qam[235].real() =  -5 / normalization;
          m_256qam[235].imag() = -13 / normalization;
          m_256qam[236].real() =  -7 / normalization;
          m_256qam[236].imag() =  -9 / normalization;
          m_256qam[237].real() =  -7 / normalization;
          m_256qam[237].imag() = -11 / normalization;
          m_256qam[238].real() =  -5 / normalization;
          m_256qam[238].imag() =  -9 / normalization;
          m_256qam[239].real() =  -5 / normalization;
          m_256qam[239].imag() = -11 / normalization;
          m_256qam[240].real() =  -1 / normalization;
          m_256qam[240].imag() =  -1 / normalization;
          m_256qam[241].real() =  -1 / normalization;
          m_256qam[241].imag() =  -3 / normalization;
          m_256qam[242].real() =  -3 / normalization;
          m_256qam[242].imag() =  -1 / normalization;
          m_256qam[243].real() =  -3 / normalization;
          m_256qam[243].imag() =  -3 / normalization;
          m_256qam[244].real() =  -1 / normalization;
          m_256qam[244].imag() =  -7 / normalization;
          m_256qam[245].real() =  -1 / normalization;
          m_256qam[245].imag() =  -5 / normalization;
          m_256qam[246].real() =  -3 / normalization;
          m_256qam[246].imag() =  -7 / normalization;
          m_256qam[247].real() =  -3 / normalization;
          m_256qam[247].imag() =  -5 / normalization;
          m_256qam[248].real() =  -7 / normalization;
          m_256qam[248].imag() =  -1 / normalization;
          m_256qam[249].real() =  -7 / normalization;
          m_256qam[249].imag() =  -3 / normalization;
          m_256qam[250].real() =  -5 / normalization;
          m_256qam[250].imag() =  -1 / normalization;
          m_256qam[251].real() =  -5 / normalization;
          m_256qam[251].imag() =  -3 / normalization;
          m_256qam[252].real() =  -7 / normalization;
          m_256qam[252].imag() =  -7 / normalization;
          m_256qam[253].real() =  -7 / normalization;
          m_256qam[253].imag() =  -5 / normalization;
          m_256qam[254].real() =  -5 / normalization;
          m_256qam[254].imag() =  -7 / normalization;
          m_256qam[255].real() =  -5 / normalization;
          m_256qam[255].imag() =  -5 / normalization;
          if (rotation == ROTATION_ON) {
            cyclic_delay = TRUE;
            rotation_angle = (2.0 * M_PI * 3.576334375) / 360;
            for (int i = 0; i < 256; i++) {
              m_temp[0].real() = m_256qam[i].real();
              m_256qam[i].real() = (m_256qam[i].real() * cos(rotation_angle)) - (m_256qam[i].imag() * sin(rotation_angle));
              m_256qam[i].imag() = (m_temp[0].real() * sin(rotation_angle)) + (m_256qam[i].imag() * cos(rotation_angle));
            }
          }
          break;
        default:
          normalization = sqrt(2);
          m_qpsk[0].real() =  1.0 / normalization;
          m_qpsk[0].imag() =  1.0 / normalization;
          m_qpsk[1].real() =  1.0 / normalization;
          m_qpsk[1].imag() = -1.0 / normalization;
          m_qpsk[2].real() = -1.0 / normalization;
          m_qpsk[2].imag() =  1.0 / normalization;
          m_qpsk[3].real() = -1.0 / normalization;
          m_qpsk[3].imag() = -1.0 / normalization;
          if (rotation == ROTATION_ON) {
            cyclic_delay = TRUE;
            rotation_angle = (2.0 * M_PI * 29.0) / 360;
            for (int i = 0; i < 4; i++) {
              m_temp[0].real() = m_qpsk[i].real();
              m_qpsk[i].real() = (m_qpsk[i].real() * cos(rotation_angle)) - (m_qpsk[i].imag() * sin(rotation_angle));
              m_qpsk[i].imag() = (m_temp[0].real() * sin(rotation_angle)) + (m_qpsk[i].imag() * cos(rotation_angle));
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
      gr_complex f_temp[1], g_temp[1];
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
                f_temp[0] = m_qpsk[index & 0x3];
                g_temp[0] = m_qpsk[index_delay & 0x3];
                f_temp[0].imag() = g_temp[0].imag();
                *out++ = f_temp[0];
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
                f_temp[0] = m_16qam[index & 0xf];
                g_temp[0] = m_16qam[index_delay & 0xf];
                f_temp[0].imag() = g_temp[0].imag();
                *out++ = f_temp[0];
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
                f_temp[0] = m_64qam[index & 0x3f];
                g_temp[0] = m_64qam[index_delay & 0x3f];
                f_temp[0].imag() = g_temp[0].imag();
                *out++ = f_temp[0];
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
                f_temp[0] = m_256qam[index & 0xff];
                g_temp[0] = m_256qam[index_delay & 0xff];
                f_temp[0].imag() = g_temp[0].imag();
                *out++ = f_temp[0];
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

