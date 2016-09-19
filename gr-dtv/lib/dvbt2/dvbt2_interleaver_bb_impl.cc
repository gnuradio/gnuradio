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
#include "dvbt2_interleaver_bb_impl.h"

namespace gr {
  namespace dtv {

    dvbt2_interleaver_bb::sptr
    dvbt2_interleaver_bb::make(dvb_framesize_t framesize, dvb_code_rate_t rate, dvb_constellation_t constellation)
    {
      return gnuradio::get_initial_sptr
        (new dvbt2_interleaver_bb_impl(framesize, rate, constellation));
    }

    /*
     * The private constructor
     */
    dvbt2_interleaver_bb_impl::dvbt2_interleaver_bb_impl(dvb_framesize_t framesize, dvb_code_rate_t rate, dvb_constellation_t constellation)
      : gr::block("dvbt2_interleaver_bb",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char)))
    {
      signal_constellation = constellation;
      code_rate = rate;
      if (framesize == FECFRAME_NORMAL) {
        frame_size = FRAME_SIZE_NORMAL;
        switch (rate) {
          case C1_2:
            nbch = 32400;
            q_val = 90;
            break;
          case C3_5:
            nbch = 38880;
            q_val = 72;
            break;
          case C2_3:
            nbch = 43200;
            q_val = 60;
            break;
          case C3_4:
            nbch = 48600;
            q_val = 45;
            break;
          case C4_5:
            nbch = 51840;
            q_val = 36;
            break;
          case C5_6:
            nbch = 54000;
            q_val = 30;
            break;
          default:
            nbch = 0;
            q_val = 0;
            break;
        }
      }
      else {
        frame_size = FRAME_SIZE_SHORT;
        switch (rate) {
          case C1_3:
            nbch = 5400;
            q_val = 30;
            break;
          case C2_5:
            nbch = 6480;
            q_val = 27;
            break;
          case C1_2:
            nbch = 7200;
            q_val = 25;
            break;
          case C3_5:
            nbch = 9720;
            q_val = 18;
            break;
          case C2_3:
            nbch = 10800;
            q_val = 15;
            break;
          case C3_4:
            nbch = 11880;
            q_val = 12;
            break;
          case C4_5:
            nbch = 12600;
            q_val = 10;
            break;
          case C5_6:
            nbch = 13320;
            q_val = 8;
            break;
          default:
            nbch = 0;
            q_val = 0;
            break;
        }
      }
      switch (constellation) {
        case MOD_QPSK:
          mod = 2;
          set_output_multiple(frame_size / mod);
          packed_items = frame_size / mod;
          break;
        case MOD_16QAM:
          mod = 4;
          set_output_multiple(frame_size / mod);
          packed_items = frame_size / mod;
          break;
        case MOD_64QAM:
          mod = 6;
          set_output_multiple(frame_size / mod);
          packed_items = frame_size / mod;
          break;
        case MOD_256QAM:
          mod = 8;
          set_output_multiple(frame_size / mod);
          packed_items = frame_size / mod;
          break;
        default:
          mod = 1;
          set_output_multiple(frame_size / mod);
          packed_items = frame_size / mod;
          break;
      }
    }

    /*
     * Our virtual destructor.
     */
    dvbt2_interleaver_bb_impl::~dvbt2_interleaver_bb_impl()
    {
    }

    void
    dvbt2_interleaver_bb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items * mod;
    }

    int
    dvbt2_interleaver_bb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];
      int consumed = 0;
      int produced = 0;
      int rows, offset, index;
      unsigned int pack;
      const int *twist;
      const int *mux;

      switch (signal_constellation) {
        case MOD_QPSK:
          for (int i = 0; i < noutput_items; i += packed_items) {
            rows = frame_size / 2;
            if (code_rate == C1_3 || code_rate == C2_5) {
              for (int k = 0; k < nbch; k++) {
                tempu[k] = *in++;
              }
              for (int t = 0; t < q_val; t++) {
                for (int s = 0; s < 360; s++) {
                  tempu[nbch + (360 * t) + s] = in[(q_val * s) + t];
                }
              }
              in = in + (q_val * 360);
              index = 0;
              for (int j = 0; j < rows; j++) {
                out[produced] = tempu[index++] << 1;
                out[produced++] |= tempu[index++];
                consumed += 2;
              }
            }
            else {
              for (int j = 0; j < rows; j++) {
                out[produced] = in[consumed++] << 1;
                out[produced++] |= in[consumed++];
              }
            }
          }
          break;
        case MOD_16QAM:
          if (frame_size == FRAME_SIZE_NORMAL) {
            twist = &twist16n[0];
          }
          else {
            twist = &twist16s[0];
          }
          if (code_rate == C3_5 && frame_size == FRAME_SIZE_NORMAL) {
            mux = &mux16_35[0];
          }
          else if (code_rate == C1_3 && frame_size == FRAME_SIZE_SHORT) {
            mux = &mux16_13[0];
          }
          else if (code_rate == C2_5 && frame_size == FRAME_SIZE_SHORT) {
            mux = &mux16_25[0];
          }
          else {
            mux = &mux16[0];
          }
          for (int i = 0; i < noutput_items; i += packed_items) {
            rows = frame_size / (mod * 2);
            const unsigned char *c1, *c2, *c3, *c4, *c5, *c6, *c7, *c8;
            c1 = &tempv[0];
            c2 = &tempv[rows];
            c3 = &tempv[rows * 2];
            c4 = &tempv[rows * 3];
            c5 = &tempv[rows * 4];
            c6 = &tempv[rows * 5];
            c7 = &tempv[rows * 6];
            c8 = &tempv[rows * 7];
            for (int k = 0; k < nbch; k++) {
              tempu[k] = *in++;
            }
            for (int t = 0; t < q_val; t++) {
              for (int s = 0; s < 360; s++) {
                tempu[nbch + (360 * t) + s] = in[(q_val * s) + t];
              }
            }
            in = in + (q_val * 360);
            index = 0;
            for (int col = 0; col < (mod * 2); col++) {
              offset = twist[col];
              for (int row = 0; row < rows; row++) {
                tempv[offset + (rows * col)] = tempu[index++];
                offset++;
                if (offset == rows) {
                  offset = 0;
                }
              }
            }
            index = 0;
            for (int j = 0; j < rows; j++) {
              tempu[index++] = c1[j];
              tempu[index++] = c2[j];
              tempu[index++] = c3[j];
              tempu[index++] = c4[j];
              tempu[index++] = c5[j];
              tempu[index++] = c6[j];
              tempu[index++] = c7[j];
              tempu[index++] = c8[j];
            }
            index = 0;
            for (int d = 0; d < frame_size / (mod * 2); d++) {
              pack = 0;
              for (int e = 0; e < (mod * 2); e++) {
                offset = mux[e];
                pack |= tempu[index++] << (((mod * 2) - 1) - offset);
              }
              out[produced++] = pack >> 4;
              out[produced++] = pack & 0xf;
              consumed += (mod * 2);
            }
          }
          break;
        case MOD_64QAM:
          if (frame_size == FRAME_SIZE_NORMAL) {
            twist = &twist64n[0];
          }
          else {
            twist = &twist64s[0];
          }
          if (code_rate == C3_5 && frame_size == FRAME_SIZE_NORMAL) {
            mux = &mux64_35[0];
          }
          else if (code_rate == C1_3 && frame_size == FRAME_SIZE_SHORT) {
            mux = &mux64_13[0];
          }
          else if (code_rate == C2_5 && frame_size == FRAME_SIZE_SHORT) {
            mux = &mux64_25[0];
          }
          else {
            mux = &mux64[0];
          }
          for (int i = 0; i < noutput_items; i += packed_items) {
            rows = frame_size / (mod * 2);
            const unsigned char *c1, *c2, *c3, *c4, *c5, *c6, *c7, *c8, *c9, *c10, *c11, *c12;
            c1 = &tempv[0];
            c2 = &tempv[rows];
            c3 = &tempv[rows * 2];
            c4 = &tempv[rows * 3];
            c5 = &tempv[rows * 4];
            c6 = &tempv[rows * 5];
            c7 = &tempv[rows * 6];
            c8 = &tempv[rows * 7];
            c9 = &tempv[rows * 8];
            c10 = &tempv[rows * 9];
            c11 = &tempv[rows * 10];
            c12 = &tempv[rows * 11];
            for (int k = 0; k < nbch; k++) {
              tempu[k] = *in++;
            }
            for (int t = 0; t < q_val; t++) {
              for (int s = 0; s < 360; s++) {
                tempu[nbch + (360 * t) + s] = in[(q_val * s) + t];
              }
            }
            in = in + (q_val * 360);
            index = 0;
            for (int col = 0; col < (mod * 2); col++) {
              offset = twist[col];
              for (int row = 0; row < rows; row++) {
                tempv[offset + (rows * col)] = tempu[index++];
                offset++;
                if (offset == rows) {
                  offset = 0;
                }
              }
            }
            index = 0;
            for (int j = 0; j < rows; j++) {
              tempu[index++] = c1[j];
              tempu[index++] = c2[j];
              tempu[index++] = c3[j];
              tempu[index++] = c4[j];
              tempu[index++] = c5[j];
              tempu[index++] = c6[j];
              tempu[index++] = c7[j];
              tempu[index++] = c8[j];
              tempu[index++] = c9[j];
              tempu[index++] = c10[j];
              tempu[index++] = c11[j];
              tempu[index++] = c12[j];
            }
            index = 0;
            for (int d = 0; d < frame_size / (mod * 2); d++) {
              pack = 0;
              for (int e = 0; e < (mod * 2); e++) {
                offset = mux[e];
                pack |= tempu[index++] << (((mod * 2) - 1) - offset);
              }
              out[produced++] = pack >> 6;
              out[produced++] = pack & 0x3f;
              consumed += (mod * 2);
            }
          }
          break;
        case MOD_256QAM:
          if (frame_size == FRAME_SIZE_NORMAL) {
            if (code_rate == C3_5) {
              mux = &mux256_35[0];
            }
            else if (code_rate == C2_3) {
              mux = &mux256_23[0];
            }
            else {
              mux = &mux256[0];
            }
            for (int i = 0; i < noutput_items; i += packed_items) {
              rows = frame_size / (mod * 2);
              const unsigned char *c1, *c2, *c3, *c4, *c5, *c6, *c7, *c8;
              const unsigned char *c9, *c10, *c11, *c12, *c13, *c14, *c15, *c16;
              c1 = &tempv[0];
              c2 = &tempv[rows];
              c3 = &tempv[rows * 2];
              c4 = &tempv[rows * 3];
              c5 = &tempv[rows * 4];
              c6 = &tempv[rows * 5];
              c7 = &tempv[rows * 6];
              c8 = &tempv[rows * 7];
              c9 = &tempv[rows * 8];
              c10 = &tempv[rows * 9];
              c11 = &tempv[rows * 10];
              c12 = &tempv[rows * 11];
              c13 = &tempv[rows * 12];
              c14 = &tempv[rows * 13];
              c15 = &tempv[rows * 14];
              c16 = &tempv[rows * 15];
              for (int k = 0; k < nbch; k++) {
                tempu[k] = *in++;
              }
              for (int t = 0; t < q_val; t++) {
                for (int s = 0; s < 360; s++) {
                  tempu[nbch + (360 * t) + s] = in[(q_val * s) + t];
                }
              }
              in = in + (q_val * 360);
              index = 0;
              for (int col = 0; col < (mod * 2); col++) {
                offset = twist256n[col];
                for (int row = 0; row < rows; row++) {
                  tempv[offset + (rows * col)] = tempu[index++];
                  offset++;
                  if (offset == rows) {
                    offset = 0;
                  }
                }
              }
              index = 0;
              for (int j = 0; j < rows; j++) {
                tempu[index++] = c1[j];
                tempu[index++] = c2[j];
                tempu[index++] = c3[j];
                tempu[index++] = c4[j];
                tempu[index++] = c5[j];
                tempu[index++] = c6[j];
                tempu[index++] = c7[j];
                tempu[index++] = c8[j];
                tempu[index++] = c9[j];
                tempu[index++] = c10[j];
                tempu[index++] = c11[j];
                tempu[index++] = c12[j];
                tempu[index++] = c13[j];
                tempu[index++] = c14[j];
                tempu[index++] = c15[j];
                tempu[index++] = c16[j];
              }
              index = 0;
              for (int d = 0; d < frame_size / (mod * 2); d++) {
                pack = 0;
                for (int e = 0; e < (mod * 2); e++) {
                  offset = mux[e];
                  pack |= tempu[index++] << (((mod * 2) - 1) - offset);
                }
                out[produced++] = pack >> 8;
                out[produced++] = pack & 0xff;
                consumed += (mod * 2);
              }
            }
          }
          else {
            if (code_rate == C1_3) {
              mux = &mux256s_13[0];
            }
            else if (code_rate == C2_5) {
              mux = &mux256s_25[0];
            }
            else {
              mux = &mux256s[0];
            }
            for (int i = 0; i < noutput_items; i += packed_items) {
              rows = frame_size / mod;
              const unsigned char *c1, *c2, *c3, *c4, *c5, *c6, *c7, *c8;
              c1 = &tempv[0];
              c2 = &tempv[rows];
              c3 = &tempv[rows * 2];
              c4 = &tempv[rows * 3];
              c5 = &tempv[rows * 4];
              c6 = &tempv[rows * 5];
              c7 = &tempv[rows * 6];
              c8 = &tempv[rows * 7];
              for (int k = 0; k < nbch; k++) {
                tempu[k] = *in++;
              }
              for (int t = 0; t < q_val; t++) {
                for (int s = 0; s < 360; s++) {
                  tempu[nbch + (360 * t) + s] = in[(q_val * s) + t];
                }
              }
              in = in + (q_val * 360);
              index = 0;
              for (int col = 0; col < mod; col++) {
                offset = twist256s[col];
                for (int row = 0; row < rows; row++) {
                  tempv[offset + (rows * col)] = tempu[index++];
                  offset++;
                  if (offset == rows) {
                    offset = 0;
                  }
                }
              }
              index = 0;
              for (int j = 0; j < rows; j++) {
                tempu[index++] = c1[j];
                tempu[index++] = c2[j];
                tempu[index++] = c3[j];
                tempu[index++] = c4[j];
                tempu[index++] = c5[j];
                tempu[index++] = c6[j];
                tempu[index++] = c7[j];
                tempu[index++] = c8[j];
              }
              index = 0;
              for (int d = 0; d < frame_size / mod; d++) {
                pack = 0;
                for (int e = 0; e < mod; e++) {
                  offset = mux[e];
                  pack |= tempu[index++] << ((mod - 1) - offset);
                }
                out[produced++] = pack & 0xff;
                consumed += mod;
              }
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

    const int dvbt2_interleaver_bb_impl::twist16n[8] =
    {
      0, 0, 2, 4, 4, 5, 7, 7
    };

    const int dvbt2_interleaver_bb_impl::twist64n[12] =
    {
      0, 0, 2, 2, 3, 4, 4, 5, 5, 7, 8, 9
    };

    const int dvbt2_interleaver_bb_impl::twist256n[16] =
    {
      0, 2, 2, 2, 2, 3, 7, 15, 16, 20, 22, 22, 27, 27, 28, 32
    };

    const int dvbt2_interleaver_bb_impl::twist16s[8] =
    {
      0, 0, 0, 1, 7, 20, 20, 21
    };

    const int dvbt2_interleaver_bb_impl::twist64s[12] =
    {
      0, 0, 0, 2, 2, 2, 3, 3, 3, 6, 7, 7
    };

    const int dvbt2_interleaver_bb_impl::twist256s[8] =
    {
      0, 0, 0, 1, 7, 20, 20, 21
    };

    const int dvbt2_interleaver_bb_impl::mux16[8] =
    {
      7, 1, 4, 2, 5, 3, 6, 0
    };

    const int dvbt2_interleaver_bb_impl::mux64[12] =
    {
      11, 7, 3, 10, 6, 2, 9, 5, 1, 8, 4, 0
    };

    const int dvbt2_interleaver_bb_impl::mux256[16] =
    {
      15, 1, 13, 3, 8, 11, 9, 5, 10, 6, 4, 7, 12, 2, 14, 0
    };

    const int dvbt2_interleaver_bb_impl::mux16_35[8] =
    {
      0, 5, 1, 2, 4, 7, 3, 6
    };

    const int dvbt2_interleaver_bb_impl::mux16_13[8] =
    {
      6, 0, 3, 4, 5, 2, 1, 7
    };

    const int dvbt2_interleaver_bb_impl::mux16_25[8] =
    {
      7, 5, 4, 0, 3, 1, 2, 6
    };

    const int dvbt2_interleaver_bb_impl::mux64_35[12] =
    {
      2, 7, 6, 9, 0, 3, 1, 8, 4, 11, 5, 10
    };

    const int dvbt2_interleaver_bb_impl::mux64_13[12] =
    {
      4, 2, 0, 5, 6, 1, 3, 7, 8, 9, 10, 11
    };

    const int dvbt2_interleaver_bb_impl::mux64_25[12] =
    {
      4, 0, 1, 6, 2, 3, 5, 8, 7, 10, 9, 11
    };

    const int dvbt2_interleaver_bb_impl::mux256_35[16] =
    {
      2, 11, 3, 4, 0, 9, 1, 8, 10, 13, 7, 14, 6, 15, 5, 12
    };

    const int dvbt2_interleaver_bb_impl::mux256_23[16] =
    {
      7, 2, 9, 0, 4, 6, 13, 3, 14, 10, 15, 5, 8, 12, 11, 1
    };

    const int dvbt2_interleaver_bb_impl::mux256s[8] =
    {
      7, 3, 1, 5, 2, 6, 4, 0
    };

    const int dvbt2_interleaver_bb_impl::mux256s_13[8] =
    {
      4, 0, 1, 2, 5, 3, 6, 7
    };

    const int dvbt2_interleaver_bb_impl::mux256s_25[8] =
    {
      4, 0, 5, 1, 2, 3, 6, 7
    };

  } /* namespace dtv */
} /* namespace gr */

