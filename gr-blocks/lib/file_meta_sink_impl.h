/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_FILE_META_SINK_IMPL_H
#define INCLUDED_BLOCKS_FILE_META_SINK_IMPL_H

#include <gnuradio/blocks/file_meta_sink.h>
#include <pmt/pmt.h>

using pmt::pmt_t;

namespace gr {
namespace blocks {

class file_meta_sink_impl : public file_meta_sink
{
private:
    enum meta_state_t { STATE_INLINE = 0, STATE_DETACHED };

    const size_t d_itemsize;
    double d_samp_rate;
    const double d_relative_rate;
    const size_t d_max_seg_size;
    size_t d_total_seg_size;
    pmt_t d_header;
    pmt_t d_extra;
    size_t d_extra_size;
    bool d_updated;
    bool d_unbuffered;

    FILE *d_new_fp, *d_new_hdr_fp;
    FILE *d_fp, *d_hdr_fp;
    meta_state_t d_state;

protected:
    void write_header(FILE* fp, pmt_t header, pmt_t extra);
    void update_header(pmt_t key, pmt_t value);
    void update_last_header();
    void update_last_header_inline();
    void update_last_header_detached();
    void write_and_update();
    void update_rx_time();

    bool _open(FILE** fp, const char* filename);

public:
    file_meta_sink_impl(size_t itemsize,
                        const std::string& filename,
                        double samp_rate = 1,
                        double relative_rate = 1,
                        gr_file_types type = GR_FILE_FLOAT,
                        bool complex = true,
                        size_t max_segment_size = 1000000,
                        pmt::pmt_t extra_dict = pmt::make_dict(),
                        bool detached_header = false);
    ~file_meta_sink_impl() override;

    bool open(const std::string& filename) override;
    void close() override;
    void do_update() override;

    void set_unbuffered(bool unbuffered) override { d_unbuffered = unbuffered; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_FILE_META_SINK_IMPL_H */
