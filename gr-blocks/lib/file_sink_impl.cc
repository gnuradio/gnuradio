/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2007,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "file_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
namespace blocks {

file_sink::sptr file_sink::make(size_t itemsize, const char* filename, bool append)
{
    return gnuradio::make_block_sptr<file_sink_impl>(itemsize, filename, append);
}

file_sink_impl::file_sink_impl(size_t itemsize, const char* filename, bool append)
    : sync_block(
          "file_sink", io_signature::make(1, 1, itemsize), io_signature::make(0, 0, 0)),
      file_sink_base(filename, true, append),
      d_itemsize(itemsize)
{
}

file_sink_impl::~file_sink_impl() {}

int file_sink_impl::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{
    const char* inbuf = static_cast<const char*>(input_items[0]);
    int nwritten = 0;

    do_update(); // update d_fp is reqd

    if (!d_fp)
        return noutput_items; // drop output on the floor

    while (nwritten < noutput_items) {
        const int count = fwrite(inbuf, d_itemsize, noutput_items - nwritten, d_fp);
        if (count == 0) {
            if (ferror(d_fp)) {
                std::stringstream s;
                s << "file_sink write failed with error " << fileno(d_fp) << std::endl;
                throw std::runtime_error(s.str());
            } else { // is EOF
                break;
            }
        }
        nwritten += count;
        inbuf += count * d_itemsize;
    }

    if (d_unbuffered)
        fflush(d_fp);

    return nwritten;
}

bool file_sink_impl::stop()
{
    do_update();
    fflush(d_fp);
    return true;
}

} /* namespace blocks */
} /* namespace gr */
