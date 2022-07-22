/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2007,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "file_sink_cpu.h"
#include "file_sink_cpu_gen.h"

namespace gr {
namespace fileio {

file_sink_cpu::file_sink_cpu(const block_args& args)
    : INHERITED_CONSTRUCTORS,
      file_sink_base(args.filename, true, args.append),
      d_itemsize(args.itemsize)

{
}

file_sink_cpu::~file_sink_cpu() {}

work_return_code_t file_sink_cpu::work(work_io& wio)
{
    auto inbuf = wio.inputs()[0].items<uint8_t>();
    auto noutput_items = wio.inputs()[0].n_items;

    if (d_itemsize == 0) {
        d_itemsize = wio.inputs()[0].buf().item_size();
    }

    size_t nwritten = 0;

    do_update(); // update d_fp is reqd

    if (!d_fp) {
        wio.inputs()[0].n_consumed = noutput_items; // drop output on the floor
        return work_return_code_t::WORK_OK;
    }

    while (nwritten < noutput_items) {
        const int count = fwrite(inbuf, d_itemsize, noutput_items - nwritten, d_fp);
        if (count == 0) {
            if (ferror(d_fp)) {
                std::stringstream s;
                s << "file_sink write failed with error " << fileno(d_fp) << std::endl;
                throw std::runtime_error(s.str());
            }
            else { // is EOF
                break;
            }
        }
        nwritten += count;
        inbuf += count * d_itemsize;
    }

    if (d_unbuffered)
        fflush(d_fp);

    wio.consume_each(nwritten);
    return work_return_code_t::WORK_OK;
}

bool file_sink_cpu::stop()
{
    do_update();
    fflush(d_fp);
    return true;
}

} // namespace fileio
} // namespace gr