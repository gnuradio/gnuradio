/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2012-2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/blocks/vector_source.h>

namespace gr {
namespace blocks {

template <class T>
class vector_source_cpu : public vector_source<T>
{
public:
    vector_source_cpu(const typename vector_source<T>::block_args& args);

    work_return_code_t work(work_io& wio) override;

    void set_data_and_tags(std::vector<T> data, std::vector<gr::tag_t> tags) override;
    void rewind() override { d_offset = 0; }

protected:
    std::vector<T> d_data;
    bool d_repeat;
    unsigned int d_offset;
    size_t d_vlen;
    bool d_settags = false;
    std::vector<tag_t> d_tags;
};


} // namespace blocks
} // namespace gr
