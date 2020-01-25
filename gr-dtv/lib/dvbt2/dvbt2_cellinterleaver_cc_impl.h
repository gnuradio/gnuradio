/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT2_CELLINTERLEAVER_CC_IMPL_H
#define INCLUDED_DTV_DVBT2_CELLINTERLEAVER_CC_IMPL_H

#include "../dvb/dvb_defines.h"

#include <gnuradio/dtv/dvbt2_cellinterleaver_cc.h>

namespace gr {
namespace dtv {

class dvbt2_cellinterleaver_cc_impl : public dvbt2_cellinterleaver_cc
{
private:
    int cell_size;
    int pn_degree;
    int ti_blocks;
    int fec_blocks;
    int permutations[32768];
    int FECBlocksPerSmallTIBlock;
    int FECBlocksPerBigTIBlock;
    int numBigTIBlocks;
    int numSmallTIBlocks;
    int interleaved_items;
    gr_complex* time_interleave;
    gr_complex** cols;

public:
    dvbt2_cellinterleaver_cc_impl(dvb_framesize_t framesize,
                                  dvb_constellation_t constellation,
                                  int fecblocks,
                                  int tiblocks);
    ~dvbt2_cellinterleaver_cc_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT2_CELLINTERLEAVER_CC_IMPL_H */
