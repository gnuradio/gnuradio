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
#include "dvbt2_cellinterleaver_cc_impl.h"

namespace gr {
  namespace dtv {

    dvbt2_cellinterleaver_cc::sptr
    dvbt2_cellinterleaver_cc::make(dvb_framesize_t framesize, dvb_constellation_t constellation, int fecblocks, int tiblocks)
    {
      return gnuradio::get_initial_sptr
        (new dvbt2_cellinterleaver_cc_impl(framesize, constellation, fecblocks, tiblocks));
    }

    /*
     * The private constructor
     */
    dvbt2_cellinterleaver_cc_impl::dvbt2_cellinterleaver_cc_impl(dvb_framesize_t framesize, dvb_constellation_t constellation, int fecblocks, int tiblocks)
      : gr::sync_block("dvbt2_cellinterleaver_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {
      int max_states, xor_size, pn_mask, result, q = 0;
      int lfsr = 0;
      int logic11[2] = {0, 3};
      int logic12[2] = {0, 2};
      int logic13[4] = {0, 1, 4, 6};
      int logic14[6] = {0, 1, 4, 5, 9, 11};
      int logic15[4] = {0, 1, 2, 12};
      int *logic;
      if (framesize == FECFRAME_NORMAL) {
        switch (constellation) {
          case MOD_QPSK:
            cell_size = 32400;
            pn_degree = 15;
            pn_mask = 0x3fff;
            max_states = 32768;
            logic = &logic15[0];
            xor_size = 4;
            break;
          case MOD_16QAM:
            cell_size = 16200;
            pn_degree = 14;
            pn_mask = 0x1fff;
            max_states = 16384;
            logic = &logic14[0];
            xor_size = 6;
            break;
          case MOD_64QAM:
            cell_size = 10800;
            pn_degree = 14;
            pn_mask = 0x1fff;
            max_states = 16384;
            logic = &logic14[0];
            xor_size = 6;
            break;
          case MOD_256QAM:
            cell_size = 8100;
            pn_degree = 13;
            pn_mask = 0xfff;
            max_states = 8192;
            logic = &logic13[0];
            xor_size = 4;
            break;
          default:
            cell_size = 32400;
            pn_degree = 15;
            pn_mask = 0x3fff;
            max_states = 32768;
            logic = &logic15[0];
            xor_size = 4;
            break;
        }
      }
      else {
        switch (constellation) {
          case MOD_QPSK:
            cell_size = 8100;
            pn_degree = 13;
            pn_mask = 0xfff;
            max_states = 8192;
            logic = &logic13[0];
            xor_size = 4;
            break;
          case MOD_16QAM:
            cell_size = 4050;
            pn_degree = 12;
            pn_mask = 0x7ff;
            max_states = 4096;
            logic = &logic12[0];
            xor_size = 2;
            break;
          case MOD_64QAM:
            cell_size = 2700;
            pn_degree = 12;
            pn_mask = 0x7ff;
            max_states = 4096;
            logic = &logic12[0];
            xor_size = 2;
            break;
          case MOD_256QAM:
            cell_size = 2025;
            pn_degree = 11;
            pn_mask = 0x3ff;
            max_states = 2048;
            logic = &logic11[0];
            xor_size = 2;
            break;
          default:
            cell_size = 8100;
            pn_degree = 13;
            pn_mask = 0xfff;
            max_states = 8192;
            logic = &logic13[0];
            xor_size = 4;
            break;
        }
      }
      for (int i = 0; i < max_states; i++) {
        if (i == 0 || i == 1) {
          lfsr = 0;
        }
        else if (i == 2) {
          lfsr = 1;
        }
        else {
          result = 0;
          for (int k = 0; k < xor_size; k++) {
            result ^= (lfsr >> logic[k]) & 1;
          }
          lfsr &= pn_mask;
          lfsr >>= 1;
          lfsr |= result << (pn_degree - 2);
        }
        lfsr |= (i % 2) << (pn_degree - 1);
        if (lfsr < cell_size) {
          permutations[q++] = lfsr;
        }
      }
      if (tiblocks == 0) {
        FECBlocksPerSmallTIBlock = 1;
        FECBlocksPerBigTIBlock = 1;
        numBigTIBlocks = 0;
        numSmallTIBlocks = fecblocks;
      }
      else {
        FECBlocksPerSmallTIBlock = floor(((float)fecblocks) / ((float)tiblocks));
        FECBlocksPerBigTIBlock = ceil(((float)fecblocks) / ((float)tiblocks));
        numBigTIBlocks = fecblocks % tiblocks;
        numSmallTIBlocks = tiblocks - numBigTIBlocks;
      }
      time_interleave = (gr_complex *) malloc(sizeof(gr_complex) * cell_size * fecblocks);
      if (time_interleave == NULL) {
        GR_LOG_FATAL(d_logger, "Cell/Time Interleaver, cannot allocate memory for time_interleave.");
        throw std::bad_alloc();
      }
      cols = (gr_complex **) malloc(sizeof(gr_complex *) * FECBlocksPerBigTIBlock * 5);
      if (cols == NULL) {
        free(time_interleave);
        GR_LOG_FATAL(d_logger, "Cell/Time Interleaver, cannot allocate memory for cols.");
        throw std::bad_alloc();
      }
      ti_blocks = tiblocks;
      fec_blocks = fecblocks;
      set_output_multiple(cell_size * fecblocks);
      interleaved_items = cell_size * fecblocks;
    }

    /*
     * Our virtual destructor.
     */
    dvbt2_cellinterleaver_cc_impl::~dvbt2_cellinterleaver_cc_impl()
    {
      free(cols);
      free(time_interleave);
    }

    int
    dvbt2_cellinterleaver_cc_impl::work(int noutput_items,
                          gr_vector_const_void_star &input_items,
                          gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      int FECBlocksPerTIBlock, n, shift, temp, index, rows, numCols, ti_index;

      for (int i = 0; i < noutput_items; i += interleaved_items) {
        index = 0;
        for (int s = 0; s < numSmallTIBlocks + numBigTIBlocks; s++) {
          n = 0;
          if (s < numSmallTIBlocks) {
            FECBlocksPerTIBlock = FECBlocksPerSmallTIBlock;
          }
          else {
            FECBlocksPerTIBlock = FECBlocksPerBigTIBlock;
          }
          for (int r = 0; r < FECBlocksPerTIBlock; r++) {
            shift = cell_size;
            while (shift >= cell_size) {
              temp = n;
              shift = 0;
              for (int p = 0; p < pn_degree; p++) {
                shift |= temp & 1;
                shift <<= 1;
                temp >>= 1;
              }
              n++;
            }
            for (int w = 0; w < cell_size; w++) {
              time_interleave[((permutations[w] + shift) % cell_size) + index] = *in++;
            }
            index += cell_size;
          }
        }
        if (ti_blocks != 0) {
          ti_index = 0;
          for (int s = 0; s < numSmallTIBlocks + numBigTIBlocks; s++) {
            if (s < numSmallTIBlocks) {
              FECBlocksPerTIBlock = FECBlocksPerSmallTIBlock;
            }
            else {
              FECBlocksPerTIBlock = FECBlocksPerBigTIBlock;
            }
            numCols = 5 * FECBlocksPerTIBlock;
            rows = cell_size / 5;
            for (int j = 0; j < numCols; j++) {
              cols[j] = &time_interleave[(rows * j) + ti_index];
            }
            index = 0;
            for (int k = 0; k < rows; k++) {
              for (int w = 0; w < numCols; w++) {
                *out++ = *(cols[w] + index);
              }
              index++;
            }
            ti_index += rows * numCols;
          }
        }
        else {
          index = 0;
          for (int w = 0; w < fec_blocks * cell_size; w++) {
            *out++ = time_interleave[index++];
          }
        }
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */

