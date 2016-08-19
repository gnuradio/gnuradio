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
#include "dvbt_configure.h"
#include <iostream>

namespace gr {
  namespace dtv {

    void
    dvbt_configure::set_frame_number(int fn)
    {
      d_frame_index = fn;
    }
    int
    dvbt_configure::get_frame_mumber()
    {
      return (d_frame_index);
    }
    void
    dvbt_configure::set_constellation(dvb_constellation_t constellation)
    {
      d_constellation = constellation;
    }
    dvb_constellation_t
    dvbt_configure::get_constellation()
    {
      return (d_constellation);
    }
    void
    dvbt_configure::set_hierarchical(dvbt_hierarchy_t hierarchy)
    {
      d_hierarchy = hierarchy;
    }
    dvbt_hierarchy_t
    dvbt_configure::get_hierarchical()
    {
      return d_hierarchy;
    }
    void
    dvbt_configure::set_code_rate_HP(dvb_code_rate_t code_rate)
    {
      d_code_rate_HP = code_rate;
    }
    void
    dvbt_configure::set_code_rate_LP(dvb_code_rate_t code_rate)
    {
      d_code_rate_LP = code_rate;
    }
    dvb_code_rate_t
    dvbt_configure::get_code_rate_HP()
    {
      return d_code_rate_HP;
    }
    dvb_code_rate_t
    dvbt_configure::get_code_rate_LP()
    {
      return d_code_rate_LP;
    }
    void
    dvbt_configure::set_transmission_mode(dvbt_transmission_mode_t transmission_mode)
    {
      d_transmission_mode = transmission_mode;
    }
    dvbt_transmission_mode_t
    dvbt_configure::get_transmission_mode()
    {
      return d_transmission_mode;
    }

    dvbt_configure::dvbt_configure(dvb_constellation_t constellation, \
      dvbt_hierarchy_t hierarchy, dvb_code_rate_t code_rate_HP, \
      dvb_code_rate_t code_rate_LP, dvb_guardinterval_t guard_interval, \
      dvbt_transmission_mode_t transmission_mode, int include_cell_id, int cell_id) :
            d_constellation(constellation), d_hierarchy(hierarchy), d_code_rate_HP(code_rate_HP),
            d_code_rate_LP(code_rate_LP), d_guard_interval(guard_interval), d_transmission_mode(transmission_mode),
            d_include_cell_id(include_cell_id), d_cell_id(cell_id)
    {
      d_symbols_per_frame = 68;
      d_frames_per_superframe = 4;
      d_symbol_index = 0;
      d_frame_index = 0;
      d_superframe_index = 0;

      switch (d_transmission_mode) {
        case T2k:
          d_Kmin = 0; d_Kmax = 1704;
          d_fft_length = 2048;
          d_payload_length = 1512;
          break;
        case T8k:
          d_Kmin = 0; d_Kmax = 6816;
          d_fft_length = 8192;
          d_payload_length = 6048;
          break;
        default:
          d_Kmin = 0; d_Kmax = 1704;
          d_fft_length = 2048;
          d_payload_length = 1512;
          break;
      }
      d_zeros_on_left = int(ceil((d_fft_length - (d_Kmax - d_Kmin + 1)) / 2.0));
      d_zeros_on_right = d_fft_length - d_zeros_on_left - (d_Kmax - d_Kmin + 1);

      switch (d_constellation) {
        case MOD_QPSK:
          d_constellation_size = 4;
          d_step = 2;
          d_m = 2;
          break;
        case MOD_16QAM:
          d_constellation_size = 16;
          d_step = 2;
          d_m = 4;
          break;
        case MOD_64QAM:
          d_constellation_size = 64;
          d_step = 2;
          d_m = 6;
          break;
        default:
          d_constellation_size = 16;
          d_step = 2;
          d_m = 4;
          break;
      }

      switch (d_code_rate_HP) {
        case C1_2:
          d_cr_k = 1; d_cr_n = 2; d_cr_p = 1;
          break;
        case C2_3:
          d_cr_k = 2; d_cr_n = 3; d_cr_p = 2;
          break;
        case C3_4:
          d_cr_k = 3; d_cr_n = 4; d_cr_p = 3;
          break;
        case C5_6:
          d_cr_k = 5; d_cr_n = 6; d_cr_p = 5;
          break;
        case C7_8:
          d_cr_k = 7; d_cr_n = 8; d_cr_p = 7;
          break;
        default:
          d_cr_k = 1; d_cr_n = 2; d_cr_p = 1;
          break;
      }

      switch (d_code_rate_LP) {
        case C1_2:
          d_cr_k = 1; d_cr_n = 2;
          break;
        case C2_3:
          d_cr_k = 2; d_cr_n = 3;
          break;
        case C3_4:
          d_cr_k = 3; d_cr_n = 4;
          break;
        case C5_6:
          d_cr_k = 5; d_cr_n = 6;
          break;
        case C7_8:
          d_cr_k = 7; d_cr_n = 8;
          break;
        default:
          d_cr_k = 1; d_cr_n = 2;
          break;
      }

      switch (guard_interval) {
        case GI_1_32:
          d_cp_length = d_fft_length / 32;
          break;
        case GI_1_16:
          d_cp_length = d_fft_length / 16;
          break;
        case GI_1_8:
          d_cp_length = d_fft_length / 8;
          break;
        case GI_1_4:
          d_cp_length = d_fft_length / 4;
          break;
        default:
          d_cp_length = d_fft_length / 32;
          break;
      }

      switch (d_hierarchy) {
        case NH:
          d_alpha = 1;
          break;
        case ALPHA1:
          d_alpha = 1;
          break;
        case ALPHA2:
          d_alpha = 2;
          break;
        case ALPHA4:
          d_alpha = 4;
          break;
        default:
          d_alpha = 1;
          break;
      }

      // ETSI EN 400 744 Clause 4.4
      // Normalization factor
      switch (d_m) {
        case 2:
          d_norm = 1.0 / sqrt(2);
          break;
        case 4:
          if (d_alpha == 1) {
            d_norm = 1.0 / sqrt(10);
          }
          if (d_alpha == 2) {
            d_norm = 1.0 / sqrt(20);
          }
          if (d_alpha == 4) {
            d_norm = 1.0 / sqrt(52);
          }
          break;
        case 6:
          if (d_alpha == 1) {
            d_norm = 1.0 / sqrt(42);
          }
          if (d_alpha == 2) {
            d_norm = 1.0 / sqrt(60);
          }
          if (d_alpha == 4) {
            d_norm = 1.0 / sqrt(108);
          }
          break;
        default:
          if (d_alpha == 1) {
            d_norm = 1.0 / sqrt(10);
          }
          if (d_alpha == 2) {
            d_norm = 1.0 / sqrt(20);
          }
          if (d_alpha == 4) {
            d_norm = 1.0 / sqrt(52);
          }
          break;
      }
    }

    dvbt_configure::~dvbt_configure()
    {
    }

  } /* namespace dtv */
} /* namespace gr */

