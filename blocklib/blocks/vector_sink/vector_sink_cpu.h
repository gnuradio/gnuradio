/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2009,2013,2017-2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/blocks/vector_sink.h>

namespace gr {
namespace blocks {

template <class T>
class vector_sink_cpu : public vector_sink<T>
{
public:
    vector_sink_cpu(const typename vector_sink<T>::block_args& args);

    work_return_t work(work_io&) override;

    // on_parameter_query is overridden here because PMT currently does not
    // support element push_back of pmtv::vector.  So it is very inefficient
    // to deal with the pmt directly in the work function.  Just work with the
    // private member variable, and pass it out as pmt when queried
    void on_parameter_query(param_action_sptr action) override
    {
        this->d_debug_logger->trace(
            "block {}: on_parameter_query param_id: {}", this->id(), action->id());
        pmtv::pmt param = d_data;
        // auto data = pmtv::cast<std::vector<float>>(*param);
        action->set_pmt_value(param);
    }

    std::vector<tag_t> tags() override { return d_tags; }

protected:
    std::vector<T> d_data;
    std::vector<tag_t> d_tags;
    size_t d_vlen;
};


} // namespace blocks
} // namespace gr
