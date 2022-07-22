/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2010,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "vector_source_cpu.h"
#include "vector_source_cpu_gen.h"
#include <algorithm>
#include <cstring> // for memcpy
#include <stdexcept>

using namespace std;

namespace gr {
namespace blocks {

template <class T>
vector_source_cpu<T>::vector_source_cpu(const typename vector_source<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T),
      d_data(args.data),
      d_repeat(args.repeat),
      d_offset(0),
      d_vlen(args.vlen),
      d_tags(args.tags)
{
    if (d_tags.empty()) {
        d_settags = 0;
    }
    else {
        d_settags = 1;
        this->set_output_multiple(d_data.size() / d_vlen);
    }
    if ((d_data.size() % d_vlen) != 0)
        throw std::invalid_argument("data length must be a multiple of vlen");
}


template <class T>
work_return_code_t vector_source_cpu<T>::work(work_io& wio)
{

    // size_t noutput_ports = wio.outputs().size(); // is 1 for this block
    int noutput_items = wio.outputs()[0].n_items;
    auto optr = wio.outputs()[0].items<T>();

    if (d_repeat) {
        unsigned int size = d_data.size();
        unsigned int offset = d_offset;
        if (size == 0)
            return work_return_code_t::WORK_DONE;

        if (d_settags) {
            int n_outputitems_per_vector = d_data.size() / d_vlen;
            for (int i = 0; i < noutput_items; i += n_outputitems_per_vector) {
                // FIXME do proper vector copy
                memcpy((void*)optr, (const void*)&d_data[0], size * sizeof(T));
                optr += size;
                for (unsigned t = 0; t < d_tags.size(); t++) {
                    wio.outputs()[0].add_tag(wio.outputs()[0].nitems_written() + i +
                                                 d_tags[t].offset(),
                                             d_tags[t].map());
                }
            }
        }
        else {
            for (int i = 0; i < static_cast<int>(noutput_items * d_vlen); i++) {
                optr[i] = d_data[offset++];
                if (offset >= size) {
                    offset = 0;
                }
            }
        }

        d_offset = offset;

        wio.outputs()[0].n_produced = noutput_items;
        return work_return_code_t::WORK_OK;
    }
    else {
        if (d_offset >= d_data.size()) {
            wio.outputs()[0].n_produced = 0;
            return work_return_code_t::WORK_DONE; // Done!
        }

        unsigned n = std::min(d_data.size() - d_offset, noutput_items * d_vlen);
        for (unsigned i = 0; i < n; i++) {
            optr[i] = d_data[d_offset + i];
        }
        for (unsigned t = 0; t < d_tags.size(); t++) {
            if ((d_tags[t].offset() >= d_offset) && (d_tags[t].offset() < d_offset + n)) {
                wio.outputs()[0].add_tag(d_tags[t]);
            }
        }
        d_offset += n;

        wio.outputs()[0].n_produced = n / d_vlen;
        return work_return_code_t::WORK_OK;
    }
}

template <class T>
void vector_source_cpu<T>::set_data_and_tags(std::vector<T> data,
                                             std::vector<gr::tag_t> tags)
{
    d_data = data;
    d_tags = tags;
    rewind();
    if (tags.empty()) {
        d_settags = false;
    }
    else {
        d_settags = true;
    }
}

} /* namespace blocks */
} /* namespace gr */
