/* -*- c++ -*- */
/*
 * Copyright 2012, 2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/fileio/file_source.h>

namespace gr {
namespace fileio {

class file_source_cpu : public file_source
{
public:
    file_source_cpu(const block_args& args);
    ~file_source_cpu() override;
    work_return_code_t work(work_io&) override;

    /*!
     * \brief seek file to \p seek_point relative to \p whence
     *
     * \param seek_point      sample offset in file
     * \param whence  one of SEEK_SET, SEEK_CUR, SEEK_END (man fseek)
     */
    bool seek(int64_t seek_point, int whence) override;

    /*!
     * \brief Opens a new file.
     *
     * \param filename        name of the file to source from
     * \param repeat  repeat file from start
     * \param offset  begin this many items into file
     * \param len     produce only items [offset, offset+len)
     */
    void
    open(const std::string& filename, bool repeat, uint64_t offset = 0, uint64_t len = 0);

    /*!
     * \brief Close the file handle.
     */
    void close();

    /*!
     * \brief Add a stream tag to the first sample of the file if true
     */
    void set_begin_tag(const std::string& val);

private:
    uint64_t d_start_offset_items;
    uint64_t d_length_items;
    uint64_t d_items_remaining;
    FILE* d_fp = nullptr;
    FILE* d_new_fp = nullptr;
    bool d_repeat;
    size_t d_itemsize;
    std::string d_filename;
    size_t d_offset;
    bool d_updated = false;
    bool d_file_begin = true;
    bool d_seekable;
    long d_repeat_cnt = 0;
    std::string d_add_begin_tag;

    std::mutex fp_mutex;
    pmtf::pmt _id;

    void do_update();
};

} // namespace fileio
} // namespace gr
