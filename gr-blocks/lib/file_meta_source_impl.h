/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_FILE_META_SOURCE_IMPL_H
#define INCLUDED_BLOCKS_FILE_META_SOURCE_IMPL_H

#include <gnuradio/blocks/file_meta_source.h>
#include <gnuradio/tags.h>
#include <gnuradio/thread/thread.h>
#include <pmt/pmt.h>

#include <gnuradio/blocks/file_meta_sink.h>

using pmt::pmt_t;
namespace gr {
namespace blocks {

class file_meta_source_impl : public file_meta_source
{
private:
    enum meta_state_t { STATE_INLINE = 0, STATE_DETACHED };

    size_t d_itemsize;
    double d_samp_rate;
    pmt_t d_time_stamp;
    size_t d_seg_size;
    bool d_updated;
    const bool d_repeat;

    FILE *d_new_fp, *d_new_hdr_fp;
    FILE *d_fp, *d_hdr_fp;
    meta_state_t d_state;

    std::vector<tag_t> d_tags;

protected:
    bool _open(FILE** fp, const char* filename);
    bool read_header(pmt_t& hdr, pmt_t& extras);
    void parse_header(pmt_t hdr, uint64_t offset, std::vector<tag_t>& tags);
    void parse_extras(pmt_t extras, uint64_t offset, std::vector<tag_t>& tags);

public:
    file_meta_source_impl(const std::string& filename,
                          bool repeat = false,
                          bool detached_header = false,
                          const std::string& hdr_filename = "");

    ~file_meta_source_impl() override;

    bool open(const std::string& filename, const std::string& hdr_filename = "") override;
    void close() override;
    void do_update() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_FILE_META_SOURCE_IMPL_H */
