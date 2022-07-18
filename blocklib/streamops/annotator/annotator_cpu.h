/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
 * Copyright 2021 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/streamops/annotator.h>

namespace gr {
namespace streamops {

class annotator_cpu : public annotator
{
public:
    annotator_cpu(const block_args& args);
    work_return_code_t work(work_io&) override;

    std::vector<tag_t> data() const override { return d_stored_tags; };

private:
    const uint64_t d_when;
    uint64_t d_tag_counter;
    std::vector<tag_t> d_stored_tags;
    size_t d_num_inputs, d_num_outputs;
    tag_propagation_policy_t d_tpp;
};

} // namespace streamops
} // namespace gr