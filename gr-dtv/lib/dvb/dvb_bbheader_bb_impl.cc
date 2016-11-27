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
#include "dvb_bbheader_bb_impl.h"

namespace gr {
  namespace dtv {

    dvb_bbheader_bb::sptr
    dvb_bbheader_bb::make(dvb_standard_t standard, dvb_framesize_t framesize, dvb_code_rate_t rate, dvbs2_rolloff_factor_t rolloff, dvbt2_inputmode_t mode, dvbt2_inband_t inband, int fecblocks, int tsrate)
    {
      return gnuradio::get_initial_sptr
        (new dvb_bbheader_bb_impl(standard, framesize, rate, rolloff, mode, inband, fecblocks, tsrate));
    }

    /*
     * The private constructor
     */
    dvb_bbheader_bb_impl::dvb_bbheader_bb_impl(dvb_standard_t standard, dvb_framesize_t framesize, dvb_code_rate_t rate, dvbs2_rolloff_factor_t rolloff, dvbt2_inputmode_t mode, dvbt2_inband_t inband, int fecblocks, int tsrate)
      : gr::block("dvb_bbheader_bb",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char)))
    {
      count = 0;
      crc = 0x0;
      dvbs2x = FALSE;
      alternate = TRUE;
      nibble = TRUE;
      frame_size = framesize;
      BBHeader *f = &m_format[0].bb_header;
      if (framesize == FECFRAME_NORMAL) {
        switch (rate) {
          case C1_4:
            kbch = 16008;
            break;
          case C1_3:
            kbch = 21408;
            break;
          case C2_5:
            kbch = 25728;
            break;
          case C1_2:
            kbch = 32208;
            break;
          case C3_5:
            kbch = 38688;
            break;
          case C2_3:
            kbch = 43040;
            break;
          case C3_4:
            kbch = 48408;
            break;
          case C4_5:
            kbch = 51648;
            break;
          case C5_6:
            kbch = 53840;
            break;
          case C8_9:
            kbch = 57472;
            break;
          case C9_10:
            kbch = 58192;
            break;
          case C2_9_VLSNR:
            kbch = 14208;
            break;
          case C13_45:
            kbch = 18528;
            break;
          case C9_20:
            kbch = 28968;
            break;
          case C90_180:
            kbch = 32208;
            break;
          case C96_180:
            kbch = 34368;
            break;
          case C11_20:
            kbch = 35448;
            break;
          case C100_180:
            kbch = 35808;
            break;
          case C104_180:
            kbch = 37248;
            break;
          case C26_45:
            kbch = 37248;
            break;
          case C18_30:
            kbch = 38688;
            break;
          case C28_45:
            kbch = 40128;
            break;
          case C23_36:
            kbch = 41208;
            break;
          case C116_180:
            kbch = 41568;
            break;
          case C20_30:
            kbch = 43008;
            break;
          case C124_180:
            kbch = 44448;
            break;
          case C25_36:
            kbch = 44808;
            break;
          case C128_180:
            kbch = 45888;
            break;
          case C13_18:
            kbch = 46608;
            break;
          case C132_180:
            kbch = 47328;
            break;
          case C22_30:
            kbch = 47328;
            break;
          case C135_180:
            kbch = 48408;
            break;
          case C140_180:
            kbch = 50208;
            break;
          case C7_9:
            kbch = 50208;
            break;
          case C154_180:
            kbch = 55248;
            break;
          default:
            kbch = 0;
            break;
        }
      }
      else if (framesize == FECFRAME_SHORT) {
        switch (rate) {
          case C1_4:
            kbch = 3072;
            break;
          case C1_3:
            kbch = 5232;
            break;
          case C2_5:
            kbch = 6312;
            break;
          case C1_2:
            kbch = 7032;
            break;
          case C3_5:
            kbch = 9552;
            break;
          case C2_3:
            kbch = 10632;
            break;
          case C3_4:
            kbch = 11712;
            break;
          case C4_5:
            kbch = 12432;
            break;
          case C5_6:
            kbch = 13152;
            break;
          case C8_9:
            kbch = 14232;
            break;
          case C11_45:
            kbch = 3792;
            break;
          case C4_15:
            kbch = 4152;
            break;
          case C14_45:
            kbch = 4872;
            break;
          case C7_15:
            kbch = 7392;
            break;
          case C8_15:
            kbch = 8472;
            break;
          case C26_45:
            kbch = 9192;
            break;
          case C32_45:
            kbch = 11352;
            break;
          case C1_5_VLSNR_SF2:
            kbch = 2512;
            break;
          case C11_45_VLSNR_SF2:
            kbch = 3792;
            break;
          case C1_5_VLSNR:
            kbch = 3072;
            break;
          case C4_15_VLSNR:
            kbch = 4152;
            break;
          case C1_3_VLSNR:
            kbch = 5232;
            break;
          default:
            kbch = 0;
            break;
        }
      }
      else {
        switch (rate) {
          case C1_5_MEDIUM:
            kbch = 5660;
            break;
          case C11_45_MEDIUM:
            kbch = 7740;
            break;
          case C1_3_MEDIUM:
            kbch = 10620;
            break;
          default:
            kbch = 0;
            break;
        }
      }

      if (standard == STANDARD_DVBS2) {
        mode = INPUTMODE_NORMAL;
        inband_type_b = FALSE;
      }
      f->ts_gs   = TS_GS_TRANSPORT;
      f->sis_mis = SIS_MIS_SINGLE;
      f->ccm_acm = CCM;
      f->issyi   = ISSYI_NOT_ACTIVE;
      f->npd     = NPD_NOT_ACTIVE;
      if (mode == INPUTMODE_NORMAL) {
        f->upl  = 188 * 8;
        f->dfl  = kbch - 80;
        f->sync = 0x47;
      }
      else {
        f->upl  = 0;
        f->dfl  = kbch - 80;
        f->sync = 0;
      }
      if (standard == STANDARD_DVBS2) {
        if (rolloff & 0x4) {
          dvbs2x = TRUE;
        }
        f->ro = rolloff & 0x3;
      }
      else {
        f->ro = 0;
      }

      build_crc8_table();
      input_mode = mode;
      inband_type_b = inband;
      fec_blocks = fecblocks;
      fec_block = 0;
      ts_rate = tsrate;
      extra = (((kbch - 80) / 8) / 187) + 1;
      if (framesize != FECFRAME_MEDIUM) {
        set_output_multiple(kbch);
      }
      else {
        set_output_multiple(kbch * 2);
      }
    }

    /*
     * Our virtual destructor.
     */
    dvb_bbheader_bb_impl::~dvb_bbheader_bb_impl()
    {
    }

    void
    dvb_bbheader_bb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      if (input_mode == INPUTMODE_NORMAL) {
        if (frame_size != FECFRAME_MEDIUM) {
          ninput_items_required[0] = ((noutput_items - 80) / 8);
        }
        else {
          ninput_items_required[0] = ((noutput_items - 160) / 8);
        }
      }
      else {
        ninput_items_required[0] = ((noutput_items - 80) / 8) + extra;
      }
    }

#define CRC_POLY 0xAB
// Reversed
#define CRC_POLYR 0xD5

    void
    dvb_bbheader_bb_impl::build_crc8_table(void)
    {
      int r, crc;

      for (int i = 0; i < 256; i++) {
        r = i;
        crc = 0;
        for (int j = 7; j >= 0; j--) {
          if ((r & (1 << j) ? 1 : 0) ^ ((crc & 0x80) ? 1 : 0)) {
            crc = (crc << 1) ^ CRC_POLYR;
          }
          else {
            crc <<= 1;
          }
        }
        crc_tab[i] = crc;
      }
    }

    /*
     * MSB is sent first
     *
     * The polynomial has been reversed
     */
    int
    dvb_bbheader_bb_impl::add_crc8_bits(unsigned char *in, int length)
    {
      int crc = 0;
      int b;
      int i = 0;

      for (int n = 0; n < length; n++) {
        b = in[i++] ^ (crc & 0x01);
        crc >>= 1;
        if (b) {
          crc ^= CRC_POLY;
        }
      }

      if (input_mode == INPUTMODE_HIEFF) {
        crc ^= 0x80;
      }

      for (int n = 0; n < 8; n++) {
        in[i++] = (crc & (1 << n)) ? 1 : 0;
      }
      return 8;// Length of CRC
    }

    void
    dvb_bbheader_bb_impl::add_bbheader(unsigned char *out, int count, int padding, bool nibble)
    {
      int temp, m_frame_offset_bits;
      unsigned char *m_frame = out;
      BBHeader *h = &m_format[0].bb_header;

      m_frame[0] = h->ts_gs >> 1;
      m_frame[1] = h->ts_gs & 1;
      m_frame[2] = h->sis_mis;
      m_frame[3] = h->ccm_acm;
      m_frame[4] = h->issyi & 1;
      m_frame[5] = h->npd & 1;
      if (dvbs2x == TRUE) {
        if (alternate == TRUE) {
          alternate = FALSE;
          m_frame[6] = 1;
          m_frame[7] = 1;
        }
        else {
          alternate = TRUE;
          m_frame[6] = h->ro >> 1;
          m_frame[7] = h->ro & 1;
        }
      }
      else {
        m_frame[6] = h->ro >> 1;
        m_frame[7] = h->ro & 1;
      }
      m_frame_offset_bits = 8;
      if (h->sis_mis == SIS_MIS_MULTIPLE) {
        temp = h->isi;
        for (int n = 7; n >= 0; n--) {
          m_frame[m_frame_offset_bits++] = temp & (1 << n) ? 1 : 0;
        }
      }
      else {
        for (int n = 7; n >= 0; n--) {
          m_frame[m_frame_offset_bits++] = 0;
        }
      }
      temp = h->upl;
      for (int n = 15; n >= 0; n--) {
        m_frame[m_frame_offset_bits++] = temp & (1 << n) ? 1 : 0;
      }
      temp = h->dfl - padding;
      for (int n = 15; n >= 0; n--) {
        m_frame[m_frame_offset_bits++] = temp & (1 << n) ? 1 : 0;
      }
      temp = h->sync;
      for (int n = 7; n >= 0; n--) {
        m_frame[m_frame_offset_bits++] = temp & (1 << n) ? 1 : 0;
      }
      // Calculate syncd, this should point to the MSB of the CRC
      temp = count;
      if (temp == 0) {
        temp = count;
      }
      else {
        temp = (188 - count) * 8;
      }
      if (nibble == FALSE) {
        temp += 4;
      }
      for (int n = 15; n >= 0; n--) {
        m_frame[m_frame_offset_bits++] = temp & (1 << n) ? 1 : 0;
      }
      // Add CRC to BB header, at end
      int len = BB_HEADER_LENGTH_BITS;
      m_frame_offset_bits += add_crc8_bits(m_frame, len);
    }

    void
    dvb_bbheader_bb_impl::add_inband_type_b(unsigned char *out, int ts_rate)
    {
      int temp, m_frame_offset_bits;
      unsigned char *m_frame = out;

      m_frame[0] = 0;
      m_frame[1] = 1;
      m_frame_offset_bits = 2;
      for (int n = 30; n >= 0; n--) {
        m_frame[m_frame_offset_bits++] = 0;
      }
      for (int n = 21; n >= 0; n--) {
        m_frame[m_frame_offset_bits++] = 0;
      }
      for (int n = 1; n >= 0; n--) {
        m_frame[m_frame_offset_bits++] = 0;
      }
      for (int n = 9; n >= 0; n--) {
        m_frame[m_frame_offset_bits++] = 0;
      }
      temp = ts_rate;
      for (int n = 26; n >= 0; n--) {
        m_frame[m_frame_offset_bits++] = temp & (1 << n) ? 1 : 0;
      }
      for (int n = 9; n >= 0; n--) {
        m_frame[m_frame_offset_bits++] = 0;
      }
    }

    int
    dvb_bbheader_bb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];
      int consumed = 0;
      int offset = 0;
      int padding;
      unsigned char b;

      for (int i = 0; i < noutput_items; i += kbch) {
        if (frame_size != FECFRAME_MEDIUM) {
          if (fec_block == 0 && inband_type_b == TRUE) {
            padding = 104;
          }
          else {
            padding = 0;
          }
          add_bbheader(&out[offset], count, padding, TRUE);
          offset = offset + 80;

          if (input_mode == INPUTMODE_HIEFF) {
            for (int j = 0; j < (int)((kbch - 80 - padding) / 8); j++) {
              if (count == 0) {
                if (*in != 0x47) {
                  GR_LOG_WARN(d_logger, "Transport Stream sync error!");
                }
                j--;
                in++;
              }
              else {
                b = *in++;
                for (int n = 7; n >= 0; n--) {
                  out[offset++] = b & (1 << n) ? 1 : 0;
                }
              }
              count = (count + 1) % 188;
              consumed++;
            }
            if (fec_block == 0 && inband_type_b == TRUE) {
              add_inband_type_b(&out[offset], ts_rate);
              offset = offset + 104;
            }
          }
          else {
            for (int j = 0; j < (int)((kbch - 80 - padding) / 8); j++) {
              if (count == 0) {
                if (*in != 0x47) {
                  GR_LOG_WARN(d_logger, "Transport Stream sync error!");
                }
                in++;
                b = crc;
                crc = 0;
              }
              else {
                b = *in++;
                crc = crc_tab[b ^ crc];
              }
              count = (count + 1) % 188;
              consumed++;
              for (int n = 7; n >= 0; n--) {
                out[offset++] = b & (1 << n) ? 1 : 0;
              }
            }
            if (fec_block == 0 && inband_type_b == TRUE) {
              add_inband_type_b(&out[offset], ts_rate);
              offset = offset + 104;
            }
          }
          if (inband_type_b == TRUE) {
            fec_block = (fec_block + 1) % fec_blocks;
          }
        }
        else {
          padding = 0;
          add_bbheader(&out[offset], count, padding, nibble);
          offset = offset + 80;
          for (int j = 0; j < (int)((kbch - 80) / 4); j++) {
            if (nibble == TRUE) {
              if (count == 0) {
                if (*in != 0x47) {
                  GR_LOG_WARN(d_logger, "Transport Stream sync error!");
                }
                in++;
                b = crc;
                crc = 0;
              }
              else {
                b = *in++;
                crc = crc_tab[b ^ crc];
              }
              bsave = b;
              count = (count + 1) % 188;
              consumed++;
              for (int n = 7; n >= 4; n--) {
                out[offset++] = b & (1 << n) ? 1 : 0;
              }
              nibble = FALSE;
            }
            else {
              for (int n = 3; n >= 0; n--) {
                out[offset++] = bsave & (1 << n) ? 1 : 0;
              }
              nibble = TRUE;
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

  } /* namespace dtv */
} /* namespace gr */

