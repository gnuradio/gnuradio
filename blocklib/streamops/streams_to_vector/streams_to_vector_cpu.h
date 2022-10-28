/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/streamops/streams_to_vector.h>

namespace gr {
namespace streamops {

class streams_to_vector_cpu : public virtual streams_to_vector
{
public:
    streams_to_vector_cpu(block_args args);
    work_return_t work(work_io& wio) override;

private:
    size_t d_vlen;
};

} // namespace streamops
} // namespace gr