/* -*- c++ -*- */
/*
 * Copyright 2015,2016 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVB_BBHEADER_BB_IMPL_H
#define INCLUDED_DTV_DVB_BBHEADER_BB_IMPL_H

#include "dvb_defines.h"
#include <gnuradio/dtv/dvb_bbheader_bb.h>

typedef struct {
    int ts_gs;
    int sis_mis;
    int ccm_acm;
    int issyi;
    int npd;
    int ro;
    int isi;
    int upl;
    int dfl;
    int sync;
    int syncd;
} BBHeader;

typedef struct {
    BBHeader bb_header;
} FrameFormat;

namespace gr {
namespace dtv {

class dvb_bbheader_bb_impl : public dvb_bbheader_bb
{
private:
    unsigned int kbch;
    unsigned int count;
    unsigned char crc;
    unsigned int input_mode;
    unsigned int extra;
    unsigned int frame_size;
    unsigned char bsave;
    int inband_type_b;
    int fec_blocks;
    int fec_block;
    int ts_rate;
    bool dvbs2x;
    bool alternate;
    bool nibble;
    FrameFormat m_format[1];
    unsigned char crc_tab[256];
    void add_bbheader(unsigned char*, int, int, bool);
    void build_crc8_table(void);
    int add_crc8_bits(unsigned char*, int);
    void add_inband_type_b(unsigned char*, int);

public:
    dvb_bbheader_bb_impl(dvb_standard_t standard,
                         dvb_framesize_t framesize,
                         dvb_code_rate_t rate,
                         dvbs2_rolloff_factor_t rolloff,
                         dvbt2_inputmode_t mode,
                         dvbt2_inband_t inband,
                         int fecblocks,
                         int tsrate);
    ~dvb_bbheader_bb_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVB_BBHEADER_BB_IMPL_H */
