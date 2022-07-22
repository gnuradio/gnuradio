/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/fileio/file_sink.h>
#include <gnuradio/fileio/file_sink_base.h>

namespace gr {
namespace fileio {

class file_sink_cpu : public file_sink, public file_sink_base
{
public:
    file_sink_cpu(const block_args& args);
    ~file_sink_cpu() override;

    bool stop() override;

    work_return_code_t work(work_io&) override;

    void set_unbuffered(bool unbuffered) override
    {
        file_sink_base::set_unbuffered(unbuffered);
    }


private:
    size_t d_itemsize;
};

} // namespace fileio
} // namespace gr