/* -*- c++ -*- */
/* 
 * Copyright 2015,2017 Free Software Foundation, Inc.
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
      double m_16qam_lookup[4] = {3.0, 1.0, -3.0, -1.0};
      double m_64qam_lookup[8] = {7.0, 5.0, 1.0, 3.0, -7.0, -5.0, -1.0, -3.0};
      double m_256qam_lookup[16] = {15.0, 13.0, 9.0, 11.0, 1.0, 3.0, 7.0, 5.0, -15.0, -13.0, -9.0, -11.0, -1.0, -3.0, -7.0, -5.0};
      int real_index, imag_index;
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
          for (int i = 0; i < 16; i++) {
            real_index = ((i & 0x8) >> 2) | ((i & 0x2) >> 1);
            imag_index = ((i & 0x4) >> 1) | ((i & 0x1) >> 0);
            m_16qam[i] = gr_complex(m_16qam_lookup[real_index] / normalization, m_16qam_lookup[imag_index] / normalization);
          }
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
          for (int i = 0; i < 64; i++) {
            real_index = ((i & 0x20) >> 3) | ((i & 0x8) >> 2) | ((i & 0x2) >> 1);
            imag_index = ((i & 0x10) >> 2) | ((i & 0x4) >> 1) | ((i & 0x1) >> 0);
            m_64qam[i] = gr_complex(m_64qam_lookup[real_index] / normalization, m_64qam_lookup[imag_index] / normalization);
          }
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
          for (int i = 0; i < 256; i++) {
            real_index = ((i & 0x80) >> 4) | ((i & 0x20) >> 3) | ((i & 0x8) >> 2) | ((i & 0x2) >> 1);
            imag_index = ((i & 0x40) >> 3) | ((i & 0x10) >> 2) | ((i & 0x4) >> 1) | ((i & 0x1) >> 0);
            m_256qam[i] = gr_complex(m_256qam_lookup[real_index] / normalization, m_256qam_lookup[imag_index] / normalization);
          }
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
              for (int j = 0; j < cell_size; j++) {
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

