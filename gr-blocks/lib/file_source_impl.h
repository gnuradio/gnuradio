/* -*- c++ -*- */
/*
 * Copyright 2012, 2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_FILE_SOURCE_IMPL_H
#define INCLUDED_BLOCKS_FILE_SOURCE_IMPL_H

#include <gnuradio/blocks/file_source.h>
#include <gnuradio/thread/thread.h>

namespace gr {
namespace blocks {

class BLOCKS_API file_source_impl : public file_source
{
private:
    const size_t d_itemsize;
    uint64_t d_start_offset_items;
    uint64_t d_length_items;
    uint64_t d_items_remaining;
    FILE* d_fp;
    FILE* d_new_fp;
    bool d_repeat;
    bool d_updated;
    bool d_file_begin;
    bool d_seekable;
    long d_repeat_cnt;
    pmt::pmt_t d_add_begin_tag;

    gr::thread::mutex fp_mutex;
    pmt::pmt_t _id;

    void do_update();

public:
    file_source_impl(size_t itemsize,
                     const char* filename,
                     bool repeat,
                     uint64_t offset,
                     uint64_t len);
    ~file_source_impl() override;

    bool seek(int64_t seek_point, int whence) override;
    void open(const char* filename, bool repeat, uint64_t offset, uint64_t len) override;
    void close() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    void set_begin_tag(pmt::pmt_t val) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_FILE_SOURCE_IMPL_H */
