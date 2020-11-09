/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_FILE_DESCRIPTOR_SOURCE_IMPL_H
#define INCLUDED_GR_FILE_DESCRIPTOR_SOURCE_IMPL_H

#include <gnuradio/blocks/file_descriptor_source.h>

namespace gr {
namespace blocks {

class file_descriptor_source_impl : public file_descriptor_source
{
private:
    const size_t d_itemsize;
    const int d_fd;
    const bool d_repeat;

    std::vector<unsigned char> d_residue;
    unsigned long d_residue_len;

protected:
    int read_items(char* buf, int nitems) override;
    int handle_residue(char* buf, int nbytes_read) override;
    void flush_residue() override { d_residue_len = 0; }

public:
    file_descriptor_source_impl(size_t itemsize, int fd, bool repeat);
    ~file_descriptor_source_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_FILE_DESCRIPTOR_SOURCE_IMPL_H */
